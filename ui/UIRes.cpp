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

#include "UIlib.h"
#include "uisys.h"
#include "pserror.h"
#include "grtext.h"
#include <string.h>
#include "mem.h"

//@@void *UIItem::operator new(size_t mem, UIItem *item, tUIResClass class_type)
//@@{
//@@//	hopefully, this will initialize the proper object and return a UIItem equivalent.
//@@	if (!item)
//@@		return NULL;
//@@
//@@	if (class_type == uiTextItem)
//@@		return new(item) UITextItem;
//@@	else if (class_type == uiBitmapItem)
//@@		return new(item) UIBitmapItem;
//@@	else {
//@@		Int3();					//	Samir!
//@@		return NULL;
//@@	}
//@@}

//	UITextItem
//		used by user interface system, contains information about how to render
//		text.  allows for alpha, color and different fonts.

int UITextItem::m_Sat = 0;
int UITextItem::m_DefaultFont = 0;
char UITextItem::dummy_str[4] = "";

UITextItem::UITextItem(const char *text, ddgr_color color, ubyte alpha) {
  int slen = strlen(text);
  if (slen > 0) {
    m_Text = mem_strdup(text);
    if (!m_Text) {
      Error("Memory allocation failed in UITextItem.");
    }
  } else {
    m_Text = &UITextItem::dummy_str[0];
    dummy_str[0] = 0;
  }

  strcpy(m_Text, text);

  m_Color = color;
  m_Alpha = alpha;
  m_Font = m_DefaultFont;
}

UITextItem::UITextItem(int font, const char *text, ddgr_color color, ubyte alpha) {
  int slen = strlen(text);
  if (slen > 0) {
    m_Text = mem_strdup(text);
    if (!m_Text) {
      Error("Memory allocation failed in UITextItem.");
    }
  } else {
    m_Text = &UITextItem::dummy_str[0];
    dummy_str[0] = 0;
  }

  strcpy(m_Text, text);

  m_Color = color;
  m_Alpha = alpha;
  m_Font = font;
}

UITextItem::~UITextItem() {
  if (m_Text && m_Text != (&UITextItem::dummy_str[0])) {
    mem_free(m_Text);
  }
}

//	Inheritable functions
bool UITextItem::draw(int x, int y, tUIDrawClass draw_class) {
  int i;

  ui_DrawSetFont(m_Font);
  ui_SetCharAlpha(m_Alpha);

  if (draw_class == uiDrawAlphaSaturate) {
    ui_DrawSetTextType(UI_TEXTTYPE_SATURATE);
    ui_DrawString(m_Color, x, y, m_Text);
    for (i = 0; i < UITextItem::m_Sat; i++)
      ui_DrawString(m_Color, x, y, m_Text);
  } else if (draw_class == uiDrawFaded) {
    ui_DrawSetTextType(0);
    ui_SetCharAlpha(m_Alpha / 2);
    ui_DrawString(m_Color, x, y, m_Text);
    ui_DrawSetTextType(UI_TEXTTYPE_SATURATE);
    for (i = 0; i < UITextItem::m_Sat; i++)
      ui_DrawString(m_Color, x, y, m_Text);
  } else {
    ui_DrawSetTextType(0);
    ui_DrawString(m_Color, x, y, m_Text);
    ui_DrawSetTextType(UI_TEXTTYPE_SATURATE);
    for (i = 0; i < UITextItem::m_Sat; i++)
      ui_DrawString(m_Color, x, y, m_Text);
  }

  return true;
}

int UITextItem::width() {
  ui_DrawSetFont(m_Font);
  return (m_Text ? ui_GetTextWidth(m_Text) : 0);
}

int UITextItem::height() {
  ui_DrawSetFont(m_Font);
  return (m_Text ? ui_GetTextHeight(m_Text) : 0);
}

//	operators
UIItem *UITextItem::CopyUIItem() {
  UITextItem *newitem = ::new UITextItem();
  if (newitem) {
    *newitem = *this;
  }
  return newitem;
}

const UITextItem &UITextItem::operator=(const UITextItem &item) {
  if (m_Text && m_Text != (&UITextItem::dummy_str[0])) {
    mem_free(m_Text);
  }

  if (strlen(item.m_Text) > 0) {
    m_Text = mem_strdup(item.m_Text);
    if (!m_Text)
      Error("Memory allocation failed in UITextItem.");
  } else {
    m_Text = &UITextItem::dummy_str[0];
    dummy_str[0] = 0;
  }

  strcpy(m_Text, item.m_Text);
  m_Color = item.m_Color;
  m_Alpha = item.m_Alpha;
  m_Font = item.m_Font;

  return *this;
}

//	UISnazzyTextItem
//		rich text item.

#define UISTIF_INCREASING 0x80000000

UISnazzyTextItem::UISnazzyTextItem(unsigned flags, const char *text, ddgr_color color, ubyte alpha)
    : UITextItem(text, color, alpha) {
  m_flags = 0; // must do, so that set_flags works.
  set_flags(flags);
}

UISnazzyTextItem::UISnazzyTextItem(unsigned flags, int font, const char *text, ddgr_color color, ubyte alpha)
    : UITextItem(font, text, color, alpha) {
  m_flags = 0; // must do, so that set_flags works.
  set_flags(flags);
}

void UISnazzyTextItem::set_flags(unsigned flags) {
  // reset old stuff
  if (m_flags & UISNAZZYTEXTF_BLINKING) {
    m_data.f = 0;
    set_color((ddgr_color)m_internaldata.i);
  }

  // set new stuff
  m_flags = flags & (~UISNAZZYTEXTF_RESERVED);

  if (m_flags & UISNAZZYTEXTF_BLINKING) {
    m_data.f = 1.0f;
    m_internaldata.i = (int)get_color();
  } else {
    m_data.i = 0;
    m_internaldata.i = 0;
  }
}

bool UISnazzyTextItem::draw(int x, int y, tUIDrawClass draw_class) {
  if (m_flags & UISNAZZYTEXTF_BLINKING) {
    int r, r2, g, g2, b, b2;
    if (m_flags & UISTIF_INCREASING)
      m_data.f += ((UIFrameTime) * 1.5f);
    else
      m_data.f -= ((UIFrameTime) * 1.5f);

    if (m_data.f < 0.0f) {
      m_data.f = 0.0;
      m_flags |= UISTIF_INCREASING;
    } else if (m_data.f > 1.0f) {
      m_data.f = 1.0f;
      m_flags &= (~UISTIF_INCREASING);
    }
    r = GR_COLOR_RED(m_internaldata.i);
    g = GR_COLOR_GREEN(m_internaldata.i);
    b = GR_COLOR_BLUE(m_internaldata.i);
    r2 = r + 96;
    if (r2 > 255)
      r2 = 255;
    g2 = g + 96;
    if (g2 > 255)
      g2 = 255;
    b2 = b + 96;
    if (b2 > 255)
      b2 = 255;
    r2 = r2 - r;
    g2 = g2 - g;
    b2 = b2 - b;

    set_color(GR_RGB(r + (int)(r2 * m_data.f), g + (int)(g2 * m_data.f), b + (int)(b2 * m_data.f)));
    //	set_alpha((ubyte)(m_data.f*m_internaldata.i));
    //	mprintf((0, "%x\n", (unsigned)get_color()));
  }

  return UITextItem::draw(x, y, draw_class);
}

UIItem *UISnazzyTextItem::CopyUIItem() {
  UISnazzyTextItem *newitem = ::new UISnazzyTextItem();
  if (newitem) {
    *newitem = *this;
  }
  return newitem;
}

const UISnazzyTextItem &UISnazzyTextItem::operator=(const UISnazzyTextItem &item) {
  // reproduce UITextItem's = operator
  if (m_Text && m_Text != (&UITextItem::dummy_str[0])) {
    mem_free(m_Text);
  }

  if (strlen(item.m_Text) > 0) {
    m_Text = mem_strdup(item.m_Text);
    if (!m_Text)
      Error("Memory allocation failed in UITextItem.");
  } else {
    m_Text = &UITextItem::dummy_str[0];
    dummy_str[0] = 0;
  }

  strcpy(m_Text, item.m_Text);

  set_color(item.get_color());
  set_alpha(item.get_alpha());
  set_font(item.get_font());

  // add snazzy enhancements
  m_internaldata = item.m_internaldata;
  m_data = item.m_data;
  m_flags = item.m_flags;

  return *this;
}

//	UIBitmapItem
//		used by user interface system, contains information about how to render
//		text.  allows for alpha, color and different fonts.

//	Inheritable functions
bool UIBitmapItem::draw(int x, int y, tUIDrawClass draw_class) {
  ui_DrawBitmap(this, x, y, m_Alpha);
  return true;
}

int UIBitmapItem::width() { return (m_IsValid ? ui_GetBitmapWidth(this) : 0); }

int UIBitmapItem::height() { return (m_IsValid ? ui_GetBitmapHeight(this) : 0); }

//	operators
UIItem *UIBitmapItem::CopyUIItem() {
  UIBitmapItem *newitem = ::new UIBitmapItem();
  if (newitem) {
    *newitem = *this;
  }
  return newitem;
}

const UIBitmapItem &UIBitmapItem::operator=(const UIBitmapItem &item) {
  m_IsValid = item.m_IsValid;
  m_IsChunked = item.m_IsChunked;

  if (m_IsChunked)
    m_Bitmap.chunk = item.m_Bitmap.chunk;
  else
    m_Bitmap.handle = item.m_Bitmap.handle;

  m_Alpha = item.m_Alpha;

  return *this;
}

bool UIPrimativeItem::draw(int x, int y, int w, int h) {
  ui_DrawSetAlpha(alpha);
  ui_DrawRect(color, x, y, x + w, y + h);
  return true;
}

UIItem *UIPrimativeItem::CopyUIItem() {
  UIPrimativeItem *newitem = ::new UIPrimativeItem(color, alpha);
  return newitem;
}
