// ryan's add. Why wasn't this here? 04/18/2000
#ifndef _INCLUDE_DYNA_GL_H_
#define _INCLUDE_DYNA_GL_H_
// end ryan's add.

#include "gl.h"
#include "module.h"

#if defined(WIN32)
#define GLFUNCCALL __stdcall
#elif defined(__LINUX__)
#define GLFUNCCALL
// ryan's add. 04/18/2000
typedef void(GLFUNCCALL *PFNGLACTIVETEXTUREARBPROC)(GLenum texture);
typedef void(GLFUNCCALL *PFNGLCLIENTACTIVETEXTUREARBPROC)(GLenum texture);
typedef void(GLFUNCCALL *PFNGLMULTITEXCOORD4FARBPROC)(GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q);
// end ryan's add.
//#include <GL/glx.h>
#else
#define GLFUNCCALL
#endif

// ryan's adds. 04/18/2000.
//  There was a bunch of stuff that was, I guess, not supported by
//  Mesa when Outrage did the original port. So buttloads of code
//  involving glActiveTextureARB() were #ifdef'd to only WIN32.
//  To make use of the extension more generic, platforms that can handle
//  it (Linux and Win32, as far as I care), define
//  _USE_OGL_ACTIVE_TEXTURES, and check that define instead of WIN32 when
//  using the extension.
#if (!defined(_USE_OGL_ACTIVE_TEXTURES))
#if (defined(WIN32))
#define _USE_OGL_ACTIVE_TEXTURES
#endif
#endif
// end ryan's adds.

typedef void(GLFUNCCALL *glAlphaFunc_fp)(GLenum func, GLclampf ref);
typedef void(GLFUNCCALL *glBegin_fp)(GLenum mode);
typedef void(GLFUNCCALL *glBindTexture_fp)(GLenum target, GLuint texture);
typedef void(GLFUNCCALL *glBlendFunc_fp)(GLenum sfactor, GLenum dfactor);
typedef void(GLFUNCCALL *glClear_fp)(GLbitfield mask);
typedef void(GLFUNCCALL *glClearColor_fp)(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
typedef void(GLFUNCCALL *glClearDepth_fp)(GLclampd depth);
typedef void(GLFUNCCALL *glColor3ub_fp)(GLubyte red, GLubyte green, GLubyte blue);
typedef void(GLFUNCCALL *glColor4ub_fp)(GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha);
typedef void(GLFUNCCALL *glColor4f_fp)(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
typedef void(GLFUNCCALL *glColorPointer_fp)(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
typedef void(GLFUNCCALL *glDeleteTextures_fp)(GLsizei n, const GLuint *textures);
typedef void(GLFUNCCALL *glDepthFunc_fp)(GLenum func);
typedef void(GLFUNCCALL *glDepthMask_fp)(GLboolean flag);
typedef void(GLFUNCCALL *glDepthRange_fp)(GLclampd zNear, GLclampd zFar);
typedef void(GLFUNCCALL *glDisable_fp)(GLenum cap);
typedef void(GLFUNCCALL *glDisableClientState_fp)(GLenum array);
typedef void(GLFUNCCALL *glDrawArrays_fp)(GLenum mode, GLint first, GLsizei count);
typedef void(GLFUNCCALL *glDrawPixels_fp)(GLsizei width, GLsizei height, GLenum format, GLenum type,
                                          const GLvoid *pixels);
typedef void(GLFUNCCALL *glEnable_fp)(GLenum cap);
typedef void(GLFUNCCALL *glEnableClientState_fp)(GLenum array);
typedef void(GLFUNCCALL *glEnd_fp)(void);
typedef void(GLFUNCCALL *glFlush_fp)(void);
typedef void(GLFUNCCALL *glFogf_fp)(GLenum pname, GLfloat param);
typedef void(GLFUNCCALL *glFogfv_fp)(GLenum pname, const GLfloat *params);
typedef void(GLFUNCCALL *glFogi_fp)(GLenum pname, GLint param);
typedef GLenum(GLFUNCCALL *glGetError_fp)(void);
typedef const GLubyte *(GLFUNCCALL *glGetString_fp)(GLenum name);
typedef void(GLFUNCCALL *glHint_fp)(GLenum target, GLenum mode);
typedef void(GLFUNCCALL *glLoadIdentity_fp)(void);
typedef void(GLFUNCCALL *glMatrixMode_fp)(GLenum mode);
typedef void(GLFUNCCALL *glOrtho_fp)(GLdouble, GLdouble, GLdouble, GLdouble, GLdouble, GLdouble);
typedef void(GLFUNCCALL *glPixelStorei_fp)(GLenum pname, GLint param);
typedef void(GLFUNCCALL *glPixelTransferi_fp)(GLenum pname, GLint param);
typedef void(GLFUNCCALL *glPolygonOffset_fp)(GLfloat factor, GLfloat units);
typedef void(GLFUNCCALL *glReadPixels_fp)(GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, GLvoid *);
typedef void(GLFUNCCALL *glScissor_fp)(GLint x, GLint y, GLsizei width, GLsizei height);
typedef void(GLFUNCCALL *glShadeModel_fp)(GLenum mode);
typedef void(GLFUNCCALL *glTexCoordPointer_fp)(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
typedef void(GLFUNCCALL *glTexEnvf_fp)(GLenum target, GLenum pname, GLfloat param);
typedef void(GLFUNCCALL *glTexImage2D_fp)(GLenum target, GLint level, GLint internalformat, GLsizei width,
                                          GLsizei height, GLint border, GLenum format, GLenum type,
                                          const GLvoid *pixels);
typedef void(GLFUNCCALL *glTexParameteri_fp)(GLenum target, GLenum pname, GLint param);
typedef void(GLFUNCCALL *glTexSubImage2D_fp)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width,
                                             GLsizei height, GLenum format, GLenum type, const GLvoid *pixels);
typedef void(GLFUNCCALL *glVertex2i_fp)(GLint, GLint);
typedef void(GLFUNCCALL *glVertex3f_fp)(GLfloat, GLfloat, GLfloat);
typedef void(GLFUNCCALL *glVertexPointer_fp)(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
typedef void(GLFUNCCALL *glViewport_fp)(GLint x, GLint y, GLsizei width, GLsizei height);
typedef void(GLFUNCCALL *glColor4fv_fp)(const GLfloat *v);
typedef void(GLFUNCCALL *glVertex3fv_fp)(const GLfloat *v);
typedef void(GLFUNCCALL *glTexCoord4fv_fp)(const GLfloat *v);
typedef void(GLFUNCCALL *glTexCoord2f_fp)(const GLfloat p1, const GLfloat p2);
typedef void(GLFUNCCALL *glGetIntegerv_fp)(GLenum pname, GLint *params);
typedef GLuint(GLFUNCCALL *glGenLists_fp)(GLsizei range);
typedef void(GLFUNCCALL *glNewList_fp)(GLuint list, GLenum mode);
typedef void(GLFUNCCALL *glEndList_fp)(void);
typedef void(GLFUNCCALL *glCallLists_fp)(GLsizei n, GLenum type, const GLvoid *lists);
typedef void(GLFUNCCALL *glListBase_fp)(GLuint base);
typedef void(GLFUNCCALL *glPushAttrib_fp)(GLbitfield mask);
typedef void(GLFUNCCALL *glPopAttrib_fp)(void);
typedef void(GLFUNCCALL *glColor3f_fp)(GLfloat red, GLfloat green, GLfloat blue);
typedef void(GLFUNCCALL *glRasterPos2i_fp)(GLint x, GLint y);
typedef void(GLFUNCCALL *glBitmap_fp)(GLsizei width, GLsizei height, GLfloat xorig, GLfloat yorig, GLfloat xmove,
                                      GLfloat ymove, const GLubyte *bitmap);

#if defined(WIN32)
typedef HGLRC(GLFUNCCALL *wglCreateContext_fp)(HDC);
typedef BOOL(GLFUNCCALL *wglDeleteContext_fp)(HGLRC);
typedef BOOL(GLFUNCCALL *wglMakeCurrent_fp)(HDC, HGLRC);
typedef PROC(GLFUNCCALL *wglGetProcAddress_fp)(LPCSTR);

extern wglCreateContext_fp dwglCreateContext;
extern wglDeleteContext_fp dwglDeleteContext;
extern wglMakeCurrent_fp dwglMakeCurrent;
extern wglGetProcAddress_fp dwglGetProcAddress;

/* bk00614 - fuckit
#elif defined(__LINUX__)
typedef Bool (*glXQueryExtension_fp)( Display *dpy, int *errorb, int *event );
typedef GLXContext (*glXCreateContext_fp)( Display *dpy, XVisualInfo *vis,GLXContext shareList, Bool direct );
typedef Bool (*glXMakeCurrent_fp)( Display *dpy, GLXDrawable drawable,GLXContext ctx);
typedef void (*glXSwapBuffers_fp)( Display *dpy, GLXDrawable drawable );
typedef int (*glXGetConfig_fp)( Display *dpy, XVisualInfo *visual, int attrib, int *value );
typedef XVisualInfo* (*glXChooseVisual_fp)( Display *dpy, int screen, int *attribList );
typedef void (*glXDestroyContext_fp)( Display *dpy, GLXContext ctx );
typedef void (*glXWaitGL_fp)( void );

glXQueryExtension_fp dglXQueryExtension=NULL;
glXCreateContext_fp dglXCreateContext=NULL;
glXMakeCurrent_fp dglXMakeCurrent=NULL;
glXSwapBuffers_fp dglXSwapBuffers=NULL;
glXGetConfig_fp dglXGetConfig=NULL;
glXChooseVisual_fp dglXChooseVisual=NULL;
glXDestroyContext_fp dglXDestroyContext=NULL;
glXWaitGL_fp dglXWaitGL=NULL;
*/
#else

#endif

// bk000614 - have to fix globals
extern glAlphaFunc_fp dglAlphaFunc;
extern glBegin_fp dglBegin;
extern glBindTexture_fp dglBindTexture;
extern glBlendFunc_fp dglBlendFunc;
extern glClear_fp dglClear;
extern glClearColor_fp dglClearColor;
extern glClearDepth_fp dglClearDepth;
extern glColor3ub_fp dglColor3ub;
extern glColor4f_fp dglColor4f;
extern glColor4fv_fp dglColor4fv;
extern glColor4ub_fp dglColor4ub;
extern glColorPointer_fp dglColorPointer;
extern glDeleteTextures_fp dglDeleteTextures;
extern glDepthFunc_fp dglDepthFunc;
extern glDepthMask_fp dglDepthMask;
extern glDepthRange_fp dglDepthRange;
extern glDisable_fp dglDisable;
extern glDisableClientState_fp dglDisableClientState;
extern glDrawArrays_fp dglDrawArrays;
extern glDrawPixels_fp dglDrawPixels;
extern glEnable_fp dglEnable;
extern glEnableClientState_fp dglEnableClientState;
extern glEnd_fp dglEnd;
extern glFlush_fp dglFlush;
extern glFogf_fp dglFogf;
extern glFogfv_fp dglFogfv;
extern glFogi_fp dglFogi;
extern glGetError_fp dglGetError;
extern glGetString_fp dglGetString;
extern glGetIntegerv_fp dglGetIntegerv;
extern glHint_fp dglHint;
extern glLoadIdentity_fp dglLoadIdentity;
extern glMatrixMode_fp dglMatrixMode;
extern glOrtho_fp dglOrtho;
extern glPixelStorei_fp dglPixelStorei;
extern glPixelTransferi_fp dglPixelTransferi;
extern glPolygonOffset_fp dglPolygonOffset;
extern glReadPixels_fp dglReadPixels;
extern glScissor_fp dglScissor;
extern glShadeModel_fp dglShadeModel;
extern glTexCoord2f_fp dglTexCoord2f;
extern glTexCoord4fv_fp dglTexCoord4fv;
extern glTexCoordPointer_fp dglTexCoordPointer;
extern glTexEnvf_fp dglTexEnvf;
extern glTexImage2D_fp dglTexImage2D;
extern glTexParameteri_fp dglTexParameteri;
extern glTexSubImage2D_fp dglTexSubImage2D;
extern glVertex2i_fp dglVertex2i;
extern glVertex3f_fp dglVertex3f;
extern glVertex3fv_fp dglVertex3fv;
extern glVertexPointer_fp dglVertexPointer;
extern glViewport_fp dglViewport;

module *LoadOpenGLDLL(char *dllname);

// ryan's add. 10/04/2000
#ifdef __CHECK_FOR_TOO_SLOW_RENDERING__
void setMinimumAcceptableRenderTime(int ms);
#endif

// ryan's add. Why wasn't this here? 04/18/2000
#endif
// end ryan's add.
