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

 * $Logfile: /DescentIII/Main/hogedit/HogEditDoc.h $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:57:56 $
 * $Author: kevinb $
 *
 * 
 *
 * $Log: not supported by cvs2svn $
 * 
 * 9     5/05/99 12:53p Nate
 * Added support for multiple file extraction
 * 
 * 8     10/30/98 11:15a Nate
 * Added support for modification of hog files.
 * 
 * 7     9/17/98 4:29p Nate
 * Added Import Directory option.
 * 
 * 6     8/14/98 4:38p Nate
 * Fixed a few minor bugs and added better error reporting
 * 
 * 5     8/14/98 1:01p Nate
 * Added better error reporting for the HogEditor
 * 
 * 4     7/22/98 2:38p Nate
 * Added Modified File prompt when exiting
 * 
 * 3     7/20/98 3:35p Nate
 * Added more Editing functionality
 * 
 * 2     7/15/98 12:31p Nate
 * Initial version
 *
 * $NoKeywords: $
 */

// HogEditDoc.h : interface of the CHogEditDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_HOGEDITDOC_H__48BE52BE_172F_11D2_8CBC_00A0C96ED60D__INCLUDED_)
#define AFX_HOGEDITDOC_H__48BE52BE_172F_11D2_8CBC_00A0C96ED60D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "pstypes.h"

#define FILE_IS_SAME		0
#define FILE_HAS_CHANGED	1
#define FILE_IS_GONE		2

#define RIB_TAG_STR "RIB_ID"

#define DEFAULT_NEW_FILENAME	"Untitled.rib"

#define ADDFILE_OK					0
#define ADDFILE_LONG_FNAME_ERROR	1
#define ADDFILE_DUP_FILE_ERROR		2
#define ADDFILE_STAT_ERROR			3

//	the following structures are slightly modified versions of the CFILE hog structs
struct hog_library_entry
{
	char		path[_MAX_PATH];		// location of data file (filename not included)
	char		name[PSFILENAME_LEN+1];		// just the filename
	unsigned	length;						// length of this file
  int32_t		timestamp;					// time and date of file
	int			flags;						// misc flags
	int			offset;						// file offset in hog (or -1 if in .rib file)
};

// the hog library structure
struct hog_library
{
	char filename[_MAX_PATH];			// full hog file path (including filename)
	int flags;								// misc flags for the hog file
	int num_entries;						// number of entries in the hog file
	CList <hog_library_entry, hog_library_entry&> filelist;
};

// Comparison function for sorting filenames
int compare( const void *arg1, const void *arg2 );

// HogEdit Doc class
class CHogEditDoc : public CDocument
{
protected: // create from serialization only
	CHogEditDoc();
	DECLARE_DYNCREATE(CHogEditDoc)

// Attributes
public:
	CString m_DocumentName;		// name of hog document
	bool m_StaticHog;			// is library a loaded hogfile or a lite hogfile(only names)
	bool m_NoNameChosen;		// Isn't set until user specifies name for a new file
	hog_library Library;		// stores the list of entries

// Operations
public:
	int LoadDocument(CString& name);	// loads a hog or rib
	int SaveDocument(CString& name);   // saves a hog or rib
	void NewDocument();					// creates a new document (entry list)

	int LoadHog(const char *pathname);	// loads a Hog file
	int SaveHog(const char *pathname);	// saves a Hog file
	int LoadRib(const char *pathname);	// loads a Rib file.
	int SaveRib(const char *pathname);	// saves a Rib file.

	int AddFile(const char *pathname, hog_library_entry *entry);	// adds a file to the file list.

	bool ReadHogLibEntry(FILE *fp, hog_library_entry *entry);
	bool WriteHogLibEntry(FILE *fp, hog_library_entry *entry);

	bool CreateFilenameList(char ***filenames);
	void SortFilenameList(char **filenames);
	bool DeleteFilenameList(char **filenames);

	int UpdatedFileCheck(hog_library_entry *entry);
	bool ExtractFile(int file_pos, unsigned file_len, char *out_fname);

	uint32_t CalcHogFileSize(void);

	FILE *FindFileInHog(char *hog_fname,tHogFileEntry *table_entry);

	int CreateNewHogFromCurrentHog(char *src_hog_fname, char *target_hog_fname, 
									int nfiles, const char **filenames, 
									void(* UpdateFunction)(char *));

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHogEditDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CHogEditDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CHogEditDoc)
	afx_msg void OnUpdateActionInsert(CCmdUI* pCmdUI);
	afx_msg void OnUpdateActionCreate(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileSave(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileSaveAs(CCmdUI* pCmdUI);
	afx_msg void OnUpdateActionUpdate(CCmdUI* pCmdUI);
	afx_msg void OnUpdateActionImport(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HOGEDITDOC_H__48BE52BE_172F_11D2_8CBC_00A0C96ED60D__INCLUDED_)
