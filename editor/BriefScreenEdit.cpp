// BriefScreenEdit.cpp : implementation file
//

#include "stdafx.h"
#include "editor.h"
#include "BriefScreenEdit.h"
#include "BriefEdit.h"
#include "BriefMissionFlagsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBriefScreenEdit dialog


CBriefScreenEdit::CBriefScreenEdit(int screen_idx,CWnd* pParent /*=NULL*/)
	: CDialog(CBriefScreenEdit::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBriefScreenEdit)
	m_sDesc = _T("");
	//}}AFX_DATA_INIT
	m_Screen = screen_idx;
	bm_handle = -1;
	m_Set = m_UnSet = 0;
}


void CBriefScreenEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBriefScreenEdit)
	DDX_Text(pDX, IDC_BRIEF_ADDS_DESC, m_sDesc);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBriefScreenEdit, CDialog)
	//{{AFX_MSG_MAP(CBriefScreenEdit)
	ON_WM_TIMER()
	ON_CBN_SELCHANGE(IDC_BRIEF_ADDS_LAYOUT_LIST, OnSelchangeBriefAddsLayoutList)
	ON_BN_CLICKED(IDC_MISSIONFLAGS, OnMissionflags)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBriefScreenEdit message handlers

void CBriefScreenEdit::OnOK() 
{
	CComboBox *combo = (CComboBox *)GetDlgItem(IDC_BRIEF_ADDS_LAYOUT_LIST);
	int sel = combo->GetCurSel();
	if(sel>0){
		 combo->GetLBText(sel,layout_str);
	}else{
		layout_str = "";
	}	

	if(bm_handle>BAD_BITMAP_HANDLE)
		bm_FreeBitmap(bm_handle);
	
	CDialog::OnOK();
}

BOOL CBriefScreenEdit::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CComboBox *combo = (CComboBox *)GetDlgItem(IDC_BRIEF_ADDS_LAYOUT_LIST);

	//Start the timer
	CWnd::SetTimer(1,50,NULL);

	combo->AddString("<None>");
	int sel = 0;

	for(int i=0;i<(*PBnum_layouts);i++){
		combo->AddString(PBlayouts[i].filename);
		if(m_Screen!=-1){
			if(!stricmp(Briefing_screens[m_Screen].layout,PBlayouts[i].filename))
				sel = i+1;
		}
	}

	if(m_Screen!=-1){
		m_Set = Briefing_screens[m_Screen].mission_mask_set;
		m_UnSet = Briefing_screens[m_Screen].mission_mask_unset;
	}

	combo->SetCurSel(sel);
	
	return TRUE;
}

void CBriefScreenEdit::UpdateView(void)
{
	CWnd *objwnd;
	RECT rect;
	int x,y,w,h;

	objwnd = GetDlgItem(IDC_BRIEF_ADDS_PICTURE);
	objwnd->GetWindowRect(&rect);
	ScreenToClient(&rect);

	int bmw,bmh;

	Desktop_surf->attach_to_window((unsigned)m_hWnd);
	
	w=rect.right-rect.left;
	h=rect.bottom-rect.top;

	bmw = w;
	bmh = h;

	static bool first_call = true;
	
	if(first_call)
		Desktop_surf->clear(rect.left,rect.top,w,h);

	m_ObjectSurf.create(bmw, bmh, BPP_16);

	if(bm_handle>BAD_BITMAP_HANDLE){
		m_ObjectSurf.load(bm_handle);
	}else
		Desktop_surf->clear(rect.left,rect.top,w,h);
	
	x = rect.left + (w/2) - m_ObjectSurf.width()/2; 
	y = rect.top + (h/2) - m_ObjectSurf.height()/2;

	Desktop_surf->blt(x, y, &m_ObjectSurf);

	m_ObjectSurf.free();
	Desktop_surf->attach_to_window((unsigned)NULL);

	if(first_call)
		first_call = false;
}

void CBriefScreenEdit::OnTimer(UINT nIDEvent) 
{
	UpdateView();	
	CDialog::OnTimer(nIDEvent);
}

void CBriefScreenEdit::OnSelchangeBriefAddsLayoutList() 
{
	if(bm_handle>BAD_BITMAP_HANDLE){
		bm_FreeBitmap(bm_handle);
		bm_handle = -1;
	}

	CComboBox *combo = (CComboBox *)GetDlgItem(IDC_BRIEF_ADDS_LAYOUT_LIST);
	int sel = combo->GetCurSel();
	if(sel>0){
		bm_handle = bm_AllocLoadFileBitmap(PBlayouts[sel-1].filename,0);
	}

	CWnd *objwnd;
	RECT rect;
	int w,h;

	objwnd = GetDlgItem(IDC_BRIEF_ADDS_PICTURE);
	objwnd->GetWindowRect(&rect);
	w=rect.right-rect.left;
	h=rect.bottom-rect.top;

	int temp = bm_AllocBitmap(w,h,0);
	if(temp>BAD_BITMAP_HANDLE){
		bm_ClearBitmap(temp);
		if(bm_handle>BAD_BITMAP_HANDLE){
			bm_ScaleBitmapToBitmap (temp,bm_handle);
		}
		bm_FreeBitmap(bm_handle);
		bm_handle = temp;
	}
}

void CBriefScreenEdit::OnMissionflags() 
{
	CBriefMissionFlagsDlg dlg(m_Set,m_UnSet);

	if(dlg.DoModal()){
		m_Set = dlg.m_Set;
		m_UnSet = dlg.m_UnSet;
	}	
}
