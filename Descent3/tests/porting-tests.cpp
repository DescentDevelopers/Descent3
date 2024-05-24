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

#include <gtest/gtest.h>

#include "osiris_predefs.h"
#include "osiris_share.h"
#include "player_external_struct.h"

// This is copy of actual Osiris_CreateGameChecksum(void) from OsirisLoadandBind.cpp
uint32_t Osiris_CreateGameChecksumTest() {
  uint32_t value = 0xe1e1b0b0;

  value += sizeof(object);
  value += sizeof(player) * 2;
  value += sizeof(tOSIRISModuleInit) * 3;
  value += sizeof(tOSIRISEventInfo) * 5;
  value += sizeof(tOSIRISTIMER) * 7;
  value += sizeof(tOSIRISSCRIPTID) * 11;
  value += sizeof(tOSIRISMEMCHUNK) * 13;
  value += sizeof(msafe_struct) * 17;
  value += sizeof(ray_info) * 19;
  value += sizeof(vector) * 23;
  value += sizeof(matrix) * 29;
  value += sizeof(tGameCinematic) * 31;
  value += sizeof(tCannedCinematicInfo) * 37;

  value = value << 2;

  /* Adds count of Osiris_module_init.fp */
  value += 123;

  return value;
}


TEST(D3, OsirisTests) {
  EXPECT_EQ(Osiris_CreateGameChecksumTest(), CHECKSUM);
}
