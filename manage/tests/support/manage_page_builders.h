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

#include <cstddef>
#include <memory>

#include "doorpage.h"
#include "gamefilepage.h"
#include "genericpage.h"
#include "megapage.h"
#include "object_external.h"
#include "shippage.h"
#include "soundpage.h"
#include "texpage.h"
#include "weaponpage.h"

namespace manage_page_builders {

void SetStr(char *dst, size_t size, const char *fmt, ...);
#define SET_STR(dst, ...) SetStr(dst, sizeof(dst), __VA_ARGS__)

// Produces a distinct, exactly representable float on every call.
class FloatSeq {
public:
  explicit FloatSeq(float start) : v_(start) {}
  float operator()() { return v_ += 0.25f; }

private:
  float v_;
};

void FillPhysics(physics_info &p, FloatSeq &f);
void ExpectPhysicsEq(const physics_info &a, const physics_info &b);
void FillLighting(light_info &l, FloatSeq &f);
void ExpectLightingEq(const light_info &a, const light_info &b);
void FillWeaponBattery(otype_wb_info &wb, int seed, FloatSeq &f);
void ExpectWeaponBatteryEq(const otype_wb_info &a, const otype_wb_info &b);

std::unique_ptr<mngs_sound_page> MakeSoundPage();
std::unique_ptr<mngs_door_page> MakeDoorPage();
std::unique_ptr<mngs_gamefile_page> MakeGamefilePage();
std::unique_ptr<mngs_megacell_page> MakeMegacellPage();
std::unique_ptr<mngs_texture_page> MakeTexturePage(bool procedural);
std::unique_ptr<mngs_weapon_page> MakeWeaponPage();
std::unique_ptr<mngs_ship_page> MakeShipPage();

// The returned page owns objinfo_struct.description (allocated with mem_rmalloc
// when non-null); release it with FreeGenericPage.
std::unique_ptr<mngs_generic_page> MakeGenericPage(int type = OBJ_ROBOT, const char *description = "A mean robot");

void FreeGenericPage(mngs_generic_page *p);

struct GenericPageDeleter {
  void operator()(mngs_generic_page *p) const {
    FreeGenericPage(p);
    delete p;
  }
};
using GenericPagePtr = std::unique_ptr<mngs_generic_page, GenericPageDeleter>;

GenericPagePtr Own(std::unique_ptr<mngs_generic_page> p);

// One page of every type that has a writer, built with the Make* functions above.
struct PageSet {
  std::unique_ptr<mngs_texture_page> plain_texture = MakeTexturePage(false);
  std::unique_ptr<mngs_texture_page> proc_texture = MakeTexturePage(true);
  std::unique_ptr<mngs_door_page> door = MakeDoorPage();
  GenericPagePtr robot = Own(MakeGenericPage(OBJ_ROBOT));
  GenericPagePtr powerup = Own(MakeGenericPage(OBJ_POWERUP, nullptr));
  std::unique_ptr<mngs_gamefile_page> gamefile = MakeGamefilePage();
  std::unique_ptr<mngs_sound_page> sound = MakeSoundPage();
  std::unique_ptr<mngs_ship_page> ship = MakeShipPage();
  std::unique_ptr<mngs_weapon_page> weapon = MakeWeaponPage();
  std::unique_ptr<mngs_megacell_page> megacell = MakeMegacellPage();
};

} // namespace manage_page_builders
