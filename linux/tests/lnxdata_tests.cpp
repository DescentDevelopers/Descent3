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

#include <cstdint>
#include <cstring>
#include <string>

#include <gtest/gtest.h>

#include "appdatabase.h"
#include "d3_test/files.h"

namespace {

// Every database keeps its registry file in the test's temporary directory instead of the user's preference directory.
class AppDatabaseTest : public ::testing::Test {
protected:
  d3_test::TempDir dir;
};

TEST_F(AppDatabaseTest, WrittenValuesPersistToTheNextInstance) {
  {
    oeLnxAppDatabase db(dir.path());
    ASSERT_TRUE(db.create_record("Descent3"));
    const char *dir_name = "C:\\Games\\Descent3";
    ASSERT_TRUE(db.write("local directory", dir_name, static_cast<int>(strlen(dir_name) + 1)));
    ASSERT_TRUE(db.write("RS_resolution", 3));
    ASSERT_TRUE(db.write("VoiceAll", 1));
    ASSERT_TRUE(db.write("EnableJoystickFF", 0));
  }

  oeLnxAppDatabase db(dir.path());
  ASSERT_TRUE(db.lookup_record("Descent3"));

  char buf[64] = {};
  int len = sizeof(buf);
  ASSERT_TRUE(db.read("local directory", buf, &len));
  EXPECT_STREQ(buf, "C:\\Games\\Descent3");
  EXPECT_EQ(len, static_cast<int>(strlen("C:\\Games\\Descent3")));

  int resolution = 0;
  ASSERT_TRUE(db.read_int("RS_resolution", &resolution));
  EXPECT_EQ(resolution, 3);

  bool voices = false, ff = true;
  ASSERT_TRUE(db.read("VoiceAll", &voices));
  EXPECT_TRUE(voices);
  ASSERT_TRUE(db.read("EnableJoystickFF", &ff));
  EXPECT_FALSE(ff);
}

TEST_F(AppDatabaseTest, LookupOfUnknownRecordFails) {
  oeLnxAppDatabase db(dir.path());
  EXPECT_FALSE(db.lookup_record("NoSuchRecord"));
}

TEST_F(AppDatabaseTest, ReadingAMissingLabelFailsAndLeavesOutputUntouched) {
  oeLnxAppDatabase db(dir.path());
  ASSERT_TRUE(db.create_record("Descent3"));

  char buf[16] = "unchanged";
  int len = sizeof(buf);
  EXPECT_FALSE(db.read("absent", buf, &len));
  EXPECT_STREQ(buf, "unchanged");

  int value = 99;
  EXPECT_FALSE(db.read_int("absent", &value));
  EXPECT_EQ(value, 99);

  bool flag = true;
  EXPECT_FALSE(db.read("absent", &flag));
  EXPECT_TRUE(flag);
}

// read_int(label, &var) passes sizeof(var) as the word size.
TEST_F(AppDatabaseTest, IntegerReadsWriteOnlyTheRequestedWordSize) {
  oeLnxAppDatabase db(dir.path());
  ASSERT_TRUE(db.create_record("Descent3"));
  ASSERT_TRUE(db.write("byte", 200));
  ASSERT_TRUE(db.write("word", 60000));

  uint8_t bytes[4];
  memset(bytes, 0xAA, sizeof(bytes));
  ASSERT_TRUE(db.read("byte", &bytes[0], 1));
  EXPECT_EQ(bytes[0], 200);
  EXPECT_EQ(bytes[1], 0xAA);
  EXPECT_EQ(bytes[2], 0xAA);
  EXPECT_EQ(bytes[3], 0xAA);

  uint16_t words[2] = {0xAAAA, 0xAAAA};
  ASSERT_TRUE(db.read("word", &words[0], 2));
  EXPECT_EQ(words[0], 60000);
  EXPECT_EQ(words[1], 0xAAAA);
}

TEST_F(AppDatabaseTest, DatabaseConstructedFromParentSeesParentsUnsavedWrites) {
  oeLnxAppDatabase parent(dir.path());
  ASSERT_TRUE(parent.create_record("Descent3"));
  ASSERT_TRUE(parent.write("Default_pilot", "Hotshot", 8));

  oeLnxAppDatabase child(&parent);
  ASSERT_TRUE(child.lookup_record("Descent3"));
  char buf[32] = {};
  int len = sizeof(buf);
  ASSERT_TRUE(child.read("Default_pilot", buf, &len));
  EXPECT_STREQ(buf, "Hotshot");
}

TEST_F(AppDatabaseTest, StringReadSucceedsWhenValueAndTerminatorExactlyFillCapacity) {
  oeLnxAppDatabase db(dir.path());
  ASSERT_TRUE(db.create_record("Descent3"));
  ASSERT_TRUE(db.write("exact", "1234567", 8));

  char buf[64];
  memset(buf, 'X', sizeof(buf));
  int len = 8;
  ASSERT_TRUE(db.read("exact", buf, &len));
  EXPECT_STREQ(buf, "1234567");
  EXPECT_EQ(len, 7);
}

} // namespace
