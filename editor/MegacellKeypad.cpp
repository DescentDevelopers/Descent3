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

// MegacellKeypad.cpp : implementation file
//

#include "stdafx.h"
#include "editor.h"
#include "MegacellKeypad.h"
#include "megacell.h"
#include "terrain.h"
#include "terraindialog.h"




#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMegacellKeypad dialog

int CMegacellKeypad::m_Instances = 0;


CMegacellKeypad::CMegacellKeypad(CWnd* pParent /*=NULL*/)
	: CKeypadDialog(CMegacellKeypad::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMegacellKeypad)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CMegacellKeypad::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMegacellKeypad)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMegacellKeypad, CDialog)
	//{{AFX_MSG_MAP(CMegacellKeypad)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_NEXT_MEGA_SET, OnNextMegaSet)
	ON_BN_CLICKED(IDC_PREV_MEGA_SET, OnPrevMegaSet)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_FILL_WITH_MEGACELL, OnFillWithMegacell)
	ON_BN_CLICKED(IDC_RANDOMIZE_MEGACELL_CHECK, OnRandomizeMegacellCheck)
	ON_BN_CLICKED(IDC_SELECT_FROM_MEGACELL, OnSelectFromMegacell)
	ON_BN_CLICKED(IDC_UNIFY_MEGACELL, OnUnifyMegacell)
	ON_WM_HELPINFO()
	ON_EN_KILLFOCUS(IDC_X_GRANULAR_EDIT, OnKillfocusXGranularEdit)
	ON_EN_KILLFOCUS(IDC_Y_GRANULAR_EDIT, OnKillfocusYGranularEdit)
	ON_BN_CLICKED(IDC_HIGHLIGHT_BORDERS, OnHighlightBorders)
	ON_BN_CLICKED(IDC_MEGACELL_SKYDOME, OnMegacellSkydome)
	ON_BN_CLICKED(IDC_MEGACELL_SKYBAND, OnMegacellSkyband)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMegacellKeypad message handlers

void CMegacellKeypad::OnDestroy() 
{
	CDialog::OnDestroy();
	
	if (m_Instances > 0) m_Instances--;	
}


BOOL CMegacellKeypad::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	((CButton *)GetDlgItem(IDC_RANDOMIZE_MEGACELL_CHECK))->SetCheck(D3EditState.randomize_megacell);

	m_Instances++;
	m_xgran=1;
	m_ygran=1;

	UpdateDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CMegacellKeypad::OnSize(UINT nType, int cx, int cy) 
{
	CKeypadDialog::OnSize(nType, cx, cy);

//	Add your code here, sweetie.	
}


void CMegacellKeypad::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	
	CKeypadDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}


void CMegacellKeypad::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	
	CKeypadDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}



void CMegacellKeypad::OnNextMegaSet() 
{
	D3EditState.current_megacell=GetNextMegacell (D3EditState.current_megacell);
	m_xgran=m_ygran=1;
	UpdateDialog();

}

void CMegacellKeypad::OnPrevMegaSet() 
{
	D3EditState.current_megacell=GetPrevMegacell (D3EditState.current_megacell);
	m_xgran=m_ygran=1;
	UpdateDialog();
}

void CMegacellKeypad::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

//	This is added so we don't paint this dialog when it's inactive.
	if (!m_Active) return;

	UpdateDialog();
}


void CMegacellKeypad::UpdateDialog() 
{
	if (Num_megacells<1)
		return;

	int n=D3EditState.current_megacell;
	char str[255];
	CEdit *ebox;

	if (!Megacells[n].used)
	{
		n=GetNextMegacell (n);
		D3EditState.current_megacell=n;
	}


	ebox=(CEdit *) GetDlgItem (IDC_MEGACELL_NAME_STATIC);
	sprintf (str,"Megacell name: %s",Megacells[n].name);
	ebox->SetWindowText (str);

	ebox=(CEdit *) GetDlgItem (IDC_MEGA_WIDTH_STATIC);
	sprintf (str,"Width: %d",Megacells[n].width);
	ebox->SetWindowText (str);

	ebox=(CEdit *) GetDlgItem (IDC_MEGA_HEIGHT_STATIC);
	sprintf (str,"Height: %d",Megacells[n].height);
	ebox->SetWindowText (str);

	ebox=(CEdit *) GetDlgItem (IDC_X_GRANULAR_EDIT);
	sprintf (str,"%d",m_xgran);
	ebox->SetWindowText (str);

	ebox=(CEdit *) GetDlgItem (IDC_Y_GRANULAR_EDIT);
	sprintf (str,"%d",m_ygran);
	ebox->SetWindowText (str);



	UpdateMegaSetView();
}

void CMegacellKeypad::UpdateMegaSetView() 
{
	int n=D3EditState.current_megacell;
	CWnd *texwnd;
	RECT rect;
	int x, y, bm_handle,w,h;
	int megawidth=Megacells[n].width;
	int megaheight=Megacells[n].height;
	int i,t;
	
	Desktop_surf->attach_to_window((unsigned)m_hWnd);

	texwnd = GetDlgItem(IDC_MEGACELL_SET_VIEW);
	texwnd->GetWindowRect(&rect);
	ScreenToClient(&rect);
	w=rect.right-rect.left;
	h=rect.bottom-rect.top;


	Desktop_surf->clear(rect.left,rect.top,w,h);

	if (megawidth > 0) {
		int texwidth=w/megawidth;
		int texheight=h/megaheight;

	// Don't have to clear if we're overwriting the whole area!

		for (i=0;i<megaheight;i++)
		{
			for (t=0;t<megawidth;t++)
			{
				m_TextureSurf.create(texwidth, texheight, BPP_16);
						
				bm_handle = GetTextureBitmap (Megacells[n].texture_handles[i*MAX_MEGACELL_WIDTH+t],0);
				m_TextureSurf.load(bm_handle);
				
				x = 1+rect.left + (t*texwidth);
				y = 1+rect.top + (i*texheight);
			
				Desktop_surf->blt(x, y, &m_TextureSurf);
						
				m_TextureSurf.free();
			}
		}
	}

	Desktop_surf->attach_to_window((unsigned)NULL);
}


void CMegacellKeypad::OnFillWithMegacell() 
{
	return;
	
}

void CMegacellKeypad::OnRandomizeMegacellCheck() 
{
	// TODO: Add your control notification handler code here
	D3EditState.randomize_megacell = (((CButton *)GetDlgItem(IDC_RANDOMIZE_MEGACELL_CHECK))->GetCheck() != 0);
	
}

// Selects all the cells that has textures from the currently selected megacell
void CMegacellKeypad::OnSelectFromMegacell() 
{

}


// Fill only the selected cells with this megacell
void CMegacellKeypad::OnUnifyMegacell() 
{
	
}

BOOL CMegacellKeypad::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	// TODO: Add your message handler code here and/or call default
	WinHelp(HID_MEGACELLTAB,HELP_CONTEXT);
	return TRUE;
	//return CDialog::OnHelpInfo(pHelpInfo);
}

void CMegacellKeypad::OnKillfocusXGranularEdit() 
{
	CEdit *ebox;
	char name[20];
	int n=D3EditState.current_megacell;
	int val;
	
	ebox=(CEdit *) GetDlgItem (IDC_X_GRANULAR_EDIT);
	ebox->GetWindowText (name,20);

	val=atoi(name);

	if (val<1)
		val=1;
	if (val>Megacells[n].width)
		val=Megacells[n].width;

	m_xgran=val;

	UpdateDialog();
	
}

void CMegacellKeypad::OnKillfocusYGranularEdit() 
{
	CEdit *ebox;
	char name[20];
	int n=D3EditState.current_megacell;
	int val;
	
	ebox=(CEdit *) GetDlgItem (IDC_Y_GRANULAR_EDIT);
	ebox->GetWindowText (name,20);

	val=atoi(name);

	if (val<1)
		val=1;
	if (val>Megacells[n].height)
		val=Megacells[n].height;

	m_ygran=val;

	UpdateDialog();
	
}

void CMegacellKeypad::OnHighlightBorders() 
{
			
}

void CMegacellKeypad::OnMegacellSkydome() 
{
	
}

void CMegacellKeypad::OnMegacellSkyband() 
{
	
}
