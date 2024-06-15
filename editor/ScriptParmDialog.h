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

#if !defined(AFX_SCRIPTPARMDIALOG_H__7B248DB1_91AA_11D1_BFC4_00A0C94B8467__INCLUDED_)
#define AFX_SCRIPTPARMDIALOG_H__7B248DB1_91AA_11D1_BFC4_00A0C94B8467__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ScriptParmDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CScriptParmDialog dialog

// struct tScriptParm;

#include "d3x.h"
#include "ObjCScript.h"

class CScriptParmDialog : public CDialog {
  // Construction
public:
  CScriptParmDialog(CWnd *pParent = NULL); // standard constructor

  // Dialog Data
  //{{AFX_DATA(CScriptParmDialog)
  enum { IDD = IDD_SCRIPTPARMS };
  CString m_Parm1;
  CString m_Parm2;
  CString m_Parm3;
  CString m_Parm4;
  CString m_Parm5;
  CString m_Parm6;
  CString m_Parm7;
  CString m_Parm8;
  CString m_ParmEdit1;
  CString m_ParmEdit2;
  CString m_ParmEdit3;
  CString m_ParmEdit4;
  CString m_ParmEdit5;
  CString m_ParmEdit6;
  CString m_ParmEdit7;
  CString m_ParmEdit8;
  //}}AFX_DATA

public:
  void ConvertToScriptParameter(tScriptParm *parm, const char *type, const char *text);
  void ConvertParamToText(CString &text, tScriptParm *parm);

  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CScriptParmDialog)
protected:
  virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
  virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
  //}}AFX_VIRTUAL

  // Implementation
protected:
  // Generated message map functions
  //{{AFX_MSG(CScriptParmDialog)
  virtual BOOL OnInitDialog();
  afx_msg void OnParamlist();
  afx_msg void OnParamlist2();
  afx_msg void OnParamlist3();
  afx_msg void OnParamlist4();
  afx_msg void OnParamlist5();
  afx_msg void OnParamlist6();
  afx_msg void OnParamlist7();
  afx_msg void OnParamlist8();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()

private:
  CWnd *m_ContextWnd;
};

inline void RETURN_SCRIPTPARM_DIALOG(tScriptParm *parmlist, CScriptParmDialog *dlg, char *parmtypelist[MAX_SCRPARAMS]) {
  dlg->ConvertToScriptParameter(&parmlist[0], parmtypelist[0], dlg->m_ParmEdit1);
  dlg->ConvertToScriptParameter(&parmlist[1], parmtypelist[1], dlg->m_ParmEdit2);
  dlg->ConvertToScriptParameter(&parmlist[2], parmtypelist[2], dlg->m_ParmEdit3);
  dlg->ConvertToScriptParameter(&parmlist[3], parmtypelist[3], dlg->m_ParmEdit4);
  dlg->ConvertToScriptParameter(&parmlist[4], parmtypelist[4], dlg->m_ParmEdit5);
  dlg->ConvertToScriptParameter(&parmlist[5], parmtypelist[5], dlg->m_ParmEdit6);
  dlg->ConvertToScriptParameter(&parmlist[6], parmtypelist[6], dlg->m_ParmEdit7);
  dlg->ConvertToScriptParameter(&parmlist[7], parmtypelist[7], dlg->m_ParmEdit8);
}

inline void SETUP_SCRIPTPARM_DIALOG(CScriptParmDialog *dlg, char *namelist[MAX_SCRPARAMS], tScriptParm *parmlist) {
  dlg->m_Parm1 = namelist[0];
  dlg->m_Parm2 = namelist[1];
  dlg->m_Parm3 = namelist[2];
  dlg->m_Parm4 = namelist[3];
  dlg->m_Parm5 = namelist[4];
  dlg->m_Parm6 = namelist[5];
  dlg->m_Parm7 = namelist[6];
  dlg->m_Parm8 = namelist[7];

  dlg->ConvertParamToText(dlg->m_ParmEdit1, &parmlist[0]);
  dlg->ConvertParamToText(dlg->m_ParmEdit2, &parmlist[1]);
  dlg->ConvertParamToText(dlg->m_ParmEdit3, &parmlist[2]);
  dlg->ConvertParamToText(dlg->m_ParmEdit4, &parmlist[3]);
  dlg->ConvertParamToText(dlg->m_ParmEdit5, &parmlist[4]);
  dlg->ConvertParamToText(dlg->m_ParmEdit6, &parmlist[5]);
  dlg->ConvertParamToText(dlg->m_ParmEdit7, &parmlist[6]);
  dlg->ConvertParamToText(dlg->m_ParmEdit8, &parmlist[7]);
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCRIPTPARMDIALOG_H__7B248DB1_91AA_11D1_BFC4_00A0C94B8467__INCLUDED_)
