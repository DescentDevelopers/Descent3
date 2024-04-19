#if !defined(AFX_DALLASTEXTUREDLG_H__68211402_B2C7_11D2_A4E0_00A0C96ED60D__INCLUDED_)
#define AFX_DALLASTEXTUREDLG_H__68211402_B2C7_11D2_A4E0_00A0C96ED60D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DallasTextureDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDallasTextureDlg dialog

class CDallasTextureDlg : public CDialog
{
// Construction
public:
	CString	m_TextureName;
	int		m_TextureIndex;


	CDallasTextureDlg(CWnd* pParent = NULL);   // standard constructor

	void FillTextureList(void);

// Dialog Data
	//{{AFX_DATA(CDallasTextureDlg)
	enum { IDD = IDD_DALLAS_TEXTURE_DIALOG };
	CListBox	m_TextureList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDallasTextureDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDallasTextureDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DALLASTEXTUREDLG_H__68211402_B2C7_11D2_A4E0_00A0C96ED60D__INCLUDED_)
