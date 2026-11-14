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
*
*/

#pragma once

#include <filesystem>
#include <string>
#include <unordered_map>

#include "MessageBank.h"

namespace D3 {

class LocDB {
public:
  size_t LoadBank(const std::string &name, const std::filesystem::path &message_file);
  void FreeBank(const std::string &name);

  std::string GetLanguage();
  size_t SetLanguage(const std::string &language);

  std::shared_ptr<std::string> GetMsg(const std::string &bank_name, const std::string &id);

  size_t GetCount() const { return m_total_count; }
private:
  std::string m_current_language = "en_US";
  std::unordered_map<std::string, std::unique_ptr<MessageBank>> m_banks;
  size_t m_total_count = 0;
};

} // namespace D3
