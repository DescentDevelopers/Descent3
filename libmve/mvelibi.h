#ifndef MVELIBI_H__
#define MVELIBI_H__

#if __SC__
#pragma SC align 1
#elif __WATCOMC__
#pragma pack(1);
#elif _MSC_VER
#pragma pack(1)
#elif __LINUX__
#pragma pack(1)
#else
#error No platform defined
#endif

#include "byteswap.h"
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

// some inlines to prevent macro craziness when using incrementers and dereferencing, and so I can use operator
// overloading
inline unsigned short IntelSwapper(unsigned short a) { return INTEL_SHORT(a); }

inline short IntelSwapper(short a) { return INTEL_SHORT(a); }

inline unsigned int IntelSwapper(unsigned int a) { return INTEL_INT(a); }

inline int IntelSwapper(int a) { return INTEL_INT(a); }

inline unsigned long IntelSwapper(unsigned long a) { return INTEL_INT(a); }

inline long IntelSwapper(long a) { return INTEL_INT(a); }

typedef struct _mve_hdr {
  char FileType[20];      // MVE_FILE_TYPE
  unsigned short HdrSize; // sizeof(mve_hdr)
  unsigned short version; // MVE_FILE_VERSION
  unsigned short id;      // ~MVE_FILE_VERSION+0x1234
  void SwapBytes() {
    HdrSize = IntelSwapper(HdrSize);
    version = IntelSwapper(version);
    id = IntelSwapper(id);
  }
} mve_hdr;

//------------------------------
// Movie File Records
//------------------------------

typedef struct _io_hdr {
  unsigned short len;  // Length of record data (pad to even)
  unsigned short kind; // See IO_REC_xxx
                       //	unsigned char data[0];	// Record data
  void SwapBytes() {
    len = IntelSwapper(len);
    kind = IntelSwapper(kind);
  }
} ioHdrRec;

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

#define MCMD_DATA(arg) ((unsigned char *)((arg) + 1))

typedef struct _mcmd_hdr {
  unsigned short len;  // Length of data (pad to even)
  unsigned char major; // Major opcode
  unsigned char minor; // Minor opcode
                       //	unsigned char data[0];	// Opcode data
  void SwapBytes() { len = IntelSwapper(len); }
} mcmd_hdr;

#define mcmd_end 0 // End processing of movie

#define mcmd_next 1 // Advance to next movie record

#define mcmd_syncInit 2
typedef struct _syncInit {
  unsigned long period;       // period of quanta
  unsigned short wait_quanta; // # of quanta per frame
  void SwapBytes() {
    period = IntelSwapper(period);
    wait_quanta = IntelSwapper(wait_quanta);
  }
} marg_syncInit;

#define mcmd_sndConfigure 3
typedef struct _sndConfigure {
  unsigned short rate; // 65536-(256E6/(frequency*(stereo+1)))
                       // comp16 is a minor opcode 1 field
                       //  It indicates that 16-bit data has been compressed to 8-bits.
                       //  When it is set, bits16 will also be set.
                       //  Each record will contain initial 16-bit sample followed
                       //   by remaining compressed 8-bit samples.
                       //  For stereo, there will be two initial 16-bit samples.
                       //   and compressed streams will be interleaved.
#ifdef OUTRAGE_BIG_ENDIAN
  unsigned char pad : 5;
  unsigned char stereo : 1, bits16 : 1, comp16 : 1;
  unsigned char pad2;
#else
  unsigned char stereo : 1, bits16 : 1, comp16 : 1;
  unsigned char pad;
#endif
  unsigned short frequency;
  // Minor opcode 1 extends buflen to be a long
  unsigned long buflen;
  void SwapBytes() {
    rate = IntelSwapper(rate);
    frequency = IntelSwapper(frequency);
    buflen = IntelSwapper(buflen);
  }
} marg_sndConfigure;

#define mcmd_sndSync 4

#define mcmd_nfConfig 5
typedef struct _nfConfig {
  unsigned short wqty;
  unsigned short hqty;
  // Minor opcode 1 fields:
  unsigned short fqty;
  // Minor opcode 2 fields:
  unsigned short hicolor; /*0=256-color, 1=HiColor, 2=HiColorSwapped*/
  void SwapBytes() {
    wqty = IntelSwapper(wqty);
    hqty = IntelSwapper(hqty);
    fqty = IntelSwapper(fqty);
    hicolor = IntelSwapper(hicolor);
  }
} marg_nfConfig;

#define mcmd_nfDecomp 6
#define mcmd_nfDecompChg 16
#define mcmd_nfPkDecomp 17
typedef struct _nfDecomp {
  unsigned short prev;   // info:Prev frames+1 needed for full picture
  unsigned short iframe; // info:Current internal frame #
  unsigned short x;
  unsigned short y;
  unsigned short w;
  unsigned short h;
#ifdef OUTRAGE_BIG_ENDIAN
  unsigned char bitpadder : 7;
  unsigned char advance : 1;
  unsigned char dummy1;
#else
  unsigned short advance : 1;
  unsigned short pad : 15;
#endif

  void SwapBytes() {
    prev = IntelSwapper(prev);
    iframe = IntelSwapper(iframe);
    x = IntelSwapper(x);
    y = IntelSwapper(y);
    w = IntelSwapper(w);
    h = IntelSwapper(h);
  }
} marg_nfDecomp;

#define mcmd_sfShowFrame 7
#if 0 // Not supported
#define mcmd_sfPkShowFrameChg 18
#endif
typedef struct _sfShowFrame {
  unsigned short pal_start;
  unsigned short pal_count;
  // Minor opcode 1 fields:
  unsigned short field; // 0:none, 2:send to even, 3:send to odd
  void SwapBytes() {
    pal_start = IntelSwapper(pal_start);
    pal_count = IntelSwapper(pal_count);
    field = IntelSwapper(field);
  }
} marg_sfShowFrame;

#define mcmd_sndAdd 8
#define mcmd_sndSilence 9
typedef struct _sndAdd {
  unsigned short iframe; // info: iframe # of sound
  unsigned short TrackMask;
  unsigned short qty; // Uncompressed audio size in bytes
  //	unsigned char data[0];
  void SwapBytes() {
    iframe = IntelSwapper(iframe);
    TrackMask = IntelSwapper(TrackMask);
    qty = IntelSwapper(qty);
  }
} marg_sndAdd;

#define mcmd_gfxMode 10
typedef struct _gfxMode {
  unsigned short minw;
  unsigned short minh;
  unsigned short mode;
  void SwapBytes() {
    minw = IntelSwapper(minw);
    minh = IntelSwapper(minh);
    mode = IntelSwapper(mode);
  }
} marg_gfxMode;

#define mcmd_palMakeSynthPalette 11
typedef struct _palMakeSynthPalette {
  unsigned char base_r;
  unsigned char range_r;
  unsigned char range_rb;
  unsigned char base_g;
  unsigned char range_g;
  unsigned char range_gb;
  void SwapBytes() {}
} marg_palMakeSynthPalette;

#define mcmd_palLoadPalette 12
typedef struct _palLoadPalette {
  unsigned short start;
  unsigned short count;
  //	unsigned char data[0];
  void SwapBytes() {
    start = IntelSwapper(start);
    count = IntelSwapper(count);
  }
} marg_palLoadPalette;

#define mcmd_palLoadCompPalette 13

#define mcmd_nfChanges 14
#define mcmd_nfParms 15
// 16 is used for mcmd_nfDecompChg, see above.
// 17 is used for mcmd_nfPkDecomp, see above.
// 18 is used for mcmd_nfPkShowFrameChg, see above

#define mcmd_nfPkInfo 19
#define mcmd_nfHPkInfo 20
typedef struct _nfPkInfo {
  unsigned long error; // scaled by 10000
  unsigned short usage[64];
} marg_nfPkInfo;

#define mcmd_idcode 21
typedef struct _idcode {
  unsigned long idcode; // Code identifying version mcomp used to create
} marg_idcode;

#if __SC__
#pragma SC align
#elif __WATCOMC__
#pragma pack();
#elif _MSC_VER
#pragma pack()
#elif __LINUX__
#pragma pack()
#else
#error No platform defined
#endif

#endif
