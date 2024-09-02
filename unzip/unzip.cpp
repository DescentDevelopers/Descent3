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

--- HISTORICAL COMMENTS FOLLOW ---

 * $Logfile: /DescentIII/Main/unzip/unzip.cpp $
 * $Revision: 3 $
 * $Date: 8/15/99 8:07p $
 * $Author: Jeff $
 *
 * Unzip class routines
 *
 * $Log: /DescentIII/Main/unzip/unzip.cpp $
 *
 * 3     8/15/99 8:07p Jeff
 * handle disk write errors when extracting
 *
 * 2     8/13/99 8:02p Jeff
 * initial creation of zip class
 *
 * $NoKeywords: $
 */

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <cctype>

#include <zlib.h>

#include "unzip.h"

#define INFLATE_INPUT_BUFFER_MAX 16384
#define DATA_CHUNK_SIZE 1024

// ECD Offsets
#define ECD_ESIG 0x00
#define ECD_EDSK 0x04
#define ECD_ECEN 0x06
#define ECD_ENUM 0x08
#define ECD_ECENN 0x0a
#define ECD_ECSZ 0x0c
#define ECD_EOFST 0x10
#define ECD_ECOML 0x14
#define ECD_ECOM 0x16

//	CD Entry Offsets
#define ZIPCD_CENSIG 0x0
#define ZIPCD_CVER 0x4
#define ZIPCD_COS 0x5
#define ZIPCD_CVXT 0x6
#define ZIPCD_CEXOS 0x7
#define ZIPCD_CFLG 0x8
#define ZIPCD_CMTHD 0xa
#define ZIPCD_CTIME 0xc
#define ZIPCD_CDATE 0xe
#define ZIPCD_CCRC 0x10
#define ZIPCD_CSIZ 0x14
#define ZIPCD_CUNC 0x18
#define ZIPCD_CFNL 0x1c
#define ZIPCD_CXTL 0x1e
#define ZIPCD_CCML 0x20
#define ZIPCD_DSK 0x22
#define ZIPCD_INT 0x24
#define ZIPCD_EXT 0x26
#define ZIPCD_OFST 0x2a
#define ZIPCD_CFN 0x2e

// Local File Header Offsets
#define LFH_LOCSIG 0x00
#define LFH_VER 0x04
#define LFH_GENFLG 0x06
#define LFH_MTHD 0x08
#define LFH_TIME 0x0a
#define LFH_DATE 0x0c
#define LFH_CRC 0x0e
#define LFH_SIZE 0x12
#define LFH_UNCMP 0x16
#define LFH_FNLN 0x1a
#define LFH_XTRALN 0x1c
#define LFH_NAME 0x1e

uint16_t get_buffer_short(char *buf) {
  uint8_t *ubuf = (uint8_t *)buf;
  return ((uint16_t)ubuf[1] << 8) | (uint16_t)ubuf[0];
}
uint32_t get_buffer_int(char *buf) {
  uint8_t *ubuf = (uint8_t *)buf;
  return ((uint32_t)ubuf[3] << 24) | ((uint32_t)ubuf[2] << 16) | ((uint32_t)ubuf[1] << 8) | (uint32_t)ubuf[0];
}

ZIP::ZIP() { m_open = false; }

ZIP::~ZIP() { CloseZip(); }

// Looks for the ECD in the buffer, if found it returns the offset
// returns true on success
bool ZIP::FindECDSignature(char *buffer, int buflen, int *offset) {
  static char sig[] = {'P', 'K', 0x05, 0x06};
  int i;

  for (i = buflen - 22; i >= 0; i--) {
    if (memcmp(buffer + i, sig, 4) == 0) {
      *offset = i;
      return true;
    }
  }
  return false;
}

int ZIP::ReadECD() {
  char *buf;
  int buf_length = 1024;

  while (true) {
    int offset;

    if (buf_length > m_length)
      buf_length = m_length;

    if (fseek(m_fp, m_length - buf_length, SEEK_SET) != 0) {
      return -1;
    }

    buf = (char *)malloc(buf_length);
    if (!buf) {
      return -1;
    }

    if (fread(buf, buf_length, 1, m_fp) != 1) {
      free(buf);
      return -1;
    }

    if (FindECDSignature(buf, buf_length, &offset)) {
      m_ecd_length = buf_length - offset;
      m_ecd = (char *)malloc(m_ecd_length);

      if (!m_ecd) {
        free(buf);
        return -1;
      }

      memcpy(m_ecd, buf + offset, m_ecd_length);
      free(buf);
      return 0;
    }

    free(buf);

    if (buf_length < m_length) {
      buf_length = 2 * buf_length;
    } else {
      return -1;
    }
  }

  return -1;
}

// opens a zip file for reading
// returns true on success
bool ZIP::OpenZip(const char *zipfile) {
  if (m_open)
    return false;

  // open the file for reading
  m_fp = fopen(zipfile, "rb");
  if (!m_fp) {
    return false;
  }

  // seek to the end and get the length of the zip file
  if (fseek(m_fp, 0, SEEK_END) != 0) {
    fclose(m_fp);
    return false;
  }

  m_length = ftell(m_fp);
  if (m_length < 0) {
    fclose(m_fp);
    return false;
  }
  if (m_length == 0) {
    fclose(m_fp);
    return false;
  }

  // read End of Central Directory (ECD) data
  if (ReadECD() != 0) {
    fclose(m_fp);
    return false;
  }

  m_end_of_cent_dir_sig = get_buffer_int(m_ecd + ECD_ESIG);
  m_number_of_this_disk = get_buffer_short(m_ecd + ECD_EDSK);
  m_number_of_disk_start_cent_dir = get_buffer_short(m_ecd + ECD_ECEN);
  m_total_entries_cent_dir_this_disk = get_buffer_short(m_ecd + ECD_ENUM);
  m_total_entries_cent_dir = get_buffer_short(m_ecd + ECD_ECENN);
  m_size_of_cent_dir = get_buffer_int(m_ecd + ECD_ECSZ);
  m_offset_to_start_of_cent_dir = get_buffer_int(m_ecd + ECD_EOFST);
  m_zipfile_comment_length = get_buffer_short(m_ecd + ECD_ECOML);
  m_zipfile_comment = m_ecd + ECD_ECOM;

  // make sure there is no disk spanning
  if ((m_number_of_this_disk != m_number_of_disk_start_cent_dir) ||
      (m_total_entries_cent_dir_this_disk != m_total_entries_cent_dir) || (m_total_entries_cent_dir < 1)) {
    free(m_ecd);
    fclose(m_fp);
    return false;
  }

  if (fseek(m_fp, m_offset_to_start_of_cent_dir, SEEK_SET) != 0) {
    free(m_ecd);
    fclose(m_fp);
    return false;
  }

  // begin reading for the begining of central directory
  m_cd = (char *)malloc(m_size_of_cent_dir);
  if (!m_cd) {
    free(m_ecd);
    fclose(m_fp);
    return false;
  }

  if (fread(m_cd, m_size_of_cent_dir, 1, m_fp) != 1) {
    free(m_cd);
    free(m_ecd);
    fclose(m_fp);
    return false;
  }

  // reset zip entry
  m_ent.name = nullptr;

  // rewind to the begining of central dir
  m_cd_pos = 0;

  m_zip = (char *)malloc(strlen(zipfile) + 1);
  if (!m_zip) {
    free(m_cd);
    free(m_ecd);
    fclose(m_fp);
    return false;
  }
  strcpy(m_zip, zipfile);
  m_open = true;

  return true;
}

// closes an open zip file
void ZIP::CloseZip() {
  if (!m_open)
    return;

  /* release all */
  if (m_ent.name)
    free(m_ent.name);
  if (m_cd)
    free(m_cd);
  if (m_ecd)
    free(m_ecd);
  if (m_fp)
    fclose(m_fp);
  if (m_zip)
    free(m_zip);
  m_open = false;
}

// Reads the current zip entry from the zip file (and moves
// to the next entry).  Returns NULL if there are no more entries
zipentry *ZIP::ReadNextZipEntry() {
  if (!m_open)
    return nullptr;

  // make sure we aren't at the end
  if (m_cd_pos >= m_size_of_cent_dir)
    return nullptr;

  m_ent.cent_file_header_sig = get_buffer_int(m_cd + m_cd_pos + ZIPCD_CENSIG);
  m_ent.version_made_by = *(m_cd + m_cd_pos + ZIPCD_CVER);
  m_ent.host_os = *(m_cd + m_cd_pos + ZIPCD_COS);
  m_ent.version_needed_to_extract = *(m_cd + m_cd_pos + ZIPCD_CVXT);
  m_ent.os_needed_to_extract = *(m_cd + m_cd_pos + ZIPCD_CEXOS);
  m_ent.general_purpose_bit_flag = get_buffer_short(m_cd + m_cd_pos + ZIPCD_CFLG);
  m_ent.compression_method = get_buffer_short(m_cd + m_cd_pos + ZIPCD_CMTHD);
  m_ent.last_mod_file_time = get_buffer_short(m_cd + m_cd_pos + ZIPCD_CTIME);
  m_ent.last_mod_file_date = get_buffer_short(m_cd + m_cd_pos + ZIPCD_CDATE);
  m_ent.crc32 = get_buffer_int(m_cd + m_cd_pos + ZIPCD_CCRC);
  m_ent.compressed_size = get_buffer_int(m_cd + m_cd_pos + ZIPCD_CSIZ);
  m_ent.uncompressed_size = get_buffer_int(m_cd + m_cd_pos + ZIPCD_CUNC);
  m_ent.filename_length = get_buffer_short(m_cd + m_cd_pos + ZIPCD_CFNL);
  m_ent.extra_field_length = get_buffer_short(m_cd + m_cd_pos + ZIPCD_CXTL);
  m_ent.file_comment_length = get_buffer_short(m_cd + m_cd_pos + ZIPCD_CCML);
  m_ent.disk_number_start = get_buffer_short(m_cd + m_cd_pos + ZIPCD_DSK);
  m_ent.internal_file_attrib = get_buffer_short(m_cd + m_cd_pos + ZIPCD_INT);
  m_ent.external_file_attrib = get_buffer_int(m_cd + m_cd_pos + ZIPCD_EXT);
  m_ent.offset_lcl_hdr_frm_frst_disk = get_buffer_int(m_cd + m_cd_pos + ZIPCD_OFST);

  // Make sure the filename isn't too long
  if (m_cd_pos + ZIPCD_CFN + m_ent.filename_length > m_size_of_cent_dir) {
    return nullptr;
  }

  if (m_ent.name)
    free(m_ent.name);
  m_ent.name = (char *)malloc(m_ent.filename_length + 1);
  if (!m_ent.name)
    return nullptr;
  memcpy(m_ent.name, m_cd + m_cd_pos + ZIPCD_CFN, m_ent.filename_length);
  m_ent.name[m_ent.filename_length] = '\0';

  // skip to next entry in central dir
  m_cd_pos += ZIPCD_CFN + m_ent.filename_length + m_ent.extra_field_length + m_ent.file_comment_length;

  return &m_ent;
}

// Resets a ZIP file to the first entry
void ZIP::Rewind() {
  if (!m_open)
    return;
  m_cd_pos = 0;
}

// Reads a file from the given zip entry into a buffer in memory
//	-1 : no ZIP file open
//	-2 : corrupt file
//	-3 : Version too new
//	-4 : OS not supported
//	-5 : No Disk Spanning
//	-6 : Error inflating
//	-7 : Compression Type Not Supported
int ZIP::ReadFile(zipentry *ent, char *data) {
  if (!m_open)
    return -1;

  if (ent->compression_method == 0x0000) {
    // this file is STORED, not compressed

    // make sure the sizes are equal, else the file is corrupt
    if (ent->compressed_size != ent->uncompressed_size) {
      return -2;
    }

    return ReadZipData(ent, data);
  } else if (ent->compression_method == 0x0008) {
    // compression type of DEFLATE

    if (ent->version_needed_to_extract > 0x14) {
      return -3;
    }

    if (ent->os_needed_to_extract != 0x00) {
      return -4;
    }

    if (ent->disk_number_start != m_number_of_this_disk) {
      return -5;
    }

    // seek to compressed data
    if (SeekToCompressedData(ent) != 0) {
      return -2;
    }

    // inflate the file
    if (InflateFile(m_fp, ent->compressed_size, (uint8_t *)data, ent->uncompressed_size)) {
      return -6;
    }

    return 0;
  }

  return -7;
}

//	Extracts a file from the given zip entry into another file
//	-1 : no ZIP file open
//	-2 : corrupt file
//	-3 : Version too new
//	-4 : OS not supported
//	-5 : No Disk Spanning
//	-6 : Error inflating
//	-7 : Compression Type Not Supported
//	-8 : Unable to open output
//	-9 : Error writing to file
int ZIP::ExtractFile(zipentry *ent, const char *filename) {
  if (!m_open)
    return -1;

  FILE *output;
  output = fopen(filename, "wb");
  if (!output)
    return -8;

  if (ent->compression_method == 0x0000) {
    // this file is STORED, not compressed

    // make sure the sizes are equal, else the file is corrupt
    if (ent->compressed_size != ent->uncompressed_size) {
      fclose(output);
      return -2;
    }

    int ret = ReadZipDataToFile(ent, output);
    fclose(output);

    return ret;
  } else if (ent->compression_method == 0x0008) {
    // compression type of DEFLATE

    if (ent->version_needed_to_extract > 0x14) {
      fclose(output);
      return -3;
    }

    if (ent->os_needed_to_extract != 0x00) {
      fclose(output);
      return -4;
    }

    if (ent->disk_number_start != m_number_of_this_disk) {
      fclose(output);
      return -5;
    }

    // seek to compressed data
    if (SeekToCompressedData(ent) != 0) {
      fclose(output);
      return -2;
    }

    // inflate the file
    int inf_ret;
    inf_ret = InflateFileToFile(m_fp, ent->compressed_size, output, ent->uncompressed_size);
    if (inf_ret != 0) {
      fclose(output);
      return (inf_ret == -2) ? -9 : -6;
    }

    fclose(output);
    return 0;
  }

  fclose(output);
  return -7;
}

int ZIP::ReadZipData(zipentry *ent, char *data) {
  int err = SeekToCompressedData(ent);
  if (err != 0)
    return -2;

  if (fread(data, ent->compressed_size, 1, m_fp) != 1) {
    return -2;
  }

  return 0;
}

int ZIP::ReadZipDataToFile(zipentry *ent, FILE *file) {
  int err = SeekToCompressedData(ent);
  if (err != 0)
    return -2;

  uint8_t data[DATA_CHUNK_SIZE];
  int size_remaining, amount;
  size_remaining = ent->compressed_size;

  while (size_remaining > 0) {
    amount = std::min(size_remaining, DATA_CHUNK_SIZE);

    if (fread(data, amount, 1, m_fp) != 1) {
      return -2;
    }
    if (fwrite(data, amount, 1, file) == 0) {
      // error writing
      return -9;
    }
    size_remaining -= amount;
  }

  return 0;
}

int ZIP::SeekToCompressedData(zipentry *ent) {
  char buf[LFH_NAME];
  int32_t offset;

  if (fseek(m_fp, ent->offset_lcl_hdr_frm_frst_disk, SEEK_SET) != 0) {
    return -1;
  }

  if (fread(buf, LFH_NAME, 1, m_fp) != 1) {
    return -1;
  }

  uint16_t filename_length = get_buffer_short(buf + LFH_FNLN);
  uint16_t extra_field_length = get_buffer_short(buf + LFH_XTRALN);

  offset = ent->offset_lcl_hdr_frm_frst_disk + LFH_NAME + filename_length + extra_field_length;

  if (fseek(m_fp, offset, SEEK_SET) != 0) {
    return -1;
  }

  return 0;
}

// uses zlib to inflate the given data
int ZIP::InflateFile(FILE *in_file, unsigned in_size, uint8_t *out_data, unsigned out_size) {
  int err;
  uint8_t *in_buffer;
  z_stream d_stream;

  d_stream.zalloc = nullptr;
  d_stream.zfree = nullptr;
  d_stream.opaque = nullptr;

  d_stream.next_in = nullptr;
  d_stream.avail_in = 0;
  d_stream.next_out = out_data;
  d_stream.avail_out = out_size;

  err = inflateInit2(&d_stream, -MAX_WBITS);
  /* windowBits is passed < 0 to tell that there is no zlib header.
   * Note that in this case inflate *requires* an extra "dummy" byte
   * after the compressed stream in order to complete decompression and
   * return Z_STREAM_END.
   */
  if (err != Z_OK) {
    return -1;
  }

  in_buffer = (uint8_t *)malloc(INFLATE_INPUT_BUFFER_MAX + 1);
  if (!in_buffer)
    return -1;

  for (;;) {
    if (in_size <= 0) {
      free(in_buffer);
      return -1;
    }
    d_stream.next_in = in_buffer;
    d_stream.avail_in = fread(in_buffer, 1, std::min<uInt>(in_size, INFLATE_INPUT_BUFFER_MAX), in_file);
    in_size -= d_stream.avail_in;
    if (in_size == 0)
      d_stream.avail_in++; // add dummy byte at end of compressed data

    err = inflate(&d_stream, Z_NO_FLUSH);
    if (err == Z_STREAM_END)
      break;
    if (err != Z_OK) {
      free(in_buffer);
      return -1;
    }
  }

  err = inflateEnd(&d_stream);
  if (err != Z_OK) {
    free(in_buffer);
    return -1;
  }

  free(in_buffer);

  if ((d_stream.avail_out > 0) || (in_size > 0)) {
    return -1;
  }

  return 0;
}

// uses zlib to inflate the given data
int ZIP::InflateFileToFile(FILE *in_file, unsigned in_size, FILE *file, unsigned out_size) {
  uint8_t out_data[DATA_CHUNK_SIZE];
  int err;
  uint32_t last_out;
  uint8_t *in_buffer;
  uint8_t *next_out;
  z_stream d_stream;

  d_stream.zalloc = nullptr;
  d_stream.zfree = nullptr;
  d_stream.opaque = nullptr;

  d_stream.next_in = nullptr;
  d_stream.avail_in = 0;
  d_stream.next_out = nullptr;
  d_stream.avail_out = 0;
  d_stream.total_out = 0;
  d_stream.total_in = 0;

  err = inflateInit2(&d_stream, -MAX_WBITS);
  /* windowBits is passed < 0 to tell that there is no zlib header.
   * Note that in this case inflate *requires* an extra "dummy" byte
   * after the compressed stream in order to complete decompression and
   * return Z_STREAM_END.
   */
  if (err != Z_OK) {
    return -1;
  }

  in_buffer = (uint8_t *)malloc(DATA_CHUNK_SIZE + 1);
  if (!in_buffer)
    return -1;

  for (;;) {
    // reset the output buffer
    last_out = d_stream.total_out;
    next_out = d_stream.next_out;

    if (in_size <= 0 && out_size <= 0) {
      free(in_buffer);
      return -1;
    }

    if (d_stream.avail_out == 0) {
      d_stream.next_out = out_data;
      d_stream.avail_out = std::min<uInt>(out_size, DATA_CHUNK_SIZE);
      out_size -= d_stream.avail_out;
      next_out = d_stream.next_out;
    }

    if (d_stream.avail_in == 0 && in_size > 0) {
      d_stream.next_in = in_buffer;
      d_stream.avail_in = fread(in_buffer, 1, std::min<uInt>(in_size, DATA_CHUNK_SIZE), in_file);
      in_size -= d_stream.avail_in;
      if (in_size == 0)
        d_stream.avail_in++; // add dummy byte at end of compressed data
    }

    err = inflate(&d_stream, Z_NO_FLUSH);
    if (err == Z_STREAM_END) {
      if (d_stream.total_out > last_out) {
        int res, to_write = (d_stream.total_out - last_out);
        res = fwrite(next_out, to_write, 1, file);
        if (res == 0) {
          // error writing to file!
          free(in_buffer);
          return -2;
        }
        break;
      }
    }
    if (err != Z_OK) {
      free(in_buffer);
      return -1;
    }
    if (d_stream.total_out > last_out) {
      int res, to_write = (d_stream.total_out - last_out);
      res = fwrite(next_out, to_write, 1, file);
      if (res == 0) {
        // error writing to file!
        free(in_buffer);
        return -2;
      }
    }
  }

  err = inflateEnd(&d_stream);
  if (err != Z_OK) {
    free(in_buffer);
    return -1;
  }

  free(in_buffer);

  if ((d_stream.avail_out > 0) || (in_size > 0)) {
    return -1;
  }

  return 0;
}

// Compare two filename without using directory
int CompareZipFileName(const char *zipfile, const char *file) {
  const char *s1 = file;
  const char *s2 = strrchr(zipfile, '/');
  if (s2)
    ++s2;
  else
    s2 = zipfile;
  while (*s1 && toupper(*s1) == toupper(*s2)) {
    ++s1;
    ++s2;
  }
  return !*s1 && !*s2;
}
