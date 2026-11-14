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
#include <vector>

namespace D3 {

/**
 * Abstract class for storing messages
 */
class MessageBank {
public:
  virtual ~MessageBank() = default;

  /**
   * Set language for message bank. If language is changed, it will reload messages from language file
   * @param language language code (like "en_US")
   * @return true on success
   */
  virtual size_t SetLanguage(const std::string &language) = 0;

  /**
   * Get current language code (like "en_US")
   * @return language code
   */
  std::string GetLanguage() { return m_current_language; };

  /**
   * Reload bank (on language changes)
   * @return number of loaded messages, 0 on failure
   */
  virtual size_t ReloadBank() = 0;

  /**
   * Get number of messages
   * @return number of loaded messages
   */
  [[nodiscard]] size_t GetCount() const { return m_messages_count; }

  /**
   * Get message from bank
   * @param id string id
   * @return message according supplied id. If there no such id, "Missing string" will be returned.
   */
  std::shared_ptr<std::string> GetMessage(const std::string &id);

  static const std::shared_ptr<std::string> MISSING_STRING;
  static const std::shared_ptr<std::string> MISSING_BANK;

protected:
  std::filesystem::path m_message_path;
  std::unordered_map<std::string, std::shared_ptr<std::string>> m_messages;
  size_t m_messages_count = 0;
  std::string m_current_language = "en_US";
  bool m_initialized = false;

  const std::string DEFAULT_LANGUAGE = "en_US";

  static constexpr size_t MAX_LINE_LENGTH = 1024;
  static constexpr size_t MAX_STRING_LENGTH = 8192;
};


/**
 * Class for storing messages originated from *.msg files
 */
class MsgBank : public MessageBank {
public:
  /**
   * Load messages from file and assign language. For non-en languages will be loaded messages from en,
   * and then from specified language.
   * @param message_file file that contains messages (format varies)
   * @param language locale code (like "en_US")
   */
  MsgBank(const std::filesystem::path &message_file, const std::string &language);
  ~MsgBank() override = default;

  /**
   * Set language for message bank. If language is changed, it will reload messages from language file
   * @param language language code (like "en_US")
   * @return true on success
   */
  size_t SetLanguage(const std::string &language) override;

  /**
   * Reload bank (on language changes)
   * @return number of loaded messages, 0 on failure
   */
  size_t ReloadBank() override;

private:
  /**
   * Resolve filename depending on current language.
   * Function uses m_message_path as base. If language is English, m_message_path will be returned.
   * Otherwise function will check file with locale suffix (like name_de_DE.msg). If this file is
   * found, and m_current_language is German, French, Italian or Spanish,
   * legacy suffix will be used (like name_GER.msg); if there no such name at all,
   * m_message_path will be returned, meaning that English messages will be loaded.
   * @return name of file
   */
  std::filesystem::path ResolveFilename();

  /**
   * Parse file and load messages into internal storage
   * @param path input file
   */
  void ParseFile(const std::filesystem::path &path);

  const std::unordered_map<std::string, std::string> m_lang_suffixes = {
    {"en_US", ""},
    {"de_DE", "_GER"},
    {"es_ES", "_SPN"},
    {"it_IT", "_ITN"},
    {"fr_FR", "_FRN"},
    {"po_PO", "_POL"},
  };
};


/**
 * Class for storing messages originated from *.str files
 */
class StrBank : public MessageBank {
public:
  /**
   * Load messages from file and assign language. For non-en languages will be loaded messages from en,
   * and then from specified language.
   * @param message_file file that contains messages (format varies)
   * @param language language code (like en_US)
   */
  StrBank(const std::filesystem::path &message_file, const std::string &language);
  ~StrBank() override = default;

  /**
   * Set language for message bank. If language is changed, it will reload messages from language file
   * @param language language code (like "en_US")
   * @return true on success
   */
  size_t SetLanguage(const std::string &language) override;

  /**
   * Reload bank (on language changes)
   * @return number of loaded messages, 0 on failure
   */
  size_t ReloadBank() override;

private:
  // Helper functions

  /**
   * Parse input char buffer and replace in it escape sequences ("\n", "\r" and so on)
   * @param buffer input buffer
   * @return parsed buffer
   */
  static std::string ParseEscapeChars(const char *buffer);

  /**
   * Parse file and load messages into internal storage. Function parses all languages available in
   * *.str file and use only chosen language. If message of chosen language is not present, default
   * language (English) will be used.
   * @param language input language
   */
  void ParseFile(const std::string &language);

  /**
   * Get tag type from string
   * @param buffer input string
   * @return type of tag string
   */
  int ParseLineInfo(const char *buffer) const;

  /**
   * Parse tagged string
   * @param buffer input string
   * @return parsed string
   */
  char *ParseStringTag(char *buffer) const;

  // Mapping locale codes to lang codes of *.str
  const std::unordered_map<std::string, int> m_lang_codes = {
    {"en_US", 0},
    {"de_DE", 1},
    {"es_ES", 2},
    {"it_IT", 3},
    {"fr_FR", 4},
    {"po_PO", 5},
  };

  // Lang tags of *.str file
  const std::vector<const char*> m_lang_tags = {
    "!=!",
    "!G!",
    "!S!",
    "!I!",
    "!F!",
    "!P!"
  };
  const int LANGUAGE_TAGS_COUNT = static_cast<int>(m_lang_tags.size());

  const char* COMMENT_TAG = "!/!"; // This line is to be ignored

  enum {
    STAG_COMMENT = -3,  // comment line
    STAG_EMPTY = -2,    // empty line
    STAG_CONTINUE = -1, // this line is just a continuation of the last line
    STAG_ENGLISH = 0,   // English message
    STAG_GERMAN = 1,    // German message
    STAG_SPANISH = 2,   // Spanish message
    STAG_ITALIC = 3,    // Italic message
    STAG_FRENCH = 4,    // French message
    STAG_POLISH = 5,    // Polish message
  };
};


}

