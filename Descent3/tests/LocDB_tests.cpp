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

#include <string>
#include <vector>
#include <gtest/gtest.h>

#include "cfile.h"
#include "LocDB.h"

class LocDBTest : public ::testing::Test {
protected:
  std::unique_ptr<D3::LocDB> loc;

  void SetUp() override {
    cf_SetSearchPath("test_data");
    loc = std::make_unique<D3::LocDB>();
  }

  const std::vector<std::string> languages = {
    "en_US",
    "de_DE",
    "es_ES",
    "it_IT",
    "fr_FR",
  };

  const std::vector<std::vector<std::string> > messages = {
    {
      "English message",
      "Deutsche Botschaft",
      "Mensaje en español",
      "Messaggio italiano",
      "Message en français",
    },
    {
      "English\nMultiline message",
      "Deutsche\nMultiline message",
      "Español\nMultiline message",
      "Italiano\nMultiline message",
      "Français\nMultiline message",
    },
  {
    "English\nEscaped\tmessage ",
    "Deutsche\nEscaped\tmessage ",
    "Español\nEscaped\tmessage ",
    "Italiano\nEscaped\tmessage ",
    "Français\nEscaped\tmessage ",
    },
    {
      "English message without comment",
      "Deutsche message without comment",
      "Español message without comment",
      "Italiano message without comment",
      "Français message without comment",
    },
    {
      "English Partial message",
      "English Partial message",
      "English Partial message",
      "English Partial message",
      "English Partial message",
    },
{
    "English Partial message 2",
    "Deutsche Partial message 2",
    "English Partial message 2",
    "English Partial message 2",
    "English Partial message 2",
  },
  };

};

TEST_F(LocDBTest, LoadFreeMsg) {
  EXPECT_EQ(loc->LoadBank("level4", "level4.msg"), 3);
  EXPECT_EQ(loc->GetCount(), 3);
  loc->FreeBank("level4");
  EXPECT_EQ(loc->GetCount(), 0);
}

TEST_F(LocDBTest, ValidMessagesMsg) {
  EXPECT_EQ(loc->LoadBank("level4", "level4.msg"), 3);
  EXPECT_EQ(*loc->GetMsg("level4", "Debug2"), "Should SHAKE!");
  EXPECT_EQ(*loc->GetMsg("level4", "Message with spaces"), "Test message");
  EXPECT_EQ(loc->SetLanguage("de_DE"), 3);
  EXPECT_EQ(loc->GetLanguage(), "de_DE");
  EXPECT_EQ(*loc->GetMsg("level4", "Debug2"), "Das sollte SCHÜTTELN!");

  EXPECT_EQ(loc->SetLanguage("fr_FR"), 3);
  EXPECT_EQ(*loc->GetMsg("level4", "Debug2"), "Ca devrait SECOUER!");
}

TEST_F(LocDBTest, NonExistentMsg) {
  EXPECT_EQ(loc->LoadBank("level4", "level4.msg"), 3);
  EXPECT_EQ(*loc->GetMsg("level4", "no_exist"), "Missing string");

  // "Debug1" not exist in fr_FR, English message should return
  loc->SetLanguage("fr_FR");
  EXPECT_EQ(*loc->GetMsg("level4", "Debug1"), "Difficulty Adjustment: %f");

  loc->FreeBank("level4");

  EXPECT_EQ(loc->LoadBank("no_exist", "no_exist.msg"), 0);
  EXPECT_EQ(loc->GetCount(), 0);
  EXPECT_EQ(*loc->GetMsg("no_exist", "0"), "Missing bank");
}

TEST_F(LocDBTest, NotSupportedLangMsg) {
  EXPECT_EQ(loc->SetLanguage("ja_JP"), 0);
  EXPECT_EQ(loc->LoadBank("level4", "level4.msg"), 3);
  EXPECT_EQ(*loc->GetMsg("level4", "Debug2"), "Should SHAKE!");
}

//

TEST_F(LocDBTest, LoadFreeStr) {
  for (int i = 0; i < 5; i++) {
    EXPECT_EQ(loc->LoadBank("d3", "d3.str"), 6);
    EXPECT_EQ(loc->GetCount(), 6);
  }
  for (int i = 0; i < 5; i++) {
    loc->FreeBank("d3");
    EXPECT_EQ(loc->GetCount(), 0);
  }
}

TEST_F(LocDBTest, ValidMessagesStr) {
  EXPECT_EQ(loc->LoadBank("d3", "d3.str"), 6);
  for (int i = 0; i < 5; i++) {
    EXPECT_EQ(loc->SetLanguage(languages[i]), 6);
    EXPECT_EQ(loc->GetLanguage(), languages[i]);

    for (int j = 0; j < 6; j++) {
      EXPECT_EQ(*loc->GetMsg("d3", std::to_string(j)), messages[j][i]);
    }
  }

  EXPECT_EQ(loc->GetCount(), 6);
}

TEST_F(LocDBTest, NonExistentStr) {
  EXPECT_EQ(loc->LoadBank("d3", "d3.str"), 6);
  EXPECT_EQ(*loc->GetMsg("d3", "no_exist"), "Missing string");

  loc->FreeBank("d3");

  EXPECT_EQ(loc->LoadBank("no_support", "no_support"), 0);
  EXPECT_EQ(loc->GetCount(), 0);
  EXPECT_EQ(*loc->GetMsg("no_support", "0"), "Missing bank");

  EXPECT_EQ(loc->LoadBank("no_exist", "no_exist.str"), 0);
  EXPECT_EQ(loc->GetCount(), 0);
  EXPECT_EQ(*loc->GetMsg("no_exist", "0"), "Missing bank");
}

TEST_F(LocDBTest, NotSupportedLangStr) {
  EXPECT_EQ(loc->SetLanguage("ja_JP"), 0);
  EXPECT_EQ(loc->LoadBank("d3", "d3.str"), 6);
  EXPECT_EQ(*loc->GetMsg("d3", "0"), messages[0][0]);
}
