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

#include <algorithm>
#include <array>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <optional>

// TODO: Use SDL_FunctionPointer properly instead
#define SDL_FUNCTION_POINTER_IS_VOID_POINTER
#include <SDL3/SDL.h>

#if defined(WIN32)
#include <windows.h>
#endif

#define DECLARE_OPENGL
#include "dyna_gl.h"

#include "byteswap.h"
#include "pserror.h"
#include "mono.h"
#include "3d.h"
#include "renderer.h"
#include "application.h"
#include "bitmap.h"
#include "lightmap.h"
#include "log.h"
#include "rend_opengl.h"
#include "grdefs.h"
#include "mem.h"
#include "config.h"
#include "rtperformance.h"
#include "HardwareInternal.h"
#include "args.h"
#include "NewBitmap.h"
#include "shaders.h"
#include "ShaderProgram.h"

#if defined(WIN32)
#include "win/arb_extensions.h"
#endif

// General renderer states
extern int gpu_Overlay_map;
int Bump_map = 0;
int Bumpmap_ready = 0;
extern uint8_t gpu_Overlay_type;
float Z_bias = 0.0f;
uint8_t Renderer_close_flag = 0;
extern uint8_t Renderer_initted;
renderer_type Renderer_type = RENDERER_OPENGL;

struct Renderer {
  Renderer() : shader_{
    std::string{kGlslVersion} + std::string{shaders::vertex},
    std::string{kGlslVersion} + std::string{shaders::fragment},
    {
      vertexAttrib(3, GL_FLOAT, GL_FALSE, &PosColorUV2Vertex::pos, "in_pos"),
      vertexAttrib(4, GL_FLOAT, GL_FALSE, &PosColorUV2Vertex::color, "in_color"),
      vertexAttrib(2, GL_FLOAT, GL_FALSE, &PosColorUV2Vertex::uv0, "in_uv0"),
      vertexAttrib(2, GL_FLOAT, GL_FALSE, &PosColorUV2Vertex::uv1, "in_uv1")
    }} {
    shader_.Use();

    // these are effectively just constants, for now
    shader_.setUniform1i("u_texture0", 0);
    shader_.setUniform1i("u_texture1", 1);
  }

  /**
   * Sets the vertex transformation matrices. Takes all three of the MVP matrices at once, in order to avoid
   * multiple SetUniform operations. Pass std::nullopt for any matrices that should not be altered.
   */
  void setTransform(std::optional<glm::mat4x4> const &model, std::optional<glm::mat4x4> const &view,
                    std::optional<glm::mat4x4> const &projection) {
    if (model) {
      model_ = *model;
    }
    if (view) {
      view_ = *view;
    }
    if (projection) {
      projection_ = *projection;
    }

    shader_.setUniformMat4f("u_modelview", view_ * model_);
    shader_.setUniformMat4f("u_projection", projection_);
  }

  void setTextureEnabled(GLuint index, bool enabled) {
    GLint bit = 1 << index;
    if (enabled) {
      texture_enable_ |= bit;
    } else {
      texture_enable_ &= ~bit;
    }
    shader_.setUniform1i("u_texture_enable", texture_enable_);
  }

  template <typename VertexIter, typename = std::enable_if_t<std::is_same_v<
                                     typename std::iterator_traits<VertexIter>::value_type, PosColorUV2Vertex>>>
  size_t addVertexData(VertexIter begin, VertexIter end) {
    return shader_.addVertexData(begin, end);
  }

  struct PosColorUVVertex_tag {};
  template <typename VertexIter, typename = std::enable_if_t<std::is_same_v<
                                     typename std::iterator_traits<VertexIter>::value_type, PosColorUVVertex>>>
  size_t addVertexData(VertexIter begin, VertexIter end, PosColorUVVertex_tag = {}) {
    std::array<PosColorUV2Vertex, MAX_POINTS_IN_POLY> converted;
    std::transform(begin, end, converted.begin(),
                   [](auto const &vtx) { return PosColorUV2Vertex{vtx.pos, vtx.color, vtx.uv, {}}; });
    return shader_.addVertexData(converted.cbegin(), converted.cend());
  }

  void setFogEnabled(bool enabled) { shader_.setUniform1i("u_fog_enable", enabled); }

  void setFogBorders(float nearz, float farz) {
    shader_.setUniform1f("u_fog_start", nearz);
    shader_.setUniform1f("u_fog_end", farz);
  }

  void setGammaCorrection(float gamma) { shader_.setUniform1f("u_gamma", gamma); }

  void setFogColor(ddgr_color color) {
    shader_.setUniform4fv("u_fog_color", GR_COLOR_RED(color) / 255.0f, GR_COLOR_GREEN(color) / 255.0f,
                          GR_COLOR_BLUE(color) / 255.0f, 1);
  }

private:
  glm::mat4x4 model_;
  glm::mat4x4 view_;
  glm::mat4x4 projection_;
  GLint texture_enable_{};
  ShaderProgram<PosColorUV2Vertex> shader_;

  static constexpr auto kGlslVersion =
#if defined(ANDROID)
      shaders::version_300_es;
#else
  shaders::version_150_core;
#endif
};
std::optional<Renderer> gRenderer;

#ifndef GL_UNSIGNED_SHORT_5_5_5_1
#define GL_UNSIGNED_SHORT_5_5_5_1 0x8034
#endif

#ifndef GL_UNSIGNED_SHORT_4_4_4_4
#define GL_UNSIGNED_SHORT_4_4_4_4 0x8033
#endif

#define CHECK_ERROR(x)

SDL_Window *GSDLWindow = nullptr;
SDL_GLContext GSDLGLContext = nullptr;
char loadedLibrary[_MAX_PATH];

#define GET_WRAP_STATE(x) ((x) >> 4)
#define GET_FILTER_STATE(x) ((x) & 0x0f)

#define SET_WRAP_STATE(x, s)                                                                                           \
  {                                                                                                                    \
    (x) &= 0x0F;                                                                                                       \
    (x) |= ((s) << 4);                                                                                                 \
  }
#define SET_FILTER_STATE(x, s)                                                                                         \
  {                                                                                                                    \
    (x) &= 0xF0;                                                                                                       \
    (x) |= (s);                                                                                                        \
  }

//	OpenGL Stuff
static int OpenGL_polys_drawn = 0;
static int OpenGL_verts_processed = 0;
static int OpenGL_uploads = 0;
static int OpenGL_sets_this_frame[10];
static int OpenGL_packed_pixels = 0;
static std::vector<GLuint> textures_;
static int OpenGL_cache_initted = 0;
static int OpenGL_last_bound[2];

extern int gpu_last_frame_polys_drawn;
extern int gpu_last_frame_verts_processed;
extern int gpu_last_uploaded;

extern float gpu_Alpha_factor;
extern float gpu_Alpha_multiplier;

uint16_t *OpenGL_bitmap_remap = nullptr;
uint16_t *OpenGL_lightmap_remap = nullptr;
uint8_t *OpenGL_bitmap_states = nullptr;
uint8_t *OpenGL_lightmap_states = nullptr;

uint32_t *opengl_Upload_data = nullptr;
uint32_t *opengl_Translate_table = nullptr;
uint32_t *opengl_4444_translate_table = nullptr;

uint16_t *opengl_packed_Upload_data = nullptr;
uint16_t *opengl_packed_Translate_table = nullptr;
uint16_t *opengl_packed_4444_translate_table = nullptr;

extern rendering_state gpu_state;
extern renderer_preferred_state gpu_preferred_state;

bool OpenGL_multitexture_state = false;
module *OpenGLDLLHandle = nullptr;
int Already_loaded = 0;
bool opengl_Blending_on = false;

static oeApplication *ParentApplication = nullptr;

static GLuint GOpenGLFBO = 0;
static GLuint GOpenGLRBOColor = 0;
static GLuint GOpenGLRBODepth = 0;
static GLuint GOpenGLFBOWidth = 0;
static GLuint GOpenGLFBOHeight = 0;

// returns true if the passed in extension name is supported
bool opengl_CheckExtension(std::string_view extName) {
  GLint numExtensions;
  dglGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);
  for (GLint i = 0; i < numExtensions; i++) {
    if (extName == reinterpret_cast<char const *>(dglGetStringi(GL_EXTENSIONS, i))) {
      return true;
    }
  }
  return false;
}

// Gets some specific information about this particular flavor of opengl
void opengl_GetInformation() {
  LOG_INFO.printf("OpenGL Vendor: %s", dglGetString(GL_VENDOR));
  LOG_INFO.printf("OpenGL Renderer: %s", dglGetString(GL_RENDERER));
  LOG_INFO.printf("OpenGL Version: %s", dglGetString(GL_VERSION));
}

int opengl_MakeTextureObject(int tn) {
  GLuint num;
  dglGenTextures(1, &num);
  textures_.push_back(num);

  dglActiveTexture(GL_TEXTURE0_ARB + tn);

  dglBindTexture(GL_TEXTURE_2D, num);
  dglPixelStorei(GL_UNPACK_ALIGNMENT, 2);

  dglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  dglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  dglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  dglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  // glTexEnvf (GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);

  CHECK_ERROR(2)

  return num;
}

int opengl_InitCache() {

  OpenGL_bitmap_remap = mem_rmalloc<uint16_t>(MAX_BITMAPS);
  ASSERT(OpenGL_bitmap_remap);
  OpenGL_lightmap_remap = mem_rmalloc<uint16_t>(MAX_LIGHTMAPS);
  ASSERT(OpenGL_lightmap_remap);

  OpenGL_bitmap_states = mem_rmalloc<uint8_t>(MAX_BITMAPS);
  ASSERT(OpenGL_bitmap_states);
  OpenGL_lightmap_states = mem_rmalloc<uint8_t>(MAX_LIGHTMAPS);
  ASSERT(OpenGL_lightmap_states);

  // Setup textures and cacheing
  int i;
  for (i = 0; i < MAX_BITMAPS; i++) {
    OpenGL_bitmap_remap[i] = 65535;
    OpenGL_bitmap_states[i] = 255;
    GameBitmaps[i].flags |= BF_CHANGED | BF_BRAND_NEW;
  }

  for (i = 0; i < MAX_LIGHTMAPS; i++) {
    OpenGL_lightmap_remap[i] = 65535;
    OpenGL_lightmap_states[i] = 255;
    GameLightmaps[i].flags |= LF_CHANGED | LF_BRAND_NEW;
  }

  CHECK_ERROR(3)

  OpenGL_cache_initted = 1;
  return 1;
}

// Sets default states for our renderer
void opengl_SetDefaults() {
  LOG_INFO << "Setting states";

  gpu_state.cur_color = 0x00FFFFFF;
  gpu_state.cur_bilinear_state = -1;
  gpu_state.cur_zbuffer_state = -1;
  gpu_state.cur_texture_quality = -1;
  gpu_state.cur_light_state = LS_GOURAUD;
  gpu_state.cur_color_model = CM_MONO;
  gpu_state.cur_bilinear_state = -1;
  gpu_state.cur_alpha_type = AT_TEXTURE;

  // Enable some states
  dglEnable(GL_BLEND);
  dglEnable(GL_DITHER);
  opengl_Blending_on = true;

  rend_SetAlphaType(AT_ALWAYS);
  rend_SetAlphaValue(255);
  rend_SetFiltering(1);
  rend_SetLighting(LS_NONE);
  rend_SetTextureType(TT_FLAT);
  rend_SetColorModel(CM_RGB);
  rend_SetZBufferState(1);
  rend_SetGammaValue(gpu_preferred_state.gamma);
  OpenGL_last_bound[0] = 9999999;
  OpenGL_last_bound[1] = 9999999;
  OpenGL_multitexture_state = false;

  dglEnable(GL_SCISSOR_TEST);
  dglScissor(0, 0, gpu_state.screen_width, gpu_state.screen_height);
  dglDisable(GL_SCISSOR_TEST);

  gpu_BindTexture(BAD_BITMAP_HANDLE, MAP_TYPE_BITMAP, 0);
  gpu_BindTexture(BAD_BITMAP_HANDLE, MAP_TYPE_BITMAP, 1);

  dglActiveTexture(GL_TEXTURE0_ARB + 1);
  gRenderer->setTextureEnabled(1, false);
  dglEnable(GL_BLEND);
  dglEnable(GL_DITHER);
  dglBlendFunc(GL_DST_COLOR, GL_ZERO);
  dglActiveTexture(GL_TEXTURE0_ARB + 0);
}

extern renderer_preferred_state Render_preferred_state;

int opengl_Setup(oeApplication *app, const int *width, const int *height) {
  int winw = Video_res_list[Current_video_resolution_id].width;
  int winh = Video_res_list[Current_video_resolution_id].height;

  SDL_ClearError();
  if (!SDL_WasInit(SDL_INIT_VIDEO)) {
    SDL_SetHint(SDL_HINT_TOUCH_MOUSE_EVENTS, "0");
    const int rc = SDL_Init(SDL_INIT_VIDEO);
    if (rc != 0) {
      char buffer[512];
      snprintf(buffer, sizeof(buffer), "SDL_GetError() reports \"%s\".\n", SDL_GetError());
      fprintf(stderr, "SDL: SDL_Init() failed! rc == (%d).\n", rc);
      fprintf(stderr, "%s", buffer);
      rend_SetErrorMessage(buffer);
      return (0);
    }
  }

  if (!Already_loaded) {
    char gl_library[256];
    int arg;
    arg = FindArgChar("-gllibrary", 'g');
    if (arg != 0) {
      strcpy(gl_library, GameArgs[arg + 1]);
    } else {
      gl_library[0] = 0;
    }

    LOG_INFO.printf("OpenGL: Attempting to use \"%s\" for OpenGL",
                    gl_library[0] ? gl_library : "[system default library]");

    // ryan's adds. 04/18/2000...SDL stuff on 04/25/2000
    bool success = true;

    OpenGLDLLHandle = LoadOpenGLDLL(gl_library);
    if (!(OpenGLDLLHandle)) {
      // rcg07072000 last ditch effort...
#if defined(POSIX)
      strcpy(gl_library, "libGL.so.1");
#else
      strcpy(gl_library, "opengl32.dll");
#endif
      OpenGLDLLHandle = LoadOpenGLDLL(gl_library);
      if (!(OpenGLDLLHandle)) {
        success = false;
      }
    } // if

    if (!success) {
      char buffer[512];
      snprintf(buffer, sizeof(buffer), "Failed to load library [%s].\n", gl_library);
      fprintf(stderr, "%s", buffer);
      rend_SetErrorMessage(buffer);
      return 0;
    } // if
  }

  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
#if !defined(ANDROID)
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#endif

  if (!GSDLWindow) {
    int display_num = 0;
    int display_arg = FindArg("-display");
    int display_count = 0;

    // High-DPI support
    {
      float scale = SDL_GetDisplayContentScale(Display_id);
      LOG_WARNING.printf("Using content scale %f", scale);
      winw = std::floor(static_cast<float>(winw) * scale);
      winh = std::floor(static_cast<float>(winh) * scale);
    }

    SDL_PropertiesID props = SDL_CreateProperties();
    SDL_SetStringProperty(props, SDL_PROP_WINDOW_CREATE_TITLE_STRING, "Descent 3");
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_X_NUMBER, SDL_WINDOWPOS_UNDEFINED_DISPLAY(Display_id));
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_Y_NUMBER, SDL_WINDOWPOS_UNDEFINED_DISPLAY(Display_id));
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, winw);
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, winh);
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_FLAGS_NUMBER, SDL_WINDOW_OPENGL);
    GSDLWindow = SDL_CreateWindowWithProperties(props);
    SDL_DestroyProperties(props);
    if (!GSDLWindow) {
      LOG_ERROR.printf("OpenGL: SDL window creation failed: %s", SDL_GetError());
      return 0;
    }

    bool grabMouse = FindArgChar("-nomousegrab", 'm') == 0;
    SDL_SetWindowRelativeMouseMode(GSDLWindow, grabMouse);

    rend_SetFullScreen(Game_fullscreen);
  } else if (!Game_fullscreen) {
    SDL_SetWindowSize(GSDLWindow, winw, winh);
  }

  if (!GSDLGLContext) {
    GSDLGLContext = SDL_GL_CreateContext(GSDLWindow);
    if (!GSDLGLContext) {
      LOG_ERROR.printf("OpenGL: OpenGL context creation failed: %s", SDL_GetError());
      SDL_DestroyWindow(GSDLWindow);
      GSDLWindow = nullptr;
      return 0;
    }
  }

  try {
    LoadGLFnPtrs();
  } catch (std::exception const &ex) {
    // TODO: more raii-esque construction and cleanup here
    SDL_GL_DestroyContext(GSDLGLContext);
    GSDLGLContext = nullptr;
    SDL_DestroyWindow(GSDLWindow);
    GSDLWindow = nullptr;
    LOG_ERROR.printf("Error loading opengl dll: %s", ex.what());
    mod_FreeModule(&OpenGLDLLInst);
    OpenGLDLLHandle = nullptr;
    return 0;
  }

  // clear the window framebuffer to start.
  dglClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  dglClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  SDL_GL_SwapWindow(GSDLWindow);

  /* Tear down the backbuffer and rebuild at new dimensions... */
  if (GOpenGLFBO) {
    dglBindFramebuffer(GL_FRAMEBUFFER, GOpenGLFBO);
    dglFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, 0);
    dglFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0);
    dglBindRenderbuffer(GL_RENDERBUFFER, 0);
    dglBindFramebuffer(GL_FRAMEBUFFER, 0);
    dglDeleteFramebuffers(1, &GOpenGLFBO);
    dglDeleteRenderbuffers(1, &GOpenGLRBOColor);
    dglDeleteRenderbuffers(1, &GOpenGLRBODepth);
    GOpenGLFBOWidth = GOpenGLFBOHeight = GOpenGLFBO = GOpenGLRBOColor = GOpenGLRBODepth = 0;
  }

  const GLsizei w = (GLsizei)*width;
  const GLsizei h = (GLsizei)*height;

  GOpenGLFBOWidth = w;
  GOpenGLFBOHeight = h;

  dglGenFramebuffers(1, &GOpenGLFBO);
  dglBindFramebuffer(GL_FRAMEBUFFER, GOpenGLFBO);

  dglGenRenderbuffers(1, &GOpenGLRBOColor);
  dglBindRenderbuffer(GL_RENDERBUFFER, GOpenGLRBOColor);
  dglRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, w, h);
  dglFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, GOpenGLRBOColor);

  dglGenRenderbuffers(1, &GOpenGLRBODepth);
  dglBindRenderbuffer(GL_RENDERBUFFER, GOpenGLRBODepth);
  dglRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, w, h);
  dglFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, GOpenGLRBODepth);

  if (dglCheckFramebufferStatus(GL_FRAMEBUFFER_EXT) != GL_FRAMEBUFFER_COMPLETE_EXT) {
    LOG_WARNING << "OpenGL: our framebuffer object is incomplete, giving up";
    dglFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, 0);
    dglFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0);
    dglBindRenderbuffer(GL_RENDERBUFFER, 0);
    dglBindFramebuffer(GL_FRAMEBUFFER, 0);
    dglDeleteFramebuffers(1, &GOpenGLFBO);
    dglDeleteRenderbuffers(1, &GOpenGLRBOColor);
    dglDeleteRenderbuffers(1, &GOpenGLRBODepth);
    GOpenGLFBO = GOpenGLRBOColor = GOpenGLRBODepth = 0;
    SDL_GL_DestroyContext(GSDLGLContext);
    SDL_DestroyWindow(GSDLWindow);
    GSDLGLContext = nullptr;
    GSDLWindow = nullptr;
    return 0;
  }

  if (ParentApplication) {
    reinterpret_cast<oeLnxApplication *>(ParentApplication)->set_sizepos(0, 0, *width, *height);
  }

  gRenderer.emplace();

  Already_loaded = 1;
  return 1;
}

// Sets up our OpenGL rendering context
// Returns 1 if ok, 0 if something bad
int opengl_Init(oeApplication *app, renderer_preferred_state *pref_state) {
  int width, height;
  int retval = 1;
  int i;

  LOG_INFO << "Setting up opengl mode!";

  if (pref_state) {
    gpu_preferred_state = *pref_state;
  }

  if (app != nullptr) {
    ParentApplication = app;
  }

  /***********************************************************
   *               LINUX OPENGL
   ***********************************************************
   */
  // Setup gpu_state.screen_width & gpu_state.screen_height & width & height
  width = gpu_preferred_state.width;
  height = gpu_preferred_state.height;

  if (!opengl_Setup(app, &width, &height)) {
    opengl_Close();
    return 0;
  }

  memset(&gpu_state, 0, sizeof(rendering_state));
  gpu_state.screen_width = width;
  gpu_state.screen_height = height;

  // Get some info
  opengl_GetInformation();

  // Do we have packed pixel formats?
  OpenGL_packed_pixels = opengl_CheckExtension("GL_EXT_packed_pixels");

  opengl_InitCache();

  if (OpenGL_packed_pixels) {
    opengl_packed_Upload_data = mem_rmalloc<uint16_t>(2048 * 2048);
    opengl_packed_Translate_table = mem_rmalloc<uint16_t>(65536);
    opengl_packed_4444_translate_table = mem_rmalloc<uint16_t>(65536);

    ASSERT(opengl_packed_Upload_data);
    ASSERT(opengl_packed_Translate_table);
    ASSERT(opengl_packed_4444_translate_table);

    LOG_INFO << "Building packed OpenGL translate table...";

    for (i = 0; i < 65536; i++) {
      int r = (i >> 10) & 0x1f;
      int g = (i >> 5) & 0x1f;
      int b = i & 0x1f;

#ifdef BRIGHTNESS_HACK
      r *= BRIGHTNESS_HACK;
      g *= BRIGHTNESS_HACK;
      b *= BRIGHTNESS_HACK;
      if (r > 0x1F)
        r = 0x1F;
      if (g > 0x1F)
        g = 0x1F;
      if (b > 0x1F)
        b = 0x1F;
#endif

      uint16_t pix;

      if (!(i & OPAQUE_FLAG)) {
        pix = 0;
      } else {
        pix = (r << 11) | (g << 6) | (b << 1) | 1;
      }

      opengl_packed_Translate_table[i] = INTEL_INT(pix);

      // 4444 table
      int a = (i >> 12) & 0xf;
      r = (i >> 8) & 0xf;
      g = (i >> 4) & 0xf;
      b = i & 0xf;

      pix = (r << 12) | (g << 8) | (b << 4) | a;
      opengl_packed_4444_translate_table[i] = INTEL_INT(pix);
    }
  } else {
    opengl_Upload_data = mem_rmalloc<uint32_t>(2048 * 2048);
    opengl_Translate_table = mem_rmalloc<uint32_t>(65536);
    opengl_4444_translate_table = mem_rmalloc<uint32_t>(65536);

    ASSERT(opengl_Upload_data);
    ASSERT(opengl_Translate_table);
    ASSERT(opengl_4444_translate_table);

    LOG_INFO << "Building OpenGL translate table...";

    for (i = 0; i < 65536; i++) {
      uint32_t pix;
      int r = (i >> 10) & 0x1f;
      int g = (i >> 5) & 0x1f;
      int b = i & 0x1f;

#ifdef BRIGHTNESS_HACK
      r *= BRIGHTNESS_HACK;
      g *= BRIGHTNESS_HACK;
      b *= BRIGHTNESS_HACK;
      if (r > 0x1F)
        r = 0x1F;
      if (g > 0x1F)
        g = 0x1F;
      if (b > 0x1F)
        b = 0x1F;
#endif

      float fr = (float)r / 31.0f;
      float fg = (float)g / 31.0f;
      float fb = (float)b / 31.0f;

      r = 255 * fr;
      g = 255 * fg;
      b = 255 * fb;

      if (!(i & OPAQUE_FLAG)) {
        pix = 0;
      } else {
        pix = (255 << 24) | (b << 16) | (g << 8) | (r);
      }

      opengl_Translate_table[i] = INTEL_INT(pix);

      // Do 4444
      int a = (i >> 12) & 0xf;
      r = (i >> 8) & 0xf;
      g = (i >> 4) & 0xf;
      b = i & 0xf;

      float fa = (float)a / 15.0f;
      fr = (float)r / 15.0f;
      fg = (float)g / 15.0f;
      fb = (float)b / 15.0f;

      a = 255 * fa;
      r = 255 * fr;
      g = 255 * fg;
      b = 255 * fb;

      pix = (a << 24) | (b << 16) | (g << 8) | (r);

      opengl_4444_translate_table[i] = INTEL_INT(pix);
    }
  }

  opengl_SetDefaults();

  g3_ForceTransformRefresh();

  CHECK_ERROR(4)

  gpu_state.initted = 1;

  LOG_INFO.printf("OpenGL initialization at %d x %d was successful.", width, height);

  return retval;
}

// Releases the rendering context
void opengl_Close(const bool just_resizing) {
  CHECK_ERROR(5)

  dglDeleteTextures(textures_.size(), textures_.data());
  textures_.clear();

  gRenderer.reset();

  if (GSDLGLContext) {
    SDL_GL_MakeCurrent(nullptr, nullptr);
    SDL_GL_DestroyContext(GSDLGLContext);
    GSDLGLContext = nullptr;
    GOpenGLFBOWidth = GOpenGLFBOHeight = GOpenGLFBO = GOpenGLRBOColor = GOpenGLRBODepth = 0;
  }

  if (!just_resizing && GSDLWindow) {
    SDL_DestroyWindow(GSDLWindow);
    GSDLWindow = nullptr;
  }

  if (OpenGL_packed_pixels) {
    if (opengl_packed_Upload_data) {
      mem_free(opengl_packed_Upload_data);
    }
    if (opengl_packed_Translate_table) {
      mem_free(opengl_packed_Translate_table);
    }
    if (opengl_packed_4444_translate_table) {
      mem_free(opengl_packed_4444_translate_table);
    }
    opengl_packed_Upload_data = nullptr;
    opengl_packed_Translate_table = nullptr;
    opengl_packed_4444_translate_table = nullptr;
  } else {
    if (opengl_Upload_data)
      mem_free(opengl_Upload_data);
    if (opengl_Translate_table)
      mem_free(opengl_Translate_table);
    if (opengl_4444_translate_table)
      mem_free(opengl_4444_translate_table);
    opengl_Upload_data = nullptr;
    opengl_Translate_table = nullptr;
    opengl_4444_translate_table = nullptr;
  }

  if (OpenGL_cache_initted) {
    mem_free(OpenGL_lightmap_remap);
    mem_free(OpenGL_bitmap_remap);
    mem_free(OpenGL_lightmap_states);
    mem_free(OpenGL_bitmap_states);
    OpenGL_cache_initted = 0;
  }

  // mod_FreeModule (OpenGLDLLHandle);
  gpu_state.initted = 0;
}

// Takes our 16bit format and converts it into the memory scheme that OpenGL wants
void opengl_TranslateBitmapToOpenGL(int texnum, int bm_handle, int map_type, int replace, int tn) {
  uint16_t *bm_ptr;

  int w, h;
  int size;

  dglActiveTexture(GL_TEXTURE0_ARB + tn);

  if (map_type == MAP_TYPE_LIGHTMAP) {
    if (GameLightmaps[bm_handle].flags & LF_BRAND_NEW)
      replace = 0;

    bm_ptr = lm_data(bm_handle);
    GameLightmaps[bm_handle].flags &= ~(LF_CHANGED | LF_BRAND_NEW);

    w = lm_w(bm_handle);
    h = lm_h(bm_handle);
    size = GameLightmaps[bm_handle].square_res;
  } else {
    if (GameBitmaps[bm_handle].flags & BF_BRAND_NEW)
      replace = 0;

    bm_ptr = bm_data(bm_handle, 0);
    GameBitmaps[bm_handle].flags &= ~(BF_CHANGED | BF_BRAND_NEW);
    w = bm_w(bm_handle, 0);
    h = bm_h(bm_handle, 0);
    size = w;
  }

  if (OpenGL_last_bound[tn] != texnum) {
    dglBindTexture(GL_TEXTURE_2D, texnum);
    OpenGL_sets_this_frame[0]++;
    OpenGL_last_bound[tn] = texnum;
  }

  if (OpenGL_packed_pixels) {
    if (map_type == MAP_TYPE_LIGHTMAP) {
      uint16_t *left_data = (uint16_t *)opengl_packed_Upload_data;
      int bm_left = 0;

      for (int i = 0; i < h; i++, left_data += size, bm_left += w) {
        uint16_t *dest_data = left_data;
        for (int t = 0; t < w; t++) {
          *dest_data++ = opengl_packed_Translate_table[bm_ptr[bm_left + t]];
        }
      }

      if (replace) {
        dglTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, size, size, GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1,
                         opengl_packed_Upload_data);
      } else {
        dglTexImage2D(GL_TEXTURE_2D, 0, GL_RGB5_A1, size, size, 0, GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1,
                      opengl_packed_Upload_data);
      }
    } else {
      int limit = 0;

      if (bm_mipped(bm_handle)) {
        limit = NUM_MIP_LEVELS + 3;
      } else {
        limit = 1;
      }

      for (int m = 0; m < limit; m++) {
        if (m < NUM_MIP_LEVELS) {
          bm_ptr = bm_data(bm_handle, m);
          w = bm_w(bm_handle, m);
          h = bm_h(bm_handle, m);
        } else {
          bm_ptr = bm_data(bm_handle, NUM_MIP_LEVELS - 1);
          w = bm_w(bm_handle, NUM_MIP_LEVELS - 1);
          h = bm_h(bm_handle, NUM_MIP_LEVELS - 1);

          w >>= m - (NUM_MIP_LEVELS - 1);
          h >>= m - (NUM_MIP_LEVELS - 1);

          if ((w < 1) || (h < 1))
            continue;
        }

        if (bm_format(bm_handle) == BITMAP_FORMAT_4444) {
          // Do 4444
          if (bm_mipped(bm_handle)) {
            for (int i = 0; i < w * h; i++)
              opengl_packed_Upload_data[i] = 0xf | (opengl_packed_4444_translate_table[bm_ptr[i]]);
          } else {
            for (int i = 0; i < w * h; i++)
              opengl_packed_Upload_data[i] = opengl_packed_4444_translate_table[bm_ptr[i]];
          }

          if (replace) {
            dglTexSubImage2D(GL_TEXTURE_2D, m, 0, 0, w, h, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4,
                             opengl_packed_Upload_data);
          } else {
            dglTexImage2D(GL_TEXTURE_2D, m, GL_RGBA4, w, h, 0, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4,
                          opengl_packed_Upload_data);
          }
        } else {
          // Do 1555
          for (int i = 0; i < w * h; i++) {
            opengl_packed_Upload_data[i] = opengl_packed_Translate_table[bm_ptr[i]];
          }

          if (replace) {
            dglTexSubImage2D(GL_TEXTURE_2D, m, 0, 0, w, h, GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1,
                             opengl_packed_Upload_data);
          } else {
            dglTexImage2D(GL_TEXTURE_2D, m, GL_RGB5_A1, w, h, 0, GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1,
                          opengl_packed_Upload_data);
          }
        }
      }
    }
  } else {
    if (map_type == MAP_TYPE_LIGHTMAP) {
      uint32_t *left_data = (uint32_t *)opengl_Upload_data;
      int bm_left = 0;

      for (int i = 0; i < h; i++, left_data += size, bm_left += w) {
        uint32_t *dest_data = left_data;
        for (int t = 0; t < w; t++) {
          *dest_data++ = opengl_Translate_table[bm_ptr[bm_left + t]];
        }
      }
      if (size > 0) {
        if (replace) {
          dglTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, size, size, GL_RGBA, GL_UNSIGNED_BYTE, opengl_Upload_data);
        } else {
          dglTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size, size, 0, GL_RGBA, GL_UNSIGNED_BYTE, opengl_Upload_data);
        }
      }
    } else {
      int limit = 0;

      if (bm_mipped(bm_handle)) {
        limit = NUM_MIP_LEVELS + 3; // ryan added +3.
      } else {
        limit = 1;
      }

      for (int m = 0; m < limit; m++) {
        bm_ptr = bm_data(bm_handle, m);
        w = bm_w(bm_handle, m);
        h = bm_h(bm_handle, m);

        if (bm_format(bm_handle) == BITMAP_FORMAT_4444) {
          // Do 4444

          if (bm_mipped(bm_handle)) {
            for (int i = 0; i < w * h; i++)
              opengl_Upload_data[i] = INTEL_INT((255 << 24)) | opengl_4444_translate_table[bm_ptr[i]];
          } else {
            for (int i = 0; i < w * h; i++)
              opengl_Upload_data[i] = opengl_4444_translate_table[bm_ptr[i]];
          }
        } else {
          // Do 1555

          for (int i = 0; i < w * h; i++)
            opengl_Upload_data[i] = opengl_Translate_table[bm_ptr[i]];
        }

        // rcg06262000 my if wrapper.
        if ((w > 0) && (h > 0)) {
          if (replace) {
            dglTexSubImage2D(GL_TEXTURE_2D, m, 0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, opengl_Upload_data);
          } else {
            dglTexImage2D(GL_TEXTURE_2D, m, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, opengl_Upload_data);
          }
        }
      }
    }
  }

  // mprintf(1,"Doing slow upload to opengl!\n");

  if (map_type == MAP_TYPE_LIGHTMAP) {
    GameLightmaps[bm_handle].flags &= ~LF_LIMITS;
  }

  CHECK_ERROR(6)
  OpenGL_uploads++;
}

extern bool Force_one_texture;

// Utilizes a LRU cacheing scheme to select/upload textures the opengl driver
int opengl_MakeBitmapCurrent(int handle, int map_type, int tn) {
  int w, h;
  int texnum;

  if (map_type == MAP_TYPE_LIGHTMAP) {
    w = GameLightmaps[handle].square_res;
    h = GameLightmaps[handle].square_res;
  } else {
    if (Force_one_texture) {
      handle = 0;
    }

    w = bm_w(handle, 0);
    h = bm_h(handle, 0);
  }

  if (w != h) {
    LOG_WARNING << "Can't use non-square textures with OpenGL!";
    return 0;
  }

  // See if the bitmaps is already in the cache
  if (map_type == MAP_TYPE_LIGHTMAP) {
    if (OpenGL_lightmap_remap[handle] == 65535) {
      texnum = opengl_MakeTextureObject(tn);
      SET_WRAP_STATE(OpenGL_lightmap_states[handle], 1);
      SET_FILTER_STATE(OpenGL_lightmap_states[handle], 0);
      OpenGL_lightmap_remap[handle] = texnum;
      opengl_TranslateBitmapToOpenGL(texnum, handle, map_type, 0, tn);
    } else {
      texnum = OpenGL_lightmap_remap[handle];
      if (GameLightmaps[handle].flags & LF_CHANGED)
        opengl_TranslateBitmapToOpenGL(texnum, handle, map_type, 1, tn);
    }
  } else {
    if (OpenGL_bitmap_remap[handle] == 65535) {
      texnum = opengl_MakeTextureObject(tn);
      SET_WRAP_STATE(OpenGL_bitmap_states[handle], 1);
      SET_FILTER_STATE(OpenGL_bitmap_states[handle], 0);
      OpenGL_bitmap_remap[handle] = texnum;
      opengl_TranslateBitmapToOpenGL(texnum, handle, map_type, 0, tn);
    } else {
      texnum = OpenGL_bitmap_remap[handle];
      if (GameBitmaps[handle].flags & BF_CHANGED) {
        opengl_TranslateBitmapToOpenGL(texnum, handle, map_type, 1, tn);
      }
    }
  }

  if (OpenGL_last_bound[tn] != texnum) {
    dglActiveTexture(GL_TEXTURE0_ARB + tn);

    dglBindTexture(GL_TEXTURE_2D, texnum);
    OpenGL_last_bound[tn] = texnum;
    OpenGL_sets_this_frame[0]++;
  }

  CHECK_ERROR(7)
  return 1;
}

// Sets up an appropriate wrap type for the current bound texture
void opengl_MakeWrapTypeCurrent(int handle, int map_type, int tn) {
  int uwrap;
  wrap_type dest_wrap;

  if (tn == 1)
    dest_wrap = WT_CLAMP;
  else
    dest_wrap = gpu_state.cur_wrap_type;

  if (map_type == MAP_TYPE_LIGHTMAP)
    uwrap = GET_WRAP_STATE(OpenGL_lightmap_states[handle]);
  else
    uwrap = GET_WRAP_STATE(OpenGL_bitmap_states[handle]);

  if (uwrap == dest_wrap)
    return;

  dglActiveTexture(GL_TEXTURE0_ARB + tn);

  OpenGL_sets_this_frame[1]++;

  if (gpu_state.cur_wrap_type == WT_CLAMP) {
    dglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    dglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  } else if (gpu_state.cur_wrap_type == WT_WRAP_V) {
    dglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    dglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  } else {
    dglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    dglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  }

  if (map_type == MAP_TYPE_LIGHTMAP) {
    SET_WRAP_STATE(OpenGL_lightmap_states[handle], dest_wrap);
  } else {
    SET_WRAP_STATE(OpenGL_bitmap_states[handle], dest_wrap);
  }

  CHECK_ERROR(8)
}

// Chooses the correct filter type for the currently bound texture
void opengl_MakeFilterTypeCurrent(int handle, int map_type, int tn) {
  int magf;
  int8_t dest_state;

  if (map_type == MAP_TYPE_LIGHTMAP) {
    magf = GET_FILTER_STATE(OpenGL_lightmap_states[handle]);
    dest_state = 1;
  } else {
    magf = GET_FILTER_STATE(OpenGL_bitmap_states[handle]);
    dest_state = gpu_preferred_state.filtering;
    if (!gpu_state.cur_bilinear_state)
      dest_state = 0;
  }

  if (magf == dest_state)
    return;

  dglActiveTexture(GL_TEXTURE0_ARB + tn);

  OpenGL_sets_this_frame[2]++;

  if (dest_state) {
    if (map_type == MAP_TYPE_BITMAP && bm_mipped(handle)) {
      dglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      dglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    } else {
      dglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      dglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }
  } else {
    if (map_type == MAP_TYPE_BITMAP && bm_mipped(handle)) {
      // dglTexParameteri (GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST_MIPMAP_NEAREST);
      dglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      dglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    } else {
      dglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      dglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    }
  }

  if (map_type == MAP_TYPE_LIGHTMAP) {
    SET_FILTER_STATE(OpenGL_lightmap_states[handle], dest_state);
  } else {
    SET_FILTER_STATE(OpenGL_bitmap_states[handle], dest_state);
  }

  CHECK_ERROR(9)
}

// Turns on/off multitexture blending
void gpu_SetMultitextureBlendMode(bool state) {
  if (OpenGL_multitexture_state == state)
    return;
  OpenGL_multitexture_state = state;

  gRenderer->setTextureEnabled(1, state);
}

void gpu_DrawFlatPolygon3D(g3Point **p, int nv) {
  gpu_SetMultitextureBlendMode(false);

  std::array<PosColorUVVertex, 100> vertices{};
  std::transform(p, p + nv, std::begin(vertices), [](auto pnt) {
    return PosColorUVVertex{pnt->p3_vecPreRot, DeterminePointColor(pnt, true, false, true),
                            tex_array{
                                // tex coord can be default-constructed, because it will ultimately be ignored
                                // because this function is only called when cur_texture_quality == 0, and anytime
                                // that is true, GL_TEXTURE_2D is also disabled
                            }};
  });

  gpu_RenderPolygon(vertices.data(), nv);
}

// Sets the gamma correction value
void rend_SetGammaValue(float val) {
  gpu_preferred_state.gamma = val;
  gRenderer->setGammaCorrection(val);
  LOG_DEBUG.printf("Setting gamma to %f", val);
}

void rend_SetFullScreen(bool fullscreen) {
  if (GSDLWindow) {
    SDL_SetWindowFullscreen(GSDLWindow, fullscreen);
    SDL_SyncWindow(GSDLWindow);
  }

  if (fullscreen) {
    LOG_DEBUG.printf("Entering fullscreen mode");
  } else {
    LOG_DEBUG.printf("Exiting fullscreen mode");
  }
}

bool rend_InitWindowMode() {
  int winArg = FindArgChar("-windowed", 'w');
  int fsArg = FindArgChar("-fullscreen", 'f');

  if ((fsArg) && (winArg)) {
    LOG_FATAL.printf("ERROR: %s AND %s specified!", GameArgs[winArg], GameArgs[fsArg]);
    return false;
  } else if (winArg) {
    // Override default value from database
    Game_fullscreen = false;
  } else if (fsArg) {
    Game_fullscreen = true;
  }
  return true;
}

// Resets the texture cache
void opengl_ResetCache() {
  if (OpenGL_cache_initted) {
    mem_free(OpenGL_lightmap_remap);
    mem_free(OpenGL_bitmap_remap);
    mem_free(OpenGL_lightmap_states);
    mem_free(OpenGL_bitmap_states);
    OpenGL_cache_initted = 0;
  }

  opengl_InitCache();
}

uint8_t opengl_Framebuffer_ready = 0;
chunked_bitmap opengl_Chunked_bitmap;

void opengl_ChangeChunkedBitmap(int bm_handle, chunked_bitmap *chunk) {
  int bw = bm_w(bm_handle, 0);
  int bh = bm_h(bm_handle, 0);

  // determine optimal size of the square bitmaps
  float fopt = 128.0f;
  int iopt;

  // find the smallest dimension and base off that
  int smallest = std::min(bw, bh);

  if (smallest <= 32)
    fopt = 32;
  else if (smallest <= 64)
    fopt = 64;
  else
    fopt = 128;

  iopt = (int)fopt;

  // Get how many pieces we need across and down
  float temp = bw / fopt;
  int how_many_across = temp;
  if ((temp - how_many_across) > 0)
    how_many_across++;

  temp = bh / fopt;
  int how_many_down = temp;
  if ((temp - how_many_down) > 0)
    how_many_down++;

  ASSERT(how_many_across > 0);
  ASSERT(how_many_down > 0);

  // Now go through our big bitmap and partition it into pieces
  uint16_t *src_data = bm_data(bm_handle, 0);
  uint16_t *sdata;
  uint16_t *ddata;

  int shift;
  switch (iopt) {
  case 32:
    shift = 5;
    break;
  case 64:
    shift = 6;
    break;
  case 128:
    shift = 7;
    break;
  default:
    Int3(); // Get Jeff
    break;
  }
  int maxx, maxy;
  int windex, hindex;
  int s_y, s_x, d_y, d_x;

  for (hindex = 0; hindex < how_many_down; hindex++) {
    for (windex = 0; windex < how_many_across; windex++) {
      // loop through the chunks
      // find end x and y
      if (windex < how_many_across - 1)
        maxx = iopt;
      else
        maxx = bw - (windex << shift);
      if (hindex < how_many_down - 1)
        maxy = iopt;
      else
        maxy = bh - (hindex << shift);

      // find the starting source x and y
      s_x = (windex << shift);
      s_y = (hindex << shift);

      // get the pointers pointing to the right spot
      ddata = bm_data(chunk->bm_array[hindex * how_many_across + windex], 0);
      GameBitmaps[chunk->bm_array[hindex * how_many_across + windex]].flags |= BF_CHANGED;
      sdata = &src_data[s_y * bw + s_x];

      // copy the data
      for (d_y = 0; d_y < maxy; d_y++) {
        for (d_x = 0; d_x < maxx; d_x++) {
          ddata[d_x] = sdata[d_x];
        } // end for d_x
        sdata += bw;
        ddata += iopt;
      } // end for d_y

    } // end for windex
  } // end for hindex
}

// Tells the software renderer whether or not to use mipping
void rend_SetMipState(int8_t mipstate) {}

// Init our renderer
int rend_Init(renderer_type state, oeApplication *app, renderer_preferred_state *pref_state) {
  int retval = 0;
#ifndef DEDICATED_ONLY
  rend_SetRendererType(state);
  if (!Renderer_initted) {
    if (!Renderer_close_flag) {
      atexit(rend_Close);
      Renderer_close_flag = 1;
    }

    Renderer_initted = 1;
  }

  LOG_DEBUG.printf("Renderer init is set to %d", Renderer_initted);

#ifndef OEM_V3
  retval = opengl_Init(app, pref_state);
#endif

#endif // #ifdef DEDICATED_ONLY

  return retval;
}

void rend_Close() {
  LOG_DEBUG.printf("CLOSE: Renderer init is set to %d", Renderer_initted);
  if (!Renderer_initted)
    return;

  opengl_Close();
  SDL_GL_UnloadLibrary();

  Renderer_initted = 0;
}

void gpu_BindTexture(int handle, int map_type, int slot) {
  opengl_MakeBitmapCurrent(handle, map_type, slot);
  opengl_MakeWrapTypeCurrent(handle, map_type, slot);
  opengl_MakeFilterTypeCurrent(handle, map_type, slot);
}

void gpu_RenderPolygon(PosColorUVVertex *vData, uint32_t nv) {
  if (gpu_state.cur_texture_quality == 0) {
    // force disable textures
    gRenderer->setTextureEnabled(0, false);
  }

  gRenderer->setTextureEnabled(1, false);

  // draw the data in the arrays
  dglDrawArrays(GL_TRIANGLE_FAN, gRenderer->addVertexData(vData, vData + nv), nv);

  if (gpu_state.cur_texture_quality == 0) {
    // re-enable textures
    gRenderer->setTextureEnabled(0, true);
  }

  OpenGL_polys_drawn++;
  OpenGL_verts_processed += nv;
}

void gpu_RenderPolygonUV2(PosColorUV2Vertex *vData, uint32_t nv) {
  gRenderer->setTextureEnabled(1, true);

  dglDrawArrays(GL_TRIANGLE_FAN, gRenderer->addVertexData(vData, vData + nv), nv);
  OpenGL_polys_drawn++;
  OpenGL_verts_processed += nv;

  CHECK_ERROR(10)
}

void rend_SetFlatColor(ddgr_color color) { gpu_state.cur_color = color; }

// Sets the fog state to TRUE or FALSE
void rend_SetFogState(int8_t state) { gRenderer->setFogEnabled(state); }

// Sets the near and far plane of fog
void rend_SetFogBorders(float nearz, float farz) { gRenderer->setFogBorders(nearz, farz); }

void rend_SetRendererType(renderer_type state) {
  Renderer_type = state;
  LOG_DEBUG.printf("RendererType is set to %d.", state);
}

void rend_SetLighting(light_state state) {
  if (state == gpu_state.cur_light_state)
    return; // No redundant state setting

  dglActiveTexture(GL_TEXTURE0_ARB + 0);

  OpenGL_sets_this_frame[4]++;

  switch (state) {
  case LS_NONE:
    gpu_state.cur_light_state = LS_NONE;
    break;
  case LS_FLAT_GOURAUD:
    gpu_state.cur_light_state = LS_FLAT_GOURAUD;
    break;
  case LS_GOURAUD:
  case LS_PHONG:
    gpu_state.cur_light_state = LS_GOURAUD;
    break;
  default:
    Int3();
    break;
  }

  CHECK_ERROR(13)
}

void rend_SetColorModel(color_model state) {
  switch (state) {
  case CM_MONO:
    gpu_state.cur_color_model = CM_MONO;
    break;
  case CM_RGB:
    gpu_state.cur_color_model = CM_RGB;
    break;
  default:
    Int3();
    break;
  }
}

void rend_SetTextureType(texture_type state) {
  if (state == gpu_state.cur_texture_type)
    return; // No redundant state setting

  dglActiveTexture(GL_TEXTURE0_ARB + 0);
  OpenGL_sets_this_frame[3]++;

  switch (state) {
  case TT_FLAT:
    gRenderer->setTextureEnabled(0, false);
    gpu_state.cur_texture_quality = 0;
    break;
  case TT_LINEAR:
  case TT_LINEAR_SPECIAL:
  case TT_PERSPECTIVE:
  case TT_PERSPECTIVE_SPECIAL:
    gRenderer->setTextureEnabled(0, true);
    gpu_state.cur_texture_quality = 2;
    break;
  default:
    Int3(); // huh? Get Jason
    break;
  }

  CHECK_ERROR(12)
  gpu_state.cur_texture_type = state;
}

void rend_StartFrame(int x1, int y1, int x2, int y2, int clear_flags) {
  if (clear_flags & RF_CLEAR_ZBUFFER) {
    dglClear(GL_DEPTH_BUFFER_BIT);
  }
  gpu_state.clip_x1 = x1;
  gpu_state.clip_y1 = y1;
  gpu_state.clip_x2 = x2;
  gpu_state.clip_y2 = y2;
}

// Flips the screen
void rend_Flip() {
#ifndef RELEASE
  int i;

  RTP_INCRVALUE(texture_uploads, OpenGL_uploads);
  RTP_INCRVALUE(polys_drawn, OpenGL_polys_drawn);

  LOG_VERBOSE.printf("Uploads=%d    Polys=%d   Verts=%d", OpenGL_uploads, OpenGL_polys_drawn, OpenGL_verts_processed);
  LOG_VERBOSE.printf("Sets= 0:%d   1:%d   2:%d   3:%d", OpenGL_sets_this_frame[0], OpenGL_sets_this_frame[1],
                     OpenGL_sets_this_frame[2], OpenGL_sets_this_frame[3]);
  LOG_VERBOSE.printf("Sets= 4:%d   5:%d", OpenGL_sets_this_frame[4], OpenGL_sets_this_frame[5]);
  for (i = 0; i < 10; i++) {
    OpenGL_sets_this_frame[i] = 0;
  }
#endif

  gpu_last_frame_polys_drawn = OpenGL_polys_drawn;
  gpu_last_frame_verts_processed = OpenGL_verts_processed;
  gpu_last_uploaded = OpenGL_uploads;

  OpenGL_uploads = 0;
  OpenGL_polys_drawn = 0;
  OpenGL_verts_processed = 0;

  // if we're rendering to an FBO, scale to the window framebuffer!
  if (GOpenGLFBO != 0) {
    int w, h;
    SDL_GetWindowSizeInPixels(GSDLWindow, &w, &h);

    int scaledHeight, scaledWidth;
    if (w < h) {
      scaledWidth = w;
      scaledHeight = (int)(((((double)GOpenGLFBOHeight) / ((double)GOpenGLFBOWidth))) * ((double)w));
    } else {
      scaledHeight = h;
      scaledWidth = (int)(((((double)GOpenGLFBOWidth) / ((double)GOpenGLFBOHeight))) * ((double)h));
    }

    const int centeredX = (w - scaledWidth) / 2;
    const int centeredY = (h - scaledHeight) / 2;

    dglBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    dglClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    dglClear(GL_COLOR_BUFFER_BIT); // in case the Steam Overlay wrote to places we don't blit over.
    dglBlitFramebuffer(0, 0, GOpenGLFBOWidth, GOpenGLFBOHeight, centeredX, centeredY, centeredX + scaledWidth,
                       centeredY + scaledHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
    dglBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  SDL_GL_SwapWindow(GSDLWindow);

  // go back to drawing on the FBO until we want to blit to the window framebuffer again.
  if (GOpenGLFBO != 0) {
    dglBindFramebuffer(GL_FRAMEBUFFER, GOpenGLFBO);
    dglViewport(0, 0, GOpenGLFBOWidth, GOpenGLFBOHeight);
    dglScissor(0, 0, GOpenGLFBOWidth, GOpenGLFBOHeight);
  }
}

void rend_EndFrame() {}

// Sets the state of z-buffering to on or off
void rend_SetZBufferState(int8_t state) {
  if (state == gpu_state.cur_zbuffer_state)
    return; // No redundant state setting

  OpenGL_sets_this_frame[5]++;
  gpu_state.cur_zbuffer_state = state;

  //	mprintf(0,"OPENGL: Setting zbuffer state to %d.\n",state);

  if (state) {
    dglEnable(GL_DEPTH_TEST);
    dglDepthFunc(GL_LEQUAL);
  } else {
    dglDisable(GL_DEPTH_TEST);
  }

  CHECK_ERROR(14)
}

// Clears the display to a specified color
void rend_ClearScreen(ddgr_color color) {
  int r = (color >> 16 & 0xFF);
  int g = (color >> 8 & 0xFF);
  int b = (color & 0xFF);

  dglClearColor((float)r / 255.0f, (float)g / 255.0f, (float)b / 255.0f, 0);

  dglClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

// Clears the zbuffer for the screen
void rend_ClearZBuffer() { dglClear(GL_DEPTH_BUFFER_BIT); }

// Clears the zbuffer for the screen
void rend_ResetCache() {
  LOG_DEBUG << "Resetting texture cache!";
  opengl_ResetCache();
}

// Fills a rectangle on the display
void rend_FillRect(ddgr_color color, int x1, int y1, int x2, int y2) {
  int r = GR_COLOR_RED(color);
  int g = GR_COLOR_GREEN(color);
  int b = GR_COLOR_BLUE(color);

  int width = x2 - x1;
  int height = y2 - y1;

  x1 += gpu_state.clip_x1;
  y1 += gpu_state.clip_y1;

  dglEnable(GL_SCISSOR_TEST);
  dglScissor(x1, gpu_state.screen_height - (height + y1), width, height);
  dglClearColor((float)r / 255.0f, (float)g / 255.0f, (float)b / 255.0f, 0);
  dglClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  width = gpu_state.clip_x2 - gpu_state.clip_x1;
  height = gpu_state.clip_y2 - gpu_state.clip_y1;

  dglScissor(gpu_state.clip_x1, gpu_state.screen_height - (gpu_state.clip_y1 + height), width, height);
  dglDisable(GL_SCISSOR_TEST);
}

// Sets a pixel on the display
void rend_SetPixel(ddgr_color color, int x, int y) {
  g3_RefreshTransforms(true);

  PosColorUV2Vertex vtx{
      {static_cast<float>(x), static_cast<float>(y), 0},
      {GR_COLOR_RED(color) / 255.0f, GR_COLOR_GREEN(color) / 255.0f, GR_COLOR_BLUE(color) / 255.0f, 1},
      {},
      {}};
  dglDrawArrays(GL_POINTS, gRenderer->addVertexData(&vtx, &vtx + 1), 1);
}

// Sets a pixel on the display
ddgr_color rend_GetPixel(int x, int y) {
  ddgr_color color[4];
  dglReadPixels(x, (gpu_state.screen_height - 1) - y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid *)color);
  return color[0];
}

// Draws a line
void rend_DrawLine(int x1, int y1, int x2, int y2) {
  int8_t atype;
  light_state ltype;
  texture_type ttype;

  g3_RefreshTransforms(true);

  atype = gpu_state.cur_alpha_type;
  ltype = gpu_state.cur_light_state;
  ttype = gpu_state.cur_texture_type;

  rend_SetAlphaType(AT_ALWAYS);
  rend_SetLighting(LS_NONE);
  rend_SetTextureType(TT_FLAT);

  color_array color{
      GR_COLOR_RED(gpu_state.cur_color) / 255.0f,
      GR_COLOR_GREEN(gpu_state.cur_color) / 255.0f,
      GR_COLOR_BLUE(gpu_state.cur_color) / 255.0f,
  };
  std::array<PosColorUV2Vertex, 2> vertices{
      PosColorUV2Vertex{
          {static_cast<float>(x1 + gpu_state.clip_x1), static_cast<float>(y1 + gpu_state.clip_y1), 0}, color, {}, {}},
      PosColorUV2Vertex{
          {static_cast<float>(x2 + gpu_state.clip_x1), static_cast<float>(y2 + gpu_state.clip_y1), 0}, color, {}, {}}};

  dglDrawArrays(GL_LINES, gRenderer->addVertexData(vertices.begin(), vertices.end()), vertices.size());

  rend_SetAlphaType(atype);
  rend_SetLighting(ltype);
  rend_SetTextureType(ttype);
}

// Sets the color of fog
void rend_SetFogColor(ddgr_color color) { gRenderer->setFogColor(color); }

void rend_SetAlphaType(int8_t atype) {
  if (atype == gpu_state.cur_alpha_type)
    return; // don't set it redundantly

  dglActiveTexture(GL_TEXTURE0_ARB + 0);
  OpenGL_sets_this_frame[6]++;

  if (atype == AT_ALWAYS) {
    if (opengl_Blending_on) {
      dglDisable(GL_BLEND);
      opengl_Blending_on = false;
    }
  } else {
    if (!opengl_Blending_on) {
      dglEnable(GL_BLEND);
      opengl_Blending_on = true;
    }
  }

  switch (atype) {
  case AT_ALWAYS:
  case AT_TEXTURE:
    rend_SetAlphaValue(255);
    dglBlendFunc(GL_ONE, GL_ZERO);
    break;
  case AT_CONSTANT:
  case AT_CONSTANT_TEXTURE:
  case AT_VERTEX:
  case AT_CONSTANT_TEXTURE_VERTEX:
  case AT_CONSTANT_VERTEX:
  case AT_TEXTURE_VERTEX:
    dglBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    break;
  case AT_LIGHTMAP_BLEND:
    dglBlendFunc(GL_DST_COLOR, GL_ZERO);
    break;
  case AT_SATURATE_TEXTURE:
  case AT_LIGHTMAP_BLEND_SATURATE:
  case AT_SATURATE_VERTEX:
  case AT_SATURATE_CONSTANT_VERTEX:
  case AT_SATURATE_TEXTURE_VERTEX:
    dglBlendFunc(GL_SRC_ALPHA, GL_ONE);
    break;
  case AT_SPECULAR:
    break;
  default:
    Int3(); // no type defined,get jason
    break;
  }
  gpu_state.cur_alpha_type = atype;
  gpu_Alpha_multiplier = rend_GetAlphaMultiplier();
  CHECK_ERROR(15)
}

// Draws a line using the states of the renderer
void rend_DrawSpecialLine(g3Point *p0, g3Point *p1) {
  g3_RefreshTransforms(true);

  std::array<g3Point const *, 2> pts{p0, p1};
  std::array<PosColorUV2Vertex, 2> vertices{};
  std::transform(pts.begin(), pts.end(), vertices.begin(), [](auto pnt) {
    return PosColorUV2Vertex{{pnt->p3_sx + gpu_state.clip_x1, pnt->p3_sy + gpu_state.clip_y1,
                              -std::clamp(1.0f - (1.0f / (pnt->p3_z + Z_bias)), 0.0f, 1.0f)},
                             DeterminePointColor(pnt, false, false, true), // extras??
                             {},
                             {}};
  });

  dglDrawArrays(GL_LINES, gRenderer->addVertexData(vertices.begin(), vertices.end()), vertices.size());
}

// Takes a screenshot of the current frame and puts it into the handle passed
std::unique_ptr<NewBitmap> rend_Screenshot() {
  auto result =
      std::make_unique<NewBitmap>(gpu_state.screen_width, gpu_state.screen_height, PixelDataFormat::RGBA32, true);

  if (!result || result->getData() == nullptr) {
    return nullptr;
  }

  dglReadPixels(0, 0, gpu_state.screen_width, gpu_state.screen_height, GL_RGBA, GL_UNSIGNED_BYTE,
                (GLvoid *)result->getData());

  return result;
}

// Takes a screenshot of the current frame and puts it into the handle passed
void rend_Screenshot(int bm_handle) {
  auto screenshot = rend_Screenshot();
  auto *temp_data = reinterpret_cast<uint32_t *>(screenshot->getData());

  uint32_t w, h;
  screenshot->getSize(w, h);

  ASSERT((bm_w(bm_handle, 0)) == gpu_state.screen_width);
  ASSERT((bm_h(bm_handle, 0)) == gpu_state.screen_height);

  uint16_t *dest_data = bm_data(bm_handle, 0);

  for (std::size_t i = 0; i < h; i++) {
    for (std::size_t t = 0; t < w; t++) {
      uint32_t spix = temp_data[i * w + t];

      int r = spix & 0xff;
      int g = (spix >> 8) & 0xff;
      int b = (spix >> 16) & 0xff;

      dest_data[(((h - 1) - i) * w) + t] = GR_RGB16(r, g, b);
    }
  }
}

// Enables/disables writes the depth buffer
void rend_SetZBufferWriteMask(int state) {
  OpenGL_sets_this_frame[5]++;
  if (state) {
    dglDepthMask(GL_TRUE);
  } else {
    dglDepthMask(GL_FALSE);
  }
}

int rend_ReInit() {
  opengl_Close(true);
  return opengl_Init(nullptr, &gpu_preferred_state);
}

// Takes a bitmap and blits it to the screen using linear frame buffer stuff
// X and Y are the destination X,Y
void rend_CopyBitmapToFramebuffer(int bm_handle, int x, int y) {
  ASSERT(opengl_Framebuffer_ready);

  if (opengl_Framebuffer_ready == 1) {
    bm_CreateChunkedBitmap(bm_handle, &opengl_Chunked_bitmap);
    opengl_Framebuffer_ready = 2;
  } else {
    opengl_ChangeChunkedBitmap(bm_handle, &opengl_Chunked_bitmap);
  }

  rend_DrawChunkedBitmap(&opengl_Chunked_bitmap, 0, 0, 255);
}

// Gets a renderer ready for a framebuffer copy, or stops a framebuffer copy
void rend_SetFrameBufferCopyState(bool state) {
  if (state) {
    ASSERT(opengl_Framebuffer_ready == 0);
    opengl_Framebuffer_ready = 1;
  } else {
    ASSERT(opengl_Framebuffer_ready != 0);
    opengl_Framebuffer_ready = 0;

    if (opengl_Framebuffer_ready == 2) {
      bm_DestroyChunkedBitmap(&opengl_Chunked_bitmap);
      opengl_ResetCache();
    }
  }
}

// Gets OpenGL ready to work in a window
int rend_InitOpenGLWindow(oeApplication *app, renderer_preferred_state *pref_state) {
  return opengl_Init(app, pref_state);
}

// Shuts down OpenGL in a window
void rend_CloseOpenGLWindow() {
  opengl_Close();
  LOG_DEBUG << "SHUTTING DOWN WINDOWED OPENGL!";
}

// Sets the hardware bias level for coplanar polygons
// This helps reduce z buffer artifacts
void rend_SetCoplanarPolygonOffset(float factor) {
  if (factor == 0.0f) {
    dglDisable(GL_POLYGON_OFFSET_FILL);
  } else {
    dglEnable(GL_POLYGON_OFFSET_FILL);
    dglPolygonOffset(-1.0f, -1.0f);
  }
}

// returns the direct draw object
void *rend_RetrieveDirectDrawObj(void **frontsurf, void **backsurf) {
  *frontsurf = nullptr;
  *backsurf = nullptr;
  return nullptr;
}

void rend_TransformSetToPassthru() {
  int width = gpu_state.screen_width;
  int height = gpu_state.screen_height;

  // TODO: Generalize
  gRenderer->setTransform(glm::mat4x4{1}, glm::mat4x4{1}, glm::ortho<float>(0, width, height, 0, 0, 1));

  // Viewport
  dglViewport(0, 0, width, height);
  dglScissor(0, 0, width, height);
}

void rend_TransformSetViewport(int lx, int ty, int width, int height) {
  dglViewport(lx, gpu_state.screen_height - (ty + height - 1), width, height);
}

void rend_TransformSetProjection(float trans[4][4]) {
  gRenderer->setTransform(std::nullopt, std::nullopt, glm::make_mat4x4(&trans[0][0]));
}

void rend_TransformSetModelView(float trans[4][4]) {
  gRenderer->setTransform(glm::make_mat4x4(&trans[0][0]), glm::mat4x4{1}, std::nullopt);
}
