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

#pragma once

#include <string>
#include <vector>

#include <SDL3/SDL_assert.h>

namespace fake_dialogs {

struct Dialog {
  int type;
  std::string title;
  // For Debug_MessageBox this is the message. For Debug_ErrorBox it is all of
  // the text arguments joined.
  std::string text;
};

extern std::vector<Dialog> shown;
extern std::vector<std::string> events;
extern int response;

void Reset();

// Debug_ErrorBox and Debug_MessageBox handlers (see Debug_SetMessageBoxHandlers) that record the dialog and answer
// with `response`.
int ErrorBox(int type, const char *a, const char *b, const char *c);
int MessageBox(int type, const char *title, const char *str);

// SDL assertion handler that increments the int at userdata, logs an "assert" event and ignores the assertion.
SDL_AssertState SDLCALL CountAndIgnoreAssertion(const SDL_AssertData *, void *userdata);

} // namespace fake_dialogs

void RecordStop();
void RecordResume();
