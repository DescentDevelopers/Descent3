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

 * $Logfile: /DescentIII/Main/D3Launch/SpeedTab.cpp $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:56:51 $
 * $Author: kevinb $
 *
 * 
 *
 * $Log: not supported by cvs2svn $
 * 
 * 18    4/21/99 7:17p Nate
 * Added "-nolightmaps" switch for LOW_DETAIL setting
 * 
 * 17    4/08/99 1:13p Nate
 * Added Pentium III detection
 * 
 * 16    3/31/99 6:25p Nate
 * Fixed potential bug in CPU speed reporting code
 * 
 * 15    10/21/98 4:51p Nate
 * Changed default to Medium; user only needs to visit the Speed tab now
 * to get past the warning.
 * 
 * 14    10/19/98 3:16p Nate
 * Modified default detail level calculations
 * 
 * 13    10/15/98 7:30p Nate
 * 
 * 12    10/15/98 1:26p Nate
 * Changed registry value name for the detail level
 * 
 * 11    10/14/98 5:52p Nate
 * More fixes
 * 
 * 10    10/13/98 3:03p Nate
 * More fixes and changes.
 * 
 * 9     9/28/98 10:33a Nate
 * Added flag for when list box selection changes (if flag is set when ok
 * button is pressed, registry settings are changed to the selected
 * default settings)
 * 
 * 8     9/25/98 6:59p Nate
 * Added code to write out detail level registry values.
 * 
 * 7     9/22/98 3:33p Nate
 * Added conditional compiling to help system (can toggle between HTML and
 * standard windows help)
 * 
 * 6     9/22/98 10:44a Nate
 * Added prompt before changing default detail level.
 * 
 * 5     9/21/98 5:40p Nate
 * Incorporated the new HTML help system
 * 
 * 4     9/02/98 9:48a Nate
 * Fixed hardware accelerator display bug in the Speed Tab
 * 
 * 3     8/10/98 10:44a Nate
 * Added Language selection support
 * 
 * 2     8/05/98 11:54a Nate
 * Initial Version
 *
 * $NoKeywords: $
 */

// SpeedTab.cpp : implementation file
//

#include "stdafx.h"
#include "afxpriv.h"
#include "afxext.h"
#include "D3Launch.h"
#include "SpeedTab.h"
#include "PsTypes.h"
#include "VideoTab.h"
#include "3D_detect.h"
#include "OS_Config.h"

#include <mmsystem.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

typedef LONGLONG longlong;


/////////////////////////////////////////////////////////////////////////////
// CSpeedTab property page

IMPLEMENT_DYNCREATE(CSpeedTab, CPropertyPage)

CSpeedTab::CSpeedTab() : CPropertyPage(CSpeedTab::IDD)
{
	//{{AFX_DATA_INIT(CSpeedTab)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_WriteDetailValues=FALSE;
}

CSpeedTab::~CSpeedTab()
{
}

void CSpeedTab::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSpeedTab)
	DDX_Control(pDX, IDC_SPEED_LIST, m_speed_list);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSpeedTab, CPropertyPage)
	//{{AFX_MSG_MAP(CSpeedTab)
	ON_BN_CLICKED(IDC_BTN_SPEED_DETECT, OnBtnSpeedDetect)
	ON_MESSAGE(WM_COMMANDHELP,OnCommandHelp)
	ON_WM_HELPINFO()
	ON_CBN_SELCHANGE(IDC_SPEED_LIST, OnSelchangeSpeedList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSpeedTab message handlers

void CSpeedTab::OnOK() 
{
	// TODO: Add your specialized code here and/or call the base class
	int cur_speed = m_speed_list.GetCurSel();

	if ( (cur_speed < 0) || (cur_speed>CUSTOM_DETAIL))	{
		cur_speed = 0;
	}

	os_config_write_uint(szSectionName, "PredefDetailSetting", cur_speed );

	// If the user has switched to a new default detail setting,
	// then write out all the default detail levels to registry
	if(m_WriteDetailValues) {
		switch(cur_speed) {
			case 0:
				break;
			case 1:
				break;
			case 2:
				break;
			case 3:
				break;
		}
		m_WriteDetailValues=FALSE;
	}

	// As long as they have viewed this page, they can get past the
	// "set detail level" message when trying to play
	DetailLevelConfigured=TRUE;

	CPropertyPage::OnOK();
}

BOOL CSpeedTab::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here
	// 0=low, 1=medium, 2=high, 3=very high
	int cur_speed = os_config_read_uint(szSectionName, "PredefDetailSetting", MEDIUM_DETAIL );

	if ( cur_speed < 0 )
		cur_speed = 0;
	else if ( cur_speed > CUSTOM_DETAIL )
		cur_speed = VERY_HIGH_DETAIL;

	m_speed_list.ResetContent();

	CString speed_type;

	speed_type.LoadString(IDS_SPEEDTAB_LOW);
	m_speed_list.InsertString( LOW_DETAIL, speed_type );
	speed_type.LoadString(IDS_SPEEDTAB_MEDIUM);
	m_speed_list.InsertString( MEDIUM_DETAIL, speed_type );
	speed_type.LoadString(IDS_SPEEDTAB_HIGH);
	m_speed_list.InsertString( HIGH_DETAIL, speed_type );
	speed_type.LoadString(IDS_SPEEDTAB_VERY_HIGH);
	m_speed_list.InsertString( VERY_HIGH_DETAIL, speed_type );

	if(cur_speed==CUSTOM_DETAIL) {
		speed_type.LoadString(IDS_SPEEDTAB_CUSTOM);
		m_speed_list.InsertString( CUSTOM_DETAIL, speed_type );
	}

	m_speed_list.SetCurSel(cur_speed);
	
	m_WriteDetailValues=FALSE;

	// Blank out property entries so user can watch them fill up again
	CString tmp_str;
	char tmp[1024];
	HWND hwnd;

	tmp_str.LoadString(IDS_SPEEDTAB_NOT_DETECTED);
	sprintf(tmp,"%s",tmp_str.GetBuffer(0));
	GetDlgItem( IDC_CPU_TYPE, &hwnd );
	::SetWindowText( hwnd, tmp );
	GetDlgItem( IDC_CPU_SPEED, &hwnd );
	::SetWindowText( hwnd, tmp );
	GetDlgItem( IDC_TOTAL_RAM, &hwnd );
	::SetWindowText( hwnd, tmp );
	GetDlgItem( IDC_RAM_SPEED, &hwnd );
	::SetWindowText( hwnd, tmp );
	GetDlgItem( IDC_VRAM_SPEED, &hwnd );
	::SetWindowText( hwnd, tmp );
	GetDlgItem( IDC_3D_GRAPHICS_ACCELERATOR, &hwnd );
	::SetWindowText( hwnd, tmp );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

#define CPUID _asm _emit 0fh _asm _emit 0a2h
#define EMMS _asm _emit 0fh _asm _emit 077h
#define RTSC _asm _emit 0fh _asm _emit 031h

DWORD ReadClocks()
{
	DWORD count;

	_asm {
		RTSC
		mov count, eax
	}
	return count;
}
		


// -----------------------------------------------------------------------
// Returns cpu type.
void detect_cpu(int *cpu, int *mmx, int *time_stamp_counter)
{
	BOOL retval = TRUE;
	DWORD RegEDX;
	DWORD RegEAX;

	// Set defaults
	*cpu = 0;
	*mmx = 0;
	*time_stamp_counter = 0;

	
   // jmp is not allowed in try 
	
  _asm {

		// Check for prescence of 
		push	eax
		push	ebx
		push	ecx
		push	edx

		pushfd			// get extended flags
		pop	eax
		mov	ebx, eax		// save current flags
		xor	eax, 200000h	// toggle bit 21
		push	eax			// push new flags on stack
		popfd					// flags updated now in flags
		pushfd			// get extended flags
		pop	eax		// store extended flags in eax
		xor	eax, ebx	// if bit 21 r/w then eax <> 0
		je		no_cpuid		
		
      mov eax, 1      // setup CPUID to return features

      CPUID           // code bytes = 0fh,  0a2h

		mov RegEAX, eax	// family, etc returned in eax
      mov RegEDX, edx	// features returned in edx
		jmp	done_checking_cpuid


no_cpuid:
		mov RegEAX, 4<<8	// family, etc returned in eax
      mov RegEDX, 0		// features returned in edx

done_checking_cpuid:								
		pop	edx
		pop	ecx
		pop	ebx
		pop	eax
	}
	


	//RegEAX	.  Bits 11:8 is family
	*cpu = (RegEAX >>8) & 0xF;

	if ( *cpu < 5 )	{
		*cpu = 4;								// processor does not support CPUID
		*mmx = 0;
	}

   if (RegEDX & (1<<4) ) {              // bit 4 is set for RTSC technology
		*time_stamp_counter = 1;
	}
	
   if (RegEDX & 0x800000)               // bit 23 is set for MMX technology
   {

           __try { 
			  
				_asm {
					EMMS
				}
			  
			  }          // try executing an MMX instruction "emms"

           __except(EXCEPTION_EXECUTE_HANDLER) { retval = FALSE; }

   }  else	{
		*mmx = 0;			// processor supports CPUID but does not support MMX technology
		return;
	}


	if ( retval == 0 )	{
		// if retval == 0 here that means the processor has MMX technology but
		// floating-point emulation is on; so MMX technology is unavailable
		*mmx = 0;			// processor supports CPUID but does not support MMX technology
		return;
	}
	
	*mmx = 1;			// processor supports CPUID but does not support MMX technology
}

// --------------------------------------------------------------------------

void CSpeedTab::OnBtnSpeedDetect() 
{
	// TODO: Add your control notification handler code here
	char tmp[1024];
	CString tmp_str;
	HWND hwnd;

	// Set the wait cursor
	CWaitCursor wc;

	// Blank out property entries so user can watch them fill up again
	tmp_str.LoadString(IDS_SPEEDTAB_NOT_DETECTED);
	sprintf(tmp,"%s",tmp_str.GetBuffer(0));
	GetDlgItem( IDC_CPU_TYPE, &hwnd );
	::SetWindowText( hwnd, tmp );
	GetDlgItem( IDC_CPU_SPEED, &hwnd );
	::SetWindowText( hwnd, tmp );
	GetDlgItem( IDC_TOTAL_RAM, &hwnd );
	::SetWindowText( hwnd, tmp );
	GetDlgItem( IDC_RAM_SPEED, &hwnd );
	::SetWindowText( hwnd, tmp );
	GetDlgItem( IDC_VRAM_SPEED, &hwnd );
	::SetWindowText( hwnd, tmp );
	GetDlgItem( IDC_3D_GRAPHICS_ACCELERATOR, &hwnd );
	::SetWindowText( hwnd, tmp );


	int cpu, mmx, timestamp_available;
	detect_cpu( &cpu, &mmx, &timestamp_available );

	switch( cpu )	{
	case 3:
		sprintf( tmp, "i386" );
		break;
	case 4:
		sprintf( tmp, "i486" );
		break;
	case 5:
		sprintf( tmp, "Pentium " );
		if ( mmx )	{
			strcat( tmp, " w/ MMX" );
		}
		break;
	case 6:
		if (SupportsKatmai()) {
			sprintf( tmp, "Pentium III" );
		} else if ( mmx )	{
			sprintf( tmp, "Pentium II w/ MMX" );
		} else {
			sprintf( tmp, "Pentium Pro" );
		}
		break;
	default:
		sprintf( tmp, "i%d86, MMX:%s", cpu, (mmx?"Yes":"No") );
		break;
	}

	GetDlgItem( IDC_CPU_TYPE, &hwnd );
	::SetWindowText( hwnd, tmp );

	int NiceMhz = 0;


	if ( timestamp_available )	{
		DWORD t1;
		DWORD c1, c2;

		do {		
	
			t1 = timeGetTime() + 1000;

			c1 = ReadClocks();
		
			while( timeGetTime() < t1 )	{
			}

			c2 = ReadClocks();
		} while ( c2 < c1 );			// Retry if it rolled

		int Mhz = c2-c1;
	
		// Round to the nearest multiple of 16.66666666666667
		int factor = (Mhz+(16666667/2)) / 16666667;

		NiceMhz = (factor*16666667);
		
		NiceMhz /= 1000000;


	} 

	if ( NiceMhz < 1 )	{
		sprintf( tmp, "Unknown\n" );
	} else {
		sprintf( tmp, "%d MHz\n", NiceMhz );
	}
		
	GetDlgItem( IDC_CPU_SPEED, &hwnd );
	::SetWindowText( hwnd, tmp );


	MEMORYSTATUS ms;
	ms.dwLength = sizeof(MEMORYSTATUS);
	GlobalMemoryStatus(&ms);
	DWORD descent3_total_ram = (ms.dwTotalPhys+(4194304/2))/(4194304);
	descent3_total_ram *= 4;
	sprintf( tmp, "%d MB", descent3_total_ram );
	GetDlgItem( IDC_TOTAL_RAM, &hwnd );
	::SetWindowText( hwnd, tmp );


	// Test memory READ speed.
	int *array1, *array2;
	
	array1 = new int[1024*1024];
	array2 = new int[1024*1024];

	longlong ct1, ct2, freq;
	
	QueryPerformanceCounter( (LARGE_INTEGER *)&ct1 );

	int count=0;

	DWORD t1 = timeGetTime() + 1000;

	while( timeGetTime() < t1 )	{
		_asm {
				push  esi
				push	edi
				push	ecx
				mov	esi, array2
				mov	edi, array1
				mov	ecx, (1024*1024/16)
				rep	movsd
				pop	ecx
				pop	edi
				pop	esi
		}
		count++;
	}

	QueryPerformanceCounter( (LARGE_INTEGER *)&ct2 );
	QueryPerformanceFrequency( (LARGE_INTEGER *)&freq );

	delete(array1);
	delete(array2);

	longlong deltat = (ct2-ct1)/count;
	
	int speed = int(freq/deltat);

	// Round to nearest 10 MB/s
//	speed = (speed+5)/10;
//	speed = speed*10;
	
	sprintf( tmp, "%d MB/s", speed );
	GetDlgItem( IDC_RAM_SPEED, &hwnd );
	::SetWindowText( hwnd, tmp );


	HDC hScreenDC = ::GetDC(NULL);
	HDC hMemDC = CreateCompatibleDC(hScreenDC);

	int w, h;
	w = GetDeviceCaps(hScreenDC,HORZRES);
	h = GetDeviceCaps(hScreenDC,VERTRES);

	HBITMAP hBmp = CreateCompatibleBitmap( hScreenDC, w, h*4 );
	if (hBmp && hScreenDC && hMemDC )	{
		SelectObject( hMemDC, hBmp );

		BitBlt( hMemDC, 0, 0, w, h, hScreenDC, 0, 0, SRCCOPY );

		QueryPerformanceCounter( (LARGE_INTEGER *)&ct1 );
		BitBlt( hScreenDC, 0, 0, w, h, hMemDC, 0, 0, SRCCOPY );
		GdiFlush();
		QueryPerformanceCounter( (LARGE_INTEGER *)&ct2 );

		QueryPerformanceFrequency( (LARGE_INTEGER *)&freq );
		
		deltat = ct2-ct1;
		
		//longlong speed = (longlong(300)*freq)/(deltat*longlong(1024));

		int bpp = GetDeviceCaps(hScreenDC,BITSPIXEL);
		int bpp1 = (bpp+7)/8;

		longlong vram_speed = freq;
		vram_speed /= (longlong)deltat;
		vram_speed *= (longlong)(w*h*bpp1);
		vram_speed /= (longlong)(1024*1024);

		speed = int(vram_speed);

		sprintf( tmp, "%d MB/s\n", speed );
		GetDlgItem( IDC_VRAM_SPEED, &hwnd );
		::SetWindowText( hwnd, tmp );

		DeleteObject(hBmp);
	} else {
		speed = 0;
		sprintf( tmp, "%d MB/s\n", speed );
		GetDlgItem( IDC_VRAM_SPEED, &hwnd );
		::SetWindowText( hwnd, "unknown" );
	}

	::DeleteDC( hMemDC );
	::DeleteDC( hScreenDC );


	int current_card = 0;
	int graphics_accelerator = RENDERER_NONE;
	GetDlgItem( IDC_3D_GRAPHICS_ACCELERATOR, &hwnd );
	
	// If the video tab has been initialized, get card from there
	if ( CurrentVideoTab && Num_cards !=0 )	{
		current_card = CurrentVideoTab->GetCurrentCard();
		::SetWindowText( hwnd, GetFullName(&Cards[current_card]) );
		graphics_accelerator=Cards[current_card].renderer_type;
	}
	else { // otherwise, get it from the registry
		RendererType renderer_id = (RendererType)os_config_read_uint(NULL, "PreferredRenderer", RENDERER_NONE);
		char *card_name = os_config_read_string(szSectionName, "RenderingDeviceName", "" );
		card3d temp_card;
		temp_card.renderer_type=renderer_id;
		strcpy(temp_card.name,card_name);
		::SetWindowText( hwnd, GetFullName(&temp_card) );
		graphics_accelerator=temp_card.renderer_type;
	}
	
	// Calculate default detail level based on CPU speed, and then weight it on
	// System RAM and whether or not they are using Glide
	int recommended_detail_level;

	// calculate setting based roughly on CPU speed
	if ( NiceMhz <= 300 )	{
		recommended_detail_level = LOW_DETAIL;	
	} else if ( NiceMhz <= 400 )	{
		recommended_detail_level = MEDIUM_DETAIL;
	} else if ( NiceMhz <= 500 )	{
		recommended_detail_level = HIGH_DETAIL;
	} else {
		recommended_detail_level = VERY_HIGH_DETAIL;
	}

	// weight the setting if user has ample supply of RAM
	if ( descent3_total_ram >= 64 )
		recommended_detail_level++;

	// weight the setting if user has Glide selected as API
	if ( graphics_accelerator == RENDERER_GLIDE )
		recommended_detail_level++;

	// Make sure detail level is capped at the highest setting
	if ( recommended_detail_level > VERY_HIGH_DETAIL )
		recommended_detail_level = VERY_HIGH_DETAIL;

	// Ask the user if he/she wants detail level changed
	if(m_speed_list.GetCurSel()!=recommended_detail_level) {
		CString speed_prompt, speed_title;
		speed_prompt.LoadString(IDS_SPEEDTAB_SPEED_PROMPT);
		speed_title.LoadString(IDS_SPEEDTAB_SPEED_TITLE);
		if(MessageBox(speed_prompt,speed_title,MB_YESNO|MB_ICONQUESTION)==IDNO)
			return;
	}

	// Set the new detail level
	m_speed_list.SetCurSel(recommended_detail_level);
	DetailLevelConfigured=TRUE;
	m_WriteDetailValues=TRUE;
}

BOOL CSpeedTab::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	// TODO: Add your message handler code here and/or call default

#ifdef USE_HTML_HELP_SYSTEM
	CWaitCursor wc;
	help_launch(SPEEDTAB_HELP);
	return 1;
#else
	return CPropertyPage::OnHelpInfo(pHelpInfo);
#endif
}

// Display the html help file
afx_msg LRESULT CSpeedTab::OnCommandHelp(WPARAM wParam, LPARAM lParam)
{
#ifdef USE_HTML_HELP_SYSTEM
	help_launch(SPEEDTAB_HELP);
	return 1;
#else
	return CPropertyPage::OnCommandHelp(wParam,lParam);
#endif
}


void CSpeedTab::OnSelchangeSpeedList() 
{
	// TODO: Add your control notification handler code here
	m_WriteDetailValues=TRUE;
}
