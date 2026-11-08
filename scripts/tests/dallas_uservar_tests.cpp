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

#include <climits>
#include <cstdint>
#include <variant>

#include <gtest/gtest.h>

#include "osiris_vector.h"
#include "osiris_import.h"
#include "osiris_common.h"
#include "DallasFuncs.h"

#include "support/dallas_fake_engine.h"

namespace {

using namespace dallas_fakes;

class DallasUserState : public ::testing::Test {
protected:
  void SetUp() override {
    // dfInit's assignment keeps each variable's current type, so put every variable back to the
    // default (float) type first; otherwise an int-typed var from an earlier test would leak.
    for (auto &uv : User_vars)
      uv.set_type<float>();
    dfInit();
    for (int i = 0; i < 32; i++)
      aUserFlagSet(i, false);

    File_WriteInt = FakeWriteInt;
    File_WriteFloat = FakeWriteFloat;
    File_WriteByte = FakeWriteByte;
    File_ReadInt = FakeReadInt;
    File_ReadFloat = FakeReadFloat;
    File_ReadByte = FakeReadByte;
    MSafe_GetValue = FakeGetValue;
    MSafe_CallFunction = FakeCallFunction;
    g_move_called = false;
  }
  void TearDown() override {
    File_WriteInt = nullptr;
    File_WriteFloat = nullptr;
    File_WriteByte = nullptr;
    File_ReadInt = nullptr;
    File_ReadFloat = nullptr;
    File_ReadByte = nullptr;
    MSafe_GetValue = nullptr;
    MSafe_CallFunction = nullptr;
    for (auto &uv : User_vars)
      uv.set_type<float>();
  }
};

TEST_F(DallasUserState, FloatUserVarArithmetic) {
  aUserVarSet(3, 2.5f);
  EXPECT_FLOAT_EQ(qUserVarValue(3), 2.5f);
  aUserVarInc(3);
  EXPECT_FLOAT_EQ(qUserVarValue(3), 3.5f);
  aUserVarAdd(1.25f, 3);
  EXPECT_FLOAT_EQ(qUserVarValue(3), 4.75f);
  aUserVarSub(0.75f, 3);
  EXPECT_FLOAT_EQ(qUserVarValue(3), 4.0f);
  EXPECT_FLOAT_EQ(qUserVarValue(2), 0.0f);
  EXPECT_FLOAT_EQ(qUserVarValue(4), 0.0f);
}

TEST_F(DallasUserState, IntegerUserVarKeepsIntegerType) {
  dfInit({4});
  ASSERT_TRUE(std::holds_alternative<int32_t>(User_vars[4]));
  EXPECT_TRUE(std::holds_alternative<float>(User_vars[3]));

  aUserVarSet(4, 1000000.0f);
  aUserVarInc(4);
  aUserVarAdd(9.0f, 4);
  EXPECT_TRUE(std::holds_alternative<int32_t>(User_vars[4]));
  EXPECT_EQ(qUserVarValueInt(4), 1000010);
  EXPECT_FLOAT_EQ(qUserVarValue(4), 1000010.0f);
}

TEST_F(DallasUserState, OutOfRangeUserVarIsIgnored) {
  aUserVarSet(-1, 7.0f);
  aUserVarSet(MAX_USER_VARS, 7.0f);
  aUserVarInc(MAX_USER_VARS);
  aUserVarAdd(1.0f, -1);
  EXPECT_EQ(qUserVarValue(-1), 0.0f);
  EXPECT_EQ(qUserVarValue(MAX_USER_VARS), 0.0f);
  EXPECT_EQ(qUserVarValueInt(MAX_USER_VARS), 0);
  for (int i = 0; i < MAX_USER_VARS; i++)
    EXPECT_EQ(qUserVarValue(i), 0.0f) << "var " << i;
}

TEST_F(DallasUserState, UserVarValueIntRoundsPositiveValues) {
  aUserVarSet(0, 2.4f);
  EXPECT_EQ(qUserVarValueInt(0), 2);
  aUserVarSet(0, 2.6f);
  EXPECT_EQ(qUserVarValueInt(0), 3);
  aUserVarSet(0, 2.5f);
  EXPECT_EQ(qUserVarValueInt(0), 3);
  aUserVarSet(0, 0.0f);
  EXPECT_EQ(qUserVarValueInt(0), 0);
}

TEST_F(DallasUserState, UserVarValueIntSaturatesOutOfRangeFloats) {
  aUserVarSet(0, 1e20f);
  EXPECT_EQ(qUserVarValueInt(0), INT_MAX);
  aUserVarSet(0, -1e20f);
  EXPECT_EQ(qUserVarValueInt(0), INT_MIN);
}

TEST_F(DallasUserState, UserFlagsAreIndependentBits) {
  aUserFlagSet(0, true);
  aUserFlagSet(5, true);
  aUserFlagSet(31, true);
  for (int i = 0; i < 32; i++)
    EXPECT_EQ(qUserFlag(i), i == 0 || i == 5 || i == 31) << "flag " << i;

  aUserFlagSet(5, false);
  EXPECT_FALSE(qUserFlag(5));
  EXPECT_TRUE(qUserFlag(0));
  EXPECT_TRUE(qUserFlag(31));
}

TEST_F(DallasUserState, OutOfRangeUserFlagIsIgnored) {
  aUserFlagSet(32, true);
  aUserFlagSet(-1, true);
  EXPECT_FALSE(qUserFlag(32));
  EXPECT_FALSE(qUserFlag(-1));
  for (int i = 0; i < 32; i++)
    EXPECT_FALSE(qUserFlag(i)) << "flag " << i;
}

TEST_F(DallasUserState, InitResetsState) {
  dfInit({2});
  aUserVarSet(0, 3.0f);
  aUserVarSet(2, 3.0f);
  aUserFlagSet(7, true);
  Spew_handles[0] = 12;
  Saved_object_handles[MAX_SAVED_OBJECT_HANDLES - 1] = 34;

  dfInit({2});
  EXPECT_EQ(qUserVarValue(0), 0.0f);
  EXPECT_EQ(qUserVarValueInt(2), 0);
  EXPECT_TRUE(std::holds_alternative<int32_t>(User_vars[2]));
  EXPECT_FALSE(qUserFlag(7));
  for (int h : Spew_handles)
    EXPECT_EQ(h, -1);
  for (int h : Saved_object_handles)
    EXPECT_EQ(h, OBJECT_HANDLE_NONE);
}

// dfRestore must consume exactly the bytes dfSave produced (level scripts write more data after it).
TEST_F(DallasUserState, SaveRestoreRoundTrip) {
  const int kTrailer = 0x5EED1234;

  dfInit({1, MAX_USER_VARS - 1});
  aUserVarSet(0, 1.5f);
  aUserVarSet(1, 42.0f);
  aUserVarSet(MAX_USER_VARS - 2, -3.25f);
  aUserVarSet(MAX_USER_VARS - 1, -77.0f);
  aUserFlagSet(0, true);
  aUserFlagSet(31, true);
  Spew_handles[0] = 7;
  Spew_handles[MAX_SPEW_HANDLES - 1] = 99;
  Saved_object_handles[0] = 1234;
  Saved_object_handles[MAX_SAVED_OBJECT_HANDLES - 1] = 5678;

  MemStream stream;
  dfSave(&stream);
  FakeWriteInt(kTrailer, &stream);

  dfInit({1, MAX_USER_VARS - 1});
  aUserVarSet(0, 100.0f);
  aUserFlagSet(3, true);

  dfRestore(&stream);
  EXPECT_EQ(FakeReadInt(&stream), kTrailer) << "dfRestore read a different amount than dfSave wrote";
  EXPECT_EQ(stream.read_pos, stream.data.size());

  EXPECT_FLOAT_EQ(qUserVarValue(0), 1.5f);
  EXPECT_EQ(qUserVarValueInt(1), 42);
  EXPECT_TRUE(std::holds_alternative<int32_t>(User_vars[1]));
  EXPECT_FLOAT_EQ(qUserVarValue(MAX_USER_VARS - 2), -3.25f);
  EXPECT_EQ(qUserVarValueInt(MAX_USER_VARS - 1), -77);
  EXPECT_TRUE(std::holds_alternative<int32_t>(User_vars[MAX_USER_VARS - 1]));
  EXPECT_FLOAT_EQ(qUserVarValue(5), 0.0f);
  for (int i = 0; i < 32; i++)
    EXPECT_EQ(qUserFlag(i), i == 0 || i == 31) << "flag " << i;
  EXPECT_EQ(Spew_handles[0], 7);
  EXPECT_EQ(Spew_handles[1], -1);
  EXPECT_EQ(Spew_handles[MAX_SPEW_HANDLES - 1], 99);
  EXPECT_EQ(Saved_object_handles[0], 1234);
  EXPECT_EQ(Saved_object_handles[1], OBJECT_HANDLE_NONE);
  EXPECT_EQ(Saved_object_handles[MAX_SAVED_OBJECT_HANDLES - 1], 5678);
}

TEST_F(DallasUserState, SaveRestorePreservesPositionClipboard) {
  const int kObj = 0x10001;
  g_world_pos.pos = vector{10.0f, -20.5f, 30.25f};
  const matrix kOrient{vector{1.0f, 2.0f, 3.0f}, vector{4.0f, 5.0f, 6.0f}, vector{7.0f, 8.0f, 9.0f}};
  g_world_pos.orient = kOrient;
  g_world_pos.roomnum = 17;
  aStoreObjectInPositionClipboard(kObj);

  MemStream stream;
  dfSave(&stream);

  g_world_pos.pos = vector{-1.0f, -1.0f, -1.0f};
  g_world_pos.orient = matrix{vector{1.0f, 0.0f, 0.0f}, vector{0.0f, 1.0f, 0.0f}, vector{0.0f, 0.0f, 1.0f}};
  g_world_pos.roomnum = 3;
  aStoreObjectInPositionClipboard(kObj);
  dfRestore(&stream);

  aMoveObjectToPositionClipboard(kObj);
  ASSERT_TRUE(g_move_called);
  EXPECT_EQ(g_moved_to.roomnum, 17);
  EXPECT_FLOAT_EQ(g_moved_to.pos.x(), 10.0f);
  EXPECT_FLOAT_EQ(g_moved_to.pos.y(), -20.5f);
  EXPECT_FLOAT_EQ(g_moved_to.pos.z(), 30.25f);
  for (int i = 0; i < 9; i++)
    EXPECT_FLOAT_EQ(g_moved_to.orient.a1d[i], kOrient.a1d[i]) << "orient element " << i;
}

} // namespace
