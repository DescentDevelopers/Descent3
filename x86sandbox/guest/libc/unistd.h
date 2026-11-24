#ifndef SB_UNISTD_H
#define SB_UNISTD_H
static inline int usleep(unsigned int us) { (void)us; return 0; }
#endif
