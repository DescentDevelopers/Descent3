#!/bin/sh

# Run clang-format on files, query git for the list, or specify them individually.
# LICENSE: Unlicense (see end of file)

only_check=0
verbose=1
only_modified=0
for arg in "$@"; do
    case $arg in
        -h|--help)
            echo "Usage: $0 [--check|-c] [--quiet|-q] [--modified|-m] [FILE..]"
            exit 0
        ;;
        --quiet|-q)
            verbose=0
            ;;
        --modified|-m)
            only_modified=1
            ;;
        --check|-c)
            only_check=1
            ;;
        *)
            break
            ;;
    esac
    shift
done


# find toplevel, so `git ls-files` finds all files
for i in 1 2 3 4 5 6 7 err; do
    if [ -e .git ] && [ -d .github ]; then
        break
    else
        if [ "$i" = err ]; then
            echo "Error: Failed to find project toplevel"
            exit 2
        fi
        cd ..
    fi
done


# env var CLANG_FORMAT can override
wanted_clang_format_version="$(awk -F"'" '/clang-format-version/{  print $2 }' .github/workflows/clang-format-check.yml)"
if [ "$CLANG_FORMAT" = "" ]; then
    CLANG_FORMAT="clang-format"
    # some linux distros have multiple clang-format-$ver installed
    if which "$CLANG_FORMAT-$wanted_clang_format_version" > /dev/null 2>&1; then
        CLANG_FORMAT="$CLANG_FORMAT-$wanted_clang_format_version"
    fi
fi
if which "$CLANG_FORMAT" > /dev/null 2>&1; then
    if ! $CLANG_FORMAT --version | grep -F -q "version $wanted_clang_format_version."; then
        echo "Note: your clang-format is not version $wanted_clang_format_version, formatting might differ."
    fi
else
    echo "Error: no '$CLANG_FORMAT' found"
    exit 2
fi


# If no files are given, format all matching files
if [ $# -eq 0 ]; then
    if [ $only_modified -eq 1 ]; then
        files="$({ git diff --name-only; git diff --name-only --staged; } | grep -e '.*\.[ch]$' -e '.*\.[ch]pp$')"
        if [ "$files" = "" ]; then
            echo "No modified files found (see 'git status')"
            exit 0
        fi
    else
        files="$(git ls-files '*.[ch]' '*.[ch]pp' ':!:lib/debugbreak.h')"
    fi
else
    files="$*"
fi

tmpfile="$(mktemp).cpp"
trap 'rm -f "$tmpfile"' EXIT

ret=0
for file in $files; do
    if [ $verbose -eq 1 ]; then
        printf "%s" "$file"
    fi

    if ! "$CLANG_FORMAT" "$file" > "$tmpfile"; then
        echo "   clang-format error"
        exit 2
    fi

    if cmp -s "$file" "$tmpfile"; then
        if [ $verbose -eq 1 ]; then
            echo ": Ok"
        fi
    else
        if [ $only_check -eq 0 ]; then
            cp "$tmpfile" "$file"
            if [ $verbose -eq 1 ]; then
                echo ": Reformatted"
            fi
        else
            ret=1
            if [ $verbose -eq 0 ]; then
                echo "Error: $file is not correctly formatted"
            else
                echo " is not correctly formatted, diff:"
                diff -u "$file" "$tmpfile"
            fi
        fi
    fi
done

exit $ret


# This is free and unencumbered software released into the public domain.
#
# Anyone is free to copy, modify, publish, use, compile, sell, or
# distribute this software, either in source code form or as a compiled
# binary, for any purpose, commercial or non-commercial, and by any
# means.
#
# In jurisdictions that recognize copyright laws, the author or authors
# of this software dedicate any and all copyright interest in the
# software to the public domain. We make this dedication for the benefit
# of the public at large and to the detriment of our heirs and
# successors. We intend this dedication to be an overt act of
# relinquishment in perpetuity of all present and future rights to this
# software under copyright law.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
# IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
# OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
# ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
# OTHER DEALINGS IN THE SOFTWARE.
#
# For more information, please refer to <http://unlicense.org/>
