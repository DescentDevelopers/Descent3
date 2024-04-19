#if !defined(AFX_ROOMLINKDLG_H__D1AFB331_2301_11D2_B2B9_00A0C94B8467__INCLUDED_)
#define AFX_ROOMLINKDLG_H__D1AFB331_2301_11D2_B2B9_00A0C94B8467__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// RoomLinkDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRoomLinkDlg dialog

class CRoomLinkDlg : public CDialog
{
// Construction
public:
	CRoomLinkDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRoomLinkDlg)
	enum { IDD = IDD_ROOMLINK };
	int		m_Room;
	BOOL	m_RoomCheck;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRoomLinkDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRoomLinkDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDoroomlink();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ROOMLINKDLG_H__D1AFB331_2301_11D2_B2B9_00A0C94B8467__INCLUDED_)
