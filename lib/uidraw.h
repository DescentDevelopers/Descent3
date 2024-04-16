

#ifndef UIDRAW_H
#define UIDRAW_H

//	Graphics Primatives.
void ui_StartDraw(int left, int top, int right, int bottom);
void ui_EndDraw();

//	sets text clipping within the startdraw region
void ui_SetTextClip(int left, int top, int right, int bottom);
void ui_ResetTextClip();

//	draw primatives
void ui_DrawSetAlpha(ubyte alpha);
void ui_DrawLine(ddgr_color color, int x1, int y1, int x2, int y2);
void ui_DrawBox(ddgr_color, int l, int t, int r, int b);
void ui_DrawLTBox(ddgr_color lt_col, ddgr_color rb_col, int l, int t, int r, int b);
void ui_DrawRect(ddgr_color, int l, int t, int r, int b);

//	draws all textures derived from ui_bm_handle.
void ui_DrawBitmap(UIBitmapItem *bi, int x, int y, ubyte alpha);
int ui_GetBitmapWidth(UIBitmapItem *bi);
int ui_GetBitmapHeight(UIBitmapItem *bi);

//	sets font for text drawing.
void ui_DrawSetFont(int handle);
int ui_DrawGetFont();
int ui_GetFontHeight();
void ui_SetCharAlpha(ubyte alpha);
int ui_GetTextWidth(const char *text);
int ui_GetTextHeight(const char *text);

#define UI_TEXTTYPE_SATURATE 1
#define UI_TEXTTYPE_SHADOW 2
void ui_DrawSetTextType(int type);

//	draws a line of text.
void ui_DrawString(ddgr_color col, int x, int y, const char *str);
void ui_DrawCenteredString(ddgr_color col, int xoff, int y, const char *str);

#endif