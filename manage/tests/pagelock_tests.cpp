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

// The lock file is a flat list of fixed-size records: pagetype, page name and
// the user holding the lock ("UNLOCKED" when nobody does). A lock is
// identified by its (name, pagetype) pair; names compare case-insensitively.

#include <cstring>
#include <filesystem>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "cfile.h"
#include "d3_test/files.h"
#include "manage.h"
#include "pstypes.h"

#include "support/manage_pagelock_records.h"

namespace {

using namespace manage_pagelock;

class PagelockTest : public ::testing::Test {
protected:
  void SetUp() override {
    saved_lock_file_ = TableLockFilename;
    strcpy(saved_temp_lock_file_, TempTableLockFilename);
    strcpy(saved_net_dir_, NetD3Dir);
    strcpy(saved_user_, TableUser);
    saved_network_up_ = Network_up;

    const std::string temp_lock_file = PATH_TO_CSTR(dir_ / "table.tmp");
    const std::string net_dir = PATH_TO_CSTR(dir_.path());
    ASSERT_LT(temp_lock_file.size(), TABLE_NAME_LEN) << temp_lock_file;
    ASSERT_LT(net_dir.size(), TABLE_NAME_LEN) << net_dir;

    TableLockFilename = dir_ / "table.lok";
    strcpy(TempTableLockFilename, temp_lock_file.c_str());
    // Non-release builds append to <NetD3Dir>/TableLog when a lock changes.
    strcpy(NetD3Dir, net_dir.c_str());
    strcpy(TableUser, "alice");
    Network_up = 1;
  }

  void TearDown() override {
    TableLockFilename = saved_lock_file_;
    strcpy(TempTableLockFilename, saved_temp_lock_file_);
    strcpy(NetD3Dir, saved_net_dir_);
    strcpy(TableUser, saved_user_);
    Network_up = saved_network_up_;
  }

  d3_test::TempDir dir_;
  std::filesystem::path saved_lock_file_;
  char saved_temp_lock_file_[TABLE_NAME_LEN];
  char saved_net_dir_[TABLE_NAME_LEN];
  char saved_user_[TABLE_NAME_LEN];
  int saved_network_up_;
};

TEST_F(PagelockTest, LocksRoundTripThroughTheLockFile) {
  WriteLockFile({MakeLock(PAGETYPE_TEXTURE, "Lava Flow", "alice"), MakeLock(PAGETYPE_SOUND, "Boom", "UNLOCKED")});

  EXPECT_EQ(ReadLockFile(), (std::vector<LockRecord>{{PAGETYPE_TEXTURE, "Lava Flow", "alice"},
                                                     {PAGETYPE_SOUND, "Boom", "UNLOCKED"}}));
}

TEST_F(PagelockTest, OldRobotLocksAreReadAsGenericLocks) {
  WriteLockFile({MakeLock(PAGETYPE_ROBOT, "Old Bot", "bob")});

  EXPECT_EQ(ReadLockFile(), (std::vector<LockRecord>{{PAGETYPE_GENERIC, "Old Bot", "bob"}}));
}

TEST_F(PagelockTest, CheckIfPageLockedReportsLockState) {
  WriteLockFile({MakeLock(PAGETYPE_TEXTURE, "Free Tex", "UNLOCKED"), MakeLock(PAGETYPE_TEXTURE, "Bobs Tex", "bob"),
                 MakeLock(PAGETYPE_TEXTURE, "My Tex", "ALICE")});

  auto check = [](uint8_t type, const char *name) {
    mngs_Pagelock pl = MakeLock(type, name, "");
    return mng_CheckIfPageLocked(&pl);
  };
  EXPECT_EQ(check(PAGETYPE_TEXTURE, "Free Tex"), 0) << "unlocked page";
  EXPECT_EQ(check(PAGETYPE_TEXTURE, "Bobs Tex"), 1) << "locked by someone else";
  EXPECT_EQ(check(PAGETYPE_TEXTURE, "my tex"), 0) << "locked by the current user (names and users are case-insensitive)";
  EXPECT_EQ(check(PAGETYPE_TEXTURE, "New Tex"), 2) << "page not in the lock file";
  EXPECT_EQ(check(PAGETYPE_SOUND, "Bobs Tex"), 2) << "same name, different page type";
}

TEST_F(PagelockTest, CheckIfPageLockedReturnsTheHolder) {
  WriteLockFile({MakeLock(PAGETYPE_DOOR, "Gate", "bob")});
  mngs_Pagelock pl = MakeLock(PAGETYPE_DOOR, "Gate", "");
  ASSERT_EQ(mng_CheckIfPageLocked(&pl), 1);
  EXPECT_STREQ(pl.holder, "bob");
}

TEST_F(PagelockTest, CheckIfPageOwnedReportsOwnership) {
  WriteLockFile({MakeLock(PAGETYPE_WEAPON, "Laser", "bob")});
  char bob[] = "BOB";
  char carol[] = "carol";

  mngs_Pagelock laser = MakeLock(PAGETYPE_WEAPON, "laser", "");
  EXPECT_EQ(mng_CheckIfPageOwned(&laser, bob), 1);
  EXPECT_EQ(mng_CheckIfPageOwned(&laser, carol), 0);

  mngs_Pagelock missing = MakeLock(PAGETYPE_SHIP, "Laser", "");
  EXPECT_EQ(mng_CheckIfPageOwned(&missing, bob), 2);
}

TEST_F(PagelockTest, GetListOfLocksFiltersByHolder) {
  WriteLockFile({MakeLock(PAGETYPE_TEXTURE, "A", "bob"), MakeLock(PAGETYPE_SOUND, "B", "UNLOCKED"),
                 MakeLock(PAGETYPE_DOOR, "C", "alice"), MakeLock(PAGETYPE_WEAPON, "D", "Bob")});
  mngs_Pagelock list[10];

  char bob[] = "bob";
  ASSERT_EQ(mng_GetListOfLocks(list, 10, bob), 2);
  EXPECT_STREQ(list[0].name, "A");
  EXPECT_STREQ(list[1].name, "D");

  ASSERT_EQ(mng_GetListOfLocks(list, 10, nullptr), 3);
  EXPECT_STREQ(list[0].name, "A");
  EXPECT_STREQ(list[1].name, "C");
  EXPECT_STREQ(list[2].name, "D");
}

TEST_F(PagelockTest, GetListOfLocksStopsAtMax) {
  WriteLockFile({MakeLock(PAGETYPE_TEXTURE, "A", "bob"), MakeLock(PAGETYPE_TEXTURE, "B", "bob"),
                 MakeLock(PAGETYPE_TEXTURE, "C", "bob")});
  mngs_Pagelock list[2];
  char bob[] = "bob";
  EXPECT_EQ(mng_GetListOfLocks(list, 2, bob), 2);
}

TEST_F(PagelockTest, ReplacePagelockUpdatesMatchingLockInPlace) {
  WriteLockFile({MakeLock(PAGETYPE_TEXTURE, "A", "UNLOCKED"), MakeLock(PAGETYPE_SOUND, "B", "UNLOCKED"),
                 MakeLock(PAGETYPE_TEXTURE, "C", "UNLOCKED")});

  mngs_Pagelock pl = MakeLock(PAGETYPE_SOUND, "B", "alice");
  ASSERT_EQ(mng_ReplacePagelock(pl.name, &pl), 1);

  EXPECT_EQ(ReadLockFile(), (std::vector<LockRecord>{{PAGETYPE_TEXTURE, "A", "UNLOCKED"},
                                                     {PAGETYPE_SOUND, "B", "alice"},
                                                     {PAGETYPE_TEXTURE, "C", "UNLOCKED"}}));
}

TEST_F(PagelockTest, ReplacePagelockAppendsUnknownLock) {
  WriteLockFile({MakeLock(PAGETYPE_TEXTURE, "A", "UNLOCKED")});

  mngs_Pagelock pl = MakeLock(PAGETYPE_SOUND, "A", "alice");
  ASSERT_EQ(mng_ReplacePagelock(pl.name, &pl), 1);

  EXPECT_EQ(ReadLockFile(), (std::vector<LockRecord>{{PAGETYPE_TEXTURE, "A", "UNLOCKED"},
                                                     {PAGETYPE_SOUND, "A", "alice"}}));
}

class TrackLockTest : public ::testing::Test {
protected:
  void SetUp() override { mng_InitTrackLocks(); }
  void TearDown() override { mng_InitTrackLocks(); }
};

TEST_F(TrackLockTest, FindMatchesNameCaseInsensitivelyAndPageType) {
  char tex[] = "Lava Flow";
  const int n = mng_AllocTrackLock(tex, PAGETYPE_TEXTURE);
  ASSERT_GE(n, 0);

  char lower[] = "lava flow";
  EXPECT_EQ(mng_FindTrackLock(lower, PAGETYPE_TEXTURE), n);
  EXPECT_EQ(mng_FindTrackLock(lower, PAGETYPE_SOUND), -1);
  char other[] = "Lava";
  EXPECT_EQ(mng_FindTrackLock(other, PAGETYPE_TEXTURE), -1);
}

TEST_F(TrackLockTest, FreedTrackLockIsNotFoundAndItsSlotIsReused) {
  char a[] = "A", b[] = "B", c[] = "C";
  const int na = mng_AllocTrackLock(a, PAGETYPE_DOOR);
  const int nb = mng_AllocTrackLock(b, PAGETYPE_DOOR);
  ASSERT_NE(na, nb);

  mng_FreeTrackLock(na);
  EXPECT_EQ(mng_FindTrackLock(a, PAGETYPE_DOOR), -1);
  EXPECT_EQ(mng_FindTrackLock(b, PAGETYPE_DOOR), nb);

  EXPECT_EQ(mng_AllocTrackLock(c, PAGETYPE_DOOR), na);
  EXPECT_EQ(mng_FindTrackLock(c, PAGETYPE_DOOR), na);
}

} // namespace
