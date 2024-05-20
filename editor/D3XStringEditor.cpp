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

 * $Logfile: /DescentIII/Main/editor/D3XStringEditor.cpp $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:57:37 $
 * $Author: kevinb $
 *
 * D3XString Editor
 *
 * $Log: not supported by cvs2svn $
 * 
 * 2     9/09/98 12:48p Samir
 * added script localizer.
 * 
 * 1     9/09/98 10:29a Samir
 * Initial revision.
 *
 * $NoKeywords: $
 */

#include "stdafx.h"
#include "editor.h"
#include "D3XStringEditor.h"
#include "pserror.h"
#include "manage.h"
#include "cfile.h"
#include "d3x.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CD3XStringEditor dialog


CD3XStringEditor::CD3XStringEditor(CWnd* pParent /*=NULL*/)
	: CDialog(CD3XStringEditor::IDD, pParent)
{
	//{{AFX_DATA_INIT(CD3XStringEditor)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_modified = false;
	m_cursel = LB_ERR;
}


void CD3XStringEditor::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CD3XStringEditor)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CD3XStringEditor, CDialog)
	//{{AFX_MSG_MAP(CD3XStringEditor)
	ON_BN_CLICKED(IDC_LOADSCRIPT, OnLoadscript)
	ON_LBN_SELCHANGE(IDC_STRINGLIST, OnSelchangeStringlist)
	ON_LBN_DBLCLK(IDC_STRINGLIST, OnDblclkStringlist)
	ON_EN_KILLFOCUS(IDC_STRINGEDIT, OnKillfocusStringedit)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CD3XStringEditor message handlers

void CD3XStringEditor::OnLoadscript() 
{
//	open either a level or a .d3x file
	char openpath[_MAX_PATH+1];

	if (!PromptSave()) 
		return;

	if (OpenFileDialog(this, "Descent 3 scripts (*.d3x)|*.d3x||", openpath, LocalLevelsDir, _MAX_PATH+1)) {
	//	load program including string table and fill list box will strings.
		tD3XProgram *prog;
		CListBox *list = (CListBox *)GetDlgItem(IDC_STRINGLIST);
		CFILE *fp = cfopen(openpath, "rb");

		if (!fp) {
			OutrageMessageBox("Unable to open %s.", openpath);
			return;
		}

		prog = D3XLoadProgram(fp);
		cfclose(fp);
		if (!prog) {
			OutrageMessageBox("Illegal d3x file format.");
			return;
		}

		list->ResetContent();
		for (uint16_t i = 0; i < prog->nstr; i++)
			list->AddString(prog->strlist[i]);
		list->EnableWindow();

		OnSelchangeStringlist();

		D3XFreeProgram(prog);

		m_pathname = openpath;
		m_modified = false;
	}
}

void CD3XStringEditor::OnSelchangeStringlist() 
{
	CListBox *list = (CListBox *)GetDlgItem(IDC_STRINGLIST);
	CEdit *edit = (CEdit *)GetDlgItem(IDC_STRINGEDIT);
	CString str;
	int cursel = list->GetCurSel();

// replace current string in edit box into this listbox
	if (m_cursel != LB_ERR) {
		edit->GetWindowText(str);
		list->DeleteString(m_cursel);
		list->InsertString(m_cursel, str);
		m_modified = true;
		GetDlgItem(IDC_SAVE)->EnableWindow();
	}

// put new string into edit box.
	if (cursel != LB_ERR) {
		list->GetText(cursel, str);
		edit->SetWindowText((LPCSTR)str);
		edit->EnableWindow();
	}
	m_cursel = cursel;
}

void CD3XStringEditor::OnDblclkStringlist() 
{
}

void CD3XStringEditor::OnKillfocusStringedit() 
{
	m_modified = true;
	GetDlgItem(IDC_SAVE)->EnableWindow();
}

void CD3XStringEditor::OnSave() 
{
	ASSERT(m_modified);

// save program by loading it in first and reconstructin a new program obhect
	CFILE *fp = cfopen(m_pathname, "rb");
	if (fp) {
		tD3XProgram *old_prog = D3XLoadProgram(fp);
		tD3XProgram *new_prog;

		cfclose(fp);

		if (old_prog) {
			CListBox *list = (CListBox *)GetDlgItem(IDC_STRINGLIST);
			int curlength = 65536;
			char *strbuf = new char[curlength];
			char *strptr;
			int i,n = list->GetCount();
			for (i = 0, strptr = strbuf; i < n; i++)
			{
				CString str;
				list->GetText(i, str);
				if ((strptr+str.GetLength()+1) > (strbuf+curlength)) {
					curlength *= 2;
					char *newbuf = new char[curlength];
					strcpy(newbuf, strbuf);
					delete[] strbuf;
					strbuf = newbuf;
				}
				strcpy(strptr, str);
				strptr += str.GetLength()+1;
			}

			new_prog = D3XReallocProgram(NULL, old_prog->len, old_prog->nscripts, old_prog->nstr);
			if (new_prog) {
				D3XLoadProgramFromComponents(new_prog, old_prog->code, old_prog->map, strbuf);
				D3XFreeProgram(old_prog);
				fp = cfopen(m_pathname, "wb");
				D3XSaveProgram(fp, new_prog);
				cfclose(fp);
				D3XFreeProgram(new_prog);
			}
			else {
				OutrageMessageBox("Failed to reallocate new program.");
				delete[] strbuf;
				return;
			}

			delete[] strbuf;
		}
	}
	else {
		OutrageMessageBox("Fatal error. Unable to load script file %s.", m_pathname);
		return;
	}


	m_modified = false;
	GetDlgItem(IDC_SAVE)->EnableWindow(FALSE);

}

void CD3XStringEditor::OnOK() 
{
	// TODO: Add extra validation here
	if (!PromptSave()) 
		return;
	
	CDialog::OnOK();
}


bool CD3XStringEditor::PromptSave()
{
	return true;
}
