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

#if !defined(AFX_PROPERTYPHYSICSDLG_H__412CCAE5_F849_11D0_8901_0020A90CB6F2__INCLUDED_)
#define AFX_PROPERTYPHYSICSDLG_H__412CCAE5_F849_11D0_8901_0020A90CB6F2__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PropertyPhysicsDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPropertyPhysicsDlg dialog

#ifndef _PROP_PHYSICS_DLG_H_
#define _PROP_PHYSICS_DLG_H_

#include "KeyPadDialog.h"

class CPropertyPhysicsDlg : public CKeypadDialog {
  // Construction
public:
  CPropertyPhysicsDlg(CWnd *pParent = NULL); // standard constructor
  void Enable(UINT control);
  void Disable(UINT control);
  void UpdateCheckBoxes();
  // Dialog Data
  //{{AFX_DATA(CPropertyPhysicsDlg)
  enum { IDD = IDD_PROPPHYSICS };
  BOOL m_bBounce;
  float m_fDrag;
  float m_fFullRotThrust;
  float m_fFullThrust;
  BOOL m_bGravity;
  BOOL m_bLevelling;
  BOOL m_bMagnetism;
  float m_fMass;
  float m_fMaxTurnrollrate;
  int m_iNumBounces;
  BOOL m_bPersistent;
  float m_fRotDrag;
  float m_fRotThrustX;
  float m_fRotThrustY;
  float m_fRotThrustZ;
  float m_fRotVelX;
  float m_fRotVelY;
  float m_fRotVelZ;
  BOOL m_bSticks;
  float m_fThrustX;
  float m_fThrustY;
  float m_fThrustZ;
  BOOL m_bTurnroll;
  float m_fTurnrollRatio;
  int16_t m_sTurnrollAngle;
  BOOL m_bUsesThrust;
  float m_fVelocityX;
  float m_fVelocityY;
  float m_fVelocityZ;
  BOOL m_bWiggle;
  float m_fWiggleAmplitude;
  float m_fWigglesPerSecond;
  BOOL m_bWind;
  BOOL m_bUnlimitedBounce;
  //}}AFX_DATA

  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CPropertyPhysicsDlg)
protected:
  virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
                                                   //}}AFX_VIRTUAL

  // Implementation
protected:
  // Generated message map functions
  //{{AFX_MSG(CPropertyPhysicsDlg)
  afx_msg void OnPturnroll();
  afx_msg void OnPlevelling();
  afx_msg void OnPbounce();
  afx_msg void OnPwiggle();
  afx_msg void OnPsticks();
  afx_msg void OnPpersistent();
  afx_msg void OnPusesthrust();
  afx_msg void OnPgravity();
  afx_msg void OnPmagnetism();
  afx_msg void OnPwind();
  virtual BOOL OnInitDialog();
  afx_msg void OnPunlimbounce();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTYPHYSICSDLG_H__412CCAE5_F849_11D0_8901_0020A90CB6F2__INCLUDED_)
