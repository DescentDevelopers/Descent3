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

// A fake of the renderer's OpenGL backend (HardwareOpenGL.cpp, HardwareBaseGPU.cpp) and of the platform video layer
// (ddgr), which need a window and a GPU. Everything here draws nothing; the 3D pipeline above it is the real one.

#include <cstdint>
#include <vector>

#include "d3_test/renderer.h"

#include "3d.h"
#include "Ddgr.h"
#include "HardwareInternal.h"
#include "renderer.h"

namespace {

d3_test::FakeViewport viewport;
std::vector<d3_test::DrawnFontCharacter> drawn_font_characters;

} // namespace

namespace d3_test {

FakeViewport &RendererViewport() { return viewport; }
const std::vector<DrawnFontCharacter> &DrawnFontCharacters() { return drawn_font_characters; }

void ResetRenderer() {
  viewport = FakeViewport();
  drawn_font_characters.clear();
}

} // namespace d3_test

// HardwareOpenGL.cpp
struct SDL_Window;
SDL_Window *GSDLWindow = nullptr;
float Z_bias = 0.0f;

bool UseHardware = true;
bool StateLimited = false;

void rend_SetFullScreen(bool) {}
void rend_Flip() {}

// The real backend derives these from the GPU clip rectangle; the fake reports RendererViewport() for both, so the
// software and matrix pipelines see the same dimensions.
void rend_GetProjectionParameters(int *width, int *height) {
  *width = viewport.width;
  *height = viewport.height;
}
void rend_GetProjectionScreenParameters(int &screenLX, int &screenTY, int &screenW, int &screenH) {
  screenLX = viewport.x;
  screenTY = viewport.y;
  screenW = viewport.width;
  screenH = viewport.height;
}

// Referenced by g3_RefreshTransforms(); the real ones only push state to the GPU.
void rend_TransformSetToPassthru() {}
void rend_TransformSetViewport(int, int, int, int) {}
void rend_TransformSetProjection(float[4][4]) {}
void rend_TransformSetModelView(float[4][4]) {}

void rend_SetSoftwareParameters(float aspect, int width, int height, int pitch, uint8_t *framebuffer) {}
void rend_SetTextureType(texture_type) {}
void rend_SetOverlayType(uint8_t type) {}
void rend_SetFiltering(int8_t state) {}
void rend_SetLighting(light_state) {}
void rend_SetAlphaType(int8_t) {}
void rend_SetColorModel(color_model) {}
void rend_SetZBufferState(int8_t state) {}
void rend_SetAlphaValue(uint8_t val) {}
void rend_SetFlatColor(ddgr_color color) {}
void rend_SetOverlayMap(int handle) {}
void rend_SetWrapType(wrap_type val) {}
void rend_SetZBias(float z_bias) {}
void rend_SetZBufferWriteMask(int state) {}
void rend_SetCoplanarPolygonOffset(float factor) {}
void rend_SetBumpmapReadyState(int state, int map) {}
void rend_SetCharacterParameters(ddgr_color color1, ddgr_color color2, ddgr_color color3, ddgr_color color4) {}
void rend_FillRect(ddgr_color color, int x1, int y1, int x2, int y2) {}
void rend_SetPixel(ddgr_color color, int x, int y) {}
ddgr_color rend_GetPixel(int x, int y) { return 0; }
void rend_DrawLine(int x1, int y1, int x2, int y2) {}
void rend_FillCircle(ddgr_color col, int x, int y, int rad) {}
void rend_DrawCircle(int x, int y, int rad) {}
void rend_DrawPolygon3D(int handle, g3Point **p, int nv, int map_type) {}
void rend_DrawPolygon2D(int handle, g3Point **p, int nv) {}
void rend_DrawChunkedBitmap(chunked_bitmap *chunk, int x, int y, uint8_t alpha) {}
void rend_StartFrame(int x1, int y1, int x2, int y2, int clear_flags) {}
void rend_EndFrame() {}
void rend_DrawSpecialLine(g3Point *p0, g3Point *p1) {}
void rend_DrawScaledBitmap(int, int, int, int, int, float, float, float, float, int, const float *) {}

void rend_DrawFontCharacter(int bm_handle, int x1, int y1, int x2, int y2, float u, float v, float w, float h) {
  drawn_font_characters.push_back({bm_handle, x1, y1, x2, y2, u, v, w, h});
}

#ifdef D3_TEST_FAKE_DDGR
// The platform video layer, where dd_grwin32 is not built.
float ddgr_GetAspectRatio() { return 1.0f; }
bool ddgr_surf_Create(ddgr_surface *sf) { return false; }
void ddgr_surf_Destroy(ddgr_surface *sf) {}
bool ddgr_surf_Lock(ddgr_surface *sf, void **ptr, int *rowsize) { return false; }
bool ddgr_surf_Unlock(ddgr_surface *sf, void *ptr) { return false; }
void ddgr_surf_AttachHandle(ddgr_surface *sf, unsigned handle) {}
bool ddgr_surf_InitVideo(ddgr_surface *sf) { return false; }
void ddgr_surf_CloseVideo(ddgr_surface *sf) {}
bool ddgr_surf_FlipVideo(ddgr_surface *sf) { return false; }
void ddgr_surf_Clear(ddgr_surface *dsf, ddgr_color col, int l, int t, int w, int h) {}
bool ddgr_surf_Blt(ddgr_surface *dsf, int dx, int dy, ddgr_surface *ssf, int sx, int sy, int sw, int sh) {
  return false;
}
#endif
