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

// BriefSoundEdit.cpp : implementation file
//

#include "stdafx.h"
#include "editor.h"
#include "BriefSoundEdit.h"
#include "BriefMissionFlagsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBriefSoundEdit dialog


CBriefSoundEdit::CBriefSoundEdit(TCSNDDESC* d,CWnd* pParent /*=NULL*/)
	: CDialog(CBriefSoundEdit::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBriefSoundEdit)
	m_sFilename = _T("");
	m_bPlayOnce = FALSE;
	m_fStartTime = 0.0f;
	m_sDesc = _T("");
	//}}AFX_DATA_INIT

	memset(&desc,0,sizeof(TCSNDDESC));

	//Set defaults
	desc.waittime = 0;
	desc.once = false;
	desc.mission_mask_set = 0;
	desc.mission_mask_unset = 0;

	
	if(d){
		//fill in values
		strcpy(desc.filename,d->filename);
		if(d->caps&TCSD_WAITTIME)
			desc.waittime = d->waittime;
		if(d->caps&TCSD_ONCE)
			desc.once = d->once;

		desc.caps = d->caps;
		desc.type = d->type;
		desc.mission_mask_set = d->mission_mask_set;
		desc.mission_mask_unset = d->mission_mask_unset;
	}

	//Setup Window dialog
	m_sFilename = desc.filename;
	m_bPlayOnce = desc.once;
	m_fStartTime = desc.waittime;
}


void CBriefSoundEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBriefSoundEdit)
	DDX_Text(pDX, IDC_BRIEF_S_FILENAME, m_sFilename);
	DDX_Check(pDX, IDC_BRIEF_S_PLAYONCE, m_bPlayOnce);
	DDX_Text(pDX, IDC_BRIEF_S_STARTTIME, m_fStartTime);
	DDV_MinMaxFloat(pDX, m_fStartTime, 0.f, 99999.f);
	DDX_Text(pDX, IDC_BRIEF_S_DESC, m_sDesc);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBriefSoundEdit, CDialog)
	//{{AFX_MSG_MAP(CBriefSoundEdit)
	ON_BN_CLICKED(IDC_BRIEF_S_CHOOSE, OnBriefSChoose)
	ON_BN_CLICKED(IDC_MISSIONFLAGS, OnMissionflags)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBriefSoundEdit message handlers

void CBriefSoundEdit::OnBriefSChoose() 
{
	UpdateData(true);
	char szFilter[] = "WAV File (*.wav)|*.wav||";
	CFileDialog dlg(true,NULL,NULL,OFN_HIDEREADONLY|OFN_FILEMUSTEXIST,szFilter);

	if(dlg.DoModal()==IDOK){
		//handle
		m_sFilename = dlg.GetFileName();
		UpdateData(false);		
	}
}

void CBriefSoundEdit::OnOK() 
{
	if(!cfexist(m_sFilename.GetBuffer(1))){
		MessageBox("Invalid Filename, Please Enter A Valid Filename","Error");
		return;
	}

	UpdateData(true);

	desc.caps = TCSD_WAITTIME|TCSD_ONCE;
	strcpy(desc.filename,m_sFilename.GetBuffer(0));
	desc.once = (bool)(m_bPlayOnce!=0);
	desc.waittime = m_fStartTime;
	
	CDialog::OnOK();
}

BOOL CBriefSoundEdit::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	UpdateData(false);
	
	return TRUE;
}

void CBriefSoundEdit::OnMissionflags() 
{
	CBriefMissionFlagsDlg dlg(desc.mission_mask_set,desc.mission_mask_unset);

	if(dlg.DoModal()){
		desc.mission_mask_set = dlg.m_Set;
		desc.mission_mask_unset = dlg.m_UnSet;
	}	
}
