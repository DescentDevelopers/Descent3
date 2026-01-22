/*
 * Descent 3
 * Copyright (C) 2024 Parallax Software
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

#ifndef DESCENT3_SDLHELPERS_H
#define DESCENT3_SDLHELPERS_H

#include <SDL3/SDL.h>
#include <tuple>

std::tuple<int, int> SDLNormalizedToGameRenderer(float x, float y);
std::tuple<int, int> SDLWindowToGameRenderer(int x, int y);
std::tuple<int, int> GameRendererToSDLWindow(int x, int y);

struct XYWH{};
struct LTRB{};
template <typename RectStyle = XYWH>
std::tuple<int, int, int, int> SDLWindowToGameRenderer(SDL_Rect const& rect);
template<> std::tuple<int, int, int, int> SDLWindowToGameRenderer<XYWH>(SDL_Rect const& rect);
template<> std::tuple<int, int, int, int> SDLWindowToGameRenderer<LTRB>(SDL_Rect const& rect);
template <typename RectStyle = XYWH>
SDL_Rect GameRendererToSDLWindow(int, int, int, int);
template<> SDL_Rect GameRendererToSDLWindow<XYWH>(int x, int y, int w, int h);
template<> SDL_Rect GameRendererToSDLWindow<LTRB>(int l, int t, int r, int b);

#endif // DESCENT3_SDLHELPERS_H
