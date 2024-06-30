/* Descent 3
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

#include "md5.h"
#include <byteswap.h>

#include <algorithm>

namespace {

const uint32_t md5_round_shifts[] = {7, 12, 17, 22, 5, 9, 14, 20, 4, 11, 16, 23, 6, 10, 15, 21};

const std::uint32_t md5_round_constants[] = {
    0xD76AA478UL, 0xE8C7B756UL, 0x242070DBUL, 0xC1BDCEEEUL, 0xF57C0FAFUL, 0x4787C62AUL, 0xA8304613UL, 0xFD469501UL,
    0x698098D8UL, 0x8B44F7AFUL, 0xFFFF5BB1UL, 0x895CD7BEUL, 0x6B901122UL, 0xFD987193UL, 0xA679438EUL, 0x49B40821UL,
    0xF61E2562UL, 0xC040B340UL, 0x265E5A51UL, 0xE9B6C7AAUL, 0xD62F105DUL, 0x02441453UL, 0xD8A1E681UL, 0xE7D3FBC8UL,
    0x21E1CDE6UL, 0xC33707D6UL, 0xF4D50D87UL, 0x455A14EDUL, 0xA9E3E905UL, 0xFCEFA3F8UL, 0x676F02D9UL, 0x8D2A4C8AUL,
    0xFFFA3942UL, 0x8771F681UL, 0x6D9D6122UL, 0xFDE5380CUL, 0xA4BEEA44UL, 0x4BDECFA9UL, 0xF6BB4B60UL, 0xBEBFBC70UL,
    0x289B7EC6UL, 0xEAA127FAUL, 0xD4EF3085UL, 0x04881D05UL, 0xD9D4D039UL, 0xE6DB99E5UL, 0x1FA27CF8UL, 0xC4AC5665UL,
    0xF4292244UL, 0x432AFF97UL, 0xAB9423A7UL, 0xFC93A039UL, 0x655B59C3UL, 0x8F0CCC92UL, 0xFFEFF47DUL, 0x85845DD1UL,
    0x6FA87E4FUL, 0xFE2CE6E0UL, 0xA3014314UL, 0x4E0811A1UL, 0xF7537E82UL, 0xBD3AF235UL, 0x2AD7D2BBUL, 0xEB86D391UL};

std::uint32_t rotl_uint32(std::uint32_t a, std::uint32_t b) noexcept { return b ? (a << b) | (a >> (32 - b)) : a; };

}; // namespace

void MD5::round(std::array<std::uint32_t, 4> &sums, const uint8_t *block) const noexcept {
  // break 64 bytes into 16 dwords
  std::uint32_t m[16];
  for (std::size_t i = 0; i < 16; ++i) {
    m[i] = (static_cast<std::uint32_t>(block[(4 * i)])) | (static_cast<std::uint32_t>(block[(4 * i) + 1]) << 8) |
           (static_cast<std::uint32_t>(block[(4 * i) + 2]) << 16) |
           (static_cast<std::uint32_t>(block[(4 * i) + 3]) << 24);
  }

  std::uint32_t a = sums[0], b = sums[1], c = sums[2], d = sums[3];

  // 64 rounds...
  for (uint32_t r = 0; r < 64; ++r) {
    std::uint32_t f, g;
    uint32_t s = md5_round_shifts[((r >> 2) & ~3) | (r & 3)];
    switch (r >> 4) {
    case 0:
      f = (b & c) | (~b & d), g = r;
      break;
    case 1:
      f = (b & d) | (c & ~d), g = (5 * r + 1) & 15;
      break;
    case 2:
      f = b ^ c ^ d, g = (3 * r + 5) & 15;
      break;
    case 3:
      f = c ^ (b | ~d), g = (7 * r) & 15;
      break;
    default:; // unreachable
    }
    f = rotl_uint32(f + a + m[g] + md5_round_constants[r], s) + b;
    a = d;
    d = c;
    c = b;
    b = f;
  }

  sums[0] += a, sums[1] += b, sums[2] += c, sums[3] += d;
}

void MD5::round(const uint8_t *block) noexcept { round(sums_, block); }

void MD5::update(const uint8_t *data, std::size_t n) noexcept {
  message_len_ += n * 8;

  // if there is already some data in tmpbuf, check if we would fill it
  if (tmpbuf_n_) {
    if ((n >= tmpbuf_.size() || tmpbuf_n_ + n >= tmpbuf_.size())) {
      // we would fill tmpbuf at least once; handle partial block first.
      std::size_t remaining = tmpbuf_.size() - tmpbuf_n_;
      std::copy(data, data + remaining, tmpbuf_.begin() + tmpbuf_n_);
      data += remaining;
      n -= remaining;
      round(tmpbuf_.data());
      tmpbuf_n_ = 0;
    } else {
      // we will not fill the buffer - just copy and return
      std::copy(data, data + n, tmpbuf_.begin() + tmpbuf_n_);
      tmpbuf_n_ += n;
      return;
    }
  }

  // do rounds on full blocks for as long as we can
  while (n >= tmpbuf_.size()) {
    round(data);
    data += tmpbuf_.size();
    n -= tmpbuf_.size();
  }

  if (n) {
    // copy partial block
    std::copy(data, data + n, tmpbuf_.begin());
    tmpbuf_n_ = n;
  }
}

void MD5::place_length(uint8_t *destination) const noexcept {
  for (std::size_t i = 0; i < 8; ++i)
    destination[i] = static_cast<uint8_t>(message_len_ >> (8 * i));
}

std::array<uint8_t, 16> MD5::digest() const noexcept {
  // copies of sums and buffers
  auto sums = sums_;
  auto buf = tmpbuf_;
  std::size_t n = tmpbuf_n_;

  if (n >= 56) {
    // must append current block, required padding bit and length won't fit.
    // n is never buf.size() (= 64) yet
    buf[n++] = 0x80U;
    while (n < buf.size())
      buf[n++] = 0;
    round(sums, buf.data());

    // all zeroes, except for length
    std::fill(buf.begin(), buf.begin() + 56, 0);

  } else { // n < 56
    // append padding to tmpbuf and then the length
    buf[n++] = 0x80U;
    while (n < 56)
      buf[n++] = 0;
  }

  place_length(&buf[56]);
  round(sums, buf.data());

  return {static_cast<uint8_t>(sums[0]),       static_cast<uint8_t>(sums[0] >> 8),  static_cast<uint8_t>(sums[0] >> 16),
          static_cast<uint8_t>(sums[0] >> 24), static_cast<uint8_t>(sums[1]),       static_cast<uint8_t>(sums[1] >> 8),
          static_cast<uint8_t>(sums[1] >> 16), static_cast<uint8_t>(sums[1] >> 24), static_cast<uint8_t>(sums[2]),
          static_cast<uint8_t>(sums[2] >> 8),  static_cast<uint8_t>(sums[2] >> 16), static_cast<uint8_t>(sums[2] >> 24),
          static_cast<uint8_t>(sums[3]),       static_cast<uint8_t>(sums[3] >> 8),  static_cast<uint8_t>(sums[3] >> 16),
          static_cast<uint8_t>(sums[3] >> 24)};
}

void MD5::update(float valin) noexcept {
  float val = INTEL_FLOAT(valin);
  uint8_t *p = (uint8_t *)&val;
  update(p, sizeof(float));
}

void MD5::update(int valin) noexcept {
  int val = INTEL_INT(valin);
  uint8_t *p = (uint8_t *)&val;
  update(p, sizeof(int));
}

void MD5::update(int16_t valin) noexcept {
  int16_t val = INTEL_SHORT(valin);
  uint8_t *p = (uint8_t *)&val;
  update(p, sizeof(int16_t));
}

void MD5::update(uint32_t valin) noexcept {
  uint32_t val = INTEL_INT(valin);
  uint8_t *p = (uint8_t *)&val;
  update(p, sizeof(uint32_t));
}

void MD5::update(uint8_t val) noexcept {
  uint8_t *p = (uint8_t *)&val;
  update(p, sizeof(uint8_t));
}

void MD5::digest(uint8_t *destination) const noexcept {
  auto digest_data = digest();
  std::copy(digest_data.begin(), digest_data.end(), destination);
}
