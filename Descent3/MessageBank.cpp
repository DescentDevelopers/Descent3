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

#include <cstring>

#include "MessageBank.h"
#include "cfile.h"
#include "pstring.h"

namespace D3 {

// MessageBank

const std::shared_ptr<std::string> MessageBank::MISSING_STRING = std::make_shared<std::string>("Missing string");
const std::shared_ptr<std::string> MessageBank::MISSING_BANK = std::make_shared<std::string>("Missing bank");

std::shared_ptr<std::string> MessageBank::GetMessage(const std::string &id) {
  if (m_messages.find(id) == m_messages.end()) {
    return MISSING_STRING;
  }
  return m_messages.at(id);
}


// MsgBank

MsgBank::MsgBank(const std::filesystem::path &message_file, const std::string &language) {
  m_current_language = language;
  m_message_path = message_file;
  m_messages_count = MsgBank::ReloadBank();
  m_initialized = true;
}

size_t MsgBank::SetLanguage(const std::string &language) {
  if (m_current_language != language) {
    m_current_language = language;
    return ReloadBank();
  }
  return m_messages_count;
}

size_t MsgBank::ReloadBank() {
  // First load English messages as failback
  ParseFile(m_message_path);
  // Now load desired language
  std::filesystem::path result = ResolveFilename();
  if (m_current_language != DEFAULT_LANGUAGE && result != m_message_path) {
    ParseFile(result);
  }

  m_messages_count = m_messages.size();
  return m_messages_count;
}

std::filesystem::path MsgBank::ResolveFilename() {
  if (m_current_language == DEFAULT_LANGUAGE) {
    return m_message_path;
  }

  // Probe new file names
  std::filesystem::path result = m_message_path.stem();
  result += "_" + m_current_language;
  result.replace_extension(".msg");
  if (cfexist(result)) {
    return result;
  }

  // Failed, try legacy names
  if (m_lang_suffixes.find(m_current_language) != m_lang_suffixes.end()) {
    result = m_message_path.stem();
    result += m_lang_suffixes.at(m_current_language);
    result.replace_extension(".msg");
    if (cfexist(result)) {
      return result;
    }
  }

  // No legacy and new filenames found, just return original base
  return m_message_path;
}

void MsgBank::ParseFile(const std::filesystem::path &path) {
  // Try to open the file for loading
  CFILE *infile = cfopen(path, "rt");
  if (!infile) {
    return;
  }

  char buffer[MAX_LINE_LENGTH + 1];
  char line[MAX_LINE_LENGTH + 1];

  // Read in and parse each line of the file
  while (!cfeof(infile)) {

    // Clear the buffer
    strcpy(buffer, "");

    // Read in a line from the file
    cf_ReadString(buffer, MAX_LINE_LENGTH, infile);

    // Remove whitespace padding at start and end of line
    CleanupStr(line, buffer, MAX_LINE_LENGTH);

    // If line is a comment, or empty, discard it
    if (strlen(line) == 0 || strncmp(line, "//", 2) == 0)
      continue;

    // Find the start of message, and mark it
    char *msg_start = strchr(line, '=');
    if (msg_start == nullptr)
      continue;
    msg_start[0] = '\0';
    msg_start++;

    // Add the message to the list
    if (m_initialized) {
      *m_messages.at(line) = msg_start;
    } else {
      m_messages.insert_or_assign(line, std::make_shared<std::string>(msg_start));
    }
  }
  cfclose(infile);
}

// StrBank

StrBank::StrBank(const std::filesystem::path &message_file, const std::string &language) {
  m_message_path = message_file;
  m_current_language = language;
  m_messages_count = StrBank::ReloadBank();
  m_initialized = true;
}

size_t StrBank::SetLanguage(const std::string &language) {
  if (m_current_language != language) {
    m_current_language = language;
    m_messages_count = ReloadBank();
  }
  return m_messages_count;
}

size_t StrBank::ReloadBank() {
  m_messages_count = 0;

  if (m_lang_codes.find(m_current_language) == m_lang_codes.end()) {
    ParseFile(DEFAULT_LANGUAGE);
  } else {
    ParseFile(m_current_language);
  }

  m_messages_count = m_messages.size();
  return m_messages_count;
}

std::string StrBank::ParseEscapeChars(const char *buffer) {
  char tempbuffer[MAX_STRING_LENGTH];

  int t_index = 0;
  int b_index = 0;

  // search for a /
  while (buffer[b_index] != '\0') {
    // see if we are at a backslash
    if (buffer[b_index] != '\\') {
      tempbuffer[t_index] = buffer[b_index];
      t_index++;
      b_index++;
    } else {
      // we are at a backslash, so determine what to do
      b_index++;
      switch (tolower(buffer[b_index])) {
      case 't':
        tempbuffer[t_index] = '\t';
        break;
      case 'n':
        tempbuffer[t_index] = '\n';
        break;
      case 'r':
        tempbuffer[t_index] = '\r';
        break;
      case '\"':
        tempbuffer[t_index] = '\"';
        break;
      case '\\':
        tempbuffer[t_index] = '\\';
        break;
      default:
        // see if we have a number
        // TODO: this code incorrectly handle number sequences > 255
        if ((buffer[b_index] >= '0') && (buffer[b_index] <= '9')) {
          // we have a number, so parse the value
          uint8_t value = 0;
          while ((buffer[b_index] >= '0') && (buffer[b_index] <= '9')) {
            value = (value * 10) + buffer[b_index] - '0';
            b_index++;
          }
          // adjust b_index
          b_index--;
          tempbuffer[t_index] = value;
        }
        break;
      }
      b_index++;
      t_index++;
    }
  }
  tempbuffer[t_index] = '\0';
  return tempbuffer;
}

char *StrBank::ParseStringTag(char *buffer) const {
  int i = ParseLineInfo(buffer);

  switch (i) {
  case STAG_CONTINUE:
  case STAG_EMPTY:
    return buffer;

  case STAG_COMMENT:
    return buffer + strlen(COMMENT_TAG);

  default:
    if (i >= 0 && i < LANGUAGE_TAGS_COUNT) {
      // we have a specific language...
      return buffer + 3;
    }
    break;
  }

  return buffer;
}

void StrBank::ParseFile(const std::string &language) {
  CFILE *file = cfopen(m_message_path, "rt");
  if (!file) {
    return;
  }

  int scount = 0;
  char buffer[MAX_STRING_LENGTH];

  bool reading_string = false;
  int reading_info = 0;

  std::unique_ptr<std::vector<std::string>> parsed_strings = std::make_unique<std::vector<std::string>>(LANGUAGE_TAGS_COUNT);

  std::string parsed_string;

  while (!cfeof(file)) {
    cf_ReadString(buffer, MAX_STRING_LENGTH, file);

    int line_info = ParseLineInfo(buffer);

    switch (line_info) {
    case STAG_CONTINUE:
      if (reading_string) {
        // we need to add on to the working buffer
        parsed_strings->at(reading_info) += '\n';
        parsed_strings->at(reading_info) += ParseEscapeChars(buffer);
      }
      break;
    case STAG_EMPTY:
    case STAG_COMMENT:
      break;
    case STAG_ENGLISH:
      // We got new message block, write current into messages map
      if (reading_string) {
        // ok we're done with the string, finish it and put it in the string table
        parsed_string = parsed_strings->at(m_lang_codes.at(DEFAULT_LANGUAGE));
        if (!parsed_strings->at(m_lang_codes.at(language)).empty()) {
          parsed_string = parsed_strings->at(m_lang_codes.at(language));
        }
        if (m_initialized) {
          *m_messages.at(std::to_string(scount)) = parsed_string;
        } else {
          m_messages.insert_or_assign(std::to_string(scount), std::make_shared<std::string>(parsed_string));
        }
        scount++;
        parsed_strings = std::make_unique<std::vector<std::string>>(LANGUAGE_TAGS_COUNT);
      }
      reading_string = false;
    case STAG_GERMAN:
    case STAG_SPANISH:
    case STAG_ITALIC:
    case STAG_FRENCH:
    case STAG_POLISH:
      reading_info = line_info;
      parsed_strings->at(reading_info) = ParseEscapeChars(ParseStringTag(buffer));
      reading_string = true;
      break;
    default:
      reading_string = false;
      break;
    } // end switch
  } // end while

  if (reading_string) {
    // we're at the end of the file and we're reading, so but it in the string table
    parsed_string = parsed_strings->at(m_lang_codes.at(DEFAULT_LANGUAGE));
    if (!parsed_strings->at(m_lang_codes.at(language)).empty()) {
      parsed_string = parsed_strings->at(m_lang_codes.at(language));
    }
    if (m_initialized) {
      *m_messages.at(std::to_string(scount)) = parsed_string;
    } else {
      m_messages.insert_or_assign(std::to_string(scount), std::make_shared<std::string>(parsed_string));
    }
  }

  cfclose(file);
}

int StrBank::ParseLineInfo(const char *buffer) const {
  for (int i = 0; i < LANGUAGE_TAGS_COUNT; i++) {
    if (!strncmp(buffer, m_lang_tags.at(i), 3))
      return i;
  }

  if (!strncmp(buffer, COMMENT_TAG, strlen(COMMENT_TAG)))
    return STAG_COMMENT;
  if (strlen(buffer) == 0)
    return STAG_EMPTY;
  return STAG_CONTINUE;
}

}
