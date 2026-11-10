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
#include <cstring>
#include <string>

#include <gtest/gtest.h>

#include "registry.h"
#include "support/registry_records.h"
#include "d3_test/files.h"

namespace {

class RegistryTest : public ::testing::Test {
protected:
  d3_test::TempDir dir;
  std::string file = (dir.path() / "registry.txt").string();
};

TEST_F(RegistryTest, RecordsCannotBeCreatedBeforeAnyKeyIsSelected) {
  CRegistry reg(file.c_str());
  char value[] = "v";
  EXPECT_FALSE(reg.CreateRecord("name", REGT_STRING, value));
  char buf[16] = {};
  EXPECT_EQ(reg.LookupRecord("name", buf), nullptr);
  EXPECT_EQ(reg.GetDataSize("name"), 0);
}

TEST_F(RegistryTest, StoresAndReturnsStringAndDwordRecords) {
  CRegistry reg(file.c_str());
  reg.CreateKey("Descent3");
  char str[] = "hello world";
  int dw = 1234;
  ASSERT_TRUE(reg.CreateRecord("greeting", REGT_STRING, str));
  ASSERT_TRUE(reg.CreateRecord("count", REGT_DWORD, &dw));

  EXPECT_EQ(ReadString(reg, "greeting"), "hello world");
  EXPECT_EQ(ReadDword(reg, "count"), 1234);

  EXPECT_EQ(reg.GetDataSize("greeting"), static_cast<int>(strlen(str) + 1));
  EXPECT_EQ(reg.GetDataSize("count"), static_cast<int>(sizeof(int)));
}

TEST_F(RegistryTest, MissingRecordIsReportedAndLeavesOutputUntouched) {
  CRegistry reg(file.c_str());
  reg.CreateKey("Descent3");
  int value = 77;
  EXPECT_EQ(reg.LookupRecord("absent", &value), nullptr);
  EXPECT_EQ(value, 77);
  EXPECT_EQ(reg.GetDataSize("absent"), 0);
}

TEST_F(RegistryTest, KeyAndRecordNamesAreCaseInsensitive) {
  CRegistry reg(file.c_str());
  reg.CreateKey("Descent3");
  int dw = 5;
  ASSERT_TRUE(reg.CreateRecord("RS_Gamma", REGT_DWORD, &dw));

  reg.CreateKey("Other");
  ASSERT_TRUE(reg.LookupKey("DESCENT3"));
  EXPECT_EQ(ReadDword(reg, "rs_gamma"), 5);
}

TEST_F(RegistryTest, RecordsAreScopedToTheirKey) {
  CRegistry reg(file.c_str());
  int a = 1, b = 2;
  reg.CreateKey("A");
  ASSERT_TRUE(reg.CreateRecord("value", REGT_DWORD, &a));
  reg.CreateKey("B");
  ASSERT_TRUE(reg.CreateRecord("value", REGT_DWORD, &b));

  ASSERT_TRUE(reg.LookupKey("A"));
  EXPECT_EQ(ReadDword(reg, "value"), 1);
  ASSERT_TRUE(reg.LookupKey("B"));
  EXPECT_EQ(ReadDword(reg, "value"), 2);

  EXPECT_FALSE(reg.LookupKey("C"));
}

TEST_F(RegistryTest, CreatingAnExistingKeySelectsItAndKeepsItsRecords) {
  CRegistry reg(file.c_str());
  int dw = 9;
  reg.CreateKey("A");
  ASSERT_TRUE(reg.CreateRecord("kept", REGT_DWORD, &dw));
  reg.CreateKey("B");

  reg.CreateKey("a");
  EXPECT_EQ(ReadDword(reg, "kept"), 9);
}

TEST_F(RegistryTest, WritingAnExistingRecordReplacesItsValueAndType) {
  CRegistry reg(file.c_str());
  reg.CreateKey("Descent3");
  char shortStr[] = "x";
  char longStr[] = "a considerably longer replacement value";
  ASSERT_TRUE(reg.CreateRecord("name", REGT_STRING, shortStr));
  ASSERT_TRUE(reg.CreateRecord("NAME", REGT_STRING, longStr));
  EXPECT_EQ(ReadString(reg, "name"), longStr);
  EXPECT_EQ(reg.GetDataSize("name"), static_cast<int>(strlen(longStr) + 1));

  int dw = 42;
  ASSERT_TRUE(reg.CreateRecord("name", REGT_DWORD, &dw));
  EXPECT_EQ(reg.GetDataSize("name"), static_cast<int>(sizeof(int)));
  EXPECT_EQ(ReadDword(reg, "name"), 42);
}

TEST_F(RegistryTest, ExportThenImportRestoresKeysAndRecords) {
  {
    CRegistry reg(file.c_str());
    reg.CreateKey("Descent3");
    char path[] = "C:\\Games\\Descent 3\\missions";
    char empty[] = "";
    int zero = 0, big = INT_MAX, hexish = 0xABCDEF;
    ASSERT_TRUE(reg.CreateRecord("local directory", REGT_STRING, path));
    ASSERT_TRUE(reg.CreateRecord("net directory", REGT_STRING, empty));
    ASSERT_TRUE(reg.CreateRecord("zero", REGT_DWORD, &zero));
    ASSERT_TRUE(reg.CreateRecord("big", REGT_DWORD, &big));
    ASSERT_TRUE(reg.CreateRecord("hexish", REGT_DWORD, &hexish));
    reg.CreateKey("Version");
    char ver[] = "1.5";
    ASSERT_TRUE(reg.CreateRecord("number", REGT_STRING, ver));
    reg.Export();
  }

  CRegistry loaded(file.c_str());
  ASSERT_TRUE(loaded.Import());

  ASSERT_TRUE(loaded.LookupKey("Descent3"));
  EXPECT_EQ(ReadString(loaded, "local directory"), "C:\\Games\\Descent 3\\missions");
  EXPECT_EQ(ReadString(loaded, "net directory"), "");
  EXPECT_EQ(loaded.GetDataSize("net directory"), 1);
  EXPECT_EQ(ReadDword(loaded, "zero"), 0);
  EXPECT_EQ(ReadDword(loaded, "big"), INT_MAX);
  EXPECT_EQ(ReadDword(loaded, "hexish"), 0xABCDEF);
  EXPECT_EQ(ReadString(loaded, "number"), "<missing>"); // belongs to the other key

  ASSERT_TRUE(loaded.LookupKey("Version"));
  EXPECT_EQ(ReadString(loaded, "number"), "1.5");
}

TEST_F(RegistryTest, ImportOfMissingFileFails) {
  CRegistry reg((dir.path() / "does_not_exist").string().c_str());
  EXPECT_FALSE(reg.Import());
}

TEST_F(RegistryTest, ImportReplacesExistingContents) {
  d3_test::WriteFile(file, "[New]\n\"a\"=\"b\"\n");

  CRegistry reg(file.c_str());
  reg.CreateKey("Old");
  ASSERT_TRUE(reg.Import());

  EXPECT_FALSE(reg.LookupKey("Old"));
  ASSERT_TRUE(reg.LookupKey("New"));
  EXPECT_EQ(ReadString(reg, "a"), "b");
}

TEST_F(RegistryTest, ImportSkipsMalformedLinesAndKeepsGoing) {
  d3_test::WriteFile(file, "[Descent3]\n"
                           "\n"
                           "not a key or a record\n"
                           "\"unterminated=\n"
                           "\"good\"=\"yes\"\n"
                           "\"num\"=dword:1f\n");

  CRegistry reg(file.c_str());
  ASSERT_TRUE(reg.Import());
  ASSERT_TRUE(reg.LookupKey("Descent3"));
  EXPECT_EQ(ReadString(reg, "good"), "yes");
  EXPECT_EQ(ReadDword(reg, "num"), 0x1f);
}

} // namespace
