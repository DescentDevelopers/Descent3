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
unsigned short *sndDecompM16_dst;
unsigned char *sndDecompM16_src;
unsigned int sndDecompM16_len;
unsigned int sndDecompM16_prev;
unsigned int sndDecompM16_return;
/* For _asm_sndDecompM16 */
unsigned short *sndDecompS16_dst;
unsigned char *sndDecompS16_src;
unsigned int sndDecompS16_len;
unsigned int sndDecompS16_prev;
unsigned int sndDecompS16_return;
/* For _asm_nfHPkDecomp */
unsigned char *nfHPkDecomp_ops;
unsigned char *nfHPkDecomp_comp;
unsigned int nfHPkDecomp_x;
unsigned int nfHPkDecomp_y;
unsigned int nfHPkDecomp_w;
unsigned int nfHPkDecomp_h;

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
  unsigned int page_start;

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
unsigned sndDecompM16(unsigned short *dst, const unsigned char *src, unsigned len, unsigned prev) {
  sndDecompM16_dst = dst;
  sndDecompM16_src = src;
  sndDecompM16_len = len;
  sndDecompM16_prev = prev;
  __asm__(" call _asm_sndDecompM16" : : : "%esi", "%edi", "%ebx", "cc", "memory");
  return (sndDecompM16_return);
}

unsigned sndDecompS16(unsigned short *dst, const unsigned char *src, unsigned len, unsigned prev) {
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

void nfHPkDecomp(unsigned char *ops, unsigned char *comp, unsigned int x, unsigned int y, unsigned int w,
                 unsigned int h) {
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

void nfHiColorDecomp(unsigned char *comp, unsigned x, unsigned y, unsigned w, unsigned h);
void nfHiColorDecompChg(unsigned short *chgs, unsigned short *parms, unsigned char *comp, unsigned x, unsigned y,
                        unsigned w, unsigned h);
void nfDecomp(unsigned char *comp, unsigned x, unsigned y, unsigned w, unsigned h);
void nfDecompChg(unsigned short *chgs, unsigned short *parms, unsigned char *comp, unsigned x, unsigned y, unsigned w,
                 unsigned h);
void nfPkPal(void);
void nfPkDecomp(unsigned char *ops, unsigned char *comp, unsigned x, unsigned y, unsigned w, unsigned h);
void nfPkDecompH(unsigned char *ops, unsigned char *comp, unsigned x, unsigned y, unsigned w, unsigned h);
void nfPkDecompD(unsigned char *ops, unsigned char *comp, unsigned x, unsigned y, unsigned w, unsigned h);
void mve_ShowFrameField(unsigned char *buf, unsigned bufw, unsigned bufh, unsigned sx, unsigned sy, unsigned w,
                        unsigned h, unsigned dstx, unsigned dsty, unsigned field);
void mve_ShowFrameFieldHi(unsigned char *buf, unsigned bufw, unsigned bufh, unsigned sx, unsigned sy, unsigned w,
                          unsigned h, unsigned dstx, unsigned dsty, unsigned field);
void mve_sfShowFrameChg(bool prvbuf, unsigned x, unsigned y, unsigned w, unsigned h, unsigned short *chgs,
                        unsigned dstx, unsigned dsty);
void mve_sfHiColorShowFrameChg(bool prvbuf, unsigned x, unsigned y, unsigned w, unsigned h, unsigned short *chgs,
                               unsigned dstx, unsigned dsty);
void mve_sfPkShowFrameChg(bool prvbuf, unsigned x, unsigned y, unsigned w, unsigned h, unsigned char *ops,
                          unsigned dstx, unsigned dsty);
void mve_sfPkHiColorShowFrameChg(bool prvbuf, unsigned x, unsigned y, unsigned w, unsigned h, unsigned char *ops,
                                 unsigned dstx, unsigned dsty);

void MVE_SetPalette(unsigned char *p, unsigned start, unsigned count);
void palLoadCompPalette(unsigned char *buf);
void gfxMode(unsigned mode);
void gfxLoadCrtc(unsigned char *crtc, unsigned char chain4, unsigned char res);
void gfxGetCrtc(unsigned char *crtc);
void gfxVres(unsigned char misc, unsigned char *crtc);
void MVE_gfxWaitRetrace(int state);
void MVE_gfxSetSplit(unsigned line);

// rcg07272000
// need this on non-Intel platforms. Intel uses int $3.
#if (defined __i386__)
#define int3 __asm__ __volatile__("int $3");
#else
#define int3 raise(SIGTRAP);
#endif

void nfHiColorDecomp(unsigned char *comp, unsigned x, unsigned y, unsigned w, unsigned h) { int3 }
void nfHiColorDecompChg(unsigned short *chgs, unsigned short *parms, unsigned char *comp, unsigned x, unsigned y,
                        unsigned w, unsigned h) {
  int3
}
void nfDecomp(unsigned char *comp, unsigned x, unsigned y, unsigned w, unsigned h) { int3 }
void nfDecompChg(unsigned short *chgs, unsigned short *parms, unsigned char *comp, unsigned x, unsigned y, unsigned w,
                 unsigned h) {
  int3
}
void nfPkPal(void) { int3 }
void nfPkDecomp(unsigned char *ops, unsigned char *comp, unsigned x, unsigned y, unsigned w, unsigned h) { int3 }
void nfPkDecompH(unsigned char *ops, unsigned char *comp, unsigned x, unsigned y, unsigned w, unsigned h) { int3 }
void nfPkDecompD(unsigned char *ops, unsigned char *comp, unsigned x, unsigned y, unsigned w, unsigned h) { int3 }
void mve_ShowFrameField(unsigned char *buf, unsigned bufw, unsigned bufh, unsigned sx, unsigned sy, unsigned w,
                        unsigned h, unsigned dstx, unsigned dsty, unsigned field) {
  int3
}
void mve_ShowFrameFieldHi(unsigned char *buf, unsigned bufw, unsigned bufh, unsigned sx, unsigned sy, unsigned w,
                          unsigned h, unsigned dstx, unsigned dsty, unsigned field) {
  int3
}
void mve_sfShowFrameChg(bool prvbuf, unsigned x, unsigned y, unsigned w, unsigned h, unsigned short *chgs,
                        unsigned dstx, unsigned dsty) {
  int3
}
void mve_sfHiColorShowFrameChg(bool prvbuf, unsigned x, unsigned y, unsigned w, unsigned h, unsigned short *chgs,
                               unsigned dstx, unsigned dsty) {
  int3
}
void mve_sfPkShowFrameChg(bool prvbuf, unsigned x, unsigned y, unsigned w, unsigned h, unsigned char *ops,
                          unsigned dstx, unsigned dsty) {
  int3
}
void mve_sfPkHiColorShowFrameChg(bool prvbuf, unsigned x, unsigned y, unsigned w, unsigned h, unsigned char *ops,
                                 unsigned dstx, unsigned dsty) {
  int3
}
/* Avoid name mangling issues by moving this into mvelibl.cpp - AH
void MVE_SetPalette(unsigned char *p, unsigned start, unsigned count)
{
        int3
} */
void palLoadCompPalette(unsigned char *buf) { int3 }
void gfxMode(unsigned mode) { int3 }
void gfxLoadCrtc(unsigned char *crtc, unsigned char chain4, unsigned char res) { int3 }
void gfxGetCrtc(unsigned char *crtc) { int3 }
void gfxVres(unsigned char misc, unsigned char *crtc) { int3 }
void MVE_gfxWaitRetrace(int state) { int3 }
void MVE_gfxSetSplit(unsigned line) { int3 }
