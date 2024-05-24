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

--- HISTORICAL COMMENTS FOLLOW ---

 * $Logfile: /DescentIII/main/TelcomEffectsRender.cpp $
 * $Revision: 15 $
 * $Date: 3/20/00 12:21p $
 * $Author: Matt $
 *
 * <insert description of file here>
 *
 * $Log: /DescentIII/main/TelcomEffectsRender.cpp $
 *
 * 15    3/20/00 12:21p Matt
 * Merge of Duane's post-1.3 changes.
 * Minor optimization.
 *
 * 14    7/28/99 3:41p Kevin
 * Macintosh!
 *
 * 13    5/17/99 1:46p Ardussi
 * changed to compile on Mac
 *
 * 12    5/03/99 1:16p Jeff
 * play sounds at full volume
 *
 * 11    4/20/99 12:46p Jeff
 * telcom main menu, mouse over button sets focus.  if you go into telcom
 * main menu, when you leave a system it will return you to main menu.
 *
 * 10    4/14/99 3:57a Jeff
 * fixed case mismatch in #includes
 *
 * $NoKeywords: $
 */
#include "TelComEffects.h"
#include "textaux.h"
#include "mem.h"
#include "hlsoundlib.h"
#include <string.h>

#include <stdlib.h>

int glitch_dx = 0, glitch_dy = 0;
#define DISABLED_TEXT_COLOR GR_RGB(180, 180, 180);
int GetTextEffectWithTabStopsOnScreen(int screen) {
  int count = 0;
  ASSERT(screen >= 0 && screen < MAX_TELCOM_SCREENS);
  int node = Screen_roots[screen];
  while (node != -1) {
    if (TCEffects[node].type == EFX_TEXT_STATIC || TCEffects[node].type == EFX_TEXT_FADE ||
        TCEffects[node].type == EFX_TEXT_TYPE) {
      if (TCEffects[node].tab_stop)
        count++;
    }
    node = TCEffects[node].next;
  }
  return count;
}
void RenderTextStatic(tceffect *tce, float frametime, int xoff, int yoff, bool ok_to_render) {
  if (!ok_to_render)
    return;
  ASSERT(tce->type == EFX_TEXT_STATIC);
  ASSERT(tce->text_buffer);
  // See if we have any events waiting
  tTCEvent evt;
  while (PopEvent(tce, &evt)) {
    switch (evt.id) {
    case TEVT_SCROLLDOWN:
      if (tce->textinfo.scroll_d) {
        // mprintf((0,"Down Baby Down!\n"));
        tce->textinfo.line_index++;
      }
      break;
    case TEVT_SCROLLUP:
      if (tce->textinfo.line_index > 0) {
        // mprintf((0,"Up Baby Up!\n"));
        tce->textinfo.line_index--;
      }
      break;
    case TEVT_GAINFOCUS: {
      // mprintf((0,"Text %d Gain Focus\n",tce-TCEffects));
    } break;
    case TEVT_LOSTFOCUS: {
      // mprintf((0,"Text %d Lost Focus\n",tce-TCEffects));
    } break;
    }
  }
  ddgr_color color_to_use = tce->color;
  int tab_count = GetTextEffectWithTabStopsOnScreen(tce->screen);
  if (tab_count > 1) {
    // we need to see what color to use for the text, cause there is more than
    // one text item with a tab stop.
    if (!tce->has_focus) {
      // use the disable text color
      color_to_use = DISABLED_TEXT_COLOR;
    }
  }
  ubyte old_alpha = grtext_GetAlpha();
  grtext_SetFont(Game_fonts[tce->textinfo.font_index]);
  grtext_SetColor(color_to_use);
  grtext_SetAlpha(tce->alpha);
  int curry = tce->pos_y + yoff;
  int height = grfont_GetHeight(Game_fonts[tce->textinfo.font_index]) + 1;
  char buff[256];
  const char *nextline;
  int count = 0; // get to the first line of text (scrolling)
  nextline = textaux_CopyTextLine(tce->text_buffer, buff);
  if ((tce->textinfo.line_index) && (nextline)) {
    count++;
    while (count <= tce->textinfo.line_index) {
      nextline = textaux_CopyTextLine(nextline, buff);
      count++;
    }
  }
  int lcount = 0;
  while ((nextline) && ((curry + height) < tce->pos_y + tce->h)) {
    grtext_Printf(tce->pos_x + glitch_dx + xoff, curry + glitch_dy, buff);
    lcount++;
    curry += height;
    nextline = textaux_CopyTextLine(nextline, buff);
  }
  grtext_Printf(tce->pos_x + glitch_dx + xoff, curry + glitch_dy, buff);
  lcount++;
  grtext_SetAlpha(old_alpha);

  if ((lcount * height) > tce->h) {
    // we can scroll down
    tce->textinfo.scroll_d = true;
  } else {
    // we can't scroll down
    tce->textinfo.scroll_d = false;
  }
  tce->age += frametime;
}
void RenderTextType(tceffect *tce, float frametime, int xoff, int yoff, bool ok_to_render) {
  if (!ok_to_render)
    return;
  ASSERT(tce->type == EFX_TEXT_TYPE);
  ASSERT(tce->text_buffer);
  // See if we have any events waiting
  tTCEvent evt;
  while (PopEvent(tce, &evt)) {
    switch (evt.id) {
    case TEVT_SCROLLDOWN:
      if (tce->textinfo.scroll_d) {
        // mprintf((0,"Down Baby Down!\n"));
        tce->textinfo.line_index++;
      }
      break;
    case TEVT_SCROLLUP:
      if (tce->textinfo.line_index > 0) {
        // mprintf((0,"Up Baby Up!\n"));
        tce->textinfo.line_index--;
      }
      break;
    case TEVT_GAINFOCUS: {
      // mprintf((0,"Text %d Gain Focus\n",tce-TCEffects));
    } break;
    case TEVT_LOSTFOCUS: {
      // mprintf((0,"Text %d Lost Focus\n",tce-TCEffects));
    } break;
    }
  }
  if ((!tce->text_buffer) || (tce->text_buffer[0] == '\0'))
    return;
  float flash_step;
  ddgr_color hi_color = tce->color; //@JEFF: GR_RGB(255,255,255);
  int strlength = strlen(tce->text_buffer);
  if (tce->textinfo.last_letter >= strlength - 1) {
    TelcomStopSound(TCSND_TYPING);
  }
  flash_step = frametime * (((float)strlength) / tce->speed);
  char *temp;
  if (tce->textinfo.last_letter < strlength - 1) {
    TelcomStartSound(TCSND_TYPING);
  }
  tce->textinfo.last_letter += flash_step;
  if (tce->textinfo.last_letter > strlength)
    tce->textinfo.last_letter = strlength;
  int ll = (int)tce->textinfo.last_letter;
  temp = (char *)mem_malloc(strlength + 10);
  if (!temp) {
    tce->age += frametime;
    return;
  }
  strcpy(temp, tce->text_buffer);
  temp[ll] = '\0';

  ubyte old_alpha = grtext_GetAlpha();
  grtext_SetFont(Game_fonts[tce->textinfo.font_index]);
  char buff[1024];
  const char *nextline;
  int height = grfont_GetHeight(Game_fonts[tce->textinfo.font_index]) + 1;
  int curry = tce->pos_y;
  ddgr_color color_to_use = hi_color;
  int tab_count = GetTextEffectWithTabStopsOnScreen(tce->screen);
  if (tab_count > 1) {
    // we need to see what color to use for the text, cause there is more than
    // one text item with a tab stop.
    if (!tce->has_focus) {
      // use the disable text color
      color_to_use = DISABLED_TEXT_COLOR;
    }
  }
  grtext_SetColor(color_to_use);
  grtext_SetAlpha(255.0);

  int count = 0; // get to the first line of text (scrolling)
  nextline = textaux_CopyTextLine(temp, buff);
  if ((tce->textinfo.line_index) && (nextline)) {
    count++;
    while (count <= tce->textinfo.line_index) {
      nextline = textaux_CopyTextLine(nextline, buff);
      count++;
    }
  }
  int lcount = 0;
  while ((nextline) && ((curry + height) <= tce->pos_y + tce->h)) {
    grtext_Printf(tce->pos_x + xoff + glitch_dx, curry + yoff + glitch_dy, buff);
    lcount++;
    curry += height;
    nextline = textaux_CopyTextLine(nextline, buff);
  }
  grtext_Printf(tce->pos_x + xoff + glitch_dx, curry + yoff + glitch_dy, buff);
  lcount++;
  if ((lcount * height) > tce->h) {
    // we can scroll down
    tce->textinfo.scroll_d = true;
  } else {
    // we can't scroll down
    tce->textinfo.scroll_d = false;
  }
  /*
  if( ll>2 ){
          grtext_SetColor(tce->color); //!<<_---------USE CORRECT COLOR HERE WHEN IMPLEMENTED
          if(ll<strlength-1){
                  temp[ll-2]='\0';
          }else{
                  //strcpy(temp,tce->text_buffer);
                  temp[11]='\0';
          }
          curry = tce->pos_y;
          count = 0;  //get to the first line of text (scrolling)
          nextline = textaux_CopyTextLine(temp,buff);
          if((tce->textinfo.line_index)&&(nextline)){
                  count++;
                  while(count<=tce->textinfo.line_index){
                          nextline = textaux_CopyTextLine(nextline,buff);
                          count++;
                  }
          }
          while((nextline)&&((curry+height)<=tce->pos_y+tce->h) ){
                  grtext_Printf(tce->pos_x+xoff+glitch_dx,curry+yoff+glitch_dy,buff);
                  curry += height;
                  nextline = textaux_CopyTextLine(nextline,buff);
          }
          grtext_Printf(tce->pos_x+xoff+glitch_dx,curry+yoff+glitch_dy,buff);
  }
  //draw scroll buttons now (if needed)
  //if m_iIndex!=0 then we can scroll up
  if(tce->textinfo.line_index){
          tce->textinfo.scroll_u = true;
  }else
          tce->textinfo.scroll_u = false;
  //if curry+height>m_iBottomY then we can scroll down
  if( (curry+height)>tce->pos_y+tce->h){
          tce->textinfo.scroll_d = true;
  }
  else
          tce->textinfo.scroll_d = false;
  */
  grtext_SetAlpha(old_alpha);
  mem_free(temp);
  tce->age += frametime;
}
void RenderTextFade(tceffect *tce, float frametime, int xoff, int yoff, bool ok_to_render) {
  if (!ok_to_render)
    return;
  ASSERT(tce->type == EFX_TEXT_FADE);
  ASSERT(tce->text_buffer);
  // See if we have any events waiting
  tTCEvent evt;
  while (PopEvent(tce, &evt)) {
    switch (evt.id) {
    case TEVT_SCROLLDOWN:
      if (tce->textinfo.scroll_d) {
        // mprintf((0,"Down Baby Down!\n"));
        tce->textinfo.line_index++;
      }
      break;
    case TEVT_SCROLLUP:
      if (tce->textinfo.line_index > 0) {
        // mprintf((0,"Up Baby Up!\n"));
        tce->textinfo.line_index--;
      }
      break;
    case TEVT_GAINFOCUS: {
      // mprintf((0,"Text %d Gain Focus\n",tce-TCEffects));
    } break;
    case TEVT_LOSTFOCUS: {
      // mprintf((0,"Text %d Lost Focus\n",tce-TCEffects));
    } break;
    }
  }

  if (tce->speed == 0)
    tce->speed = 0.0001f;
  float amount = tce->age * (255.0 / tce->speed);
  uint8_t alpha;
  if (tce->flags == TC_TEXTF_IN) {
    // fade in, but not fade out
    tce->alpha += amount;
    if (tce->alpha > 255.0f) {
      tce->alpha = 255.0f;
    }
    alpha = tce->alpha;
  } else if (tce->flags == TC_TEXTF_OUT) {
    // fade out, but not fade in
    tce->alpha -= amount;
    if (tce->alpha < 0.0f) {
      tce->alpha = 0.0f;
    }
    alpha = tce->alpha;
  } else {
    // fade in, then fade out (if alpha is negative, then we are fading out)
    if (tce->alpha < 0.0f) {
      tce->alpha += amount;
      alpha = (-tce->alpha);
      if (tce->alpha > 0.0f) {
        // we're done fading out
        tce->alpha = 1234.0f;
        alpha = 0;
      }
    } else if (tce->alpha != 1234.0f) {
      tce->alpha += amount;
      alpha = tce->alpha;
      if (tce->alpha > 255.0f) {
        // switch to fading out
        alpha = 255;
        tce->alpha = -255.0f;
      }
    } else
      alpha = 0;
  }
  ddgr_color color_to_use = tce->color;
  int tab_count = GetTextEffectWithTabStopsOnScreen(tce->screen);
  if (tab_count > 1) {
    // we need to see what color to use for the text, cause there is more than
    // one text item with a tab stop.
    if (!tce->has_focus) {
      // use the disable text color
      color_to_use = DISABLED_TEXT_COLOR;
    }
  }
  ubyte old_alpha = grtext_GetAlpha();
  grtext_SetFont(Game_fonts[tce->textinfo.font_index]);
  grtext_SetColor(color_to_use);
  grtext_SetAlpha(alpha);
  int curry = tce->pos_y;
  int height = grfont_GetHeight(Game_fonts[tce->textinfo.font_index]) + 1;
  char buff[256];
  const char *nextline;
  int count = 0; // get to the first line of text (scrolling)
  nextline = textaux_CopyTextLine(tce->text_buffer, buff);
  if ((tce->textinfo.line_index) && (nextline)) {
    count++;
    while (count <= tce->textinfo.line_index) {
      nextline = textaux_CopyTextLine(nextline, buff);
      count++;
    }
  }
  int lcount = 0;
  while ((nextline) && ((curry + height) < tce->pos_y + tce->h)) {
    grtext_Printf(tce->pos_x + glitch_dx + xoff, curry + glitch_dy + yoff, buff);
    lcount++;
    curry += height;
    nextline = textaux_CopyTextLine(nextline, buff);
  }
  grtext_Printf(tce->pos_x + glitch_dx + xoff, curry + glitch_dy + yoff, buff);
  lcount++;
  grtext_SetAlpha(old_alpha);
  if ((lcount * height) > tce->h) {
    // we can scroll down
    tce->textinfo.scroll_d = true;
  } else {
    // we can't scroll down
    tce->textinfo.scroll_d = false;
  }
  tce->age += frametime;
}
void RenderBmpStatic(tceffect *tce, float frametime, int xoff, int yoff, bool ok_to_render) {
  if (!ok_to_render)
    return;
  ASSERT(tce->type == EFX_BMP_STATIC);
  BltBmpToScreen(tce->pos_x + glitch_dx + xoff, tce->pos_y + glitch_dy + yoff, &tce->bmpinfo.chunk_bmp);
  tce->age += frametime;
}
void BlurBitmapArea(ushort *srcbm, ushort *dstbm, short width, short height, short startx, short starty, short bmw) {
  int pixel_count, blue_total, red_total, green_total;
  pixel_count = width * height;
  blue_total = red_total = green_total = 0;
  short x, y;
  short pos;
  if (!pixel_count)
    return;

  for (y = starty; y < height + starty; y++)
    for (x = startx; x < width + startx; x++) {
      pos = y * bmw + x;
      if (srcbm[pos] & OPAQUE_FLAG) {
        blue_total += GR_COLOR_BLUE(GR_16_TO_COLOR(srcbm[pos]));
        red_total += GR_COLOR_RED(GR_16_TO_COLOR(srcbm[pos]));
        green_total += GR_COLOR_GREEN(GR_16_TO_COLOR(srcbm[pos]));
      }
    }
  ushort col;
  if (red_total + green_total + blue_total)
    col = OPAQUE_FLAG | GR_RGB16(red_total / pixel_count, green_total / pixel_count, blue_total / pixel_count);
  else
    col = 0;
  for (x = startx; x < width + startx; x++)
    for (y = starty; y < height + starty; y++) {
      dstbm[y * bmw + x] = col;
    }
}
void RenderBmpBlur(tceffect *tce, float frametime, int xoff, int yoff, bool ok_to_render) {
  ASSERT(tce->type == EFX_BMP_BLUR);
  if (!ok_to_render) {
    if (tce->flags == TC_BMPF_OUT && (!(tce->flags & TC_NOEARLYRENDER))) {
      UploadChunk(&tce->bmpinfo.chunk_bmp);
      BltBmpToScreen(tce->pos_x + glitch_dx + xoff, tce->pos_y + glitch_dy + yoff, &tce->bmpinfo.chunk_bmp);
    }
    return;
  }

  if ((tce->flags == TC_BMPF_OUT) && (tce->bmpinfo.stage <= 0))
    return;
  int sbm_handle = tce->bmpinfo.bm_handle;
  int dbm_handle = tce->bmpinfo.temp_bmhandle;
  bm_ClearBitmap(dbm_handle);
  int x = 0;
  int y = 0;
  bool done = false;
  if ((tce->flags == TC_BMPF_OUT) && (tce->bmpinfo.stage < 0))
    done = true;
  if ((tce->flags == TC_BMPF_IN) && (tce->bmpinfo.stage > BLUR_STAGES))
    done = true;

  ushort *src, *dest;
  int rowsize, w, h;

  if (!done) {
    src = bm_data(sbm_handle, 0);
    dest = bm_data(dbm_handle, 0);
    rowsize = bm_w(sbm_handle, 0);
    w = bm_w(sbm_handle, 0);
    h = bm_h(sbm_handle, 0);
    int current_stage;
    int block_size;
    current_stage = tce->bmpinfo.stage;
    block_size = 1 << (BLUR_STAGES - current_stage);
    int x = 0, y = 0, bs_w, bs_h;

    for (x = 0; x < w; x += block_size)
      for (y = 0; y < h; y += block_size) {
        if (x + block_size > w)
          bs_w = w - x;
        else
          bs_w = block_size;
        if (y + block_size > h)
          bs_h = h - y;
        else
          bs_h = block_size;
        BlurBitmapArea(src, dest, bs_w, bs_h, x, y, w);
      }
    int dim = bm_w(tce->bmpinfo.chunk_bmp.bm_array[0], 0);
    int how_many_across = tce->bmpinfo.chunk_bmp.w;
    int bh = tce->bmpinfo.chunk_bmp.ph;
    int bw = tce->bmpinfo.chunk_bmp.pw;
    ushort *src_data = bm_data(dbm_handle, 0);
    for (y = 0; y < bh; y++) {
      for (x = 0; x < bw; x++) {
        int piece_y = y / dim;
        int piece_x = x / dim;
        int sub_x = x % dim;
        int sub_y = y % dim;
        int bm = tce->bmpinfo.chunk_bmp.bm_array[piece_y * how_many_across + piece_x];
        ushort *dest_data = bm_data(bm, 0);
        ushort pix = src_data[y * bw + x];
        dest_data[sub_y * dim + sub_x] = pix;
      }
    }
    UploadChunk(&tce->bmpinfo.chunk_bmp);
    BltBmpToScreen(tce->pos_x + glitch_dx + xoff, tce->pos_y + glitch_dy + yoff, &tce->bmpinfo.chunk_bmp);

    float amt = tce->speed * frametime;
    if (tce->flags == TC_BMPF_IN)
      tce->bmpinfo.stage += amt;
    else
      tce->bmpinfo.stage -= amt;
  } else {
    BltBmpToScreen(tce->pos_x + glitch_dx + xoff, tce->pos_y + glitch_dy + yoff, &tce->bmpinfo.chunk_bmp);
  }
  tce->age += frametime;
}
#define N_FADE_STAGES 4
void RenderBmpScanline(tceffect *tce, float frametime, int xoff, int yoff, bool ok_to_render) {
  ASSERT(tce->type == EFX_BMP_SCANLINE);
  if (!ok_to_render) {
    if (tce->flags == TC_BMPF_OUT && (!(tce->flags & TC_NOEARLYRENDER))) {
      UploadChunk(&tce->bmpinfo.chunk_bmp);
      BltBmpToScreen(tce->pos_x + glitch_dx + xoff, tce->pos_y + glitch_dy + yoff, &tce->bmpinfo.chunk_bmp);
    }
    return;
  }

  bool changed = true;
  bool done;
  int x, y;
  int sbm_handle = tce->bmpinfo.bm_handle;
  int dbm_handle = tce->bmpinfo.temp_bmhandle;
  ushort *src, *dest;
  int rowsize, w, h, xx, yy, stage;
  float k;
  if (dbm_handle == BAD_BITMAP_HANDLE)
    goto done_scanline;
  bm_ClearBitmap(dbm_handle);

  x = 0;
  y = 0;
  done = false;

  k = tce->age / tce->speed;

  if (k > 1.2)
    goto done_scanline;
  if (k > 1.0) {
    k = 1.0;
    done = 1;
  }
  if (tce->flags == TC_BMPF_OUT) {
    k = 1.0 - k;
  }
  src = bm_data(sbm_handle, 0);
  dest = bm_data(dbm_handle, 0);
  rowsize = bm_w(sbm_handle, 0);
  w = bm_w(sbm_handle, 0);
  h = bm_h(sbm_handle, 0);
  for (yy = stage = 0; yy < h; yy++) {
    int span_width;
    span_width = w * ((k - ((float)stage / N_FADE_STAGES)) * N_FADE_STAGES);
    if (span_width < 0)
      span_width = 0;
    if (span_width > w)
      span_width = w;
    for (xx = 0; xx < span_width; xx++)
      if (dest && src)
        *dest++ = *src++;

    dest += (rowsize - xx);
    src += (w - span_width);
    switch (stage) {
    case 0:
      stage = 2;
      break;
    case 1:
      stage = 3;
      break;
    case 2:
      stage = 1;
      break;
    case 3:
      stage = 0;
      break;
    }
  }
  // we now chunk the bitmap (this is slow, the above should be done right to the chunk)
  if (changed) {
    int dim = bm_w(tce->bmpinfo.chunk_bmp.bm_array[0], 0);
    int how_many_across = tce->bmpinfo.chunk_bmp.w;
    int bh = tce->bmpinfo.chunk_bmp.ph;
    int bw = tce->bmpinfo.chunk_bmp.pw;
    ushort *src_data = bm_data(dbm_handle, 0);
    int x, y;
    for (y = 0; y < bh; y++) {
      for (x = 0; x < bw; x++) {
        int piece_y = y / dim;
        int piece_x = x / dim;
        int sub_x = x % dim;
        int sub_y = y % dim;
        int bm = tce->bmpinfo.chunk_bmp.bm_array[piece_y * how_many_across + piece_x];
        ushort *dest_data = bm_data(bm, 0);
        ushort pix = src_data[y * bw + x];
        dest_data[sub_y * dim + sub_x] = pix;
      }
    }
    UploadChunk(&tce->bmpinfo.chunk_bmp);
  }
done_scanline:
  BltBmpToScreen(tce->pos_x + glitch_dx + xoff, tce->pos_y + glitch_dy + yoff, &tce->bmpinfo.chunk_bmp);
  tce->age += frametime;
}
void RenderBmpInvert(tceffect *tce, float frametime, int xoff, int yoff, bool ok_to_render) {
  ASSERT(tce->type == EFX_BMP_INVERT);
  int srcbmph = tce->bmpinfo.bm_handle;
  bool changed = true;
  if (!ok_to_render && tce->flags & TC_NOEARLYRENDER)
    return;
  float k;

  if (ok_to_render) {
    k = tce->age / tce->speed;
  } else {
    k = 0;
  }
  int inv;
  if (k > 1.0) {
    k = 1.0;
    changed = false;
  } else if (k < 0) {
    mprintf((0, "k<0 on inverse bitmap...bashing (%.2f/%.2f)\n", tce->age, tce->speed));
    if (tce->age < 0)
      Int3();
    if (tce->speed < 0)
      tce->speed = 0.1f;
    k = 0;
    changed = false;
  }
  if (tce->flags == TC_BMPF_OUT)
    k = 1 - k;
  inv = 255 - (255.0f * k);
  ubyte r, g, b;
  int bh = bm_h(srcbmph, 0);
  int bw = bm_w(srcbmph, 0);
  int how_many = tce->bmpinfo.chunk_bmp.w * tce->bmpinfo.chunk_bmp.h;
  int how_many_across = tce->bmpinfo.chunk_bmp.w;
  int dim = bm_h(tce->bmpinfo.chunk_bmp.bm_array[0], 0);
  int shift; // speed up (since dim is always a power of 2)
  switch (dim) {
  case 32:
    shift = 5;
    break;
  case 64:
    shift = 6;
    break;
  case 128:
    shift = 7;
    break;
  }
  ushort *src_data = bm_data(srcbmph, 0);
  ushort *sdata;
  ushort *ddata;
  ushort pix;
  int maxx, maxy;
  int windex, hindex;
  int s_y, s_x, d_y, d_x;
  // is there any need to do this if !changed!?!?
  for (hindex = 0; hindex < tce->bmpinfo.chunk_bmp.h; hindex++) {
    for (windex = 0; windex < tce->bmpinfo.chunk_bmp.w; windex++) {
      // loop through the chunks
      // find end x and y
      if (windex < tce->bmpinfo.chunk_bmp.w - 1)
        maxx = dim;
      else
        maxx = bw - (windex << shift);
      if (hindex < tce->bmpinfo.chunk_bmp.h - 1)
        maxy = dim;
      else
        maxy = bh - (hindex << shift);
      // find the starting source x and y
      s_x = (windex << shift);
      s_y = (hindex << shift);
      // get the pointers pointing to the right spot
      ddata = bm_data(tce->bmpinfo.chunk_bmp.bm_array[hindex * how_many_across + windex], 0);
      sdata = &src_data[s_y * bw + s_x];
      // copy the data
      for (d_y = 0; d_y < maxy; d_y++) {
        for (d_x = 0; d_x < maxx; d_x++) {
          pix = sdata[d_x];
          if (pix) {
            r = GR_COLOR_RED(GR_16_TO_COLOR(pix));
            g = GR_COLOR_GREEN(GR_16_TO_COLOR(pix));
            b = GR_COLOR_BLUE(GR_16_TO_COLOR(pix));
            ddata[d_x] = OPAQUE_FLAG | GR_RGB16(abs(r - inv), abs(g - inv), abs(b - inv));
          } else {
            ddata[d_x] = 0;
          }
        } // end for d_x
        sdata += bw;
        ddata += dim;
      } // end for d_y
    }   // end for windex
  }     // end for hindex

  if (changed)
    UploadChunk(&tce->bmpinfo.chunk_bmp);
  BltBmpToScreen(tce->pos_x + glitch_dx + xoff, tce->pos_y + glitch_dy + yoff, &tce->bmpinfo.chunk_bmp);
  if (ok_to_render)
    tce->age += frametime;
}
void RenderBmpStretch(tceffect *tce, float frametime, int xoff, int yoff, bool ok_to_render) {
  ASSERT(tce->type == EFX_BMP_STRETCH);
  if (!ok_to_render) {
    return;
  }

  float k = (tce->age) / tce->speed;
  if (k > 1.0) {
    k = 1.0;
  } else if (k < 0) {
    k = 0;
  }
  if (tce->flags == TC_BMPF_OUT)
    k = 1 - k;
  int w, h;
  // width
  // h = 64;
  // w = k*64.0;
  // height
  // w = 64;
  // h = k*64.0;
  // width & height
  h = w = (k * 64.0);
  int src_w = bm_w(tce->bmpinfo.bm_handle, 0);
  int src_h = bm_h(tce->bmpinfo.bm_handle, 0);
  int realx, realy;
  int w_count = (src_w + 63) / 64;
  int h_count = (src_h + 63) / 64;
  float tw = ((float)k * src_w) / ((float)w_count);
  float th = ((float)k * src_h) / ((float)h_count);
  int startx = tce->pos_x + (src_w / 2) - (((float)w_count) * tw / 2.0f);
  int starty = tce->pos_y + (src_h / 2) - (((float)h_count) * th / 2.0f);
  rend_SetAlphaType(AT_CONSTANT_TEXTURE);
  rend_SetAlphaValue(255.0);
  rend_SetLighting(LS_NONE);
  rend_SetColorModel(CM_MONO);
  rend_SetOverlayType(OT_NONE);
  rend_SetWrapType(WT_CLAMP);
  rend_SetFiltering(0);
  int dx, dy;
  int index;
  index = 0;
  for (dy = 0, realy = starty; dy < h_count; dy++, realy += h)
    for (dx = 0, realx = startx; dx < w_count; dx++, realx += w) {
      rend_DrawScaledBitmap(realx + xoff + glitch_dx, realy + yoff + glitch_dy, realx + w + xoff + glitch_dx,
                            realy + h + yoff + glitch_dy, tce->bmpinfo.bitmaps[index], 0, 0, 1, 1);
      index++;
    }
  rend_SetFiltering(1);
  tce->age += frametime;
}
void RenderBackground(tceffect *tce, float frametime, int xoff, int yoff, bool ok_to_render) {
  if (!ok_to_render)
    return;
  ASSERT(tce->type == EFX_BACKGROUND);
  int lx, rx, ty, by;
  lx = tce->pos_x + xoff;
  rx = tce->pos_x + xoff + tce->w;
  ty = tce->pos_y + yoff;
  by = tce->pos_y + yoff + tce->h;
  rend_FillRect(tce->color, lx, ty, rx, by);
  tce->age += frametime;
}
void RenderMovie(tceffect *tce, float frametime, int xoff, int yoff, bool ok_to_render) {
  if (!ok_to_render)
    return;
  ASSERT(tce->type == EFX_MOVIE);

  if (!tce->movieinfo.handle)
    return;
  bool done = false;
  FrameMovie(tce->movieinfo.handle, tce->pos_x + glitch_dx, tce->pos_y + glitch_dy, false);
  float end_time = tce->age + frametime;
  while ((end_time > tce->age) && (!done)) {
    if (!FrameMovie(tce->movieinfo.handle, tce->pos_x + glitch_dx + xoff, tce->pos_y + glitch_dy + yoff)) {
      /*
      if(IsLooping())	//looping?
      {
              EndMovie(m_MovieHandle);
              m_MovieHandle = StartMovie(m_szMovieName);
      }
      else
      {
              EndMovie(m_MovieHandle);
              m_MovieHandle = NULL;
              done = true;
      }*/
      EndMovie(tce->movieinfo.handle);
      tce->movieinfo.handle = NULL;
      done = true;
    } else
      tce->age += 1.0f / tce->movieinfo.fps;
  }
}
void RenderPolyModel(tceffect *tce, float frametime, int xoff, int yoff, bool ok_to_render) {
  /*
  if(!ok_to_render)
          return;
  ASSERT(tce->type==EFX_POLYMODEL);
  if(tce->polyinfo.handle==-1)
          return false;
  float k = tce->age;
  //setup how much we're gonna rotate
  float xrot = k*tce->polyinfo.m_Rot.x + tce->polyinfo.m_Ori.x;
  float yrot = k*tce->polyinfo.m_Rot.y + tce->polyinfo.m_Ori.y;
  float zrot = k*tce->polyinfo.m_Rot.z + tce->polyinfo.m_Ori.z;
  //fade in speed 2.0f
  k*=(1.0f/2.0f);
  if (k > 1.3) {
          k = 1.3f;
  }
  else
  if (k < 0){
          k = 0.0f;
  }
  poly_model *pm=GetPolymodelPointer(tce->polyinfo.handle);
  vector view_vec;
  matrix view_orient;
  float light_scalar;
  //set all animation to default position
  float norm_angles[MAX_SUBOBJECTS];
  for (int i=0;i<MAX_SUBOBJECTS;i++){
          norm_angles[i]=0;
  }
  //viewer at origin
  vm_MakeIdentity(&view_orient);
  vector pos;
  pos.x = - (tce->polyinfo.m_Pos.x + (((float)glitch_dx)/tce->polyinfo.m_Pos.z));
  pos.y = - (tce->polyinfo.m_Pos.y + (((float)glitch_dy)/tce->polyinfo.m_Pos.z));
  pos.z = - tce->polyinfo.m_Pos.z;

  g3_StartFrame (&pos,&view_orient,D3_DEFAULT_ZOOM);
  //shed some light on the subject
  light_scalar = 0.86f;
  //we need to rotate our model about Y 180 degrees so it faces the camera
  vm_AnglesToMatrix(&view_orient,xrot,yrot,zrot);
  tce->polyinfo.m_mOrient += view_orient;
  vm_Orthogonalize(&tce->polyinfo.m_mOrient);

  //draw the polymodel with our little fade in effect
  polymodel_effect pe={0};
  pe.type=PEF_ALPHA;
  if(k>=0.3)
          pe.alpha=k-0.3f;
  else
          pe.alpha=0.005f;
  SetPolymodelEffect(&pe);
  vm_MakeZero (&view_vec);
  rend_SetZBufferState(1);
  DrawPolygonModel(&view_vec,&tce->polyinfo.m_mOrient,tce->polyinfo.handle,norm_angles,0,light_scalar,light_scalar,light_scalar,0xFFFFFFFF,1);
  //DrawPolygonModel(&m_Pos,&m_mOrient,m_iHandle,norm_angles,0,light_scalar,0xFFFFFFFF,1);
  if(k<1)
          DrawModelOutline(GR_RGB(255,255,255),1.0-k);
  g3_EndFrame();
  rend_SetZBufferState(0);
  */
  tce->age += frametime;
}
void RenderSound(tceffect *tce, float frametime, int xoff, int yoff, bool ok_to_render) {
  if (!ok_to_render)
    return;
  ASSERT(tce->type == EFX_SOUND);
  tce->age += frametime;
  if (tce->soundinfo.started)
    return;
  tce->soundinfo.started = true;
  if (tce->soundinfo.handle != -1)
    Sound_system.Play2dSound(tce->soundinfo.handle, MAX_GAME_VOLUME);
}
#define FLASH_SWITCH 1.0f
extern tceffect TCEffects[MAX_TCEFFECTS];
void RenderButton(tceffect *tce, float frametime, int xoff, int yoff, bool ok_to_render) {
  if (!ok_to_render)
    return;
  ASSERT(tce->type == EFX_BUTTON);
  tTCEvent evt;
  bool time_to_flash = false;
  bool glow = false;
  while (PopEvent(tce, &evt)) {
    switch (evt.id) {
    case TEVT_GAINFOCUS: {
      int num = tce - TCEffects;
      // mprintf((0,"Button: Recv'd GAIN FOCUS Event (%d)\n",num));
    } break;
    case TEVT_LOSTFOCUS: {
      int num = tce - TCEffects;
      // mprintf((0,"Button: Recv'd LOST FOCUS Event (%d)\n",num));
    } break;
    case TEVT_MOUSEOVER: {
      if (tce->flags & OBF_GLOW) {
        if (tce->flags & OBF_MOUSEOVERFOCUS) {
          // for effects with this flag set, only glow if we have focus
          if (tce->tab_stop && tce->has_focus) {
            time_to_flash = true;
            glow = true;
          }
        } else {
          time_to_flash = true;
          glow = true;
        }
      }
    } break;
    case TEVT_MOUSEENTER: {
      if (tce->flags & OBF_MOUSEOVERFOCUS) {
        // check to see if we have focus, if not, and we are a tab, set focus
        if (tce->tab_stop && !tce->has_focus) {
          // set the focus on us!
          TelComSetFocusOnEffect(tce - TCEffects);
        }
      }
    } break;
    case TEVT_MOUSECLICK:
    case TEVT_MOUSEDOWN:
    case TEVT_MOUSEUP: {
      // send out the event to the appropriate system/effect
      switch (tce->buttoninfo.button_type) {
      case BUTT_UPARROW: {
        // mprintf((0,"Button: Telling Text to Scroll Up\n"));
        int efxnum = GetEfxNumFromID(tce->buttoninfo.parent, tce->screen);
        if (efxnum != -1) {
          SendEventToEffect(efxnum, TEVT_SCROLLUP);
        }
      } break;
      case BUTT_DOWNARROW: {
        // mprintf((0,"Button: Telling Text to Scroll Down\n"));
        int efxnum = GetEfxNumFromID(tce->buttoninfo.parent, tce->screen);
        if (efxnum != -1) {
          SendEventToEffect(efxnum, TEVT_SCROLLDOWN);
        }
      } break;
      case BUTT_NEXTPAGE:
        // mprintf((0,"Button: Telling TelCom to go to Next Page\n"));
        TelComSendEvent(TEVT_TCNEXT);
        break;
      case BUTT_PREVPAGE:
        // mprintf((0,"Button: Telling TelCom to go to Prev Page\n"));
        TelComSendEvent(TEVT_TCPREV);
        break;
      case BUTT_QUIT:
        // mprintf((0,"Button: Telling TelCom to go to Quit\n"));
        TelComSendEvent(TEVT_TCQUIT);
        break;
      case BUTT_JUMP:
        // mprintf((0,"Button: Telling TelCom to jump to %d\n",tce->buttoninfo.jump_page));
        TelComSendEvent(TEVT_TCJUMP, tce->buttoninfo.jump_page);
        break;
      case BUTT_INTERNAL:
        // mprintf((0,"Button: Calling internal callback\n"));
        if (tce->buttoninfo.internal)
          tce->buttoninfo.internal((int)(tce - TCEffects));
        break;
      default:
        mprintf((0, "Button: Invalid Button Type\n"));
        Int3(); // invalid button type
        // missing JUMP!!!!!!!!!!!!!!!!
      };
    } break;
    default: {
      mprintf((0, "Button: Recv'd Evt %d\n", evt.id));
    } break;
    };
  }
  tce->age += frametime;
  if (tce->age > FLASH_SWITCH) {
    tce->buttoninfo.flash_state = !tce->buttoninfo.flash_state;
    tce->age = 0;
  }
  if (tce->buttoninfo.flash_state && tce->flags & OBF_FLASH) {
    time_to_flash = true;
  }
  if (tce->buttoninfo.flash_time > 0)
    tce->buttoninfo.flash_time -= frametime;
  if (tce->buttoninfo.flash_time > 0 && !glow)
    time_to_flash = false;
  if (tce->buttoninfo.flash_state &&
      (tce->buttoninfo.button_type == BUTT_UPARROW || tce->buttoninfo.button_type == BUTT_DOWNARROW)) {
    // this is an up/down arrow check to see if we can scroll in the appropriate direction
    if (tce->buttoninfo.parent > -1) {
      int pid = GetEfxNumFromID(tce->buttoninfo.parent, tce->screen);
      ASSERT(pid != -1);
      if (pid != -1) {
        tceffect *t = &TCEffects[pid];
        ASSERT(t->type == EFX_TEXT_STATIC || t->type == EFX_TEXT_TYPE || t->type == EFX_TEXT_FADE);
        switch (tce->buttoninfo.button_type) {
        case BUTT_UPARROW: {
          if (t->textinfo.line_index > 0) {
            time_to_flash = true;
          }
        } break;
        case BUTT_DOWNARROW: {
          if (t->textinfo.scroll_d) {
            time_to_flash = true;
          }
        } break;
        }
      }
    }
  }
  if (tce->buttoninfo.flash_handle == -1 && time_to_flash)
    time_to_flash = false;
  if (tce->has_focus) {
    BltBmpToScreen(tce->pos_x + glitch_dx, tce->pos_y + glitch_dy,
                   (time_to_flash) ? &tce->buttoninfo.flashfocus_chunk : &tce->buttoninfo.chunkfocus_bmp);
  } else {
    BltBmpToScreen(tce->pos_x + glitch_dx, tce->pos_y + glitch_dy,
                   (time_to_flash) ? &tce->buttoninfo.flash_chunk : &tce->buttoninfo.chunk_bmp);
  }
}
