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

 * $Logfile: /DescentIII/Main/editor/LightingDialog.h $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:57:38 $
 * $Author: kevinb $
 *
 * Lighting keypad dialog
 *
 * $Log: not supported by cvs2svn $
 * 
 * 37    8/17/99 4:09p Matt
 * Added some accelerator keys, and a function to copy face light
 * multiples.
 * 
 * 36    5/05/99 12:58a Matt
 * Added button to clear the lightmaps on a level.
 * 
 * 35    4/03/99 7:55p Matt
 * Use new keypad update system instead of timer to update.
 * 
 * 34    3/24/99 5:54p Jason
 * 
 * 33    2/18/99 12:33p Jason
 * added room multiplier
 * 
 * 32    1/21/99 12:22p Jason
 * adding strobing and flickering
 * 
 * 31    1/15/99 2:33p Jason
 * added global multiplier
 * 
 * 30    8/04/98 11:52a Jason
 * added reverse corona check
 * 
 * 29    7/24/98 4:44p Jason
 * added corona button functionality
 * 
 * 28    6/05/98 5:22p Jason
 * added volumetric fog
 * 
 * 27    5/26/98 5:56p Jason
 * only render coronas which are flagged
 * 
 * 26    5/20/98 5:44p Jason
 * incremental checkin for bumpmapping
 * 
 * 25    4/24/98 12:58p Jason
 * added functions to resave all bitmaps
 * 
 * 24    4/21/98 2:36p Jason
 * added some lighting options
 * 
 * 23    4/02/98 12:23p Jason
 * trimmed some fat from our structures
 * 
 * 22    2/10/98 5:09p Jason
 * added a SetRoomPulse function for settinng pulse parameters for an
 * entire room
 * 
 * 21    2/10/98 3:50p Jason
 * added pulsing walls
 * 
 * 20    2/04/98 8:25p Jason
 * added light multiplier for faces
 * 
 * 19    12/31/97 12:51p Jason
 * debugged and implementd BSP tree for lighting
 * 
 * 18    12/16/97 4:17p Jason
 * added ignore_satellites and ignore terrain
 * 
 * 17    12/10/97 12:23p Jason
 * added "SuperDetail"
 * 
 * 
 * 16    12/05/97 12:34p Jason
 * added ambient light to the terrain
 * 
 * 15    11/28/97 6:41p Jason
 * added functions to blur lightmaps - not very helpful though
 * 
 * 14    11/25/97 12:28p Jason
 * another pass at shadows
 * 
 * 13    11/19/97 12:39p Jason
 * added dynamic terrain button
 * 
 * 12    11/14/97 6:39p Jason
 * added ability to do lighting on a single room
 * 
 * 11    11/14/97 1:13p Jason
 * added better light counting
 * 
 * 10    10/28/97 3:41p Jason
 * added "use hemicubes" option
 * 
 * 9     10/10/97 1:10p Jason
 * more support for volumetrics
 * 
 * 8     10/10/97 11:38a Jason
 * put in better volumetric support
 * 
 * 7     9/25/97 4:47p Jason
 * update alpha edit box via timer
 * 
 * 6     9/23/97 5:53p Jason
 * added more custom options to radiosity
 * 
 * 5     9/04/97 12:12p Jason
 * added lightmap visibility
 * 
 * 4     9/02/97 11:47a Jason
 * Got alpha per vertex working
 * 
 * 3     8/27/97 5:19p Jason
 * added "Count lights"
 * 
 * 2     8/26/97 4:36p Jason
 * added terrain radiosity
 * 
 * 7     6/03/97 4:55p Mark
 * 
 * 6     5/30/97 5:45p Jason
 * changes to support user-definable lighting parameters
 * 
 * 5     2/20/97 2:37p Mark
 * 
 * 4     1/27/97 11:38a Samir
 * Added horizontal scrolling of keypad modeless dialog.
 *
 * $NoKeywords: $
 */


#include "KeypadDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CLightingDialog dialog

class CLightingDialog : public CKeypadDialog
{
// Construction
public:
	CLightingDialog(CWnd* pParent = NULL);   // standard constructor
	void UpdateKeypad(int mask);

// Dialog Data
	//{{AFX_DATA(CLightingDialog)
	enum { IDD = IDD_LIGHTINGKEYPAD };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLightingDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLightingDialog)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnLitepadSetdefault();
	afx_msg void OnLitepadSmoothall();
	afx_msg void OnKillfocusLightIterationEdit();
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnLitepadTerrainRadiosity();
	afx_msg void OnLitepadCountLights();
	afx_msg void OnLitepadNextvertex();
	afx_msg void OnPaint();
	afx_msg void OnKillfocusAlphaEdit();
	afx_msg void OnShowLightmaps();
	afx_msg void OnKillfocusHemicubeResEdit();
	afx_msg void OnKillfocusLightSpacingEdit();
	afx_msg void OnBestfitCheck();
	afx_msg void OnCreateVolumeCone();
	afx_msg void OnClearVolumetricRoom();
	afx_msg void OnFillVolumeTexture();
	afx_msg void OnKillfocusAngleEdit();
	afx_msg void OnKillfocusStartAlphaEdit();
	afx_msg void OnKillfocusResolutionEdit();
	afx_msg void OnEdgeVolumetric();
	afx_msg void OnKillfocusFalloffEdit();
	afx_msg void OnFarthestPoint();
	afx_msg void OnKillfocusPitchAngleEdit();
	afx_msg void OnKillfocusHeadingAngleEdit();
	afx_msg void OnHemicubeCheck();
	afx_msg void OnCountTerrainLights();
	afx_msg void OnCurroomRadiosity();
	afx_msg void OnDynamicTerrain();
	afx_msg void OnBlurLightmaps();
	afx_msg void OnKillfocusIgnoreLimitEdit();
	afx_msg void OnKillfocusRedAmbientEdit();
	afx_msg void OnKillfocusGreenAmbientEdit();
	afx_msg void OnKillfocusBlueAmbientEdit();
	afx_msg void OnElementCheck();
	afx_msg void OnIgnoreTerrain();
	afx_msg void OnIgnoreSatellites();
	afx_msg void OnUseBspCheck();
	afx_msg void OnKillfocusLightMultiplyEdit();
	afx_msg void OnKillfocusPulseTimeEdit();
	afx_msg void OnKillfocusPulseOffsetEdit();
	afx_msg void OnPulseRoom();
	afx_msg void OnResetMultiples();
	afx_msg void OnDestroyBsp();
	afx_msg void OnCombineFaces();
	afx_msg void OnVolumeLights();
	afx_msg void OnCoronaCheck();
	afx_msg void OnFogCheck();
	afx_msg void OnKillfocusFogDensity();
	afx_msg void OnKillfocusFogRed();
	afx_msg void OnKillfocusFogGreen();
	afx_msg void OnKillfocusFogBlue();
	afx_msg void OnRoomCoronas();
	afx_msg void OnMineCoronas();
	afx_msg void OnReverseCorona();
	afx_msg void OnKillfocusGlobalMultiplierEdit();
	afx_msg void OnStrobeCheck();
	afx_msg void OnFlickerCheck();
	afx_msg void OnKillfocusRoomMultiplierEdit();
	afx_msg void OnKillfocusRoomAmred();
	afx_msg void OnKillfocusRoomAmgreen();
	afx_msg void OnKillfocusRoomAmblue();
	afx_msg void OnClearLightmaps();
	afx_msg void OnCopyMultiplier();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	void UpdateDialog();
	void ShootRayForLighting (vector *dest,vector *from,vector *to,int startseg);
	int m_angle;
	float m_start_alpha;
	int m_resolution;
	float m_falloff;
	int m_pitch_angle;
	int m_heading_angle;

};
