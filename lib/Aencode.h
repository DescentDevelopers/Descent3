#ifndef AENCODE_H_
#define AENCODE_H_

#include <stdio.h>

typedef long ReadSampleFunction(void *data);
enum AudioError {
  ReadSampleEof = 0x80000000,
};

unsigned long AudioEncode(ReadSampleFunction *read, void *data, unsigned channels, unsigned sample_rate, float volume,
                          FILE *out, int levels, int samples_per_subband, float comp_ratio);

#endif
