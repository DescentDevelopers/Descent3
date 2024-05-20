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

#if !defined(AFX_BRIEFEDIT_H__839B53D1_41D6_11D2_B359_006008BF0B09__INCLUDED_)
#define AFX_BRIEFEDIT_H__839B53D1_41D6_11D2_B359_006008BF0B09__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// BriefEdit.h : header file
//

#include "pstypes.h"
#include "telcomefxstructs.h"
#include "telcom.h"
#include "gr.h"
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CBriefEdit dialog

#define MAX_EFFECTS_PER_SCREEN	32

//	BriefGlobalValues
//
//	struct for global variables for a telcom briefing
struct{
	char title[128];
	float static_val,glitch_val;
}BriefGlobalValues;

#define BE_NONE			-1
#define BE_TEXT			0
#define BE_BMP			1
#define BE_MOVIE		2
#define BE_BKG			3
#define BE_POLY			4
#define BE_SND			5
#define BE_BUTTON		6
//	tBriefEffect
//
//	struct for a briefing effect
struct{
	uint8_t type;							//type of effect (for union)
	bool used;							//is the effect being used
	int id;								//ID# used
	char *text;							//buffer of text
	char description[128];
	int w,h;//used for bitmaps
	union{
		TCTEXTDESC		text_desc;
		TCBMPDESC		bmp_desc;
		TCMOVIEDESC		movie_desc;
		TCBKGDESC		bkg_desc;
		TCPOLYDESC		poly_desc;
		TCSNDDESC		snd_desc;
		TCBUTTONDESC	button_desc;
	};
	int next,prev;						//next/prev effect in list
}tBriefEffect;

struct{
	bool used;										//is screen being used
	int root_effect;								//root effect to start the list
	char layout[_MAX_PATH];							//layout screen to use
	tBriefEffect effects[MAX_EFFECTS_PER_SCREEN];	//the effects for the screen
	uint32_t mission_mask_set,mission_mask_unset;
	int next,prev;									//next/prev screen in list
}tBriefScreen;

extern int Briefing_root_screen;
extern tBriefScreen Briefing_screens[MAX_TELCOM_SCREENS];

/*
 ****************************************************************************
 *			Functions to manipulate effects/screens in briefing editor		*
 ****************************************************************************
 *
*/

//	BriefEditInitScreens
//
//	Initializes the briefing screens so they are empty
void BriefEditInitScreens(void);

//	BriefEditInitEffect
//	
//	Initializes an effect to empty
void BriefEditInitEffect(tBriefEffect *efx);

//	BriefEditFreeScreens
//
//	Closes down all the briefing screens, freeing memory
void BriefEditFreeScreens(void);

//	BriefEditFreeEffect
//
//	Frees up any memory allocated in an effect
void BriefEditFreeEffect(tBriefEffect *efx);

//	BriefEditSaveScreens
//
//	Saves out the screens to a briefing file
uint8_t BriefEditSaveScreens(char *filename,CComboBox *screen_combo,BriefGlobalValues *glob);

//  BriefEditLoadScreens
//
//	Loads in screens from a briefing file
uint8_t BriefEditLoadScreens(char *filename,CComboBox *screen_combo,CComboBox *effect_combo,BriefGlobalValues *glob);


#define MAX_LAYOUT_PREDEFS	10
struct{
	char filename[_MAX_PATH];
	int num_texts,num_bmps;
	struct{
		int lx,rx,ty,by;
	}texts[MAX_LAYOUT_PREDEFS];
	struct{
		int x,y;
	}bmps[MAX_LAYOUT_PREDEFS];
}tLayoutScreen;

extern tLayoutScreen *PBlayouts;
extern int *PBnum_layouts;

class CBriefEdit : public CDialog
{
// Construction
public:
	BriefGlobalValues m_GlobalValues;
	tLayoutScreen *layouts;
	int num_layouts;
	CBriefEdit(CWnd* pParent = NULL);   // standard constructor
	void ParseLayoutScreenFile(char *filename);	
	void SetLayoutBitmap(char *filename);
	bool has_changed;
	//	BriefEditAllocateEffect
	//
	//	Allocates an effect (links it and all) to be edited.  Returns false on error
	bool BriefEditAllocateEffect(int *screen,int *eff);
	//	BriefEditGetCurScreenEffect
	//
	//	Fills in the current screen and effect that is being worked on
	void BriefEditGetCurScreenEffect(int *ret_screen,int *ret_effect);

// Dialog Data
	//{{AFX_DATA(CBriefEdit)
	enum { IDD = IDD_BRIEF_MAIN };
	float	m_fGlitch;
	float	m_fStatic;
	CString	m_sTitle;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBriefEdit)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	int layout_bmp;
	void UpdateView();
	void UpdateInfo();
	void UpdateLayoutBitmap(void);
	void DrawRect(ddgr_color color,int lx,int ty,int rx,int by,grViewport *vport);
	
	grHardwareSurface m_ObjectSurf;

	// Generated message map functions
	//{{AFX_MSG(CBriefEdit)
	virtual void OnOK();
	afx_msg void OnBriefEffectBitmap();
	afx_msg void OnBriefEffectButton();
	afx_msg void OnBriefEffectDel();
	afx_msg void OnBriefEffectEdit();
	afx_msg void OnBriefEffectMovie();
	afx_msg void OnBriefEffectSound();
	afx_msg void OnBriefEffectText();
	afx_msg void OnBriefScreenAdd();
	afx_msg void OnBriefScreenDel();
	afx_msg void OnBriefScreenEdit();
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnSelchangeBriefScreenList();
	afx_msg void OnBriefSave();
	afx_msg void OnLoad();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnManage();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BRIEFEDIT_H__839B53D1_41D6_11D2_B359_006008BF0B09__INCLUDED_)
