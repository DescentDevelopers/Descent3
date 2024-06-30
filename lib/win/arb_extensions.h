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

#ifndef ARB_EXTENSIONS_H

#define ARB_EXTENSIONS_H

#ifndef APIENTRY
#include <windows.h>
#endif

/* ARB_multitexture */
#define GL_ACTIVE_TEXTURE_ARB 0x84E0
#define GL_CLIENT_ACTIVE_TEXTURE_ARB 0x84E1
#define GL_MAX_TEXTURES_UNITS_ARB 0x84E2
#define GL_TEXTURE0_ARB 0x84C0
#define GL_TEXTURE1_ARB 0x84C1
#define GL_TEXTURE2_ARB 0x84C2
#define GL_TEXTURE3_ARB 0x84C3

/* ARB_multitexture */
typedef void(APIENTRY *PFNGLMULTITEXCOORD1DARBPROC)(GLenum target, GLdouble s);
typedef void(APIENTRY *PFNGLMULTITEXCOORD1DVARBPROC)(GLenum target, const GLdouble *v);
typedef void(APIENTRY *PFNGLMULTITEXCOORD1FARBPROC)(GLenum target, GLfloat s);
typedef void(APIENTRY *PFNGLMULTITEXCOORD1FVARBPROC)(GLenum target, const GLfloat *v);
typedef void(APIENTRY *PFNGLMULTITEXCOORD1IARBPROC)(GLenum target, GLint s);
typedef void(APIENTRY *PFNGLMULTITEXCOORD1IVARBPROC)(GLenum target, const GLint *v);
typedef void(APIENTRY *PFNGLMULTITEXCOORD1SARBPROC)(GLenum target, GLshort s);
typedef void(APIENTRY *PFNGLMULTITEXCOORD1SVARBPROC)(GLenum target, const GLshort *v);
typedef void(APIENTRY *PFNGLMULTITEXCOORD2DARBPROC)(GLenum target, GLdouble s, GLdouble t);
typedef void(APIENTRY *PFNGLMULTITEXCOORD2DVARBPROC)(GLenum target, const GLdouble *v);
typedef void(APIENTRY *PFNGLMULTITEXCOORD2FARBPROC)(GLenum target, GLfloat s, GLfloat t);
typedef void(APIENTRY *PFNGLMULTITEXCOORD2FVARBPROC)(GLenum target, const GLfloat *v);
typedef void(APIENTRY *PFNGLMULTITEXCOORD2IARBPROC)(GLenum target, GLint s, GLint t);
typedef void(APIENTRY *PFNGLMULTITEXCOORD2IVARBPROC)(GLenum target, const GLint *v);
typedef void(APIENTRY *PFNGLMULTITEXCOORD2SARBPROC)(GLenum target, GLshort s, GLshort t);
typedef void(APIENTRY *PFNGLMULTITEXCOORD2SVARBPROC)(GLenum target, const GLshort *v);
typedef void(APIENTRY *PFNGLMULTITEXCOORD3DARBPROC)(GLenum target, GLdouble s, GLdouble t, GLdouble r);
typedef void(APIENTRY *PFNGLMULTITEXCOORD3DVARBPROC)(GLenum target, const GLdouble *v);
typedef void(APIENTRY *PFNGLMULTITEXCOORD3FARBPROC)(GLenum target, GLfloat s, GLfloat t, GLfloat r);
typedef void(APIENTRY *PFNGLMULTITEXCOORD3FVARBPROC)(GLenum target, const GLfloat *v);
typedef void(APIENTRY *PFNGLMULTITEXCOORD3IARBPROC)(GLenum target, GLint s, GLint t, GLint r);
typedef void(APIENTRY *PFNGLMULTITEXCOORD3IVARBPROC)(GLenum target, const GLint *v);
typedef void(APIENTRY *PFNGLMULTITEXCOORD3SARBPROC)(GLenum target, GLshort s, GLshort t, GLshort r);
typedef void(APIENTRY *PFNGLMULTITEXCOORD3SVARBPROC)(GLenum target, const GLshort *v);
typedef void(APIENTRY *PFNGLMULTITEXCOORD4DARBPROC)(GLenum target, GLdouble s, GLdouble t, GLdouble r, GLdouble q);
typedef void(APIENTRY *PFNGLMULTITEXCOORD4DVARBPROC)(GLenum target, const GLdouble *v);
typedef void(APIENTRY *PFNGLMULTITEXCOORD4FARBPROC)(GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q);
typedef void(APIENTRY *PFNGLMULTITEXCOORD4FVARBPROC)(GLenum target, const GLfloat *v);
typedef void(APIENTRY *PFNGLMULTITEXCOORD4IARBPROC)(GLenum target, GLint s, GLint t, GLint r, GLint q);
typedef void(APIENTRY *PFNGLMULTITEXCOORD4IVARBPROC)(GLenum target, const GLint *v);
typedef void(APIENTRY *PFNGLMULTITEXCOORD4SARBPROC)(GLenum target, GLshort s, GLshort t, GLshort r, GLshort q);
typedef void(APIENTRY *PFNGLMULTITEXCOORD4SVARBPROC)(GLenum target, const GLshort *v);
typedef void(APIENTRY *PFNGLACTIVETEXTUREARBPROC)(GLenum target);
typedef void(APIENTRY *PFNGLCLIENTACTIVETEXTUREARBPROC)(GLenum target);

#endif
