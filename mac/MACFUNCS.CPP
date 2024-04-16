#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "mem.h"
#include "streamaudio.h"

#if 0
// Create an audio decoder
// You supply a function for reading bytes from the compressed data via a
// void *data handle, and the handle itself (typically a FILE *).
// Create_AudioDecoder returns a new AudioDecoder which can be used to
// read uncompressed decoded data from the compressed stream,
// and also returns the number of channels (1 or 2), the sample rate
// (e.g. 22050), and the number of samples contained in the compressed file
// (in case you want to pre-allocate a buffer to load them all into memory).
typedef unsigned ReadFunction(void *data, void *buf, unsigned qty);
AudioDecoder *Create_AudioDecoder(ReadFunction *reader, void *data,unsigned *pChannels, unsigned *pSampleRate,long *pSampleCount)
{
	return (AudioDecoder*)mem_malloc(sizeof(AudioDecoder));	
}
// Read from audio decoder at most the specified qty of bytes
// (each sample takes two bytes).
// Returns zero when the end of file is reached.
unsigned AudioDecoder_Read(AudioDecoder *ad, void *buf, unsigned qty)
{
}
// Close audio decoder
void AudioDecoder_Close(AudioDecoder *ad)
{
	if(ad) mem_free(ad);
}
// Optional interface for supplying your own malloc and free functions
// Default is to use standard malloc and free.
void AudioDecoder_MallocFree(ad_malloc *fn_malloc, ad_free *fn_free)
{
}
#endif