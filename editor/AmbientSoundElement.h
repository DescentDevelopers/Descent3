#if !defined(AFX_AMBIENTSOUNDELEMENT_H__D3451742_3680_11D2_A13A_0060089A8026__INCLUDED_)
#define AFX_AMBIENTSOUNDELEMENT_H__D3451742_3680_11D2_A13A_0060089A8026__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// AmbientSoundElement.h : header file
//

#include "ambient.h"

/////////////////////////////////////////////////////////////////////////////
// CAmbientSoundElement dialog

class CAmbientSoundElement : public CDialog
{
// Construction
public:
	CAmbientSoundElement(ase *ase,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAmbientSoundElement)
	enum { IDD = IDD_AMBIENTSOUNDELEMENT };
	float	m_min_volume;
	float	m_max_volume;
	int		m_probability;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAmbientSoundElement)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAmbientSoundElement)
	virtual void OnOK();
	afx_msg void OnSelendokASESoundList();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	ase	*m_ase;
	int	m_handle;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AMBIENTSOUNDELEMENT_H__D3451742_3680_11D2_A13A_0060089A8026__INCLUDED_)
