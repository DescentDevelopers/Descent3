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

// BriefBitmapEdit.cpp : implementation file
//

#include "stdafx.h"
#include "editor.h"
#include "BriefBitmapEdit.h"
#include "BriefEdit.h"
#include "BriefMissionFlagsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBriefBitmapEdit dialog


CBriefBitmapEdit::CBriefBitmapEdit(int curr_screen,TCBMPDESC* d,CWnd* pParent /*=NULL*/)
	: CDialog(CBriefBitmapEdit::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBriefBitmapEdit)
	m_sFilename = _T("");
	m_fSpeed = 0.0f;
	m_fStartTime = 0.0f;
	m_iX = 0;
	m_iY = 0;
	m_iEffectType = -1;
	m_sDesc = _T("");
	m_NoRenderUntilStart = FALSE;
	//}}AFX_DATA_INIT

	//set to default values
	memset(&desc,0,sizeof(TCBMPDESC));
	desc.type = TC_BMP_STATIC;
	m_Screen = curr_screen;
	desc.mission_mask_set = 0;
	desc.mission_mask_unset = 0;


	if(d){
		//set to values passed in
		desc.caps = d->caps;
		if(d->caps&TCBD_XY){
			desc.x = d->x;
			desc.y = d->y;
		}
		if(d->caps&TCBD_LOOPING){
			desc.looping = d->looping;
		}
		if(d->caps&TCBD_WAITTIME){
			desc.waittime = d->waittime;
		}
		if(d->caps&TCBD_SPEED){
			desc.speed = d->speed;
		}
		if(d->flags&TC_NOEARLYRENDER)
		{
			m_NoRenderUntilStart = true;
		}

		desc.type = d->type;
		desc.flags = d->flags;
		strcpy(desc.filename,d->filename);
		desc.mission_mask_set = d->mission_mask_set;
		desc.mission_mask_unset = d->mission_mask_unset;
	}

	switch(desc.type){
	case TC_BMP_STATIC:
		m_iEffectType = 0;
		break;
	case TC_BMP_BLUR:
		if(desc.flags==TC_BMPF_IN)
			m_iEffectType = 1;
		else
			m_iEffectType = 2;
		break;
	case TC_BMP_SCANLINE:
		if(desc.flags==TC_BMPF_IN)
			m_iEffectType = 3;
		else
			m_iEffectType = 4;
		break;
	case TC_BMP_INVERT:
		if(desc.flags==TC_BMPF_IN)
			m_iEffectType = 5;
		else
			m_iEffectType = 6;
		break;
	case TC_BMP_STRETCH:
		if(desc.flags==TC_BMPF_IN)
			m_iEffectType = 7;
		else
			m_iEffectType = 8;
		break;
	}

	m_sFilename = desc.filename;
	m_fSpeed = desc.speed;
	m_fStartTime = desc.waittime;
	m_iX = desc.x;
	m_iY = desc.y;
}


void CBriefBitmapEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBriefBitmapEdit)
	DDX_Text(pDX, IDC_BRIEF_B_FILENAME, m_sFilename);
	DDX_Text(pDX, IDC_BRIEF_B_SPEED, m_fSpeed);
	DDV_MinMaxFloat(pDX, m_fSpeed, 0.f, 99999.f);
	DDX_Text(pDX, IDC_BRIEF_B_STARTTIME, m_fStartTime);
	DDV_MinMaxFloat(pDX, m_fStartTime, 0.f, 99999.f);
	DDX_Text(pDX, IDC_BRIEF_B_X, m_iX);
	DDV_MinMaxInt(pDX, m_iX, 0, 639);
	DDX_Text(pDX, IDC_BRIEF_B_Y, m_iY);
	DDV_MinMaxInt(pDX, m_iY, 0, 479);
	DDX_Radio(pDX, IDC_BRIEF_B_STATIC, m_iEffectType);
	DDX_Text(pDX, IDC_BRIEF_B_DESC, m_sDesc);
	DDX_Check(pDX, IDC_BRIEF_B_NORENDER, m_NoRenderUntilStart);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBriefBitmapEdit, CDialog)
	//{{AFX_MSG_MAP(CBriefBitmapEdit)
	ON_BN_CLICKED(IDC_BRIEF_B_CHOOSE, OnBriefBChoose)
	ON_CBN_SELCHANGE(IDC_BRIEF_B_PREDEF, OnSelchangeBriefBPredef)
	ON_BN_CLICKED(IDC_MISSIONFLAGS, OnMissionflags)
	ON_BN_CLICKED(IDC_BRIEF_B_NORENDER, OnBriefBNorender)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBriefBitmapEdit message handlers

void CBriefBitmapEdit::OnOK() 
{
	UpdateData(true);

	if(!cfexist(m_sFilename.GetBuffer(0))){
		MessageBox("Not A Valid Filename","Error");
		return;
	}

	desc.caps = TCBD_XY|TCBD_LOOPING|TCBD_WAITTIME|TCBD_SPEED;
	strcpy(desc.filename,m_sFilename);
	desc.speed = m_fSpeed;
	desc.waittime = m_fStartTime;
	desc.x = m_iX;
	desc.y = m_iY;

	switch(m_iEffectType){
	case 0:	desc.type = TC_BMP_STATIC;	desc.flags = TC_BMPF_IN;	break;
	case 1:	desc.type = TC_BMP_BLUR;	desc.flags = TC_BMPF_IN;	break;
	case 2:	desc.type = TC_BMP_BLUR;	desc.flags = TC_BMPF_OUT;	break;
	case 3:	desc.type = TC_BMP_SCANLINE;	desc.flags = TC_BMPF_IN;	break;
	case 4:	desc.type = TC_BMP_SCANLINE;	desc.flags = TC_BMPF_OUT;	break;
	case 5:	desc.type = TC_BMP_INVERT;	desc.flags = TC_BMPF_IN;	break;
	case 6:	desc.type = TC_BMP_INVERT;	desc.flags = TC_BMPF_OUT;	break;
	case 7:	desc.type = TC_BMP_STRETCH;	desc.flags = TC_BMPF_IN;	break;
	case 8:	desc.type = TC_BMP_STRETCH;	desc.flags = TC_BMPF_OUT;	break;
	}

	if(m_NoRenderUntilStart)
		desc.flags |= TC_NOEARLYRENDER;
	else
		desc.flags &= ~TC_NOEARLYRENDER;
	
	CDialog::OnOK();
}

void CBriefBitmapEdit::OnBriefBChoose() 
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

BOOL CBriefBitmapEdit::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CComboBox *combo = (CComboBox *)GetDlgItem(IDC_BRIEF_B_PREDEF);
	combo->AddString("<Raw>");
	combo->SetCurSel(0);

	int layout = -1;

	for(int i=0;i<(*PBnum_layouts);i++){
		if(!stricmp(Briefing_screens[m_Screen].layout,PBlayouts[i].filename))
			layout = i;
	}

	if(layout!=-1){
		int count = PBlayouts[layout].num_bmps;
		for(int j=0;j<count;j++){
			char buffer[100];
			sprintf(buffer,"(%d,%d)",PBlayouts[layout].bmps[j].x,PBlayouts[layout].bmps[j].y);
			combo->AddString(buffer);
		}
	}

	UpdateData(false);
	
	return TRUE;
}

void CBriefBitmapEdit::OnSelchangeBriefBPredef() 
{
	CComboBox *combo = (CComboBox *)GetDlgItem(IDC_BRIEF_B_PREDEF);
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
		m_iX = PBlayouts[layout].bmps[item-1].x;
		m_iY = PBlayouts[layout].bmps[item-1].y;
		UpdateData(false);
	}	

}

void CBriefBitmapEdit::OnMissionflags() 
{
	CBriefMissionFlagsDlg dlg(desc.mission_mask_set,desc.mission_mask_unset);

	if(dlg.DoModal()){
		desc.mission_mask_set = dlg.m_Set;
		desc.mission_mask_unset = dlg.m_UnSet;
	}	
}

void CBriefBitmapEdit::OnBriefBNorender() 
{
}
