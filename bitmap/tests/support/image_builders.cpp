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

#include "image_builders.h"

#include <cstring>

d3_test::ByteWriter TgaHeader(uint8_t type, int w, int h, uint8_t pixsize, uint8_t descriptor, const char *image_id) {
  d3_test::ByteWriter out;
  out.U8(static_cast<uint8_t>(strlen(image_id))); // ID length
  out.U8(0);                                      // no colour map
  out.U8(type);
  out.Fill(5, 0); // colour map spec
  out.U16(0);     // x origin
  out.U16(0);     // y origin
  out.U16(w);
  out.U16(h);
  out.U8(pixsize);
  out.U8(descriptor);
  out.Text(image_id);
  return out;
}

void PutPixel(d3_test::ByteWriter &out, const Bgra &p, uint8_t pixsize) {
  out.U8(p.b).U8(p.g).U8(p.r);
  if (pixsize == 32)
    out.U8(p.a);
}

std::vector<uint8_t> RawTga(const std::vector<std::vector<Bgra>> &rows_top_first, uint8_t pixsize, bool top_down,
                            const char *image_id) {
  const int h = static_cast<int>(rows_top_first.size());
  const int w = static_cast<int>(rows_top_first[0].size());
  d3_test::ByteWriter out = TgaHeader(kTgaTrueColor, w, h, pixsize, top_down ? kTgaTopDown : kTgaBottomUp, image_id);
  for (int i = 0; i < h; i++) {
    const auto &row = rows_top_first[top_down ? i : h - 1 - i];
    for (const Bgra &p : row)
      PutPixel(out, p, pixsize);
  }
  return out.bytes();
}

std::vector<uint8_t> RleTga(const std::vector<std::vector<Bgra>> &rows_top_first, uint8_t pixsize) {
  const int h = static_cast<int>(rows_top_first.size());
  const int w = static_cast<int>(rows_top_first[0].size());
  d3_test::ByteWriter out = TgaHeader(kTgaTrueColorRle, w, h, pixsize, kTgaBottomUp);
  auto same = [](const Bgra &x, const Bgra &y) { return x.b == y.b && x.g == y.g && x.r == y.r && x.a == y.a; };
  for (int i = h - 1; i >= 0; i--) {
    const auto &row = rows_top_first[i];
    int x = 0;
    while (x < w) {
      int run = 1;
      while (x + run < w && run < 128 && same(row[x + run], row[x]))
        run++;
      if (run > 1) {
        out.U8(static_cast<uint8_t>(0x80 | (run - 1)));
        PutPixel(out, row[x], pixsize);
        x += run;
      } else {
        int raw = 1;
        while (x + raw < w && raw < 128 && !(x + raw + 1 < w && same(row[x + raw], row[x + raw + 1])))
          raw++;
        out.U8(static_cast<uint8_t>(raw - 1));
        for (int k = 0; k < raw; k++)
          PutPixel(out, row[x + k], pixsize);
        x += raw;
      }
    }
  }
  return out.bytes();
}

d3_test::ByteWriter PcxHeader(int w, int h, uint8_t planes, uint16_t bytes_per_line) {
  d3_test::ByteWriter out;
  out.U8(10).U8(5).U8(1).U8(8); // manufacturer, version 5, RLE, 8 bits per plane
  out.U16(0).U16(0).U16(w - 1).U16(h - 1);
  out.U16(72).U16(72);
  out.Fill(48, 0); // EGA palette
  out.U8(0);       // reserved
  out.U8(planes);
  out.U16(bytes_per_line);
  out.U16(1);
  out.Fill(128 - out.size(), 0);
  return out;
}

void PcxEncodeLine(d3_test::ByteWriter &out, const std::vector<uint8_t> &line) {
  size_t i = 0;
  while (i < line.size()) {
    size_t run = 1;
    while (i + run < line.size() && run < 63 && line[i + run] == line[i])
      run++;
    if (run > 1 || line[i] >= 0xC0)
      out.U8(static_cast<uint8_t>(0xC0 | run));
    out.U8(line[i]);
    i += run;
  }
}

std::vector<uint8_t> Pcx8(const std::vector<std::vector<uint8_t>> &rows, const std::vector<Rgb> &palette) {
  const int w = static_cast<int>(rows[0].size());
  const int h = static_cast<int>(rows.size());
  const uint16_t bytes_per_line = static_cast<uint16_t>((w + 1) & ~1); // must be even
  d3_test::ByteWriter out = PcxHeader(w, h, 1, bytes_per_line);
  for (auto row : rows) {
    row.resize(bytes_per_line, 0);
    PcxEncodeLine(out, row);
  }
  out.U8(0x0C); // palette marker
  for (int i = 0; i < 256; i++) {
    Rgb c = i < static_cast<int>(palette.size()) ? palette[i] : Rgb{0, 0, 0};
    out.U8(c.r).U8(c.g).U8(c.b);
  }
  return out.bytes();
}

std::vector<uint8_t> Pcx24(const std::vector<std::vector<Rgb>> &rows) {
  const int w = static_cast<int>(rows[0].size());
  const int h = static_cast<int>(rows.size());
  const uint16_t bytes_per_line = static_cast<uint16_t>((w + 1) & ~1);
  d3_test::ByteWriter out = PcxHeader(w, h, 3, bytes_per_line);
  for (const auto &row : rows) {
    std::vector<uint8_t> r(bytes_per_line, 0), g(bytes_per_line, 0), b(bytes_per_line, 0);
    for (int x = 0; x < w; x++) {
      r[x] = row[x].r;
      g[x] = row[x].g;
      b[x] = row[x].b;
    }
    PcxEncodeLine(out, r);
    PcxEncodeLine(out, g);
    PcxEncodeLine(out, b);
  }
  return out.bytes();
}

std::vector<uint8_t> IffPbm(int w, int h, uint8_t masking, uint16_t transparent, const std::vector<Rgb> &palette,
                            uint8_t compression, const std::vector<uint8_t> &body, const char *form_type) {
  d3_test::ByteWriter bmhd;
  bmhd.U16BE(w).U16BE(h).U16BE(0).U16BE(0);
  bmhd.U8(8).U8(masking).U8(compression).U8(0);
  bmhd.U16BE(transparent);
  bmhd.U8(5).U8(6);
  bmhd.U16BE(320).U16BE(200);

  d3_test::ByteWriter cmap;
  for (const Rgb &c : palette)
    cmap.U8(c.r).U8(c.g).U8(c.b);

  d3_test::ByteWriter chunks;
  chunks.Text(form_type);
  chunks.Text("BMHD").U32BE(static_cast<uint32_t>(bmhd.size())).Bytes(bmhd.bytes());
  chunks.Text("CMAP").U32BE(static_cast<uint32_t>(cmap.size())).Bytes(cmap.bytes());
  if (cmap.size() & 1)
    chunks.U8(0);
  chunks.Text("BODY").U32BE(static_cast<uint32_t>(body.size())).Bytes(body);
  if (body.size() & 1)
    chunks.U8(0);

  d3_test::ByteWriter form;
  form.Text("FORM").U32BE(static_cast<uint32_t>(chunks.size())).Bytes(chunks.bytes());
  return form.bytes();
}
