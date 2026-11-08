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

#include <cstring>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "gametexture.h"
#include "sounds.h"
#include "soundload.h"

#include "support/sdl_assert_guard.h"
#include "support/soundload_test_support.h"

namespace {

class SoundTableTest : public ::testing::Test {
protected:
  void SetUp() override { InitSounds(); }
  void TearDown() override {
    InitSounds();
    for (int t : touched_textures_)
      GameTextures[t] = texture{};
  }

  SdlAssertGuard assert_guard_;
  std::vector<int> touched_textures_;
};

TEST_F(SoundTableTest, AllocSoundNeverHandsOutReservedSlotZero) {
  // Sound index 0 means "default/none" throughout the game.
  for (int i = 0; i < 10; ++i)
    EXPECT_NE(AllocSound(), 0);
}

TEST_F(SoundTableTest, FreedSoundIsNoLongerFound) {
  const int a = AddSound("Alpha");
  const int b = AddSound("Beta");
  FreeSound(a);
  EXPECT_EQ(FindSoundName("Alpha"), -1);
  EXPECT_EQ(FindSoundName("Beta"), b);
}

TEST_F(SoundTableTest, FindSoundNameIsCaseInsensitive) {
  const int idx = AddSound("BreakingGlass");
  EXPECT_EQ(FindSoundName("breakingglass"), idx);
  EXPECT_EQ(FindSoundName("BREAKINGGLASS"), idx);
  EXPECT_EQ(FindSoundName("BreakingGlas"), -1);
}

TEST_F(SoundTableTest, NextAndPrevSoundWalkUsedSlotsAndWrapAround) {
  EXPECT_EQ(GetNextSound(0), -1);
  EXPECT_EQ(GetPrevSound(0), -1);

  const int a = AddSound("A");
  const int b = AddSound("B");
  const int c = AddSound("C");
  FreeSound(b);

  EXPECT_EQ(GetNextSound(a), c);
  EXPECT_EQ(GetNextSound(c), a);
  EXPECT_EQ(GetPrevSound(c), a);
  EXPECT_EQ(GetPrevSound(a), c);

  FreeSound(c);
  EXPECT_EQ(GetNextSound(a), a);
  EXPECT_EQ(GetPrevSound(a), a);
}

TEST_F(SoundTableTest, RemapSoundsMovesStaticSoundsToFixedSlotsAndFixesReferences) {
  // Some non-static sounds occupy the low slots that static sounds belong in, and
  // the static sounds are loaded in reverse order.
  const int filler_a = AddSound("FillerA");
  const int filler_b = AddSound("FillerB");
  for (int i = NUM_STATIC_SOUNDS - 1; i >= 0; --i)
    AddSound(Static_sound_names[i]);

  SetTextureSound(touched_textures_, 10, FindSoundName(Static_sound_names[SOUND_REFUELING]));
  SetTextureSound(touched_textures_, 11, FindSoundName(Static_sound_names[SOUND_BREAKING_GLASS]));
  SetTextureSound(touched_textures_, 12, filler_a);
  SetTextureSound(touched_textures_, 13, filler_b);

  RemapSounds();

  for (int i = 0; i < NUM_STATIC_SOUNDS; ++i)
    EXPECT_EQ(FindSoundName(Static_sound_names[i]), i) << Static_sound_names[i];

  const int new_filler_a = FindSoundName("FillerA");
  const int new_filler_b = FindSoundName("FillerB");
  EXPECT_GE(new_filler_a, NUM_STATIC_SOUNDS);
  EXPECT_GE(new_filler_b, NUM_STATIC_SOUNDS);

  EXPECT_EQ(GameTextures[10].sound, SOUND_REFUELING);
  EXPECT_EQ(GameTextures[11].sound, SOUND_BREAKING_GLASS);
  EXPECT_EQ(GameTextures[12].sound, new_filler_a);
  EXPECT_EQ(GameTextures[13].sound, new_filler_b);
}

TEST(ChangeSoundFileName, ReplacesExtensionWithWav) {
  char dest[256] = {};
  ChangeSoundFileName("Explosion.ogg", dest);
  EXPECT_STREQ(dest, "Explosion.wav");
}

} // namespace
