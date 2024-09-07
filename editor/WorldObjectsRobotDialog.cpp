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

--- HISTORICAL COMMENTS FOLLOW ---

 * $Logfile: /DescentIII/Main/editor/WorldObjectsRobotDialog.cpp $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:57:39 $
 * $Author: kevinb $
 *
 * WorldObjectsRobotDialog.cpp : implementation file
 *
 * $Log: not supported by cvs2svn $
 *
 * 8     8/08/97 3:48p Samir
 * Added script number edit box for temporary editing.
 *
 * 7     8/08/97 11:45a Matt
 * Made anim states dialog keep local copy of anim array.  The caller then
 * copies the data to his own array if the user exits from the dialog via
 * OK.
 *
 * 6     8/06/97 2:33p Jason
 * fixed potential memory bug
 *
 * 5     8/06/97 1:32p Matt
 * Ripped out a bunch of stuff, and added separate physics and ai dialogs
 *
 * 4     7/24/97 6:11p Matt
 * Created symbolic constant for default zoom, and used it every place
 * that specifies zoom
 *
 * $NoKeywords: $
 */

#include "mfc_compatibility.h"
#include "editor.h"
#include "WorldObjectsRobotDialog.h"
#include "AnimStatesDialog.h"
#include "robotpage.h"
#include "manage.h"
#include "robot.h"
#include "polymodel.h"
#include "PhysicsDlg.h"
#include "AISettingsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWorldObjectsRobotDialog dialog

CWorldObjectsRobotDialog::CWorldObjectsRobotDialog(CWnd *pParent /*=NULL*/)
    : CDialog(CWorldObjectsRobotDialog::IDD, pParent) {
  //{{AFX_DATA_INIT(CWorldObjectsRobotDialog)
  // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT
}

void CWorldObjectsRobotDialog::DoDataExchange(CDataExchange *pDX) {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CWorldObjectsRobotDialog)
  // NOTE: the ClassWizard will add DDX and DDV calls here
  //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CWorldObjectsRobotDialog, CDialog)
//{{AFX_MSG_MAP(CWorldObjectsRobotDialog)
ON_BN_CLICKED(IDC_ADD_NEW_ROBOT, OnAddNewRobot)
ON_BN_CLICKED(IDC_CHECKIN_ROBOT, OnCheckinRobot)
ON_BN_CLICKED(IDC_DELETE_ROBOT, OnDeleteRobot)
ON_BN_CLICKED(IDC_LOCK_ROBOT, OnLockRobot)
ON_BN_CLICKED(IDC_NEXT_ROBOT, OnNextRobot)
ON_BN_CLICKED(IDC_PREV_ROBOT, OnPrevRobot)
ON_BN_CLICKED(IDC_ROBOT_MODEL, OnRobotModel)
ON_EN_KILLFOCUS(IDC_ROBOT_NAME_EDIT, OnKillfocusRobotNameEdit)
ON_CBN_SELENDOK(IDC_ROBOT_PULLDOWN, OnSelendokRobotPulldown)
ON_WM_TIMER()
ON_EN_KILLFOCUS(IDC_ROBOT_SIZE_EDIT, OnKillfocusRobotSizeEdit)
ON_BN_CLICKED(IDC_ROBOTS_OUT, OnRobotsOut)
ON_BN_CLICKED(IDC_DEFINE_ANIMSTATES, OnDefineAnimstates)
ON_WM_HELPINFO()
ON_BN_CLICKED(IDC_ROBOT_EDIT_PHYSICS, OnRobotEditPhysics)
ON_BN_CLICKED(IDC_ROBOT_USES_PHYSICS, OnRobotUsesPhysics)
ON_BN_CLICKED(IDC_ROBOT_USES_AI, OnRobotUsesAI)
ON_BN_CLICKED(IDC_ROBOT_EDIT_AI, OnRobotEditAI)
ON_EN_KILLFOCUS(IDC_SCRIPTID, OnKillfocusScriptid)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWorldObjectsRobotDialog message handlers

void CWorldObjectsRobotDialog::OnAddNewRobot() {
#if 0 // LGT: undeclared symbols
	char filename[255];
	char cur_name[100];
	int img_handle;
	int robot_handle;
	int c=1,finding_name=1;
	
	if (!Network_up)
	{
		OutrageMessageBox ("Sorry babe, the network is down.  This action is a no-no.\n");
		return;
	}

	// Get the filename of the representing image
	
	CString filter = "Descent III files (*.pof,*.oof)|*.pof;*.oof||";

	CFileDialog dlg_open(TRUE, 0,0, OFN_FILEMUSTEXIST, (LPCTSTR)filter, this);

	if (dlg_open.DoModal() == IDCANCEL) return;

//	Okay, we selected a file. Lets do what needs to be done here.
	
	strcpy (filename,dlg_open.GetPathName());
	img_handle=LoadRobotImage (filename);
	
	if (img_handle<0)
	{
		OutrageMessageBox ("Couldn't open that model file.");
		return;
	}

	// Alloc a robot and give a name not already taken by some other robot

	robot_handle=AllocRobot();

	while (finding_name)
	{
		// sprintf (cur_name,"%s%d",dlg_open.GetFileName(),c); // LGT: CString not char*
		if (FindRobotName (cur_name)!=-1)
			c++;
		else
			finding_name=0;
	}	
	
	strcpy (Robots[robot_handle].name,cur_name);

	Robots[robot_handle].render_handle=img_handle;

	// figure out size of robot
	vector size_vec;
	float size;

	size_vec=Poly_models[img_handle].mins-Poly_models[img_handle].maxs;
	size=vm_GetMagnitude (&size_vec);
	Robots[robot_handle].size=size/2;

	// Finally, save a local copy of the model/anim and alloc a tracklock
	mprintf(0,"Making a copy of this model locally...\n");

	char destname[100];
	sprintf (destname,"%s\\%s",LocalModelsDir,Poly_models[Robots[robot_handle].render_handle].name);
	if (stricmp (destname,filename))	// only copy if they are different
		cf_CopyFile (destname,filename);
	
	mng_AllocTrackLock (cur_name,PAGETYPE_ROBOT);

	D3EditState.current_robot=robot_handle;

	RemapRobots();
		
	UpdateDialog ();

#endif
}

void CWorldObjectsRobotDialog::OnCheckinRobot() {
#if 0 // LGT: undeclared symbols
	int n=D3EditState.current_robot;
	mngs_Pagelock temp_pl;
	int r;

	if (Num_robots<1)
		return;

	if (!mng_MakeLocker())
	{
		OutrageMessageBox ("Either you have no network support, or someone else has exclusive access to the table files (retry in a 5 seconds)");
		return;
	}


	// Make sure we own this robot
	strcpy (temp_pl.name,Robots[n].name);
	temp_pl.pagetype=PAGETYPE_ROBOT;
	
	r=mng_CheckIfPageOwned (&temp_pl,TableUser);
	if (r<0)
		OutrageMessageBox (ErrorString);
	else if (r==0)
		OutrageMessageBox (InfoString);
	else 
	{
		// Change the pagelock state to UNLOCKED
		strcpy (temp_pl.holder,"UNLOCKED");
		if (!mng_ReplacePagelock (temp_pl.name,&temp_pl))
		{
			MessageBox (ErrorString,"Error!");
			mng_EraseLocker();
			return;
		}
		else
		{
			// Now actually replace the copy on the net with our local one

			if (!mng_ReplaceRobotPage (Robots[n].name,n,0))
				OutrageMessageBox (ErrorString);
			else
			{
				// Save this robot anim/model to the network for all

				char destname[100],srcname[100];
					
				sprintf (srcname,"%s\\%s",LocalModelsDir,Poly_models[Robots[n].render_handle].name);
				sprintf (destname,"%s\\%s",NetModelsDir,Poly_models[Robots[n].render_handle].name);

				cf_CopyFile (destname,srcname);
				
				OutrageMessageBox ("Robot checked in.");

				// Delete it from local pagefile if it is there
				int dret=mng_DeleteRobotPage (Robots[n].name,1);
				ASSERT (dret==1);
				mng_EraseLocker();

				// Free the tracklock
				int p=mng_FindTrackLock (Robots[n].name,PAGETYPE_ROBOT);
				ASSERT (p!=-1);
				mng_FreeTrackLock (p);
				UpdateDialog ();
			}
		}
	}
#endif
}

void CWorldObjectsRobotDialog::OnDeleteRobot() {
#if 0 // LGT: undeclared symbols
	int answer,tl;
	mngs_Pagelock pl;
	int n=D3EditState.current_robot;

	if (Num_robots<1)
		return;

	// Check to see if we even have it locked
	if ((tl=mng_FindTrackLock (Robots[n].name,PAGETYPE_ROBOT))==-1)
	{
		OutrageMessageBox ("This robot is not yours to delete.  Lock first.");
		return;
	}

	// Make sure it is to be deleted
	answer=MessageBox ("Are you sure you want to delete this robot?",Robots[n].name,MB_YESNO);
	if (answer==IDNO)
		return;

	if (!mng_MakeLocker())
	{
		OutrageMessageBox ("Either you have no network support, or someone else has exclusive access to the table files (retry in a 5 seconds)");
		return;
	}
	

	strcpy (pl.name,Robots[n].name);
	pl.pagetype=PAGETYPE_ROBOT;

	// Check to see if this is a local robot only.  If so, only delete it locally
	if (mng_CheckIfPageOwned (&pl,TableUser)!=1)
	{
		mng_FreeTrackLock (tl);
		if (!mng_DeleteRobotPage (Robots[n].name,1))
		{
			mprintf(0,ErrorString);
			Int3();
		}
	}
	else // if it is network, delete it from both the net and local drives
	{
		
		mng_FreeTrackLock (tl);
		mng_DeleteRobotPage (Robots[n].name,1);
		mng_DeleteRobotPage (Robots[n].name,0);
		mng_DeletePagelock (Robots[n].name,PAGETYPE_ROBOT);
	}

	// Advance to the next robot
	D3EditState.current_robot=GetNextRobot (n);

	FreePolyModel (Robots[n].render_handle);
	FreeRobot (n);
	mng_EraseLocker();
		
	OutrageMessageBox ("Robot deleted.");
	
	RemapRobots();

	UpdateDialog ();
#endif
}

void CWorldObjectsRobotDialog::OnLockRobot() {
#if 0 // LGT: undeclared symbols
	int n=D3EditState.current_robot;
	mngs_Pagelock temp_pl;
	mngs_robot_page robotpage;
	int r;

	if (Num_robots<1)
		return;

	if (!mng_MakeLocker())
	{
		OutrageMessageBox ("Either you have no network support, or someone else has exclusive access to the table files (retry in a 5 seconds)");
		return;
	}

	// Make sure it can be locked
	strcpy (temp_pl.name,Robots[n].name);
	temp_pl.pagetype=PAGETYPE_ROBOT;
	
	r=mng_CheckIfPageLocked (&temp_pl);
	if (r==2)
	{
		int answer;
		answer=OutrageMessageBox (MBOX_YESNO, "This page is not even in the table file, or the database maybe corrupt.  Override to 'Unlocked'? (Select NO if you don't know what you're doing)");
		if (answer==1)
		{
			strcpy (temp_pl.holder,"UNLOCKED");
			if (!mng_ReplacePagelock (temp_pl.name,&temp_pl))
				MessageBox (ErrorString,"Error!");
				
		}
	}
	else if (r<0)
		OutrageMessageBox (ErrorString);
	else if (r==1)
		OutrageMessageBox (InfoString);
	else 
	{

		// Everything is ok.  Tell the network we're locking it and get a copy to
		// our local drive

		strcpy (temp_pl.holder,TableUser);
		if (!mng_ReplacePagelock (temp_pl.name,&temp_pl))
		{
			MessageBox (ErrorString,"Error!");
			mng_EraseLocker();
			return;
		}
		else
		{
			// Search thru the net pagefile and get a new copy in RAM in case anyone
			// changed it since we started the editor
			if (mng_FindSpecificRobotPage (temp_pl.name,&robotpage))
			{
				if (mng_AssignRobotPageToRobot (&robotpage,n))
				{
					if (!mng_ReplaceRobotPage (Robots[n].name,n,1))
					{
						OutrageMessageBox ("There was problem writing that page locally!");
						mng_EraseLocker();
						return;
					}	
					OutrageMessageBox ("Robot locked.");
				}
				else
					OutrageMessageBox ("There was a problem loading this robot.  You might encounter problems in dealing with it.  Good luck!");
				
				mng_AllocTrackLock(Robots[n].name,PAGETYPE_ROBOT);
				UpdateDialog ();
			}
			else
				OutrageMessageBox ("Couldn't find that robot in the table file!");
		}
	}
	mng_EraseLocker();
#endif
}

void CWorldObjectsRobotDialog::OnNextRobot() {
#if 0 // LGT: undeclared symbols
	D3EditState.current_robot=GetNextRobot(D3EditState.current_robot);
	UpdateDialog();
#endif
}

void CWorldObjectsRobotDialog::OnPrevRobot() {
#if 0 // LGT: undeclared symbols
	D3EditState.current_robot=GetPrevRobot(D3EditState.current_robot);
	UpdateDialog();
#endif
}

void CWorldObjectsRobotDialog::OnRobotModel() {
#if 0 // LGT: undeclared symbols
	char filename[255];
	char curname[255];
	int img_handle;
	int robot_handle;
	int c=1,finding_name=1;
	int model=0;

	// Get the filename of the representing image
	
	CString filter = "Descent III files (*.pof,*.oof)|*.pof;*.oof||";

	CFileDialog dlg_open(TRUE, 0,0, OFN_FILEMUSTEXIST, (LPCTSTR)filter, this);

	if (dlg_open.DoModal() == IDCANCEL) return;

//	Okay, we selected a file. Lets do what needs to be done here.
	
	strcpy (filename,dlg_open.GetPathName());
	img_handle=LoadRobotImage (filename);
	
	if (img_handle<0)
	{
		OutrageMessageBox ("Couldn't open that animation/model file.");
		return;
	}

	
	robot_handle=D3EditState.current_robot;

	Robots[robot_handle].render_handle=img_handle;

	// Finally, save a local copy of the model

	sprintf (curname,"%s\\%s",LocalModelsDir,Poly_models[Robots[robot_handle].render_handle].name);
	cf_CopyFile (curname,filename);
	
	UpdateDialog();
#endif
}

void CWorldObjectsRobotDialog::OnKillfocusRobotNameEdit() {
#if 0 // LGT: undeclared symbols
	CEdit *ebox;
	int n=D3EditState.current_robot;
	char name[PAGENAME_LEN];
	mngs_Pagelock pl;

	// Make sure we have this robot locked, if not reset name and bail
	int p=mng_FindTrackLock (Robots[n].name,PAGETYPE_ROBOT);
	if (p==-1)
	{
		OutrageMessageBox ("You must lock this robot if you wish to change its name.");
		CEdit *ebox=(CEdit *) GetDlgItem (IDC_ROBOT_NAME_EDIT);
		ebox->SetWindowText (Robots[n].name);
		return;
	}	

	// Get the new name
	ebox=(CEdit *) GetDlgItem (IDC_ROBOT_NAME_EDIT);
	ebox->GetWindowText (name,PAGENAME_LEN);


	// Check to see if there is already a robot with this name.  If so, reset and bail
	if (FindRobotName(name)!=-1)
	{
		OutrageMessageBox ("There already is a robot with that name...choose another name.");

		CEdit *ebox=(CEdit *) GetDlgItem (IDC_ROBOT_NAME_EDIT);
		ebox->SetWindowText (Robots[n].name);
		return;
	
	}

		
	if (!mng_MakeLocker())
	{
		OutrageMessageBox ("Either you have no network support, or someone else has exclusive access to the table files (retry in a 5 seconds)");
		return;
	}

	// Check to see if this page exists on the network.  If so, we have to 
	// rename it so we can prevent havoc 
	strcpy (pl.name,Robots[n].name);
	pl.pagetype=PAGETYPE_ROBOT;

	int ret=mng_CheckIfPageOwned (&pl,TableUser);
	if (ret<0)
		OutrageMessageBox (ErrorString);
	else if (ret==1)
		mng_RenamePage (Robots[n].name,name,PAGETYPE_ROBOT);
	else if (ret==2)
	{
		// This page has never been checked in, replace only local copy
		
		char oldname[PAGENAME_LEN];
		strcpy (oldname,Robots[n].name);
		strcpy (Robots[n].name,name);
		mng_ReplaceRobotPage (oldname,n,1);
	}
	else if (ret==0)
	{
		OutrageMessageBox ("You don't own this page.  Get Jason now!");
		mng_FreeTrackLock (p);
		return;
	}
	
		// Finally, copy our new name to the appropriate arrays	
	strcpy (GlobalTrackLocks[p].name,name);
	strcpy (Robots[n].name,name);
	mng_EraseLocker();

	RemapRobots();
	UpdateDialog();
#endif
}

void CWorldObjectsRobotDialog::OnSelendokRobotPulldown() {
#if 0 // LGT: undeclared symbols
	int i,cur;
	char name[200];

	cur=SendDlgItemMessage( IDC_ROBOT_PULLDOWN, CB_GETCURSEL,0,0);
	SendDlgItemMessage( IDC_ROBOT_PULLDOWN, CB_GETLBTEXT,cur,(LPARAM) (LPCTSTR)name);
	
	i=FindRobotName (name);
		
	if (i==-1)
	{
		mprintf(0,"Possible corrupted robot list, but probably nothing.");
		UpdateDialog();
	}

	D3EditState.current_robot=i;
	UpdateDialog();
#endif
}

void CWorldObjectsRobotDialog::UpdateDialog() {
#if 0
	CEdit *ebox;
	CButton *bbox;
	int n=D3EditState.current_robot;
	char str[16];

	if (Num_robots<1)
	{
		bbox=(CButton *) GetDlgItem (IDC_NEXT_ROBOT);
		bbox->EnableWindow (FALSE);
		bbox=(CButton *) GetDlgItem (IDC_PREV_ROBOT);
		bbox->EnableWindow (FALSE);

		return;
	}
	else
	{
		bbox=(CButton *) GetDlgItem (IDC_NEXT_ROBOT);
		bbox->EnableWindow (TRUE);
		bbox=(CButton *) GetDlgItem (IDC_PREV_ROBOT);
		bbox->EnableWindow (TRUE);
	}

	if (!Robots[n].used)
		n=D3EditState.current_robot=GetNextRobot(n);

	
	ebox=(CEdit *) GetDlgItem (IDC_ROBOT_NAME_EDIT);
	ebox->SetWindowText (Robots[n].name);

	ebox=(CEdit *) GetDlgItem (IDC_ROBOT_MODEL_NAME_EDIT);
	ebox->SetWindowText (Poly_models[Robots[n].render_handle].name);	

	bbox = (CButton *)GetDlgItem(IDC_ROBOT_USES_PHYSICS);
	bbox->SetCheck(Robots[n].flags & OIF_USES_PHYSICS);
	((CButton *)GetDlgItem(IDC_ROBOT_EDIT_PHYSICS))->EnableWindow(Robots[n].flags & OIF_USES_PHYSICS);

	bbox = (CButton *)GetDlgItem(IDC_ROBOT_USES_AI);
	bbox->SetCheck(Robots[n].flags & OIF_CONTROL_AI);
	((CButton *)GetDlgItem(IDC_ROBOT_EDIT_AI))->EnableWindow(Robots[n].flags & OIF_CONTROL_AI);

	bbox=(CButton *) GetDlgItem (IDC_CHECKIN_ROBOT);
	if (mng_FindTrackLock (Robots[n].name,PAGETYPE_ROBOT)==-1)
	{
		bbox->EnableWindow (FALSE);
		bbox=(CButton *) GetDlgItem (IDC_LOCK_ROBOT);
		bbox->EnableWindow (TRUE);
	}
	else
	{
		bbox->EnableWindow (TRUE);
		bbox=(CButton *) GetDlgItem (IDC_LOCK_ROBOT);
		bbox->EnableWindow (FALSE);
	}

	sprintf(str, "%d", Robots[n].script_handle);
	ebox = (CEdit *)GetDlgItem(IDC_SCRIPTID);
	ebox->SetWindowText(str);
	
	SendDlgItemMessage( IDC_ROBOT_PULLDOWN, CB_RESETCONTENT,0,0);

	for (int i=0;i<MAX_ROBOTS;i++)
	{
		if (Robots[i].used)
			SendDlgItemMessage( IDC_ROBOT_PULLDOWN, CB_ADDSTRING,0,(LPARAM) (LPCTSTR) Robots[i].name);
	}
	SendDlgItemMessage( IDC_ROBOT_PULLDOWN, CB_SELECTSTRING,0,(LPARAM) (LPCTSTR) Robots[n].name);

	UpdateRobotView();
#endif
}

#define NUM_ANIM_FRAMES 30

void CWorldObjectsRobotDialog::UpdateRobotView(void) {
#if 0
	int n=D3EditState.current_robot;
	CWnd *robotwnd;
	RECT rect;
	int x, y, bm_handle,w,h;
	static int frame=0,spin_frame=0;
	static last_robot=-1;
	static last_render_handle=-1;
	static robot_dir=0;
	
	if (Num_robots<1)
		return;

	if (robot_dir==0)
	{
		frame++;
		if (frame==NUM_ANIM_FRAMES)
		{
			frame=NUM_ANIM_FRAMES-1;
			robot_dir=1;
		}
	}
	else
	{
		frame--;
		if (frame==-1)
		{
			frame=0;
			robot_dir=0;
		}
	}



	robotwnd = GetDlgItem(IDC_ROBOTVIEW);
	robotwnd->GetWindowRect(&rect);
	ScreenToClient(&rect);

	Desktop_surf->attach_to_window((unsigned)m_hWnd);
	
	w=rect.right-rect.left;
	h=rect.bottom-rect.top;

	if (last_robot!=n || last_render_handle!=Robots[n].render_handle)
	{
		Desktop_surf->clear(rect.left,rect.top,w,h);
		last_robot=n;
		last_render_handle=Robots[n].render_handle;
	}

	bm_handle=GetRobotImage (n);

	vector zero_vector;
	vector view_vector={0,0,-20};
	matrix id_matrix,rot_matrix;
	poly_model *pm=&Poly_models[bm_handle];

	vm_MakeZero (&zero_vector);
	vm_MakeIdentity (&id_matrix);

	float norm_angles[30];

	for (int i=0;i<30;i++)
		norm_angles[i]=(float)frame/(float)NUM_ANIM_FRAMES;

	spin_frame++;

	vm_AnglesToMatrix (&rot_matrix,0,spin_frame*400,0);
	view_vector.z=-(fabs(pm->maxs.z-pm->mins.z)*2);

	m_RobotSurf.create(128, 128, BPP_16);
	grViewport *vport=new grViewport (&m_RobotSurf);
	g3_StartFrame (vport,&view_vector,&id_matrix,D3_DEFAULT_ZOOM);
	DrawPolygonModel (&zero_vector,&rot_matrix,bm_handle,norm_angles,0,1.0);
	g3_EndFrame();

	x = rect.left + ((rect.right-rect.left)/2) - m_RobotSurf.width()/2; 
	y = rect.top + ((rect.bottom-rect.top)/2) - m_RobotSurf.height()/2;
	Desktop_surf->blt(x, y, &m_RobotSurf);

	delete vport;
	m_RobotSurf.free();
#endif
}

BOOL CWorldObjectsRobotDialog::OnInitDialog() {
#if 0 // LGT: undeclared symbols
	CDialog::OnInitDialog();
	
	CWnd::SetTimer(1,50,NULL);

	UpdateDialog();
#endif
  return TRUE; // return TRUE unless you set the focus to a control
               // EXCEPTION: OCX Property Pages should return FALSE
}

void CWorldObjectsRobotDialog::OnTimer(UINT nIDEvent) {
#if 0 // LGT: undeclared symbols
	UpdateRobotView();
	
	CDialog::OnTimer(nIDEvent);
#endif
}

void CWorldObjectsRobotDialog::OnKillfocusRobotSizeEdit() {
#if 0 // LGT: undeclared symbols
	CEdit *ebox;
	char str[20];
	int n=D3EditState.current_robot;
	
	ebox=(CEdit *) GetDlgItem (IDC_ROBOT_SIZE_EDIT);
	ebox->GetWindowText (str,20);

	Robots[n].size = atof (str);
#endif
}

void CWorldObjectsRobotDialog::OnRobotsOut() {
#if 0 // LGT: undeclared symbols
	char str[10000];
	int total=0;

	mng_DisplayLockList (TableUser);

	sprintf (str,"User %s has these robots held locally:\n\n",TableUser);
	for (int i=0;i<MAX_TRACKLOCKS;i++)
	{
		if (GlobalTrackLocks[i].used && GlobalTrackLocks[i].pagetype==PAGETYPE_ROBOT)
		{
			char temp[90];

			sprintf (temp,"%s",GlobalTrackLocks[i].name);
			strcat (str,temp);
			strcat (str,"\n");
			total++;
		}
	}

	if (total!=0)
	{
		// Display that string
		MessageBox (str,"Robots",MB_OK);
	}
#endif
}
// When closing, save all our checked out robots locally so we know
// what stuff to flag as "checked out" the next time we start up
void CWorldObjectsRobotDialog::SaveRobotsOnClose() {
#if 0 // LGT: undeclared symbols
	int i,t;


	if (!Network_up)
		return;			// don't save a damn thing if the network is down
		

	for (i=0;i<MAX_TRACKLOCKS;i++)
	{
		if (GlobalTrackLocks[i].used==1 && GlobalTrackLocks[i].pagetype==PAGETYPE_ROBOT)
		{	
			t=FindRobotName(GlobalTrackLocks[i].name);
			ASSERT (t!=-1);
			mng_ReplaceRobotPage (Robots[t].name,t,1);
		}
	}
#endif
}

BOOL CWorldObjectsRobotDialog::DestroyWindow() {
  SaveRobotsOnClose();

  return CDialog::DestroyWindow();
}

void CWorldObjectsRobotDialog::OnDefineAnimstates() {
#if 0 // LGT: used undefined
	int n=D3EditState.current_robot;

	if (!Robots[n].used)
		return;

	CAnimStatesDialog dlg(Robots[n].render_handle,Robots[n].anim);

	if (dlg.DoModal() == IDOK)		//Copy back into object
		dlg.GetAnimData(Robots[n].anim);
#endif
}

BOOL CWorldObjectsRobotDialog::OnHelpInfo(HELPINFO *pHelpInfo) {
  // TODO: Add your message handler code here and/or call default
  WinHelp(HID_ROBOTSDLG, HELP_CONTEXT);
  return TRUE;
  // return CDialog::OnHelpInfo(pHelpInfo);
}

void CWorldObjectsRobotDialog::OnRobotUsesPhysics() {
#if 0 // LGT: undeclared symbols
	int n=D3EditState.current_robot;
	CButton *btn;
	
	btn = (CButton *)GetDlgItem(IDC_ROBOT_USES_PHYSICS);

	if (btn->GetCheck())
		Robots[n].flags |= OIF_USES_PHYSICS;
	else
		Robots[n].flags &= ~OIF_USES_PHYSICS;

	((CButton *)GetDlgItem(IDC_ROBOT_EDIT_PHYSICS))->EnableWindow(Robots[n].flags & OIF_USES_PHYSICS);
#endif
}

void CWorldObjectsRobotDialog::OnRobotEditPhysics() {
#if 0 // LGT: CopyPhysicsData undefined
	CPhysicsDlg dlg;

	CopyPhysicsData(&dlg,&Robots[D3EditState.current_robot].phys_info);

	if (dlg.DoModal() == IDOK)
		CopyPhysicsData(&Robots[D3EditState.current_robot].phys_info,&dlg);
#endif
}

void CWorldObjectsRobotDialog::OnRobotUsesAI() {
#if 0 // LGT: undeclared symbols
	int n=D3EditState.current_robot;
	CButton *btn;
	
	btn = (CButton *)GetDlgItem(IDC_ROBOT_USES_AI);

	if (btn->GetCheck())
		Robots[n].flags |= OIF_CONTROL_AI;
	else
		Robots[n].flags &= ~OIF_CONTROL_AI;

	((CButton *)GetDlgItem(IDC_ROBOT_EDIT_AI))->EnableWindow(Robots[n].flags & OIF_CONTROL_AI);
#endif
}

void CWorldObjectsRobotDialog::OnRobotEditAI() {
#if 0 // LGT: CopyAIData undefined
	CAISettingsDlg dlg;

	CopyAIData(&dlg,&Robots[D3EditState.current_robot].ai_info);

	if (dlg.DoModal() == IDOK)
		CopyAIData(&Robots[D3EditState.current_robot].ai_info,&dlg);
#endif
}

void CWorldObjectsRobotDialog::OnKillfocusScriptid() {
#if 0 // LGT: script_handle undefined
//	really all this does is set the script item in memory.
	CEdit *edit = (CEdit *)GetDlgItem(IDC_SCRIPTID);
	char buf[8];

	edit->GetWindowText(buf, sizeof(buf));
	Robots[D3EditState.current_robot].script_handle = atoi(buf);
#endif
}
