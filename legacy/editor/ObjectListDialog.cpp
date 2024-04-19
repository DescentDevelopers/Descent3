// ObjectListDialog.cpp : implementation file
//

#include "stdafx.h"
#include "editor.h"
#include "ObjectListDialog.h"
#include "HObject.h"
#include "MainFrm.h"
																				 
#include "object.h"
#include "objinfo.h"
#include "pserror.h"

#include <string.h>
#include <stdio.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CObjectListDialog dialog


CObjectListDialog::CObjectListDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CObjectListDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CObjectListDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_Leafs[CLIPBOARD_LEAF] = NULL;
	m_Leafs[POWERUP_LEAF] = NULL;
	m_Leafs[ROBOT_LEAF] = NULL;
	m_Leafs[BUILDING_LEAF] = NULL;
	m_Leafs[DOOR_LEAF] = NULL;
	m_Leafs[CLUTTER_LEAF] = NULL;
	m_Leafs[PLAYER_LEAF] = NULL;
	m_Leafs[CAMERA_LEAF] = NULL;
	m_Leafs[SOUNDSOURCE_LEAF] = NULL;
	m_Leafs[MISC_LEAF] = NULL;

	nclipboard = 0;
	npowerup = 0;
	nrobot = 0;
	nbuilding = 0;
	nclutter = 0;
	ndoor = 0;
	nplayer = 0;
	ncamera = 0;
	nsndsrc = 0;
	nmisc = 0;
}


void CObjectListDialog::MakeInfoStr(object *obj,  char *str)
{
	char tempstr[128];

	if (obj->type == OBJ_POWERUP || obj->type == OBJ_ROBOT || obj->type == OBJ_BUILDING || obj->type == OBJ_CLUTTER) 
		sprintf(tempstr, "%d-(0x%x)[%s](%s) ", OBJNUM(obj), obj->handle, Object_info[obj->id].name, (obj->name)?obj->name:"No Name Given");
	else {
		char *obj_type_name = (obj->type >=0 && obj->type < MAX_OBJECT_TYPES) ? Object_type_names[obj->type] : "Unnamed type";
		sprintf(tempstr, "%d-(0x%x)[%s](%s) ", OBJNUM(obj), obj->handle, obj_type_name, (obj->name)?obj->name:"No Name Given");
	}

	strcpy(str, tempstr);

	if (OBJECT_OUTSIDE(obj)) 
		sprintf(tempstr, "cell %d ", CELLNUM(obj->roomnum));
	else
		sprintf(tempstr, "room %d ", obj->roomnum);
	
	strcat(str, tempstr);
}


void CObjectListDialog::Refresh()
{
	int i;

//	Generate object tree
//	categorize them here.
	CTreeCtrl *tree = (CTreeCtrl *)GetDlgItem(IDC_OBJTREE);

	nclipboard = 0;
	npowerup = 0;
	nrobot = 0;
	nbuilding = 0;
	nclutter = 0;
	ndoor = 0;
	nplayer = 0;
	ncamera = 0;
	nsndsrc = 0;
	nmisc = 0;

	for (i = 0; i <= Highest_object_index; i++)
	{
		if (!(Objects[i].flags & OF_DEAD)) {
			if (Objects[i].type == OBJ_POWERUP)
				npowerup++;
			else if (Objects[i].type == OBJ_ROBOT)
				nrobot++;
			else if (Objects[i].type == OBJ_BUILDING)
				nbuilding++;
			else if (Objects[i].type == OBJ_CLUTTER)
				nclutter++;
			else if (Objects[i].type == OBJ_DOOR)
				ndoor++;
			else if (Objects[i].type == OBJ_PLAYER)
				nplayer++;
			else if (Objects[i].type == OBJ_CAMERA)
				ncamera++;
			else if (Objects[i].type == OBJ_SOUNDSOURCE) 
				nsndsrc++;
			else if (Objects[i].type != OBJ_VIEWER) 
				nmisc++;
		}
	}

	ClearTree();

	if (nclipboard)
		m_Leafs[CLIPBOARD_LEAF] = new CObjectListDialog::tObjNode[nclipboard];
	if (npowerup)
		m_Leafs[POWERUP_LEAF] = new CObjectListDialog::tObjNode[npowerup];
	if (nrobot) 
		m_Leafs[ROBOT_LEAF] = new CObjectListDialog::tObjNode[nrobot];
	if (nbuilding)
		m_Leafs[BUILDING_LEAF] = new CObjectListDialog::tObjNode[nbuilding];
	if (nclutter)
		m_Leafs[CLUTTER_LEAF] = new CObjectListDialog::tObjNode[nclutter];
	if (nplayer)
		m_Leafs[PLAYER_LEAF] = new CObjectListDialog::tObjNode[nplayer];
	if (ndoor)
		m_Leafs[DOOR_LEAF] = new CObjectListDialog::tObjNode[ndoor];
	if (ncamera)
		m_Leafs[CAMERA_LEAF] = new CObjectListDialog::tObjNode[ncamera];
	if (nsndsrc)
		m_Leafs[SOUNDSOURCE_LEAF] = new CObjectListDialog::tObjNode[nsndsrc];
	if (nmisc)
		m_Leafs[MISC_LEAF] = new CObjectListDialog::tObjNode[nmisc];

//	add sections to tree
	tree->DeleteAllItems();

	hClipboardTree = tree->InsertItem("Clipboard");
	hPowerupTree = tree->InsertItem("Powerups");
	hRobotTree = tree->InsertItem("Robots");
	hBuildingTree = tree->InsertItem("Buildings");
	hDoorTree = tree->InsertItem("Door");
	hClutterTree = tree->InsertItem("Clutter");
	hPlayerTree = tree->InsertItem("Players");
	hCameraTree = tree->InsertItem("Cameras");
	hSoundSourceTree = tree->InsertItem("Sound sources");
	hMiscTree = tree->InsertItem("Miscellaneous");

	nclipboard = 0;
	npowerup = 0;
	nrobot = 0;
	nbuilding = 0;
	nclutter = 0;
	ndoor = 0;
	nplayer = 0;
	ncamera = 0;
	nsndsrc = 0;
	nmisc = 0;

//	generate list
	for (i = 0; i <= Highest_object_index; i++)
	{
		if (!(Objects[i].flags & OF_DEAD)) {
			char str[256];

			MakeInfoStr(&Objects[i], str);

			if (Objects[i].type == OBJ_POWERUP) {
				m_Leafs[POWERUP_LEAF][npowerup].hItem = tree->InsertItem(str, hPowerupTree);
				m_Leafs[POWERUP_LEAF][npowerup++].handle = Objects[i].handle;
			}
			else if (Objects[i].type == OBJ_ROBOT) {
				m_Leafs[ROBOT_LEAF][nrobot].hItem = tree->InsertItem(str, hRobotTree);
				m_Leafs[ROBOT_LEAF][nrobot++].handle = Objects[i].handle;
			}
			else if (Objects[i].type == OBJ_BUILDING) {
				m_Leafs[BUILDING_LEAF][nbuilding].hItem = tree->InsertItem(str, hBuildingTree);
				m_Leafs[BUILDING_LEAF][nbuilding++].handle = Objects[i].handle;
			}
			else if (Objects[i].type == OBJ_CLUTTER) {
				m_Leafs[CLUTTER_LEAF][nclutter].hItem = tree->InsertItem(str, hClutterTree);
				m_Leafs[CLUTTER_LEAF][nclutter++].handle = Objects[i].handle;
			}
			else if (Objects[i].type == OBJ_PLAYER) {
				m_Leafs[PLAYER_LEAF][nplayer].hItem = tree->InsertItem(str, hPlayerTree);
				m_Leafs[PLAYER_LEAF][nplayer++].handle = Objects[i].handle;
			}
			else if (Objects[i].type == OBJ_DOOR) {
				m_Leafs[DOOR_LEAF][ndoor].hItem = tree->InsertItem(str, hDoorTree);
				m_Leafs[DOOR_LEAF][ndoor++].handle = Objects[i].handle;
			}
			else if (Objects[i].type == OBJ_CAMERA) {
				m_Leafs[CAMERA_LEAF][ncamera].hItem = tree->InsertItem(str, hCameraTree);
				m_Leafs[CAMERA_LEAF][ncamera++].handle = Objects[i].handle;
			}
			else if (Objects[i].type == OBJ_SOUNDSOURCE) {
				m_Leafs[SOUNDSOURCE_LEAF][nsndsrc].hItem = tree->InsertItem(str, hSoundSourceTree);
				m_Leafs[SOUNDSOURCE_LEAF][nsndsrc++].handle = Objects[i].handle;
			}
			else  if (Objects[i].type != OBJ_VIEWER) {
				m_Leafs[MISC_LEAF][nmisc].hItem = tree->InsertItem(str, hMiscTree);
				m_Leafs[MISC_LEAF][nmisc++].handle = Objects[i].handle;
			}
		}
	}
}


void CObjectListDialog::ClearTree()
{
	if (m_Leafs[CLIPBOARD_LEAF])
		delete[] m_Leafs[CLIPBOARD_LEAF];
	if (m_Leafs[POWERUP_LEAF])
		delete[] m_Leafs[POWERUP_LEAF];
	if (m_Leafs[ROBOT_LEAF])
		delete[] m_Leafs[ROBOT_LEAF];
	if (m_Leafs[BUILDING_LEAF])
		delete[] m_Leafs[BUILDING_LEAF];
	if (m_Leafs[CLUTTER_LEAF])
		delete[] m_Leafs[CLUTTER_LEAF];
	if (m_Leafs[DOOR_LEAF])
		delete[] m_Leafs[DOOR_LEAF];
	if (m_Leafs[PLAYER_LEAF])
		delete[] m_Leafs[PLAYER_LEAF];
	if (m_Leafs[CAMERA_LEAF])
		delete[] m_Leafs[CAMERA_LEAF];
	if (m_Leafs[SOUNDSOURCE_LEAF])
		delete[] m_Leafs[SOUNDSOURCE_LEAF];
	if (m_Leafs[MISC_LEAF])
		delete[] m_Leafs[MISC_LEAF];

	m_Leafs[CLIPBOARD_LEAF] = NULL;
	m_Leafs[POWERUP_LEAF] = NULL;
	m_Leafs[ROBOT_LEAF] = NULL;
	m_Leafs[BUILDING_LEAF] = NULL;
	m_Leafs[DOOR_LEAF] = NULL;
	m_Leafs[CLUTTER_LEAF] = NULL;
	m_Leafs[PLAYER_LEAF] = NULL;
	m_Leafs[CAMERA_LEAF] = NULL;
	m_Leafs[SOUNDSOURCE_LEAF] = NULL;
	m_Leafs[MISC_LEAF] = NULL;

	m_ObjectHandle = -1;
}


void CObjectListDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CObjectListDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


int CObjectListDialog::GetObjectSelected() const
{
	return m_ObjectHandle;
}


//	takes a tree item and returns the object handle by looking through all the ObjNodes.
int CObjectListDialog::GetObjHandleFromItem(HTREEITEM hItem)
{
	CTreeCtrl *tree = (CTreeCtrl *)GetDlgItem(IDC_OBJTREE);
	HTREEITEM hParent;
	tObjNode *leaf;
	int count, i;

	if (!(hParent = tree->GetParentItem(hItem))) 
		return -1;

//	check each leaf
	if (hParent == hClipboardTree) {
		leaf = m_Leafs[CLIPBOARD_LEAF];
		count = nclipboard;
	}
	else if (hParent == hRobotTree) {
		leaf = m_Leafs[ROBOT_LEAF];
		count = nrobot;
	}
	else if (hParent == hPowerupTree) {
		leaf = m_Leafs[POWERUP_LEAF];
		count = npowerup;
	}
	else if (hParent == hBuildingTree) {
		leaf = m_Leafs[BUILDING_LEAF];
		count = nbuilding;
	}
	else if (hParent == hDoorTree) {
		leaf = m_Leafs[DOOR_LEAF];
		count = ndoor;
	}
	else if (hParent == hClutterTree) {
		leaf = m_Leafs[CLUTTER_LEAF];
		count = nclutter;
	}
	else if (hParent == hPlayerTree) {
		leaf = m_Leafs[PLAYER_LEAF];
		count = nplayer;
	}
	else if (hParent == hCameraTree) {
		leaf = m_Leafs[CAMERA_LEAF];
		count = ncamera;
	}
	else if (hParent == hSoundSourceTree) {
		leaf = m_Leafs[SOUNDSOURCE_LEAF];
		count = nsndsrc;
	}
	else if (hParent == hMiscTree) {
		leaf = m_Leafs[MISC_LEAF];
		count = nmisc;
	}
	else {
		count = 0;
	}

	for (i = 0; i < count; i++, leaf++)
		if (leaf->hItem == hItem) 
			return leaf->handle;

//	mprintf((0, "ObjectListDialog: Unable to find handle in object tree!!\n"));

	return -1;
}


BEGIN_MESSAGE_MAP(CObjectListDialog, CDialog)
	//{{AFX_MSG_MAP(CObjectListDialog)
	ON_WM_DESTROY()
	ON_NOTIFY(TVN_SELCHANGED, IDC_OBJTREE, OnSelchangedObjtree)
	ON_BN_CLICKED(IDC_GOTO, OnGoto)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_CLEAR, OnClear)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CObjectListDialog message handlers

BOOL CObjectListDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

//	Generate object tree
//	categorize them here.
	Refresh();

	CTreeCtrl *tree = (CTreeCtrl *)GetDlgItem(IDC_OBJTREE);
	CWnd *btn1 = GetDlgItem(IDC_GOTO);
	CWnd *btn2 = GetDlgItem(IDC_DELETE);

	if (!tree->GetSelectedItem()) {
		btn1->EnableWindow(FALSE);
		btn2->EnableWindow(FALSE);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CObjectListDialog::OnDestroy() 
{
	CDialog::OnDestroy();

	ClearTree();
}


void CObjectListDialog::OnSelchangedObjtree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CWnd *btn1 = GetDlgItem(IDC_GOTO);
	CWnd *btn2 = GetDlgItem(IDC_DELETE);

	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	m_ObjectHandle = GetObjHandleFromItem(pNMTreeView->itemNew.hItem);

	if (m_ObjectHandle < 0) {
		btn1->EnableWindow(FALSE);
		btn2->EnableWindow(FALSE);
	}
	else {
		btn1->EnableWindow(TRUE);
		btn2->EnableWindow(TRUE);
	}

	*pResult = 0;
}


void CObjectListDialog::OnGoto() 
{
	int objhandle = GetObjectSelected();	
	object *obj = NULL;

	if (objhandle > -1) {
		obj = ObjGet(objhandle);
	}

	if (obj) {
		if (OBJECT_OUTSIDE(obj) && Editor_view_mode != VM_TERRAIN) {
			SetViewMode(VM_TERRAIN);
		}
		else if (!OBJECT_OUTSIDE(obj) && Editor_view_mode != VM_MINE) {
			SetViewMode(VM_MINE);
		}
		ObjSetPos(Viewer_object,&obj->pos,obj->roomnum,&obj->orient,false);
		EditorStatus("Viewer moved to object %d",OBJNUM(obj));
		Viewer_moved = 1;
	}
}


void CObjectListDialog::OnDelete() 
{
	int objhandle = GetObjectSelected();	
	object *obj = NULL;

	if (objhandle > -1) {
		obj = ObjGet(objhandle);
	}

	if (obj) {
		CTreeCtrl *tree = (CTreeCtrl *)GetDlgItem(IDC_OBJTREE);
		Cur_object_index = OBJNUM(obj);
		HObjectDelete();
		tree->DeleteItem(tree->GetSelectedItem());
	}
}


void CObjectListDialog::OnClear() 
{
//	delete all items.
	int i;

	if (OutrageMessageBox(MBOX_YESNO,"Will delete ALL objects except doors.  Are you sure?") != IDYES)
		return;

	for (i = 0; i <= Highest_object_index; i++)
	{
		if (&Objects[i] == Player_object) {
			continue;
		}
		if (&Objects[i] == Viewer_object) {
			continue;
		}
		if (Objects[i].type == OBJ_DOOR) {
			continue;
		}
		if (Objects[i].type != OBJ_NONE) 
			ObjDelete(i);
	}
	World_changed = 1;
	Cur_object_index = -1;
	
	Refresh();
}




/////////////////////////////////////////////////////////////////////////////
// CObjectListDialog dialog

CObjectListModeless::CObjectListModeless(CWnd* pParent /*=NULL*/)
	: CObjectListDialog(pParent)
{
	//{{AFX_DATA_INIT(CObjectListModeless)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_wndParent = pParent;
}


void CObjectListModeless::DoDataExchange(CDataExchange* pDX)
{
	CObjectListDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CObjectListModeless)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BOOL CObjectListModeless::Create()
{
	return CDialog::Create(IDD, m_wndParent);
}


BEGIN_MESSAGE_MAP(CObjectListModeless, CObjectListDialog)
	//{{AFX_MSG_MAP(CObjectListModeless)
	ON_WM_MOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CObjectListModeless message handlers


void CObjectListModeless::PostNcDestroy() 
{
	ClearTree();
	delete this;
}

void CObjectListModeless::OnMove(int x, int y) 
{
	CObjectListDialog::OnMove(x, y);
	
//	save location of modeless.
	RECT rect;
	GetWindowRect(&rect);
	GetParent()->ScreenToClient(&rect);
	D3EditState.objmodeless_x = rect.left;
	D3EditState.objmodeless_y = rect.top;
}


void CObjectListModeless::OnCancel() 
{
	// TODO: Add extra cleanup here

	DestroyWindow();
	D3EditState.objmodeless_on = false;
}



BOOL CObjectListModeless::OnInitDialog() 
{
	CObjectListDialog::OnInitDialog();
	
	CWnd *okbtn = GetDlgItem(IDOK);
	CWnd *canbtn = GetDlgItem(IDCANCEL);

//	remove ok and cancel buttons.
	okbtn->DestroyWindow();
	canbtn->DestroyWindow();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}



