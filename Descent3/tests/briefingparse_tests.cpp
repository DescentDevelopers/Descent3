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

// Most fixture lines use the exact layout the briefing editor writes (see BriefEditSaveScreens in
// editor/BriefEdit.cpp), including the doubled spaces it leaves where an optional field is empty.
//
// ParseBriefing reports PBERR_NOERR even when the file contains an error; the observable effect of an error is
// that parsing stops, so error tests check that nothing after the bad line is delivered.

#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "BriefingParse.h"
#include "TelComEffects.h"
#include "d3_test/files.h"
#include "gamefont.h"
#include "support/briefing_parse.h"
#include "support/briefing_recorder.h"

namespace {

using namespace briefing_test;

class BriefingParseTest : public ::testing::Test {
protected:
  void SetUp() override {
    rec = &r;
    tBriefParseCallbacks cb = {};
    cb.AddTextEffect = OnText;
    cb.AddBmpEffect = OnBitmap;
    cb.AddMovieEffect = OnMovie;
    cb.AddBkgEffect = OnBkg;
    cb.AddPolyEffect = OnPoly;
    cb.AddSoundEffect = OnSound;
    cb.AddButtonEffect = OnButton;
    cb.StartScreen = OnStartScreen;
    cb.EndScreen = OnEndScreen;
    cb.LoopCallback = nullptr;
    cb.SetTitle = OnTitle;
    cb.SetStatic = OnStatic;
    cb.SetGlitch = OnGlitch;
    cb.AddVoice = OnVoice;
    parser.SetCallbacks(&cb);
  }
  void TearDown() override { rec = nullptr; }

  using Events = std::vector<std::string>;

  Recorder r;
  CBriefParse parser;
  d3_test::TempDir dir;
};

TEST_F(BriefingParseTest, MissingFileIsReported) {
  auto missing = dir.path() / "missing.brf";
  EXPECT_EQ(parser.ParseBriefing(missing.string().c_str()), PBERR_FILENOTEXIST);
  EXPECT_TRUE(r.events.empty());
}

TEST_F(BriefingParseTest, GlobalSettings) {
  ASSERT_EQ(Parse(parser, dir,
                  "$title \"Mission One\"\n"
                  "$glitch 0.250\n"
                  "$static 0.500\n"),
            PBERR_NOERR);

  EXPECT_EQ(r.titles, (std::vector<std::string>{"Mission One"}));
  ASSERT_EQ(r.glitches.size(), 1u);
  EXPECT_FLOAT_EQ(r.glitches[0], 0.25f);
  ASSERT_EQ(r.statics.size(), 1u);
  EXPECT_FLOAT_EQ(r.statics[0], 0.5f);
}

TEST_F(BriefingParseTest, ScreensAreDeliveredInOrderWithTheirSettings) {
  ASSERT_EQ(Parse(parser, dir,
                  "$screen 0 isset 2 isset 4  isnset 5  desc \"Intro\" layout \"lay.scr\" start\n"
                  "$endscreen\n"
                  "\n"
                  "$screen 1   desc \"Second\" start\n"
                  "$endscreen\n"),
            PBERR_NOERR);

  EXPECT_EQ(r.events, (Events{"screen", "endscreen", "screen", "endscreen"}));
  ASSERT_EQ(r.screens.size(), 2u);

  EXPECT_EQ(r.screens[0].num, 0);
  EXPECT_EQ(r.screens[0].description, "Intro");
  EXPECT_TRUE(r.screens[0].has_layout);
  EXPECT_EQ(r.screens[0].layout, "lay.scr");
  EXPECT_EQ(r.screens[0].mask_set, (1u << 2) | (1u << 4));
  EXPECT_EQ(r.screens[0].mask_unset, 1u << 5);

  EXPECT_EQ(r.screens[1].num, 1);
  EXPECT_EQ(r.screens[1].description, "Second");
  EXPECT_FALSE(r.screens[1].has_layout);
  EXPECT_EQ(r.screens[1].mask_set, 0u);
  EXPECT_EQ(r.screens[1].mask_unset, 0u);
}

TEST_F(BriefingParseTest, KeywordsAreCaseInsensitive) {
  ASSERT_EQ(Parse(parser, dir,
                  "$SCREEN 3 DESC \"x\" START\n"
                  "$EndScreen\n"),
            PBERR_NOERR);

  EXPECT_EQ(r.events, (Events{"screen", "endscreen"}));
  ASSERT_EQ(r.screens.size(), 1u);
  EXPECT_EQ(r.screens[0].num, 3);
}

TEST_F(BriefingParseTest, CommentsAndBlankLinesAreIgnored) {
  ASSERT_EQ(Parse(parser, dir,
                  "; a comment\n"
                  "\n"
                  "$screen 0   desc \"s\" start\n"
                  ";another comment\n"
                  "$endscreen\n"),
            PBERR_NOERR);

  EXPECT_EQ(r.events, (Events{"screen", "endscreen"}));
}

TEST_F(BriefingParseTest, TextBlockWithEditorSettings) {
  ASSERT_EQ(ParseInScreen(parser, dir,
                          "$text effect fade_in font lg_brief  speed 1.500 box 10,300,20,200 color 255,128,0 "
                          "desc \"body\" id 7 starttime 2.000 isset 1  show\n"
                          "First line\n"
                          "Second line\n"
                          "$endtext\n"),
            PBERR_NOERR);

  EXPECT_EQ(r.events, (Events{"screen", "text", "endscreen"}));
  ASSERT_EQ(r.texts.size(), 1u);
  const Text &t = r.texts[0];
  EXPECT_EQ(t.text, "First line\nSecond line");
  EXPECT_EQ(t.description, "body");
  EXPECT_EQ(t.id, 7);
  EXPECT_EQ(t.desc.type, TC_TEXT_FADE);
  EXPECT_EQ(t.desc.flags, TC_TEXTF_IN);

  ASSERT_TRUE(t.desc.caps & TCTD_FONT);
  EXPECT_EQ(t.desc.font, BBRIEF_FONT_INDEX);
  ASSERT_TRUE(t.desc.caps & TCTD_SPEED);
  EXPECT_FLOAT_EQ(t.desc.speed, 1.5f);
  ASSERT_TRUE(t.desc.caps & TCTD_TEXTBOX);
  EXPECT_EQ(t.desc.textbox.left, 10);
  EXPECT_EQ(t.desc.textbox.right, 300);
  EXPECT_EQ(t.desc.textbox.top, 20);
  EXPECT_EQ(t.desc.textbox.bottom, 200);
  ASSERT_TRUE(t.desc.caps & TCTD_COLOR);
  EXPECT_EQ(GR_COLOR_RED(t.desc.color), 255);
  EXPECT_EQ(GR_COLOR_GREEN(t.desc.color), 128);
  EXPECT_EQ(GR_COLOR_BLUE(t.desc.color), 0);
  ASSERT_TRUE(t.desc.caps & TCTD_WAITTIME);
  EXPECT_FLOAT_EQ(t.desc.waittime, 2.0f);
  EXPECT_FALSE(t.desc.caps & TCTD_TABSTOP);
  EXPECT_EQ(t.desc.mission_mask_set, 1u << 1);
  EXPECT_EQ(t.desc.mission_mask_unset, 0u);
}

TEST_F(BriefingParseTest, TextBlockKeepsBlankAndSemicolonLinesVerbatim) {
  ASSERT_EQ(ParseInScreen(parser, dir,
                          "$text effect None font sm_brief tabstop speed 1.000 box 0,10,0,10 color 0,0,0 "
                          "desc \"\" id 0 starttime 0.000   show\n"
                          "a\n"
                          "\n"
                          "; not a comment\n"
                          "b\n"
                          "$endtext\n"),
            PBERR_NOERR);

  ASSERT_EQ(r.texts.size(), 1u);
  EXPECT_EQ(r.texts[0].text, "a\n\n; not a comment\nb");
  EXPECT_EQ(r.texts[0].desc.font, BRIEF_FONT_INDEX);
  EXPECT_EQ(r.texts[0].desc.type, TC_TEXT_STATIC);
  EXPECT_TRUE(r.texts[0].desc.caps & TCTD_TABSTOP);
}

TEST_F(BriefingParseTest, TextLinesLongerThanTheReadBufferAreKeptWhole) {
  // ReadFullLine reads through a 512-byte buffer.
  std::vector<std::string> lines = {std::string(511, 'a'), std::string(512, 'b'), std::string(513, 'c'),
                                    std::string(1500, 'd')};
  std::string body = "$text effect None font sm_brief  speed 1.000 box 0,10,0,10 color 0,0,0 desc \"\" id 0 "
                     "starttime 0.000   show\n";
  for (auto &l : lines)
    body += l + "\n";
  body += "$endtext\n";

  ASSERT_EQ(ParseInScreen(parser, dir, body), PBERR_NOERR);

  ASSERT_EQ(r.texts.size(), 1u);
  EXPECT_EQ(r.texts[0].text, lines[0] + "\n" + lines[1] + "\n" + lines[2] + "\n" + lines[3]);
}

struct TextEffectCase {
  const char *name;
  int type;
  int flags;
};
void PrintTo(const TextEffectCase &c, std::ostream *os) { *os << c.name; }

class BriefingTextEffectTest : public BriefingParseTest, public ::testing::WithParamInterface<TextEffectCase> {};

TEST_P(BriefingTextEffectTest, EffectNameSelectsTypeAndFlags) {
  const TextEffectCase &c = GetParam();
  ASSERT_EQ(ParseInScreen(parser, dir,
                          std::string("$text effect ") + c.name +
                              " font sm_brief  speed 1.000 box 0,10,0,10 color 0,0,0 desc \"\" id 0 starttime 0.000   "
                              "show\nx\n$endtext\n"),
            PBERR_NOERR);

  ASSERT_EQ(r.texts.size(), 1u);
  EXPECT_EQ(r.texts[0].desc.type, c.type);
  if (c.type != TC_TEXT_STATIC && c.type != TC_TEXT_FLASH)
    EXPECT_EQ(r.texts[0].desc.flags, c.flags);
}

// Names as written by the editor (plus fade_in_and_out, which shares a prefix with fade_in).
INSTANTIATE_TEST_SUITE_P(Effects, BriefingTextEffectTest,
                         ::testing::Values(TextEffectCase{"None", TC_TEXT_STATIC, 0},
                                           TextEffectCase{"flash", TC_TEXT_FLASH, 0},
                                           TextEffectCase{"scroll_l2r", TC_TEXT_SCROLL, TC_TEXTF_L2R},
                                           TextEffectCase{"scroll_r2l", TC_TEXT_SCROLL, TC_TEXTF_R2L},
                                           TextEffectCase{"scroll_t2b", TC_TEXT_SCROLL, TC_TEXTF_T2B},
                                           TextEffectCase{"scroll_b2t", TC_TEXT_SCROLL, TC_TEXTF_B2T},
                                           TextEffectCase{"fade_in", TC_TEXT_FADE, TC_TEXTF_IN},
                                           TextEffectCase{"fade_out", TC_TEXT_FADE, TC_TEXTF_OUT},
                                           TextEffectCase{"fade_in_and_out", TC_TEXT_FADE, TC_TEXTF_PINGPONG}),
                         [](const ::testing::TestParamInfo<TextEffectCase> &info) { return std::string(info.param.name); });

TEST_F(BriefingParseTest, BitmapWithEditorSettings) {
  ASSERT_EQ(ParseInScreen(parser, dir,
                          "$bitmap effect Blur_out starttime 1.250  speed 2.500 position 10,20 isset 3  "
                          "desc \"pic\" show \"image.ogf\"\n"),
            PBERR_NOERR);

  EXPECT_EQ(r.events, (Events{"screen", "bitmap", "endscreen"}));
  ASSERT_EQ(r.bitmaps.size(), 1u);
  const Bitmap &b = r.bitmaps[0];
  EXPECT_STREQ(b.desc.filename, "image.ogf");
  EXPECT_EQ(b.description, "pic");
  EXPECT_EQ(b.desc.type, TC_BMP_BLUR);
  EXPECT_EQ(b.desc.flags, TC_BMPF_OUT);
  ASSERT_TRUE(b.desc.caps & TCBD_XY);
  EXPECT_EQ(b.desc.x, 10);
  EXPECT_EQ(b.desc.y, 20);
  ASSERT_TRUE(b.desc.caps & TCBD_SPEED);
  EXPECT_FLOAT_EQ(b.desc.speed, 2.5f);
  ASSERT_TRUE(b.desc.caps & TCBD_WAITTIME);
  EXPECT_FLOAT_EQ(b.desc.waittime, 1.25f);
  EXPECT_EQ(b.desc.mission_mask_set, 1u << 3);
  EXPECT_EQ(b.desc.mission_mask_unset, 0u);
}

TEST_F(BriefingParseTest, BitmapEffectNamesSelectTypeAndDirection) {
  ASSERT_EQ(
      ParseInScreen(parser, dir,
                    "$bitmap effect None starttime 0.000  speed 1.000 position 0,0   desc \"\" show \"a\"\n"
                    "$bitmap effect Scan_in starttime 0.000  speed 1.000 position 0,0   desc \"\" show \"b\"\n"
                    "$bitmap effect Invert_out starttime 0.000  speed 1.000 position 0,0   desc \"\" show \"c\"\n"
                    "$bitmap effect Stretch_in starttime 0.000  speed 1.000 position 0,0   desc \"\" show \"d\"\n"),
      PBERR_NOERR);

  ASSERT_EQ(r.bitmaps.size(), 4u);
  EXPECT_EQ(r.bitmaps[0].desc.type, TC_BMP_STATIC);
  EXPECT_EQ(r.bitmaps[1].desc.type, TC_BMP_SCANLINE);
  EXPECT_EQ(r.bitmaps[1].desc.flags, TC_BMPF_IN);
  EXPECT_EQ(r.bitmaps[2].desc.type, TC_BMP_INVERT);
  EXPECT_EQ(r.bitmaps[2].desc.flags, TC_BMPF_OUT);
  EXPECT_EQ(r.bitmaps[3].desc.type, TC_BMP_STRETCH);
  EXPECT_EQ(r.bitmaps[3].desc.flags, TC_BMPF_IN);
}

TEST_F(BriefingParseTest, BitmapNoEarlyRenderFlag) {
  ASSERT_EQ(ParseInScreen(parser, dir,
                          "$bitmap effect None starttime 0.000 no_early_rend speed 1.000 position 0,0   "
                          "desc \"\" show \"a\"\n"),
            PBERR_NOERR);

  ASSERT_EQ(r.bitmaps.size(), 1u);
  EXPECT_TRUE(r.bitmaps[0].desc.flags & TC_NOEARLYRENDER);
}

TEST_F(BriefingParseTest, MovieWithEditorSettings) {
  ASSERT_EQ(ParseInScreen(parser, dir,
                          "$movie position 5,6 fps 15.000 looping starttime 3.000 isnset 7  "
                          "desc \"vid\" show \"intro.mve\"\n"),
            PBERR_NOERR);

  ASSERT_EQ(r.movies.size(), 1u);
  const Movie &m = r.movies[0];
  EXPECT_STREQ(m.desc.filename, "intro.mve");
  EXPECT_EQ(m.description, "vid");
  ASSERT_TRUE(m.desc.caps & TCMD_XY);
  EXPECT_EQ(m.desc.x, 5);
  EXPECT_EQ(m.desc.y, 6);
  ASSERT_TRUE(m.desc.caps & TCMD_FPS);
  EXPECT_FLOAT_EQ(m.desc.fps, 15.0f);
  ASSERT_TRUE(m.desc.caps & TCMD_LOOPING);
  EXPECT_TRUE(m.desc.looping);
  ASSERT_TRUE(m.desc.caps & TCMD_WAITTIME);
  EXPECT_FLOAT_EQ(m.desc.waittime, 3.0f);
  EXPECT_EQ(m.desc.mission_mask_set, 0u);
  EXPECT_EQ(m.desc.mission_mask_unset, 1u << 7);
}

TEST_F(BriefingParseTest, SoundWithEditorSettings) {
  ASSERT_EQ(ParseInScreen(parser, dir,
                          "$sound once starttime 1.500   desc \"beep\" play \"beep.wav\"\n"
                          "$sound  starttime 0.000   desc \"\" play \"loop.wav\"\n"),
            PBERR_NOERR);

  ASSERT_EQ(r.sounds.size(), 2u);
  EXPECT_STREQ(r.sounds[0].desc.filename, "beep.wav");
  EXPECT_EQ(r.sounds[0].description, "beep");
  ASSERT_TRUE(r.sounds[0].desc.caps & TCSD_ONCE);
  EXPECT_TRUE(r.sounds[0].desc.once);
  ASSERT_TRUE(r.sounds[0].desc.caps & TCSD_WAITTIME);
  EXPECT_FLOAT_EQ(r.sounds[0].desc.waittime, 1.5f);

  EXPECT_STREQ(r.sounds[1].desc.filename, "loop.wav");
  EXPECT_FALSE(r.sounds[1].desc.caps & TCSD_ONCE);
}

TEST_F(BriefingParseTest, PolyPositionOrientationAndRotation) {
  ASSERT_EQ(ParseInScreen(parser, dir, "$poly 1.5,2,-3 4,5,6 7,8,9 \"ship\"\n"), PBERR_NOERR);

  ASSERT_EQ(r.polys.size(), 1u);
  const TCPOLYDESC &p = r.polys[0];
  EXPECT_STREQ(p.polyname, "ship");
  EXPECT_FLOAT_EQ(p.pos_x, 1.5f);
  EXPECT_FLOAT_EQ(p.pos_y, 2.0f);
  EXPECT_FLOAT_EQ(p.pos_z, -3.0f);
  EXPECT_FLOAT_EQ(p.ori_x, 4.0f);
  EXPECT_FLOAT_EQ(p.ori_y, 5.0f);
  EXPECT_FLOAT_EQ(p.ori_z, 6.0f);
  EXPECT_FLOAT_EQ(p.rot_x, 7.0f);
  EXPECT_FLOAT_EQ(p.rot_y, 8.0f);
  EXPECT_FLOAT_EQ(p.rot_z, 9.0f);
}

TEST_F(BriefingParseTest, JumpButtonWithEditorSettings) {
  ASSERT_EQ(ParseInScreen(parser, dir,
                          "$button 100,200 type Jump 3   click HoldDown id 4 desc \"go\" parent_id 5 sibling_id 6 "
                          "isset 0  show \"btn.ogf\" \"btnf.ogf\" \n"),
            PBERR_NOERR);

  ASSERT_EQ(r.buttons.size(), 1u);
  const Button &b = r.buttons[0];
  EXPECT_EQ(b.id, 4);
  EXPECT_EQ(b.description, "go");
  EXPECT_EQ(b.desc.x, 100);
  EXPECT_EQ(b.desc.y, 200);
  EXPECT_EQ(b.desc.button_type, BUTT_JUMP);
  EXPECT_EQ(b.desc.jump_page, 3);
  EXPECT_EQ(b.desc.click_type, CLICKTYPE_DOWN);
  EXPECT_EQ(b.desc.parent_id, 5);
  EXPECT_EQ(b.desc.sibling_id, 6);
  EXPECT_STREQ(b.desc.filename, "btn.ogf");
  EXPECT_STREQ(b.desc.filename_focus, "btnf.ogf");
  EXPECT_EQ(b.desc.mission_mask_set, 1u);
  EXPECT_FALSE(b.desc.flasher);
  EXPECT_EQ(b.desc.osflags, 0);
}

TEST_F(BriefingParseTest, ButtonTypesAndClickTypes) {
  const char *rest = "id 0 desc \"\" parent_id -1 sibling_id -1   show \"n\" \"f\" \n";
  ASSERT_EQ(ParseInScreen(
                parser, dir,
                std::string("$button 0,0 type Up   click ClickDown ") + rest +
                    "$button 0,0 type Down   click ClickUp " + rest + "$button 0,0 type Next   click ClickUp " + rest +
                    "$button 0,0 type Prev   click ClickUp " + rest + "$button 0,0 type Quit   click ClickUp " + rest),
            PBERR_NOERR);

  ASSERT_EQ(r.buttons.size(), 5u);
  EXPECT_EQ(r.buttons[0].desc.button_type, BUTT_UPARROW);
  EXPECT_EQ(r.buttons[0].desc.click_type, CLICKTYPE_CLICKDOWN);
  EXPECT_EQ(r.buttons[1].desc.button_type, BUTT_DOWNARROW);
  EXPECT_EQ(r.buttons[1].desc.click_type, CLICKTYPE_CLICKUP);
  EXPECT_EQ(r.buttons[2].desc.button_type, BUTT_NEXTPAGE);
  EXPECT_EQ(r.buttons[3].desc.button_type, BUTT_PREVPAGE);
  EXPECT_EQ(r.buttons[4].desc.button_type, BUTT_QUIT);
  EXPECT_EQ(r.buttons[0].desc.parent_id, -1);
  EXPECT_EQ(r.buttons[0].desc.sibling_id, -1);
}

TEST_F(BriefingParseTest, FlashingButton) {
  ASSERT_EQ(ParseInScreen(parser, dir,
                          "$button 1,2 type Next flash 0.500 \"fl.ogf\" \"flf.ogf\"  click ClickUp id 0 desc \"\" "
                          "parent_id -1 sibling_id -1   show \"n.ogf\" \"nf.ogf\" \n"),
            PBERR_NOERR);

  ASSERT_EQ(r.buttons.size(), 1u);
  const TCBUTTONDESC &d = r.buttons[0].desc;
  EXPECT_TRUE(d.flasher);
  EXPECT_EQ(d.osflags, OBF_FLASH);
  EXPECT_FLOAT_EQ(d.flash_time, 0.5f);
  EXPECT_STREQ(d.flash_filename, "fl.ogf");
  EXPECT_STREQ(d.flash_filename_focus, "flf.ogf");
  EXPECT_STREQ(d.filename, "n.ogf");
  EXPECT_STREQ(d.filename_focus, "nf.ogf");
}

TEST_F(BriefingParseTest, GlowingButtonReadsGlowImagesAfterTheNormalOnes) {
  ASSERT_EQ(ParseInScreen(parser, dir,
                          "$button 1,2 type Next  glow click ClickUp id 0 desc \"\" parent_id -1 sibling_id -1   "
                          "show \"n.ogf\" \"nf.ogf\" \"g.ogf\" \"gf.ogf\"\n"),
            PBERR_NOERR);

  ASSERT_EQ(r.buttons.size(), 1u);
  const TCBUTTONDESC &d = r.buttons[0].desc;
  EXPECT_TRUE(d.flasher);
  EXPECT_EQ(d.osflags, OBF_GLOW);
  EXPECT_STREQ(d.filename, "n.ogf");
  EXPECT_STREQ(d.filename_focus, "nf.ogf");
  EXPECT_STREQ(d.flash_filename, "g.ogf");
  EXPECT_STREQ(d.flash_filename_focus, "gf.ogf");
}

TEST_F(BriefingParseTest, FlashingAndGlowingButtonUsesFlashImages) {
  ASSERT_EQ(ParseInScreen(parser, dir,
                          "$button 1,2 type Next flash 0.250 \"fl.ogf\" \"flf.ogf\" glow click ClickUp id 0 "
                          "desc \"\" parent_id -1 sibling_id -1   show \"n.ogf\" \"nf.ogf\" \n"
                          "$static 0.1\n"),
            PBERR_NOERR);

  ASSERT_EQ(r.buttons.size(), 1u);
  const TCBUTTONDESC &d = r.buttons[0].desc;
  EXPECT_EQ(d.osflags, OBF_FLASH | OBF_GLOW);
  EXPECT_STREQ(d.flash_filename, "fl.ogf");
  EXPECT_STREQ(d.flash_filename_focus, "flf.ogf");
  EXPECT_EQ(r.statics.size(), 1u);
}

TEST_F(BriefingParseTest, CrLfLineEndings) {
  ASSERT_EQ(Parse(parser, dir,
                  "$title \"T\"\r\n"
                  "$screen 0   desc \"s\" start\r\n"
                  "$text effect None font sm_brief  speed 1.000 box 0,10,0,10 color 0,0,0 desc \"\" id 0 "
                  "starttime 0.000   show\r\n"
                  "line one\r\n"
                  "line two\r\n"
                  "$endtext\r\n"
                  "$endscreen\r\n"),
            PBERR_NOERR);

  EXPECT_EQ(r.events, (Events{"title", "screen", "text", "endscreen"}));
  ASSERT_EQ(r.texts.size(), 1u);
  EXPECT_EQ(r.texts[0].text, "line one\nline two");
}

TEST_F(BriefingParseTest, UnknownKeywordStopsParsing) {
  Parse(parser, dir,
        "$title \"T\"\n"
        "$bogus 1\n"
        "$static 0.5\n");

  EXPECT_EQ(r.events, (Events{"title"}));
}

TEST_F(BriefingParseTest, KeywordMustBeAWholeWord) {
  Parse(parser, dir, "$static0.5\n");

  EXPECT_TRUE(r.statics.empty());
  EXPECT_TRUE(r.events.empty());
}

TEST_F(BriefingParseTest, StrayTextOutsideTextBlockStopsParsing) {
  Parse(parser, dir,
        "$title \"T\"\n"
        "this is not a keyword\n"
        "$static 0.5\n");

  EXPECT_EQ(r.events, (Events{"title"}));
}

TEST_F(BriefingParseTest, SecondTitleIsAnError) {
  Parse(parser, dir,
        "$title \"A\"\n"
        "$title \"B\"\n"
        "$static 0.5\n");

  EXPECT_TRUE(r.statics.empty());
}

TEST_F(BriefingParseTest, UnterminatedTextBlockIsNotDelivered) {
  Parse(parser, dir,
        "$screen 0   desc \"s\" start\n"
        "$text effect None font sm_brief  speed 1.000 box 0,10,0,10 color 0,0,0 desc \"\" id 0 starttime 0.000   "
        "show\n"
        "dangling\n");

  EXPECT_TRUE(r.texts.empty());
}

TEST_F(BriefingParseTest, EndTextWithoutTextStopsParsing) {
  Parse(parser, dir,
        "$endtext\n"
        "$static 0.5\n");

  EXPECT_TRUE(r.statics.empty());
}

TEST_F(BriefingParseTest, EndScreenWithoutScreenStopsParsing) {
  Parse(parser, dir,
        "$endscreen\n"
        "$static 0.5\n");

  EXPECT_TRUE(r.events.empty());
}

TEST_F(BriefingParseTest, MalformedNumberStopsParsing) {
  Parse(parser, dir,
        "$static 0.5x\n"
        "$glitch 0.5\n");

  EXPECT_TRUE(r.events.empty());
}

TEST_F(BriefingParseTest, MissingClosingQuoteStopsParsing) {
  Parse(parser, dir,
        "$title \"unterminated\n"
        "$static 0.5\n");

  EXPECT_TRUE(r.events.empty());
}

TEST_F(BriefingParseTest, OutOfRangeScreenNumberStopsParsing) {
  Parse(parser, dir,
        "$screen " + std::to_string(MAX_TELCOM_SCREENS) + "   desc \"s\" start\n" +
            "$endscreen\n"
            "$static 0.5\n");

  EXPECT_TRUE(r.statics.empty());
}

TEST_F(BriefingParseTest, VoiceIsAcceptedOncePerScreen) {
  ASSERT_EQ(ParseInScreen(parser, dir,
                          "$voice compressed stereo desc \"v\" play \"voice.osf\"\n"
                          "$static 0.5\n"),
            PBERR_NOERR);
  EXPECT_EQ(r.statics.size(), 1u);
}

TEST_F(BriefingParseTest, SecondVoiceInAScreenStopsParsing) {
  ParseInScreen(parser, dir,
                "$voice play \"a.osf\"\n"
                "$voice play \"b.osf\"\n"
                "$static 0.5\n");

  EXPECT_TRUE(r.statics.empty());
}

} // namespace
