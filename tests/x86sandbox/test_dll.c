/*
 * Descent 3
 * Copyright (C) 2024 Parallax Software
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/* A 32-bit Windows DLL for x86sandbox_tests. It is built with clang and lld-link
   for i686-pc-windows-msvc with x87 floating point, and imports a few C runtime
   and kernel32 functions the way an MSVC-built script DLL does. */

__declspec(dllimport) int sprintf(char *, const char *, ...);
__declspec(dllimport) unsigned strlen(const char *);
__declspec(dllimport) void *malloc(unsigned);
__declspec(dllimport) void free(void *);
__declspec(dllimport) double sin(double);
__declspec(dllimport) void qsort(void *, unsigned, unsigned, int(__cdecl *)(const void *, const void *));
__declspec(dllimport) void *__stdcall CreateFileA(const char *, unsigned, unsigned, void *, unsigned, unsigned,
                                                  void *);
__declspec(dllimport) int __stdcall GetDiskFreeSpaceA(const char *, unsigned *, unsigned *, unsigned *, unsigned *);

int _fltused = 0;

/* The host API table: the test's HostCallHandler implements these. */
typedef int (*api_add_fn)(int, int);
typedef int (*api_call_back_fn)(void *fn, int arg);
typedef float (*api_getf_fn)(void);
typedef int (*api_read_fn)(int addr);

static api_add_fn g_add;
static api_call_back_fn g_call_back;
static api_getf_fn g_getf;
static api_read_fn g_read;
static char g_buf[128];
static int g_attached;

__declspec(dllexport) int __stdcall Init(void **fp) {
  g_add = (api_add_fn)fp[0];
  g_call_back = (api_call_back_fn)fp[1];
  g_getf = (api_getf_fn)fp[2];
  g_read = (api_read_fn)fp[3];
  return g_attached;
}

__declspec(dllexport) int __stdcall Add3(int a, int b, int c) { return a + b + c; }
__declspec(dllexport) int __stdcall UseApi(int a, int b) { return g_add(a, b) * 2; }

__declspec(dllexport) const char *__stdcall Fmt(int v, double d) {
  sprintf(g_buf, "v=%d d=%.3f s=%s x=%04x", v, d, "ok", 0xbeef);
  return g_buf;
}

__declspec(dllexport) int __stdcall FloatMath(int x) {
  volatile double d = x;
  d = d * 1.5 + 0.25;
  return (int)d;
}

__declspec(dllexport) int __stdcall SinTimes1000(int deg) { return (int)(sin(deg * 3.14159265358979 / 180.0) * 1000.0); }
__declspec(dllexport) int __stdcall GetF100(void) { return (int)(g_getf() * 100.0f); }

static int __cdecl Square(int v) { return v * v; }
__declspec(dllexport) int __stdcall Nested(int v) { return g_call_back((void *)Square, v) + 1; }
__declspec(dllexport) int __stdcall Recurse(int n) { return n <= 0 ? 0 : 1 + g_call_back((void *)Recurse, n - 1); }

__declspec(dllexport) int __stdcall Strings(void) {
  char *p = malloc(64);
  sprintf(p, "%s-%d", "abc", 42);
  int n = (int)strlen(p);
  free(p);
  return n;
}

static int __cdecl CompareInts(const void *a, const void *b) { return *(const int *)a - *(const int *)b; }
__declspec(dllexport) int __stdcall Sort(void) {
  int v[6] = {5, 3, 9, 1, 7, 2};
  qsort(v, 6, sizeof(int), CompareInts);
  return v[0] * 100000 + v[1] * 10000 + v[2] * 1000 + v[3] * 100 + v[4] * 10 + v[5];
}

__declspec(dllexport) int __stdcall OpenFile(void) {
  return CreateFileA("C:\\x.txt", 0x40000000, 0, 0, 2, 0, 0) == (void *)-1;
}

/* Asks the host to read guest memory at an address the guest chooses. */
__declspec(dllexport) int __stdcall HostRead(int addr) { return g_read(addr); }

/* Each of these stops the module. */
__declspec(dllexport) int __stdcall Loop(void) {
  volatile int i = 0;
  for (;;)
    i++;
}
__declspec(dllexport) int __stdcall Trap(void) { __builtin_trap(); }
__declspec(dllexport) int __stdcall Privileged(void) {
  __asm__ volatile("hlt");
  return 1;
}
__declspec(dllexport) int __stdcall SoftwareInterrupt(void) {
  __asm__ volatile("int $0x80");
  return 1;
}
__declspec(dllexport) int __stdcall LoadSegment(void) {
  __asm__ volatile("movw $0x10, %%ax\n\tmovw %%ax, %%ds" ::: "eax");
  return 1;
}
__declspec(dllexport) int __stdcall WildWrite(void) {
  *(volatile int *)0x000A0000 = 1; /* tiny386's I/O window */
  return 1;
}
__declspec(dllexport) int __stdcall OutToOtherPort(void) {
  __asm__ volatile("outb %%al, $0x80" ::: "eax");
  return 1;
}
__declspec(dllexport) int __stdcall MissingImport(void) {
  unsigned a, b, c, d;
  return GetDiskFreeSpaceA("C:\\", &a, &b, &c, &d);
}
static int __stdcall RecurseForever(int n) { return RecurseForever(n + 1) + 1; }
__declspec(dllexport) int __stdcall StackOverflow(void) { return RecurseForever(0); }

int __stdcall DllMain(void *h, unsigned reason, void *r) {
  (void)h;
  (void)r;
  if (reason == 1)
    g_attached = 7;
  return 1;
}
