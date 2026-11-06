# Guest build support

Files written for the test, used with `../../data/level1` to build the original level1
script as a Win32 DLL with clang and lld-link:

- `crt/*.h`: declarations of the C runtime functions the script uses. The DLL
  imports them from `msvcrt.dll`.
- `crt0.cpp`: DLL startup that runs the C++ global constructors through
  `msvcrt!_initterm`, as an MSVC DLL built against the dynamic C runtime does.
- `msvcrt.def`: the imports, for the import library made with llvm-dlltool.
