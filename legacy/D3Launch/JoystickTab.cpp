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

 * $Logfile: /DescentIII/Main/D3Launch/JoystickTab.cpp $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:56:51 $
 * $Author: kevinb $
 *
 * 
 *
 * $Log: not supported by cvs2svn $
 * 
 * 16    5/20/99 12:53p Nate
 * Added GLSetup window, EAX mixer support, and CHFlight+Mouseman Mode
 * options
 * 
 * 15    2/22/99 2:46p Nate
 * Fixed problem with long joystick names/descriptions
 * 
 * 14    11/30/98 3:42p Nate
 * Disabled Force Feedback checkbox for demo
 * 
 * 13    10/12/98 7:13p Nate
 * Fixed several bugs.
 * 
 * 12    10/10/98 4:05p Nate
 * Added detection/scanning window messages
 * 
 * 11    10/08/98 6:23p Nate
 * Fixed a few bugs.
 * 
 * 10    9/22/98 3:33p Nate
 * Added conditional compiling to help system (can toggle between HTML and
 * standard windows help)
 * 
 * 9     9/21/98 5:40p Nate
 * Incorporated the new HTML help system
 * 
 * 8     9/15/98 6:02p Nate
 * Fixed double detection NT bug.
 * 
 * 7     9/15/98 6:02p Nate
 * Added Force Feedback detection.
 * 
 * 6     9/02/98 6:42p Nate
 * Added improved sound support.
 * 
 * 5     9/01/98 7:15p Nate
 * Major Revision #2
 * 
 * 4     8/31/98 6:44p Nate
 * Major Revision
 * 
 * 3     8/10/98 10:44a Nate
 * Added Language selection support
 * 
 * 2     8/05/98 11:54a Nate
 * Initial Version
 *
 * $NoKeywords: $
 */

// JoystickTab.cpp : implementation file
//

#include "stdafx.h"
#include "afxpriv.h"
#include "afxext.h"
#include <mmsystem.h>
#include <regstr.h>
#include "D3Launch.h"
#include "JoystickTab.h"
#include "dinput.h"
#include "OS_Config.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Declare the joystick data
JoystickData joystick_data[MAX_NUM_JOYSTICKS];
int Num_joysticks=0;

/////////////////////////////////////////////////////////////////////////////
// CJoystickTab property page

IMPLEMENT_DYNCREATE(CJoystickTab, CPropertyPage)

CJoystickTab::CJoystickTab() : CPropertyPage(CJoystickTab::IDD)
{
	//{{AFX_DATA_INIT(CJoystickTab)
	//}}AFX_DATA_INIT
}

CJoystickTab::~CJoystickTab()
{
}

void CJoystickTab::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CJoystickTab)
	DDX_Control(pDX, IDC_CHFLIGHT_CHECK, m_CHFlightEnabled);
	DDX_Control(pDX, IDC_MOUSEMAN_CHECK, m_MousemanEnabled);
	DDX_Control(pDX, IDC_JOYSTICK_LIST, m_joystick_list);
	DDX_Control(pDX, IDC_FF_ENABLED, m_ff_enabled);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CJoystickTab, CPropertyPage)
	//{{AFX_MSG_MAP(CJoystickTab)
	ON_BN_CLICKED(IDC_BTN_CALIBRATE, OnBtnCalibrate)
	ON_BN_CLICKED(IDC_BTN_DETECT, OnBtnDetect)
	ON_CBN_SELCHANGE(IDC_JOYSTICK_LIST, OnSelchangeJoystickList)
	ON_MESSAGE(WM_COMMANDHELP,OnCommandHelp)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CJoystickTab message handlers

int GetJoystickName(int joy_num,char *szRegKey,char *szReturnName);
int di_detect_joysticks(int cur_joy, CComboBox *box);


void CJoystickTab::OnBtnCalibrate() 
{
	// TODO: Add your control notification handler code here
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	
	memset( &si, 0, sizeof(STARTUPINFO) );
	si.cb = sizeof(si);
	
	BOOL ret = CreateProcess(	NULL,	// pointer to name of executable module 
								"rundll32.exe shell32.dll,Control_RunDLL joy.cpl",				// pointer to command line string
								NULL,							// pointer to process security attributes 
								NULL,							// pointer to thread security attributes 
								FALSE,							// handle inheritance flag 
								CREATE_DEFAULT_ERROR_MODE,		// creation flags 
								NULL,							// pointer to new environment block 
								NULL,	// pointer to current directory name 
								&si,	// pointer to STARTUPINFO 
								&pi 	// pointer to PROCESS_INFORMATION  
							);			

	if ( !ret )	{
		char *lpMsgBuf;

		FormatMessage(
			 FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			 NULL,
			 GetLastError(),
			 MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			 (LPTSTR) &lpMsgBuf,
			 0,
			 NULL
		);

		// Display the string.
		CString err_msg;
		err_msg.LoadString(IDS_JOYSTICKTAB_ERR1);
		MessageBox( lpMsgBuf, err_msg, MB_OK | MB_ICONEXCLAMATION);

		// Free the buffer.
		LocalFree( lpMsgBuf );
 	}	
}


// Detect all the joysticks
void CJoystickTab::DetectSticks()
{
	char str[1024];
	int j, max, cur_joy;
	JOYCAPS JoyCaps;

	Num_joysticks=0;
	//m_joystick_list.ResetContent();

	// Add "none" option
	CString none_msg;
	none_msg.LoadString(IDS_JOYSTICKTAB_NONE);
	strcpy(joystick_data[Num_joysticks].name,none_msg.GetBuffer(0));
	joystick_data[Num_joysticks].FF_capable=FALSE;
	joystick_data[Num_joysticks].ID=JOYSTICK_NONE_ID;
	Num_joysticks++;

	//index = m_joystick_list.InsertString(-1, none_msg);

	//m_joystick_list.SetItemData(index, 99999);  // link joystick id to list item
	//m_joystick_list.SetCurSel(0);

	//cur_joy = os_config_read_uint(szSectionName, "CurrentJoystick", 0);
	cur_joy=0;
	if (di_detect_joysticks(cur_joy, &m_joystick_list)==0) {
		max = joyGetNumDevs();
		for (j=JOYSTICKID1; j<JOYSTICKID1+max; j++) {
			if (JOYERR_NOERROR == joyGetDevCaps(j, &JoyCaps, sizeof(JoyCaps))) {
				char szReturnName[1024];

				if ( !GetJoystickName(j,JoyCaps.szRegKey,szReturnName) )	{
					strcpy( str, szReturnName );
				} else {
					sprintf(str, "Joystick %d", j + 1);
				}

				strcpy(joystick_data[Num_joysticks].name,str);
				joystick_data[Num_joysticks].FF_capable=FALSE;
				joystick_data[Num_joysticks].ID=j;
				Num_joysticks++;


				//index = m_joystick_list.InsertString(-1, str);
				//m_joystick_list.SetItemData(index, j);  // link joystick id to list item
				//if (j == cur_joy)
				//	m_joystick_list.SetCurSel(index);
			}
		}
	}
	
/*
	int ff_enabled = os_config_read_uint(szSectionName, "EnableJoystickFF", 0);
	if (ff_enabled) {
		m_ff_enabled.SetCheck(1);

	} else {
		m_ff_enabled.SetCheck(0);
	}
*/

}

// Detect the sticks and fill the list box
BOOL CJoystickTab::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here
	// TODO: Add your control notification handler code here
	//int cur_stick, i;
	int selected_stick;
	HWND hwnd;

	// Set the wait cursor
	CWaitCursor wc;

	// Display the scanning window
	m_MsgDlg.m_ScanningMsg.LoadString(IDS_JOYSTICKTAB_SCANNING);
	m_MsgDlg.m_WaitMsg.LoadString(IDS_PLEASE_WAIT_MSG);
	m_MsgDlg.Create(IDD_MSG_DLG,this);
	m_MsgDlg.ShowWindow(SW_SHOW);
	m_MsgDlg.UpdateData(FALSE);

	// Detect all available joysticks
	DetectSticks();

	// Remove the scanning window
	m_MsgDlg.DestroyWindow();
	
	// Take the first joystick (if one exists)
	if(Num_joysticks>1)
		selected_stick=1;  // First joystick in list
	else
		selected_stick=0;  // None
/*
	// Clear the list
	m_joystick_list.ResetContent();

	// Read in the selected joystick
	cur_stick = os_config_read_uint(szSectionName, "CurrentJoystick", JOYSTICK_NONE_ID);

	// Fill the list with the detected joysticks
	selected_stick=Num_joysticks-1;
	for (i=0; i<Num_joysticks; i++) {
		m_joystick_list.InsertString(i, joystick_data[i].name);
		if(joystick_data[i].ID==cur_stick) selected_stick=i;
	}

	// Set the joystick selection 
	m_joystick_list.SetCurSel(selected_stick);
*/

	// Display the primary joystick name in the text box
	GetDlgItem(IDC_PRIMARY_JOYSTICK_TEXT)->SetWindowText(joystick_data[selected_stick].name);
	UpdateData(FALSE);

	// Set the FF checkbox is user wants to use FF
	int ff_enabled = os_config_read_uint(szSectionName, "EnableJoystickFF", 1);
	if (ff_enabled && joystick_data[selected_stick].FF_capable) {
		m_ff_enabled.SetCheck(1);
	} else {
		m_ff_enabled.SetCheck(0);
	}
	UpdateData(FALSE);

	// Enable or disable the FF checkbox
	GetDlgItem(IDC_FF_ENABLED,&hwnd);
#ifdef DEMO
	::EnableWindow(hwnd, FALSE);
#else
	::EnableWindow(hwnd, joystick_data[selected_stick].FF_capable);
#endif

	// Set the CHFlight checkbox is user wants to use that mode
	if(os_config_read_uint(szSectionName, "EnableCHFlight", 0)) {
		m_CHFlightEnabled.SetCheck(1);
	} else {
		m_CHFlightEnabled.SetCheck(0);
	}
	UpdateData(FALSE);

	// Set the Mouseman checkbox is user wants to use that mode
	if(os_config_read_uint(szSectionName, "EnableMouseman", 0)) {
		m_MousemanEnabled.SetCheck(1);
	} else {
		m_MousemanEnabled.SetCheck(0);
	}
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


// JAS: Taken from Nov 1996 Game Developer, page 49.
// Damn, that is some UGLY code!  (But it works...)
/*---------------------------------------------------------------------*/    
int GetJoystickName(int joy_num,char *szRegKey,char *szReturnName)
/* 
   Description :  	Opens the MediaResources\Joysitick\mjstick.drv<xxxx>\JoystickSettings and 
					extracts Joystick%dOEMName string
   Arguments :     	joy_num    (r/o) - Joystick Number
					szRegKey   (r/o) - Registry Key of the msjstick.drv
					ReturnName (r/w) - Return String for name listed in Control Panel                                               
   Returns :		0 for success 1 for failure
/*-----------------------------------------------------------------------*/ 
{
   BYTE KeyStr[_MAX_PATH] = REGSTR_PATH_JOYCONFIG;       	// found in regstr.h
   BYTE KeyJoySetStr[_MAX_PATH] = REGSTR_KEY_JOYSETTINGS; 	// found in Regstr.h
   BYTE szOEMName[_MAX_PATH];                             	// OEM name from Current Settings
   HKEY ConfigKey;
   HKEY JoyConfigKey;                                    	// Joystick Configuration
   HKEY DriverKey;                                       	// Joystick Driver Key
   HKEY OEMPropKey;
   HKEY PropKey;
   DWORD Length;
   if( ERROR_SUCCESS != RegOpenKey( HKEY_LOCAL_MACHINE,REGSTR_PATH_JOYCONFIG,&ConfigKey ) )

      {
      return( 1 );                	// It should never get here key received from Caps
      }                              
  
   if( ERROR_SUCCESS != RegOpenKey( ConfigKey, szRegKey, &DriverKey ) )
      {
      return( 1 );       	// It should never get here key received from Caps
      }                              
                                                	// Open CurrentSettings Key
   
   if( ERROR_SUCCESS != RegOpenKey( DriverKey, REGSTR_KEY_JOYCURR, &JoyConfigKey ) )
      {
      return( 1 );                  	// It should never get here always a Current Settings
      }
   sprintf((char *)KeyStr,(char *)REGSTR_VAL_JOYNOEMNAME,joy_num+1);   
   Length=sizeof(szOEMName);                        	// Get OEMNAME Configuration
   
   if( ERROR_SUCCESS != RegQueryValueEx( JoyConfigKey,(char *)KeyStr,NULL,NULL,(uint8_t *)&szOEMName,&Length))
      {
        return( 1 );                                	// No OEM name listed return error
      }
   RegCloseKey( ConfigKey );      	// Closes the registry Key
                                                    	
	// Open OEM Properties Key
    if( ERROR_SUCCESS != RegOpenKey(HKEY_LOCAL_MACHINE,REGSTR_PATH_JOYOEM,&PropKey ) )
      {
      return( 1 );                  	// It should never get here key received from Caps
      }                              
   
   if( ERROR_SUCCESS != RegOpenKey( PropKey, (char *)szOEMName, &OEMPropKey ) )
      {
      return( 1 );                  	// It should never get here if device is selected
      }
   Length=_MAX_PATH;                        	// Get Name as listed in Control Panel
   
   if( ERROR_SUCCESS != RegQueryValueEx( OEMPropKey,REGSTR_VAL_JOYOEMNAME,NULL,NULL,(uint8_t *)szReturnName,&Length))
      {
        return( 1 );                              	 // No OEM name listed return error
      }
   RegCloseKey( OEMPropKey );         	// Closes the registry Key
   return 0;

} /* End GetJoystickName */


void CJoystickTab::OnOK() 
{
	// TODO: Add your specialized code here and/or call the base class
	//int index; 
	int cur_joy = 0;

	if ( m_ff_enabled.GetCheck() ) {
		os_config_write_uint( szSectionName, "EnableJoystickFF", 1 );
	} else {
		os_config_write_uint( szSectionName, "EnableJoystickFF", 0 );
	}

	if ( m_CHFlightEnabled.GetCheck() ) {
		os_config_write_uint( szSectionName, "EnableCHFlight", 1 );
	} else {
		os_config_write_uint( szSectionName, "EnableCHFlight", 0 );
	}

	if ( m_MousemanEnabled.GetCheck() ) {
		os_config_write_uint( szSectionName, "EnableMouseman", 1 );
	} else {
		os_config_write_uint( szSectionName, "EnableMouseman", 0 );
	}

	/*
	// save the joystick selected to be used in Descent 3
	index = m_joystick_list.GetCurSel();
	if (index >= 0)
		cur_joy = joystick_data[index].ID;

	os_config_write_uint(szSectionName, "CurrentJoystick", cur_joy);
	*/
	OutputDebugString("Setting the joystick!\n");

	CPropertyPage::OnOK();
}


// Pointers to functions contained in DSOUND.dll
int Di_dll_loaded = 0;
HINSTANCE Di_dll_handle = NULL;
HRESULT (__stdcall *pfn_DirectInputCreate) (HINSTANCE, DWORD, LPDIRECTINPUT *, LPUNKNOWN) = NULL;

// obtain the function pointers from the dsound.dll
void di_dll_get_functions()
{
	pfn_DirectInputCreate = (HRESULT (__stdcall *) (HINSTANCE, DWORD, LPDIRECTINPUT *, LPUNKNOWN)) GetProcAddress(Di_dll_handle, "DirectInputCreateA");
}

// Load the dsound.dll, and get funtion pointers
// exit:	0	->	dll loaded successfully
//			!0	->	dll could not be loaded
int di_dll_load()
{
	if (!Di_dll_loaded) {
		Di_dll_handle = LoadLibrary("dinput.dll");
		if (!Di_dll_handle)
			return -1;

		di_dll_get_functions();
		Di_dll_loaded = 1;
	}

	return 0;
}

void di_dll_free()
{
	if (Di_dll_loaded) {
		FreeLibrary(Di_dll_handle);
		Di_dll_handle = NULL;
		Di_dll_loaded = 0;
	}
}

struct stuff {
	CComboBox *box;
	int cur_joy;
	int count;
};

// Create storage for any temporary debug messages
char msg_buffer[1024];

// Callback function for enumerating ALL joysticks
BOOL CALLBACK enum_joysticks(LPCDIDEVICEINSTANCE lpddi, LPVOID ptr)
{
	int num;
	stuff *stuffp = (stuff *) ptr;

	OutputDebugString("Found joystick in enum!\n");
	sprintf(msg_buffer,"%s (%s)\n", lpddi->tszProductName, lpddi->tszInstanceName);
	OutputDebugString(msg_buffer);

	// Make sure it is a joystick
	if (strnicmp(lpddi->tszInstanceName, "Joystick ", 9))
		return DIENUM_CONTINUE;

	// Get the joystick number
	num = atoi(lpddi->tszInstanceName + 9) - 1;
	sprintf(msg_buffer,"num=%d\n",num);
	OutputDebugString(msg_buffer);

	// Fill in the data
	sprintf(joystick_data[Num_joysticks].name, "%s (%s)", lpddi->tszProductName, lpddi->tszInstanceName);
	joystick_data[Num_joysticks].FF_capable=FALSE;
	joystick_data[Num_joysticks].ID=num;
/*
	i = stuffp->box->InsertString(-1, joystick_data[Num_joysticks].name);
	stuffp->box->SetItemData(i, num);
	if (num == stuffp->cur_joy)
		stuffp->box->SetCurSel(i);
*/

	Num_joysticks++;
	stuffp->count++;

	return DIENUM_CONTINUE;
}

// Callback function for enumerating ONLY force feedback joysticks
BOOL CALLBACK enum_ff_joysticks(LPCDIDEVICEINSTANCE lpddi, LPVOID ptr)
{
	int i, num;
	OutputDebugString("Found force feedback joystick in enum!\n");

	sprintf(msg_buffer, "%s (%s)\n", lpddi->tszProductName, lpddi->tszInstanceName);
	OutputDebugString(msg_buffer);

	// Make sure it is a joystick
	if (strnicmp(lpddi->tszInstanceName, "Joystick ", 9))
		return DIENUM_CONTINUE;

	// Get the joystick number
	num = atoi(lpddi->tszInstanceName + 9) - 1;

	// Set the ff_capable flag of this joystick
	for(i=0;i<Num_joysticks;i++)
		if(joystick_data[i].ID==num)
			joystick_data[i].FF_capable=TRUE;

/*
	strcpy(buf, lpddi->tszInstanceName);
	if (strnicmp(buf, "Joystick ", 9))
		return DIENUM_CONTINUE;

	// Get the joystick number
	num = atoi(buf + 9) - 1;

	// Get the name
	sprintf(buf, "%s (%s)", lpddi->tszProductName, lpddi->tszInstanceName);

	// Fill in the data
	strcpy(joystick_data[Num_joysticks].name,buf);
	joystick_data[Num_joysticks].FF_capable=FALSE;
	joystick_data[Num_joysticks].ID=num;
	Num_joysticks++;

	i = stuffp->box->InsertString(-1, buf);
	stuffp->box->SetItemData(i, num);
	if (num == stuffp->cur_joy)
		stuffp->box->SetCurSel(i);

	stuffp->count++;
*/
	return DIENUM_CONTINUE;
}

int di_detect_joysticks(int cur_joy, CComboBox *box)
{
	HRESULT hr;
	LPDIRECTINPUT di_joystick_obj;
	stuff s;

	if (di_dll_load())
		return 0;

	if (!pfn_DirectInputCreate) {
		di_dll_free();
		return 0;
	}

	hr = pfn_DirectInputCreate(GetModuleHandle(NULL), DIRECTINPUT_VERSION, &di_joystick_obj, NULL);
	if (FAILED(hr)) {
		hr = pfn_DirectInputCreate(GetModuleHandle(NULL), 0x300, &di_joystick_obj, NULL);
		if (FAILED(hr)) {
			di_dll_free();
			return 0;
		}
	}

	s.count = 0;
	s.box = box;
	s.cur_joy = cur_joy;

	//hr = di_joystick_obj->EnumDevices(DIDEVTYPE_JOYSTICK, enum_joysticks, &s, DIEDFL_ALLDEVICES);
	hr = di_joystick_obj->EnumDevices(DIDEVTYPE_JOYSTICK, enum_joysticks, &s, DIEDFL_ATTACHEDONLY);
	if(hr==DIERR_INVALIDPARAM) {
		OutputDebugString("Bad parm\n");
	}
	if (FAILED(hr)) {
		di_joystick_obj->Release();
		di_dll_free();
		return 0;
	}

	// Enumerate any force feedback devices if at least one joystick was found
	if(s.count>0) {
		hr = di_joystick_obj->EnumDevices(DIDEVTYPE_JOYSTICK, enum_ff_joysticks, &s, DIEDFL_FORCEFEEDBACK | DIEDFL_ATTACHEDONLY);
		if(hr==DIERR_INVALIDPARAM) {
			OutputDebugString("Bad parm\n");
		}
		if (FAILED(hr)) {
			di_joystick_obj->Release();
			di_dll_free();
			return s.count; // return the number found in first enum
		}
	}

	di_joystick_obj->Release();

	di_dll_free();

	return s.count;
}


void CJoystickTab::OnBtnDetect() 
{
	// TODO: Add your control notification handler code here
	//int cur_stick, i;
	int selected_stick;
	HWND hwnd;

	// Set the wait cursor
	CWaitCursor wc;

	// Display the scanning window
	m_MsgDlg.m_ScanningMsg.LoadString(IDS_JOYSTICKTAB_SCANNING);
	m_MsgDlg.m_WaitMsg.LoadString(IDS_PLEASE_WAIT_MSG);
	m_MsgDlg.Create(IDD_MSG_DLG,this);
	m_MsgDlg.ShowWindow(SW_SHOW);
	m_MsgDlg.UpdateData(FALSE);

	// Detect all available joysticks
	DetectSticks();	

	// Remove the scanning window
	m_MsgDlg.DestroyWindow();

	// Take the first joystick (if one exists)
	if(Num_joysticks>1)
		selected_stick=1;  // First joystick in list
	else
		selected_stick=0;  // None

/*
	// Clear the list
	m_joystick_list.ResetContent();

	// Read in the selected joystick
	cur_stick = Num_joysticks-1;

	// Fill the list with the detected joysticks
	selected_stick=Num_joysticks-1;
	for (i=0; i<Num_joysticks; i++) {
		m_joystick_list.InsertString(i, joystick_data[i].name);
		if(joystick_data[i].ID==cur_stick) selected_stick=i;
	}

	// Set the joystick selection 
	m_joystick_list.SetCurSel(selected_stick);
*/

	// Display the primary joystick name in the text box
	GetDlgItem(IDC_PRIMARY_JOYSTICK_TEXT)->SetWindowText(joystick_data[selected_stick].name);
	UpdateData(FALSE);

	// Set the FF checkbox is user wants to use FF
	int ff_enabled = os_config_read_uint(szSectionName, "EnableJoystickFF", 1);
	if (ff_enabled && joystick_data[selected_stick].FF_capable) {
		m_ff_enabled.SetCheck(1);

	} else {
		m_ff_enabled.SetCheck(0);
	}

	// Enable or disable the FF checkbox
	GetDlgItem(IDC_FF_ENABLED,&hwnd);
#ifdef DEMO
	::EnableWindow(hwnd, FALSE);
#else
	::EnableWindow(hwnd, joystick_data[selected_stick].FF_capable);
#endif
	UpdateData(FALSE);

}


void CJoystickTab::OnSelchangeJoystickList() 
{
	// TODO: Add your control notification handler code here
	int index;
	HWND hwnd;

	// Get the new joystick list position
	index=m_joystick_list.GetCurSel();

	// Enable or disable the FF checkbox
	if(index>=0 && index<Num_joysticks) {
		m_ff_enabled.SetCheck(joystick_data[index].FF_capable);
		GetDlgItem(IDC_FF_ENABLED,&hwnd);
#ifdef DEMO
		::EnableWindow(hwnd, FALSE);
#else
		::EnableWindow(hwnd, joystick_data[index].FF_capable);
#endif
		UpdateData(FALSE);
	}
}

BOOL CJoystickTab::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	// TODO: Add your message handler code here and/or call default
#ifdef USE_HTML_HELP_SYSTEM
	CWaitCursor wc;
	help_launch(JOYSTICKTAB_HELP);
	return 1;	
#else
	return CPropertyPage::OnHelpInfo(pHelpInfo);
#endif
}

// Display the html help file
afx_msg LRESULT CJoystickTab::OnCommandHelp(WPARAM wParam, LPARAM lParam)
{
#ifdef USE_HTML_HELP_SYSTEM
	help_launch(JOYSTICKTAB_HELP);
	return 1;
#else
	return CPropertyPage::OnCommandHelp(wParam,lParam);
#endif
}
