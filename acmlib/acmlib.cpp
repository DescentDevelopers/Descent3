

#include "Adecode.h"
#include "mem.h"

#ifdef __cplusplus
extern "C" {
#endif


AudioDecoder * Create_AudioDecoder(
	ReadFunction *reader, void *data,
	unsigned *pChannels, unsigned *pSampleRate,
	long *pSampleCount)
{
  return (AudioDecoder*)mem_malloc(sizeof(AudioDecoder));
}

// Read from audio decoder at most the specified qty of bytes
// (each sample takes two bytes).
// Returns zero when the end of file is reached.
unsigned __cdecl AudioDecoder_Read(AudioDecoder *ad, void *buf, unsigned qty)
{
  return 0;
}

// Close audio decoder
void __cdecl AudioDecoder_Close(AudioDecoder *ad)
{
  if(ad) mem_free(ad);
}

// Optional interface for supplying your own malloc and free functions
// Default is to use standard malloc and free.

void __cdecl AudioDecoder_MallocFree(ad_malloc *fn_malloc, ad_free *fn_free)
{
  

}


#ifdef __cplusplus
};
#endif
