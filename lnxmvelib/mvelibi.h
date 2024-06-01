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

#if __SC__
#pragma SC align 1
#elif __WATCOMC__
#pragma pack(1);
#elif _MSC_VER
#pragma pack(1)
#else
//???
#pragma pack(push, 1)
#endif

#include "byteswap.h"

//--------------------------------
// Useful type definitions
//--------------------------------

#ifndef __cplusplus
typedef unsigned bool;
#endif
#define FALSE 0
#define TRUE 1

//--------------------------------
// 386 Assembler Support
//--------------------------------

#define OPERAND32 __asm _emit 0x66
#define SEG_ES __asm _emit 0x26
#define SEG_FS __asm _emit 0x64
#define SEG_GS __asm _emit 0x65
#define MOVZX_BP_IND_BX __asm _emit 0x0F __asm _emit 0xB6 __asm _emit 0x2F
#define MOV_FS_AX __asm _emit 0x8e __asm _emit 0xE0
#define MOV_GS_AX __asm _emit 0x8e __asm _emit 0xE8

//--------------------------------
// Compressed Video Constants
//--------------------------------

// Width and height of sections in pixels.
#define SWIDTH 8
#define SHEIGHT 8

#define LOG2_SWIDTH 3
#define LOG2_SHEIGHT 3

//------------------------------
// Movie File Header
//------------------------------

#define MVE_FILE_TYPE "Interplay MVE File\x1A\0"
#define MVE_FILE_VERSION 0x0100

struct mve_hdr {
  char FileType[20];      // MVE_FILE_TYPE
  uint16_t HdrSize; // sizeof(mve_hdr)
  uint16_t version; // MVE_FILE_VERSION
  uint16_t id;      // ~MVE_FILE_VERSION+0x1234
};

//------------------------------
// Movie File Records
//------------------------------

struct ioHdrRec {
  uint16_t len;  // Length of record data (pad to even)
  uint16_t kind; // See IO_REC_xxx
                       //	uint8_t data[0];	// Record data
};

// This record classifications simplify utilities which must operate on
// records.  They are not used by this library when running a movie.
//
#define IO_REC_SND_INIT 0    // Sound setup
#define IO_REC_SND_PRELOAD 1 // Sound preload
#define IO_REC_FRAME_INIT 2  // Frame (video) setup
#define IO_REC_FRAME 3       // Movie frames
#define IO_REC_END 4         // Last Record (End of Movie)
#define IO_REC_EOF 5         // Empty eof record at end of file.

//------------------------------
// Movie File Major Opcodes
//------------------------------
//

#define MCMD_DATA(arg) ((uint8_t *)((arg) + 1))

struct mcmd_hdr {
  uint16_t len;  // Length of data (pad to even)
  uint8_t major; // Major opcode
  uint8_t minor; // Minor opcode
                       //	uint8_t data[0];	// Opcode data
};

#define mcmd_end 0 // End processing of movie

#define mcmd_next 1 // Advance to next movie record

#define mcmd_syncInit 2
struct marg_syncInit {
  uint32_t period;       // period of quanta
  uint16_t wait_quanta; // # of quanta per frame
};

#define mcmd_sndConfigure 3
struct marg_sndConfigure {
  uint16_t rate; // 65536-(256E6/(frequency*(stereo+1)))
                       // comp16 is a minor opcode 1 field
                       //  It indicates that 16-bit data has been compressed to 8-bits.
                       //  When it is set, bits16 will also be set.
                       //  Each record will contain initial 16-bit sample followed
                       //   by remaining compressed 8-bit samples.
                       //  For stereo, there will be two initial 16-bit samples.
                       //   and compressed streams will be interleaved.
                       // uint16_t stereo:1, bits16:1, comp16:1;
#ifdef OUTRAGE_BIG_ENDIAN
  uint8_t bitpadder : 5;
#endif
  uint8_t stereo : 1, bits16 : 1, comp16 : 1;
  uint8_t dummy1;
  uint16_t frequency;
  // Minor opcode 1 extends buflen to be a long
  uint32_t buflen;
};

#define mcmd_sndSync 4

#define mcmd_nfConfig 5
struct marg_nfConfig {
  uint16_t wqty;
  uint16_t hqty;
  // Minor opcode 1 fields:
  uint16_t fqty;
  // Minor opcode 2 fields:
  uint16_t hicolor; /*0=256-color, 1=HiColor, 2=HiColorSwapped*/
};

#define mcmd_nfDecomp 6
#define mcmd_nfDecompChg 16
#define mcmd_nfPkDecomp 17
struct marg_nfDecomp {
  uint16_t prev;   // info:Prev frames+1 needed for full picture
  uint16_t iframe; // info:Current internal frame #
  uint16_t x;
  uint16_t y;
  uint16_t w;
  uint16_t h;
  // uint16_t advance:1;
#ifdef OUTRAGE_BIG_ENDIAN
  uint8_t bitpadder : 7;
#endif
  uint8_t advance : 1;
  uint8_t dummy1;
  //	uint8_t comp[0];
};

#define mcmd_sfShowFrame 7
#if 0 // Not supported
#define mcmd_sfPkShowFrameChg 18
#endif
struct marg_sfShowFrame {
  uint16_t pal_start;
  uint16_t pal_count;
  // Minor opcode 1 fields:
  uint16_t field; // 0:none, 2:send to even, 3:send to odd
};

#define mcmd_sndAdd 8
#define mcmd_sndSilence 9
struct marg_sndAdd {
  uint16_t iframe; // info: iframe # of sound
  uint16_t TrackMask;
  uint16_t qty; // Uncompressed audio size in bytes
  //	uint8_t data[0];
};

#define mcmd_gfxMode 10
struct marg_gfxMode {
  uint16_t minw;
  uint16_t minh;
  uint16_t mode;
};

#define mcmd_palMakeSynthPalette 11
struct marg_palLoadPalette {
  uint8_t base_r;
  uint8_t range_r;
  uint8_t range_rb;
  uint8_t base_g;
  uint8_t range_g;
  uint8_t range_gb;
};

#define mcmd_palLoadPalette 12
struct marg_palLoadPalette {
  uint16_t start;
  uint16_t count;
  //	uint8_t data[0];
};

#define mcmd_palLoadCompPalette 13

#define mcmd_nfChanges 14
#define mcmd_nfParms 15
// 16 is used for mcmd_nfDecompChg, see above.
// 17 is used for mcmd_nfPkDecomp, see above.
// 18 is used for mcmd_nfPkShowFrameChg, see above

#define mcmd_nfPkInfo 19
#define mcmd_nfHPkInfo 20
struct marg_nfPkInfo {
  uint32_t error; // scaled by 10000
  uint16_t usage[64];
};

#define mcmd_idcode 21
struct marg_idcode {
  uint32_t idcode; // Code identifying version mcomp used to create
};

#if __SC__
#pragma SC align
#elif __WATCOMC__
#pragma pack();
#elif _MSC_VER
#pragma pack()
#else
//???
#pragma pack(pop)
#endif
