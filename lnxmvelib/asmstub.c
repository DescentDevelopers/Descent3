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

#include <unistd.h>
#include <sys/mman.h>

/* These are our global variables for passing values - AH */
/* For _asm_sndDecompM16 */
uint16_t *sndDecompM16_dst;
uint8_t *sndDecompM16_src;
uint32_t sndDecompM16_len;
uint32_t sndDecompM16_prev;
uint32_t sndDecompM16_return;
/* For _asm_sndDecompM16 */
uint16_t *sndDecompS16_dst;
uint8_t *sndDecompS16_src;
uint32_t sndDecompS16_len;
uint32_t sndDecompS16_prev;
uint32_t sndDecompS16_return;
/* For _asm_nfHPkDecomp */
uint8_t *nfHPkDecomp_ops;
uint8_t *nfHPkDecomp_comp;
uint32_t nfHPkDecomp_x;
uint32_t nfHPkDecomp_y;
uint32_t nfHPkDecomp_w;
uint32_t nfHPkDecomp_h;

#ifdef __cplusplus /* Avoid C++ name mangling - AH */
extern "C" {
#endif
void _asm_sndDecompM16(void);
void _asm_sndDecompS16(void);
void _asm_nfPkConfig(void);
void _asm_nfHPkDecomp(void);
void _asm_selfModify(void);
#ifdef __cplusplus
}
#endif

#define MAX_MEM_UNLOCK_POINTS 20
int global_unlock_memory_pointers[MAX_MEM_UNLOCK_POINTS]; // _asm_selfModify() sets these

int allow_self_modification(void) {
  int i;
  uint32_t page_start;

  for (i = 0; i < MAX_MEM_UNLOCK_POINTS; i++)
    global_unlock_memory_pointers[i] = 0;

  _asm_selfModify();

  for (i = 0; i < MAX_MEM_UNLOCK_POINTS; i++)
    if (global_unlock_memory_pointers[i] != 0) {
      page_start = global_unlock_memory_pointers[i] - (global_unlock_memory_pointers[i] % getpagesize());
      mprotect((void *)page_start, getpagesize() * 2, PROT_READ | PROT_WRITE | PROT_EXEC);
      // fprintf(stderr, "Unlocked memory location %x for location %x.\n",page_start, global_unlock_memory_pointers[i]);
    }
  return (1);
}
unsigned sndDecompM16(uint16_t *dst, const uint8_t *src, unsigned len, unsigned prev) {
  sndDecompM16_dst = dst;
  sndDecompM16_src = src;
  sndDecompM16_len = len;
  sndDecompM16_prev = prev;
  __asm__(" call _asm_sndDecompM16" : : : "%esi", "%edi", "%ebx", "cc", "memory");
  return (sndDecompM16_return);
}

unsigned sndDecompS16(uint16_t *dst, const uint8_t *src, unsigned len, unsigned prev) {
  sndDecompS16_dst = dst;
  sndDecompS16_src = src;
  sndDecompS16_len = len;
  sndDecompS16_prev = prev;
  __asm__("call _asm_sndDecompS16" : : : "%esi", "%edi", "%ebx", "cc", "memory");
  return (sndDecompS16_return);
}

void nfPkConfig(void) {
  __asm__("call _asm_nfPkConfig" : : : "%esi", "%edi", "%ebx", "cc", "memory");
  return;
}

void nfHPkDecomp(uint8_t *ops, uint8_t *comp, uint32_t x, uint32_t y, uint32_t w, uint32_t h) {
  nfHPkDecomp_ops = ops;
  nfHPkDecomp_comp = comp;
  nfHPkDecomp_x = x;
  nfHPkDecomp_y = y;
  nfHPkDecomp_w = w;
  nfHPkDecomp_h = h;
  __asm__("call _asm_nfHPkDecomp" : : : "%esi", "%edi", "%ebx", "cc", "memory");
  return;
}

/***********************************************************/
/* Non-Implemented functions (from inside mveasm.cpp - AH) */
/***********************************************************/

void nfHiColorDecomp(uint8_t *comp, unsigned x, unsigned y, unsigned w, unsigned h);
void nfHiColorDecompChg(uint16_t *chgs, uint16_t *parms, uint8_t *comp, unsigned x, unsigned y, unsigned w, unsigned h);
void nfDecomp(uint8_t *comp, unsigned x, unsigned y, unsigned w, unsigned h);
void nfDecompChg(uint16_t *chgs, uint16_t *parms, uint8_t *comp, unsigned x, unsigned y, unsigned w, unsigned h);
void nfPkPal(void);
void nfPkDecomp(uint8_t *ops, uint8_t *comp, unsigned x, unsigned y, unsigned w, unsigned h);
void nfPkDecompH(uint8_t *ops, uint8_t *comp, unsigned x, unsigned y, unsigned w, unsigned h);
void nfPkDecompD(uint8_t *ops, uint8_t *comp, unsigned x, unsigned y, unsigned w, unsigned h);
void mve_ShowFrameField(uint8_t *buf, unsigned bufw, unsigned bufh, unsigned sx, unsigned sy, unsigned w, unsigned h,
                        unsigned dstx, unsigned dsty, unsigned field);
void mve_ShowFrameFieldHi(uint8_t *buf, unsigned bufw, unsigned bufh, unsigned sx, unsigned sy, unsigned w, unsigned h,
                          unsigned dstx, unsigned dsty, unsigned field);
void mve_sfShowFrameChg(bool prvbuf, unsigned x, unsigned y, unsigned w, unsigned h, uint16_t *chgs, unsigned dstx,
                        unsigned dsty);
void mve_sfHiColorShowFrameChg(bool prvbuf, unsigned x, unsigned y, unsigned w, unsigned h, uint16_t *chgs,
                               unsigned dstx, unsigned dsty);
void mve_sfPkShowFrameChg(bool prvbuf, unsigned x, unsigned y, unsigned w, unsigned h, uint8_t *ops, unsigned dstx,
                          unsigned dsty);
void mve_sfPkHiColorShowFrameChg(bool prvbuf, unsigned x, unsigned y, unsigned w, unsigned h, uint8_t *ops,
                                 unsigned dstx, unsigned dsty);

void MVE_SetPalette(uint8_t *p, unsigned start, unsigned count);
void palLoadCompPalette(uint8_t *buf);
void gfxMode(unsigned mode);
void gfxLoadCrtc(uint8_t *crtc, uint8_t chain4, uint8_t res);
void gfxGetCrtc(uint8_t *crtc);
void gfxVres(uint8_t misc, uint8_t *crtc);
void MVE_gfxWaitRetrace(int state);
void MVE_gfxSetSplit(unsigned line);

// rcg07272000
// need this on non-Intel platforms. Intel uses int $3.
#if (defined __i386__) || defined(__x86_64__)
#define int3 __asm__ __volatile__("int $3");
#else
#define int3 raise(SIGTRAP);
#endif

void nfHiColorDecomp(uint8_t *comp, unsigned x, unsigned y, unsigned w, unsigned h) { int3 }
void nfHiColorDecompChg(uint16_t *chgs, uint16_t *parms, uint8_t *comp, unsigned x, unsigned y, unsigned w,
                        unsigned h) {
  int3
}
void nfDecomp(uint8_t *comp, unsigned x, unsigned y, unsigned w, unsigned h) { int3 }
void nfDecompChg(uint16_t *chgs, uint16_t *parms, uint8_t *comp, unsigned x, unsigned y, unsigned w, unsigned h) {
  int3
}
void nfPkPal(void) { int3 }
void nfPkDecomp(uint8_t *ops, uint8_t *comp, unsigned x, unsigned y, unsigned w, unsigned h) { int3 }
void nfPkDecompH(uint8_t *ops, uint8_t *comp, unsigned x, unsigned y, unsigned w, unsigned h) { int3 }
void nfPkDecompD(uint8_t *ops, uint8_t *comp, unsigned x, unsigned y, unsigned w, unsigned h) { int3 }
void mve_ShowFrameField(uint8_t *buf, unsigned bufw, unsigned bufh, unsigned sx, unsigned sy, unsigned w, unsigned h,
                        unsigned dstx, unsigned dsty, unsigned field) {
  int3
}
void mve_ShowFrameFieldHi(uint8_t *buf, unsigned bufw, unsigned bufh, unsigned sx, unsigned sy, unsigned w, unsigned h,
                          unsigned dstx, unsigned dsty, unsigned field) {
  int3
}
void mve_sfShowFrameChg(bool prvbuf, unsigned x, unsigned y, unsigned w, unsigned h, uint16_t *chgs, unsigned dstx,
                        unsigned dsty) {
  int3
}
void mve_sfHiColorShowFrameChg(bool prvbuf, unsigned x, unsigned y, unsigned w, unsigned h, uint16_t *chgs,
                               unsigned dstx, unsigned dsty) {
  int3
}
void mve_sfPkShowFrameChg(bool prvbuf, unsigned x, unsigned y, unsigned w, unsigned h, uint8_t *ops, unsigned dstx,
                          unsigned dsty) {
  int3
}
void mve_sfPkHiColorShowFrameChg(bool prvbuf, unsigned x, unsigned y, unsigned w, unsigned h, uint8_t *ops,
                                 unsigned dstx, unsigned dsty) {
  int3
}
/* Avoid name mangling issues by moving this into mvelibl.cpp - AH
void MVE_SetPalette(uint8_t *p, unsigned start, unsigned count)
{
        int3
} */
void palLoadCompPalette(uint8_t *buf) { int3 }
void gfxMode(unsigned mode) { int3 }
void gfxLoadCrtc(uint8_t *crtc, uint8_t chain4, uint8_t res) { int3 }
void gfxGetCrtc(uint8_t *crtc) { int3 }
void gfxVres(uint8_t misc, uint8_t *crtc) { int3 }
void MVE_gfxWaitRetrace(int state) { int3 }
void MVE_gfxSetSplit(unsigned line) { int3 }
