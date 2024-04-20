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

// AISettingsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "editor.h"
#include "AISettingsDlg.h"
#include "mono.h"
#include "soundload.h"
#include "pserror.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MAX_SLIDER_RANGE 100

/////////////////////////////////////////////////////////////////////////////
// CAISettingsDlg dialog


CAISettingsDlg::CAISettingsDlg(t_ai_info *ai_info,CWnd* pParent /*=NULL*/)
	: CDialog(CAISettingsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAISettingsDlg)
	m_ai_class = _T("");
	m_ai_flock_alignment = 0.0f;
	m_ai_flock_cohesion = 0.0f;
	m_ai_flock_member_check = FALSE;
	m_ai_flock_seperation = 0.0f;
	m_initial_action = _T("");
	m_ai_uses_thrust_check = FALSE;
	m_ai_type = _T("");
	m_ai_movement_type = _T("");
	m_ai_movement_subtype = _T("");
	m_fluctuate_speed_properties_check = FALSE;
	m_ai_max_turn_rate_time = 0.0f;
	m_angry_sound_combo = _T("");
	m_attack_sound_combo = _T("");
	m_circle_distance = 0.0f;
	m_attack_vel_percent = 0.0f;
	m_dogde_percent = 0.0f;
	m_dodge_vel_percent = 0.0f;
	m_flee_sound_combo = _T("");
	m_flee_vel_percent = 0.0f;
	m_melee1_latency = 0.0f;
	m_melee1_distance = 0.0f;
	m_melee2_distance = 0.0f;
	m_melee2_latency = 0.0f;
	m_see_sound_combo = _T("");
	m_ordered_wb_firing_check = FALSE;
	m_orient_to_vel = FALSE;
	m_xz_dist = FALSE;
	m_target_by_dist_check = FALSE;
	m_turret_sound_combo = _T("");
	m_avoid_friends_check = FALSE;
	m_avoid_friends_dist = 0.0f;
	m_bfhmax = 0.0f;
	m_bfhmin = 0.0f;
	m_bfh_check = FALSE;
	m_fov_uvec = FALSE;
	m_fov_aim = FALSE;
	//}}AFX_DATA_INIT

	//Store the pointer to the stucture we're editing
	m_ai_info = ai_info;

	//Copy out the data
	m_ai_class = Ai_class_strings[ai_info->ai_class];
	m_ai_type = Ai_type_strings[ai_info->ai_type];

	m_ai_movement_type = Ai_movement_type_strings[ai_info->movement_type];
	if(!stricmp(m_ai_movement_type,"Walking"))
	{
		m_ai_movement_subtype = Ai_movement_subtype_walking_strings[ai_info->movement_subtype];
	}
	else if (!stricmp(m_ai_movement_type,"Flying"))
	{
		m_ai_movement_subtype = Ai_movement_subtype_flying_strings[ai_info->movement_subtype];
	}
	else 
	{
		m_ai_movement_subtype = "";
	}

	m_ai_max_velocity = ai_info->max_velocity;
	
	if(ai_info->max_delta_velocity > 0.0)
	{
		m_ai_max_vel_time = ai_info->max_velocity / ai_info->max_delta_velocity;
	}
	else
	{
		m_ai_max_vel_time = 1.0f;
		m_ai_max_velocity = 0.0f;
	}

	m_ai_max_turn_rate = ai_info->max_turn_rate;

	m_ai_fov = acos(ai_info->fov)*(360.0/PI);
//	mprintf((0, "FOV = %f\n.", ai_info->fov));

	m_orient_to_vel = (ai_info->flags & AIF_ORIENT_TO_VEL) != 0; 
	m_xz_dist = (ai_info->flags & AIF_XZ_DIST) != 0; 

	m_avoid_friends_check = (ai_info->flags & AIF_AUTO_AVOID_FRIENDS) != 0;
	m_avoid_friends_dist = ai_info->avoid_friends_distance;

	m_fire_weapon1 = (ai_info->flags & AIF_WEAPON1) != 0;
	m_fire_weapon2 = (ai_info->flags & AIF_WEAPON2) != 0;
	m_melee1 = (ai_info->flags & AIF_MELEE1) != 0;
	m_melee2 = (ai_info->flags & AIF_MELEE2) != 0;

	m_stay_inout = (ai_info->flags & AIF_STAYS_INOUT) != 0;

	m_persistant_check = (ai_info->flags & AIF_PERSISTANT) != 0;
	m_dodge_check = (ai_info->flags & AIF_DODGE) != 0;
	m_fireattarget_check = (ai_info->flags & AIF_FIRE) != 0;
	m_flinch_check = (ai_info->flags & AIF_FLINCH) != 0;
	m_retarget_check = (ai_info->flags & AIF_DETERMINE_TARGET) != 0;
	m_turretcontrol_check = (ai_info->flags & AIF_AIM) != 0;
	m_tauntatdeath_check = (ai_info->flags & AIF_ONLY_TAUNT_AT_DEATH) != 0;
	m_avoidwalls_check = (ai_info->flags & AIF_AVOID_WALLS) != 0;
	m_ordered_wb_firing_check = (ai_info->flags & AIF_ORDERED_WB_FIRING) != 0;
	m_target_by_dist_check = (ai_info->flags & AIF_TARGET_BY_DIST) != 0;
	m_bfh_check = (ai_info->flags & AIF_BIASED_FLIGHT_HEIGHT) != 0;
	m_fov_uvec = (ai_info->flags & AIF_UVEC_FOV) != 0;
	m_fov_aim = (ai_info->flags & AIF_AIM_PNT_FOV) != 0;

	m_fluctuate_speed_properties_check = (ai_info->flags & AIF_FLUCTUATE_SPEED_PROPERTIES) != 0;

	m_notify_hears_noise = (ai_info->notify_flags & (0x00000001 << AIN_HEAR_NOISE)) != 0;
	m_notify_hitbyweapon_check = (ai_info->notify_flags & (0x00000001 << AIN_HIT_BY_WEAPON)) != 0;
	m_notify_nearwall_check = (ai_info->notify_flags & (0x00000001 << AIN_NEAR_WALL)) != 0;
	m_notify_objectfired_check = (ai_info->notify_flags & (0x00000001 << AIN_OBJ_FIRED)) != 0;
	m_notify_seeplayer_check = (ai_info->notify_flags & (0x00000001 << AIN_SEE_TARGET)) != 0;
	m_notify_weaponhittarget_check = (ai_info->notify_flags & (0x00000001 << AIN_WHIT_OBJECT)) != 0;

	if(ai_info->max_delta_turn_rate > 0.0)
	{
		m_ai_max_turn_rate_time = ai_info->max_turn_rate / ai_info->max_delta_turn_rate;
	}
	else
	{
		m_ai_max_turn_rate_time = 1.0f;
		m_ai_max_turn_rate = 0.0f;
	}
	
	m_circle_distance = ai_info->circle_distance;
	m_attack_vel_percent = ai_info->attack_vel_percent * 100.0f;
	m_dogde_percent = ai_info->dodge_percent * 100.0f;
	m_dodge_vel_percent = ai_info->dodge_vel_percent * 100.0f;
	m_flee_vel_percent = ai_info->flee_vel_percent * 100.0f;
	m_melee1_distance = ai_info->melee_damage[0];
	m_melee2_distance = ai_info->melee_damage[1];
	m_melee1_latency = ai_info->melee_latency[0];
	m_melee2_latency = ai_info->melee_latency[1];

	m_bfhmin = ai_info->biased_flight_min;
	m_bfhmax = ai_info->biased_flight_max;

	m_angry_sound_combo = Sounds[ai_info->sound[AI_ANGRY_SOUND]].name;
	m_attack_sound_combo = Sounds[ai_info->sound[AI_ATTACK_SOUND]].name;
	m_flee_sound_combo = Sounds[ai_info->sound[AI_FLEE_SOUND]].name;
	m_see_sound_combo = Sounds[ai_info->sound[AI_SEE_SOUND]].name;
	m_turret_sound_combo = Sounds[ai_info->sound[AI_TURRET_SOUND]].name;
}


void CAISettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAISettingsDlg)
	DDX_Control(pDX, IDC_BFHI_SLIDER, m_bfhi_slider);
	DDX_Control(pDX, IDC_AI_TLV_SLIDER, m_tlv_slider);
	DDX_Control(pDX, IDC_AI_TLA_SLIDER, m_tla_slider);
	DDX_Control(pDX, IDC_AI_ROAMING_SLIDER, m_roaming_slider);
	DDX_Control(pDX, IDC_AI_NV_SLIDER, m_nv_slider);
	DDX_Control(pDX, IDC_AI_LIFE_PRESERVATION_SLIDER, m_lp_slider);
	DDX_Control(pDX, IDC_AI_HEARING_SLIDER, m_hearing_slider);
	DDX_Control(pDX, IDC_AI_FV_SLIDER, m_fv_slider);
	DDX_Control(pDX, IDC_AI_FRUSTRATION_SLIDER, m_frustration_slider);
	DDX_Control(pDX, IDC_AI_FIGHT_TEAM_SLIDER, m_fight_team_slider);
	DDX_Control(pDX, IDC_AI_FIGHT_SAME_SLIDER, m_fight_same_slider);
	DDX_Control(pDX, IDC_AI_CURIOUSITY_SLIDER, m_curiousity_slider);
	DDX_Control(pDX, IDC_AI_AGRESSION_SLIDER, m_agression_slider);
	DDX_Control(pDX, IDC_AI_FIRE_SPREAD_SLIDER, m_fire_spread_slider);
	DDX_Check(pDX, IDC_AI_FIRE_WEAPON_1_CHECK, m_fire_weapon1);
	DDX_Check(pDX, IDC_AI_FIRE_WEAPON_2_CHECK, m_fire_weapon2);
	DDX_Check(pDX, IDC_AI_MELEE_1_CHECK, m_melee1);
	DDX_Check(pDX, IDC_AI_MELEE_2_CHECK, m_melee2);
	DDX_Check(pDX, IDC_AI_STAY_IN_OUT_CHECK, m_stay_inout);
	DDX_CBString(pDX, IDC_AI_CLASS_COMBO, m_ai_class);
	DDX_Text(pDX, IDC_AI_FOV_EDIT, m_ai_fov);
	DDV_MinMaxFloat(pDX, m_ai_fov, 0.0f, 360.0f);
	DDX_CBString(pDX, IDC_AI_INITIAL_ACTION_COMBO, m_initial_action);
	DDX_Check(pDX, IDC_AI_USES_THRUST_CHECK, m_ai_uses_thrust_check);
	DDX_CBString(pDX, IDC_AI_TYPE_COMBO, m_ai_type);
	DDX_CBString(pDX, IDC_AI_MOVEMENT_TYPE_COMBO, m_ai_movement_type);
	DDX_CBString(pDX, IDC_AI_MOVEMENT_SUBTYPE_COMBO, m_ai_movement_subtype);
	DDX_Text(pDX, IDC_AI_MAX_VELOCITY_EDIT, m_ai_max_velocity);
	DDX_Text(pDX, IDC_AI_MAX_VEL_TIME_EDIT, m_ai_max_vel_time);
	DDX_Text(pDX, IDC_AI_MAX_TURN_RATE_EDIT, m_ai_max_turn_rate);
	DDX_Check(pDX, IDC_AI_DODGE_CHECK, m_dodge_check);
	DDX_Check(pDX, IDC_AI_FIREATTARGET_CHECK, m_fireattarget_check);
	DDX_Check(pDX, IDC_AI_FLINCH_CHECK, m_flinch_check);
	DDX_Check(pDX, IDC_AI_NOTIFY_HITBYWEAPON_CHECK, m_notify_hitbyweapon_check);
	DDX_Check(pDX, IDC_AI_NOTIFY_NEARWALL_CHECK, m_notify_nearwall_check);
	DDX_Check(pDX, IDC_AI_NOTIFY_OBJFIRED_CHECK, m_notify_objectfired_check);
	DDX_Check(pDX, IDC_AI_NOTIFY_SEEPLAYER_CHECK, m_notify_seeplayer_check);
	DDX_Check(pDX, IDC_AI_NOTIFY_WEAPONHITPLAYER_CHECK, m_notify_weaponhittarget_check);
	DDX_Check(pDX, IDC_AI_RETARGET_CHECK, m_retarget_check);
	DDX_Check(pDX, IDC_AUTO_TURRETCONTROL_CHECK, m_turretcontrol_check);
	DDX_Check(pDX, IDC_TAUNTATDEATH_CHECK, m_tauntatdeath_check);
	DDX_Check(pDX, IDC_AI_AVOIDWALLS_CHECK, m_avoidwalls_check);
	DDX_Check(pDX, IDC_HEARS_NOISE_CHECK, m_notify_hears_noise);
	DDX_Check(pDX, IDC_PERSISTANT_CHECK, m_persistant_check);
	DDX_Check(pDX, IDC_AI_FLUCTUATE_PROPERTIES, m_fluctuate_speed_properties_check);
	DDX_Text(pDX, IDC_AI_MAX_TURN_RATE_TIME_EDIT, m_ai_max_turn_rate_time);
	DDV_MinMaxFloat(pDX, m_ai_max_turn_rate_time, 0.0f, 1.e+007f);
	DDX_CBString(pDX, IDC_AI_ANGRY_SOUND_COMBO, m_angry_sound_combo);
	DDX_CBString(pDX, IDC_AI_ATTACK_SOUND_COMBO, m_attack_sound_combo);
	DDX_Text(pDX, IDC_AI_CIRCLE_DISTANCE_EDIT, m_circle_distance);
	DDV_MinMaxFloat(pDX, m_circle_distance, 0.0f, 10000.0f);
	DDX_Text(pDX, IDC_AI_ATTACK_VEL_PER_EDIT, m_attack_vel_percent);
	DDV_MinMaxFloat(pDX, m_attack_vel_percent, 0.0f, 10000.0f);
	DDX_Text(pDX, IDC_AI_DODGE_PER_EDIT, m_dogde_percent);
	DDV_MinMaxFloat(pDX, m_dogde_percent, 0.0f, 100.0f);
	DDX_Text(pDX, IDC_AI_DODGE_VEL_PER_EDIT, m_dodge_vel_percent);
	DDV_MinMaxFloat(pDX, m_dodge_vel_percent, 0.f, 10000.f);
	DDX_CBString(pDX, IDC_AI_FLEE_SOUND_COMBO, m_flee_sound_combo);
	DDX_Text(pDX, IDC_AI_FLEE_VEL_PER_EDIT, m_flee_vel_percent);
	DDV_MinMaxFloat(pDX, m_flee_vel_percent, 0.0f, 10000.0f);
	DDX_Text(pDX, IDC_AI_MELEE1_LATENCY_EDIT, m_melee1_latency);
	DDX_Text(pDX, IDC_AI_MELEE1_DIST_EDIT, m_melee1_distance);
	DDV_MinMaxFloat(pDX, m_melee1_distance, 0.0f, 10000.0f);
	DDX_Text(pDX, IDC_AI_MELEE2_DIST_EDIT, m_melee2_distance);
	DDV_MinMaxFloat(pDX, m_melee2_distance, 0.0f, 10000.0f);
	DDX_Text(pDX, IDC_AI_MELEE2_LATENCY_EDIT, m_melee2_latency);
	DDV_MinMaxFloat(pDX, m_melee2_latency, 0.0f, 10000.0f);
	DDX_CBString(pDX, IDC_AI_SEE_SOUND_COMBO, m_see_sound_combo);
	DDX_Check(pDX, IDC_AI_ORDERED_WB_FIRING_CHECK, m_ordered_wb_firing_check);
	DDX_Check(pDX, IDC_AI_ORIENT_VEL_CHECK, m_orient_to_vel);
	DDX_Check(pDX, IDC_AI_XZ_TARGET_DIST, m_xz_dist);
	DDX_Check(pDX, IDC_AI_TARGET_BY_DIST_CHECK, m_target_by_dist_check);
	DDX_CBString(pDX, IDC_AI_TURRET_SOUND_COMBO, m_turret_sound_combo);
	DDX_Check(pDX, IDC_AI_AVOID_FRIENDS_CHECK, m_avoid_friends_check);
	DDX_Text(pDX, IDC_AVOID_FRIENDS_DIST_EDIT, m_avoid_friends_dist);
	DDV_MinMaxFloat(pDX, m_avoid_friends_dist, 0.f, 300.f);
	DDX_Text(pDX, IDC_BFHMAX_EDIT, m_bfhmax);
	DDX_Text(pDX, IDC_BFHMIN_EDIT, m_bfhmin);
	DDX_Check(pDX, IDC_BFH_CHECK, m_bfh_check);
	DDX_Check(pDX, IDC_AI_AIM_WITH_UVEC, m_fov_uvec);
	DDX_Check(pDX, IDC_AIM_FOV_CHECK, m_fov_aim);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAISettingsDlg, CDialog)
	//{{AFX_MSG_MAP(CAISettingsDlg)
	ON_CBN_SELCHANGE(IDC_AI_MOVEMENT_TYPE_COMBO, OnSelchangeAiMovementTypeCombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAISettingsDlg message handlers

BOOL CAISettingsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	int i;
	CButton *bbox;

	switch(m_ai_info->flags & AIF_TEAM_MASK)
	{
		case AIF_TEAM_PTMC:
			bbox = (CButton *)GetDlgItem(IDC_AI_PTMC_RADIO);
			break;
		case AIF_TEAM_REBEL:
			bbox = (CButton *)GetDlgItem(IDC_AI_REBEL_RADIO);
			break;
		case AIF_TEAM_NEUTRAL:
			bbox = (CButton *)GetDlgItem(IDC_AI_NEUTRAL_RADIO);
			break;
		case AIF_TEAM_HOSTILE:
			bbox = (CButton *)GetDlgItem(IDC_AI_HOSTILE_RADIO);
			break;
	}

	bbox->SetCheck(true);

	SendDlgItemMessage( IDC_AI_ANGRY_SOUND_COMBO, CB_RESETCONTENT,0,0);
	for (i=0;i<MAX_SOUNDS;i++)
	{
		if (Sounds[i].used)
			SendDlgItemMessage( IDC_AI_ANGRY_SOUND_COMBO, CB_ADDSTRING,0,(LPARAM) (LPCTSTR) Sounds[i].name);
	}
	SendDlgItemMessage( IDC_AI_ANGRY_SOUND_COMBO, CB_SELECTSTRING,0,(LPARAM) (LPCTSTR) m_angry_sound_combo);

	SendDlgItemMessage( IDC_AI_ATTACK_SOUND_COMBO, CB_RESETCONTENT,0,0);
	for (i=0;i<MAX_SOUNDS;i++)
	{
		if (Sounds[i].used)
			SendDlgItemMessage( IDC_AI_ATTACK_SOUND_COMBO, CB_ADDSTRING,0,(LPARAM) (LPCTSTR) Sounds[i].name);
	}
	SendDlgItemMessage( IDC_AI_ATTACK_SOUND_COMBO, CB_SELECTSTRING,0,(LPARAM) (LPCTSTR) m_attack_sound_combo);

	SendDlgItemMessage( IDC_AI_FLEE_SOUND_COMBO, CB_RESETCONTENT,0,0);
	for (i=0;i<MAX_SOUNDS;i++)
	{
		if (Sounds[i].used)
			SendDlgItemMessage( IDC_AI_FLEE_SOUND_COMBO, CB_ADDSTRING,0,(LPARAM) (LPCTSTR) Sounds[i].name);
	}
	SendDlgItemMessage( IDC_AI_FLEE_SOUND_COMBO, CB_SELECTSTRING,0,(LPARAM) (LPCTSTR) m_flee_sound_combo);

	SendDlgItemMessage( IDC_AI_SEE_SOUND_COMBO, CB_RESETCONTENT,0,0);
	for (i=0;i<MAX_SOUNDS;i++)
	{
		if (Sounds[i].used)
			SendDlgItemMessage( IDC_AI_SEE_SOUND_COMBO, CB_ADDSTRING,0,(LPARAM) (LPCTSTR) Sounds[i].name);
	}
	SendDlgItemMessage( IDC_AI_SEE_SOUND_COMBO, CB_SELECTSTRING,0,(LPARAM) (LPCTSTR) m_see_sound_combo);

	SendDlgItemMessage( IDC_AI_TURRET_SOUND_COMBO, CB_RESETCONTENT,0,0);
	for (i=0;i<MAX_SOUNDS;i++)
	{
		if (Sounds[i].used)
			SendDlgItemMessage( IDC_AI_TURRET_SOUND_COMBO, CB_ADDSTRING,0,(LPARAM) (LPCTSTR) Sounds[i].name);
	}
	SendDlgItemMessage( IDC_AI_TURRET_SOUND_COMBO, CB_SELECTSTRING,0,(LPARAM) (LPCTSTR) m_turret_sound_combo);

	SendDlgItemMessage( IDC_AI_CLASS_COMBO, CB_RESETCONTENT,0,0);
	for (i=0;i<MAX_AI_INIT_CLASSES;i++)
	{
		if(!stricmp("", Ai_class_strings[i])) break;
			SendDlgItemMessage( IDC_AI_CLASS_COMBO, CB_ADDSTRING,0,(LPARAM) (LPCTSTR) Ai_class_strings[i]);
	}
	SendDlgItemMessage( IDC_AI_CLASS_COMBO, CB_SELECTSTRING,0,(LPARAM) (LPCTSTR) m_ai_class);

	SendDlgItemMessage( IDC_AI_TYPE_COMBO, CB_RESETCONTENT,0,0);
	for (i=0;i<MAX_AI_INIT_TYPES;i++)
	{
		if(!stricmp("", Ai_type_strings[i])) break;
			SendDlgItemMessage( IDC_AI_TYPE_COMBO, CB_ADDSTRING,0,(LPARAM) (LPCTSTR) Ai_type_strings[i]);
	}
	SendDlgItemMessage( IDC_AI_TYPE_COMBO, CB_SELECTSTRING,0,(LPARAM) (LPCTSTR) m_ai_type);

	SendDlgItemMessage( IDC_AI_MOVEMENT_TYPE_COMBO, CB_RESETCONTENT,0,0);
	for (i=0;i<MAX_AI_INIT_MOVEMENT_TYPES;i++)
	{
		if(!stricmp("", Ai_movement_type_strings[i])) break;
			SendDlgItemMessage( IDC_AI_MOVEMENT_TYPE_COMBO, CB_ADDSTRING,0,(LPARAM) (LPCTSTR) Ai_movement_type_strings[i]);
	}
	SendDlgItemMessage( IDC_AI_MOVEMENT_TYPE_COMBO, CB_SELECTSTRING,0,(LPARAM) (LPCTSTR) m_ai_movement_type);

	SendDlgItemMessage( IDC_AI_MOVEMENT_SUBTYPE_COMBO, CB_RESETCONTENT,0,0);
	if(!stricmp(m_ai_movement_type, "FLYING"))
	{
		for (i=0;i<MAX_AI_INIT_MOVEMENT_SUBTYPES;i++)
		{
			if(!stricmp("", Ai_movement_subtype_flying_strings[i])) break;
				SendDlgItemMessage( IDC_AI_MOVEMENT_SUBTYPE_COMBO, CB_ADDSTRING,0,(LPARAM) (LPCTSTR) Ai_movement_subtype_flying_strings[i]);
		}
		SendDlgItemMessage( IDC_AI_MOVEMENT_SUBTYPE_COMBO, CB_SELECTSTRING,0,(LPARAM) (LPCTSTR) m_ai_movement_subtype);
	}
	else if(!stricmp(m_ai_movement_type, "WALKING"))
	{
		for (i=0;i<MAX_AI_INIT_MOVEMENT_SUBTYPES;i++)
		{
			if(!stricmp("", Ai_movement_subtype_walking_strings[i])) break;
				SendDlgItemMessage( IDC_AI_MOVEMENT_SUBTYPE_COMBO, CB_ADDSTRING,0,(LPARAM) (LPCTSTR) Ai_movement_subtype_walking_strings[i]);
		}
		SendDlgItemMessage( IDC_AI_MOVEMENT_SUBTYPE_COMBO, CB_SELECTSTRING,0,(LPARAM) (LPCTSTR) m_ai_movement_subtype);
	}
	else
	{
		SendDlgItemMessage( IDC_AI_MOVEMENT_SUBTYPE_COMBO, CB_SELECTSTRING,0,(LPARAM) (LPCTSTR) "");
	}

	m_fire_spread_slider.SetRange(0, MAX_SLIDER_RANGE);
	m_fire_spread_slider.SetPos((int)(m_ai_info->fire_spread * MAX_SLIDER_RANGE));

	m_bfhi_slider.SetRange(0, MAX_SLIDER_RANGE);
	m_bfhi_slider.SetPos((int)(m_ai_info->biased_flight_importance * MAX_SLIDER_RANGE));

	m_nv_slider.SetRange(0, MAX_SLIDER_RANGE);
	m_nv_slider.SetPos((int)(m_ai_info->night_vision * MAX_SLIDER_RANGE));

	m_fv_slider.SetRange(0, MAX_SLIDER_RANGE);
	m_fv_slider.SetPos((int)(m_ai_info->fog_vision * MAX_SLIDER_RANGE));

	m_curiousity_slider.SetRange(0, MAX_SLIDER_RANGE);
	m_curiousity_slider.SetPos((int)(m_ai_info->curiousity * MAX_SLIDER_RANGE));

	m_tla_slider.SetRange(0, MAX_SLIDER_RANGE);
	m_tla_slider.SetPos((int)(m_ai_info->lead_accuracy * MAX_SLIDER_RANGE));

	m_tlv_slider.SetRange(0, MAX_SLIDER_RANGE);
	m_tlv_slider.SetPos((int)(m_ai_info->lead_varience * MAX_SLIDER_RANGE));

	m_fight_team_slider.SetRange(0, MAX_SLIDER_RANGE);
	m_fight_team_slider.SetPos((int)(m_ai_info->fight_team * MAX_SLIDER_RANGE));

	m_fight_same_slider.SetRange(0, MAX_SLIDER_RANGE);
	m_fight_same_slider.SetPos((int)(m_ai_info->fight_same * MAX_SLIDER_RANGE));

	m_agression_slider.SetRange(0, MAX_SLIDER_RANGE);
	m_agression_slider.SetPos((int)(m_ai_info->agression * MAX_SLIDER_RANGE));

	m_hearing_slider.SetRange(0, MAX_SLIDER_RANGE);
	m_hearing_slider.SetPos((int)(m_ai_info->hearing * MAX_SLIDER_RANGE));

	m_frustration_slider.SetRange(0, MAX_SLIDER_RANGE);
	m_frustration_slider.SetPos((int)(m_ai_info->frustration * MAX_SLIDER_RANGE));

	m_roaming_slider.SetRange(0, MAX_SLIDER_RANGE);
	m_roaming_slider.SetPos((int)(m_ai_info->roaming * MAX_SLIDER_RANGE));

	m_lp_slider.SetRange(0, MAX_SLIDER_RANGE);
	m_lp_slider.SetPos((int)(m_ai_info->life_preservation * MAX_SLIDER_RANGE));

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAISettingsDlg::OnSelchangeAiMovementTypeCombo() 
{
	int i;

	int cur;
	char name[200];

	if(!UpdateData(true)) return;	

	cur=SendDlgItemMessage( IDC_AI_MOVEMENT_TYPE_COMBO, CB_GETCURSEL,0,0);
	SendDlgItemMessage( IDC_AI_MOVEMENT_TYPE_COMBO, CB_GETLBTEXT,cur,(LPARAM) (LPCTSTR)name);

	m_ai_movement_type = name;

	UpdateData(false);
	
	SendDlgItemMessage( IDC_AI_MOVEMENT_SUBTYPE_COMBO, CB_RESETCONTENT,0,0);
	if(!stricmp(m_ai_movement_type, "FLYING"))
	{
		for (i=0;i<MAX_AI_INIT_MOVEMENT_SUBTYPES;i++)
		{
			if(!stricmp("", Ai_movement_subtype_flying_strings[i])) break;
				SendDlgItemMessage( IDC_AI_MOVEMENT_SUBTYPE_COMBO, CB_ADDSTRING,0,(LPARAM) (LPCTSTR) Ai_movement_subtype_flying_strings[i]);
		}
		SendDlgItemMessage( IDC_AI_MOVEMENT_SUBTYPE_COMBO, CB_SELECTSTRING,0,(LPARAM) (LPCTSTR) Ai_movement_subtype_flying_strings[0]);
	}
	else if(!stricmp(m_ai_movement_type, "WALKING"))
	{
		for (i=0;i<MAX_AI_INIT_MOVEMENT_SUBTYPES;i++)
		{
			if(!stricmp("", Ai_movement_subtype_walking_strings[i])) break;
				SendDlgItemMessage( IDC_AI_MOVEMENT_SUBTYPE_COMBO, CB_ADDSTRING,0,(LPARAM) (LPCTSTR) Ai_movement_subtype_walking_strings[i]);
		}
		SendDlgItemMessage( IDC_AI_MOVEMENT_SUBTYPE_COMBO, CB_SELECTSTRING,0,(LPARAM) (LPCTSTR) Ai_movement_subtype_walking_strings[0]);
	}
	else
	{
		SendDlgItemMessage( IDC_AI_MOVEMENT_SUBTYPE_COMBO, CB_SELECTSTRING,0,(LPARAM) (LPCTSTR) "");
	}
}

void CAISettingsDlg::OnOK() 
{
	CDialog::OnOK();

	//Copy the new data into our original struct
	int i;

	m_ai_info->flags &= ~AIF_TEAM_MASK;

	if(((CButton *)GetDlgItem(IDC_AI_PTMC_RADIO))->GetCheck())
		m_ai_info->flags |= AIF_TEAM_PTMC;
	else if(((CButton *)GetDlgItem(IDC_AI_REBEL_RADIO))->GetCheck())
		m_ai_info->flags |= AIF_TEAM_REBEL;
	else if(((CButton *)GetDlgItem(IDC_AI_NEUTRAL_RADIO))->GetCheck())
		m_ai_info->flags |= AIF_TEAM_NEUTRAL;
	else if(((CButton *)GetDlgItem(IDC_AI_HOSTILE_RADIO))->GetCheck())
		m_ai_info->flags |= AIF_TEAM_HOSTILE;
	else
	{
		mprintf((0, "ERROR: No team\n"));
		Int3();
	}

	if (m_fire_weapon1)
		m_ai_info->flags |= AIF_WEAPON1;
	else
		m_ai_info->flags &= ~AIF_WEAPON1;

	if (m_fire_weapon2)
		m_ai_info->flags |= AIF_WEAPON2;
	else
		m_ai_info->flags &= ~AIF_WEAPON2;

	if (m_melee1)
		m_ai_info->flags |= AIF_MELEE1;
	else
		m_ai_info->flags &= ~AIF_MELEE1;

	if(m_orient_to_vel)
		m_ai_info->flags |= AIF_ORIENT_TO_VEL;
	else
		m_ai_info->flags &= ~AIF_ORIENT_TO_VEL;

	if(m_xz_dist)
		m_ai_info->flags |= AIF_XZ_DIST;
	else
		m_ai_info->flags &= ~AIF_XZ_DIST;

	if(m_avoid_friends_check)
		m_ai_info->flags |= AIF_AUTO_AVOID_FRIENDS;
	else
		m_ai_info->flags &= ~AIF_AUTO_AVOID_FRIENDS;

	if (m_melee2)
		m_ai_info->flags |= AIF_MELEE2;
	else
		m_ai_info->flags &= ~AIF_MELEE2;

	if (m_stay_inout)
		m_ai_info->flags |= AIF_STAYS_INOUT;
	else
		m_ai_info->flags &= ~AIF_STAYS_INOUT;

	for(i = 0; i < MAX_AI_INIT_CLASSES; i++)
	{
		if(!stricmp(m_ai_class, Ai_class_strings[i]))
		{
			m_ai_info->ai_class = i;
			break;
		}
	}
	ASSERT(i < MAX_AI_INIT_CLASSES);

	for(i = 0; i < MAX_AI_INIT_TYPES; i++)
	{
		if(!stricmp(m_ai_type, Ai_type_strings[i]))
		{
			m_ai_info->ai_type = i;
			break;
		}
	}
	ASSERT(i < MAX_AI_INIT_TYPES);

	for(i = 0; i < MAX_AI_INIT_MOVEMENT_TYPES; i++)
	{
		if(!stricmp(m_ai_movement_type, Ai_movement_type_strings[i]))
		{
			m_ai_info->movement_type = i;
			break;
		}
	}
	ASSERT(i < MAX_AI_INIT_MOVEMENT_TYPES);

	if(!stricmp(m_ai_movement_type,"WALKING"))
	{
		for(i = 0; i < MAX_AI_INIT_MOVEMENT_SUBTYPES; i++)
		{
			if(!stricmp(m_ai_movement_subtype, Ai_movement_subtype_walking_strings[i]))
			{
				m_ai_info->movement_subtype = i;
				break;
			}
		}
		ASSERT(i < MAX_AI_INIT_MOVEMENT_SUBTYPES);
	}
	else if (!stricmp(m_ai_movement_type,"FLYING"))
	{
		for(i = 0; i < MAX_AI_INIT_MOVEMENT_SUBTYPES; i++)
		{
			if(!stricmp(m_ai_movement_subtype, Ai_movement_subtype_flying_strings[i]))
			{
				m_ai_info->movement_subtype = i;
				break;
			}
		}
		ASSERT(i < MAX_AI_INIT_MOVEMENT_SUBTYPES);
	}
	else
	{
		m_ai_info->movement_subtype = 0;
	}
	
	m_ai_info->max_velocity = m_ai_max_velocity;
	if(m_ai_max_vel_time > 0.0f)
		m_ai_info->max_delta_velocity = m_ai_max_velocity / m_ai_max_vel_time;
	else
		m_ai_info->max_delta_velocity = 0.0f;
	m_ai_info->max_turn_rate = m_ai_max_turn_rate;

	if(m_persistant_check)
		m_ai_info->flags |= AIF_PERSISTANT;
	else
		m_ai_info->flags &= ~AIF_PERSISTANT;

	if(m_dodge_check)
		m_ai_info->flags |= AIF_DODGE;
	else
		m_ai_info->flags &= ~AIF_DODGE;
	if(m_fireattarget_check)
		m_ai_info->flags |= AIF_FIRE;
	else
		m_ai_info->flags &= ~AIF_FIRE;
	if(m_flinch_check)
		m_ai_info->flags |= AIF_FLINCH;
	else
		m_ai_info->flags &= ~AIF_FLINCH;
	if(m_retarget_check)
		m_ai_info->flags |= AIF_DETERMINE_TARGET;
	else
		m_ai_info->flags &= ~AIF_DETERMINE_TARGET;
	if(m_turretcontrol_check)
		m_ai_info->flags |= AIF_AIM;
	else
		m_ai_info->flags &= ~AIF_AIM;
	if(m_tauntatdeath_check)
		m_ai_info->flags |= AIF_ONLY_TAUNT_AT_DEATH;
	else
		m_ai_info->flags &= ~AIF_ONLY_TAUNT_AT_DEATH;
	if(m_avoidwalls_check)
		m_ai_info->flags |= AIF_AVOID_WALLS;
	else
		m_ai_info->flags &= ~AIF_AVOID_WALLS;
	if(m_fluctuate_speed_properties_check)
		m_ai_info->flags |= AIF_FLUCTUATE_SPEED_PROPERTIES;
	else
		m_ai_info->flags &= ~AIF_FLUCTUATE_SPEED_PROPERTIES;
	if(m_ordered_wb_firing_check)
		m_ai_info->flags |= AIF_ORDERED_WB_FIRING;
	else
		m_ai_info->flags &= ~AIF_ORDERED_WB_FIRING;
	if(m_target_by_dist_check)
		m_ai_info->flags |= AIF_TARGET_BY_DIST;
	else
		m_ai_info->flags &= ~AIF_TARGET_BY_DIST;

	if(m_bfh_check)
		m_ai_info->flags |= AIF_BIASED_FLIGHT_HEIGHT;
	else
		m_ai_info->flags &= ~AIF_BIASED_FLIGHT_HEIGHT;

	if(m_fov_uvec)
		m_ai_info->flags |= AIF_UVEC_FOV;
	else
		m_ai_info->flags &= ~AIF_UVEC_FOV;

	if(m_fov_aim)
		m_ai_info->flags |= AIF_AIM_PNT_FOV;
	else
		m_ai_info->flags &= ~AIF_AIM_PNT_FOV;

	if(m_notify_hears_noise)
		m_ai_info->notify_flags |= (0x00000001 << AIN_HEAR_NOISE);
	else
		m_ai_info->notify_flags &= ~(0x00000001 << AIN_HEAR_NOISE);
	
	if(m_notify_hitbyweapon_check)
		m_ai_info->notify_flags |= (0x00000001 << AIN_HIT_BY_WEAPON);
	else
		m_ai_info->notify_flags &= ~(0x00000001 << AIN_HIT_BY_WEAPON);
	
	if(m_notify_nearwall_check)
		m_ai_info->notify_flags |= (0x00000001 << AIN_NEAR_WALL);
	else
		m_ai_info->notify_flags &= ~(0x00000001 << AIN_NEAR_WALL);
	
	if(m_notify_objectfired_check)
		m_ai_info->notify_flags |= (0x00000001 << AIN_OBJ_FIRED);
	else
		m_ai_info->notify_flags &= ~(0x00000001 << AIN_OBJ_FIRED);
	
	if(m_notify_seeplayer_check)
		m_ai_info->notify_flags |= (0x00000001 << AIN_SEE_TARGET);
	else
		m_ai_info->notify_flags &= ~(0x00000001 << AIN_SEE_TARGET);
	
	if(m_notify_weaponhittarget_check)
		m_ai_info->notify_flags |= (0x00000001 << AIN_WHIT_OBJECT);
	else
		m_ai_info->notify_flags &= ~(0x00000001 << AIN_WHIT_OBJECT);

	m_ai_info->fov = cos(m_ai_fov*PI/(360.0));
	
	if(m_ai_max_turn_rate_time > 0.0f)
		m_ai_info->max_delta_turn_rate = m_ai_max_turn_rate / m_ai_max_turn_rate_time;
	else
		m_ai_info->max_delta_turn_rate = 0.0f;
	
	m_ai_info->circle_distance = m_circle_distance;
	m_ai_info->attack_vel_percent = m_attack_vel_percent/100.0;
	m_ai_info->dodge_percent = m_dogde_percent/100.0;
	m_ai_info->dodge_vel_percent = m_dodge_vel_percent/100.0;
	m_ai_info->flee_vel_percent = m_flee_vel_percent/100.0;
	m_ai_info->melee_damage[0] = m_melee1_distance;
	m_ai_info->melee_damage[1] = m_melee2_distance;
	m_ai_info->melee_latency[0] = m_melee1_latency;
	m_ai_info->melee_latency[1] = m_melee2_latency;
	m_ai_info->avoid_friends_distance = m_avoid_friends_dist;

	m_ai_info->biased_flight_min = m_bfhmin;
	m_ai_info->biased_flight_max = m_bfhmax;

	m_ai_info->sound[AI_ANGRY_SOUND] = FindSoundName(m_angry_sound_combo.GetBuffer(0));
	m_ai_info->sound[AI_ATTACK_SOUND] = FindSoundName(m_attack_sound_combo.GetBuffer(0));
	m_ai_info->sound[AI_FLEE_SOUND] = FindSoundName(m_flee_sound_combo.GetBuffer(0));
	m_ai_info->sound[AI_SEE_SOUND] = FindSoundName(m_see_sound_combo.GetBuffer(0));
	m_ai_info->sound[AI_TURRET_SOUND] = FindSoundName(m_turret_sound_combo.GetBuffer(0));

	m_ai_info->fire_spread = m_fire_spread_slider.GetPos()/(float)MAX_SLIDER_RANGE;
	m_ai_info->night_vision = m_nv_slider.GetPos()/(float)MAX_SLIDER_RANGE;
	m_ai_info->fog_vision = m_fv_slider.GetPos()/(float)MAX_SLIDER_RANGE;
	m_ai_info->curiousity = m_curiousity_slider.GetPos()/(float)MAX_SLIDER_RANGE;
	m_ai_info->lead_accuracy = m_tla_slider.GetPos()/(float)MAX_SLIDER_RANGE;
	m_ai_info->lead_varience = m_tlv_slider.GetPos()/(float)MAX_SLIDER_RANGE;
	m_ai_info->fight_team = m_fight_team_slider.GetPos()/(float)MAX_SLIDER_RANGE;
	m_ai_info->fight_same = m_fight_same_slider.GetPos()/(float)MAX_SLIDER_RANGE;
	m_ai_info->agression = m_agression_slider.GetPos()/(float)MAX_SLIDER_RANGE;
	m_ai_info->hearing = m_hearing_slider.GetPos()/(float)MAX_SLIDER_RANGE;
	m_ai_info->frustration = m_frustration_slider.GetPos()/(float)MAX_SLIDER_RANGE;
	m_ai_info->roaming = m_roaming_slider.GetPos()/(float)MAX_SLIDER_RANGE;
	m_ai_info->life_preservation = m_lp_slider.GetPos()/(float)MAX_SLIDER_RANGE;
	m_ai_info->biased_flight_importance = m_bfhi_slider.GetPos()/(float)MAX_SLIDER_RANGE;
}
