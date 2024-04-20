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

// DallasImportDlg.cpp : implementation file
//

#include "stdafx.h"
#include "editor.h"
#include "cfile.h"
#include "DallasMainDlg.h"
#include "DallasUtilities.h"
#include "DallasImportDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDallasImportDlg dialog


CDallasImportDlg::CDallasImportDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDallasImportDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDallasImportDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_ScriptName="";
	m_Filename="";
}


void CDallasImportDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDallasImportDlg)
	DDX_Control(pDX, IDC_SCRIPT_LIST, m_ScriptListBox);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDallasImportDlg, CDialog)
	//{{AFX_MSG_MAP(CDallasImportDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDallasImportDlg message handlers

void CDallasImportDlg::OnOK() 
{
	// TODO: Add extra validation here
	int index=m_ScriptListBox.GetCurSel();
	if(index==LB_ERR) return;

	m_ScriptListBox.GetText(index,m_ScriptName);

	CDialog::OnOK();
}

void CDallasImportDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

BOOL CDallasImportDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	FillScriptListBox(m_Filename.GetBuffer(0));
	m_ScriptListBox.SetCurSel(0);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


BOOL CDallasImportDlg::FillScriptListBox(char *filename)
{
	CFILE *infile;
	char linebuf[2048];
	char *line;
	int linenum;
	int valid_lines_read;

	// Try to open the file for loading
	infile=cfopen (filename,"rt");
	if (!infile)
	{
		CString msg;
		msg.Format("Unable to open \"%s\"!",filename);
		MessageBox(msg,"Script Library File Not Found!",MB_OK|MB_ICONEXCLAMATION);

		return FALSE;
	}

	linenum=0;

	// Read in and parse each line of the file
	while (!cfeof(infile)) {

		// Clear the buffer
		strcpy(linebuf,"");

		// Read in a line from the file
		cf_ReadString(linebuf, sizeof(linebuf), infile);
		linenum++;

		// Remove whitespace padding at start and end of line
		RemoveTrailingWhitespace(linebuf);
		line=SkipInitialWhitespace(linebuf);

		// Check for Start of Script Block Section
		if (strncmp(line,SCRIPT_START_TAG,strlen(SCRIPT_START_TAG)) == 0) {
			bool done = false;

			// Set valid line counter to track whether we're reading header info or tree nodes
			valid_lines_read=0;

			// Read all the lines in the block
			while (!done && !cfeof(infile)) {
				
				strcpy(linebuf,"");
				cf_ReadString(linebuf,sizeof(linebuf),infile);
				linenum++;

				// Remove whitespace padding at start and end of line
				RemoveTrailingWhitespace(linebuf);
				line=SkipInitialWhitespace(linebuf);

				// If it's an empty line or a comment, skip it
				if(strlen(line)==0 || strncmp(line,"//",2)==0)
					continue;

				// Check for End of Script Block Section
				if (strncmp(line,SCRIPT_END_TAG,strlen(SCRIPT_END_TAG)) == 0) {
					done=true;
					continue;
				}

				// See if it should be the script name line
				if(valid_lines_read==0) {
					m_ScriptListBox.AddString(line);
					valid_lines_read++;
					continue;
				}
			}
		}
	}

	cfclose(infile);

	return TRUE;
}