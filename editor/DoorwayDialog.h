// DoorwayDialog.h : header file
//

#include "KeypadDialog.h"
#include "gr.h"
#include "doorway.h"

/////////////////////////////////////////////////////////////////////////////
// CDoorwayDialog dialog

class CDoorwayDialog : public CKeypadDialog
{
// Construction
public:
	CDoorwayDialog(CWnd* pParent = NULL);   // standard constructor
	void UpdateKeypad(int mask);

// Dialog Data
	//{{AFX_DATA(CDoorwayDialog)
	enum { IDD = IDD_DOORWAYKEYPAD };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDoorwayDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDoorwayDialog)
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
	afx_msg void OnDoorwayPlaceDoorway();
	afx_msg void OnNextDoor();
	afx_msg void OnPrevDoor();
	afx_msg void OnDoorwayLocked();
	afx_msg void OnKey1Check();
	afx_msg void OnKey2Check();
	afx_msg void OnKey3Check();
	afx_msg void OnKey4Check();
	afx_msg void OnKey5Check();
	afx_msg void OnKey6Check();
	afx_msg void OnKey7Check();
	afx_msg void OnKey8Check();
	afx_msg void OnDoorwayAttachDoor();
	afx_msg void OnDoorwayAuto();
	afx_msg void OnKillfocusDoorwayPosEdit();
	afx_msg void OnKillfocusDoorwayHitpointEdit();
	afx_msg void OnDoorwayKeyAll();
	afx_msg void OnDoorwayKeyOnlyOne();
	afx_msg void OnDoorwayGBIgnoreLocked();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	virtual void OnOK();						// prevent edit controls from closing this window

private:
	void UpdateDialog();
	void DrawDoorwayDoorPic();					// draws a 3d door in the selection box.
	void EnableCurrentItems(bool state);
	void OnKeyCheck(int id,int keynum);

	grHardwareSurface m_DoorSurf;				// surface for door display
};
