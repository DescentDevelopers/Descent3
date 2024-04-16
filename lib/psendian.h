/*
* $Logfile: /DescentIII/Main/lib/psendian.h $
* $Revision: 1 $
* $Date: 5/05/99 5:26a $
* $Author: Jeff $
*
* Big/Little Endian detection and functions
*
* $Log: /DescentIII/Main/lib/psendian.h $
 * 
 * 1     5/05/99 5:26a Jeff
 * 
 * 3     5/01/99 8:47p Jeff
 * removed 2 useless functions, use externC for linux compile
 * 
 * 2     5/01/99 2:52p Jeff
 * added automatic endian detection of the system
*
* $NoKeywords: $
*/


#ifndef __ENDIAN_H___
#define __ENDIAN_H___

extern "C"
{
// Endian_IsLittleEndian
//
//	Returns true if the machine is Little Endian (i.e. 80x86)
//	Returns false if the machine is Big Endian (i.e. Macintosh)
bool Endian_IsLittleEndian(void);

// Swaps (if needed) a short value (2 bytes) (assumes incoming value is in little endian format)
short Endian_SwapShort(short value);

// Swaps (if needed) an int value (4 bytes) (assumes incoming value is in little endian format)
int Endian_SwapInt(int value);

// Swaps (if needed) a float value (4 bytes) (assumes incoming value is in little endian format)
float Endian_SwapFloat(float value);
}
#endif