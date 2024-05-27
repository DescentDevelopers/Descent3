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

 * $Logfile: /DescentIII/Main/editor/WorldObjectsPowerupDialog.cpp $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:57:39 $
 * $Author: kevinb $
 *
 * WorldObjectsPowerupDialog.cpp : implementation file
 *
 * $Log: not supported by cvs2svn $
 * 
 * 6     8/08/97 2:25p Matt
 * Took out error messages which are now handled by mng_MakeLocker()
 * 
 * 5     8/06/97 12:48p Jason
 * fixed dumb function naming error
 * 
 * 4     8/06/97 12:40p Jason
 * fixed some potentially serious memory problems
 * 
 * 3     7/24/97 6:11p Matt
 * Created symbolic constant for default zoom, and used it every place
 * that specifies zoom
 *
 * $NoKeywords: $
 */

#include "stdafx.h"
#include "editor.h"
#include "WorldObjectsPowerupDialog.h"
#include "powerup.h"
#include "manage.h"
#include "powerpage.h"
#include "mono.h"
#include "vclip.h"
#include "polymodel.h"
#include "soundload.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWorldObjectsPowerupDialog dialog


CWorldObjectsPowerupDialog::CWorldObjectsPowerupDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CWorldObjectsPowerupDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWorldObjectsPowerupDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CWorldObjectsPowerupDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWorldObjectsPowerupDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWorldObjectsPowerupDialog, CDialog)
	//{{AFX_MSG_MAP(CWorldObjectsPowerupDialog)
	ON_BN_CLICKED(IDC_ADD_NEW_POWERUP, OnAddNewPowerup)
	ON_BN_CLICKED(IDC_CHECKIN, OnCheckin)
	ON_BN_CLICKED(IDC_DELETE_POWERUP, OnDeletePowerup)
	ON_BN_CLICKED(IDC_LOCK_POWERUP, OnLockPowerup)
	ON_BN_CLICKED(IDC_NEXT_POWERUP, OnNextPowerup)
	ON_BN_CLICKED(IDC_PREV, OnPrev)
	ON_BN_CLICKED(IDC_POWERUPS_OUT, OnPowerupsOut)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_EN_KILLFOCUS(IDC_POWER_NAME_EDIT, OnKillfocusPowerNameEdit)
	ON_EN_KILLFOCUS(IDC_POWERUP_SIZE_EDIT, OnKillfocusPowerupSizeEdit)
	ON_CBN_SELENDOK(IDC_POWER_PULLDOWN, OnSelendokPowerPulldown)
	ON_BN_CLICKED(IDC_LOAD_IMAGE, OnLoadImage)
	ON_BN_CLICKED(IDC_GRAVITY_CHECK, OnGravityCheck)
	ON_BN_CLICKED(IDC_MAGNETISM_CHECK, OnMagnetismCheck)
	ON_BN_CLICKED(IDC_WIND_CHECK, OnWindCheck)
	ON_EN_KILLFOCUS(IDC_POW_MASS_EDIT, OnKillfocusPowMassEdit)
	ON_EN_KILLFOCUS(IDC_POW_DRAG_EDIT, OnKillfocusPowDragEdit)
	ON_CBN_SELENDOK(IDC_POWERUP_AMBIENT_SOUND_LIST, OnSelendokPowerupAmbientSoundList)
	ON_CBN_SELENDOK(IDC_POWERUP_PICKUP_SOUND_LIST, OnSelendokPowerupPickupSoundList)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWorldObjectsPowerupDialog message handlers


// This function allocates a newpower, has the user pick the image to display the
// powerup, and allocs a tracklock to keep track of it
void CWorldObjectsPowerupDialog::OnAddNewPowerup() 
{
	char filename[255];
	char cur_name[100];
	int img_handle;
	int powerup_handle;
	int c=1,finding_name=1;
	int model=0;

	if (!Network_up)
	{
		OutrageMessageBox ("Sorry babe, the network is down.  This action is a no-no.\n");
		return;
	}

	// Get the filename of the representing image
	
	CString filter = "Descent III files (*.oaf,*.ilf,*.abm,*.pof,*.oof)|*.oaf;*.ifl;*.abm;*.pof;*.oof||";

	CFileDialog dlg_open(TRUE, 0,0, OFN_FILEMUSTEXIST, (LPCTSTR)filter, this);

	if (dlg_open.DoModal() == IDCANCEL) return;

//	Okay, we selected a file. Lets do what needs to be done here.
	
	strcpy (filename,dlg_open.GetPathName());
	img_handle=LoadPowerupImage (filename,&model);
	
	if (img_handle<0)
	{
		OutrageMessageBox ("Couldn't open that animation/model file.");
		return;
	}

	// Alloc a powerup and give a name not already taken by some other powerup

	powerup_handle=AllocPowerup();

	while (finding_name)
	{
		sprintf (cur_name,"%s%d",dlg_open.GetFileName(),c);
		if (FindPowerupName (cur_name)!=-1)
			c++;
		else
			finding_name=0;
	}	
	
	strcpy (Powerups[powerup_handle].name,cur_name);

	// If its a 3d model instead of an animation, flag it

	if (!model)
		Powerups[powerup_handle].flags|=PF_IMAGE_BITMAP;
	else
		Powerups[powerup_handle].flags&=~PF_IMAGE_BITMAP;
		
	
	Powerups[powerup_handle].image_handle=img_handle;

	// Finally, save a local copy of the model/anim and alloc a tracklock
	mprintf(0,"Making a copy of this model/animation locally...\n");

	if (model)
	{
		char destname[100];
		sprintf (destname,"%s\\%s",LocalModelsDir,Poly_models[Powerups[powerup_handle].image_handle].name);
		if (stricmp (destname,filename))	// only copy if they are different
			cf_CopyFile (destname,filename);
	}
	else 
	{
		
		sprintf (filename,"%s\\%s",LocalManageGraphicsDir,GameVClips[Powerups[powerup_handle].image_handle].name);
		SaveVClip (filename,Powerups[powerup_handle].image_handle);
	}
		
	mng_AllocTrackLock (cur_name,PAGETYPE_POWERUP);

	D3EditState.current_powerup=powerup_handle;

	RemapPowerups();
	
	UpdateDialog ();

}

// This functions saves the current powerup to the net
void CWorldObjectsPowerupDialog::OnCheckin() 
{
	int n=D3EditState.current_powerup;
	mngs_Pagelock temp_pl;
	int r;

	if (Num_powerups<1)
		return;

	if (!mng_MakeLocker())
		return;

	// Make sure we own this powerup
	strcpy (temp_pl.name,Powerups[n].name);
	temp_pl.pagetype=PAGETYPE_POWERUP;
	
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

			if (!mng_ReplacePowPage (Powerups[n].name,n,0))
				OutrageMessageBox (ErrorString);
			else
			{
				// Save this powerups anim/model to the network for all

				char fname[255];
				
				if (Powerups[n].flags & PF_IMAGE_BITMAP)
				{
					sprintf (fname,"%s\\%s",ManageGraphicsDir,GameVClips[Powerups[n].image_handle].name);
					SaveVClip (fname,Powerups[n].image_handle);

					sprintf (fname,"%s\\%s",LocalManageGraphicsDir,GameVClips[Powerups[n].image_handle].name);
					SaveVClip (fname,Powerups[n].image_handle);
				}
				else
				{
					char destname[100],srcname[100];
					
					sprintf (srcname,"%s\\%s",LocalModelsDir,Poly_models[Powerups[n].image_handle].name);
					sprintf (destname,"%s\\%s",NetModelsDir,Poly_models[Powerups[n].image_handle].name);

					cf_CopyFile (destname,srcname);
				}

				OutrageMessageBox ("Powerup checked in.");

				// Delete it from local pagefile if its there
				int dret=mng_DeletePowPage (Powerups[n].name,1);
				ASSERT (dret==1);
				mng_EraseLocker();

				// Free the tracklock
				int p=mng_FindTrackLock (Powerups[n].name,PAGETYPE_POWERUP);
				ASSERT (p!=-1);
				mng_FreeTrackLock (p);
				UpdateDialog ();
			}
		}
	}
}

// Deletes the current powerup 
// If the user has it checked out, will delete it from the net pagefile
// If its local only, it will get deleted from the local pagefile
void CWorldObjectsPowerupDialog::OnDeletePowerup() 
{
	int answer,tl;
	mngs_Pagelock pl;
	int n=D3EditState.current_powerup;

	if (Num_powerups<1)
		return;

	// Check to see if we even have it locked
	if ((tl=mng_FindTrackLock (Powerups[n].name,PAGETYPE_POWERUP))==-1)
	{
		OutrageMessageBox ("This powerup is not yours to delete.  Lock first.");
		return;
	}

	//Make sure its to be deleted
	answer=MessageBox ("Are you sure you want to delete this powerup?",Powerups[n].name,MB_YESNO);
	if (answer==IDNO)
		return;

	if (!mng_MakeLocker())
		return;

	strcpy (pl.name,Powerups[n].name);
	pl.pagetype=PAGETYPE_POWERUP;

	// Check to see if this is a local powerup only.  If so, only delete it locally
	if (mng_CheckIfPageOwned (&pl,TableUser)!=1)
	{
		mng_FreeTrackLock (tl);
		if (!mng_DeletePowPage (Powerups[n].name,1))
		{
			mprintf(0,ErrorString);
			Int3();
		}
	}
	else // if its network, delete it from both the net and local drives
	{
		mng_FreeTrackLock (tl);
		mng_DeletePowPage (Powerups[n].name,1);
		mng_DeletePowPage (Powerups[n].name,0);
		mng_DeletePagelock (Powerups[n].name,PAGETYPE_POWERUP);
	}

	// Advance to the next powerup
	D3EditState.current_powerup=GetNextPowerup (n);

	if (Powerups[n].flags & PF_IMAGE_BITMAP)
		FreeVClip (Powerups[n].image_handle);
	else
		FreePolyModel (Powerups[n].image_handle);

	FreePowerup (n);
	mng_EraseLocker();
	RemapPowerups();
	
	OutrageMessageBox ("Powerup deleted.");
	UpdateDialog ();

}

// This functions locks a powerup from the net.  
void CWorldObjectsPowerupDialog::OnLockPowerup() 
{
	int n=D3EditState.current_powerup;
	mngs_Pagelock temp_pl;
	mngs_power_page powpage;
	int r;

	if (Num_powerups<1)
		return;

	if (!mng_MakeLocker())
		return;

	// Make sure it can be locked
	strcpy (temp_pl.name,Powerups[n].name);
	temp_pl.pagetype=PAGETYPE_POWERUP;
	
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
			if (mng_FindSpecificPowPage (temp_pl.name,&powpage))
			{
				if (mng_AssignPowPageToPowerup (&powpage,n))
				{
					if (!mng_ReplacePowPage (Powerups[n].name,n,1))
					{
						OutrageMessageBox ("There was problem writing that page locally!");
						mng_EraseLocker();
						return;
					}	
					OutrageMessageBox ("Powerup locked.");
				}

				else
					OutrageMessageBox ("There was a problem loading this powerup.  You might encounter problems in dealing with it.  Good luck!");
				
				mng_AllocTrackLock(Powerups[n].name,PAGETYPE_POWERUP);
				UpdateDialog ();
			}
			else
				OutrageMessageBox ("Couldn't find that powerup in the table file!");
		}
	}
	mng_EraseLocker();

	
}

// Gets the next powerup and sets it as the current one
void CWorldObjectsPowerupDialog::OnNextPowerup() 
{
	D3EditState.current_powerup=GetNextPowerup(D3EditState.current_powerup);
	UpdateDialog();
	
}

// Gets the previous powerup and sets it as the current one
void CWorldObjectsPowerupDialog::OnPrev() 
{

	D3EditState.current_powerup=GetPrevPowerup(D3EditState.current_powerup);
	UpdateDialog();
}

// This simply gets a list of all the pages we have checked out
void CWorldObjectsPowerupDialog::OnPowerupsOut() 
{
	char str[10000];
	int total=0;

	mng_DisplayLockList (TableUser);
	
	sprintf (str,"User %s has these powerups held locally:\n\n",TableUser);
	for (int i=0;i<MAX_TRACKLOCKS;i++)
	{
		if (GlobalTrackLocks[i].used && GlobalTrackLocks[i].pagetype==PAGETYPE_POWERUP)
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
		MessageBox (str,"Powerups",MB_OK);
	}


}

									
// When closing, save all our checked out powerups locally so we know 
// what stuff to flag as "checked out" the next time we start up
void CWorldObjectsPowerupDialog::SavePowerupsOnClose()
{
	int i,t;


	if (!Network_up)
		return;			// don't save a damn thing if the network is down
		

	for (i=0;i<MAX_TRACKLOCKS;i++)
	{
		if (GlobalTrackLocks[i].used==1 && GlobalTrackLocks[i].pagetype==PAGETYPE_POWERUP)
		{	
			t=FindPowerupName(GlobalTrackLocks[i].name);
			ASSERT (t!=-1);
			mng_ReplacePowPage (Powerups[t].name,t,1);
		}
	}
}


void CWorldObjectsPowerupDialog::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	UpdateDialog();
	
	
}

// Redraw the dialog based on our current powerup
void CWorldObjectsPowerupDialog::UpdateDialog(void)
{
	CEdit *ebox;
	CButton *bbox;
	int n=D3EditState.current_powerup;
	char str[50];
	int i;

	if (!Powerups[n].used)
		n=D3EditState.current_powerup=GetNextPowerup(n);

	if (Num_powerups<1)
	{
		bbox=(CButton *) GetDlgItem (IDC_NEXT_POWERUP);
		bbox->EnableWindow (FALSE);
		bbox=(CButton *) GetDlgItem (IDC_PREV);
		bbox->EnableWindow (FALSE);

		return;
	}
	else
	{
		bbox=(CButton *) GetDlgItem (IDC_NEXT_POWERUP);
		bbox->EnableWindow (TRUE);
		bbox=(CButton *) GetDlgItem (IDC_PREV);
		bbox->EnableWindow (TRUE);
	}

	ebox=(CEdit *) GetDlgItem (IDC_POWER_NAME_EDIT);
	ebox->SetWindowText (Powerups[n].name);

	ebox=(CEdit *) GetDlgItem (IDC_POWER_IMAGE_EDIT);

	if (Powerups[n].flags & PF_IMAGE_BITMAP)
		ebox->SetWindowText (GameVClips[Powerups[n].image_handle].name);
	else
		ebox->SetWindowText (Poly_models[Powerups[n].image_handle].name);	

	bbox=(CButton *) GetDlgItem (IDC_CHECKIN);
	if (mng_FindTrackLock (Powerups[n].name,PAGETYPE_POWERUP)==-1)
	{
		bbox->EnableWindow (FALSE);
		bbox=(CButton *) GetDlgItem (IDC_LOCK_POWERUP);
		bbox->EnableWindow (TRUE);
	}
	else
	{
		bbox->EnableWindow (TRUE);
		bbox=(CButton *) GetDlgItem (IDC_LOCK_POWERUP);
		bbox->EnableWindow (FALSE);
	}

	SendDlgItemMessage( IDC_POWERUP_AMBIENT_SOUND_LIST, CB_RESETCONTENT,0,0);
	for (i=0;i<MAX_SOUNDS;i++)
	{
		if (Sounds[i].used)
			SendDlgItemMessage( IDC_POWERUP_AMBIENT_SOUND_LIST, CB_ADDSTRING,0,(LPARAM) (LPCTSTR) Sounds[i].name);
	}
	SendDlgItemMessage( IDC_POWERUP_AMBIENT_SOUND_LIST, CB_SELECTSTRING,0,(LPARAM) (LPCTSTR) Sounds[n].name);

	SendDlgItemMessage( IDC_POWERUP_PICKUP_SOUND_LIST, CB_RESETCONTENT,0,0);
	for (i=0;i<MAX_SOUNDS;i++)
	{
		if (Sounds[i].used)
			SendDlgItemMessage( IDC_POWERUP_PICKUP_SOUND_LIST, CB_ADDSTRING,0,(LPARAM) (LPCTSTR) Sounds[i].name);
	}
	if(Powerups[n].sounds[PSI_PICKUP] >= 0 && Powerups[n].sounds[PSI_PICKUP] < MAX_SOUNDS && Sounds[Powerups[n].sounds[PSI_PICKUP]].used != 0)
		SendDlgItemMessage( IDC_POWERUP_PICKUP_SOUND_LIST, CB_SELECTSTRING,0,(LPARAM) (LPCTSTR) Sounds[Powerups[n].sounds[PSI_PICKUP]].name);
	else
		SendDlgItemMessage( IDC_POWERUP_PICKUP_SOUND_LIST, CB_SELECTSTRING,0,(LPARAM) (LPCTSTR) "\0");

	ebox=(CEdit *) GetDlgItem (IDC_POWERUP_SIZE_EDIT);
	sprintf (str,"%f",Powerups[n].size);
	ebox->SetWindowText (str);

	ebox=(CEdit *) GetDlgItem (IDC_POW_MASS_EDIT);
	sprintf (str,"%f",Powerups[n].phys_info.mass);
	ebox->SetWindowText (str);

	ebox=(CEdit *) GetDlgItem (IDC_POW_DRAG_EDIT);
	sprintf (str,"%f",Powerups[n].phys_info.drag);
	ebox->SetWindowText (str);

	bbox = (CButton *)GetDlgItem(IDC_GRAVITY_CHECK);
	bbox->SetCheck(Powerups[n].phys_info.flags & PF_GRAVITY);

	bbox = (CButton *)GetDlgItem(IDC_MAGNETISM_CHECK);
	bbox->SetCheck(Powerups[n].phys_info.flags & PF_MAGNETISM);

	bbox = (CButton *)GetDlgItem(IDC_WIND_CHECK);
	bbox->SetCheck(Powerups[n].phys_info.flags & PF_WIND);

	SendDlgItemMessage( IDC_POWER_PULLDOWN, CB_RESETCONTENT,0,0);

	for (i=0;i<MAX_POWERUPS;i++)
	{
		if (Powerups[i].used)
			SendDlgItemMessage( IDC_POWER_PULLDOWN, CB_ADDSTRING,0,(LPARAM) (LPCTSTR) Powerups[i].name);
	}
	SendDlgItemMessage( IDC_POWER_PULLDOWN, CB_SELECTSTRING,0,(LPARAM) (LPCTSTR) Powerups[n].name);

	UpdatePowerupView();

}

void CWorldObjectsPowerupDialog::UpdatePowerupView(void)
{
	int n=D3EditState.current_powerup;
	CWnd *powwnd;
	RECT rect;
	int x, y, bm_handle,w,h;
	static int frame=0;
	static last_powerup=-1;
	static last_image_handle=-1;
	
	if (Num_powerups<1)
		return;

	frame++;

	powwnd = GetDlgItem(IDC_POWERUPVIEW);
	powwnd->GetWindowRect(&rect);
	ScreenToClient(&rect);

	Desktop_surf->attach_to_window((unsigned)m_hWnd);
	
	w=rect.right-rect.left;
	h=rect.bottom-rect.top;

	if (last_powerup!=n || last_image_handle!=Powerups[n].image_handle)
	{
		Desktop_surf->clear(rect.left,rect.top,w,h);
		last_powerup=n;
		last_image_handle=Powerups[n].image_handle;
	}

	bm_handle=GetPowerupImage (n,frame);

	if (Powerups[n].flags & PF_IMAGE_BITMAP)
	{
		m_PowerupSurf.create(bm_w(bm_handle,0), bm_h(bm_handle,0), bm_bpp(bm_handle));
		m_PowerupSurf.load(bm_handle);

		x = rect.left + ((rect.right-rect.left)/2) - m_PowerupSurf.width()/2; 
		y = rect.top + ((rect.bottom-rect.top)/2) - m_PowerupSurf.height()/2;

		Desktop_surf->blt(x, y, &m_PowerupSurf);
		m_PowerupSurf.free();
	}
	else
	{
		vector zero_vector;
		vector view_vector={0,0,-2};
		matrix id_matrix,rot_matrix;
	
		vm_MakeZero (&zero_vector);
		vm_MakeIdentity (&id_matrix);
		vm_AnglesToMatrix (&rot_matrix,0,frame*400,0);

		m_PowerupSurf.create(128, 128, BPP_16);
		grViewport *vport=new grViewport (&m_PowerupSurf);
		g3_StartFrame (vport,&view_vector,&id_matrix,D3_DEFAULT_ZOOM);
		DrawPolygonModel (&zero_vector,&rot_matrix,bm_handle,NULL,0,1.0);
		g3_EndFrame();

		x = rect.left + ((rect.right-rect.left)/2) - m_PowerupSurf.width()/2; 
		y = rect.top + ((rect.bottom-rect.top)/2) - m_PowerupSurf.height()/2;
		Desktop_surf->blt(x, y, &m_PowerupSurf);

		delete vport;
		m_PowerupSurf.free();
	}
}



BOOL CWorldObjectsPowerupDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CWnd::SetTimer(1,100,NULL);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CWorldObjectsPowerupDialog::OnTimer(UINT nIDEvent) 
{
	UpdatePowerupView ();
	
	CDialog::OnTimer(nIDEvent);
}

BOOL CWorldObjectsPowerupDialog::DestroyWindow() 
{
	

	SavePowerupsOnClose();
	
	return CDialog::DestroyWindow();
}

void CWorldObjectsPowerupDialog::OnKillfocusPowerNameEdit() 
{
	CEdit *ebox;
	int n=D3EditState.current_powerup;
	char name[PAGENAME_LEN];
	mngs_Pagelock pl;

	// Make sure we have this powerup locked, if not reset name and bail
	int p=mng_FindTrackLock (Powerups[n].name,PAGETYPE_POWERUP);
	if (p==-1)
	{
		OutrageMessageBox ("You must lock this powerup if you wish to change its name.");
		CEdit *ebox=(CEdit *) GetDlgItem (IDC_POWER_NAME_EDIT);
		ebox->SetWindowText (Powerups[n].name);
		return;
	}	

	// Get the new name
	ebox=(CEdit *) GetDlgItem (IDC_POWER_NAME_EDIT);
	ebox->GetWindowText (name,PAGENAME_LEN);


	// Check to see if there is already a powerup with this name.  If so, reset and bail
	if (FindPowerupName(name)!=-1)
	{
		OutrageMessageBox ("There already is a powerup with that name...choose another name.");

		CEdit *ebox=(CEdit *) GetDlgItem (IDC_POWER_NAME_EDIT);
		ebox->SetWindowText (Powerups[n].name);
		return;
	
	}

		
	if (!mng_MakeLocker())
		return;

	// Check to see if this page exists on the network.  If so, we have to 
	// rename it so we can prevent havoc 
	strcpy (pl.name,Powerups[n].name);
	pl.pagetype=PAGETYPE_POWERUP;

	int ret=mng_CheckIfPageOwned (&pl,TableUser);
	if (ret<0)
		OutrageMessageBox (ErrorString);
	else if (ret==1)
		mng_RenamePage (Powerups[n].name,name,PAGETYPE_POWERUP);
	else if (ret==2)
	{
		// This page has never been checked in, replace only local copy
		
		char oldname[PAGENAME_LEN];
		strcpy (oldname,Powerups[n].name);
		strcpy (Powerups[n].name,name);
		mng_ReplacePowPage (oldname,n,1);
	}
	else if (ret==0)
	{
		OutrageMessageBox ("You don't own this page.  Get Jason now!");
		mng_FreeTrackLock (p);
		return;
	}
	
	RemapPowerups();

	// Finally, copy our new name to the appropriate arrays	
	strcpy (GlobalTrackLocks[p].name,name);
	strcpy (Powerups[n].name,name);
	UpdateDialog();
	mng_EraseLocker();
}

void CWorldObjectsPowerupDialog::OnKillfocusPowerupSizeEdit() 
{
	CEdit *ebox;
	char str[20];
	int n=D3EditState.current_powerup;
	
	ebox=(CEdit *) GetDlgItem (IDC_POWERUP_SIZE_EDIT);
	ebox->GetWindowText (str,20);

	Powerups[n].size=atof (str);
	
}

void CWorldObjectsPowerupDialog::OnSelendokPowerPulldown() 
{
	int i,cur;
	char name[200];

	cur=SendDlgItemMessage( IDC_POWER_PULLDOWN, CB_GETCURSEL,0,0);
	SendDlgItemMessage( IDC_POWER_PULLDOWN, CB_GETLBTEXT,cur,(LPARAM) (LPCTSTR)name);
	
	i=FindPowerupName (name);
		
	if (i==-1)
	{
		mprintf(0,"Possible corrupted texture list, but probably nothing.");
		UpdateDialog();
	}

	D3EditState.current_powerup=i;
	UpdateDialog();
}

void CWorldObjectsPowerupDialog::OnLoadImage() 
{
	char filename[255];
	char curname[255];
	int img_handle;
	int powerup_handle;
	int c=1,finding_name=1;
	int model=0;

	// Get the filename of the representing image
	
	CString filter = "Descent III files (*.oaf,*.ilf,*.abm,*.pof,*.oof)|*.oaf;*.ifl;*.abm;*.pof;*.oof||";

	CFileDialog dlg_open(TRUE, 0,0, OFN_FILEMUSTEXIST, (LPCTSTR)filter, this);

	if (dlg_open.DoModal() == IDCANCEL) return;

//	Okay, we selected a file. Lets do what needs to be done here.
	
	strcpy (filename,dlg_open.GetPathName());
	img_handle=LoadPowerupImage (filename,&model);
	
	if (img_handle<0)
	{
		OutrageMessageBox ("Couldn't open that animation/model file.");
		return;
	}

	// If its a 3d model instead of an animation, flag it
	
	powerup_handle=D3EditState.current_powerup;

	if (!model)
		Powerups[powerup_handle].flags|=PF_IMAGE_BITMAP;
	else
		Powerups[powerup_handle].flags&=~PF_IMAGE_BITMAP;
		
	
	Powerups[powerup_handle].image_handle=img_handle;

	// Finally, save a local copy of the model/anim and alloc a tracklock

	
	if (model)
	{
		sprintf (curname,"%s\\%s",LocalModelsDir,Poly_models[Powerups[powerup_handle].image_handle].name);
		cf_CopyFile (curname,filename);
	}
	else 
	{
		sprintf (filename,"%s\\%s",LocalManageGraphicsDir,GameVClips[Powerups[powerup_handle].image_handle].name);
		SaveVClip (filename,Powerups[powerup_handle].image_handle);
	}

	UpdateDialog();
	
}

void CWorldObjectsPowerupDialog::OnGravityCheck() 
{
	// TODO: Add your control notification handler code here
	int n=D3EditState.current_powerup;
	CButton *btn;
	
	btn = (CButton *)GetDlgItem(IDC_GRAVITY_CHECK);

	if (btn->GetCheck())
		Powerups[n].phys_info.flags |= PF_GRAVITY;
	else
		Powerups[n].phys_info.flags &= ~PF_GRAVITY;

}

void CWorldObjectsPowerupDialog::OnMagnetismCheck() 
{
	// TODO: Add your control notification handler code here
	int n=D3EditState.current_powerup;
	CButton *btn;
	
	btn = (CButton *)GetDlgItem(IDC_MAGNETISM_CHECK);

	if (btn->GetCheck())
		Powerups[n].phys_info.flags |= PF_MAGNETISM;
	else
		Powerups[n].phys_info.flags &= ~PF_MAGNETISM;

	
}

void CWorldObjectsPowerupDialog::OnWindCheck() 
{
	// TODO: Add your control notification handler code here
	int n=D3EditState.current_powerup;
	CButton *btn;
	
	btn = (CButton *)GetDlgItem(IDC_WIND_CHECK);

	if (btn->GetCheck())
		Powerups[n].phys_info.flags |= PF_WIND;
	else
		Powerups[n].phys_info.flags &= ~PF_WIND;

	
}

void CWorldObjectsPowerupDialog::OnKillfocusPowMassEdit() 
{
	// TODO: Add your control notification handler code here
	CEdit *ebox;
	char str[20];
	int n=D3EditState.current_powerup;
	
	ebox=(CEdit *) GetDlgItem (IDC_POW_MASS_EDIT);
	ebox->GetWindowText (str,20);

	Powerups[n].phys_info.mass = atof (str);
}

void CWorldObjectsPowerupDialog::OnKillfocusPowDragEdit() 
{
	// TODO: Add your control notification handler code here
	CEdit *ebox;
	char str[20];
	int n=D3EditState.current_powerup;
	
	ebox=(CEdit *) GetDlgItem (IDC_POW_DRAG_EDIT);
	ebox->GetWindowText (str,20);

	Powerups[n].phys_info.drag = atof (str);
}

void CWorldObjectsPowerupDialog::OnSelendokPowerupAmbientSoundList() 
{
	// TODO: Add your control notification handler code here
	
}

void CWorldObjectsPowerupDialog::OnSelendokPowerupPickupSoundList() 
{
	// TODO: Add your control notification handler code here
	int cur;
	char name[200];
	int n=D3EditState.current_powerup;

	cur=SendDlgItemMessage( IDC_POWERUP_PICKUP_SOUND_LIST, CB_GETCURSEL,0,0);
	SendDlgItemMessage( IDC_POWERUP_PICKUP_SOUND_LIST, CB_GETLBTEXT,cur,(LPARAM) (LPCTSTR)name);

	Powerups[n].sounds[PSI_PICKUP] = FindSoundName (name);
	//ASSERT(Powerups[n].sounds[PSI_PICKUP] != -1);
}

BOOL CWorldObjectsPowerupDialog::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	// TODO: Add your message handler code here and/or call default
	WinHelp(HID_POWERUPSDLG,HELP_CONTEXT);
	return TRUE;
	//return CDialog::OnHelpInfo(pHelpInfo);
}
