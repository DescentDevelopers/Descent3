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

#include "RendererConfig.h"
#ifdef USE_SOFTWARE_TNL

#if defined(WIN32)
#include <windows.h>
#include "ddraw.h"
#elif defined(__LINUX__)
#include "linux/linux_fix.h"
#include "linux/dyna_xext.h"
#include "lnxscreenmode.h"
#include <X11/Xatom.h>
#else
#endif

#include "DDAccess.h"
#include "pstypes.h"
#include "pserror.h"
#include "mono.h"
#include "3d.h"
#include "renderer.h"
#include "ddvid.h"
#include "ddio.h"
#include "application.h"
#include "bitmap.h"
#include "lightmap.h"
#include "rend_opengl.h"
#include "grdefs.h"
#include "mem.h"
#include "rtperformance.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <gl/gl.h>
#include <gl/glu.h>

#define DECLARE_OPENGL
#include "dyna_gl.h"

#if defined(WIN32)
#include "win/arb_extensions.h"
#endif

#include <algorithm>

int FindArg(char *);
void rend_SetLightingState(light_state state);

int OpenGL_window_initted = 0;

// The font characteristics
static float rend_FontRed[4], rend_FontBlue[4], rend_FontGreen[4], rend_FontAlpha[4];
char Renderer_error_message[256] = "Generic renderer error";
int Triangles_drawn = 0;
bool UseHardware = 0;
bool NoLightmaps = 0;
bool StateLimited = 0;
bool UseMultitexture = 0;
bool UseWBuffer = 0;
int Overlay_map = -1;
int Bump_map = 0, Bumpmap_ready = 0;
ubyte Overlay_type = OT_NONE;
float Z_bias = 0.0;
ubyte Renderer_close_flag = 0, Renderer_initted = 0;
// Is this hardware or software rendered?
renderer_type Renderer_type = RENDERER_SOFTWARE_16BIT;
int WindowGL = 0;

extern matrix Unscaled_matrix;
extern vector View_position;

#define CHECK_ERROR(x)

#if defined(WIN32)
//	Moved from DDGR library
static HWND hOpenGLWnd = NULL;
static HDC hOpenGLDC = NULL;
HGLRC ResourceContext;
static WORD Saved_gamma_values[256 * 3];
#elif defined(__LINUX__)
static Display *OpenGL_Display = NULL;
static Window OpenGL_Window;
static XVisualInfo OpenGL_VisualInfo;
static GLXContext OpenGL_Context;
static bool OpenGL_TextureHack = false;
static bool OpenGL_UseLists = false;
static oeLnxApplication *OpenGL_LinuxApp = NULL;
#define glXQueryExtension dglXQueryExtension
#define glXCreateContext dglXCreateContext
#define glXMakeCurrent dglXMakeCurrent
#define glXSwapBuffers dglXSwapBuffers
#define glXDestroyContext dglXDestroyContext
#define glXWaitGL dglXWaitGL
#else
#endif

#define GET_WRAP_STATE(x) (x >> 4)
#define GET_FILTER_STATE(x) (x & 0x0f)

#define SET_WRAP_STATE(x, s)                                                                                           \
  {                                                                                                                    \
    x &= 0x0F;                                                                                                         \
    x |= (s << 4);                                                                                                     \
  }
#define SET_FILTER_STATE(x, s)                                                                                         \
  {                                                                                                                    \
    x &= 0xF0;                                                                                                         \
    x |= (s);                                                                                                          \
  }

//	OpenGL Stuff
#define UNSIGNED_SHORT_5_5_5_1_EXT 0x8034
#define UNSIGNED_SHORT_4_4_4_4_EXT 0x8033
static int OpenGL_polys_drawn = 0;
static int OpenGL_verts_processed = 0;
static int OpenGL_uploads = 0;
static int OpenGL_sets_this_frame[10];
static int OpenGL_packed_pixels = 0;
static int OpenGL_multitexture = 0;
static int Cur_texture_object_num = 1;
static int OpenGL_cache_initted = 0;
static int OpenGL_last_bound[2];
static int Last_texel_unit_set = -1;
static int OpenGL_last_frame_polys_drawn = 0;
static int OpenGL_last_frame_verts_processed = 0;
static int OpenGL_last_uploaded = 0;
static float OpenGL_Alpha_factor = 1.0f;

#ifndef RELEASE
// This is for the Microsoft OpenGL reference driver
// Setting this will turn off bilinear filtering and zbuffer so we can get decent
// framerates to discern driver problems
static ubyte Fast_test_render = 0;
#endif

#if defined(WIN32)
PFNGLACTIVETEXTUREARBPROC oglActiveTextureARB;
PFNGLCLIENTACTIVETEXTUREARBPROC oglClientActiveTextureARB;
PFNGLMULTITEXCOORD4FARBPROC oglMultiTexCoord4f;
#endif

ushort *OpenGL_bitmap_remap;
ushort *OpenGL_lightmap_remap;
ubyte *OpenGL_bitmap_states;
ubyte *OpenGL_lightmap_states;

uint *opengl_Upload_data = NULL;
uint *opengl_Translate_table = NULL;
uint *opengl_4444_translate_table = NULL;

ushort *opengl_packed_Upload_data = NULL;
ushort *opengl_packed_Translate_table = NULL;
ushort *opengl_packed_4444_translate_table = NULL;

rendering_state OpenGL_state;
static float Alpha_multiplier = 1.0;

renderer_preferred_state OpenGL_preferred_state = {0, 1, 1.5};

// These structs are for drawing with OpenGL vertex arrays
// Useful for fast indexing
typedef struct {
  float r, g, b, a;
} color_array;

typedef struct {
  float s, t, r, w;
} tex_array;

vector GL_verts[100];
color_array GL_colors[100];
tex_array GL_tex_coords[100];
tex_array GL_tex_coords2[100];

bool OpenGL_multitexture_state = false;
module *OpenGLDLLHandle = NULL;
int Already_loaded = 0;
bool opengl_Blending_on = 0;

static oeApplication *ParentApplication;

// Sets up multi-texturing using ARB extensions
void opengl_GetDLLFunctions(void) {
#if defined(WIN32)
  oglActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC)dwglGetProcAddress("glActiveTextureARB");
  if (!oglActiveTextureARB)
    goto dll_error;

  oglClientActiveTextureARB = (PFNGLCLIENTACTIVETEXTUREARBPROC)dwglGetProcAddress("glClientActiveTextureARB");
  if (!oglClientActiveTextureARB)
    goto dll_error;

  oglMultiTexCoord4f = (PFNGLMULTITEXCOORD4FARBPROC)dwglGetProcAddress("glMultiTexCoord4f");
  if (!oglMultiTexCoord4f)
    goto dll_error;

  return;

dll_error:
#endif
  OpenGL_multitexture = 0;
}

// returns true if the passed in extension name is supported
int opengl_CheckExtension(char *extName) {
  char *p = (char *)dglGetString(GL_EXTENSIONS);
  int extNameLen = strlen(extName);
  char *end = p + strlen(p);

  while (p < end) {
    int n = strcspn(p, " ");
    if ((extNameLen == n) && (strncmp(extName, p, n) == 0))
      return 1;

    p += (n + 1);
  }

  return 0;
}

// Gets some specific information about this particular flavor of opengl
void opengl_GetInformation() {
  mprintf((0, "OpenGL Vendor: %s\n", dglGetString(GL_VENDOR)));
  mprintf((0, "OpenGL Renderer: %s\n", dglGetString(GL_RENDERER)));
  mprintf((0, "OpenGL Version: %s\n", dglGetString(GL_VERSION)));
  mprintf((0, "OpenGL Extensions: %s\n", dglGetString(GL_EXTENSIONS)));

  /*
  #ifndef RELEASE
  // If this is the microsoft driver, then make stuff go faster
  const ubyte *renderer=dglGetString(GL_RENDERER);
  if (!(strnicmp ((const char *)renderer,"GDI",3)))
          Fast_test_render=1;
  else
          Fast_test_render=0;
  #endif
  */
}

int opengl_MakeTextureObject(int tn) {
  int num = Cur_texture_object_num;

  Cur_texture_object_num++;

  if (OpenGL_multitexture && Last_texel_unit_set != tn) {
#if defined(WIN32)
    oglActiveTextureARB(GL_TEXTURE0_ARB + tn);
    Last_texel_unit_set = tn;
#endif
  }

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

int opengl_InitCache(void) {
  OpenGL_bitmap_remap = (ushort *)mem_malloc(MAX_BITMAPS * 2);
  ASSERT(OpenGL_bitmap_remap);
  OpenGL_lightmap_remap = (ushort *)mem_malloc(MAX_LIGHTMAPS * 2);
  ASSERT(OpenGL_lightmap_remap);

  OpenGL_bitmap_states = (ubyte *)mem_malloc(MAX_BITMAPS);
  ASSERT(OpenGL_bitmap_states);
  OpenGL_lightmap_states = (ubyte *)mem_malloc(MAX_LIGHTMAPS);
  ASSERT(OpenGL_lightmap_states);

  Cur_texture_object_num = 1;

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

  dglTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

  if (OpenGL_multitexture) {
#if defined(WIN32)
    oglActiveTextureARB(GL_TEXTURE0_ARB + 1);
    dglTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    oglActiveTextureARB(GL_TEXTURE0_ARB + 0);
#endif
  }

  CHECK_ERROR(3)

  OpenGL_cache_initted = 1;
  return 1;
}

// Sets default states for our renderer
void opengl_SetDefaults() {
  mprintf((0, "Setting states\n"));

  OpenGL_state.cur_color = 0x00FFFFFF;
  OpenGL_state.cur_bilinear_state = -1;
  OpenGL_state.cur_zbuffer_state = -1;
  OpenGL_state.cur_texture_quality = -1;
  OpenGL_state.cur_light_state = LS_GOURAUD;
  OpenGL_state.cur_color_model = CM_MONO;
  OpenGL_state.cur_bilinear_state = -1;
  OpenGL_state.cur_alpha_type = AT_TEXTURE;

  // Enable some states
  dglAlphaFunc(GL_GREATER, 0);
  dglEnable(GL_ALPHA_TEST);
  dglEnable(GL_BLEND);
  dglEnable(GL_DITHER);
  opengl_Blending_on = true;

#ifndef RELEASE
  if (Fast_test_render)
    dglDisable(GL_DITHER);
#endif

  rend_SetAlphaType(AT_ALWAYS);
  rend_SetAlphaValue(255);
  rend_SetFiltering(1);
  rend_SetLightingState(LS_NONE);
  rend_SetTextureType(TT_FLAT);
  rend_SetColorModel(CM_RGB);
  rend_SetZBufferState(1);
  rend_SetZValues(0, 3000);
  opengl_SetGammaValue(OpenGL_preferred_state.gamma);
  OpenGL_last_bound[0] = 9999999;
  OpenGL_last_bound[1] = 9999999;
  Last_texel_unit_set = -1;
  OpenGL_multitexture_state = false;

#ifdef __LINUX__
  OpenGL_UseLists = (FindArg("-gllists")) ? true : false;
  if (OpenGL_UseLists) {
#endif
    dglEnableClientState(GL_VERTEX_ARRAY);
    dglEnableClientState(GL_COLOR_ARRAY);
    dglEnableClientState(GL_TEXTURE_COORD_ARRAY);

    dglVertexPointer(3, GL_FLOAT, 0, GL_verts);
    dglColorPointer(4, GL_FLOAT, 0, GL_colors);
    dglTexCoordPointer(4, GL_FLOAT, 0, GL_tex_coords);
#ifdef __LINUX__
  }
#endif

  dglHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
  dglHint(GL_FOG_HINT, GL_FASTEST);
  dglEnable(GL_SCISSOR_TEST);
  dglScissor(0, 0, OpenGL_state.screen_width, OpenGL_state.screen_height);
  dglDisable(GL_SCISSOR_TEST);
  dglDepthRange(0.0f, 1.0f);

  if (OpenGL_multitexture) {
#if defined(WIN32)
    oglActiveTextureARB(GL_TEXTURE0_ARB + 1);
    oglClientActiveTextureARB(GL_TEXTURE0_ARB + 1);
    dglEnableClientState(GL_TEXTURE_COORD_ARRAY);
    dglTexCoordPointer(4, GL_FLOAT, 0, GL_tex_coords2);
    dglHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    dglHint(GL_FOG_HINT, GL_FASTEST);

    oglClientActiveTextureARB(GL_TEXTURE0_ARB + 0);

    dglDisable(GL_TEXTURE_2D);
    dglAlphaFunc(GL_GREATER, 0);
    dglEnable(GL_ALPHA_TEST);
    dglEnable(GL_BLEND);
    dglEnable(GL_DITHER);
    dglBlendFunc(GL_DST_COLOR, GL_ZERO);
    oglActiveTextureARB(GL_TEXTURE0_ARB + 0);
#endif
  }
}

#if defined(WIN32)
// Check for OpenGL support,
int opengl_Setup(HDC glhdc) {
  if (!Already_loaded) {
    if (!(OpenGLDLLHandle = LoadOpenGLDLL("opengl32.dll"))) {
      rend_SetErrorMessage("Failed to load opengl dll!\n");
      Int3();
      return 0;
    }
  }

  // Finds an acceptable pixel format to render to
  PIXELFORMATDESCRIPTOR pfd, pfd_copy;
  int pf;

  memset(&pfd, 0, sizeof(pfd));
  pfd.nSize = sizeof(pfd);
  pfd.nVersion = 1;
  pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | PFD_GENERIC_ACCELERATED;
  pfd.iPixelType = PFD_TYPE_RGBA;

  /*if (!WindowGL)
  {
  if (OpenGL_preferred_state.bit_depth==32)
  {
  pfd.cColorBits   = 32;
  pfd.cDepthBits   = 32;
  }
  else
  {
  pfd.cColorBits   = OpenGL_preferred_state.bit_depth;
  pfd.cDepthBits   =OpenGL_preferred_state.bit_depth;
  }

  pfd.cColorBits   = 16;
  pfd.cDepthBits   =16;

  }
  else
  {
  pfd.cColorBits   = 16;
  pfd.cDepthBits   =16;
  }*/

  // Find the user's "best match" PFD
  pf = ChoosePixelFormat(glhdc, &pfd);
  if (pf == 0) {
    Int3();
    // FreeLibrary(opengl_dll_handle);
    return NULL;
  }

  mprintf((0, "Choose pixel format successful!\n"));

  // Try and set the new PFD
  if (SetPixelFormat(glhdc, pf, &pfd) == FALSE) {
    DWORD ret = GetLastError();
    Int3();
    // FreeLibrary(opengl_dll_handle);
    return NULL;
  }

  mprintf((0, "SetPixelFormat successful!\n"));

  // Get a copy of the newly set PFD
  if (DescribePixelFormat(glhdc, pf, sizeof(PIXELFORMATDESCRIPTOR), &pfd_copy) == 0) {
    Int3();
    // FreeLibrary(opengl_dll_handle);
    return NULL;
  }

  // Check the returned PFD to see if it is hardware accelerated
  if ((pfd_copy.dwFlags & PFD_GENERIC_ACCELERATED) == 0 && (pfd_copy.dwFlags & PFD_GENERIC_FORMAT) != 0) {
    Int3();
    // FreeLibrary(opengl_dll_handle);
    return NULL;
  }

  // Create an OpenGL context, and make it the current context
  ResourceContext = dwglCreateContext((HDC)glhdc);
  if (ResourceContext == NULL) {
    DWORD ret = GetLastError();
    // FreeLibrary(opengl_dll_handle);
    Int3();
    return NULL;
  }

  ASSERT(ResourceContext != NULL);
  mprintf((0, "Making context current\n"));
  dwglMakeCurrent((HDC)glhdc, ResourceContext);

  Already_loaded = 1;

  return 1;
}
#elif defined(__LINUX__)
bool opengl_GetXConfig(Display *dpy, XVisualInfo *vis, int attrib, int *value) {
  int res;

  res = dglXGetConfig(dpy, vis, attrib, value);

  if (res == 0)
    return true;

  switch (res) {
  case GLX_NO_EXTENSION:
    fprintf(stdout, "GLX: Display does not support the GLX extension (%d)\n", attrib);
    break;
  case GLX_BAD_SCREEN:
    fprintf(stdout, "GLX: The screen of the vis does not correspond to a screen (%d)\n", attrib);
    break;
  case GLX_BAD_ATTRIBUTE:
    fprintf(stdout, "GLX: Bad Attribute (%d\%d)\n", attrib, *value);
    break;
  case GLX_BAD_VISUAL:
    fprintf(stdout, "GLX: Bad Visual on non GLX_USE_GL call (%d)\n", attrib);
    break;
  default:
    fprintf(stdout, "GLX: Unknown error (%d)\n", attrib);
    break;
  }

  return false;
}

int opengl_Setup(oeApplication *app, int *width, int *height) {
  if (!Already_loaded) {
#define MAX_ARGS 30
#define MAX_CHARS_PER_ARG 100
    extern char GameArgs[MAX_ARGS][MAX_CHARS_PER_ARG];

    char gl_library[256];
    int arg;
    arg = FindArg("-gllibrary");
    if (arg != 0) {
      strcpy(gl_library, GameArgs[arg + 1]);
      fprintf(stdout, "OpenGL: Loading %s library\n", gl_library);
    } else {
      strcpy(gl_library, "libGL.so");
    }
    if (!(OpenGLDLLHandle = LoadOpenGLDLL(gl_library))) {
      char buffer[512];
      snprintf(buffer, sizeof(buffer), "Failed to load %s!\n", gl_library);
      fprintf(stderr, buffer);
      rend_SetErrorMessage(buffer);
      Int3();
      return 0;
    }
  }

  int dummy;

  OpenGL_TextureHack = (FindArg("-gltexturehack")) ? true : false;

  if (app) {
    tLnxAppInfo app_info;
    OpenGL_LinuxApp = (oeLnxApplication *)app;

    app->get_info((void *)&app_info);

    OpenGL_Display = app_info.m_Display;
    OpenGL_Window = app_info.m_window;
    memcpy(&OpenGL_VisualInfo, &app_info.m_VisualInfo, sizeof(XVisualInfo));
  } else {
    // since the application object wasn't passed in, make sure
    // this isn't the first init
    if (!OpenGL_Display) {
      // ACK!
      fprintf(stdout, "OGL: Error in opengl_Setup().  Application object not specified\n");
      return 0;
    }
  }

  // we need to destroy the OpenGL window
  XUnmapWindow(OpenGL_Display, OpenGL_Window);
  XDestroyWindow(OpenGL_Display, OpenGL_Window);

  // Make sure OpenGL's GLX extension is supported.  The glXQueryExtension also returns
  // the GLX extension's error base and event base.  For almost all OpenGL programs,
  // this information is irrelevant.
  if (!glXQueryExtension(OpenGL_Display, &dummy, &dummy)) {
    fprintf(stderr, "GLX extension not supported by Display\n");
    Int3();
    return 0;
  }

  // Choose our visual
  int screen_num = DefaultScreen(OpenGL_Display);
  int vis_attrib[] = {GLX_RGBA, GLX_RED_SIZE,   1,  GLX_GREEN_SIZE,   1,   GLX_BLUE_SIZE,
                      1,        GLX_DEPTH_SIZE, 16, GLX_DOUBLEBUFFER, None};
  XVisualInfo *new_vis = dglXChooseVisual(OpenGL_Display, screen_num, vis_attrib);
  if (!new_vis) {
    fprintf(stdout, "OpenGL: glXChooseVisual returned NULL\n");
    Int3();
    return 0;
  }

  // Create an OpenGL rendering context
  OpenGL_Context = glXCreateContext(OpenGL_Display, new_vis, None, True);
  if (OpenGL_Context == NULL) {
    fprintf(stderr, "OpenGL: Unable to create GLX Context\n");
    Int3();
    return 0;
  }

  // Create a new window
  XSetWindowAttributes swa;
  swa.override_redirect = true;
  swa.border_pixel = 0;
  swa.event_mask = ExposureMask | StructureNotifyMask | KeyPressMask | KeyReleaseMask | PointerMotionMask |
                   ButtonPressMask | ButtonReleaseMask;

  OpenGL_Window =
      XCreateWindow(OpenGL_Display, RootWindow(OpenGL_Display, OpenGL_VisualInfo.screen), 0, 0, *width, *height, 0,
                    OpenGL_VisualInfo.depth, InputOutput, OpenGL_VisualInfo.visual, CWBorderPixel | CWEventMask, &swa);

  XSizeHints sizeHints = {0};
  sizeHints.flags |= USSize | USPosition | PAspect;
  sizeHints.width = *width;
  sizeHints.height = *height;
  sizeHints.x = 0;
  sizeHints.y = 0;
  sizeHints.min_aspect.x = sizeHints.max_aspect.x = *width;
  sizeHints.min_aspect.y = sizeHints.max_aspect.y = *height;

  char *argv[1];
  XWMHints *wmHints;
  Atom wmDeleteWindow;

  argv[0] = strdup("opengl");

  XSetStandardProperties(OpenGL_Display, OpenGL_Window, "", "", None, (char **)argv, 0, &sizeHints);
  free(argv[0]);

  wmHints = XAllocWMHints();
  wmHints->initial_state = NormalState;
  wmHints->flags = StateHint;

  XSetWMHints(OpenGL_Display, OpenGL_Window, wmHints);
  wmDeleteWindow = XInternAtom(OpenGL_Display, "WM_DELETE_WINDOW", False);
  XSetWMProtocols(OpenGL_Display, OpenGL_Window, &wmDeleteWindow, 1);

  // move and resize the application window
  XMoveResizeWindow(OpenGL_Display, OpenGL_Window, 0, 0, *width, *height);

  OpenGL_LinuxApp->set_sizepos(0, 0, *width, *height);
  OpenGL_LinuxApp->set_windowinfo(OpenGL_Display, OpenGL_Window);
  OpenGL_LinuxApp->hide_mouse();
  OpenGL_LinuxApp->clear_window();

  //	warp the mouse to 0,0 to start so our screen is in full view
  XWarpPointer(OpenGL_Display, None, OpenGL_Window, 0, 0, 0, 0, *width / 2, *height / 2);

  XStoreName(OpenGL_Display, OpenGL_Window, OpenGL_LinuxApp->get_window_name());
  XSetIconName(OpenGL_Display, OpenGL_Window, OpenGL_LinuxApp->get_window_name());

  XMapWindow(OpenGL_Display, OpenGL_Window);

  bool wait_for_draw = false;
  XEvent event;
  while (!wait_for_draw) {
    XNextEvent(OpenGL_Display, &event);
    if (event.type == Expose && !event.xexpose.count)
      wait_for_draw = true;
  }

  XRaiseWindow(OpenGL_Display, OpenGL_Window);

  // Bind the rendering context
  glXMakeCurrent(OpenGL_Display, OpenGL_Window, OpenGL_Context);

  XSync(OpenGL_Display, False);

  Already_loaded = 1;
  return 1;
}
#endif

// Sets up our OpenGL rendering context
// Returns 1 if ok, 0 if something bad
int opengl_Init(oeApplication *app, renderer_preferred_state *pref_state) {
  int width, height;
  int retval = 1;
  int i;

  mprintf((0, "Setting up opengl mode!\n"));

  if (pref_state) {
    OpenGL_preferred_state = *pref_state;
  }

  int windowX = 0, windowY = 0;
#if defined(WIN32)
  /***********************************************************
   *               WINDOWS OPENGL
   ***********************************************************
   */
  static HWND hwnd;

  if (app != NULL) {
    ParentApplication = app;
    hwnd = (HWND)((oeWin32Application *)app)->m_hWnd;
  }

  if (!WindowGL) {
    // First set our display mode
    // Create direct draw surface

    DEVMODE devmode;

    devmode.dmSize = sizeof(devmode);
    devmode.dmBitsPerPel = 16;
    // devmode.dmBitsPerPel=OpenGL_preferred_state.bit_depth;
    devmode.dmPelsWidth = OpenGL_preferred_state.width;
    devmode.dmPelsHeight = OpenGL_preferred_state.height;
    devmode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

    int retval = ChangeDisplaySettings(&devmode, 0);

    if (retval != DISP_CHANGE_SUCCESSFUL) {
      mprintf((0, "Display mode change failed (err=%d), trying default!\n", retval));
      retval = -1;
      devmode.dmBitsPerPel = 16;
      devmode.dmPelsWidth = 640;
      devmode.dmPelsHeight = 480;
      devmode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

      retval = ChangeDisplaySettings(&devmode, 0);
      if (retval != DISP_CHANGE_SUCCESSFUL) {
        mprintf((0, "OpenGL_INIT:Change display setting failed failed!\n"));
        rend_SetErrorMessage("OGL: ChangeDisplaySettings failed.  Make sure your desktop is set to 16bit mode!");
        ChangeDisplaySettings(NULL, 0);
        goto D3DError;
      } else {
        OpenGL_preferred_state.bit_depth = 16;
        OpenGL_preferred_state.width = 640;
        OpenGL_preferred_state.height = 480;
      }
    } else
      mprintf((0, "Setdisplaymode to %d x %d (%d bits) is successful!\n", OpenGL_preferred_state.width,
               OpenGL_preferred_state.height, OpenGL_preferred_state.bit_depth));
  }

  memset(&OpenGL_state, 0, sizeof(rendering_state));

  //	These values are set here - samir
  if (app != NULL) {
    hOpenGLWnd = (HWND)((oeWin32Application *)app)->m_hWnd;
  }

  hOpenGLDC = GetDC(hOpenGLWnd);

  if (WindowGL) {
    RECT rect;
    POINT topLeft;
    GetClientRect((HWND)hOpenGLWnd, &rect);

    topLeft.x = rect.left;
    topLeft.y = rect.top;
    ClientToScreen((HWND)hOpenGLWnd, &topLeft);

    width = rect.right - rect.left + 1;
    height = rect.bottom - rect.top + 1;
    windowX = topLeft.x;
    windowY = topLeft.y;
  } else {
    SetWindowPos(hOpenGLWnd, HWND_TOPMOST, 0, 0, OpenGL_preferred_state.width, OpenGL_preferred_state.height,
                 SWP_FRAMECHANGED);
    width = OpenGL_preferred_state.width;
    height = OpenGL_preferred_state.height;
    RECT rect;
    GetWindowRect((HWND)hOpenGLWnd, &rect);
    mprintf((0, "rect=%d %d %d %d\n", rect.top, rect.right, rect.bottom, rect.left));
  }

  OpenGL_state.screen_width = width;
  OpenGL_state.screen_height = height;

  if (!opengl_Setup(hOpenGLDC))
    goto D3DError;

  // Save gamma values for later
  GetDeviceGammaRamp(hOpenGLDC, (LPVOID)Saved_gamma_values);

#elif defined(__LINUX__)
  /***********************************************************
   *               LINUX OPENGL
   ***********************************************************
   */
  // Setup OpenGL_state.screen_width & OpenGL_state.screen_height & width & height
  width = OpenGL_preferred_state.width;
  height = OpenGL_preferred_state.height;

  if (!opengl_Setup(app, &width, &height))
    goto D3DError;

  memset(&OpenGL_state, 0, sizeof(rendering_state));
  OpenGL_state.screen_width = width;
  OpenGL_state.screen_height = height;
#else
  // Setup OpenGL_state.screen_width & OpenGL_state.screen_height & width & height

#endif
  // Get some info
  opengl_GetInformation();

  mprintf((0, "Setting up projection matrix\n"));

  dglMatrixMode(GL_PROJECTION);
  dglLoadIdentity();
  dglOrtho((GLfloat)0.0f, (GLfloat)(width), (GLfloat)(height), (GLfloat)0.0f, 0.0f, 1.0f);
  dglViewport(0, 0, width, height);
  dglMatrixMode(GL_MODELVIEW);
  dglLoadIdentity();

  opengl_InitCache();

  OpenGL_multitexture = opengl_CheckExtension("GL_ARB_multitexture");

  if (!OpenGL_multitexture)
    OpenGL_multitexture = opengl_CheckExtension("GL_SGIS_multitexture");

  OpenGL_packed_pixels = opengl_CheckExtension("GL_EXT_packed_pixels");

  opengl_GetDLLFunctions();

  if (FindArg("-NoMultitexture"))
    OpenGL_multitexture = false;

  if (OpenGL_packed_pixels) {
    opengl_packed_Upload_data = (ushort *)mem_malloc(2048 * 2048 * 2);
    opengl_packed_Translate_table = (ushort *)mem_malloc(65536 * 2);
    opengl_packed_4444_translate_table = (ushort *)mem_malloc(65536 * 2);

    ASSERT(opengl_packed_Upload_data);
    ASSERT(opengl_packed_Translate_table);
    ASSERT(opengl_packed_4444_translate_table);

    mprintf((0, "Building packed OpenGL translate table...\n"));

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

      ushort pix;

      if (!(i & OPAQUE_FLAG)) {
        pix = 0;
      } else {
        pix = (r << 11) | (g << 6) | (b << 1) | 1;
      }

      opengl_packed_Translate_table[i] = pix;

      // 4444 table
      int a = (i >> 12) & 0xf;
      r = (i >> 8) & 0xf;
      g = (i >> 4) & 0xf;
      b = i & 0xf;

      pix = (r << 12) | (g << 8) | (b << 4) | a;

      opengl_packed_4444_translate_table[i] = pix;
    }
  } else {
    opengl_Upload_data = (uint *)mem_malloc(256 * 256 * 4);
    opengl_Translate_table = (uint *)mem_malloc(65536 * 4);
    opengl_4444_translate_table = (uint *)mem_malloc(65536 * 4);

    ASSERT(opengl_Upload_data);
    ASSERT(opengl_Translate_table);
    ASSERT(opengl_4444_translate_table);

    mprintf((0, "Building OpenGL translate table...\n"));

    for (i = 0; i < 65536; i++) {
      uint pix;
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

      if (!(i & OPAQUE_FLAG))
        pix = 0;
      else
        pix = (255 << 24) | (b << 16) | (g << 8) | (r);

      opengl_Translate_table[i] = pix;

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

      opengl_4444_translate_table[i] = pix;
    }
  }

  opengl_SetDefaults();

  CHECK_ERROR(4)

  // Tell our app to use multitexture
  if (OpenGL_multitexture)
    UseMultitexture = true;

  OpenGL_state.initted = 1;

  mprintf((0, "OpenGL initialization at %d x %d was successful.\n", width, height));

  return retval;

D3DError:
  opengl_Close();
  return 0;
}

// Releases the rendering context
void opengl_Close() {
  CHECK_ERROR(5)

  uint *delete_list = (uint *)mem_malloc(Cur_texture_object_num * sizeof(int));
  ASSERT(delete_list);
  for (int i = 1; i < Cur_texture_object_num; i++)
    delete_list[i] = i;

  if (Cur_texture_object_num > 1)
    dglDeleteTextures(Cur_texture_object_num, (const uint *)delete_list);

  mem_free(delete_list);

#if defined(WIN32)
  if (dwglMakeCurrent)
    dwglMakeCurrent(NULL, NULL);

  if (dwglDeleteContext)
    dwglDeleteContext(ResourceContext);

  // Change our display back
  if (!WindowGL)
    ChangeDisplaySettings(NULL, 0);
#elif defined(__LINUX__)
  // Restore our video mode
  LinuxVideoMode.Lock(false);
  LinuxVideoMode.RestoreVideoMode();
  if (OpenGL_Display) {
    if (dglXMakeCurrent)
      glXMakeCurrent(OpenGL_Display, OpenGL_Window, NULL);
    if (dglXDestroyContext)
      glXDestroyContext(OpenGL_Display, OpenGL_Context);
  }
#else

#endif

  if (OpenGL_packed_pixels) {
    if (opengl_packed_Upload_data)
      mem_free(opengl_packed_Upload_data);
    if (opengl_packed_Translate_table)
      mem_free(opengl_packed_Translate_table);
    if (opengl_packed_4444_translate_table)
      mem_free(opengl_packed_4444_translate_table);
    opengl_packed_Upload_data = NULL;
    opengl_packed_Translate_table = NULL;
    opengl_packed_4444_translate_table = NULL;
  } else {
    if (opengl_Upload_data)
      mem_free(opengl_Upload_data);
    if (opengl_Translate_table)
      mem_free(opengl_Translate_table);
    if (opengl_4444_translate_table)
      mem_free(opengl_4444_translate_table);
    opengl_Upload_data = NULL;
    opengl_Translate_table = NULL;
    opengl_4444_translate_table = NULL;
  }

  if (OpenGL_cache_initted) {
    mem_free(OpenGL_lightmap_remap);
    mem_free(OpenGL_bitmap_remap);
    mem_free(OpenGL_lightmap_states);
    mem_free(OpenGL_bitmap_states);
    OpenGL_cache_initted = 0;
  }

#if defined(WIN32)
  // Restore gamma values
  SetDeviceGammaRamp(hOpenGLDC, (LPVOID)Saved_gamma_values);
  //	I'm freeing the DC here - samir
  ReleaseDC(hOpenGLWnd, hOpenGLDC);
#elif defined(__LINUX__)

#else

#endif
  // mod_FreeModule (OpenGLDLLHandle);
  OpenGL_state.initted = 0;
}

// Takes our 16bit format and converts it into the memory scheme that OpenGL wants
void opengl_TranslateBitmapToOpenGL(int texnum, int bm_handle, int map_type, int replace, int tn) {
  ushort *bm_ptr;

  int w, h;
  int size;

  if (OpenGL_multitexture && Last_texel_unit_set != tn) {
#if defined(WIN32)
    oglActiveTextureARB(GL_TEXTURE0_ARB + tn);
    Last_texel_unit_set = tn;
#endif
  }

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

  int i;

  if (OpenGL_packed_pixels) {
    if (map_type == MAP_TYPE_LIGHTMAP) {
      ushort *left_data = (ushort *)opengl_packed_Upload_data;
      int bm_left = 0;

      for (int i = 0; i < h; i++, left_data += size, bm_left += w) {
        ushort *dest_data = left_data;
        for (int t = 0; t < w; t++) {
          *dest_data++ = opengl_packed_Translate_table[bm_ptr[bm_left + t]];
        }
      }

      if (replace) {
        dglTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, size, size, GL_RGBA, UNSIGNED_SHORT_5_5_5_1_EXT,
                         opengl_packed_Upload_data);
      } else {
        dglTexImage2D(GL_TEXTURE_2D, 0, GL_RGB5_A1, size, size, 0, GL_RGBA, UNSIGNED_SHORT_5_5_5_1_EXT,
                      opengl_packed_Upload_data);
      }
    } else {
      int limit = 0;

      if (bm_mipped(bm_handle))
        limit = NUM_MIP_LEVELS + 3;
      else
        limit = 1;

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

          if (w < 1)
            continue;
        }

        if (bm_format(bm_handle) == BITMAP_FORMAT_4444) {
          // Do 4444

          if (bm_mipped(bm_handle)) {
            for (i = 0; i < w * h; i++)
              opengl_packed_Upload_data[i] = 0xf | (opengl_packed_4444_translate_table[bm_ptr[i]]);
          } else {
            for (i = 0; i < w * h; i++)
              opengl_packed_Upload_data[i] = opengl_packed_4444_translate_table[bm_ptr[i]];
          }

          if (replace) {
            dglTexSubImage2D(GL_TEXTURE_2D, m, 0, 0, w, h, GL_RGBA, UNSIGNED_SHORT_4_4_4_4_EXT,
                             opengl_packed_Upload_data);
          } else {
            dglTexImage2D(GL_TEXTURE_2D, m, GL_RGBA4, w, h, 0, GL_RGBA, UNSIGNED_SHORT_4_4_4_4_EXT,
                          opengl_packed_Upload_data);
          }
        } else {
          // Do 1555
          for (i = 0; i < w * h; i++)
            opengl_packed_Upload_data[i] = opengl_packed_Translate_table[bm_ptr[i]];

          if (replace) {
            dglTexSubImage2D(GL_TEXTURE_2D, m, 0, 0, w, h, GL_RGBA, UNSIGNED_SHORT_5_5_5_1_EXT,
                             opengl_packed_Upload_data);
          } else {
            dglTexImage2D(GL_TEXTURE_2D, m, GL_RGB5_A1, w, h, 0, GL_RGBA, UNSIGNED_SHORT_5_5_5_1_EXT,
                          opengl_packed_Upload_data);
          }
        }
      }
    }
  } else {
    if (map_type == MAP_TYPE_LIGHTMAP) {
      uint *left_data = (uint *)opengl_Upload_data;
      int bm_left = 0;

      for (int i = 0; i < h; i++, left_data += size, bm_left += w) {
        uint *dest_data = left_data;
        for (int t = 0; t < w; t++) {
          *dest_data++ = opengl_Translate_table[bm_ptr[bm_left + t]];
        }
      }

      if (replace) {
        dglTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, size, size, GL_RGBA, GL_UNSIGNED_BYTE, opengl_Upload_data);
      } else {
        dglTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size, size, 0, GL_RGBA, GL_UNSIGNED_BYTE, opengl_Upload_data);
      }
    } else {
      int limit = 0;

      if (bm_mipped(bm_handle))
        limit = NUM_MIP_LEVELS;
      else
        limit = 1;

      for (int m = 0; m < limit; m++) {
        bm_ptr = bm_data(bm_handle, m);
        w = bm_w(bm_handle, m);
        h = bm_h(bm_handle, m);

        if (bm_format(bm_handle) == BITMAP_FORMAT_4444) {
          // Do 4444

          if (bm_mipped(bm_handle)) {
            for (i = 0; i < w * h; i++)
              opengl_Upload_data[i] = (255 << 24) | opengl_4444_translate_table[bm_ptr[i]];
          } else {
            for (i = 0; i < w * h; i++)
              opengl_Upload_data[i] = opengl_4444_translate_table[bm_ptr[i]];
          }
        } else {
          // Do 1555

          for (i = 0; i < w * h; i++)
            opengl_Upload_data[i] = opengl_Translate_table[bm_ptr[i]];
        }

        if (replace) {
          dglTexSubImage2D(GL_TEXTURE_2D, m, 0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, opengl_Upload_data);
        } else {
          dglTexImage2D(GL_TEXTURE_2D, m, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, opengl_Upload_data);
        }
      }
    }
  }

  // mprintf ((1,"Doing slow upload to opengl!\n"));

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
    mprintf((0, "Can't use non-square textures with OpenGL!\n"));
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
    if (OpenGL_multitexture && Last_texel_unit_set != tn) {
#if defined(WIN32)
      oglActiveTextureARB(GL_TEXTURE0_ARB + tn);
      Last_texel_unit_set = tn;
#endif
    }

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
    dest_wrap = OpenGL_state.cur_wrap_type;

  if (map_type == MAP_TYPE_LIGHTMAP)
    uwrap = GET_WRAP_STATE(OpenGL_lightmap_states[handle]);
  else
    uwrap = GET_WRAP_STATE(OpenGL_bitmap_states[handle]);

  if (uwrap == dest_wrap)
    return;

  if (OpenGL_multitexture && Last_texel_unit_set != tn) {
#if defined(WIN32)
    oglActiveTextureARB(GL_TEXTURE0_ARB + tn);
    Last_texel_unit_set = tn;
#endif
  }

  OpenGL_sets_this_frame[1]++;

  if (OpenGL_state.cur_wrap_type == WT_CLAMP) {
    dglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    dglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

  } else if (OpenGL_state.cur_wrap_type == WT_WRAP_V) {
    dglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
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
  sbyte dest_state;

  if (map_type == MAP_TYPE_LIGHTMAP) {
    magf = GET_FILTER_STATE(OpenGL_lightmap_states[handle]);
    dest_state = 1;
  } else {
    magf = GET_FILTER_STATE(OpenGL_bitmap_states[handle]);
    dest_state = OpenGL_preferred_state.filtering;
    if (!OpenGL_state.cur_bilinear_state)
      dest_state = 0;
  }

  if (magf == dest_state)
    return;

  if (OpenGL_multitexture && Last_texel_unit_set != tn) {
#if defined(WIN32)
    oglActiveTextureARB(GL_TEXTURE0_ARB + tn);
    Last_texel_unit_set = tn;
#endif
  }

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

// returns the alpha that we should use
float opengl_GetAlphaMultiplier(void) {
  switch (OpenGL_state.cur_alpha_type) {
  case AT_ALWAYS:
    return 1.0;
  case AT_CONSTANT:
    return OpenGL_state.cur_alpha / 255.0;
  case AT_TEXTURE:
    return 1.0;
  case AT_CONSTANT_TEXTURE:
    return OpenGL_state.cur_alpha / 255.0;
  case AT_VERTEX:
    return 1.0;
  case AT_CONSTANT_TEXTURE_VERTEX:
  case AT_CONSTANT_VERTEX:
    return OpenGL_state.cur_alpha / 255.0;
  case AT_TEXTURE_VERTEX:
    return 1.0;
  case AT_LIGHTMAP_BLEND:
  case AT_LIGHTMAP_BLEND_SATURATE:
    return OpenGL_state.cur_alpha / 255.0;
  case AT_SATURATE_TEXTURE:
    return OpenGL_state.cur_alpha / 255.0;
  case AT_SATURATE_VERTEX:
    return 1.0;
  case AT_SATURATE_CONSTANT_VERTEX:
    return OpenGL_state.cur_alpha / 255.0;
  case AT_SATURATE_TEXTURE_VERTEX:
    return 1.0;
  case AT_SPECULAR:
    return 1.0;
  default:
    // Int3();		// no type defined,get jason
    return 0;
  }
}

// Turns on/off multitexture blending
void opengl_SetMultitextureBlendMode(bool state) {
  if (OpenGL_multitexture_state == state)
    return;

  OpenGL_multitexture_state = state;

  if (state) {
#if defined(WIN32)
    oglClientActiveTextureARB(GL_TEXTURE0_ARB + 1);
    oglActiveTextureARB(GL_TEXTURE0_ARB + 1);
    dglEnableClientState(GL_TEXTURE_COORD_ARRAY);
    dglEnable(GL_TEXTURE_2D);

    /*if (Overlay_type==OT_BLEND_SATURATE)
    glBlendFunc (GL_SRC_ALPHA,GL_ONE);
    else
    glBlendFunc (GL_DST_COLOR,GL_ZERO);*/

    oglActiveTextureARB(GL_TEXTURE0_ARB + 0);
    Last_texel_unit_set = 0;
#endif
  } else {
#if defined(WIN32)
    oglClientActiveTextureARB(GL_TEXTURE0_ARB + 1);
    oglActiveTextureARB(GL_TEXTURE0_ARB + 1);
    dglDisableClientState(GL_TEXTURE_COORD_ARRAY);
    dglDisable(GL_TEXTURE_2D);
    oglActiveTextureARB(GL_TEXTURE0_ARB + 0);
    Last_texel_unit_set = 0;
#endif
  }
}

// Takes nv vertices and draws the polygon defined by those vertices.  Uses bitmap "handle"
// as a texture
void opengl_DrawMultitexturePolygon(int handle, g3Point **p, int nv, int map_type) {
  g3Point *pnt;
  int i, fr, fg, fb;
  float alpha;
  vector *vertp;
  color_array *colorp;
  tex_array *texp, *texp2;
  float one_over_square_res = 1.0 / GameLightmaps[Overlay_map].square_res;

  float xscalar = (float)GameLightmaps[Overlay_map].width * one_over_square_res;
  float yscalar = (float)GameLightmaps[Overlay_map].height * one_over_square_res;

  ASSERT(nv < 100);

  int x_add = OpenGL_state.clip_x1;
  int y_add = OpenGL_state.clip_y1;

  if (OpenGL_state.cur_light_state == LS_NONE) {
    fr = GR_COLOR_RED(OpenGL_state.cur_color);
    fg = GR_COLOR_GREEN(OpenGL_state.cur_color);
    fb = GR_COLOR_BLUE(OpenGL_state.cur_color);
  }

  alpha = Alpha_multiplier * OpenGL_Alpha_factor;

  vertp = &GL_verts[0];
  texp = &GL_tex_coords[0];
  texp2 = &GL_tex_coords2[0];
  colorp = &GL_colors[0];

  // Specify our coordinates
  for (i = 0; i < nv; i++, vertp++, texp++, colorp++, texp2++) {
    pnt = p[i];

    if (OpenGL_state.cur_alpha_type & ATF_VERTEX)
      alpha = pnt->p3_a * Alpha_multiplier * OpenGL_Alpha_factor;

    // If we have a lighting model, apply the correct lighting!
    if (OpenGL_state.cur_light_state != LS_NONE) {
      // Do lighting based on intesity (MONO) or colored (RGB)
      if (OpenGL_state.cur_color_model == CM_MONO) {
        colorp->r = pnt->p3_l;
        colorp->g = pnt->p3_l;
        colorp->b = pnt->p3_l;
        colorp->a = alpha;
      } else {
        colorp->r = pnt->p3_r;
        colorp->g = pnt->p3_g;
        colorp->b = pnt->p3_b;
        colorp->a = alpha;
      }
    } else {
      colorp->r = 1;
      colorp->g = 1;
      colorp->b = 1;
      colorp->a = alpha;
    }

    // Texture this polygon!
    float texw = 1.0 / (pnt->p3_z + Z_bias);
    texp->s = pnt->p3_u * texw;
    texp->t = pnt->p3_v * texw;
    texp->r = 0;
    texp->w = texw;

    texp2->s = pnt->p3_u2 * xscalar * texw;
    texp2->t = pnt->p3_v2 * yscalar * texw;
    texp2->r = 0;
    texp2->w = texw;

    // Finally, specify a vertex
    vertp->x = pnt->p3_sx + x_add;
    vertp->y = pnt->p3_sy + y_add;

    //@@vertp->z=-((pnt->p3_z+Z_bias)/OpenGL_state.cur_far_z);
    vertp->z = -std::max(0, std::min(1.0, 1.0 - (1.0 / (pnt->p3_z + Z_bias))));
  }

  // make sure our bitmap is ready to be drawn
  opengl_MakeBitmapCurrent(handle, map_type, 0);
  opengl_MakeWrapTypeCurrent(handle, map_type, 0);
  opengl_MakeFilterTypeCurrent(handle, map_type, 0);

  // make sure our bitmap is ready to be drawn
  opengl_MakeBitmapCurrent(Overlay_map, MAP_TYPE_LIGHTMAP, 1);
  opengl_MakeWrapTypeCurrent(Overlay_map, MAP_TYPE_LIGHTMAP, 1);
  opengl_MakeFilterTypeCurrent(Overlay_map, MAP_TYPE_LIGHTMAP, 1);

  opengl_SetMultitextureBlendMode(true);

  // And draw!
#ifdef __LINUX__
  if (OpenGL_UseLists) {
#endif
    dglDrawArrays(GL_POLYGON, 0, nv);
#ifdef __LINUX__
  } else {
    dglBegin(GL_POLYGON);
    for (i = 0; i < nv; i++) {
      dglTexCoord4fv((GLfloat *)&GL_tex_coords[i]);
      dglColor4fv((GLfloat *)&GL_colors[i]);
      dglVertex3fv((GLfloat *)&GL_verts[i]);
    }
    dglEnd();
  }
#endif

  OpenGL_polys_drawn++;
  OpenGL_verts_processed += nv;

  CHECK_ERROR(10)
}

void opengl_DrawFlatPolygon(g3Point **p, int nv) {
  int x_add = OpenGL_state.clip_x1;
  int y_add = OpenGL_state.clip_y1;
  float fr, fg, fb;
  int i;

  if (OpenGL_multitexture)
    opengl_SetMultitextureBlendMode(false);

  float alpha = Alpha_multiplier * OpenGL_Alpha_factor;

  fr = GR_COLOR_RED(OpenGL_state.cur_color);
  fg = GR_COLOR_GREEN(OpenGL_state.cur_color);
  fb = GR_COLOR_BLUE(OpenGL_state.cur_color);
  fr /= 255.0;
  fg /= 255.0;
  fb /= 255.0;

  // And draw!
  dglBegin(GL_POLYGON);
  for (i = 0; i < nv; i++) {
    g3Point *pnt = p[i];

    if (OpenGL_state.cur_alpha_type & ATF_VERTEX)
      alpha = pnt->p3_a * Alpha_multiplier * OpenGL_Alpha_factor;

    // If we have a lighting model, apply the correct lighting!
    if (OpenGL_state.cur_light_state != LS_NONE) {
      // Do lighting based on intesity (MONO) or colored (RGB)
      if (OpenGL_state.cur_color_model == CM_MONO)
        dglColor4f(pnt->p3_l, pnt->p3_l, pnt->p3_l, alpha);
      else {
        dglColor4f(pnt->p3_r, pnt->p3_g, pnt->p3_b, alpha);
      }

    } else
      dglColor4f(fr, fg, fb, alpha);

    // Finally, specify a vertex
    //@@dglVertex3f (pnt->p3_sx+x_add,pnt->p3_sy+y_add,-(pnt->p3_z/OpenGL_state.cur_far_z));
    float z = std::max(0, std::min(1.0, 1.0 - (1.0 / (pnt->p3_z + Z_bias))));
    dglVertex3f(pnt->p3_sx + x_add, pnt->p3_sy + y_add, -z);
  }

  dglEnd();
  CHECK_ERROR(11)
  OpenGL_polys_drawn++;
  OpenGL_verts_processed += nv;
}

// Sets the gamma correction value
void opengl_SetGammaValue(float val) {
  if (WindowGL)
    return;

  OpenGL_preferred_state.gamma = val;
  mprintf((0, "Setting gamma to %f\n", val));

#if defined(WIN32)
  WORD rampvals[3 * 256];

  for (int i = 0; i < 256; i++) {
    float norm = (float)i / 255.0f;

    float newval = powf(norm, 1.0f / val);

    newval *= 65535;

    newval = std::min(65535, newval);

    rampvals[i] = newval;
    rampvals[i + 256] = newval;
    rampvals[i + 512] = newval;
  }

  SetDeviceGammaRamp(hOpenGLDC, (LPVOID)rampvals);
#endif
}

// Resets the texture cache
void opengl_ResetCache(void) {
  if (OpenGL_cache_initted) {
    mem_free(OpenGL_lightmap_remap);
    mem_free(OpenGL_bitmap_remap);
    mem_free(OpenGL_lightmap_states);
    mem_free(OpenGL_bitmap_states);
    OpenGL_cache_initted = 0;
  }

  opengl_InitCache();
}

ubyte opengl_Framebuffer_ready = 0;
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
  ushort *src_data = bm_data(bm_handle, 0);
  ushort *sdata;
  ushort *ddata;

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
  }   // end for hindex
}

#ifdef __LINUX__
void CreateFullScreenWindow(Display *dpy, Window rootwin, Window window, int DisplayScreen, int DisplayWidth,
                            int DisplayHeight) {
  // see if a motif based window manager is running.  do this by
  // getting the _MOTIF_WM_INFO property on the root window.  if
  // it exists then make sure the window it refers to also exists.
  Bool noWM, isMWMRunning = False;
  Atom a = XInternAtom(dpy, "_MOTIF_WM_INFO", True);
  if (a) {
    struct BzfPropMotifWmInfo {
    public:
      long flags;
      Window wmWindow;
    };

    Atom type;
    int format;
    unsigned long nitems;
    unsigned long bytes_after;
    long *mwmInfo;

    XGetWindowProperty(dpy, rootwin, a, 0, 4, False, a, &type, &format, &nitems, &bytes_after,
                       (unsigned char **)&mwmInfo);

    if (mwmInfo) {
      // get the mwm window from the properties
      const Window mwmWindow = ((BzfPropMotifWmInfo *)mwmInfo)->wmWindow;
      XFree(mwmInfo);

      // verify that window is a child of the root window
      Window root, parent, *children;
      unsigned int numChildren;
      if (XQueryTree(dpy, mwmWindow, &root, &parent, &children, &numChildren)) {
        XFree(children);
        if (parent == rootwin)
          isMWMRunning = True;
      }
    }
  }

  // turning off decorations is window manager dependent
  if (isMWMRunning) {
    fprintf(stdout, "Motif Window Manager\n");
    // it's a Motif based window manager
    long hints[4];
    hints[0] = 0;
    hints[1] = 0;
    hints[2] = 0;
    hints[3] = 0;
    long *xhints;

    a = XInternAtom(dpy, "_MOTIF_WM_HINTS", False);

    {
      // get current hints
      Atom type;
      int format;
      unsigned long nitems;
      unsigned long bytes_after;

      XGetWindowProperty(dpy, window, a, 0, 4, False, a, &type, &format, &nitems, &bytes_after,
                         (unsigned char **)&xhints);

      if (xhints) {
        hints[0] = xhints[0];
        hints[1] = xhints[1];
        hints[2] = xhints[2];
        hints[3] = xhints[3];
        XFree(xhints);
      }
    }

    hints[0] |= 2; // MWM_HINTS_DECORATIONS flag
    hints[2] = 0;  // no decorations

    XChangeProperty(dpy, window, a, a, 32, PropModeReplace, (unsigned char *)&hints, 4);

    noWM = False;
  } else {
    // non-motif window manager.  use override redirect to prevent window
    // manager from messing with our appearance.  unfortunately, the user
    // can't move or iconify the window either.
    XSetWindowAttributes attr;
    attr.override_redirect = True;
    XChangeWindowAttributes(dpy, window, CWOverrideRedirect, &attr);
    noWM = True;
  }

  // now set position and size
  long dummy;
  XSizeHints xsh;
  XGetWMNormalHints(dpy, window, &xsh, &dummy);
  xsh.x = 0;
  xsh.y = 0;
  xsh.base_width = DisplayWidth;
  xsh.base_height = DisplayHeight;
  xsh.flags |= USPosition | PPosition | PBaseSize;

  {
    char *env;

    env = getenv("MESA_GLX_FX");
    if (env && *env == 'f') // Full screen Mesa mode
    {
      xsh.base_width = 640;
      xsh.base_height = 480;
    } else {
      // Check if we have the XF86 vidmode extension, for virtual roots
      if (LinuxVideoMode.QueryExtension(dpy)) {
        int dotclock;
        XF86VidModeModeLine modeline;

        LinuxVideoMode.GetModeLine(dpy, DisplayScreen, &dotclock, &modeline);

        xsh.base_width = modeline.hdisplay;
        xsh.base_height = modeline.vdisplay;

        // if (modeline.c_private)
        //	XFree(modeline.c_private);
      }
    }
  }

  // set the window manager hints for the window and move and resize
  // the window (overriding the window manager).  we have to override
  // the window manager for the move and resize because the window
  // *must* be the correct size when we first bind the OpenGL context
  // for the 3Dfx driver since it cannot handle later resizes.  if we
  // don't override the window manager, our move and resize will
  // probably be ignored.
  if (!noWM) {
    XSetWindowAttributes attr;
    attr.override_redirect = True;
    XChangeWindowAttributes(dpy, window, CWOverrideRedirect, &attr);
  }

  XSetWMNormalHints(dpy, window, &xsh);
  XMoveResizeWindow(dpy, window, xsh.x, xsh.y, xsh.base_width, xsh.base_height);

  if (!noWM) {
    XSetWindowAttributes attr;
    attr.override_redirect = False;
    XChangeWindowAttributes(dpy, window, CWOverrideRedirect, &attr);
  }

  XSync(dpy, False);
}
#endif

// Tells the software renderer whether or not to use mipping
void rend_SetMipState(sbyte mipstate) {}

void rend_SetInitOptions(void) {
  if (FindArg("-forcelightmaps")) {
    NoLightmaps = false;
  }

  if (FindArg("-nolightmaps")) {
    NoLightmaps = true;
    UseMultitexture = false;
  }

  if (FindArg("-NoMultitexture")) {
    UseMultitexture = false;
  }
}

// Init our renderer
int rend_Init(renderer_type state, oeApplication *app, renderer_preferred_state *pref_state) {
#ifndef DEDICATED_ONLY
  int retval = 0;
  rend_SetRendererType(state);
  if (!Renderer_initted) {
    if (!Renderer_close_flag) {
      atexit(rend_Close);
      Renderer_close_flag = 1;
    }

    Renderer_initted = 1;
  }

  if (OpenGL_window_initted) {
    rend_CloseOpenGLWindow();
    OpenGL_window_initted = 0;
  }

  mprintf((0, "Renderer init is set to %d\n", Renderer_initted));
  NoLightmaps = false;
  UseHardware = 1;
  StateLimited = 1;
  UseMultitexture = 0;
  UseWBuffer = 0;

#ifndef OEM_V3
  int flags = app->flags();
  if (flags & OEAPP_WINDOWED) {
    // initialize for windowed
    retval = rend_InitOpenGLWindow(app, pref_state);
  } else {
    // initialize for full screen
    retval = opengl_Init(app, pref_state);
  }
#endif

  if (retval != 0) {
    rend_SetInitOptions();
  }
  return retval;
#else
  return 0;
#endif // #ifdef DEDICATED_ONLY
}

void rend_Close(void) {
  mprintf((0, "CLOSE:Renderer init is set to %d\n", Renderer_initted));
  if (!Renderer_initted)
    return;

  if (OpenGL_window_initted) {
    if (Renderer_type != RENDERER_OPENGL) {
      rend_CloseOpenGLWindow();
    }
    OpenGL_window_initted = 0;
  }

  opengl_Close();

  Renderer_initted = 0;
}

#ifdef DEDICATED_ONLY

void rend_DrawPolygon3D(int, g3Point **, int, int) {}
void rend_DrawPolygon2D(int, g3Point **, int, int) {}

#else

// Takes nv vertices and draws the 3D polygon defined by those vertices.
// Uses bitmap "handle" as a texture
void rend_DrawPolygon3D(int handle, g3Point **p, int nv, int map_type) {
  g3Point *pnt;
  int i;
  float fr, fg, fb;
  float alpha;
  vector *vertp;
  color_array *colorp;
  tex_array *texp;

  ASSERT(nv < 100);

  if (OpenGL_state.cur_texture_quality == 0) {
    opengl_DrawFlatPolygon(p, nv);
    return;
  }

  if (Overlay_type != OT_NONE && OpenGL_multitexture) {
    opengl_DrawMultitexturePolygon(handle, p, nv, map_type);
    return;
  }

  int x_add = OpenGL_state.clip_x1;
  int y_add = OpenGL_state.clip_y1;

  if (OpenGL_state.cur_light_state == LS_FLAT_GOURAUD) {
    fr = GR_COLOR_RED(OpenGL_state.cur_color) / 255.0;
    fg = GR_COLOR_GREEN(OpenGL_state.cur_color) / 255.0;
    fb = GR_COLOR_BLUE(OpenGL_state.cur_color) / 255.0;
  }

  if (OpenGL_multitexture)
    opengl_SetMultitextureBlendMode(false);

  // make sure our bitmap is ready to be drawn
  opengl_MakeBitmapCurrent(handle, map_type, 0);
  opengl_MakeWrapTypeCurrent(handle, map_type, 0);
  opengl_MakeFilterTypeCurrent(handle, map_type, 0);

  alpha = Alpha_multiplier * OpenGL_Alpha_factor;

  vertp = &GL_verts[0];
  texp = &GL_tex_coords[0];
  colorp = &GL_colors[0];

  // Specify our coordinates
  for (i = 0; i < nv; i++, vertp++, texp++, colorp++) {
    pnt = p[i];

    ////////////////////////////////////////////
    if (pnt->p3_flags & PF_ORIGPOINT) {
      // get the original point
      float origPoint[4];
      origPoint[0] = pnt->p3_vecPreRot.x;
      origPoint[1] = pnt->p3_vecPreRot.y;
      origPoint[2] = pnt->p3_vecPreRot.z;
      origPoint[3] = 1.0f;

      // transform by the full transform
      float view[4];
      g3_TransformVert(view, origPoint, gTransformFull);

      vector tempv = pnt->p3_vecPreRot - View_position;
      vector testPt = tempv * Unscaled_matrix;

      float screenX = pnt->p3_sx + x_add;
      float screenY = pnt->p3_sy + y_add;

      // normalize
      float oOW = 1.0f / view[3];
      view[0] *= oOW;
      view[1] *= oOW;
      view[2] *= oOW;

      oOW *= 1.0f;
    }
    ////////////////////////////////////////////

    if (OpenGL_state.cur_alpha_type & ATF_VERTEX)
      alpha = pnt->p3_a * Alpha_multiplier * OpenGL_Alpha_factor;

    // If we have a lighting model, apply the correct lighting!
    if (OpenGL_state.cur_light_state != LS_NONE) {
      if (OpenGL_state.cur_light_state == LS_FLAT_GOURAUD) {
        colorp->r = fr;
        colorp->g = fg;
        colorp->b = fb;
        colorp->a = alpha;
      } else

        // Do lighting based on intesity (MONO) or colored (RGB)
        if (OpenGL_state.cur_color_model == CM_MONO) {
          colorp->r = pnt->p3_l;
          colorp->g = pnt->p3_l;
          colorp->b = pnt->p3_l;
          colorp->a = alpha;
        } else {
          colorp->r = pnt->p3_r;
          colorp->g = pnt->p3_g;
          colorp->b = pnt->p3_b;
          colorp->a = alpha;
        }
    } else {
      colorp->r = 1;
      colorp->g = 1;
      colorp->b = 1;
      colorp->a = alpha;
    }

#ifdef __LINUX__
    // MY TEST HACK...MAYBE BAD DRIVERS? OR MAYBE THIS IS
    // HOW IT SHOULD BE DONE (STILL BUGGY)
    // Texture this polygon!
    float texw = 1.0 / (pnt->p3_z + Z_bias);
    if (OpenGL_TextureHack) {
      texp->s = pnt->p3_u;
      texp->t = pnt->p3_v;
    } else {
      texp->s = pnt->p3_u * texw;
      texp->t = pnt->p3_v * texw;
    }
    texp->r = 0;
    texp->w = texw;
#else
    // Texture this polygon!
    float texw = 1.0 / (pnt->p3_z + Z_bias);
    texp->s = pnt->p3_u * texw;
    texp->t = pnt->p3_v * texw;
    texp->r = 0;
    texp->w = texw;
#endif
    // Finally, specify a vertex
    vertp->x = pnt->p3_sx + x_add;
    vertp->y = pnt->p3_sy + y_add;

    //@@float z=(pnt->p3_z+Z_bias)/OpenGL_state.cur_far_z;
    float z = std::max(0, std::min(1.0, 1.0 - (1.0 / (pnt->p3_z + Z_bias))));
    vertp->z = -z;
  }

  // And draw!
#ifdef __LINUX__
  if (OpenGL_UseLists) {
#endif
    dglDrawArrays(GL_POLYGON, 0, nv);
#ifdef __LINUX__
  } else {
    dglBegin(GL_POLYGON);
    for (i = 0; i < nv; i++) {
      dglTexCoord4fv((GLfloat *)&GL_tex_coords[i]);
      dglColor4fv((GLfloat *)&GL_colors[i]);
      dglVertex3fv((GLfloat *)&GL_verts[i]);
    }
    dglEnd();
  }
#endif
  OpenGL_polys_drawn++;
  OpenGL_verts_processed += nv;

  CHECK_ERROR(10)

  // If there is a lightmap to draw, draw it as well
  if (Overlay_type != OT_NONE) {
    return; // Temp fix until I figure out whats going on
    Int3(); // Shouldn't reach here
  }
}

// Takes nv vertices and draws the 2D polygon defined by those vertices.
// Uses bitmap "handle" as a texture
void rend_DrawPolygon2D(int handle, g3Point **p, int nv) {
  ASSERT(nv < 100);
  ASSERT(Overlay_type == OT_NONE);

  if (OpenGL_multitexture) {
    opengl_SetMultitextureBlendMode(false);
  }

  int xAdd = OpenGL_state.clip_x1;
  int yAdd = OpenGL_state.clip_y1;

  float fr, fg, fb;
  if (OpenGL_state.cur_light_state == LS_FLAT_GOURAUD || OpenGL_state.cur_texture_quality == 0) {
    float scale = 1.0f / 255.0f;
    fr = GR_COLOR_RED(OpenGL_state.cur_color) * scale;
    fg = GR_COLOR_GREEN(OpenGL_state.cur_color) * scale;
    fb = GR_COLOR_BLUE(OpenGL_state.cur_color) * scale;
  }

  // make sure our bitmap is ready to be drawn
  opengl_MakeBitmapCurrent(handle, MAP_TYPE_BITMAP, 0);
  opengl_MakeWrapTypeCurrent(handle, MAP_TYPE_BITMAP, 0);
  opengl_MakeFilterTypeCurrent(handle, MAP_TYPE_BITMAP, 0);

  float alpha = Alpha_multiplier * OpenGL_Alpha_factor;

  vector *vertp = &GL_verts[0];
  tex_array *texp = &GL_tex_coords[0];
  color_array *colorp = &GL_colors[0];

  // Specify our coordinates
  int i;
  for (i = 0; i < nv; ++i, ++vertp, ++texp, ++colorp) {
    g3Point *pnt = p[i];

    if (OpenGL_state.cur_alpha_type & ATF_VERTEX) {
      // the alpha should come from the vertex
      alpha = pnt->p3_a * Alpha_multiplier * OpenGL_Alpha_factor;
    }

    // If we have a lighting model, apply the correct lighting!
    if (OpenGL_state.cur_light_state == LS_FLAT_GOURAUD || OpenGL_state.cur_texture_quality == 0) {
      // pull the color from the constant color data
      colorp->r = fr;
      colorp->g = fg;
      colorp->b = fb;
      colorp->a = alpha;
    } else if (OpenGL_state.cur_light_state != LS_NONE) {
      // Do lighting based on intensity (MONO) or colored (RGB)
      if (OpenGL_state.cur_color_model == CM_MONO) {
        colorp->r = pnt->p3_l;
        colorp->g = pnt->p3_l;
        colorp->b = pnt->p3_l;
        colorp->a = alpha;
      } else {
        colorp->r = pnt->p3_r;
        colorp->g = pnt->p3_g;
        colorp->b = pnt->p3_b;
        colorp->a = alpha;
      }
    } else {
      // force white
      colorp->r = 1.0f;
      colorp->g = 1.0f;
      colorp->b = 1.0f;
      colorp->a = alpha;
    }

    texp->s = pnt->p3_u;
    texp->t = pnt->p3_v;
    texp->r = 0.0f;
    texp->w = 1.0f;

    // Finally, specify a vertex
    vertp->x = pnt->p3_sx + xAdd;
    vertp->y = pnt->p3_sy + yAdd;
    vertp->z = 0.0f;
  }

// And draw!
#ifdef __LINUX__
  if (OpenGL_UseLists) {
#endif
    if (OpenGL_state.cur_texture_quality == 0) {
      // force disable textures
      dglDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }

    // draw the data in the arrays
    dglDrawArrays(GL_POLYGON, 0, nv);

    if (OpenGL_state.cur_texture_quality == 0) {
      // re-enable textures
      dglEnableClientState(GL_TEXTURE_COORD_ARRAY);
    }

#ifdef __LINUX__
  } else {
    dglBegin(GL_POLYGON);
    for (i = 0; i < nv; ++i) {
      if (OpenGL_state.cur_texture_quality != 0) {
        dglTexCoord4fv((GLfloat *)&GL_tex_coords[i]);
      }
      dglColor4fv((GLfloat *)&GL_colors[i]);
      dglVertex3fv((GLfloat *)&GL_verts[i]);
    }
    dglEnd();
  }
#endif

  OpenGL_polys_drawn++;
  OpenGL_verts_processed += nv;

  CHECK_ERROR(10)
}

#endif // DEDICATED_ONLY

void rend_SetFlatColor(ddgr_color color) { OpenGL_state.cur_color = color; }

// Sets the fog state to TRUE or FALSE
void rend_SetFogState(sbyte state) {
  if (state == OpenGL_state.cur_fog_state)
    return; // No redundant state setting

  OpenGL_state.cur_fog_state = state;

  if (state == 1) {
    dglEnable(GL_FOG);
  } else {
    dglDisable(GL_FOG);
  }
}

// Sets the near and far plane of fog
void rend_SetFogBorders(float nearz, float farz) {
  // Sets the near and far plane of fog
  // Note, the opengl_Far_z variable must be valid for this function to work correctly
  float fog_start, fog_end;

  fog_start = std::max(0, std::min(1.0, 1.0 - (1.0 / nearz)));
  fog_end = std::max(0, std::min(1.0, 1.0 - (1.0 / farz)));

  OpenGL_state.cur_fog_start = fog_start;
  OpenGL_state.cur_fog_end = fog_end;

  dglFogi(GL_FOG_MODE, GL_LINEAR);
  dglFogf(GL_FOG_START, fog_start);
  dglFogf(GL_FOG_END, fog_end);
}

void rend_SetRendererType(renderer_type state) {
  Renderer_type = state;
  mprintf((0, "RendererType is set to %d.\n", state));
}

void rend_SetLighting(light_state state) {
  if (state == OpenGL_state.cur_light_state)
    return; // No redundant state setting

  if (OpenGL_multitexture && Last_texel_unit_set != 0) {
#if defined(WIN32)
    oglActiveTextureARB(GL_TEXTURE0_ARB + 0);
    Last_texel_unit_set = 0;
#endif
  }

  OpenGL_sets_this_frame[4]++;

  switch (state) {
  case LS_NONE:
    dglShadeModel(GL_SMOOTH);
    OpenGL_state.cur_light_state = LS_NONE;
    break;
  case LS_FLAT_GOURAUD:
    dglShadeModel(GL_SMOOTH);
    OpenGL_state.cur_light_state = LS_FLAT_GOURAUD;
    break;
  case LS_GOURAUD:
  case LS_PHONG:
    dglShadeModel(GL_SMOOTH);
    OpenGL_state.cur_light_state = LS_GOURAUD;
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
    OpenGL_state.cur_color_model = CM_MONO;
    break;
  case CM_RGB:
    OpenGL_state.cur_color_model = CM_RGB;
    break;
  default:
    Int3();
    break;
  }
}

void rend_SetTextureType(texture_type state) {
  if (state == OpenGL_state.cur_texture_type)
    return; // No redundant state setting

  if (OpenGL_multitexture && Last_texel_unit_set != 0) {
#if defined(WIN32)
    oglActiveTextureARB(GL_TEXTURE0_ARB + 0);
    Last_texel_unit_set = 0;
#endif
  }

  OpenGL_sets_this_frame[3]++;

  switch (state) {
  case TT_FLAT:
    dglDisable(GL_TEXTURE_2D);
    OpenGL_state.cur_texture_quality = 0;
    break;
  case TT_LINEAR:
  case TT_LINEAR_SPECIAL:
  case TT_PERSPECTIVE:
  case TT_PERSPECTIVE_SPECIAL:
    dglEnable(GL_TEXTURE_2D);
    OpenGL_state.cur_texture_quality = 2;
    break;
  default:
    Int3(); // huh? Get Jason
    break;
  }

  CHECK_ERROR(12)
  OpenGL_state.cur_texture_type = state;
}

void rend_StartFrame(int x1, int y1, int x2, int y2, int clear_flags) {
  if (clear_flags & RF_CLEAR_ZBUFFER) {
    dglClear(GL_DEPTH_BUFFER_BIT);
  }
  OpenGL_state.clip_x1 = x1;
  OpenGL_state.clip_y1 = y1;
  OpenGL_state.clip_x2 = x2;
  OpenGL_state.clip_y2 = y2;
}

// Flips the screen
void rend_Flip(void) {
#ifndef RELEASE
  int i;

  RTP_INCRVALUE(texture_uploads, OpenGL_uploads);
  RTP_INCRVALUE(polys_drawn, OpenGL_polys_drawn);

  mprintf_at(
      (1, 1, 0, "Uploads=%d    Polys=%d   Verts=%d   ", OpenGL_uploads, OpenGL_polys_drawn, OpenGL_verts_processed));
  mprintf_at((1, 2, 0, "Sets= 0:%d   1:%d   2:%d   3:%d   ", OpenGL_sets_this_frame[0], OpenGL_sets_this_frame[1],
              OpenGL_sets_this_frame[2], OpenGL_sets_this_frame[3]));
  mprintf_at((1, 3, 0, "Sets= 4:%d   5:%d  ", OpenGL_sets_this_frame[4], OpenGL_sets_this_frame[5]));
  for (i = 0; i < 10; i++)
    OpenGL_sets_this_frame[i] = 0;
#endif

  OpenGL_last_frame_polys_drawn = OpenGL_polys_drawn;
  OpenGL_last_frame_verts_processed = OpenGL_verts_processed;
  OpenGL_last_uploaded = OpenGL_uploads;

  OpenGL_uploads = 0;
  OpenGL_polys_drawn = 0;
  OpenGL_verts_processed = 0;

#if defined(WIN32)
  SwapBuffers((HDC)hOpenGLDC);
#elif defined(__LINUX__)
  glXWaitGL();
  glXSwapBuffers(OpenGL_Display, OpenGL_Window);
#endif
}

void rend_EndFrame(void) {}

// draws a scaled 2d bitmap to our buffer
void rend_DrawScaledBitmap(int x1, int y1, int x2, int y2, int bm, float u0, float v0, float u1, float v1, int color,
                           float *alphas) {
  g3Point *ptr_pnts[4];
  g3Point pnts[4];
  float r, g, b;
  if (color != -1) {
    r = GR_COLOR_RED(color) / 255.0;
    g = GR_COLOR_GREEN(color) / 255.0;
    b = GR_COLOR_BLUE(color) / 255.0;
  }
  for (int i = 0; i < 4; i++) {
    if (color == -1)
      pnts[i].p3_l = 1.0;
    else {
      pnts[i].p3_r = r;
      pnts[i].p3_g = g;
      pnts[i].p3_b = b;
    }
    if (alphas) {
      pnts[i].p3_a = alphas[i];
    }

    pnts[i].p3_z = 1.0f;
    pnts[i].p3_flags = PF_PROJECTED;
  }

  pnts[0].p3_sx = x1;
  pnts[0].p3_sy = y1;
  pnts[0].p3_u = u0;
  pnts[0].p3_v = v0;
  pnts[1].p3_sx = x2;
  pnts[1].p3_sy = y1;
  pnts[1].p3_u = u1;
  pnts[1].p3_v = v0;
  pnts[2].p3_sx = x2;
  pnts[2].p3_sy = y2;
  pnts[2].p3_u = u1;
  pnts[2].p3_v = v1;
  pnts[3].p3_sx = x1;
  pnts[3].p3_sy = y2;
  pnts[3].p3_u = u0;
  pnts[3].p3_v = v1;
  ptr_pnts[0] = &pnts[0];
  ptr_pnts[1] = &pnts[1];
  ptr_pnts[2] = &pnts[2];
  ptr_pnts[3] = &pnts[3];
  rend_SetTextureType(TT_LINEAR);
  rend_DrawPolygon2D(bm, ptr_pnts, 4);
}

// Sets where the software renderer should write to
void rend_SetSoftwareParameters(float aspect, int width, int height, int pitch, ubyte *framebuffer) {}

// Sets the state of bilinear filtering for our textures
void rend_SetFiltering(sbyte state) {
#ifndef RELEASE
  if (Fast_test_render)
    state = 0;
#endif

  OpenGL_state.cur_bilinear_state = state;
}

// Sets the state of z-buffering to on or off
void rend_SetZBufferState(sbyte state) {
#ifndef RELEASE
  if (Fast_test_render)
    state = 0;
#endif

  if (state == OpenGL_state.cur_zbuffer_state)
    return; // No redundant state setting

  OpenGL_sets_this_frame[5]++;
  OpenGL_state.cur_zbuffer_state = state;

  // mprintf ((0,"OPENGL: Setting zbuffer state to %d.\n",state));

  if (state) {
    dglEnable(GL_DEPTH_TEST);
    dglDepthFunc(GL_LEQUAL);
  } else
    dglDisable(GL_DEPTH_TEST);

  CHECK_ERROR(14)
}

// Sets the near and far planes for z buffer
void rend_SetZValues(float nearz, float farz) {
  OpenGL_state.cur_near_z = nearz;
  OpenGL_state.cur_far_z = farz;

  // mprintf ((0,"OPENGL:Setting depth range to %f - %f\n",nearz,farz));

  // JEFF: glDepthRange must take parameters [0,1]
  // It is set in init
  //@@dglDepthRange (0,farz);
}

// Sets a bitmap as a overlay map to rendered on top of the next texture map
// a -1 value indicates no overlay map
void rend_SetOverlayMap(int handle) { Overlay_map = handle; }

void rend_SetOverlayType(ubyte type) { Overlay_type = type; }

// Clears the display to a specified color
void rend_ClearScreen(ddgr_color color) {
  int r = (color >> 16 & 0xFF);
  int g = (color >> 8 & 0xFF);
  int b = (color & 0xFF);

  dglClearColor((float)r / 255.0f, (float)g / 255.0f, (float)b / 255.0f, 0);

  dglClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

// Clears the zbuffer for the screen
void rend_ClearZBuffer(void) { dglClear(GL_DEPTH_BUFFER_BIT); }

// Clears the zbuffer for the screen
void rend_ResetCache(void) {
  mprintf((0, "Resetting texture cache!\n"));
  opengl_ResetCache();
}

// Fills a rectangle on the display
void rend_FillRect(ddgr_color color, int x1, int y1, int x2, int y2) {
  int r = GR_COLOR_RED(color);
  int g = GR_COLOR_GREEN(color);
  int b = GR_COLOR_BLUE(color);

  int width = x2 - x1;
  int height = y2 - y1;

  x1 += OpenGL_state.clip_x1;
  y1 += OpenGL_state.clip_y1;

  dglEnable(GL_SCISSOR_TEST);
  dglScissor(x1, OpenGL_state.screen_height - (height + y1), width, height);
  dglClearColor((float)r / 255.0, (float)g / 255.0, (float)b / 255.0, 0);
  dglClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  width = OpenGL_state.clip_x2 - OpenGL_state.clip_x1;
  height = OpenGL_state.clip_y2 - OpenGL_state.clip_y1;

  dglScissor(OpenGL_state.clip_x1, OpenGL_state.screen_height - (OpenGL_state.clip_y1 + height), width, height);
  dglDisable(GL_SCISSOR_TEST);
}

// Sets a pixel on the display
void rend_SetPixel(ddgr_color color, int x, int y) {
  int r = (color >> 16 & 0xFF);
  int g = (color >> 8 & 0xFF);
  int b = (color & 0xFF);

  dglColor3ub(r, g, b);

  dglBegin(GL_POINTS);
  dglVertex2i(x, y);
  dglEnd();
}

// Sets a pixel on the display
ddgr_color rend_GetPixel(int x, int y) {
  ddgr_color color[4];
  dglReadPixels(x, (OpenGL_state.screen_height - 1) - y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid *)color);
  return color[0];
}

void rend_FillCircle(ddgr_color col, int x, int y, int rad) {}

void rend_DrawCircle(int x, int y, int rad) {}

// Sets up a font character to draw.  We draw our fonts as pieces of textures
void rend_DrawFontCharacter(int bm_handle, int x1, int y1, int x2, int y2, float u, float v, float w, float h) {
  g3Point *ptr_pnts[4];
  g3Point pnts[4];
  for (int i = 0; i < 4; i++) {
    pnts[i].p3_z = 1; // Make REALLY close!
    pnts[i].p3_flags = PF_PROJECTED;
    ptr_pnts[i] = &pnts[i];
  }
  pnts[0].p3_sx = x1;
  pnts[0].p3_sy = y1;
  pnts[0].p3_u = u;
  pnts[0].p3_v = v;
  pnts[1].p3_sx = x2;
  pnts[1].p3_sy = y1;
  pnts[1].p3_u = u + w;
  pnts[1].p3_v = v;
  pnts[2].p3_sx = x2;
  pnts[2].p3_sy = y2;
  pnts[2].p3_u = u + w;
  pnts[2].p3_v = v + h;
  pnts[3].p3_sx = x1;
  pnts[3].p3_sy = y2;
  pnts[3].p3_u = u;
  pnts[3].p3_v = v + h;
  rend_DrawPolygon2D(bm_handle, ptr_pnts, 4);
}

// Draws a line
void rend_DrawLine(int x1, int y1, int x2, int y2) {
  sbyte atype;
  light_state ltype;
  texture_type ttype;
  int color = OpenGL_state.cur_color;

  int r = GR_COLOR_RED(color);
  int g = GR_COLOR_GREEN(color);
  int b = GR_COLOR_BLUE(color);

  atype = OpenGL_state.cur_alpha_type;
  ltype = OpenGL_state.cur_light_state;
  ttype = OpenGL_state.cur_texture_type;

  rend_SetAlphaType(AT_ALWAYS);
  rend_SetLighting(LS_NONE);
  rend_SetTextureType(TT_FLAT);

  dglBegin(GL_LINES);
  dglColor4ub(r, g, b, 255);
  dglVertex2i(x1 + OpenGL_state.clip_x1, y1 + OpenGL_state.clip_y1);
  dglColor4ub(r, g, b, 255);
  dglVertex2i(x2 + OpenGL_state.clip_x1, y2 + OpenGL_state.clip_y1);
  dglEnd();

  rend_SetAlphaType(atype);
  rend_SetLighting(ltype);
  rend_SetTextureType(ttype);
}

// Sets the argb characteristics of the font characters.  color1 is the upper left and proceeds clockwise
void rend_SetCharacterParameters(ddgr_color color1, ddgr_color color2, ddgr_color color3, ddgr_color color4) {
  rend_FontRed[0] = (float)(GR_COLOR_RED(color1) / 255.0f);
  rend_FontRed[1] = (float)(GR_COLOR_RED(color2) / 255.0f);
  rend_FontRed[2] = (float)(GR_COLOR_RED(color3) / 255.0f);
  rend_FontRed[3] = (float)(GR_COLOR_RED(color4) / 255.0f);
  rend_FontGreen[0] = (float)(GR_COLOR_GREEN(color1) / 255.0f);
  rend_FontGreen[1] = (float)(GR_COLOR_GREEN(color2) / 255.0f);
  rend_FontGreen[2] = (float)(GR_COLOR_GREEN(color3) / 255.0f);
  rend_FontGreen[3] = (float)(GR_COLOR_GREEN(color4) / 255.0f);
  rend_FontBlue[0] = (float)(GR_COLOR_BLUE(color1) / 255.0f);
  rend_FontBlue[1] = (float)(GR_COLOR_BLUE(color2) / 255.0f);
  rend_FontBlue[2] = (float)(GR_COLOR_BLUE(color3) / 255.0f);
  rend_FontBlue[3] = (float)(GR_COLOR_BLUE(color4) / 255.0f);
  rend_FontAlpha[0] = (color1 >> 24) / 255.0f;
  rend_FontAlpha[1] = (color2 >> 24) / 255.0f;
  rend_FontAlpha[2] = (color3 >> 24) / 255.0f;
  rend_FontAlpha[3] = (color4 >> 24) / 255.0f;
}

// Sets the color of fog
void rend_SetFogColor(ddgr_color color) {
  if (color == OpenGL_state.cur_fog_color)
    return;

  float fc[4];
  fc[0] = GR_COLOR_RED(color);
  fc[1] = GR_COLOR_GREEN(color);
  fc[2] = GR_COLOR_BLUE(color);
  fc[3] = 1;

  fc[0] /= 255.0f;
  fc[1] /= 255.0f;
  fc[2] /= 255.0f;

  dglFogfv(GL_FOG_COLOR, fc);
}

// Sets the lighting state of opengl
void rend_SetLightingState(light_state state) {
  if (state == OpenGL_state.cur_light_state)
    return; // No redundant state setting

  if (OpenGL_multitexture && Last_texel_unit_set != 0) {
#if defined(WIN32)
    oglActiveTextureARB(GL_TEXTURE0_ARB + 0);
    Last_texel_unit_set = 0;
#endif
  }

  OpenGL_sets_this_frame[4]++;

  switch (state) {
  case LS_NONE:
    dglShadeModel(GL_SMOOTH);
    OpenGL_state.cur_light_state = LS_NONE;
    break;
  case LS_FLAT_GOURAUD:
    dglShadeModel(GL_SMOOTH);
    OpenGL_state.cur_light_state = LS_FLAT_GOURAUD;
    break;
  case LS_GOURAUD:
  case LS_PHONG:
    dglShadeModel(GL_SMOOTH);
    OpenGL_state.cur_light_state = LS_GOURAUD;
    break;
  default:
    Int3();
    break;
  }

  CHECK_ERROR(13)
}

void rend_SetAlphaType(sbyte atype) {
  if (atype == OpenGL_state.cur_alpha_type)
    return; // don't set it redundantly

  if (OpenGL_multitexture && Last_texel_unit_set != 0) {
#if defined(WIN32)
    oglActiveTextureARB(GL_TEXTURE0_ARB + 0);
    Last_texel_unit_set = 0;
#endif
  }

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
    rend_SetAlphaValue(255);
    dglBlendFunc(GL_ONE, GL_ZERO);
    break;
  case AT_CONSTANT:
    dglBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    break;
  case AT_TEXTURE:
    rend_SetAlphaValue(255);
    dglBlendFunc(GL_ONE, GL_ZERO);
    break;
  case AT_CONSTANT_TEXTURE:
    dglBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    break;
  case AT_VERTEX:
    dglBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    break;
  case AT_CONSTANT_TEXTURE_VERTEX:
  case AT_CONSTANT_VERTEX:
    dglBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    break;
  case AT_TEXTURE_VERTEX:
    dglBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    break;
  case AT_LIGHTMAP_BLEND:
    dglBlendFunc(GL_DST_COLOR, GL_ZERO);
    break;
  case AT_SATURATE_TEXTURE:
  case AT_LIGHTMAP_BLEND_SATURATE:
    dglBlendFunc(GL_SRC_ALPHA, GL_ONE);
    break;
  case AT_SATURATE_VERTEX:
    dglBlendFunc(GL_SRC_ALPHA, GL_ONE);
    break;
  case AT_SATURATE_CONSTANT_VERTEX:
    dglBlendFunc(GL_SRC_ALPHA, GL_ONE);
    break;
  case AT_SATURATE_TEXTURE_VERTEX:
    dglBlendFunc(GL_SRC_ALPHA, GL_ONE);
    break;
  case AT_SPECULAR:
    break;
  default:
    Int3(); // no type defined,get jason
    break;
  }
  OpenGL_state.cur_alpha_type = atype;
  Alpha_multiplier = opengl_GetAlphaMultiplier();
  CHECK_ERROR(15)
}

// Sets the alpha value for constant alpha
void rend_SetAlphaValue(ubyte val) {
  OpenGL_state.cur_alpha = val;
  Alpha_multiplier = opengl_GetAlphaMultiplier();
}

// Sets the overall alpha scale factor (all alpha values are scaled by this value)
// usefull for motion blur effect
void rend_SetAlphaFactor(float val) {
  if (val < 0.0f)
    val = 0.0f;
  if (val > 1.0f)
    val = 1.0f;
  OpenGL_Alpha_factor = val;
}

// Returns the current Alpha factor
float rend_GetAlphaFactor(void) { return OpenGL_Alpha_factor; }

// Sets the texture wrapping type
void rend_SetWrapType(wrap_type val) { OpenGL_state.cur_wrap_type = val; }

// Draws a line using the states of the renderer
void rend_DrawSpecialLine(g3Point *p0, g3Point *p1) {
  int x_add = OpenGL_state.clip_x1;
  int y_add = OpenGL_state.clip_y1;
  float fr, fg, fb, alpha;
  int i;

  fr = GR_COLOR_RED(OpenGL_state.cur_color);
  fg = GR_COLOR_GREEN(OpenGL_state.cur_color);
  fb = GR_COLOR_BLUE(OpenGL_state.cur_color);

  fr /= 255.0f;
  fg /= 255.0f;
  fb /= 255.0f;

  alpha = Alpha_multiplier * OpenGL_Alpha_factor;

  // And draw!
  dglBegin(GL_LINES);
  for (i = 0; i < 2; i++) {
    g3Point *pnt = p0;

    if (i == 1)
      pnt = p1;

    if (OpenGL_state.cur_alpha_type & ATF_VERTEX)
      alpha = pnt->p3_a * Alpha_multiplier * OpenGL_Alpha_factor;

    // If we have a lighting model, apply the correct lighting!
    if (OpenGL_state.cur_light_state != LS_NONE) {
      if (OpenGL_state.cur_light_state == LS_FLAT_GOURAUD) {
        dglColor4f(fr, fg, fb, alpha);
      } else {
        // Do lighting based on intesity (MONO) or colored (RGB)
        if (OpenGL_state.cur_color_model == CM_MONO)
          dglColor4f(pnt->p3_l, pnt->p3_l, pnt->p3_l, alpha);
        else {
          dglColor4f(pnt->p3_r, pnt->p3_g, pnt->p3_b, alpha);
        }
      }
    } else {
      dglColor4f(fr, fg, fb, alpha);
    }

    // Finally, specify a vertex
    //@@float z=(pnt->p3_z+Z_bias)/OpenGL_state.cur_far_z;
    float z = std::max(0, std::min(1.0, 1.0 - (1.0 / (pnt->p3_z + Z_bias))));
    dglVertex3f(pnt->p3_sx + x_add, pnt->p3_sy + y_add, -z);
  }

  dglEnd();
}

// Takes a screenshot of the current frame and puts it into the handle passed
void rend_Screenshot(int bm_handle) {
  ushort *dest_data;
  uint *temp_data;
  int i, t;
  int total = OpenGL_state.screen_width * OpenGL_state.screen_height;

  ASSERT((bm_w(bm_handle, 0)) == OpenGL_state.screen_width);
  ASSERT((bm_h(bm_handle, 0)) == OpenGL_state.screen_height);

  int w = bm_w(bm_handle, 0);
  int h = bm_h(bm_handle, 0);

  temp_data = (uint *)mem_malloc(total * 4);
  ASSERT(temp_data); // Ran out of memory?

  dest_data = bm_data(bm_handle, 0);

  dglReadPixels(0, 0, OpenGL_state.screen_width, OpenGL_state.screen_height, GL_RGBA, GL_UNSIGNED_BYTE,
                (GLvoid *)temp_data);

  for (i = 0; i < h; i++) {
    for (t = 0; t < w; t++) {
      uint spix = temp_data[i * w + t];

      int r = spix & 0xff;
      int g = (spix >> 8) & 0xff;
      int b = (spix >> 16) & 0xff;

      dest_data[(((h - 1) - i) * w) + t] = GR_RGB16(r, g, b);
    }
  }

  mem_free(temp_data);
}

void rend_SetZBias(float z_bias) { Z_bias = z_bias; }

// Enables/disables writes the depth buffer
void rend_SetZBufferWriteMask(int state) {
  OpenGL_sets_this_frame[5]++;
  if (state) {
    dglDepthMask(GL_TRUE);
  } else {
    dglDepthMask(GL_FALSE);
  }
}

// Gets a pointer to a linear frame buffer
void rend_GetLFBLock(renderer_lfb *lfb) {}

// Releases an lfb lock
void rend_ReleaseLFBLock(renderer_lfb *lfb) {}

// Returns the aspect ratio of the physical screen
void rend_GetProjectionParameters(int *width, int *height) {
  *width = OpenGL_state.clip_x2 - OpenGL_state.clip_x1;
  *height = OpenGL_state.clip_y2 - OpenGL_state.clip_y1;
}

void rend_GetProjectionScreenParameters(int &screenLX, int &screenTY, int &screenW, int &screenH) {
  screenLX = OpenGL_state.clip_x1;
  screenTY = OpenGL_state.clip_y1;
  screenW = OpenGL_state.clip_x2 - OpenGL_state.clip_x1;
  screenH = OpenGL_state.clip_y2 - OpenGL_state.clip_y1;
}

// Returns the aspect ratio of the physical screen
float rend_GetAspectRatio(void) {
  float aspect_ratio = (float)((3.0f * OpenGL_state.screen_width) / (4.0f * OpenGL_state.screen_height));
  return aspect_ratio;
}

// Given a source x,y and width,height, draws any sized bitmap into the renderer lfb
void rend_DrawLFBBitmap(int sx, int sy, int w, int h, int dx, int dy, ushort *data, int rowsize) {}

//	given a chunked bitmap, renders it.
void rend_DrawChunkedBitmap(chunked_bitmap *chunk, int x, int y, ubyte alpha) {
  int *bm_array = chunk->bm_array;
  int w = chunk->w;
  int h = chunk->h;
  int piece_w = bm_w(bm_array[0], 0);
  int piece_h = bm_h(bm_array[0], 0);
  int screen_w, screen_h;
  int i, t;
  rend_SetZBufferState(0);
  rend_GetProjectionParameters(&screen_w, &screen_h);
  for (i = 0; i < h; i++) {
    for (t = 0; t < w; t++) {
      int dx = x + (piece_w * t);
      int dy = y + (piece_h * i);
      int dw, dh;
      if ((dx + piece_w) > screen_w)
        dw = piece_w - ((dx + piece_w) - screen_w);
      else
        dw = piece_w;
      if ((dy + piece_h) > screen_h)
        dh = piece_h - ((dy + piece_h) - screen_h);
      else
        dh = piece_h;

      float u2 = (float)dw / (float)piece_w;
      float v2 = (float)dh / (float)piece_h;
      rend_DrawSimpleBitmap(bm_array[i * w + t], dx, dy);
    }
  }
  rend_SetZBufferState(1);
}

//	given a chunked bitmap, renders it.scaled
void rend_DrawScaledChunkedBitmap(chunked_bitmap *chunk, int x, int y, int neww, int newh, ubyte alpha) {
  int *bm_array = chunk->bm_array;
  int w = chunk->w;
  int h = chunk->h;
  int piece_w;
  int piece_h;
  int screen_w, screen_h;
  int i, t;

  float scalew, scaleh;

  scalew = ((float)neww) / ((float)chunk->pw);
  scaleh = ((float)newh) / ((float)chunk->ph);
  piece_w = scalew * ((float)bm_w(bm_array[0], 0));
  piece_h = scaleh * ((float)bm_h(bm_array[0], 0));
  rend_GetProjectionParameters(&screen_w, &screen_h);
  rend_SetOverlayType(OT_NONE);
  rend_SetLighting(LS_NONE);
  rend_SetColorModel(CM_MONO);
  rend_SetZBufferState(0);
  rend_SetAlphaType(AT_CONSTANT_TEXTURE);
  rend_SetAlphaValue(alpha);
  rend_SetWrapType(WT_WRAP);
  for (i = 0; i < h; i++) {
    for (t = 0; t < w; t++) {
      int dx = x + (piece_w * t);
      int dy = y + (piece_h * i);
      int dw, dh;
      if ((dx + piece_w) > screen_w)
        dw = piece_w - ((dx + piece_w) - screen_w);
      else
        dw = piece_w;
      if ((dy + piece_h) > screen_h)
        dh = piece_h - ((dy + piece_h) - screen_h);
      else
        dh = piece_h;

      float u2 = (float)dw / (float)piece_w;
      float v2 = (float)dh / (float)piece_h;
      rend_DrawScaledBitmap(dx, dy, dx + dw, dy + dh, bm_array[i * w + t], 0, 0, u2, v2);
    }
  }
  rend_SetZBufferState(1);
}

// Sets some global preferences for the renderer
int rend_SetPreferredState(renderer_preferred_state *pref_state) {
  int retval = 1;
  renderer_preferred_state old_state = OpenGL_preferred_state;

  OpenGL_preferred_state = *pref_state;
  if (OpenGL_state.initted) {
    int reinit = 0;
    mprintf((0, "Inside pref state!\n"));

    // Change gamma if needed
    if (pref_state->width != OpenGL_state.screen_width || pref_state->height != OpenGL_state.screen_height ||
        old_state.bit_depth != pref_state->bit_depth) {
      reinit = 1;
    }

    if (reinit) {
      opengl_Close();
      retval = opengl_Init(NULL, &OpenGL_preferred_state);
    } else {
      if (old_state.gamma != pref_state->gamma) {
        opengl_SetGammaValue(pref_state->gamma);
      }
    }
  } else {
    OpenGL_preferred_state = *pref_state;
  }

  rend_SetInitOptions();
  return retval;
}

// Sets the gamma for this display
void rend_SetGammaValue(float val) {}

// Draws a simple bitmap at the specified x,y location
void rend_DrawSimpleBitmap(int bm_handle, int x, int y) {
  rend_SetAlphaType(AT_CONSTANT_TEXTURE);
  rend_SetAlphaValue(255);
  rend_SetLighting(LS_NONE);
  rend_SetColorModel(CM_MONO);
  rend_SetOverlayType(OT_NONE);
  rend_SetFiltering(0);
  rend_DrawScaledBitmap(x, y, x + bm_w(bm_handle, 0), y + bm_h(bm_handle, 0), bm_handle, 0, 0, 1, 1);
  rend_SetFiltering(1);
}

// Fills in the passed in pointer with the current rendering state
void rend_GetRenderState(rendering_state *rstate) { memcpy(rstate, &OpenGL_state, sizeof(rendering_state)); }

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

// Changes the resolution of the renderer
void rend_SetResolution(int width, int height) {}

// Gets OpenGL ready to work in a window
int rend_InitOpenGLWindow(oeApplication *app, renderer_preferred_state *pref_state) {
  WindowGL = 1;
  return opengl_Init(app, pref_state);
}

// Shuts down OpenGL in a window
void rend_CloseOpenGLWindow(void) {
  opengl_Close();
  WindowGL = 0;
  OpenGL_window_initted = 0;
  mprintf((1, "SHUTTING DOWN WINDOWED OPENGL!"));
}

// Sets the state of the OpenGLWindow to on or off
static renderer_type Save_rend;
static bool Save_state_limit;
void rend_SetOpenGLWindowState(int state, oeApplication *app, renderer_preferred_state *pref_state) {
  if (state) {
    if (!OpenGL_window_initted) {
      if (rend_InitOpenGLWindow(app, pref_state))
        OpenGL_window_initted = 1;
      else
        return;
    }
    UseHardware = 1;
    Save_rend = Renderer_type;
    Save_state_limit = StateLimited;
    Renderer_type = RENDERER_OPENGL;
    StateLimited = 1;
    NoLightmaps = false;
  } else {
    if (OpenGL_window_initted) {
      UseHardware = 0;
      Renderer_type = RENDERER_SOFTWARE_16BIT;
      StateLimited = Save_state_limit;
    }
  }
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

// Preuploads a texture to the video card
void rend_PreUploadTextureToCard(int handle, int map_type) {}

// Frees an uploaded texture from the video card
void rend_FreePreUploadedTexture(int handle, int map_type) {}

// Retrieves an error message
char *rend_GetErrorMessage() { return (char *)Renderer_error_message; }

// Sets an error message
void rend_SetErrorMessage(char *str) {
  ASSERT(strlen(str) < 256);
  strcpy(Renderer_error_message, str);
}

// Returns 1 if there is mid video memory, 2 if there is low vid memory, or 0 if there is large vid memory
int rend_LowVidMem(void) { return 0; }

// Returns 1 if the renderer supports bumpmapping
int rend_SupportsBumpmapping(void) { return 0; }

// Sets a bumpmap to be rendered, or turns off bumpmapping altogether
void rend_SetBumpmapReadyState(int state, int map) {}

// returns the direct draw object
void *rend_RetrieveDirectDrawObj(void **frontsurf, void **backsurf) {
  *frontsurf = NULL;
  *backsurf = NULL;
  return NULL;
}

// returns rendering statistics for the frame
void rend_GetStatistics(tRendererStats *stats) {
  if (Renderer_initted) {
    stats->poly_count = OpenGL_last_frame_polys_drawn;
    stats->vert_count = OpenGL_last_frame_verts_processed;
    stats->texture_uploads = OpenGL_last_uploaded;
  } else {
    memset(stats, 0, sizeof(tRendererStats));
  }
}

#endif
