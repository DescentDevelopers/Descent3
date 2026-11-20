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

#include "LocDB.h"
#include "cfile.h"
#include "log.h"

namespace D3 {

size_t LocDB::LoadBank(const std::string &name, const std::filesystem::path &message_file) {
  if (m_banks.find(name) != m_banks.end()) {
    m_total_count -= m_banks.at(name)->GetCount();
  }

  std::filesystem::path file_extension = message_file.extension();

  std::unique_ptr<MessageBank> bank;
  size_t count = 0;
  if (file_extension == ".msg") {
    bank = std::make_unique<MsgBank>(message_file, m_current_language);
    count = bank->GetCount();
  } else if (file_extension == ".str") {
    bank = std::make_unique<StrBank>(message_file, m_current_language);
    count = bank->GetCount();
  }
  if (count != 0) {
    m_total_count += count;
    m_banks.insert_or_assign(name, std::move(bank));
  }

  return count;
}

void LocDB::FreeBank(const std::string &name) {
  if (m_banks.find(name) != m_banks.end()) {
    m_total_count -= m_banks.at(name)->GetCount();
  }
  m_banks.erase(name);
}

std::string LocDB::GetLanguage(){
  return m_current_language;
}

size_t LocDB::SetLanguage(const std::string &language) {
  // Don't reload banks
  if (language == m_current_language) {
    return m_total_count;
  }
  m_current_language = language;
  size_t result = 0;
  for (const auto &[fst, snd] : m_banks) {
    result += snd->SetLanguage(language);
  }
  return result;
}

std::shared_ptr<std::string> LocDB::GetMsg(const std::string &bank_name, const std::string &id) {
  if (m_banks.find(bank_name) == m_banks.end()) {
    LOG_WARNING << "Bank " << bank_name << " not found";
    return MessageBank::MISSING_BANK;
  }
  std::shared_ptr<std::string> result = m_banks.at(bank_name)->GetMessage(id);
  if (result == MessageBank::MISSING_STRING) {
    LOG_WARNING << "Message " << bank_name << ":" << id << " not found";
  }
  return result;
}

} // namespace D3
