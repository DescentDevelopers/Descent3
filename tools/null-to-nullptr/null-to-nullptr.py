#!/usr/bin/python3

# Descent 3
# Copyright (C) 2024 Descent Developers
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

"""Rewrite the NULL uses that clang-tidy's modernize-use-nullptr skips.

modernize-use-nullptr only rewrites NULL where it is implicitly converted to
a pointer outside of macros. This tool also rewrites:

  * NULL passed as the last argument to a function declared with
    __attribute__((sentinel)) (VA_SENTINEL), e.g. ddio_MakePath. It goes
    through "..." without being converted, but the attribute declares that
    the callee reads it as a pointer;
  * NULL inside a macro definition or passed as a macro argument, where every
    expansion of that token converts it to a pointer.

It runs clang-query over every translation unit in a compilation database,
classifies each expansion of NULL from the AST, and maps it back to the NULL
token that was written in the source. A token is rewritten only if every
expansion of it, in every translation unit, is a pointer conversion or a
sentinel. Other tokens are reported and left alone: NULL used as an integer
(e.g. `(unsigned)NULL`, `buf[0] = NULL`), and NULL passed through "..." to a
function without the sentinel attribute, which may read it as an int.
"""

import argparse
import concurrent.futures
import json
import os
import re
import shutil
import subprocess
import sys
import tempfile

NULL_EXPR = ('expr(anyOf(gnuNullExpr(), integerLiteral(equals(0))), '
             'isExpandedFromMacro("NULL"))')
SENTINEL_CALL = 'callExpr(callee(decl(hasAttr("attr::Sentinel"))))'
# clang-query has no "last argument" matcher, so the sentinel matcher spells
# out every argument count up to this limit. A guard matcher reports calls
# with more arguments than that.
SENTINEL_MAX_ARGS = 64

# Order matters: the output is split into sections in this order.
MATCHERS = {
    # Every expansion of NULL.
    'all': f'{NULL_EXPR}.bind("null")',
    # NULL passed to a declared (non-variadic) parameter.
    'fixed': (f'expr(anyOf('
              f'callExpr(forEachArgumentWithParamType('
              f'ignoringParenImpCasts({NULL_EXPR}.bind("null")), anything())), '
              f'cxxConstructExpr(forEachArgumentWithParamType('
              f'ignoringParenImpCasts({NULL_EXPR}.bind("null")), anything()))))'),
    # NULL as the last argument of a call to a sentinel function. Minus 'fixed',
    # this is the set of NULLs in the sentinel position. The attribute's
    # optional position argument is not supported; VA_SENTINEL does not use it.
    'sentinel': (f'{SENTINEL_CALL[:-1]}, anyOf('
                 + ', '.join(f'allOf(argumentCountIs({n}), hasArgument({n - 1}, '
                             f'ignoringParens({NULL_EXPR}.bind("null"))))'
                             for n in range(1, SENTINEL_MAX_ARGS + 1))
                 + '))'),
    # Guard: sentinel calls the 'sentinel' matcher cannot see the end of.
    'too_many_args': (f'{SENTINEL_CALL[:-1]}, unless(anyOf('
                      + ', '.join(f'argumentCountIs({n})' for n in range(SENTINEL_MAX_ARGS + 1))
                      + '))).bind("call")'),
    # NULL converted to a pointer, implicitly or by an explicit cast.
    'pointer': ('castExpr(anyOf(hasCastKind("CK_NullToPointer"), '
                'hasCastKind("CK_NullToMemberPointer")), '
                f'hasSourceExpression(ignoringParens({NULL_EXPR}.bind("null"))))'),
}

# -Xclang spellings work in both the GCC and the clang-cl driver modes.
# Warnings are disabled because their notes would mix with the match output.
EXTRA_ARGS = [
    '--extra-arg=-Xclang', '--extra-arg=-w',
    '--extra-arg=-Xclang', '--extra-arg=-fmacro-backtrace-limit=0',
    '--extra-arg=-Xclang', '--extra-arg=-fno-caret-diagnostics',
]

NOTE_RE = re.compile(r'^(?P<path>.+?):(?P<line>\d+):(?P<col>\d+): note: '
                     r'(?:"\w+" binds here|expanded from macro \'(?P<macro>[^\']+)\')$')
MATCH_RE = re.compile(r'^Match #\d+:$')
SUMMARY_RE = re.compile(r'^(?P<n>\d+) match(?:es)?\.$')
ERROR_RE = re.compile(r': (?:fatal )?error: ')


class ToolError(Exception):
    pass


def run_clang_query(clang_query, build_dir, source):
    cmd = [clang_query, '-p', build_dir, *EXTRA_ARGS,
           '-c', 'set output diag', '-c', 'set bind-root false']
    for matcher in MATCHERS.values():
        cmd += ['-c', f'match {matcher}']
    cmd.append(source)
    proc = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                          text=True, errors='replace')
    if proc.returncode != 0:
        raise ToolError(f'{source}: clang-query exited {proc.returncode}:\n{proc.stdout}')
    return proc.stdout


def parse_output(source, output):
    """Return ({matcher name: [chain, ...]}, compile error lines).

    A chain is a tuple of (path, line, col) for the "binds here" location
    followed by each "expanded from macro" note, in the order clang prints them.
    """
    names = list(MATCHERS)
    sections = {name: [] for name in names}
    errors = []
    current = None
    index = 0
    for line in output.splitlines():
        if 'skipping' in line and 'expansions' in line:
            raise ToolError(f'{source}: macro backtrace was elided:\n{line}')
        if MATCH_RE.match(line):
            if index >= len(names):
                raise ToolError(f'{source}: more output sections than matchers')
            current = []
            sections[names[index]].append(current)
            continue
        note = NOTE_RE.match(line)
        if note and current is not None:
            current.append((note['path'], int(note['line']), int(note['col'])))
            continue
        summary = SUMMARY_RE.match(line)
        if summary:
            if index >= len(names):
                raise ToolError(f'{source}: more output sections than matchers')
            if int(summary['n']) != len(sections[names[index]]):
                raise ToolError(f'{source}: "{line}" but parsed '
                                f'{len(sections[names[index]])} matches for {names[index]}')
            index += 1
            current = None
            continue
        if current is None and ERROR_RE.search(line):
            errors.append(line)
    if index != len(names):
        raise ToolError(f'{source}: expected {len(names)} match summaries, got {index}:\n{output}')
    for name, chains in sections.items():
        for chain in chains:
            if not chain:
                raise ToolError(f'{source}: empty match in {name}')
    return {name: [tuple(c) for c in chains] for name, chains in sections.items()}, errors


class SourceCache:
    def __init__(self):
        self.files = {}

    def read(self, path):
        if path not in self.files:
            try:
                with open(path, 'rb') as f:
                    data = f.read()
            except OSError:
                data = None
            starts = None
            if data is not None:
                starts = [0] + [m.end() for m in re.finditer(b'\n', data)]
            self.files[path] = (data, starts)
        return self.files[path]

    def offset(self, path, line, col):
        data, starts = self.read(path)
        if data is None or line > len(starts):
            return None
        return starts[line - 1] + col - 1

    def is_null_token(self, path, offset):
        data, _ = self.read(path)
        if data is None or offset is None or data[offset:offset + 4] != b'NULL':
            return False
        before = data[offset - 1:offset] if offset > 0 else b''
        after = data[offset + 4:offset + 5]
        ident = re.compile(rb'[A-Za-z0-9_]')
        return not ident.match(before) and not ident.match(after)


def locate_token(cache, directory, chain):
    """Return (realpath, offset) of the NULL token written in the source.

    Relative paths in clang's output are relative to the compile command's
    working directory.
    """
    found = []
    for path, line, col in chain:
        path = os.path.join(directory, path)
        offset = cache.offset(path, line, col)
        if cache.is_null_token(path, offset):
            found.append((os.path.normcase(os.path.realpath(path)), offset))
    found = sorted(set(found))
    if len(found) != 1:
        raise ToolError(f'expected exactly one NULL token in expansion chain, found {len(found)}: '
                        + ' -> '.join(f'{p}:{l}:{c}' for p, l, c in chain))
    return found[0]


def classify(sections):
    """Return {chain: 'pointer' | 'sentinel' | 'other'} for one translation unit."""
    if sections['too_many_args']:
        raise ToolError(f'call to a sentinel function with more than {SENTINEL_MAX_ARGS} '
                        f'arguments: {sections["too_many_args"][0][0]}')
    every = set(sections['all'])
    fixed = set(sections['fixed'])
    sentinel = set(sections['sentinel'])
    pointer = set(sections['pointer'])
    stray = (fixed | sentinel | pointer) - every
    if stray:
        raise ToolError(f'matches outside of the "all" set: {sorted(stray)[:3]}')
    result = {}
    for chain in every:
        if chain in pointer:
            result[chain] = 'pointer'
        elif chain in sentinel and chain not in fixed:
            result[chain] = 'sentinel'
        else:
            result[chain] = 'other'
    return result


def load_sources(build_dir, file_regex):
    db_path = os.path.join(build_dir, 'compile_commands.json')
    with open(db_path) as f:
        db = json.load(f)
    sources = {}
    for entry in db:
        path = os.path.normpath(os.path.join(entry['directory'], entry['file']))
        if file_regex.search(path):
            sources.setdefault(path, entry['directory'])
    return sources


def collect(args, sources):
    """Run clang-query on every source and return per-token classifications."""
    cache = SourceCache()
    tokens = {}  # (path, offset) -> {kind: example location}
    compile_errors = {}
    failures = []

    def work(source):
        output = run_clang_query(args.clang_query_binary, args.build_dir, source)
        return parse_output(source, output)

    with concurrent.futures.ThreadPoolExecutor(max_workers=args.jobs) as pool:
        futures = {pool.submit(work, s): s for s in sorted(sources)}
        for done, future in enumerate(concurrent.futures.as_completed(futures), 1):
            source = futures[future]
            if not args.quiet:
                print(f'[{done}/{len(sources)}] {source}', file=sys.stderr)
            try:
                sections, errors = future.result()
                if errors:
                    compile_errors[source] = errors
                for chain, kind in classify(sections).items():
                    token = locate_token(cache, sources[source], chain)
                    tokens.setdefault(token, {}).setdefault(kind, chain[0])
            except ToolError as e:
                failures.append(f'{source}: {e}')
    return cache, tokens, compile_errors, failures


def in_scope(path, root, exclude_regex):
    rel = os.path.relpath(path, root)
    if rel.startswith(os.pardir + os.sep) or os.path.isabs(rel):
        return False
    return not exclude_regex.search(rel.replace(os.sep, '/'))


def describe(cache, path, offset, root):
    _, starts = cache.read(path)
    line = 0
    while line + 1 < len(starts) and starts[line + 1] <= offset:
        line += 1
    return f'{os.path.relpath(path, root)}:{line + 1}:{offset - starts[line] + 1}'


def apply_fixes(fixes):
    by_file = {}
    for path, offset in fixes:
        by_file.setdefault(path, []).append(offset)
    for path, offsets in by_file.items():
        with open(path, 'rb') as f:
            data = f.read()
        for offset in sorted(offsets, reverse=True):
            if data[offset:offset + 4] != b'NULL':
                raise ToolError(f'{path}:{offset}: expected NULL, found {data[offset:offset + 4]!r}')
            data = data[:offset] + b'nullptr' + data[offset + 4:]
        with open(path, 'wb') as f:
            f.write(data)


def run(args):
    root = os.path.realpath(args.root)
    exclude_regex = re.compile(args.exclude)
    sources = load_sources(args.build_dir, re.compile(args.files))
    if not sources:
        raise ToolError('no translation units matched')
    cache, tokens, compile_errors, failures = collect(args, sources)

    convert, keep, mixed = [], [], []
    for token, kinds in tokens.items():
        if not in_scope(token[0], root, exclude_regex):
            continue
        if 'other' not in kinds:
            convert.append(token)
        elif len(kinds) == 1:
            keep.append(token)
        else:
            mixed.append(token)

    for title, items in (('Converting', convert),
                         ('Leaving (not used as a pointer)', keep),
                         ('Leaving (used both as a pointer and not)', mixed)):
        print(f'{title}: {len(items)}')
        for path, offset in sorted(items):
            kinds = ','.join(sorted(tokens[(path, offset)]))
            print(f'  {describe(cache, path, offset, root)}  [{kinds}]')
    if compile_errors:
        print(f'Translation units with compile errors (expansions in unparsed code are not '
              f'classified): {len(compile_errors)}')
        for source, errors in sorted(compile_errors.items()):
            print(f'  {os.path.relpath(source, root)} ({len(errors)} errors)')
    if failures:
        print(f'FAILED: {len(failures)} translation units could not be processed:', file=sys.stderr)
        for failure in failures:
            print(f'  {failure}', file=sys.stderr)
        return 1
    if args.fix:
        apply_fixes(convert)
        print(f'Rewrote {len(convert)} NULL tokens.')
    return 0


SELF_TEST_SOURCE = r'''#ifdef GNU_NULL
#define NULL __null
#else
#define NULL 0
#endif
#include "third_party/vendor.h"
#define SENT __attribute__((sentinel))
void makepath(char *out, const char *a, ...) SENT;
struct W { virtual void hash(const char *p, ...) SENT = 0; };
typedef void (*makepath_fp)(char *out, const char *a, ...);
makepath_fp DLLmakepath SENT;
int ai_goal(int handle, int type, ...);
void bail(const char *msg);
void take_unsigned(unsigned h);
void take_int(int v);
void take_long(long v);
void take_ptr(char *p);
int msgbox(const char *t, int key0, ...) SENT;
#define M_ASSERT(x, m) { if (!(x)) { bail((m)); return; } }
#define M_ASSERT_NOMSG(x) { if (!(x)) { bail(NULL); return; } }
#define OUTER(v) INNER(v)
#define INNER(v) take_ptr(v)
#define MIXED NULL
void f(char *buf, W &w, int x) {
  makepath(buf, "a", "b", NULL);
  w.hash("x", "y", NULL);
  DLLmakepath(buf, "a", NULL);
  makepath(buf, "a", NULL, "c", (NULL));
  M_ASSERT(x, NULL);
  M_ASSERT_NOMSG(x);
  OUTER(NULL);
  char *p = (char *)NULL;
  VENDOR_BAIL(x);
  take_unsigned((unsigned)NULL);
  take_unsigned(NULL);
  take_int(NULL);
  take_long(NULL);
  msgbox("t", NULL, "ok", NULL);
  char c = NULL;
  take_ptr(MIXED);
  int i = MIXED;
  ai_goal(1, 2, NULL);
  ai_goal(1, 2, 3, NULL);
  msgbox("t", NULL);
  (void)c; (void)i; (void)p;
}
'''

SELF_TEST_EXPECTED = r'''#ifdef GNU_NULL
#define NULL __null
#else
#define NULL 0
#endif
#include "third_party/vendor.h"
#define SENT __attribute__((sentinel))
void makepath(char *out, const char *a, ...) SENT;
struct W { virtual void hash(const char *p, ...) SENT = 0; };
typedef void (*makepath_fp)(char *out, const char *a, ...);
makepath_fp DLLmakepath SENT;
int ai_goal(int handle, int type, ...);
void bail(const char *msg);
void take_unsigned(unsigned h);
void take_int(int v);
void take_long(long v);
void take_ptr(char *p);
int msgbox(const char *t, int key0, ...) SENT;
#define M_ASSERT(x, m) { if (!(x)) { bail((m)); return; } }
#define M_ASSERT_NOMSG(x) { if (!(x)) { bail(nullptr); return; } }
#define OUTER(v) INNER(v)
#define INNER(v) take_ptr(v)
#define MIXED NULL
void f(char *buf, W &w, int x) {
  makepath(buf, "a", "b", nullptr);
  w.hash("x", "y", nullptr);
  DLLmakepath(buf, "a", nullptr);
  makepath(buf, "a", NULL, "c", (nullptr));
  M_ASSERT(x, nullptr);
  M_ASSERT_NOMSG(x);
  OUTER(nullptr);
  char *p = (char *)nullptr;
  VENDOR_BAIL(x);
  take_unsigned((unsigned)NULL);
  take_unsigned(NULL);
  take_int(NULL);
  take_long(NULL);
  msgbox("t", NULL, "ok", nullptr);
  char c = NULL;
  take_ptr(MIXED);
  int i = MIXED;
  ai_goal(1, 2, NULL);
  ai_goal(1, 2, 3, NULL);
  msgbox("t", NULL);
  (void)c; (void)i; (void)p;
}
'''

SELF_TEST_VENDOR = '#define VENDOR_BAIL(x) { if (!(x)) { bail(NULL); return; } }\n'


def self_test(args):
    """Run the tool on a fixture with GNU (__null) and MSVC (0) NULL definitions."""
    status = 0
    for variant, define in (('GNU NULL', ['-DGNU_NULL']), ('MSVC NULL', [])):
        tmp = tempfile.mkdtemp(prefix='null-to-nullptr-')
        try:
            os.makedirs(os.path.join(tmp, 'third_party'))
            source = os.path.join(tmp, 'case.cpp')
            with open(source, 'w', newline='\n') as f:
                f.write(SELF_TEST_SOURCE)
            with open(os.path.join(tmp, 'third_party', 'vendor.h'), 'w', newline='\n') as f:
                f.write(SELF_TEST_VENDOR)
            with open(os.path.join(tmp, 'compile_commands.json'), 'w') as f:
                json.dump([{'directory': tmp, 'file': 'case.cpp',
                            'arguments': ['clang++', '-std=c++17', '-fsyntax-only', '-w',
                                          *define, 'case.cpp']}], f)
            test_args = argparse.Namespace(**vars(args))
            test_args.build_dir = tmp
            test_args.root = tmp
            test_args.files = '.*'
            test_args.fix = True
            test_args.quiet = True
            print(f'=== self-test: {variant}')
            rc = run(test_args)
            with open(source) as f:
                result = f.read()
            with open(os.path.join(tmp, 'third_party', 'vendor.h')) as f:
                vendor = f.read()
            if rc != 0 or result != SELF_TEST_EXPECTED or vendor != SELF_TEST_VENDOR:
                status = 1
                print(f'self-test FAILED ({variant}): exit {rc}', file=sys.stderr)
                for i, (got, want) in enumerate(zip(result.splitlines(),
                                                    SELF_TEST_EXPECTED.splitlines()), 1):
                    if got != want:
                        print(f'  line {i}: got  {got!r}\n          want {want!r}', file=sys.stderr)
                if vendor != SELF_TEST_VENDOR:
                    print('  third_party/vendor.h was modified', file=sys.stderr)
            else:
                print(f'self-test passed ({variant})')
        finally:
            shutil.rmtree(tmp)
    return status


def main():
    parser = argparse.ArgumentParser(
        prog='null-to-nullptr.py',
        description='Rewrite NULL as nullptr where modernize-use-nullptr does not: '
                    'sentinel arguments and macros.',
    )
    parser.add_argument('-p', dest='build_dir', help='build directory containing compile_commands.json')
    parser.add_argument('-clang-query-binary', default='clang-query', help='path to clang-query')
    parser.add_argument('-j', dest='jobs', type=int, default=os.cpu_count(),
                        help='number of clang-query processes to run in parallel')
    parser.add_argument('-root', default='.',
                        help='source root; NULL tokens outside it are never rewritten (default: .)')
    parser.add_argument('-exclude', default='third_party|builds',
                        help='regex on root-relative paths of files never to rewrite '
                             '(default: third_party|builds)')
    parser.add_argument('-fix', action='store_true', help='rewrite files (default: report only)')
    parser.add_argument('-quiet', action='store_true', help='do not print progress')
    parser.add_argument('-self-test', action='store_true',
                        help='check this tool and clang-query against a built-in fixture')
    parser.add_argument('files', nargs='?', default='.*',
                        help='regex selecting translation units from the database (default: .*)')
    args = parser.parse_args()
    if shutil.which(args.clang_query_binary) is None:
        parser.error(f'clang-query not found: {args.clang_query_binary}')
    try:
        if args.self_test:
            return self_test(args)
        if not args.build_dir:
            parser.error('-p is required')
        return run(args)
    except ToolError as e:
        print(f'error: {e}', file=sys.stderr)
        return 1


if __name__ == '__main__':
    sys.exit(main())
