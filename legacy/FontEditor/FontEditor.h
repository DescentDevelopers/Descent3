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

 * $Source: /home/kevin/cvsstuff/descent3/descent3/Main/FontEditor/FontEditor.h,v $
 * $Revision: 1.1.1.1 $
 * $Author: kevinb $
 * $Date: 2003-08-26 03:57:45 $
 *
 * Font Editor
 *
 * $Log: not supported by cvs2svn $
 * 
 * 2     5/19/99 8:48p Samir
 * added kerning dump utilitiy.
 * 
 * 1     4/17/99 4:04p Samir
 * standard header.
 * 
 */

extern void FontDoIO();
extern void FontKern(const char *fnt_file_name);
extern void FontKernDump(const char *fnt_file_name);
extern void FontView(const char *fnt_file_name);
extern void FontCreate(const char *fnt_file_source, const char *fnt_file_dest, int min_ascii);

extern int System_font_handle;

