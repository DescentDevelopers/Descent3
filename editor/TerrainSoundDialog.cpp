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

// TerrainSoundDialog.cpp : implementation file
//

#include "stdafx.h"
#include "editor.h"
#include "TerrainSoundDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTerrainSoundDialog dialog


CTerrainSoundDialog::CTerrainSoundDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CTerrainSoundDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTerrainSoundDialog)
	m_high_alt = 0;
	m_low_alt = 0;
	m_high_volume = 0.0f;
	m_low_volume = 0.0f;
	//}}AFX_DATA_INIT

	for (int b=0;b<NUM_TERRAIN_SOUND_BANDS;b++)
		m_bands[b] = Terrain_sound_bands[b];
}


void CTerrainSoundDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTerrainSoundDialog)
	DDX_Control(pDX, IDC_TERRAIN_SOUND_COMBO, m_sound_combo);
	DDX_Text(pDX, IDC_TERRAIN_SOUND_HIGH_ALT, m_high_alt);
	DDV_MinMaxInt(pDX, m_high_alt, 0, 255);
	DDX_Text(pDX, IDC_TERRAIN_SOUND_LOW_ALT, m_low_alt);
	DDV_MinMaxInt(pDX, m_low_alt, 0, 255);
	DDX_Text(pDX, IDC_TERRAIN_SOUND_HIGH_VOLUME, m_high_volume);
	DDV_MinMaxFloat(pDX, m_high_volume, 0.f, 1.f);
	DDX_Text(pDX, IDC_TERRAIN_SOUND_LOW_VOLUME, m_low_volume);
	DDV_MinMaxFloat(pDX, m_low_volume, 0.f, 1.f);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTerrainSoundDialog, CDialog)
	//{{AFX_MSG_MAP(CTerrainSoundDialog)
	ON_BN_CLICKED(IDC_TERRAIN_SOUND_NEXT, OnTerrainSoundNext)
	ON_BN_CLICKED(IDC_TERRAIN_SOUND_PREV, OnTerrainSoundPrev)
	ON_CBN_SELCHANGE(IDC_TERRAIN_SOUND_COMBO, OnSelchangeTerrainSoundCombo)
	ON_EN_KILLFOCUS(IDC_TERRAIN_SOUND_HIGH_ALT, OnKillfocusTerrainSoundHighAlt)
	ON_EN_KILLFOCUS(IDC_TERRAIN_SOUND_HIGH_VOLUME, OnKillfocusTerrainSoundHighVolume)
	ON_EN_KILLFOCUS(IDC_TERRAIN_SOUND_LOW_ALT, OnKillfocusTerrainSoundLowAlt)
	ON_EN_KILLFOCUS(IDC_TERRAIN_SOUND_LOW_VOLUME, OnKillfocusTerrainSoundLowVolume)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTerrainSoundDialog message handlers

BOOL CTerrainSoundDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_sound_combo.Init(Terrain_sound_bands[0].sound_index);
	m_current = 0;
	CopyToControls();
	UpdateDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTerrainSoundDialog::EnableDisableAll(int flag)
{
	((CButton *) GetDlgItem(IDC_TERRAIN_SOUND_LOW_ALT))->EnableWindow(flag);
	((CButton *) GetDlgItem(IDC_TERRAIN_SOUND_HIGH_ALT))->EnableWindow(flag);
	((CButton *) GetDlgItem(IDC_TERRAIN_SOUND_LOW_VOLUME))->EnableWindow(flag);
	((CButton *) GetDlgItem(IDC_TERRAIN_SOUND_HIGH_VOLUME))->EnableWindow(flag);
}

void CTerrainSoundDialog::UpdateDialog()
{
	//Enabled/disable edit boxes
  	EnableDisableAll(m_bands[m_current].sound_index != -1);

	//Show which band
	PrintToDlgItem(this,IDC_TERRAIN_SOUND_BAND_TEXT,"Band %d:",m_current);

	//Enable/disable next & prev
	((CButton *) GetDlgItem(IDC_TERRAIN_SOUND_NEXT))->EnableWindow(m_current < NUM_TERRAIN_SOUND_BANDS-1);
	((CButton *) GetDlgItem(IDC_TERRAIN_SOUND_PREV))->EnableWindow(m_current > 0);
}

void CTerrainSoundDialog::CopyToControls()
{
	m_low_alt = m_bands[m_current].low_alt;
	m_high_alt = m_bands[m_current].high_alt;
	m_low_volume = m_bands[m_current].low_volume;
	m_high_volume = m_bands[m_current].high_volume;
	m_sound_combo.SetSelected(m_bands[m_current].sound_index);

	UpdateData(false);
}

void CTerrainSoundDialog::CopyFromControls()
{
	UpdateData(true);

	m_bands[m_current].low_alt = m_low_alt;
	m_bands[m_current].high_alt = m_high_alt;
	m_bands[m_current].low_volume = m_low_volume;
	m_bands[m_current].high_volume = m_high_volume;
	m_bands[m_current].sound_index = m_sound_combo.GetSelected();
}

void CTerrainSoundDialog::OnTerrainSoundNext() 
{
	ASSERT(m_current < NUM_TERRAIN_SOUND_BANDS-1);

	CopyFromControls();

	if (m_low_alt > m_high_alt) {
		OutrageMessageBox("High elevation must be greater than or equal to low elevation.");
		return;
	}

	m_current++;

	CopyToControls();

	UpdateDialog();
}

void CTerrainSoundDialog::OnTerrainSoundPrev() 
{
	ASSERT(m_current > 0);

	CopyFromControls();

	if (m_low_alt > m_high_alt) {
		OutrageMessageBox("High elevation must be greater than or equal to low elevation.");
		return;
	}

	m_current--;

	CopyToControls();

	UpdateDialog();
}

void CTerrainSoundDialog::OnOK() 
{
	//Validate and make sure values ok
	if (UpdateData(true)) {

		//Make sure high/low alt values are ok
		if (m_low_alt > m_high_alt) {
			OutrageMessageBox("High elevation must be greater than or equal to low elevation.");
		}
		else {		//everything ok, so bail

			CDialog::OnOK();

			//Copy the new data into our original struct
			for (int b=0;b<NUM_TERRAIN_SOUND_BANDS;b++)
				Terrain_sound_bands[b] = m_bands[b];

			//It's possible that nothing has changed, but we'll assume something has
			World_changed = 1;
		}
	}
}


void CTerrainSoundDialog::OnSelchangeTerrainSoundCombo() 
{
	CopyFromControls();
	UpdateDialog();
}

void CTerrainSoundDialog::OnKillfocusTerrainSoundHighAlt() 
{
	//Force validation
	UpdateData(true);
}

void CTerrainSoundDialog::OnKillfocusTerrainSoundHighVolume() 
{
	//Force validation
	UpdateData(true);
}

void CTerrainSoundDialog::OnKillfocusTerrainSoundLowAlt() 
{
	//Force validation
	UpdateData(true);
}

void CTerrainSoundDialog::OnKillfocusTerrainSoundLowVolume() 
{
	//Force validation
	UpdateData(true);
}
