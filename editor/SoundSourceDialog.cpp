// SoundSourceDialog.cpp : implementation file
//

#include "stdafx.h"
#include "editor.h"
#include "SoundSourceDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSoundSourceDialog dialog


CSoundSourceDialog::CSoundSourceDialog(soundsource_info_s *data,CWnd* pParent /*=NULL*/)
	: CDialog(CSoundSourceDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSoundSourceDialog)
	m_volume = 0.0f;
	//}}AFX_DATA_INIT

	m_data_ptr = data;
	m_volume = data->volume;
}


void CSoundSourceDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSoundSourceDialog)
	DDX_Control(pDX, IDC_SOUND_COMBO, m_sound_combo);
	DDX_Text(pDX, IDC_VOLUME, m_volume);
	DDV_MinMaxFloat(pDX, m_volume, 0.f, 1.f);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSoundSourceDialog, CDialog)
	//{{AFX_MSG_MAP(CSoundSourceDialog)
	ON_BN_CLICKED(IDC_SELECT, OnSelect)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSoundSourceDialog message handlers

BOOL CSoundSourceDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_sound_combo.Init(m_data_ptr->sound_index);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSoundSourceDialog::OnOK() 
{
	if (UpdateData(true)) {
		m_data_ptr->volume = m_volume;
		m_data_ptr->sound_index = m_sound_combo.GetSelected();
	
		CDialog::OnOK();
	}
}

#include "soundload.h"
#include "DallasSoundDlg.h"

void CSoundSourceDialog::OnSelect() 
{
	CDallasSoundDlg dlg;

	int selected = m_sound_combo.GetSelected();
	if (selected != -1)
		dlg.m_SoundName = Sounds[selected].name;

	if (dlg.DoModal() == IDCANCEL)
		return;

	m_sound_combo.SetSelected(dlg.m_SoundIndex);
}
