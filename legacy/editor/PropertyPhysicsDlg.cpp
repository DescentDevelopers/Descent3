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

// PropertyPhysicsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "editor.h"
#include "PropertyPhysicsDlg.h"
#include "object.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropertyPhysicsDlg dialog


CPropertyPhysicsDlg::CPropertyPhysicsDlg(CWnd* pParent /*=NULL*/)
	: CKeypadDialog(CPropertyPhysicsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPropertyPhysicsDlg)
	m_bBounce = FALSE;
	m_fDrag = 0.0f;
	m_fFullRotThrust = 0.0f;
	m_fFullThrust = 0.0f;
	m_bGravity = FALSE;
	m_bLevelling = FALSE;
	m_bMagnetism = FALSE;
	m_fMass = 0.0f;
	m_fMaxTurnrollrate = 0.0f;
	m_iNumBounces = 0;
	m_bPersistent = FALSE;
	m_fRotDrag = 0.0f;
	m_fRotThrustX = 0.0f;
	m_fRotThrustY = 0.0f;
	m_fRotThrustZ = 0.0f;
	m_fRotVelX = 0.0f;
	m_fRotVelY = 0.0f;
	m_fRotVelZ = 0.0f;
	m_bSticks = FALSE;
	m_fThrustX = 0.0f;
	m_fThrustY = 0.0f;
	m_fThrustZ = 0.0f;
	m_bTurnroll = FALSE;
	m_fTurnrollRatio = 0.0f;
	m_sTurnrollAngle = 0;
	m_bUsesThrust = FALSE;
	m_fVelocityX = 0.0f;
	m_fVelocityY = 0.0f;
	m_fVelocityZ = 0.0f;
	m_bWiggle = FALSE;
	m_fWiggleAmplitude = 0.0f;
	m_fWigglesPerSecond = 0.0f;
	m_bWind = FALSE;
	m_bUnlimitedBounce = FALSE;
	//}}AFX_DATA_INIT
}


void CPropertyPhysicsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropertyPhysicsDlg)
	DDX_Check(pDX, IDC_PBOUNCE, m_bBounce);
	DDX_Text(pDX, IDC_PDRAG, m_fDrag);
	DDX_Text(pDX, IDC_PFULL_ROTTHRUST, m_fFullRotThrust);
	DDX_Text(pDX, IDC_PFULL_THRUST, m_fFullThrust);
	DDX_Check(pDX, IDC_PGRAVITY, m_bGravity);
	DDX_Check(pDX, IDC_PLEVELLING, m_bLevelling);
	DDX_Check(pDX, IDC_PMAGNETISM, m_bMagnetism);
	DDX_Text(pDX, IDC_PMASS, m_fMass);
	DDX_Text(pDX, IDC_PMAX_TURNROLL_RATE, m_fMaxTurnrollrate);
	DDX_Text(pDX, IDC_PNUM_BOUNCES, m_iNumBounces);
	DDX_Check(pDX, IDC_PPERSISTENT, m_bPersistent);
	DDX_Text(pDX, IDC_PROTDRAG, m_fRotDrag);
	DDX_Text(pDX, IDC_PROTTHRUST_X, m_fRotThrustX);
	DDX_Text(pDX, IDC_PROTTHRUST_Y, m_fRotThrustY);
	DDX_Text(pDX, IDC_PROTTHRUST_Z, m_fRotThrustZ);
	DDX_Text(pDX, IDC_PROTVEL_X, m_fRotVelX);
	DDX_Text(pDX, IDC_PROTVEL_Y, m_fRotVelY);
	DDX_Text(pDX, IDC_PROTVEL_Z, m_fRotVelZ);
	DDX_Check(pDX, IDC_PSTICKS, m_bSticks);
	DDX_Text(pDX, IDC_PTHRUST_X, m_fThrustX);
	DDX_Text(pDX, IDC_PTHRUST_Y, m_fThrustY);
	DDX_Text(pDX, IDC_PTHRUST_Z, m_fThrustZ);
	DDX_Check(pDX, IDC_PTURNROLL, m_bTurnroll);
	DDX_Text(pDX, IDC_PTURNROLL_RATIO, m_fTurnrollRatio);
	DDX_Text(pDX, IDC_PTURNROLLANGLE, m_sTurnrollAngle);
	DDX_Check(pDX, IDC_PUSESTHRUST, m_bUsesThrust);
	DDX_Text(pDX, IDC_PVELOCITY_X, m_fVelocityX);
	DDX_Text(pDX, IDC_PVELOCITY_Y, m_fVelocityY);
	DDX_Text(pDX, IDC_PVELOCITY_Z, m_fVelocityZ);
	DDX_Check(pDX, IDC_PWIGGLE, m_bWiggle);
	DDX_Text(pDX, IDC_PWIGGLE_AMPLITUDE, m_fWiggleAmplitude);
	DDX_Text(pDX, IDC_PWIGGLES_PER_SECOND, m_fWigglesPerSecond);
	DDX_Check(pDX, IDC_PWIND, m_bWind);
	DDX_Check(pDX, IDC_PUNLIMBOUNCE, m_bUnlimitedBounce);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropertyPhysicsDlg, CDialog)
	//{{AFX_MSG_MAP(CPropertyPhysicsDlg)
	ON_BN_CLICKED(IDC_PTURNROLL, OnPturnroll)
	ON_BN_CLICKED(IDC_PLEVELLING, OnPlevelling)
	ON_BN_CLICKED(IDC_PBOUNCE, OnPbounce)
	ON_BN_CLICKED(IDC_PWIGGLE, OnPwiggle)
	ON_BN_CLICKED(IDC_PSTICKS, OnPsticks)
	ON_BN_CLICKED(IDC_PPERSISTENT, OnPpersistent)
	ON_BN_CLICKED(IDC_PUSESTHRUST, OnPusesthrust)
	ON_BN_CLICKED(IDC_PGRAVITY, OnPgravity)
	ON_BN_CLICKED(IDC_PMAGNETISM, OnPmagnetism)
	ON_BN_CLICKED(IDC_PWIND, OnPwind)
	ON_BN_CLICKED(IDC_PUNLIMBOUNCE, OnPunlimbounce)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertyPhysicsDlg message handlers

void CPropertyPhysicsDlg::OnPturnroll() 
{
	// TODO: Add your control notification handler code here
	if(m_bTurnroll)
	{
		m_bTurnroll=false;
		Disable(IDC_PTURNROLL);
	}
	else
	{
		m_bTurnroll=true;
		Enable(IDC_PTURNROLL);
	}
}

void CPropertyPhysicsDlg::OnPlevelling() 
{
	// TODO: Add your control notification handler code here
	if(m_bLevelling)
	{
		m_bLevelling=false;
		Disable(IDC_PLEVELLING);
	}
	else
	{
		m_bLevelling=true;
		Enable(IDC_PLEVELLING);
	}	
}

void CPropertyPhysicsDlg::OnPbounce() 
{
	// TODO: Add your control notification handler code here
	if(m_bBounce)
	{
		m_bBounce=false;
		Disable(IDC_PBOUNCE);
	}
	else
	{
		m_bBounce=true;
		Enable(IDC_PBOUNCE);
	}	
}

void CPropertyPhysicsDlg::OnPwiggle() 
{
	// TODO: Add your control notification handler code here
	if(m_bWiggle)
	{
		m_bWiggle=false;
		Disable(IDC_PWIGGLE);
	}
	else
	{
		m_bWiggle=true;
		Enable(IDC_PWIGGLE);
	}	
}

void CPropertyPhysicsDlg::OnPsticks() 
{
	// TODO: Add your control notification handler code here
	if(m_bSticks)
	{
		m_bSticks=false;
		Disable(IDC_PSTICKS);
	}
	else
	{
		m_bSticks=true;
		Enable(IDC_PSTICKS);
	}	
}

void CPropertyPhysicsDlg::OnPpersistent() 
{
	// TODO: Add your control notification handler code here
	if(m_bPersistent)
	{
		m_bPersistent=false;
		Disable(IDC_PPERSISTENT);
	}
	else
	{
		m_bPersistent=true;
		Enable(IDC_PPERSISTENT);
	}	
}

void CPropertyPhysicsDlg::OnPusesthrust() 
{
	// TODO: Add your control notification handler code here
	if(m_bUsesThrust)
	{
		m_bUsesThrust=false;
		Disable(IDC_PUSESTHRUST);
	}
	else
	{
		m_bUsesThrust=true;
		Enable(IDC_PUSESTHRUST);
	}	
}

void CPropertyPhysicsDlg::OnPgravity() 
{
	// TODO: Add your control notification handler code here
	if(m_bGravity)
	{
		m_bGravity=false;
		Disable(IDC_PGRAVITY);
	}
	else
	{
		m_bGravity=true;
		Enable(IDC_PGRAVITY);
	}	
}

void CPropertyPhysicsDlg::OnPmagnetism() 
{
	// TODO: Add your control notification handler code here
	if(m_bMagnetism)
	{
		m_bMagnetism=false;
		Disable(IDC_PMAGNETISM);
	}
	else
	{
		m_bMagnetism=true;
		Enable(IDC_PMAGNETISM);
	}	
}

void CPropertyPhysicsDlg::OnPwind() 
{
	// TODO: Add your control notification handler code here
	if(m_bWind)
	{
		m_bWind=false;
		Disable(IDC_PWIND);
	}
	else
	{
		m_bWind=true;
		Enable(IDC_PWIND);
	}	
}

BOOL CPropertyPhysicsDlg::OnInitDialog() 
{

	// TODO: Add extra initialization here
	object *curobj;

	curobj=&Objects[Cur_object_index];

	m_fVelocityX=curobj->mtype.phys_info.velocity.x;
	m_fVelocityY=curobj->mtype.phys_info.velocity.y;
	m_fVelocityZ=curobj->mtype.phys_info.velocity.z;
	
	m_fThrustX=curobj->mtype.phys_info.thrust.x;
	m_fThrustY=curobj->mtype.phys_info.thrust.y;
	m_fThrustZ=curobj->mtype.phys_info.thrust.z;
	
	m_fRotVelX=curobj->mtype.phys_info.rotvel.x;
	m_fRotVelY=curobj->mtype.phys_info.rotvel.y;
	m_fRotVelZ=curobj->mtype.phys_info.rotvel.z;

	m_fRotThrustX=curobj->mtype.phys_info.rotthrust.x;
	m_fRotThrustY=curobj->mtype.phys_info.rotthrust.y;
	m_fRotThrustZ=curobj->mtype.phys_info.rotthrust.z;

	m_sTurnrollAngle=curobj->mtype.phys_info.turnroll;

	m_iNumBounces=curobj->mtype.phys_info.num_bounces;
	if(m_iNumBounces==-1) m_bUnlimitedBounce=true;

	m_fMass=curobj->mtype.phys_info.mass;

	m_fDrag=curobj->mtype.phys_info.drag;

	m_fRotDrag=curobj->mtype.phys_info.rotdrag;

	m_fFullThrust=curobj->mtype.phys_info.full_thrust;

	m_fFullRotThrust=curobj->mtype.phys_info.full_rotthrust;

	m_fMaxTurnrollrate=curobj->mtype.phys_info.max_turnroll_rate;

	m_fTurnrollRatio=curobj->mtype.phys_info.turnroll_ratio;

	m_fWiggleAmplitude=curobj->mtype.phys_info.wiggle_amplitude;

	m_fWigglesPerSecond=curobj->mtype.phys_info.wiggles_per_sec;

	if(curobj->mtype.phys_info.flags&PF_TURNROLL)		m_bTurnroll=true;
	if(curobj->mtype.phys_info.flags&PF_LEVELING)		m_bLevelling=true;
	if(curobj->mtype.phys_info.flags&PF_BOUNCE)			m_bBounce=true;
	if(curobj->mtype.phys_info.flags&PF_WIGGLE)			m_bWiggle=true;
	if(curobj->mtype.phys_info.flags&PF_STICK)			if(m_bBounce) m_bSticks=false; else m_bSticks=true;
	if(curobj->mtype.phys_info.flags&PF_PERSISTENT)		m_bPersistent=true;
	if(curobj->mtype.phys_info.flags&PF_USES_THRUST)	m_bUsesThrust=true;
	if(curobj->mtype.phys_info.flags&PF_GRAVITY)		m_bGravity=true;
	if(curobj->mtype.phys_info.flags&PF_IGNORE_OWN_CONC_FORCES)		m_bMagnetism=true;
	if(curobj->mtype.phys_info.flags&PF_WIND)			m_bWind=true;
	
	CDialog::OnInitDialog();	
	UpdateCheckBoxes();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPropertyPhysicsDlg::Enable(UINT control)
{
	CWnd *box;
	switch(control)
	{
	case IDC_PTURNROLL: 
		{
			box=(CWnd *)GetDlgItem(IDC_PTURNROLLANGLE);
			box->EnableWindow(true);
			box=(CWnd *)GetDlgItem(IDC_PMAX_TURNROLL_RATE);
			box->EnableWindow(true);
			box=(CWnd *)GetDlgItem(IDC_PTURNROLL_RATIO);
			box->EnableWindow(true);
			break;
		}
	case IDC_PLEVELLING: break;
	case IDC_PBOUNCE: 
		{
			box=(CWnd *)GetDlgItem(IDC_PUNLIMBOUNCE);
			box->EnableWindow(true);
			if(!m_bUnlimitedBounce)
			{
				box=(CWnd *)GetDlgItem(IDC_PNUM_BOUNCES);
				box->EnableWindow(true);
			}
			else
			{
				box=(CWnd *)GetDlgItem(IDC_PNUM_BOUNCES);
				box->EnableWindow(false);
			}
			box=(CWnd *)GetDlgItem(IDC_PSTICKS);
			box->EnableWindow(false);
			break;
		}
	case IDC_PWIGGLE:
		{
			box=(CWnd *)GetDlgItem(IDC_PWIGGLE_AMPLITUDE);
			box->EnableWindow(true);
			box=(CWnd *)GetDlgItem(IDC_PWIGGLES_PER_SECOND);
			box->EnableWindow(true);
			break;
		}
	case IDC_PSTICKS:
		{
			box=(CWnd *)GetDlgItem(IDC_PSTICKS);
			box->EnableWindow(true);
			box=(CWnd *)GetDlgItem(IDC_PBOUNCE);
			box->EnableWindow(false);
			box=(CWnd *)GetDlgItem(IDC_PNUM_BOUNCES);
			box->EnableWindow(false);
			box=(CWnd *)GetDlgItem(IDC_PUNLIMBOUNCE);
			box->EnableWindow(false);
			break;
		}
	case IDC_PPERSISTENT: break;
	case IDC_PUSESTHRUST:
		{
			box=(CWnd *)GetDlgItem(IDC_PTHRUST_X);
			box->EnableWindow(true);
			box=(CWnd *)GetDlgItem(IDC_PTHRUST_Y);
			box->EnableWindow(true);
			box=(CWnd *)GetDlgItem(IDC_PTHRUST_Z);
			box->EnableWindow(true);
			box=(CWnd *)GetDlgItem(IDC_PROTTHRUST_X);
			box->EnableWindow(true);
			box=(CWnd *)GetDlgItem(IDC_PROTTHRUST_Y);
			box->EnableWindow(true);
			box=(CWnd *)GetDlgItem(IDC_PROTTHRUST_Z);
			box->EnableWindow(true);
			box=(CWnd *)GetDlgItem(IDC_PFULL_THRUST);
			box->EnableWindow(true);
			box=(CWnd *)GetDlgItem(IDC_PFULL_ROTTHRUST);
			box->EnableWindow(true);
			box=(CWnd *)GetDlgItem(IDC_PTURNROLL);
			box->EnableWindow(true);
			if(m_bTurnroll)
			{
				box=(CWnd *)GetDlgItem(IDC_PTURNROLLANGLE);
				box->EnableWindow(true);
				box=(CWnd *)GetDlgItem(IDC_PMAX_TURNROLL_RATE);
				box->EnableWindow(true);
				box=(CWnd *)GetDlgItem(IDC_PTURNROLL_RATIO);
				box->EnableWindow(true);
			}
			break;
		}
	case IDC_PGRAVITY: break;
	case IDC_PMAGNETISM: break;
	case IDC_PWIND: break;
	case IDC_PUNLIMBOUNCE:
		{
			if(m_bUnlimitedBounce)
			{
				box=(CWnd *)GetDlgItem(IDC_PNUM_BOUNCES);
				box->EnableWindow(false);
			}
			else
			{
				box=(CWnd *)GetDlgItem(IDC_PNUM_BOUNCES);
				box->EnableWindow(true);
			}
			break;
		}


	}
}
void CPropertyPhysicsDlg::Disable(UINT control) 
{
	CWnd *box;
	switch(control)
	{
	case IDC_PTURNROLL: 
		{
			box=(CWnd *)GetDlgItem(IDC_PTURNROLLANGLE);
			box->EnableWindow(false);
			box=(CWnd *)GetDlgItem(IDC_PMAX_TURNROLL_RATE);
			box->EnableWindow(false);
			box=(CWnd *)GetDlgItem(IDC_PTURNROLL_RATIO);
			box->EnableWindow(false);
			break;
		}
	case IDC_PLEVELLING: break;
	case IDC_PBOUNCE: 
		{
			box=(CWnd *)GetDlgItem(IDC_PUNLIMBOUNCE);
			box->EnableWindow(false);
			box=(CWnd *)GetDlgItem(IDC_PNUM_BOUNCES);
			box->EnableWindow(false);
			box=(CWnd *)GetDlgItem(IDC_PSTICKS);
			box->EnableWindow(true);
			break;
		}
	case IDC_PWIGGLE:
		{
			box=(CWnd *)GetDlgItem(IDC_PWIGGLE_AMPLITUDE);
			box->EnableWindow(false);
			box=(CWnd *)GetDlgItem(IDC_PWIGGLES_PER_SECOND);
			box->EnableWindow(false);
			break;
		}
	case IDC_PSTICKS:
		{
			box=(CWnd *)GetDlgItem(IDC_PBOUNCE);
			box->EnableWindow(true);
			if(m_bBounce)
			{
				box=(CWnd *)GetDlgItem(IDC_PNUM_BOUNCES);
				box->EnableWindow(true);
			}
			else
			{
				box=(CWnd *)GetDlgItem(IDC_PNUM_BOUNCES);
				box->EnableWindow(false);
			}
			break;
		}
	case IDC_PPERSISTENT: break;
	case IDC_PUSESTHRUST:
		{
			box=(CWnd *)GetDlgItem(IDC_PTHRUST_X);
			box->EnableWindow(false);
			box=(CWnd *)GetDlgItem(IDC_PTHRUST_Y);
			box->EnableWindow(false);
			box=(CWnd *)GetDlgItem(IDC_PTHRUST_Z);
			box->EnableWindow(false);
			box=(CWnd *)GetDlgItem(IDC_PROTTHRUST_X);
			box->EnableWindow(false);
			box=(CWnd *)GetDlgItem(IDC_PROTTHRUST_Y);
			box->EnableWindow(false);
			box=(CWnd *)GetDlgItem(IDC_PROTTHRUST_Z);
			box->EnableWindow(false);
			box=(CWnd *)GetDlgItem(IDC_PFULL_THRUST);
			box->EnableWindow(false);
			box=(CWnd *)GetDlgItem(IDC_PFULL_ROTTHRUST);
			box->EnableWindow(false);
			box=(CWnd *)GetDlgItem(IDC_PTURNROLL);
			box->EnableWindow(false);
			box=(CWnd *)GetDlgItem(IDC_PTURNROLLANGLE);
			box->EnableWindow(false);
			box=(CWnd *)GetDlgItem(IDC_PMAX_TURNROLL_RATE);
			box->EnableWindow(false);
			box=(CWnd *)GetDlgItem(IDC_PTURNROLL_RATIO);
			box->EnableWindow(false);
			break;
		}
	case IDC_PGRAVITY: break;
	case IDC_PMAGNETISM: break;
	case IDC_PWIND: break;
	case IDC_PUNLIMBOUNCE:
		{
			box=(CWnd *)GetDlgItem(IDC_PNUM_BOUNCES);
			box->EnableWindow(true);
			break;
		}

	}
}

void CPropertyPhysicsDlg::UpdateCheckBoxes() 
{
	if(m_bTurnroll) Enable(IDC_PTURNROLL); else Disable(IDC_PTURNROLL);
	if(m_bLevelling) Enable(IDC_PLEVELLING); else Disable(IDC_PLEVELLING);
	if(m_bBounce) Enable(IDC_PBOUNCE); else Disable(IDC_PBOUNCE);
	if(m_bWiggle) Enable(IDC_PWIGGLE); else Disable(IDC_PWIGGLE);
	if(m_bSticks) Enable(IDC_PSTICKS); else Disable(IDC_PSTICKS);
	if(m_bPersistent) Enable(IDC_PPERSISTENT); else Disable(IDC_PPERSISTENT);
	if(m_bUsesThrust) Enable(IDC_PUSESTHRUST); else Disable(IDC_PUSESTHRUST);
	if(m_bGravity) Enable(IDC_PGRAVITY); else Disable(IDC_PGRAVITY);
	if(m_bMagnetism) Enable(IDC_PMAGNETISM); else Disable(IDC_PMAGNETISM);
	if(m_bWind) Enable(IDC_PWIND); else Disable(IDC_PWIND);
	if(m_bUnlimitedBounce) Enable(IDC_PUNLIMBOUNCE); else Disable(IDC_PUNLIMBOUNCE);
}

void CPropertyPhysicsDlg::OnPunlimbounce() 
{
	// TODO: Add your control notification handler code here
	if(m_bUnlimitedBounce)
	{
		m_bUnlimitedBounce=false;
		Disable(IDC_PUNLIMBOUNCE);
	}
	else
	{
		m_bUnlimitedBounce=true;
		Enable(IDC_PUNLIMBOUNCE);
	}	
}
