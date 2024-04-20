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

// TerrainDialog.h : header file
//
#include "keypaddialog.h"

#define CC_MODE_SKY	0
#define CC_MODE_HORIZON	1
#define CC_MODE_FOG		2
#define CC_MODE_SAT		3

/////////////////////////////////////////////////////////////////////////////
// CTerrainDialog dialog
									   
class CTerrainDialog : public CKeypadDialog
{
// Construction
public:
	CTerrainDialog(CWnd* pParent = NULL);   // standard constructor
	virtual void RunKeypadFunction(int code);
	void SetCurrentMoon(int);

	void MoveSat (int,int);

// Dialog Data
	//{{AFX_DATA(CTerrainDialog)
	enum { IDD = IDD_TERRAINKEYPAD };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTerrainDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation

private:
	int Current_satellite;
	int cc_mode;
	void UpdateDialog();
	void DrawSwatch (int,int,int,int);
	
protected:

	// Generated message map functions
	//{{AFX_MSG(CTerrainDialog)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTerrpadMoveUp();
	afx_msg void OnTerrpadMoveDown();
	afx_msg void OnTerrpadImport();
	afx_msg void OnTerrpadRenormalize();
	afx_msg void OnTerrpadRaise10();
	afx_msg void OnTerrpadLower10();
	afx_msg void OnSkyTexture();
	afx_msg void OnTerrpadSelectNone();
	afx_msg void OnTerrpadRotTexture();
	afx_msg void OnSkyNearer();
	afx_msg void OnSkyFarther();
	afx_msg void OnStarsCheck();
	afx_msg void OnFastTerrainCheck();
	afx_msg void OnTerrpadFillArea();
	afx_msg void OnTerrpadRedoTopmap();
	afx_msg void OnTerrpadMakeMax();
	afx_msg void OnTerrpadMakeMin();
	afx_msg void OnTerrpadMakeZero();
	afx_msg void OnTerrpadPyramid();
	afx_msg void OnTerrpadHill();
	afx_msg void OnTerrpadPancakes();
	afx_msg void OnTerrMoveMoon();
	afx_msg void OnTerrRandomizeSky();
	afx_msg void OnPaint();
	afx_msg void OnTerrMoreMoons();
	afx_msg void OnTerrLessMoons();
	afx_msg void OnTerrNextMoon();
	afx_msg void OnTerrPrevMoon();
	afx_msg void OnShowTerrain();
	afx_msg void OnFlatShadeTerrainCheck();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnTerrSelectAll();
	afx_msg void OnTerrain2d();
	afx_msg void OnTextureSky();
	afx_msg void OnKillfocusSkyRedEdit();
	afx_msg void OnKillfocusSkyGreenEdit();
	afx_msg void OnKillfocusSkyBlueEdit();
	afx_msg void OnNoLodEngine();
	afx_msg void OnKillfocusFogDistanceEdit();
	afx_msg void OnKillfocusPixelErrorEdit();
	afx_msg void OnTerrpadSelectrange();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSkyRadio();
	afx_msg void OnHorizonRadio();
	afx_msg void OnFogRadio();
	afx_msg void OnToggleVisibility();
	afx_msg void OnShowInvisible();
	afx_msg void OnTerrMoveMoonAway();
	afx_msg void OnMoveSatUp();
	afx_msg void OnMoveSatDown();
	afx_msg void OnMoveSatLeft();
	afx_msg void OnMoveSatRight();
	afx_msg void OnSatelliteCheck();
	afx_msg void OnSmoothTerrain();
	afx_msg void OnUseFog();
	afx_msg void OnUseHalo();
	afx_msg void OnUseAtmosphere();
	afx_msg void OnDropTerrain();
	afx_msg void OnSaveAsPcx();
	afx_msg void OnNoExtRoomsObjs();
	afx_msg void OnTerrainOcclusion();
	afx_msg void OnSatelliteRadio();
	afx_msg void OnForceVisible();
	afx_msg void OnKillfocusDamagePerSecEdit();
	afx_msg void OnKillfocusFogScalarEdit();
	afx_msg void OnRotateStars();
	afx_msg void OnRotateSky();
	afx_msg void OnKillfocusRotateSpeedEdit();
	afx_msg void OnTileMore();
	afx_msg void OnTileLess();
	afx_msg void OnTerrSound();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

// Makes a copy of current terrain into an undo buffer
void PushTerrainUndo ();

// Copies the undo buffer onto the terrain
void PopTerrainUndo();
