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

#include <gtest/gtest.h>

#include "support/ui_test_util.h"

namespace {

constexpr int kShiftTab = KEY_SHIFTED + KEY_TAB;

class UIWindowFocusTest : public UIWindowTest {
protected:
  PlainGadget g[6];
};

TEST_F(UIWindowFocusTest, FirstFrameFocusesFirstEnabledGadget) {
  CreateGadgets(wnd, g, 3);
  g[0].Disable();
  Idle();
  EXPECT_EQ(wnd.GetFocus(), &g[1]);
}

TEST_F(UIWindowFocusTest, TabCyclesForwardSkippingDisabledAndWraps) {
  CreateGadgets(wnd, g, 4);
  g[2].Disable();
  Idle();
  ASSERT_EQ(wnd.GetFocus(), &g[0]);

  Press(KEY_TAB);
  EXPECT_EQ(wnd.GetFocus(), &g[1]);
  Press(KEY_TAB);
  EXPECT_EQ(wnd.GetFocus(), &g[3]);
  Press(KEY_TAB);
  EXPECT_EQ(wnd.GetFocus(), &g[0]);
}

TEST_F(UIWindowFocusTest, ShiftTabCyclesBackwardSkippingDisabledAndWraps) {
  CreateGadgets(wnd, g, 4);
  g[2].Disable();
  Idle();
  ASSERT_EQ(wnd.GetFocus(), &g[0]);

  Press(kShiftTab);
  EXPECT_EQ(wnd.GetFocus(), &g[3]);
  Press(kShiftTab);
  EXPECT_EQ(wnd.GetFocus(), &g[1]);
  Press(kShiftTab);
  EXPECT_EQ(wnd.GetFocus(), &g[0]);
}

TEST_F(UIWindowFocusTest, TabSkipsSlaveGadgets) {
  CreateGadgets(wnd, g, 3);
  g[0].AttachSlaveGadget(&g[1]);
  Idle();
  ASSERT_EQ(wnd.GetFocus(), &g[0]);
  Press(KEY_TAB);
  EXPECT_EQ(wnd.GetFocus(), &g[2]);
  Press(kShiftTab);
  EXPECT_EQ(wnd.GetFocus(), &g[0]);
}

TEST_F(UIWindowFocusTest, GroupIsOneTabStopNavigatedWithArrows) {
  const int flags[] = {0, UIF_GROUP_START, 0, UIF_GROUP_END, 0};
  CreateGadgets(wnd, g, 5, flags);
  Idle();
  ASSERT_EQ(wnd.GetFocus(), &g[0]);

  Press(KEY_TAB);
  EXPECT_EQ(wnd.GetFocus(), &g[1]);

  Press(KEY_DOWN);
  EXPECT_EQ(wnd.GetFocus(), &g[2]);
  Press(KEY_RIGHT);
  EXPECT_EQ(wnd.GetFocus(), &g[3]);
  Press(KEY_DOWN); // end of group
  EXPECT_EQ(wnd.GetFocus(), &g[3]);

  Press(KEY_TAB);
  EXPECT_EQ(wnd.GetFocus(), &g[4]);

  Press(kShiftTab); // re-enters the group at its first gadget
  EXPECT_EQ(wnd.GetFocus(), &g[1]);
  Press(KEY_UP); // start of group
  EXPECT_EQ(wnd.GetFocus(), &g[1]);
}

TEST_F(UIWindowFocusTest, ShiftTabLeavesAGroupBackwards) {
  const int flags[] = {0, UIF_GROUP_START, 0, UIF_GROUP_END, 0};
  CreateGadgets(wnd, g, 5, flags);
  Idle();
  Press(KEY_TAB);
  ASSERT_EQ(wnd.GetFocus(), &g[1]);

  Press(kShiftTab);
  EXPECT_EQ(wnd.GetFocus(), &g[0]);
}

TEST_F(UIWindowFocusTest, ArrowKeysDoNotMoveFocusOutsideAGroup) {
  CreateGadgets(wnd, g, 3);
  Idle();
  ASSERT_EQ(wnd.GetFocus(), &g[0]);
  Press(KEY_DOWN);
  Press(KEY_RIGHT);
  EXPECT_EQ(wnd.GetFocus(), &g[0]);
}

TEST_F(UIWindowFocusTest, RemovingTheFocusedGadgetMovesFocusToFirstUsable) {
  CreateGadgets(wnd, g, 3);
  Idle();
  Press(KEY_TAB);
  ASSERT_EQ(wnd.GetFocus(), &g[1]);

  wnd.RemoveGadget(&g[1]);
  EXPECT_NE(wnd.GetFocus(), &g[1]);
  EXPECT_EQ(g[0].Next(), &g[2]);
  EXPECT_EQ(g[2].Prev(), &g[0]);

  Idle();
  EXPECT_EQ(wnd.GetFocus(), &g[0]);
  Press(KEY_TAB);
  EXPECT_EQ(wnd.GetFocus(), &g[2]);
}

TEST_F(UIWindowFocusTest, HotkeySelectsItsGadget) {
  CreateGadgets(wnd, g, 3);
  g[2].SetHotkey(KEY_F);
  Idle();
  EXPECT_EQ(Press(KEY_G), -1);
  EXPECT_EQ(Press(KEY_F), 102);
}

TEST_F(UIWindowFocusTest, DisabledGadgetIgnoresItsHotkey) {
  CreateGadgets(wnd, g, 2);
  g[1].SetHotkey(KEY_F);
  g[1].Disable();
  Idle();
  EXPECT_EQ(Press(KEY_F), -1);
}

TEST_F(UIWindowFocusTest, EscapeSelectsTheCancelGadget) {
  CreateGadgets(wnd, g, 1);
  g[1].Create(&wnd, UID_CANCEL, 10, 200, 100, 20, 0);
  Idle();
  EXPECT_EQ(Press(KEY_ESC), UID_CANCEL);
}

TEST_F(UIWindowFocusTest, AcceleratorKeyReturnsItsId) {
  CreateGadgets(wnd, g, 1);
  wnd.AddAcceleratorKey(KEY_X, 55);
  wnd.AddAcceleratorKey(KEY_Y, 66);
  Idle();
  EXPECT_EQ(Press(KEY_Y), 66);
  EXPECT_EQ(Press(KEY_X), 55);
  EXPECT_EQ(Press(KEY_Z), -1);

  wnd.ResetAcceleratorKey();
  EXPECT_EQ(Press(KEY_X), -1);
}

TEST_F(UIWindowFocusTest, HeldHotkeysAreIgnoredUntilResumed) {
  CreateGadgets(wnd, g, 2);
  g[1].SetHotkey(KEY_F);
  wnd.AddAcceleratorKey(KEY_X, 55);
  Idle();

  wnd.HoldHotkeys();
  EXPECT_EQ(Press(KEY_F), -1);
  EXPECT_EQ(Press(KEY_X), -1);

  wnd.ResumeHotkeys();
  EXPECT_EQ(Press(KEY_F), 101);
  EXPECT_EQ(Press(KEY_X), 55);
}

} // namespace
