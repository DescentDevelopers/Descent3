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

 * $Logfile: /DescentIII/Main/streamaudio/osfarchive.cpp $
 * $Revision: 7 $
 * $Date: 4/14/99 1:47a $
 * $Author: Jeff $
 *
 * OSF File Reader
 *
 * $Log: /DescentIII/Main/streamaudio/osfarchive.cpp $
 *
 * 7     4/14/99 1:47a Jeff
 * fixed case mismatched #includes
 *
 * 6     4/02/99 4:32p Samir
 * fixed problem with really long stream names screwing up the osf header.
 *
 * 5     4/01/99 4:50p Matt
 * Took out Warning() function, chaning those calls to mprintf()
 *
 * 4     3/09/99 6:36p Samir
 * try catch blocks.
 *
 * 3     10/21/98 8:13p Samir
 * fixed music utils problems.
 *
 * 2     7/24/98 5:18p Samir
 * moved OSF header to end of file.
 *
 * 1     7/20/98 2:54p Samir
 * initial revision.
 *
 * $NoKeywords: $
 */

#include "streamaudio.h"
#include "cfile.h"
#include "pserror.h"
#include <string.h>

OSFArchive::OSFArchive() {
  m_fp = NULL;
  m_length = 0;
  m_type = 0;
  m_comp = 0;
  m_flags = 0;
  m_rate = 0;
  m_writemode = false;
  m_name[0] = 0;
}

OSFArchive::~OSFArchive() {
  if (m_fp) {
    cfclose(m_fp);
    m_fp = NULL;
  }
}

bool OSFArchive::Open(const char *filename, bool write) {
  char tag[8];

  ASSERT(!m_fp);

  try {
    if (write)
      m_fp = cfopen(filename, "wb");
    else
      m_fp = cfopen(filename, "rb");

    if (!m_fp)
      return false;

    m_writemode = write;

    if (m_writemode)
      return true;

    //	okay, if we are in read mode, then we read in the header NOW
    memset(tag, 0, sizeof(tag));
    cfseek(m_fp, -OSF_HDR_SIZE, SEEK_END);
    cf_ReadBytes((ubyte *)tag, strlen(OSF_TAG), m_fp);
    if (strcmp(OSF_TAG, tag) != 0) {
      mprintf((0, "Illegal OSF file format for %s.\n", filename));
      cfclose(m_fp);
      m_fp = NULL;
      return false;
    }

    //	read in rest of header.
    m_type = (ubyte)cf_ReadByte(m_fp);
    m_comp = (ubyte)cf_ReadByte(m_fp);
    m_flags = (ubyte)cf_ReadByte(m_fp);
    m_rate = (uint)cf_ReadByte(m_fp);

    if (m_type == OSF_DIGITAL_STRM) {
      if (m_rate == 11)
        m_rate = 11025;
      else if (m_rate == 22)
        m_rate = 22050;
      else if (m_rate == 44)
        m_rate = 44100;

      //	read in aux header based off of type.
      m_hdr.digi.measure = (uint)cf_ReadInt(m_fp);
    } else {
      mprintf((0, "Unsupported OSF file type in %s!\n", filename));
      cfclose(m_fp);
      m_fp = NULL;
      return false;
    }

    //	read length.
    m_length = (uint)cf_ReadInt(m_fp);

    cfseek(m_fp, -OSF_HDR_TITLE_OFS, SEEK_END);
    if (!cf_ReadBytes((ubyte *)m_name, OSF_HDR_TITLE_LEN, m_fp)) {
      mprintf((0, "Stream title not found for %s.\n", filename));
      cfclose(m_fp);
      m_fp = NULL;
      return false;
    }
  } catch (...) {
    if (m_fp) {
      try {
        cfclose(m_fp);
      } catch (...) {
      }
      m_fp = NULL;
    }
    return false;
  }

  m_datastart = 0;

  OSFArchive::Rewind();

  return true;
}

void OSFArchive::Close() {
  if (m_fp) {
    try {
      cfclose(m_fp);
    } catch (...) {
    }
    m_fp = NULL;
  }

  m_writemode = false;
}

void OSFArchive::Rewind() {
  try {
    cfseek(m_fp, m_datastart, SEEK_SET);
  } catch (...) {
  }
}

//	write out operations.
bool OSFArchive::SaveHeader(ubyte type, ubyte comp, ubyte flags, uint rate, uint length, void *hdr, const char *name) {
  char chbuf[OSF_HDR_SIZE];
  int hdrstart, hdrcur;

  if (!m_fp || !m_writemode)
    return false;

  //	write out data.
  try {
    char osftitle[OSF_HDR_TITLE_LEN];
    memset((ubyte *)chbuf, 0, OSF_HDR_SIZE);
    cf_WriteBytes((ubyte *)chbuf, OSF_HDR_SIZE, m_fp);
    cfseek(m_fp, -OSF_HDR_SIZE, SEEK_CUR);

    hdrstart = cftell(m_fp);
    if (!cf_WriteBytes((ubyte *)OSF_TAG, strlen(OSF_TAG), m_fp))
      return false;

    cf_WriteByte(m_fp, (sbyte)type);
    cf_WriteByte(m_fp, (sbyte)comp);
    cf_WriteByte(m_fp, (sbyte)flags);

    if (type == OSF_DIGITAL_STRM) {
      tOSFDigiHdr *digihdr = (tOSFDigiHdr *)hdr;
      cf_WriteByte(m_fp, (rate == 11025) ? 11 : (rate == 22050) ? 22 : (rate == 44100) ? 44 : 0);
      cf_WriteInt(m_fp, (int)digihdr->measure);
    } else {
      return false;
    }

    cf_WriteInt(m_fp, (int)length);
    hdrcur = cftell(m_fp);

    if ((hdrcur - hdrstart) > OSF_HDR_TITLE_OFS) {
      mprintf((0, "stream header too big for title!\n"));
      return false;
    }

    cfseek(m_fp, OSF_HDR_TITLE_OFS - (hdrcur - hdrstart), SEEK_CUR);
    strncpy(osftitle, name, OSF_HDR_TITLE_LEN);
    osftitle[OSF_HDR_TITLE_LEN - 1] = 0;
    if (!cf_WriteBytes((ubyte *)name, OSF_HDR_TITLE_LEN, m_fp))
      return false;
  } catch (...) {
    return false;
  }

  return true;
}

bool OSFArchive::WriteBlock(ubyte *blk, int size) { return (cf_WriteBytes(blk, size, m_fp) < size) ? false : true; }

//	read in operations.
int OSFArchive::Read(ubyte *blk, int size) {
  int retval;
  try {
    retval = cf_ReadBytes(blk, size, m_fp);
  } catch (...) {
    retval = 0;
  }
  return retval;
}
