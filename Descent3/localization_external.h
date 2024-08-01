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

#ifndef LOCALIZATION_EXTERNAL_H
#define LOCALIZATION_EXTERNAL_H

// =======================================================================
// Language defines
// =======================================================================

enum LANGS {
  LANGUAGE_ENGLISH = 0,
  LANGUAGE_GERMAN,
  LANGUAGE_SPANISH,
  LANGUAGE_ITALIAN,
  LANGUAGE_FRENCH,
  LANGUAGE_POLISH,

  LANGUAGE_TOTAL,
};

// Lang suffixes for message files
const std::vector<std::string> lang_suffixes = {
    "",     // No suffix for English
    "_GER", // German
    "_SPN", // Spanish
    "_ITN", // Italian
    "_FRN", // French
    "_POL", // Polish
};

#endif // LOCALIZATION_EXTERNAL_H
