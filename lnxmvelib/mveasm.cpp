
/*
** mveasm.cpp
**
** Interplay Movie File (MVE) Player Library (32-Bit Linux Version)
** Written by Paul Allen Edelstein. Partial Linux port by Jeff Slutter.
**
** (c) 1997 Interplay Productions.  All Rights Reserved.
** This file is confidential and consists of proprietary information
** of Interplay Productions.  This file and associated libraries
** may not, in whole or in part, be disclosed to third parties,
** incorporated into any software product which is not being created
** for Interplay Productions, copied or duplicated in any form,
** without the prior written permission of Interplay Productions.
** Further, you may not reverse engineer, decompile or otherwise
** attempt to derive source code of this material.
**
*/
#include "mvelibl.h"
#include "mvelibi.h"
#include "byteswap.h"

// rcg07272000
// need this for SIGTRAP on non-Intel platforms. Intel uses int $3.
#if (!defined __i386__)
#include <signal.h>
#endif

extern unsigned char *nf_buf_cur;
extern unsigned char *nf_buf_prv;
extern unsigned nf_new_x;
extern unsigned nf_new_y;
extern unsigned nf_new_w;
extern unsigned nf_new_h;
extern unsigned char nf_fqty;  // Number of fields
extern unsigned nf_new_row0;   // SHEIGHT*width*2-width
extern unsigned nf_width;      // wqty * SWIDTH
extern unsigned nf_new_line;   // width - SWIDTH
extern unsigned nf_back_right; // (SHEIGHT-1)*width

extern signed short snd_8to16[256];
void nfHPkDecomp(unsigned char *ops, unsigned char *comp, int x, int y, int w, int h);
void nfPkConfig(void);
unsigned sndDecompM16(unsigned short *dst, const unsigned char *src, unsigned len, unsigned prev);
unsigned sndDecompS16(unsigned short *dst, const unsigned char *src, unsigned len, unsigned prev);

void Trans16Blk(unsigned char *edi, unsigned char *idx);
void call_hnfxycshift(unsigned int eax, unsigned char **medi, unsigned char **mesi, int nfpk_back_right);
void call_hnfxypshift(unsigned int eax, unsigned char **medi, unsigned char **mesi, int nfpk_back_right,
                      int DiffBufPtrs);
void call_hnfshift(unsigned int meax, unsigned char **medi, unsigned char **mesi, int nfpk_back_right);

//--------------------------------------------------------------------
// Sound Management
//--------------------

// Decompresses a mono stream containing len samples
// (src is len bytes, dst is len*2 bytes)
// prev is the previous decompression state or zero.
// Returns new decompression state.
unsigned sndDecompM16(unsigned short *dst, const unsigned char *src, unsigned len, unsigned prev) {
  unsigned int i, eax, ebx;
  if (len == 0)
    return prev;
  eax = prev;
  ebx = 0;

  for (i = 0; i < len; i++) {
    ebx = *src;
    src++;

    eax += snd_8to16[ebx];
    *dst = eax & 0xFFFF;
    dst++;
  }

  return eax;
}

//	Decompresses a stereo stream containing len samples
//	(src is len*2 bytes, dst is len*4 bytes)
//	prev is the previous decompression state or zero
//	(It encodes the 16-bit states of the two stereo channels
//	in its low and high order 16-bit halves.)
//	Returns new decompression state.
unsigned sndDecompS16(unsigned short *dst, const unsigned char *src, unsigned len, unsigned prev) {
  unsigned re = 0;
  unsigned int eax, edx, ebx, i;

  if (len == 0) {
    return prev;
  }

  eax = (prev & 0xFFFF);
  edx = ((prev & 0xFFFF0000) >> 16);
  ebx = 0;

  for (i = 0; i < len; i++) {
    ebx = *src;
    src++;

    eax += snd_8to16[ebx];
    *dst = (eax & 0xFFFF);
    dst++;

    ebx = *src;
    src++;

    edx += snd_8to16[ebx];
    *dst = (edx & 0xFFFF);
    dst++;
  }

  return (eax & 0xFFFF) | ((edx & 0xFFFF) << 16);
}

typedef struct {
  unsigned char *comp;
  unsigned char *tbuf;
  int new_row, DiffBufPtrs, parms_sz;
  unsigned x, y, w, h;
} tNextFrame;

// NOTE: On exit ebx is nf_fqty
void NF_DECOMP_INIT(bool HI_COLOR_FLAG, tNextFrame *nf) {
  nf->DiffBufPtrs = nf_buf_prv - nf_buf_cur;

  if (HI_COLOR_FLAG) {
    nf_new_x = nf->x << (LOG2_SWIDTH + 1);
    nf_new_w = nf->w << (LOG2_SWIDTH + 1);
  } else {
    nf_new_x = nf->x << LOG2_SWIDTH;
    nf_new_w = nf->w << LOG2_SWIDTH;
  }
  nf_new_y = (nf->y << LOG2_SHEIGHT) * nf_fqty;
  nf_new_h = (nf->h << LOG2_SHEIGHT) * nf_fqty;

  nf->new_row = nf_new_row0 - nf_new_w;

  //	Move to correct place in current buffer
  nf->tbuf = nf_buf_cur;
  if (nf->x || nf->y) {
    nf->tbuf += nf_new_y * nf_width + nf_new_x;
  }
}

//----------------------------------------------------------------------

// signed 8-bit y * nf_width
signed int nfpk_ShiftY[256];

// Constant tables

// 8-bit -8:7 x nf_width +  -8:7
signed short nfpk_ShiftP1[256];
signed short nfpk_ShiftP2[256];

// Constant tables
//		mov eax, ebx/ecx
// EBX = 0
// ECX = 1
unsigned char nfhpk_mov4l[64];

//	mov ds:[edi+0/4/8/12], ebx/edx/ecx/ebp
// EBX = 0
// EDX = 1
// ECX = 2
// EBP = 3
unsigned char nfhpk_mov8[1024];

//	mov eax, ebx/edx/ecx/ebp
// EBX = 0
// EDX = 1
// ECX = 2
// EBP = 3
unsigned char nfhpk_mov4[1024];

class initme {
public:
  initme() {
    int x, y;
    int m4, m3, m2, m1;
    signed char *ptr;
    unsigned char *uptr;

    // Do nfhpk_mov4l
    uptr = nfhpk_mov4l;
    for (m4 = 0; m4 < 2; m4++)
      for (m3 = 0; m3 < 2; m3++)
        for (m2 = 0; m2 < 2; m2++)
          for (m1 = 0; m1 < 2; m1++) {
            *uptr = m1;
            uptr++;
            *uptr = m2;
            uptr++;
            *uptr = m3;
            uptr++;
            *uptr = m4;
            uptr++;
          }

    // Do nfhpk_mov8
    uptr = nfhpk_mov8;
    for (m4 = 0; m4 < 4; m4++)
      for (m3 = 0; m3 < 4; m3++)
        for (m2 = 0; m2 < 4; m2++)
          for (m1 = 0; m1 < 4; m1++) {
            *uptr = m1;
            uptr++;
            *uptr = m2;
            uptr++;
            *uptr = m3;
            uptr++;
            *uptr = m4;
            uptr++;
          }

    // Do nfhpk_mov4
    uptr = nfhpk_mov4;
    for (m4 = 0; m4 < 4; m4++)
      for (m3 = 0; m3 < 4; m3++)
        for (m2 = 0; m2 < 4; m2++)
          for (m1 = 0; m1 < 4; m1++) {
            *uptr = m1;
            uptr++;
            *uptr = m2;
            uptr++;
            *uptr = m3;
            uptr++;
            *uptr = m4;
            uptr++;
          }
    // do nfpk_ShiftP1
    ptr = (signed char *)nfpk_ShiftP1;

    for (y = -8; y != 8; y++) {
      for (x = -8; x != 8; x++) {
#ifdef OUTRAGE_BIG_ENDIAN
        *(ptr) = y;
        *(ptr + 1) = x;
#else
        *(ptr) = x;
        *(ptr + 1) = y;
#endif
        ptr += 2;
      }
    }

    // do nfpk_ShiftP2[]
    ptr = (signed char *)nfpk_ShiftP2;

    for (y = 0; y != 8; y++) {
      for (x = 8; x != 15; x++) {
#ifdef OUTRAGE_BIG_ENDIAN
        *(ptr) = y;
        *(ptr + 1) = x;
#else
        *(ptr) = x;
        *(ptr + 1) = y;
#endif
        ptr += 2;
      }
    }

    for (y = 8; y != 14; y++) {
      for (x = -14; x != 0; x++) {
        *(ptr) = x;
        *(ptr + 1) = y;
        ptr += 2;
      }

      for (x = 0; x != 15; x++) {
        *(ptr) = x;
        *(ptr + 1) = y;
        ptr += 2;
      }
    }
    for (x = -14; x != 0; x++) {
      *(ptr) = x;
      *(ptr + 1) = 14;
      ptr += 2;
    }
    for (x = 0; x != 12; x++) {
      *(ptr) = x;
      *(ptr + 1) = 14;
      ptr += 2;
    }
  }
};

initme _initme;

//	nfPkConfig initializes tables used by nfPkDecomp
//	which are dependent on screen size.
void nfPkConfig(void) {
  // Build ShiftY table
  int i, val, index;

  val = 0;
  index = 0;

  for (i = 0; i < 128; i++) {
    nfpk_ShiftY[index] = val;
    index++;
    val += nf_width;
  }

  val = nf_width;
  val = val << 7;
  val = -val;

  for (i = 0; i < 128; i++) {
    nfpk_ShiftY[index] = val;
    index++;
    val += nf_width;
  }
}

extern unsigned short nf_trans16_lo[256];
extern unsigned short nf_trans16_hi[256];

// NOTE: EAX is destroyed after this call (actually the value of
// nf_trans16_hi[idx+1]
void Trans16(unsigned short *dst, unsigned short *idx, bool mask) {
  *dst = nf_trans16_lo[*idx];
  *dst |= nf_trans16_hi[*(idx + 1)];
}

//	HiColor version
//
void nfHPkDecomp(unsigned char *ops, unsigned char *comp, unsigned x, unsigned y, unsigned w, unsigned h) {
  tNextFrame nf;
  nf.w = w;
  nf.h = h;
  nf.x = x;
  nf.y = y;

  int nfpk_back_right, wcnt;
  unsigned char *bcomp;
  unsigned char *esi, *edi;
  unsigned char opcode;

  NF_DECOMP_INIT(1, &nf);

  nfpk_back_right = nf_back_right - SWIDTH * 2;

  esi = comp;
  edi = nf.tbuf;

  unsigned short swapped = (*(unsigned short *)esi);
  swapped = INTEL_SHORT(swapped);
  bcomp = swapped + esi;
  esi = esi + 2;

  wcnt = w >> 1;

  do {
    wcnt--;
    if (wcnt >= 0) {
      opcode = *ops; // al == opcode
      ops++;

      bool first_opcode = true;
      int opcode_to_use = opcode & 0xF;

    do_next_opcode:

      switch (opcode_to_use) {
      case 0: {
        // No change from previous buffer
        call_hnfshift(nf.DiffBufPtrs, &edi, &esi, nfpk_back_right);
      } break;
      case 1: {
        // No change (and copied to screen) (0)
        edi += SWIDTH * 2;
      } break;
      case 2: {
        unsigned int eax;
        eax = *bcomp;
        bcomp++;
        eax = nfpk_ShiftP2[eax];

        call_hnfxycshift(eax, &edi, &esi, nfpk_back_right);

      } break;
      case 3: {
        // Near shift from newer part of current buffer
#ifdef OUTRAGE_BIG_ENDIAN
        typedef struct {
          unsigned short hax, ax;
        } reg_word;
        typedef struct {
          signed char hah, hal, ah, al;
        } reg_byte;
#else
        typedef struct {
          unsigned short ax, hax;
        } reg_word;
        typedef struct {
          signed char al, ah, hal, hah;
        } reg_byte;
#endif
        union {
          unsigned int eax;
          reg_word word;
          reg_byte byte;
        } myeax;

        myeax.eax = *bcomp;
        bcomp++;

        myeax.word.ax = nfpk_ShiftP2[myeax.eax];
        myeax.byte.al = -myeax.byte.al;
        myeax.byte.ah = -myeax.byte.ah;

        call_hnfxycshift(myeax.eax, &edi, &esi, nfpk_back_right);

      } break;
      case 4: {
        // Near shift from previous buffer
#ifdef OUTRAGE_BIG_ENDIAN
        typedef struct {
          unsigned short hax, ax;
        } reg_word;
        typedef struct {
          signed char hah, hal, ah, al;
        } reg_byte;
#else
        typedef struct {
          unsigned short ax, hax;
        } reg_word;
        typedef struct {
          signed char al, ah, hal, hah;
        } reg_byte;
#endif
        union {
          unsigned int eax;
          reg_word word;
          reg_byte byte;
        } myeax;

        myeax.eax = 0;
        myeax.byte.al = *bcomp;
        bcomp++;

        myeax.word.ax = nfpk_ShiftP1[myeax.eax];
        call_hnfxypshift(myeax.eax, &edi, &esi, nfpk_back_right, nf.DiffBufPtrs);

      } break;
      case 5: {
        unsigned short swapper = *(unsigned short *)(esi);
        unsigned int eax = INTEL_SHORT(swapper);
        esi += 2;
        call_hnfxypshift(eax, &edi, &esi, nfpk_back_right, nf.DiffBufPtrs);
      } break;
      case 6: {
        // Far shift from current buffer
        unsigned int val1, val2;

        unsigned short swapper = *(unsigned short *)(esi);
        val1 = INTEL_SHORT(swapper);
        esi += 2;

        val2 = ((val1 & 0xFF00) >> 8);

        // sign extend al into eax, and multiply by two
        if (val1 & 0x80) {
          val1 = ((val1 & 0x7F) << 1);
          val1 = (val1 & 0xFF) | 0xFFFFFF00;
        } else {
          val1 = ((val1 & 0x7F) << 1);
        }

        val1 += nfpk_ShiftY[val2];

        call_hnfshift(val1, &edi, &esi, nfpk_back_right);

      } break;
      case 7: {
        bool donf23 = false;
        unsigned short val;
        int rep_count, max_repcount;

        val = *(unsigned short *)esi;
        val = INTEL_SHORT(val);
        if (val & 0x8000) {
          donf23 = true;
        }

        if (!donf23) {
          unsigned int colors[4];
          max_repcount = 8;

          unsigned int temp_color;
          temp_color = nf_trans16_lo[*(esi + 2)] | nf_trans16_hi[*(esi + 3)];
          temp_color = (nf_trans16_lo[*(esi + 0)] | nf_trans16_hi[*(esi + 1)]) | (temp_color << 16);

          colors[2] = temp_color;
          temp_color = ((temp_color & 0xFFFF0000) >> 16) | ((temp_color & 0xFFFF) << 16);
          colors[0] = (temp_color & 0xFFFF0000) | (colors[2] & 0xFFFF);
          colors[1] = temp_color;
          colors[3] = (colors[2] & 0xFFFF0000) | (colors[1] & 0xFFFF);

          for (rep_count = 0; rep_count < max_repcount; rep_count++) {
            unsigned int w1, w2, w3, w4;
            unsigned int color_idx;
            int idx;

            idx = rep_count + 4;
            color_idx = *(unsigned int *)(nfhpk_mov8 + ((*(esi + idx)) * 4));
            w1 = colors[color_idx & 0x000000FF];
            w2 = colors[(color_idx & 0x0000FF00) >> 8];
            w3 = colors[(color_idx & 0x00FF0000) >> 16];
            w4 = colors[(color_idx & 0xFF000000) >> 24];

            *(unsigned int *)(edi + 0) = w1;
            *(unsigned int *)(edi + 4) = w2;
            *(unsigned int *)(edi + 8) = w3;
            *(unsigned int *)(edi + 12) = w4;
            if (rep_count != (max_repcount - 1))
              edi += nf_width;
          }

          esi += 12;
          edi -= nfpk_back_right;
        } else {
          unsigned int colors[2];
          max_repcount = 4;

          unsigned int temp;
          temp = nf_trans16_lo[*(esi)] | nf_trans16_hi[*(esi + 1)];
          colors[0] = (temp << 16) | temp;
          temp = nf_trans16_lo[*(esi + 2)] | nf_trans16_hi[*(esi + 3)];
          colors[1] = (temp << 16) | temp;

          for (rep_count = 0; rep_count < max_repcount; rep_count++) {
            unsigned int w1, w2, w3, w4;
            unsigned int color_idx;
            int idx;

            // 4l,4h,5l,5h (nibbles)
            switch (rep_count) {
            case 0:
              idx = (*(esi + 4)) & 0xF;
              break;
            case 1:
              idx = ((*(esi + 4)) >> 4);
              break;
            case 2:
              idx = (*(esi + 5)) & 0xF;
              break;
            case 3:
              idx = ((*(esi + 5)) >> 4);
              break;
            }

            color_idx = *(unsigned int *)(nfhpk_mov4l + (idx * 4));
            w1 = colors[color_idx & 0x000000FF];
            w2 = colors[(color_idx & 0x0000FF00) >> 8];
            w3 = colors[(color_idx & 0x00FF0000) >> 16];
            w4 = colors[(color_idx & 0xFF000000) >> 24];

            *(unsigned int *)(edi + 0) = w1;
            *(unsigned int *)(edi + nf_width) = w1;
            *(unsigned int *)(edi + 4) = w2;
            *(unsigned int *)(edi + nf_width + 4) = w2;
            *(unsigned int *)(edi + 8) = w3;
            *(unsigned int *)(edi + nf_width + 8) = w3;
            *(unsigned int *)(edi + 12) = w4;
            *(unsigned int *)(edi + nf_width + 12) = w4;
            if (rep_count != (max_repcount - 1))
              edi = edi + nf_width * 2;
          }
          edi += nf_width;
          edi -= nfpk_back_right;
          esi += 6;
        }
      } break;
      case 8: {
        bool donf24 = false;
        bool donf40 = false;
        unsigned short val;
        int rep_count, max_repcount;

        val = *(unsigned short *)esi;
        val = INTEL_SHORT(val);
        if (val & 0x8000) {
          val = *(unsigned short *)(esi + 8);
          val = INTEL_SHORT(val);
          if (val & 0x8000) {
            donf40 = true;
          } else {
            donf24 = true;
          }
        }

        if (!donf24 && !donf40) {
          unsigned int colors[4];
          max_repcount = 8;

          unsigned int tempcolor;
          tempcolor = nf_trans16_lo[*(esi + 2)] | nf_trans16_hi[*(esi + 3)];
          tempcolor = (tempcolor << 16) | (nf_trans16_lo[*(esi + 0)] | nf_trans16_hi[*(esi + 1)]);
          colors[2] = tempcolor;
          tempcolor = ((tempcolor & 0xFFFF0000) >> 16) | ((tempcolor & 0xFFFF) << 16);
          colors[0] = (tempcolor & 0xFFFF0000) | (colors[2] & 0xFFFF);
          colors[1] = tempcolor;
          colors[3] = (colors[2] & 0xFFFF0000) | (colors[1] & 0xFFFF);

          for (rep_count = 0; rep_count < max_repcount; rep_count++) {
            unsigned int w1, w2, w3, w4;
            unsigned int color_idx;
            int idx;

            switch (rep_count) {
            case 0:
              idx = 4;
              break;
            case 1:
              idx = 5;
              break;
            case 2:
              idx = 10;
              break;
            case 3:
              idx = 11;
              break;
            case 4:
              idx = 16;
              break;
            case 5:
              idx = 17;
              break;
            case 6:
              idx = 22;
              break;
            case 7:
              idx = 23;
              break;
            }

            color_idx = *(unsigned int *)(nfhpk_mov8 + (*(esi + idx) * 4));
            w1 = colors[color_idx & 0x000000FF];
            w2 = colors[(color_idx & 0x0000FF00) >> 8];
            w3 = colors[(color_idx & 0x00FF0000) >> 16];
            w4 = colors[(color_idx & 0xFF000000) >> 24];

            *(unsigned int *)(edi + 0) = w1;
            *(unsigned int *)(edi + 4) = w2;
            edi += nf_width;
            *(unsigned int *)(edi + 0) = w3;
            *(unsigned int *)(edi + 4) = w4;
            if (rep_count != (max_repcount - 1))
              edi += nf_width;

            if (rep_count == 1) {
              tempcolor = nf_trans16_lo[*(esi + 8)] | nf_trans16_hi[*(esi + 9)];
              tempcolor = (tempcolor << 16) | (nf_trans16_lo[*(esi + 6)] | nf_trans16_hi[*(esi + 7)]);
              colors[2] = tempcolor;
              tempcolor = ((tempcolor & 0xFFFF0000) >> 16) | ((tempcolor & 0xFFFF) << 16);
              colors[0] = (tempcolor & 0xFFFF0000) | (colors[2] & 0xFFFF);
              colors[1] = tempcolor;
              colors[3] = (colors[2] & 0xFFFF0000) | (colors[1] & 0xFFFF);
            }

            if (rep_count == 3) {
              edi -= nf_width * 8 - 8;

              tempcolor = nf_trans16_lo[*(esi + 14)] | nf_trans16_hi[*(esi + 15)];
              tempcolor = (tempcolor << 16) | (nf_trans16_lo[*(esi + 12)] | nf_trans16_hi[*(esi + 13)]);
              colors[2] = tempcolor;
              tempcolor = ((tempcolor & 0xFFFF0000) >> 16) | ((tempcolor & 0xFFFF) << 16);
              colors[0] = (tempcolor & 0xFFFF0000) | (colors[2] & 0xFFFF);
              colors[1] = tempcolor;
              colors[3] = (colors[2] & 0xFFFF0000) | (colors[1] & 0xFFFF);
            }

            if (rep_count == 5) {
              tempcolor = nf_trans16_lo[*(esi + 20)] | nf_trans16_hi[*(esi + 21)];
              tempcolor = (tempcolor << 16) | (nf_trans16_lo[*(esi + 18)] | nf_trans16_hi[*(esi + 19)]);
              colors[2] = tempcolor;
              tempcolor = ((tempcolor & 0xFFFF0000) >> 16) | ((tempcolor & 0xFFFF) << 16);
              colors[0] = (tempcolor & 0xFFFF0000) | (colors[2] & 0xFFFF);
              colors[1] = tempcolor;
              colors[3] = (colors[2] & 0xFFFF0000) | (colors[1] & 0xFFFF);
            }
          }

          esi += 24;
          edi -= (8 + nfpk_back_right);
        }

        if (donf24) {
          unsigned int colors[4];
          max_repcount = 8;

          unsigned int tempcolor;
          tempcolor = nf_trans16_lo[*(esi + 2)] | nf_trans16_hi[*(esi + 3)];
          tempcolor = (tempcolor << 16) | (nf_trans16_lo[*(esi + 0)] | nf_trans16_hi[*(esi + 1)]);
          colors[2] = tempcolor;
          tempcolor = ((tempcolor & 0xFFFF0000) >> 16) | ((tempcolor & 0xFFFF) << 16);
          colors[0] = (tempcolor & 0xFFFF0000) | (colors[2] & 0xFFFF);
          colors[1] = tempcolor;
          colors[3] = (colors[2] & 0xFFFF0000) | (colors[1] & 0xFFFF);

          for (rep_count = 0; rep_count < max_repcount; rep_count++) {
            unsigned int w1, w2, w3, w4;
            unsigned int color_idx;
            int idx;

            switch (rep_count) {
            case 0:
              idx = 4;
              break;
            case 1:
              idx = 5;
              break;
            case 2:
              idx = 6;
              break;
            case 3:
              idx = 7;
              break;
            case 4:
              idx = 12;
              break;
            case 5:
              idx = 13;
              break;
            case 6:
              idx = 14;
              break;
            case 7:
              idx = 15;
              break;
            }

            color_idx = *(unsigned int *)(nfhpk_mov8 + (*(esi + idx) * 4));
            w1 = colors[color_idx & 0x000000FF];
            w2 = colors[(color_idx & 0x0000FF00) >> 8];
            w3 = colors[(color_idx & 0x00FF0000) >> 16];
            w4 = colors[(color_idx & 0xFF000000) >> 24];

            *(unsigned int *)(edi + 0) = w1;
            *(unsigned int *)(edi + 4) = w2;
            edi += nf_width;
            *(unsigned int *)(edi + 0) = w3;
            *(unsigned int *)(edi + 4) = w4;
            if (rep_count != (max_repcount - 1))
              edi += nf_width;

            if (rep_count == 3) {
              edi -= nf_width * 8 - 8;

              tempcolor = nf_trans16_lo[*(esi + 10)] | nf_trans16_hi[*(esi + 11)];
              tempcolor = (tempcolor << 16) | (nf_trans16_lo[*(esi + 8)] | nf_trans16_hi[*(esi + 9)]);
              colors[2] = tempcolor;
              tempcolor = ((tempcolor & 0xFFFF0000) >> 16) | ((tempcolor & 0xFFFF) << 16);
              colors[0] = (tempcolor & 0xFFFF0000) | (colors[2] & 0xFFFF);
              colors[1] = tempcolor;
              colors[3] = (colors[2] & 0xFFFF0000) | (colors[1] & 0xFFFF);
            }
          }

          esi += 16;
          edi -= (8 + nfpk_back_right);
        }

        if (donf40) {
          unsigned int colors[4];
          max_repcount = 8;

          unsigned int tempcolor;
          tempcolor = nf_trans16_lo[*(esi + 2)] | nf_trans16_hi[*(esi + 3)];
          tempcolor = (tempcolor << 16) | (nf_trans16_lo[*(esi + 0)] | nf_trans16_hi[*(esi + 1)]);
          colors[2] = tempcolor;
          tempcolor = ((tempcolor & 0xFFFF0000) >> 16) | ((tempcolor & 0xFFFF) << 16);
          colors[0] = (tempcolor & 0xFFFF0000) | (colors[2] & 0xFFFF);
          colors[1] = tempcolor;
          colors[3] = (colors[2] & 0xFFFF0000) | (colors[1] & 0xFFFF);

          for (rep_count = 0; rep_count < max_repcount; rep_count++) {
            unsigned int w1, w2, w3, w4;
            unsigned int color_idx;
            int idx;

            switch (rep_count) {
            case 0:
              idx = 4;
              break;
            case 1:
              idx = 5;
              break;
            case 2:
              idx = 6;
              break;
            case 3:
              idx = 7;
              break;
            case 4:
              idx = 12;
              break;
            case 5:
              idx = 13;
              break;
            case 6:
              idx = 14;
              break;
            case 7:
              idx = 15;
              break;
            }

            color_idx = *(unsigned int *)(nfhpk_mov8 + (*(esi + idx) * 4));
            w1 = colors[color_idx & 0x000000FF];
            w2 = colors[(color_idx & 0x0000FF00) >> 8];
            w3 = colors[(color_idx & 0x00FF0000) >> 16];
            w4 = colors[(color_idx & 0xFF000000) >> 24];

            *(unsigned int *)(edi + 0) = w1;
            *(unsigned int *)(edi + 4) = w2;
            *(unsigned int *)(edi + 8) = w3;
            *(unsigned int *)(edi + 12) = w4;
            if (rep_count != (max_repcount - 1))
              edi += nf_width;

            if (rep_count == 3) {
              tempcolor = nf_trans16_lo[*(esi + 10)] | nf_trans16_hi[*(esi + 11)];
              tempcolor = (tempcolor << 16) | (nf_trans16_lo[*(esi + 8)] | nf_trans16_hi[*(esi + 9)]);
              colors[2] = tempcolor;
              tempcolor = ((tempcolor & 0xFFFF0000) >> 16) | ((tempcolor & 0xFFFF) << 16);
              colors[0] = (tempcolor & 0xFFFF0000) | (colors[2] & 0xFFFF);
              colors[1] = tempcolor;
              colors[3] = (colors[2] & 0xFFFF0000) | (colors[1] & 0xFFFF);
            }
          }

          esi += 16;
          edi -= nfpk_back_right;
        }
      } break;
      case 9: {
        bool donf41 = false;
        bool donf25 = false;
        bool donf57 = false;
        unsigned short val;
        int rep_count, max_repcount;

        val = *(unsigned short *)esi;
        val = INTEL_SHORT(val);
        if (val & 0x8000) {
          val = *(unsigned short *)(esi + 4);
          val = INTEL_SHORT(val);
          if (val & 0x8000) {
            donf57 = true;
          } else {
            donf41 = true;
          }

        } else {
          val = *(unsigned short *)(esi + 4);
          val = INTEL_SHORT(val);
          if (val & 0x8000) {
            donf25 = true;
          }
        }

        if (donf57) {
          unsigned short colors[4];
          max_repcount = 8;

          colors[0] = nf_trans16_lo[*(esi)] | nf_trans16_hi[*(esi + 1)];
          colors[1] = nf_trans16_lo[*(esi + 2)] | nf_trans16_hi[*(esi + 3)];
          colors[2] = nf_trans16_lo[*(esi + 4)] | nf_trans16_hi[*(esi + 5)];
          colors[3] = nf_trans16_lo[*(esi + 6)] | nf_trans16_hi[*(esi + 7)];

          for (rep_count = 0; rep_count < max_repcount; rep_count++) {
            unsigned int w1, w2, w3, w4;
            unsigned int color_idx;
            int idx;

            idx = rep_count + 8;
            color_idx = *(unsigned int *)(nfhpk_mov4 + ((*(esi + idx)) * 4));
            w1 = colors[color_idx & 0x000000FF];
            w2 = colors[(color_idx & 0x0000FF00) >> 8];
            w3 = colors[(color_idx & 0x00FF0000) >> 16];
            w4 = colors[(color_idx & 0xFF000000) >> 24];

            if (rep_count % 2) {
              *(unsigned int *)(edi + 8) = (w1 & 0xFFFF) | ((w2 & 0xFFFF) << 16);
              *(unsigned int *)(edi + nf_width + 8) = (w1 & 0xFFFF) | ((w2 & 0xFFFF) << 16);

              *(unsigned int *)(edi + 12) = (w3 & 0xFFFF) | ((w4 & 0xFFFF) << 16);
              *(unsigned int *)(edi + nf_width + 12) = (w3 & 0xFFFF) | ((w4 & 0xFFFF) << 16);

              if (rep_count != (max_repcount - 1))
                edi = edi + nf_width * 2;
            } else {
              *(unsigned int *)(edi + 0) = (w1 & 0xFFFF) | ((w2 & 0xFFFF) << 16);
              *(unsigned int *)(edi + nf_width) = (w1 & 0xFFFF) | ((w2 & 0xFFFF) << 16);

              *(unsigned int *)(edi + 4) = (w3 & 0xFFFF) | ((w4 & 0xFFFF) << 16);
              *(unsigned int *)(edi + nf_width + 4) = (w3 & 0xFFFF) | ((w4 & 0xFFFF) << 16);
            }
          }

          edi += nf_width;
          esi += 16;
          edi -= nfpk_back_right;
        }

        if (donf41) {
          unsigned int colors[4];
          max_repcount = 8;

          colors[0] = nf_trans16_lo[*(esi)] | nf_trans16_hi[*(esi + 1)];
          colors[0] = ((colors[0]) << 16) | colors[0];
          colors[1] = nf_trans16_lo[*(esi + 2)] | nf_trans16_hi[*(esi + 3)];
          colors[1] = ((colors[1]) << 16) | colors[1];
          colors[2] = nf_trans16_lo[*(esi + 4)] | nf_trans16_hi[*(esi + 5)];
          colors[2] = ((colors[2]) << 16) | colors[2];
          colors[3] = nf_trans16_lo[*(esi + 6)] | nf_trans16_hi[*(esi + 7)];
          colors[3] = ((colors[3]) << 16) | colors[3];

          for (rep_count = 0; rep_count < max_repcount; rep_count++) {
            unsigned int w1, w2, w3, w4;
            unsigned int color_idx;
            int idx;

            idx = rep_count + 8;
            color_idx = *(unsigned int *)(nfhpk_mov8 + (*(esi + idx) * 4));
            w1 = colors[color_idx & 0x000000FF];
            w2 = colors[(color_idx & 0x0000FF00) >> 8];
            w3 = colors[(color_idx & 0x00FF0000) >> 16];
            w4 = colors[(color_idx & 0xFF000000) >> 24];

            *(unsigned int *)(edi + 0) = w1;
            *(unsigned int *)(edi + 4) = w2;
            *(unsigned int *)(edi + 8) = w3;
            *(unsigned int *)(edi + 12) = w4;

            if (rep_count != (max_repcount - 1))
              edi += nf_width;
          }

          esi += 16;
          edi -= nfpk_back_right;
        }

        if (donf25) {
          unsigned int colors[4];
          max_repcount = 4;

          colors[0] = nf_trans16_lo[*(esi)] | nf_trans16_hi[*(esi + 1)];
          colors[0] = ((colors[0]) << 16) | colors[0];
          colors[1] = nf_trans16_lo[*(esi + 2)] | nf_trans16_hi[*(esi + 3)];
          colors[1] = ((colors[1]) << 16) | colors[1];
          colors[2] = nf_trans16_lo[*(esi + 4)] | nf_trans16_hi[*(esi + 5)];
          colors[2] = ((colors[2]) << 16) | colors[2];
          colors[3] = nf_trans16_lo[*(esi + 6)] | nf_trans16_hi[*(esi + 7)];
          colors[3] = ((colors[3]) << 16) | colors[3];

          for (rep_count = 0; rep_count < max_repcount; rep_count++) {
            unsigned int w1, w2, w3, w4;
            unsigned int color_idx;
            int idx;

            idx = rep_count + 8;

            color_idx = *(unsigned int *)(nfhpk_mov4 + ((*(esi + idx)) * 4));
            w1 = colors[color_idx & 0x000000FF];
            w2 = colors[(color_idx & 0x0000FF00) >> 8];
            w3 = colors[(color_idx & 0x00FF0000) >> 16];
            w4 = colors[(color_idx & 0xFF000000) >> 24];

            *(unsigned int *)(edi + 0) = w1;
            *(unsigned int *)(edi + nf_width) = w1;
            *(unsigned int *)(edi + 4) = w2;
            *(unsigned int *)(edi + nf_width + 4) = w2;
            *(unsigned int *)(edi + 8) = w3;
            *(unsigned int *)(edi + nf_width + 8) = w3;
            *(unsigned int *)(edi + 12) = w4;
            *(unsigned int *)(edi + nf_width + 12) = w4;

            if (rep_count != (max_repcount - 1))
              edi = edi + nf_width * 2;
          }

          edi += nf_width;
          esi += 12;
          edi -= nfpk_back_right;
        }

        if (!donf25 && !donf41 && !donf57) {
          unsigned short colors[4];
          max_repcount = 16;

          colors[0] = nf_trans16_lo[*(esi)] | nf_trans16_hi[*(esi + 1)];
          colors[1] = nf_trans16_lo[*(esi + 2)] | nf_trans16_hi[*(esi + 3)];
          colors[2] = nf_trans16_lo[*(esi + 4)] | nf_trans16_hi[*(esi + 5)];
          colors[3] = nf_trans16_lo[*(esi + 6)] | nf_trans16_hi[*(esi + 7)];

          for (rep_count = 0; rep_count < max_repcount; rep_count++) {
            unsigned int w1, w2, w3, w4;
            unsigned int color_idx;
            int idx;

            idx = rep_count + 8;

            color_idx = *(unsigned int *)(nfhpk_mov4 + ((*(esi + idx)) * 4));
            w1 = colors[color_idx & 0x000000FF];
            w2 = colors[(color_idx & 0x0000FF00) >> 8];
            w3 = colors[(color_idx & 0x00FF0000) >> 16];
            w4 = colors[(color_idx & 0xFF000000) >> 24];

            if (rep_count % 2) {
              *(unsigned int *)(edi + 8) = (w1 & 0xFFFF) | ((w2 & 0xFFFF) << 16);
              *(unsigned int *)(edi + 12) = (w3 & 0xFFFF) | ((w4 & 0xFFFF) << 16);

              if (rep_count != (max_repcount - 1))
                edi += nf_width;
            } else {
              *(unsigned int *)(edi + 0) = (w1 & 0xFFFF) | ((w2 & 0xFFFF) << 16);
              *(unsigned int *)(edi + 4) = (w3 & 0xFFFF) | ((w4 & 0xFFFF) << 16);
            }
          }

          esi += 24;
          edi -= nfpk_back_right;
        }
      } break;
      case 10: {
        // 2x2 4x4x2 (32 bytes) or 2x1 4x8x2 (24 bytes) or 1x2 4x8x2 (24 bytes)
        int val1;
        int rep_count;
        unsigned short colors[4];
        bool do26 = false;
        bool do42 = false;

        unsigned short swapper = *(unsigned short *)esi;
        val1 = INTEL_SHORT(swapper);
        if (val1 & 0x8000) {
          swapper = *(unsigned short *)(esi + 16);
          val1 = INTEL_SHORT(swapper);
          if (val1 & 0x8000) {
            do42 = true;
          } else {
            do26 = true;
          }
        }

        // Load bx,dx,cx,bp with four colors
        colors[0] = nf_trans16_lo[*(esi)] | nf_trans16_hi[*(esi + 1)];
        colors[1] = nf_trans16_lo[*(esi + 2)] | nf_trans16_hi[*(esi + 3)];
        colors[2] = nf_trans16_lo[*(esi + 4)] | nf_trans16_hi[*(esi + 5)];
        colors[3] = nf_trans16_lo[*(esi + 6)] | nf_trans16_hi[*(esi + 7)];

        for (rep_count = 0; rep_count < 16; rep_count++) {
          unsigned int w1, w2, w3, w4;
          unsigned int color_idx;
          int idx;

          if (!do26 && !do42) {
            switch (rep_count) {
            case 0:
              idx = 8;
              break;
            case 1:
              idx = 9;
              break;
            case 2:
              idx = 10;
              break;
            case 3:
              idx = 11;
              break;
            case 4:
              idx = 20;
              break;
            case 5:
              idx = 21;
              break;
            case 6:
              idx = 22;
              break;
            case 7:
              idx = 23;
              break;
            case 8:
              idx = 32;
              break;
            case 9:
              idx = 33;
              break;
            case 10:
              idx = 34;
              break;
            case 11:
              idx = 35;
              break;
            case 12:
              idx = 44;
              break;
            case 13:
              idx = 45;
              break;
            case 14:
              idx = 46;
              break;
            case 15:
              idx = 47;
              break;
            }
          } else {
            switch (rep_count) {
            case 0:
              idx = 8;
              break;
            case 1:
              idx = 9;
              break;
            case 2:
              idx = 10;
              break;
            case 3:
              idx = 11;
              break;
            case 4:
              idx = 12;
              break;
            case 5:
              idx = 13;
              break;
            case 6:
              idx = 14;
              break;
            case 7:
              idx = 15;
              break;
            case 8:
              idx = 24;
              break;
            case 9:
              idx = 25;
              break;
            case 10:
              idx = 26;
              break;
            case 11:
              idx = 27;
              break;
            case 12:
              idx = 28;
              break;
            case 13:
              idx = 29;
              break;
            case 14:
              idx = 30;
              break;
            case 15:
              idx = 31;
              break;
            }
          }
          color_idx = *(unsigned int *)(nfhpk_mov4 + (*(esi + idx) * 4));
          w1 = colors[color_idx & 0x000000FF];
          w2 = colors[(color_idx & 0x0000FF00) >> 8];
          w3 = colors[(color_idx & 0x00FF0000) >> 16];
          w4 = colors[(color_idx & 0xFF000000) >> 24];

          if (!do42) {
            *(unsigned int *)(edi + 0) = (w1 & 0xFFFF) | ((w2 & 0xFFFF) << 16);
            *(unsigned int *)(edi + 4) = (w3 & 0xFFFF) | ((w4 & 0xFFFF) << 16);

            if (rep_count != 15)
              edi += nf_width;
          } else {
            if (rep_count % 2) {
              *(unsigned int *)(edi + 8) = (w1 & 0xFFFF) | ((w2 & 0xFFFF) << 16);
              *(unsigned int *)(edi + 12) = (w3 & 0xFFFF) | ((w4 & 0xFFFF) << 16);

              if (rep_count != 15)
                edi += nf_width;
            } else {
              *(unsigned int *)(edi + 0) = (w1 & 0xFFFF) | ((w2 & 0xFFFF) << 16);
              *(unsigned int *)(edi + 4) = (w3 & 0xFFFF) | ((w4 & 0xFFFF) << 16);
            }
          }

          if (rep_count == 3 && !do42 && !do26) {
            // Load bx,dx,cx,bp with four colors
            colors[0] = nf_trans16_lo[*(esi + 12)] | nf_trans16_hi[*(esi + 13)];
            colors[1] = nf_trans16_lo[*(esi + 14)] | nf_trans16_hi[*(esi + 15)];
            colors[2] = nf_trans16_lo[*(esi + 16)] | nf_trans16_hi[*(esi + 17)];
            colors[3] = nf_trans16_lo[*(esi + 18)] | nf_trans16_hi[*(esi + 19)];
          }

          if (rep_count == 7) {
            if (!do42 && !do26) {
              edi -= nf_width * 8 - 8;

              // Load bx,dx,cx,bp with four colors
              colors[0] = nf_trans16_lo[*(esi + 24)] | nf_trans16_hi[*(esi + 25)];
              colors[1] = nf_trans16_lo[*(esi + 26)] | nf_trans16_hi[*(esi + 27)];
              colors[2] = nf_trans16_lo[*(esi + 28)] | nf_trans16_hi[*(esi + 29)];
              colors[3] = nf_trans16_lo[*(esi + 30)] | nf_trans16_hi[*(esi + 31)];
            }

            if (do26) {
              edi -= nf_width * 8 - 8;
              // Load bx,dx,cx,bp with four colors
              colors[0] = nf_trans16_lo[*(esi + 16)] | nf_trans16_hi[*(esi + 17)];
              colors[1] = nf_trans16_lo[*(esi + 18)] | nf_trans16_hi[*(esi + 19)];
              colors[2] = nf_trans16_lo[*(esi + 20)] | nf_trans16_hi[*(esi + 21)];
              colors[3] = nf_trans16_lo[*(esi + 22)] | nf_trans16_hi[*(esi + 23)];
            }

            if (do42) {
              // Load bx,dx,cx,bp with four colors
              colors[0] = nf_trans16_lo[*(esi + 16)] | nf_trans16_hi[*(esi + 17)];
              colors[1] = nf_trans16_lo[*(esi + 18)] | nf_trans16_hi[*(esi + 19)];
              colors[2] = nf_trans16_lo[*(esi + 20)] | nf_trans16_hi[*(esi + 21)];
              colors[3] = nf_trans16_lo[*(esi + 22)] | nf_trans16_hi[*(esi + 23)];
            }
          }

          if (rep_count == 11 && !do42 && !do26) {
            // Load bx,dx,cx,bp with four colors
            colors[0] = nf_trans16_lo[*(esi + 36)] | nf_trans16_hi[*(esi + 37)];
            colors[1] = nf_trans16_lo[*(esi + 38)] | nf_trans16_hi[*(esi + 39)];
            colors[2] = nf_trans16_lo[*(esi + 40)] | nf_trans16_hi[*(esi + 41)];
            colors[3] = nf_trans16_lo[*(esi + 42)] | nf_trans16_hi[*(esi + 43)];
          }
        }

        if (!do42 && !do26) {
          esi += 48;
        } else {
          esi += 32;
        }

        if (!do42) {
          edi -= (8 + nfpk_back_right);
        } else {
          edi -= nfpk_back_right;
        }

      } break;

      case 11: {
        // 8x8x16 (128 bytes)
        Trans16Blk(edi, esi);
        edi += nf_width;

        Trans16Blk(edi, esi + 16);
        edi += nf_width;

        Trans16Blk(edi, esi + 32);
        edi += nf_width;

        Trans16Blk(edi, esi + 48);
        edi += nf_width;

        Trans16Blk(edi, esi + 64);
        edi += nf_width;

        Trans16Blk(edi, esi + 80);
        edi += nf_width;

        Trans16Blk(edi, esi + 96);
        edi += nf_width;

        Trans16Blk(edi, esi + 112);

        esi += 128;
        edi -= nfpk_back_right;
      } break;
      case 12: {
        // low 4x4x16 (32 bytes)
        int i;
        unsigned int eax, ebx;

        for (i = 0; i < 4; i++) {
          eax = *(unsigned char *)(esi + i * 8 + 0);
          ebx = nf_trans16_lo[eax];
          eax = *(unsigned char *)(esi + i * 8 + 1);
          ebx |= nf_trans16_hi[eax];
          eax = ((ebx & 0xFFFF) << 16) | (ebx & 0xFFFF);
          *(unsigned int *)(edi + 0) = eax;
          *(unsigned int *)(edi + nf_width) = eax;

          eax = *(unsigned char *)(esi + i * 8 + 2);
          ebx = nf_trans16_lo[eax];
          eax = *(unsigned char *)(esi + i * 8 + 3);
          ebx |= nf_trans16_hi[eax];
          eax = ((ebx & 0xFFFF) << 16) | (ebx & 0xFFFF);
          *(unsigned int *)(edi + 4) = eax;
          *(unsigned int *)(edi + nf_width + 4) = eax;

          eax = *(unsigned char *)(esi + i * 8 + 4);
          ebx = nf_trans16_lo[eax];
          eax = *(unsigned char *)(esi + i * 8 + 5);
          ebx |= nf_trans16_hi[eax];
          eax = ((ebx & 0xFFFF) << 16) | (ebx & 0xFFFF);
          *(unsigned int *)(edi + 8) = eax;
          *(unsigned int *)(edi + nf_width + 8) = eax;

          eax = *(unsigned char *)(esi + i * 8 + 6);
          ebx = nf_trans16_lo[eax];
          eax = *(unsigned char *)(esi + i * 8 + 7);
          ebx |= nf_trans16_hi[eax];
          eax = ((ebx & 0xFFFF) << 16) | (ebx & 0xFFFF);
          *(unsigned int *)(edi + 12) = eax;
          *(unsigned int *)(edi + nf_width + 12) = eax;

          if (i != 3)
            edi = edi + nf_width * 2;
        }

        edi += nf_width;
        edi -= nfpk_back_right;
        esi += 32;
      } break;
      case 13: {
        // 2x2 4x4x0 (8 bytes)
        unsigned int temp, ebx, ecx;

        temp = nf_trans16_lo[(*esi)] | nf_trans16_hi[(*(esi + 1))];
        ebx = ((temp & 0xFFFF) << 16) | (temp & 0xFFFF);

        temp = nf_trans16_lo[(*(esi + 2))] | nf_trans16_hi[(*(esi + 3))];
        ecx = ((temp & 0xFFFF) << 16) | (temp & 0xFFFF);

        *(unsigned int *)(edi + 0) = ebx;
        *(unsigned int *)(edi + 4) = ebx;
        *(unsigned int *)(edi + 8) = ecx;
        *(unsigned int *)(edi + 12) = ecx;
        *(unsigned int *)(edi + nf_width) = ebx;
        *(unsigned int *)(edi + nf_width + 4) = ebx;
        *(unsigned int *)(edi + nf_width + 8) = ecx;
        *(unsigned int *)(edi + nf_width + 12) = ecx;

        edi = edi + nf_width * 2;

        *(unsigned int *)(edi + 0) = ebx;
        *(unsigned int *)(edi + 4) = ebx;
        *(unsigned int *)(edi + 8) = ecx;
        *(unsigned int *)(edi + 12) = ecx;
        *(unsigned int *)(edi + nf_width) = ebx;
        *(unsigned int *)(edi + nf_width + 4) = ebx;
        *(unsigned int *)(edi + nf_width + 8) = ecx;
        *(unsigned int *)(edi + nf_width + 12) = ecx;

        edi = edi + nf_width * 2;

        temp = nf_trans16_lo[(*(esi + 4))] | nf_trans16_hi[(*(esi + 5))];
        ebx = ((temp & 0xFFFF) << 16) | (temp & 0xFFFF);

        temp = nf_trans16_lo[(*(esi + 6))] | nf_trans16_hi[(*(esi + 7))];
        ecx = ((temp & 0xFFFF) << 16) | (temp & 0xFFFF);

        *(unsigned int *)(edi + 0) = ebx;
        *(unsigned int *)(edi + 4) = ebx;
        *(unsigned int *)(edi + 8) = ecx;
        *(unsigned int *)(edi + 12) = ecx;
        *(unsigned int *)(edi + nf_width) = ebx;
        *(unsigned int *)(edi + nf_width + 4) = ebx;
        *(unsigned int *)(edi + nf_width + 8) = ecx;
        *(unsigned int *)(edi + nf_width + 12) = ecx;

        edi = edi + nf_width * 2;

        *(unsigned int *)(edi + 0) = ebx;
        *(unsigned int *)(edi + 4) = ebx;
        *(unsigned int *)(edi + 8) = ecx;
        *(unsigned int *)(edi + 12) = ecx;
        *(unsigned int *)(edi + nf_width) = ebx;
        *(unsigned int *)(edi + nf_width + 4) = ebx;
        *(unsigned int *)(edi + nf_width + 8) = ecx;
        *(unsigned int *)(edi + nf_width + 12) = ecx;

        edi += nf_width;
        edi -= nfpk_back_right;
        esi += 8;
      } break;
      case 14: {
        //   8x8x0 (2 bytes)
        unsigned int ecx, ebx;
        ecx = nf_trans16_lo[*(esi)] | nf_trans16_hi[*(esi + 1)];
        esi += 2;
        ebx = ((ecx & 0xFFFF) << 16) | (ecx & 0xFFFF);

        *(unsigned int *)(edi + 0) = ebx;
        *(unsigned int *)(edi + 4) = ebx;
        *(unsigned int *)(edi + 8) = ebx;
        *(unsigned int *)(edi + 12) = ebx;
        edi += nf_width;

        *(unsigned int *)(edi + 0) = ebx;
        *(unsigned int *)(edi + 4) = ebx;
        *(unsigned int *)(edi + 8) = ebx;
        *(unsigned int *)(edi + 12) = ebx;
        edi += nf_width;

        *(unsigned int *)(edi + 0) = ebx;
        *(unsigned int *)(edi + 4) = ebx;
        *(unsigned int *)(edi + 8) = ebx;
        *(unsigned int *)(edi + 12) = ebx;
        edi += nf_width;

        *(unsigned int *)(edi + 0) = ebx;
        *(unsigned int *)(edi + 4) = ebx;
        *(unsigned int *)(edi + 8) = ebx;
        *(unsigned int *)(edi + 12) = ebx;
        edi += nf_width;

        *(unsigned int *)(edi + 0) = ebx;
        *(unsigned int *)(edi + 4) = ebx;
        *(unsigned int *)(edi + 8) = ebx;
        *(unsigned int *)(edi + 12) = ebx;
        edi += nf_width;

        *(unsigned int *)(edi + 0) = ebx;
        *(unsigned int *)(edi + 4) = ebx;
        *(unsigned int *)(edi + 8) = ebx;
        *(unsigned int *)(edi + 12) = ebx;
        edi += nf_width;

        *(unsigned int *)(edi + 0) = ebx;
        *(unsigned int *)(edi + 4) = ebx;
        *(unsigned int *)(edi + 8) = ebx;
        *(unsigned int *)(edi + 12) = ebx;
        edi += nf_width;

        *(unsigned int *)(edi + 0) = ebx;
        *(unsigned int *)(edi + 4) = ebx;
        *(unsigned int *)(edi + 8) = ebx;
        *(unsigned int *)(edi + 12) = ebx;

        edi -= nfpk_back_right;
      } break;
      case 15: {
        int i;
        i = 0;
      } break;
      }

      if (first_opcode) {
        first_opcode = false;
        opcode_to_use = (opcode >> 4);
        goto do_next_opcode;
      }

      // go back up
      continue;
    } else {
      edi += nf.new_row;
      h--;
      wcnt = w >> 1;
    }
  } while (h != 0);
}

void Trans16Blk(unsigned char *edi, unsigned char *idx) {
  *((unsigned short *)(edi + 0)) = nf_trans16_lo[*(idx + 0)] | nf_trans16_hi[*(idx + 1)];
  *((unsigned short *)(edi + 2)) = nf_trans16_lo[*(idx + 2)] | nf_trans16_hi[*(idx + 3)];
  *((unsigned short *)(edi + 4)) = nf_trans16_lo[*(idx + 4)] | nf_trans16_hi[*(idx + 5)];
  *((unsigned short *)(edi + 6)) = nf_trans16_lo[*(idx + 6)] | nf_trans16_hi[*(idx + 7)];
  *((unsigned short *)(edi + 8)) = nf_trans16_lo[*(idx + 8)] | nf_trans16_hi[*(idx + 9)];
  *((unsigned short *)(edi + 10)) = nf_trans16_lo[*(idx + 10)] | nf_trans16_hi[*(idx + 11)];
  *((unsigned short *)(edi + 12)) = nf_trans16_lo[*(idx + 12)] | nf_trans16_hi[*(idx + 13)];
  *((unsigned short *)(edi + 14)) = nf_trans16_lo[*(idx + 14)] | nf_trans16_hi[*(idx + 15)];
}

void call_hnfxycshift(unsigned int eax, unsigned char **medi, unsigned char **mesi, int nfpk_back_right) {
  unsigned int ebx;
  ebx = ((eax & 0xFF00) >> 8);

  if (eax & 0x80) {
    // we have to sign extend also
    eax = ((eax & 0x7F) << 1);
    eax = (eax & 0xFF) | 0xFFFFFF00;
  } else {
    eax = ((eax & 0x7F) << 1);
  }
  eax += nfpk_ShiftY[ebx];

  call_hnfshift(eax, medi, mesi, nfpk_back_right);
}

void call_hnfxypshift(unsigned int eax, unsigned char **medi, unsigned char **mesi, int nfpk_back_right,
                      int DiffBufPtrs) {
  unsigned int ebx;
  ebx = ((eax & 0xFF00) >> 8);

  if (eax & 0x80) {
    // we have to sign extend also
    eax = ((eax & 0x7F) << 1);
    eax = (eax & 0xFF) | 0xFFFFFF00;
  } else {
    eax = ((eax & 0x7F) << 1);
  }
  eax += nfpk_ShiftY[ebx];
  eax += DiffBufPtrs;

  call_hnfshift(eax, medi, mesi, nfpk_back_right);
}

void call_hnfshift(unsigned int meax, unsigned char **medi, unsigned char **mesi, int nfpk_back_right) {
  unsigned char *esi, *edi, *saved_esi;
  int i;

  edi = *medi;
  saved_esi = esi = *mesi;
  esi = edi + meax;

  for (i = 0; i < 8; i++) {
#define HNFSHIFT_WRITEOUT(x) *(unsigned int *)(edi + x) = *(unsigned int *)(esi + x);

    HNFSHIFT_WRITEOUT(0);
    HNFSHIFT_WRITEOUT(4);
    HNFSHIFT_WRITEOUT(8);
    HNFSHIFT_WRITEOUT(12);

#undef HNFSHIFT_WRITEOUT

    if (i != 7) {
      esi += nf_width;
      edi += nf_width;
    }
  }

  edi -= nfpk_back_right;
  esi = saved_esi;

  *medi = edi;
  *mesi = esi;
}

////////////////////////////////////////////////
// Non-Implemented Functions
////////////////////////////////////////////////
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
#if (defined __i386__) || defined(__x86_64__)
#error
#define int3() __asm__ __volatile__("int $3")
#else
#define int3() raise(SIGTRAP)
#endif

void nfHiColorDecomp(unsigned char *comp, unsigned x, unsigned y, unsigned w, unsigned h) { int3(); }
void nfHiColorDecompChg(unsigned short *chgs, unsigned short *parms, unsigned char *comp, unsigned x, unsigned y,
                        unsigned w, unsigned h) {
  int3();
}
void nfDecomp(unsigned char *comp, unsigned x, unsigned y, unsigned w, unsigned h) { int3(); }
void nfDecompChg(unsigned short *chgs, unsigned short *parms, unsigned char *comp, unsigned x, unsigned y, unsigned w,
                 unsigned h) {
  int3();
}
void nfPkPal(void) { int3(); }
void nfPkDecomp(unsigned char *ops, unsigned char *comp, unsigned x, unsigned y, unsigned w, unsigned h) { int3(); }
void nfPkDecompH(unsigned char *ops, unsigned char *comp, unsigned x, unsigned y, unsigned w, unsigned h) { int3(); }
void nfPkDecompD(unsigned char *ops, unsigned char *comp, unsigned x, unsigned y, unsigned w, unsigned h) { int3(); }
void mve_ShowFrameField(unsigned char *buf, unsigned bufw, unsigned bufh, unsigned sx, unsigned sy, unsigned w,
                        unsigned h, unsigned dstx, unsigned dsty, unsigned field) {
  int3();
}
void mve_ShowFrameFieldHi(unsigned char *buf, unsigned bufw, unsigned bufh, unsigned sx, unsigned sy, unsigned w,
                          unsigned h, unsigned dstx, unsigned dsty, unsigned field) {
  int3();
}
void mve_sfShowFrameChg(bool prvbuf, unsigned x, unsigned y, unsigned w, unsigned h, unsigned short *chgs,
                        unsigned dstx, unsigned dsty) {
  int3();
}
void mve_sfHiColorShowFrameChg(bool prvbuf, unsigned x, unsigned y, unsigned w, unsigned h, unsigned short *chgs,
                               unsigned dstx, unsigned dsty) {
  int3();
}
void mve_sfPkShowFrameChg(bool prvbuf, unsigned x, unsigned y, unsigned w, unsigned h, unsigned char *ops,
                          unsigned dstx, unsigned dsty) {
  int3();
}
void mve_sfPkHiColorShowFrameChg(bool prvbuf, unsigned x, unsigned y, unsigned w, unsigned h, unsigned char *ops,
                                 unsigned dstx, unsigned dsty) {
  int3();
}
void MVE_SetPalette(unsigned char *p, unsigned start, unsigned count) { int3(); }
void palLoadCompPalette(unsigned char *buf) { int3(); }
void gfxMode(unsigned mode) { int3(); }
void gfxLoadCrtc(unsigned char *crtc, unsigned char chain4, unsigned char res) { int3(); }
void gfxGetCrtc(unsigned char *crtc) { int3(); }
void gfxVres(unsigned char misc, unsigned char *crtc) { int3(); }
void MVE_gfxWaitRetrace(int state) { int3(); }
void MVE_gfxSetSplit(unsigned line) { int3(); }
