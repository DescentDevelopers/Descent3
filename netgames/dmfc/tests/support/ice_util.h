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

#ifndef DMFC_ICE_UTIL_H
#define DMFC_ICE_UTIL_H

#include <array>
#include <cstdint>
#include <string>

#include "encryption.h"

namespace dmfc_test {

// One 8-byte ICE block.
using Block = std::array<uint8_t, 8>;

Block Encrypt(const IceKey &key, const Block &in);
Block Decrypt(const IceKey &key, const Block &in);

// Lower-case hex, two digits per byte.
std::string Hex(const Block &block);

} // namespace dmfc_test

#endif
