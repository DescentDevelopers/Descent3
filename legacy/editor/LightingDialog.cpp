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

/*
 * $Logfile: /DescentIII/Main/editor/LightingDialog.cpp $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:57:38 $
 * $Author: kevinb $
 *
 * Lighting keypad dialog
 *
 * $Log: not supported by cvs2svn $
 * 
 * 67    9/13/99 12:24p Matt
 * Changed the copy-light-multiple function to work either in the whole
 * mine or the current room.
 * 
 * 66    8/17/99 4:09p Matt
 * Added some accelerator keys, and a function to copy face light
 * multiples.
 * 
 * 65    5/05/99 12:58a Matt
 * Added button to clear the lightmaps on a level.
 * 
 * 64    4/18/99 5:42a Chris
 * Added the FQ_IGNORE_RENDER_THROUGH_PORTALS flag
 * 
 * 63    4/03/99 7:55p Matt
 * Use new keypad update system instead of timer to update.
 * 
 * 62    3/24/99 5:53p Jason
 * added per room ambience settings
 * 
 * 61    3/23/99 5:13p Jason
 * fixed counting of invisible terrain cells
 * 
 * 60    2/18/99 12:32p Jason
 * added room multiplier
 * 
 * 59    2/05/99 4:54p Jason
 * added extra option to reset multiples
 * 
 * 58    1/21/99 12:22p Jason
 * adding strobing and flickering
 * 
 * 57    1/20/99 10:50a Jason
 * added new terrain
 * 
 * 56    1/15/99 3:15p Jason
 * fixed global multilier bug
 * 
 * 55    1/15/99 2:33p Jason
 * added global multiplier
 * 
 * 54    10/06/98 11:48a Jason
 * fixed bug with last rev
 * 
 * 53    10/06/98 11:45a Jason
 * added extra feature for Sean
 * 
 * 52    8/04/98 11:52a Jason
 * fixed stupid bug
 * 
 * 51    8/04/98 11:52a Jason
 * added reverse corona check
 * 
 * 50    7/24/98 4:44p Jason
 * added corona button functionality
 * 
 * 49    6/22/98 12:00p Chris
 * Working on sound system to make it in a nice shape.
 * 
 * 48    6/16/98 3:48p Chris
 * Updated the sound system and added the start of sound streaming
 * 
 * 47    6/05/98 5:22p Jason
 * added volumetric fog
 * 
 * 46    5/26/98 5:56p Jason
 * only render coronas which are flagged
 * 
 * 45    5/22/98 3:28p Jason
 * added specular lighting
 * 
 * 44    5/20/98 5:44p Jason
 * incremental checkin for bumpmapping
 * 
 * 43    4/29/98 2:19p Jason
 * added yes/no question to confirm lighting
 * 
 * 42    4/24/98 12:58p Jason
 * added functions to resave all bitmaps
 * 
 * 41    4/21/98 2:36p Jason
 * added some lighting options
 * 
 * 40    4/02/98 12:23p Jason
 * trimmed some fat from our structures
 * 
 * 39    3/24/98 10:40p Jason
 * made closer lightmap spacing
 * 
 * 38    3/16/98 9:55p Chris
 * Working on optimizing fvi
 * 
 * 37    2/26/98 10:48a Jason
 * fixed floating point/int problem in inputting face light multiplier
 * 
 * 36    2/23/98 7:35p Chris
 * Made the rad. floating point
 * 
 * 35    2/16/98 2:12p Jason
 * fixed bug with lightmaps and door shells
 * 
 * 34    2/13/98 12:57p Jason
 * changes for adjusting light multiples
 * 
 * 33    2/10/98 5:09p Jason
 * added a SetRoomPulse function for settinng pulse parameters for an
 * entire room
 * 
 * 32    2/10/98 3:50p Jason
 * added pulsing walls
 * 
 * 31    2/04/98 8:25p Jason
 * added light multiplier for faces
 * 
 * 30    2/04/98 6:23p Matt
 * Changed object room number to indicate a terrain cell via a flag.  Got
 * rid of the object flag which used to indicate terrain.
 * 
 * 29    1/15/98 7:34p Matt
 * Revamped error checking when computing face normals
 * 
 * 28    1/02/98 5:32p Chris
 * More radical changes to the sound system
 * 
 * 27    12/31/97 12:51p Jason
 * debugged and implementd BSP tree for lighting
 * 
 * 26    12/18/97 12:24p Chris
 * Turned off sound system while doing casting rays.
 * 
 * 25    12/16/97 4:17p Jason
 * added ignore_satellites and ignore terrain
 * 
 * 24    12/10/97 12:23p Jason
 * added "SuperDetail"
 * 
 * 
 * 23    12/05/97 12:34p Jason
 * added ambient light to the terrain
 * 
 * 22    11/28/97 6:41p Jason
 * added functions to blur lightmaps - not very helpful though
 * 
 * 21    11/25/97 12:28p Jason
 * another pass at shadows
 * 
 * 20    11/19/97 12:39p Jason
 * added dynamic terrain button
 * 
 * 19    11/14/97 6:38p Jason
 * added ability to do lighting on a single room
 * 
 * 18    11/14/97 1:13p Jason
 * added better light counting
 * 
 * 17    10/28/97 3:41p Jason
 * added "use hemicubes" option
 * 
 * 16    10/10/97 2:34p Jason
 * tweaked volumetrics
 * 
 * 15    10/10/97 1:10p Jason
 * more support for volumetrics
 * 
 * 14    10/10/97 11:38a Jason
 * put in better volumetric support
 * 
 * 13    9/26/97 11:51a Jason
 * tweaked terrain radiosity
 * 
 * 12    9/25/97 4:47p Jason
 * update alpha edit box via timer
 * 
 * 11    9/23/97 5:53p Jason
 * added more custom options to radiosity
 * 
 * 10    9/09/97 12:22p Matt
 * Renamed a flag
 * 
 * 9     9/04/97 2:54p Jason
 * fixed small radiosity bug that wasn't flagging invisible elements
 * 
 * 8     9/04/97 12:12p Jason
 * added lightmap visibility
 * 
 * 7     9/02/97 12:54p Jason
 * classify faces as alphaed or not
 * 
 * 6     9/02/97 11:47a Jason
 * Got alpha per vertex working
 * 
 * 5     8/28/97 2:59p Jason
 * added confirmation for terrain radiosity
 * 
 * 4     8/27/97 5:19p Jason
 * added "Count lights"
 * 
 * 3     8/26/97 4:36p Jason
 * added terrain radiosity
 * 
 * 2     8/12/97 1:10a Jason
 * added code to support radiosity lighting
 * 
 * 8     6/03/97 4:55p Mark
 * 
 * 8	6/03/97	4:53p	Jeff
 *	Added Context Sensitive Help
 *
 * 7     5/30/97 5:45p Jason
 * changes to support user-definable lighting parameters
 * 
 * 6     5/23/97 10:48a Jason
 * checked in for safekeeping...
 * 
 * 5     2/20/97 2:37p Mark
 * 
 * 4     1/27/97 11:38a Samir
 * Added horizontal scrolling of keypad modeless dialog.
 *
 * $NoKeywords: $
 */


#include "stdafx.h"
#include "editor.h"
#include "LightingDialog.h"
#include "editor_lighting.h"
#include "gametexture.h"
#include "radiosity.h"
#include "hemicube.h"
#include "findintersection.h"
#include "erooms.h"
#include "vecmat.h"
#include "polymodel.h"
#include "lightmap_info.h"
#include "hlsoundlib.h"
#include "lighting.h"
#include "bsp.h"
#include "ddio.h"
#include "vclip.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLightingDialog dialog


CLightingDialog::CLightingDialog(CWnd* pParent /*=NULL*/)
	: CKeypadDialog(CLightingDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLightingDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CLightingDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLightingDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLightingDialog, CDialog)
	//{{AFX_MSG_MAP(CLightingDialog)
	ON_WM_SIZE()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_LITEPAD_SETDEFAULT, OnLitepadSetdefault)
	ON_BN_CLICKED(IDC_LITEPAD_SMOOTHALL, OnLitepadSmoothall)
	ON_EN_KILLFOCUS(IDC_LIGHT_ITERATION_EDIT, OnKillfocusLightIterationEdit)
	ON_WM_HELPINFO()
	ON_BN_CLICKED(IDC_LITEPAD_TERRAIN_RADIOSITY, OnLitepadTerrainRadiosity)
	ON_BN_CLICKED(IDC_LITEPAD_COUNT_LIGHTS, OnLitepadCountLights)
	ON_BN_CLICKED(IDC_LITEPAD_NEXTVERTEX, OnLitepadNextvertex)
	ON_WM_PAINT()
	ON_EN_KILLFOCUS(IDC_ALPHA_EDIT, OnKillfocusAlphaEdit)
	ON_BN_CLICKED(IDC_SHOW_LIGHTMAPS, OnShowLightmaps)
	ON_EN_KILLFOCUS(IDC_HEMICUBE_RES_EDIT, OnKillfocusHemicubeResEdit)
	ON_EN_KILLFOCUS(IDC_LIGHT_SPACING_EDIT, OnKillfocusLightSpacingEdit)
	ON_BN_CLICKED(IDC_BESTFIT_CHECK, OnBestfitCheck)
	ON_BN_CLICKED(IDC_CREATE_VOLUME_CONE, OnCreateVolumeCone)
	ON_BN_CLICKED(IDC_CLEAR_VOLUMETRIC_ROOM, OnClearVolumetricRoom)
	ON_BN_CLICKED(IDC_FILL_VOLUME_TEXTURE, OnFillVolumeTexture)
	ON_EN_KILLFOCUS(IDC_ANGLE_EDIT, OnKillfocusAngleEdit)
	ON_EN_KILLFOCUS(IDC_START_ALPHA_EDIT, OnKillfocusStartAlphaEdit)
	ON_EN_KILLFOCUS(IDC_RESOLUTION_EDIT, OnKillfocusResolutionEdit)
	ON_BN_CLICKED(IDC_EDGE_VOLUMETRIC, OnEdgeVolumetric)
	ON_EN_KILLFOCUS(IDC_FALLOFF_EDIT, OnKillfocusFalloffEdit)
	ON_BN_CLICKED(IDC_FARTHEST_POINT, OnFarthestPoint)
	ON_EN_KILLFOCUS(IDC_PITCH_ANGLE_EDIT, OnKillfocusPitchAngleEdit)
	ON_EN_KILLFOCUS(IDC_HEADING_ANGLE_EDIT, OnKillfocusHeadingAngleEdit)
	ON_BN_CLICKED(IDC_HEMICUBE_CHECK, OnHemicubeCheck)
	ON_BN_CLICKED(IDC_COUNT_TERRAIN_LIGHTS, OnCountTerrainLights)
	ON_BN_CLICKED(IDC_CURROOM_RADIOSITY, OnCurroomRadiosity)
	ON_BN_CLICKED(IDC_DYNAMIC_TERRAIN, OnDynamicTerrain)
	ON_BN_CLICKED(IDC_BLUR_LIGHTMAPS, OnBlurLightmaps)
	ON_EN_KILLFOCUS(IDC_IGNORE_LIMIT_EDIT, OnKillfocusIgnoreLimitEdit)
	ON_EN_KILLFOCUS(IDC_RED_AMBIENT_EDIT, OnKillfocusRedAmbientEdit)
	ON_EN_KILLFOCUS(IDC_GREEN_AMBIENT_EDIT, OnKillfocusGreenAmbientEdit)
	ON_EN_KILLFOCUS(IDC_BLUE_AMBIENT_EDIT, OnKillfocusBlueAmbientEdit)
	ON_BN_CLICKED(IDC_ELEMENT_CHECK, OnElementCheck)
	ON_BN_CLICKED(IDC_IGNORE_TERRAIN, OnIgnoreTerrain)
	ON_BN_CLICKED(IDC_IGNORE_SATELLITES, OnIgnoreSatellites)
	ON_BN_CLICKED(IDC_USE_BSP_CHECK, OnUseBspCheck)
	ON_EN_KILLFOCUS(IDC_LIGHT_MULTIPLY_EDIT, OnKillfocusLightMultiplyEdit)
	ON_EN_KILLFOCUS(IDC_PULSE_TIME_EDIT, OnKillfocusPulseTimeEdit)
	ON_EN_KILLFOCUS(IDC_PULSE_OFFSET_EDIT, OnKillfocusPulseOffsetEdit)
	ON_BN_CLICKED(IDC_PULSE_ROOM, OnPulseRoom)
	ON_BN_CLICKED(IDC_RESET_MULTIPLES, OnResetMultiples)
	ON_BN_CLICKED(IDC_DESTROY_BSP, OnDestroyBsp)
	ON_BN_CLICKED(IDC_COMBINE_FACES, OnCombineFaces)
	ON_BN_CLICKED(IDC_VOLUME_LIGHTS, OnVolumeLights)
	ON_BN_CLICKED(IDC_CORONA_CHECK, OnCoronaCheck)
	ON_BN_CLICKED(IDC_FOG_CHECK, OnFogCheck)
	ON_EN_KILLFOCUS(IDC_FOG_DENSITY, OnKillfocusFogDensity)
	ON_EN_KILLFOCUS(IDC_FOG_RED, OnKillfocusFogRed)
	ON_EN_KILLFOCUS(IDC_FOG_GREEN, OnKillfocusFogGreen)
	ON_EN_KILLFOCUS(IDC_FOG_BLUE, OnKillfocusFogBlue)
	ON_BN_CLICKED(IDC_ROOM_CORONAS, OnRoomCoronas)
	ON_BN_CLICKED(IDC_MINE_CORONAS, OnMineCoronas)
	ON_BN_CLICKED(IDC_REVERSE_CORONA, OnReverseCorona)
	ON_EN_KILLFOCUS(IDC_GLOBAL_MULTIPLIER_EDIT, OnKillfocusGlobalMultiplierEdit)
	ON_BN_CLICKED(IDC_STROBE_CHECK, OnStrobeCheck)
	ON_BN_CLICKED(IDC_FLICKER_CHECK, OnFlickerCheck)
	ON_EN_KILLFOCUS(IDC_ROOM_MULTIPLIER_EDIT, OnKillfocusRoomMultiplierEdit)
	ON_EN_KILLFOCUS(IDC_ROOM_AMRED, OnKillfocusRoomAmred)
	ON_EN_KILLFOCUS(IDC_ROOM_AMGREEN, OnKillfocusRoomAmgreen)
	ON_EN_KILLFOCUS(IDC_ROOM_AMBLUE, OnKillfocusRoomAmblue)
	ON_BN_CLICKED(IDC_CLEAR_LIGHTMAPS, OnClearLightmaps)
	ON_BN_CLICKED(IDC_COPY_MULTIPLIER, OnCopyMultiplier)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLightingDialog message handlers


void CLightingDialog::OnSize(UINT nType, int cx, int cy) 
{
	CKeypadDialog::OnSize(nType, cx, cy);
		
}


void CLightingDialog::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	
	CKeypadDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}


void CLightingDialog::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	
	CKeypadDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CLightingDialog::OnLitepadSetdefault() 
{
	int i,alphaed=0;

	Curroomp->faces[Curface].face_uvls[Curvert].alpha=255;

	for (i=0;i<Curroomp->faces[Curface].num_verts;i++)
		if (Curroomp->faces[Curface].face_uvls[i].alpha!=255)
			alphaed=1;

	if (alphaed)
		Curroomp->faces[Curface].flags|=FF_VERTEX_ALPHA;
	else
		Curroomp->faces[Curface].flags&=~FF_VERTEX_ALPHA;

	World_changed=1;
	UpdateDialog();
	
}

void CLightingDialog::OnLitepadSmoothall() 
{
	int answer=MessageBox ("Are you sure you wish to calculate lighting on the indoor mine?","Light Question",MB_YESNO);
	if (answer==IDNO)
		return;

	MessageBox ("I will now calculate radiosity for the indoor engine.  You may prematurely break out by pressing any key during the calculations","Message",MB_OK);

	DoRadiosityForRooms();
	World_changed=1;
}

void CLightingDialog::OnKillfocusLightIterationEdit() 
{
	CEdit *ebox;
	char str[20];
		
	ebox=(CEdit *) GetDlgItem (IDC_LIGHT_ITERATION_EDIT);
	ebox->GetWindowText (str,20);

	rad_MaxStep=atoi (str);
}

BOOL CLightingDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_angle=30;
	m_resolution=50;
	m_start_alpha=1.0;
	m_falloff=20;
	m_pitch_angle=0;
	m_heading_angle=0;
	UpdateDialog();
		
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CLightingDialog::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	// TODO: Add your message handler code here and/or call default
	WinHelp(HID_LIGHTINGTAB,HELP_CONTEXT);
	return TRUE;
	//return CDialog::OnHelpInfo(pHelpInfo);
}

void CLightingDialog::OnLitepadTerrainRadiosity() 
{
	int answer=MessageBox ("Are you sure you wish to calculate lighting on the terrain?","Light Question",MB_YESNO);
	if (answer==IDNO)
		return;

	Sound_system.KillSoundLib(false);

	MessageBox ("I will now calculate radiosity for the terrain engine.  You may prematurely break out by pressing any key during the calculations","Message",MB_OK);
	DoRadiosityForTerrain();
	World_changed=1;

	Sound_system.InitSoundLib(Descent,  Sound_mixer, Sound_quality, false);
}

void CLightingDialog::OnLitepadCountLights() 
{
	int lightcount,i,t;
	char str[1000];

	for (lightcount=0,i=0;i<MAX_ROOMS;i++)
	{
		if (Rooms[i].used && !(Rooms[i].flags & RF_EXTERNAL))
		{
			for (t=0;t<Rooms[i].num_faces;t++)
			{
				int texnum=Rooms[i].faces[t].tmap;
				if (GameTextures[texnum].r>0 || GameTextures[texnum].g>0 || GameTextures[texnum].b>0)
					lightcount++;
			}
		}
	}

	for (i=0;i<=Highest_object_index;i++)
	{
		if (OBJECT_OUTSIDE(&Objects[i]))
			continue;

		if (Objects[i].type!=OBJ_NONE && Objects[i].lighting_render_type==LRT_LIGHTMAPS)
		{
			poly_model *po=&Poly_models[Objects[i].rtype.pobj_info.model_num];

			if (!po->new_style)
				continue;
			
			for (t=0;t<po->n_models;t++)
			{
				bsp_info *sm=&po->submodel[t];

				if (IsNonRenderableSubmodel (po,t))
					continue;

				for (int j=0;j<sm->num_faces;j++)
				{
					if (sm->faces[j].texnum==-1)
						continue;

						int texnum=po->textures[sm->faces[j].texnum];
						if (GameTextures[texnum].r>0 || GameTextures[texnum].g>0 || GameTextures[texnum].b>0)
							lightcount++;
				}
			}
		}
	}
		
	sprintf (str,"There are %d lights present in the mine.  Do you wish for me to set the 'light iterations' control to match this number?",lightcount);
	int answer=MessageBox (str,"Light Question",MB_YESNO);
	if (answer==IDNO)
		return;

	CEdit *ebox;
	
	ebox=(CEdit *) GetDlgItem (IDC_LIGHT_ITERATION_EDIT);
	sprintf (str,"%d",lightcount);
	ebox->SetWindowText (str);

	rad_MaxStep=lightcount;

}

void CLightingDialog::OnLitepadNextvertex() 
{	
	if (++Curvert >= Curroomp->faces[Curface].num_verts)
		Curvert = 0;

	EditorStatus("Vertex %d selected.",Curvert);
	UpdateDialog();

	State_changed = 1;

}

extern float GlobalMultiplier;
void CLightingDialog::UpdateDialog() 
{
	CEdit *ebox;
	float aval;
	char str[100];

	// Do volumetrics
	if (Curroomp && Curroomp->used)
	{
		aval=Ubyte_to_float[Curroomp->faces[Curface].face_uvls[Curvert].alpha];

		ebox=(CEdit *) GetDlgItem (IDC_ALPHA_EDIT);
		sprintf (str,"%f",aval);
		ebox->SetWindowText (str);

		ubyte pval=Curroomp->pulse_time;

		ebox=(CEdit *) GetDlgItem (IDC_PULSE_TIME_EDIT);
		sprintf (str,"%d",pval);
		ebox->SetWindowText (str);

		pval=Curroomp->pulse_offset;

		ebox=(CEdit *) GetDlgItem (IDC_PULSE_OFFSET_EDIT);
		sprintf (str,"%d",pval);
		ebox->SetWindowText (str);

		ebox=(CEdit *) GetDlgItem (IDC_FOG_DENSITY);
		sprintf (str,"%f",Curroomp->fog_depth);
		ebox->SetWindowText (str);

		ebox=(CEdit *) GetDlgItem (IDC_FOG_RED);
		sprintf (str,"%f",Curroomp->fog_r);
		ebox->SetWindowText (str);

		ebox=(CEdit *) GetDlgItem (IDC_FOG_GREEN);
		sprintf (str,"%f",Curroomp->fog_g);
		ebox->SetWindowText (str);

		ebox=(CEdit *) GetDlgItem (IDC_FOG_BLUE);
		sprintf (str,"%f",Curroomp->fog_b);
		ebox->SetWindowText (str);
	}
	else
	{
		ebox=(CEdit *) GetDlgItem (IDC_ALPHA_EDIT);
		ebox->SetWindowText ("No room selected");
	}

	// Set element check
	CheckDlgButton (IDC_ELEMENT_CHECK,Shoot_from_patch?0:1);
	CheckDlgButton (IDC_COMBINE_FACES,AllowCombining);
	CheckDlgButton (IDC_IGNORE_TERRAIN,Ignore_terrain);
	CheckDlgButton (IDC_IGNORE_SATELLITES,Ignore_satellites);
	CheckDlgButton (IDC_USE_BSP_CHECK,UseBSP);
	CheckDlgButton (IDC_VOLUME_LIGHTS,UseVolumeLights);

	if (Curroomp==NULL)
	{
		CheckDlgButton (IDC_CORONA_CHECK,0);
		CheckDlgButton (IDC_FOG_CHECK,0);
		CheckDlgButton (IDC_STROBE_CHECK,0);
		CheckDlgButton (IDC_FLICKER_CHECK,0);

	}
	else
	{
		CheckDlgButton (IDC_CORONA_CHECK,(Curroomp->faces[Curface].flags & FF_CORONA)?1:0);
		CheckDlgButton (IDC_FOG_CHECK,(Curroomp->flags & RF_FOG)?1:0);
		CheckDlgButton (IDC_STROBE_CHECK,(Curroomp->flags & RF_STROBE)?1:0);
		CheckDlgButton (IDC_FLICKER_CHECK,(Curroomp->flags & RF_FLICKER)?1:0);

		ebox=(CEdit *) GetDlgItem (IDC_ROOM_MULTIPLIER_EDIT);
		sprintf (str,"%f",Room_multiplier[Curroomp-Rooms]);
		ebox->SetWindowText (str);

		ebox=(CEdit *) GetDlgItem (IDC_ROOM_AMRED);
		sprintf (str,"%f",Room_ambience_r[Curroomp-Rooms]);
		ebox->SetWindowText (str);

		ebox=(CEdit *) GetDlgItem (IDC_ROOM_AMGREEN);
		sprintf (str,"%f",Room_ambience_g[Curroomp-Rooms]);
		ebox->SetWindowText (str);

		ebox=(CEdit *) GetDlgItem (IDC_ROOM_AMBLUE);
		sprintf (str,"%f",Room_ambience_b[Curroomp-Rooms]);
		ebox->SetWindowText (str);
	}

	// Do Global multiplier
	ebox=(CEdit *) GetDlgItem (IDC_GLOBAL_MULTIPLIER_EDIT);
	sprintf (str,"%f",GlobalMultiplier);
	ebox->SetWindowText (str);
	 
	// Do maxsteps for radiosity
	ebox=(CEdit *) GetDlgItem (IDC_LIGHT_ITERATION_EDIT);
	sprintf (str,"%d",rad_MaxStep);
	ebox->SetWindowText (str);

	ebox=(CEdit *) GetDlgItem (IDC_HEMICUBE_RES_EDIT);
	sprintf (str,"%d",Hemicube_resolution);
	ebox->SetWindowText (str);

	ebox=(CEdit *) GetDlgItem (IDC_LIGHT_SPACING_EDIT);
	sprintf (str,"%d",LightSpacing);
	ebox->SetWindowText (str);

	ebox=(CEdit *) GetDlgItem (IDC_ANGLE_EDIT);
	sprintf (str,"%d",m_angle);
	ebox->SetWindowText (str);

	ebox=(CEdit *) GetDlgItem (IDC_RESOLUTION_EDIT);
	sprintf (str,"%d",m_resolution);
	ebox->SetWindowText (str);

	ebox=(CEdit *) GetDlgItem (IDC_START_ALPHA_EDIT);
	sprintf (str,"%f",m_start_alpha);
	ebox->SetWindowText (str);

	ebox=(CEdit *) GetDlgItem (IDC_FALLOFF_EDIT);
	sprintf (str,"%f",m_falloff);
	ebox->SetWindowText (str);

	ebox=(CEdit *) GetDlgItem (IDC_PITCH_ANGLE_EDIT);
	sprintf (str,"%d",m_pitch_angle);
	ebox->SetWindowText (str);

	ebox=(CEdit *) GetDlgItem (IDC_HEADING_ANGLE_EDIT);
	sprintf (str,"%d",m_heading_angle);
	ebox->SetWindowText (str);
	
	ebox=(CEdit *) GetDlgItem (IDC_IGNORE_LIMIT_EDIT);
	sprintf (str,"%f",Ignore_limit);
	ebox->SetWindowText (str);

	ebox=(CEdit *) GetDlgItem (IDC_RED_AMBIENT_EDIT);
	sprintf (str,"%.2f",Ambient_red);
	ebox->SetWindowText (str);

	ebox=(CEdit *) GetDlgItem (IDC_GREEN_AMBIENT_EDIT);
	sprintf (str,"%.2f",Ambient_green);
	ebox->SetWindowText (str);

	ebox=(CEdit *) GetDlgItem (IDC_BLUE_AMBIENT_EDIT);
	sprintf (str,"%.2f",Ambient_blue);
	ebox->SetWindowText (str);
	
	ebox=(CEdit *) GetDlgItem (IDC_LIGHT_MULTIPLY_EDIT);
	sprintf (str,"%.2f",(Curroomp->faces[Curface].light_multiple)/4.0);
	ebox->SetWindowText (str);
	
}

void CLightingDialog::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	UpdateDialog();
	
	// Do not call CDialog::OnPaint() for painting messages
}

void CLightingDialog::OnKillfocusAlphaEdit() 
{
	CEdit *ebox;
	char str[40];
	float aval;
	int i,alphaed=0;
		
	ebox=(CEdit *) GetDlgItem (IDC_ALPHA_EDIT);
	ebox->GetWindowText (str,20);

	aval=atof (str);
	if (aval<0)
		aval=0;
	if (aval>1.0)
		aval=1.0;

	Curroomp->faces[Curface].face_uvls[Curvert].alpha=Float_to_ubyte(aval);

	for (i=0;i<Curroomp->faces[Curface].num_verts;i++)
		if (Curroomp->faces[Curface].face_uvls[i].alpha!=255)
			alphaed=1;

	if (alphaed)
		Curroomp->faces[Curface].flags|=FF_VERTEX_ALPHA;
	else
		Curroomp->faces[Curface].flags&=~FF_VERTEX_ALPHA;

	UpdateDialog();
	World_changed=1;
}


extern bool Outline_lightmaps;
void CLightingDialog::OnShowLightmaps() 
{
	int c=IsDlgButtonChecked(IDC_SHOW_LIGHTMAPS);
	
	if (c)
		Outline_lightmaps=1;
	else
		Outline_lightmaps=0;

	State_changed=1;
		
	
}

void CLightingDialog::OnKillfocusHemicubeResEdit() 
{
	CEdit *ebox;
	char str[20];
	int val;
		
	ebox=(CEdit *) GetDlgItem (IDC_HEMICUBE_RES_EDIT);
	ebox->GetWindowText (str,20);

	val=atoi (str);
	
	if (val<50)
		val=50;
	if (val>2048)
		val=2048;

	if ((val % 2)==1)
		val++;

	Hemicube_resolution=val;
	UpdateDialog();

}

void CLightingDialog::OnKillfocusLightSpacingEdit() 
{
	CEdit *ebox;
	char str[20];
	int val;
		
	ebox=(CEdit *) GetDlgItem (IDC_LIGHT_SPACING_EDIT);
	ebox->GetWindowText (str,20);

	val=atoi (str);
	
	if (val<1)
		val=1;
	if (val>20)
		val=20;

	LightSpacing=val;
	UpdateDialog();
	
}

void CLightingDialog::OnBestfitCheck() 
{
	int c=IsDlgButtonChecked(IDC_BESTFIT_CHECK);
	
	BestFit=c;

	State_changed=1;
}


void CLightingDialog::UpdateKeypad(int mask)
{
	if (mask & (KUF_CURROOM_CHANGED+KUF_CURFACE_CHANGED+KUF_CURPORTAL_CHANGED))
		UpdateDialog();
}

void CLightingDialog::ShootRayForLighting (vector *dest,vector *from,vector *to,int startseg)
{
	fvi_info hit_info;  
	fvi_query fq;
	
	// shoot a ray from the light position to the current vertex
	fq.p0=from;         
	fq.p1=to;
	fq.startroom= startseg; 
				
	fq.rad=0.0f; 
	fq.flags=FQ_CHECK_OBJS|FQ_IGNORE_NON_LIGHTMAP_OBJECTS|FQ_NO_RELINK|FQ_IGNORE_RENDER_THROUGH_PORTALS;
	fq.thisobjnum   = -1; 
	fq.ignore_obj_list = NULL;

	fvi_FindIntersection(&fq,&hit_info); 

	*dest=hit_info.hit_pnt;
}


#define MAX_VOLUMETRIC_DISTANCE	5000

// Creates a volumetric cone of light that casts from the center of the current face
void CLightingDialog::OnCreateVolumeCone() 
{
	int i,t;

	ASSERT (Curface<Curroomp->num_faces);
	int resolution=m_resolution;
	int first_vert_index,first_face_index,center_index;

	vector faceverts[MAX_VERTS_PER_FACE];
	vector center;
	matrix face_matrix;
	
	vector fvec=Curroomp->faces[Curface].normal;

	float pitch_increment=65536/resolution;
	float heading_increment=65536/resolution;
	float desired_angle=m_angle;
	float pitch,heading;
	
	// Fixup angle for our system
	desired_angle=(desired_angle/360.0)*65536;

	// Get face matrix
	vm_VectorToMatrix(&face_matrix,&fvec,NULL,NULL);
	vm_TransposeMatrix (&face_matrix);

	// Get polygon center
	face *fp=&Curroomp->faces[Curface];
	for (i=0;i<fp->num_verts;i++)
		faceverts[i]=Curroomp->verts[fp->face_verts[i]];
	vm_GetCentroid (&center,faceverts,fp->num_verts);
	
	// Add vertices to room
	center_index=Curroomp->num_verts;
	RoomAddVertices(Curroomp,resolution+1);
	Curroomp->verts[center_index]=center;
	first_vert_index=center_index+1;
	
	for (i=0;i<resolution;i++)
	{
		vector dest_vec;
		vector hit_vec;
		
		matrix tempm;

		// Get our direction
		heading=FixCos (heading_increment*i);
		pitch=FixSin (pitch_increment*i);
		
		vm_AnglesToMatrix (&tempm,pitch*desired_angle,heading*desired_angle,0);
		vm_MatrixMulVector (&dest_vec,&tempm.fvec,&face_matrix);
				
		dest_vec*=m_falloff;
		dest_vec+=center;

		// Now shoot a ray out
		ShootRayForLighting (&hit_vec,&center,&dest_vec,Curroomp-Rooms);

		Curroomp->verts[first_vert_index+i]=hit_vec;
		
	}

	// Add faces to room
	first_face_index=Curroomp->num_faces;
	RoomAddFaces (Curroomp,resolution);

	for (i=0;i<resolution;i++)
	{
		int	a=center_index;
		int	b=first_vert_index+i;
		int	c=first_vert_index+((i+1)%resolution);

		// First do front face
		fp=&Curroomp->faces[first_face_index+i];

		InitRoomFace (fp,3);
		fp->flags|=FF_VOLUMETRIC;
		fp->tmap=D3EditState.texdlg_texture;
		
		fp->face_verts[0]=a;
		fp->face_verts[1]=b;
		fp->face_verts[2]=c;

		if (! ComputeFaceNormal (Curroomp,first_face_index+i))
			Int3();	//Get Jason

		AssignDefaultUVsToRoomFace (Curroomp,first_face_index+i);

		// Now assign appropriate alpha

		fp->face_uvls[0].alpha=Float_to_ubyte(m_start_alpha);
		for (t=1;t<3;t++)
		{
			vector subvec=Curroomp->verts[fp->face_verts[t]]-center;
			float mag=vm_GetMagnitude (&subvec);
			float norm=1-(mag/m_falloff);
			if (norm<0)
				norm=0;
			fp->face_uvls[t].alpha=Float_to_ubyte (norm*m_start_alpha);
		}
	}
	World_changed=1;
}

void CLightingDialog::OnClearVolumetricRoom() 
{
	int i;

	StartOver:
	for (i=0;i<Curroomp->num_faces;i++)
	{
		face *fp=&Curroomp->faces[i];
		if (fp->flags & FF_VOLUMETRIC)
		{
			DeleteRoomFace (Curroomp,i);
			goto StartOver;
		}
	}

	World_changed=1;
	
}

void CLightingDialog::OnFillVolumeTexture() 
{
	int i;

	for (i=0;i<Curroomp->num_faces;i++)
	{
		face *fp=&Curroomp->faces[i];
		if (fp->flags & FF_VOLUMETRIC)
			Curroomp->faces[i].tmap=D3EditState.texdlg_texture;

	}

	World_changed=1;
	
}

void CLightingDialog::OnKillfocusAngleEdit() 
{
	CEdit *ebox;
	char str[20];
	int val;
		
	ebox=(CEdit *) GetDlgItem (IDC_ANGLE_EDIT);
	ebox->GetWindowText (str,20);

	val=atoi (str);
	
	if (val<0)
		val=0;
	if (val>90)
		val=70;
	
	m_angle=val;
	UpdateDialog();
}

void CLightingDialog::OnKillfocusStartAlphaEdit() 
{
	CEdit *ebox;
	char str[20];
	float val;
		
	ebox=(CEdit *) GetDlgItem (IDC_START_ALPHA_EDIT);
	ebox->GetWindowText (str,20);

	val=atof (str);
	
	if (val<0)
		val=0;
	if (val>1)
		val=1;
	
	m_start_alpha=val;
	UpdateDialog();
	
}

void CLightingDialog::OnKillfocusResolutionEdit() 
{
	CEdit *ebox;
	char str[20];
	int val;
		
	ebox=(CEdit *) GetDlgItem (IDC_RESOLUTION_EDIT);
	ebox->GetWindowText (str,20);

	val=atoi (str);
	
	if (val<1)
		val=1;
	if (val>200)
		val=200;
	
	m_resolution=val;
	UpdateDialog();
	
}

void CLightingDialog::OnEdgeVolumetric() 
{
	int i,t;
	float rad,mytan,tan_table[360];
	
	ASSERT (Curface<Curroomp->num_faces);
	int resolution=m_resolution;
	int first_vert_index,first_face_index,index,face_index;
	face *cur_fp=&Curroomp->faces[Curface];
	vector subvec;
	int total_face_verts,num_new_verts;

	float falloff_distance=0;
	vector nvec=Curroomp->faces[Curface].normal;
	vector fvec;

	vector faceverts[MAX_VERTS_PER_FACE];
	vector center;
	int pitch=m_pitch_angle;
	int heading=m_heading_angle;

	if (pitch<0)
		pitch+=360;
	if (heading<0)
		heading+=360;

	int desired_pitch=pitch*(65536.0/360.0);
	int desired_heading=heading*(65536.0/360.0);
	matrix tempm,face_matrix;

	// Get facematrix
	vm_VectorToMatrix(&face_matrix,&nvec,NULL,NULL);
	vm_TransposeMatrix (&face_matrix);

	// Get adjusted angle of direction
	vm_AnglesToMatrix (&tempm,desired_pitch,desired_heading,0);
	vm_MatrixMulVector (&fvec,&tempm.fvec,&face_matrix);
	
	// Build tan table
	for (i=0;i<360;i++)
	{
		rad=(float)(3.14*(float)i/180.0);
		mytan=tan(rad);
		tan_table[i]=mytan; 
	}

	// Get polygon center
	face *fp=&Curroomp->faces[Curface];
	for (i=0;i<fp->num_verts;i++)
		faceverts[i]=Curroomp->verts[fp->face_verts[i]];
	vm_GetCentroid (&center,faceverts,fp->num_verts);
		
	// Add vertices to room
	total_face_verts=cur_fp->num_verts;
	first_vert_index=Curroomp->num_verts;
	RoomAddVertices(Curroomp,cur_fp->num_verts*resolution*2);
	num_new_verts=cur_fp->num_verts*resolution*2;
	index=first_vert_index;

	// Compute the rays
	for (i=0;i<total_face_verts;i++)
	{
		vector vec_a=Curroomp->verts[cur_fp->face_verts[i]];
		vector vec_b=Curroomp->verts[cur_fp->face_verts[(i+1)%total_face_verts]];
		subvec=vec_b-vec_a;
		vector add_vec=subvec/resolution;

		for (t=0;t<resolution;t++,index+=2)
		{
			vector dest_vec;
			vector hit_vec;
			vector src_vec;
			vector norm_vec;
			vector subvec;
			float cone_distance,mag;
		
			src_vec=vec_a+(add_vec*t);
			src_vec+=(cur_fp->normal/64.0);
			dest_vec=src_vec+(fvec*m_falloff);

			cone_distance=tan_table[m_angle]*m_falloff;
			norm_vec=center+(fvec*m_falloff);
			subvec=dest_vec-norm_vec;
			mag=vm_GetMagnitude (&subvec);
			if (mag<cone_distance)
			{
				vm_NormalizeVector (&subvec);
				dest_vec=norm_vec+(subvec*cone_distance);
			}

			// Now shoot a ray out
			ShootRayForLighting (&hit_vec,&src_vec,&dest_vec,Curroomp-Rooms);

			Curroomp->verts[index]=src_vec;
			Curroomp->verts[index+1]=hit_vec;
		}
	
	}
	
	// Add faces to room
	first_face_index=Curroomp->num_faces;
	RoomAddFaces (Curroomp,resolution*total_face_verts);
	index=first_vert_index;
	face_index=first_face_index;
		
	for (i=0;i<total_face_verts;i++)
	{
		for (t=0;t<resolution;t++,index+=2,face_index++)
		{
			int	a=index;
			int	b=(index+2);
			int	c=(index+3);
			int	d=index+1;
			
			if ((b-first_vert_index)>=num_new_verts)
				b-=num_new_verts;
			if ((c-first_vert_index)>=num_new_verts)
				c-=num_new_verts;


			// First do front face
			face *fp=&Curroomp->faces[face_index];

			InitRoomFace (fp,4);
			fp->flags|=FF_VOLUMETRIC;
			fp->tmap=D3EditState.texdlg_texture;
		
			fp->face_verts[0]=a;
			fp->face_verts[1]=b;
			fp->face_verts[2]=c;
			fp->face_verts[3]=d;

			if (! ComputeFaceNormal (Curroomp,face_index))
				Int3();	//Get Jason

			AssignDefaultUVsToRoomFace (Curroomp,face_index);

		// Now assign appropriate alpha

			fp->face_uvls[0].alpha=Float_to_ubyte(m_start_alpha);
			fp->face_uvls[1].alpha=Float_to_ubyte(m_start_alpha);

			subvec=Curroomp->verts[c]-Curroomp->verts[b];
			float mag=vm_GetMagnitude (&subvec);
			float norm=1-(mag/m_falloff);
			if (norm<0)
				norm=0;
			fp->face_uvls[2].alpha=Float_to_ubyte(norm*m_start_alpha);

			subvec=Curroomp->verts[d]-Curroomp->verts[a];
			mag=vm_GetMagnitude (&subvec);
			norm=1-(mag/m_falloff);
			if (norm<0)
				norm=0;
			fp->face_uvls[3].alpha=Float_to_ubyte(norm*m_start_alpha);
			
		}
	}
	World_changed=1;
	
}

void CLightingDialog::OnKillfocusFalloffEdit() 
{
	CEdit *ebox;
	char str[20];
	int val;
		
	ebox=(CEdit *) GetDlgItem (IDC_FALLOFF_EDIT);
	ebox->GetWindowText (str,20);

	val=atoi (str);
	
	if (val<2)
		val=2;
	if (val>2000)
		val=2000;
	
	m_falloff=val;
	UpdateDialog();


	
}

void CLightingDialog::OnFarthestPoint() 
{
	face *fp=&Curroomp->faces[Curface];
	float max_distance=0;
	char str[255];
	int i;
		
	for (i=0;i<fp->num_verts;i++)
	{
		vector dest_vec;
		vector hit_vec;
		vector src_vec;
		vector subvec;

		src_vec=Curroomp->verts[fp->face_verts[i]];
		src_vec+=(fp->normal/64.0);
		dest_vec=src_vec+(fp->normal*MAX_VOLUMETRIC_DISTANCE);
		
		// Now shoot a ray out
		ShootRayForLighting (&hit_vec,&src_vec,&dest_vec,Curroomp-Rooms);

		subvec=hit_vec-src_vec;
		float mag=vm_GetMagnitude (&subvec);
		if (mag>max_distance)
			max_distance=mag;
	}

	sprintf (str,"The farthest point is %f units away.  Do you wish to set the falloff rate to this value?",max_distance);
	int answer=MessageBox (str,"Light Question",MB_YESNO);
	if (answer==IDNO)
		return;

	m_falloff=max_distance;
	UpdateDialog();
}

void CLightingDialog::OnKillfocusPitchAngleEdit() 
{
	CEdit *ebox;
	char str[20];
	int val;
		
	ebox=(CEdit *) GetDlgItem (IDC_PITCH_ANGLE_EDIT);
	ebox->GetWindowText (str,20);

	val=atoi (str);
	
	if (val<-70)
		val=-70;
	if (val>70)
		val=70;
	
	m_pitch_angle=val;
	UpdateDialog();

	
}

void CLightingDialog::OnKillfocusHeadingAngleEdit() 
{
	CEdit *ebox;
	char str[20];
	int val;
		
	ebox=(CEdit *) GetDlgItem (IDC_HEADING_ANGLE_EDIT);
	ebox->GetWindowText (str,20);

	val=atoi (str);
	
	if (val<-70)
		val=-70;
	if (val>70)
		val=70;
	
	m_heading_angle=val;
	UpdateDialog();

	
}

void CLightingDialog::OnHemicubeCheck() 
{
	int c=IsDlgButtonChecked(IDC_HEMICUBE_CHECK);

	if (c)	
		D3EditState.hemicube_radiosity=1;
	else
		D3EditState.hemicube_radiosity=0;

	
}

void CLightingDialog::OnCountTerrainLights() 
{
	int lightcount,i,t;
	char str[1000];

	for (lightcount=0,i=0;i<TERRAIN_WIDTH*TERRAIN_DEPTH;i++)
	{
		if (Terrain_seg[i].flags & TF_INVISIBLE)
			continue;

		int texnum=Terrain_tex_seg[Terrain_seg[i].texseg_index].tex_index;
		if (GameTextures[texnum].r>0 || GameTextures[texnum].g>0 || GameTextures[texnum].b>0)
			lightcount+=2;
	}

	lightcount+=Terrain_sky.num_satellites;

	for (i=0;i<=Highest_object_index;i++)
	{
		if (! OBJECT_OUTSIDE(&Objects[i]))
				continue;

		if (Objects[i].type!=OBJ_NONE && Objects[i].lighting_render_type==LRT_LIGHTMAPS)
		{
			poly_model *po=&Poly_models[Objects[i].rtype.pobj_info.model_num];

			if (!po->new_style)
				continue;
			
			for (t=0;t<po->n_models;t++)
			{
				bsp_info *sm=&po->submodel[t];
		
				if (IsNonRenderableSubmodel (po,t))
					continue;

				for (int j=0;j<sm->num_faces;j++)
				{
					if (sm->faces[j].texnum==-1)
						continue;

						int texnum=po->textures[sm->faces[j].texnum];
						if (GameTextures[texnum].r>0 || GameTextures[texnum].g>0 || GameTextures[texnum].b>0)
							lightcount++;
				}
			}
		}
	}

	// Count rooms
	for (i=0;i<MAX_ROOMS;i++)
	{
		if (Rooms[i].used && (Rooms[i].flags & RF_EXTERNAL))
		{
			for (t=0;t<Rooms[i].num_faces;t++)
			{
				int texnum=Rooms[i].faces[t].tmap;
				if (GameTextures[texnum].r>0 || GameTextures[texnum].g>0 || GameTextures[texnum].b>0)
					lightcount++;
			}
		}
	}
		
	sprintf (str,"There are %d lights present on the terrain.  Do you wish for me to set the 'light iterations' control to match this number?",lightcount);
	int answer=MessageBox (str,"Light Question",MB_YESNO);
	if (answer==IDNO)
		return;

	CEdit *ebox;
	
	ebox=(CEdit *) GetDlgItem (IDC_LIGHT_ITERATION_EDIT);
	sprintf (str,"%d",lightcount);
	ebox->SetWindowText (str);

	rad_MaxStep=lightcount;

	
}

void CLightingDialog::OnCurroomRadiosity() 
{
	MessageBox ("I will now calculate radiosity for the current room.  You may prematurely break out by pressing any key during the calculations","Message",MB_OK);

	DoRadiosityForCurrentRoom(Curroomp);
	World_changed=1;
}

void DoTerrainDynamicTable();
void CLightingDialog::OnDynamicTerrain() 
{
	if ((MessageBox("Are you sure you wish to do dynamic lighting on the terrain.  The reason I ask is that this procedure takes a long time, and I don't want to upset you.","Question",MB_YESNO))==IDYES)
		DoTerrainDynamicTable();
	
}

void CLightingDialog::OnBlurLightmaps() 
{
	
	if ((MessageBox("Are you sure you wish to blur all lightmaps?  This operation cannot be undone...","Question",MB_YESNO))==IDYES)
	{
		BlurLightmapInfos (LMI_ROOM);
		BlurLightmapInfos (LMI_ROOM_OBJECT);
		BlurLightmapInfos (LMI_EXTERNAL_ROOM);
		BlurLightmapInfos (LMI_TERRAIN_OBJECT);

		OutrageMessageBox ("Blurring complete.");
	}
	
}

void CLightingDialog::OnKillfocusIgnoreLimitEdit() 
{
	CEdit *ebox;
	char str[20];
	float val;
		
	ebox=(CEdit *) GetDlgItem (IDC_IGNORE_LIMIT_EDIT);
	ebox->GetWindowText (str,20);

	val=atof (str);

	if (val<0 || val>.5)
	{
		OutrageMessageBox ("Invalid value.  Must be in the range 0 to .5!");
		return;
	}

	Ignore_limit=val;
	UpdateDialog();

}

void CLightingDialog::OnKillfocusRedAmbientEdit() 
{
	CEdit *ebox;
	char str[20];
	float val;
		
	ebox=(CEdit *) GetDlgItem (IDC_RED_AMBIENT_EDIT);
	ebox->GetWindowText (str,20);

	val=atof (str);

	if (val<0 || val>1.0)
	{
		OutrageMessageBox ("Invalid value.  Must be in the range 0 to 1.0!");
		return;
	}

	Ambient_red=val;
	UpdateDialog();

	
}

void CLightingDialog::OnKillfocusGreenAmbientEdit() 
{
	CEdit *ebox;
	char str[20];
	float val;

	ebox=(CEdit *) GetDlgItem (IDC_GREEN_AMBIENT_EDIT);
	ebox->GetWindowText (str,20);

	val=atof (str);

	if (val<0 || val>1.0)
	{
		OutrageMessageBox ("Invalid value.  Must be in the range 0 to 1.0!");
		return;
	}

	Ambient_green=val;
	UpdateDialog();
}

void CLightingDialog::OnKillfocusBlueAmbientEdit() 
{
	CEdit *ebox;
	char str[20];
	float val;


	ebox=(CEdit *) GetDlgItem (IDC_BLUE_AMBIENT_EDIT);
	ebox->GetWindowText (str,20);

	val=atof (str);

	if (val<0 || val>1.0)
	{
		OutrageMessageBox ("Invalid value.  Must be in the range 0 to 1.0!");
		return;
	}

	Ambient_blue=val;
	UpdateDialog();
	
}

void CLightingDialog::OnElementCheck() 
{
	int c=IsDlgButtonChecked(IDC_ELEMENT_CHECK);

	if (c)	
		Shoot_from_patch=0;
	else
		Shoot_from_patch=1;
	
}

void CLightingDialog::OnIgnoreTerrain() 
{
	int c=IsDlgButtonChecked(IDC_IGNORE_TERRAIN);

	if (c)	
		Ignore_terrain=1;
	else
		Ignore_terrain=0;
	
}

void CLightingDialog::OnIgnoreSatellites() 
{
	int c=IsDlgButtonChecked(IDC_IGNORE_SATELLITES);

	if (c)	
		Ignore_satellites=1;
	else
		Ignore_satellites=0;
}

void CLightingDialog::OnUseBspCheck() 
{
	int c=IsDlgButtonChecked(IDC_USE_BSP_CHECK);

	if (c)	
		UseBSP=1;
	else
		UseBSP=0;

}

void CLightingDialog::OnKillfocusLightMultiplyEdit() 
{
	if (Curroomp==NULL)
		return;

	CEdit *ebox;
	char str[20];
	float val;
		
	ebox=(CEdit *) GetDlgItem (IDC_LIGHT_MULTIPLY_EDIT);
	ebox->GetWindowText (str,20);

	val=atof (str);

	if (val<.25 || val>63.75)
	{
		OutrageMessageBox ("Invalid value, setting to one!");
		val=1.0;
	}

	Curroomp->faces[Curface].light_multiple=(val*4.0);	
	UpdateDialog();
	
}

void CLightingDialog::OnKillfocusPulseTimeEdit() 
{
	CEdit *ebox;
	char str[20];
	int aval;
		
	ebox=(CEdit *) GetDlgItem (IDC_PULSE_TIME_EDIT);
	ebox->GetWindowText (str,20);

	aval=atol (str);
	if (aval<0)
		aval=0;
	if (aval>255)
		aval=255;

	Curroomp->pulse_time=aval;

	UpdateDialog();
	World_changed=1;
}

void CLightingDialog::OnKillfocusPulseOffsetEdit() 
{
	CEdit *ebox;
	char str[20];
	int aval;
		
	ebox=(CEdit *) GetDlgItem (IDC_PULSE_OFFSET_EDIT);
	ebox->GetWindowText (str,20);

	aval=atol (str);
	if (aval<0)
		aval=0;
	if (aval>255)
		aval=255;

	Curroomp->pulse_offset=aval;

	UpdateDialog();
	World_changed=1;
	
}

void CLightingDialog::OnPulseRoom() 
{
	int ptime=Curroomp->pulse_time;
	int poffset=Curroomp->pulse_offset;

	SetRoomPulse (Curroomp,ptime,poffset);
	UpdateDialog();
	World_changed=1;
}

void CLightingDialog::OnResetMultiples() 
{
	int testroom;
	int answer=MessageBox ("Are you sure you wish to reset all light multiples?","Light Question",MB_YESNO);
	if (answer==IDNO)
		return;

	answer=MessageBox ("Just for this room? (Answering no does it for the entire mine)","Light Question",MB_YESNO);

	if (answer==IDNO)
		testroom=-1;
	else
		testroom=Curroomp-Rooms;
	
	int i;

	for (i=0;i<=Highest_room_index;i++)
	{
		if (Rooms[i].used)
		{
			if (testroom!=-1 && i!=testroom)
				continue;

			for (int t=0;t<Rooms[i].num_faces;t++)
			{
				Rooms[i].faces[t].light_multiple=4;
			}
		}
	}

	OutrageMessageBox ("Light multiples reset!");
}

void CLightingDialog::OnDestroyBsp() 
{
	int answer=MessageBox ("Are you sure you wish to destroy the BSP tree?","Light Question",MB_YESNO);
	if (answer==IDNO)
		return;

	if (BSP_initted)
	{
		DestroyBSPTree (&MineBSP);
	}

	OutrageMessageBox ("BSP tree destroyed!");
	
}

void CLightingDialog::OnCombineFaces() 
{
	int c=IsDlgButtonChecked(IDC_COMBINE_FACES);

	if (c)	
		AllowCombining=1;
	else
		AllowCombining=0;
	
}

// Takes a bitmap name and resaves it
void ResaveBitmap (char *name)
{
	int bm_handle=bm_FindBitmapName (name);
	if (bm_handle>=0)
	{
		char search[256];
		
		// Make sure its paged in
		bm_data (bm_handle,0);
		
		ddio_MakePath(search,LocalD3Dir,"data","graphics",GameBitmaps[bm_handle].name,NULL);
		mprintf ((0,"Resaving bitmap %s.\n",search));
		
		bm_SaveFileBitmap (search,bm_handle);
	}
	else
	{
		char search[256];
				
		ddio_MakePath(search,LocalD3Dir,"data","graphics",name,NULL);

		bm_handle=bm_AllocLoadFileBitmap (search,0);
		if (bm_handle>=0)
		{
			// Make sure its paged in
			bm_data (bm_handle,0);
					
			mprintf ((0,"Resaving bitmap %s.\n",search));
		
			bm_SaveFileBitmap (search,bm_handle);
		}
		else
			Int3();
	}
		
}

// Takes a vclip name and resaves it
void ResaveVClip (char *name)
{
	int bm_handle=FindVClipName (name);
	if (bm_handle>=0)
	{
		char search[256];
		
		ddio_MakePath(search,LocalD3Dir,"data","graphics",GameVClips[bm_handle].name,NULL);
		mprintf ((0,"Resaving vclip %s.\n",search));
		
		SaveVClip (search,bm_handle);
	}
	else
	{
		char search[256];
				
		ddio_MakePath(search,LocalD3Dir,"data","graphics",name,NULL);

		bm_handle=AllocLoadVClip (search,NOT_TEXTURE,0);
		if (bm_handle>=0)
		{								
			mprintf ((0,"Resaving vclip %s.\n",search));
		
			SaveVClip (search,bm_handle);
		}
		else
			Int3();
	}
		
}

void ResaveAllBitmaps() 
{
	char buffer[_MAX_PATH];
	char search[256];

	ddio_MakePath(search,LocalD3Dir,"data","graphics","*.ogf",NULL);
	
	if(ddio_FindFileStart(search,buffer))
	{
		ResaveBitmap (buffer);

		while( (ddio_FindNextFile(buffer)))
		{
			ResaveBitmap(buffer);
		}
	}
	
	ddio_FindFileClose();

	ddio_MakePath(search,LocalD3Dir,"data","graphics","*.oaf",NULL);
	
	if(ddio_FindFileStart(search,buffer))
	{
		ResaveVClip (buffer);

		while( (ddio_FindNextFile(buffer)))
		{
			ResaveVClip(buffer);
		}
	}
	
	ddio_FindFileClose();
	
}

void CLightingDialog::OnVolumeLights() 
{
	int c=IsDlgButtonChecked(IDC_VOLUME_LIGHTS);

	if (c)	
		UseVolumeLights=1;
	else
		UseVolumeLights=0;
	
}

void CLightingDialog::OnCoronaCheck() 
{
	if (Curroomp==NULL)
		return;

	int c=IsDlgButtonChecked(IDC_CORONA_CHECK);

	if (c)	
		Curroomp->faces[Curface].flags|=FF_CORONA;
	else
		Curroomp->faces[Curface].flags&=~FF_CORONA;
}

void CLightingDialog::OnFogCheck() 
{
	if (Curroomp==NULL)
		return;

	int c=IsDlgButtonChecked(IDC_FOG_CHECK);

	if (c)	
		Curroomp->flags|=RF_FOG;
	else
		Curroomp->flags&=~RF_FOG;
}

void CLightingDialog::OnKillfocusFogDensity() 
{

	CEdit *ebox;
	char str[20];
	float aval;
		
	ebox=(CEdit *) GetDlgItem (IDC_FOG_DENSITY);
	ebox->GetWindowText (str,20);

	aval=atof (str);
	if (aval<0)
		aval=5;
	if (aval>5000)
		aval=5000;

	Curroomp->fog_depth=aval;

	UpdateDialog();
	World_changed=1;
}

void CLightingDialog::OnKillfocusFogRed() 
{
	CEdit *ebox;
	char str[20];
	float aval;
		
	ebox=(CEdit *) GetDlgItem (IDC_FOG_RED);
	ebox->GetWindowText (str,20);

	aval=atof (str);
	if (aval<0)
		aval=0;
	if (aval>1.0)
		aval=1;

	Curroomp->fog_r=aval;

	UpdateDialog();
	World_changed=1;
	
}

void CLightingDialog::OnKillfocusFogGreen() 
{
	CEdit *ebox;
	char str[20];
	float aval;
		
	ebox=(CEdit *) GetDlgItem (IDC_FOG_GREEN);
	ebox->GetWindowText (str,20);

	aval=atof (str);
	if (aval<0)
		aval=0;
	if (aval>1.0)
		aval=1;

	Curroomp->fog_g=aval;

	UpdateDialog();
	World_changed=1;
	
}

void CLightingDialog::OnKillfocusFogBlue() 
{
	CEdit *ebox;
	char str[20];
	float aval;
		
	ebox=(CEdit *) GetDlgItem (IDC_FOG_BLUE);
	ebox->GetWindowText (str,20);

	aval=atof (str);
	if (aval<0)
		aval=0;
	if (aval>1.0)
		aval=1;

	Curroomp->fog_b=aval;

	UpdateDialog();
	World_changed=1;
	
}

void CLightingDialog::OnRoomCoronas() 
{
	int rev,answer,only_cur_texture;

	rev=IsDlgButtonChecked(IDC_REVERSE_CORONA);
	
	if (rev)
		answer=MessageBox ("Are you sure you wish to remove all coronas in the current room?","Light Question",MB_YESNO);
	else
		answer=MessageBox ("Are you sure you wish to make all lights in the current room have coronas?","Light Question",MB_YESNO);

	if (answer==IDNO)
		return;

	only_cur_texture=MessageBox ("Do you wish to do this operation only on the faces that have the same texture as the currently selected face?","Light Question",MB_YESNO);
	


	int i;
	
	for (i=0;i<Curroomp->num_faces;i++)
	{
		if (only_cur_texture==IDYES)
		{
			if (Curroomp->faces[i].tmap!=Curroomp->faces[Curface].tmap)
				continue;
		}

		if (rev)
			Curroomp->faces[i].flags&=~FF_CORONA;
		else
			Curroomp->faces[i].flags|=FF_CORONA;
	}

	World_changed=1;

	OutrageMessageBox ("Operation complete!");
	UpdateDialog();
}

void CLightingDialog::OnMineCoronas() 
{
	int answer,only_cur_texture;
	int rev;

	rev=IsDlgButtonChecked(IDC_REVERSE_CORONA);
	
	if (rev)
		answer=MessageBox ("Are you sure you wish to remove all coronas in this level?","Light Question",MB_YESNO);
	else
		answer=MessageBox ("Are you sure you wish to make all lights in the mine have coronas?","Light Question",MB_YESNO);
		
	if (answer==IDNO)
		return;

	only_cur_texture=MessageBox ("Do you wish to do this operation only on the faces that have the same texture as the currently selected face?","Light Question",MB_YESNO);

	int i,t;
	
	for (i=0;i<=Highest_room_index;i++)
	{
		if (!Rooms[i].used)
			continue;
		if ((Rooms[i].flags & RF_EXTERNAL))
			continue;

		for (t=0;t<Rooms[i].num_faces;t++)
		{
			if (only_cur_texture==IDYES)
			{
				if (Rooms[i].faces[t].tmap!=Curroomp->faces[Curface].tmap)
					continue;
			}

			if (!rev)
				Rooms[i].faces[t].flags|=FF_CORONA;
			else
				Rooms[i].faces[t].flags&=~FF_CORONA;

		}
	}
	
	World_changed=1;

	OutrageMessageBox ("Operation complete!");
	UpdateDialog();
	
}

void CLightingDialog::OnReverseCorona() 
{
	// TODO: Add your control notification handler code here
	
}

void CLightingDialog::OnKillfocusGlobalMultiplierEdit() 
{
	CEdit *ebox;
	char str[20];
	float aval;
		
	ebox=(CEdit *) GetDlgItem (IDC_GLOBAL_MULTIPLIER_EDIT);
	ebox->GetWindowText (str,20);

	aval=atof (str);
	if (aval<0)
		aval=0;
	
	GlobalMultiplier=aval;

	UpdateDialog();

}

void CLightingDialog::OnStrobeCheck() 
{
	if (Curroomp==NULL)
		return;

	int c=IsDlgButtonChecked(IDC_STROBE_CHECK);

	if (c)	
		Curroomp->flags|=RF_STROBE;
	else
		Curroomp->flags&=~RF_STROBE;
	
}

void CLightingDialog::OnFlickerCheck() 
{
	if (Curroomp==NULL)
		return;

	int c=IsDlgButtonChecked(IDC_FLICKER_CHECK);

	if (c)	
		Curroomp->flags|=RF_FLICKER;
	else
		Curroomp->flags&=~RF_FLICKER;
	
}

void CLightingDialog::OnKillfocusRoomMultiplierEdit() 
{
	CEdit *ebox;
	char str[20];
	float aval;
		
	ebox=(CEdit *) GetDlgItem (IDC_ROOM_MULTIPLIER_EDIT);
	ebox->GetWindowText (str,20);

	aval=atof (str);
	if (aval<0)
		aval=0;
	
	Room_multiplier[Curroomp-Rooms]=aval;

	UpdateDialog();
}

void CLightingDialog::OnKillfocusRoomAmred() 
{
	CEdit *ebox;
	char str[20];
	float aval;
		
	ebox=(CEdit *) GetDlgItem (IDC_ROOM_AMRED);
	ebox->GetWindowText (str,20);

	aval=atof (str);
	if (aval<0)
		aval=0;
	
	Room_ambience_r[Curroomp-Rooms]=aval;

	UpdateDialog();
	
}

void CLightingDialog::OnKillfocusRoomAmgreen() 
{
	CEdit *ebox;
	char str[20];
	float aval;
		
	ebox=(CEdit *) GetDlgItem (IDC_ROOM_AMGREEN);
	ebox->GetWindowText (str,20);

	aval=atof (str);
	if (aval<0)
		aval=0;
	
	Room_ambience_g[Curroomp-Rooms]=aval;

	UpdateDialog();
	
}

void CLightingDialog::OnKillfocusRoomAmblue() 
{
	CEdit *ebox;
	char str[20];
	float aval;
		
	ebox=(CEdit *) GetDlgItem (IDC_ROOM_AMBLUE);
	ebox->GetWindowText (str,20);

	aval=atof (str);
	if (aval<0)
		aval=0;
	
	Room_ambience_b[Curroomp-Rooms]=aval;

	UpdateDialog();
	
}

#include "object_lighting.h"

void CLightingDialog::OnClearLightmaps() 
{
	if (OutrageMessageBox(MBOX_YESNO,"Are you sure you want to clear the lightmaps?  If you say yes, your level will have no lighting.") != IDYES)
		return;

	ClearAllRoomLightmaps(0);
	ClearAllObjectLightmaps(0);
}

int CopyFaceLightMultipleRoom(room *rp,int tmap,float multiple)
{
	int n_changed = 0;

	for (int f=0;f<rp->num_faces;f++)
		if (rp->faces[f].tmap == tmap) {
			rp->faces[f].light_multiple = multiple;
			n_changed++;
		}

	return n_changed;
}

void CopyFaceLightMultiple(room *rp,int facenum)
{
	int answer = OutrageMessageBox(MBOX_YESNOCANCEL,"Do you want to copy the light multiple from face %d to all faces in the mine that have the same texture?\n\n"
									"YES means copy to faces in the whole mine.\n"
									"NO means copy just to faces in the same room.\n"
									"CANCEL will abort this operation.",facenum);

	if (answer == IDCANCEL)
		return;

	int n_changed = 0;
	if (answer == IDNO)
		n_changed = CopyFaceLightMultipleRoom(rp,rp->faces[facenum].tmap,rp->faces[facenum].light_multiple);
	else {
		for (int r=0;r<=Highest_room_index;r++) {
			if (Rooms[r].used)
				n_changed += CopyFaceLightMultipleRoom(&Rooms[r],rp->faces[facenum].tmap,rp->faces[facenum].light_multiple);
		}
	}

	n_changed--;	//don't count the source face

	if (n_changed) {
		EditorStatus("The light multiple has been changed for %d faces.",n_changed);
		World_changed=1;
	}
	else
		EditorStatus("No faces had the same texture as face %d.",facenum);

}

void CLightingDialog::OnCopyMultiplier() 
{
	CopyFaceLightMultiple(Curroomp,Curface);
}
