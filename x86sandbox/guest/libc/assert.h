#ifndef SB_ASSERT_H
#define SB_ASSERT_H
/* A failed assertion inside the sandbox traps the whole Wasm call. */
#define assert(x) ((x) ? (void)0 : __builtin_trap())
#endif
