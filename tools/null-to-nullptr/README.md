# null-to-nullptr.py

**null-to-nullptr.py** rewrites `NULL` as `nullptr` in the places that
clang-tidy's `modernize-use-nullptr` check leaves alone:

* `NULL` as the last argument to a variadic function declared with
  `VA_SENTINEL` (`__attribute__((sentinel))`), such as `ddio_MakePath`.
* `NULL` inside a macro definition, or passed as a macro argument, when every
  expansion of it converts it to a pointer.

Run `modernize-use-nullptr` first; this tool handles what remains.

## Requirements

* Python 3 (standard library only).
* clang-query 19.1.5. On Linux it is in the `LLVM-19.1.5-Linux-X64.tar.xz`
  release from https://github.com/llvm/llvm-project/releases/tag/llvmorg-19.1.5.
  On Windows, check whether Visual Studio's bundled LLVM includes
  `clang-query.exe`; if it does not, the `clang+llvm-19.1.5-x86_64-pc-windows-msvc.tar.xz`
  release should.
* A CMake build directory with `compile_commands.json`. The project's
  presets enable it. Build once first so that generated headers exist.

## Usage

Check that clang-query behaves as the tool expects. This runs the tool on a
built-in test file, once with `NULL` defined as `__null` (GCC, Clang) and once
as `0` (MSVC), and compares the result with the expected output:

    null-to-nullptr.py -self-test -clang-query-binary <path to clang-query>

List what would change, without changing anything:

    null-to-nullptr.py -p builds/linux -clang-query-binary <path to clang-query> -quiet '^(?!.*(third_party|builds)).*$'

Add `-fix` to rewrite the files. The last argument is a regular expression
that selects translation units from the compilation database. `-exclude`
(default `third_party|builds`) is matched against source-root-relative paths;
`NULL` tokens in matching files, or outside the source root (`-root`, default
the current directory), are never rewritten.

## How it works

For each translation unit, clang-query finds every expansion of the `NULL`
macro and reports which of these it is:

* **pointer**: converted to a pointer type, implicitly or by a cast.
* **sentinel**: the last argument of a call to a function, method or function
  pointer variable that has the `sentinel` attribute, and not bound to a
  declared parameter.
* **other**: anything else. This includes `NULL` used as an integer
  (`(unsigned)NULL`, `buf[0] = NULL`) and `NULL` passed through `...` to a
  function without the attribute, such as `AI_AddGoal`, which reads those
  arguments with `va_arg(..., int)`.

Each expansion is mapped back to the `NULL` token written in the source, using
the macro expansion notes clang prints. The same token can expand many times,
for example in a macro defined in a header. A token is rewritten only if every
expansion in every translation unit is a pointer or a sentinel. Tokens with
any other use are listed in the output and left alone.

The tool stops without changing any file if clang-query fails on a
translation unit or its output cannot be parsed.

## Limitations

* Only code compiled in the given build is seen. Sources in other
  configurations (another platform, `#ifdef` branches that build does not
  enable, files outside the build) need their own run.
* Translation units with compile errors are listed. Expansions of `NULL` in
  code clang could not parse are not classified, so they are left alone.
* The `sentinel` attribute's optional position argument is not supported.
  `VA_SENTINEL` does not use it.
* Clang ignores the `sentinel` attribute on a function pointer typedef. Put it
  on the function pointer variable instead.
