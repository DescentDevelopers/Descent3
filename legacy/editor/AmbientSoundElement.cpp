// AmbientSoundElement.cpp : implementation file
//

#include "stdafx.h"
#include "editor.h"
#include "AmbientSoundElement.h"
#include "soundload.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAmbientSoundElement dialog


CAmbientSoundElement::CAmbientSoundElement(ase *ase,CWnd* pParent /*=NULL*/)
	: CDialog(CAmbientSoundElement::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAmbientSoundElement)
	m_min_volume = 0.0f;
	m_max_volume = 0.0f;
	m_probability = 0;
	//}}AFX_DATA_INIT

	m_ase = ase;

	m_min_volume = ase->min_volume;
	m_max_volume = ase->max_volume;
	m_probability = ase->probability;
	m_handle = ase->handle;
}


void CAmbientSoundElement::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAmbientSoundElement)
	DDX_Text(pDX, IDC_MIN_VOLUME, m_min_volume);
	DDX_Text(pDX, IDC_ASE_MAX_VOLUME, m_max_volume);
	DDX_Text(pDX, IDC_ASE_PROBABILITY, m_probability);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAmbientSoundElement, CDialog)
	//{{AFX_MSG_MAP(CAmbientSoundElement)
	ON_CBN_SELENDOK(IDC_ASE_SOUND_LIST, OnSelendokASESoundList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAmbientSoundElement message handlers

void CAmbientSoundElement::OnOK() 
{
	CDialog::OnOK();

	m_ase->min_volume = m_min_volume;
	m_ase->max_volume = m_max_volume;
	m_ase->probability = m_probability;
	m_ase->handle = m_handle;
	
}

#include "pserror.h"

void CAmbientSoundElement::OnSelendokASESoundList() 
{
	char name[200];
	CComboBox *soundlist = (CComboBox *) GetDlgItem(IDC_ASE_SOUND_LIST);

	soundlist->GetLBText(soundlist->GetCurSel(),name);
	m_handle = FindSoundName(name);
}

BOOL CAmbientSoundElement::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CComboBox *soundlist = (CComboBox *) GetDlgItem(IDC_ASE_SOUND_LIST);
	soundlist->ResetContent();
	for (int i=0;i<MAX_SOUNDS;i++)
		if (Sounds[i].used)
			soundlist->AddString(Sounds[i].name);
	soundlist->SelectString(-1,Sounds[m_handle].name);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
