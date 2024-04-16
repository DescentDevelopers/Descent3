#if defined(WIN32)
#include <windows.h>
#include "ddraw.h"

#elif defined(__LINUX__)
#include "linux/linux_fix.h"
// #include "linux/dyna_xext.h"
#include "lnxscreenmode.h"

#ifdef __CHECK_FOR_TOO_SLOW_RENDERING__
#include "sdl_utils.h"
#endif

#include "byteswap.h"

// #include <X11/Xatom.h>

extern bool ddio_mouseGrabbed;

#else
#endif

#ifdef OPENGL_TRACING
#warning *********************************************************************
#warning *********************************************************************
#warning *********************************************************************
#warning ******* OPENGL_TRACING NEEDS TO BE REMOVED FROM THE MAKEFILE! *******
#warning *********************************************************************
#warning *********************************************************************
#warning *********************************************************************
#define _D(x)                                                                                                          \
  DGL_Printf(" // called in Descent 3:\n // ");                                                                        \
  DGL_Printf x;                                                                                                        \
  DGL_Printf("\n\n");
#else
#define _D(x)
#endif

#include "DDAccess.h" // Device Dependent access level module

#include "pstypes.h"
#include "pserror.h"
#include "application.h"
#include "renderer.h"
#include "3d.h"
#include "bitmap.h"
#include "lightmap.h"
#include "rend_opengl.h"
#include "grdefs.h"
#include "gl.h"
// #include "glu.h"
#include "mem.h"
#include "rtperformance.h"
#include "dyna_gl.h"
#include "../args.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <algorithm>

#if defined(WIN32)
#include "arb_extensions.h"

wglCreateContext_fp dwglCreateContext = NULL;
wglDeleteContext_fp dwglDeleteContext = NULL;
wglMakeCurrent_fp dwglMakeCurrent = NULL;
wglGetProcAddress_fp dwglGetProcAddress = NULL;

#endif

#ifdef __RYANS_EASTER_EGGS__
bool __pumpkinBromo = false;
#endif

// bk000614 - instantiation of the gloablly visible symbols
glAlphaFunc_fp dglAlphaFunc = NULL;
glBegin_fp dglBegin = NULL;
glBindTexture_fp dglBindTexture = NULL;
glBlendFunc_fp dglBlendFunc = NULL;
glClear_fp dglClear = NULL;
glClearColor_fp dglClearColor = NULL;
glClearDepth_fp dglClearDepth = NULL;
glColor3ub_fp dglColor3ub = NULL;
glColor4f_fp dglColor4f = NULL;
glColor4fv_fp dglColor4fv = NULL;
glColor4ub_fp dglColor4ub = NULL;
glColorPointer_fp dglColorPointer = NULL;
glDeleteTextures_fp dglDeleteTextures = NULL;
glDepthFunc_fp dglDepthFunc = NULL;
glDepthMask_fp dglDepthMask = NULL;
glDepthRange_fp dglDepthRange = NULL;
glDisable_fp dglDisable = NULL;
glDisableClientState_fp dglDisableClientState = NULL;
glDrawArrays_fp dglDrawArrays = NULL;
glDrawPixels_fp dglDrawPixels = NULL;
glEnable_fp dglEnable = NULL;
glEnableClientState_fp dglEnableClientState = NULL;
glEnd_fp dglEnd = NULL;
glFlush_fp dglFlush = NULL;
glFogf_fp dglFogf = NULL;
glFogfv_fp dglFogfv = NULL;
glFogi_fp dglFogi = NULL;
glGetError_fp dglGetError = NULL;
glGetString_fp dglGetString = NULL;
glGetIntegerv_fp dglGetIntegerv = NULL;
glHint_fp dglHint = NULL;
glLoadIdentity_fp dglLoadIdentity = NULL;
glMatrixMode_fp dglMatrixMode = NULL;
glOrtho_fp dglOrtho = NULL;
glPixelStorei_fp dglPixelStorei = NULL;
glPixelTransferi_fp dglPixelTransferi = NULL;
glPolygonOffset_fp dglPolygonOffset = NULL;
glReadPixels_fp dglReadPixels = NULL;
glScissor_fp dglScissor = NULL;
glShadeModel_fp dglShadeModel = NULL;
glTexCoord2f_fp dglTexCoord2f = NULL;
glTexCoord4fv_fp dglTexCoord4fv = NULL;
glTexCoordPointer_fp dglTexCoordPointer = NULL;
glTexEnvf_fp dglTexEnvf = NULL;
glTexImage2D_fp dglTexImage2D = NULL;
glTexParameteri_fp dglTexParameteri = NULL;
glTexSubImage2D_fp dglTexSubImage2D = NULL;
glVertex2i_fp dglVertex2i = NULL;
glVertex3f_fp dglVertex3f = NULL;
glVertex3fv_fp dglVertex3fv = NULL;
glVertexPointer_fp dglVertexPointer = NULL;
glViewport_fp dglViewport = NULL;
glGenLists_fp dglGenLists = NULL;
glNewList_fp dglNewList = NULL;
glEndList_fp dglEndList = NULL;
glCallLists_fp dglCallLists = NULL;
glListBase_fp dglListBase = NULL;
glBitmap_fp dglBitmap = NULL;
glPushAttrib_fp dglPushAttrib = NULL;
glPopAttrib_fp dglPopAttrib = NULL;
glColor3f_fp dglColor3f = NULL;
glRasterPos2i_fp dglRasterPos2i = NULL;

// bk000614 - instantiation of the symbols from the DLL
glAlphaFunc_fp dllAlphaFunc = NULL;
glBegin_fp dllBegin = NULL;
glBindTexture_fp dllBindTexture = NULL;
glBlendFunc_fp dllBlendFunc = NULL;
glClear_fp dllClear = NULL;
glClearColor_fp dllClearColor = NULL;
glClearDepth_fp dllClearDepth = NULL;
glColor3ub_fp dllColor3ub = NULL;
glColor4f_fp dllColor4f = NULL;
glColor4fv_fp dllColor4fv = NULL;
glColor4ub_fp dllColor4ub = NULL;
glColorPointer_fp dllColorPointer = NULL;
glDeleteTextures_fp dllDeleteTextures = NULL;
glDepthFunc_fp dllDepthFunc = NULL;
glDepthMask_fp dllDepthMask = NULL;
glDepthRange_fp dllDepthRange = NULL;
glDisable_fp dllDisable = NULL;
glDisableClientState_fp dllDisableClientState = NULL;
glDrawArrays_fp dllDrawArrays = NULL;
glDrawPixels_fp dllDrawPixels = NULL;
glEnable_fp dllEnable = NULL;
glEnableClientState_fp dllEnableClientState = NULL;
glEnd_fp dllEnd = NULL;
glFlush_fp dllFlush = NULL;
glFogf_fp dllFogf = NULL;
glFogfv_fp dllFogfv = NULL;
glFogi_fp dllFogi = NULL;
glGetError_fp dllGetError = NULL;
glGetString_fp dllGetString = NULL;
glGetIntegerv_fp dllGetIntegerv = NULL;
glHint_fp dllHint = NULL;
glLoadIdentity_fp dllLoadIdentity = NULL;
glMatrixMode_fp dllMatrixMode = NULL;
glOrtho_fp dllOrtho = NULL;
glPixelStorei_fp dllPixelStorei = NULL;
glPixelTransferi_fp dllPixelTransferi = NULL;
glPolygonOffset_fp dllPolygonOffset = NULL;
glReadPixels_fp dllReadPixels = NULL;
glScissor_fp dllScissor = NULL;
glShadeModel_fp dllShadeModel = NULL;
glTexCoord2f_fp dllTexCoord2f = NULL;
glTexCoord4fv_fp dllTexCoord4fv = NULL;
glTexCoordPointer_fp dllTexCoordPointer = NULL;
glTexEnvf_fp dllTexEnvf = NULL;
glTexImage2D_fp dllTexImage2D = NULL;
glTexParameteri_fp dllTexParameteri = NULL;
glTexSubImage2D_fp dllTexSubImage2D = NULL;
glVertex2i_fp dllVertex2i = NULL;
glVertex3f_fp dllVertex3f = NULL;
glVertex3fv_fp dllVertex3fv = NULL;
glVertexPointer_fp dllVertexPointer = NULL;
glViewport_fp dllViewport = NULL;

glGenLists_fp dllGenLists = NULL;
glNewList_fp dllNewList = NULL;
glEndList_fp dllEndList = NULL;
glCallLists_fp dllCallLists = NULL;
glListBase_fp dllListBase = NULL;
glBitmap_fp dllBitmap = NULL;
glPushAttrib_fp dllPushAttrib = NULL;
glPopAttrib_fp dllPopAttrib = NULL;
glColor3f_fp dllColor3f = NULL;
glRasterPos2i_fp dllRasterPos2i = NULL;

// ryan's add. 04/18/2000   (Should this be elsewhere?)
#if defined(__LINUX__)
extern char *__orig_pwd;
#include <unistd.h>
#include "SDL.h"

static char loadedLibrary[_MAX_PATH];

typedef void(GLFUNCCALL *PFNGLACTIVETEXTUREARBPROC)(GLenum texture);
typedef void(GLFUNCCALL *PFNGLCLIENTACTIVETEXTUREARBPROC)(GLenum texture);
typedef void(GLFUNCCALL *PFNGLMULTITEXCOORD4FARBPROC)(GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q);

// rcg09182000 don't check FPS.
#ifdef __CHECK_FOR_TOO_SLOW_RENDERING__
static bool nofpscheck = false;
#endif

#endif
// end ryan's add.

#ifdef __PERMIT_GL_LOGGING
void DGL_LogNewFrame(void);
void DGL_EnableLogging(int enable);
void DGL_Printf(char *fmt, ...);
bool __glLog = false;
#endif

#if defined(WIN32)
extern int WindowGL;
#else
#define WindowGL 0
#endif

static bool OpenGL_fog_enabled = true;

extern int FindArg(char *);

/*
#ifndef NDEBUG
        GLenum GL_error_code;
        const ubyte *GL_error_string;
        #define CHECK_ERROR(x)	{	GL_error_code=dglGetError ();\
                                                                        if (GL_error_code!=GL_NO_ERROR)\
                                                                        {\
                                                                                GL_error_string=gluErrorString
(GL_error_code);\
                                                                                mprintf ((0,"GL_ERROR: id=%d code=%d
String=%s\n",x,GL_error_code,GL_error_string));\
                                                                        }\
                                                                        }
#else
        #define CHECK_ERROR(x)
#endif*/

#define CHECK_ERROR(x)

#if defined(WIN32)
//	Moved from DDGR library
static HWND hOpenGLWnd = NULL;
static HDC hOpenGLDC = NULL;
HGLRC ResourceContext;
static WORD Saved_gamma_values[256 * 3];
#elif defined(__LINUX__)
// static Display *OpenGL_Display=NULL;
// static Window OpenGL_Window;
// static XVisualInfo OpenGL_VisualInfo;
// static GLXContext OpenGL_Context;
static bool OpenGL_TextureHack = false;
static bool OpenGL_UseLists = false;
static oeLnxApplication *OpenGL_LinuxApp = NULL;

// void CreateFullScreenWindow(Display *dpy,Window rootwin,Window window,int DisplayScreen,int DisplayWidth,int
// DisplayHeight);
/*
#define glXQueryExtension dglXQueryExtension
#define glXCreateContext dglXCreateContext
#define glXMakeCurrent dglXMakeCurrent
#define glXSwapBuffers dglXSwapBuffers
#define glXDestroyContext dglXDestroyContext
#define glXWaitGL dglXWaitGL
*/
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
#ifdef __LINUX__
#define UNSIGNED_SHORT_5_5_5_1_EXT GL_UNSIGNED_SHORT_5_5_5_1
#define UNSIGNED_SHORT_4_4_4_4_EXT GL_UNSIGNED_SHORT_4_4_4_4
#else
#define UNSIGNED_SHORT_5_5_5_1_EXT 0x8034
#define UNSIGNED_SHORT_4_4_4_4_EXT 0x8033
#endif

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

#if defined(__LINUX__) //(defined(_USE_OGL_ACTIVE_TEXTURES))
PFNGLACTIVETEXTUREARBPROC dglActiveTextureARB = NULL;
PFNGLCLIENTACTIVETEXTUREARBPROC dglClientActiveTextureARB = NULL;
PFNGLMULTITEXCOORD4FARBPROC dglMultiTexCoord4f = NULL;

PFNGLACTIVETEXTUREARBPROC dllActiveTextureARB = NULL;
PFNGLCLIENTACTIVETEXTUREARBPROC dllClientActiveTextureARB = NULL;
PFNGLMULTITEXCOORD4FARBPROC dllMultiTexCoord4f = NULL;
#endif

static module OpenGLDLLInst;

#ifdef __LINUX__
static void *__SDL_mod_GetSymbol(char *funcStr) {
  void *retVal = NULL;

  mprintf((0, "Looking up GL function [%s]...", funcStr));

  retVal = SDL_GL_GetProcAddress(funcStr);

  if (retVal == NULL)
    fprintf(stderr, " Could not find GL symbol [%s]!\n\n", funcStr);
  else {
    mprintf((0, "Found at (%p).", retVal));
  } // else

  return (retVal);
} // __SDL_mod_GetSymbol

/****************** WARNING: NASTY HACK! ***********************/
#define mod_GetSymbol(x, funcStr, y) __SDL_mod_GetSymbol(funcStr)
/****************** WARNING: NASTY HACK! ***********************/

#endif

module *LoadOpenGLDLL(char *dllname) {
  _D(("LoadOpenGLDLL(\"%s\");", dllname));

  mprintf((0, "OpenGL: Loading OpenGL dll [%s]...\n", dllname));

#ifdef __LINUX__
  char *tmp = getcwd(NULL, 0);
  chdir(__orig_pwd);
  int rc = SDL_GL_LoadLibrary(dllname);
  chdir(tmp);
  free(tmp);
  if (rc < 0) {
    char *sdlErr = SDL_GetError();
    mprintf((0, "OpenGL: Couldn't open library [%s].\n", dllname));
    mprintf((0, "OpenGL:  SDL error is [%s].", sdlErr));
    return NULL;
  }

  strcpy(loadedLibrary, dllname);

#else
  if (!mod_LoadModule(&OpenGLDLLInst, dllname, MODF_LAZY | MODF_GLOBAL)) {
    int err = mod_GetLastError();
    mprintf((0, "OpenGL: Couldn't open module called %s. err==%d.", dllname, err));
    return NULL;
  }
#endif

  dglAlphaFunc = dllAlphaFunc = (glAlphaFunc_fp)mod_GetSymbol(&OpenGLDLLInst, "glAlphaFunc", 255);
  if (!dglAlphaFunc)
    goto dll_error;

  dglBegin = dllBegin = (glBegin_fp)mod_GetSymbol(&OpenGLDLLInst, "glBegin", 255);
  if (!dglBegin)
    goto dll_error;

  dglBindTexture = dllBindTexture = (glBindTexture_fp)mod_GetSymbol(&OpenGLDLLInst, "glBindTexture", 255);
  if (!dglBindTexture)
    goto dll_error;

  dglBlendFunc = dllBlendFunc = (glBlendFunc_fp)mod_GetSymbol(&OpenGLDLLInst, "glBlendFunc", 255);
  if (!dglBlendFunc)
    goto dll_error;

  dglClear = dllClear = (glClear_fp)mod_GetSymbol(&OpenGLDLLInst, "glClear", 255);
  if (!dglClear)
    goto dll_error;

  dglClearColor = dllClearColor = (glClearColor_fp)mod_GetSymbol(&OpenGLDLLInst, "glClearColor", 255);
  if (!dglClearColor)
    goto dll_error;

  dglClearDepth = dllClearDepth = (glClearDepth_fp)mod_GetSymbol(&OpenGLDLLInst, "glClearDepth", 255);
  if (!dglClearDepth)
    goto dll_error;

  dglColor3ub = dllColor3ub = (glColor3ub_fp)mod_GetSymbol(&OpenGLDLLInst, "glColor3ub", 255);
  if (!dglColor3ub)
    goto dll_error;

  dglColor4ub = dllColor4ub = (glColor4ub_fp)mod_GetSymbol(&OpenGLDLLInst, "glColor4ub", 255);
  if (!dglColor4ub)
    goto dll_error;

  dglColor4f = dllColor4f = (glColor4f_fp)mod_GetSymbol(&OpenGLDLLInst, "glColor4f", 255);
  if (!dglColor4f)
    goto dll_error;

  dglColorPointer = dllColorPointer = (glColorPointer_fp)mod_GetSymbol(&OpenGLDLLInst, "glColorPointer", 255);
  if (!dglColorPointer)
    goto dll_error;

  dglDeleteTextures = dllDeleteTextures = (glDeleteTextures_fp)mod_GetSymbol(&OpenGLDLLInst, "glDeleteTextures", 255);
  if (!dglDeleteTextures)
    goto dll_error;

  dglDepthFunc = dllDepthFunc = (glDepthFunc_fp)mod_GetSymbol(&OpenGLDLLInst, "glDepthFunc", 255);
  if (!dglDepthFunc)
    goto dll_error;

  dglDepthMask = dllDepthMask = (glDepthMask_fp)mod_GetSymbol(&OpenGLDLLInst, "glDepthMask", 255);
  if (!dglDepthMask)
    goto dll_error;

  dglDepthRange = dllDepthRange = (glDepthRange_fp)mod_GetSymbol(&OpenGLDLLInst, "glDepthRange", 255);
  if (!dglDepthRange)
    goto dll_error;

  dglDisable = dllDisable = (glDisable_fp)mod_GetSymbol(&OpenGLDLLInst, "glDisable", 255);
  if (!dglDisable)
    goto dll_error;

  dglDisableClientState = dllDisableClientState =
      (glDisableClientState_fp)mod_GetSymbol(&OpenGLDLLInst, "glDisableClientState", 255);
  if (!dglDisableClientState)
    goto dll_error;

  dglDrawArrays = dllDrawArrays = (glDrawArrays_fp)mod_GetSymbol(&OpenGLDLLInst, "glDrawArrays", 255);
  if (!dglDrawArrays)
    goto dll_error;

  dglDrawPixels = dllDrawPixels = (glDrawPixels_fp)mod_GetSymbol(&OpenGLDLLInst, "glDrawPixels", 255);
  if (!dglDrawPixels)
    goto dll_error;

  dglEnable = dllEnable = (glEnable_fp)mod_GetSymbol(&OpenGLDLLInst, "glEnable", 255);
  if (!dglEnable)
    goto dll_error;

  dglEnableClientState = dllEnableClientState =
      (glEnableClientState_fp)mod_GetSymbol(&OpenGLDLLInst, "glEnableClientState", 255);
  if (!dglEnableClientState)
    goto dll_error;

  dglEnd = dllEnd = (glEnd_fp)mod_GetSymbol(&OpenGLDLLInst, "glEnd", 255);
  if (!dglEnd)
    goto dll_error;

  dglFlush = dllFlush = (glFlush_fp)mod_GetSymbol(&OpenGLDLLInst, "glFlush", 255);
  if (!dglFlush)
    goto dll_error;

  dglFogf = dllFogf = (glFogf_fp)mod_GetSymbol(&OpenGLDLLInst, "glFogf", 255);
  if (!dglFogf)
    goto dll_error;

  dglFogfv = dllFogfv = (glFogfv_fp)mod_GetSymbol(&OpenGLDLLInst, "glFogfv", 255);
  if (!dglFogfv)
    goto dll_error;

  dglFogi = dllFogi = (glFogi_fp)mod_GetSymbol(&OpenGLDLLInst, "glFogi", 255);
  if (!dglFogi)
    goto dll_error;

  dglGetError = dllGetError = (glGetError_fp)mod_GetSymbol(&OpenGLDLLInst, "glGetError", 255);
  if (!dglGetError)
    goto dll_error;

  dglGetString = dllGetString = (glGetString_fp)mod_GetSymbol(&OpenGLDLLInst, "glGetString", 255);
  if (!dglGetString)
    goto dll_error;

  dglHint = dllHint = (glHint_fp)mod_GetSymbol(&OpenGLDLLInst, "glHint", 255);
  if (!dglHint)
    goto dll_error;

  dglLoadIdentity = dllLoadIdentity = (glLoadIdentity_fp)mod_GetSymbol(&OpenGLDLLInst, "glLoadIdentity", 255);
  if (!dglLoadIdentity)
    goto dll_error;

  dglMatrixMode = dllMatrixMode = (glMatrixMode_fp)mod_GetSymbol(&OpenGLDLLInst, "glMatrixMode", 255);
  if (!dglMatrixMode)
    goto dll_error;

  dglOrtho = dllOrtho = (glOrtho_fp)mod_GetSymbol(&OpenGLDLLInst, "glOrtho", 255);
  if (!dglOrtho)
    goto dll_error;

  dglPixelStorei = dllPixelStorei = (glPixelStorei_fp)mod_GetSymbol(&OpenGLDLLInst, "glPixelStorei", 255);
  if (!dglPixelStorei)
    goto dll_error;

  dglPixelTransferi = dllPixelTransferi = (glPixelTransferi_fp)mod_GetSymbol(&OpenGLDLLInst, "glPixelTransferi", 255);
  if (!dglPixelTransferi)
    goto dll_error;

  dglPolygonOffset = dllPolygonOffset = (glPolygonOffset_fp)mod_GetSymbol(&OpenGLDLLInst, "glPolygonOffset", 255);
  if (!dglPolygonOffset)
    goto dll_error;

  dglReadPixels = dllReadPixels = (glReadPixels_fp)mod_GetSymbol(&OpenGLDLLInst, "glReadPixels", 255);
  if (!dglReadPixels)
    goto dll_error;

  dglScissor = dllScissor = (glScissor_fp)mod_GetSymbol(&OpenGLDLLInst, "glScissor", 255);
  if (!dglScissor)
    goto dll_error;

  dglShadeModel = dllShadeModel = (glShadeModel_fp)mod_GetSymbol(&OpenGLDLLInst, "glShadeModel", 255);
  if (!dglShadeModel)
    goto dll_error;

  dglTexCoordPointer = dllTexCoordPointer =
      (glTexCoordPointer_fp)mod_GetSymbol(&OpenGLDLLInst, "glTexCoordPointer", 255);
  if (!dglTexCoordPointer)
    goto dll_error;

  dglTexEnvf = dllTexEnvf = (glTexEnvf_fp)mod_GetSymbol(&OpenGLDLLInst, "glTexEnvf", 255);
  if (!dglTexEnvf)
    goto dll_error;

  dglTexImage2D = dllTexImage2D = (glTexImage2D_fp)mod_GetSymbol(&OpenGLDLLInst, "glTexImage2D", 255);
  if (!dglTexImage2D)
    goto dll_error;

  dglTexParameteri = dllTexParameteri = (glTexParameteri_fp)mod_GetSymbol(&OpenGLDLLInst, "glTexParameteri", 255);
  if (!dglTexParameteri)
    goto dll_error;

  dglTexSubImage2D = dllTexSubImage2D = (glTexSubImage2D_fp)mod_GetSymbol(&OpenGLDLLInst, "glTexSubImage2D", 255);
  if (!dglTexSubImage2D)
    goto dll_error;

  dglVertex2i = dllVertex2i = (glVertex2i_fp)mod_GetSymbol(&OpenGLDLLInst, "glVertex2i", 255);
  if (!dglVertex2i)
    goto dll_error;

  dglVertex3f = dllVertex3f = (glVertex3f_fp)mod_GetSymbol(&OpenGLDLLInst, "glVertex3f", 255);
  if (!dglVertex3f)
    goto dll_error;

  dglVertexPointer = dllVertexPointer = (glVertexPointer_fp)mod_GetSymbol(&OpenGLDLLInst, "glVertexPointer", 255);
  if (!dglVertexPointer)
    goto dll_error;

  dglViewport = dllViewport = (glViewport_fp)mod_GetSymbol(&OpenGLDLLInst, "glViewport", 255);
  if (!dglViewport)
    goto dll_error;

  dglColor4fv = dllColor4fv = (glColor4fv_fp)mod_GetSymbol(&OpenGLDLLInst, "glColor4fv", 255);
  if (!dglColor4fv)
    goto dll_error;

  dglVertex3fv = dllVertex3fv = (glVertex3fv_fp)mod_GetSymbol(&OpenGLDLLInst, "glVertex3fv", 255);
  if (!dglVertex3fv)
    goto dll_error;

  dglTexCoord4fv = dllTexCoord4fv = (glTexCoord4fv_fp)mod_GetSymbol(&OpenGLDLLInst, "glTexCoord4fv", 255);
  if (!dglTexCoord4fv)
    goto dll_error;

  dglTexCoord2f = dllTexCoord2f = (glTexCoord2f_fp)mod_GetSymbol(&OpenGLDLLInst, "glTexCoord2f", 255);
  if (!dglTexCoord2f)
    goto dll_error;

  dglGetIntegerv = dllGetIntegerv = (glGetIntegerv_fp)mod_GetSymbol(&OpenGLDLLInst, "glGetIntegerv", 255);
  if (!dglGetIntegerv)
    goto dll_error;

  dglGenLists = dllGenLists = (glGenLists_fp)mod_GetSymbol(&OpenGLDLLInst, "glGenLists", 255);
  if (!dglGenLists)
    goto dll_error;

  dglNewList = dllNewList = (glNewList_fp)mod_GetSymbol(&OpenGLDLLInst, "glNewList", 255);
  if (!dglNewList)
    goto dll_error;

  dglEndList = dllEndList = (glEndList_fp)mod_GetSymbol(&OpenGLDLLInst, "glEndList", 255);
  if (!dglEndList)
    goto dll_error;

  dglCallLists = dllCallLists = (glCallLists_fp)mod_GetSymbol(&OpenGLDLLInst, "glCallLists", 255);
  if (!dglCallLists)
    goto dll_error;

  dglListBase = dllListBase = (glListBase_fp)mod_GetSymbol(&OpenGLDLLInst, "glListBase", 255);
  if (!dglListBase)
    goto dll_error;

  dglBitmap = dllBitmap = (glBitmap_fp)mod_GetSymbol(&OpenGLDLLInst, "glBitmap", 255);
  if (!dglBitmap)
    goto dll_error;

  dglPushAttrib = dllPushAttrib = (glPushAttrib_fp)mod_GetSymbol(&OpenGLDLLInst, "glPushAttrib", 255);
  if (!dglPushAttrib)
    goto dll_error;

  dglPopAttrib = dllPopAttrib = (glPopAttrib_fp)mod_GetSymbol(&OpenGLDLLInst, "glPopAttrib", 255);
  if (!dglPopAttrib)
    goto dll_error;

  dglColor3f = dllColor3f = (glColor3f_fp)mod_GetSymbol(&OpenGLDLLInst, "glColor3f", 255);
  if (!dglColor3f)
    goto dll_error;

  dglRasterPos2i = dllRasterPos2i = (glRasterPos2i_fp)mod_GetSymbol(&OpenGLDLLInst, "glRasterPos2i", 255);
  if (!dglRasterPos2i)
    goto dll_error;

#if defined(WIN32)
  dwglCreateContext = (wglCreateContext_fp)mod_GetSymbol(&OpenGLDLLInst, "wglCreateContext", 255);
  if (!dwglCreateContext)
    goto dll_error;

  dwglDeleteContext = (wglDeleteContext_fp)mod_GetSymbol(&OpenGLDLLInst, "wglDeleteContext", 255);
  if (!dwglDeleteContext)
    goto dll_error;

  dwglMakeCurrent = (wglMakeCurrent_fp)mod_GetSymbol(&OpenGLDLLInst, "wglMakeCurrent", 255);
  if (!dwglMakeCurrent)
    goto dll_error;

  dwglGetProcAddress = (wglGetProcAddress_fp)mod_GetSymbol(&OpenGLDLLInst, "wglGetProcAddress", 255);
  if (!dwglGetProcAddress)
    goto dll_error;

#elif defined(__LINUX__)

    /* God I feel fuckin' dirty. */
#undef mod_GetSymbol

/*
        dglXQueryExtension = (glXQueryExtension_fp)mod_GetSymbol(&OpenGLDLLInst,"glXQueryExtension",255);
        if(!dglXQueryExtension) goto dll_error;

        dglXCreateContext = (glXCreateContext_fp)mod_GetSymbol(&OpenGLDLLInst,"glXCreateContext",255);
        if(!dglXCreateContext) goto dll_error;

        dglXMakeCurrent = (glXMakeCurrent_fp)mod_GetSymbol(&OpenGLDLLInst,"glXMakeCurrent",255);
        if(!dglXMakeCurrent) goto dll_error;

        dglXSwapBuffers = (glXSwapBuffers_fp)mod_GetSymbol(&OpenGLDLLInst,"glXSwapBuffers",255);
        if(!dglXSwapBuffers) goto dll_error;

        dglXGetConfig = (glXGetConfig_fp)mod_GetSymbol(&OpenGLDLLInst,"glXGetConfig",255);
        if(!dglXGetConfig) goto dll_error;

        dglXChooseVisual = (glXChooseVisual_fp)mod_GetSymbol(&OpenGLDLLInst,"glXChooseVisual",255);
        if(!dglXChooseVisual) goto dll_error;

        dglXDestroyContext = (glXDestroyContext_fp)mod_GetSymbol(&OpenGLDLLInst,"glXDestroyContext",255);
        if(!dglXDestroyContext) goto dll_error;

        dglXWaitGL = (glXWaitGL_fp)mod_GetSymbol(&OpenGLDLLInst,"glXWaitGL",255);
        if(!dglXWaitGL) goto dll_error;
*/
#else
#endif

  mprintf((0, "OpenGL: DLL loading successful.\n"));
  return &OpenGLDLLInst;

dll_error:
  mprintf((0, "OpenGL: Error loading DLL!\n"));

#ifndef __LINUX__
  mod_FreeModule(&OpenGLDLLInst);
#endif

  return NULL;
}

// Sets up multitexturing using ARB extensions
void opengl_GetDLLFunctions() {
  _D(("opengl_GetDLLFunctions();"));
#if defined(WIN32)
  dglActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC)dwglGetProcAddress("glActiveTextureARB");
  if (!dglActiveTextureARB)
    goto dll_error;

  dglClientActiveTextureARB = (PFNGLCLIENTACTIVETEXTUREARBPROC)dwglGetProcAddress("glClientActiveTextureARB");
  if (!dglClientActiveTextureARB)
    goto dll_error;

  dglMultiTexCoord4f = (PFNGLMULTITEXCOORD4FARBPROC)dwglGetProcAddress("glMultiTexCoord4f");
  if (!dglMultiTexCoord4f)
    goto dll_error;

  return;

dll_error:
#endif

#if (!defined(_USE_OGL_ACTIVE_TEXTURES))
  OpenGL_multitexture = 0;
#endif

  ; // need something here for win32 compiles.
}

// returns true if the passed in extension name is supported
int opengl_CheckExtension(char *extName) {
  _D(("opengl_checkExtension(\"%s\");", extName));

  char *p = (char *)dglGetString(GL_EXTENSIONS);
  char *end;
  int extNameLen;

  extNameLen = strlen(extName);
  end = p + strlen(p);

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
  _D(("opengl_GetInformation();"));

  mprintf((1, "OpenGL Vendor: %s", dglGetString(GL_VENDOR)));
  mprintf((1, "OpenGL Renderer: %s", dglGetString(GL_RENDERER)));
  mprintf((1, "OpenGL Version: %s", dglGetString(GL_VERSION)));
  mprintf((0, "OpenGL Extensions: %s", dglGetString(GL_EXTENSIONS)));

// rcg11292000 added glinfo...
#ifdef __LINUX__
  if (FindArgChar("-glinfo", 'I')) {
    printf("OpenGL information follows...\n");
    printf(" * Library we opened (may be in path): %s\n", loadedLibrary);
    printf(" * OpenGL Vendor: %s\n", dglGetString(GL_VENDOR));
    printf(" * OpenGL Renderer: %s\n", dglGetString(GL_RENDERER));
    printf(" * OpenGL Version: %s\n", dglGetString(GL_VERSION));
    printf(" * OpenGL Extensions: %s\n", dglGetString(GL_EXTENSIONS));
  } // if
#endif

  /*	#ifndef RELEASE
                  // If this is the microsoft driver, then make stuff go faster
                  const ubyte *renderer=dglGetString(GL_RENDERER);
                  if (!(strnicmp ((const char *)renderer,"GDI",3)))
                          Fast_test_render=1;
                  else
                          Fast_test_render=0;
          #endif
  */
}

void opengl_GetVersion(int *major, int *minor) {
  _D(("opengl_GetVersion(%p, %p);", major, minor));

  if ((major == NULL) || (minor == NULL))
    return;

  const unsigned char *glVer = dglGetString(GL_VERSION);
  unsigned char buffer[25];
  unsigned char *ptr;
  int i = 0;

  for (ptr = (unsigned char *)glVer; ((*ptr != '\0') && (*ptr != '.')); ptr++) {
    buffer[i] = *ptr;
    i++;
  } // for

  buffer[i] = '\0'; // terminate string.
  *major = atoi((const char *)buffer);

  if (*ptr == '\0') //?!
    *minor = 0;
  else {
    ptr++; // get past '.'
    *minor = atoi((const char *)ptr);
  } // else
} // opengl_GetVersion

int opengl_MakeTextureObject(int tn) {
  _D(("opengl_MakeTextureObject(%d);", tn));

  int num = Cur_texture_object_num;

  Cur_texture_object_num++;

#if (defined(_USE_OGL_ACTIVE_TEXTURES))
  if (OpenGL_multitexture && Last_texel_unit_set != tn) {
    dglActiveTextureARB((GLenum)(GL_TEXTURE0_ARB + tn));
    Last_texel_unit_set = tn;
  }
#endif

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
  _D(("opengl_initCache();"));

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

#if (defined(_USE_OGL_ACTIVE_TEXTURES))
  if (OpenGL_multitexture) {
    dglActiveTextureARB((GLenum)(GL_TEXTURE0_ARB + 1));
    dglTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    dglActiveTextureARB((GLenum)(GL_TEXTURE0_ARB + 0));
  }
#endif

  CHECK_ERROR(3)

  OpenGL_cache_initted = 1;
  return 1;
}

// Sets default states for our renderer
void opengl_SetDefaults() {
  _D(("opengl_SetDefaults();"));

  mprintf((0, "OpenGL: Setting states\n"));

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

  opengl_SetAlphaType(AT_ALWAYS);
  opengl_SetAlphaValue(255);
  opengl_SetFiltering(1);
  opengl_SetLightingState(LS_NONE);
  opengl_SetTextureType(TT_FLAT);
  opengl_SetColorModel(CM_RGB);
  opengl_SetZBufferState(1);
  opengl_SetZValues(0, 3000);
  opengl_SetGammaValue(OpenGL_preferred_state.gamma);
  OpenGL_last_bound[0] = 9999999;
  OpenGL_last_bound[1] = 9999999;
  Last_texel_unit_set = -1;
  OpenGL_multitexture_state = false;

#if (defined(_USE_OGL_LISTS_OPTIONAL))
  OpenGL_UseLists = (FindArg("-gllists")) ? true : false;
  mprintf((0, "Use GL lists: %s.", OpenGL_UseLists ? "Enabled" : "Disabled"));
  if (OpenGL_UseLists) {
#endif
    dglEnableClientState(GL_VERTEX_ARRAY);
    dglEnableClientState(GL_COLOR_ARRAY);
    dglEnableClientState(GL_TEXTURE_COORD_ARRAY);

    dglVertexPointer(3, GL_FLOAT, 0, GL_verts);
    dglColorPointer(4, GL_FLOAT, 0, GL_colors);
    dglTexCoordPointer(4, GL_FLOAT, 0, GL_tex_coords);
#if (defined(_USE_OGL_LISTS_OPTIONAL))
  }
#endif

  dglHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
  dglHint(GL_FOG_HINT, GL_FASTEST);
  dglEnable(GL_SCISSOR_TEST);
  dglScissor(0, 0, OpenGL_state.screen_width, OpenGL_state.screen_height);
  dglDisable(GL_SCISSOR_TEST);
  dglDepthRange(0.0f, 1.0f);

#if (defined(_USE_OGL_ACTIVE_TEXTURES))
  if (OpenGL_multitexture) {
    dglActiveTextureARB((GLenum)(GL_TEXTURE0_ARB + 1));
    dglClientActiveTextureARB((GLenum)(GL_TEXTURE0_ARB + 1));
    dglEnableClientState(GL_TEXTURE_COORD_ARRAY);
    dglTexCoordPointer(4, GL_FLOAT, 0, GL_tex_coords2);
    dglHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    dglHint(GL_FOG_HINT, GL_FASTEST);

    dglClientActiveTextureARB((GLenum)(GL_TEXTURE0_ARB + 0));

    dglDisable(GL_TEXTURE_2D);
    dglAlphaFunc(GL_GREATER, 0);
    dglEnable(GL_ALPHA_TEST);
    dglEnable(GL_BLEND);
    dglEnable(GL_DITHER);
    dglBlendFunc(GL_DST_COLOR, GL_ZERO);
    dglActiveTextureARB((GLenum)(GL_TEXTURE0_ARB + 0));
  }
#endif
}

#if defined(WIN32)
// Check for OpenGL support,
int opengl_Setup(HDC glhdc) {
  _D(("opengl_Setup();"));
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
/*
bool opengl_GetXConfig(Display *dpy,XVisualInfo *vis,int attrib,int *value)
{
        int res;

        res = dglXGetConfig( dpy, vis , attrib, value );

        if(res==0)
                return true;

        switch(res)
        {
        case GLX_NO_EXTENSION:
                fprintf(stdout,"GLX: Display does not support the GLX extension (%d)\n",attrib);
                break;
        case GLX_BAD_SCREEN:
                fprintf(stdout,"GLX: The screen of the vis does not correspond to a screen (%d)\n",attrib);
                break;
        case GLX_BAD_ATTRIBUTE:
                fprintf(stdout,"GLX: Bad Attribute (%d\%d)\n",attrib,*value);
                break;
        case GLX_BAD_VISUAL:
                fprintf(stdout,"GLX: Bad Visual on non GLX_USE_GL call (%d)\n",attrib);
                break;
        default:
                fprintf(stdout,"GLX: Unknown error (%d)\n",attrib);
                break;
        }

        return false;
}
*/

#ifdef __CHECK_FOR_TOO_SLOW_RENDERING__
static long minimumAcceptableRender = -1;
static Uint32 lastSwapTicks = 0;
static int tooSlowCount = 0;
static int tooSlowChecksLeft = 0;

void setMinimumAcceptableRenderTime(int ms) {
  if (nofpscheck)
    return;

  minimumAcceptableRender = ms;
  lastSwapTicks = SDL_GetTicks();
  tooSlowCount = 0;
  tooSlowChecksLeft = 15; // check 15 frames.
}
#endif

extern bool linux_permit_gamma;
extern renderer_preferred_state Render_preferred_state;

int d3SDLEventFilter(const SDL_Event *event);

int opengl_Setup(oeApplication *app, int *width, int *height) {
  _D(("opengl_Setup(%p, %p, %p);", app, width, height));

// rcg11192000 don't check for FPS.
#ifdef __CHECK_FOR_TOO_SLOW_RENDERING__
  nofpscheck = (FindArgChar("-nofpscheck", 'H') != 0);

  // rcg01252000 reset these, so they don't barf if we change
  // video modes for a movie or whatnot in the middle of
  // checking rendering speed...
  minimumAcceptableRender = -1;
  lastSwapTicks = 0;
  tooSlowCount = 0;
  tooSlowChecksLeft = 0;
#endif

  // rcg09182000 don't need to quitsubsystem anymore...
  //    SDL_QuitSubSystem(SDL_INIT_VIDEO);  // here goes nothing...
  //    Already_loaded = false;
  SDL_ClearError();
  int rc = SDL_Init(SDL_INIT_VIDEO);
  if (rc != 0) {
    char buffer[512];
    snprintf(buffer, sizeof(buffer), "SDL_GetError() reports \"%s\".\n", SDL_GetError());
    fprintf(stderr, "SDL: SDL_Init() failed! rc == (%d).\n", rc);
    fprintf(stderr, "%s", buffer);
    rend_SetErrorMessage(buffer);
    return (0);
  } // if
  SDL_SetEventFilter(d3SDLEventFilter);

  bool fullscreen = false;
  char *env = getenv("MESA_GLX_FX");

  if ((!env) || (*env == 'f')) // Full screen Mesa mode    !!! needs more.
    fullscreen = true;

  if (FindArgChar("-fullscreen", 'f'))
    fullscreen = true;
  else if (FindArgChar("-windowed", 'w'))
    fullscreen = false;

  if (env == NULL)
    putenv((char *)(fullscreen ? "MESA_GLX_FX=f" : "MESA_GLX_FX=w"));

  putenv("FX_GLIDE_NO_SPLASH=1");

  if (!Already_loaded) {
#define MAX_ARGS 30
#define MAX_CHARS_PER_ARG 100
    extern char GameArgs[MAX_ARGS][MAX_CHARS_PER_ARG];

    char gl_library[256];
    int arg;
    arg = FindArgChar("-gllibrary", 'g');
    if (arg != 0)
      strcpy(gl_library, GameArgs[arg + 1]);
    else
      strcpy(gl_library, "libGL.so");

    mprintf((0, "OpenGL: Attempting to use \"%s\" for OpenGL\n", gl_library));
    putenv("MESA_FX_NO_SIGNALS=ihatesegfaults");

    // ryan's adds. 04/18/2000...SDL stuff on 04/25/2000
    bool success = true;

    OpenGLDLLHandle = LoadOpenGLDLL(gl_library);
    if (!(OpenGLDLLHandle)) {
      // rcg07072000 last ditch effort...
      OpenGLDLLHandle = LoadOpenGLDLL("libGL.so.1");
      if (!(OpenGLDLLHandle))
        success = false;
    } // if

    if (success == false) {
      char buffer[512];
      sprintf(buffer, "Failed to load library [%s].\n", gl_library);
      fprintf(stderr, buffer);
      rend_SetErrorMessage(buffer);
      //			Int3();
      return 0;
    } // if
  }

#ifdef __PERMIT_GL_LOGGING
  if (FindArg("-gllogging")) {
    printf("\n"
           "************************************************************\n"
           "************************************************************\n"
           "************************************************************\n"
           "************************************************************\n"
           "************************************************************\n"
           "********   GL LOGGING ENABLED.   ***************************\n"
           "************************************************************\n"
           "************************************************************\n"
           "************************************************************\n"
           "************************************************************\n"
           "************************************************************\n"
           "\n");
    DGL_EnableLogging(1);
    __glLog = true;
  } // if
#endif

  OpenGL_TextureHack = (FindArg("-gltexturehack")) ? true : false;
  mprintf((0, "Linux texture hack: %s.", OpenGL_TextureHack ? "Enabled" : "Disabled"));

  // Thanks to the Heavy Gear II team for doing
  //  the fuckwork before I had to.  :)  --ryan.

  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

  int flags = SDL_OPENGL;

  if (fullscreen)
    flags |= SDL_FULLSCREEN;

  // Zany fun.

  // ShowCursor(0) and input grabbing need to be done before setting
  //  the video mode, or the Voodoo 3 gets a hardware cursor stuck
  //  on the screen.
  SDL_ShowCursor(0);
  SDL_WM_GrabInput(SDL_GRAB_ON);

  // Should we shoot for 32-bpp if available?  !!!
  SDL_Surface *surface = SDL_SetVideoMode(*width, *height, 32, flags);
  mprintf((0, "OpenGL: SDL GL surface is %sNULL.", (surface == NULL) ? "" : "NOT "));

  if (ddio_mouseGrabbed == false)
    SDL_WM_GrabInput(SDL_GRAB_OFF);

  SDL_WM_SetCaption("Descent 3", "Descent3");

  if (app)
    OpenGL_LinuxApp = (oeLnxApplication *)app;

  // rcg09182000 gamma fun.
  // rcg01112000 --nogamma fun.
  if (FindArgChar("-nogamma", 'M'))
    linux_permit_gamma = false;
  else {
    float f = Render_preferred_state.gamma;
    bool gammarc = SDL_SetGamma(f, f, f);
    linux_permit_gamma = (gammarc == 0);
  } // else

  if (OpenGL_LinuxApp) {
    OpenGL_LinuxApp->set_sizepos(0, 0, *width, *height);
    //    	OpenGL_LinuxApp->set_windowinfo(OpenGL_Display,OpenGL_Window);
    OpenGL_LinuxApp->clear_window();
  } // if

  /*
          int dummy;

          if(app)
          {
                  tLnxAppInfo app_info;
                  OpenGL_LinuxApp = (oeLnxApplication *)app;

                  app->get_info((void *)&app_info);

                  OpenGL_Display = app_info.m_Display;
                  OpenGL_Window = app_info.m_window;
                  memcpy(&OpenGL_VisualInfo,&app_info.m_VisualInfo,sizeof(XVisualInfo));
          }
      else
          {
                  // since the application object wasn't passed in, make sure
                  // this isn't the first init
                  if(!OpenGL_Display)
                  {
                          // ACK!
                          fprintf(stdout,"OGL: Error in opengl_Setup().  Application object not specified\n");
                          return 0;
                  }
          }

          // we need to destroy the OpenGL window
          XUnmapWindow(OpenGL_Display,OpenGL_Window);
          XDestroyWindow(OpenGL_Display,OpenGL_Window);

          // Make sure OpenGL's GLX extension is supported.  The glXQueryExtension also returns
          // the GLX extension's error base and event base.  For almost all OpenGL programs,
          // this information is irrelevant.
          if(!glXQueryExtension(OpenGL_Display,&dummy,&dummy))
          {
                  fprintf(stderr,"GLX extension not supported by Display\n");
                  Int3();
                  return 0;
          }

          // Choose our visual
          int screen_num = DefaultScreen(OpenGL_Display);
          int vis_attrib[] = {GLX_RGBA,
                                          GLX_RED_SIZE, 1,
                                          GLX_GREEN_SIZE, 1,
                                          GLX_BLUE_SIZE, 1 ,
                                          GLX_DEPTH_SIZE, 16,
                                          GLX_DOUBLEBUFFER,
                                          None};
          XVisualInfo *new_vis = dglXChooseVisual(OpenGL_Display,screen_num,vis_attrib);
          if(!new_vis)
          {
                  fprintf(stdout,"OpenGL: glXChooseVisual returned NULL\n");
                  Int3();
                  return 0;
          }

          // Create an OpenGL rendering context
          OpenGL_Context = glXCreateContext(OpenGL_Display,new_vis,NULL,True);
          if(OpenGL_Context==NULL)
          {
                  fprintf(stderr,"OpenGL: Unable to create GLX Context\n");
                  Int3();
                  return 0;
          }

          // Create a new window
          XSetWindowAttributes swa;
          swa.override_redirect = true;
          swa.border_pixel = 0;
          swa.event_mask =
  ExposureMask|StructureNotifyMask|KeyPressMask|KeyReleaseMask|PointerMotionMask|ButtonPressMask|ButtonReleaseMask;

  //	OpenGL_Window =
  XCreateWindow(OpenGL_Display,RootWindow(OpenGL_Display,OpenGL_VisualInfo.screen),0,0,*width,*height,0,OpenGL_VisualInfo.depth,
  //					InputOutput,OpenGL_VisualInfo.visual,CWBorderPixel|CWEventMask,&swa);

          XSizeHints sizeHints = {0};
          sizeHints.flags |= USSize|USPosition|PAspect;
          sizeHints.width = *width;
          sizeHints.height = *height;
          sizeHints.x = 0;
          sizeHints.y = 0;
          sizeHints.min_aspect.x = sizeHints.max_aspect.x = *width;
          sizeHints.min_aspect.y = sizeHints.max_aspect.y = *height;

  #if 0
      char *argv[1];
          XWMHints *wmHints;
          Atom wmDeleteWindow;

          argv[0] = strdup("opengl");

          XSetStandardProperties(OpenGL_Display,OpenGL_Window,"","",None,(char **)argv,0,&sizeHints);
          free(argv[0]);

          wmHints = XAllocWMHints();
          wmHints->initial_state = NormalState;
          wmHints->flags = StateHint;

          XSetWMHints(OpenGL_Display,OpenGL_Window,wmHints);
          wmDeleteWindow = XInternAtom(OpenGL_Display,"WM_DELETE_WINDOW",False);
          XSetWMProtocols(OpenGL_Display,OpenGL_Window,&wmDeleteWindow,1);

          // move and resize the application window
          XMoveResizeWindow(OpenGL_Display,OpenGL_Window,0,0,*width,*height);

          OpenGL_LinuxApp->set_sizepos(0,0,*width,*height);
          OpenGL_LinuxApp->set_windowinfo(OpenGL_Display,OpenGL_Window);
          OpenGL_LinuxApp->hide_mouse();
          OpenGL_LinuxApp->clear_window();

          //	warp the mouse to 0,0 to start so our screen is in full view
          XWarpPointer(OpenGL_Display,None,OpenGL_Window,0,0,0,0,*width/2,*height/2);

          XStoreName(OpenGL_Display, OpenGL_Window, OpenGL_LinuxApp->get_window_name());
          XSetIconName(OpenGL_Display, OpenGL_Window, OpenGL_LinuxApp->get_window_name());

          XMapWindow(OpenGL_Display,OpenGL_Window);

          bool wait_for_draw = false;
          XEvent event;
          while (!wait_for_draw)
          {
                  XNextEvent(OpenGL_Display, &event);
                  if (event.type == Expose && !event.xexpose.count)
                          wait_for_draw = true;
          }

          XRaiseWindow(OpenGL_Display,OpenGL_Window);

  #endif

          // Bind the rendering context
          glXMakeCurrent(OpenGL_Display,OpenGL_Window,OpenGL_Context);

  //	XSync(OpenGL_Display,False);
  */

  Already_loaded = 1;
  return 1;
}
#endif

// Sets up our OpenGL rendering context
// Returns 1 if ok, 0 if something bad
int opengl_Init(oeApplication *app, renderer_preferred_state *pref_state) {
  _D(("opengl_Init(%p, %p);", app, pref_state));

  int width, height;
  int retval = 1;
  int i;

  mprintf((0, "OpenGL: Setting up OpenGL mode!\n"));

  if (pref_state)
    OpenGL_preferred_state = *pref_state;

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
    GetWindowRect((HWND)hOpenGLWnd, &rect);
    width = abs(rect.right - rect.left);
    height = abs(rect.bottom - rect.top);
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

  mprintf((0, "OpenGL: Setting up projection matrix\n"));

  dglMatrixMode(GL_PROJECTION);
  dglLoadIdentity();
  dglOrtho(0.0f, (GLfloat)width, (GLfloat)height, 0, 0, 1);
  dglViewport(0, 0, width, height);
  dglMatrixMode(GL_MODELVIEW);
  dglLoadIdentity();

  opengl_InitCache();

  OpenGL_multitexture = false;
  OpenGL_packed_pixels = false;
#if (defined(_USE_OGL_ACTIVE_TEXTURES))
  OpenGL_multitexture = opengl_CheckExtension("GL_ARB_multitexture");

#ifdef __LINUX__
  if (OpenGL_multitexture) {

    /****************** WARNING: NASTY HACK! ***********************/
#define mod_GetSymbol(x, funcStr, y) __SDL_mod_GetSymbol(funcStr)
    /****************** WARNING: NASTY HACK! ***********************/

    // get extra's that aren't loaded in LoadOpenGLDLL()...
    dglActiveTextureARB = dllActiveTextureARB =
        (PFNGLACTIVETEXTUREARBPROC)mod_GetSymbol(OpenGLDLLHandle, "glActiveTextureARB", 255);
    if (dglActiveTextureARB == NULL)
      OpenGL_multitexture = false;

    dglClientActiveTextureARB = dllClientActiveTextureARB =
        (PFNGLCLIENTACTIVETEXTUREARBPROC)mod_GetSymbol(OpenGLDLLHandle, "glClientActiveTextureARB", 255);
    if (dglClientActiveTextureARB == NULL)
      OpenGL_multitexture = false;

    dglMultiTexCoord4f = dllMultiTexCoord4f =
        (PFNGLMULTITEXCOORD4FARBPROC)mod_GetSymbol(OpenGLDLLHandle, "glMultiTexCoord4fARB", 255);
    if (dglMultiTexCoord4f == NULL)
      OpenGL_multitexture = false;

/****************** WARNING: NASTY HACK! ***********************/
#undef mod_GetSymbol
    /****************** WARNING: NASTY HACK! ***********************/

  } // if
#endif

#endif

  // ryan sez : Don't use SGIS_multitexture
  //	if (!OpenGL_multitexture)
  //		OpenGL_multitexture=opengl_CheckExtension ("GL_SGIS_multitexture");
  //

  int major, minor;
  opengl_GetVersion(&major, &minor);

  mprintf((0, "OpenGL: Version is (%d) major, (%d) minor.", major, minor));

  // OpenGL 1.2 specifies the packed pixel "extension" by default.
  if ((major >= 1) && (minor >= 2))
    OpenGL_packed_pixels = true;
  else
    OpenGL_packed_pixels = opengl_CheckExtension("GL_EXT_packed_pixels");

  if (FindArgChar("-nopackedpixels", 'x'))
    OpenGL_packed_pixels = false;

  opengl_GetDLLFunctions();

  if (FindArgChar("-nomultitexture", 't'))
    OpenGL_multitexture = false;

  OpenGL_fog_enabled = false;
  if (FindArgChar("-glfog", 'o'))
    OpenGL_fog_enabled = true;

  mprintf((0, "OpenGL: multitextures are%s turned on.", (OpenGL_multitexture) ? "" : " NOT"));

  mprintf((0, "OpenGL: Packed pixels are%s turned on.", (OpenGL_packed_pixels) ? "" : " NOT"));

  mprintf((0, "OpenGL: Fog is%s turned on.", (OpenGL_fog_enabled) ? "" : " NOT"));

  if (OpenGL_packed_pixels) {
    opengl_packed_Upload_data = (ushort *)mem_malloc(256 * 256 * 2);
    opengl_packed_Translate_table = (ushort *)mem_malloc(65536 * 2);
    opengl_packed_4444_translate_table = (ushort *)mem_malloc(65536 * 2);

    ASSERT(opengl_packed_Upload_data);
    ASSERT(opengl_packed_Translate_table);
    ASSERT(opengl_packed_4444_translate_table);

    mprintf((0, "OpenGL: Building packed translate table...\n"));

    for (i = 0; i < 65536; i++) {
      int r = (i >> 10) & 0x1f;
      int g = (i >> 5) & 0x1f;
      int b = i & 0x1f;

      ushort pix;

      if (!(i & OPAQUE_FLAG))
        pix = 0;
      else
        pix = (r << 11) | (g << 6) | (b << 1) | 1;

      opengl_packed_Translate_table[i] = INTEL_SHORT(pix);

      // 4444 table
      int a = (i >> 12) & 0xf;
      r = (i >> 8) & 0xf;
      g = (i >> 4) & 0xf;
      b = i & 0xf;

      if (WindowGL)
        a = 0xf;

      pix = (r << 12) | (g << 8) | (b << 4) | a;

      opengl_packed_4444_translate_table[i] = INTEL_SHORT(pix);
    }
  } else {
    opengl_Upload_data = (uint *)mem_malloc(256 * 256 * 4);
    opengl_Translate_table = (uint *)mem_malloc(65536 * 4);
    opengl_4444_translate_table = (uint *)mem_malloc(65536 * 4);

    ASSERT(opengl_Upload_data);
    ASSERT(opengl_Translate_table);
    ASSERT(opengl_4444_translate_table);

    mprintf((0, "OpenGL: Building translate table...\n"));

    for (i = 0; i < 65536; i++) {
      uint pix;
      int r = (i >> 10) & 0x1f;
      int g = (i >> 5) & 0x1f;
      int b = i & 0x1f;

      float fr = (float)r / 31.0;
      float fg = (float)g / 31.0;
      float fb = (float)b / 31.0;

      r = (int)(255 * fr);
      g = (int)(255 * fg);
      b = (int)(255 * fb);

      if (!(i & OPAQUE_FLAG))
        pix = 0;
      else
        pix = (255 << 24) | (b << 16) | (g << 8) | (r);

      opengl_Translate_table[i] = INTEL_INT(pix);

      // Do 4444
      int a = (i >> 12) & 0xf;
      r = (i >> 8) & 0xf;
      g = (i >> 4) & 0xf;
      b = i & 0xf;

      float fa = (float)a / 15.0;
      fr = (float)r / 15.0;
      fg = (float)g / 15.0;
      fb = (float)b / 15.0;

      a = (int)(255 * fa);
      r = (int)(255 * fr);
      g = (int)(255 * fg);
      b = (int)(255 * fb);

      if (WindowGL)
        a = 0xf;

      pix = (a << 24) | (b << 16) | (g << 8) | (r);

      opengl_4444_translate_table[i] = INTEL_INT(pix);
    }
  }

  opengl_SetDefaults();

  CHECK_ERROR(4)

  // Tell our app to use multitexture
  if (OpenGL_multitexture)
    UseMultitexture = true;

  OpenGL_state.initted = 1;

  mprintf((0, "OpenGL: initialization at %dx%d was successful.\n", width, height));

  return retval;

D3DError:
  opengl_Close();
  return 0;
}

// Releases the rendering context
void opengl_Close() {
  _D(("opengl_Close();"));

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

  // SDL_Quit() handles this for us.

/*
        LinuxVideoMode.Lock(false);
        LinuxVideoMode.RestoreVideoMode();
        if(OpenGL_Display)
        {
                if(dglXMakeCurrent)
                        glXMakeCurrent(OpenGL_Display,OpenGL_Window,NULL);
                if(dglXDestroyContext)
                        glXDestroyContext(OpenGL_Display,OpenGL_Context);
        }
*/
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
  _D(("opengl_TranslateBitmapToOpenGL(%d, %d, %d, %d, %d);", texnum, bm_handle, map_type, replace, tn));

  ushort *bm_ptr;

  int w = 0;
  int h = 0;
  int size = 0;

#if (defined(_USE_OGL_ACTIVE_TEXTURES))
  if (OpenGL_multitexture && Last_texel_unit_set != tn) {
    dglActiveTextureARB((GLenum)(GL_TEXTURE0_ARB + tn));
    Last_texel_unit_set = tn;
  }
#endif

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
#ifndef __LINUX__
      ushort *left_data = (ushort *)opengl_packed_Upload_data;
      int bm_left = 0;

      for (int i = 0; i < h; i++, left_data += size, bm_left += w) {
        ushort *dest_data = left_data;
        for (int t = 0; t < w; t++) {
          *dest_data++ = opengl_packed_Translate_table[bm_ptr[bm_left + t]];
        }
      }
#endif

      // rcg06262000 my if wrapper.
      if (size > 0) {
        // rcg01112001 GL_BGRA, etc aren't in the headers that are used on win32.
#ifdef __LINUX__
        if (replace) {
          dglTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, size, size, GL_BGRA, GL_UNSIGNED_SHORT_1_5_5_5_REV,
                           bm_ptr /*opengl_packed_Upload_data*/);
        } // if
        else {
          dglTexImage2D(GL_TEXTURE_2D, 0, GL_RGB5_A1, size, size, 0, GL_BGRA, GL_UNSIGNED_SHORT_1_5_5_5_REV,
                        bm_ptr /*opengl_packed_Upload_data*/);
        } // else
#else
        if (replace) {
          dglTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, size, size, GL_RGBA, UNSIGNED_SHORT_5_5_5_1_EXT,
                           opengl_packed_Upload_data);
        } // if
        else {
          dglTexImage2D(GL_TEXTURE_2D, 0, GL_RGB5_A1, size, size, 0, GL_RGBA, UNSIGNED_SHORT_5_5_5_1_EXT,
                        opengl_packed_Upload_data);
        } // else
#endif
      } // if
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
        }

        if ((w < 1) || (h < 1))
          continue;

        if (bm_format(bm_handle) == BITMAP_FORMAT_4444) {
          // Do 4444

          // rcg01112001 GL_BGRA, etc aren't in the headers that are used on win32.
#ifdef __LINUX__

          if (bm_mipped(bm_handle)) {
            for (i = 0; i < w * h; i++)
              opengl_packed_Upload_data[i] = 0xf000 | bm_ptr[i];
          }

          if (replace)
            dglTexSubImage2D(GL_TEXTURE_2D, m, 0, 0, w, h, GL_BGRA, GL_UNSIGNED_SHORT_4_4_4_4_REV,
                             bm_mipped(bm_handle) ? opengl_packed_Upload_data : bm_ptr);
          else
            dglTexImage2D(GL_TEXTURE_2D, m, GL_RGBA4, w, h, 0, GL_BGRA, GL_UNSIGNED_SHORT_4_4_4_4_REV,
                          bm_mipped(bm_handle) ? opengl_packed_Upload_data : bm_ptr);

#else

          if (bm_mipped(bm_handle)) {
            for (i = 0; i < w * h; i++)
              opengl_packed_Upload_data[i] = 0xf | (opengl_packed_4444_translate_table[bm_ptr[i]]);
          } else {
            for (i = 0; i < w * h; i++)
              opengl_packed_Upload_data[i] = opengl_packed_4444_translate_table[bm_ptr[i]];
          }

          if (replace)
            dglTexSubImage2D(GL_TEXTURE_2D, m, 0, 0, w, h, GL_RGBA, UNSIGNED_SHORT_4_4_4_4_EXT,
                             opengl_packed_Upload_data);
          else
            dglTexImage2D(GL_TEXTURE_2D, m, GL_RGBA4, w, h, 0, GL_RGBA, UNSIGNED_SHORT_4_4_4_4_EXT,
                          opengl_packed_Upload_data);

#endif
        } else {
          // Do 1555

#ifdef __LINUX__

          if (replace)
            dglTexSubImage2D(GL_TEXTURE_2D, m, 0, 0, w, h, GL_BGRA, GL_UNSIGNED_SHORT_1_5_5_5_REV,
                             bm_ptr /*opengl_packed_Upload_data*/);
          else
            dglTexImage2D(GL_TEXTURE_2D, m, GL_RGB5_A1, w, h, 0, GL_BGRA, GL_UNSIGNED_SHORT_1_5_5_5_REV,
                          bm_ptr /*opengl_packed_Upload_data*/);

#else

          for (i = 0; i < w * h; i++)
            opengl_packed_Upload_data[i] = opengl_packed_Translate_table[bm_ptr[i]];

          if (replace)
            dglTexSubImage2D(GL_TEXTURE_2D, m, 0, 0, w, h, GL_RGBA, UNSIGNED_SHORT_5_5_5_1_EXT,
                             opengl_packed_Upload_data);
          else
            dglTexImage2D(GL_TEXTURE_2D, m, GL_RGB5_A1, w, h, 0, GL_RGBA, UNSIGNED_SHORT_5_5_5_1_EXT,
                          opengl_packed_Upload_data);

#endif
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

      // rcg06262000 my if wrapper.
      if (size > 0) {
        if (replace)
          dglTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, size, size, GL_RGBA, GL_UNSIGNED_BYTE, opengl_Upload_data);
        else
          dglTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size, size, 0, GL_RGBA, GL_UNSIGNED_BYTE, opengl_Upload_data);
      } // if
    } else {
      int limit = 0;

      if (bm_mipped(bm_handle))
        limit = NUM_MIP_LEVELS + 3; // ryan added +3.
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

        // rcg06262000 my if wrapper.
        if ((w > 0) && (h > 0)) {
          if (replace)
            dglTexSubImage2D(GL_TEXTURE_2D, m, 0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, opengl_Upload_data);
          else
            dglTexImage2D(GL_TEXTURE_2D, m, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, opengl_Upload_data);
        } // if
      }
    }
  }

  // mprintf ((1,"Doing slow upload to opengl!\n"));

  if (map_type == MAP_TYPE_LIGHTMAP)
    GameLightmaps[bm_handle].flags &= ~LF_LIMITS;

  CHECK_ERROR(6)

  OpenGL_uploads++;
}
extern bool Force_one_texture;
// Utilizes a LRU cacheing scheme to select/upload textures the opengl driver
int opengl_MakeBitmapCurrent(int handle, int map_type, int tn) {
  _D(("opengl_MakeBitmapCurrent(%d, %d, %d);", handle, map_type, tn));

  int w, h;
  int texnum;

  if (map_type == MAP_TYPE_LIGHTMAP) {
    w = GameLightmaps[handle].square_res;
    h = GameLightmaps[handle].square_res;
  } else {
    if (Force_one_texture)
      handle = 0;

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
      if (GameBitmaps[handle].flags & BF_CHANGED)
        opengl_TranslateBitmapToOpenGL(texnum, handle, map_type, 1, tn);
    }
  }

  if (OpenGL_last_bound[tn] != texnum) {
#if (defined(_USE_OGL_ACTIVE_TEXTURES))
    if (OpenGL_multitexture && Last_texel_unit_set != tn) {
      dglActiveTextureARB((GLenum)(GL_TEXTURE0_ARB + tn));
      Last_texel_unit_set = tn;
    }
#endif

    dglBindTexture(GL_TEXTURE_2D, texnum);
    OpenGL_last_bound[tn] = texnum;
    OpenGL_sets_this_frame[0]++;
  }

  CHECK_ERROR(7)
  return 1;
}

// Sets up an appropriate wrap type for the current bound texture
void opengl_MakeWrapTypeCurrent(int handle, int map_type, int tn) {
  _D(("opengl_MakeWrapTypeCurrent(%d, %d, %d);", handle, map_type, tn));

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

#if (defined(_USE_OGL_ACTIVE_TEXTURES))
  if (OpenGL_multitexture && Last_texel_unit_set != tn) {
    dglActiveTextureARB((GLenum)(GL_TEXTURE0_ARB + tn));
    Last_texel_unit_set = tn;
  }
#endif

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
  _D(("opengl_MakeFilterTypeCurrent(%d, %d, %d);", handle, map_type, tn));

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

#if (defined(_USE_OGL_ACTIVE_TEXTURES))
  if (OpenGL_multitexture && Last_texel_unit_set != tn) {
    dglActiveTextureARB((GLenum)(GL_TEXTURE0_ARB + tn));
    Last_texel_unit_set = tn;
  }
#endif

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
float opengl_GetAlphaMultiplier() {
  _D(("opengl_GetAlphaMultiplier();"));

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

// Sets the alpha multiply factor
void opengl_SetAlphaMultiplier() {
  _D(("opengl_SetAlphaMultiplier();"));
  Alpha_multiplier = opengl_GetAlphaMultiplier();
}

// Turns on/off multitexture blending
void opengl_SetMultitextureBlendMode(bool state) {
  _D(("opengl_SetMultitextureBlendMode();"));
  if (OpenGL_multitexture_state == state)
    return;

  OpenGL_multitexture_state = state;

#if (defined(_USE_OGL_ACTIVE_TEXTURES))
  if (state) {
    dglClientActiveTextureARB((GLenum)(GL_TEXTURE0_ARB + 1));
    dglActiveTextureARB((GLenum)(GL_TEXTURE0_ARB + 1));
    dglEnableClientState(GL_TEXTURE_COORD_ARRAY);
    dglEnable(GL_TEXTURE_2D); //!!!

    /*if (Overlay_type==OT_BLEND_SATURATE)
            glBlendFunc (GL_SRC_ALPHA,GL_ONE);
    else
            glBlendFunc (GL_DST_COLOR,GL_ZERO);*/

    dglActiveTextureARB((GLenum)(GL_TEXTURE0_ARB + 0));
    Last_texel_unit_set = 0;
  } else {
    dglClientActiveTextureARB((GLenum)(GL_TEXTURE0_ARB + 1));
    dglActiveTextureARB((GLenum)(GL_TEXTURE0_ARB + 1));
    dglDisableClientState(GL_TEXTURE_COORD_ARRAY);
    dglDisable(GL_TEXTURE_2D);
    dglActiveTextureARB((GLenum)(GL_TEXTURE0_ARB + 0));
    Last_texel_unit_set = 0;
  }
#endif
}

// Takes nv vertices and draws the polygon defined by those vertices.  Uses bitmap "handle"
// as a texture
void opengl_DrawMultitexturePolygon(int handle, g3Point **p, int nv, int map_type) {
  _D(("opengl_DrawMultitexturePolygon(%d, %p, %d, %d);", handle, p, nv, map_type));

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
#ifdef __RYANS_EASTER_EGGS__
    if (__pumpkinBromo == true) {
      texp->s = pnt->p3_u;
      texp->t = pnt->p3_v;
      texp->r = 0.0;
      texp->w = 1.0;

      texp2->s = pnt->p3_u2 * xscalar;
      texp2->t = pnt->p3_v2 * yscalar;
      texp2->r = 0.0;
      texp2->w = 1.0;
    } // if
    else
#endif
    {
      float texw = 1.0 / (pnt->p3_z + Z_bias);
      texp->s = pnt->p3_u * texw;
      texp->t = pnt->p3_v * texw;
      texp->r = 0.0;
      texp->w = texw;

      texp2->s = pnt->p3_u2 * xscalar * texw;
      texp2->t = pnt->p3_v2 * yscalar * texw;
      texp2->r = 0.0;
      texp2->w = texw;
    } // else

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
#if (defined(_USE_OGL_LISTS_OPTIONAL))
  if (OpenGL_UseLists) {
#endif

    dglDrawArrays(GL_POLYGON, 0, nv);

#if (defined(_USE_OGL_LISTS_OPTIONAL))
  } else {
    // FIXME...this might still be broken when used in
    //  conjunction with multitexturing. Use at your own
    //  risk.  --rcg 06082000. !!!
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
  _D(("opengl_DrawFlatPolygon(%p, %d);", p, nv));

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

static inline float __recip(float x) {
#if MACOSX
  __asm__ __volatile__("fres %0, %1  \n\t" : "=f"(x) : "f"(x));
  return (x);
#else
  return (1.0f / x);
#endif
}

// Takes nv vertices and draws the polygon defined by those vertices.  Uses bitmap "handle"
// as a texture
void opengl_DrawPolygon(int handle, g3Point **p, int nv, int map_type) {
  _D(("opengl_DrawPolygon(%d, %p, %d, %d,);", handle, p, nv, map_type));

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

    // ryan's adds. 04/18/2000 ...moved around, since the code originally
    //  #ifdef'd for __LINUX__ was mostly a duplicate.
    /*
    #ifdef __LINUX__
    //MY TEST HACK...MAYBE BAD DRIVERS? OR MAYBE THIS IS
    //HOW IT SHOULD BE DONE (STILL BUGGY)
                    // Texture this polygon!
                    if(OpenGL_TextureHack)
                    {
                            texp->s=pnt->p3_u;
                            texp->t=pnt->p3_v;
                    }else
                    {
    #endif
    */
// rcg07012000 this "pumpkin bromo" code seems to work,
//  but for now, I'm not fixing what ain't broke, so it's
//  activated via cheatcode. This may change in a later patch.
#ifdef __RYANS_EASTER_EGGS__
    if (__pumpkinBromo == true) {
      texp->r = 0.0;
      texp->s = pnt->p3_u;
      texp->t = pnt->p3_v;
      texp->w = 1.0;
    } // if
    else
#endif
    {
      // rcg07012000 old code.
      float texw = __recip(pnt->p3_z + Z_bias);
      texp->r = 0.0f;
      texp->s = pnt->p3_u * texw;
      texp->t = pnt->p3_v * texw;
      texp->w = texw;
    } // else

    /*
    #ifdef __LINUX__
                    }
    #endif
    */
    // end ryan's adds.

    // Finally, specify a vertex
    vertp->x = pnt->p3_sx + x_add;
    vertp->y = pnt->p3_sy + y_add;

    //@@float z=(pnt->p3_z+Z_bias)/OpenGL_state.cur_far_z;
    float val = 1.0f - (__recip(pnt->p3_z + Z_bias));
    float z = std::max(0.0f, std::min(1.0f, val));
    vertp->z = -z;
  }

  // And draw!
#if (defined(_USE_OGL_LISTS_OPTIONAL))
  if (OpenGL_UseLists) {
#endif
    dglDrawArrays(GL_POLYGON, 0, nv);
#if (defined(_USE_OGL_LISTS_OPTIONAL))
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

void opengl_BeginFrame(int x1, int y1, int x2, int y2, int clear_flags) {
  _D(("opengl_BeginFrame(%d, %d, %d, %d, %d);", x1, y1, x2, y2, clear_flags));

  if (clear_flags & RF_CLEAR_ZBUFFER)
    dglClear(GL_DEPTH_BUFFER_BIT);

  OpenGL_state.clip_x1 = x1;
  OpenGL_state.clip_y1 = y1;
  OpenGL_state.clip_x2 = x2;
  OpenGL_state.clip_y2 = y2;
}

void opengl_EndFrame() { _D(("opengl_EndFrame();")); }

// Takes a screenshot of the frontbuffer and puts it into the passed bitmap handle
void opengl_Screenshot(int bm_handle) {
  _D(("opengl_ScreenShot(%d);", bm_handle));

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

#ifdef __CHECK_FOR_TOO_SLOW_RENDERING__
static void slownessAbort(void) {

#ifdef __LINUX__
  SDL_Surface *surface = SDL_GetVideoSurface();
  SDL_SetVideoMode(surface->w, surface->h, surface->format->BitsPerPixel, surface->flags & ~SDL_OPENGL);

  sdl_ShowMessage("Your OpenGL driver is too slow to play this game.\n"
                  "Driver used: [ %s ]\n"
                  "Please change your driver!\n"
                  "Email support@lokigames.com for help,\n"
                  "or call 1-714-508-2140 (9-5 PM US Pacific Time).\n",
                  loadedLibrary);

  SDL_GL_SwapBuffers();
  Sleep(10000);
  SDL_Quit();
  _exit(99);
#else
#error Fill in an aborting notice for your platform.
#endif

} // slownessAbort
#endif

// Flips the screen
void opengl_Flip() {
  _D(("opengl_Flip();"));

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
  //	glXWaitGL();
  //	glXSwapBuffers(OpenGL_Display,OpenGL_Window);
  SDL_GL_SwapBuffers();
#endif

#ifdef __PERMIT_GL_LOGGING
  if (__glLog == true)
    DGL_LogNewFrame();
#endif

#ifdef __CHECK_FOR_TOO_SLOW_RENDERING__
  if (minimumAcceptableRender > 0) {
    Uint32 newticks = SDL_GetTicks();
    if ((newticks - lastSwapTicks) > minimumAcceptableRender) {
      tooSlowCount++;
      if (tooSlowCount >= 3) {
        slownessAbort();
      } // if
    }   // if

    // disable check?
    tooSlowChecksLeft--;
    if (tooSlowChecksLeft <= 0)
      minimumAcceptableRender = -1;

    lastSwapTicks = newticks;
  } // if
#endif
}

void opengl_SetTextureType(texture_type state) {
  _D(("opengl_SetTextureType(%d);", state));

  if (state == OpenGL_state.cur_texture_type)
    return; // No redundant state setting

#if (defined(_USE_OGL_ACTIVE_TEXTURES))
  if (OpenGL_multitexture && Last_texel_unit_set != 0) {
    dglActiveTextureARB((GLenum)(GL_TEXTURE0_ARB + 0));
    Last_texel_unit_set = 0;
  }
#endif

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
    dglEnable(GL_TEXTURE_2D); //!!!
    OpenGL_state.cur_texture_quality = 2;
    break;
  default:
    Int3(); // huh? Get Jason
    break;
  }

  CHECK_ERROR(12)
  OpenGL_state.cur_texture_type = state;
}

// Sets the lighting state of opengl
void opengl_SetLightingState(light_state state) {
  _D(("opengl_SetLightingState(%d);", state));

  if (state == OpenGL_state.cur_light_state)
    return; // No redundant state setting

#if (defined(_USE_OGL_ACTIVE_TEXTURES))
  if (OpenGL_multitexture && Last_texel_unit_set != 0) {
    dglActiveTextureARB((GLenum)(GL_TEXTURE0_ARB + 0));
    Last_texel_unit_set = 0;
  }
#endif

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

// Sets the opengl color model (either rgb or mono)
void opengl_SetColorModel(color_model state) {
  _D(("opengl_SetColorModel(%d);", state));

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

// Sets the state of bilinear filtering for our textures
void opengl_SetFiltering(sbyte state) {
  _D(("opengl_SetFiltering(%d);", (int)state));

#ifndef RELEASE
  if (Fast_test_render)
    state = 0;
#endif

  OpenGL_state.cur_bilinear_state = state;
}

// Sets the state of zbuffering to on or off
void opengl_SetZBufferState(sbyte state) {
  _D(("opengl_SetZBufferState(%d);", (int)state));

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

void opengl_SetZValues(float nearz, float farz) {
  _D(("opengl_SetZValues(%f, %f);", nearz, farz));

  OpenGL_state.cur_near_z = nearz;
  OpenGL_state.cur_far_z = farz;

  // mprintf ((0,"OPENGL:Setting depth range to %f - %f\n",nearz,farz));

  // JEFF: glDepthRange must take parameters [0,1]
  // It is set in init
  //@@dglDepthRange (0,farz);
}

// Clears the display to a specified color
void opengl_ClearScreen(ddgr_color color) {
#ifdef __GL_CLEAR_CHECKING
  //        static ddgr_color clrcolor = 0x35344;
  color = 1 + (int)(200000000.0 * rand() / (RAND_MAX + 1.0));
#endif

  _D(("opengl_ClearScreen(%d);", (int)color));
  int r = (color >> 16 & 0xFF);
  int g = (color >> 8 & 0xFF);
  int b = (color & 0xFF);

  dglClearColor((float)r / 255.0, (float)g / 255.0, (float)b / 255.0, 0);
  dglClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

// Clears the zbuffer
void opengl_ClearZBuffer() {
  _D(("opengl_ClearZBuffer();"));
  dglClear(GL_DEPTH_BUFFER_BIT);
}

// Fills a rectangle on the display
void opengl_FillRect(ddgr_color color, int x1, int y1, int x2, int y2) {
  _D(("opengl_FillRect(%d, %d, %d, %d, %d);", color, x1, y1, x2, y2));

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
void opengl_SetPixel(ddgr_color color, int x, int y) {
  _D(("opengl_SetPixel(%d, %d, %d);", color, x, y));

  int r = (color >> 16 & 0xFF);
  int g = (color >> 8 & 0xFF);
  int b = (color & 0xFF);

  dglColor3ub(r, g, b);

  dglBegin(GL_POINTS);
  dglVertex2i(x, y);
  dglEnd();
}

// Returns the pixel color at x,y
ddgr_color opengl_GetPixel(int x, int y) {
  _D(("opengl_GetPixel(%d, %d);", x, y));

  ddgr_color color[4];

  dglReadPixels(x, (OpenGL_state.screen_height - 1) - y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid *)color);

  return color[0];
}

// Sets the color that opengl uses for fog
void opengl_SetFogColor(ddgr_color color) {
  _D(("opengl_SetFogColor(%d);", color));

  if (color == OpenGL_state.cur_fog_color)
    return;

  float fc[4];
  fc[0] = GR_COLOR_RED(color);
  fc[1] = GR_COLOR_GREEN(color);
  fc[2] = GR_COLOR_BLUE(color);
  fc[3] = 1;

  fc[0] /= 255.0;
  fc[1] /= 255.0;
  fc[2] /= 255.0;

  dglFogfv(GL_FOG_COLOR, fc);
}

// Sets the near and far plane of fog
// Note, the opengl_Far_z variable must be valid for this function to work correctly
void opengl_SetFogBorders(float nearz, float farz) {
  _D(("opengl_SetFogBorders(%f, %f);", nearz, farz));

  float fog_start, fog_end;

  fog_start = std::max(0, std::min(1.0, 1.0 - (1.0 / nearz)));
  fog_end = std::max(0, std::min(1.0, 1.0 - (1.0 / farz)));

  OpenGL_state.cur_fog_start = fog_start;
  OpenGL_state.cur_fog_end = fog_end;

  dglFogi(GL_FOG_MODE, GL_LINEAR);
  dglFogf(GL_FOG_START, fog_start);
  dglFogf(GL_FOG_END, fog_end);
}

// Sets the fog state to on or off
void opengl_SetFogState(sbyte state) {
  if (state == OpenGL_state.cur_fog_state)
    return; // No redundant state setting

  OpenGL_state.cur_fog_state = state;

  if (OpenGL_fog_enabled) {
    if (state == 1) {
      dglEnable(GL_FOG);
    } else {
      dglDisable(GL_FOG);
    }
  } // if
}

// Fills in projection variables
void opengl_GetProjectionParameters(int *width, int *height) {
  _D(("opengl_GetProjectionParameters(%p, %p);", width, height));

  *width = OpenGL_state.clip_x2 - OpenGL_state.clip_x1;
  *height = OpenGL_state.clip_y2 - OpenGL_state.clip_y1;
}

// Returns the aspect ratio of the physical screen
float opengl_GetAspectRatio() {
  _D(("opengl_GetAspectRatio();"));

  float aspect_ratio = (float)((3.0 * OpenGL_state.screen_width) / (4.0 * OpenGL_state.screen_height));
  return aspect_ratio;
}

// Sets the type of alpha blending you want
void opengl_SetAlphaType(sbyte atype) {
  _D(("opengl_SetAlphaType(%d);", (int)atype));

  if (atype == OpenGL_state.cur_alpha_type)
    return; // don't set it redundantly

#if (defined(_USE_OGL_ACTIVE_TEXTURES))
  if (OpenGL_multitexture && Last_texel_unit_set != 0) {
    dglActiveTextureARB((GLenum)(GL_TEXTURE0_ARB + 0));
    Last_texel_unit_set = 0;
  }
#endif

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
    opengl_SetAlphaValue(255);
    dglBlendFunc(GL_ONE, GL_ZERO);
    break;
  case AT_CONSTANT:
    dglBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    break;
  case AT_TEXTURE:
    opengl_SetAlphaValue(255);
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
  /*case AT_FLAT_BLEND:
          glide_SetAlphaValue (255);
          grAlphaCombine(GR_COMBINE_FUNCTION_SCALE_OTHER, GR_COMBINE_FACTOR_ONE, GR_COMBINE_LOCAL_NONE,
  GR_COMBINE_OTHER_CONSTANT, FXFALSE); grAlphaBlendFunction(GR_BLEND_DST_COLOR, GR_BLEND_ZERO, 	GR_BLEND_ONE,
  GR_BLEND_ZERO); break; case AT_ANTIALIAS: grAlphaCombine(GR_COMBINE_FUNCTION_LOCAL, GR_COMBINE_FACTOR_ONE,
  GR_COMBINE_LOCAL_ITERATED, GR_COMBINE_OTHER_NONE, FXFALSE); grAlphaBlendFunction(GR_BLEND_SRC_ALPHA,
  GR_BLEND_ONE_MINUS_SRC_ALPHA, 	GR_BLEND_ZERO, GR_BLEND_ZERO); break;*/
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
  opengl_SetAlphaMultiplier();
  CHECK_ERROR(15)
}

// Sets texture wrapping type
void opengl_SetWrapType(wrap_type val) {
  _D(("opengl_SetWrapType(%d);", (int)val));
  OpenGL_state.cur_wrap_type = val;
}

// Sets whether or not to write into the zbuffer
void opengl_SetZBufferWriteMask(int state) {
  _D(("opengl_SetZBufferWriteMask(%d);", state));

  OpenGL_sets_this_frame[5]++;
  if (state) {
    dglDepthMask(GL_TRUE);
  } else {
    dglDepthMask(GL_FALSE);
  }
}

void opengl_SetFlatColor(ddgr_color color) {
  _D(("opengl_SetFlatColor(%d);", color));
  OpenGL_state.cur_color = color;
}

// Sets the constant alpha value
void opengl_SetAlphaValue(ubyte val) {
  _D(("opengl_SetAlphaValue(%d);", (int)val));
  OpenGL_state.cur_alpha = val;
  opengl_SetAlphaMultiplier();
}

// Sets the overall alpha scale factor (all alpha values are scaled by this value)
// usefull for motion blur effect
void opengl_SetAlphaFactor(float val) {
  _D(("opengl_SetAlphaFactor(%f);", val));
  OpenGL_Alpha_factor = val;
}

// Returns the current Alpha factor
float opengl_GetAlphaFactor(void) {
  _D(("opengl_GetAlphaFactor();"));
  return OpenGL_Alpha_factor;
}

// Gets the current state of the renderer
void opengl_GetRenderState(rendering_state *rstate) {
  _D(("opengl_GetRenderState(%p);", rstate));
  memcpy(rstate, &OpenGL_state, sizeof(rendering_state));
}

// draws a line
void opengl_DrawLine(int x1, int y1, int x2, int y2) {
  _D(("opengl_DrawLine(%d, %d, %d, %d);", x1, y1, x2, y2));

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

// draws a line
void opengl_DrawSpecialLine(g3Point *p0, g3Point *p1) {
  _D(("opengl_DrawSpecialLine(%p, %p);", p0, p1));

  int x_add = OpenGL_state.clip_x1;
  int y_add = OpenGL_state.clip_y1;
  float fr, fg, fb, alpha;
  int i;

  fr = GR_COLOR_RED(OpenGL_state.cur_color);
  fg = GR_COLOR_GREEN(OpenGL_state.cur_color);
  fb = GR_COLOR_BLUE(OpenGL_state.cur_color);

  fr /= 255.0;
  fg /= 255.0;
  fb /= 255.0;

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

// Sets the coplanar z bias for rendered polygons
void opengl_SetCoplanarPolygonOffset(float factor) {
  _D(("opengl_SetCoplanarPolygonOffset(%f);", factor));

  if (factor == 0) {
    dglDisable(GL_POLYGON_OFFSET_FILL);
  } else {
    dglEnable(GL_POLYGON_OFFSET_FILL);
    dglPolygonOffset(-1.0, -1.0);
  }
}

// Sets the gamma correction value
void opengl_SetGammaValue(float val) {
  _D(("opengl_SetGammaValue(%f);", val));

  if (WindowGL)
    return;

  OpenGL_preferred_state.gamma = val;

  mprintf((0, "Setting gamma to %f\n", val));

#if defined(WIN32)
  WORD rampvals[3 * 256];

  for (int i = 0; i < 256; i++) {
    float norm = (float)i / 255.0;

    float newval = pow(norm, 1.0 / val);

    newval *= 65535;

    newval = std::min(65535, newval);

    rampvals[i] = newval;
    rampvals[i + 256] = newval;
    rampvals[i + 512] = newval;
  }

  SetDeviceGammaRamp(hOpenGLDC, (LPVOID)rampvals);
#endif
}

// Sets up a some global preferences for openGL
int opengl_SetPreferredState(renderer_preferred_state *pref_state) {
  _D(("opengl_SetPreferredState(%p);", pref_state));

  int retval = 1;
  renderer_preferred_state old_state = OpenGL_preferred_state;

  OpenGL_preferred_state = *pref_state;

  if (OpenGL_state.initted) {
    int reinit = 0;

    mprintf((0, "Inside pref state!\n"));

    // Change gamma if needed
    if (pref_state->width != OpenGL_state.screen_width || pref_state->height != OpenGL_state.screen_height ||
        old_state.bit_depth != pref_state->bit_depth)
      reinit = 1;

    if (reinit) {
      opengl_Close();
      retval = opengl_Init(NULL, &OpenGL_preferred_state);
    } else {
      if (old_state.gamma != pref_state->gamma) {
        opengl_SetGammaValue(pref_state->gamma);
      }

      /*if (old_state.mipping!=pref_state->mipping)
      {
              opengl_SetMipState (pref_state->mipping);
      }*/
    }
  } else
    OpenGL_preferred_state = *pref_state;

  return retval;
}

// Resets the texture cache
void opengl_ResetCache() {
  _D(("opengl_ResetCache();"));

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
  _D(("opengl_ChangeChunkedBitmap(%d, %p);", bm_handle, chunk));

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
  int how_many_across = (int)temp;
  if ((temp - how_many_across) > 0)
    how_many_across++;

  temp = bh / fopt;
  int how_many_down = (int)temp;
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

// Takes a bitmap and blits it to the screen using linear frame buffer stuff
// X and Y are the destination X,Y
void opengl_CopyBitmapToFramebuffer(int bm_handle, int x, int y) {
  _D(("opengl_CopyBitmapToFramebuffer(%d, %d, %d);", bm_handle, x, y));

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
void opengl_SetFrameBufferCopyState(bool state) {
  _D(("opengl_SetFrameBufferCopyState(%s);", state ? "true" : "false"));

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

// returns rendering statistics for the frame
void opengl_GetStatistics(tRendererStats *stats) {
  _D(("opengl_GetStatistics(%p);", stats));

  stats->poly_count = OpenGL_last_frame_polys_drawn;
  stats->vert_count = OpenGL_last_frame_verts_processed;
  stats->texture_uploads = OpenGL_last_uploaded;
}

/*
#ifdef __LINUX__
void CreateFullScreenWindow(Display *dpy,Window rootwin,Window window,int DisplayScreen,int DisplayWidth,int
DisplayHeight)
{
        // see if a motif based window manager is running.  do this by
        // getting the _MOTIF_WM_INFO property on the root window.  if
        // it exists then make sure the window it refers to also exists.
        Bool noWM,isMWMRunning = False;
        Atom a = XInternAtom(dpy, "_MOTIF_WM_INFO", True);
        if (a)
        {
                struct BzfPropMotifWmInfo
                {
                public:
                        long  flags;
                        Window  wmWindow;
                };

                Atom type;
                int format;
                unsigned long nitems;
                unsigned long bytes_after;
                long* mwmInfo;

                XGetWindowProperty(dpy,rootwin,a, 0, 4, False,a, &type, &format, &nitems, &bytes_after,(unsigned
char**)&mwmInfo);

                if (mwmInfo)
                {
                        // get the mwm window from the properties
                        const Window mwmWindow = ((BzfPropMotifWmInfo*)mwmInfo)->wmWindow;
                        XFree(mwmInfo);

                        // verify that window is a child of the root window
                        Window root, parent, *children;
                        unsigned int numChildren;
                        if (XQueryTree(dpy, mwmWindow, &root, &parent,   &children, &numChildren))
                        {
                                XFree(children);
                                if (parent == rootwin)
                                        isMWMRunning = True;
                        }
                }
        }

        // turning off decorations is window manager dependent
        if (isMWMRunning)
        {
                fprintf(stdout,"Motif Window Manager\n");
                // it's a Motif based window manager
                long hints[4];
                hints[0] = 0;
                hints[1] = 0;
                hints[2] = 0;
                hints[3] = 0;
                long* xhints;

                a = XInternAtom(dpy, "_MOTIF_WM_HINTS", False);

                {
                        // get current hints
                        Atom type;
                        int format;
                        unsigned long nitems;
                        unsigned long bytes_after;

                        XGetWindowProperty(dpy, window, a, 0, 4, False,a, &type, &format, &nitems,
&bytes_after,(unsigned char**)&xhints);

                        if (xhints)
                        {
                                hints[0] = xhints[0];
                                hints[1] = xhints[1];
                                hints[2] = xhints[2];
                                hints[3] = xhints[3];
                                XFree(xhints);
                        }
                }

                hints[0] |= 2;  // MWM_HINTS_DECORATIONS flag
                hints[2] = 0;  // no decorations

                XChangeProperty(dpy, window, a, a, 32,PropModeReplace, (unsigned char*)&hints, 4);

                noWM = False;
        }else
        {
                // non-motif window manager.  use override redirect to prevent window
                // manager from messing with our appearance.  unfortunately, the user
                // can't move or iconify the window either.
                XSetWindowAttributes attr;
                attr.override_redirect = True;
                XChangeWindowAttributes(dpy,window, CWOverrideRedirect, &attr);
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

                env=getenv("MESA_GLX_FX");
                if (env && *env=='f') // Full screen Mesa mode
                {
                        xsh.base_width=640;
                        xsh.base_height=480;
                }else
                {
                        // Check if we have the XF86 vidmode extension, for virtual roots
                        if (LinuxVideoMode.QueryExtension(dpy))
                        {
                                int dotclock;
                                XF86VidModeModeLine modeline;

                                LinuxVideoMode.GetModeLine(dpy,DisplayScreen,&dotclock,&modeline);

                                xsh.base_width=modeline.hdisplay;
                                xsh.base_height=modeline.vdisplay;

                                //if (modeline.c_private)
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
        if (!noWM)
        {
                XSetWindowAttributes attr;
                attr.override_redirect = True;
                XChangeWindowAttributes(dpy,window, CWOverrideRedirect, &attr);
        }
        XSetWMNormalHints(dpy, window, &xsh);
        XMoveResizeWindow(dpy, window, xsh.x, xsh.y, xsh.base_width, xsh.base_height);

        if (!noWM)
        {
                XSetWindowAttributes attr;
                attr.override_redirect = False;
                XChangeWindowAttributes(dpy,window, CWOverrideRedirect, &attr);
        }

        XSync(dpy, False);
}
#endif
*/
