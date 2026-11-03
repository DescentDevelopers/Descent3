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

#ifndef INFFILE_TEST_UTILS_H
#define INFFILE_TEST_UTILS_H

#include <cstring>
#include <string>
#include <utility>
#include <vector>

#include "inffile.h"

namespace cfile_test {

// Command ids returned by TestLex.
enum { CMD_STREAM = 1, CMD_IFI, CMD_LLPT, CMD_LPLC, CMD_SETI, CMD_PLAY, CMD_NAME };

// Lexer for InfFile that recognises the theme-file style commands above.
inline int TestLex(const char *command) {
  static const std::pair<const char *, int> kCommands[] = {
      {"stream", CMD_STREAM}, {"ifi", CMD_IFI},   {"llpt", CMD_LLPT}, {"lplc", CMD_LPLC},
      {"seti", CMD_SETI},     {"play", CMD_PLAY}, {"name", CMD_NAME},
  };
  for (const auto &[name, id] : kCommands)
    if (std::strcmp(name, command) == 0)
      return id;
  return INFFILE_ERROR;
}

// A ParseLine result: the command id and, for commands, its operand.
using Parsed = std::pair<int, std::string>;

// Reads the next line of inf and returns every ParseLine result up to and including the first non-command.
inline std::vector<Parsed> ParseNextLine(InfFile &inf) {
  std::vector<Parsed> out;
  if (!inf.ReadLine())
    return out;
  char operand[INFFILE_LINELEN];
  for (;;) {
    int cmd = inf.ParseLine(operand, INFFILE_LINELEN);
    out.emplace_back(cmd, cmd > INFFILE_ERROR ? std::string(operand) : std::string());
    if (cmd <= INFFILE_ERROR)
      break;
  }
  return out;
}

} // namespace cfile_test

#endif
