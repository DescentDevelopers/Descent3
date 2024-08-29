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

#pragma once

#include <cstdint>

/**
 * Encrypt buffer with provided key
 * @param key pointer to key
 * @param buffer_ptr pointer to buffer for encryption
 * @param buffer_len buffer length
 */
void gs_encrypt(const uint8_t *key, uint8_t *buffer_ptr, int buffer_len);

/**
 * Encode buffer with some sort of Base64. Output buffer must be equal to input buffer.
 * @param ins input buffer
 * @param size size of input/output buffer
 * @param result output buffer
 */
void gs_encode(uint8_t *ins, int size, uint8_t *result);
