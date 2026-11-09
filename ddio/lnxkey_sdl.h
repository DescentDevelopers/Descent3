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

struct ddio_init_info;

// sdl mode ddio
bool ddio_sdl_InternalKeyInit(ddio_init_info *init_info);
void ddio_sdl_InternalKeyClose();
bool ddio_sdl_InternalKeyState(uint8_t key);
void ddio_sdl_InternalKeySuspend();
void ddio_sdl_InternalKeyResume();
float ddio_sdl_InternalKeyDownTime(uint8_t key);
void ddio_sdl_InternalResetKey(uint8_t key);
void ddio_sdl_InternalKeyFrame(void);
