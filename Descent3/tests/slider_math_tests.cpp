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

// The options dialogs turn a slider position into a setting with CALC_SLIDER_*_VALUE, store it, and the next time the
// dialog opens turn the setting back into a slider position with CALC_SLIDER_POS_*. Those two must be inverse at every
// slider step, otherwise the slider creeps down a notch each time the dialog is opened.

#include <gtest/gtest.h>

#include "newui_core.h"
#include "support/slider_settings.h"

namespace {

TEST(SliderMath, FloatValueAtEachStepMapsBackToTheSameStep) {
  // The gamma dialog's range (1.0 - 3.0 in 100 steps) plus others whose step values are not exactly representable.
  struct Case {
    float min, max;
    int16_t range;
  } cases[] = {{1.0f, 3.0f, 100}, {0.0f, 1.0f, 10}, {0.5f, 2.0f, 30}, {0.0f, 4.0f, 200}, {0.0f, 0.3f, 3},
               {-1.0f, 1.0f, 20}};

  for (const Case &c : cases) {
    tSliderSettings settings = FloatSettings(c.min, c.max);
    for (int16_t pos = 0; pos <= c.range; pos++) {
      float value = CALC_SLIDER_FLOAT_VALUE(pos, c.min, c.max, c.range);
      EXPECT_EQ(CALC_SLIDER_POS_FLOAT(value, &settings, c.range), pos)
          << "min=" << c.min << " max=" << c.max << " range=" << c.range << " value=" << value;
    }
  }
}

TEST(SliderMath, FloatValueSpansTheConfiguredRange) {
  EXPECT_NEAR(CALC_SLIDER_FLOAT_VALUE(0, 0.5f, 2.0f, 30), 0.5f, 1e-4f);
  EXPECT_NEAR(CALC_SLIDER_FLOAT_VALUE(15, 0.5f, 2.0f, 30), 1.25f, 1e-4f);
  EXPECT_NEAR(CALC_SLIDER_FLOAT_VALUE(30, 0.5f, 2.0f, 30), 2.0f, 1e-4f);
}

TEST(SliderMath, IntValueAtEachStepMapsBackToTheSameStep) {
  struct Case {
    int min, max;
    int16_t range;
  } cases[] = {{1, 20, 19}, {0, 100, 100}, {2, 12, 5}, {-10, 10, 20}};

  for (const Case &c : cases) {
    tSliderSettings settings = IntSettings(c.min, c.max);
    for (int16_t pos = 0; pos <= c.range; pos++) {
      int value = CALC_SLIDER_INT_VALUE(pos, c.min, c.max, c.range);
      EXPECT_EQ(CALC_SLIDER_POS_INT(value, &settings, c.range), pos)
          << "min=" << c.min << " max=" << c.max << " range=" << c.range << " value=" << value;
    }
  }
}

TEST(SliderMath, IntValueSpansTheConfiguredRange) {
  EXPECT_EQ(CALC_SLIDER_INT_VALUE(0, 1, 20, 19), 1);
  EXPECT_EQ(CALC_SLIDER_INT_VALUE(19, 1, 20, 19), 20);
  EXPECT_EQ(CALC_SLIDER_INT_VALUE(5, 0, 100, 10), 50);
}

} // namespace
