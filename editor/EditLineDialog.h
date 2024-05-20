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

// EditLineDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEditLineDialog dialog

class CEditLineDialog : public CDialog
{
// Construction
public:
	CEditLineDialog(char *caption, char *title, char *initial, bool numeric, CWnd* pParent);   // standard constructor
	CEditLineDialog(char *caption, CWnd* pParent);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEditLineDialog)
	enum { IDD = IDD_EDITLINEDLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditLineDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEditLineDialog)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CString m_EditBuf;
//	char m_Caption[64];
	int size;
	char *m_Buffer;
	char *m_Caption;
	char *m_Initial;
	char *m_Title;

	bool m_Numeric;		//flag for whether this is numeric


public:
//	returns a pointer to the inputted text.
	LPCSTR GetText() const { return (LPCSTR)m_EditBuf; };
};

//Gets a string from the user
//Parameters:	buf - buffer the string is written to.  should be initialized to default value
//					maxsize - the length of buf
//					title - the title for the input window
//					prompt - the prompt for the input box
//Returns:	false if cancel was pressed on the dialog, else true
//				If false returned, buf is unchanged
bool InputString(char *buf,int maxsize,char *title,char *prompt,CWnd *wnd = NULL);

//Gets a number from the user
//Parameters:	n - filled in the with return value
//					title - the title for the input window
//					prompt - the prompt for the input box
//Returns:	false if cancel was pressed on the dialog, else true
//				If false returned, n is unchanged
bool InputNumber(int *n,char *title,char *prompt,CWnd *wnd = NULL);
