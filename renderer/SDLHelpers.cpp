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

#include <SDL3/SDL.h>

#include "SDLHelpers.h"
#include "application.h"

extern oeLnxApplication* Lnx_app_obj;
extern SDL_Window* GSDLWindow;

namespace {

std::tuple<int, int> SDLWindowSize() {
  int w, h;
  SDL_GetWindowSize(GSDLWindow, &w, &h);
  return {w, h};
}

std::tuple<double, double> factorAndOffset(std::tuple<int, int> sdlWindowSize) {
  auto const& [w, h] = sdlWindowSize;
  auto factor = static_cast<double>(Lnx_app_obj->m_H) / h;
  auto w_overrun = (factor * w - Lnx_app_obj->m_W) / 2;
  return {factor, w_overrun};
}

std::tuple<int, int> applyFactor(int x, int y, double factor, double x_offset) {
  return {
      static_cast<int>(x * factor - x_offset),
      static_cast<int>(y * factor),
  };
}

std::tuple<int, int> removeFactor(int x, int y, double factor, double x_offset) {
  return {
      static_cast<int>((x + x_offset) / factor),
      static_cast<int>(y / factor),
  };
}

} // namespace

std::tuple<int, int> SDLNormalizedToGameRenderer(double x, double y) {
  auto const& windowSize = SDLWindowSize();
  auto const& [w, h] = windowSize;
  auto const& [factor, x_offset] = factorAndOffset(windowSize);

  return applyFactor(x * w, y * h, factor, x_offset);
}

std::tuple<int, int> SDLWindowToGameRenderer(int x, int y) {
  auto const& [factor, x_offset] = factorAndOffset(SDLWindowSize());
  return applyFactor(x, y, factor, x_offset);
}

std::tuple<int, int> GameRendererToSDLWindow(int x, int y) {
  auto const& [factor, x_offset] = factorAndOffset(SDLWindowSize());
  return removeFactor(x, y, factor, x_offset);
}

template<> std::tuple<int, int, int, int> SDLWindowToGameRenderer<XYWH>(SDL_Rect const& rect) {
  auto const& [factor, x_offset] = factorAndOffset(SDLWindowSize());
  auto const& [x, y] = applyFactor(rect.x, rect.y, factor, x_offset);
  auto const& [w, h] = applyFactor(rect.w, rect.h, factor, 0);
  return {x, y, w, h};
}

template<> std::tuple<int, int, int, int> SDLWindowToGameRenderer<LTRB>(SDL_Rect const& rect) {
  auto const& [x, y, w, h] = SDLWindowToGameRenderer<XYWH>(rect);
  return {x, y, x + w, y + h};
}

template<> SDL_Rect GameRendererToSDLWindow<XYWH>(int x, int y, int w, int h) {
  SDL_Rect rect{};
  auto const& [factor, x_offset] = factorAndOffset(SDLWindowSize());
  std::tie(rect.x, rect.y) = removeFactor(x, y, factor, x_offset);
  std::tie(rect.w, rect.h) = removeFactor(w, h, factor, 0);
  return rect;
}

template<> SDL_Rect GameRendererToSDLWindow<LTRB>(int l, int t, int r, int b) {
  return GameRendererToSDLWindow<XYWH>(l, t, r - l, b - t);
}