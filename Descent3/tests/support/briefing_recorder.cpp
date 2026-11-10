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

#include "briefing_recorder.h"

namespace briefing_test {

Recorder *rec = nullptr;

std::string Str(const char *s) { return s ? s : ""; }

void OnText(TCTEXTDESC *desc, char *text, char *description, int id) {
  rec->events.push_back("text");
  rec->texts.push_back({*desc, Str(text), Str(description), id});
}
void OnBitmap(TCBMPDESC *desc, char *description) {
  rec->events.push_back("bitmap");
  rec->bitmaps.push_back({*desc, Str(description)});
}
void OnMovie(TCMOVIEDESC *desc, char *description) {
  rec->events.push_back("movie");
  rec->movies.push_back({*desc, Str(description)});
}
void OnBkg(TCBKGDESC *, char *) { rec->events.push_back("bkg"); }
void OnPoly(TCPOLYDESC *desc, char *) {
  rec->events.push_back("poly");
  rec->polys.push_back(*desc);
}
void OnSound(TCSNDDESC *desc, char *description) {
  rec->events.push_back("sound");
  rec->sounds.push_back({*desc, Str(description)});
}
void OnButton(TCBUTTONDESC *desc, char *description, int id) {
  rec->events.push_back("button");
  rec->buttons.push_back({*desc, Str(description), id});
}
void OnStartScreen(int num, char *description, char *layout, uint32_t mask_set, uint32_t mask_unset) {
  rec->events.push_back("screen");
  rec->screens.push_back({num, Str(description), layout != nullptr, Str(layout), mask_set, mask_unset});
}
void OnEndScreen() { rec->events.push_back("endscreen"); }
void OnTitle(char *title) {
  rec->events.push_back("title");
  rec->titles.push_back(Str(title));
}
void OnStatic(float amount) {
  rec->events.push_back("static");
  rec->statics.push_back(amount);
}
void OnGlitch(float amount) {
  rec->events.push_back("glitch");
  rec->glitches.push_back(amount);
}
void OnVoice(char *, int, char *) { rec->events.push_back("voice"); }

} // namespace briefing_test
