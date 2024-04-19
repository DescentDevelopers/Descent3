/*
 * $Logfile: /DescentIII/Main/editor/MainFrm.h $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:57:38 $
 * $Author: kevinb $
 *
 * The Editor Main Frame Window
 *
 * $Log: not supported by cvs2svn $
 * 
 * 59    8/18/99 11:29a Matt
 * Added shortcuts for next vertex and add vert to face.
 * 
 * 58    8/17/99 4:09p Matt
 * Added some accelerator keys, and a function to copy face light
 * multiples.
 * 
 * 57    5/08/99 1:39a Matt
 * Added a function to delete all objects of a certain type, and support
 * for placing and attaching groups to the terrain.
 * 
 * 56    4/29/99 11:51p Matt
 * Improved viewer move functions.
 * 
 * 55    4/16/99 3:54p Nate
 * Added Viewer Properties Dialog
 * 
 * 54    4/12/99 12:50p Matt
 * Added function to dump goal text to a file.
 * 
 * 53    4/05/99 10:54a Matt
 * Added auto-waypoint system
 * 
 * 52    3/18/99 8:41p Jeff
 * created orphan hunter dialog
 * 
 * 51    2/22/99 11:39p Matt
 * Deleted debris editor menu item, since it' no longer needed.
 * 
 * 50    2/21/99 4:20p Matt
 * Added SoundSource objects (and reformatted parts of the object header
 * files).
 * 
 * 49    2/05/99 1:29p Matt
 * Added a function to show the viewer's forward vector.  (Also checked
 * select-object-by-number to use common code to print out the selected
 * object info.)
 * 
 * 48    2/03/99 1:58a Matt
 * Added a system to show all pages locked.
 * 
 * 47    2/01/99 11:36p Matt
 * Added menu functions to select an object by number and move the viewer
 * to the current object.
 * 
 * 46    1/11/99 3:34p Jeff
 * added checks for when going editor->game to see if scripts are out of
 * date, if so give the option of breaking out.  Add some options to mass
 * script compiler, along with a toolbar shortcut.
 * 
 * 45    1/08/99 2:56p Samir
 * Ripped out OSIRIS1.
 * 
 * 44    12/23/98 4:01p Nate
 * Added code create Dallas as a modeless dialog
 * 
 * 43    12/11/98 6:48p Nate
 * Added support for starting D.A.L.L.A.S. from the main menu
 * 
 * 42    12/11/98 5:50p Jeff
 * implemented and added changes regarding Level&Scripting manage system
 * and compiler interface
 * 
 * 41    12/01/98 11:17p Matt
 * Added menu option to disable drawing objects in the wireframe view
 * 
 * 40    10/06/98 12:39p Nate
 * Added TableFileFilter menu option.
 * 
 * 39    10/04/98 1:25p Matt
 * 
 * 38    9/09/98 12:48p Samir
 * added script localizer.
 * 
 * 37    9/03/98 5:29p Matt
 * Added code to fix cracks in levels.
 * 
 * 36    9/02/98 2:55p Jeff
 * initial creation of briefing editor
 * 
 * 35    8/27/98 6:42p Chris
 * Added the goal keypad and the start of the matcen keypad
 * 
 * 34    8/20/98 12:35p Matt
 * Added nice editing for ambient sound patterns
 * 
 * 33    8/10/98 12:14p Nate
 * Added Table File Edit dialog.
 * 
 * 32    5/01/98 3:57p Matt
 * Added function to place a camera in the center of the current face.
 * 
 * 31    4/21/98 2:40p Matt
 * Added option to show level stats
 * 
 * 30    4/14/98 7:50p Matt
 * Added system to keep info for each level
 * 
 * 29    3/27/98 6:05p Samir
 * Added key handler for main frame.
 * 
 * 28    1/30/98 6:07p Matt
 * Added code and menu items to deal with camera objects
 * 
 * 27    1/29/98 2:15p Samir
 * Implemented ObjectListModeless and Toolbar button.
 * 
 * 26    1/21/98 1:08p Matt
 * Made SetViewMode() not take new_viewer parm, since it's not needed
 * after my revamp of the editor view system.
 * 
 * 25    1/21/98 12:25p Matt
 * Added needed include
 * 
 * 24    10/22/97 4:30p Chris
 * We can now slew between the mine and the terrain
 * 
 * 23    9/23/97 10:38a Jason
 * Took out OnTerrain
 * 
 * 22    9/17/97 12:58p Samir
 * Got rid of Curve Keypad.
 * 
 * 21    9/14/97 9:47p Matt
 * Made CEditorView a friend class
 * 
 * 20    9/09/97 12:23p Samir
 * Added a menu item to go to level properties dialog.
 * 
 * 19    9/02/97 6:41p Matt
 * Added code for group paste
 * 
 * 18    8/29/97 2:49p Samir
 * Nuked SegmentKeypad.
 * 
 * 17    8/15/97 6:32p Samir
 * Added ScriptWizard!
 * 
 * 16    8/12/97 5:47p Matt
 * Added menu items to edit debris, clutter, & buildings
 * Increased number of items in objpage dropdown box
 * 
 * 15    8/11/97 1:53p Matt
 * Ripped out robot & powerup pages, and added generic page
 * 
 * 14    8/08/97 3:17p Matt
 * Added testing calls to new generic page dialog
 * 
 * 13    7/29/97 2:19p Jason
 * added filepage editor to mainframe
 * 
 * 12    7/29/97 2:11p Jeff
 * Added compile on exit for Osiris
 * 
 * 11    7/28/97 5:33p Matt
 * Added function to change the view mode
 * 
 * 10    7/26/97 8:48p Jeff
 * fixed up calls to script editor.  Added DefaultExternalEditor global
 * variable
 * 
 * 9     7/25/97 6:16p Samir
 * Fixed the tile-cascade switching/default madness.
 * 
 * 8     7/24/97 3:00p Matt
 * Save/load game & editor settings from/to the registry, and added code
 * to display current keypad.
 * 
 * 7     7/18/97 5:17p Jeff
 * 
 * 6     7/16/97 5:20p Jason
 * implemented selection of moons with mouse click
 * 
 * 51    6/27/97 7:21p Matt
 * Added function to move player 0 to current segment
 * 
 * 50    6/27/97 12:20p Samir
 * Added room keypad dialog.
 * 
 * 49    6/18/97 11:57a Jason
 * added terrain functions for mark
 * 
 * 48    6/11/97 11:03a Jason
 * added reorder pages menu item
 * 
 * 47    6/05/97 3:37p Samir
 * Added megacell keypad.
 * 
 * 46    6/05/97 2:52p Jason
 * added megacell functions
 * 
 * 45    6/03/97 4:55p Mark
 * 
 * 44    5/13/97 4:38p Samir
 * Added OnIdle function
 * 
 * 43    5/05/97 3:37p Matt
 * 
 * 42    4/17/97 4:03p Samir
 * Apply Tmap 1 and Tmap2 implemented, but no Tmap2 rendering is occurring
 * yet.
 * 
 * 41    4/16/97 6:11p Matt
 * Added code to divide a segment into two
 * 
 * 40    4/08/97 10:42a Jason
 * implemented path following interface
 * 
 * 39    4/04/97 3:15p Samir
 * Added paths keypad, removed player keypad since it's joined with object
 * keypad.
 * 
 * 38    4/03/97 1:20p Jason
 * changes to get the sound page system playing
 * 
 * 37    4/02/97 10:56a Matt
 * Changed "Other" menu to "Object", and removed obsolete set-player-start
 * option
 * 
 * 36    3/28/97 12:20p Jason
 * added terrain dialog stuff, plus bitmap memory indicator
 * 
 * 35    3/26/97 4:37p Samir
 * Splash screen up and running.
 * 
 * 34    3/21/97 12:30p Samir
 * Added more keypad functionality and NET pane.
 * 
 * 33    3/20/97 5:01p Matt
 * Removed obsolete select-from-box code
 * 
 * 32    3/20/97 11:55a Jason
 * changes for terrain editing/drawing
 * 
 * 31    3/17/97 1:32p Matt
 * Took out redundant and unneeded handlers for View menu items.
 * 
 * 30    3/14/97 6:53p Samir
 * Took out unused functions and added delete segment function.
 * 
 * 29    3/14/97 12:50p Samir
 * Added marked segment pane
 * 
 * 28    3/13/97 7:02p Samir
 * Added current segment:side pane.
 * 
 * 27    3/12/97 3:25p Matt
 * Added funcs for cut, copy, paste, & delete, and to save and load
 * groups.
 * 
 * 26    3/05/97 11:49a Samir
 * Added doorway and terrain keypad tabs.
 * 
 * 25    2/28/97 6:40p Matt
 * Call code to call box selection code
 * 
 * 24    2/28/97 4:00p Matt
 * Added code to handle the list of selected segments
 * 
 * 23    2/28/97 2:37p Mark
 * 
 * 22    2/28/97 11:09a Samir
 * Fixed Play in 640x480 menu item.
 * 
 * 21    2/27/97 5:29p Mark
 * 
 * 20    2/26/97 3:35p Mark
 * 
 * 19    2/20/97 5:10p Samir
 * Added Tmap1 and Tmap2 apply accels.  Tmap1 works by doing a CTRL-1.
 * 
 * 18    2/20/97 9:57a Matt
 * Added Add Segment 
 * 
 * 17    2/19/97 2:15p Samir
 * Added keypad info.
 * 
 * 16    2/18/97 7:04p Samir
 * Added print status bar message functions
 * 
 * 15    2/12/97 2:33p Samir
 * Added hog dialog.
 * 
 * 14    2/06/97 2:46p Jason
 * added import bitmap function
 * 
 * 13    2/05/97 12:16p Samir
 * Added menu item for preferences.
 * 
 * 12    2/04/97 5:18p Samir
 * Added Leaving editor... option in File Menu
 * 
 * 11    1/23/97 4:35p Samir
 * Added floating keypad toggle and support
 * 
 * 10    1/21/97 12:45p Samir
 * Toggle to get rid of keypad
 *
 * $NoKeywords: $
 */


#ifndef M_MAINFRM_H
#define M_MAINFRM_H

#include "stdafx.h"

class CTextureDialog;
class CObjectDialog;
class CLightingDialog;
class CPathPadDialog;
class CTriggerDialog;
class CDoorwayDialog;
class CRoomKeypadDialog;
class CFloatingKeypadDialog;
class CKeypadDialog;
class CTerrainDialog;
class CMegacellKeypad;
class CD3Splash;
class CObjectListModeless;
class CEditorDoc;
class levelkeypad;
class matcenkeypad;

class CMainFrame : public CFrameWnd
{
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void ActivateFrame(int nCmdShow = -1);
	protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	//}}AFX_VIRTUAL

public:
	void DockKeypad(BOOL dock);
	void SetStatusMessage(char *str);
	void OnIdle();

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;
	CDialogBar	m_wndKeypadBar;

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnToolsWorldTextures();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnToolsWorldWeapons();
	afx_msg void OnToolsWorldObjectsDoor();
	afx_msg void OnToolsWorldObjectsLights();
	afx_msg void OnToolsWorldObjectsPlayer();
	afx_msg void OnToolsWorldObjectsPowerups();
	afx_msg void OnToolsWorldObjectsRobots();
	afx_msg void OnSubeditorsFont();
	afx_msg void OnActivateApp(BOOL bActive, HTASK hTask);
	afx_msg void OnFilePlayin640x480();
	afx_msg void OnUpdateViewKeypadToggle(CCmdUI* pCmdUI);
	afx_msg void OnViewKeypadToggle();
	afx_msg void OnFileLeaveEditor();
	afx_msg void OnFilePreferences();
	afx_msg void OnImportBitmap();
	afx_msg void OnSubeditorsHogmaker();
	afx_msg void OnNumpad0();
	afx_msg void OnNumpad1();
	afx_msg void OnNumpad2();
	afx_msg void OnNumpad3();
	afx_msg void OnNumpad4();
	afx_msg void OnNumpad5();
	afx_msg void OnNumpad6();
	afx_msg void OnNumpad7();
	afx_msg void OnNumpad8();
	afx_msg void OnNumpad9();
	afx_msg void OnNumpaddel();
	afx_msg void OnNumpadminus();
	afx_msg void OnNumpadadd();
	afx_msg void OnViewCenterOnMine();
	afx_msg void OnViewMoveCameraToSelectedRoom();
	afx_msg void OnViewCenterOnCube();
	afx_msg void OnViewCenterOnObject();
	afx_msg void OnGroupCreateGroup();
	afx_msg void OnGroupLoadGroup();
	afx_msg void OnGroupMirrorGroup();
	afx_msg void OnGroupSaveGroup();
	afx_msg void OnEditAddSelect();
	afx_msg void OnEditClearSelected();
	afx_msg void OnEditRemoveSelect();
	afx_msg void OnEditSelectAttached();
	afx_msg void OnEditSaveScrap();
	afx_msg void OnEditLoadScrap();
	afx_msg void OnEditCopy();
	afx_msg void OnEditCut();
	afx_msg void OnEditDelete();
	afx_msg void OnSegSidePane(CCmdUI *pCmdUI);
	afx_msg void OnNetPane(CCmdUI *pCmdUI);
	afx_msg void OnBMMemPane(CCmdUI *pCmdUI);
	afx_msg void OnEnginePane(CCmdUI *pCmdUI);
	afx_msg void OnViewerPane(CCmdUI *pCmdUI);
	afx_msg void OnNumpadDiv();
	afx_msg void OnNumpadEnter();
	afx_msg void OnNumpadLock();
	afx_msg void OnNumpadMul();
	afx_msg void OnObjectDeleteObject();
	afx_msg void OnObjectPlaceObject();
	afx_msg void OnToolsWorldObjectsSound();
	afx_msg void OnTestTest1();
	afx_msg void OnTestTest2();
	afx_msg void OnTestTest3();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnEditorsMegacells();
	afx_msg void OnReorderPages();
	afx_msg void OnFileImportRoom();
	afx_msg void OnObjectMovePlayer();
	afx_msg void OnRoomView();
	afx_msg void OnTerrainView();
	afx_msg void OnMineView();
	afx_msg void OnUpdateMineView(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRoomView(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTerrainView(CCmdUI* pCmdUI);
	afx_msg void OnViewNewviewer();
	afx_msg void OnViewNextviewer();
	afx_msg void OnViewDeleteviewer();
	afx_msg void OnD3help();
	afx_msg void OnHotspotTga();
	afx_msg void OnEditorsFiles();
	afx_msg void OnToolsWorldObjectsClutter();
	afx_msg void OnToolsWorldObjectsBuildings();
	afx_msg void OnEditPlace();
	afx_msg void OnEditAttach();
	afx_msg void OnUpdateEditAttach(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditSaveScrap(CCmdUI* pCmdUI);
	afx_msg void OnFileLevelProps();
	afx_msg void OnObjbutton();
	afx_msg void OnUpdateObjbutton(CCmdUI* pCmdUI);
	afx_msg void OnNcDestroy();
	afx_msg void OnObjectSetCameraFromViewer();
	afx_msg void OnObjectPlaceCameraAtViewer();
	afx_msg void OnObjectSetViewerFromCamera();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnFileLevelInfo();
	afx_msg void OnFileStats();
	afx_msg void OnFileVerifyLevel();
	afx_msg void OnObjectPlaceCameraAtCurrentFace();
	afx_msg void OnSubEditorsTableFileEdit();
	afx_msg void OnEditorsAmbientSounds();
	afx_msg void OnBriefingEditor();
	afx_msg void OnScriptLevelInterface();
	afx_msg void OnFileFixCracks();
	afx_msg void OnViewResetViewRadius();
	afx_msg void OnSubeditorsTablefilefilter();
	afx_msg void OnViewShowObjectsInWireframeView();
	afx_msg void OnUpdateViewShowObjectsInWireframeView(CCmdUI* pCmdUI);
	afx_msg void OnEditorsDallas();
	afx_msg void OnUpdateOsiriscompile(CCmdUI* pCmdUI);
	afx_msg void OnOsiriscompile();
	afx_msg void OnViewMoveCameraToCurrentObject();
	afx_msg void OnObjectSelectByNumber();
	afx_msg void OnShowAllCheckedOut();
	afx_msg void OnViewShowViewerForwardVector();
	afx_msg void OnObjectPlaceSoundSourceAtViewer();
	afx_msg void OnOrphanhunter();
	afx_msg void OnObjectPlaceWaypointAtViewer();
	afx_msg void OnFileSaveGoalText();
	afx_msg void OnViewViewprop();
	afx_msg void OnUpdateViewViewprop(CCmdUI* pCmdUI);
	afx_msg void OnViewMoveCameraToSelectedFace();
	afx_msg void OnEditPlaceTerrain();
	afx_msg void OnAccelCopyLightMultiple();
	afx_msg void OnAccelSlidedown();
	afx_msg void OnAccelSlideleft();
	afx_msg void OnAccelSlideright();
	afx_msg void OnAccelSlideup();
	afx_msg void OnAccelNextVertex();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void InitKeypadDialog();			// Initialized the keypad tab dialog
	void DoKeypadTabNotify(NMHDR *nmhdr);
	void ShowCurrentKeypad();
	void ResizeKeypad();

	void InitGraphicsSystem();
	void DisplaySplashScreen();
	void RemoveSplashScreen();

private:
	CFloatingKeypadDialog *m_FloatingKeypadDialog;

	CD3Splash *m_D3Splash;						// splash screen dialog

	CKeypadDialog *m_KeypadTabDialog;			// modeless dialog in tab control 
	CTextureDialog *m_TextureDialog;
	CObjectDialog *m_ObjectDialog;
	CLightingDialog *m_LightingDialog;
	CPathPadDialog *m_PathPadDialog;
	CTriggerDialog *m_TriggerDialog;
	CDoorwayDialog *m_DoorwayDialog;
	CTerrainDialog *m_TerrainDialog;
	CMegacellKeypad *m_MegacellKeypad;
	CRoomKeypadDialog *m_RoomDialog;
	levelkeypad *m_LevelDialog;
	matcenkeypad *m_MatcenDialog;

	CObjectListModeless *m_ObjModeless;

	CTabCtrl *m_KeypadTabCtrl;

	BOOL m_ViewActivated;

	friend class CTextureGrWnd;
	friend class CEditorView;
	friend class CEditorDoc;
};

/////////////////////////////////////////////////////////////////////////////

//Set the editor view mode
void SetViewMode(int view_mode);

#endif
