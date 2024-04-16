#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "Aencode.h"

typedef unsigned int uint32;
typedef signed int sint32;
typedef unsigned short uint16;
typedef signed short sint16;
typedef unsigned char uint8;
typedef signed char sint8;

struct BitsEncoder {
  FILE *m_outFile;   // var50 | offset 0x10
  uint32 m_bitData;  // var4C | offset 0x14
  uint32 m_bitCount; // var48 | offset 0x18

  void WriteBits(sint32 val, uint32 numBits) {
    assert((numBits + m_bitCount) <= 32);
    m_bitData |= static_cast<uint32>(val << m_bitCount);
    m_bitCount += numBits;

    while (m_bitCount >= 8) {
      uint8 v = m_bitData & 0xFF;
      putc(v, m_outFile);

      m_bitData >>= 8;
      m_bitCount -= 8;
    }
  }

  void Flush() {
    while (m_bitCount >= 8) {
      uint8 v = m_bitData & 0xFF;
      putc(v, m_outFile);

      m_bitData >>= 8;
      m_bitCount -= 8;
    }

    if (m_bitCount > 0) {
      uint8 v = m_bitData & 0xFF;
      putc(v, m_outFile);
      m_bitCount = 0;
      m_bitData = 0;
    }
  }
};

struct Encoder {
  ReadSampleFunction *m_reader;           // var60         | offset 0x00
  void *m_pReaderData;                    // var5C         | offset 0x04
  uint32 m_sampleCount;                   // var58         | offset 0x08
  float m_volume;                         // var54         | offset 0x0C
  BitsEncoder m_bits;                     // var50 - var48 | offset 0x10 - 0x18
  sint8 m_levels;                         // var44*        | offset 0x1C
  sint8 m_pad[3];                         //   43, 42, 41
  sint32 m_numColumns;                    // var40         | offset 0x20
  sint32 m_samples_per_subband;           // var3C         | offset 0x24
  sint32 m_samplesPerBlock;               // var38         | offset 0x28
  sint32 m_adjustedSamplesTimeNumColumns; // var34         | offset 0x2C
  float **m_levelSlots;                   // var30         | offset 0x30
  float *m_pCurrBlockData;                // var2C         | offset 0x34
  sint32 m_blockSamplesRemaining;         // var28         | offset 0x38
  sint32 m_bandWriteEnabled;              // var24         | offset 0x3C
  sint32 m_finishedReading;               // var20         | offset 0x40
  sint32 m_someVal;                       // var1C         | offset 0x44
  float *m_lo_filter;                     // var18         | offset 0x48
  float *m_hi_filter;                     // var14         | offset 0x4C
  uint32 *m_pFormatIdPerColumn;           // var10         | offset 0x50
  sint32 m_currBlockBitPower;             // var0C         | offset 0x54
  sint32 m_currBlockBitValue;             // var08         | offset 0x58
  sint32 m_threshold;                     // var04         | offset 0x5C
};

typedef void (*WriteBandFunc)(Encoder &enc, sint32 colIndex, uint32 packerId);
void WriteBand_Fmt0(Encoder &enc, sint32 colIndex, uint32 packerId);
void WriteBand_Fmt3_16(Encoder &enc, sint32 colIndex, uint32 packerId);
void WriteBand_Fmt17(Encoder &enc, sint32 colIndex, uint32 packerId);
void WriteBand_Fmt18(Encoder &enc, sint32 colIndex, uint32 packerId);
void WriteBand_Fmt19(Encoder &enc, sint32 colIndex, uint32 packerId);
void WriteBand_Fmt20(Encoder &enc, sint32 colIndex, uint32 packerId);
void WriteBand_Fmt21(Encoder &enc, sint32 colIndex, uint32 packerId);
void WriteBand_Fmt22(Encoder &enc, sint32 colIndex, uint32 packerId);
void WriteBand_Fmt23(Encoder &enc, sint32 colIndex, uint32 packerId);
void WriteBand_Fmt24(Encoder &enc, sint32 colIndex, uint32 packerId);
void WriteBand_Fmt26(Encoder &enc, sint32 colIndex, uint32 packerId);
void WriteBand_Fmt27(Encoder &enc, sint32 colIndex, uint32 packerId);
void WriteBand_Fmt29(Encoder &enc, sint32 colIndex, uint32 packerId);

WriteBandFunc WriteBand_tbl[] = {WriteBand_Fmt0,
                                 NULL,
                                 NULL,
                                 WriteBand_Fmt3_16,
                                 WriteBand_Fmt3_16,
                                 WriteBand_Fmt3_16,
                                 WriteBand_Fmt3_16,
                                 WriteBand_Fmt3_16,
                                 WriteBand_Fmt3_16,
                                 WriteBand_Fmt3_16,
                                 WriteBand_Fmt3_16,
                                 WriteBand_Fmt3_16,
                                 WriteBand_Fmt3_16,
                                 WriteBand_Fmt3_16,
                                 WriteBand_Fmt3_16,
                                 WriteBand_Fmt3_16,
                                 WriteBand_Fmt3_16,
                                 WriteBand_Fmt17,
                                 WriteBand_Fmt18,
                                 WriteBand_Fmt19,
                                 WriteBand_Fmt20,
                                 WriteBand_Fmt21,
                                 WriteBand_Fmt22,
                                 WriteBand_Fmt23,
                                 WriteBand_Fmt24,
                                 NULL,
                                 WriteBand_Fmt26,
                                 WriteBand_Fmt27,
                                 NULL,
                                 WriteBand_Fmt29,
                                 NULL,
                                 NULL};

float std_lo_filter[] = {
    -0.0012475221f, -0.0024950907f, 0.0087309526f, 0.019957958f, -0.050528999f, -0.12055097f, 0.29304558f, 0.70617616f,
};

float std_hi_filter[] = {
    0.0012475221f, -0.0024950907f, -0.0087309526f, 0.019957958f, 0.050528999f, -0.12055097f, -0.29304558f, 0.70617616f,
};

const float T911 = -32767.0f;
const float T913 = 32767.0f;
const float T1266 = 0.0f;

void WriteBand_Fmt0(Encoder &enc, sint32 colIndex, uint32 formatId) {}

void WriteBand_Fmt3_16(Encoder &enc, sint32 colIndex, uint32 formatId) {
  const float currBlockValue = float(enc.m_currBlockBitValue);
  const float halfCurrBlockValue = currBlockValue * 0.5f;
  const sint32 minValue = (sint32)ceilf(-32767.0f / currBlockValue);
  const sint32 maxValue = (sint32)floorf(32767.0f / currBlockValue);

  float *pColumnData = &enc.m_levelSlots[enc.m_levels][colIndex];
  for (sint32 i = 0; i < enc.m_samples_per_subband; ++i) {
    sint32 val = (sint32)floorf((*pColumnData + halfCurrBlockValue) / currBlockValue);
    if (minValue > val) {
      val = minValue;
    } else if (val > maxValue) {
      val = maxValue;
    }

    pColumnData += enc.m_numColumns;
    enc.m_bits.WriteBits(val + (1 << (formatId - 1)), formatId);
  }
}

void WriteBand_Fmt17(Encoder &enc, sint32 colIndex, uint32 formatId) {
  const float currBitValue = static_cast<float>(enc.m_currBlockBitValue);
  const float halfCurrBitValue = currBitValue * 0.5f;
  const sint32 minValue = (sint32)ceilf(-32767.0f / currBitValue);
  const sint32 maxValue = (sint32)floorf(32767.0f / currBitValue);

  const float *pCurrSample = &enc.m_levelSlots[enc.m_levels][colIndex];
  sint32 currSampleIndex = enc.m_samples_per_subband;
  while (currSampleIndex) {
    --currSampleIndex;
    sint32 val = (sint32)floorf((*pCurrSample + halfCurrBitValue) / currBitValue);
    if (minValue > val) {
      val = minValue;
    } else if (maxValue < val) {
      val = maxValue;
    }

    pCurrSample += enc.m_numColumns;
    if (val == 0) {
      if (currSampleIndex != 0 && !(int)floorf((*pCurrSample + halfCurrBitValue) / currBitValue)) {
        enc.m_bits.WriteBits(0, 1);

        if (currSampleIndex == 0)
          return;
        --currSampleIndex;
        pCurrSample += enc.m_numColumns;
        continue;
      }

      enc.m_bits.WriteBits(1, 2);
      continue;
    }

    enc.m_bits.WriteBits(3, 2);
    enc.m_bits.WriteBits((val == 1) ? 1 : 0, 1);
  }
}

void WriteBand_Fmt18(Encoder &enc, sint32 colIndex, uint32 formatId) {
  const float currBlockBitValue = static_cast<float>(enc.m_currBlockBitValue);
  const float halfCurrBlockBitValue = enc.m_currBlockBitValue * 0.5f;
  const sint32 minValue = (sint32)ceilf(-32767.0f / currBlockBitValue);
  const sint32 maxValue = (sint32)floorf(32767.0f / currBlockBitValue);

  const float *pCurrSample = &enc.m_levelSlots[enc.m_levels][colIndex];
  sint32 currSampleIndex = enc.m_samples_per_subband;
  while (currSampleIndex) {
    --currSampleIndex;
    sint32 val = (sint32)floorf((*pCurrSample + halfCurrBlockBitValue) / currBlockBitValue);
    if (minValue > val) {
      val = minValue;
    } else if (val > maxValue) {
      val = maxValue;
    }

    pCurrSample += enc.m_numColumns;

    if (!val) {
      enc.m_bits.WriteBits(0, 1);
      continue;
    }

    enc.m_bits.WriteBits(1, 1);
    enc.m_bits.WriteBits((val == 1) ? 1 : 0, 1);
  }
}

void WriteBand_Fmt19(Encoder &enc, sint32 colIndex, uint32 formatId) {
  const float currBlockBitValue = static_cast<float>(enc.m_currBlockBitValue);
  const float halfCurrBlockBitValue = currBlockBitValue * 0.5f;
  const sint32 minValue = (sint32)ceilf(-32767.0f / currBlockBitValue);
  const sint32 maxValue = (sint32)floorf(32767.0f / currBlockBitValue);

  const float *pCurrSample = &enc.m_levelSlots[enc.m_levels][colIndex];
  sint32 currSampleIndex = enc.m_samples_per_subband;
  while (currSampleIndex) {
    --currSampleIndex;
    sint32 workingVal = (sint32)floorf((*pCurrSample + halfCurrBlockBitValue) / currBlockBitValue);
    if (minValue > workingVal) {
      workingVal = minValue;
    } else if (maxValue < workingVal) {
      workingVal = maxValue;
    }
    pCurrSample += enc.m_numColumns;
    sint32 baseValue = workingVal + 1;
    if (currSampleIndex) {
      --currSampleIndex;
      workingVal = (sint32)floorf((*pCurrSample + halfCurrBlockBitValue) / currBlockBitValue);
      if (minValue > workingVal) {
        workingVal = minValue;
      } else if (maxValue < workingVal) {
        workingVal = maxValue;
      }
      pCurrSample += enc.m_numColumns;
    } else {
      workingVal = 0;
    }

    baseValue += workingVal * 3 + 3;
    if (currSampleIndex) {
      --currSampleIndex;
      workingVal = (sint32)floorf((*pCurrSample + halfCurrBlockBitValue) / currBlockBitValue);
      if (minValue > workingVal) {
        workingVal = minValue;
      } else if (maxValue < workingVal) {
        workingVal = maxValue;
      }
      pCurrSample += enc.m_numColumns;
    } else {
      workingVal = 0;
    }

    enc.m_bits.WriteBits(workingVal * 9 + 9 + baseValue, 5);
  }
}

void WriteBand_Fmt20(Encoder &enc, sint32 colIndex, uint32 formatId) {
  const float currBlockValue = static_cast<float>(enc.m_currBlockBitValue);
  const float halfCurrBlockValue = currBlockValue * 0.5f;
  const sint32 minValue = (sint32)ceilf(-32767.0f / currBlockValue);
  const sint32 maxValue = (sint32)floorf(32767.0f / currBlockValue);

  const float *pCurrSample = &enc.m_levelSlots[enc.m_levels][colIndex];
  sint32 currSampleIndex = enc.m_samples_per_subband;
  while (currSampleIndex) {
    --currSampleIndex;
    sint32 val = (sint32)floorf((*pCurrSample + halfCurrBlockValue) / currBlockValue);
    if (minValue > val) {
      val = minValue;
    } else if (maxValue < val) {
      val = maxValue;
    }

    pCurrSample += enc.m_numColumns;
    if (val == 0) {
      if (currSampleIndex != 0 && !(sint32)floorf((*pCurrSample + halfCurrBlockValue) / currBlockValue)) {
        enc.m_bits.WriteBits(0, 1);

        if (currSampleIndex == 0)
          return;
        --currSampleIndex;
        pCurrSample += enc.m_numColumns;
        continue;
      }
      enc.m_bits.WriteBits(1, 2);
      continue;
    }
    enc.m_bits.WriteBits(3, 2);
    if (val < 0) {
      val += 2;
    } else {
      ++val;
    }

    enc.m_bits.WriteBits(val, 2);
  }
}

void WriteBand_Fmt21(Encoder &enc, sint32 colIndex, uint32 formatId) {
  const float currBlockValue = static_cast<float>(enc.m_currBlockBitValue);
  const float halfCurrBlockValue = currBlockValue * 0.5f;
  const sint32 minValue = (sint32)ceilf(-32767.0f / currBlockValue);
  const sint32 maxValue = (sint32)floorf(32767.0f / currBlockValue);

  const float *pCurrSample = &enc.m_levelSlots[enc.m_levels][colIndex];
  sint32 currSampleIndex = enc.m_samples_per_subband;
  while (currSampleIndex) {
    --currSampleIndex;
    sint32 val = (sint32)floorf((*pCurrSample + halfCurrBlockValue) / currBlockValue);
    if (minValue > val) {
      val = minValue;
    } else if (maxValue < val) {
      val = maxValue;
    }

    pCurrSample += enc.m_numColumns;
    if (!val) {
      enc.m_bits.WriteBits(0, 1);
      continue;
    }
    enc.m_bits.WriteBits(1, 1);

    if (val < 0) {
      val += 2;
    } else {
      ++val;
    }

    enc.m_bits.WriteBits(val, 2);
  }
}

void WriteBand_Fmt22(Encoder &enc, sint32 colIndex, uint32 formatId) {
  const float currBlockValue = static_cast<float>(enc.m_currBlockBitValue);
  const float halfCurrBlockValue = currBlockValue * 0.5f;
  const sint32 minValue = (sint32)ceilf(-32767.0f / currBlockValue);
  const sint32 maxValue = (sint32)floorf(32767.0f / currBlockValue);

  const float *pCurrSample = &enc.m_levelSlots[enc.m_levels][colIndex];

  sint32 currSampleIndex = enc.m_samples_per_subband;
  while (currSampleIndex) {
    --currSampleIndex;
    sint32 workingVal = (sint32)floorf((*pCurrSample + halfCurrBlockValue) / currBlockValue);
    if (minValue > workingVal) {
      workingVal = minValue;
    } else if (maxValue < workingVal) {
      workingVal = maxValue;
    }

    sint32 baseValue = workingVal + 2;
    pCurrSample += enc.m_numColumns;
    if (currSampleIndex) {
      --currSampleIndex;
      workingVal = (sint32)floorf((*pCurrSample + halfCurrBlockValue) / currBlockValue);
      if (minValue > workingVal) {
        workingVal = minValue;
      } else if (maxValue < workingVal) {
        workingVal = maxValue;
      }
      pCurrSample += enc.m_numColumns;
    } else {
      workingVal = 0;
    }

    baseValue += workingVal * 5 + 10;
    if (currSampleIndex) {
      --currSampleIndex;
      workingVal = (sint32)floorf((*pCurrSample + halfCurrBlockValue) / currBlockValue);
      if (minValue > workingVal) {
        workingVal = minValue;
      } else if (maxValue < workingVal) {
        workingVal = maxValue;
      }
      pCurrSample += enc.m_numColumns;
    } else {
      workingVal = 0;
    }

    enc.m_bits.WriteBits(workingVal * 25 + 50 + baseValue, 7);
  }
}

void WriteBand_Fmt23(Encoder &enc, sint32 colIndex, uint32 formatId) {
  const float currBlockValue = static_cast<float>(enc.m_currBlockBitValue);
  const float halfCurrBlockValue = currBlockValue * 0.5f;
  const sint32 minValue = (sint32)ceilf(-32767.0f / currBlockValue);
  const sint32 maxValue = (sint32)floorf(32767.0f / currBlockValue);

  const float *pCurrSample = &enc.m_levelSlots[enc.m_levels][colIndex];
  sint32 currSampleIndex = enc.m_samples_per_subband;
  while (currSampleIndex) {
    --currSampleIndex;

    sint32 val = (sint32)floorf((*pCurrSample + halfCurrBlockValue) / currBlockValue);
    if (minValue > val) {
      val = minValue;
    } else if (maxValue < val) {
      val = maxValue;
    }

    pCurrSample += enc.m_numColumns;
    if (!val) {
      if (currSampleIndex != 0) {
        if (!(sint32)floorf((*pCurrSample + halfCurrBlockValue) / currBlockValue)) {
          enc.m_bits.WriteBits(0, 1);

          if (currSampleIndex == 0)
            return;
          --currSampleIndex;

          pCurrSample += enc.m_numColumns;
          continue;
        }
      }

      enc.m_bits.WriteBits(1, 2);
      continue;
    }

    enc.m_bits.WriteBits(3, 2);

    if (val != -1 && val != 1) {
      enc.m_bits.WriteBits(1, 1);

      if (val < 0) {
        val += 3;
      }

      enc.m_bits.WriteBits(val, 2);
      continue;
    }

    enc.m_bits.WriteBits(0, 1);
    enc.m_bits.WriteBits((val == 1) ? 1 : 0, 1);
  }
}

void WriteBand_Fmt24(Encoder &enc, sint32 colIndex, uint32 formatId) {
  const float currBlockBitValue = static_cast<float>(enc.m_currBlockBitValue);
  const float halfCurrBlockBitValue = currBlockBitValue * 0.5f;
  const sint32 minValue = (sint32)ceilf(-32767.0f / currBlockBitValue);
  const sint32 maxValue = (sint32)floorf(32767.0f / currBlockBitValue);

  const float *pCurrSample = &enc.m_levelSlots[enc.m_levels][colIndex];
  sint32 currSampleIndex = enc.m_samples_per_subband;
  while (currSampleIndex) {
    --currSampleIndex;
    sint32 val = (sint32)floorf((*pCurrSample + halfCurrBlockBitValue) / currBlockBitValue);
    if (minValue > val) {
      val = minValue;
    } else if (maxValue < val) {
      val = maxValue;
    }

    pCurrSample += enc.m_numColumns;
    if (!val) {
      enc.m_bits.WriteBits(0, 1);
      continue;
    }

    enc.m_bits.WriteBits(1, 1);

    if (val != -1 && val != 1) {
      enc.m_bits.WriteBits(1, 1);

      if (val < 0) {
        val += 3;
      }
      enc.m_bits.WriteBits(val, 2);
      continue;
    }

    enc.m_bits.WriteBits(0, 1);
    enc.m_bits.WriteBits((val == 1) ? 1 : 0, 1);
  }
}

void WriteBand_Fmt26(Encoder &enc, sint32 colIndex, uint32 formatId) {
  const float currBlockValue = static_cast<float>(enc.m_currBlockBitValue);
  const float halfCurrBlockValue = currBlockValue * 0.5f;
  const sint32 minValue = static_cast<sint32>(ceilf(-32767.0f / currBlockValue));
  const sint32 maxValue = static_cast<sint32>(floorf(32767.0f / currBlockValue));
  const float *pColumnData = &enc.m_levelSlots[enc.m_levels][colIndex];

  sint32 currSampleIdx = enc.m_samples_per_subband;
  while (currSampleIdx) {
    --currSampleIdx;

    sint32 val = (sint32)floorf((*pColumnData + halfCurrBlockValue) / currBlockValue);
    if (minValue > val) {
      val = minValue;
    } else if (maxValue < val) {
      val = maxValue;
    }

    pColumnData += enc.m_numColumns;
    if (!val) {
      if (currSampleIdx) {
        sint32 testVal = (sint32)floorf((*pColumnData + halfCurrBlockValue) / currBlockValue);
        if (!testVal) {
          enc.m_bits.WriteBits(0, 1);

          if (currSampleIdx == 0)
            return;

          --currSampleIdx;
          pColumnData += enc.m_numColumns;
          continue;
        }
      }

      enc.m_bits.WriteBits(1, 2);
      continue;
    }

    enc.m_bits.WriteBits(3, 2);

    if (val >= 0) {
      val += 3;
    } else {
      val += 4;
    }

    enc.m_bits.WriteBits(val, 3);
  }
}

void WriteBand_Fmt27(Encoder &enc, sint32 colIndex, uint32 formatId) {
  const float currBlockValue = static_cast<float>(enc.m_currBlockBitValue);
  const float halfCurrBlockValue = currBlockValue * 0.5f;
  const sint32 minValue = (sint32)ceilf(-32767.0f / currBlockValue);
  const sint32 maxValue = (sint32)floorf(32767.0f / currBlockValue);

  const float *pCurrSample = &enc.m_levelSlots[enc.m_levels][colIndex];
  sint32 currSampleIndex = enc.m_samples_per_subband;
  while (currSampleIndex) {
    --currSampleIndex;
    sint32 val = (sint32)floorf((*pCurrSample + halfCurrBlockValue) / currBlockValue);
    if (val < minValue) {
      val = minValue;
    } else if (maxValue < val) {
      val = maxValue;
    }

    pCurrSample += enc.m_numColumns;
    if (!val) {
      enc.m_bits.WriteBits(0, 1);
      continue;
    }

    enc.m_bits.WriteBits(1, 1);

    if (val < 0) {
      val += 4;
    } else {
      val += 3;
    }

    enc.m_bits.WriteBits(val, 3);
  }
}

void WriteBand_Fmt29(Encoder &enc, sint32 colIndex, uint32 formatId) {
  const float currBitValue = (float)enc.m_currBlockBitValue;
  const float halfCurrBitValue = currBitValue * 0.5f;
  const sint32 minValue = (sint32)ceilf(-32767.0f / currBitValue);
  const sint32 maxValue = (sint32)floorf(32767.0f / currBitValue);

  const float *pColumnData = &enc.m_levelSlots[enc.m_levels][colIndex];
  sint32 currSampleIndex = enc.m_samples_per_subband;
  while (currSampleIndex) {
    --currSampleIndex;

    sint32 val = (sint32)floorf((*pColumnData + halfCurrBitValue) / currBitValue);
    if (minValue > val) {
      val = minValue;
    } else if (maxValue < val) {
      val = maxValue;
    }

    sint32 baseValue = val + 5;
    pColumnData += enc.m_numColumns;
    if (currSampleIndex != 0) {
      --currSampleIndex;

      val = (sint32)floorf((*pColumnData + halfCurrBitValue) / currBitValue);
      if (minValue > val) {
        val = minValue;
      } else if (maxValue < val) {
        val = maxValue;
      }

      pColumnData += enc.m_numColumns;
    } else {
      val = 0;
    }

    enc.m_bits.WriteBits(11 * val + 55 + baseValue, 7);
  }
}

int ReadSample_init(Encoder &enc, ReadSampleFunction *read, void *data) {
  enc.m_reader = read;
  enc.m_pReaderData = data;
  return 1;
}

int bits_init(BitsEncoder &bits, FILE *out) {
  bits.m_outFile = out;
  bits.m_bitData = 0;
  bits.m_bitCount = 0;
  return 1;
}

int SetupEncoder(Encoder &enc, int someVal, float std_lo_filter[], float std_hi_filter[], sint8 levels,
                 int samples_per_subband) {
  enc.m_someVal = someVal;
  enc.m_lo_filter = std_lo_filter;
  enc.m_hi_filter = std_hi_filter;
  enc.m_levels = levels;
  enc.m_numColumns = 1 << levels;
  enc.m_samples_per_subband = samples_per_subband;
  enc.m_samplesPerBlock = samples_per_subband * enc.m_numColumns;

  int adjustedSomeVal = (((someVal < -1) ? (someVal + 1) : (someVal)) + 1) >> 1;
  enc.m_adjustedSamplesTimeNumColumns = adjustedSomeVal * (enc.m_numColumns - 1);
  enc.m_levelSlots = reinterpret_cast<float **>(malloc(sizeof(float *) * (levels + 1)));
  if (enc.m_levelSlots == NULL)
    return 0;

  if (levels >= 0) {
    for (sint8 i = 0; i <= levels; ++i) {
      int extraSamples = 0;
      if (i != levels) {
        extraSamples = (someVal - 1) << i;
      }

      float *blockData = reinterpret_cast<float *>(malloc((enc.m_samplesPerBlock + extraSamples) * sizeof(float)));
      enc.m_levelSlots[i] = blockData;
      if (blockData == NULL)
        return 0;

      memset(blockData, 0, (enc.m_samplesPerBlock + extraSamples) * sizeof(float));
      enc.m_levelSlots[i] += extraSamples;
    }
  }

  enc.m_pFormatIdPerColumn = reinterpret_cast<uint32 *>(malloc(enc.m_numColumns * sizeof(uint32)));
  if (enc.m_pFormatIdPerColumn == NULL)
    return 0;

  enc.m_sampleCount = 0;

  sint32 edxOffset =
      ((enc.m_samplesPerBlock * sizeof(float) * 25) - enc.m_adjustedSamplesTimeNumColumns) % enc.m_samplesPerBlock;
  enc.m_pCurrBlockData = enc.m_levelSlots[0] + edxOffset;
  enc.m_blockSamplesRemaining = enc.m_samplesPerBlock - edxOffset;
  enc.m_bandWriteEnabled = 0;
  enc.m_finishedReading = 0;
  return 1;
}

void DestroyEncoder(Encoder &enc) {
  if (enc.m_levelSlots != NULL) {
    for (int i = 0; i <= enc.m_levels; ++i) {
      if (enc.m_levelSlots[i] != 0) {
        int extraSamples = 0;
        if (enc.m_levels != i) {
          extraSamples = (enc.m_someVal - 1) << i;
        }

        free(enc.m_levelSlots[i] - extraSamples);
      }
    }

    free(enc.m_levelSlots);
  }

  if (enc.m_pFormatIdPerColumn != NULL) {
    free(enc.m_pFormatIdPerColumn);
  }
}

void transform_subband(Encoder &enc, float *pD0, float *pD1, sint32 subBandCount, sint32 sampleCount) {
  if (sampleCount <= 0)
    return;

  const sint32 var_8 = (enc.m_someVal - 1) >> 1;
  const sint32 edx = var_8 * subBandCount;
  pD0 -= edx;

  for (int i = 0; i < sampleCount; ++i) {
    float *pFilter = (i & 1) ? enc.m_hi_filter : enc.m_lo_filter;
    float *ebx = pD0 - edx;
    float *eax = pD0 + edx;

    float var_4 = 0.0f;
    if (var_8 > 0) {
      for (sint32 ebp = var_8; ebp != 0; --ebp) {
        var_4 += (*eax + *ebx) * *pFilter++;
        ebx += subBandCount;
        eax -= subBandCount;
      }
    }

    *pD1 = (*ebx * *pFilter) + var_4;
    pD1 += subBandCount;
    pD0 += subBandCount;
  }
}

void transform_all(Encoder &enc) {
  if (enc.m_levels <= 0)
    return;

  sint32 subBandCount = 1;
  sint32 sampleCount = enc.m_samplesPerBlock;
  for (int i = 0; i < enc.m_levels; ++i) {
    float *levelDataEBX = enc.m_levelSlots[i];
    float *levelDataEBP = enc.m_levelSlots[i + 1];

    for (int sbc = 0; sbc < subBandCount; ++sbc) {
      transform_subband(enc, levelDataEBX++, levelDataEBP++, subBandCount, sampleCount);
    }

    subBandCount += subBandCount;
    sampleCount >>= 1;
  }
}

sint32 calc_bits(Encoder &enc, sint32 val) {
  static uint32 calc_bits_data[] = {0x00, 0x13, 0x16, 0x03, 0x1D, 0x00};

  sint32 bitPower = 3;
  sint32 result = enc.m_numColumns * 5 + 20;

  float halfVal = float(val) * 0.5f;
  float *var_18 = enc.m_levelSlots[enc.m_levels];
  float *var_8 = enc.m_levelSlots[enc.m_levels];

  for (sint32 var_1C = 0; var_1C < enc.m_numColumns; ++var_1C) {
    sint32 minValue = 0x10000;
    sint32 maxValue = -0x10000;
    if (enc.m_samples_per_subband > 0) {
      float *pSlotData = var_8;
      for (int ebp = enc.m_samples_per_subband; ebp != 0; --ebp) {
        sint32 testVal = (sint32)floor((*pSlotData + halfVal) / float(val));
        if (minValue > testVal) {
          minValue = testVal;
        }

        if (maxValue < testVal) {
          maxValue = testVal;
        }

        pSlotData += enc.m_numColumns;
      }
    }

    sint32 absMaxVal = abs(minValue);
    if (absMaxVal < maxValue) {
      absMaxVal = maxValue;
    } else if (absMaxVal < -maxValue) {
      absMaxVal = -maxValue;
    }

    if (absMaxVal == 0) {
      minValue = 0;
      enc.m_pFormatIdPerColumn[var_1C] = 0;
    } else if (absMaxVal <= 4) {
      sint32 ebx = 1;
      sint32 var_28 = absMaxVal * 3 + 14;
      if (absMaxVal != 1) {
        ebx = ((absMaxVal - 2) < 1) ? 2 : 3;
      }

      minValue = 0;
      maxValue = 0;
      for (int ebp = 0; ebp < enc.m_samples_per_subband; ++ebp) {
        sint32 v = (int)floor((var_18[(ebp * enc.m_numColumns) + var_1C] + halfVal) / float(val));
        if (v) {
          if (ebx != 1) {
            if (v == -1 || v == 1) {
              minValue += 4;
              maxValue += 3;
            } else {
              minValue += ebx + 2;
              maxValue += ebx + 1;
            }
          } else {
            minValue += 3;
            maxValue += 2;
          }
        } else if ((enc.m_samples_per_subband - 1) <= ebp) {
          minValue += 2;
          ++maxValue;
        } else {
          sint32 v = (int)floor((var_18[((ebp + 1) * enc.m_numColumns) + var_1C] + halfVal) / float(val));
          if (v) {
            minValue += 2;
            ++maxValue;
          } else {
            ++minValue;
            maxValue += 2;
            ++ebp;
          }
        }
      }

      if (minValue > maxValue) {
        minValue = maxValue;
        ++var_28;
      }

      sint32 ecx;
      if (absMaxVal != 4) {
        ecx = ((enc.m_samples_per_subband + 2) / 3) * ((absMaxVal * 2) + 3);
      } else {
        ecx = (((enc.m_samples_per_subband < -1) ? (enc.m_samples_per_subband + 2) : (enc.m_samples_per_subband + 1)) >>
               1) *
              7;
      }

      if (minValue > ecx) {
        minValue = ecx;
        var_28 = calc_bits_data[absMaxVal];
      }

      enc.m_pFormatIdPerColumn[var_1C] = var_28;
    } else if (minValue >= -5 && maxValue <= 5) {
      minValue =
          (((enc.m_samples_per_subband < -1) ? (enc.m_samples_per_subband + 2) : (enc.m_samples_per_subband + 1)) >>
           1) *
          7;
      enc.m_pFormatIdPerColumn[var_1C] = 0x1D;
    } else {
      sint32 eax = 0;
      if (minValue < 0) {
        eax = ~minValue;
      }

      if (maxValue > 0 && ((unsigned int)(eax) < (unsigned int)(maxValue))) {
        eax = maxValue;
      }

      minValue = 1;
      while (eax) {
        eax >>= 1;
        ++minValue;
      }

      if (bitPower < (minValue - 1)) {
        bitPower = minValue - 1;
      }
      enc.m_pFormatIdPerColumn[var_1C] = minValue;
      minValue *= enc.m_samples_per_subband;
    }

    result += minValue;
    ++var_8;
  }

  enc.m_currBlockBitPower = bitPower;
  enc.m_currBlockBitValue = val;
  return result;
}

void DetermineStep(Encoder &enc) {
  sint32 lo = 1;
  sint32 hi = 0x7FFF;

  do {
    const sint32 midPoint = (lo + hi) >> 1;
    sint32 errorAmt = calc_bits(enc, midPoint);
    if (enc.m_threshold < errorAmt) {
      lo = midPoint + 1;
    } else {
      hi = midPoint - 1;
    }
  } while (hi >= lo);

  if (enc.m_currBlockBitValue != lo) {
    calc_bits(enc, lo);
  }
}

void WriteBands(Encoder &enc) {
  enc.m_bits.WriteBits(enc.m_currBlockBitPower, 4);
  enc.m_bits.WriteBits(enc.m_currBlockBitValue, 16);

  for (int i = 0; i < enc.m_numColumns; ++i) {
    const uint32 formatId = enc.m_pFormatIdPerColumn[i];
    enc.m_bits.WriteBits(formatId, 5);
    int currPos = ftell(enc.m_bits.m_outFile);
    WriteBand_tbl[formatId](enc, i, formatId);
  }
}

void shift_transform_levels(Encoder &enc) {
  sint32 levelCount = enc.m_someVal - 1;
  for (int i = 0; i < enc.m_levels; ++i, levelCount += levelCount) {
    float *pDst = enc.m_levelSlots[i] - levelCount;
    float *pSrc = pDst + enc.m_samplesPerBlock;
    memcpy(pDst, pSrc, levelCount * sizeof(float));
  }
}

void ProcessBlock(Encoder &enc) {
  transform_all(enc);

  if (enc.m_bandWriteEnabled != 0) {
    DetermineStep(enc);

    WriteBands(enc);
  }

  shift_transform_levels(enc);

  enc.m_blockSamplesRemaining += enc.m_samplesPerBlock;
  enc.m_pCurrBlockData -= enc.m_samplesPerBlock;
}

void EncodeSample(Encoder &enc) {
  sint32 sample = 0;
  if (enc.m_finishedReading == 0) {
    sample = (*enc.m_reader)(enc.m_pReaderData);
    if (sample == ReadSampleEof) {
      enc.m_finishedReading = 1;
      return;
    }

    ++enc.m_sampleCount;
  }

  *enc.m_pCurrBlockData++ = enc.m_volume * float(sample);

  if (--enc.m_blockSamplesRemaining == 0) {
    ProcessBlock(enc);
  }
}

void EncodeFlush(Encoder &enc) {
  if (enc.m_samplesPerBlock == enc.m_blockSamplesRemaining) {
    // no data in the block
    return;
  }

  // Zero out the remaining data in the block
  while (enc.m_blockSamplesRemaining != 0) {
    *enc.m_pCurrBlockData++ = 0.0f;
    --enc.m_blockSamplesRemaining;
  }

  // Send it off for processing
  ProcessBlock(enc);
}

unsigned long AudioEncode(ReadSampleFunction *read, void *data, unsigned channels, unsigned sample_rate, float volume,
                          FILE *out, int levels, int samples_per_subband, float comp_ratio) {
  Encoder enc;
  memset(&enc, 0, sizeof(enc));

  if (!ReadSample_init(enc, read, data)) {
    DestroyEncoder(enc);
    return 0;
  }

  if (!bits_init(enc.m_bits, out)) {
    DestroyEncoder(enc);
    return 0;
  }

  enc.m_volume = volume;
  if (!SetupEncoder(enc, 0xF, std_lo_filter, std_hi_filter, levels, samples_per_subband)) {
    DestroyEncoder(enc);
    return 0;
  }

  enc.m_threshold = (sint32)(float(enc.m_samplesPerBlock) * comp_ratio * 16.0f);

  int originalPosVAR64 = ftell(out);

  // Header
  enc.m_bits.WriteBits(0x97, 8);
  enc.m_bits.WriteBits(0x28, 8);
  enc.m_bits.WriteBits(0x03, 8);

  // Version
  enc.m_bits.WriteBits(1, 8);

  // Sample Count (Placeholder 32bits for now)
  enc.m_bits.WriteBits(0, 8);
  enc.m_bits.WriteBits(0, 8);
  enc.m_bits.WriteBits(0, 8);
  enc.m_bits.WriteBits(0, 8);

  // Number of channels
  enc.m_bits.WriteBits(channels, 16);

  // Sample Rate
  enc.m_bits.WriteBits(sample_rate, 16);

  // Levels
  enc.m_bits.WriteBits(levels, 4);

  // Samples per Sub-band (rows)
  enc.m_bits.WriteBits(samples_per_subband, 12);

  enc.m_bandWriteEnabled = 0;

  sint32 esi = enc.m_adjustedSamplesTimeNumColumns;
  while (esi) {
    EncodeSample(enc);
    --esi;
  }

  enc.m_bandWriteEnabled = 1;

  while (!enc.m_finishedReading) {
    EncodeSample(enc);
  }

  esi = enc.m_adjustedSamplesTimeNumColumns;
  while (esi) {
    EncodeSample(enc);
    --esi;
  }

  EncodeFlush(enc);

  /////////////
  // NOTE: The Interplay one doesn't do this ... but it should as there
  // may be bits left in the bit processor that should go out
  enc.m_bits.Flush();
  /////////////

  // Go back and write the Sample Count out proper
  int endPos = ftell(out);
  fseek(out, originalPosVAR64 + 4, 0);
  putc((enc.m_sampleCount >> 0) & 0xFF, out);
  putc((enc.m_sampleCount >> 8) & 0xFF, out);
  putc((enc.m_sampleCount >> 16) & 0xFF, out);
  putc((enc.m_sampleCount >> 24) & 0xFF, out);
  fseek(out, endPos, 0);

  DestroyEncoder(enc);
  return endPos;
}
