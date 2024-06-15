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

#if !defined(AFX_SOUNDSOURCEDIALOG_H__42188B21_C97F_11D2_A13A_0060089A83BE__INCLUDED_)
#define AFX_SOUNDSOURCEDIALOG_H__42188B21_C97F_11D2_A13A_0060089A83BE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SoundSourceDialog.h : header file
//

#include "SoundListCombo.h"
#include "object.h"

/////////////////////////////////////////////////////////////////////////////
// CSoundSourceDialog dialog

class CSoundSourceDialog : public CDialog {
  // Construction
public:
  CSoundSourceDialog(soundsource_info_s *data, CWnd *pParent = NULL); // standard constructor

  // Dialog Data
  //{{AFX_DATA(CSoundSourceDialog)
  enum { IDD = IDD_SOUNDSOURCE_DIALOG };
  CSoundListCombo m_sound_combo;
  float m_volume;
  //}}AFX_DATA

  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CSoundSourceDialog)
protected:
  virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
                                                   //}}AFX_VIRTUAL

  // Implementation
private:
  soundsource_info_s *m_data_ptr;

protected:
  // Generated message map functions
  //{{AFX_MSG(CSoundSourceDialog)
  virtual void OnOK();
  virtual BOOL OnInitDialog();
  afx_msg void OnSelect();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SOUNDSOURCEDIALOG_H__42188B21_C97F_11D2_A13A_0060089A83BE__INCLUDED_)
