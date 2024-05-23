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

#include "debug.h"
#include "pstypes.h"
#include "mvelibl.h"
#include "mvelibi.h"
#include <string.h>

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
extern unsigned nf_hicolor;

extern unsigned short nf_trans16_lo[256];
extern unsigned short nf_trans16_hi[256];

extern signed short snd_8to16[256];
void PkDecompWorker(const bool hiColor, const unsigned char *ops, const unsigned char *comp, const unsigned x,
                    const unsigned y, const unsigned w, const unsigned h);
void nfHPkDecomp(unsigned char *ops, unsigned char *comp, int x, int y, int w, int h);
void nfPkDecomp(unsigned char *ops, unsigned char *comp, unsigned x, unsigned y, unsigned w, unsigned h);
void nfPkConfig(void);
unsigned sndDecompM16(unsigned short *dst, const unsigned char *src, unsigned len, unsigned prev);
unsigned sndDecompS16(unsigned short *dst, const unsigned char *src, unsigned len, unsigned prev);

void Trans16Blk(unsigned char *edi, const unsigned char *idx);
void DOnf_xycshift(const bool hiColor, const unsigned int eax, unsigned char *&edi, const int nfpk_back_right);
void DOnf_xypshift(const bool hiColor, const unsigned int eax, unsigned char *&edi, const int nfpk_back_right,
                   const int DiffBufPtrs);
void DOnf_shift(const bool hiColor, int eax, unsigned char *&edi, const int nfpk_back_right);

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
  unsigned char *tbuf;
  int new_row, DiffBufPtrs;
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

//		mov ax, bx,cx
// EBX = 0
// ECX = 1
unsigned char nfpk_mov4l[64];

//	mov ds:[edi+0/4/8/12], ebx/edx/ecx/ebp
// EBX = 0
// EDX = 1
// ECX = 2
// EBP = 3
unsigned char nfhpk_mov8[1024];

//  mov ax,	bx/dx/cx/bp
// BX = 0
// DX = 1
// CX = 2
// BP = 3
unsigned char nfpk_mov8[1024];

//	mov eax, ebx/edx/ecx/ebp
// EBX = 0
// EDX = 1
// ECX = 2
// EBP = 3
unsigned char nfhpk_mov4[1024];

//  mov al, bl/bh/cl/ch
// BL = 0
// BH = 1
// CL = 2
// CH = 3
unsigned char nfpk_mov4[1024];

class initme {
public:
  initme() {
    int x, y;
    int m4, m3, m2, m1;
    signed char *ptr;
    unsigned char *uptr;

    // Do nfhpk_mov4l
    uptr = nfhpk_mov4l;
    for (m4 = 0; m4 < 2; ++m4)
      for (m3 = 0; m3 < 2; ++m3)
        for (m2 = 0; m2 < 2; ++m2)
          for (m1 = 0; m1 < 2; ++m1) {
            *uptr++ = m1;
            *uptr++ = m2;
            *uptr++ = m3;
            *uptr++ = m4;
          }

    // Do nfpk_mov4l
    uptr = nfpk_mov4l;
    for (m4 = 0; m4 < 2; ++m4)
      for (m3 = 0; m3 < 2; ++m3)
        for (m2 = 0; m2 < 2; ++m2)
          for (m1 = 0; m1 < 2; ++m1) {
            *uptr++ = m1;
            *uptr++ = m2;
            *uptr++ = m3;
            *uptr++ = m4;
          }

    // Do nfhpk_mov8
    uptr = nfhpk_mov8;
    for (m4 = 0; m4 < 4; ++m4)
      for (m3 = 0; m3 < 4; ++m3)
        for (m2 = 0; m2 < 4; ++m2)
          for (m1 = 0; m1 < 4; ++m1) {
            *uptr++ = m1;
            *uptr++ = m2;
            *uptr++ = m3;
            *uptr++ = m4;
          }

    // Do nfpk_mov8
    uptr = nfpk_mov8;
    for (m4 = 0; m4 < 4; ++m4)
      for (m3 = 0; m3 < 4; ++m3)
        for (m2 = 0; m2 < 4; ++m2)
          for (m1 = 0; m1 < 4; ++m1) {
            *uptr++ = m1;
            *uptr++ = m2;
            *uptr++ = m3;
            *uptr++ = m4;
          }

    // Do nfhpk_mov4
    uptr = nfhpk_mov4;
    for (m4 = 0; m4 < 4; ++m4)
      for (m3 = 0; m3 < 4; ++m3)
        for (m2 = 0; m2 < 4; ++m2)
          for (m1 = 0; m1 < 4; ++m1) {
            *uptr++ = m1;
            *uptr++ = m2;
            *uptr++ = m3;
            *uptr++ = m4;
          }

    // Do nfpk_mov4
    uptr = nfpk_mov4;
    for (m4 = 0; m4 < 4; ++m4)
      for (m3 = 0; m3 < 4; ++m3)
        for (m2 = 0; m2 < 4; ++m2)
          for (m1 = 0; m1 < 4; ++m1) {
            *uptr++ = m1;
            *uptr++ = m2;
            *uptr++ = m3;
            *uptr++ = m4;
          }

    // do nfpk_ShiftP1
    ptr = (signed char *)nfpk_ShiftP1;
    for (y = -8; y != 8; y++) {
      for (x = -8; x != 8; x++) {
#ifdef OUTRAGE_BIG_ENDIAN
        ptr[0] = y;
        ptr[1] = x;
#else
        ptr[0] = x;
        ptr[1] = y;
#endif
        ptr += 2;
      }
    }

    // do nfpk_ShiftP2[]
    ptr = (signed char *)nfpk_ShiftP2;
    for (y = 0; y != 8; y++) {
      for (x = 8; x != 15; x++) {
#ifdef OUTRAGE_BIG_ENDIAN
        ptr[0] = y;
        ptr[1] = x;
#else
        ptr[0] = x;
        ptr[1] = y;
#endif
        ptr += 2;
      }
    }

    for (y = 8; y != 14; y++) {
      for (x = -14; x != 0; x++) {
#ifdef OUTRAGE_BIG_ENDIAN
        ptr[0] = y;
        ptr[1] = x;
#else
        ptr[0] = x;
        ptr[1] = y;
#endif
        ptr += 2;
      }

      for (x = 0; x != 15; x++) {
#ifdef OUTRAGE_BIG_ENDIAN
        ptr[0] = y;
        ptr[1] = x;
#else
        ptr[0] = x;
        ptr[1] = y;
#endif
        ptr += 2;
      }
    }
    for (x = -14; x != 0; x++) {
#ifdef OUTRAGE_BIG_ENDIAN
      ptr[0] = 14;
      ptr[1] = x;
#else
      ptr[0] = x;
      ptr[1] = 14;
#endif
      ptr += 2;
    }
    for (x = 0; x != 12; x++) {
#ifdef OUTRAGE_BIG_ENDIAN
      ptr[0] = 14;
      ptr[1] = x;
#else
      ptr[0] = x;
      ptr[1] = 14;
#endif
      ptr += 2;
    }
  }
};

initme _initme;

//	nfPkConfig initializes tables used by nfPkDecomp
//	which are dependent on screen size.
void nfPkConfig(void) {
  // Build ShiftY table
  int index = 0;
  int val;

  val = 0;
  for (int i = 0; i < 128; ++i) {
    nfpk_ShiftY[index++] = val;
    val += nf_width;
  }

  val = -int(nf_width << 7);
  for (int i = 0; i < 128; ++i) {
    nfpk_ShiftY[index++] = val;
    val += nf_width;
  }
}

unsigned short Trans16(const unsigned char *idx) { return nf_trans16_lo[idx[0]] | nf_trans16_hi[idx[1]]; }

int SkipOpcode(int opcode_to_use, bool hiColor, const unsigned char *&esi, const unsigned char *&bcomp,
               unsigned char *&edi) {
  switch (opcode_to_use) {
  case 0:
  case 1:
  case 2:
    // no esi change
    break;
  case 3:
  case 4:
    if (hiColor) {
      ++bcomp;
    } else {
      ++esi;
    }
    break;
  case 5:
    esi += 2;
    break;
  case 6:
    if (hiColor) {
      esi += 2;
    } else {
      // we can't skip this -- though I've never came across this
      return opcode_to_use;
    }
    break;
  case 7: {
    bool donf23 = false;
    if (hiColor) {
      unsigned short val = IntelSwapper(*(unsigned short *)esi);
      if (val & 0x8000) {
        donf23 = true;
      }
    } else {
      if (esi[0] > esi[1]) {
        donf23 = true;
      }
    }

    if (donf23) {
      esi += (hiColor) ? 6 : 4;
    } else {
      esi += (hiColor) ? 12 : 10;
    }
  } break;

  case 8: {
    bool mode2 = false;
    if (hiColor) {
      unsigned short val = IntelSwapper(*(unsigned short *)esi);
      if (val & 0x8000) {
        mode2 = true;
      }
    } else {
      if (esi[0] > esi[1]) {
        mode2 = true;
      }
    }

    if (!mode2) {
      esi += (hiColor) ? 24 : 16;
    } else {
      esi += (hiColor) ? 16 : 12;
    }
  } break;

  case 9: {
    bool mode2 = false;
    bool mode3 = false;

    if (hiColor) {
      unsigned short val = IntelSwapper(*(unsigned short *)esi);
      if (val & 0x8000) {
        mode2 = true;
      } else {
        val = IntelSwapper(*(unsigned short *)(esi + 4));
        if (val & 0x8000) {
          mode3 = true;
        }
      }
    } else {
      if (esi[0] > esi[1]) {
        mode2 = true;
      } else if (esi[2] > esi[3]) {
        mode3 = true;
      }
    }

    if (mode2)
      esi += (hiColor) ? 16 : 12;
    else if (mode3)
      esi += (hiColor) ? 12 : 8;
    else
      esi += (hiColor) ? 24 : 20;
  } break;
  case 10: {
    // 2x2 4x4x2 (32 bytes) or 2x1 4x8x2 (24 bytes) or 1x2 4x8x2 (24 bytes)
    bool mode2 = false;

    if (hiColor) {
      int val1 = IntelSwapper(*(unsigned short *)esi);
      if (val1 & 0x8000) {
        mode2 = true;
      }
    } else {
      if (esi[0] > esi[1]) {
        mode2 = true;
      }
    }

    if (!mode2) {
      esi += (hiColor) ? 48 : 32;
    } else {
      esi += (hiColor) ? 32 : 24;
    }
  } break;

  case 11: {
    esi += (hiColor) ? 128 : 64;
  } break;

  case 12: {
    esi += (hiColor) ? 32 : 16;
  } break;

  case 13: {
    esi += (hiColor) ? 8 : 4;
  } break;

  case 14: {
    esi += (hiColor) ? 2 : 1;
  } break;

  case 15: {
    esi += (hiColor) ? 0 : 2;
  } break;
  }
  return -1;
}

//	HiColor version
//
void nfHPkDecomp(unsigned char *ops, unsigned char *comp, unsigned x, unsigned y, unsigned w, unsigned h) {
  PkDecompWorker(true, ops, comp, x, y, w, h);
}

void nfPkDecomp(unsigned char *ops, unsigned char *comp, unsigned x, unsigned y, unsigned w, unsigned h) {
  PkDecompWorker(false, ops, comp, x, y, w, h);
}

void PkDecompWorker(const bool hiColor, const unsigned char *ops, const unsigned char *comp, const unsigned x,
                    const unsigned y, unsigned w, unsigned h) {
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

  tNextFrame nf;
  nf.w = w;
  nf.h = h;
  nf.x = x;
  nf.y = y;

  NF_DECOMP_INIT((hiColor) ? 1 : 0, &nf);
  const unsigned int kSWidthScaled = (hiColor) ? SWIDTH * 2 : SWIDTH;

  int nfpk_back_right = nf_back_right - kSWidthScaled;

  const unsigned char *esi = comp;
  unsigned char *edi = nf.tbuf;

  const unsigned char *bcomp = NULL;
  if (hiColor) {
    unsigned int bcompOffset = IntelSwapper(*((const unsigned short *)esi));
    bcomp = esi + bcompOffset;
    esi += 2;
  }

  int wcnt = w >> 1;
  do {
    if (--wcnt < 0) {
      edi += nf.new_row;
      --h;
      wcnt = w >> 1;
      continue;
    }

    bool first_opcode = true;
    bool skipNextOpCode = false;

    unsigned char opcode = *ops++;
    int opcode_to_use = opcode & 0xF;

  do_next_opcode:

    /////////////////////////////////////////////////////////////////////////////
    // Note: this is all you should have to tweak
    bool opcodesToProcess[16] = {true, true, true, true,  // 0  - 3
                                 true, true, true, true,  // 4  - 7
                                 true, true, true, true,  // 8  - 11
                                 true, true, true, true}; // 12 - 15
    if (!opcodesToProcess[opcode_to_use]) {
      opcode_to_use = SkipOpcode(opcode_to_use, hiColor, esi, bcomp, edi);
    }
    /////////////////////////////////////////////////////////////////////////////

    switch (opcode_to_use) {
    case -1: {
      // DEBUG: skip processing
      // Note: esi will have been adjusted above in SkipOpcode

      if (hiColor) {
        // Here we can set the color to exactly what we want
        const unsigned short kColor = 0;
        unsigned short *pDst = reinterpret_cast<unsigned short *>(edi);
        for (int r = 0; r < 8; ++r) {
          pDst[0] = kColor;
          pDst[1] = kColor;
          pDst[2] = kColor;
          pDst[3] = kColor;
          pDst[4] = kColor;
          pDst[5] = kColor;
          pDst[6] = kColor;
          pDst[7] = kColor;
          pDst += nf_width >> 1;
        }
      } else {
        // Here we paint with the palette index we want
        const unsigned int kPaletteIndex = 0;

        unsigned int *pDst = reinterpret_cast<unsigned int *>(edi);
        const unsigned int kPalIndexDWord =
            (kPaletteIndex << 24) | (kPaletteIndex << 16) | (kPaletteIndex << 8) | kPaletteIndex;
        for (int r = 0; r < 8; ++r) {
          pDst[0] = kPalIndexDWord;
          pDst[1] = kPalIndexDWord;
          pDst += nf_width >> 2;
        }
      }

      // Adjust edi
      edi += kSWidthScaled;
    } break;

    case 0: {
      // No change from previous buffer
      DOnf_shift(hiColor, nf.DiffBufPtrs, edi, nfpk_back_right);
    } break;
    case 1: {
      // No change (and copied to screen)
      edi += kSWidthScaled;
    } break;
    case 2: {
      // Near shift from older part of current buffer
      unsigned int offset;
      if (hiColor) {
        offset = *bcomp++;
      } else {
        offset = *esi++;
      }
      DOnf_xycshift(hiColor, nfpk_ShiftP2[offset], edi, nfpk_back_right);
    } break;
    case 3: {
      // Near shift from newer part of current buffer
      union {
        unsigned int eax;
        reg_word word;
        reg_byte byte;
      } myeax;

      if (hiColor) {
        myeax.eax = *bcomp++;
      } else {
        myeax.eax = *esi++;
      }

      myeax.word.ax = nfpk_ShiftP2[myeax.eax];
      myeax.byte.al = -myeax.byte.al;
      myeax.byte.ah = -myeax.byte.ah;

      DOnf_xycshift(hiColor, myeax.eax, edi, nfpk_back_right);
    } break;
    case 4: {
      // Near shift from previous buffer
      union {
        unsigned int eax;
        reg_word word;
        reg_byte byte;
      } myeax;

      if (hiColor) {
        myeax.eax = *bcomp++;
      } else {
        myeax.eax = *esi++;
      }

      myeax.word.ax = nfpk_ShiftP1[myeax.eax];
      DOnf_xypshift(hiColor, myeax.eax, edi, nfpk_back_right, nf.DiffBufPtrs);
    } break;
    case 5: {
      // Far shift from previous buffer
      unsigned int eax = IntelSwapper(*(unsigned short *)(esi));
      esi += 2;
      DOnf_xypshift(hiColor, eax, edi, nfpk_back_right, nf.DiffBufPtrs);
    } break;
    case 6: {
      if (hiColor) {
        // Far shift from current buffer
        unsigned int val1 = IntelSwapper(*(unsigned short *)esi);
        esi += 2;
        DOnf_xycshift(hiColor, val1, edi, nfpk_back_right);
      } else {
        // Run of no changes (must only appear in first nibble opcodes)
        // Next nibble k specifies 2k+4 squares with no changes
        skipNextOpCode = true;                        // Next nibble is not an opcode
        unsigned int cnt = ((opcode >> 4) & 0xF) + 2; // (minimum of 4 squares)

        while (true) {
          edi += SWIDTH * 2; // Advance over two squares
          if (--cnt == 0)
            break; // Last pair of squares

          if (--wcnt >= 0) // Same row?
            continue;      // Yes

          edi += nf.new_row;   // Advance to next row
          --h;                 // Decrement row count (should never become zero here)
          wcnt = (w >> 1) - 1; // Reset wcnt
        }
      }
    } break;
    case 7: {
      bool donf23 = false;
      if (hiColor) {
        unsigned short val = IntelSwapper(*(unsigned short *)esi);
        if (val & 0x8000) {
          donf23 = true;
        }
      } else {
        if (esi[0] > esi[1]) {
          donf23 = true;
        }
      }

      if (!donf23) {
        // 8x8x1 (12/10 bytes)
        unsigned int colors[4];
        if (hiColor) {
          unsigned int ecx = (Trans16(esi + 2) << 16) | Trans16(esi);
          unsigned int edx = (ecx >> 16) | (ecx << 16);
          unsigned int ebx = (edx & 0xFFFF0000) | (ecx & 0xFFFF);
          unsigned int ebp = (ecx & 0xFFFF0000) | (edx & 0xFFFF);
          colors[0] = ebx;
          colors[1] = edx;
          colors[2] = ecx;
          colors[3] = ebp;
        } else {
          unsigned int tcLo = esi[0];
          unsigned int tcHi = esi[1];
          colors[0] = tcLo | (tcLo << 8);
          colors[1] = tcHi | (tcLo << 8);
          colors[2] = tcLo | (tcHi << 8);
          colors[3] = tcHi | (tcHi << 8);
        }

        const unsigned char *lookupTable = (hiColor) ? nfhpk_mov8 : nfpk_mov8;
        const int kOffset = (hiColor) ? 4 : 2;
        const int max_repcount = 8;
        for (int rep_count = 0; rep_count < max_repcount; ++rep_count) {
          const unsigned char *color_idx = lookupTable + (esi[rep_count + kOffset] * 4);
#ifdef OUTRAGE_BIG_ENDIAN
          unsigned int w1 = colors[color_idx[3]];
          unsigned int w2 = colors[color_idx[2]];
          unsigned int w3 = colors[color_idx[1]];
          unsigned int w4 = colors[color_idx[0]];
#else
          unsigned int w1 = colors[color_idx[0]];
          unsigned int w2 = colors[color_idx[1]];
          unsigned int w3 = colors[color_idx[2]];
          unsigned int w4 = colors[color_idx[3]];
#endif
          if (hiColor) {
            *(unsigned int *)(edi + 0) = w1;
            *(unsigned int *)(edi + 4) = w2;
            *(unsigned int *)(edi + 8) = w3;
            *(unsigned int *)(edi + 12) = w4;
          } else {
            *(unsigned short *)(edi + 0) = w1;
            *(unsigned short *)(edi + 2) = w2;
            *(unsigned short *)(edi + 4) = w3;
            *(unsigned short *)(edi + 6) = w4;
          }

          edi += nf_width;
        }
        esi += hiColor ? 12 : 10;
        edi -= nf_width;
        edi -= nfpk_back_right;
      } else {
        // low 4x4x1 (6/4 bytes)
        unsigned int colors[2];
        if (hiColor) {
          unsigned int temp = nf_trans16_lo[*(esi)] | nf_trans16_hi[*(esi + 1)];
          colors[0] = (temp << 16) | temp;
          temp = nf_trans16_lo[*(esi + 2)] | nf_trans16_hi[*(esi + 3)];
          colors[1] = (temp << 16) | temp;
        } else {
          unsigned int tcLo = esi[0];
          unsigned int tcHi = esi[1];
          colors[0] = tcLo | (tcLo << 8);
          colors[1] = tcHi | (tcHi << 8);
        }

        const unsigned char *esiOffset = (hiColor) ? (esi + 4) : (esi + 2);
        const unsigned char *lookupTable = (hiColor) ? nfhpk_mov4l : nfpk_mov4l;
        const int max_repcount = 4;
        for (int rep_count = 0; rep_count < max_repcount; ++rep_count) {
          int idx;
          switch (rep_count) {
          case 0:
            idx = esiOffset[0] & 0xF;
            break;
          case 1:
            idx = esiOffset[0] >> 4;
            break;
          case 2:
            idx = esiOffset[1] & 0xF;
            break;
          case 3:
            idx = esiOffset[1] >> 4;
            break;
          }

          const unsigned char *color_idx = lookupTable + (idx * 4);
#ifdef OUTRAGE_BIG_ENDIAN
          unsigned int w1 = colors[color_idx[3]];
          unsigned int w2 = colors[color_idx[2]];
          unsigned int w3 = colors[color_idx[1]];
          unsigned int w4 = colors[color_idx[0]];
#else
          unsigned int w1 = colors[color_idx[0]];
          unsigned int w2 = colors[color_idx[1]];
          unsigned int w3 = colors[color_idx[2]];
          unsigned int w4 = colors[color_idx[3]];
#endif

          if (hiColor) {
            *(unsigned int *)(edi + 0) = w1;
            *(unsigned int *)(edi + 4) = w2;
            *(unsigned int *)(edi + 8) = w3;
            *(unsigned int *)(edi + 12) = w4;
            *(unsigned int *)(edi + nf_width + 0) = w1;
            *(unsigned int *)(edi + nf_width + 4) = w2;
            *(unsigned int *)(edi + nf_width + 8) = w3;
            *(unsigned int *)(edi + nf_width + 12) = w4;
          } else {
            *(unsigned short *)(edi + 0) = w1;
            *(unsigned short *)(edi + 2) = w2;
            *(unsigned short *)(edi + 4) = w3;
            *(unsigned short *)(edi + 6) = w4;
            *(unsigned short *)(edi + 0 + nf_width) = w1;
            *(unsigned short *)(edi + 2 + nf_width) = w2;
            *(unsigned short *)(edi + 4 + nf_width) = w3;
            *(unsigned short *)(edi + 6 + nf_width) = w4;
          }

          edi += nf_width * 2;
        }
        edi -= nf_width * 2;
        edi += nf_width;
        edi -= nfpk_back_right;
        esi += (hiColor) ? 6 : 4;
      }
    } break;
    case 8: {
      bool donf24 = false;
      bool donf40 = false;

      if (hiColor) {
        unsigned short val = IntelSwapper(*(unsigned short *)esi);
        if (val & 0x8000) {
          val = IntelSwapper(*(unsigned short *)(esi + 8));
          if (val & 0x8000) {
            donf40 = true;
          } else {
            donf24 = true;
          }
        }
      } else {
        if (esi[0] > esi[1]) {
          if (esi[6] > esi[7]) {
            donf40 = true;
          } else {
            donf24 = true;
          }
        }
      }

      if (!donf24 && !donf40) {
        unsigned int colors[4];
        int max_repcount = 8;

        if (hiColor) {
          unsigned int tempcolor;
          tempcolor = nf_trans16_lo[*(esi + 2)] | nf_trans16_hi[*(esi + 3)];
          tempcolor = (tempcolor << 16) | (nf_trans16_lo[*(esi + 0)] | nf_trans16_hi[*(esi + 1)]);
          colors[2] = tempcolor;
          tempcolor = ((tempcolor & 0xFFFF0000) >> 16) | ((tempcolor & 0xFFFF) << 16);
          colors[0] = (tempcolor & 0xFFFF0000) | (colors[2] & 0xFFFF);
          colors[1] = tempcolor;
          colors[3] = (colors[2] & 0xFFFF0000) | (colors[1] & 0xFFFF);
        } else {
          unsigned int tcLo = esi[0];
          unsigned int tcHi = esi[1];
          colors[0] = tcLo | (tcLo << 8);
          colors[1] = tcHi | (tcLo << 8);
          colors[2] = tcLo | (tcHi << 8);
          colors[3] = tcHi | (tcHi << 8);
        }

        int repLookup[8] = {4, 5, 10, 11, 16, 17, 22, 23};
        if (!hiColor) {
          repLookup[0] -= 2;
          repLookup[1] -= 2;
          repLookup[2] -= 4;
          repLookup[3] -= 4;
          repLookup[4] -= 6;
          repLookup[5] -= 6;
          repLookup[6] -= 8;
          repLookup[7] -= 8;
        }
        const unsigned char *lookupTable = (hiColor) ? nfhpk_mov8 : nfpk_mov8;

        for (int rep_count = 0; rep_count < max_repcount; ++rep_count) {
          int idx = repLookup[rep_count];
          const unsigned char *color_idx = lookupTable + (esi[idx] * 4);
#ifdef OUTRAGE_BIG_ENDIAN
          unsigned int w1 = colors[color_idx[3]];
          unsigned int w2 = colors[color_idx[2]];
          unsigned int w3 = colors[color_idx[1]];
          unsigned int w4 = colors[color_idx[0]];
#else
          unsigned int w1 = colors[color_idx[0]];
          unsigned int w2 = colors[color_idx[1]];
          unsigned int w3 = colors[color_idx[2]];
          unsigned int w4 = colors[color_idx[3]];
#endif

          if (hiColor) {
            *(unsigned int *)(edi + 0) = w1;
            *(unsigned int *)(edi + 4) = w2;
            edi += nf_width;
            *(unsigned int *)(edi + 0) = w3;
            *(unsigned int *)(edi + 4) = w4;
            edi += nf_width;
          } else {
            *(unsigned short *)(edi + 0) = w1;
            *(unsigned short *)(edi + 2) = w2;
            edi += nf_width;
            *(unsigned short *)(edi + 0) = w3;
            *(unsigned short *)(edi + 2) = w4;
            edi += nf_width;
          }

          if (rep_count == 1) {
            if (hiColor) {
              unsigned int tempcolor = nf_trans16_lo[*(esi + 8)] | nf_trans16_hi[*(esi + 9)];
              tempcolor = (tempcolor << 16) | (nf_trans16_lo[*(esi + 6)] | nf_trans16_hi[*(esi + 7)]);
              colors[2] = tempcolor;
              tempcolor = ((tempcolor & 0xFFFF0000) >> 16) | ((tempcolor & 0xFFFF) << 16);
              colors[0] = (tempcolor & 0xFFFF0000) | (colors[2] & 0xFFFF);
              colors[1] = tempcolor;
              colors[3] = (colors[2] & 0xFFFF0000) | (colors[1] & 0xFFFF);
            } else {
              unsigned int tcLo = esi[4];
              unsigned int tcHi = esi[5];
              colors[0] = tcLo | (tcLo << 8);
              colors[1] = tcHi | (tcLo << 8);
              colors[2] = tcLo | (tcHi << 8);
              colors[3] = tcHi | (tcHi << 8);
            }
          } else if (rep_count == 3) {
            if (hiColor) {
              edi -= nf_width * 8 - 8;
              unsigned int tempcolor = nf_trans16_lo[*(esi + 14)] | nf_trans16_hi[*(esi + 15)];
              tempcolor = (tempcolor << 16) | (nf_trans16_lo[*(esi + 12)] | nf_trans16_hi[*(esi + 13)]);
              colors[2] = tempcolor;
              tempcolor = ((tempcolor & 0xFFFF0000) >> 16) | ((tempcolor & 0xFFFF) << 16);
              colors[0] = (tempcolor & 0xFFFF0000) | (colors[2] & 0xFFFF);
              colors[1] = tempcolor;
              colors[3] = (colors[2] & 0xFFFF0000) | (colors[1] & 0xFFFF);
            } else {
              edi -= nf_width * 8 - 4;
              unsigned int tcLo = esi[8];
              unsigned int tcHi = esi[9];
              colors[0] = tcLo | (tcLo << 8);
              colors[1] = tcHi | (tcLo << 8);
              colors[2] = tcLo | (tcHi << 8);
              colors[3] = tcHi | (tcHi << 8);
            }
          } else if (rep_count == 5) {
            if (hiColor) {
              unsigned int tempcolor = nf_trans16_lo[*(esi + 20)] | nf_trans16_hi[*(esi + 21)];
              tempcolor = (tempcolor << 16) | (nf_trans16_lo[*(esi + 18)] | nf_trans16_hi[*(esi + 19)]);
              colors[2] = tempcolor;
              tempcolor = ((tempcolor & 0xFFFF0000) >> 16) | ((tempcolor & 0xFFFF) << 16);
              colors[0] = (tempcolor & 0xFFFF0000) | (colors[2] & 0xFFFF);
              colors[1] = tempcolor;
              colors[3] = (colors[2] & 0xFFFF0000) | (colors[1] & 0xFFFF);
            } else {
              unsigned int tcLo = esi[12];
              unsigned int tcHi = esi[13];
              colors[0] = tcLo | (tcLo << 8);
              colors[1] = tcHi | (tcLo << 8);
              colors[2] = tcLo | (tcHi << 8);
              colors[3] = tcHi | (tcHi << 8);
            }
          }
        }

        edi -= nf_width;
        esi += (hiColor) ? 24 : 16;
        edi -= ((hiColor) ? 8 : 4) + nfpk_back_right;
      }

      if (donf24) {
        unsigned int colors[4];

        if (hiColor) {
          unsigned int tempcolor = nf_trans16_lo[*(esi + 2)] | nf_trans16_hi[*(esi + 3)];
          tempcolor = (tempcolor << 16) | (nf_trans16_lo[*(esi + 0)] | nf_trans16_hi[*(esi + 1)]);
          colors[2] = tempcolor;
          tempcolor = ((tempcolor & 0xFFFF0000) >> 16) | ((tempcolor & 0xFFFF) << 16);
          colors[0] = (tempcolor & 0xFFFF0000) | (colors[2] & 0xFFFF);
          colors[1] = tempcolor;
          colors[3] = (colors[2] & 0xFFFF0000) | (colors[1] & 0xFFFF);
        } else {
          unsigned int tcLo = esi[0];
          unsigned int tcHi = esi[1];
          colors[0] = tcLo | (tcLo << 8);
          colors[1] = tcHi | (tcLo << 8);
          colors[2] = tcLo | (tcHi << 8);
          colors[3] = tcHi | (tcHi << 8);
        }

        int repLookupTable[8] = {4, 5, 6, 7, 12, 13, 14, 15};
        if (!hiColor) {
          repLookupTable[0] -= 2;
          repLookupTable[1] -= 2;
          repLookupTable[2] -= 2;
          repLookupTable[3] -= 2;
          repLookupTable[4] -= 4;
          repLookupTable[5] -= 4;
          repLookupTable[6] -= 4;
          repLookupTable[7] -= 4;
        }

        const unsigned char *lookupTable = (hiColor) ? nfhpk_mov8 : nfpk_mov8;

        int max_repcount = 8;
        for (int rep_count = 0; rep_count < max_repcount; rep_count++) {
          int idx = repLookupTable[rep_count];
          const unsigned char *color_idx = lookupTable + (esi[idx] * 4);
          unsigned int w1 = colors[color_idx[0]];
          unsigned int w2 = colors[color_idx[1]];
          unsigned int w3 = colors[color_idx[2]];
          unsigned int w4 = colors[color_idx[3]];

          if (hiColor) {
            *(unsigned int *)(edi + 0) = w1;
            *(unsigned int *)(edi + 4) = w2;
            edi += nf_width;
            *(unsigned int *)(edi + 0) = w3;
            *(unsigned int *)(edi + 4) = w4;
            edi += nf_width;
          } else {
            *(unsigned short *)(edi + 0) = w1;
            *(unsigned short *)(edi + 2) = w2;
            edi += nf_width;
            *(unsigned short *)(edi + 0) = w3;
            *(unsigned short *)(edi + 2) = w4;
            edi += nf_width;
          }

          if (rep_count == 3) {
            if (hiColor) {
              edi -= nf_width * 8 - 8;

              unsigned int tempcolor = nf_trans16_lo[*(esi + 10)] | nf_trans16_hi[*(esi + 11)];
              tempcolor = (tempcolor << 16) | (nf_trans16_lo[*(esi + 8)] | nf_trans16_hi[*(esi + 9)]);
              colors[2] = tempcolor;
              tempcolor = ((tempcolor & 0xFFFF0000) >> 16) | ((tempcolor & 0xFFFF) << 16);
              colors[0] = (tempcolor & 0xFFFF0000) | (colors[2] & 0xFFFF);
              colors[1] = tempcolor;
              colors[3] = (colors[2] & 0xFFFF0000) | (colors[1] & 0xFFFF);
            } else {
              edi -= nf_width * 8 - 4;

              unsigned int tcLo = esi[6];
              unsigned int tcHi = esi[7];
              colors[0] = tcLo | (tcLo << 8);
              colors[1] = tcHi | (tcLo << 8);
              colors[2] = tcLo | (tcHi << 8);
              colors[3] = tcHi | (tcHi << 8);
            }
          }
        }

        esi += (hiColor) ? 16 : 12;
        edi -= nf_width + ((hiColor) ? 8 : 4) + nfpk_back_right;
      }

      if (donf40) {
        unsigned int colors[4];

        if (hiColor) {
          unsigned int tempcolor = nf_trans16_lo[*(esi + 2)] | nf_trans16_hi[*(esi + 3)];
          tempcolor = (tempcolor << 16) | (nf_trans16_lo[*(esi + 0)] | nf_trans16_hi[*(esi + 1)]);
          colors[2] = tempcolor;
          tempcolor = ((tempcolor & 0xFFFF0000) >> 16) | ((tempcolor & 0xFFFF) << 16);
          colors[0] = (tempcolor & 0xFFFF0000) | (colors[2] & 0xFFFF);
          colors[1] = tempcolor;
          colors[3] = (colors[2] & 0xFFFF0000) | (colors[1] & 0xFFFF);
        } else {
          unsigned int tcLo = esi[0];
          unsigned int tcHi = esi[1];
          colors[0] = tcLo | (tcLo << 8);
          colors[1] = tcHi | (tcLo << 8);
          colors[2] = tcLo | (tcHi << 8);
          colors[3] = tcHi | (tcHi << 8);
        }

        int repLookupTable[8] = {4, 5, 6, 7, 12, 13, 14, 15};
        if (!hiColor) {
          repLookupTable[0] -= 2;
          repLookupTable[1] -= 2;
          repLookupTable[2] -= 2;
          repLookupTable[3] -= 2;
          repLookupTable[4] -= 4;
          repLookupTable[5] -= 4;
          repLookupTable[6] -= 4;
          repLookupTable[7] -= 4;
        }
        const unsigned char *lookupTable = (hiColor) ? nfhpk_mov8 : nfpk_mov8;

        int max_repcount = 8;
        for (int rep_count = 0; rep_count < max_repcount; ++rep_count) {
          int idx = repLookupTable[rep_count];
          const unsigned char *color_idx = lookupTable + (esi[idx] * 4);
#ifdef OUTRAGE_BIG_ENDIAN
          unsigned int w1 = colors[color_idx[3]];
          unsigned int w2 = colors[color_idx[2]];
          unsigned int w3 = colors[color_idx[1]];
          unsigned int w4 = colors[color_idx[0]];
#else
          unsigned int w1 = colors[color_idx[0]];
          unsigned int w2 = colors[color_idx[1]];
          unsigned int w3 = colors[color_idx[2]];
          unsigned int w4 = colors[color_idx[3]];
#endif
          if (hiColor) {
            *(unsigned int *)(edi + 0) = w1;
            *(unsigned int *)(edi + 4) = w2;
            *(unsigned int *)(edi + 8) = w3;
            *(unsigned int *)(edi + 12) = w4;
          } else {
            *(unsigned short *)(edi + 0) = w1;
            *(unsigned short *)(edi + 2) = w2;
            *(unsigned short *)(edi + 4) = w3;
            *(unsigned short *)(edi + 6) = w4;
          }
          edi += nf_width;

          if (rep_count == 3) {
            if (hiColor) {
              unsigned int tempcolor = nf_trans16_lo[*(esi + 10)] | nf_trans16_hi[*(esi + 11)];
              tempcolor = (tempcolor << 16) | (nf_trans16_lo[*(esi + 8)] | nf_trans16_hi[*(esi + 9)]);
              colors[2] = tempcolor;
              tempcolor = ((tempcolor & 0xFFFF0000) >> 16) | ((tempcolor & 0xFFFF) << 16);
              colors[0] = (tempcolor & 0xFFFF0000) | (colors[2] & 0xFFFF);
              colors[1] = tempcolor;
              colors[3] = (colors[2] & 0xFFFF0000) | (colors[1] & 0xFFFF);
            } else {
              unsigned int tcLo = esi[6];
              unsigned int tcHi = esi[7];
              colors[0] = tcLo | (tcLo << 8);
              colors[1] = tcHi | (tcLo << 8);
              colors[2] = tcLo | (tcHi << 8);
              colors[3] = tcHi | (tcHi << 8);
            }
          }
        }

        esi += (hiColor) ? 16 : 12;
        edi -= nf_width + nfpk_back_right;
      }
    } break;
    case 9: {
      bool donf41 = false;
      bool donf25 = false;
      bool donf57 = false;

      if (hiColor) {
        unsigned short val = IntelSwapper(*(unsigned short *)esi);
        if (val & 0x8000) {
          val = IntelSwapper(*(unsigned short *)(esi + 4));
          if (val & 0x8000) {
            donf57 = true;
          } else {
            donf41 = true;
          }

        } else {
          val = IntelSwapper(*(unsigned short *)(esi + 4));
          if (val & 0x8000) {
            donf25 = true;
          }
        }
      } else {
        if (esi[0] > esi[1]) {
          if (esi[2] > esi[3]) {
            donf57 = true;
          } else {
            donf41 = true;
          }
        } else if (esi[2] > esi[3]) {
          donf25 = true;
        }
      }

      if (donf57) {
        unsigned short colors[4];
        if (hiColor) {
          colors[0] = nf_trans16_lo[*(esi)] | nf_trans16_hi[*(esi + 1)];
          colors[1] = nf_trans16_lo[*(esi + 2)] | nf_trans16_hi[*(esi + 3)];
          colors[2] = nf_trans16_lo[*(esi + 4)] | nf_trans16_hi[*(esi + 5)];
          colors[3] = nf_trans16_lo[*(esi + 6)] | nf_trans16_hi[*(esi + 7)];
        } else {
          colors[0] = esi[0];
          colors[1] = esi[1];
          colors[2] = esi[2];
          colors[3] = esi[3];
        }

        const int max_repcount = 8;
        const unsigned char *lookupTable = (hiColor) ? nfhpk_mov4 : nfpk_mov4;
        for (int rep_count = 0; rep_count < max_repcount; ++rep_count) {
          int idx = rep_count + ((hiColor) ? 8 : 4);
          const unsigned char *color_idx = lookupTable + (esi[idx] * 4);

          unsigned int w1 = colors[color_idx[0]];
          unsigned int w2 = colors[color_idx[1]];
          unsigned int w3 = colors[color_idx[2]];
          unsigned int w4 = colors[color_idx[3]];

          if (rep_count & 1) {
#ifdef OUTRAGE_BIG_ENDIAN
            if (hiColor) {
              *(unsigned int *)(edi + 8) = (w1 & 0xFFFF) | ((w2 & 0xFFFF) << 16);
              *(unsigned int *)(edi + nf_width + 8) = (w1 & 0xFFFF) | ((w2 & 0xFFFF) << 16);

              *(unsigned int *)(edi + 12) = (w3 & 0xFFFF) | ((w4 & 0xFFFF) << 16);
              *(unsigned int *)(edi + nf_width + 12) = (w3 & 0xFFFF) | ((w4 & 0xFFFF) << 16);
            } else {
              *(unsigned char *)(edi + 4) = w4;
              *(unsigned char *)(edi + 5) = w3;
              *(unsigned char *)(edi + 6) = w2;
              *(unsigned char *)(edi + 7) = w1;

              *(unsigned char *)(edi + nf_width + 4) = w4;
              *(unsigned char *)(edi + nf_width + 5) = w3;
              *(unsigned char *)(edi + nf_width + 6) = w2;
              *(unsigned char *)(edi + nf_width + 7) = w1;
            }
#else
            if (hiColor) {
              *(unsigned int *)(edi + 8) = (w1 & 0xFFFF) | ((w2 & 0xFFFF) << 16);
              *(unsigned int *)(edi + nf_width + 8) = (w1 & 0xFFFF) | ((w2 & 0xFFFF) << 16);

              *(unsigned int *)(edi + 12) = (w3 & 0xFFFF) | ((w4 & 0xFFFF) << 16);
              *(unsigned int *)(edi + nf_width + 12) = (w3 & 0xFFFF) | ((w4 & 0xFFFF) << 16);
            } else {
              *(unsigned char *)(edi + 4) = w1;
              *(unsigned char *)(edi + 5) = w2;
              *(unsigned char *)(edi + 6) = w3;
              *(unsigned char *)(edi + 7) = w4;

              *(unsigned char *)(edi + nf_width + 4) = w1;
              *(unsigned char *)(edi + nf_width + 5) = w2;
              *(unsigned char *)(edi + nf_width + 6) = w3;
              *(unsigned char *)(edi + nf_width + 7) = w4;
            }
#endif
            edi += nf_width * 2;
          } else {
            if (hiColor) {
#ifdef OUTRAGE_BIG_ENDIAN
              *(unsigned int *)(edi + 0) = (w4 & 0xFFFF) | ((w3 & 0xFFFF) << 16);
              *(unsigned int *)(edi + nf_width) = (w4 & 0xFFFF) | ((w3 & 0xFFFF) << 16);

              *(unsigned int *)(edi + 4) = (w2 & 0xFFFF) | ((w1 & 0xFFFF) << 16);
              *(unsigned int *)(edi + nf_width + 4) = (w2 & 0xFFFF) | ((w1 & 0xFFFF) << 16);
#else
              *(unsigned int *)(edi + 0) = (w1 & 0xFFFF) | ((w2 & 0xFFFF) << 16);
              *(unsigned int *)(edi + nf_width) = (w1 & 0xFFFF) | ((w2 & 0xFFFF) << 16);

              *(unsigned int *)(edi + 4) = (w3 & 0xFFFF) | ((w4 & 0xFFFF) << 16);
              *(unsigned int *)(edi + nf_width + 4) = (w3 & 0xFFFF) | ((w4 & 0xFFFF) << 16);
#endif
            } else {
              const unsigned int value = IntelSwapper((unsigned int)(w4 << 24) | (w3 << 16) | (w2 << 8) | w1);
              *(unsigned int *)(edi) = value;
              *(unsigned int *)(edi + nf_width) = value;
            }
          }
        }

        esi += (hiColor) ? 16 : 12;
        edi -= nf_width * 2;
        edi += nf_width;
        edi -= nfpk_back_right;
      } else if (donf41) {
        unsigned int colors[4];
        if (hiColor) {
          colors[0] = nf_trans16_lo[*(esi)] | nf_trans16_hi[*(esi + 1)];
          colors[0] = ((colors[0]) << 16) | colors[0];
          colors[1] = nf_trans16_lo[*(esi + 2)] | nf_trans16_hi[*(esi + 3)];
          colors[1] = ((colors[1]) << 16) | colors[1];
          colors[2] = nf_trans16_lo[*(esi + 4)] | nf_trans16_hi[*(esi + 5)];
          colors[2] = ((colors[2]) << 16) | colors[2];
          colors[3] = nf_trans16_lo[*(esi + 6)] | nf_trans16_hi[*(esi + 7)];
          colors[3] = ((colors[3]) << 16) | colors[3];
        } else {
          colors[0] = esi[0];
          colors[1] = esi[1];
          colors[2] = esi[2];
          colors[3] = esi[3];
        }

        const int max_repcount = 8;
        const unsigned char *lookupTable = (hiColor) ? nfhpk_mov8 : nfpk_mov4;
        for (int rep_count = 0; rep_count < max_repcount; ++rep_count) {
          int idx = rep_count + ((hiColor) ? 8 : 4);
          const unsigned char *color_idx = lookupTable + (esi[idx] * 4);
          unsigned int w1 = colors[color_idx[0]];
          unsigned int w2 = colors[color_idx[1]];
          unsigned int w3 = colors[color_idx[2]];
          unsigned int w4 = colors[color_idx[3]];
#ifdef OUTRAGE_BIG_ENDIAN
          if (hiColor) {
            *(unsigned int *)(edi + 0) = w4;
            *(unsigned int *)(edi + 4) = w3;
            *(unsigned int *)(edi + 8) = w2;
            *(unsigned int *)(edi + 12) = w1;
          } else {
            *(unsigned char *)(edi + 0) = w4;
            *(unsigned char *)(edi + 1) = w4;
            *(unsigned char *)(edi + 2) = w3;
            *(unsigned char *)(edi + 3) = w3;
            *(unsigned char *)(edi + 4) = w2;
            *(unsigned char *)(edi + 5) = w2;
            *(unsigned char *)(edi + 6) = w1;
            *(unsigned char *)(edi + 7) = w1;
          }
#else
          if (hiColor) {
            *(unsigned int *)(edi + 0) = w1;
            *(unsigned int *)(edi + 4) = w2;
            *(unsigned int *)(edi + 8) = w3;
            *(unsigned int *)(edi + 12) = w4;
          } else {
            *(unsigned char *)(edi + 0) = w1;
            *(unsigned char *)(edi + 1) = w1;
            *(unsigned char *)(edi + 2) = w2;
            *(unsigned char *)(edi + 3) = w2;
            *(unsigned char *)(edi + 4) = w3;
            *(unsigned char *)(edi + 5) = w3;
            *(unsigned char *)(edi + 6) = w4;
            *(unsigned char *)(edi + 7) = w4;
          }

#endif
          edi += nf_width;
        }

        esi += (hiColor) ? 16 : 12;
        edi -= nf_width;
        edi -= nfpk_back_right;
      } else if (donf25) {
        unsigned int colors[4];
        if (hiColor) {
          colors[0] = nf_trans16_lo[*(esi)] | nf_trans16_hi[*(esi + 1)];
          colors[0] = ((colors[0]) << 16) | colors[0];
          colors[1] = nf_trans16_lo[*(esi + 2)] | nf_trans16_hi[*(esi + 3)];
          colors[1] = ((colors[1]) << 16) | colors[1];
          colors[2] = nf_trans16_lo[*(esi + 4)] | nf_trans16_hi[*(esi + 5)];
          colors[2] = ((colors[2]) << 16) | colors[2];
          colors[3] = nf_trans16_lo[*(esi + 6)] | nf_trans16_hi[*(esi + 7)];
          colors[3] = ((colors[3]) << 16) | colors[3];
        } else {
          colors[0] = esi[0];
          colors[1] = esi[1];
          colors[2] = esi[2];
          colors[3] = esi[3];
        }

        const unsigned char *lookupTable = (hiColor) ? nfhpk_mov4 : nfpk_mov4;
        const int max_repcount = 4;
        for (int rep_count = 0; rep_count < max_repcount; ++rep_count) {
          int idx = rep_count + (hiColor ? 8 : 4);
          const unsigned char *color_idx = lookupTable + (esi[idx] * 4);
          unsigned int w1 = colors[color_idx[0]];
          unsigned int w2 = colors[color_idx[1]];
          unsigned int w3 = colors[color_idx[2]];
          unsigned int w4 = colors[color_idx[3]];

          if (hiColor) {
#ifdef OUTRAGE_BIG_ENDIAN
            *(unsigned int *)(edi + 0) = w4;
            *(unsigned int *)(edi + nf_width) = w4;
            *(unsigned int *)(edi + 4) = w3;
            *(unsigned int *)(edi + nf_width + 4) = w3;
            *(unsigned int *)(edi + 8) = w2;
            *(unsigned int *)(edi + nf_width + 8) = w2;
            *(unsigned int *)(edi + 12) = w1;
            *(unsigned int *)(edi + nf_width + 12) = w1;
#else
            *(unsigned int *)(edi + 0) = w1;
            *(unsigned int *)(edi + nf_width) = w1;
            *(unsigned int *)(edi + 4) = w2;
            *(unsigned int *)(edi + nf_width + 4) = w2;
            *(unsigned int *)(edi + 8) = w3;
            *(unsigned int *)(edi + nf_width + 8) = w3;
            *(unsigned int *)(edi + 12) = w4;
            *(unsigned int *)(edi + nf_width + 12) = w4;
#endif
          } else {
            unsigned int c0 = IntelSwapper((unsigned int)(w2 << 24) | (w2 << 16) | (w1 << 8) | w1);
            unsigned int c1 = IntelSwapper((unsigned int)(w4 << 24) | (w4 << 16) | (w3 << 8) | w3);
            *(unsigned int *)(edi + 0) = c0;
            *(unsigned int *)(edi + nf_width) = c0;
            *(unsigned int *)(edi + 4) = c1;
            *(unsigned int *)(edi + nf_width + 4) = c1;
          }

          edi += nf_width * 2;
        }

        edi -= nf_width * 2;
        edi += nf_width;
        esi += (hiColor) ? 12 : 8;
        edi -= nfpk_back_right;
      } else {
        unsigned short colors[4];
        if (hiColor) {
          colors[0] = Trans16(esi + 0);
          colors[1] = Trans16(esi + 2);
          colors[2] = Trans16(esi + 4);
          colors[3] = Trans16(esi + 6);
        } else {
          colors[0] = esi[0];
          colors[1] = esi[1];
          colors[2] = esi[2];
          colors[3] = esi[3];
        }

        const unsigned char *lookupTable = (hiColor) ? nfhpk_mov4 : nfpk_mov4;
        const int max_repcount = 16;
        for (int rep_count = 0; rep_count < max_repcount; ++rep_count) {
          int idx = rep_count + ((hiColor) ? 8 : 4);
          const unsigned char *color_idx = lookupTable + (esi[idx] * 4);

          unsigned int w1 = colors[color_idx[0]];
          unsigned int w2 = colors[color_idx[1]];
          unsigned int w3 = colors[color_idx[2]];
          unsigned int w4 = colors[color_idx[3]];

          if (rep_count & 1) {
            if (hiColor) {
#ifdef OUTRAGE_BIG_ENDIAN
              *(unsigned int *)(edi + 8) = (w4 & 0xFFFF) | ((w3 & 0xFFFF) << 16);
              *(unsigned int *)(edi + 12) = (w2 & 0xFFFF) | ((w1 & 0xFFFF) << 16);
#else
              *(unsigned int *)(edi + 8) = (w1 & 0xFFFF) | ((w2 & 0xFFFF) << 16);
              *(unsigned int *)(edi + 12) = (w3 & 0xFFFF) | ((w4 & 0xFFFF) << 16);
#endif
            } else {
              *(unsigned int *)(edi + 4) = IntelSwapper((unsigned int)(w4 << 24) | (w3 << 16) | (w2 << 8) | w1);
            }

            edi += nf_width;
          } else {
            if (hiColor) {
#ifdef OUTRAGE_BIG_ENDIAN
              *(unsigned int *)(edi + 0) = (w4 & 0xFFFF) | ((w3 & 0xFFFF) << 16);
              *(unsigned int *)(edi + 4) = (w2 & 0xFFFF) | ((w1 & 0xFFFF) << 16);
#else
              *(unsigned int *)(edi + 0) = (w1 & 0xFFFF) | ((w2 & 0xFFFF) << 16);
              *(unsigned int *)(edi + 4) = (w3 & 0xFFFF) | ((w4 & 0xFFFF) << 16);
#endif
            } else {
              *(unsigned int *)(edi + 0) = IntelSwapper((unsigned int)(w4 << 24) | (w3 << 16) | (w2 << 8) | w1);
            }
          }
        }

        edi -= nf_width;
        esi += (hiColor) ? 24 : 20;
        edi -= nfpk_back_right;
      }
    } break;
    case 10: {
      // 2x2 4x4x2 (32 bytes) or 2x1 4x8x2 (24 bytes) or 1x2 4x8x2 (24 bytes)
      bool do26 = false;
      bool do42 = false;

      if (hiColor) {
        int val1 = IntelSwapper(*(unsigned short *)esi);
        if (val1 & 0x8000) {
          val1 = IntelSwapper(*(unsigned short *)(esi + 16));
          if (val1 & 0x8000) {
            do42 = true;
          } else {
            do26 = true;
          }
        }
      } else {
        if (esi[0] > esi[1]) {
          if (esi[12] > esi[13]) {
            do42 = true;
          } else {
            do26 = true;
          }
        }
      }

      // Load bx,dx,cx,bp with four colors
      unsigned short colors[4];
      if (hiColor) {
        colors[0] = nf_trans16_lo[*(esi)] | nf_trans16_hi[*(esi + 1)];
        colors[1] = nf_trans16_lo[*(esi + 2)] | nf_trans16_hi[*(esi + 3)];
        colors[2] = nf_trans16_lo[*(esi + 4)] | nf_trans16_hi[*(esi + 5)];
        colors[3] = nf_trans16_lo[*(esi + 6)] | nf_trans16_hi[*(esi + 7)];
      } else {
        colors[0] = esi[0];
        colors[1] = esi[1];
        colors[2] = esi[2];
        colors[3] = esi[3];
      }

      // build our iteration lookup table
      int repLookup[16];
      if (do26 || do42) {
        int baseCnt1 = (hiColor) ? 8 : 4;
        int baseCnt2 = (hiColor) ? 24 : 16;
        for (int i = 0; i < 8; ++i) {
          repLookup[i + 0] = baseCnt1 + i;
          repLookup[i + 8] = baseCnt2 + i;
        }
      } else {
        int baseCnt1 = (hiColor) ? 8 : 4;
        int baseCnt2 = (hiColor) ? 20 : 12;
        int baseCnt3 = (hiColor) ? 32 : 20;
        int baseCnt4 = (hiColor) ? 44 : 28;
        for (int i = 0; i < 4; ++i) {
          repLookup[i + 0] = baseCnt1 + i;
          repLookup[i + 4] = baseCnt2 + i;
          repLookup[i + 8] = baseCnt3 + i;
          repLookup[i + 12] = baseCnt4 + i;
        }
      }

      const unsigned char *lookupTable = (hiColor) ? nfhpk_mov4 : nfpk_mov4;
      for (int rep_count = 0; rep_count < 16; ++rep_count) {
        int idx = repLookup[rep_count];
        const unsigned char *color_idx = lookupTable + (esi[idx] * 4);
        unsigned int w1 = colors[color_idx[0]];
        unsigned int w2 = colors[color_idx[1]];
        unsigned int w3 = colors[color_idx[2]];
        unsigned int w4 = colors[color_idx[3]];

        if (!do42) {
          if (hiColor) {
#ifdef OUTRAGE_BIG_ENDIAN
            *(unsigned int *)(edi + 0) = ((unsigned int)(w4 & 0xFFFF) | ((w3 & 0xFFFF) << 16));
            *(unsigned int *)(edi + 4) = ((unsigned int)(w2 & 0xFFFF) | ((w1 & 0xFFFF) << 16));
#else
            *(unsigned int *)(edi + 0) = ((unsigned int)(w1 & 0xFFFF) | ((w2 & 0xFFFF) << 16));
            *(unsigned int *)(edi + 4) = ((unsigned int)(w3 & 0xFFFF) | ((w4 & 0xFFFF) << 16));
#endif
          } else {
            *(unsigned int *)(edi + 0) = IntelSwapper((unsigned int)(w4 << 24) | (w3 << 16) | (w2 << 8) | w1);
          }
          edi += nf_width;
        } else {
          // 42
          if (rep_count & 1) {
            if (hiColor) {
#ifdef OUTRAGE_BIG_ENDIAN
              *(unsigned int *)(edi + 8) = ((unsigned int)(w4 & 0xFFFF) | ((w3 & 0xFFFF) << 16));
              *(unsigned int *)(edi + 12) = ((unsigned int)(w2 & 0xFFFF) | ((w1 & 0xFFFF) << 16));
#else

              *(unsigned int *)(edi + 8) = ((unsigned int)(w1 & 0xFFFF) | ((w2 & 0xFFFF) << 16));
              *(unsigned int *)(edi + 12) = ((unsigned int)(w3 & 0xFFFF) | ((w4 & 0xFFFF) << 16));
#endif
            } else {
              *(unsigned int *)(edi + 4) = IntelSwapper((unsigned int)(w4 << 24) | (w3 << 16) | (w2 << 8) | w1);
            }
            edi += nf_width;
          } else {
            if (hiColor) {
#ifdef OUTRAGE_BIG_ENDIAN
              *(unsigned int *)(edi + 0) = ((unsigned int)(w4 & 0xFFFF) | ((w3 & 0xFFFF) << 16));
              *(unsigned int *)(edi + 4) = ((unsigned int)(w2 & 0xFFFF) | ((w1 & 0xFFFF) << 16));
#else
              *(unsigned int *)(edi + 0) = ((unsigned int)(w1 & 0xFFFF) | ((w2 & 0xFFFF) << 16));
              *(unsigned int *)(edi + 4) = ((unsigned int)(w3 & 0xFFFF) | ((w4 & 0xFFFF) << 16));
#endif
            } else {
              *(unsigned int *)(edi + 0) = IntelSwapper((unsigned int)(w4 << 24) | (w3 << 16) | (w2 << 8) | w1);
            }
          }
        }

        if (rep_count == 3 && !do42 && !do26) {
          if (hiColor) {
            // Load bx,dx,cx,bp with four colors
            colors[0] = nf_trans16_lo[*(esi + 12)] | nf_trans16_hi[*(esi + 13)];
            colors[1] = nf_trans16_lo[*(esi + 14)] | nf_trans16_hi[*(esi + 15)];
            colors[2] = nf_trans16_lo[*(esi + 16)] | nf_trans16_hi[*(esi + 17)];
            colors[3] = nf_trans16_lo[*(esi + 18)] | nf_trans16_hi[*(esi + 19)];
          } else {
            colors[0] = esi[8];
            colors[1] = esi[9];
            colors[2] = esi[10];
            colors[3] = esi[11];
          }
        }

        if (rep_count == 7) {
          if (!do42 && !do26) {
            edi -= nf_width * 8 - ((hiColor) ? 8 : 4);

            if (hiColor) {
              // Load bx,dx,cx,bp with four colors
              colors[0] = nf_trans16_lo[*(esi + 24)] | nf_trans16_hi[*(esi + 25)];
              colors[1] = nf_trans16_lo[*(esi + 26)] | nf_trans16_hi[*(esi + 27)];
              colors[2] = nf_trans16_lo[*(esi + 28)] | nf_trans16_hi[*(esi + 29)];
              colors[3] = nf_trans16_lo[*(esi + 30)] | nf_trans16_hi[*(esi + 31)];
            } else {
              colors[0] = esi[16];
              colors[1] = esi[17];
              colors[2] = esi[18];
              colors[3] = esi[19];
            }
          }

          if (do26 || do42) {
            if (do26) {
              edi -= nf_width * 8 - ((hiColor) ? 8 : 4);
            }

            if (hiColor) {
              // Load bx,dx,cx,bp with four colors
              colors[0] = nf_trans16_lo[*(esi + 16)] | nf_trans16_hi[*(esi + 17)];
              colors[1] = nf_trans16_lo[*(esi + 18)] | nf_trans16_hi[*(esi + 19)];
              colors[2] = nf_trans16_lo[*(esi + 20)] | nf_trans16_hi[*(esi + 21)];
              colors[3] = nf_trans16_lo[*(esi + 22)] | nf_trans16_hi[*(esi + 23)];
            } else {
              colors[0] = esi[12];
              colors[1] = esi[13];
              colors[2] = esi[14];
              colors[3] = esi[15];
            }
          }
        }

        if (rep_count == 11 && !do42 && !do26) {
          if (hiColor) {
            // Load bx,dx,cx,bp with four colors
            colors[0] = nf_trans16_lo[*(esi + 36)] | nf_trans16_hi[*(esi + 37)];
            colors[1] = nf_trans16_lo[*(esi + 38)] | nf_trans16_hi[*(esi + 39)];
            colors[2] = nf_trans16_lo[*(esi + 40)] | nf_trans16_hi[*(esi + 41)];
            colors[3] = nf_trans16_lo[*(esi + 42)] | nf_trans16_hi[*(esi + 43)];
          } else {
            colors[0] = esi[24];
            colors[1] = esi[25];
            colors[2] = esi[26];
            colors[3] = esi[27];
          }
        }
      }

      if (!do42 && !do26) {
        esi += (hiColor) ? 48 : 32;
      } else {
        esi += (hiColor) ? 32 : 24;
      }

      edi -= nf_width;
      if (!do42) {
        edi -= ((hiColor) ? 8 : 4) + nfpk_back_right;
      } else {
        edi -= nfpk_back_right;
      }
    } break;

    case 11: {
      // 8x8x16 (128 bytes)
      if (hiColor) {
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
      } else {
        // 8x8x8 (64 bytes)
        unsigned int *ediW = (unsigned int *)edi;
        unsigned int *esiW = (unsigned int *)esi;
        const unsigned int width = nf_width >> 2;
        ediW[0] = (esiW[0]);
        ediW[1] = (esiW[1]);
        ediW += width;
        ediW[0] = (esiW[2]);
        ediW[1] = (esiW[3]);
        ediW += width;
        ediW[0] = (esiW[4]);
        ediW[1] = (esiW[5]);
        ediW += width;
        ediW[0] = (esiW[6]);
        ediW[1] = (esiW[7]);
        ediW += width;
        ediW[0] = (esiW[8]);
        ediW[1] = (esiW[9]);
        ediW += width;
        ediW[0] = (esiW[10]);
        ediW[1] = (esiW[11]);
        ediW += width;
        ediW[0] = (esiW[12]);
        ediW[1] = (esiW[13]);
        ediW += width;
        ediW[0] = (esiW[14]);
        ediW[1] = (esiW[15]);
        esi += 64;
        edi = ((unsigned char *)ediW);
      }
      edi -= nfpk_back_right;
    } break;
    case 12: {
      for (int i = 0; i < 4; ++i) {
        if (hiColor) {
          // low 4x4x16 (32 bytes)
          unsigned int eax, ebx;

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
        } else {
          // low 4x4x8 (16 bytes)
          unsigned int eax, ebx;

          eax = IntelSwapper(((unsigned int *)esi)[i]);
          ebx = ((eax & 0xFF00) << 16) | ((eax & 0xFF00) << 8) | ((eax & 0xFF) << 8) | (eax & 0xFF);
          ((unsigned int *)edi)[0] = ebx;
          ((unsigned int *)(edi + nf_width))[0] = ebx;

          eax = eax >> 16;
          ebx = ((eax & 0xFF00) << 16) | ((eax & 0xFF00) << 8) | ((eax & 0xFF) << 8) | (eax & 0xFF);
          ((unsigned int *)edi)[1] = ebx;
          ((unsigned int *)(edi + nf_width))[1] = ebx;
        }

        edi += nf_width * 2;
      }

      edi -= nf_width * 2;
      edi += nf_width;
      edi -= nfpk_back_right;
      esi += (hiColor) ? 32 : 16;
    } break;
    case 13: {
      if (hiColor) {
        // 2x2 4x4x0 (8 bytes)
        for (int i = 0; i < 2; ++i) {
          const unsigned char *loopEsi = esi + (i * 4);

          unsigned int temp = nf_trans16_lo[loopEsi[0]] | nf_trans16_hi[loopEsi[1]];
          unsigned int ebx = ((temp & 0xFFFF) << 16) | (temp & 0xFFFF);

          temp = nf_trans16_lo[loopEsi[2]] | nf_trans16_hi[loopEsi[3]];
          unsigned int ecx = ((temp & 0xFFFF) << 16) | (temp & 0xFFFF);

          *(unsigned int *)(edi + 0) = ebx;
          *(unsigned int *)(edi + 4) = ebx;
          *(unsigned int *)(edi + 8) = ecx;
          *(unsigned int *)(edi + 12) = ecx;
          *(unsigned int *)(edi + nf_width) = ebx;
          *(unsigned int *)(edi + nf_width + 4) = ebx;
          *(unsigned int *)(edi + nf_width + 8) = ecx;
          *(unsigned int *)(edi + nf_width + 12) = ecx;

          edi += nf_width * 2;

          *(unsigned int *)(edi + 0) = ebx;
          *(unsigned int *)(edi + 4) = ebx;
          *(unsigned int *)(edi + 8) = ecx;
          *(unsigned int *)(edi + 12) = ecx;
          *(unsigned int *)(edi + nf_width) = ebx;
          *(unsigned int *)(edi + nf_width + 4) = ebx;
          *(unsigned int *)(edi + nf_width + 8) = ecx;
          *(unsigned int *)(edi + nf_width + 12) = ecx;

          edi += nf_width * 2;
        }
      } else {
        // 2x2 4x4x0 (4 bytes)
        for (int i = 0; i < 2; ++i) {
          unsigned int c1 = esi[(i << 1) + 0];
          unsigned int eax = (c1 << 24) | (c1 << 16) | (c1 << 8) | c1;

          unsigned int c2 = esi[(i << 1) + 1];
          unsigned int ebx = (c2 << 24) | (c2 << 16) | (c2 << 8) | c2;

          *(unsigned int *)(edi + 0) = eax;
          *(unsigned int *)(edi + 4) = ebx;
          *(unsigned int *)(edi + 0 + nf_width) = eax;
          *(unsigned int *)(edi + 4 + nf_width) = ebx;
          edi += nf_width * 2;

          *(unsigned int *)(edi + 0) = eax;
          *(unsigned int *)(edi + 4) = ebx;
          *(unsigned int *)(edi + 0 + nf_width) = eax;
          *(unsigned int *)(edi + 4 + nf_width) = ebx;
          edi += nf_width * 2;
        }
      }

      edi -= nf_width * 2;
      edi += nf_width;
      edi -= nfpk_back_right;
      esi += (hiColor) ? 8 : 4;
    } break;
    case 14: {
      if (hiColor) {
        //  8x8x0 (2 bytes)
        unsigned int ecx = nf_trans16_lo[esi[0]] | nf_trans16_hi[esi[1]];
        unsigned int ebx = ((ecx & 0xFFFF) << 16) | (ecx & 0xFFFF);
        esi += 2;

        for (int i = 0; i < 8; ++i) {
          *(unsigned int *)(edi + 0) = ebx;
          *(unsigned int *)(edi + 4) = ebx;
          *(unsigned int *)(edi + 8) = ebx;
          *(unsigned int *)(edi + 12) = ebx;
          edi += nf_width;
        }

        edi -= nf_width;
        edi -= nfpk_back_right;
      } else {
        // 8x8x0 (1 byte)
        unsigned int c = *esi++; // Copy color into 8 positions
        unsigned int col = (c << 24) | (c << 16) | (c << 8) | c;

        for (int i = 0; i < 8; ++i) {
          *(unsigned int *)(edi + 0) = col;
          *(unsigned int *)(edi + 4) = col;
          edi += nf_width;
        }
        edi -= nf_width;
        edi -= nfpk_back_right;
      }
    } break;
    case 15: {
      if (!hiColor) {
        // mix 8x8x0 (2 bytes)
        unsigned int ebx = IntelSwapper(*(unsigned short *)(esi)); // Copy 2 colors into 8 positions
        esi += 2;                                                  // in a checkerboard
        unsigned int eax = (ebx << 16) | ebx;
        ebx = (eax << 8) | ((eax >> 24) & 0xFF);

        for (int i = 0; i < 4; ++i) {
          *(unsigned int *)(edi + 0) = eax;
          *(unsigned int *)(edi + 4) = eax;
          edi += nf_width;

          *(unsigned int *)(edi + 0) = ebx;
          *(unsigned int *)(edi + 4) = ebx;
          edi += nf_width;
        }
        edi -= nf_width;
        edi -= nfpk_back_right;
      }
    } break;
    }

    if (first_opcode && !skipNextOpCode) {
      first_opcode = false;
      opcode_to_use = (opcode >> 4);
      goto do_next_opcode;
    }
  } while (h != 0);
}

void Trans16Blk(unsigned char *edi, const unsigned char *idx) {
  *((unsigned short *)(edi + 0)) = nf_trans16_lo[*(idx + 0)] | nf_trans16_hi[*(idx + 1)];
  *((unsigned short *)(edi + 2)) = nf_trans16_lo[*(idx + 2)] | nf_trans16_hi[*(idx + 3)];
  *((unsigned short *)(edi + 4)) = nf_trans16_lo[*(idx + 4)] | nf_trans16_hi[*(idx + 5)];
  *((unsigned short *)(edi + 6)) = nf_trans16_lo[*(idx + 6)] | nf_trans16_hi[*(idx + 7)];
  *((unsigned short *)(edi + 8)) = nf_trans16_lo[*(idx + 8)] | nf_trans16_hi[*(idx + 9)];
  *((unsigned short *)(edi + 10)) = nf_trans16_lo[*(idx + 10)] | nf_trans16_hi[*(idx + 11)];
  *((unsigned short *)(edi + 12)) = nf_trans16_lo[*(idx + 12)] | nf_trans16_hi[*(idx + 13)];
  *((unsigned short *)(edi + 14)) = nf_trans16_lo[*(idx + 14)] | nf_trans16_hi[*(idx + 15)];
}

void DOnf_xycshift(const bool hiColor, const unsigned int eax, unsigned char *&edi, const int nfpk_back_right) {
  unsigned int ebx = (eax >> 8) & 0xFF;

  // get the lower byte of the offset, but sign extend it
  int offset = eax & 0xFF;
  if (eax & 0x80) {
    // extend...
    offset |= 0xFFFFFF00;
  }

  // hiColor is multiplied by two
  if (hiColor) {
    offset <<= 1;
  }

  // factor in the table
  offset += nfpk_ShiftY[ebx];
  DOnf_shift(hiColor, offset, edi, nfpk_back_right);
}

void DOnf_xypshift(const bool hiColor, const unsigned int eax, unsigned char *&edi, const int nfpk_back_right,
                   const int DiffBufPtrs) {
  unsigned int ebx = (eax >> 8) & 0xFF;

  // get the lower byte of the offset, but sign extend it
  int offset = eax & 0xFF;
  if (eax & 0x80) {
    // extend...
    offset |= 0xFFFFFF00;
  }

  // hiColor is multiplied by two
  if (hiColor) {
    offset <<= 1;
  }

  // factor in the table
  offset += nfpk_ShiftY[ebx] + DiffBufPtrs;
  DOnf_shift(hiColor, offset, edi, nfpk_back_right);
}

// Copy the 128/64 bytes from an offset of edi to edi is.
void DOnf_shift(const bool hiColor, const int offset, unsigned char *&edi, const int nfpk_back_right) {
  union ptr {
    unsigned int *pAsInt;
    unsigned char *pAsChar;
  };
  ptr dstBuffer;
  dstBuffer.pAsChar = edi;
  ptr srcBuffer;
  srcBuffer.pAsChar = edi + offset;

  for (int i = 0; i < 8; ++i) {
    dstBuffer.pAsInt[0] = (srcBuffer.pAsInt[0]);
    dstBuffer.pAsInt[1] = (srcBuffer.pAsInt[1]);

    if (hiColor) {
      dstBuffer.pAsInt[2] = (srcBuffer.pAsInt[2]);
      dstBuffer.pAsInt[3] = (srcBuffer.pAsInt[3]);
    }

    dstBuffer.pAsChar += nf_width;
    srcBuffer.pAsChar += nf_width;
  }

  // restore edi
  dstBuffer.pAsChar -= nf_width;
  dstBuffer.pAsChar -= nfpk_back_right;
  edi = dstBuffer.pAsChar;
}

////////////////////////////////////////////////
// Non-Implemented Functions
////////////////////////////////////////////////
void nfHiColorDecomp(const unsigned char *comp, unsigned x, unsigned y, unsigned w, unsigned h);
void nfHiColorDecompChg(const unsigned short *chgs, const unsigned short *parms, const unsigned char *comp, unsigned x,
                        unsigned y, unsigned w, unsigned h);
void nfDecomp(const unsigned char *comp, unsigned x, unsigned y, unsigned w, unsigned h);
void nfDecompChg(const unsigned short *chgs, const unsigned short *parms, const unsigned char *comp, unsigned x,
                 unsigned y, unsigned w, unsigned h);
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


void DECOMP_BODY(bool HI_COLOR_FLAG, const unsigned char *&comp, unsigned int _x, unsigned int _y, unsigned int _w,
                 unsigned int _h) {
  unsigned int HI_COLOR_SCALE = (HI_COLOR_FLAG) ? 2 : 1;

  tNextFrame nf;
  nf.w = _w;
  nf.h = _h;
  nf.x = _x;
  nf.y = _y;
  NF_DECOMP_INIT(HI_COLOR_FLAG ? 1 : 0, &nf);

  unsigned int parms_sz = (nf.w * nf.h * nf_fqty) << 1;
  const unsigned char *compData = comp + parms_sz;

  // New Data
  //=====================

  // Iterate over params and copy new hires data to appropriate sections.
  unsigned char *curr_tbuf = nf.tbuf;
  const unsigned short *compAsWord = (const unsigned short *)comp;
  for (unsigned char fqIt = nf_fqty; fqIt != 0; --fqIt, curr_tbuf += nf_width) {
    unsigned char *this_tbuf = curr_tbuf;
    for (unsigned int ch = nf.h; ch != 0; --ch, this_tbuf += nf.new_row) {
      for (unsigned int cl = nf.w; cl != 0; --cl) {
        int flags = (int)IntelSwapper(*compAsWord++);
        if (flags != 0) {
          this_tbuf += SWIDTH * HI_COLOR_SCALE;
          continue;
        }

        // Copy new data to one section
        // Enter with esi(compData) pointing to source data, edi(this_tbuf) to screen section.
        // Assumes SWIDTH=8 (16-bit data) and SHEIGHT=8
        for (int currH = 0; currH < 8; ++currH) {
          size_t amt = sizeof(unsigned int) * 2 * HI_COLOR_SCALE;
          // memcpy( this_tbuf, compData, amt );
          for (unsigned int ii = 0; ii < (amt / 2); ii++) {
            unsigned short *destword = (unsigned short *)this_tbuf[ii];
            unsigned short *srcword = (unsigned short *)compData[ii];
            *destword = IntelSwapper(*srcword);
          }
          compData += amt;
          this_tbuf += amt + nf_new_line;
        }
        this_tbuf -= nf_new_line;
        this_tbuf -= nf_back_right;
      }
    }
  }

  // Motion Compensation
  //=====================

  // Iterate over flags and motion source addresses from params
  //  to determine which sections to move.
  // ebx indexes params.
  // esi indexes source from buffer
  //   esi will be computed as +- 16K relative to edi.
  compAsWord = (const unsigned short *)comp;
  curr_tbuf = nf.tbuf;
  for (unsigned char fqIt = nf_fqty; fqIt != 0; --fqIt, curr_tbuf += nf_width) {
    unsigned char *this_tbuf = curr_tbuf;
    for (unsigned int ch = nf.h; ch != 0; --ch, this_tbuf += nf.new_row) {
      for (unsigned int cl = nf.w; cl != 0; --cl) {
        int flags = (int)IntelSwapper(*compAsWord++);
        if (flags == 0) {
          this_tbuf += SWIDTH * HI_COLOR_SCALE;
          continue;
        }

        // Make esi absolute
        unsigned char *src = this_tbuf;
        if (flags > 0) {
          // jg
          src += (flags - 0x4000) * HI_COLOR_SCALE;
        } else {
          // jl
          src += (flags - 0xC000) * HI_COLOR_SCALE + nf.DiffBufPtrs;
        }

        for (int i = 0; i < 8; ++i) {
          size_t amt = sizeof(unsigned int) * 2 * HI_COLOR_SCALE;
          // memcpy( this_tbuf, src, amt );
          for (unsigned int ii = 0; ii < (amt / 2); ii++) {
            unsigned short *destword = (unsigned short *)this_tbuf[ii];
            unsigned short *srcword = (unsigned short *)src[ii];
            *destword = IntelSwapper(*srcword);
          }
          src += amt + nf_new_line;
          this_tbuf += amt + nf_new_line;
        }

        this_tbuf -= nf_new_line;
        this_tbuf -= nf_back_right;
      }
    }
  }
}

void DECOMP_CHG_BODY(bool HI_COLOR_FLAG, const unsigned short *&chgs, const unsigned short *&parms,
                     const unsigned char *&comp, unsigned int _x, unsigned int _y, unsigned int _w, unsigned int _h) {
  unsigned int HI_COLOR_SCALE = (HI_COLOR_FLAG) ? 2 : 1;

  tNextFrame nf;
  nf.w = _w;
  nf.h = _h;
  nf.x = _x;
  nf.y = _y;
  NF_DECOMP_INIT(HI_COLOR_FLAG ? 1 : 0, &nf);

  // Iterate over params and copy new hires data to appropriate sections.
  const unsigned short *pChgs = chgs;
  unsigned int eax = 0;
  const unsigned char *compData = comp;
  unsigned char *curr_tbuf = nf.tbuf;
  const unsigned short *curr_parms = parms;
  for (unsigned char fqIt = nf_fqty; fqIt != 0; --fqIt, curr_tbuf += nf_width) {
    unsigned char *this_tbuf = curr_tbuf;
    for (unsigned int ch = nf.h; ch != 0; --ch, this_tbuf += nf.new_row) {
      for (unsigned int cl = nf.w; cl != 0; --cl) {
        eax *= 2;
        while (eax == 0) {
          eax = IntelSwapper(*pChgs++);
          eax *= 2;
        }

        if (((int)eax) > 0) {
          this_tbuf += SWIDTH * HI_COLOR_SCALE;
          continue;
        }

        unsigned short flags = IntelSwapper(*curr_parms++);
        if (flags != 0) {
          this_tbuf += SWIDTH * HI_COLOR_SCALE;
          continue;
        }

        // Copy new data to one section
        // Enter with ds:si pointing to source data, es:di to screen section.
        // Assumes SWIDTH=8 (16-bit data) and SHEIGHT=8
        for (int i = 0; i < 8; ++i) {
          size_t amt = sizeof(unsigned int) * 2 * HI_COLOR_SCALE;
          // TODO: Do these bytes need swapping? Is this data shorts?
          // memcpy( this_tbuf, compData, amt );
          for (unsigned int ii = 0; ii < (amt / 2); ii++) {
            unsigned short *dest = (unsigned short *)this_tbuf[ii];
            unsigned short *src = (unsigned short *)compData[ii];
            *dest = IntelSwapper(*src);
          }
          compData += amt;
          this_tbuf += amt + nf_new_line;
        }
        this_tbuf -= nf_new_line;
        this_tbuf -= nf_back_right;
      }
    }
  }

  // Iterate over flags and motion source addresses from params
  //  to determine which sections to move.
  // ebx indexes params.
  // esi indexes source from buffer
  //   esi will be computed as +- 16K relative to edi.
  curr_tbuf = nf.tbuf;
  curr_parms = parms;
  pChgs = chgs;
  eax = 0;

  for (unsigned char fqIt = nf_fqty; fqIt != 0; --fqIt, curr_tbuf += nf_width) {
    unsigned char *this_tbuf = curr_tbuf;
    for (unsigned int ch = nf.h; ch != 0; --ch, this_tbuf += nf.new_row) {
      for (unsigned int cl = nf.w; cl != 0; --cl) {
        eax *= 2;
        while (eax == 0) {
          eax = IntelSwapper(*pChgs++);
          eax *= 2;
        }

        if (((int)eax) > 0) {
          this_tbuf += SWIDTH * HI_COLOR_SCALE;
          continue;
        }

        int flags = (int)IntelSwapper((*curr_parms++));
        if (flags == 0) {
          this_tbuf += SWIDTH * HI_COLOR_SCALE;
          continue;
        }

        // Make esi absolute
        unsigned char *src = this_tbuf;
        if (flags > 0) {
          src += (flags - 0x4000) * HI_COLOR_SCALE;
        } else {
          src += (flags - 0xC000) * HI_COLOR_SCALE;
          src += nf.DiffBufPtrs; // and point to other buffer
        }

        for (int i = 0; i < 8; ++i) {
          size_t amt = sizeof(unsigned int) * 2 * HI_COLOR_SCALE;
          // memcpy( this_tbuf, src, amt );
          for (unsigned int ii = 0; ii < (amt / 2); ii++) {
            unsigned short *destword = (unsigned short *)this_tbuf[ii];
            unsigned short *srcword = (unsigned short *)src[ii];
            *destword = IntelSwapper(*srcword);
          }
          src += amt + nf_new_line;
          this_tbuf += amt + nf_new_line;
        }
        this_tbuf -= nf_new_line;
        this_tbuf -= nf_back_right; // (SHEIGHT-1)*width
      }
    }
  }
}

// Decompress into subsection of current buffer specified by x,y,w,h in units of SWIDTHxSHEIGHT (8x8).
void nfHiColorDecomp(const unsigned char *comp, unsigned int x, unsigned int y, unsigned int w, unsigned int h) {
  DECOMP_BODY(true, comp, x, y, w, h);
}

// Decompress into subsection of current buffer specified
//  by x,y,w,h in units of SWIDTHxSHEIGHT (8x8).
// Chgs specifies which squares to update.
// Parms are motion parms for squares to update.
void nfHiColorDecompChg(const unsigned short *chgs, const unsigned short *parms, const unsigned char *comp, unsigned x,
                        unsigned y, unsigned w, unsigned h) {
  DECOMP_CHG_BODY(true, chgs, parms, comp, x, y, w, h);
}

// Decompress into subsection of current buffer specified
//  by x,y,w,h in units of SWIDTHxSHEIGHT (8x8).
void nfDecomp(const unsigned char *comp, unsigned x, unsigned y, unsigned w, unsigned h) {
  if (nf_hicolor) {
    nfHiColorDecomp(comp, x, y, w, h);
    return;
  }

  DECOMP_BODY(false, comp, x, y, w, h);
}

// Decompress into subsection of current buffer specified
//  by x,y,w,h in units of SWIDTHxSHEIGHT (8x8).
// Chgs specifies which squares to update.
// Parms are motion parms for squares to update.
void nfDecompChg(const unsigned short *chgs, const unsigned short *parms, const unsigned char *comp, unsigned x,
                 unsigned y, unsigned w, unsigned h) {
  if (nf_hicolor) {
    nfHiColorDecompChg(chgs, parms, comp, x, y, w, h);
    return;
  }

  DECOMP_CHG_BODY(false, chgs, parms, comp, x, y, w, h);
}

void nfPkDecompH(unsigned char *ops, unsigned char *comp, unsigned x, unsigned y, unsigned w, unsigned h) { debug_break(); }
void nfPkDecompD(unsigned char *ops, unsigned char *comp, unsigned x, unsigned y, unsigned w, unsigned h) { debug_break(); }
void mve_ShowFrameField(unsigned char *buf, unsigned bufw, unsigned bufh, unsigned sx, unsigned sy, unsigned w,
                        unsigned h, unsigned dstx, unsigned dsty, unsigned field) {
  debug_break();
}
void mve_ShowFrameFieldHi(unsigned char *buf, unsigned bufw, unsigned bufh, unsigned sx, unsigned sy, unsigned w,
                          unsigned h, unsigned dstx, unsigned dsty, unsigned field) {
  debug_break();
}
void mve_sfShowFrameChg(bool prvbuf, unsigned x, unsigned y, unsigned w, unsigned h, unsigned short *chgs,
                        unsigned dstx, unsigned dsty) {
  debug_break();
}
void mve_sfHiColorShowFrameChg(bool prvbuf, unsigned x, unsigned y, unsigned w, unsigned h, unsigned short *chgs,
                               unsigned dstx, unsigned dsty) {
  debug_break();
}
void mve_sfPkShowFrameChg(bool prvbuf, unsigned x, unsigned y, unsigned w, unsigned h, unsigned char *ops,
                          unsigned dstx, unsigned dsty) {
  debug_break();
}
void mve_sfPkHiColorShowFrameChg(bool prvbuf, unsigned x, unsigned y, unsigned w, unsigned h, unsigned char *ops,
                                 unsigned dstx, unsigned dsty) {
  debug_break();
}
void MVE_SetPalette(unsigned char *p, unsigned start, unsigned count) { debug_break(); }
void palLoadCompPalette(unsigned char *buf) { debug_break(); }
void gfxMode(unsigned mode) { debug_break(); }
void gfxLoadCrtc(unsigned char *crtc, unsigned char chain4, unsigned char res) { debug_break(); }
void gfxGetCrtc(unsigned char *crtc) { debug_break(); }
void gfxVres(unsigned char misc, unsigned char *crtc) { debug_break(); }
void MVE_gfxWaitRetrace(int state) { debug_break(); }
void MVE_gfxSetSplit(unsigned line) { debug_break(); }
