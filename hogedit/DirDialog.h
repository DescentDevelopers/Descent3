////////////////////////////////////////////////////////////////////////
// DirDialog.h: interface for the CDirDialog class.
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIRDIALOG_H__62FFAC92_1DEE_11D1_B87A_0060979CDF6D__INCLUDED_)
#define AFX_DIRDIALOG_H__62FFAC92_1DEE_11D1_B87A_0060979CDF6D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CDirDialog
{
public:

    CDirDialog();
    virtual ~CDirDialog();

    BOOL DoBrowse(CWnd *pwndParent = NULL);

    CString m_strWindowTitle;
    CString m_strPath;
    CString m_strInitDir;
    CString m_strSelDir;
    CString m_strTitle;
    int  m_iImageIndex;
    BOOL m_bStatus;

private:

    virtual BOOL SelChanged(LPCSTR lpcsSelection, CString& csStatusText) { return TRUE; };
    static int __stdcall CDirDialog::BrowseCtrlCallback(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData);
};

#endif // !defined(AFX_DIRDIALOG_H__62FFAC92_1DEE_11D1_B87A_0060979CDF6D__INCLUDED_)

