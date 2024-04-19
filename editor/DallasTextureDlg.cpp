// DallasTextureDlg.cpp : implementation file
//

#include "stdafx.h"
#include "editor.h"
#include "gametexture.h"
#include "DallasTextureDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDallasTextureDlg dialog


CDallasTextureDlg::CDallasTextureDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDallasTextureDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDallasTextureDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_TextureName="";
	m_TextureIndex=-1;
}


void CDallasTextureDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDallasTextureDlg)
	DDX_Control(pDX, IDC_TEXTURE_LIST, m_TextureList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDallasTextureDlg, CDialog)
	//{{AFX_MSG_MAP(CDallasTextureDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDallasTextureDlg message handlers

BOOL CDallasTextureDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	FillTextureList();
	
	if(!m_TextureName.IsEmpty())
		m_TextureList.SelectString(-1,m_TextureName.GetBuffer(0));
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDallasTextureDlg::OnOK() 
{
	int index=m_TextureList.GetCurSel();
	if(index==LB_ERR) return;

	m_TextureIndex=m_TextureList.GetItemData(index);
	
	CDialog::OnOK();
}

void CDallasTextureDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

void CDallasTextureDlg::FillTextureList(void)
{
	int i;

	// Fill the menus with sounds
	for (i=0;i<MAX_TEXTURES;i++) {
		if((GameTextures[i].used) && (strlen(GameTextures[i].name)>0)) {
			int index;
			index=m_TextureList.AddString(GameTextures[i].name);
			if(index!=LB_ERR) {
				m_TextureList.SetItemData(index,i);
			}
		}
	}
}
