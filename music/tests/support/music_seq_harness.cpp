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

#include "music_seq_harness.h"

#include <gtest/gtest.h>

namespace musictest {

bool operator==(const LoopSample &a, const LoopSample &b) {
  return a.name == b.name && a.count == b.count && a.theme == b.theme && a.region == b.region;
}

std::ostream &operator<<(std::ostream &os, const LoopSample &s) {
  return os << "{" << s.name << " x" << s.count << " theme=" << s.theme << " region=" << s.region << "}";
}

std::string WriteTheme(const d3_test::TempDir &dir, const std::vector<std::string> &lines, const char *name) {
  std::string contents = "[theme file]\n";
  for (const auto &line : lines)
    contents += line + "\n";
  return dir.Write(name, contents).string();
}

void LoadAndStart(OutrageMusicSeq &seq, const d3_test::TempDir &themes, const std::vector<std::string> &lines) {
  ASSERT_TRUE(seq.Init(WriteTheme(themes, lines).c_str()));
  seq.Start();
  ASSERT_TRUE(seq.IsSequencerRunning());
}

std::vector<std::string> SimpleSection(const std::string &theme, const std::string &sym, int count) {
  return {"section " + theme,
          "llpt " + sym,
          "lplc " + std::to_string(count),
          "play",
          "label wait_" + theme,
          "bnif wait_" + theme,
          "endsection"};
}

void Append(std::vector<std::string> &dst, const std::vector<std::string> &src) {
  dst.insert(dst.end(), src.begin(), src.end());
}

std::vector<LoopSample> RunUntilLoops(OutrageMusicSeq &seq, size_t n, int max_frames) {
  std::vector<LoopSample> out;
  for (int frame = 0; frame < max_frames && out.size() < n; frame++) {
    seq.Frame(1.0f / 30.0f);
    oms_q_evt evt;
    while (seq.m_output_q.recv(&evt)) {
      if (evt.cmd != OMS_EVT_LOOPENDING)
        continue;
      int count = -1;
      const char *name = seq.GetCurrentLoopName(&count);
      out.push_back({name ? name : "<none>", count, evt.parm.i, seq.GetPlayingRegion()});
    }
  }
  return out;
}

int CountEvents(OutrageMusicSeq &seq, int frames) {
  int n = 0;
  for (int i = 0; i < frames; i++) {
    seq.Frame(1.0f / 30.0f);
    oms_q_evt evt;
    while (seq.m_output_q.recv(&evt))
      n++;
  }
  return n;
}

} // namespace musictest
