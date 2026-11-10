/*
 * Descent 3
 * Copyright (C) 2026 Descent Developers
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

#ifndef BITMAP_TESTS_IMAGE_BUILDERS_H
#define BITMAP_TESTS_IMAGE_BUILDERS_H

#include <cstdint>
#include <vector>

#include "d3_test/byte_writer.h"

constexpr uint8_t kTgaTrueColor = 2;
constexpr uint8_t kTgaTrueColorRle = 10;
// Descriptor: low nibble = alpha bits, bit 5 = origin at top-left
constexpr uint8_t kTgaBottomUp = 0x08;
constexpr uint8_t kTgaTopDown = 0x28;

struct Bgra {
  uint8_t b, g, r, a;
};

d3_test::ByteWriter TgaHeader(uint8_t type, int w, int h, uint8_t pixsize, uint8_t descriptor,
                              const char *image_id = "");

void PutPixel(d3_test::ByteWriter &out, const Bgra &p, uint8_t pixsize);

std::vector<uint8_t> RawTga(const std::vector<std::vector<Bgra>> &rows_top_first, uint8_t pixsize, bool top_down,
                            const char *image_id = "");

// RLE TGA (bottom-up), each row encoded separately: runs of identical pixels
// become run packets, everything else raw packets.
std::vector<uint8_t> RleTga(const std::vector<std::vector<Bgra>> &rows_top_first, uint8_t pixsize);

struct Rgb {
  uint8_t r, g, b;
};

d3_test::ByteWriter PcxHeader(int w, int h, uint8_t planes, uint16_t bytes_per_line);

// PCX RLE for one scanline: runs, and any byte >= 0xC0, become count/value pairs.
void PcxEncodeLine(d3_test::ByteWriter &out, const std::vector<uint8_t> &line);

std::vector<uint8_t> Pcx8(const std::vector<std::vector<uint8_t>> &rows, const std::vector<Rgb> &palette);

std::vector<uint8_t> Pcx24(const std::vector<std::vector<Rgb>> &rows);

std::vector<uint8_t> IffPbm(int w, int h, uint8_t masking, uint16_t transparent, const std::vector<Rgb> &palette,
                            uint8_t compression, const std::vector<uint8_t> &body, const char *form_type = "PBM ");

#endif
