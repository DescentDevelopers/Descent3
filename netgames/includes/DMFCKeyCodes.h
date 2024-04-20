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

/*
 * $Logfile: /DescentIII/main/DMFCKeyCodes.h $
 * $Revision: 1.2 $
 * $Date: 2004/02/09 04:14:49 $
 * $Author: kevinb $
 *
 * Keycodes for use in DMFC
 *
 * $Log: DMFCKeyCodes.h,v $
 * Revision 1.2  2004/02/09 04:14:49  kevinb
 * Added newlines to all headers to reduce number of warnings printed
 *
 * Made some small changes to get everything compiling.
 *
 * All Ready to merge the 1.5 tree.
 *
 * Revision 1.1.1.1  2000/04/18 00:00:30  icculus
 * initial checkin
 *
 *
 * 2     9/21/98 7:11p Jeff
 * made InputCommand interface API and moved existing input commands to
 * the interface.  Changed mprintf/ASSERT so they are valid in DMFC
 *
 * $NoKeywords: $
 */

#ifndef __DMFC_KEYB_H_
#define __DMFC_KEYB_H_

//	codes
#define K_0 0x0B
#define K_1 0x02
#define K_2 0x03
#define K_3 0x04
#define K_4 0x05
#define K_5 0x06
#define K_6 0x07
#define K_7 0x08
#define K_8 0x09
#define K_9 0x0A
#define K_A 0x1E
#define K_B 0x30
#define K_C 0x2E
#define K_D 0x20
#define K_E 0x12
#define K_F 0x21
#define K_G 0x22
#define K_H 0x23
#define K_I 0x17
#define K_J 0x24
#define K_K 0x25
#define K_L 0x26
#define K_M 0x32
#define K_N 0x31
#define K_O 0x18
#define K_P 0x19
#define K_Q 0x10
#define K_R 0x13
#define K_S 0x1F
#define K_T 0x14
#define K_U 0x16
#define K_V 0x2F
#define K_W 0x11
#define K_X 0x2D
#define K_Y 0x15
#define K_Z 0x2C
#define K_MINUS 0x0C
#define K_EQUAL 0x0D
#define K_DIVIDE 0x35
#define K_SLASH 0x2B
#define K_COMMA 0x33
#define K_PERIOD 0x34
#define K_SEMICOL 0x27
#define K_LBRACKET 0x1A
#define K_RBRACKET 0x1B
#define K_RAPOSTRO 0x28
#define K_LAPOSTRO 0x29
#define K_ESC 0x01
#define K_ENTER 0x1C
#define K_BACKSP 0x0E
#define K_TAB 0x0F
#define K_SPACEBAR 0x39
#define K_NUMLOCK 0x45
#define K_SCROLLOCK 0x46
#define K_CAPSLOCK 0x3A
#define K_LSHIFT 0x2A
#define K_RSHIFT 0x36
#define K_LALT 0x38
#define K_RALT 0xB8
#define K_LCTRL 0x1D
#define K_RCTRL 0x9D

#define K_F1 0x3B
#define K_F2 0x3C
#define K_F3 0x3D
#define K_F4 0x3E
#define K_F5 0x3F
#define K_F6 0x40
#define K_F7 0x41
#define K_F8 0x42
#define K_F9 0x43
#define K_F10 0x44
#define K_F11 0x57
#define K_F12 0x58

#define K_PAD0 0x52
#define K_PAD1 0x4F
#define K_PAD2 0x50
#define K_PAD3 0x51
#define K_PAD4 0x4B
#define K_PAD5 0x4C
#define K_PAD6 0x4D
#define K_PAD7 0x47
#define K_PAD8 0x48
#define K_PAD9 0x49
#define K_PADMINUS 0x4A
#define K_PADPLUS 0x4E
#define K_PADPERIOD 0x53
#define K_PADDIVIDE 0xB5
#define K_PADMULTIPLY 0x37
#define K_PADENTER 0x9C

#define K_INSERT 0xD2
#define K_HOME 0xC7
#define K_PAGEUP 0xC9
#define K_DELETE 0xD3
#define K_END 0xCF
#define K_PAGEDOWN 0xD1
#define K_UP 0xC8
#define K_DOWN 0xD0
#define K_LEFT 0xCB
#define K_RIGHT 0xCD
#define K_PRINT_SCREEN 0xB7
#define K_PAUSE 0x61

#endif
