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

// Only the argv overload of GatherArgs is exercised: the single-string overload keeps its write position in a
// function-local static that is never reset, so it can only be called meaningfully once per process.

#include <cstring>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "args.h"
#include "support/gather_args.h"

namespace {

class ArgsTest : public ::testing::Test {
protected:
  void SetUp() override {
    memset(GameArgs, 0, sizeof(GameArgs));
  }
};

TEST_F(ArgsTest, FindArgReturnsIndexOfPresentArgumentAndZeroWhenAbsent) {
  Gather({"Descent3", "-nosound", "-width", "640"});

  EXPECT_EQ(FindArg("-nosound"), 1);
  EXPECT_EQ(FindArg("-width"), 2);
  EXPECT_EQ(FindArg("-height"), 0);
}

TEST_F(ArgsTest, FindArgNeverMatchesProgramName) {
  // Index 0 is the program name and 0 is also the "not found" result.
  Gather({"Descent3", "-x"});

  EXPECT_EQ(FindArg("Descent3"), 0);
}

TEST_F(ArgsTest, FindArgIsCaseInsensitive) {
  Gather({"Descent3", "-NOSOUND"});

  EXPECT_EQ(FindArg("-nosound"), 1);
  EXPECT_EQ(FindArg("-NoSound"), 1);
}

TEST_F(ArgsTest, FindArgTreatsDashDoubleDashAndPlusPrefixesAsEquivalent) {
  Gather({"Descent3", "--setdir", "+launched", "-dedicated"});

  EXPECT_EQ(FindArg("-setdir"), 1);
  EXPECT_EQ(FindArg("+setdir"), 1);
  EXPECT_EQ(FindArg("--launched"), 2);
  EXPECT_EQ(FindArg("-launched"), 2);
  EXPECT_EQ(FindArg("--dedicated"), 3);
  EXPECT_EQ(FindArg("dedicated"), 3);
}

TEST_F(ArgsTest, FindArgTreatsBarePrefixTokensLiterally) {
  Gather({"Descent3", "--", "-x"});

  EXPECT_EQ(FindArg("--"), 1);
  EXPECT_EQ(FindArg("-"), 0);
  EXPECT_EQ(FindArg("+"), 0);
}

TEST_F(ArgsTest, FindArgDoesNotMatchOnPrefixOfALongerArgument) {
  Gather({"Descent3", "-widthx"});

  EXPECT_EQ(FindArg("-width"), 0);
}

TEST_F(ArgsTest, FindArgStartIndexAllowsFindingRepeatedArguments) {
  Gather({"Descent3", "-additionaldir", "a", "-additionaldir", "b"});

  int first = FindArg("-additionaldir");
  ASSERT_EQ(first, 1);
  int second = FindArg("-additionaldir", first + 1);
  ASSERT_EQ(second, 3);
  EXPECT_EQ(FindArg("-additionaldir", second + 1), 0);
}

TEST_F(ArgsTest, GetArgReturnsValueFollowingAFlag) {
  Gather({"Descent3", "-width", "1024", "-height"});

  const char *width = GetArg(FindArg("-width") + 1);
  ASSERT_NE(width, nullptr);
  EXPECT_STREQ(width, "1024");

  EXPECT_EQ(GetArg(FindArg("-height") + 1), nullptr);
}

TEST_F(ArgsTest, GetArgRejectsProgramNameAndOutOfRangeIndices) {
  Gather({"Descent3", "-a", "-b"});

  EXPECT_EQ(GetArg(0), nullptr);
  EXPECT_STREQ(GetArg(1), "-a");
  EXPECT_STREQ(GetArg(2), "-b");
  EXPECT_EQ(GetArg(3), nullptr);
}

TEST_F(ArgsTest, FindArgCharMatchesExactSingleCharacterFlag) {
  Gather({"Descent3", "-w", "640"});

  EXPECT_EQ(FindArgChar("-windowed", 'w'), 1);
}

TEST_F(ArgsTest, FindArgCharFallsBackToLongForm) {
  Gather({"Descent3", "--windowed"});

  EXPECT_EQ(FindArgChar("-windowed", 'w'), 1);
}

TEST_F(ArgsTest, FindArgCharDoesNotMatchLongerArgumentsStartingWithTheCharacter) {
  Gather({"Descent3", "-width", "640"});

  EXPECT_EQ(FindArgChar("-windowed", 'w'), 0);
}

TEST_F(ArgsTest, RegatheringReplacesPreviousArguments) {
  Gather({"Descent3", "-a"});
  Gather({"Descent3", "-b"});

  EXPECT_EQ(FindArg("-b"), 1);
  EXPECT_EQ(FindArg("-a"), 0);
}

} // namespace
