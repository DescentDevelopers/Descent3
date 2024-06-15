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

#if !defined(AFX_AISETTINGSDLG_H__E93B3161_0DB4_11D1_86CC_004033312F7C__INCLUDED_)
#define AFX_AISETTINGSDLG_H__E93B3161_0DB4_11D1_86CC_004033312F7C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// AISettingsDlg.h : header file
//

#include "objinfo.h"

/////////////////////////////////////////////////////////////////////////////
// CAISettingsDlg dialog

class CAISettingsDlg : public CDialog {
  // Construction
public:
  CAISettingsDlg(t_ai_info *ai_info, CWnd *pParent = NULL); // standard constructor

  // Dialog Data
  //{{AFX_DATA(CAISettingsDlg)
  enum { IDD = IDD_AI };
  CSliderCtrl m_bfhi_slider;
  CSliderCtrl m_tlv_slider;
  CSliderCtrl m_tla_slider;
  CSliderCtrl m_roaming_slider;
  CSliderCtrl m_nv_slider;
  CSliderCtrl m_lp_slider;
  CSliderCtrl m_hearing_slider;
  CSliderCtrl m_fv_slider;
  CSliderCtrl m_frustration_slider;
  CSliderCtrl m_fight_team_slider;
  CSliderCtrl m_fight_same_slider;
  CSliderCtrl m_curiousity_slider;
  CSliderCtrl m_agression_slider;
  CSliderCtrl m_fire_spread_slider;
  BOOL m_fire_weapon1;
  BOOL m_fire_weapon2;
  BOOL m_melee1;
  BOOL m_melee2;
  BOOL m_stay_inout;
  CString m_ai_class;
  float m_ai_flock_alignment;
  float m_ai_flock_cohesion;
  BOOL m_ai_flock_member_check;
  float m_ai_flock_seperation;
  float m_ai_fov;
  CString m_initial_action;
  BOOL m_ai_uses_thrust_check;
  CString m_ai_type;
  CString m_ai_movement_type;
  CString m_ai_movement_subtype;
  float m_ai_max_velocity;
  float m_ai_max_vel_time;
  float m_ai_max_turn_rate;
  BOOL m_dodge_check;
  BOOL m_fireattarget_check;
  BOOL m_flinch_check;
  BOOL m_notify_hitbyweapon_check;
  BOOL m_notify_nearwall_check;
  BOOL m_notify_objectfired_check;
  BOOL m_notify_seeplayer_check;
  BOOL m_notify_weaponhittarget_check;
  BOOL m_retarget_check;
  BOOL m_turretcontrol_check;
  BOOL m_tauntatdeath_check;
  BOOL m_avoidwalls_check;
  BOOL m_notify_hears_noise;
  BOOL m_persistant_check;
  BOOL m_fluctuate_speed_properties_check;
  float m_ai_max_turn_rate_time;
  CString m_angry_sound_combo;
  CString m_attack_sound_combo;
  float m_circle_distance;
  float m_attack_vel_percent;
  float m_dogde_percent;
  float m_dodge_vel_percent;
  CString m_flee_sound_combo;
  float m_flee_vel_percent;
  float m_melee1_latency;
  float m_melee1_distance;
  float m_melee2_distance;
  float m_melee2_latency;
  CString m_see_sound_combo;
  BOOL m_ordered_wb_firing_check;
  BOOL m_orient_to_vel;
  BOOL m_xz_dist;
  BOOL m_target_by_dist_check;
  CString m_turret_sound_combo;
  BOOL m_avoid_friends_check;
  float m_avoid_friends_dist;
  float m_bfhmax;
  float m_bfhmin;
  BOOL m_bfh_check;
  BOOL m_fov_uvec;
  BOOL m_fov_aim;
  //}}AFX_DATA

  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CAISettingsDlg)
protected:
  virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
                                                   //}}AFX_VIRTUAL

  // Implementation
protected:
  // The data we're editing
  t_ai_info *m_ai_info;

  // Generated message map functions
  //{{AFX_MSG(CAISettingsDlg)
  virtual BOOL OnInitDialog();
  afx_msg void OnSelchangeAiMovementTypeCombo();
  virtual void OnOK();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

#include "objinfo.h"

// Copy data from ai_info struct to CAISettingsDlg class
void CopyAIData(CAISettingsDlg *dlg, ai_frame *ai_info);

// Copy data from CAISettingsDlg class to ai_info struct
void CopyAIData(ai_frame *ai_info, CAISettingsDlg *dlg);

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AISETTINGSDLG_H__E93B3161_0DB4_11D1_86CC_004033312F7C__INCLUDED_)
