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

#if !defined(AFX_VIDEODETECTDLG_H__11362FC3_61E3_11D2_A4E0_00A0C96ED60D__INCLUDED_)
#define AFX_VIDEODETECTDLG_H__11362FC3_61E3_11D2_A4E0_00A0C96ED60D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VideoDetectDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CVideoDetectDlg dialog

class CVideoDetectDlg : public CDialog {
  // Construction
public:
  CVideoDetectDlg(CWnd *pParent = NULL); // standard constructor

  int DetectOpenGL;
  int DetectDirect3D;
  int DetectGlide;

  // Dialog Data
  //{{AFX_DATA(CVideoDetectDlg)
  enum { IDD = IDD_VIDEODETECT_DLG };
  CButton m_OpenGLCheck;
  CButton m_GlideCheck;
  CButton m_Direct3DCheck;
  //}}AFX_DATA

  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CVideoDetectDlg)
protected:
  virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
                                                   //}}AFX_VIRTUAL

  // Implementation
protected:
  // Generated message map functions
  //{{AFX_MSG(CVideoDetectDlg)
  virtual void OnOK();
  virtual BOOL OnInitDialog();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIDEODETECTDLG_H__11362FC3_61E3_11D2_A4E0_00A0C96ED60D__INCLUDED_)
