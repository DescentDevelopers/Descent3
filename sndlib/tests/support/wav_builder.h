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

#pragma once

#include <cstdint>
#include <vector>

#include "d3_test/byte_writer.h"

class WavBuilder {
public:
  WavBuilder &Raw(const std::vector<uint8_t> &bytes) {
    body_.Bytes(bytes);
    return *this;
  }
  WavBuilder &Fmt(uint16_t format_tag, uint16_t channels, uint32_t rate, uint16_t bits) {
    d3_test::ByteWriter c;
    c.U16(format_tag);
    c.U16(channels);
    c.U32(rate);
    c.U32(rate * channels * (bits / 8)); // average bytes per second
    c.U16(static_cast<uint16_t>(channels * (bits / 8))); // block align
    c.U16(bits);
    return Chunk("fmt ", c);
  }
  WavBuilder &PcmFmt(uint16_t bits) { return Fmt(1, 1, 22050, bits); }
  WavBuilder &Data8(const std::vector<uint8_t> &samples) { return Chunk("data", samples); }
  WavBuilder &Data16(const std::vector<int16_t> &samples) {
    d3_test::ByteWriter c;
    for (int16_t s : samples)
      c.I16(s);
    return Chunk("data", c);
  }
  WavBuilder &Chunk(const char id[5], const std::vector<uint8_t> &payload) {
    body_.Bytes(id, 4).U32(static_cast<uint32_t>(payload.size())).Bytes(payload);
    return *this;
  }

  std::vector<uint8_t> Build(const char riff[5] = "RIFF", const char wave[5] = "WAVE") const {
    d3_test::ByteWriter out;
    out.Bytes(riff, 4).U32(static_cast<uint32_t>(body_.size() + 4)).Bytes(wave, 4).Bytes(body_);
    return out.bytes();
  }

private:
  d3_test::ByteWriter body_;
};
