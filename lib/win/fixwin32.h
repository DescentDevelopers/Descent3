/*
 * $Logfile: /descent3/main/lib/win/fixwin32.h $
 * $Revision: 3 $
 * $Date: 2/27/97 4:57p $
 * $Author: Samir $
 *
 * $Log: /descent3/main/lib/win/fixwin32.h $
 * 
 * 3     2/27/97 4:57p Samir
 * disabled annoying no return value message for this header.
 * 
 * 2     2/27/97 4:49 PM Jeremy
 * fixed a typo
 * 
 * 1     2/27/97 4:45 PM Jeremy
 * inline assembly language functions for Intel processor for
 * fixmul/fixdiv/fixmuldiv
  *
 * $NoKeywords: $
 */

#ifndef _FIXWIN32_H
#define _FIXWIN32_H

//what does this do?  Why didn't Jason put a comment here?
// Jason replies: This pragma disables the "no return value" warning that
// is generated when converting doubles to floats
// A thousand pardons for the confusion

#pragma warning (disable:4035)

inline fix FixDiv (fix a,fix b)
{
	__asm 
	{
		mov eax,a
		mov ebx,b
		cdq
		shld edx, eax, 16
		sal eax,16
		idiv ebx
	}
}


inline fix FixMul (fix a,fix b)
{
	__asm
	{
		mov eax,a
		mov ebx,b	
	    imul ebx
        shrd eax,edx,16
	}
}

inline fix FixMulDiv (fix a,fix b,fix c)
{
	__asm 	
	{
		mov eax, a
		mov edx, b
		mov ebx, c
		imul edx
		idiv ebx
	}
}

#pragma warning (default:4035)

#endif
