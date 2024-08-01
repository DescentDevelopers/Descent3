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

#pragma once

#include <filesystem>
#include <stdexcept>
#include <string>
#include <tuple>
#include <vector>

#define GL_GLEXT_PROTOTYPES
#include <SDL_opengl.h>

#include "descent.h"
#include "module.h"
#include "pserror.h"

#if defined(WIN32)
#define GLFUNCCALL __stdcall
#elif defined(POSIX)
#include <unistd.h>
#define GLFUNCCALL
#endif

inline void CheckError() {
#if defined(_DEBUG)
  static auto _dglGetError = reinterpret_cast<GLenum (*)()>(SDL_GL_GetProcAddress("glGetError"));
  auto err = _dglGetError();
  switch (err) {
  case GL_NO_ERROR:
    return;
  case GL_INVALID_ENUM:
    throw std::runtime_error("invalid enum");
  case GL_INVALID_VALUE:
    throw std::runtime_error("invalid value");
  case GL_INVALID_OPERATION:
    throw std::runtime_error("invalid operation");
  case GL_INVALID_FRAMEBUFFER_OPERATION:
    throw std::runtime_error("invalid framebuffer operation");
  case GL_INVALID_INDEX:
    throw std::runtime_error("invalid index");
  default:
    throw std::runtime_error("unknown error: " + std::to_string(err));
  }
#endif
}

template <typename T> struct FnPtr;
template <typename Ret, typename... Args>
struct FnPtr<Ret(Args...)> {
  explicit FnPtr(std::string_view name, bool optional = false);

  Ret operator()(Args... args) const {
    if constexpr (std::is_same_v<Ret, void>) {
      fn_(args...);
      CheckError();
    } else {
      auto ret = fn_(args...);
      CheckError();
      return ret;
    }
  }

  operator bool() const { return fn_; }

private:
  Ret (GLFUNCCALL *fn_)(Args...);
};

#if defined(DECLARE_OPENGL)
extern char loadedLibrary[_MAX_PATH];
static module OpenGLDLLInst;

static std::vector<std::tuple<void **, std::string_view, bool>> inits_;
static void LoadGLFnPtrs() {
  for (auto &[ptr, name, optional] : inits_) {
    *ptr = SDL_GL_GetProcAddress(name.data());
    if (!*ptr && !optional) {
      throw std::runtime_error(std::string{"failed to find "} + name.data());
    }
  }
  inits_.clear();
}

template<typename Ret, typename... Args>
FnPtr<Ret(Args...)>::FnPtr(std::string_view name, bool optional) : fn_{} {
  inits_.push_back(std::make_tuple(reinterpret_cast<void**>(&fn_), name, optional));
}

static module *LoadOpenGLDLL(const char *dllname) {
  mprintf(0, "Loading OpenGL dll...\n");

  std::filesystem::path tmp = std::filesystem::current_path();
  std::filesystem::current_path(orig_pwd);
  int rc = SDL_GL_LoadLibrary(dllname[0] ? dllname : nullptr);
  std::filesystem::current_path(tmp);

  if (rc < 0) {
    const char *sdlErr = SDL_GetError();
    mprintf(0, "OpenGL: Couldn't open library [%s].\n", dllname[0] ? dllname : "system default");
    mprintf(0, "OpenGL:  SDL error is [%s].", sdlErr);
    return NULL;
  }

  strcpy(loadedLibrary, dllname);

  mprintf(0, "OpenGL dll loading successful.\n");

  return &OpenGLDLLInst;
}

#define DYNAEXTERN_OPT(Name, Opt) FnPtr<decltype(Name)> const d##Name{#Name, Opt}
#define DYNAEXTERN(Name) DYNAEXTERN_OPT(Name, false)
#else
#define DYNAEXTERN(Name) extern FnPtr<decltype(Name)> const d##Name
#endif

DYNAEXTERN(glAttachShader);
DYNAEXTERN(glBindAttribLocation);
DYNAEXTERN(glBindBuffer);
DYNAEXTERN(glBindTexture);
DYNAEXTERN(glBindVertexArray);
DYNAEXTERN(glBlendFunc);
DYNAEXTERN(glBufferData);
DYNAEXTERN(glBufferSubData);
DYNAEXTERN(glClear);
DYNAEXTERN(glClearColor);
DYNAEXTERN(glCompileShader);
DYNAEXTERN(glCreateProgram);
DYNAEXTERN(glCreateShader);
DYNAEXTERN(glDeleteBuffers);
DYNAEXTERN(glDeleteProgram);
DYNAEXTERN(glDeleteShader);
DYNAEXTERN(glDeleteTextures);
DYNAEXTERN(glDeleteVertexArrays);
DYNAEXTERN(glDepthFunc);
DYNAEXTERN(glDepthMask);
DYNAEXTERN(glDisable);
DYNAEXTERN(glDrawArrays);
DYNAEXTERN(glEnable);
DYNAEXTERN(glEnableVertexAttribArray);
DYNAEXTERN(glFlush);
DYNAEXTERN(glFogf);
DYNAEXTERN(glFogfv);
DYNAEXTERN(glFogi);
DYNAEXTERN(glGenBuffers);
DYNAEXTERN(glGenTextures);
DYNAEXTERN(glGenVertexArrays);
DYNAEXTERN(glGetError);
DYNAEXTERN(glGetIntegerv);
DYNAEXTERN(glGetProgramInfoLog);
DYNAEXTERN(glGetProgramiv);
DYNAEXTERN(glGetShaderInfoLog);
DYNAEXTERN(glGetShaderiv);
DYNAEXTERN(glGetString);
DYNAEXTERN(glGetStringi);
DYNAEXTERN(glGetUniformLocation);
DYNAEXTERN(glHint);
DYNAEXTERN(glLinkProgram);
DYNAEXTERN(glPixelStorei);
DYNAEXTERN(glPolygonOffset);
DYNAEXTERN(glReadPixels);
DYNAEXTERN(glScissor);
DYNAEXTERN(glShaderSource);
DYNAEXTERN(glTexImage2D);
DYNAEXTERN(glTexParameteri);
DYNAEXTERN(glTexSubImage2D);
DYNAEXTERN(glUniform1i);
DYNAEXTERN(glUniformMatrix4fv);
DYNAEXTERN(glUseProgram);
DYNAEXTERN(glVertexAttribPointer);
DYNAEXTERN(glViewport);

// FBO entry points for render-to-texture ...
DYNAEXTERN(glBindFramebufferEXT);
DYNAEXTERN(glBindRenderbufferEXT);
DYNAEXTERN(glBlitFramebufferEXT);
DYNAEXTERN(glCheckFramebufferStatusEXT);
DYNAEXTERN(glDeleteFramebuffersEXT);
DYNAEXTERN(glDeleteRenderbuffersEXT);
DYNAEXTERN(glFramebufferRenderbufferEXT);
DYNAEXTERN(glGenFramebuffersEXT);
DYNAEXTERN(glGenRenderbuffersEXT);
DYNAEXTERN(glRenderbufferStorageEXT);

DYNAEXTERN(glActiveTextureARB);
DYNAEXTERN(glMultiTexCoord4f);

#if defined(WIN32)
DYNAEXTERN(wglCreateContext);
DYNAEXTERN(wglDeleteContext);
DYNAEXTERN(wglGetProcAddress);
DYNAEXTERN(wglMakeCurrent);
#endif
