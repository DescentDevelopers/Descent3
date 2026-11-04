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

#include <string>
#include <vector>
#include <gtest/gtest.h>

#include "cfile.h"
#include "localization.h"

static std::vector<std::vector<std::string> > messages = {
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
};

static std::string error_string = "!!ERROR MISSING STRING!!";

TEST(D3, LocalizationTests) {
  cf_SetSearchPath("test_data");

  for (int i = 0; i < 5; i++) {
    Localization_SetLanguage(i);

    int count = LoadStringTables();
    EXPECT_EQ(count, 2);

    const char *message;
    message = GetStringFromTable(0);
    EXPECT_STREQ(message, messages[0][i].c_str());
    message = GetStringFromTable(1);
    EXPECT_STREQ(message, messages[1][i].c_str());
    message = GetStringFromTable(2);
    EXPECT_STREQ(message, error_string.c_str());
    FreeStringTables();
  }

  for (int i = 0; i < 5; i++) {
    Localization_SetLanguage(i);
    std::vector<std::string> loc_msgs;
    bool success = CreateStringTable("d3.str", loc_msgs);
    EXPECT_TRUE(success);
    EXPECT_STREQ(loc_msgs[0].c_str(), messages[0][i].c_str());
  }
}
