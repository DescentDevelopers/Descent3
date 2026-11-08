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

#ifndef AUDIOENCODE_TESTS_SIGNAL_GEN_H
#define AUDIOENCODE_TESTS_SIGNAL_GEN_H

#include <cstddef>
#include <cstdint>
#include <vector>

constexpr double kPi = 3.14159265358979323846;

// The decoder starts with zeroed filter history, so a signal that is already
// non-zero at the very first sample cannot be reconstructed for the first few
// hundred samples (2 * columns - 2 of overlap). Game sounds start from silence;
// test signals do the same so that whole-signal comparisons are meaningful.
constexpr size_t kLeadIn = 1024;

// `count` samples: kLeadIn of silence followed by a sine tone.
std::vector<int16_t> MakeTone(size_t count, double freq, double rate, double amplitude, double phase = 0.0);

std::vector<int16_t> Interleave(const std::vector<int16_t> &left, const std::vector<int16_t> &right);

std::vector<int16_t> Channel(const std::vector<int16_t> &interleaved, size_t channel, size_t channels);

double SnrDb(const std::vector<int16_t> &ref, const std::vector<int16_t> &test);

#endif
