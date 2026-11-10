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

#include "fake_message_boxes.h"

#include "debug.h"

namespace fake_dialogs {

std::vector<Dialog> shown;
std::vector<std::string> events;
int response = IDOK;

void Reset() {
  shown.clear();
  events.clear();
  response = IDOK;
}

SDL_AssertState SDLCALL CountAndIgnoreAssertion(const SDL_AssertData *, void *userdata) {
  ++*static_cast<int *>(userdata);
  events.emplace_back("assert");
  return SDL_ASSERTION_IGNORE;
}

// Note: the parameter names in debug.h and the platform implementations
// disagree about whether the 2nd or 3rd argument is the title, so the fake
// does not rely on either and records all text together.
int ErrorBox(int type, const char *a, const char *b, const char *c) {
  std::string text;
  for (const char *s : {a, b, c}) {
    if (s) {
      text += s;
      text += '\n';
    }
  }
  shown.push_back({type, std::string(), text});
  events.emplace_back("dialog");
  return response;
}

int MessageBox(int type, const char *title, const char *str) {
  shown.push_back({type, title ? title : "", str ? str : ""});
  events.emplace_back("dialog");
  return response;
}

} // namespace fake_dialogs

void RecordStop() { fake_dialogs::events.emplace_back("stop"); }
void RecordResume() { fake_dialogs::events.emplace_back("resume"); }
