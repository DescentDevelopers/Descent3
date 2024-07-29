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

#include <cstddef>
#include <functional>
#include <stdexcept>
#include <string>
#include "dyna_gl.h"
#include "holder.h"

template <GLint Size, GLenum Type, GLboolean Normalized, auto Member>
struct VertexAttrib;
template <GLint Size, GLenum Type, GLboolean Normalized, typename EnclosingType, typename MemberType, MemberType EnclosingType::*Member>
struct VertexAttrib<Size, Type, Normalized, Member> {
  using VertexType = EnclosingType;
  static constexpr GLint kSize = Size;
  static constexpr GLenum kType = Type;
  static constexpr GLboolean kNormalized = Normalized;
  static void const* offset() {
    EnclosingType e;
    return reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(&(e.*Member)) - reinterpret_cast<uintptr_t>(&e));
  }
};

template <typename V, typename... Traits>
struct VertexBuffer {
  static_assert((std::is_same_v<V, typename Traits::VertexType> && ...), "Vertex attributes must all be for the same vertex type");

  VertexBuffer(size_t vertex_count, GLenum bufferType, V const* initialData = nullptr) : vao_{outval(dglGenVertexArrays)}, vbo_{outval(dglGenBuffers)} {
    dglBindVertexArray(vao_);
    dglBindBuffer(GL_ARRAY_BUFFER, vbo_);
    dglBufferData(GL_ARRAY_BUFFER, vertex_count * sizeof(PosColorUV2Vertex), initialData, bufferType);

    GLint location{};
    ((dglEnableVertexAttribArray(location),
      dglVertexAttribPointer(location, Traits::kSize, Traits::kType, Traits::kNormalized, sizeof(V), Traits::offset()),
      location++),
     ...);
  }

  void UpdateData(size_t vtx_offset, size_t vtx_count, V const* vertices) const {
    dglBindBuffer(GL_ARRAY_BUFFER, vbo_);
    dglBufferSubData(GL_ARRAY_BUFFER, vtx_offset * sizeof(V), vtx_count * sizeof(V), vertices);
  }

private:
  static void DeleteBuffer(GLuint id) {
    dglDeleteBuffers(1, &id);
  }
  static void DeleteVertexArray(GLuint id) {
    dglDeleteVertexArrays(1, &id);
  }
  template <typename Generator>
  GLuint outval(Generator&& gen) {
    GLuint id;
    gen(1, &id);
    return id;
  }

  MoveOnlyHolder<GLuint, DeleteVertexArray> vao_;
  MoveOnlyHolder<GLuint, DeleteBuffer> vbo_;
};

template <GLenum kType>
struct Shader {
  static_assert(kType == GL_VERTEX_SHADER || kType == GL_FRAGMENT_SHADER);

  explicit Shader(std::string_view src)
      : id_{dglCreateShader(kType)} {
    if (id_ == 0) {
      throw std::runtime_error("failed to create shader");
    }

    char const* srcptr = src.data();
    GLint srclen = src.size();
    dglShaderSource(id_, 1, &srcptr, &srclen);
    dglCompileShader(id_);

    GLint compile_result;
    dglGetShaderiv(id_, GL_COMPILE_STATUS, &compile_result);
    if (compile_result != GL_TRUE) {
      GLsizei log_length = 0;
      GLchar message[1024];
      dglGetShaderInfoLog(id_, 1024, &log_length, message);
      throw std::runtime_error(std::string{message, static_cast<size_t>(log_length)});
    }
  }

  GLuint id() const {
    ASSERT(id_ != 0);
    return id_;
  }

private:
  static void DeleteShader(GLuint id) {
    dglDeleteShader(id);
  }
  MoveOnlyHolder<GLuint, DeleteShader> id_;
};

struct ShaderProgram {
  explicit ShaderProgram(std::string_view vertexSrc, std::string_view fragmentSrc)
    : vertex_{vertexSrc}, fragment_{fragmentSrc}, id_{dglCreateProgram()} {
    if (id_ == 0) {
      throw std::runtime_error("error creating GL program");
    }

    dglAttachShader(id_, vertex_.id());
    dglAttachShader(id_, fragment_.id());

    dglLinkProgram(id_);

    GLint link_result;
    dglGetProgramiv(id_, GL_LINK_STATUS, &link_result);
    if (link_result != GL_TRUE)
    {
      GLsizei log_length = 0;
      GLchar message[1024];
      dglGetProgramInfoLog(id_, 1024, &log_length, message);
      throw std::runtime_error(std::string{message, static_cast<size_t>(log_length)});
    }
  }

  void Use() const {
    dglUseProgram(id_);
  }

  void Unuse() const {
    dglUseProgram(0);
  }

private:
  static void DeleteProgram(GLuint id) {
    dglDeleteProgram(id);
  }

  Shader<GL_VERTEX_SHADER> vertex_;
  Shader<GL_FRAGMENT_SHADER> fragment_;
  MoveOnlyHolder<GLuint, DeleteProgram> id_;
};