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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>

/*
// Create an audio decoder
// You supply a function for reading bytes from the compressed data via a
// void *data handle, and the handle itself (typically a FILE *).
// Create_AudioDecoder returns a new AudioDecoder which can be used to
// read uncompressed decoded data from the compressed stream,
// and also returns the number of channels (1 or 2), the sample rate
// (e.g. 22050), and the number of samples contained in the compressed file
// (in case you want to pre-allocate a buffer to load them all into memory).
typedef unsigned ReadFunction(void *data, void *buf, unsigned qty);
typedef struct {bool empty;} AudioDecoder;
AudioDecoder *Create_AudioDecoder(ReadFunction *reader, void *data,unsigned *pChannels, unsigned *pSampleRate,long
*pSampleCount)
{
        return malloc(sizeof(AudioDecoder));
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
        if(ad) free(ad);
}

// Optional interface for supplying your own malloc and free functions
// Default is to use standard malloc and free.
typedef void *(*ad_malloc)(unsigned size);
typedef void (*ad_free)(void *p);
void AudioDecoder_MallocFree(ad_malloc *fn_malloc, ad_free *fn_free)
{
}
*/