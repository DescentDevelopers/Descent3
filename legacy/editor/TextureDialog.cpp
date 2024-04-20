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
 * $Logfile: /DescentIII/Main/editor/TextureDialog.cpp $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:57:39 $
 * $Author: kevinb $
 *
 * Texture Dialog keypad
 *
 * $Log: not supported by cvs2svn $
 * 
 * 26    5/06/99 12:43p Matt
 * Fixed a bug when did a texture grab on the terrain with no cells
 * selected.
 * 
 * 25    4/19/99 6:45p Jason
 * fixed satellite grabbing bug
 * 
 * 24    4/08/99 5:47p Jason
 * added face finding function
 * 
 * 23    2/23/99 3:07p Jason
 * added grabbing of terrain textures
 * 
 * 22    12/04/98 1:14p Jason
 * added "Currently Used" texture flag
 * 
 * 21    8/20/98 4:09p Jason
 * added facemap feature
 * 
 * 20    8/05/98 8:44p Jason
 * added uniform scaling of textures
 * 
 * 19    8/03/98 2:38p Matt
 * Changed the replace texture function to replace all instances of a
 * texture in the current room *or* in the whole mine, at the user's
 * discretion.
 * 
 * 18    6/23/98 2:43p Matt
 * Changed calls to OutrageMessageBox() & Debug_MessageBox() to deal with
 * int return value (instead of bool).
 * 
 * 17    6/15/98 4:00p Jason
 * replaced monochromatic polymodel lighting with rgb lighting
 * 
 * 16    6/10/98 1:57p Samir
 * fixed some listbox problems.
 * 
 * 15    6/10/98 12:21p Matt
 * Revamped system that controls what textures are displayed on the
 * texture tab.  Should work correctly now, and will now save the settings
 * to the registry.  Also, textures now default to no type set, not all
 * types set.
 * 
 * 14    5/20/98 1:08p Jason
 * more stuff for adding bumpmaps
 * 
 * 13    3/03/98 5:39p Jason
 * added swatch to texture tab
 * 
 * 12    11/14/97 12:41p Jason
 * Added ReplaceTexture function
 * 
 * 11    9/18/97 12:32p Samir
 * If currently selected texture is not in the list, default to 0 texture.
 * 
 * 10    9/17/97 1:01p Matt
 * Ripped out segment code
 * 
 * 9     9/17/97 12:06p Jason
 * ripped out segment engine
 * 
 * 8     9/16/97 6:08p Jason
 * made texture names appear
 * 
 * 7     9/15/97 1:18p Jason
 * got selectable tmap2s working
 * 
 * 6     9/12/97 3:47p Matt
 * Fixed grab texture for room engine
 * 
 * 5     8/29/97 1:52p Samir
 * Added bitmap buttons for Texture Keypad.
 * 
 * 4     8/25/97 7:39p Samir
 * Using new GrListBox from old editorPictListBox
 * 
 * 3     8/13/97 9:53a Matt
 * Show selected texture with magenta box around it.
 * Use new texture rotate & scale functions
 * 
 * 15    6/03/97 4:55p Mark
 *
 * 15	6/3/97	4:14p Jeff
 * Added Context Sensitive Help
 *
 * 14    5/28/97 6:15p Samir
 * Fixups for missing textures in scrollbox.
 * 
 * 13    5/27/97 5:29p Samir
 * Took out mprintf.
 * 
 * 12    5/27/97 3:11p Samir
 * Allow 'masking' of textures to be displayed in listbox.
 * 
 * 11    4/11/97 4:53p Samir
 * Grab texture button works for tmap 1s
 * 
 * 10    3/13/97 1:42p Matt
 * Don't show texture 0 (the error texture) in the texture list
 * 
 * 9     2/26/97 4:00p Samir
 * Added most keypad functions.
 * 
 * 8     2/21/97 6:25p Samir
 * Set properly the max items for the listbox.
 * 
 * 7     2/21/97 5:09p Samir
 * Uses new PictListBox class for texture listbox here.
 * 
 * 6     2/20/97 6:05p Samir
 * Allow for clean activation and deactivation of surfaces in editor when
 * going into fullscreen mode.
 * 
 * 5     2/19/97 7:29p Samir
 * Got scrolling texture view to work.
 * 
 * 4     1/27/97 11:38a Samir
 * Added horizontal scrolling of keypad modeless dialog.
 *
 * $NoKeywords: $
 */


#include "stdafx.h"
#include "editor.h"
#include "TextureDialog.h"

#include "HTexture.h"
#include "texture.h"
#include "gametexture.h"
#include "gr.h"
#include "descent.h"
#include "erooms.h"
#include "room.h"
#include "terrain.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTextureDialog dialog

// Initialization and Destruction of the Dialog.

CTextureDialog::CTextureDialog(CWnd* pParent /*=NULL*/)
	: CKeypadDialog(CTextureDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTextureDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CTextureDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTextureDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


void CTextureDialog::RunKeypadFunction(int code)
{
	switch (code)
	{
		case VK_NUMPAD0:		OnTexpadRotate90(); break;
		case VK_NUMPAD1:		OnTexpadRotLeft(); break;
		case VK_NUMPAD2:		OnTexpadSlideDown(); break;
		case VK_NUMPAD3:		OnTexpadRotRight(); break;
		case VK_NUMPAD4:		OnTexpadSlideLeft(); break;
		case VK_NUMPAD5:		OnTexpadSetDefault(); break;
		case VK_NUMPAD6:		OnTexpadSlideRight(); break;
		case VK_NUMPAD7:		OnTexpadStretchLess(); break;
		case VK_NUMPAD8:		OnTexpadSlideUp(); break;
		case VK_NUMPAD9:		OnTexpadStretchMore(); break;
		case VK_DELETE:			OnTexpadNextEdge(); break;
	}
}


//	selects a texture in the texture dialog
void CTextureDialog::SelectTexture(int texnum)
{
	m_TexLB.SelectItem(texnum);
}



BEGIN_MESSAGE_MAP(CTextureDialog, CDialog)
	//{{AFX_MSG_MAP(CTextureDialog)
	ON_WM_KEYDOWN()
	ON_WM_SIZE()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_LBUTTONDOWN()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_TEXPAD_FLIPX, OnTexpadFlipX)
	ON_BN_CLICKED(IDC_TEXPAD_FLIPY, OnTexpadFlipY)
	ON_BN_CLICKED(IDC_TEXPAD_SLIDELEFT, OnTexpadSlideLeft)
	ON_BN_CLICKED(IDC_TEXPAD_SLIDERIGHT, OnTexpadSlideRight)
	ON_BN_CLICKED(IDC_TEXPAD_ROTLEFT, OnTexpadRotLeft)
	ON_BN_CLICKED(IDC_TEXPAD_ROTRIGHT, OnTexpadRotRight)
	ON_BN_CLICKED(IDC_TEXPAD_ROTATE90, OnTexpadRotate90)
	ON_BN_CLICKED(IDC_TEXPAD_STRETCHLESS, OnTexpadStretchLess)
	ON_BN_CLICKED(IDC_TEXPAD_STRETCHMORE, OnTexpadStretchMore)
	ON_BN_CLICKED(IDC_TEXPAD_SLIDEUP, OnTexpadSlideUp)
	ON_BN_CLICKED(IDC_TEXPAD_SLIDEDOWN, OnTexpadSlideDown)
	ON_BN_CLICKED(IDC_TEXPAD_SETDEFAULT, OnTexpadSetDefault)
	ON_BN_CLICKED(IDC_TEXPAD_NEXTEDGE, OnTexpadNextEdge)
	ON_BN_CLICKED(IDC_TEXPAD_GRAB, OnTexpadGrab)
	ON_BN_CLICKED(IDC_SHOW_MINE_TEXTURES2, OnShowMineTextures2)
	ON_BN_CLICKED(IDC_SHOW_OBJ_TEXTURES, OnShowObjTextures)
	ON_BN_CLICKED(IDC_SHOW_LIGHT_TEXTURES, OnShowLightTextures)
	ON_BN_CLICKED(IDC_SHOW_TERRAIN_TEXTURES, OnShowTerrainTextures)
	ON_WM_HELPINFO()
	ON_BN_CLICKED(IDC_RESET_ALL_TEXTURES, OnResetAllTextures)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_SHOW_TMAP2_TEXTURES, OnShowTmap2Textures)
	ON_BN_CLICKED(IDC_REPLACE_TEXTURE, OnReplaceTexture)
	ON_EN_KILLFOCUS(IDC_TEXSCALE_EDIT, OnKillfocusTexscaleEdit)
	ON_BN_CLICKED(IDC_TEXPAD_EXPAND_U, OnTexpadExpandU)
	ON_BN_CLICKED(IDC_TEXPAD_EXPAND_V, OnTexpadExpandV)
	ON_BN_CLICKED(IDC_TEXPAD_CONTRACT_U, OnTexpadContractU)
	ON_BN_CLICKED(IDC_TEXPAD_CONTRACT_V, OnTexpadContractV)
	ON_BN_CLICKED(IDC_FACE_MAP, OnFaceMap)
	ON_BN_CLICKED(IDC_CURRENT_USED, OnCurrentUsed)
	ON_BN_CLICKED(IDC_FIND_TEXTURE, OnFindTexture)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTextureDialog message handlers

static CDialog *ParentDialog=NULL;
static CTextureDialog *ParentTextureDialog=NULL;
ubyte CurrentlyUsedTextures[MAX_TEXTURES];
bool Show_used=0;

BOOL CTextureDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

	RECT rect;
	CWnd *wnd;
	CEdit *ebox;
	char str[20];

	memset (CurrentlyUsedTextures,0,MAX_TEXTURES);

	ParentDialog=this;
	ParentTextureDialog=this;
	wnd = GetDlgItem(IDC_TEXTURE_LISTBOX);
	wnd->GetClientRect(&rect);

	if (!GameTextures[D3EditState.texdlg_texture].used) D3EditState.texdlg_texture = 0;

	m_TexLB.Create(wnd, IDC_TEXTURE_LISTBOX, rect, 36);
	m_TexLB.SelectItem(D3EditState.texdlg_texture);
	
	if (D3EditState.texture_display_flags & TF_TERRAIN) 
		((CButton *)GetDlgItem(IDC_SHOW_TERRAIN_TEXTURES))->SetCheck(1);
	if (D3EditState.texture_display_flags & TF_MINE) 
		((CButton *)GetDlgItem(IDC_SHOW_MINE_TEXTURES2))->SetCheck(1);
	if (D3EditState.texture_display_flags & TF_OBJECT) 
		((CButton *)GetDlgItem(IDC_SHOW_OBJ_TEXTURES))->SetCheck(1);
	if (D3EditState.texture_display_flags & TF_TMAP2) 
		((CButton *)GetDlgItem(IDC_SHOW_TMAP2_TEXTURES))->SetCheck(1);
	if (D3EditState.texture_display_flags & TF_LIGHT) 
		((CButton *)GetDlgItem(IDC_SHOW_LIGHT_TEXTURES))->SetCheck(1);

//	Do bitmap buttons
	m_bmUp.LoadBitmap(IDB_NORTH);
	m_bmDown.LoadBitmap(IDB_SOUTH);
	m_bmLeft.LoadBitmap(IDB_WEST);
	m_bmRight.LoadBitmap(IDB_EAST);
	m_bmRotLeft.LoadBitmap(IDB_ROTATELEFT);
	m_bmRotRight.LoadBitmap(IDB_ROTATERIGHT);
	m_bmStretchLess.LoadBitmap(IDB_INARROW);
	m_bmStretchMore.LoadBitmap(IDB_OUTARROW);

//	Set bitmap buttons
	CButton *btn;
	btn = (CButton*)GetDlgItem(IDC_TEXPAD_SLIDEUP);
	btn->SetBitmap((HBITMAP)m_bmUp);
	btn = (CButton*)GetDlgItem(IDC_TEXPAD_SLIDEDOWN);
	btn->SetBitmap((HBITMAP)m_bmDown);
	btn = (CButton*)GetDlgItem(IDC_TEXPAD_SLIDELEFT);
	btn->SetBitmap((HBITMAP)m_bmLeft);
	btn = (CButton*)GetDlgItem(IDC_TEXPAD_SLIDERIGHT);
	btn->SetBitmap((HBITMAP)m_bmRight);
	btn = (CButton*)GetDlgItem(IDC_TEXPAD_ROTLEFT);
	btn->SetBitmap((HBITMAP)m_bmRotLeft);
	btn = (CButton*)GetDlgItem(IDC_TEXPAD_ROTRIGHT);
	btn->SetBitmap((HBITMAP)m_bmRotRight);
	btn = (CButton*)GetDlgItem(IDC_TEXPAD_STRETCHLESS);
	btn->SetBitmap((HBITMAP)m_bmStretchLess);
	btn = (CButton*)GetDlgItem(IDC_TEXPAD_STRETCHMORE);
	btn->SetBitmap((HBITMAP)m_bmStretchMore);
		
	ebox=(CEdit *) GetDlgItem (IDC_TEXSCALE_EDIT);
	sprintf (str,"%f",D3EditState.texscale);
	ebox->SetWindowText (str);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CTextureDialog::OnDestroy() 
{
	CDialog::OnDestroy();
	
//	kill all bitmaps
	m_bmUp.DeleteObject();	
	m_bmDown.DeleteObject();
	m_bmLeft.DeleteObject();
	m_bmRight.DeleteObject();
	m_bmRotLeft.DeleteObject();
	m_bmRotRight.DeleteObject();
	m_bmStretchLess.DeleteObject();
	m_bmStretchMore.DeleteObject();
}


void CTextureDialog::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	
	CDialog::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CTextureDialog::OnSize(UINT nType, int cx, int cy) 
{
	CKeypadDialog::OnSize(nType, cx, cy);
	
}


void CTextureDialog::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
 	CKeypadDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}


void CTextureDialog::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	
	CKeypadDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CTextureDialog::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CDialog::OnLButtonDown(nFlags, point);
}


void CTextureDialog::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	if (!m_Active) return;
}


//	Keypad Functions!!

void CTextureDialog::OnTexpadFlipX() 
{
	HTextureFlipX();
}


void CTextureDialog::OnTexpadFlipY() 
{
	HTextureFlipY();
}


void CTextureDialog::OnTexpadSlideLeft() 
{
	HTextureSlide(Curroomp, Curface, (float) -1.0*D3EditState.texscale, 0.0f);
}


void CTextureDialog::OnTexpadSlideRight() 
{
	HTextureSlide(Curroomp, Curface, (float) 1.0*D3EditState.texscale, 0.0f);
}


void CTextureDialog::OnTexpadSlideUp() 
{
	HTextureSlide(Curroomp, Curface, 0.0f, (float) 1.0*D3EditState.texscale);
}


void CTextureDialog::OnTexpadSlideDown() 
{
	HTextureSlide(Curroomp, Curface, 0.0f, (float) -1.0*D3EditState.texscale);
}


void CTextureDialog::OnTexpadRotLeft() 
{
	HTextureRotate(Curroomp, Curface, (-512*D3EditState.texscale));
}


void CTextureDialog::OnTexpadRotRight() 
{
	HTextureRotate(Curroomp, Curface, (512*D3EditState.texscale));	
}


void CTextureDialog::OnTexpadRotate90() 
{
	HTextureRotate(Curroomp, Curface, 0x4000);	
}


void CTextureDialog::OnTexpadStretchLess() 
{
	HTextureStretchLess(Curroomp, Curface, Curedge);	
}


void CTextureDialog::OnTexpadStretchMore() 
{
	HTextureStretchMore(Curroomp, Curface, Curedge);	
}


void CTextureDialog::OnTexpadSetDefault() 
{
	HTextureSetDefault(Curroomp);	
}


void CTextureDialog::OnTexpadNextEdge() 
{
	HTextureNextEdge();	
}


void CTextureDialog::OnTexpadGrab() 
{
	int texnum=-1;
	
	if (Editor_view_mode==VM_TERRAIN)
	{
		for (int i=0;i<TERRAIN_WIDTH*TERRAIN_DEPTH;i++)
		{
			if (TerrainSelected[i])
			{
				if (texnum != -1) {
					OutrageMessageBox("Cannot grab texture: More than one terrain cell selected.");
					return;
				}
				texnum=Terrain_tex_seg[Terrain_seg[i].texseg_index].tex_index;
			}
		}
		if (texnum == -1) {
			OutrageMessageBox("Cannot grab texture: No terrain cell selected.");
			return;
		}
	}
	else
	{
		if (!Curroomp || Curroomp->used==0 || Curface>=Curroomp->num_faces)
			return;

		texnum = Curroomp->faces[Curface].tmap;
	}

	CTextureDialog::SelectTexture(texnum);
	EditorStatus("Texture %d, \"%s\", selected.",texnum,GameTextures[texnum].name);
}


void CTextureDialog::OnShowMineTextures2() 
{
	D3EditState.texture_display_flags ^= TF_MINE;	
	Invalidate();
}

void CTextureDialog::OnShowObjTextures() 
{
	D3EditState.texture_display_flags ^= TF_OBJECT;	
	Invalidate();
}

void CTextureDialog::OnShowLightTextures() 
{
	D3EditState.texture_display_flags ^= TF_LIGHT;
	Invalidate();
}

void CTextureDialog::OnShowTerrainTextures() 
{
	D3EditState.texture_display_flags ^= TF_TERRAIN;	
	Invalidate();
}

BOOL CTextureDialog::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	// TODO: Add your message handler code here and/or call default
	WinHelp(HID_TEXTURETAB,HELP_CONTEXT);
	return TRUE;
	//return CDialog::OnHelpInfo(pHelpInfo);
}

extern void AssignDefaultUVsToRoom(room *rp);
void CTextureDialog::OnResetAllTextures() 
{
	int answer=OutrageMessageBox (MBOX_YESNO,"Are you sure you wish to reset all textures for this room?  This change cannot be undone!");
	if (answer!=IDYES)
		return;

	AssignDefaultUVsToRoom (Curroomp);

	World_changed=1;
}


/////////////////////////////////////////////////////////////////////
//	CGrTextureListBox implementation

//	enumerates items maximum in the list box.  this is here to insure that any changes in
//	the user-managed list.
int CGrTextureListBox::ListEnumerate()
{
	int count = 0,i;

	for (i=0;i<MAX_TEXTURES;i++)
	{
		bool countit=true;;
		if (!GameTextures[i].used)
			countit=false;
		if (!(GameTextures[i].flags & D3EditState.texture_display_flags))
			countit=false;
		if (Show_used && !CurrentlyUsedTextures[i])
			countit=false;

		if (countit)
			count++;
	}

	return count;
}


// Specify the first item to be drawn in the box by DrawItem. returns the item #
//	return -1 if there is no first item (list is empty, maybe)
int CGrTextureListBox::ListFirstItem(int firsttex)
{
//	skip the error texture 
	if (firsttex < 0) firsttex = 0;
	if (firsttex == 0) firsttex++;

//	find first texture that is used if the texture passed became invalid.
	if (!GameTextures[firsttex].used || !(GameTextures[firsttex].flags & D3EditState.texture_display_flags) || (Show_used && !CurrentlyUsedTextures[firsttex])) {
		int new_first = ListNextItem(firsttex);
		if (new_first == firsttex)
			return -1;
		else
			return new_first;
	}
	else 
		return firsttex;
}


// Specify the next/prev item to be drawn in the box by DrawItem. 
//	Returns the item # following or preceding the passed the item #
int CGrTextureListBox::ListNextItem(int curtex)
{
	int next_texture = -1;

	if (curtex >= 0) 
		next_texture = GetNextTexture(curtex);
	if (next_texture <= curtex) 
		return curtex;

	while ((Show_used && !CurrentlyUsedTextures[next_texture]) || !(GameTextures[next_texture].flags & D3EditState.texture_display_flags) ) 
	{
		next_texture = GetNextTexture(next_texture);
	//	mprintf((0, "next_tex = %d\n", next_texture));
		if (next_texture == curtex) {
			break;
		}
	}

	return next_texture;
}


int CGrTextureListBox::ListPrevItem(int curtex)
{
	int prev_texture = -1;

	if (curtex >=0) 
		prev_texture = GetPreviousTexture(curtex);
	if (prev_texture >= curtex) 
		return curtex;

	while ((Show_used && !CurrentlyUsedTextures[prev_texture]) || !(GameTextures[prev_texture].flags & D3EditState.texture_display_flags) ) 
	{
		prev_texture = GetPreviousTexture(prev_texture);
	//	mprintf((0, "prev_tex = %d\n", prev_texture));
		if (prev_texture == curtex) {
			break;
		}
	}

	return prev_texture;
}


//	draw the item
void CGrTextureListBox::DrawItem(int x, int y, grHardwareSurface *itemsurf, int item)
{
	int bm_handle;

	bm_handle = GetTextureBitmap (item,0);

	itemsurf->load(bm_handle);
	Desktop_surf->blt(x, y, itemsurf);
}


//	if item was selected from the list box, this function is invoked.
void CGrTextureListBox::OnItemSelected(int tex)
{
	D3EditState.texdlg_texture = tex;
	
	if (ParentDialog!=NULL)
	{
		PrintToDlgItem (ParentDialog,IDC_CURRENT_TEXTURE_NAME,"Texture:  %s",GameTextures[tex].name);
		ParentTextureDialog->DrawSwatch (IDC_TEXTURE_SWATCH,GameTextures[tex].r,GameTextures[tex].g,GameTextures[tex].b);
	}
}



void CTextureDialog::OnShowTmap2Textures() 
{
	// TODO: Add your control notification handler code here
	D3EditState.texture_display_flags ^= TF_TMAP2;	
	Invalidate();
	
}

//Returns the number of changes
int ReplaceTextureInRoom(room *rp,int old_tmap,int new_tmap)
{
	int n_changes = 0;

	for (int i=0;i<rp->num_faces;i++) {

		if (rp->faces[i].tmap == old_tmap) {

			rp->faces[i].tmap = new_tmap;
			n_changes++;
		}
	}

	return n_changes;
}

void CTextureDialog::OnReplaceTexture() 
{
	ASSERT (Curroomp!=NULL);
	int n_changes;

	int answer = OutrageMessageBox(MBOX_YESNOCANCEL,"Do you wish to change this texture for the whole mine?\n\n(Answer NO to change only the current room.)");

	if (answer == IDCANCEL)
		return;

	int selected_texture=Curroomp->faces[Curface].tmap;

	if (answer == IDYES) {

		n_changes = 0;

		for (int r=0;r<=Highest_room_index;r++)
			if (Rooms[r].used)
				n_changes += ReplaceTextureInRoom(&Rooms[r],selected_texture,D3EditState.texdlg_texture);
	}
	else {

		n_changes = ReplaceTextureInRoom(Curroomp,selected_texture,D3EditState.texdlg_texture);
	}

	EditorStatus("Texture %d replaced with texture %d on %d faces.",selected_texture,D3EditState.texdlg_texture,n_changes);

	World_changed=1;

}

void CTextureDialog::DrawSwatch(int handle,float r,float g,float b)
{
	CWnd *texwnd;
	RECT rect;
	int w,h;
	float rmax=max(r,g);
	rmax=max(rmax,b);

	if (rmax>=.001)
	{
		r/=rmax;
		g/=rmax;
		b/=rmax;
	}
	
	texwnd = GetDlgItem(handle);
	texwnd->GetWindowRect(&rect);
	ScreenToClient(&rect);

	Desktop_surf->attach_to_window((unsigned)m_hWnd);
	
	w=rect.right-rect.left;
	h=rect.bottom-rect.top;

	ddgr_color color=GR_RGB(r*255,g*255,b*255);

	Desktop_surf->clear(rect.left,rect.top,w,h,color);

	Desktop_surf->attach_to_window(NULL);

}

void CTextureDialog::OnKillfocusTexscaleEdit() 
{
	CEdit *ebox;
	char str[20];
	float val;
	
	ebox=(CEdit *) GetDlgItem (IDC_TEXSCALE_EDIT);
	ebox->GetWindowText (str,20);

	val = atof (str);
	if (val<0)
		val=0;
	if (val>64)
		val=64;

	D3EditState.texscale=val;

	ebox=(CEdit *) GetDlgItem (IDC_TEXSCALE_EDIT);
	sprintf (str,"%f",D3EditState.texscale);
	ebox->SetWindowText (str);

}

void CTextureDialog::OnTexpadExpandU() 
{
	if (Curroomp==NULL)
		return;
	
	face *fp=&Curroomp->faces[Curface];

	int i;
	float mid=0;

	for (i=0;i<fp->num_verts;i++)
		mid+=fp->face_uvls[i].u;

	mid/=fp->num_verts;

	for (i=0;i<fp->num_verts;i++)
	{
		float diff=fp->face_uvls[i].u-mid;

		diff*=1.0/D3EditState.texscale;

		fp->face_uvls[i].u=mid+diff;
	}

	World_changed=1;
}

void CTextureDialog::OnTexpadExpandV() 
{
	if (Curroomp==NULL)
		return;
	
	face *fp=&Curroomp->faces[Curface];

	int i;
	float mid=0;

	for (i=0;i<fp->num_verts;i++)
		mid+=fp->face_uvls[i].v;

	mid/=fp->num_verts;

	for (i=0;i<fp->num_verts;i++)
	{
		float diff=fp->face_uvls[i].v-mid;

		diff*=1.0/D3EditState.texscale;

		fp->face_uvls[i].v=mid+diff;
	}

	World_changed=1;
	
}

void CTextureDialog::OnTexpadContractU() 
{
if (Curroomp==NULL)
		return;
	
	face *fp=&Curroomp->faces[Curface];

	int i;
	float mid=0;

	for (i=0;i<fp->num_verts;i++)
		mid+=fp->face_uvls[i].u;

	mid/=fp->num_verts;

	for (i=0;i<fp->num_verts;i++)
	{
		float diff=fp->face_uvls[i].u-mid;

		diff*=D3EditState.texscale;

		fp->face_uvls[i].u=mid+diff;
	}

	World_changed=1;
	
}

void CTextureDialog::OnTexpadContractV() 
{
	if (Curroomp==NULL)
		return;
	
	face *fp=&Curroomp->faces[Curface];

	int i;
	float mid=0;

	for (i=0;i<fp->num_verts;i++)
		mid+=fp->face_uvls[i].v;

	mid/=fp->num_verts;

	for (i=0;i<fp->num_verts;i++)
	{
		float diff=fp->face_uvls[i].v-mid;

		diff*=D3EditState.texscale;

		fp->face_uvls[i].v=mid+diff;
	}

	World_changed=1;
	
}

void CTextureDialog::OnFaceMap() 
{
	if (Curroomp==NULL)
		return;
	
	face *fp=&Curroomp->faces[Curface];

	if (fp->num_verts!=4)
	{	
		OutrageMessageBox ("Face must have four vertices to use face mapping.");
		return;
	}

	// Simply square off this face

	fp->face_uvls[0].u=0.0;
	fp->face_uvls[0].v=0.0;

	fp->face_uvls[1].u=1.0;
	fp->face_uvls[1].v=0.0;

	fp->face_uvls[2].u=1.0;
	fp->face_uvls[2].v=1.0;

	fp->face_uvls[3].u=0.0;
	fp->face_uvls[3].v=1.0;
	
	World_changed=1;
}

void CTextureDialog::OnCurrentUsed() 
{
	// TODO: Add your control notification handler code here
	int c=IsDlgButtonChecked(IDC_CURRENT_USED);

	// If we just turned this on, build a list of textures used
	if (c)
	{
		Show_used=1;
		memset (CurrentlyUsedTextures,0,MAX_TEXTURES);
		for (int i=0;i<=Highest_room_index;i++)
		{
			room *rp=&Rooms[i];
			if (!rp->used)
				continue;
			for (int t=0;t<rp->num_faces;t++)
			{
				CurrentlyUsedTextures[rp->faces[t].tmap]=1;
			}
		}
	}
	else
		Show_used=0;
	
	Invalidate();
	
}

void CTextureDialog::OnFindTexture() 
{
	int i,t;

	mprintf ((0,"Searching...please wait...\n"));
	int found=0,found_room,found_face;

	for (i=0;i<=Highest_room_index && !found;i++)
	{
		room *rp=&Rooms[i];
		if (rp->used==0)
			continue;

		for (t=0;t<rp->num_faces && !found;t++)
		{
			if (rp->faces[t].tmap==D3EditState.texdlg_texture)
			{
				if (rp!=Curroomp || t!=Curface)
				{
					found_room=i;
					found_face=t;
					found=1;
				}
			}
			
		}
	}

	mprintf ((0,"Done searching!\n"));

	if (found==0)
	{
		mprintf ((0,"Couldn't find another face with that texture!\n"));
	}
	else
	{
		Curroomp=&Rooms[found_room];
		Curface=found_face;
		mprintf ((0,"Found that texture in room %d face %d.\n",found_room,found_face));
		mprintf ((0,"Press C to center on that face\n"));
	}

}
