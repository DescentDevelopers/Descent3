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

// BriefMovieEdit.cpp : implementation file
//

#include "stdafx.h"
#include "editor.h"
#include "BriefMovieEdit.h"
#include "manage.h"
#include "BriefMissionFlagsDlg.h"
#include "ddio.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBriefMovieEdit dialog


CBriefMovieEdit::CBriefMovieEdit(TCMOVIEDESC* d,CWnd* pParent /*=NULL*/)
	: CDialog(CBriefMovieEdit::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBriefMovieEdit)
	m_sFilename = _T("");
	m_fFPS = 0.0f;
	m_bLooping = FALSE;
	m_fStartTime = 0.0f;
	m_iX = 0;
	m_iY = 0;
	m_sDesc = _T("");
	//}}AFX_DATA_INIT

	memset(&desc,0,sizeof(TCMOVIEDESC));

	//set defaults
	desc.fps = 20.0f;
	desc.mission_mask_set = 0;
	desc.mission_mask_unset = 0;


	if(d){
		//get given values)
		desc.caps = d->caps;

		if(d->caps&TCMD_XY){
			desc.x = d->x;
			desc.y = d->y;
		}
		if(d->caps&TCMD_LOOPING)
			desc.looping = d->looping;
		if(d->caps&TCMD_WAITTIME)
			desc.waittime = d->waittime;
		if(d->caps&TCMD_FPS)
			desc.fps = d->fps;

		desc.mission_mask_set = d->mission_mask_set;
		desc.mission_mask_unset = d->mission_mask_unset;

		strcpy(desc.filename,d->filename);
	}
	
	//fill in with whats valid
	m_sFilename = desc.filename;
	m_fFPS = desc.fps;
	m_bLooping = desc.looping;
	m_fStartTime = desc.waittime;
	m_iX = desc.x;
	m_iY = desc.y;
	
}


void CBriefMovieEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBriefMovieEdit)
	DDX_Text(pDX, IDC_BRIEF_M_FILENAME, m_sFilename);
	DDX_Text(pDX, IDC_BRIEF_M_FPS, m_fFPS);
	DDV_MinMaxFloat(pDX, m_fFPS, 0.f, 100.f);
	DDX_Check(pDX, IDC_BRIEF_M_LOOPING, m_bLooping);
	DDX_Text(pDX, IDC_BRIEF_M_STARTTIME, m_fStartTime);
	DDV_MinMaxFloat(pDX, m_fStartTime, 0.f, 99999.f);
	DDX_Text(pDX, IDC_BRIEF_M_X, m_iX);
	DDV_MinMaxInt(pDX, m_iX, 0, 639);
	DDX_Text(pDX, IDC_BRIEF_M_Y, m_iY);
	DDV_MinMaxInt(pDX, m_iY, 0, 479);
	DDX_Text(pDX, IDC_BRIEF_M_DESC, m_sDesc);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBriefMovieEdit, CDialog)
	//{{AFX_MSG_MAP(CBriefMovieEdit)
	ON_BN_CLICKED(IDC_BRIEF_M_CHOOSE, OnBriefMChoose)
	ON_BN_CLICKED(IDC_MISSIONFLAGS, OnMissionflags)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBriefMovieEdit message handlers

void CBriefMovieEdit::OnBriefMChoose() 
{
	UpdateData(true);
	char szFilter[] = "Outrage Movie Files (*.mve)|*.mve||";
	CFileDialog dlg(true,NULL,NULL,OFN_HIDEREADONLY|OFN_FILEMUSTEXIST,szFilter);

	if(dlg.DoModal()==IDOK){
		//handle
		m_sFilename = dlg.GetFileName();
		UpdateData(false);		
	}
}

void CBriefMovieEdit::OnOK() 
{
	UpdateData(true);

	char path[_MAX_PATH];
	ddio_MakePath(path,LocalD3Dir,"Movies",m_sFilename.GetBuffer(1),NULL);

	if(!cfexist(path)){
		MessageBox("Invalid Filename","Error");
		return;
	}

	desc.caps = TCMD_XY|TCMD_LOOPING|TCMD_WAITTIME|TCMD_FPS;
	strcpy(desc.filename,m_sFilename.GetBuffer(0));
	desc.fps = m_fFPS;
	desc.looping = (bool)(m_bLooping!=0);
	desc.waittime = m_fStartTime; 
	desc.x = m_iX; 
	desc.y = m_iY; 	

	CDialog::OnOK();
}

BOOL CBriefMovieEdit::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	UpdateData(false);
	
	return TRUE;
}

void CBriefMovieEdit::OnMissionflags() 
{
	CBriefMissionFlagsDlg dlg(desc.mission_mask_set,desc.mission_mask_unset);

	if(dlg.DoModal()){
		desc.mission_mask_set = dlg.m_Set;
		desc.mission_mask_unset = dlg.m_UnSet;
	}	
}
