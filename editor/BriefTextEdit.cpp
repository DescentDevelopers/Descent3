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

// BriefTextEdit.cpp : implementation file
//

#include "stdafx.h"
#include "editor.h"
#include "BriefTextEdit.h"
#include "BriefEdit.h"
#include "BriefMissionFlagsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBriefTextEdit dialog


CBriefTextEdit::CBriefTextEdit(int curr_screen,TCTEXTDESC* d,char *text_buffer,int id,CWnd* pParent /*=NULL*/)
	: CDialog(CBriefTextEdit::IDD, pParent)
{
	m_Screen = curr_screen;
	//{{AFX_DATA_INIT(CBriefTextEdit)
	m_iID = 0;
	m_iBY = 0;
	m_iRX = 0;
	m_fSpeed = 0.0f;
	m_fStartTime = 0.0f;
	m_iTY = 0;
	m_iLX = 0;
	m_iEffectType = -1;
	m_sDesc = _T("");
	m_iBlue = 0;
	m_iGreen = 0;
	m_iRed = 0;
	m_TabStop = FALSE;
	//}}AFX_DATA_INIT

	memset(&desc,0,sizeof(TCTEXTDESC));

	//set defaults
	desc.type = TC_TEXT_STATIC;
	desc.font = BRIEF_FONT_INDEX;
	desc.color = GR_GREEN;
	desc.speed = 1.0f;
	desc.looping = false;
	desc.waittime = 0;
	desc.textbox.left = desc.textbox.top = 0;
	desc.textbox.right = 639;
	desc.textbox.bottom = 479;
	desc.mission_mask_set = 0;
	desc.mission_mask_unset = 0;

	m_TabStop = false;
	if(d){
		desc.caps = d->caps;
		desc.flags = d->flags;
		desc.type = d->type;
		//fill in values
		if(d->caps&TCTD_FONT)
			desc.font = d->font;
		if(d->caps&TCTD_COLOR)
			desc.color = d->color;
		if(d->caps&TCTD_SPEED)
			desc.speed = d->speed;
		if(d->caps&TCTD_LOOPING)
			desc.looping = d->looping;
		if(d->caps&TCTD_TEXTBOX)
			memcpy(&desc.textbox,&d->textbox,sizeof(tc_text));
		if(d->caps&TCTD_WAITTIME)
			desc.waittime = d->waittime;
		if(d->caps&TCTD_TABSTOP)
			m_TabStop = true;

		desc.mission_mask_set = d->mission_mask_set;
		desc.mission_mask_unset = d->mission_mask_unset;
	}

	if(text_buffer)
		m_Text = text_buffer;
	else
		m_Text = "";

	//Set window dialog information
	m_iID = id;
	m_iBY = desc.textbox.bottom;
	m_iRX = desc.textbox.right;
	m_iTY = desc.textbox.top;
	m_iLX = desc.textbox.left;
	m_fSpeed = desc.speed;
	m_fStartTime = desc.waittime;
	m_iRed = GR_COLOR_RED(desc.color);
	m_iGreen = GR_COLOR_GREEN(desc.color);
	m_iBlue = GR_COLOR_BLUE(desc.color);

	switch(desc.type){
	case TC_TEXT_STATIC:
		m_iEffectType = 0;
		break;
	case TC_TEXT_SCROLL:
		switch(desc.flags){
		case TC_TEXTF_L2R:
			m_iEffectType = 4;
			break;
		case TC_TEXTF_R2L:
			m_iEffectType = 5;
			break;
		case TC_TEXTF_T2B:
			m_iEffectType = 6;
			break;
		case TC_TEXTF_B2T:
			m_iEffectType = 7;
			break;
		default:
			m_iEffectType = 4;
			break;
		}
		break;
	case TC_TEXT_FADE:
		switch(desc.flags){
		case TC_TEXTF_IN:
			m_iEffectType = 2;
			break;
		case TC_TEXTF_OUT:
			m_iEffectType = 3;
			break;
		default:
			m_iEffectType = 2;
			break;
		}
		break;
	case TC_TEXT_FLASH:
		m_iEffectType = 1;
		break;
	default:
		m_iEffectType = 0;
	}
}


void CBriefTextEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBriefTextEdit)
	DDX_Text(pDX, IDC_BRIEF_T_ID, m_iID);
	DDX_Text(pDX, IDC_BRIEF_T_LR_Y, m_iBY);
	DDV_MinMaxInt(pDX, m_iBY, 0, 479);
	DDX_Text(pDX, IDC_BRIEF_T_LR_X, m_iRX);
	DDV_MinMaxInt(pDX, m_iRX, 0, 639);
	DDX_Text(pDX, IDC_BRIEF_T_SPEED, m_fSpeed);
	DDV_MinMaxFloat(pDX, m_fSpeed, 0.f, 99999.f);
	DDX_Text(pDX, IDC_BRIEF_T_STARTTIME, m_fStartTime);
	DDV_MinMaxFloat(pDX, m_fStartTime, 0.f, 99999.f);
	DDX_Text(pDX, IDC_BRIEF_T_UL_Y, m_iTY);
	DDV_MinMaxInt(pDX, m_iTY, 0, 479);
	DDX_Text(pDX, IDC_BRIEF_T_UL_X, m_iLX);
	DDV_MinMaxInt(pDX, m_iLX, 0, 639);
	DDX_Radio(pDX, IDC_BRIEF_T_STATIC, m_iEffectType);
	DDX_Text(pDX, IDC_BRIEF_T_DESC, m_sDesc);
	DDX_Text(pDX, IDC_BRIEF_COLOR_B, m_iBlue);
	DDX_Text(pDX, IDC_BRIEF_COLOR_G, m_iGreen);
	DDX_Text(pDX, IDC_BRIEF_COLOR_R, m_iRed);
	DDX_Check(pDX, IDC_TABSTOP, m_TabStop);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBriefTextEdit, CDialog)
	//{{AFX_MSG_MAP(CBriefTextEdit)
	ON_BN_CLICKED(IDC_BRIEF_COLOR_PAL, OnBriefColorPal)
	ON_CBN_SELCHANGE(IDC_BRIEF_T_PREDEF, OnSelchangeBriefTPredef)
	ON_BN_CLICKED(IDC_MISSIONFLAGS, OnMissionflags)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBriefTextEdit message handlers

void CBriefTextEdit::OnOK() 
{
	UpdateData(true);
	
	//Set window dialog information
	desc.caps = TCTD_FONT|TCTD_COLOR|TCTD_SPEED|TCTD_LOOPING|TCTD_WAITTIME|TCTD_TEXTBOX|TCTD_SCROLL;
	desc.textbox.bottom = m_iBY;
	desc.textbox.right  = m_iRX;
	desc.textbox.top    = m_iTY;
	desc.textbox.left   = m_iLX;
	desc.speed			= m_fSpeed;
	desc.waittime       = m_fStartTime;
	desc.color			= GR_RGB(m_iRed,m_iGreen,m_iBlue);

	if(m_TabStop)
		desc.caps |= TCTD_TABSTOP;

	CComboBox *combo = (CComboBox *)GetDlgItem(IDC_BRIEF_T_FONT);
	if(combo->GetCurSel()==1)
		desc.font = BBRIEF_FONT_INDEX;
	else
		desc.font = BRIEF_FONT_INDEX;

	switch(m_iEffectType){
	case 0:
		desc.type = TC_TEXT_STATIC;
		break;
	case 1:
		desc.type = TC_TEXT_FLASH;
		break;
	case 2:
		desc.type = TC_TEXT_FADE;
		desc.flags = TC_TEXTF_IN;
		break;
	case 3:
		desc.type = TC_TEXT_FADE;
		desc.flags = TC_TEXTF_OUT;
		break;
	case 4:
		desc.type = TC_TEXT_SCROLL;
		desc.flags = TC_TEXTF_L2R;
		break;
	case 5:
		desc.type = TC_TEXT_SCROLL;
		desc.flags = TC_TEXTF_R2L;
		break;
	case 6:
		desc.type = TC_TEXT_SCROLL;
		desc.flags = TC_TEXTF_T2B;
		break;
	case 7:
		desc.type = TC_TEXT_SCROLL;
		desc.flags = TC_TEXTF_B2T;
		break;
	};


	m_RichEdit.GetWindowText(m_Text);

	CDialog::OnOK();
}

BOOL CBriefTextEdit::OnInitDialog() 
{
 	RECT rect;
	CWnd *wnd;
	PARAFORMAT pf;
	CHARFORMAT cf;

	m_RichEditInit = false;

	CDialog::OnInitDialog();

//	Create our Rich Edit Control add keyboard handler for those SPECIAL keys like TAB.
	wnd = GetDlgItem(IDC_RICHFRAME);
	wnd->GetWindowRect(&rect); rect.left += 2;  rect.right -= 2;  rect.top += 2; rect.bottom-=2;
	ScreenToClient(&rect);
	m_RichEdit.Create(WS_CHILD | WS_VISIBLE |ES_LEFT
								| ES_MULTILINE | ES_WANTRETURN 
								| ES_AUTOVSCROLL | ES_SAVESEL  | ES_NOHIDESEL 
								| WS_VSCROLL, rect, 
								this, IDC_SCRIPTVIEW);
	m_RichEdit.SetEventMask(ENM_KEYEVENTS | ENM_MOUSEEVENTS);


	m_RichEdit.LimitText(0);
	m_RichEdit.GetParaFormat(pf);

	pf.cbSize = sizeof(pf);
	pf.dwMask = PFM_TABSTOPS;
	pf.cTabCount = MAX_TAB_STOPS;
	for (int i = 0; i < pf.cTabCount; i++)
		pf.rgxTabs[i] = 360 * (i+1);

	m_RichEdit.SetParaFormat(pf);

//	sets character format
	ZeroMemory(&cf, sizeof(cf));
	cf.dwMask = CFM_FACE | CFM_CHARSET | CFM_BOLD;
	strcpy(cf.szFaceName, "Courier");
	cf.bCharSet = DEFAULT_CHARSET;
	cf.bPitchAndFamily = FIXED_PITCH | FF_DONTCARE;
	m_RichEdit.SetDefaultCharFormat(cf);

	///////////////

	CComboBox *combo = (CComboBox *)GetDlgItem(IDC_BRIEF_T_FONT);
		
	combo->AddString("sm_brief");
	combo->AddString("lg_brief");

	if(desc.font == BRIEF_FONT_INDEX)
		combo->SetCurSel(0);
	else
		combo->SetCurSel(1);

	combo = (CComboBox *)GetDlgItem(IDC_BRIEF_T_PREDEF);
	combo->AddString("<Raw>");
	combo->SetCurSel(0);

	int layout = -1;

	for(i=0;i<(*PBnum_layouts);i++){
		if(!stricmp(Briefing_screens[m_Screen].layout,PBlayouts[i].filename))
			layout = i;
	}

	if(layout!=-1){
		int count = PBlayouts[layout].num_texts;
		for(int j=0;j<count;j++){
			char buffer[100];
			sprintf(buffer,"(%d,%d)->(%d,%d)",PBlayouts[layout].texts[j].lx,PBlayouts[layout].texts[j].ty,PBlayouts[layout].texts[j].rx,PBlayouts[layout].texts[j].by);
			combo->AddString(buffer);
		}
	}

	m_RichEdit.SetWindowText((LPCSTR)m_Text.GetBuffer(0));
	
	UpdateData(false);
	
	return TRUE;
}

void CBriefTextEdit::OnBriefColorPal() 
{
	UpdateData(true);
	CColorDialog dlg;

	if(dlg.DoModal()==IDOK){
		COLORREF ref;
		ref = dlg.GetColor();
		m_iRed = GetRValue(ref);
		m_iGreen = GetGValue(ref);
		m_iBlue = GetBValue(ref);
	}
	UpdateData(false);
}

void CBriefTextEdit::OnSelchangeBriefTPredef() 
{
	CComboBox *combo = (CComboBox *)GetDlgItem(IDC_BRIEF_T_PREDEF);
	int item = combo->GetCurSel();

	if(item<=0)
		return;

	UpdateData(true);

	int layout = -1;

	for(int i=0;i<(*PBnum_layouts);i++){
		if(!stricmp(Briefing_screens[m_Screen].layout,PBlayouts[i].filename))
			layout = i;
	}

	if(layout!=-1){
		m_iLX = PBlayouts[layout].texts[item-1].lx;
		m_iTY = PBlayouts[layout].texts[item-1].ty;
		m_iRX = PBlayouts[layout].texts[item-1].rx;
		m_iBY = PBlayouts[layout].texts[item-1].by;
		UpdateData(false);
	}	
}

void CBriefTextEdit::OnMissionflags() 
{
	CBriefMissionFlagsDlg dlg(desc.mission_mask_set,desc.mission_mask_unset);

	if(dlg.DoModal()){
		desc.mission_mask_set = dlg.m_Set;
		desc.mission_mask_unset = dlg.m_UnSet;
	}	
}
