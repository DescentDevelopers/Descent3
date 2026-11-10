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

#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "d3_test/byte_writer.h"

namespace pof {

struct Vec {
  float x = 0, y = 0, z = 0;
};

struct FaceVert {
  int vertnum;
  float u, v;
};

struct Face {
  Vec normal;
  int texnum = -1; // -1 = untextured, use rgb
  uint8_t r = 0, g = 0, b = 0;
  std::vector<FaceVert> verts;
};

struct SubObject {
  int parent = -1;
  Vec offset;
  std::string name;
  std::string props;
  std::vector<Vec> verts;
  std::vector<float> alpha; // version >= 2300 only; defaults to 1.0
  std::vector<Face> faces;
};

struct Point {
  int parent = 0;
  Vec pnt;
  Vec norm;
};

struct RotKey {
  int start_time = 0; // timed (version >= 2200) models only
  Vec axis;
  int angle = 0; // 16-bit fixed angle, 0x4000 == 90 degrees
};

struct PosKey {
  int start_time = 0; // timed (version >= 2200) models only
  Vec pos;
};

struct Track {
  int track_min = 0, track_max = 0; // timed models only
  std::vector<RotKey> rot;
  std::vector<PosKey> pos;
};

struct WeaponBattery {
  std::vector<int> gun_points;
  std::vector<int> turrets;
};

class Writer {
public:
  explicit Writer(int version) : version_(version) { buf_.Text("PSPO").I32(version); }

  int version() const { return version_; }

  void header(int n_models, float rad = 0.0f, Vec mins = {}, Vec maxs = {}) {
    begin("OHDR");
    buf_.I32(n_models);
    buf_.F32(rad);
    vec(mins);
    vec(maxs);
    buf_.I32(0); // detail level count
    end();
  }

  void subobject(int index, const SubObject &so) {
    const int major = version_ / 100;
    begin("SOBJ");
    buf_.I32(index);
    buf_.I32(so.parent);
    vec({0, 0, 1}); // separation plane normal (unused by the new-style loader)
    buf_.F32(0);    // separation plane d
    vec({});        // separation plane point
    vec(so.offset);
    buf_.F32(0); // radius
    buf_.I32(0); // tree offset
    buf_.I32(0); // data offset
    if (version_ > 1805)
      vec({}); // geometric center
    str(so.name);
    str(so.props);
    buf_.I32(0); // movement type
    buf_.I32(0); // movement axis
    buf_.I32(0); // freespace chunk count
    buf_.I32(static_cast<int>(so.verts.size()));
    for (const Vec &v : so.verts)
      vec(v);
    for (size_t i = 0; i < so.verts.size(); i++)
      vec({0, 0, 1}); // vertex normals
    if (major >= 23) {
      for (size_t i = 0; i < so.verts.size(); i++)
        buf_.F32(i < so.alpha.size() ? so.alpha[i] : 1.0f);
    }
    buf_.I32(static_cast<int>(so.faces.size()));
    for (const Face &f : so.faces) {
      vec(f.normal);
      buf_.I32(static_cast<int>(f.verts.size()));
      if (f.texnum >= 0) {
        buf_.I32(1);
        buf_.I32(f.texnum);
      } else {
        buf_.I32(0);
        buf_.U8(f.r);
        buf_.U8(f.g);
        buf_.U8(f.b);
      }
      for (const FaceVert &fv : f.verts) {
        buf_.I32(fv.vertnum);
        buf_.F32(fv.u);
        buf_.F32(fv.v);
      }
      if (major >= 21) {
        buf_.F32(0); // lightmap x resolution
        buf_.F32(0); // lightmap y resolution
      }
    }
    end();
  }

  void textures(const std::vector<std::string> &names) {
    begin("TXTR");
    buf_.I32(static_cast<int>(names.size()));
    for (const std::string &n : names)
      str(n);
    end();
  }

  void gun_points(const std::vector<Point> &points) {
    begin("GPNT");
    buf_.I32(static_cast<int>(points.size()));
    for (const Point &p : points) {
      if (version_ >= 1908)
        buf_.I32(p.parent);
      vec(p.pnt);
      vec(p.norm);
    }
    end();
  }

  void ground_planes(const std::vector<Point> &points) {
    begin("GRND");
    buf_.I32(static_cast<int>(points.size()));
    for (const Point &p : points) {
      buf_.I32(p.parent);
      vec(p.pnt);
      vec(p.norm);
    }
    end();
  }

  void attach_points(const std::vector<Point> &points) {
    begin("ATCH");
    buf_.I32(static_cast<int>(points.size()));
    for (const Point &p : points) {
      buf_.I32(p.parent);
      vec(p.pnt);
      vec(p.norm);
    }
    end();
  }

  void attach_uvecs(const std::vector<Vec> &uvecs) {
    begin("NATH");
    buf_.I32(static_cast<int>(uvecs.size()));
    for (const Vec &u : uvecs) {
      buf_.I32(0); // unused
      vec({});     // unused
      vec(u);
    }
    end();
  }

  void weapon_batteries(const std::vector<WeaponBattery> &wbs) {
    begin("WBAT");
    buf_.I32(static_cast<int>(wbs.size()));
    for (const WeaponBattery &wb : wbs) {
      buf_.I32(static_cast<int>(wb.gun_points.size()));
      for (int g : wb.gun_points)
        buf_.I32(g);
      buf_.I32(static_cast<int>(wb.turrets.size()));
      for (int t : wb.turrets)
        buf_.I32(t);
    }
    end();
  }

  // One Track per subobject, in subobject order. Untimed models (version < 2200) share one key
  // count across all subobjects, taken from tracks[0].
  void rot_anim(const std::vector<Track> &tracks) {
    const bool timed = version_ / 100 >= 22;
    begin("RANI");
    if (!timed)
      buf_.I32(static_cast<int>(tracks.at(0).rot.size()));
    for (const Track &t : tracks) {
      if (timed) {
        buf_.I32(static_cast<int>(t.rot.size()));
        buf_.I32(t.track_min);
        buf_.I32(t.track_max);
      }
      for (const RotKey &k : t.rot) {
        if (timed)
          buf_.I32(k.start_time);
        vec(k.axis);
        buf_.I32(k.angle);
      }
    }
    end();
  }

  void pos_anim(const std::vector<Track> &tracks) {
    const bool timed = version_ / 100 >= 22;
    begin("PANI");
    if (!timed)
      buf_.I32(static_cast<int>(tracks.at(0).pos.size()));
    for (const Track &t : tracks) {
      if (timed) {
        buf_.I32(static_cast<int>(t.pos.size()));
        buf_.I32(t.track_min);
        buf_.I32(t.track_max);
      }
      for (const PosKey &k : t.pos) {
        if (timed)
          buf_.I32(k.start_time);
        vec(k.pos);
      }
    }
    end();
  }

  void unknown_chunk(const char id[4], const std::vector<uint8_t> &payload) {
    begin(id);
    buf_.Bytes(payload);
    end();
  }

  // The model file built so far.
  const std::vector<uint8_t> &bytes() const { return buf_; }

private:
  void vec(const Vec &v) { buf_.F32(v.x).F32(v.y).F32(v.z); }
  // The count includes a terminating NUL because the loader does not terminate the buffer itself.
  void str(const std::string &s) { buf_.I32(static_cast<int32_t>(s.size() + 1)).Text(s).U8(0); }

  void begin(const char id[4]) {
    buf_.Bytes(id, 4);
    len_pos_ = buf_.size();
    buf_.U32(0);
  }
  void end() { buf_.PatchU32(len_pos_, static_cast<uint32_t>(buf_.size() - len_pos_ - 4)); }

  int version_;
  d3_test::ByteWriter buf_;
  size_t len_pos_ = 0;
};

} // namespace pof
