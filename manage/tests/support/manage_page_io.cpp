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

#include "manage_page_io.h"

#include "manage.h"

namespace manage_page_io {

std::vector<uint8_t> WriteAllPages(CFILE *out, manage_page_builders::PageSet &s) {
  mng_WriteNewTexturePage(out, s.plain_texture.get());
  mng_WriteNewTexturePage(out, s.proc_texture.get());
  mng_WriteNewDoorPage(out, s.door.get());
  mng_WriteNewGenericPage(out, s.robot.get());
  mng_WriteNewGenericPage(out, s.powerup.get());
  mng_WriteNewUnknownPage(out);
  mng_WriteNewGamefilePage(out, s.gamefile.get());
  mng_WriteNewSoundPage(out, s.sound.get());
  mng_WriteNewShipPage(out, s.ship.get());
  mng_WriteNewWeaponPage(out, s.weapon.get());
  mng_WriteNewMegacellPage(out, s.megacell.get());
  return {PAGETYPE_TEXTURE,  PAGETYPE_TEXTURE, PAGETYPE_DOOR, PAGETYPE_GENERIC, PAGETYPE_GENERIC, PAGETYPE_UNKNOWN,
          PAGETYPE_GAMEFILE, PAGETYPE_SOUND,   PAGETYPE_SHIP, PAGETYPE_WEAPON,  PAGETYPE_MEGACELL};
}

} // namespace manage_page_io
