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

#ifndef AENCODE_H_
#define AENCODE_H_

#include <stdio.h>

typedef long ReadSampleFunction(void *data);
enum AudioError {
  ReadSampleEof = 0x80000000,
};

unsigned int AudioEncode(ReadSampleFunction *read, void *data, unsigned channels, unsigned sample_rate, float volume,
                          FILE *out, int levels, int samples_per_subband, float comp_ratio);

#endif
