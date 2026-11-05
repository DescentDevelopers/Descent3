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
__declspec(dllimport) void *__stdcall LoadLibraryA(const char *);
__declspec(dllimport) void *__stdcall GetProcAddress(void *, const char *);
__declspec(dllimport) void *__stdcall GetModuleHandleA(const char *);
__declspec(dllimport) void __stdcall DebugBreak(void);
__declspec(dllimport) void *__stdcall GetProcessHeap(void);
__declspec(dllimport) void *__stdcall HeapAlloc(void *, unsigned, unsigned);
__declspec(dllimport) void *__stdcall HeapReAlloc(void *, unsigned, void *, unsigned);
__declspec(dllimport) int __stdcall IsBadReadPtr(const void *, unsigned);
__declspec(dllimport) int __stdcall CompareStringA(unsigned, unsigned, const char *, int, const char *, int);
/* From crtdll.dll, the C runtime GCC-built scripts use. */
__declspec(dllimport) int strcmp(const char *, const char *);
__declspec(dllimport) void __GetMainArgs(int *, char ***, char ***, int);

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

/* Scratch memory for the qsort and REP tests. */
static unsigned char g_big[3][200000];

/* qsort of n ints (n <= 150000) in pseudo-random order. Returns 1 if sorted. */
__declspec(dllexport) int __stdcall SortMany(int n) {
  int *v = (int *)g_big;
  unsigned s = 12345;
  for (int i = 0; i < n; i++) {
    s = s * 1103515245u + 12345u;
    v[i] = (int)(s >> 8);
  }
  qsort(v, (unsigned)n, sizeof(int), CompareInts);
  for (int i = 1; i < n; i++)
    if (v[i - 1] > v[i])
      return 0;
  return 1;
}

static unsigned g_rng;
static int *g_hostile;
static int __cdecl CompareRandomly(const void *a, const void *b) {
  (void)a;
  (void)b;
  g_rng = g_rng * 1103515245u + 12345u;
  return (int)((g_rng >> 16) % 3) - 1;
}
static int __cdecl CompareAndScribble(const void *a, const void *b) {
  g_rng = g_rng * 1103515245u + 12345u;
  g_hostile[(g_rng >> 16) % 999] = (int)g_rng;
  return *(const int *)a - *(const int *)b;
}
/* Sorts 999 ints between two guard words with a comparison function that
   returns random results (mode 0) or writes into the array (mode 1). Returns 1
   if qsort returned and the guard words are intact. The count is odd so that
   the last heap node has one child, where an off-by-one would reach past it. */
__declspec(dllexport) int __stdcall SortHostile(int mode) {
  unsigned *g = (unsigned *)g_big;
  g[0] = g[1000] = 0xdeadbeefu;
  g_hostile = (int *)(g + 1);
  for (int i = 0; i < 999; i++)
    g_hostile[i] = 999 - i;
  g_rng = 1;
  qsort(g_hostile, 999, sizeof(int), mode ? CompareAndScribble : CompareRandomly);
  return g[0] == 0xdeadbeefu && g[1000] == 0xdeadbeefu;
}

__declspec(dllexport) int __stdcall OpenFile(void) {
  return CreateFileA("C:\\x.txt", 0x40000000, 0, 0, 2, 0, 0) == (void *)-1;
}

/* Asks the host to read guest memory at an address the guest chooses. */
__declspec(dllexport) int __stdcall HostRead(int addr) { return g_read(addr); }

/* crtdll imports. Returns 1111 if all four checks pass. */
__declspec(dllexport) int __stdcall CrtdllImports(void) {
  int argc = 0;
  char **argv = 0, **env = 0;
  __GetMainArgs(&argc, &argv, &env, 0);
  return (strcmp("abc", "abd") < 0) * 1000 + (argc == 1) * 100 + (strcmp(argv[0], "descent3.exe") == 0) * 10 +
         (env[0] == 0);
}

/* Imports resolved at run time. Returns 1111 if all four checks pass. */
__declspec(dllexport) int __stdcall RuntimeImports(void) {
  typedef unsigned(__cdecl * strlen_fn)(const char *);
  typedef int(__stdcall * add3_fn)(int, int, int);
  strlen_fn len = (strlen_fn)GetProcAddress(LoadLibraryA("C:\\WINDOWS\\SYSTEM\\MSVCRT.DLL"), "strlen");
  add3_fn add3 = (add3_fn)GetProcAddress(GetModuleHandleA(0), "_Add3@12");
  return (len && len("hello") == 5) * 1000 + (add3 && add3(1, 2, 3) == 6) * 100 +
         (GetProcAddress(LoadLibraryA("kernel32.dll"), "GetDiskFreeSpaceA") == 0) * 10 + (LoadLibraryA("evil.dll") == 0);
}

/* CompareStringA. Returns 111 if all three checks pass. */
__declspec(dllexport) int __stdcall CompareStrings(void) {
  return (CompareStringA(0x409, 1 /* NORM_IGNORECASE */, "Hello", -1, "hELLO", -1) == 2) * 100 +
         (CompareStringA(0, 0, "abcX", 3, "abd", 3) == 1) * 10 + (CompareStringA(0, 0, "abcd", -1, "abc", -1) == 3);
}

/* HeapReAlloc with HEAP_REALLOC_IN_PLACE_ONLY (0x10) fails rather than moving the
   block. Returns 111 if all three checks pass. */
__declspec(dllexport) int __stdcall ReallocInPlace(void) {
  void *heap = GetProcessHeap();
  void *a = HeapAlloc(heap, 0, 64);
  void *b = HeapAlloc(heap, 0, 64); /* a cannot grow into b */
  return (HeapReAlloc(heap, 0x10, a, 48) == a) * 100 + (HeapReAlloc(heap, 0x10, a, 4096) == 0) * 10 +
         (b && HeapReAlloc(heap, 0, a, 4096) != a);
}

/* IsBadReadPtr agrees with the guest's page mapping. Returns 1111 if all four
   checks pass. */
__declspec(dllexport) int __stdcall BadPointers(void) {
  void *heap = HeapAlloc(GetProcessHeap(), 0, 64);
  return IsBadReadPtr((void *)0x00080000, 4) * 1000 +   /* the gap below the stack */
         IsBadReadPtr((void *)0x0003fff0, 0x20) * 100 + /* runs off the end of a mapped range */
         IsBadReadPtr((void *)0x08000000, 4) * 10 +     /* past the end of guest memory */
         (heap && !IsBadReadPtr(heap, 64));
}

__declspec(dllexport) int __stdcall CallDebugBreak(void) {
  DebugBreak();
  return 1;
}

/* REP string instructions over more iterations than the sandbox runs in one
   chunk (4096 steps), so they are stopped and resumed. */
__declspec(dllexport) int __stdcall RepMovs(int n) {
  for (int i = 0; i < n; i++)
    g_big[0][i] = (unsigned char)(i * 7);
  unsigned char *src = g_big[0], *dst = g_big[1];
  unsigned c = (unsigned)n;
  __asm__ volatile("rep movsb" : "+S"(src), "+D"(dst), "+c"(c) : : "memory");
  unsigned sum = 0;
  for (int i = 0; i < 200000; i++)
    sum = sum * 31 + g_big[1][i];
  return (int)sum;
}
/* strlen with repne scasb: the string is n bytes long. */
__declspec(dllexport) int __stdcall RepScas(int n) {
  for (int i = 0; i < n; i++)
    g_big[2][i] = 'x';
  g_big[2][n] = 0;
  unsigned char *p = g_big[2];
  unsigned c = 0xffffffffu;
  __asm__ volatile("repne scasb" : "+D"(p), "+c"(c) : "a"(0) : "memory", "cc");
  return (int)(~c - 1);
}
/* memcmp with repe cmpsb: returns the index of the first difference, at n. */
__declspec(dllexport) int __stdcall RepCmps(int n) {
  for (int i = 0; i < 200000; i++)
    g_big[0][i] = g_big[1][i] = (unsigned char)i;
  g_big[1][n] ^= 1;
  unsigned char *a = g_big[0], *b = g_big[1];
  unsigned c = 200000;
  __asm__ volatile("repe cmpsb" : "+S"(a), "+D"(b), "+c"(c) : : "memory", "cc");
  return (int)(a - g_big[0]) - 1;
}
/* Runs `count` rep stosd instructions of 50000 dwords each. */
__declspec(dllexport) int __stdcall RepStosLoop(int count) {
  for (int i = 0; i < count; i++) {
    void *d = g_big[0];
    unsigned c = 50000;
    __asm__ volatile("rep stosl" : "+D"(d), "+c"(c) : "a"(i) : "memory");
  }
  return 1;
}

/* Memory access at an address the test chooses. */
__declspec(dllexport) int __stdcall ReadAt(unsigned addr) { return *(volatile int *)addr; }
__declspec(dllexport) int __stdcall WriteAt(unsigned addr) {
  *(volatile int *)addr = 1;
  return 1;
}
__declspec(dllexport) int __stdcall CallAt(unsigned addr) { return ((int(__stdcall *)(void))addr)(); }

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
/* The volatile frame keeps the compiler from turning the recursion into a loop. */
__attribute__((noinline)) static int __stdcall RecurseForever(int n) {
  volatile char frame[256];
  frame[0] = (char)n;
  return RecurseForever(n + 1) + frame[0];
}
__declspec(dllexport) int __stdcall StackOverflow(void) { return RecurseForever(0); }

int __stdcall DllMain(void *h, unsigned reason, void *r) {
  (void)h;
  (void)r;
  if (reason == 1)
    g_attached = 7;
  return 1;
}
