# level1 test data

The level script of the first Descent 3 level as it was released, which
`osiris_sandbox_tests` builds into a 32-bit Windows DLL and runs through the
OSIRIS sandbox.

Every file here except this README and `SHA256SUMS` is copied unmodified from
the initial import of the source release (commit `df209742`): `level1.cpp`,
`DallasFuncs.cpp`, `level1.msg`, the OSIRIS headers and the `*_external.h`
headers they include. They describe the structure layouts the original Windows
scripts were compiled against, which differ from the current headers on 64-bit
hosts, so they must not be updated along with the engine.

Do not reformat, modernize or otherwise edit these files. The build checks them
against `SHA256SUMS` (see `../../support/verify_data.cmake`) and stops if anything other
than whitespace has changed, or if a file is added or removed. Whitespace is
ignored so that line-ending conversion and editor settings do not break the
build.

The files the test build adds are in `../../support/guest`: C runtime declarations
(`crt/`), the DLL startup routine (`crt0.cpp`) and the import list
(`msvcrt.def`). The DLL is linked with `scripts/MS-LVL.DEF`, as the original
build was.
