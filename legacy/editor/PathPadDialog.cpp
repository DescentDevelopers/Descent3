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

// PathPadDialog.cpp : implementation file
//

#include "stdafx.h"
#include "editor.h"
#include "PathPadDialog.h"
#include "EditLineDialog.h"
#include "epath.h"
#include "object.h"
#include "room.h"
#include "FindIntersection.h"
#include "ebnode.h"
#include "boa.h"
#include "object.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPathPadDialog dialog


CPathPadDialog::CPathPadDialog(CWnd* pParent /*=NULL*/)
	: CKeypadDialog(CPathPadDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPathPadDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CPathPadDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPathPadDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPathPadDialog, CDialog)
	//{{AFX_MSG_MAP(CPathPadDialog)
	ON_EN_KILLFOCUS(IDC_CURRENT_NODE_EDIT, OnKillfocusCurrentNodeEdit)
	ON_BN_CLICKED(IDC_PATHPAD_ADD_PATH, OnPathpadAddPath)
	ON_BN_CLICKED(IDC_PATHPAD_INSERT_NODE, OnPathpadInsertNode)
	ON_BN_CLICKED(IDC_PATHPAD_DELETE_NODE, OnPathpadDeleteNode)
	ON_BN_CLICKED(IDC_PATHPAD_MOVE_FORWARD, OnPathpadMoveForward)
	ON_BN_CLICKED(IDC_PATHPAD_MOVE_BACKWARD, OnPathpadMoveBackward)
	ON_BN_CLICKED(IDC_PATHPAD_MOVE_LEFT, OnPathpadMoveLeft)
	ON_BN_CLICKED(IDC_PATHPAD_MOVE_RIGHT, OnPathpadMoveRight)
	ON_CBN_SELENDOK(IDC_PATHPAD_PULLDOWN, OnSelendokPathpadPulldown)
	ON_BN_CLICKED(IDC_PATHPAD_CENTER_VIEW, OnPathpadCenterView)
	ON_BN_CLICKED(IDC_PATHPAD_MOVE_DOWN, OnPathpadMoveDown)
	ON_BN_CLICKED(IDC_PATHPAD_MOVE_UP, OnPathpadMoveUp)
	ON_BN_CLICKED(IDC_PATHPAD_PREV_NODE, OnPathpadPrevNode)
	ON_BN_CLICKED(IDC_PATHPAD_NEXT_NODE, OnPathpadNextNode)
	ON_BN_CLICKED(IDC_DELETE_PATH, OnDeletePath)
	ON_BN_CLICKED(IDC_SHOW_NODES_CHECK, OnShowNodesCheck)
	ON_WM_HELPINFO()
	ON_BN_CLICKED(IDC_PATH_MOVE_OBJECT_BUTTON, OnPathMoveObjectButton)
	ON_BN_CLICKED(IDC_PATH_REORIENT_VIEWER_BUTTON, OnPathReorientViewerButton)
	ON_EN_KILLFOCUS(IDC_PATH_INC_TEXT, OnKillfocusPathIncText)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_PATH_MOVE_NODE_TO_CURRENT_OBJECT, OnPathMoveNodeToCurrentObject)
	ON_BN_CLICKED(IDC_PATH_MOVE_OBJ_NO_ORIENT, OnPathMoveObjNoOrient)
	ON_BN_CLICKED(IDC_PATH_VERIFY_BUTTON, OnPathVerifyButton)
	ON_BN_CLICKED(IDC_AIN_AUTO_EDGE_NODE, OnAinAutoEdgeNode)
	ON_BN_CLICKED(IDC_AIN_BACK, OnAinBack)
	ON_BN_CLICKED(IDC_AIN_CREATE_NODE_AT_VIEWER, OnAinCreateNodeAtViewer)
	ON_BN_CLICKED(IDC_AIN_CREATE_NODES, OnAinCreateNodes)
	ON_CBN_SELENDOK(IDC_AIN_CURRENT_NODE, OnSelendokAinCurrentNode)
	ON_BN_CLICKED(IDC_AIN_DELETE_EDGE, OnAinDeleteEdge)
	ON_BN_CLICKED(IDC_AIN_DELETE_NODE, OnAinDeleteNode)
	ON_BN_CLICKED(IDC_AIN_DOWN, OnAinDown)
	ON_BN_CLICKED(IDC_AIN_FORWARD, OnAinForward)
	ON_BN_CLICKED(IDC_AIN_INSERT_NODE_ON_EDGE, OnAinInsertNodeOnEdge)
	ON_BN_CLICKED(IDC_AIN_LEFT, OnAinLeft)
	ON_BN_CLICKED(IDC_AIN_MAKE_EDGE, OnAinMakeEdge)
	ON_EN_KILLFOCUS(IDC_AIN_MOVE_AMOUNT, OnKillfocusAinMoveAmount)
	ON_BN_CLICKED(IDC_AIN_MOVE_NODE_TO_VIEWER, OnAinMoveNodeToViewer)
	ON_BN_CLICKED(IDC_AIN_MPNS, OnAinMpns)
	ON_CBN_SELENDOK(IDC_AIN_OTHER_NODE, OnSelendokAinOtherNode)
	ON_BN_CLICKED(IDC_AIN_RESET_ROOM_NODES, OnAinResetRoomNodes)
	ON_BN_CLICKED(IDC_AIN_RIGHT, OnAinRight)
	ON_BN_CLICKED(IDC_AIN_SEL_NODE_EDGES, OnAinSelNodeEdges)
	ON_BN_CLICKED(IDC_AIN_SEL_ROOM, OnAinSelRoom)
	ON_BN_CLICKED(IDC_AIN_UP, OnAinUp)
	ON_BN_CLICKED(IDC_AIN_SEL_ROOM_AND_NEXT_ROOMS, OnAinSelRoomAndNextRooms)
	ON_BN_CLICKED(IDC_AIN_VERIFY_LEVEL, OnAinVerifyLevel)
	ON_BN_CLICKED(IDC_AIN_VERIFY_ROOM, OnAinVerifyRoom)
	ON_BN_CLICKED(IDC_AIN_VIEW_ALL_NODES, OnAinViewAllNodes)
	ON_BN_CLICKED(IDC_AIN_VIEW_NONE, OnAinViewNone)
	ON_BN_CLICKED(IDC_AIN_VIEW_ROOM, OnAinViewRoom)
	ON_BN_CLICKED(IDC_AIN_VIEW_ROOM_AND_NEXT_ROOMS, OnAinViewRoomAndNextRooms)
	ON_CBN_SETFOCUS(IDC_AIN_CURRENT_NODE, OnSetfocusAinCurrentNode)
	ON_CBN_SETFOCUS(IDC_AIN_OTHER_NODE, OnSetfocusAinOtherNode)
	ON_BN_CLICKED(IDC_IENAV_BUTTON, OnIenavButton)
	ON_BN_CLICKED(IDC_MVTN_BUTTON, OnMvtnButton)
	ON_BN_CLICKED(IDC_AER_BUTTON, OnAerButton)
	ON_BN_CLICKED(IDC_ONTVGP_BUTTON, OnOntvgpButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPathPadDialog message handlers

void CPathPadDialog::OnKillfocusCurrentNodeEdit() 
{
	CEdit *ebox;
	char str[20];
	int curpath=D3EditState.current_path;
	
	if (Num_game_paths<1)
	{
		MessageBox ("There are no paths to operate on.");
		return;
	}

	ebox=(CEdit *) GetDlgItem (IDC_CURRENT_NODE_EDIT);
	ebox->GetWindowText (str,20);

	// Do the base 1 to base 0 conversion
	D3EditState.current_node = atoi (str) - 1;
	
	if(D3EditState.current_node >= GamePaths[curpath].num_nodes)
		D3EditState.current_node = GamePaths[curpath].num_nodes - 1;
	else if(D3EditState.current_node < 0)
		D3EditState.current_node = 0;
	
	UpdateDialog();
}

void CPathPadDialog::OnPathpadAddPath() 
{
	
	int curnode;
	char *namestr;
	int pathnum;
	int dup_test;

	pathnum=AllocGamePath ();
	if (pathnum < 0)
	{
		return;
	}

	CEditLineDialog dlg ("Enter path name",this);
	int ret=dlg.DoModal();

	if (ret==IDCANCEL)
	{
		FreeGamePath(pathnum);
		return;
	}

	namestr=(char *)dlg.GetText ();
	dup_test = FindGamePathName(namestr);
	if(dup_test != -1)
	{
		OutrageMessageBox("PATH NOT CREATED: A path named %s already exists.", namestr);
		FreeGamePath(pathnum);
		return;
	}
	strcpy (GamePaths[pathnum].name,namestr);

	curnode=InsertNodeIntoPath (pathnum,-1,0);
	if(curnode < 0) return;

	D3EditState.current_path=pathnum;
	D3EditState.current_node=curnode;
	World_changed=1;
	UpdateDialog();
}

void CPathPadDialog::OnPathpadInsertNode() 
{
	int curpath=D3EditState.current_path;
	int curnode=D3EditState.current_node;

	if (Num_game_paths<1)
	{
		MessageBox ("There are no paths to operate on.");
		return;
	}

	curnode=InsertNodeIntoPath (curpath,curnode,0);
	if(curnode < 0) return;

	D3EditState.current_node=curnode;
	World_changed=1;
	UpdateDialog();
		
}

void CPathPadDialog::OnPathpadDeleteNode() 
{
	int curpath=D3EditState.current_path;
	int curnode=D3EditState.current_node;

	if (Num_game_paths<1)
	{
		MessageBox ("There are no paths to operate on.");
		return;
	}

	if (GamePaths[curpath].num_nodes==1)
	{
		MessageBox ("This is the only node in this path.  If you wish to delete this path, click the 'Delete Path' button.");
		return;
	}

	DeleteNodeFromPath (curpath,curnode);

	// If this was the ending node, move it back one
	if (curnode==GamePaths[curpath].num_nodes)
		D3EditState.current_node=curnode-1;

	State_changed=1;
	UpdateDialog();

}

void CPathPadDialog::OnPathpadMoveForward() 
{
	if (Num_game_paths<1)
	{
		MessageBox ("There are no paths to operate on.");
		return;
	}

	int p=D3EditState.current_path;
	int n=D3EditState.current_node;

	vector delta_movement = Viewer_object->orient.fvec * D3EditState.node_movement_inc;

	MovePathNode(p, n, &delta_movement);

	World_changed=1;
	UpdateDialog();
}

void CPathPadDialog::OnPathpadMoveBackward() 
{
	if (Num_game_paths<1)
	{
		MessageBox ("There are no paths to operate on.");
		return;
	}

	int p=D3EditState.current_path;
	int n=D3EditState.current_node;

	vector delta_movement = Viewer_object->orient.fvec * -D3EditState.node_movement_inc;

	MovePathNode(p, n, &delta_movement);

	World_changed=1;
	UpdateDialog();
}

void CPathPadDialog::OnPathpadMoveLeft() 
{
	if (Num_game_paths<1)
	{
		MessageBox ("There are no paths to operate on.");
		return;
	}

	int p=D3EditState.current_path;
	int n=D3EditState.current_node;

	vector delta_movement = Viewer_object->orient.rvec * -D3EditState.node_movement_inc;

	MovePathNode(p, n, &delta_movement);

	World_changed=1;
	UpdateDialog();
}

void CPathPadDialog::OnPathpadMoveRight() 
{
	if (Num_game_paths<1)
	{
		MessageBox ("There are no paths to operate on.");
		return;
	}

	int p=D3EditState.current_path;
	int n=D3EditState.current_node;

	vector delta_movement = Viewer_object->orient.rvec * D3EditState.node_movement_inc;

	MovePathNode(p, n, &delta_movement);

	World_changed=1;
	UpdateDialog();
}

void CPathPadDialog::OnSelendokPathpadPulldown() 
{
	int i,cur;
	char name[200];

	cur=SendDlgItemMessage( IDC_PATHPAD_PULLDOWN, CB_GETCURSEL,0,0);
	SendDlgItemMessage( IDC_PATHPAD_PULLDOWN, CB_GETLBTEXT,cur,(LPARAM) (LPCTSTR)name);
	
	i=FindGamePathName (name);
	
	D3EditState.current_path=i;
	D3EditState.current_node=0;

	State_changed=1;
	UpdateDialog();
}

void CPathPadDialog::OnPathpadCenterView() 
{
	if (Num_game_paths<1)
	{
		MessageBox ("There are no paths to operate on.");
		return;
	}

	int p=D3EditState.current_path;
	int n=D3EditState.current_node;

	game_path *gp=&GamePaths[p];

	vector attempted_pos;
	vector subvec={0.0,0.0,0.0};  // changed for now

	attempted_pos = gp->pathnodes[n].pos - subvec;

	fvi_query fq;
	fvi_info	hit_info;
	
	fq.p0					= &GamePaths[p].pathnodes[n].pos;
	fq.startroom		= GamePaths[p].pathnodes[n].roomnum;
	fq.p1					= &attempted_pos;
	fq.rad					= 0.0f;
	fq.thisobjnum			= -1;
	fq.ignore_obj_list	= NULL;
	fq.flags				= FQ_TRANSPOINT | FQ_IGNORE_RENDER_THROUGH_PORTALS;

	int hit = fvi_FindIntersection(&fq, &hit_info);

	if (hit != HIT_NONE)
		Int3();		//what does it mean that we hit something?

	ObjSetPos(Viewer_object,&hit_info.hit_pnt,hit_info.hit_room,NULL,false);

	World_changed=1;
	UpdateDialog();
	
	
}

void CPathPadDialog::OnPathpadMoveDown() 
{
	if (Num_game_paths<1)
	{
		MessageBox ("There are no paths to operate on.");
		return;
	}

	int p=D3EditState.current_path;
	int n=D3EditState.current_node;

	vector delta_movement = Viewer_object->orient.uvec * -D3EditState.node_movement_inc;

	MovePathNode(p, n, &delta_movement);

	World_changed=1;
	UpdateDialog();
}

void CPathPadDialog::OnPathpadMoveUp() 
{
	if (Num_game_paths<1)
	{
		MessageBox ("There are no paths to operate on.");
		return;
	}

	int p=D3EditState.current_path;
	int n=D3EditState.current_node;

	vector delta_movement = Viewer_object->orient.uvec * D3EditState.node_movement_inc;

	MovePathNode(p, n, &delta_movement);

	World_changed=1;
	UpdateDialog();
}

void CPathPadDialog::OnPathpadPrevNode() 
{
	if (Num_game_paths<1)
		return;

	int p=D3EditState.current_path;
	int n=D3EditState.current_node;

	n--;

	if (n<0)
		n=GamePaths[p].num_nodes-1;

	D3EditState.current_node=n;
	
	World_changed=1;
	UpdateDialog();
	
}

void CPathPadDialog::OnPathpadNextNode() 
{
	if (Num_game_paths<1)
		return;

	int p=D3EditState.current_path;
	int n=D3EditState.current_node;

	n++;

	n%=GamePaths[p].num_nodes;
	D3EditState.current_node=n;
	
	World_changed=1;
	UpdateDialog();
}

void CPathPadDialog::UpdateDialog()
{
	int n=D3EditState.current_path;
	char str[50];
	CEdit *ebox;

	SendDlgItemMessage( IDC_PATHPAD_PULLDOWN, CB_RESETCONTENT,0,0);

	for (int i=0;i<MAX_GAME_PATHS;i++)
	{
		if (GamePaths[i].used)
			SendDlgItemMessage( IDC_PATHPAD_PULLDOWN, CB_ADDSTRING,0,(LPARAM) (LPCTSTR) GamePaths[i].name);
	}
	SendDlgItemMessage( IDC_PATHPAD_PULLDOWN, CB_SELECTSTRING,0,(LPARAM) (LPCTSTR) GamePaths[n].name);

	ebox=(CEdit *) GetDlgItem (IDC_CURRENT_NODE_EDIT);
	sprintf (str,"%d",D3EditState.current_node + 1);
	ebox->SetWindowText (str);

	ebox=(CEdit *) GetDlgItem (IDC_PATH_INC_TEXT);
	sprintf (str,"%f",D3EditState.node_movement_inc);
	ebox->SetWindowText (str);

	if(n < Num_game_paths && D3EditState.current_node < GamePaths[n].num_nodes)
	{
		ebox=(CEdit *) GetDlgItem (IDC_PATHPAD_CUR_NODE_ROOM);
		if(!ROOMNUM_OUTSIDE(GamePaths[n].pathnodes[D3EditState.current_node].roomnum))
			sprintf (str,"Current Mine Room: %d",GamePaths[n].pathnodes[D3EditState.current_node].roomnum);
		else
			sprintf (str,"Current Terrain Cell: %d", (0x7FFFFFFF & GamePaths[n].pathnodes[D3EditState.current_node].roomnum));
		ebox->SetWindowText (str);
	}

	ebox=(CEdit *) GetDlgItem (IDC_PATHPAD_NUM_NODES);
	sprintf (str,"Number of nodes: %d",GamePaths[n].num_nodes);
	ebox->SetWindowText (str);

	CheckDlgButton (IDC_SHOW_NODES_CHECK,Show_paths?1:0);

}


void CPathPadDialog::OnDeletePath() 
{
	int ret;
	int n=D3EditState.current_path;

	if (Num_game_paths<1)
		return;

	ret=MessageBox ("Are you sure you want to delete this path?",GamePaths[n].name,MB_YESNO);
	if (ret==IDNO)
		return;

	FreeGamePath(n);

	D3EditState.current_path=GetNextPath(n);
	World_changed=1;
	UpdateDialog();
}

void CPathPadDialog::OnShowNodesCheck() 
{
	int c=IsDlgButtonChecked(IDC_SHOW_NODES_CHECK);

	if (c)
		Show_paths=1;
	else
		Show_paths=0;

	TV_changed=1;
}

BOOL CPathPadDialog::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	WinHelp(HID_PATHSTAB,HELP_CONTEXT);
	return TRUE;
	//return CDialog::OnHelpInfo(pHelpInfo);
}

void CPathPadDialog::OnPathMoveObjectButton() 
{
	if (Num_game_paths<1)
	{
		MessageBox ("There are no paths to operate on.");
		return;
	}

	if (Cur_object_index != -1) //	we have a selected object
	{	

		int p=D3EditState.current_path;
		int n=D3EditState.current_node;

		game_path *gp=&GamePaths[p];

		ObjSetPos(&Objects[Cur_object_index],&gp->pathnodes[n].pos,gp->pathnodes[n].roomnum,NULL,false);
		
		matrix node_orient;

		vm_VectorToMatrix(&node_orient, &gp->pathnodes[n].fvec, &gp->pathnodes[n].uvec, NULL);
		ObjSetOrient(&Objects[Cur_object_index], &node_orient);

		World_changed=1;
	}
}

void CPathPadDialog::OnPathReorientViewerButton() 
{
	if (Num_game_paths<1)
	{
		MessageBox ("There are no paths to operate on.");
		return;
	}

	int p=D3EditState.current_path;
	int n=D3EditState.current_node;

	game_path *gp=&GamePaths[p];

	matrix node_orient;

	vm_VectorToMatrix(&node_orient, &gp->pathnodes[n].fvec, &gp->pathnodes[n].uvec, NULL);
	ObjSetOrient(Viewer_object, &node_orient);

	World_changed=1;
	UpdateDialog();
}

void CPathPadDialog::OnKillfocusPathIncText() 
{
	CEdit *ebox;
	char str[20];

	ebox=(CEdit *) GetDlgItem (IDC_PATH_INC_TEXT);
	ebox->GetWindowText (str,20);

	// Do the base 1 to base 0 conversion
	sscanf(str, "%f", &D3EditState.node_movement_inc);

	if(D3EditState.node_movement_inc <= 0.0f || D3EditState.node_movement_inc > 32000.0f)
		D3EditState.node_movement_inc = 1.0f;

	UpdateDialog();
}

void CPathPadDialog::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	UpdateDialog();
}

void CPathPadDialog::OnPathMoveNodeToCurrentObject() 
{
	if (Num_game_paths<1)
	{
		MessageBox ("There are no paths to operate on.");
		return;
	}

	if (Cur_object_index != -1) //	we have a selected object
	{	

		int p=D3EditState.current_path;
		int n=D3EditState.current_node;

		game_path *gp=&GamePaths[p];
		MovePathNodeToPos(p, n, &Objects[Cur_object_index].pos);

		World_changed=1;
		UpdateDialog();
	}
}

void CPathPadDialog::OnPathMoveObjNoOrient() 
{
	if (Num_game_paths<1)
	{
		MessageBox ("There are no paths to operate on.");
		return;
	}

	if (Cur_object_index != -1) //	we have a selected object
	{	

		int p=D3EditState.current_path;
		int n=D3EditState.current_node;

		game_path *gp=&GamePaths[p];

		ObjSetPos(&Objects[Cur_object_index],&gp->pathnodes[n].pos,gp->pathnodes[n].roomnum, NULL, false);
		
		//matrix node_orient;

		World_changed=1;
		UpdateDialog();
	}
}

void CPathPadDialog::OnPathVerifyButton() 
{
	
}

int BNode_current_id = 0;
int BNode_current_room = 0;
int BNode_other_id = 0;
int BNode_other_room = 0;

float BNode_move_amount = 1.0f;

void CPathPadDialog::OnAinAutoEdgeNode() 
{
	EBNode_AutoEdgeNode(BNode_current_id, BNode_current_room);
	EBNode_VerifyGraph();	
	World_changed = 1;
}

void CPathPadDialog::OnAinBack() 
{
	vector diff = -Viewer_object->orient.fvec * BNode_move_amount;

	EBNode_Move(true, BNode_current_room, BNode_current_id, &diff);
	World_changed = 1;
}

void CPathPadDialog::OnAinCreateNodeAtViewer() 
{
	int node = EBNode_AddNode(Viewer_object->roomnum, &Viewer_object->pos, true, true);
	
	if(node != -1)
	{
		World_changed = 1;
	}

//	EBNode_VerifyGraph();	
}

void CPathPadDialog::OnAinCreateNodes() 
{
	MakeBOA();
	EBNode_MakeFirstPass();	
	World_changed = 1;
}

void CPathPadDialog::OnSelendokAinCurrentNode() 
{
	int cur;
	char name[200];

	cur=SendDlgItemMessage( IDC_AIN_CURRENT_NODE, CB_GETCURSEL,0,0);
	SendDlgItemMessage( IDC_AIN_CURRENT_NODE, CB_GETLBTEXT,cur,(LPARAM) (LPCTSTR)name);

	if(sscanf(name, "Room %d, Node %d", &BNode_current_room, &BNode_current_id) != 2)
	{
		sscanf(name, "Region %d, Node %d", &BNode_current_room, &BNode_current_id);
		BNode_current_room += Highest_room_index + 1;
	}

	BNode_current_id--;
}

void CPathPadDialog::OnAinDeleteEdge() 
{
	EBNode_RemoveEdge(BNode_current_id, BNode_current_room, BNode_other_id, BNode_other_room);	
	World_changed = 1;

	EBNode_VerifyGraph();	
}

void CPathPadDialog::OnAinDeleteNode() 
{
	EBNode_RemoveNode(BNode_current_room, BNode_current_id);
	World_changed = 1;

	EBNode_VerifyGraph();	
}

void CPathPadDialog::OnAinDown() 
{
	vector diff = -Viewer_object->orient.uvec * BNode_move_amount;

	EBNode_Move(true, BNode_current_room, BNode_current_id, &diff);
	World_changed = 1;
}

void CPathPadDialog::OnAinForward() 
{
	vector diff = Viewer_object->orient.fvec * BNode_move_amount;

	EBNode_Move(true, BNode_current_room, BNode_current_id, &diff);
	World_changed = 1;
}

void CPathPadDialog::OnAinInsertNodeOnEdge() 
{
	EBNode_InsertNodeOnEdge(BNode_current_id, BNode_current_room, BNode_other_id, BNode_other_room);	
	EBNode_VerifyGraph();	
	World_changed = 1;
}

void CPathPadDialog::OnAinLeft() 
{
	vector diff = -Viewer_object->orient.rvec * BNode_move_amount;

	EBNode_Move(true, BNode_current_room, BNode_current_id, &diff);
	World_changed = 1;
}

void CPathPadDialog::OnAinMakeEdge() 
{
	EBNode_AddEdge(BNode_current_id, BNode_current_room, BNode_other_id, BNode_other_room);	
	EBNode_VerifyGraph();	
	World_changed = 1;
}

void CPathPadDialog::OnKillfocusAinMoveAmount() 
{
	CEdit *ebox;
	char str[20];
	
	ebox=(CEdit *) GetDlgItem (IDC_AIN_MOVE_AMOUNT);
	ebox->GetWindowText (str,20);
	sscanf(str, "%f", &BNode_move_amount);

	UpdateDialog();
}

void CPathPadDialog::OnAinMoveNodeToViewer() 
{
	vector diff = Viewer_object->pos;

	EBNode_Move(false, BNode_current_room, BNode_current_id, &diff);
	World_changed = 1;
}

void CPathPadDialog::OnAinMpns() 
{
	// TODO: Add your control notification handler code here
	
}

void CPathPadDialog::OnSelendokAinOtherNode() 
{
	int cur;
	char name[200];

	cur=SendDlgItemMessage( IDC_AIN_OTHER_NODE, CB_GETCURSEL,0,0);
	SendDlgItemMessage( IDC_AIN_OTHER_NODE, CB_GETLBTEXT,cur,(LPARAM) (LPCTSTR)name);

	if(sscanf(name, "Room %d, Node %d", &BNode_other_room, &BNode_other_id) != 2)
	{
		sscanf(name, "Region %d, Node %d", &BNode_other_room, &BNode_other_id);
		BNode_other_room += Highest_room_index + 1;
	}

	BNode_other_id--;
}

void CPathPadDialog::OnAinResetRoomNodes() 
{
	// TODO: Add your control notification handler code here
	EBNode_VerifyGraph();	
}

void CPathPadDialog::OnAinRight() 
{
	vector diff = Viewer_object->orient.rvec * BNode_move_amount;

	EBNode_Move(true, BNode_current_room, BNode_current_id, &diff);
	World_changed = 1;
}

#define EBVIEW_ROOM                1
#define EBVIEW_ROOM_AND_NEXT_ROOMS 2
#define EBVIEW_CUR_EDGES			  3

char ENode_view_type = EBVIEW_ROOM;

void CPathPadDialog::OnAinSelNodeEdges() 
{
	ENode_view_type = EBVIEW_CUR_EDGES;
	ResetNodesForComboBox(IDC_AIN_CURRENT_NODE, ENode_view_type);
	ResetNodesForComboBox(IDC_AIN_OTHER_NODE ,ENode_view_type);
}

void CPathPadDialog::OnAinSelRoom() 
{
	ENode_view_type = EBVIEW_ROOM;
	ResetNodesForComboBox(IDC_AIN_CURRENT_NODE, ENode_view_type);
	ResetNodesForComboBox(IDC_AIN_OTHER_NODE ,ENode_view_type);
}

void CPathPadDialog::OnAinSelRoomAndNextRooms() 
{
	ENode_view_type = EBVIEW_ROOM_AND_NEXT_ROOMS;
	ResetNodesForComboBox(IDC_AIN_CURRENT_NODE, ENode_view_type);
	ResetNodesForComboBox(IDC_AIN_OTHER_NODE ,ENode_view_type);
}

void CPathPadDialog::OnAinUp() 
{
	vector diff = Viewer_object->orient.uvec * BNode_move_amount;

	EBNode_Move(true, BNode_current_room, BNode_current_id, &diff);
	World_changed = 1;
}

void CPathPadDialog::OnAinVerifyLevel() 
{
	EBNode_VerifyGraph();
}

void CPathPadDialog::OnAinVerifyRoom() 
{
	EBNode_ClearLevel();
	World_changed = 1;
}

void CPathPadDialog::OnAinViewAllNodes() 
{
	EBN_draw_type = EBDRAW_LEVEL;
	World_changed = 1;
}

void CPathPadDialog::OnAinViewNone() 
{
	EBN_draw_type = EBDRAW_NONE;
	World_changed = 1;
}

void CPathPadDialog::OnAinViewRoom() 
{
	EBN_draw_type = EBDRAW_ROOM;
	World_changed = 1;
}

void CPathPadDialog::OnAinViewRoomAndNextRooms() 
{
	EBN_draw_type = EBDRAW_ROOM_AND_NEXT_ROOMS;
	World_changed = 1;
}

void CPathPadDialog::ResetNodesForComboBox(int handle, char selection_type)
{
	char message[25];
	SendDlgItemMessage(handle, CB_RESETCONTENT, 0, 0);
	bool f_add = false;

	int num_next_rooms = 0;
	int next_rooms[1000];

	if(Viewer_object->roomnum >= 0 && Viewer_object->roomnum <= Highest_room_index && Rooms[Viewer_object->roomnum].used)
	{
		if(selection_type == EBVIEW_ROOM || selection_type == EBVIEW_ROOM_AND_NEXT_ROOMS)
		{
			for (int i = 0; i < Rooms[Viewer_object->roomnum].bn_info.num_nodes; i++)
			{
				f_add = true;

				sprintf(message, "Room %d, Node %d", Viewer_object->roomnum, i + 1);
				SendDlgItemMessage( handle, CB_ADDSTRING,0,(LPARAM) (LPCTSTR) message);
			}

			if(selection_type == EBVIEW_ROOM_AND_NEXT_ROOMS)
			{
				int i;
				int j;
				int k;

				for(i = 0; i < Rooms[Viewer_object->roomnum].num_portals; i++)
				{
					bool f_found = false;

					for(j = 0; j < num_next_rooms; j++)
					{
						if(next_rooms[j] == Rooms[Viewer_object->roomnum].portals[i].croom)
						{
							f_found = true;
							break;
						}
					}

					if(!f_found && 
						Rooms[Viewer_object->roomnum].portals[i].croom >= 0 && 
						!(Rooms[Rooms[Viewer_object->roomnum].portals[i].croom].flags & RF_EXTERNAL))
					{
						for(k = 0; k < Rooms[Rooms[Viewer_object->roomnum].portals[i].croom].bn_info.num_nodes; k++)
						{
							sprintf(message, "Room %d, Node %d", Rooms[Viewer_object->roomnum].portals[i].croom, k + 1);
							SendDlgItemMessage( handle, CB_ADDSTRING,0,(LPARAM) (LPCTSTR) message);
						}
					}
				}
			}

			if(f_add)
			{
				sprintf(message, "Room %d, Node %d", Viewer_object->roomnum, 0 + 1);
				SendDlgItemMessage( handle, CB_SELECTSTRING,0,(LPARAM) (LPCTSTR) message);
			}
			else
			{
				strcpy(message, "No Nodes in room");
				SendDlgItemMessage( handle, CB_SELECTSTRING,0,(LPARAM) (LPCTSTR) message);
			}
		}
	}	
	else if(ROOMNUM_OUTSIDE(Viewer_object->roomnum))
	{
		if(selection_type == EBVIEW_ROOM || selection_type == EBVIEW_ROOM_AND_NEXT_ROOMS)
		{
			int room = BOA_INDEX(Viewer_object->roomnum);
			int tr = room - Highest_room_index - 1;
			bn_list *bnlist = BNode_GetBNListPtr(Viewer_object->roomnum);

			for (int i = 0; i < bnlist->num_nodes; i++)
			{
				f_add = true;

				sprintf(message, "Region %d, Node %d", tr, i + 1);
				SendDlgItemMessage( handle, CB_ADDSTRING,0,(LPARAM) (LPCTSTR) message);
			}

			if(selection_type == EBVIEW_ROOM_AND_NEXT_ROOMS)
			{
				int i;
				int j;
				int k;

				for(i = 0; i < BOA_num_connect[tr]; i++)
				{
					bool f_found = false;

					for(j = 0; j < num_next_rooms; j++)
					{
						if(next_rooms[j] == BOA_connect[tr][i].roomnum)
						{
							f_found = true;
							break;
						}
					}

					if(!f_found && 
						BOA_connect[tr][i].roomnum >= 0 && 
						!(Rooms[BOA_connect[tr][i].roomnum].flags & RF_EXTERNAL))
					{
						for(k = 0; k < Rooms[BOA_connect[tr][i].roomnum].bn_info.num_nodes; k++)
						{
							sprintf(message, "Room %d, Node %d", BOA_connect[tr][i].roomnum, k + 1);
							SendDlgItemMessage( handle, CB_ADDSTRING,0,(LPARAM) (LPCTSTR) message);
						}
					}
				}
			}

			if(f_add)
			{
				sprintf(message, "Region %d, Node %d", tr, 0 + 1);
				SendDlgItemMessage( handle, CB_SELECTSTRING,0,(LPARAM) (LPCTSTR) message);
			}
			else
			{
				strcpy(message, "No Nodes in region");
				SendDlgItemMessage( handle, CB_SELECTSTRING,0,(LPARAM) (LPCTSTR) message);
			}
		}
	}
}

void CPathPadDialog::OnSetfocusAinCurrentNode() 
{
	ResetNodesForComboBox(IDC_AIN_CURRENT_NODE, ENode_view_type);
}

void CPathPadDialog::OnSetfocusAinOtherNode() 
{
	ResetNodesForComboBox(IDC_AIN_OTHER_NODE ,ENode_view_type);
}

void CPathPadDialog::OnIenavButton() 
{
	int node = EBNode_InsertNodeOnEdge(BNode_current_id, BNode_current_room, BNode_other_id, BNode_other_room);		
	if(node >= 0)
	{
		vector diff = Viewer_object->pos;

		EBNode_Move(false, BNode_current_room, node, &diff);
		World_changed = 1;
	}
}

void CPathPadDialog::OnMvtnButton() 
{
	vector pos;
	int roomnum;
	
	bn_list *bnlist = BNode_GetBNListPtr(BNode_current_room);
	if(!bnlist)
		return;

	pos = bnlist->nodes[BNode_current_id].pos;

	if(BNode_current_room >= 0 && BNode_current_room <= Highest_room_index)
	{
		roomnum = BNode_current_room;
	}
	else
	{
		roomnum = GetTerrainRoomFromPos(&pos);
	}
	
	BNode_current_id,

	ObjSetPos(Viewer_object, &pos, roomnum, NULL, false);
	World_changed = 1;	
}

void CPathPadDialog::OnAerButton() 
{
	bn_list *bnlist = BNode_GetBNListPtr(BNode_current_room);
	if(!bnlist)
		return;

	for(int i = 0; i < bnlist->num_nodes; i++)
	{
		EBNode_AutoEdgeNode(i, BNode_current_room);
	}

	EBNode_VerifyGraph();	
	World_changed = 1;
}


void CPathPadDialog::OnOntvgpButton() 
{
	if (Num_game_paths<1)
	{
		MessageBox ("There are no paths to operate on.");
		return;
	}

	if (Cur_object_index != -1) //	we have a selected object
	{	

		int p=D3EditState.current_path;
		int n=D3EditState.current_node;

		game_path *gp=&GamePaths[p];

		gp->pathnodes[n].fvec = Viewer_object->orient.fvec;
		gp->pathnodes[n].uvec = Viewer_object->orient.uvec;

		World_changed=1;
		UpdateDialog();
	}	
}
