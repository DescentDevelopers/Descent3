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

 * $Logfile: /DescentIII/main/DLLUiItems.h $
 * $Revision: 1.3 $
 * $Date: 2004/02/09 04:14:49 $
 * $Author: kevinb $
 *
 * UI helper classes for the DLLs, make sure whatever file you include this in has the following line
 * int DLLUIClass_CurrID = 0xE0;
 *
 * $Log: DLLUiItems.h,v $
 * Revision 1.3  2004/02/09 04:14:49  kevinb
 * Added newlines to all headers to reduce number of warnings printed
 *
 * Made some small changes to get everything compiling.
 *
 * All Ready to merge the 1.5 tree.
 *
 * Revision 1.2  2000/05/29 05:03:22  icculus
 * Moved class implementation to DLLUiItems.cpp, since statically
 * linking the network clients would result in duplicate definitions.
 *
 * Revision 1.1.1.1  2000/04/18 00:00:30  icculus
 * initial checkin
 *
 *
 * 3     10/08/98 4:23p Kevin
 * Changed code to comply with memory library usage. Always use mem_malloc
 * , mem_free and mem_strdup
 *
 * 2     8/07/98 12:39p Jeff
 * initial creation
 *
 * $NoKeywords: $
 */

extern int DLLUIClass_CurrID;

class CheckBoxItem {
public:
  CheckBoxItem(void *window, char *title, bool initial_state, int x, int y, int flags);
  ~CheckBoxItem();
  void SetState(bool checked);
  bool GetState(void);
  bool Update(int id);

private:
  char *m_Title;
  bool m_State;
  int m_x, m_y, m_flags;
  void *m_textitem_i_off;
  void *m_textitem_o_off;
  void *m_textitem_i_on;
  void *m_textitem_o_on;
  void *m_hotspot;
  void *m_hWnd;
  int m_ID;
};

#ifndef DLLUICLASS_H_
#define DLLUICLASS_H_

#define JEFF_RED GR_RGB(255, 40, 40)
#define JEFF_BLUE GR_RGB(40, 40, 255)
#define JEFF_GREEN GR_RGB(40, 255, 40)

#endif
