/*
 * $Logfile: /DescentIII/Main/lib/audio_encode.h $
 * $Revision: 1 $
 * $Date: 1/27/99 1:33a $
 * $Author: Jeff $
 *
 * Header file for audio compression library
 *
 * $Log: /DescentIII/Main/lib/audio_encode.h $
 *
 * 1     1/27/99 1:33a Jeff
 *
 * $NoKeywords: $
 */

#ifndef __AUDIO_ENCODE_H_
#define __AUDIO_ENCODE_H_

// input_levels (default 7 or for 2k total)
// input_samples (default 16 or for 2k total)
// input_rate (default 22K)
// input_channels (default 1)
// input_factor (compression factor) (default 4 for 22K, 8 for 44K)
// input_volscale (Volume scaling) (slightly <= 1.0, default ,97)
int aenc_Compress(char *input_filename, char *output_filename, int *input_levels = NULL, int *input_samples = NULL,
                  int *input_rate = NULL, int *input_channels = NULL, float *input_factor = NULL,
                  float *input_volscale = NULL);

#endif