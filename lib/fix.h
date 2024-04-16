/*
 * $Logfile: /DescentIII/Main/Lib/fix.h $
 * $Revision: 4 $
 * $Date: 10/21/99 9:27p $
 * $Author: Jeff $
 *
 *  Header for fixed-point math functions
 *
 * $Log: /DescentIII/Main/Lib/fix.h $
 *
 * 4     10/21/99 9:27p Jeff
 * B.A. Macintosh code merge
 *
 * 3     9/09/97 3:53p Chris
 * Added a #define PI to fix.h
 *
 * 2     8/22/97 12:34p Jason
 * added FixCeil and FixFloor
 *
 * 1     6/23/97 9:25p Samir
 * added because source safe sucks
 *
 * 7     2/27/97 4:57p Samir
 * include fixwin32.h from win\fixwin32.h
 *
 * 6     2/27/97 4:52 PM Jeremy
 * moved inline asm fixmul/div functions into separate header files for
 * mac and pc.
 *
 * 5     2/12/97 5:27p Jason
 * implemented asin,acos,atan2
 *
 * 4     2/10/97 1:57p Jason
 * changed FloatToFix back to what it originally was
 *
 * 3     2/07/97 5:44p Matt
 * Moved fixed-point math funcs from vecmat to fix
 * Changed trig functions to use small table like D2
 *
 * $NoKeywords: $
 */

#ifndef _FIX_H
#define _FIX_H

#include "math.h"

// Disable the "possible loss of data" warning
#pragma warning(disable : 4244)

// Angles are unsigned shorts
typedef unsigned short angle;

// The basic fixed-point type
typedef long fix;

#define PI 3.141592654
#define PIOVER2 1.570796327 // DAJ

// Constants for converted between fix and float
#define FLOAT_SCALER 65536.0
#define FIX_SHIFT 16

// 1.0 in fixed-point
#define F1_0 (1 << FIX_SHIFT)

// Generate the data for the trig tables.  Must be called before trig functions
void InitMathTables();

// Returns the sine of the given angle.  Linearly interpolates between two entries in a 256-entry table
float FixSin(angle a);

// Returns the cosine of the given angle.  Linearly interpolates between two entries in a 256-entry table
float FixCos(angle a);

// Returns the sine of the given angle, but does no interpolation
float FixSinFast(angle a);

// Returns the cosine of the given angle, but does no interpolation
float FixCosFast(angle a);

#define Round(x) ((int)(x + 0.5))

fix FloatToFixFast(float num);

// Conversion macros
//??#define FloatToFix(num) Round((num) * FLOAT_SCALER)
#define FloatToFix(num) ((fix)((num)*FLOAT_SCALER))
#define IntToFix(num) ((num) << FIX_SHIFT)
#define ShortToFix(num) (((long)(num)) << FIX_SHIFT)
#define FixToFloat(num) (((float)(num)) / FLOAT_SCALER)
#define FixToInt(num) ((num) >> FIX_SHIFT)
#define FixToShort(num) ((short)((num) >> FIX_SHIFT))

// Fixed-point math functions in inline ASM form
#if defined(MACINTOSH)
#include "fixmac.h"
#elif defined(WIN32)
#include "win\fixwin32.h"
#endif

// use this instead of:
// for:  (int)floor(x+0.5f) use FloatRound(x)
//       (int)ceil(x-0.5f)  use FloatRound(x)
//       (int)floor(x-0.5f) use FloatRound(x-1.0f)
//       (int)floor(x)      use FloatRound(x-0.5f)
// for values in the range -2048 to 2048
int FloatRound(float x);

angle FixAtan2(float cos, float sin);
angle FixAsin(float v);
angle FixAcos(float v);

// Does a ceiling operation on a fixed number
fix FixCeil(fix num);

// Floors a fixed number
fix FixFloor(fix num);

#endif
