// PathPadDialog.h : header file
//

#include "keypaddialog.h"

/////////////////////////////////////////////////////////////////////////////
// CPathPadDialog dialog

class CPathPadDialog : public CKeypadDialog
{
// Construction
public:
	CPathPadDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPathPadDialog)
	enum { IDD = IDD_PATHKEYPAD };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPathPadDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation

private:

	void UpdateDialog();
	void ResetNodesForComboBox(int handle, char s_type);

protected:

	// Generated message map functions
	//{{AFX_MSG(CPathPadDialog)
	afx_msg void OnKillfocusCurrentNodeEdit();
	afx_msg void OnPathpadAddPath();
	afx_msg void OnPathpadInsertNode();
	afx_msg void OnPathpadDeleteNode();
	afx_msg void OnPathpadMoveForward();
	afx_msg void OnPathpadMoveBackward();
	afx_msg void OnPathpadMoveLeft();
	afx_msg void OnPathpadMoveRight();
	afx_msg void OnSelendokPathpadPulldown();
	afx_msg void OnPathpadCenterView();
	afx_msg void OnPathpadMoveDown();
	afx_msg void OnPathpadMoveUp();
	afx_msg void OnPathpadPrevNode();
	afx_msg void OnPathpadNextNode();
	afx_msg void OnDeletePath();
	afx_msg void OnShowNodesCheck();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnPathMoveObjectButton();
	afx_msg void OnPathReorientViewerButton();
	afx_msg void OnKillfocusPathIncText();
	afx_msg void OnPaint();
	afx_msg void OnPathMoveNodeToCurrentObject();
	afx_msg void OnPathMoveObjNoOrient();
	afx_msg void OnPathVerifyButton();
	afx_msg void OnPathpadCurNodeRoom();
	afx_msg void OnAinAutoEdgeNode();
	afx_msg void OnAinBack();
	afx_msg void OnAinCreateNodeAtViewer();
	afx_msg void OnAinCreateNodes();
	afx_msg void OnSelendokAinCurrentNode();
	afx_msg void OnAinDeleteEdge();
	afx_msg void OnAinDeleteNode();
	afx_msg void OnAinDown();
	afx_msg void OnAinForward();
	afx_msg void OnAinInsertNodeOnEdge();
	afx_msg void OnAinLeft();
	afx_msg void OnAinMakeEdge();
	afx_msg void OnKillfocusAinMoveAmount();
	afx_msg void OnAinMoveNodeToViewer();
	afx_msg void OnAinMpns();
	afx_msg void OnSelendokAinOtherNode();
	afx_msg void OnAinResetRoomNodes();
	afx_msg void OnAinRight();
	afx_msg void OnAinSelNodeEdges();
	afx_msg void OnAinSelRoom();
	afx_msg void OnAinUp();
	afx_msg void OnAinSelRoomAndNextRooms();
	afx_msg void OnAinVerifyLevel();
	afx_msg void OnAinVerifyRoom();
	afx_msg void OnAinViewAllNodes();
	afx_msg void OnAinViewNone();
	afx_msg void OnAinViewRoom();
	afx_msg void OnAinViewRoomAndNextRooms();
	afx_msg void OnSetfocusAinCurrentNode();
	afx_msg void OnSetfocusAinOtherNode();
	afx_msg void OnIenavButton();
	afx_msg void OnMvtnButton();
	afx_msg void OnAerButton();
	afx_msg void OnPathOrientNodeToViewerButton();
	afx_msg void OnOntvgpButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
