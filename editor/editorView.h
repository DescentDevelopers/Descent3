/*
 * $Logfile: /DescentIII/Main/editor/editorView.h $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:57:37 $
 * $Author: kevinb $
 *
 * CEditorView containss GrWnds and other editor views.
 *
 * $Log: not supported by cvs2svn $
 * 
 * 48    10/14/99 4:21p Matt
 * Added code to remove duplicate faces from a room.
 * 
 * 47    10/07/99 3:12p Matt
 * Added a function to flip the view 180 degrees.
 * 
 * 46    9/07/99 12:10p Matt
 * Added a function to propagate a texture to all adjacent coplanar faces
 * in a room.
 * 
 * 45    8/24/99 4:24p Matt
 * Added a function to rotate a placed room by 45 degrees.
 * 
 * 44    5/08/99 6:39p Matt
 * Added a function to delete a face and all faces connected to it.
 * 
 * 43    4/30/99 6:52p Matt
 * Added a function to merge an object's geometry into a room.
 * 
 * 42    4/28/99 12:51a Matt
 * Added some funtions to edit face geometry.
 * 
 * 41    4/19/99 12:10a Matt
 * Added a menu item to delete a vertex from a face.
 * 
 * 40    4/06/99 10:23a Matt
 * 
 * 39    3/31/99 12:58p Matt
 * Added snap-point-to-face
 * 
 * 38    3/29/99 6:46p Matt
 * Added menu item to fix/remove degenerate faces
 * 
 * 37    3/23/99 5:12p Matt
 * Added function to combine rooms.
 * 
 * 36    3/01/99 10:39p Matt
 * Added menu options to set & clear room center points
 * 
 * 35    2/28/99 6:32p Matt
 * Disable z-buffer button if OpenGL is being used in the editor.
 * 
 * 34    1/15/99 10:50a Matt
 * Added rename room to Room menu & added shortcut key
 * 
 * 33    12/23/98 10:53a Matt
 * Added functions to create a face
 * 
 * 32    10/03/98 11:22p Matt
 * Added system to seperately control outline mode for mine, terrain, sky,
 * & objects
 * 
 * 31    10/03/98 8:31p Matt
 * Added Join Rooms Exact function.
 * 
 * 30    9/09/98 4:07p Matt
 * Added Smooth Bridge function
 * 
 * 29    9/08/98 6:12p Matt
 * Added code to check for and fix duplicate and unused points.
 * 
 * 28    9/08/98 12:06p Jason
 * added ReInit openGL button
 * 
 * 27    9/07/98 10:58p Matt
 * Added snap point to point.
 * 
 * 26    9/04/98 3:34p Matt
 * Added groovy vertex snap code
 * 
 * 25    6/25/98 7:15p Matt
 * Added a function to delete a pair of portals.
 * 
 * 24    6/08/98 12:28p Matt
 * Added a function to triangulate a face
 * 
 * 23    5/22/98 4:47p Matt
 * Added menu item to propagate a texture to all the faces in a room.
 * 
 * 22    4/27/98 6:41p Matt
 * Added code to join all adjacent faces between two rooms
 * 
 * 21    2/16/98 1:27p Matt
 * Added function to snap the placed room to a vertex/edge on the base
 * room/face
 * 
 * 20    2/11/98 12:39p Matt
 * Implemented Delete Face menu item.
 * 
 * 19    1/29/98 2:15p Samir
 * Implemented ObjectListModeless and Toolbar button.
 * 
 * 18    1/27/98 6:15p Samir
 * Added Object toolbar button.
 * 
 * 17    1/20/98 4:08p Matt
 * Added function to swap current and marked room:face.  Added some
 * addition error checking to bridge function.
 * 
 * 16    11/05/97 7:13p Matt
 * Added join rooms function
 * 
 * 15    10/03/97 3:37p Matt
 * Added menu item to place a building on the terrain
 * 
 * 14    9/24/97 3:22p Matt
 * Added Drop Room function
 * 
 * 13    9/17/97 11:21a Matt
 * Ripped out segment code
 * 
 * 12    9/16/97 4:09p Jason
 * implemented software zbuffer
 * 
 * 11    9/14/97 11:07p Matt
 * Fixed up Room menu
 * 
 * 10    8/21/97 6:00p Matt
 * Added delete current room function
 * 
 * 9     8/19/97 5:58p Samir
 * OLE additions
 * 
 * 8     8/19/97 10:54a Matt
 * Added remove placed room option
 * 
 * 7     8/18/97 6:59p Matt
 * Implemented Place Room/Attach room system
 * 
 * 6     8/04/97 12:52p Matt
 * Added hooks for mark & bridge
 * 
 * 5     8/01/97 6:10p Matt
 * Added hooks for attach room
 * 
 * 4     7/28/97 11:29a Samir
 * Fixed editor->game goofiness.  Isolated GrWnd creation and destruction
 * to a couple of functions.
 * 
 * 3     7/22/97 10:33a Matt
 * Added functions for new room menu
 * 
 * 18    6/30/97 1:29p Jason
 * added netherspace stuff
 * 
 * 17    6/03/97 4:55p Mark
 * 
 * 16    5/27/97 3:30p Matt
 * Added editor button to toggle lighting
 * 
 * 15    4/03/97 1:17p Matt
 * Took out support for toolbar movement mode toggle button, now that that
 * function has been moved to the object keypad.
 * 
 * 14    4/02/97 8:07p Matt
 * Added variable & button to toggle object move state
 * 
 * 13    3/07/97 4:18p Jason
 * implemented terrain functionality in the editor
 * 
 * 12    2/28/97 6:37p Matt
 * Added variable & toggle button for box selection mode
 * 
 * 11    2/11/97 1:42p Samir
 * Moved file opening and closing to editorDoc where it should be.
 * 
 * 10    2/10/97 5:40p Matt
 * Code to handle several File menu options
 * 
 * 9     2/07/97 7:41p Matt
 * Hooked in next & prev segment & side funcs
 * 
 * 8     1/23/97 4:09p Samir
 * Added stuff for toggling wireframe, texture windows, tiling or
 * cascading
 * 
 * 7     1/21/97 12:51p Samir
 * GrWnds created based on D3EditState info
 *
 * $NoKeywords: $
 */


// editorView.h : interface of the CEditorView class
//
/////////////////////////////////////////////////////////////////////////////

#ifndef C_EDITORVIEW_H
#define C_EDITORVIEW_H

#include "WireframeGrWnd.h"
#include "TextureGrWnd.h"


class CEditorView : public CView
{
protected: // create from serialization only
	CEditorView();
	DECLARE_DYNCREATE(CEditorView)

// Attributes
public:
	int SafetyCheck();
	CEditorDoc* GetDocument();

// Operations
public:
	void ActivateView();
	void DeactivateView();
	bool SetupPopup(CMenu *popup);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditorView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CEditorView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CEditorView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg void OnButtonOutline();
	afx_msg void OnUpdateButtonOutline(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewTexturemine(CCmdUI* pCmdUI);
	afx_msg void OnViewTexturemine();
	afx_msg void OnUpdateViewWireframemine(CCmdUI* pCmdUI);
	afx_msg void OnViewWireframemine();
	afx_msg void OnParentNotify(UINT message, LPARAM lParam);
	afx_msg void OnWindowTile();
	afx_msg void OnWindowCascade();
	afx_msg void OnButtonWindowSelection();
	afx_msg void OnUpdateButtonWindowSelection(CCmdUI* pCmdUI);
	afx_msg void OnButtonLighting();
	afx_msg void OnUpdateButtonLighting(CCmdUI* pCmdUI);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnRoomAdd();
	afx_msg void OnRoomGrabTexture();
	afx_msg void OnRoomNextFace();
	afx_msg void OnRoomPreviousFace();
	afx_msg void OnRoomSelectByNumber();
	afx_msg void OnRoomAttachRoom();
	afx_msg void OnRoomBuildBridge();
	afx_msg void OnRoomMark();
	afx_msg void OnRoomPlaceRoom();
	afx_msg void OnUpdateRoomAttachRoom(CCmdUI* pCmdUI);
	afx_msg void OnRoomUnPlaceRoom();
	afx_msg void OnUpdateRoomUnPlaceRoom(CCmdUI* pCmdUI);
	afx_msg void OnCancelEditSrvr();
	afx_msg void OnRoomDelete();
	afx_msg void OnRoomSelectFaceByNumber();
	afx_msg void OnRoomSaveCurrentRoom();
	afx_msg void OnZbutton();
	afx_msg void OnUpdateZbutton(CCmdUI* pCmdUI);
	afx_msg void OnRoomDropRoom();
	afx_msg void OnRoomPlaceTerrainRoom();
	afx_msg void OnRoomJoinRooms();
	afx_msg void OnRoomSwapMarkedAndCurrentRoomFace();
	afx_msg void OnRoomDeleteFace();
	afx_msg void OnRoomSnapPlacedRoom();
	afx_msg void OnUpdateRoomSnapPlacedRoom(CCmdUI* pCmdUI);
	afx_msg void OnRoomJoinAdjacentFaces();
	afx_msg void OnRoomPropagateToAll();
	afx_msg void OnRoomSplitFace();
	afx_msg void OnRoomDeletePortal();
	afx_msg void OnRoomSnapPointToEdge();
	afx_msg void OnRoomUndoSnap();
	afx_msg void OnUpdateRoomUndoSnap(CCmdUI* pCmdUI);
	afx_msg void OnRoomSnapPointToPoint();
	afx_msg void OnReinitOpengl();
	afx_msg void OnUpdateReinitOpengl(CCmdUI* pCmdUI);
	afx_msg void OnFileRemoveExtraPoints();
	afx_msg void OnRoomBuildSmoothBridge();
	afx_msg void OnRoomJoinRoomsExact();
	afx_msg void OnRoomStartNewFace();
	afx_msg void OnRoomFinishNewFace();
	afx_msg void OnRoomAddVertToNewFace();
	afx_msg void OnRoomRenameRoom();
	afx_msg void OnRoomSetCenterFromViewer();
	afx_msg void OnRoomClearCenterPoint();
	afx_msg void OnUpdateRoomClearCenterPoint(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRoomSetCenterFromViewer(CCmdUI* pCmdUI);
	afx_msg void OnRoomCombine();
	afx_msg void OnFileFixDegenerateFaces();
	afx_msg void OnRoomSnapPointToFace();
	afx_msg void OnRoomLinkToNewExternal();
	afx_msg void OnRoomDeleteVert();
	afx_msg void OnRoomFaceAddVertToEdge();
	afx_msg void OnRoomFaceMoveVertOnEdge();
	afx_msg void OnRoomFaceSplitFace();
	afx_msg void OnRoomFaceDeleteVertOnEdge();
	afx_msg void OnRoomMergeObjectIntoRoom();
	afx_msg void OnRoomDeleteConnectedFaces();
	afx_msg void OnRoomRotatePlacedRoom45Degrees();
	afx_msg void OnRoomPropagateToAdjacentCoplanarFaces();
	afx_msg void OnViewFlip();
	afx_msg void OnFileRemoveDuplicateFacesFromCurrentRoom();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

//	afx_msg void OnFileSave();
//	afx_msg void OnFileSaveAs();


private:
	CWireframeGrWnd m_grwndWireframe;
	CTextureGrWnd m_grwndTexture;

	BOOL m_grwndCreated;
	BOOL m_grwndActive;

private:
	void KillChildViews();
	void CreateChildViews();
};

#ifndef _DEBUG  // debug version in editorView.cpp
inline CEditorDoc* CEditorView::GetDocument()
   { return (CEditorDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
#endif
