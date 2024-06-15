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

#if !defined(AFX_PHYSICSDLG_H__A0BC9221_0DA6_11D1_86CC_004033312F7C__INCLUDED_)
#define AFX_PHYSICSDLG_H__A0BC9221_0DA6_11D1_86CC_004033312F7C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PhysicsDlg.h : header file
//

#include "object.h"

/////////////////////////////////////////////////////////////////////////////
// CPhysicsDlg dialog

class CPhysicsDlg : public CDialog {
  // Construction
public:
  CPhysicsDlg(physics_info *phys_info, CWnd *pParent = NULL); // standard constructor

  // Dialog Data
  //{{AFX_DATA(CPhysicsDlg)
  enum { IDD = IDD_PHYSICS };
  float m_mass;
  float m_drag;
  BOOL m_magnetism;
  float m_max_thrust;
  float m_max_turnroll_rate;
  float m_rot_drag;
  float m_rot_thrust;
  float m_turnroll_ratio;
  BOOL m_wind;
  float m_wiggle_frequency;
  float m_wiggle_size;
  BOOL m_wiggle;
  BOOL m_bouncy_check;
  BOOL m_fixed_rotate_check;
  BOOL m_fixed_velocity_check;
  float m_init_rot_velocity_x;
  float m_init_rot_velocity_y;
  float m_init_rot_velocity_z;
  float m_init_velocity;
  int m_max_bounces;
  float m_percent_loss;
  BOOL m_sticky_check;
  BOOL m_thrusts_check;
  BOOL m_turn_roll_check;
  BOOL m_auto_leveling;
  BOOL m_no_collide_check;
  BOOL m_point_collide_walls;
  BOOL m_ignore_robot_collisions;
  CString m_terminal_rot_vel_text;
  CString m_terminal_velocity_text;
  BOOL m_ignore_concussive_forces_check;
  float m_hit_die_angle;
  BOOL m_persistent;
  BOOL m_l_b;
  BOOL m_l_h;
  BOOL m_l_p;
  BOOL m_l_x;
  BOOL m_l_y;
  BOOL m_l_z;
  BOOL m_big_sphere;
  BOOL m_ignore_same;
  BOOL m_ignore_doors;
  //}}AFX_DATA

  int m_gravity_flag;

  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CPhysicsDlg)
protected:
  virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
                                                   //}}AFX_VIRTUAL

  // Implementation
protected:
  void EnableDisableFullPhysics();
  void EnableDisableWiggle();
  void EnableDisableBounce();
  void EnableDisableTurnRoll();
  void EnableDisableThrust();
  void UpdateTerminalText(bool f_refresh);

  // The data we're editing
  physics_info *m_phys_info;

  // Copy data from class to physics_info struct
  void GetPhysicsData(physics_info *phys_info);

  // Copy data from physics_info struct to class
  void SetPhysicsData(physics_info *phys_info);

  // Generated message map functions
  //{{AFX_MSG(CPhysicsDlg)
  virtual BOOL OnInitDialog();
  afx_msg void OnPhysicsWiggleCheck();
  afx_msg void OnPhysicsGravityRadio();
  afx_msg void OnPhysicsNoGravityRadio();
  afx_msg void OnPhysicsReverseGravityRadio();
  afx_msg void OnTurnRollCheck();
  afx_msg void OnPhysicsBouncyCheck();
  afx_msg void OnPhysicsThrustsCheck();
  virtual void OnOK();
  afx_msg void OnKillfocusPhysicsDragEdit();
  afx_msg void OnKillfocusPhysicsRotdragEdit();
  afx_msg void OnKillfocusPhysicsMaxthrustEdit();
  afx_msg void OnKillfocusPhysicsRotthrustEdit();
  afx_msg void OnPhysicsCopy();
  afx_msg void OnPhysicsPaste();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PHYSICSDLG_H__A0BC9221_0DA6_11D1_86CC_004033312F7C__INCLUDED_)
