/* stubs for the little bits of loki_utils we actually need. */

#ifndef _INCL_LOKI_UTILS_H_
#define _INCL_LOKI_UTILS_H_

// never liked these register option thingys...
#define loki_register_stdoptions()
#define loki_registeroption(x, y, z)
#define loki_setgamename(x, y, z)

#ifdef __cplusplus
extern "C" {
#endif

void loki_initialize(int argc, char **argv, char *desc);
int loki_getmountpoint(const char *device, char *mountpoint, int max_size);
const char *loki_getdatapath(void);
const char *loki_getprefpath(void);

#ifdef __cplusplus
}
#endif

#endif
