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

#include <gtest/gtest.h>

#include <algorithm>
#include <cstring>
#include <iterator>
#include <string>

#include "md5.h"
#include "support/digest.h"
#include "support/test_data.h"

namespace {

// Reference digests of counting_bytes(n), computed independently with Python's hashlib:
//   hashlib.md5(bytes(i & 0xff for i in range(n))).hexdigest()
struct LengthVector {
  std::size_t length;
  const char *hex;
};

const char *const kCounting64 = "b2d3f56bc197fd985d5965079b5e7148";
const char *const kCounting128 = "37eff01866ba3f538421b30b7cbefcac";
const char *const kCounting1000 = "cbecbdb0fdd5cec1e242493b6008cc79";

const LengthVector kCountingVectors[] = {
    {1, "93b885adfe0da089cdf634904fd59f71"},
    {55, "6912ee65fff2d9f9ce2508cddf8bcda0"},
    {56, "51fdd1acda72405dfdfa03fcb85896d7"},
    {57, "5320ef4c17ef34a0cf2db763338d25eb"},
    {63, "48a6295221902e8e0938f773a7185e72"},
    {64, kCounting64},
    {65, "8bd7053801c768420faf816fadba971c"},
    {119, "1c772251899a7ff007400b888d6b2042"},
    {120, "b7ba1efc6022e9ed272f00b8831e26e6"},
    {127, "8402b21e7bc7906493bae0dac017f1f9"},
    {128, kCounting128},
    {129, "46f986692847558fc38b0cece591c20f"},
    {1000, kCounting1000},
};

}; // namespace

TEST(MD5, Rfc1321TestSuite) {
  EXPECT_EQ(md5_hex(""), "d41d8cd98f00b204e9800998ecf8427e");
  EXPECT_EQ(md5_hex("a"), "0cc175b9c0f1b6a831c399e269772661");
  EXPECT_EQ(md5_hex("abc"), "900150983cd24fb0d6963f7d28e17f72");
  EXPECT_EQ(md5_hex("message digest"), "f96b697d7cb7938d525a2f31aaf161d0");
  EXPECT_EQ(md5_hex("abcdefghijklmnopqrstuvwxyz"), "c3fcd3d76192e4007dfb496cca67e13b");
  EXPECT_EQ(md5_hex("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"),
            "d174ab98d277d9f5a5611c2c9f419d9f");
  EXPECT_EQ(md5_hex("12345678901234567890123456789012345678901234567890123456789012345678901234567890"),
            "57edf4a22be3c955ac49da2e2107b67a");
}

// Lengths around the padding boundaries: 55 is the longest message whose padding and length fit in the
// final block, 56..63 force an extra padding block, and 64/128 are exact block multiples.
TEST(MD5, PaddingBoundaryLengths) {
  for (const auto &vec : kCountingVectors) {
    auto msg = counting_bytes(vec.length);
    MD5 md5;
    md5.update(msg.data(), msg.size());
    EXPECT_EQ(to_hex(md5.digest()), vec.hex) << "message length " << vec.length;
  }
}

TEST(MD5, ChunkingDoesNotAffectDigest) {
  const auto msg = counting_bytes(1000);

  for (std::size_t chunk : {1, 3, 55, 56, 63, 64, 65, 127, 128, 200, 999}) {
    MD5 md5;
    for (std::size_t pos = 0; pos < msg.size(); pos += chunk)
      md5.update(msg.data() + pos, std::min(chunk, msg.size() - pos));
    EXPECT_EQ(to_hex(md5.digest()), kCounting1000) << "chunk size " << chunk;
  }

  // Irregular split pattern, including empty updates, updates that exactly fill a partially-filled block,
  // and updates larger than a block while the internal buffer is partially filled.
  const std::size_t pattern[] = {0, 1, 63, 0, 5, 130, 64, 7, 57, 200, 2};
  MD5 md5;
  std::size_t pos = 0;
  for (std::size_t i = 0; pos < msg.size(); ++i) {
    std::size_t len = std::min(pattern[i % std::size(pattern)], msg.size() - pos);
    md5.update(msg.data() + pos, len);
    pos += len;
  }
  EXPECT_EQ(to_hex(md5.digest()), kCounting1000);
}

TEST(MD5, DigestDoesNotAlterState) {
  const auto msg = counting_bytes(1000);
  MD5 md5;

  // 57 bytes leaves the tail in the region where finalisation needs two blocks.
  md5.update(msg.data(), 57);
  EXPECT_EQ(to_hex(md5.digest()), "5320ef4c17ef34a0cf2db763338d25eb");
  EXPECT_EQ(to_hex(md5.digest()), "5320ef4c17ef34a0cf2db763338d25eb");

  md5.update(msg.data() + 57, msg.size() - 57);
  EXPECT_EQ(to_hex(md5.digest()), kCounting1000);
}

TEST(MD5, DigestRightAfterUpdateThatFillsPartialBlock) {
  const auto msg = counting_bytes(128);

  MD5 one_block;
  one_block.update(msg.data(), 1);
  one_block.update(msg.data() + 1, 63);
  EXPECT_EQ(to_hex(one_block.digest()), kCounting64);

  MD5 two_blocks;
  two_blocks.update(msg.data(), 1);
  two_blocks.update(msg.data() + 1, 127);
  EXPECT_EQ(to_hex(two_blocks.digest()), kCounting128);
}

TEST(MD5, PointerDigestWritesExactlySixteenBytes) {
  MD5 md5;
  md5.update(reinterpret_cast<const uint8_t *>("abc"), 3);

  uint8_t buf[18];
  std::memset(buf, 0xCC, sizeof(buf));
  md5.digest(buf + 1);

  auto expected = md5.digest();
  EXPECT_EQ(buf[0], 0xCC);
  EXPECT_EQ(0, std::memcmp(buf + 1, expected.data(), expected.size()));
  EXPECT_EQ(buf[17], 0xCC);
}

TEST(MD5, MillionAs) {
  const std::string chunk(1000, 'a');
  MD5 md5;
  for (int i = 0; i < 1000; ++i)
    md5.update(reinterpret_cast<const uint8_t *>(chunk.data()), chunk.size());
  EXPECT_EQ(to_hex(md5.digest()), "7707d6ae4e027c70eea2a935c2296f21");
}

TEST(MD5, TypedUpdatesHashLittleEndianFixedWidthBytes) {
  EXPECT_EQ(digest_of_typed(static_cast<int>(0x01020304)), digest_of_bytes({0x04, 0x03, 0x02, 0x01}));
  EXPECT_EQ(digest_of_typed(static_cast<int>(-2)), digest_of_bytes({0xFE, 0xFF, 0xFF, 0xFF}));
  EXPECT_EQ(digest_of_typed(static_cast<uint32_t>(0xDEADBEEFU)), digest_of_bytes({0xEF, 0xBE, 0xAD, 0xDE}));
  EXPECT_EQ(digest_of_typed(static_cast<int16_t>(0x0102)), digest_of_bytes({0x02, 0x01}));
  EXPECT_EQ(digest_of_typed(static_cast<int16_t>(-3)), digest_of_bytes({0xFD, 0xFF}));
  EXPECT_EQ(digest_of_typed(static_cast<uint8_t>(0xAB)), digest_of_bytes({0xAB}));
  // IEEE 754 single precision: 1.0f == 0x3F800000, -2.5f == 0xC0200000.
  EXPECT_EQ(digest_of_typed(1.0f), digest_of_bytes({0x00, 0x00, 0x80, 0x3F}));
  EXPECT_EQ(digest_of_typed(-2.5f), digest_of_bytes({0x00, 0x00, 0x20, 0xC0}));
}

TEST(D3, MD5) {
  EXPECT_EQ(hexdigest_after_n_rounds_of_fuzz(0), "d41d8cd98f00b204e9800998ecf8427e");
  EXPECT_EQ(hexdigest_after_n_rounds_of_fuzz(1), "c56c005504f5b9b17df9d83f1106e9b2");
  EXPECT_EQ(hexdigest_after_n_rounds_of_fuzz(2), "98327a3f4ee311c9bdc8613508922c95");
  EXPECT_EQ(hexdigest_after_n_rounds_of_fuzz(4), "f2343ce526646c087df92d708e883675");
  EXPECT_EQ(hexdigest_after_n_rounds_of_fuzz(8), "73396b338a0a769c4e6ae5b7b0bc187e");
  EXPECT_EQ(hexdigest_after_n_rounds_of_fuzz(100), "99b30c1dcc42d97c3d914c26a14bb1d7");
  EXPECT_EQ(hexdigest_after_n_rounds_of_fuzz(1999), "82eeff8c7d574c8232b0ca6ca2c9dd40");
}
