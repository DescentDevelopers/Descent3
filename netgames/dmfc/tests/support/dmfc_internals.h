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

#ifndef DMFC_INTERNALS_H
#define DMFC_INTERNALS_H

#include <cstdint>

#include "gamedll_header.h"
#include "DMFC.h"
#include "dmfcinternal.h"

// Functions defined in DMFC's sources but not declared in any of its headers.

// dmfcbase.cpp
void ParseHostsFile(char *filename, tHostsNode **root);

// dmfcinputcommand.cpp
bool StringParseWord(const char *string, char *word, int size, const char **newpos);
bool StringParseNumber(const char *string, int *number, const char **newpos);

// dmfcprecord.cpp
void MultiPackNetworkAddress(network_address *addr, uint8_t *data, int *count);
void MultiUnpackNetworkAddress(network_address *addr, uint8_t *data, int *count);
void MultiPackDStats(t_dstat *stat, uint8_t *data, int *count);
void MultiUnpackDStats(t_dstat *stat, uint8_t *data, int *count);

// dmfcremote.cpp: pads a buffer up to the 8-byte block size of the remote-admin cipher.
void Remote_pad_buffer(int orig_size, const uint8_t *orig_buffer, int *new_size, uint8_t **new_buffer);

#endif
