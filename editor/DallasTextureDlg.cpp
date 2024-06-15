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

CDallasTextureDlg::CDallasTextureDlg(CWnd *pParent /*=NULL*/) : CDialog(CDallasTextureDlg::IDD, pParent) {
  //{{AFX_DATA_INIT(CDallasTextureDlg)
  // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT

  m_TextureName = "";
  m_TextureIndex = -1;
}

void CDallasTextureDlg::DoDataExchange(CDataExchange *pDX) {
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

BOOL CDallasTextureDlg::OnInitDialog() {
  CDialog::OnInitDialog();

  FillTextureList();

  if (!m_TextureName.IsEmpty())
    m_TextureList.SelectString(-1, m_TextureName.GetBuffer(0));

  return TRUE; // return TRUE unless you set the focus to a control
               // EXCEPTION: OCX Property Pages should return FALSE
}

void CDallasTextureDlg::OnOK() {
  int index = m_TextureList.GetCurSel();
  if (index == LB_ERR)
    return;

  m_TextureIndex = m_TextureList.GetItemData(index);

  CDialog::OnOK();
}

void CDallasTextureDlg::OnCancel() {
  // TODO: Add extra cleanup here

  CDialog::OnCancel();
}

void CDallasTextureDlg::FillTextureList(void) {
  int i;

  // Fill the menus with sounds
  for (i = 0; i < MAX_TEXTURES; i++) {
    if ((GameTextures[i].used) && (strlen(GameTextures[i].name) > 0)) {
      int index;
      index = m_TextureList.AddString(GameTextures[i].name);
      if (index != LB_ERR) {
        m_TextureList.SetItemData(index, i);
      }
    }
  }
}
