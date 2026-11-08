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

#include <algorithm>
#include <vector>
#include <gtest/gtest.h>

#include "ddio.h"

#include "support/key_queue.h"

// KEY_DELETE (the debug modifier in _DEBUG builds) and KEY_CMD also fold into queued keys, so the queue tests avoid
// them.

namespace {

// Plain keys that add no modifier flags when held
const std::vector<int> kLetters = {KEY_A, KEY_B, KEY_C, KEY_D, KEY_E, KEY_F, KEY_G, KEY_H, KEY_I,
                                   KEY_J, KEY_K, KEY_L, KEY_M, KEY_N, KEY_O, KEY_P, KEY_Q, KEY_R,
                                   KEY_S, KEY_T, KEY_U, KEY_V, KEY_W, KEY_X, KEY_Y, KEY_Z};

struct KeyQueue : testing::Test {
  KeyQueue() { ResetKeyboard(); }
  ~KeyQueue() override { ResetKeyboard(); }
};

} // namespace

TEST_F(KeyQueue, EmptyQueueReturnsZero) {
  EXPECT_EQ(ddio_KeyPeek(), 0);
  EXPECT_EQ(ddio_KeyInKey(), 0);
}

TEST_F(KeyQueue, KeysComeOutInPressOrder) {
  Tap(KEY_C);
  Tap(KEY_A);
  Tap(KEY_B);
  EXPECT_EQ(DrainQueue(), (std::vector<int>{KEY_C, KEY_A, KEY_B}));
}

TEST_F(KeyQueue, PeekDoesNotConsume) {
  Tap(KEY_A);
  Tap(KEY_B);
  EXPECT_EQ(ddio_KeyPeek(), KEY_A);
  EXPECT_EQ(ddio_KeyPeek(), KEY_A);
  EXPECT_EQ(ddio_KeyInKey(), KEY_A);
  EXPECT_EQ(ddio_KeyPeek(), KEY_B);
  EXPECT_EQ(ddio_KeyInKey(), KEY_B);
  EXPECT_EQ(ddio_KeyPeek(), 0);
}

TEST_F(KeyQueue, ReleaseDoesNotQueue) {
  ddio_UpdateKeyState(KEY_A, true);
  EXPECT_EQ(ddio_KeyInKey(), KEY_A);
  ddio_UpdateKeyState(KEY_A, false);
  EXPECT_EQ(ddio_KeyInKey(), 0);
}

TEST_F(KeyQueue, HeldKeyIsOnePress) {
  ddio_UpdateKeyState(KEY_A, true);
  ddio_UpdateKeyState(KEY_A, true);
  ddio_UpdateKeyState(KEY_A, true);
  EXPECT_EQ(DrainQueue(), std::vector<int>{KEY_A});
  EXPECT_EQ(ddio_KeyDownCount(KEY_A), 1);
}

TEST_F(KeyQueue, SurvivesWrapAround) {
  for (int i = 0; i < 100; i++) {
    const int key = kLetters[i % kLetters.size()];
    Tap(key);
    ASSERT_EQ(ddio_KeyInKey(), key) << "iteration " << i;
    ASSERT_EQ(ddio_KeyInKey(), 0) << "iteration " << i;
  }
}

// The queue is a small fixed buffer, so typing far ahead of the reader loses keys.
TEST_F(KeyQueue, OverflowKeepsOrderAndRecovers) {
  std::vector<int> typed;
  for (int round = 0; round < 3; round++) {
    typed.insert(typed.end(), kLetters.begin(), kLetters.end());
  }
  for (int key : typed) {
    Tap(key);
  }

  std::vector<int> got = DrainQueue();
  ASSERT_FALSE(got.empty());
  EXPECT_LT(got.size(), typed.size());
  EXPECT_NE(std::search(typed.begin(), typed.end(), got.begin(), got.end()), typed.end())
      << "the keys that came out are not a contiguous, in-order run of the keys typed";

  Tap(KEY_Z);
  EXPECT_EQ(DrainQueue(), std::vector<int>{KEY_Z});
}

TEST_F(KeyQueue, HeldModifiersAreFoldedIntoQueuedKey) {
  ddio_UpdateKeyState(KEY_LSHIFT, true);
  DrainQueue();

  Tap(KEY_A);
  EXPECT_EQ(ddio_KeyInKey(), KEY_A | KEY_SHIFTED);
}

TEST_F(KeyQueue, RightHandModifiersCountToo) {
  ddio_UpdateKeyState(KEY_RSHIFT, true);
  ddio_UpdateKeyState(KEY_RCTRL, true);
  ddio_UpdateKeyState(KEY_RALT, true);
  DrainQueue();

  Tap(KEY_X);
  EXPECT_EQ(ddio_KeyInKey(), KEY_X | KEY_SHIFTED | KEY_CTRLED | KEY_ALTED);
}

TEST_F(KeyQueue, ReleasedModifierNoLongerApplies) {
  Tap(KEY_LCTRL);
  Tap(KEY_LALT);
  DrainQueue();

  Tap(KEY_X);
  EXPECT_EQ(ddio_KeyInKey(), KEY_X);
}

TEST_F(KeyQueue, DownCountCountsPressesAndResetsOnRead) {
  Tap(KEY_A);
  Tap(KEY_A);
  Tap(KEY_A);
  Tap(KEY_B);
  EXPECT_EQ(ddio_KeyDownCount(KEY_A), 3);
  EXPECT_EQ(ddio_KeyDownCount(KEY_A), 0);
  EXPECT_EQ(ddio_KeyDownCount(KEY_B), 1);
}

TEST_F(KeyQueue, FlushKeyForgetsStateAndCount) {
  ddio_UpdateKeyState(KEY_A, true);
  ddio_KeyFlushKey(KEY_A);
  EXPECT_FALSE(ddio_GetAdjKeyState(KEY_A));
  EXPECT_EQ(ddio_KeyDownCount(KEY_A), 0);

  ddio_UpdateKeyState(KEY_A, true);
  EXPECT_EQ(ddio_KeyDownCount(KEY_A), 1);
}

TEST_F(KeyQueue, AdjKeyStateFollowsRawKey) {
  EXPECT_FALSE(ddio_GetAdjKeyState(KEY_A));
  ddio_UpdateKeyState(KEY_A, true);
  EXPECT_TRUE(ddio_GetAdjKeyState(KEY_A));
  ddio_UpdateKeyState(KEY_A, false);
  EXPECT_FALSE(ddio_GetAdjKeyState(KEY_A));
}

TEST_F(KeyQueue, AdjKeyStateNeedsRequestedModifier) {
  ddio_UpdateKeyState(KEY_A, true);
  EXPECT_FALSE(ddio_GetAdjKeyState(KEY_A | KEY_SHIFTED));
  EXPECT_FALSE(ddio_GetAdjKeyState(KEY_A | KEY_CTRLED));
  EXPECT_FALSE(ddio_GetAdjKeyState(KEY_A | KEY_ALTED));

  ddio_UpdateKeyState(KEY_LSHIFT, true);
  EXPECT_TRUE(ddio_GetAdjKeyState(KEY_A | KEY_SHIFTED));
  EXPECT_FALSE(ddio_GetAdjKeyState(KEY_A | KEY_SHIFTED | KEY_CTRLED));

  ddio_UpdateKeyState(KEY_RCTRL, true);
  EXPECT_TRUE(ddio_GetAdjKeyState(KEY_A | KEY_SHIFTED | KEY_CTRLED));
}

TEST_F(KeyQueue, AdjKeyStateAcceptsEitherSideModifier) {
  ddio_UpdateKeyState(KEY_A, true);
  for (int modkey : {KEY_LALT, KEY_RALT}) {
    ddio_UpdateKeyState(modkey, true);
    EXPECT_TRUE(ddio_GetAdjKeyState(KEY_A | KEY_ALTED)) << "modifier key " << modkey;
    ddio_UpdateKeyState(modkey, false);
  }
}

TEST_F(KeyQueue, AdjKeyStateNeedsRawKeyEvenWithModifierHeld) {
  ddio_UpdateKeyState(KEY_LSHIFT, true);
  EXPECT_FALSE(ddio_GetAdjKeyState(KEY_A | KEY_SHIFTED));
}

TEST(KeyAscii, PrintableAsciiRoundTrips) {
  for (int c = ' '; c <= '~'; c++) {
    const int key = ddio_AsciiToKey(c);
    EXPECT_NE(key, 0) << "no key for '" << static_cast<char>(c) << "'";
    EXPECT_EQ(ddio_KeyToAscii(key), c) << "'" << static_cast<char>(c) << "' -> key 0x" << std::hex << key;
  }
}

TEST(KeyAscii, UppercaseIsShiftedLowercase) {
  for (int c = 'a'; c <= 'z'; c++) {
    const int lower = ddio_AsciiToKey(c);
    EXPECT_EQ(lower & KEY_SHIFTED, 0) << static_cast<char>(c);
    EXPECT_EQ(ddio_AsciiToKey(c - 'a' + 'A'), lower | KEY_SHIFTED) << static_cast<char>(c);
  }
}

TEST(KeyAscii, KeypadDigitsTypeDigits) {
  const int pad[10] = {KEY_PAD0, KEY_PAD1, KEY_PAD2, KEY_PAD3, KEY_PAD4,
                       KEY_PAD5, KEY_PAD6, KEY_PAD7, KEY_PAD8, KEY_PAD9};
  for (int i = 0; i < 10; i++) {
    EXPECT_EQ(ddio_KeyToAscii(pad[i]), '0' + i);
  }
}

// 255 is the "no character" value callers test for
TEST(KeyAscii, NonCharacterKeysGiveNoCharacter) {
  for (int key : {KEY_F1, KEY_F12, KEY_LSHIFT, KEY_RSHIFT, KEY_LCTRL, KEY_RALT, KEY_CAPSLOCK, KEY_UP, KEY_HOME,
                  KEY_DELETE, KEY_PADENTER}) {
    EXPECT_EQ(ddio_KeyToAscii(key), 255) << "key 0x" << std::hex << key;
    EXPECT_EQ(ddio_KeyToAscii(key | KEY_SHIFTED), 255) << "shifted key 0x" << std::hex << key;
  }
}

TEST(KeyAscii, AsciiOutOfRangeGivesNoKey) {
  for (int c : {-1, -128, 128, 200, 255, 1000}) {
    EXPECT_EQ(ddio_AsciiToKey(c), 0) << c;
  }
}
