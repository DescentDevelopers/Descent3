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

// MegacellKeypad.h : header file
//

#include "gr.h"
#include "KeypadDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CMegacellKeypad dialog

class CMegacellKeypad : public CKeypadDialog
{
// Construction
public:
	CMegacellKeypad(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMegacellKeypad)
	enum { IDD = IDD_MEGAKEYPAD };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMegacellKeypad)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMegacellKeypad)
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnNextMegaSet();
	afx_msg void OnPrevMegaSet();
	afx_msg void OnPaint();
	afx_msg void OnFillWithMegacell();
	afx_msg void OnRandomizeMegacellCheck();
	afx_msg void OnSelectFromMegacell();
	afx_msg void OnUnifyMegacell();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnKillfocusXGranularEdit();
	afx_msg void OnKillfocusYGranularEdit();
	afx_msg void OnHighlightBorders();
	afx_msg void OnMegacellSkydome();
	afx_msg void OnMegacellSkyband();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
//	loads and saves settings for this keypad
	void LoadSettings();
	void SaveSettings();

	void UpdateDialog();
	void UpdateMegaSetView();
	grHardwareSurface m_TextureSurf;
	int m_xgran,m_ygran;

	static int m_Instances;				// number of instances of this keypad.
};
