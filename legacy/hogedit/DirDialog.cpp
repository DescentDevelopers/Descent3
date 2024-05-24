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

///////////////////////////////////////////////////////////////////////////
// DirDialog.cpp: implementation of the CDirDialog class.
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DirDialog.h"
#include "shlobj.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// Callback function called by SHBrowseForFolder's browse control
// after initialization and when selection changes
int __stdcall CDirDialog::BrowseCtrlCallback(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
    CDirDialog* pDirDialogObj = (CDirDialog*)lpData;
    if (uMsg == BFFM_INITIALIZED )
    {
        if( ! pDirDialogObj->m_strSelDir.IsEmpty() )
            ::SendMessage(hwnd, BFFM_SETSELECTION, TRUE, (LPARAM)(LPCTSTR)(pDirDialogObj->m_strSelDir));
        if( ! pDirDialogObj->m_strWindowTitle.IsEmpty() )
            ::SetWindowText(hwnd, (LPCTSTR) pDirDialogObj->m_strWindowTitle);
    }
    else if( uMsg == BFFM_SELCHANGED )
    {
        LPITEMIDLIST pidl = (LPITEMIDLIST) lParam;
        char selection[_MAX_PATH];
        if( ! ::SHGetPathFromIDList(pidl, selection) )
            selection[0] = '\0';

        CString csStatusText;
        BOOL bOk = pDirDialogObj->SelChanged(selection, csStatusText);

        if( pDirDialogObj->m_bStatus )
            ::SendMessage(hwnd, BFFM_SETSTATUSTEXT , 0, (LPARAM)(LPCSTR)csStatusText);

        ::SendMessage(hwnd, BFFM_ENABLEOK, 0, bOk);
    }
  return 0;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDirDialog::CDirDialog()
{
	m_bStatus = FALSE;
}

CDirDialog::~CDirDialog()
{
}

BOOL CDirDialog::DoBrowse(CWnd *pwndParent)
{

    if( ! m_strSelDir.IsEmpty() )
    {
        m_strSelDir.TrimRight();
        if( m_strSelDir.Right(1) == "\\" || m_strSelDir.Right(1) == "//" )
            m_strSelDir = m_strSelDir.Left(m_strSelDir.GetLength() - 1);
    }

    LPMALLOC pMalloc;
    if (SHGetMalloc (&pMalloc)!= NOERROR)
        return FALSE;

    BROWSEINFO bInfo;
    LPITEMIDLIST pidl;
    ZeroMemory ( (PVOID) &bInfo,sizeof (BROWSEINFO));

    if (!m_strInitDir.IsEmpty ())
    {
        OLECHAR       olePath[_MAX_PATH];
        ULONG         chEaten;
        ULONG         dwAttributes;
        HRESULT       hr;
        LPSHELLFOLDER pDesktopFolder;
        //
        // Get a pointer to the Desktop's IShellFolder interface.
        //
        if (SUCCEEDED(SHGetDesktopFolder(&pDesktopFolder)))
        {
            //
            // IShellFolder::ParseDisplayName requires the file name be in Unicode.
            //
            MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, m_strInitDir.GetBuffer(_MAX_PATH), -1,
                                olePath, _MAX_PATH);

            m_strInitDir.ReleaseBuffer (-1);
            //
            // Convert the path to an ITEMIDLIST.
            //
            hr = pDesktopFolder->ParseDisplayName(NULL,
                                                NULL,
                                                olePath,
                                                &chEaten,
                                                &pidl,
                                                &dwAttributes);
            if (FAILED(hr))
            {
                pMalloc ->Free (pidl);
                pMalloc ->Release ();
                return FALSE;
            }
            bInfo.pidlRoot = pidl;

        }
    }
    bInfo.hwndOwner = pwndParent == NULL ? NULL : pwndParent->GetSafeHwnd();
    bInfo.pszDisplayName = m_strPath.GetBuffer (_MAX_PATH);
    bInfo.lpszTitle = (m_strTitle.IsEmpty()) ? "Open" : m_strTitle;
    bInfo.ulFlags = BIF_RETURNFSANCESTORS
                    | BIF_RETURNONLYFSDIRS
                    | (m_bStatus ? BIF_STATUSTEXT : 0);

    bInfo.lpfn = BrowseCtrlCallback;  // address of callback function
    bInfo.lParam = (LPARAM)this;      // pass address of object to callback function

    if ((pidl = ::SHBrowseForFolder(&bInfo)) == NULL)
    {
        return FALSE;
    }
    m_strPath.ReleaseBuffer();
    m_iImageIndex = bInfo.iImage;

    if (::SHGetPathFromIDList(pidl, m_strPath.GetBuffer(_MAX_PATH)) == FALSE)
    {
        pMalloc ->Free(pidl);
        pMalloc ->Release();
        return FALSE;
    }

    m_strPath.ReleaseBuffer();

    pMalloc ->Free(pidl);
    pMalloc ->Release();

    return TRUE;
}