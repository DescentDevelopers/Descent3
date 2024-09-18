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

#include <queue>

#define TSETSEFLYAPI_HOST "http://api.tsetsefly.de"
#define TSETSEFLYAPI_URI "/?format=linebyline&template=simpleServerList&get=gameServerList&filter=gameName[d3];network[directip];&request=hp"

struct apiServerEntry {
  uint32_t ipv4adr;
  uint16_t port;
};

void RequestDIPGameList();
std::queue<apiServerEntry> GetDIpGameList();
