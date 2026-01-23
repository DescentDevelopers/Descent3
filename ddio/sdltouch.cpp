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

#include <algorithm>

#include "sdltouch.h"
#include "SDLHelpers.h"

namespace {

std::vector<TouchPoint> currentTouches;

} // namespace (anonymous)

std::vector<TouchPoint> const& ddio_GetCurrentTouches() {
  return currentTouches;
}

void sdlTouchEvent(SDL_Event const *event) {
  auto matchesTouchId = [id = event->tfinger.fingerID](auto const &t) { return t.id == id; };
  switch (event->type) {
  case SDL_EVENT_FINGER_DOWN: {

    auto const& [x, y] = SDLNormalizedToGameRenderer(event->tfinger.x, event->tfinger.y);
    currentTouches.push_back({event->tfinger.fingerID, x, y});
    break;
  }
  case SDL_EVENT_FINGER_MOTION: {
    auto it = std::find_if(std::begin(currentTouches), std::end(currentTouches),
                           matchesTouchId);
    std::tie(it->x, it->y) = SDLNormalizedToGameRenderer(event->tfinger.x, event->tfinger.y);
    break;
  }
  case SDL_EVENT_FINGER_UP: // fallthrough
  case SDL_EVENT_FINGER_CANCELED: {
    for (auto it = std::begin(currentTouches); it != std::end(currentTouches); it++) {
      if (matchesTouchId(*it)) {
        currentTouches.erase(it);
        break;
      }
    }
    break;
  }
  }
}