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

#include "signal_gen.h"

#include <algorithm>
#include <cmath>
#include <limits>

std::vector<int16_t> MakeTone(size_t count, double freq, double rate, double amplitude, double phase) {
  std::vector<int16_t> out(count, 0);
  for (size_t i = kLeadIn; i < count; ++i) {
    out[i] = static_cast<int16_t>(std::lround(amplitude * std::sin(2.0 * kPi * freq * double(i) / rate + phase)));
  }
  return out;
}

std::vector<int16_t> Interleave(const std::vector<int16_t> &left, const std::vector<int16_t> &right) {
  std::vector<int16_t> out;
  out.reserve(left.size() * 2);
  for (size_t i = 0; i < left.size(); ++i) {
    out.push_back(left[i]);
    out.push_back(right[i]);
  }
  return out;
}

std::vector<int16_t> Channel(const std::vector<int16_t> &interleaved, size_t channel, size_t channels) {
  std::vector<int16_t> out;
  for (size_t i = channel; i < interleaved.size(); i += channels)
    out.push_back(interleaved[i]);
  return out;
}

double SnrDb(const std::vector<int16_t> &ref, const std::vector<int16_t> &test) {
  double signal = 0.0, noise = 0.0;
  const size_t n = std::min(ref.size(), test.size());
  for (size_t i = 0; i < n; ++i) {
    const double r = ref[i];
    const double d = r - double(test[i]);
    signal += r * r;
    noise += d * d;
  }
  if (noise == 0.0)
    return std::numeric_limits<double>::infinity();
  return 10.0 * std::log10(signal / noise);
}
