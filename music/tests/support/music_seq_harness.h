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

#include <cstddef>
#include <cstdint>
#include <ostream>
#include <string>
#include <vector>

#include "d3_test/files.h"
#include "music.h"

namespace musictest {

struct LoopSample {
  std::string name; // loop reported by GetCurrentLoopName ("<none>" if null)
  int count;        // loop count reported alongside it
  int theme;        // parm of the OMS_EVT_LOOPENDING event (theme type)
  int16_t region;   // GetPlayingRegion() at that moment
};

bool operator==(const LoopSample &a, const LoopSample &b);
std::ostream &operator<<(std::ostream &os, const LoopSample &s);

// Writes a theme file in `dir` with the "[theme file]" tag followed by `lines`, and returns its path.
std::string WriteTheme(const d3_test::TempDir &dir, const std::vector<std::string> &lines,
                       const char *name = "theme.omf");

// Writes `lines` as a theme file in `themes`, then initializes and starts `seq` with it.
void LoadAndStart(OutrageMusicSeq &seq, const d3_test::TempDir &themes, const std::vector<std::string> &lines);

// A section that plays loop `sym` `count` times and then waits for the end of the loop.
std::vector<std::string> SimpleSection(const std::string &theme, const std::string &sym, int count);

void Append(std::vector<std::string> &dst, const std::vector<std::string> &src);

// Runs the sequencer at 30 fps until `n` OMS_EVT_LOOPENDING events have been seen (or `max_frames`
// frames have passed), recording what was playing at each one.
std::vector<LoopSample> RunUntilLoops(OutrageMusicSeq &seq, size_t n, int max_frames = 500);

// Runs the sequencer for `frames` frames at 30 fps and returns how many events it emitted.
int CountEvents(OutrageMusicSeq &seq, int frames);

} // namespace musictest
