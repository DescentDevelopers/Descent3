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

#include "aiscripts_message_files.h"

#include "aiscripts_test_support.h"

namespace aiscripts_test {

bool LoadMessages(const d3_test::TempDir &dir, const std::string &contents) {
  return LoadMessageFile(dir.Write("test.msg", contents).string().c_str());
}

std::string NotFoundMessage() { return LookupMessage("No Message Has This Name"); }

} // namespace aiscripts_test
