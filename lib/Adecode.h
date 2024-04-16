#ifndef AUDIODECODE_H_
#define AUDIODECODE_H_

namespace AudioDecoder {
typedef unsigned int uint32;
typedef signed int sint32;
typedef unsigned short uint16;
typedef signed short sint16;
typedef unsigned char uint8;
typedef signed char sint8;

class IAudioDecoder {
public:
  virtual ~IAudioDecoder() {}

  // Read data from the audio decoder.
  //   pBuffer: The buffer to receive the data from
  //    amount: How much data to read
  // Returns the number of bytes read - zero when we're at the end of the file
  virtual uint32 Read(void *pBuffer, uint32 amount) = 0;
};

// Create an audio decoder
// You supply a function for reading bytes from the compressed data via a
// void* pData handle, and the handle itself (typically a FILE *).
// Create_AudioDecoder returns a new AudioDecoder which can be used to
// read uncompressed decoded data from the compressed stream,
// and also returns the number of channels (1 or 2), the sample rate
// (e.g. 22050), and the number of samples contained in the compressed file
// (in case you want to pre-allocate a buffer to load them all into memory).
typedef sint32 (*ReadDataFunction)(void *pData, void *pBuffer, unsigned int amount);
IAudioDecoder *CreateDecoder(ReadDataFunction reader, void *pData, uint32 &numChannels, uint32 &sampleRate,
                             uint32 &sampleCount);

// Optional interface for supplying your own malloc and free functions
// Default is to use standard malloc and free.
typedef void *(*MemoryAllocFunc)(uint32 size);
typedef void (*MemoryFreeFunc)(void *p);
void RegisterMemoryFunctions(MemoryAllocFunc memAlloc, MemoryFreeFunc memFree);
} // namespace AudioDecoder

#endif
