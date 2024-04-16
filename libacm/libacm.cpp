// Interplay ACM audio codec decoder
//
// Based on code from the decoder source of libacm, licensed under a minimal BSD/ISC license:
// Copyright (c) 2004-2008, Marko Kreen
//
// Permission to use, copy, modify, and distribute this software for any
// purpose with or without fee is hereby granted, provided that the above
// copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
// WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
// ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
// ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
// OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

#include "Adecode.h"
#include <stdlib.h>
#ifdef MACOSX
#include <sys/malloc.h>
#else
#include <malloc.h>
#endif
#include <cstring>
#include "pserror.h"
#include "BYTESWAP.H"
using namespace AudioDecoder;

namespace {
// default memory allocation function
void *DefaultMalloc(uint32 size) { return malloc(size); }

// default memory release function
void DefaultFree(void *pPtr) { free(pPtr); }

// Constants
const uint32 kBitBufferSize = 64 * 1024;
const uint32 kNumAmpSamples = 64 * 1024;
const uint32 kACMId = 0x032897;
const uint32 kACMVersion = 1;

// Internal Classes
struct ACMInfo {
  uint32 m_id;
  uint32 m_version;
  uint32 m_sampleCount;
  uint32 m_numChannels;
  uint32 m_sampleRate;
  uint32 m_level;
  uint32 m_columns;
  uint32 m_rows;
};

class InternalAudioDecoder : public IAudioDecoder {
public:
  InternalAudioDecoder(ReadDataFunction readerFunction, void *pReaderData);
  ~InternalAudioDecoder();

  // Extract the header information
  const ACMInfo &GetHeader() const { return m_acm; }

  // Initialize the decoder
  bool Initialize();

  // Read data from the audio decoder.
  //   pBuffer: The buffer to receive the data from
  //    amount: How much data to read
  // Returns the number of bytes read - zero when we're at the end of the file
  uint32 Read(void *pBuffer, uint32 amount);

  // Operator overloads
  void *operator new(size_t numBytes);
  void operator delete(void *pPtr);

  // Unpackers
  static bool UnpackZeroFill(InternalAudioDecoder &decoder, uint32 id, uint32 col, bool &atEOF);
  static bool UnpackIgnore(InternalAudioDecoder &decoder, uint32 id, uint32 col, bool &atEOF);
  static bool UnpackLinear(InternalAudioDecoder &decoder, uint32 id, uint32 col, bool &atEOF);
  static bool UnpackK13(InternalAudioDecoder &decoder, uint32 id, uint32 col, bool &atEOF);
  static bool UnpackK12(InternalAudioDecoder &decoder, uint32 id, uint32 col, bool &atEOF);
  static bool UnpackK24(InternalAudioDecoder &decoder, uint32 id, uint32 col, bool &atEOF);
  static bool UnpackK23(InternalAudioDecoder &decoder, uint32 id, uint32 col, bool &atEOF);
  static bool UnpackK35(InternalAudioDecoder &decoder, uint32 id, uint32 col, bool &atEOF);
  static bool UnpackK34(InternalAudioDecoder &decoder, uint32 id, uint32 col, bool &atEOF);
  static bool UnpackK45(InternalAudioDecoder &decoder, uint32 id, uint32 col, bool &atEOF);
  static bool UnpackK44(InternalAudioDecoder &decoder, uint32 id, uint32 col, bool &atEOF);
  static bool UnpackT15(InternalAudioDecoder &decoder, uint32 id, uint32 col, bool &atEOF);
  static bool UnpackT27(InternalAudioDecoder &decoder, uint32 id, uint32 col, bool &atEOF);
  static bool UnpackT37(InternalAudioDecoder &decoder, uint32 id, uint32 col, bool &atEOF);

  static void SetMemoryFunctions(MemoryAllocFunc memAlloc, MemoryFreeFunc memFree);

private:
  // Read data from the read function
  sint32 ReadData(void *pBuffer, uint32 amount);
  uint32 InternalRead(void *pBuffer, uint32 amount);

  bool LoadBitBufferFromFile();
  bool LoadBitBuffer();
  bool ReloadBits(uint32 bits, uint32 &newData, bool &hitEOF);
  bool GetBits(uint32 bits, uint32 &resultData);
  bool GetBitsEOF(uint32 bits, uint32 &resultData, bool &hadEOF);

  bool DecodeBlock(bool &atEOF);
  bool UnpackBlock(bool &atEOF);
  void OutputValues(const sint32 *pSrcData, void *pBuffer, uint32 numWords);

  void JuggleBlock();

  ACMInfo m_acm;
  ReadDataFunction m_readerFunction;
  void *m_pReaderData;

  uint8 *m_pFileBitBuffer;
  uint32 m_bitBufferAvailableSize;
  uint32 m_bitBufferCurrPos;
  uint32 m_numBitsAvailable;
  uint32 m_bitData;

  uint32 m_blockLengthInSamples;
  uint32 m_wrapBufferLength;
  sint32 *m_pBlock;
  sint32 *m_pWrapBuffer;
  sint32 *m_pAmpBuffer;
  sint32 *m_pMidBuffer;

  bool m_blockReady;
  bool m_bitBufferAtEOF;
  uint32 m_streamPos;
  uint32 m_blockPos;

  // Memory management
  void *m_pMemoryBuffer;
  static MemoryAllocFunc s_Malloc;
  static MemoryFreeFunc s_Free;
};

// Tables
const sint32 gMap1Bit[] = {-1, +1};
const sint32 gMap2BitNear[] = {-2, -1, +1, +2};
const sint32 gMap2BitFar[] = {-3, -2, +2, +3};
const sint32 gMap3Bit[] = {-4, -3, -2, -1, +1, +2, +3, +4};

const uint16 gMul3x3[27] = {
    0x0000, 0x0001, 0x0002, 0x0010, 0x0011, 0x0012, 0x0020, 0x0021, 0x0022, 0x0100, 0x0101, 0x0102, 0x0110, 0x0111,
    0x0112, 0x0120, 0x0121, 0x0122, 0x0200, 0x0201, 0x0202, 0x0210, 0x0211, 0x0212, 0x0220, 0x0221, 0x0222,
};

const uint16 gMul3x5[125] = {
    0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0010, 0x0011, 0x0012, 0x0013, 0x0014, 0x0020, 0x0021, 0x0022, 0x0023,
    0x0024, 0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0040, 0x0041, 0x0042, 0x0043, 0x0044, 0x0100, 0x0101, 0x0102,
    0x0103, 0x0104, 0x0110, 0x0111, 0x0112, 0x0113, 0x0114, 0x0120, 0x0121, 0x0122, 0x0123, 0x0124, 0x0130, 0x0131,
    0x0132, 0x0133, 0x0134, 0x0140, 0x0141, 0x0142, 0x0143, 0x0144, 0x0200, 0x0201, 0x0202, 0x0203, 0x0204, 0x0210,
    0x0211, 0x0212, 0x0213, 0x0214, 0x0220, 0x0221, 0x0222, 0x0223, 0x0224, 0x0230, 0x0231, 0x0232, 0x0233, 0x0234,
    0x0240, 0x0241, 0x0242, 0x0243, 0x0244, 0x0300, 0x0301, 0x0302, 0x0303, 0x0304, 0x0310, 0x0311, 0x0312, 0x0313,
    0x0314, 0x0320, 0x0321, 0x0322, 0x0323, 0x0324, 0x0330, 0x0331, 0x0332, 0x0333, 0x0334, 0x0340, 0x0341, 0x0342,
    0x0343, 0x0344, 0x0400, 0x0401, 0x0402, 0x0403, 0x0404, 0x0410, 0x0411, 0x0412, 0x0413, 0x0414, 0x0420, 0x0421,
    0x0422, 0x0423, 0x0424, 0x0430, 0x0431, 0x0432, 0x0433, 0x0434, 0x0440, 0x0441, 0x0442, 0x0443, 0x0444,
};

const uint8 gMul2x11[121] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16,
    0x17, 0x18, 0x19, 0x1A, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x30, 0x31, 0x32,
    0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49,
    0x4A, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x60, 0x61, 0x62, 0x63, 0x64, 0x65,
    0x66, 0x67, 0x68, 0x69, 0x6A, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x80, 0x81,
    0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98,
    0x99, 0x9A, 0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA};
} // namespace

/**************************************************************/
/*                   Interface Functions                      */
/**************************************************************/

// Optional interface for supplying your own malloc and free functions
// Default is to use standard malloc and free.
void RegisterMemoryFunctions(MemoryAllocFunc memAlloc, MemoryFreeFunc memFree) {
  InternalAudioDecoder::SetMemoryFunctions(memAlloc, memFree);
}

// Create an audio decoder
// You supply a function for reading bytes from the compressed data via a
// void* pData handle, and the handle itself (typically a FILE *).
// Create_AudioDecoder returns a new AudioDecoder which can be used to
// read uncompressed decoded data from the compressed stream,
// and also returns the number of channels (1 or 2), the sample rate
// (e.g. 22050), and the number of samples contained in the compressed file
// (in case you want to pre-allocate a buffer to load them all into memory).
IAudioDecoder *AudioDecoder::CreateDecoder(ReadDataFunction readerFunction, void *pReaderData, uint32 &numChannels,
                                           uint32 &sampleRate, uint32 &sampleCount) {
  // allocate our decoder
  InternalAudioDecoder *pDecoder = new InternalAudioDecoder(readerFunction, pReaderData);
  if (pDecoder == NULL)
    return NULL;

  // initialize
  if (!pDecoder->Initialize()) {
    // Failed
    delete pDecoder;
    return NULL;
  }

  // extract the header information for the caller
  const ACMInfo &header = pDecoder->GetHeader();
  numChannels = header.m_numChannels;
  sampleRate = header.m_sampleRate;
  sampleCount = header.m_sampleCount;

  // return the decoder back to the user
  return pDecoder;
}

/**************************************************************/
/*                      Memory Management                     */
/**************************************************************/

// Static memory
MemoryAllocFunc InternalAudioDecoder::s_Malloc = DefaultMalloc;
MemoryFreeFunc InternalAudioDecoder::s_Free = DefaultFree;

void InternalAudioDecoder::SetMemoryFunctions(MemoryAllocFunc memAlloc, MemoryFreeFunc memFree) {
  if ((memAlloc && !memFree) || (!memAlloc && memFree))
    return;

  s_Malloc = (memAlloc) ? memAlloc : DefaultMalloc;
  s_Free = (memFree) ? memFree : DefaultFree;
}

void *InternalAudioDecoder::operator new(size_t numBytes) { return s_Malloc(static_cast<uint32>(numBytes)); }

void InternalAudioDecoder::operator delete(void *pPtr) {
  if (pPtr) {
    s_Free(pPtr);
  }
}

/**************************************************************/
/*                         Construction                       */
/**************************************************************/

InternalAudioDecoder::InternalAudioDecoder(ReadDataFunction readerFunction, void *pReaderData)
    : m_readerFunction(readerFunction), m_pReaderData(pReaderData), m_pFileBitBuffer(NULL), m_bitBufferAvailableSize(0),
      m_bitBufferCurrPos(0), m_numBitsAvailable(0), m_bitData(0), m_blockLengthInSamples(0), m_wrapBufferLength(0),
      m_pBlock(NULL), m_pWrapBuffer(NULL), m_pAmpBuffer(NULL), m_pMidBuffer(NULL), m_blockReady(false),
      m_bitBufferAtEOF(false), m_streamPos(0), m_blockPos(0), m_pMemoryBuffer(NULL) {
  std::memset(&m_acm, 0, sizeof(m_acm));
}

// Initialize the decoder
bool InternalAudioDecoder::Initialize() {
  // Allocate the bit buffer before we start reading it
  m_pFileBitBuffer = reinterpret_cast<uint8 *>(s_Malloc(kBitBufferSize));
  if (!m_pFileBitBuffer)
    return false;

  // Read in the ACM header
  if (!GetBits(24, m_acm.m_id))
    return false;
  if (m_acm.m_id != kACMId)
    return false;
  if (!GetBits(8, m_acm.m_version))
    return false;
  if (m_acm.m_version != kACMVersion)
    return false;

  // total value count
  uint32 temp;
  if (!GetBits(16, m_acm.m_sampleCount))
    return false;
  if (!GetBits(16, temp))
    return false;
  m_acm.m_sampleCount += temp << 16;

  if (m_acm.m_sampleCount == 0)
    return false;

  // num channels
  if (!GetBits(16, m_acm.m_numChannels))
    return false;

  if (m_acm.m_numChannels < 1 || m_acm.m_numChannels > 2)
    return false;

  // sample rate
  if (!GetBits(16, m_acm.m_sampleRate))
    return false;

  if (m_acm.m_sampleRate < 4096)
    return false;

  // level
  if (!GetBits(4, m_acm.m_level))
    return false;

  // rows
  if (!GetBits(12, m_acm.m_rows))
    return false;

  if (m_acm.m_rows == 0)
    return false;

  // Calculate the blocks
  m_acm.m_columns = 1 << m_acm.m_level;
  m_wrapBufferLength = 2 * m_acm.m_columns - 2;
  m_blockLengthInSamples = m_acm.m_rows * m_acm.m_columns;

  // Calculate the amount of memory that needs to be allocated
  const size_t blockMemSize = m_blockLengthInSamples * sizeof(uint32);
  const size_t wrapMemSize = m_wrapBufferLength * sizeof(uint32);
  const size_t ampMemSize = kNumAmpSamples * sizeof(uint32);
  const size_t totalMemSize = blockMemSize + wrapMemSize + ampMemSize;
  m_pMemoryBuffer = s_Malloc(static_cast<uint32>(totalMemSize));
  if (m_pMemoryBuffer == NULL)
    return false;

  // Assign buffer pointers
  m_pBlock = reinterpret_cast<sint32 *>(m_pMemoryBuffer);
  m_pWrapBuffer = reinterpret_cast<sint32 *>(m_pBlock + m_blockLengthInSamples);
  m_pAmpBuffer = reinterpret_cast<sint32 *>(m_pWrapBuffer + m_wrapBufferLength);
  m_pMidBuffer = reinterpret_cast<sint32 *>(m_pAmpBuffer + (kNumAmpSamples >> 1));

  // Initialize data buffers
  std::memset(m_pWrapBuffer, 0, m_wrapBufferLength * sizeof(uint32));

  // Lets get going
  return true;
}

/**************************************************************/
/*                         Destruction                        */
/**************************************************************/

InternalAudioDecoder::~InternalAudioDecoder() {
  if (m_pMemoryBuffer) {
    s_Free(m_pMemoryBuffer);
  }

  if (m_pFileBitBuffer) {
    s_Free(m_pFileBitBuffer);
  }
}

/**************************************************************/
/*                           Reading                          */
/**************************************************************/

// Read data from the audio decoder.
//   pBuffer: The buffer to receive the data from
//    amount: How much data to read
// Returns the number of bytes read - zero when we're at the end of the file
uint32 InternalAudioDecoder::Read(void *pBuffer, uint32 amount) {
  uint32 totalBytesRead = 0;
  uint8 *pBuf = reinterpret_cast<uint8 *>(pBuffer);

  do {
    uint32 numWords = amount >> 1;
    if (m_streamPos + numWords > m_acm.m_sampleCount) {
      // Don't read past the end of file
      numWords = m_acm.m_sampleCount - m_streamPos;
    }

    if (m_acm.m_numChannels > 1) {
      // Read full channel data
      numWords -= numWords & 1;
    }

    if (numWords == 0)
      break;

    uint32 res = InternalRead(pBuf, numWords << 1);
    if (res == 0)
      break;

    pBuf += res;
    amount -= res;
    totalBytesRead += res;
  } while (amount > 0);

  return totalBytesRead;
}

uint32 InternalAudioDecoder::InternalRead(void *pBuffer, uint32 amount) {
  // Check for End-of-File
  if (m_streamPos >= m_acm.m_sampleCount)
    return 0;

  uint32 numWords = amount >> 1;
  if (!m_blockReady) {
    bool atEOF;
    if (!DecodeBlock(atEOF)) {
      // TODO: Report proper error?
      return atEOF ? 0 : 0;
    }
  }

  uint32 numAvailableWords = m_blockLengthInSamples - m_blockPos;
  if (numAvailableWords < numWords) {
    // Don't read too past the end of the block
    numWords = numAvailableWords;
  }

  if (m_streamPos + numWords > m_acm.m_sampleCount) {
    // Don't read past the end of file
    numWords = m_acm.m_sampleCount - m_streamPos;
  }

  if (m_acm.m_numChannels > 1) {
    // Read full channel data
    numWords -= numWords & 1;
  }

  const sint32 *pSrcData = m_pBlock + m_blockPos;
  OutputValues(pSrcData, pBuffer, numWords);

  m_streamPos += numWords;
  m_blockPos += numWords;
  if (m_blockPos == m_blockLengthInSamples) {
    m_blockReady = false;
  }

  return numWords << 1;
}

/**************************************************************/
/*                        Output                              */
/**************************************************************/

void InternalAudioDecoder::OutputValues(const sint32 *pSrcData, void *pBuffer, uint32 numWords) {
  uint8 *pDst = reinterpret_cast<uint8 *>(pBuffer);

  const uint32 shiftAmount = m_acm.m_level;
  while (numWords--) {
    sint32 src = *pSrcData++;
    sint32 val = INTEL_INT(src) >> shiftAmount;
    *pDst++ = val & 0xFF;
    *pDst++ = (val >> 8) & 0xFF;
  }
}

/**************************************************************/
/*                       Decoding                             */
/**************************************************************/

bool InternalAudioDecoder::DecodeBlock(bool &atEOF) {
  // Reset block state
  m_blockReady = false;
  m_blockPos = 0;

  // Read in data
  uint32 bitPower;
  if (!GetBitsEOF(4, bitPower, atEOF))
    return false;

  uint32 bitValue;
  if (!GetBitsEOF(16, bitValue, atEOF))
    return false;

  // Process
  sint32 x;
  uint32 i, count = 1 << bitPower;
  for (i = 0, x = 0; i < count; ++i) {
    m_pMidBuffer[i] = x;
    x += bitValue;
  }

  for (i = 1, x = -static_cast<sint32>(bitValue); i <= count; ++i) {
    m_pMidBuffer[-static_cast<sint32>(i)] = x;
    x -= bitValue;
  }

  if (!UnpackBlock(atEOF))
    return false;

  JuggleBlock();

  m_blockReady = true;
  return true;
}

static void Juggle(sint32 *pWrap, sint32 *pBlock, uint32 subLen, uint32 subCount) {
  for (uint32 i = 0; i < subLen; ++i) {
    sint32 *pPtr = pBlock;
    sint32 r0 = pWrap[0];
    sint32 r1 = pWrap[1];

    uint32 cnt = subCount >> 1;
    for (uint32 j = 0; j < cnt; ++j) {
      sint32 r2 = *pPtr;
      *pPtr = (r1 << 1) + (r0 + r2);
      pPtr += subLen;
      sint32 r3 = *pPtr;
      *pPtr = (r2 << 1) - (r1 + r3);
      pPtr += subLen;

      r0 = r2;
      r1 = r3;
    }

    *pWrap++ = r0;
    *pWrap++ = r1;
    ++pBlock;
  }
}

void InternalAudioDecoder::JuggleBlock() {
  // check subblock length
  if (m_acm.m_level == 0)
    return;

  // Apply juggle()  (rows)x(cols)
  // from (step_subcount * 2)            x (subblock_len/2)
  // to   (step_subcount * subblock_len) x (1)
  const uint32 stepSubCount = (m_acm.m_level > 9) ? 1 : ((2048 >> m_acm.m_level) - 2);
  uint32 todoCount = m_acm.m_rows;
  sint32 *pBlock = m_pBlock;
  while (true) {
    sint32 *pWrap = m_pWrapBuffer;
    uint32 subCount = stepSubCount;
    if (subCount > todoCount) {
      subCount = todoCount;
    }

    uint32 subLen = m_acm.m_columns >> 1;
    subCount <<= 1;

    Juggle(pWrap, pBlock, subLen, subCount);
    pWrap += subLen << 1;

    uint32 i;
    sint32 *pPtr;
    for (i = 0, pPtr = pBlock; i < subCount; ++i) {
      ++pPtr[0];
      pPtr += subLen;
    }

    while (subLen > 1) {
      subLen >>= 1;
      subCount <<= 1;
      Juggle(pWrap, pBlock, subLen, subCount);
      pWrap += subLen << 1;
    }

    if (todoCount <= stepSubCount)
      break;

    todoCount -= stepSubCount;
    pBlock += stepSubCount << m_acm.m_level;
  }
}

/**************************************************************/
/*                       Unpacking                            */
/**************************************************************/

bool InternalAudioDecoder::UnpackZeroFill(InternalAudioDecoder &decoder, uint32 id, uint32 col, bool &atEOF) {
  const uint32 numRows = decoder.m_acm.m_rows;
  const uint32 level = decoder.m_acm.m_level;
  const sint32 midZero = decoder.m_pMidBuffer[0];
  sint32 *pBlock = decoder.m_pBlock;

  for (uint32 i = 0; i < numRows; ++i) {
    const uint32 pos = (i << level) + col;
    pBlock[pos] = midZero;
  }

  return true;
}

bool InternalAudioDecoder::UnpackIgnore(InternalAudioDecoder &decoder, uint32 id, uint32 col, bool &atEOF) {
  return true;
}

bool InternalAudioDecoder::UnpackLinear(InternalAudioDecoder &decoder, uint32 id, uint32 col, bool &atEOF) {
  const sint32 middleIndex = 1 << (id - 1);
  const uint32 numRows = decoder.m_acm.m_rows;
  const uint32 level = decoder.m_acm.m_level;
  sint32 *pBlock = decoder.m_pBlock;
  for (uint32 i = 0; i < numRows; ++i) {
    uint32 offset;
    if (!decoder.GetBitsEOF(id, offset, atEOF))
      return false;

    const uint32 pos = (i << level) + col;
    pBlock[pos] = decoder.m_pMidBuffer[offset - middleIndex];
  }

  return true;
}

bool InternalAudioDecoder::UnpackK13(InternalAudioDecoder &decoder, uint32 id, uint32 col, bool &atEOF) {
  const sint32 midZero = decoder.m_pMidBuffer[0];
  const uint32 numRows = decoder.m_acm.m_rows;
  const uint32 level = decoder.m_acm.m_level;
  sint32 *pBlock = decoder.m_pBlock;

  for (uint32 i = 0; i < numRows; ++i) {
    uint32 flag;
    if (!decoder.GetBitsEOF(1, flag, atEOF))
      return false;

    if (flag == 0) {
      pBlock[(i++ << level) + col] = midZero;
      if (i >= numRows)
        break;
      pBlock[(i << level) + col] = midZero;
      continue;
    }

    if (!decoder.GetBitsEOF(1, flag, atEOF))
      return false;

    if (flag == 0) {
      pBlock[(i << level) + col] = midZero;
      continue;
    }

    if (!decoder.GetBitsEOF(1, flag, atEOF))
      return false;

    pBlock[(i << level) + col] = decoder.m_pMidBuffer[gMap1Bit[flag]];
  }

  return true;
}

bool InternalAudioDecoder::UnpackK12(InternalAudioDecoder &decoder, uint32 id, uint32 col, bool &atEOF) {
  const sint32 midZero = decoder.m_pMidBuffer[0];
  const uint32 numRows = decoder.m_acm.m_rows;
  const uint32 level = decoder.m_acm.m_level;
  sint32 *pBlock = decoder.m_pBlock;

  for (uint32 i = 0; i < numRows; ++i) {
    uint32 offset;
    if (!decoder.GetBitsEOF(1, offset, atEOF))
      return false;

    if (offset == 0) {
      pBlock[(i << level) + col] = midZero;
      continue;
    }

    if (!decoder.GetBitsEOF(1, offset, atEOF))
      return false;
    pBlock[(i << level) + col] = decoder.m_pMidBuffer[gMap1Bit[offset]];
  }

  return true;
}

bool InternalAudioDecoder::UnpackK24(InternalAudioDecoder &decoder, uint32 id, uint32 col, bool &atEOF) {
  const sint32 midZero = decoder.m_pMidBuffer[0];
  const uint32 numRows = decoder.m_acm.m_rows;
  const uint32 level = decoder.m_acm.m_level;
  sint32 *pBlock = decoder.m_pBlock;

  for (uint32 i = 0; i < numRows; ++i) {
    uint32 offset;
    if (!decoder.GetBitsEOF(1, offset, atEOF))
      return false;

    if (offset == 0) {
      pBlock[(i++ << level) + col] = midZero;
      if (i >= numRows)
        break;
      pBlock[(i << level) + col] = midZero;
      continue;
    }

    if (!decoder.GetBitsEOF(1, offset, atEOF))
      return false;
    if (offset == 0) {
      pBlock[(i << level) + col] = midZero;
      continue;
    }

    if (!decoder.GetBitsEOF(2, offset, atEOF))
      return false;
    pBlock[(i << level) + col] = decoder.m_pMidBuffer[gMap2BitNear[offset]];
  }

  return true;
}

bool InternalAudioDecoder::UnpackK23(InternalAudioDecoder &decoder, uint32 id, uint32 col, bool &atEOF) {
  const sint32 midZero = decoder.m_pMidBuffer[0];
  const uint32 numRows = decoder.m_acm.m_rows;
  const uint32 level = decoder.m_acm.m_level;
  sint32 *pBlock = decoder.m_pBlock;

  for (uint32 i = 0; i < numRows; ++i) {
    uint32 offset;
    if (!decoder.GetBitsEOF(1, offset, atEOF))
      return false;

    if (offset == 0) {
      pBlock[(i << level) + col] = midZero;
      continue;
    }

    if (!decoder.GetBitsEOF(2, offset, atEOF))
      return false;
    pBlock[(i << level) + col] = decoder.m_pMidBuffer[gMap2BitNear[offset]];
  }

  return true;
}

bool InternalAudioDecoder::UnpackK35(InternalAudioDecoder &decoder, uint32 id, uint32 col, bool &atEOF) {
  const sint32 midZero = decoder.m_pMidBuffer[0];
  const uint32 numRows = decoder.m_acm.m_rows;
  const uint32 level = decoder.m_acm.m_level;
  sint32 *pBlock = decoder.m_pBlock;

  for (uint32 i = 0; i < numRows; ++i) {
    uint32 offset;
    if (!decoder.GetBitsEOF(1, offset, atEOF))
      return false;

    if (offset == 0) {
      pBlock[(i++ << level) + col] = midZero;
      if (i >= numRows)
        break;
      pBlock[(i << level) + col] = midZero;
      continue;
    }

    if (!decoder.GetBitsEOF(1, offset, atEOF))
      return false;
    if (offset == 0) {
      pBlock[(i << level) + col] = midZero;
      continue;
    }

    if (!decoder.GetBitsEOF(1, offset, atEOF))
      return false;
    if (offset == 0) {
      if (!decoder.GetBitsEOF(1, offset, atEOF))
        return false;
      pBlock[(i << level) + col] = decoder.m_pMidBuffer[gMap1Bit[offset]];
      continue;
    }

    if (!decoder.GetBitsEOF(2, offset, atEOF))
      return false;
    pBlock[(i << level) + col] = decoder.m_pMidBuffer[gMap2BitFar[offset]];
  }

  return true;
}

bool InternalAudioDecoder::UnpackK34(InternalAudioDecoder &decoder, uint32 id, uint32 col, bool &atEOF) {
  const sint32 midZero = decoder.m_pMidBuffer[0];
  const uint32 numRows = decoder.m_acm.m_rows;
  const uint32 level = decoder.m_acm.m_level;
  sint32 *pBlock = decoder.m_pBlock;

  for (uint32 i = 0; i < numRows; ++i) {
    uint32 offset;
    if (!decoder.GetBitsEOF(1, offset, atEOF))
      return false;
    if (offset == 0) {
      pBlock[(i << level) + col] = midZero;
      continue;
    }

    if (!decoder.GetBitsEOF(1, offset, atEOF))
      return false;
    if (offset == 0) {
      if (!decoder.GetBitsEOF(1, offset, atEOF))
        return false;
      pBlock[(i << level) + col] = decoder.m_pMidBuffer[gMap1Bit[offset]];
      continue;
    }

    if (!decoder.GetBitsEOF(2, offset, atEOF))
      return false;
    pBlock[(i << level) + col] = decoder.m_pMidBuffer[gMap2BitFar[offset]];
  }

  return true;
}

bool InternalAudioDecoder::UnpackK45(InternalAudioDecoder &decoder, uint32 id, uint32 col, bool &atEOF) {
  const sint32 midZero = decoder.m_pMidBuffer[0];
  const uint32 numRows = decoder.m_acm.m_rows;
  const uint32 level = decoder.m_acm.m_level;
  sint32 *pBlock = decoder.m_pBlock;

  for (uint32 i = 0; i < numRows; ++i) {
    uint32 offset;
    if (!decoder.GetBitsEOF(1, offset, atEOF))
      return false;
    if (offset == 0) {
      pBlock[(i++ << level) + col] = midZero;
      if (i >= numRows)
        break;
      pBlock[(i << level) + col] = midZero;
      continue;
    }

    if (!decoder.GetBitsEOF(1, offset, atEOF))
      return false;
    if (offset == 0) {
      pBlock[(i << level) + col] = midZero;
      continue;
    }

    if (!decoder.GetBitsEOF(3, offset, atEOF))
      return false;
    pBlock[(i << level) + col] = decoder.m_pMidBuffer[gMap3Bit[offset]];
  }

  return true;
}

bool InternalAudioDecoder::UnpackK44(InternalAudioDecoder &decoder, uint32 id, uint32 col, bool &atEOF) {
  const sint32 midZero = decoder.m_pMidBuffer[0];
  const uint32 numRows = decoder.m_acm.m_rows;
  const uint32 level = decoder.m_acm.m_level;
  sint32 *pBlock = decoder.m_pBlock;

  for (uint32 i = 0; i < numRows; ++i) {
    uint32 offset;
    if (!decoder.GetBitsEOF(1, offset, atEOF))
      return false;
    if (offset == 0) {
      pBlock[(i << level) + col] = midZero;
      continue;
    }

    if (!decoder.GetBitsEOF(3, offset, atEOF))
      return false;
    pBlock[(i << level) + col] = decoder.m_pMidBuffer[gMap3Bit[offset]];
  }

  return true;
}

bool InternalAudioDecoder::UnpackT15(InternalAudioDecoder &decoder, uint32 id, uint32 col, bool &atEOF) {
  const uint32 numRows = decoder.m_acm.m_rows;
  const uint32 level = decoder.m_acm.m_level;
  sint32 *pBlock = decoder.m_pBlock;

  for (uint32 i = 0; i < numRows; ++i) {
    uint32 offset;
    if (!decoder.GetBitsEOF(5, offset, atEOF))
      return false;

    uint32 n1 = (gMul3x3[offset] & 0x0F) - 1;
    uint32 n2 = ((gMul3x3[offset] >> 4) & 0x0F) - 1;
    uint32 n3 = ((gMul3x3[offset] >> 8) & 0x0F) - 1;

    pBlock[(i++ << level) + col] = decoder.m_pMidBuffer[n1];
    if (i >= numRows)
      break;

    pBlock[(i++ << level) + col] = decoder.m_pMidBuffer[n2];
    if (i >= numRows)
      break;

    pBlock[(i << level) + col] = decoder.m_pMidBuffer[n3];
  }

  return true;
}

bool InternalAudioDecoder::UnpackT27(InternalAudioDecoder &decoder, uint32 id, uint32 col, bool &atEOF) {
  const uint32 numRows = decoder.m_acm.m_rows;
  const uint32 level = decoder.m_acm.m_level;
  sint32 *pBlock = decoder.m_pBlock;

  for (uint32 i = 0; i < numRows; ++i) {
    uint32 offset;
    if (!decoder.GetBitsEOF(7, offset, atEOF))
      return false;

    uint32 n1 = (gMul3x5[offset] & 0x0F) - 2;
    uint32 n2 = ((gMul3x5[offset] >> 4) & 0x0F) - 2;
    uint32 n3 = ((gMul3x5[offset] >> 8) & 0x0F) - 2;

    pBlock[(i++ << level) + col] = decoder.m_pMidBuffer[n1];
    if (i >= numRows)
      break;

    pBlock[(i++ << level) + col] = decoder.m_pMidBuffer[n2];
    if (i >= numRows)
      break;

    pBlock[(i << level) + col] = decoder.m_pMidBuffer[n3];
  }

  return true;
}

bool InternalAudioDecoder::UnpackT37(InternalAudioDecoder &decoder, uint32 id, uint32 col, bool &atEOF) {
  const uint32 numRows = decoder.m_acm.m_rows;
  const uint32 level = decoder.m_acm.m_level;
  sint32 *pBlock = decoder.m_pBlock;

  for (uint32 i = 0; i < numRows; ++i) {
    uint32 offset;
    if (!decoder.GetBitsEOF(7, offset, atEOF))
      return false;

    uint32 n1 = (gMul2x11[offset] & 0x0F) - 5;
    uint32 n2 = ((gMul2x11[offset] >> 4) & 0x0F) - 5;

    pBlock[(i++ << level) + col] = decoder.m_pMidBuffer[n1];
    if (i >= numRows)
      break;

    pBlock[(i << level) + col] = decoder.m_pMidBuffer[n2];
  }

  return true;
}

typedef bool (*UnpackerFunction)(InternalAudioDecoder &decoder, uint32 id, uint32 col, bool &atEOF);
static const UnpackerFunction Unpacker[] = {
    InternalAudioDecoder::UnpackZeroFill, InternalAudioDecoder::UnpackIgnore, InternalAudioDecoder::UnpackIgnore,
    InternalAudioDecoder::UnpackLinear,   InternalAudioDecoder::UnpackLinear, InternalAudioDecoder::UnpackLinear,
    InternalAudioDecoder::UnpackLinear,   InternalAudioDecoder::UnpackLinear, InternalAudioDecoder::UnpackLinear,
    InternalAudioDecoder::UnpackLinear,   InternalAudioDecoder::UnpackLinear, InternalAudioDecoder::UnpackLinear,
    InternalAudioDecoder::UnpackLinear,   InternalAudioDecoder::UnpackLinear, InternalAudioDecoder::UnpackLinear,
    InternalAudioDecoder::UnpackLinear,   InternalAudioDecoder::UnpackLinear, InternalAudioDecoder::UnpackK13,
    InternalAudioDecoder::UnpackK12,      InternalAudioDecoder::UnpackT15,    InternalAudioDecoder::UnpackK24,
    InternalAudioDecoder::UnpackK23,      InternalAudioDecoder::UnpackT27,    InternalAudioDecoder::UnpackK35,
    InternalAudioDecoder::UnpackK34,      InternalAudioDecoder::UnpackIgnore, InternalAudioDecoder::UnpackK45,
    InternalAudioDecoder::UnpackK44,      InternalAudioDecoder::UnpackIgnore, InternalAudioDecoder::UnpackT37,
    InternalAudioDecoder::UnpackIgnore,   InternalAudioDecoder::UnpackIgnore,
};

bool InternalAudioDecoder::UnpackBlock(bool &atEOF) {
  for (uint32 i = 0; i < m_acm.m_columns; ++i) {
    uint32 unpackerId;
    if (!GetBitsEOF(5, unpackerId, atEOF))
      return false;

    // Jump to the unpacker
    if (!Unpacker[unpackerId](*this, unpackerId, i, atEOF))
      return false;
  }

  return true;
}

/**************************************************************/
/*                     Loading Data                           */
/**************************************************************/

bool InternalAudioDecoder::LoadBitBufferFromFile() {
  if (m_bitBufferAtEOF)
    return true;

  // Callback to the user to get more data
  sint32 res = m_readerFunction(m_pReaderData, m_pFileBitBuffer, kBitBufferSize);
  if (res < 0)
    return false;

  if (res == 0) {
    // No more data
    m_bitBufferAtEOF = true;
    m_pFileBitBuffer[0] = 0;
    m_bitBufferAvailableSize = 1;
  } else {
    // We still have data
    m_bitBufferAvailableSize = static_cast<uint32>(res);
  }

  m_bitBufferCurrPos = 0;
  return true;
}

bool InternalAudioDecoder::LoadBitBuffer() {
  uint32 numLoadedBits = 0;  // How many bits are still available
  uint32 loadedDataBits = 0; // The bit data still available

  // How many bytes of data remain?
  const uint32 bufferRemaining = m_bitBufferAvailableSize - m_bitBufferCurrPos;
  ASSERT(bufferRemaining < 4);

  // Bring in the remaining bits from the buffer
  const uint8 *pCurrBuffer = m_pFileBitBuffer + m_bitBufferCurrPos;
  switch (bufferRemaining) {
  case 3:
    loadedDataBits += pCurrBuffer[2] << 16;
  case 2:
    loadedDataBits += pCurrBuffer[1] << 8;
  case 1:
    loadedDataBits += pCurrBuffer[0];
  }
  numLoadedBits = bufferRemaining << 3;

  // Fill back up the file buffer
  if (!LoadBitBufferFromFile())
    return false;

  // Bring in the rest of the bits that we can to fill up the 32 bits (or
  // when we run out of available buffer)
  while (numLoadedBits < 32) {
    if (m_bitBufferAvailableSize - m_bitBufferCurrPos == 0)
      break;

    // Bring in the next byte
    loadedDataBits |= m_pFileBitBuffer[m_bitBufferCurrPos] << numLoadedBits;
    numLoadedBits += 8;
    ++m_bitBufferCurrPos;
  }

  // Continue with processing
  m_bitData = loadedDataBits;
  m_numBitsAvailable = numLoadedBits;
  return true;
}

// Reloads the available bits back to a full 32
bool InternalAudioDecoder::ReloadBits(uint32 bits, uint32 &newData, bool &hitEOF) {
  ASSERT(bits > m_numBitsAvailable);
  hitEOF = false;

  // Bring in the bits that we can
  uint32 currBitData = m_bitData;
  uint32 numLoadedBits = m_numBitsAvailable;
  bits -= numLoadedBits;

  // Bring in more bits from the buffer
  uint32 bitData, bitsAvail;
  if ((m_bitBufferAvailableSize - m_bitBufferCurrPos) >= 4) {
    // We have a full 32bits available
    uint8 *pCurrBuffer = m_pFileBitBuffer + m_bitBufferCurrPos;
    m_bitBufferCurrPos += 4;
    bitData = pCurrBuffer[0] + (pCurrBuffer[1] << 8) + (pCurrBuffer[2] << 16) + (pCurrBuffer[3] << 24);
    bitsAvail = 32;
  } else {
    // We're almost out of buffer space - fill it back up
    if (!LoadBitBuffer())
      return false;

    if (m_numBitsAvailable < bits) {
      // Unexpected EOF
      hitEOF = true;
      return false;
    }

    bitData = m_bitData;
    bitsAvail = m_numBitsAvailable;
  }

  // Fold in the bits necessary to fill back up
  currBitData |= (bitData & ((1 << bits) - 1)) << numLoadedBits;
  m_bitData = bitData >> bits;
  m_numBitsAvailable = bitsAvail - bits;
  newData = currBitData;
  return true;
}

/**************************************************************/
/*                     Data Retrieval                         */
/**************************************************************/

bool InternalAudioDecoder::GetBits(uint32 bits, uint32 &resultData) {
  if (m_numBitsAvailable >= bits) {
    // We have enough data to pull from
    resultData = m_bitData & ((1 << bits) - 1);
    m_bitData >>= bits;
    m_numBitsAvailable -= bits;
    return true;
  }

  // Reload our bitData
  bool hadEOF;
  bool res = ReloadBits(bits, resultData, hadEOF);
  ASSERT(!hadEOF);
  return res;
}

bool InternalAudioDecoder::GetBitsEOF(uint32 bits, uint32 &resultData, bool &hadEOF) {
  if (m_numBitsAvailable >= bits) {
    // We have enough data to pull from
    resultData = m_bitData & ((1 << bits) - 1);
    m_bitData >>= bits;
    m_numBitsAvailable -= bits;
    return true;
  }

  // Reload our bitData
  return ReloadBits(bits, resultData, hadEOF);
}
