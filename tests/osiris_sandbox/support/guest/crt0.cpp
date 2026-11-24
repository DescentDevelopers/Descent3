// Startup for an MSVC-style DLL that uses the dynamic C runtime (msvcrt.dll):
// run the C++ global constructors listed in .CRT$XCA..XCZ through msvcrt!_initterm.
typedef void(__cdecl *_PVFV)(void);
#pragma section(".CRT$XCA", read)
#pragma section(".CRT$XCZ", read)
__declspec(allocate(".CRT$XCA")) _PVFV __xc_a[] = {0};
__declspec(allocate(".CRT$XCZ")) _PVFV __xc_z[] = {0};
extern "C" void __cdecl _initterm(_PVFV *, _PVFV *);
extern "C" int _fltused = 0;
extern "C" int __stdcall _DllMainCRTStartup(void *h, unsigned reason, void *r) {
  (void)h;
  (void)r;
  if (reason == 1)
    _initterm(__xc_a, __xc_z);
  return 1;
}
