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

// PhysicsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "editor.h"
#include "PhysicsDlg.h"
#include "mono.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPhysicsDlg dialog

// Stuff for Copy/Paste
physics_info Paste_data;
bool Paste_data_used = 0;

void CPhysicsDlg::UpdateTerminalText(bool f_refresh) {
  char text[200];

  if (f_refresh) {
    UpdateData(true);
  }

  if (m_drag > 0.0) {
    sprintf(text, "Terminal Velocity(thrust/drag): %f", m_max_thrust / m_drag);
  } else {
    sprintf(text, "Terminal Velocity(thrust/drag): 0.0");
  }

  m_terminal_velocity_text = text;

  if (m_rot_drag > 0.0) {
    sprintf(text, "Terminal Rot. Vel.(rot. thrust/ rot. drag): %f", m_rot_thrust / m_rot_drag);
  } else {
    sprintf(text, "Terminal Rot. Vel.(rot. thrust/ rot. drag): 0.0");
  }

  m_terminal_rot_vel_text = text;

  if (f_refresh) {
    UpdateData(false);
  }
}

CPhysicsDlg::CPhysicsDlg(physics_info *phys_info, CWnd *pParent /*=NULL*/) : CDialog(CPhysicsDlg::IDD, pParent) {
  //{{AFX_DATA_INIT(CPhysicsDlg)
  m_bouncy_check = FALSE;
  m_fixed_rotate_check = FALSE;
  m_fixed_velocity_check = FALSE;
  m_init_rot_velocity_x = 0.0f;
  m_init_rot_velocity_y = 0.0f;
  m_init_rot_velocity_z = 0.0f;
  m_init_velocity = 0.0f;
  m_max_bounces = 0;
  m_percent_loss = 0.0f;
  m_sticky_check = FALSE;
  m_thrusts_check = FALSE;
  m_turn_roll_check = FALSE;
  m_auto_leveling = FALSE;
  m_no_collide_check = FALSE;
  m_terminal_rot_vel_text = _T("");
  m_terminal_velocity_text = _T("");
  m_ignore_concussive_forces_check = FALSE;
  m_hit_die_angle = 0.0f;
  m_persistent = FALSE;
  m_l_b = FALSE;
  m_l_h = FALSE;
  m_l_p = FALSE;
  m_l_x = FALSE;
  m_l_y = FALSE;
  m_l_z = FALSE;
  m_big_sphere = FALSE;
  m_ignore_same = FALSE;
  m_ignore_doors = FALSE;
  //}}AFX_DATA_INIT

  // Store the pointer to the stucture we're editing
  m_phys_info = phys_info;

  SetPhysicsData(phys_info);

  UpdateTerminalText(false);
}

void CPhysicsDlg::DoDataExchange(CDataExchange *pDX) {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CPhysicsDlg)
  DDX_Text(pDX, IDC_PHYSICS_MASS_EDIT, m_mass);
  DDV_MinMaxFloat(pDX, m_mass, 1.e-007f, 1.e+009f);
  DDX_Text(pDX, IDC_PHYSICS_DRAG_EDIT, m_drag);
  DDV_MinMaxFloat(pDX, m_drag, 1.e-010f, 9.e+006f);
  DDX_Check(pDX, IDC_PHYSICS_MAGNETISM_CHECK, m_magnetism);
  DDX_Text(pDX, IDC_PHYSICS_MAXTHRUST_EDIT, m_max_thrust);
  DDX_Text(pDX, IDC_PHYSICS_MAXTURNROLLRATE_EDIT, m_max_turnroll_rate);
  DDV_MinMaxFloat(pDX, m_max_turnroll_rate, 0.0f, 32000.0f);
  DDX_Text(pDX, IDC_PHYSICS_ROTDRAG_EDIT, m_rot_drag);
  DDV_MinMaxFloat(pDX, m_rot_drag, 0.0f, 1.e+009f);
  DDX_Text(pDX, IDC_PHYSICS_ROTTHRUST_EDIT, m_rot_thrust);
  DDX_Text(pDX, IDC_PHYSICS_TURNROLLRATIO_EDIT, m_turnroll_ratio);
  DDV_MinMaxFloat(pDX, m_turnroll_ratio, 0.0f, 1.0f);
  DDX_Check(pDX, IDC_PHYSICS_WIND_CHECK, m_wind);
  DDX_Text(pDX, IDC_PHYSICS_WIGGLEFREQ_EDIT, m_wiggle_frequency);
  DDX_Text(pDX, IDC_PHYSICS_WIGGLESIZE_EDIT, m_wiggle_size);
  DDV_MinMaxFloat(pDX, m_wiggle_size, 0.0f, 999.0f);
  DDX_Check(pDX, IDC_PHYSICS_WIGGLE_CHECK, m_wiggle);
  DDX_Check(pDX, IDC_PHYSICS_BOUNCY_CHECK, m_bouncy_check);
  DDX_Check(pDX, IDC_PHYSICS_FIXED_ROTATE_CHECK, m_fixed_rotate_check);
  DDX_Check(pDX, IDC_PHYSICS_FIXED_VELOCITY_CHECK, m_fixed_velocity_check);
  DDX_Text(pDX, IDC_PHYSICS_INIT_ROT_VELOCITY_X_EDIT, m_init_rot_velocity_x);
  DDX_Text(pDX, IDC_PHYSICS_INIT_ROT_VELOCITY_Y_EDIT, m_init_rot_velocity_y);
  DDX_Text(pDX, IDC_PHYSICS_INIT_ROT_VELOCITY_Z_EDIT, m_init_rot_velocity_z);
  DDX_Text(pDX, IDC_PHYSICS_INIT_VELOCITY_EDIT, m_init_velocity);
  DDX_Text(pDX, IDC_PHYSICS_MAX_BOUNCES_EDIT, m_max_bounces);
  DDV_MinMaxInt(pDX, m_max_bounces, -1, 32000);
  DDX_Text(pDX, IDC_PHYSICS_PERCENT_LOSS_EDIT, m_percent_loss);
  DDX_Check(pDX, IDC_PHYSICS_STICKY_CHECK, m_sticky_check);
  DDX_Check(pDX, IDC_PHYSICS_THRUSTS_CHECK, m_thrusts_check);
  DDX_Check(pDX, IDC_TURN_ROLL_CHECK, m_turn_roll_check);
  DDX_Check(pDX, IDC_PHYSICS_AUTO_LEVELING_CHECK, m_auto_leveling);
  DDX_Check(pDX, IDC_NO_COLLIDE_CHECK, m_no_collide_check);
  DDX_Check(pDX, IDC_PHYSICS_POINT_WALL_CHECK, m_point_collide_walls);
  DDX_Check(pDX, IDC_PHYSICS_IGNORE_ROBOTS_CHECK, m_ignore_robot_collisions);
  DDX_Text(pDX, IDC_PHYSICS_TERMINAL_ROT_VELOCITY_STATIC, m_terminal_rot_vel_text);
  DDX_Text(pDX, IDC_PHYSICS_TERMINAL_VELOCITY_STATIC, m_terminal_velocity_text);
  DDX_Check(pDX, IDC_PHYSICS_IGNORE_CONCUSSIVE_FORCES, m_ignore_concussive_forces_check);
  DDX_Text(pDX, IDC_PHYSICS_HIT_DIE_ANGLE, m_hit_die_angle);
  DDV_MinMaxFloat(pDX, m_hit_die_angle, 0.f, 90.f);
  DDX_Check(pDX, IDC_PHYSICS_PERSISTENT_CHECK, m_persistent);
  DDX_Check(pDX, IDC_L_B_CHECK, m_l_b);
  DDX_Check(pDX, IDC_L_H_CHECK, m_l_h);
  DDX_Check(pDX, IDC_L_P_CHECK, m_l_p);
  DDX_Check(pDX, IDC_L_X_CHECK, m_l_x);
  DDX_Check(pDX, IDC_L_Y_CHECK, m_l_y);
  DDX_Check(pDX, IDC_L_Z_CHECK, m_l_z);
  DDX_Check(pDX, IDC_NO_BIG_SPHERE_CHECK, m_big_sphere);
  DDX_Check(pDX, IDC_PHYSICS_IGNORE_SAME_CHECK, m_ignore_same);
  DDX_Check(pDX, IDC_PHYSICS_IGNORE_DOOR_CHECK, m_ignore_doors);
  //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPhysicsDlg, CDialog)
//{{AFX_MSG_MAP(CPhysicsDlg)
ON_BN_CLICKED(IDC_PHYSICS_WIGGLE_CHECK, OnPhysicsWiggleCheck)
ON_BN_CLICKED(IDC_PHYSICS_GRAVITY_RADIO, OnPhysicsGravityRadio)
ON_BN_CLICKED(IDC_PHYSICS_NO_GRAVITY_RADIO, OnPhysicsNoGravityRadio)
ON_BN_CLICKED(IDC_PHYSICS_REVERSE_GRAVITY_RADIO, OnPhysicsReverseGravityRadio)
ON_BN_CLICKED(IDC_TURN_ROLL_CHECK, OnTurnRollCheck)
ON_BN_CLICKED(IDC_PHYSICS_BOUNCY_CHECK, OnPhysicsBouncyCheck)
ON_BN_CLICKED(IDC_PHYSICS_THRUSTS_CHECK, OnPhysicsThrustsCheck)
ON_EN_KILLFOCUS(IDC_PHYSICS_DRAG_EDIT, OnKillfocusPhysicsDragEdit)
ON_EN_KILLFOCUS(IDC_PHYSICS_ROTDRAG_EDIT, OnKillfocusPhysicsRotdragEdit)
ON_EN_KILLFOCUS(IDC_PHYSICS_MAXTHRUST_EDIT, OnKillfocusPhysicsMaxthrustEdit)
ON_EN_KILLFOCUS(IDC_PHYSICS_ROTTHRUST_EDIT, OnKillfocusPhysicsRotthrustEdit)
ON_BN_CLICKED(IDC_PHYSICS_COPY, OnPhysicsCopy)
ON_BN_CLICKED(IDC_PHYSICS_PASTE, OnPhysicsPaste)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPhysicsDlg message handlers

void CPhysicsDlg::EnableDisableFullPhysics() {
  int enabled = 1; //((CButton *)GetDlgItem(IDC_PHYSICS_FULL_PHYSICS))->GetCheck();

  ((CButton *)GetDlgItem(IDC_PHYSICS_DRAG_EDIT))->EnableWindow(enabled);
  ((CButton *)GetDlgItem(IDC_PHYSICS_ROTDRAG_EDIT))->EnableWindow(enabled);
  ((CButton *)GetDlgItem(IDC_PHYSICS_MAXTHRUST_EDIT))->EnableWindow(enabled);
  ((CButton *)GetDlgItem(IDC_PHYSICS_ROTTHRUST_EDIT))->EnableWindow(enabled);
  ((CButton *)GetDlgItem(IDC_PHYSICS_MAXTURNROLLRATE_EDIT))->EnableWindow(enabled);
  ((CButton *)GetDlgItem(IDC_PHYSICS_TURNROLLRATIO_EDIT))->EnableWindow(enabled);

  ((CButton *)GetDlgItem(IDC_PHYSICS_DRAG_TEXT))->EnableWindow(enabled);
  ((CButton *)GetDlgItem(IDC_PHYSICS_ROTDRAG_TEXT))->EnableWindow(enabled);
  ((CButton *)GetDlgItem(IDC_PHYSICS_MAXTHRUST_TEXT))->EnableWindow(enabled);
  ((CButton *)GetDlgItem(IDC_PHYSICS_ROTTHRUST_TEXT))->EnableWindow(enabled);
  ((CButton *)GetDlgItem(IDC_PHYSICS_MAXTURNROLLRATE_TEXT))->EnableWindow(enabled);
  ((CButton *)GetDlgItem(IDC_PHYSICS_TURNROLLRATIO_TEXT))->EnableWindow(enabled);
}

void CPhysicsDlg::EnableDisableWiggle() {
  int enabled = ((CButton *)GetDlgItem(IDC_PHYSICS_WIGGLE_CHECK))->GetCheck();

  ((CButton *)GetDlgItem(IDC_PHYSICS_WIGGLESIZE_EDIT))->EnableWindow(enabled);
  ((CButton *)GetDlgItem(IDC_PHYSICS_WIGGLEFREQ_EDIT))->EnableWindow(enabled);

  ((CButton *)GetDlgItem(IDC_PHYSICS_WIGGLESIZE_TEXT))->EnableWindow(enabled);
  ((CButton *)GetDlgItem(IDC_PHYSICS_WIGGLEFREQ_TEXT))->EnableWindow(enabled);
}

void CPhysicsDlg::EnableDisableBounce() {
  int enabled = ((CButton *)GetDlgItem(IDC_PHYSICS_BOUNCY_CHECK))->GetCheck();

  ((CButton *)GetDlgItem(IDC_PHYSICS_MAX_BOUNCES_EDIT))->EnableWindow(enabled);
  ((CButton *)GetDlgItem(IDC_PHYSICS_PERCENT_LOSS_EDIT))->EnableWindow(enabled);

  ((CButton *)GetDlgItem(IDC_MAX_BOUNCES_TEXT))->EnableWindow(enabled);
  ((CButton *)GetDlgItem(IDC_PERCENT_LOSS_TEXT))->EnableWindow(enabled);
}

void CPhysicsDlg::EnableDisableTurnRoll() {
  int enabled = ((CButton *)GetDlgItem(IDC_TURN_ROLL_CHECK))->GetCheck();

  ((CButton *)GetDlgItem(IDC_PHYSICS_MAXTURNROLLRATE_EDIT))->EnableWindow(enabled);
  ((CButton *)GetDlgItem(IDC_PHYSICS_TURNROLLRATIO_EDIT))->EnableWindow(enabled);

  ((CButton *)GetDlgItem(IDC_PHYSICS_MAXTURNROLLRATE_TEXT))->EnableWindow(enabled);
  ((CButton *)GetDlgItem(IDC_PHYSICS_TURNROLLRATIO_TEXT))->EnableWindow(enabled);
}

void CPhysicsDlg::EnableDisableThrust() {
  int enabled = ((CButton *)GetDlgItem(IDC_PHYSICS_THRUSTS_CHECK))->GetCheck();

  ((CButton *)GetDlgItem(IDC_PHYSICS_MAXTHRUST_EDIT))->EnableWindow(enabled);
  ((CButton *)GetDlgItem(IDC_PHYSICS_ROTTHRUST_EDIT))->EnableWindow(enabled);

  ((CButton *)GetDlgItem(IDC_PHYSICS_MAXTHRUST_TEXT))->EnableWindow(enabled);
  ((CButton *)GetDlgItem(IDC_PHYSICS_ROTTHRUST_TEXT))->EnableWindow(enabled);
}

void CPhysicsDlg::OnPhysicsWiggleCheck() { EnableDisableWiggle(); }

BOOL CPhysicsDlg::OnInitDialog() {
  CDialog::OnInitDialog();
  CButton *bbox;

  EnableDisableFullPhysics();
  EnableDisableWiggle();
  EnableDisableBounce();
  EnableDisableTurnRoll();
  EnableDisableThrust();

  // Turn paste off if no paste data
  ((CButton *)GetDlgItem(IDC_PHYSICS_PASTE))->EnableWindow(Paste_data_used);

  switch (m_gravity_flag) {
  case PF_GRAVITY:
    bbox = (CButton *)GetDlgItem(IDC_PHYSICS_GRAVITY_RADIO);
    bbox->SetCheck(true);
    break;
  case PF_REVERSE_GRAVITY:
    bbox = (CButton *)GetDlgItem(IDC_PHYSICS_REVERSE_GRAVITY_RADIO);
    bbox->SetCheck(true);
    break;
  case 0:
    bbox = (CButton *)GetDlgItem(IDC_PHYSICS_NO_GRAVITY_RADIO);
    bbox->SetCheck(true);
    break;
  default:
    mprintf(0, "ERROR: Invalid gravity flags cannot have PF_GRAVITY and PF_REVERSE_GRAVITY\n");
    //		Int3();
    break;
  }

  // Disable hit die dot if it has -1
  ((CButton *)GetDlgItem(IDC_PHYSICS_HIT_DIE_ANGLE))->EnableWindow(m_phys_info->hit_die_dot != -1);

  return TRUE; // return TRUE unless you set the focus to a control
               // EXCEPTION: OCX Property Pages should return FALSE
}

// Copy data from physics_info struct to CPhysicsDlg class
void CPhysicsDlg::SetPhysicsData(physics_info *phys_info) {
  // Copy out the data
  m_mass = phys_info->mass;
  m_drag = phys_info->drag;
  m_rot_drag = phys_info->rotdrag;
  m_max_thrust = phys_info->full_thrust;
  m_rot_thrust = phys_info->full_rotthrust;
  m_max_turnroll_rate = phys_info->max_turnroll_rate;
  m_turnroll_ratio = phys_info->turnroll_ratio;

  m_wiggle_size = phys_info->wiggle_amplitude;
  m_wiggle_frequency = phys_info->wiggles_per_sec;

  m_sticky_check = (phys_info->flags & PF_STICK) != 0;
  m_bouncy_check = (phys_info->flags & PF_BOUNCE) != 0;
  m_thrusts_check = (phys_info->flags & PF_USES_THRUST) != 0;
  m_fixed_velocity_check = (phys_info->flags & PF_FIXED_VELOCITY) != 0;
  m_fixed_rotate_check = (phys_info->flags & PF_FIXED_ROT_VELOCITY) != 0;
  m_no_collide_check = (phys_info->flags & PF_NO_COLLIDE) != 0;
  m_turn_roll_check = (phys_info->flags & PF_TURNROLL) != 0;
  m_auto_leveling = (phys_info->flags & PF_LEVELING) != 0;
  m_point_collide_walls = (phys_info->flags & PF_POINT_COLLIDE_WALLS) != 0;
  m_ignore_robot_collisions = (phys_info->flags & PF_NO_ROBOT_COLLISIONS) != 0;
  m_ignore_same = (phys_info->flags & PF_NO_SAME_COLLISIONS) != 0;
  m_ignore_doors = (phys_info->flags & PF_NO_DOOR_COLLISIONS) != 0;
  m_ignore_concussive_forces_check = (phys_info->flags & PF_IGNORE_CONCUSSIVE_FORCES) != 0;
  m_l_x = (phys_info->flags & PF_LOCK_X) != 0;
  m_l_y = (phys_info->flags & PF_LOCK_Y) != 0;
  m_l_z = (phys_info->flags & PF_LOCK_Z) != 0;
  m_l_p = (phys_info->flags & PF_LOCK_P) != 0;
  m_l_b = (phys_info->flags & PF_LOCK_B) != 0;
  m_l_h = (phys_info->flags & PF_LOCK_H) != 0;

  m_big_sphere = (phys_info->flags & PF_NEVER_USE_BIG_SPHERE) != 0;

  m_init_velocity = phys_info->velocity.z();
  m_init_rot_velocity_x = phys_info->rotvel.x();
  m_init_rot_velocity_y = phys_info->rotvel.y();
  m_init_rot_velocity_z = phys_info->rotvel.z();

  m_gravity_flag = phys_info->flags & (PF_GRAVITY | PF_REVERSE_GRAVITY);

  m_max_bounces = phys_info->num_bounces;
  m_percent_loss = 100.0 - (phys_info->coeff_restitution * 100.0);

  m_wiggle = (phys_info->flags & PF_WIGGLE) != 0;
  m_magnetism = (phys_info->flags & PF_IGNORE_OWN_CONC_FORCES) != 0;
  m_wind = (phys_info->flags & PF_WIND) != 0;
  //	m_gravity = (phys_info->flags & PF_GRAVITY) != 0;
  m_persistent = (phys_info->flags & PF_PERSISTENT) != 0;

  m_hit_die_angle = (phys_info->hit_die_dot == -1) ? 0 : asin(phys_info->hit_die_dot) * (180.0 / PI);
}

// Copy data from CPhysicsDlg class to physics_info struct
void CPhysicsDlg::GetPhysicsData(physics_info *phys_info) {
  phys_info->mass = m_mass;
  phys_info->drag = m_drag;
  phys_info->rotdrag = m_rot_drag;
  phys_info->full_thrust = m_max_thrust;
  phys_info->full_rotthrust = m_rot_thrust;
  phys_info->max_turnroll_rate = m_max_turnroll_rate;
  phys_info->turnroll_ratio = m_turnroll_ratio;

  phys_info->wiggle_amplitude = m_wiggle_size;
  phys_info->wiggles_per_sec = m_wiggle_frequency;

  if (m_wiggle)
    phys_info->flags |= PF_WIGGLE;
  else
    phys_info->flags &= ~PF_WIGGLE;

  if (m_persistent)
    phys_info->flags |= PF_PERSISTENT;
  else
    phys_info->flags &= ~PF_PERSISTENT;

  if (m_magnetism)
    phys_info->flags |= PF_IGNORE_OWN_CONC_FORCES;
  else
    phys_info->flags &= ~PF_IGNORE_OWN_CONC_FORCES;

  if (m_wind)
    phys_info->flags |= PF_WIND;
  else
    phys_info->flags &= ~PF_WIND;

  //	if (m_gravity)
  //		phys_info->flags |= PF_GRAVITY;
  //	else
  //		phys_info->flags &= ~PF_GRAVITY;

  if (m_sticky_check)
    phys_info->flags |= PF_STICK;
  else
    phys_info->flags &= ~PF_STICK;

  if (m_bouncy_check)
    phys_info->flags |= PF_BOUNCE;
  else
    phys_info->flags &= ~PF_BOUNCE;

  if (m_thrusts_check)
    phys_info->flags |= PF_USES_THRUST;
  else
    phys_info->flags &= ~PF_USES_THRUST;

  if (m_fixed_velocity_check)
    phys_info->flags |= PF_FIXED_VELOCITY;
  else
    phys_info->flags &= ~PF_FIXED_VELOCITY;

  if (m_fixed_rotate_check)
    phys_info->flags |= PF_FIXED_ROT_VELOCITY;
  else
    phys_info->flags &= ~PF_FIXED_ROT_VELOCITY;

  if (m_no_collide_check)
    phys_info->flags |= PF_NO_COLLIDE;
  else
    phys_info->flags &= ~PF_NO_COLLIDE;

  if (m_turn_roll_check)
    phys_info->flags |= PF_TURNROLL;
  else
    phys_info->flags &= ~PF_TURNROLL;

  if (m_auto_leveling)
    phys_info->flags |= PF_LEVELING;
  else
    phys_info->flags &= ~PF_LEVELING;

  if (m_point_collide_walls)
    phys_info->flags |= PF_POINT_COLLIDE_WALLS;
  else
    phys_info->flags &= ~PF_POINT_COLLIDE_WALLS;

  if (m_ignore_robot_collisions)
    phys_info->flags |= PF_NO_ROBOT_COLLISIONS;
  else
    phys_info->flags &= ~PF_NO_ROBOT_COLLISIONS;

  if (m_ignore_same)
    phys_info->flags |= PF_NO_SAME_COLLISIONS;
  else
    phys_info->flags &= ~PF_NO_SAME_COLLISIONS;

  if (m_ignore_doors)
    phys_info->flags |= PF_NO_DOOR_COLLISIONS;
  else
    phys_info->flags &= ~PF_NO_DOOR_COLLISIONS;

  if (m_ignore_concussive_forces_check)
    phys_info->flags |= PF_IGNORE_CONCUSSIVE_FORCES;
  else
    phys_info->flags &= ~PF_IGNORE_CONCUSSIVE_FORCES;

  if (m_l_x)
    phys_info->flags |= PF_LOCK_X;
  else
    phys_info->flags &= ~PF_LOCK_X;

  if (m_l_y)
    phys_info->flags |= PF_LOCK_Y;
  else
    phys_info->flags &= ~PF_LOCK_Y;

  if (m_l_z)
    phys_info->flags |= PF_LOCK_Z;
  else
    phys_info->flags &= ~PF_LOCK_Z;

  if (m_l_b)
    phys_info->flags |= PF_LOCK_B;
  else
    phys_info->flags &= ~PF_LOCK_B;

  if (m_l_p)
    phys_info->flags |= PF_LOCK_P;
  else
    phys_info->flags &= ~PF_LOCK_P;

  if (m_l_h)
    phys_info->flags |= PF_LOCK_H;
  else
    phys_info->flags &= ~PF_LOCK_H;

  if (m_big_sphere)
    phys_info->flags |= PF_NEVER_USE_BIG_SPHERE;
  else
    phys_info->flags &= ~PF_NEVER_USE_BIG_SPHERE;

  phys_info->velocity.z() = m_init_velocity;
  phys_info->rotvel.x() = m_init_rot_velocity_x;
  phys_info->rotvel.y() = m_init_rot_velocity_y;
  phys_info->rotvel.z() = m_init_rot_velocity_z;

  phys_info->num_bounces = m_max_bounces;
  phys_info->coeff_restitution = (100.0 - m_percent_loss) / 100.0;

  if (phys_info->hit_die_dot != -1)
    phys_info->hit_die_dot = sin(m_hit_die_angle * PI / (180.0));

  phys_info->flags &= (~PF_GRAVITY_MASK);
  phys_info->flags |= m_gravity_flag;
}

void CPhysicsDlg::OnPhysicsGravityRadio() { m_gravity_flag = PF_GRAVITY; }

void CPhysicsDlg::OnPhysicsNoGravityRadio() { m_gravity_flag = 0; }

void CPhysicsDlg::OnPhysicsReverseGravityRadio() { m_gravity_flag = PF_REVERSE_GRAVITY; }

void CPhysicsDlg::OnTurnRollCheck() { EnableDisableTurnRoll(); }

void CPhysicsDlg::OnPhysicsBouncyCheck() { EnableDisableBounce(); }

void CPhysicsDlg::OnPhysicsThrustsCheck() { EnableDisableThrust(); }

void CPhysicsDlg::OnOK() {
  CDialog::OnOK();

  // Copy the new data into our original struct
  GetPhysicsData(m_phys_info);
}

void CPhysicsDlg::OnKillfocusPhysicsDragEdit() { UpdateTerminalText(true); }

void CPhysicsDlg::OnKillfocusPhysicsRotdragEdit() { UpdateTerminalText(true); }

void CPhysicsDlg::OnKillfocusPhysicsMaxthrustEdit() { UpdateTerminalText(true); }

void CPhysicsDlg::OnKillfocusPhysicsRotthrustEdit() { UpdateTerminalText(true); }

void CPhysicsDlg::OnPhysicsCopy() {
  GetPhysicsData(&Paste_data);
  Paste_data_used = 1;
}

void CPhysicsDlg::OnPhysicsPaste() {
  if (Paste_data_used) {
    SetPhysicsData(&Paste_data);
    UpdateData(0);
  }
}
