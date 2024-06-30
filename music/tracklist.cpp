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

 * $Source: $
 * $Revision: 4 $
 * $Author: Jeff $
 * $Date: 3/15/99 4:32p $
 *
 *
 *
 * $Log: /DescentIII/Main/music/tracklist.cpp $
 *
 * 4     3/15/99 4:32p Jeff
 * fixed code so mem library compiles correctly
 *
 * 3     12/11/98 4:03p Samir
 * error checking.
 *
 * 2     11/13/98 2:27p Samir
 * created.
 *
 */

#include "music.h"
#include "mem.h"
#include "pserror.h"

#include <string.h>

//////////////////////////////////////////////////////////////////////////////

oms_tracklist::oms_tracklist() {
  m_fnamelist = NULL;
  m_symlist = NULL;
  m_numtracks = 0;
  m_maxtracks = 0;
  m_init = false;
}

oms_tracklist::~oms_tracklist() { oms_tracklist::free(); }

// initializes track list system
void oms_tracklist::init(int16_t maxtracks) {
  if (!m_init) {
    m_maxtracks = maxtracks;
    m_numtracks = 0;

    m_fnamelist = new char *[m_maxtracks];
    m_symlist = new char *[m_maxtracks];
    m_init = true;
  }
}

// frees track list system
void oms_tracklist::free() {
  if (m_init) {
    oms_tracklist::reset();
    delete[] m_symlist;
    delete[] m_fnamelist;

    m_init = false;
  }
}

// resets track list to 0
void oms_tracklist::reset() {
  int i;

  if (!m_init)
    return;

  for (i = 0; i < m_numtracks; i++) {
    ::mem_free(m_fnamelist[i]);
    ::mem_free(m_symlist[i]);
  }

  m_numtracks = 0;
}

// adds a track to list.
bool oms_tracklist::add(const char *fname, const char *sym) {
  if (!m_init)
    return false;

  if (m_numtracks == m_maxtracks) {
    Int3();
    return false;
  }

  // add symbol and track name
  m_fnamelist[m_numtracks] = mem_strdup(fname);
  m_symlist[m_numtracks] = mem_strdup(sym);
  m_numtracks++;

  return true;
}

// returns a track filename.
const char *oms_tracklist::get(const char *sym) {
  int i;

  if (sym) {
    for (i = 0; i < m_numtracks; i++) {
      if (strcmp(m_symlist[i], sym) == 0)
        return m_fnamelist[i];
    }
  }

  return NULL;
}
