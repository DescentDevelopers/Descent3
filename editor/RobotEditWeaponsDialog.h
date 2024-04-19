#if !defined(AFX_ROBOTEDITWEAPONSDIALOG_H__2878E6E1_1A46_11D1_97A6_004095AA4FAE__INCLUDED_)
#define AFX_ROBOTEDITWEAPONSDIALOG_H__2878E6E1_1A46_11D1_97A6_004095AA4FAE__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// RobotEditWeaponsDialog.h : header file
//

#include "object.h"
#include "polymodel.h"
#include "objinfo.h"
#include "robotfirestruct.h"

/////////////////////////////////////////////////////////////////////////////
// RobotEditWeaponsDialog dialog

class RobotEditWeaponsDialog : public CDialog
{
private:
	void NewWBSelected(void);
	
// Construction
public:
	RobotEditWeaponsDialog(otype_wb_info static_wb[], poly_model *pm, CWnd* pParent = NULL);   // standard constructor
	void GetData(void);

// Dialog Data
	//{{AFX_DATA(RobotEditWeaponsDialog)
	enum { IDD = IDD_ROBOT_WEAPON_DIALOG };
	CString	m_current_gb;
	float	m_mask_latency_0;
	float	m_mask_latency_1;
	float	m_mask_latency_2;
	float	m_mask_latency_3;
	float	m_mask_latency_4;
	float	m_mask_latency_5;
	float	m_mask_latency_6;
	float	m_mask_latency_7;
	UINT	m_num_masks;
	float	m_turret_speed;
	CString	m_total_gb_text;
	CString	m_total_gp_text;
	CString	m_num_turrets_text;
	BOOL	m_gp77;
	BOOL	m_gp76;
	BOOL	m_gp75;
	BOOL	m_gp74;
	BOOL	m_gp73;
	BOOL	m_gp72;
	BOOL	m_gp71;
	BOOL	m_gp70;
	BOOL	m_gp67;
	BOOL	m_gp66;
	BOOL	m_gp65;
	BOOL	m_gp64;
	BOOL	m_gp63;
	BOOL	m_gp62;
	BOOL	m_gp61;
	BOOL	m_gp60;
	BOOL	m_gp56;
	BOOL	m_gp57;
	BOOL	m_gp55;
	BOOL	m_gp54;
	BOOL	m_gp53;
	BOOL	m_gp52;
	BOOL	m_gp51;
	BOOL	m_gp50;
	BOOL	m_gp47;
	BOOL	m_gp46;
	BOOL	m_gp45;
	BOOL	m_gp44;
	BOOL	m_gp43;
	BOOL	m_gp42;
	BOOL	m_gp41;
	BOOL	m_gp40;
	BOOL	m_gp37;
	BOOL	m_gp36;
	BOOL	m_gp35;
	BOOL	m_gp34;
	BOOL	m_gp33;
	BOOL	m_gp32;
	BOOL	m_gp31;
	BOOL	m_gp30;
	BOOL	m_gp27;
	BOOL	m_gp26;
	BOOL	m_gp25;
	BOOL	m_gp24;
	BOOL	m_gp23;
	BOOL	m_gp22;
	BOOL	m_gp21;
	BOOL	m_gp20;
	BOOL	m_gp17;
	BOOL	m_gp16;
	BOOL	m_gp15;
	BOOL	m_gp14;
	BOOL	m_gp13;
	BOOL	m_gp12;
	BOOL	m_gp11;
	BOOL	m_gp10;
	BOOL	m_gp07;
	BOOL	m_gp06;
	BOOL	m_gp05;
	BOOL	m_gp04;
	BOOL	m_gp03;
	BOOL	m_gp02;
	BOOL	m_gp01;
	BOOL	m_gp00;
	float	m_reaction_time;
	CString	m_turret_reaction_time_text;
	CString	m_turret_fov_text;
	CString	m_turret_spr_text;
	CString	m_current_turret_combo;
	CString	m_turret_sobj_text;
	float	m_view_cone_angle;
	float	m_max_distance;
	float	m_xz_plane_angle;
	CString	m_weapon_0_text;
	CString	m_weapon_1_text;
	CString	m_weapon_2_text;
	CString	m_weapon_4_text;
	CString	m_weapon_3_text;
	CString	m_weapon_5_text;
	CString	m_weapon_6_text;
	CString	m_weapon_7_text;
	CString	m_fire_sound_0_text;
	CString	m_fire_sound_1_text;
	CString	m_fire_sound_2_text;
	CString	m_fire_sound_3_text;
	CString	m_fire_sound_4_text;
	CString	m_fire_sound_5_text;
	CString	m_fire_sound_6_text;
	CString	m_fire_sound_7_text;
	float	m_energy_usage;
	float	m_ammo_usage;
	BOOL	m_spray_check;
	float	m_fire0_cycle_time;
	float	m_fire1_cycle_time;
	float	m_fire2_cycle_time;
	float	m_fire3_cycle_time;
	float	m_fire4_cycle_time;
	float	m_fire5_cycle_time;
	float	m_fire6_cycle_time;
	float	m_fire7_cycle_time;
	float	m_fire0_end_tick;
	float	m_fire1_end_tick;
	float	m_fire2_end_tick;
	float	m_fire3_end_tick;
	float	m_fire4_end_tick;
	float	m_fire5_end_tick;
	float	m_fire6_end_tick;
	float	m_fire7_end_tick;
	float	m_fire0_fire_tick;
	float	m_fire1_fire_tick;
	float	m_fire2_fire_tick;
	float	m_fire3_fire_tick;
	float	m_fire4_fire_tick;
	float	m_fire5_fire_tick;
	float	m_fire6_fire_tick;
	float	m_fire7_fire_tick;
	float	m_fire0_start_tick;
	float	m_fire1_start_tick;
	float	m_fire2_start_tick;
	float	m_fire3_start_tick;
	float	m_fire4_start_tick;
	float	m_fire5_start_tick;
	float	m_fire6_start_tick;
	float	m_fire7_start_tick;
	BOOL	m_random_check;
	BOOL	m_guided;
	BOOL	m_gpq0;
	BOOL	m_gpq1;
	BOOL	m_gpq2;
	BOOL	m_gpq3;
	BOOL	m_gpq4;
	BOOL	m_gpq5;
	BOOL	m_gpq6;
	BOOL	m_gpq7;
	BOOL	m_custom_view_cone_check;
	BOOL	m_onoff_check;
	BOOL	m_custom_max_dist;
	BOOL	m_user_timeout;
	BOOL	m_force_fvec_check;
	BOOL	m_aim_fvec_check;
	BOOL	m_force_target_check;
	//}}AFX_DATA

	int m_current_wb_index;
	int m_current_turret_index;
	int m_current_aiming_point;

	int m_anim_type;
	
	otype_wb_info *m_obj_static_wb;
	poly_model *m_obj_pm;

	// Copy all necessary data
	void GetWBData(object_info *obj_info);
	void UpdateDialog();
	void UpdateTurretInfo();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(RobotEditWeaponsDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	
	// Generated message map functions
	//{{AFX_MSG(RobotEditWeaponsDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnKillfocusGbNumMasksEdit();
	afx_msg void OnSelendokCurretTurretCombo();
	afx_msg void OnSelendokCurrentGbCombo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ROBOTEDITWEAPONSDIALOG_H__2878E6E1_1A46_11D1_97A6_004095AA4FAE__INCLUDED_)
