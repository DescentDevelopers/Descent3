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

// A table file is a sequence of pages. Each page is
//   [uint8 pagetype][int32 length][body]
// where length counts the length field itself plus the body. The loaders read
// the header and then hand the stream to the page reader without seeking, so
// every reader must consume exactly the body its writer produced; otherwise
// every following page is misparsed.

#include <cstdio>
#include <cstring>
#include <memory>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "cfile.h"
#include "d3_test/files.h"
#include "doorpage.h"
#include "gamefilepage.h"
#include "gametexture.h"
#include "genericpage.h"
#include "manage.h"
#include "megapage.h"
#include "object_external.h"
#include "shippage.h"
#include "soundpage.h"
#include "texpage.h"
#include "weaponpage.h"

#include "support/manage_page_builders.h"
#include "support/manage_page_io.h"

namespace {

using namespace manage_page_builders;
using namespace manage_page_io;

class PageFormatTest : public ::testing::Test {
protected:
  d3_test::TempDir dir_;
};

TEST_F(PageFormatTest, SoundPageRoundTrips) {
  auto in = MakeSoundPage();
  auto out = RoundTrip(dir_ / "page.bin", in.get(), mng_WriteNewSoundPage, mng_ReadNewSoundPage, PAGETYPE_SOUND);
  ASSERT_NE(out, nullptr);

  const sound_info &a = in->sound_struct, &b = out->sound_struct;
  EXPECT_STREQ(b.name, a.name);
  EXPECT_STREQ(out->raw_name, in->raw_name);
  EXPECT_EQ(b.flags, a.flags);
  EXPECT_EQ(b.loop_start, a.loop_start);
  EXPECT_EQ(b.loop_end, a.loop_end);
  EXPECT_EQ(b.outer_cone_volume, a.outer_cone_volume);
  EXPECT_EQ(b.inner_cone_angle, a.inner_cone_angle);
  EXPECT_EQ(b.outer_cone_angle, a.outer_cone_angle);
  EXPECT_EQ(b.max_distance, a.max_distance);
  EXPECT_EQ(b.min_distance, a.min_distance);
  EXPECT_EQ(b.import_volume, a.import_volume);
  EXPECT_TRUE(b.used);
}

TEST_F(PageFormatTest, DoorPageRoundTrips) {
  auto in = MakeDoorPage();
  auto out = RoundTrip(dir_ / "page.bin", in.get(), mng_WriteNewDoorPage, mng_ReadNewDoorPage, PAGETYPE_DOOR);
  ASSERT_NE(out, nullptr);

  const door &a = in->door_struct, &b = out->door_struct;
  EXPECT_STREQ(b.name, a.name);
  EXPECT_STREQ(out->image_name, in->image_name);
  EXPECT_EQ(b.total_open_time, a.total_open_time);
  EXPECT_EQ(b.total_close_time, a.total_close_time);
  EXPECT_EQ(b.total_time_open, a.total_time_open);
  EXPECT_EQ(b.flags, a.flags);
  EXPECT_EQ(b.hit_points, a.hit_points);
  EXPECT_STREQ(out->open_sound_name, in->open_sound_name);
  EXPECT_STREQ(out->close_sound_name, in->close_sound_name);
  EXPECT_STREQ(b.module_name, a.module_name);
  EXPECT_TRUE(b.used);
}

TEST_F(PageFormatTest, GamefilePageRoundTrips) {
  auto in = MakeGamefilePage();
  auto out =
      RoundTrip(dir_ / "page.bin", in.get(), mng_WriteNewGamefilePage, mng_ReadNewGamefilePage, PAGETYPE_GAMEFILE);
  ASSERT_NE(out, nullptr);

  EXPECT_STREQ(out->gamefile_struct.name, in->gamefile_struct.name);
  EXPECT_STREQ(out->gamefile_struct.dir_name, in->gamefile_struct.dir_name);
  EXPECT_TRUE(out->gamefile_struct.used);
}

TEST_F(PageFormatTest, MegacellPageRoundTrips) {
  auto in = MakeMegacellPage();
  auto out =
      RoundTrip(dir_ / "page.bin", in.get(), mng_WriteNewMegacellPage, mng_ReadNewMegacellPage, PAGETYPE_MEGACELL);
  ASSERT_NE(out, nullptr);

  EXPECT_STREQ(out->megacell_struct.name, in->megacell_struct.name);
  for (int i = 0; i < MAX_MEGACELL_WIDTH * MAX_MEGACELL_HEIGHT; i++)
    EXPECT_STREQ(out->cellname[i], in->cellname[i]) << "cell " << i;
  EXPECT_EQ(out->megacell_struct.width, in->megacell_struct.width);
  EXPECT_EQ(out->megacell_struct.height, in->megacell_struct.height);
  EXPECT_TRUE(out->megacell_struct.used);
}

TEST_F(PageFormatTest, PlainTexturePageRoundTrips) {
  auto in = MakeTexturePage(false);
  auto out = RoundTrip(dir_ / "page.bin", in.get(), mng_WriteNewTexturePage, mng_ReadNewTexturePage, PAGETYPE_TEXTURE);
  ASSERT_NE(out, nullptr);

  const texture &a = in->tex_struct, &b = out->tex_struct;
  EXPECT_STREQ(b.name, a.name);
  EXPECT_STREQ(out->bitmap_name, in->bitmap_name);
  EXPECT_STREQ(out->destroy_name, in->destroy_name);
  EXPECT_STREQ(out->sound_name, in->sound_name);
  EXPECT_EQ(b.r, a.r);
  EXPECT_EQ(b.g, a.g);
  EXPECT_EQ(b.b, a.b);
  EXPECT_EQ(b.alpha, a.alpha);
  EXPECT_EQ(b.speed, a.speed);
  EXPECT_EQ(b.slide_u, a.slide_u);
  EXPECT_EQ(b.slide_v, a.slide_v);
  EXPECT_EQ(b.reflectivity, a.reflectivity);
  EXPECT_EQ(b.corona_type, a.corona_type);
  EXPECT_EQ(b.damage, a.damage);
  EXPECT_EQ(b.flags, a.flags);
  EXPECT_EQ(b.sound_volume, a.sound_volume);
  EXPECT_EQ(out->num_proc_elements, 0);
  EXPECT_TRUE(b.used);
}

TEST_F(PageFormatTest, ProceduralTexturePageRoundTrips) {
  auto in = MakeTexturePage(true);
  auto out = RoundTrip(dir_ / "page.bin", in.get(), mng_WriteNewTexturePage, mng_ReadNewTexturePage, PAGETYPE_TEXTURE);
  ASSERT_NE(out, nullptr);

  EXPECT_EQ(out->tex_struct.flags, in->tex_struct.flags);
  for (int i = 0; i < 255; i++)
    EXPECT_EQ(out->proc_palette[i], in->proc_palette[i]) << "palette entry " << i;
  EXPECT_EQ(out->proc_heat, in->proc_heat);
  EXPECT_EQ(out->proc_light, in->proc_light);
  EXPECT_EQ(out->proc_thickness, in->proc_thickness);
  EXPECT_EQ(out->proc_evaluation_time, in->proc_evaluation_time);
  EXPECT_EQ(out->osc_time, in->osc_time);
  EXPECT_EQ(out->osc_value, in->osc_value);
  ASSERT_EQ(out->num_proc_elements, in->num_proc_elements);
  for (int i = 0; i < in->num_proc_elements; i++) {
    SCOPED_TRACE("element " + std::to_string(i));
    EXPECT_EQ(out->proc_type[i], in->proc_type[i]);
    EXPECT_EQ(out->proc_frequency[i], in->proc_frequency[i]);
    EXPECT_EQ(out->proc_speed[i], in->proc_speed[i]);
    EXPECT_EQ(out->proc_size[i], in->proc_size[i]);
    EXPECT_EQ(out->proc_x1[i], in->proc_x1[i]);
    EXPECT_EQ(out->proc_y1[i], in->proc_y1[i]);
    EXPECT_EQ(out->proc_x2[i], in->proc_x2[i]);
    EXPECT_EQ(out->proc_y2[i], in->proc_y2[i]);
  }
  EXPECT_STREQ(out->sound_name, in->sound_name);
  EXPECT_EQ(out->tex_struct.sound_volume, in->tex_struct.sound_volume);
}

TEST_F(PageFormatTest, WeaponPageRoundTrips) {
  auto in = MakeWeaponPage();
  auto out = RoundTrip(dir_ / "page.bin", in.get(), mng_WriteNewWeaponPage, mng_ReadNewWeaponPage, PAGETYPE_WEAPON);
  ASSERT_NE(out, nullptr);

  const weapon &a = in->weapon_struct, &b = out->weapon_struct;
  EXPECT_STREQ(b.name, a.name);
  EXPECT_STREQ(out->hud_image_name, in->hud_image_name);
  EXPECT_STREQ(out->fire_image_name, in->fire_image_name);
  EXPECT_STREQ(out->particle_name, in->particle_name);
  EXPECT_EQ(b.particle_count, a.particle_count);
  EXPECT_EQ(b.particle_life, a.particle_life);
  EXPECT_EQ(b.particle_size, a.particle_size);
  EXPECT_EQ(b.flags, a.flags);
  EXPECT_STREQ(out->spawn_name, in->spawn_name);
  EXPECT_EQ(b.spawn_count, a.spawn_count);
  EXPECT_STREQ(out->robot_spawn_name, in->robot_spawn_name);
  EXPECT_STREQ(out->alternate_spawn_name, in->alternate_spawn_name);
  EXPECT_EQ(b.alternate_chance, a.alternate_chance);
  EXPECT_EQ(b.gravity_time, a.gravity_time);
  EXPECT_EQ(b.gravity_size, a.gravity_size);
  EXPECT_EQ(b.homing_fov, a.homing_fov);
  EXPECT_EQ(b.custom_size, a.custom_size);
  EXPECT_EQ(b.size, a.size);
  EXPECT_EQ(b.thrust_time, a.thrust_time);
  ExpectPhysicsEq(a.phys_info, b.phys_info);
  EXPECT_EQ(b.terrain_damage_size, a.terrain_damage_size);
  EXPECT_EQ(b.terrain_damage_depth, a.terrain_damage_depth);
  EXPECT_EQ(b.alpha, a.alpha);
  EXPECT_STREQ(out->explode_image_name, in->explode_image_name);
  EXPECT_EQ(b.explode_time, a.explode_time);
  EXPECT_EQ(b.explode_size, a.explode_size);
  EXPECT_EQ(b.player_damage, a.player_damage);
  EXPECT_EQ(b.generic_damage, a.generic_damage);
  EXPECT_EQ(b.impact_size, a.impact_size);
  EXPECT_EQ(b.impact_time, a.impact_time);
  EXPECT_EQ(b.impact_player_damage, a.impact_player_damage);
  EXPECT_EQ(b.impact_generic_damage, a.impact_generic_damage);
  EXPECT_EQ(b.impact_force, a.impact_force);
  EXPECT_EQ(b.life_time, a.life_time);
  ExpectLightingEq(a.lighting_info, b.lighting_info);
  EXPECT_EQ(b.recoil_force, a.recoil_force);
  for (int i = 0; i < MAX_WEAPON_SOUNDS; i++)
    EXPECT_STREQ(out->sound_name[i], in->sound_name[i]) << "sound " << i;
  EXPECT_STREQ(out->smoke_image_name, in->smoke_image_name);
  EXPECT_STREQ(out->scorch_image_name, in->scorch_image_name);
  EXPECT_EQ(b.scorch_size, a.scorch_size);
  EXPECT_STREQ(out->icon_name, in->icon_name);
  EXPECT_TRUE(b.used);
}

TEST_F(PageFormatTest, ShipPageRoundTrips) {
  auto in = MakeShipPage();
  auto out = RoundTrip(dir_ / "page.bin", in.get(), mng_WriteNewShipPage, mng_ReadNewShipPage, PAGETYPE_SHIP);
  ASSERT_NE(out, nullptr);

  const ship &a = in->ship_struct, &b = out->ship_struct;
  EXPECT_STREQ(b.name, a.name);
  EXPECT_STREQ(b.cockpit_name, a.cockpit_name);
  EXPECT_STREQ(b.hud_config_name, a.hud_config_name);
  EXPECT_STREQ(out->image_name, in->image_name);
  EXPECT_STREQ(out->dying_image_name, in->dying_image_name);
  EXPECT_STREQ(out->med_image_name, in->med_image_name);
  EXPECT_STREQ(out->lo_image_name, in->lo_image_name);
  EXPECT_EQ(b.med_lod_distance, a.med_lod_distance);
  EXPECT_EQ(b.lo_lod_distance, a.lo_lod_distance);
  ExpectPhysicsEq(a.phys_info, b.phys_info);
  EXPECT_EQ(b.size, a.size);
  EXPECT_EQ(b.armor_scalar, a.armor_scalar);
  EXPECT_EQ(b.flags, a.flags);
  for (int i = 0; i < MAX_PLAYER_WEAPONS; i++) {
    SCOPED_TRACE("weapon " + std::to_string(i));
    EXPECT_EQ(b.fire_flags[i], a.fire_flags[i]);
    EXPECT_STREQ(out->firing_sound_name[i], in->firing_sound_name[i]);
    EXPECT_STREQ(out->release_sound_name[i], in->release_sound_name[i]);
    EXPECT_STREQ(out->spew_powerup_name[i], in->spew_powerup_name[i]);
    EXPECT_EQ(b.max_ammo[i], a.max_ammo[i]);
    ExpectWeaponBatteryEq(a.static_wb[i], b.static_wb[i]);
    for (int j = 0; j < MAX_WB_GUNPOINTS; j++)
      EXPECT_STREQ(out->weapon_name[i][j], in->weapon_name[i][j]) << "gunpoint " << j;
    for (int j = 0; j < MAX_WB_FIRING_MASKS; j++)
      EXPECT_STREQ(out->fire_sound_name[i][j], in->fire_sound_name[i][j]) << "mask " << j;
  }
  EXPECT_TRUE(b.used);
}

TEST_F(PageFormatTest, GenericPageRoundTrips) {
  auto in = Own(MakeGenericPage());
  auto raw = RoundTrip(dir_ / "page.bin", in.get(), mng_WriteNewGenericPage, mng_ReadNewGenericPage, PAGETYPE_GENERIC);
  ASSERT_NE(raw, nullptr);
  auto out = Own(std::move(raw));

  const object_info &a = in->objinfo_struct, &b = out->objinfo_struct;
  EXPECT_EQ(b.type, a.type);
  EXPECT_STREQ(b.name, a.name);
  EXPECT_STREQ(out->image_name, in->image_name);
  EXPECT_STREQ(out->med_image_name, in->med_image_name);
  EXPECT_STREQ(out->lo_image_name, in->lo_image_name);
  EXPECT_EQ(b.impact_size, a.impact_size);
  EXPECT_EQ(b.impact_time, a.impact_time);
  EXPECT_EQ(b.damage, a.damage);
  EXPECT_EQ(b.score, a.score);
  EXPECT_STREQ(b.module_name, a.module_name);
  EXPECT_STREQ(b.script_name_override, a.script_name_override);
  ASSERT_NE(b.description, nullptr);
  EXPECT_STREQ(b.description, a.description);
  EXPECT_STREQ(b.icon_name, a.icon_name);
  EXPECT_EQ(b.med_lod_distance, a.med_lod_distance);
  EXPECT_EQ(b.lo_lod_distance, a.lo_lod_distance);
  ExpectPhysicsEq(a.phys_info, b.phys_info);
  EXPECT_EQ(b.size, a.size);
  ExpectLightingEq(a.lighting_info, b.lighting_info);
  EXPECT_EQ(b.hit_points, a.hit_points);
  EXPECT_EQ(b.flags, a.flags);

  const t_ai_info &ai = in->ai_info, &bi = out->ai_info;
  EXPECT_EQ(bi.flags, ai.flags);
  EXPECT_EQ(bi.ai_class, ai.ai_class);
  EXPECT_EQ(bi.ai_type, ai.ai_type);
  EXPECT_EQ(bi.movement_type, ai.movement_type);
  EXPECT_EQ(bi.movement_subtype, ai.movement_subtype);
  EXPECT_EQ(bi.fov, ai.fov);
  EXPECT_EQ(bi.max_velocity, ai.max_velocity);
  EXPECT_EQ(bi.max_turn_rate, ai.max_turn_rate);
  EXPECT_EQ(bi.circle_distance, ai.circle_distance);
  EXPECT_EQ(bi.melee_damage[1], ai.melee_damage[1]);
  EXPECT_EQ(bi.melee_latency[1], ai.melee_latency[1]);
  EXPECT_EQ(bi.curiousity, ai.curiousity);
  EXPECT_EQ(bi.life_preservation, ai.life_preservation);
  EXPECT_EQ(bi.avoid_friends_distance, ai.avoid_friends_distance);
  EXPECT_EQ(bi.biased_flight_importance, ai.biased_flight_importance);
  EXPECT_EQ(bi.biased_flight_min, ai.biased_flight_min);
  EXPECT_EQ(bi.biased_flight_max, ai.biased_flight_max);

  EXPECT_EQ(b.f_dspew, a.f_dspew);
  for (int i = 0; i < MAX_DSPEW_TYPES; i++) {
    SCOPED_TRACE("dspew " + std::to_string(i));
    EXPECT_EQ(b.dspew_percent[i], a.dspew_percent[i]);
    EXPECT_EQ(b.dspew_number[i], a.dspew_number[i]);
    EXPECT_STREQ(out->dspew_name[i], in->dspew_name[i]);
  }

  for (int i = 0; i < NUM_MOVEMENT_CLASSES; i++) {
    for (int j = 0; j < NUM_ANIMS_PER_CLASS; j++) {
      SCOPED_TRACE("anim " + std::to_string(i) + "/" + std::to_string(j));
      EXPECT_EQ(out->anim[i].elem[j].from, in->anim[i].elem[j].from);
      EXPECT_EQ(out->anim[i].elem[j].to, in->anim[i].elem[j].to);
      EXPECT_EQ(out->anim[i].elem[j].spc, in->anim[i].elem[j].spc);
      EXPECT_STREQ(out->anim_sound_name[i][j], in->anim_sound_name[i][j]);
    }
  }

  for (int i = 0; i < MAX_WBS_PER_OBJ; i++) {
    SCOPED_TRACE("battery " + std::to_string(i));
    ExpectWeaponBatteryEq(in->static_wb[i], out->static_wb[i]);
    for (int j = 0; j < MAX_WB_GUNPOINTS; j++)
      EXPECT_STREQ(out->weapon_name[i][j], in->weapon_name[i][j]) << "gunpoint " << j;
    for (int j = 0; j < MAX_WB_FIRING_MASKS; j++)
      EXPECT_STREQ(out->fire_sound_name[i][j], in->fire_sound_name[i][j]) << "mask " << j;
  }

  for (int i = 0; i < MAX_OBJ_SOUNDS; i++)
    EXPECT_STREQ(out->sound_name[i], in->sound_name[i]) << "sound " << i;
  for (int i = 0; i < MAX_AI_SOUNDS; i++)
    EXPECT_STREQ(out->ai_sound_name[i], in->ai_sound_name[i]) << "ai sound " << i;

  EXPECT_EQ(b.respawn_scalar, a.respawn_scalar);
  for (int i = 0; i < MAX_DEATH_TYPES; i++) {
    SCOPED_TRACE("death type " + std::to_string(i));
    EXPECT_EQ(b.death_types[i].flags, a.death_types[i].flags);
    EXPECT_EQ(b.death_types[i].delay_min, a.death_types[i].delay_min);
    EXPECT_EQ(b.death_types[i].delay_max, a.death_types[i].delay_max);
    EXPECT_EQ(b.death_probabilities[i], a.death_probabilities[i]);
  }
}

TEST_F(PageFormatTest, MaximumLengthNamesRoundTrip) {
  const std::string long_name(PAGENAME_LEN - 1, 'N');
  const std::string long_raw(PAGENAME_LEN - 1, 'R');
  auto in = MakeSoundPage();
  strcpy(in->sound_struct.name, long_name.c_str());
  strcpy(in->raw_name, long_raw.c_str());

  auto out = RoundTrip(dir_ / "page.bin", in.get(), mng_WriteNewSoundPage, mng_ReadNewSoundPage, PAGETYPE_SOUND);
  ASSERT_NE(out, nullptr);
  EXPECT_EQ(std::string(out->sound_struct.name), long_name);
  EXPECT_EQ(std::string(out->raw_name), long_raw);
  EXPECT_EQ(out->sound_struct.import_volume, in->sound_struct.import_volume);
}

TEST_F(PageFormatTest, GenericPageWithoutDescriptionReadsNullDescription) {
  auto in = Own(MakeGenericPage(OBJ_ROBOT, nullptr));
  auto raw = RoundTrip(dir_ / "page.bin", in.get(), mng_WriteNewGenericPage, mng_ReadNewGenericPage, PAGETYPE_GENERIC);
  ASSERT_NE(raw, nullptr);
  auto out = Own(std::move(raw));

  EXPECT_EQ(out->objinfo_struct.description, nullptr);
  EXPECT_STREQ(out->objinfo_struct.icon_name, in->objinfo_struct.icon_name);
  EXPECT_EQ(out->objinfo_struct.respawn_scalar, in->objinfo_struct.respawn_scalar);
}

TEST_F(PageFormatTest, GenericPageStoresAmmoCountForPowerups) {
  auto in = Own(MakeGenericPage(OBJ_POWERUP));
  ASSERT_NE(in->objinfo_struct.ammo_count, 0);
  auto raw = RoundTrip(dir_ / "page.bin", in.get(), mng_WriteNewGenericPage, mng_ReadNewGenericPage, PAGETYPE_GENERIC);
  ASSERT_NE(raw, nullptr);
  auto out = Own(std::move(raw));

  EXPECT_EQ(out->objinfo_struct.type, OBJ_POWERUP);
  EXPECT_EQ(out->objinfo_struct.ammo_count, in->objinfo_struct.ammo_count);
  EXPECT_STREQ(out->objinfo_struct.module_name, in->objinfo_struct.module_name);
  EXPECT_EQ(out->objinfo_struct.respawn_scalar, in->objinfo_struct.respawn_scalar);
}

TEST_F(PageFormatTest, GenericPageDoesNotStoreAmmoCountForNonPowerups) {
  auto in = Own(MakeGenericPage(OBJ_ROBOT));
  in->objinfo_struct.ammo_count = 99;
  auto raw = RoundTrip(dir_ / "page.bin", in.get(), mng_WriteNewGenericPage, mng_ReadNewGenericPage, PAGETYPE_GENERIC);
  ASSERT_NE(raw, nullptr);
  auto out = Own(std::move(raw));

  EXPECT_EQ(out->objinfo_struct.ammo_count, 0);
  EXPECT_STREQ(out->objinfo_struct.module_name, in->objinfo_struct.module_name);
}

// AI_NOTIFIES_ALWAYS_ON are notifications every AI gets; they are kept out of
// the file.
TEST_F(PageFormatTest, GenericPageReaderForcesAlwaysOnNotifications) {
  auto in = Own(MakeGenericPage());
  const int stored_notifies = in->ai_info.notify_flags & ~AI_NOTIFIES_ALWAYS_ON;
  in->ai_info.notify_flags = stored_notifies;
  auto raw = RoundTrip(dir_ / "page.bin", in.get(), mng_WriteNewGenericPage, mng_ReadNewGenericPage, PAGETYPE_GENERIC);
  ASSERT_NE(raw, nullptr);
  auto out = Own(std::move(raw));

  EXPECT_EQ(out->ai_info.notify_flags, stored_notifies | AI_NOTIFIES_ALWAYS_ON);
}

TEST_F(PageFormatTest, TextureReaderClearsProceduralFlagWhenThereAreNoElements) {
  auto in = MakeTexturePage(true);
  in->num_proc_elements = 0;
  auto out = RoundTrip(dir_ / "page.bin", in.get(), mng_WriteNewTexturePage, mng_ReadNewTexturePage, PAGETYPE_TEXTURE);
  ASSERT_NE(out, nullptr);

  EXPECT_EQ(out->tex_struct.flags & TF_PROCEDURAL, 0);
  EXPECT_EQ(out->tex_struct.flags, in->tex_struct.flags & ~TF_PROCEDURAL) << "other flags are kept";
  EXPECT_STREQ(out->sound_name, in->sound_name);
}

// The editor used "INVALID..." as a placeholder for "no destroyed bitmap".
TEST_F(PageFormatTest, TextureReaderTreatsInvalidDestroyNameAsNone) {
  auto in = MakeTexturePage(false);
  strcpy(in->destroy_name, "INVALID NAME");
  auto out = RoundTrip(dir_ / "page.bin", in.get(), mng_WriteNewTexturePage, mng_ReadNewTexturePage, PAGETYPE_TEXTURE);
  ASSERT_NE(out, nullptr);

  EXPECT_STREQ(out->destroy_name, "");
  EXPECT_STREQ(out->bitmap_name, in->bitmap_name);
}

class TableFileTest : public PageFormatTest {};

TEST_F(TableFileTest, ReadDummyPageSkipsEveryPageType) {
  PageSet pages;
  const auto file = dir_ / "table.gam";
  CFILE *out = cfopen(file, "wb");
  ASSERT_NE(out, nullptr);
  const auto expected_types = WriteAllPages(out, pages);
  cfclose(out);

  CFILE *in = cfopen(file, "rb");
  ASSERT_NE(in, nullptr);
  std::vector<uint8_t> seen_types;
  while (!cfeof(in)) {
    const uint8_t pagetype = static_cast<uint8_t>(cf_ReadByte(in));
    const long len_pos = cftell(in);
    const int len = cf_ReadInt(in);
    seen_types.push_back(pagetype);
    SCOPED_TRACE("page " + std::to_string(seen_types.size()) + " type " + std::to_string(pagetype));
    mng_ReadDummyPage(in, pagetype);
    ASSERT_EQ(cftell(in), len_pos + len);
  }
  cfclose(in);
  EXPECT_EQ(seen_types, expected_types);
}

TEST_F(TableFileTest, ReadWriteDummyPageCopiesEveryPageTypeVerbatim) {
  PageSet pages;
  const auto src = dir_ / "src.gam";
  const auto dst = dir_ / "dst.gam";
  CFILE *out = cfopen(src, "wb");
  ASSERT_NE(out, nullptr);
  WriteAllPages(out, pages);
  cfclose(out);

  CFILE *in = cfopen(src, "rb");
  ASSERT_NE(in, nullptr);
  out = cfopen(dst, "wb");
  ASSERT_NE(out, nullptr);
  while (!cfeof(in)) {
    const uint8_t pagetype = static_cast<uint8_t>(cf_ReadByte(in));
    cf_ReadInt(in); // length
    mng_ReadWriteDummyPage(in, out, pagetype);
  }
  cfclose(out);
  cfclose(in);

  const auto src_bytes = d3_test::ReadFileBytes(src);
  const auto dst_bytes = d3_test::ReadFileBytes(dst);
  ASSERT_FALSE(src_bytes.empty());
  ASSERT_EQ(dst_bytes.size(), src_bytes.size());
  EXPECT_TRUE(dst_bytes == src_bytes);
}

} // namespace
