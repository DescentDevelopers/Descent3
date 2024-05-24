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

#if !defined(AFX_SCRIPTSELECT_H__84E5B8C1_29FC_11D1_86CC_004033312F7C__INCLUDED_)
#define AFX_SCRIPTSELECT_H__84E5B8C1_29FC_11D1_86CC_004033312F7C__INCLUDED_
												 
#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ScriptSelect.h : header file
//

#include "objscript.h"
#include "objcscript.h"
#include "d3x.h"

/////////////////////////////////////////////////////////////////////////////
// CScriptSelect dialog

#include "object.h"
#include "trigger.h"															   

class CScriptSelect : public CDialog
{
// Construction
public:
	CScriptSelect(uint16_t scr_type = 0xffff, int custom = CUSTOM_SCRIPT_MASK, CWnd* pParent=NULL);   // standard constructor

	int m_CurScriptSel;
	
//	arguments passed to script from object.
	int m_NumParms;
	tScriptParm m_Parm[MAX_SCRPARAMS];
	int m_CustomType;												// 0 = none, 1 = default, 2 = custom

// Dialog Data
	//{{AFX_DATA(CScriptSelect)
	enum { IDD = IDD_SCRIPTSELECT };
	CString	m_ScriptName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScriptSelect)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CScriptSelect)
	afx_msg void OnDblclkScriptselListbox();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnParameters();
	afx_msg void OnSelchangeScriptselListbox();
	afx_msg void OnScriptselAdd();
	afx_msg void OnEditScript();
	afx_msg void OnScrtype();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void UpdateScriptListbox();
	void EditScript(const char *filename, const char *scrname);

private:
	tD3XProgram *m_ScriptCode;
	uint16_t m_ScriptType;
};


inline void SETUP_SCRIPTSELECT_OBJECT(CScriptSelect& sel, object *obj)
{
	sel.m_ScriptName = obj->script.name;
	sel.m_NumParms = obj->script.num_parms;
	obj->script.copy_parms(sel.m_Parm);	
}

inline void RETURN_SCRIPTSELECT_OBJECT(object *obj, CScriptSelect& sel)
{
	obj->script.set_parms(sel.m_NumParms, sel.m_Parm);
	if (sel.m_ScriptName == "null") 
		obj->script.set_name(NULL);
	else
		obj->script.set_name(sel.m_ScriptName);
	obj->script.is_custom = (sel.m_CustomType==CUSTOM_SCRIPT_MASK) ? 1 : 0;

	mprintf((0, "Script [%s:custom=%d] assigned.\n", sel.m_ScriptName, obj->script.is_custom));
}

inline void SETUP_SCRIPTSELECT_TRIGGER(CScriptSelect& sel, trigger *trig)
{
	sel.m_ScriptName = trig->script.name;
	sel.m_NumParms = trig->script.num_parms;
	trig->script.copy_parms(sel.m_Parm);	
}

inline void RETURN_SCRIPTSELECT_TRIGGER(trigger *trig, CScriptSelect& sel)
{
	trig->script.set_parms(sel.m_NumParms, sel.m_Parm);
	if (sel.m_ScriptName == "null") 
		trig->script.set_name(NULL);
	else
		trig->script.set_name(sel.m_ScriptName);
	trig->script.is_custom = (sel.m_CustomType==CUSTOM_SCRIPT_MASK) ? 1 : 0;

	mprintf((0, "Script [%s:custom=%d] assigned.\n", sel.m_ScriptName, trig->script.is_custom));
}



//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.


#endif // !defined(AFX_SCRIPTSELECT_H__84E5B8C1_29FC_11D1_86CC_004033312F7C__INCLUDED_)
