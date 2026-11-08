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

#include <cstdint>
#include <string>
#include <vector>

#include "TelComEfxStructs.h"

namespace briefing_test {

struct Screen {
  int num;
  std::string description;
  bool has_layout;
  std::string layout;
  uint32_t mask_set, mask_unset;
};

struct Text {
  TCTEXTDESC desc;
  std::string text;
  std::string description;
  int id;
};

struct Bitmap {
  TCBMPDESC desc;
  std::string description;
};

struct Movie {
  TCMOVIEDESC desc;
  std::string description;
};

struct Sound {
  TCSNDDESC desc;
  std::string description;
};

struct Button {
  TCBUTTONDESC desc;
  std::string description;
  int id;
};

struct Recorder {
  std::vector<std::string> events;
  std::vector<std::string> titles;
  std::vector<float> statics, glitches;
  std::vector<Screen> screens;
  std::vector<Text> texts;
  std::vector<Bitmap> bitmaps;
  std::vector<Movie> movies;
  std::vector<Sound> sounds;
  std::vector<Button> buttons;
  std::vector<TCPOLYDESC> polys;
};

// The parser takes plain function pointers, so the callbacks record into this.
extern Recorder *rec;

void OnText(TCTEXTDESC *desc, char *text, char *description, int id);
void OnBitmap(TCBMPDESC *desc, char *description);
void OnMovie(TCMOVIEDESC *desc, char *description);
void OnBkg(TCBKGDESC *, char *);
void OnPoly(TCPOLYDESC *desc, char *);
void OnSound(TCSNDDESC *desc, char *description);
void OnButton(TCBUTTONDESC *desc, char *description, int id);
void OnStartScreen(int num, char *description, char *layout, uint32_t mask_set, uint32_t mask_unset);
void OnEndScreen();
void OnTitle(char *title);
void OnStatic(float amount);
void OnGlitch(float amount);
void OnVoice(char *, int, char *);

} // namespace briefing_test
