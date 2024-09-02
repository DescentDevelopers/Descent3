/*
 * Descent 3
 * Copyright (C) 2024 Descent Developers
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

#include <algorithm>
#include <cstring>

#include "gamespyutils.h"

// Helper functions based on GameSpy SDK functions.
// See https://github.com/nitrocaster/GameSpy/blob/master/src/GameSpy/qr2/qr2.c for reference


void gs_encrypt(const uint8_t *key, uint8_t *buffer_ptr, int buffer_len) {
  uint8_t x, y;
  uint8_t state[256];

  // length WITHOUT null terminator!
  size_t key_len = strlen((const char *)key);

  for (int counter = 0; counter < 256; counter++) {
    state[counter] = counter;
  }

  // Generate key
  x = 0;
  y = 0;
  for (int i = 0; i < 256; i++) {
    y = (key[x] + state[i] + y) % 256;
    x = (x + 1) % key_len;
    std::swap(state[i], state[y]);
  }

  x = 0;
  y = 0;
  for (int i = 0; i < buffer_len; i++) {
    x = (x + buffer_ptr[i] + 1) % 256;
    y = (state[x] + y) % 256;
    std::swap(state[x], state[y]);
    buffer_ptr[i] ^= state[((state[x] + state[y]) % 256)];
  }
}

static uint8_t encode_ct(uint8_t c) {
  if (c < 26)
    return (uint8_t)('A' + c);
  if (c < 52)
    return (uint8_t)('a' + c - 26);
  if (c < 62)
    return (uint8_t)('0' + c - 52);
  if (c == 62)
    return (uint8_t)('+');
  if (c == 63)
    return (uint8_t)('/');

  return 0;
}

void gs_encode(uint8_t *ins, int size, uint8_t *result) {
  int i, pos;
  uint8_t trip[3];
  uint8_t kwart[4];

  i = 0;
  while (i < size) {
    for (pos = 0; pos <= 2; pos++, i++) {
      if (i < size) {
        trip[pos] = *ins++;
      } else {
        trip[pos] = '\0';
      }
    }
    kwart[0] = (uint8_t)((trip[0]) >> 2);
    kwart[1] = (uint8_t)((((trip[0]) & 3) << 4) + ((trip[1]) >> 4));
    kwart[2] = (uint8_t)((((trip[1]) & 15) << 2) + ((trip[2]) >> 6));
    kwart[3] = (uint8_t)((trip[2]) & 63);
    for (pos = 0; pos <= 3; pos++) {
      *result++ = encode_ct(kwart[pos]);
    }
  }
  *result = '\0';
}
