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

// BriefButtonEdit.cpp : implementation file
//

#include "stdafx.h"
#include "editor.h"
#include "BriefButtonEdit.h"
#include "CFile.h"
#include "BriefMissionFlagsDlg.h"
#include "briefedit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBriefButtonEdit dialog


CBriefButtonEdit::CBriefButtonEdit(TCBUTTONDESC* d,int id,CWnd* pParent /*=NULL*/)
	: CDialog(CBriefButtonEdit::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBriefButtonEdit)
	m_sFilename = _T("");
	m_sFlashFilename = _T("");
	m_bFlasher = FALSE;
	m_fFlashStart = 0.0f;
	m_bGlow = FALSE;
	m_iID = 0;
	m_iPID = 0;
	m_iSID = 0;
	m_iClickType = -1;
	m_iButtonType = -1;
	m_sDesc = _T("");
	m_sFilenameFocus = _T("");
	m_sFlashFilenameFocus = _T("");
	m_iX = 0;
	m_iY = 0;
	m_ScreenToJump = 0;
	//}}AFX_DATA_INIT

	m_iID = id;

	memset(&desc,0,sizeof(TCBUTTONDESC));

	desc.mission_mask_set = 0;
	desc.mission_mask_unset = 0;

	if(d){
		//initialize to what was passed in
		strcpy(desc.filename,d->filename);
		strcpy(desc.filename_focus,d->filename_focus);
		if(d->flasher){
			desc.flasher = true;
			desc.flash_time = d->flash_time;
			strcpy(desc.flash_filename,d->flash_filename);
			strcpy(desc.flash_filename_focus,d->flash_filename_focus);
		}
		desc.sibling_id = d->sibling_id;
		desc.parent_id = d->parent_id;
		desc.x = d->x; desc.y = d->y;
		desc.w = d->w; desc.h = d->h;
		desc.osflags = d->osflags;
		desc.button_type = d->button_type;
		desc.click_type = d->click_type;
		desc.mission_mask_set = d->mission_mask_set;
		desc.mission_mask_unset = d->mission_mask_unset;
		desc.jump_page = d->jump_page;

	}else{
		//set all defaults
		desc.button_type = BUTT_NEXTPAGE;
		desc.click_type = CLICKTYPE_CLICKDOWN;
	}

	m_ScreenToJump = 0;
	switch(desc.button_type){
	case BUTT_UPARROW:
		m_iButtonType = 0;
		break;
	case BUTT_DOWNARROW:
		m_iButtonType = 1;
		break;
	case BUTT_NEXTPAGE:
		m_iButtonType = 2;
		break;
	case BUTT_PREVPAGE:
		m_iButtonType = 3;
		break;
	case BUTT_QUIT:
		m_iButtonType = 4;
		break;
	case BUTT_JUMP:
		m_iButtonType = 5;
		m_ScreenToJump = desc.jump_page;
		break;
	default:
		m_iButtonType = 0;
		break;
	}

	switch(desc.click_type){
	case CLICKTYPE_CLICKUP:
		m_iClickType = 0;
		break;
	case CLICKTYPE_CLICKDOWN:
		m_iClickType = 1;
		break;
	case CLICKTYPE_DOWN:
		m_iClickType = 2;
		break;
	default:
		m_iClickType = 0;
		break;
	}

	m_sFilename = desc.filename;
	m_sFilenameFocus = desc.filename_focus;
	if(desc.flasher){
		m_sFlashFilename = desc.flash_filename;		
		m_sFlashFilenameFocus = desc.flash_filename_focus;
	}
	if(desc.osflags&OBF_GLOW)
		m_bGlow = true;
	else
		m_bGlow = false;
	if(desc.osflags&OBF_FLASH){
		m_bFlasher = true;
		m_fFlashStart = desc.flash_time;
	}else{
		m_bFlasher = false;
		m_fFlashStart = 0;
	}

	m_iPID = desc.parent_id;
	m_iSID = desc.sibling_id;
	m_iX = desc.x;
	m_iY = desc.y;
}


void CBriefButtonEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBriefButtonEdit)
	DDX_Text(pDX, IDC_BRIEF_BT_FILENAME, m_sFilename);
	DDX_Text(pDX, IDC_BRIEF_BT_FLASH_FILENAME, m_sFlashFilename);
	DDX_Check(pDX, IDC_BRIEF_BT_FLASHER, m_bFlasher);
	DDX_Text(pDX, IDC_BRIEF_BT_FLASHSTART, m_fFlashStart);
	DDV_MinMaxFloat(pDX, m_fFlashStart, 0.f, 99999.f);
	DDX_Check(pDX, IDC_BRIEF_BT_GLOW, m_bGlow);
	DDX_Text(pDX, IDC_BRIEF_BT_ID, m_iID);
	DDX_Text(pDX, IDC_BRIEF_BT_PID, m_iPID);
	DDX_Text(pDX, IDC_BRIEF_BT_SID, m_iSID);
	DDX_Radio(pDX, IDC_BRIEF_BT_MOUSEUP, m_iClickType);
	DDX_Radio(pDX, IDC_BRIEF_BT_SCROLLUP, m_iButtonType);
	DDX_Text(pDX, IDC_BRIEF_BT_DESC, m_sDesc);
	DDX_Text(pDX, IDC_BRIEF_BT_FILENAME_FOCUS, m_sFilenameFocus);
	DDX_Text(pDX, IDC_BRIEF_BT_FLASH_FILENAME_FOCUS, m_sFlashFilenameFocus);
	DDX_Text(pDX, IDC_BRIEF_BT_X, m_iX);
	DDX_Text(pDX, IDC_BRIEF_BT_Y, m_iY);
	DDX_Text(pDX, IDC_BRIEF_BT_JUMP_SCREEN, m_ScreenToJump);
	DDV_MinMaxInt(pDX, m_ScreenToJump, 0, 64);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBriefButtonEdit, CDialog)
	//{{AFX_MSG_MAP(CBriefButtonEdit)
	ON_BN_CLICKED(IDC_BRIEF_BT_CHOOSE, OnBriefBtChoose)
	ON_BN_CLICKED(IDC_BRIEF_BT_FLASH_CHOOSE, OnBriefBtFlashChoose)
	ON_BN_CLICKED(IDC_BRIEF_BT_FLASHER, OnBriefBtFlasher)
	ON_BN_CLICKED(IDC_BRIEF_BT_GLOW, OnBriefBtGlow)
	ON_BN_CLICKED(IDC_BRIEF_BT_CHOOSE_FOCUS, OnBriefBtChooseFocus)
	ON_BN_CLICKED(IDC_BRIEF_BT_FLASH_CHOOSE_FOCUS, OnBriefBtFlashChooseFocus)
	ON_BN_CLICKED(IDC_MISSIONFLAGS, OnMissionflags)
	ON_BN_CLICKED(IDC_BRIEF_BT_JUMP, OnBriefBtJump)
	ON_BN_CLICKED(IDC_BRIEF_BT_NEXTSCREEN, OnBriefBtNextscreen)
	ON_BN_CLICKED(IDC_BRIEF_BT_PREVSCREEN, OnBriefBtPrevscreen)
	ON_BN_CLICKED(IDC_BRIEF_BT_QUIT, OnBriefBtQuit)
	ON_BN_CLICKED(IDC_BRIEF_BT_SCROLLDOWN, OnBriefBtScrolldown)
	ON_BN_CLICKED(IDC_BRIEF_BT_SCROLLUP, OnBriefBtScrollup)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBriefButtonEdit message handlers

void CBriefButtonEdit::OnBriefBtChoose() 
{
	UpdateData(true);
	char szFilter[] = "Outrage Graphic Files (*.ogf)|*.ogf|32bit Uncompressed Targa (*.tga)|*.tga|PCX (*.pcx)|*.pcx|All Files (*.*)|*.*||";
	CFileDialog dlg(true,NULL,NULL,OFN_HIDEREADONLY|OFN_FILEMUSTEXIST,szFilter);

	if(dlg.DoModal()==IDOK){
		//handle
		m_sFilename = dlg.GetFileName();
		UpdateData(false);		
	}	
}

void CBriefButtonEdit::OnBriefBtFlashChoose() 
{
	UpdateData(true);
	char szFilter[] = "Outrage Graphic Files (*.ogf)|*.ogf|32bit Uncompressed Targa (*.tga)|*.tga|PCX (*.pcx)|*.pcx|All Files (*.*)|*.*||";
	CFileDialog dlg(true,NULL,NULL,OFN_HIDEREADONLY|OFN_FILEMUSTEXIST,szFilter);

	if(dlg.DoModal()==IDOK){
		//handle
		m_sFlashFilename = dlg.GetFileName();
		UpdateData(false);		
	}
}

void CBriefButtonEdit::OnOK() 
{
	if(!VerifyData())
		return;

	desc.osflags = 0;
	if(m_bFlasher || m_bGlow){
		desc.flasher = true;
		if(m_bFlasher)
			desc.osflags |= OBF_FLASH;
		if(m_bGlow)
			desc.osflags |= OBF_GLOW;
		
	}else{
		desc.flasher = false;
	}

	//If we got here then fill in all the data
	if(desc.flasher){
		//we need to get flash information
		strcpy(desc.flash_filename,m_sFlashFilename.GetBuffer(0));
		strcpy(desc.flash_filename_focus,m_sFlashFilenameFocus.GetBuffer(0));
		if(desc.osflags&OBF_FLASH){
			//get flash start time
			desc.flash_time = m_fFlashStart;
		}
	}else{
		desc.flash_filename[0] = '\0';
		desc.flash_filename_focus[0] = '\0';
	}

	desc.parent_id = m_iPID;
	desc.sibling_id = m_iSID;
	desc.x = m_iX;
	desc.y = m_iY;
	desc.jump_page = m_ScreenToJump;

	switch(m_iButtonType){
	case 0:	desc.button_type = BUTT_UPARROW;	break;
	case 1: desc.button_type = BUTT_DOWNARROW;	break;
	case 2: desc.button_type = BUTT_NEXTPAGE;	break;
	case 3: desc.button_type = BUTT_PREVPAGE;	break;
	case 4: desc.button_type = BUTT_QUIT;		break;
	case 5:	desc.button_type = BUTT_JUMP;		break;
	}

	switch(m_iClickType){
	case 0:	desc.click_type = CLICKTYPE_CLICKUP;	break;
	case 1: desc.click_type = CLICKTYPE_CLICKDOWN;	break;
	case 2: desc.click_type = CLICKTYPE_DOWN;		break;
	}

	strcpy(desc.filename,m_sFilename.GetBuffer(0));
	strcpy(desc.filename_focus,m_sFilenameFocus.GetBuffer(0));

	CDialog::OnOK();
}

BOOL CBriefButtonEdit::OnInitDialog() 
{
	CDialog::OnInitDialog();

	UpdateData(false);

	UpdateDialog();	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CBriefButtonEdit::UpdateDialog(void)
{
	CEdit *edit;
	CButton *but;
	edit = (CEdit *)GetDlgItem(IDC_BRIEF_BT_FLASH_FILENAME);
	but = (CButton *)GetDlgItem(IDC_BRIEF_BT_FLASH_CHOOSE);

	if( (desc.osflags&OBF_FLASH) || (desc.osflags&OBF_GLOW) ){
		//Enable the flash filename window		
		edit->EnableWindow(true);
		but->EnableWindow(true);

		edit = (CEdit *)GetDlgItem(IDC_BRIEF_BT_FLASHSTART);
		if(desc.osflags&OBF_FLASH)		
			edit->EnableWindow(true);
		else
			edit->EnableWindow(false);
		
		edit = (CEdit *)GetDlgItem(IDC_BRIEF_BT_FLASH_FILENAME_FOCUS);
		edit->EnableWindow(true);
	}else{
		edit->EnableWindow(false);
		but->EnableWindow(false);
		edit = (CEdit *)GetDlgItem(IDC_BRIEF_BT_FLASHSTART);
		edit->EnableWindow(false);
		edit = (CEdit *)GetDlgItem(IDC_BRIEF_BT_FLASH_FILENAME_FOCUS);
		edit->EnableWindow(false);
	}

	OnBriefBtJump();
}

void CBriefButtonEdit::OnBriefBtFlasher() 
{
	UpdateData(true);
	if(m_bFlasher)
		desc.osflags |= OBF_FLASH;
	else
		desc.osflags &= ~OBF_FLASH;
	if(m_bFlasher||m_bGlow)
		desc.flasher = true;
	else
		desc.flasher = false;
	UpdateDialog();
}

void CBriefButtonEdit::OnBriefBtGlow() 
{
	UpdateData(true);
	if(m_bGlow)
		desc.osflags |= OBF_GLOW;
	else
		desc.osflags &= ~OBF_GLOW;
	if(m_bFlasher||m_bGlow)
		desc.flasher = true;
	else
		desc.flasher = false;
	UpdateDialog();
}

extern int Briefing_root_screen;
extern tBriefScreen Briefing_screens[MAX_TELCOM_SCREENS];

bool CBriefButtonEdit::VerifyData(void)
{
	UpdateData(true);
	bool ret = true;

	if(desc.flasher){
		//flash filename should be specified		
		if(!cfexist(m_sFlashFilename.GetBuffer(0))){
			//file doesn't exist
			MessageBox("Flash Filename Does Not Exist","Error");
			ret = false;
		}
		//flash focus filename should be specified		
		if(!cfexist(m_sFlashFilenameFocus.GetBuffer(0))){
			//file doesn't exist
			MessageBox("Flash Focus Filename Does Not Exist","Error");
			ret = false;
		}
	}

	//filename should be specified		
	if(!cfexist(m_sFilename.GetBuffer(0))){
		//file doesn't exist
		MessageBox("Filename Does Not Exist","Error");
		ret = false;
	}

	//focus filename should be specified		
	if(!cfexist(m_sFilenameFocus.GetBuffer(0))){
		//file doesn't exist
		MessageBox("Focus Filename Does Not Exist","Error");
		ret = false;
	}

	if(desc.button_type==BUTT_JUMP)
	{
		//verify the jump screen exists
		int num_screens = 0;
		int curr = Briefing_root_screen;
		while(curr!=-1)
		{
			num_screens++;
			curr = Briefing_screens[curr].next;
		}

		if(m_ScreenToJump<0 || m_ScreenToJump>=num_screens)
		{
			//invalid screen
			MessageBox("The Jump-To Screen Does Not Exist","Error");
			ret = false;
		}
	}

	return ret;
}
void CBriefButtonEdit::OnBriefBtChooseFocus() 
{
	UpdateData(true);
	char szFilter[] = "Outrage Graphic Files (*.ogf)|*.ogf|32bit Uncompressed Targa (*.tga)|*.tga|PCX (*.pcx)|*.pcx|All Files (*.*)|*.*||";
	CFileDialog dlg(true,NULL,NULL,OFN_HIDEREADONLY|OFN_FILEMUSTEXIST,szFilter);

	if(dlg.DoModal()==IDOK){
		//handle
		m_sFilenameFocus = dlg.GetFileName();
		UpdateData(false);		
	}
}

void CBriefButtonEdit::OnBriefBtFlashChooseFocus() 
{
	UpdateData(true);
	char szFilter[] = "Outrage Graphic Files (*.ogf)|*.ogf|32bit Uncompressed Targa (*.tga)|*.tga|PCX (*.pcx)|*.pcx|All Files (*.*)|*.*||";
	CFileDialog dlg(true,NULL,NULL,OFN_HIDEREADONLY|OFN_FILEMUSTEXIST,szFilter);

	if(dlg.DoModal()==IDOK){
		//handle
		m_sFlashFilenameFocus = dlg.GetFileName();
		UpdateData(false);		
	}
}

void CBriefButtonEdit::OnMissionflags() 
{
	CBriefMissionFlagsDlg dlg(desc.mission_mask_set,desc.mission_mask_unset);

	if(dlg.DoModal()){
		desc.mission_mask_set = dlg.m_Set;
		desc.mission_mask_unset = dlg.m_UnSet;
	}	
}

void CBriefButtonEdit::OnBriefBtJump() 
{
	CButton *button = (CButton *)GetDlgItem(IDC_BRIEF_BT_JUMP);
	CEdit *edit = (CEdit *)GetDlgItem(IDC_BRIEF_BT_JUMP_SCREEN);

	if((button->GetState()&0x03)!=0)
		edit->EnableWindow(true);
	else
		edit->EnableWindow(false);	
}

void CBriefButtonEdit::OnBriefBtNextscreen() 
{
	OnBriefBtJump();	
}

void CBriefButtonEdit::OnBriefBtPrevscreen() 
{
	OnBriefBtJump();	
}

void CBriefButtonEdit::OnBriefBtQuit() 
{
	OnBriefBtJump();}

void CBriefButtonEdit::OnBriefBtScrolldown() 
{
	OnBriefBtJump();
}

void CBriefButtonEdit::OnBriefBtScrollup() 
{
	OnBriefBtJump();
}
