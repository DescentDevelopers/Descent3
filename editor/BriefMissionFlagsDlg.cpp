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

// BriefMissionFlagsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "editor.h"
#include "BriefMissionFlagsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBriefMissionFlagsDlg dialog


CBriefMissionFlagsDlg::CBriefMissionFlagsDlg(uint32_t setflags,uint32_t unsetflags,CWnd* pParent /*=NULL*/)
	: CDialog(CBriefMissionFlagsDlg::IDD, pParent)
{
	m_Set = setflags;
	m_UnSet = unsetflags;

	//{{AFX_DATA_INIT(CBriefMissionFlagsDlg)
	m_1 = 0;
	m_10 = 0;
	m_11 = 0;
	m_12 = 0;
	m_13 = 0;
	m_14 = 0;
	m_15 = 0;
	m_16 = 0;
	m_17 = 0;
	m_18 = 0;
	m_19 = 0;
	m_2 = 0;
	m_20 = 0;
	m_21 = 0;
	m_22 = 0;
	m_23 = 0;
	m_24 = 0;
	m_25 = 0;
	m_26 = 0;
	m_27 = 0;
	m_28 = 0;
	m_29 = 0;
	m_3 = 0;
	m_30 = 0;
	m_31 = 0;
	m_32 = 0;
	m_4 = 0;
	m_5 = 0;
	m_6 = 0;
	m_7 = 0;
	m_9 = 0;
	m_8 = 0;
	//}}AFX_DATA_INIT
}


void CBriefMissionFlagsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBriefMissionFlagsDlg)
	DDX_Radio(pDX, IDC_D_FLAG1, m_1);
	DDX_Radio(pDX, IDC_D_FLAG10, m_10);
	DDX_Radio(pDX, IDC_D_FLAG11, m_11);
	DDX_Radio(pDX, IDC_D_FLAG12, m_12);
	DDX_Radio(pDX, IDC_D_FLAG13, m_13);
	DDX_Radio(pDX, IDC_D_FLAG14, m_14);
	DDX_Radio(pDX, IDC_D_FLAG15, m_15);
	DDX_Radio(pDX, IDC_D_FLAG16, m_16);
	DDX_Radio(pDX, IDC_D_FLAG17, m_17);
	DDX_Radio(pDX, IDC_D_FLAG18, m_18);
	DDX_Radio(pDX, IDC_D_FLAG19, m_19);
	DDX_Radio(pDX, IDC_D_FLAG2, m_2);
	DDX_Radio(pDX, IDC_D_FLAG20, m_20);
	DDX_Radio(pDX, IDC_D_FLAG21, m_21);
	DDX_Radio(pDX, IDC_D_FLAG22, m_22);
	DDX_Radio(pDX, IDC_D_FLAG23, m_23);
	DDX_Radio(pDX, IDC_D_FLAG24, m_24);
	DDX_Radio(pDX, IDC_D_FLAG25, m_25);
	DDX_Radio(pDX, IDC_D_FLAG26, m_26);
	DDX_Radio(pDX, IDC_D_FLAG27, m_27);
	DDX_Radio(pDX, IDC_D_FLAG28, m_28);
	DDX_Radio(pDX, IDC_D_FLAG29, m_29);
	DDX_Radio(pDX, IDC_D_FLAG3, m_3);
	DDX_Radio(pDX, IDC_D_FLAG30, m_30);
	DDX_Radio(pDX, IDC_D_FLAG31, m_31);
	DDX_Radio(pDX, IDC_D_FLAG32, m_32);
	DDX_Radio(pDX, IDC_D_FLAG4, m_4);
	DDX_Radio(pDX, IDC_D_FLAG5, m_5);
	DDX_Radio(pDX, IDC_D_FLAG6, m_6);
	DDX_Radio(pDX, IDC_D_FLAG7, m_7);
	DDX_Radio(pDX, IDC_D_FLAG9, m_9);
	DDX_Radio(pDX, IDC_D_FLAG8, m_8);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBriefMissionFlagsDlg, CDialog)
	//{{AFX_MSG_MAP(CBriefMissionFlagsDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBriefMissionFlagsDlg message handlers

void CBriefMissionFlagsDlg::OnOK() 
{
	UpdateData(true);

	m_Set = m_UnSet = 0;

	uint32_t bit = 0x01;
	int i,value;

	for(i=0;i<32;i++){

		switch(i){
		case 0:value = m_1;break;
		case 1:value =  m_2;break;
		case 2:value =  m_3;break;
		case 3:value =  m_4;break;
		case 4:value =  m_5;break;
		case 5:value =  m_6;break;
		case 6:value =  m_7;break;
		case 7:value =  m_8;break;
		case 8:value =  m_9;break;
		case 9:value =  m_10;break;
		case 10:value= m_11;break;
		case 11:value= m_12;break;
		case 12:value= m_13;break;
		case 13:value= m_14;break;
		case 14:value= m_15;break;
		case 15:value= m_16;break;
		case 16:value= m_17;break;
		case 17:value= m_18;break;
		case 18:value= m_19;break;
		case 19:value= m_20;break;
		case 20:value= m_21;break;
		case 21:value= m_22;break;
		case 22:value= m_23;break;
		case 23:value= m_24;break;
		case 24:value= m_25;break;
		case 25:value= m_26;break;
		case 26:value= m_27;break;
		case 27:value= m_28;break;
		case 28:value= m_29;break;
		case 29:value= m_30;break;
		case 30:value= m_31;break;
		case 31:value= m_32;break;
		};
 
		switch(value){
		case 0:break;
		case 1: m_Set |= bit; break;
		case 2: m_UnSet |= bit; break;
		};

		bit = bit << 1;
	}
	
	CDialog::OnOK();
}

BOOL CBriefMissionFlagsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	uint32_t bit = 0x01;
	int i;

	//First go through the set bits
	for(i=0;i<32;i++){
		if(m_Set&bit){
			//turn on
			switch(i){
			case 0: m_1 = 1;	break;
			case 1: m_2 = 1;	break;
			case 2: m_3 = 1;	break;
			case 3: m_4 = 1;	break;
			case 4: m_5 = 1;	break;
			case 5: m_6 = 1;	break;
			case 6: m_7 = 1;	break;
			case 7: m_8 = 1;	break;
			case 8: m_9 = 1;	break;
			case 9: m_10 = 1;	break;
			case 10: m_11 = 1;	break;
			case 11: m_12 = 1;	break;
			case 12: m_13 = 1;	break;
			case 13: m_14 = 1;	break;
			case 14: m_15 = 1;	break;
			case 15: m_16 = 1;	break;
			case 16: m_17 = 1;	break;
			case 17: m_18 = 1;	break;
			case 18: m_19 = 1;	break;
			case 19: m_20 = 1;	break;
			case 20: m_21 = 1;	break;
			case 21: m_22 = 1;	break;
			case 22: m_23 = 1;	break;
			case 23: m_24 = 1;	break;
			case 24: m_25 = 1;	break;
			case 25: m_26 = 1;	break;
			case 26: m_27 = 1;	break;
			case 27: m_28 = 1;	break;
			case 28: m_29 = 1;	break;
			case 29: m_30 = 1;	break;
			case 30: m_31 = 1;	break;
			case 31: m_32 = 1;	break;
			};
 
		}
		bit = bit << 1;	
	}
	
	bit = 0x01;
	//now do the turned off bits
	for(i=0;i<32;i++){
		if(m_UnSet&bit){
			//turn on
			switch(i){
			case 0: m_1 = 2;	break;
			case 1: m_2 = 2;	break;
			case 2: m_3 = 2;	break;
			case 3: m_4 = 2;	break;
			case 4: m_5 = 2;	break;
			case 5: m_6 = 2;	break;
			case 6: m_7 = 2;	break;
			case 7: m_8 = 2;	break;
			case 8: m_9 = 2;	break;
			case 9: m_10 = 2;	break;
			case 10: m_11 = 2;	break;
			case 11: m_12 = 2;	break;
			case 12: m_13 = 2;	break;
			case 13: m_14 = 2;	break;
			case 14: m_15 = 2;	break;
			case 15: m_16 = 2;	break;
			case 16: m_17 = 2;	break;
			case 17: m_18 = 2;	break;
			case 18: m_19 = 2;	break;
			case 19: m_20 = 2;	break;
			case 20: m_21 = 2;	break;
			case 21: m_22 = 2;	break;
			case 22: m_23 = 2;	break;
			case 23: m_24 = 2;	break;
			case 24: m_25 = 2;	break;
			case 25: m_26 = 2;	break;
			case 26: m_27 = 2;	break;
			case 27: m_28 = 2;	break;
			case 28: m_29 = 2;	break;
			case 29: m_30 = 2;	break;
			case 30: m_31 = 2;	break;
			case 31: m_32 = 2;	break;
			};
 
		}
		bit = bit << 1;	
	}

	UpdateData(false);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
