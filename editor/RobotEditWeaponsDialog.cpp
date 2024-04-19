/*
* $Logfile: /DescentIII/Main/editor/RobotEditWeaponsDialog.cpp $
* $Revision: 1.1.1.1 $
* $Date: 2003-08-26 03:57:38 $
* $Author: kevinb $
*
* Robot weapon battery dialog
*
* $Log: not supported by cvs2svn $
 * 
 * 35    3/02/99 7:23p Chris
 * Add the 'fire at target' check box for weapon batteries
 * 
 * 34    1/31/99 11:26p Chris
 * added WBF_AIM_FVEC
 * 
 * 33    1/25/99 8:51a Chris
 * 
 * 32    11/13/98 12:30p Jason
 * changes for weapons
 * 
 * 31    11/10/98 6:18p Chris
 * Improved AI - added support for manual max firing distances
 * 
 * 30    11/03/98 6:37p Chris
 * Started to add support for on/off and spray weapons for robots
 * 
 * 29    10/19/98 8:11p Chris
 * 
 * 28    10/19/98 7:51p Chris
 * 
 * 27    10/19/98 7:38p Chris
 * 
 * 26    10/19/98 7:20p Chris
 * Added custom fire dot code
 * 
 * 25    9/22/98 12:04p Matt
 * Fixed bogus handling of OutrageMessageBox() results.
 * 
 * 24    8/14/98 4:57p Jeff
 * edit quad fire mask
 * 
 * 23    8/14/98 3:32p Chris
 * Added quad support
 * 
 * 22    8/05/98 12:41p Chris
 * Fixed a bug with zero turret wbs
 * 
 * 21    4/09/98 2:23p Jason
 * added guided/afterburner stuff
 * 
 * 20    3/30/98 10:50a Chris
 * I added support of random next firing masks
 * 
 * 19    3/23/98 10:03a Chris
 * Added independant wb animations
 * 
 * 18    2/09/98 11:12a Chris
 * Allowed for the edititng of multiple weapon batteries
 * 
 * 17    2/04/98 12:35a Chris
 * Fixed weapon battery stuff for non-turret'ed weapon batteries
 * 
 * 16    1/23/98 6:25p Jason
 * Got spray weapons working
 * 
 * 15    1/23/98 12:31p Chris
 * Fixed bug induced by Jason because of different dialog coding
 * conventions
*
* $NoKeywords: $
*/

#include "stdafx.h"
#include "editor.h"
#include "RobotEditWeaponsDialog.h"
#include "weapon.h"
#include "string.h"
#include "polymodel.h"
#include "fix.h"
#include "weapon.h"
#include "soundload.h"
#include "soundpage.h"
#include "hlsoundlib.h"
#include "mono.h"
#include "pserror.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// RobotEditWeaponsDialog dialog

void RobotEditWeaponsDialog::NewWBSelected()
{
	char str[200];

	m_current_turret_index = 0;
	
	sprintf(str, "Num Turrets for GB: %d", m_obj_pm->poly_wb[m_current_wb_index].num_turrets);
	m_num_turrets_text = str;

	m_num_masks = m_obj_static_wb[m_current_wb_index].num_masks;

	m_ammo_usage=m_obj_static_wb[m_current_wb_index].ammo_usage;
	m_energy_usage=m_obj_static_wb[m_current_wb_index].energy_usage;

	m_view_cone_angle = acos(m_obj_static_wb[m_current_wb_index].aiming_3d_dot)*(360.0/PI);
	m_max_distance = m_obj_static_wb[m_current_wb_index].aiming_3d_dist;
	m_xz_plane_angle = acos(m_obj_static_wb[m_current_wb_index].aiming_XZ_dot)*(360.0/PI);

	m_current_aiming_point = m_obj_static_wb[m_current_wb_index].aiming_gp_index;
	m_anim_type = m_obj_static_wb[m_current_wb_index].flags & WBF_ANIM_MASKS;

	sprintf(str, "%d", m_current_wb_index+1);
	m_current_gb = str;

	UpdateTurretInfo();
	
//	sprintf(str, "%d", m_obj_pm->poly_wb[m_current_wb_index].turret_index[m_current_turret_index]);
	sprintf(str, "%d", m_current_turret_index+1);
	m_current_turret_combo = str;

	m_weapon_0_text = Weapons[m_obj_static_wb[m_current_wb_index].gp_weapon_index[0]].name;
	m_weapon_1_text = Weapons[m_obj_static_wb[m_current_wb_index].gp_weapon_index[1]].name;
	m_weapon_2_text = Weapons[m_obj_static_wb[m_current_wb_index].gp_weapon_index[2]].name;
	m_weapon_3_text = Weapons[m_obj_static_wb[m_current_wb_index].gp_weapon_index[3]].name;
	m_weapon_4_text = Weapons[m_obj_static_wb[m_current_wb_index].gp_weapon_index[4]].name;
	m_weapon_5_text = Weapons[m_obj_static_wb[m_current_wb_index].gp_weapon_index[5]].name;
	m_weapon_6_text = Weapons[m_obj_static_wb[m_current_wb_index].gp_weapon_index[6]].name;
	m_weapon_7_text = Weapons[m_obj_static_wb[m_current_wb_index].gp_weapon_index[7]].name;

	m_fire_sound_0_text = Sounds[m_obj_static_wb[m_current_wb_index].fm_fire_sound_index[0]].name;
	m_fire_sound_1_text = Sounds[m_obj_static_wb[m_current_wb_index].fm_fire_sound_index[1]].name;
	m_fire_sound_2_text = Sounds[m_obj_static_wb[m_current_wb_index].fm_fire_sound_index[2]].name;
	m_fire_sound_3_text = Sounds[m_obj_static_wb[m_current_wb_index].fm_fire_sound_index[3]].name;
	m_fire_sound_4_text = Sounds[m_obj_static_wb[m_current_wb_index].fm_fire_sound_index[4]].name;
	m_fire_sound_5_text = Sounds[m_obj_static_wb[m_current_wb_index].fm_fire_sound_index[5]].name;
	m_fire_sound_6_text = Sounds[m_obj_static_wb[m_current_wb_index].fm_fire_sound_index[6]].name;
	m_fire_sound_7_text = Sounds[m_obj_static_wb[m_current_wb_index].fm_fire_sound_index[7]].name;

	m_mask_latency_0 = m_obj_static_wb[m_current_wb_index].gp_fire_wait[0];
	m_mask_latency_1 = m_obj_static_wb[m_current_wb_index].gp_fire_wait[1];
	m_mask_latency_2 = m_obj_static_wb[m_current_wb_index].gp_fire_wait[2];
	m_mask_latency_3 = m_obj_static_wb[m_current_wb_index].gp_fire_wait[3];
	m_mask_latency_4 = m_obj_static_wb[m_current_wb_index].gp_fire_wait[4];
	m_mask_latency_5 = m_obj_static_wb[m_current_wb_index].gp_fire_wait[5];
	m_mask_latency_6 = m_obj_static_wb[m_current_wb_index].gp_fire_wait[6];
	m_mask_latency_7 = m_obj_static_wb[m_current_wb_index].gp_fire_wait[7];

	m_gp77 = ((m_obj_static_wb[m_current_wb_index].gp_fire_masks[7] & 0x01 << 7) != 0);
	m_gp76 = ((m_obj_static_wb[m_current_wb_index].gp_fire_masks[7] & 0x01 << 6) != 0);
	m_gp75 = ((m_obj_static_wb[m_current_wb_index].gp_fire_masks[7] & 0x01 << 5) != 0);
	m_gp74 = ((m_obj_static_wb[m_current_wb_index].gp_fire_masks[7] & 0x01 << 4) != 0);
	m_gp73 = ((m_obj_static_wb[m_current_wb_index].gp_fire_masks[7] & 0x01 << 3) != 0);
	m_gp72 = ((m_obj_static_wb[m_current_wb_index].gp_fire_masks[7] & 0x01 << 2) != 0);
	m_gp71 = ((m_obj_static_wb[m_current_wb_index].gp_fire_masks[7] & 0x01 << 1) != 0);
	m_gp70 = ((m_obj_static_wb[m_current_wb_index].gp_fire_masks[7] & 0x01) != 0);
	m_gp67 = ((m_obj_static_wb[m_current_wb_index].gp_fire_masks[6] & 0x01 << 7) != 0);
	m_gp66 = ((m_obj_static_wb[m_current_wb_index].gp_fire_masks[6] & 0x01 << 6) != 0);
	m_gp65 = ((m_obj_static_wb[m_current_wb_index].gp_fire_masks[6] & 0x01 << 5) != 0);
	m_gp64 = ((m_obj_static_wb[m_current_wb_index].gp_fire_masks[6] & 0x01 << 4) != 0);
	m_gp63 = ((m_obj_static_wb[m_current_wb_index].gp_fire_masks[6] & 0x01 << 3) != 0);
	m_gp62 = ((m_obj_static_wb[m_current_wb_index].gp_fire_masks[6] & 0x01 << 2) != 0);
	m_gp61 = ((m_obj_static_wb[m_current_wb_index].gp_fire_masks[6] & 0x01 << 1) != 0);
	m_gp60 = ((m_obj_static_wb[m_current_wb_index].gp_fire_masks[6] & 0x01) != 0);
	m_gp57 = ((m_obj_static_wb[m_current_wb_index].gp_fire_masks[5] & 0x01 << 7) != 0);
	m_gp56 = ((m_obj_static_wb[m_current_wb_index].gp_fire_masks[5] & 0x01 << 6) != 0);
	m_gp55 = ((m_obj_static_wb[m_current_wb_index].gp_fire_masks[5] & 0x01 << 5) != 0);
	m_gp54 = ((m_obj_static_wb[m_current_wb_index].gp_fire_masks[5] & 0x01 << 4) != 0);
	m_gp53 = ((m_obj_static_wb[m_current_wb_index].gp_fire_masks[5] & 0x01 << 3) != 0);
	m_gp52 = ((m_obj_static_wb[m_current_wb_index].gp_fire_masks[5] & 0x01 << 2) != 0);
	m_gp51 = ((m_obj_static_wb[m_current_wb_index].gp_fire_masks[5] & 0x01 << 1) != 0);
	m_gp50 = ((m_obj_static_wb[m_current_wb_index].gp_fire_masks[5] & 0x01) != 0);
	m_gp47 = ((m_obj_static_wb[m_current_wb_index].gp_fire_masks[4] & 0x01 << 7) != 0);
	m_gp46 = ((m_obj_static_wb[m_current_wb_index].gp_fire_masks[4] & 0x01 << 6) != 0);
	m_gp45 = ((m_obj_static_wb[m_current_wb_index].gp_fire_masks[4] & 0x01 << 5) != 0);
	m_gp44 = ((m_obj_static_wb[m_current_wb_index].gp_fire_masks[4] & 0x01 << 4) != 0);
	m_gp43 = ((m_obj_static_wb[m_current_wb_index].gp_fire_masks[4] & 0x01 << 3) != 0);
	m_gp42 = ((m_obj_static_wb[m_current_wb_index].gp_fire_masks[4] & 0x01 << 2) != 0);
	m_gp41 = ((m_obj_static_wb[m_current_wb_index].gp_fire_masks[4] & 0x01 << 1) != 0);
	m_gp40 = ((m_obj_static_wb[m_current_wb_index].gp_fire_masks[4] & 0x01) != 0);
	m_gp37 = ((m_obj_static_wb[m_current_wb_index].gp_fire_masks[3] & 0x01 << 7) != 0);
	m_gp36 = ((m_obj_static_wb[m_current_wb_index].gp_fire_masks[3] & 0x01 << 6) != 0);
	m_gp35 = ((m_obj_static_wb[m_current_wb_index].gp_fire_masks[3] & 0x01 << 5) != 0);
	m_gp34 = ((m_obj_static_wb[m_current_wb_index].gp_fire_masks[3] & 0x01 << 4) != 0);
	m_gp33 = ((m_obj_static_wb[m_current_wb_index].gp_fire_masks[3] & 0x01 << 3) != 0);
	m_gp32 = ((m_obj_static_wb[m_current_wb_index].gp_fire_masks[3] & 0x01 << 2) != 0);
	m_gp31 = ((m_obj_static_wb[m_current_wb_index].gp_fire_masks[3] & 0x01 << 1) != 0);
	m_gp30 = ((m_obj_static_wb[m_current_wb_index].gp_fire_masks[3] & 0x01) != 0);
	m_gp27 = ((m_obj_static_wb[m_current_wb_index].gp_fire_masks[2] & 0x01 << 7) != 0);
	m_gp26 = ((m_obj_static_wb[m_current_wb_index].gp_fire_masks[2] & 0x01 << 6) != 0);
	m_gp25 = ((m_obj_static_wb[m_current_wb_index].gp_fire_masks[2] & 0x01 << 5) != 0);
	m_gp24 = ((m_obj_static_wb[m_current_wb_index].gp_fire_masks[2] & 0x01 << 4) != 0);
	m_gp23 = ((m_obj_static_wb[m_current_wb_index].gp_fire_masks[2] & 0x01 << 3) != 0);
	m_gp22 = ((m_obj_static_wb[m_current_wb_index].gp_fire_masks[2] & 0x01 << 2) != 0);
	m_gp21 = ((m_obj_static_wb[m_current_wb_index].gp_fire_masks[2] & 0x01 << 1) != 0);
	m_gp20 = ((m_obj_static_wb[m_current_wb_index].gp_fire_masks[2] & 0x01) != 0);
	m_gp17 = ((m_obj_static_wb[m_current_wb_index].gp_fire_masks[1] & 0x01 << 7) != 0);
	m_gp16 = ((m_obj_static_wb[m_current_wb_index].gp_fire_masks[1] & 0x01 << 6) != 0);
	m_gp15 = ((m_obj_static_wb[m_current_wb_index].gp_fire_masks[1] & 0x01 << 5) != 0);
	m_gp14 = ((m_obj_static_wb[m_current_wb_index].gp_fire_masks[1] & 0x01 << 4) != 0);
	m_gp13 = ((m_obj_static_wb[m_current_wb_index].gp_fire_masks[1] & 0x01 << 3) != 0);
	m_gp12 = ((m_obj_static_wb[m_current_wb_index].gp_fire_masks[1] & 0x01 << 2) != 0);
	m_gp11 = ((m_obj_static_wb[m_current_wb_index].gp_fire_masks[1] & 0x01 << 1) != 0);
	m_gp10 = ((m_obj_static_wb[m_current_wb_index].gp_fire_masks[1] & 0x01) != 0);
	m_gp07 = ((m_obj_static_wb[m_current_wb_index].gp_fire_masks[0] & 0x01 << 7) != 0);
	m_gp06 = ((m_obj_static_wb[m_current_wb_index].gp_fire_masks[0] & 0x01 << 6) != 0);
	m_gp05 = ((m_obj_static_wb[m_current_wb_index].gp_fire_masks[0] & 0x01 << 5) != 0);
	m_gp04 = ((m_obj_static_wb[m_current_wb_index].gp_fire_masks[0] & 0x01 << 4) != 0);
	m_gp03 = ((m_obj_static_wb[m_current_wb_index].gp_fire_masks[0] & 0x01 << 3) != 0);
	m_gp02 = ((m_obj_static_wb[m_current_wb_index].gp_fire_masks[0] & 0x01 << 2) != 0);
	m_gp01 = ((m_obj_static_wb[m_current_wb_index].gp_fire_masks[0] & 0x01 << 1) != 0);
	m_gp00 = ((m_obj_static_wb[m_current_wb_index].gp_fire_masks[0] & 0x01) != 0);

	m_gpq7 = ((m_obj_static_wb[m_current_wb_index].gp_quad_fire_mask & 0x01 << 7) != 0);
	m_gpq6 = ((m_obj_static_wb[m_current_wb_index].gp_quad_fire_mask & 0x01 << 6) != 0);
	m_gpq5 = ((m_obj_static_wb[m_current_wb_index].gp_quad_fire_mask & 0x01 << 5) != 0);
	m_gpq4 = ((m_obj_static_wb[m_current_wb_index].gp_quad_fire_mask & 0x01 << 4) != 0);
	m_gpq3 = ((m_obj_static_wb[m_current_wb_index].gp_quad_fire_mask & 0x01 << 3) != 0);
	m_gpq2 = ((m_obj_static_wb[m_current_wb_index].gp_quad_fire_mask & 0x01 << 2) != 0);
	m_gpq1 = ((m_obj_static_wb[m_current_wb_index].gp_quad_fire_mask & 0x01 << 1) != 0);
	m_gpq0 = ((m_obj_static_wb[m_current_wb_index].gp_quad_fire_mask & 0x01) != 0 );

	m_fire0_cycle_time = m_obj_static_wb[m_current_wb_index].anim_time[0];
	m_fire1_cycle_time = m_obj_static_wb[m_current_wb_index].anim_time[1];
	m_fire2_cycle_time = m_obj_static_wb[m_current_wb_index].anim_time[2];
	m_fire3_cycle_time = m_obj_static_wb[m_current_wb_index].anim_time[3];
	m_fire4_cycle_time = m_obj_static_wb[m_current_wb_index].anim_time[4];
	m_fire5_cycle_time = m_obj_static_wb[m_current_wb_index].anim_time[5];
	m_fire6_cycle_time = m_obj_static_wb[m_current_wb_index].anim_time[6];
	m_fire7_cycle_time = m_obj_static_wb[m_current_wb_index].anim_time[7];
	m_fire0_end_tick = m_obj_static_wb[m_current_wb_index].anim_end_frame[0];
	m_fire1_end_tick = m_obj_static_wb[m_current_wb_index].anim_end_frame[1];
	m_fire2_end_tick = m_obj_static_wb[m_current_wb_index].anim_end_frame[2];
	m_fire3_end_tick = m_obj_static_wb[m_current_wb_index].anim_end_frame[3];
	m_fire4_end_tick = m_obj_static_wb[m_current_wb_index].anim_end_frame[4];
	m_fire5_end_tick = m_obj_static_wb[m_current_wb_index].anim_end_frame[5];
	m_fire6_end_tick = m_obj_static_wb[m_current_wb_index].anim_end_frame[6];
	m_fire7_end_tick = m_obj_static_wb[m_current_wb_index].anim_end_frame[7];
	m_fire0_fire_tick = m_obj_static_wb[m_current_wb_index].anim_fire_frame[0];
	m_fire1_fire_tick = m_obj_static_wb[m_current_wb_index].anim_fire_frame[1];
	m_fire2_fire_tick = m_obj_static_wb[m_current_wb_index].anim_fire_frame[2];
	m_fire3_fire_tick = m_obj_static_wb[m_current_wb_index].anim_fire_frame[3];
	m_fire4_fire_tick = m_obj_static_wb[m_current_wb_index].anim_fire_frame[4];
	m_fire5_fire_tick = m_obj_static_wb[m_current_wb_index].anim_fire_frame[5];
	m_fire6_fire_tick = m_obj_static_wb[m_current_wb_index].anim_fire_frame[6];
	m_fire7_fire_tick = m_obj_static_wb[m_current_wb_index].anim_fire_frame[7];
	m_fire0_start_tick = m_obj_static_wb[m_current_wb_index].anim_start_frame[0];
	m_fire1_start_tick = m_obj_static_wb[m_current_wb_index].anim_start_frame[1];
	m_fire2_start_tick = m_obj_static_wb[m_current_wb_index].anim_start_frame[2];
	m_fire3_start_tick = m_obj_static_wb[m_current_wb_index].anim_start_frame[3];
	m_fire4_start_tick = m_obj_static_wb[m_current_wb_index].anim_start_frame[4];
	m_fire5_start_tick = m_obj_static_wb[m_current_wb_index].anim_start_frame[5];
	m_fire6_start_tick = m_obj_static_wb[m_current_wb_index].anim_start_frame[6];
	m_fire7_start_tick = m_obj_static_wb[m_current_wb_index].anim_start_frame[7];
	
	m_spray_check = ((m_obj_static_wb[m_current_wb_index].flags & WBF_SPRAY) != 0);
	m_guided = ((m_obj_static_wb[m_current_wb_index].flags & WBF_GUIDED) != 0);
	m_user_timeout = ((m_obj_static_wb[m_current_wb_index].flags & WBF_USER_TIMEOUT) != 0);
	m_random_check = ((m_obj_static_wb[m_current_wb_index].flags & WBF_RANDOM_FIRE_ORDER) != 0);
	m_force_fvec_check = ((m_obj_static_wb[m_current_wb_index].flags & WBF_FIRE_FVEC) != 0);
	m_force_target_check = ((m_obj_static_wb[m_current_wb_index].flags & WBF_FIRE_TARGET) != 0);
	m_aim_fvec_check  = ((m_obj_static_wb[m_current_wb_index].flags & WBF_AIM_FVEC) != 0);
	m_onoff_check = ((m_obj_static_wb[m_current_wb_index].flags & WBF_ON_OFF) != 0);
	
	m_custom_view_cone_check = ((m_obj_static_wb[m_current_wb_index].flags & WBF_USE_CUSTOM_FOV) != 0);
	m_custom_max_dist = ((m_obj_static_wb[m_current_wb_index].flags & WBF_USE_CUSTOM_MAX_DIST) != 0);
}

RobotEditWeaponsDialog::RobotEditWeaponsDialog(otype_wb_info static_wb[] ,poly_model *pm, CWnd* pParent /*=NULL*/)
	: CDialog(RobotEditWeaponsDialog::IDD, pParent)
{
//	CEdit *ebox;
	char str[200];

	m_obj_static_wb = static_wb;
	m_obj_pm = pm;

	//{{AFX_DATA_INIT(RobotEditWeaponsDialog)
	m_weapon_0_text = _T("");
	m_weapon_1_text = _T("");
	m_weapon_2_text = _T("");
	m_weapon_4_text = _T("");
	m_weapon_3_text = _T("");
	m_weapon_5_text = _T("");
	m_weapon_6_text = _T("");
	m_weapon_7_text = _T("");
	m_fire_sound_0_text = _T("");
	m_fire_sound_1_text = _T("");
	m_fire_sound_2_text = _T("");
	m_fire_sound_3_text = _T("");
	m_fire_sound_4_text = _T("");
	m_fire_sound_5_text = _T("");
	m_fire_sound_6_text = _T("");
	m_fire_sound_7_text = _T("");
	m_energy_usage = 0.0f;
	m_ammo_usage = 0.0f;
	m_spray_check = FALSE;
	m_random_check = FALSE;
	m_guided = FALSE;
	m_gpq0 = FALSE;
	m_gpq1 = FALSE;
	m_gpq2 = FALSE;
	m_gpq3 = FALSE;
	m_gpq4 = FALSE;
	m_gpq5 = FALSE;
	m_gpq6 = FALSE;
	m_gpq7 = FALSE;
	m_custom_view_cone_check = FALSE;
	m_onoff_check = FALSE;
	m_custom_max_dist = FALSE;
	m_user_timeout = FALSE;
	m_force_fvec_check = FALSE;
	m_aim_fvec_check = FALSE;
	m_force_target_check = FALSE;
	//}}AFX_DATA_INIT

	m_current_wb_index = 0;

	sprintf(str, "Total Gun Batteries: %d", m_obj_pm->num_wbs);
	m_total_gb_text = str;

	sprintf(str, "Total Gun Points: %d", m_obj_pm->n_guns);
	m_total_gp_text = str;

	NewWBSelected();
}


void RobotEditWeaponsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(RobotEditWeaponsDialog)
	DDX_CBString(pDX, IDC_CURRENT_GB_COMBO, m_current_gb);
	DDX_Text(pDX, IDC_GB_MASK_LATENCY_0_EDIT, m_mask_latency_0);
	DDV_MinMaxFloat(pDX, m_mask_latency_0, 0.0f, 100000.0f);
	DDX_Text(pDX, IDC_GB_MASK_LATENCY_1_EDIT, m_mask_latency_1);
	DDV_MinMaxFloat(pDX, m_mask_latency_1, 0.0f, 100000.0f);
	DDX_Text(pDX, IDC_GB_MASK_LATENCY_2_EDIT, m_mask_latency_2);
	DDV_MinMaxFloat(pDX, m_mask_latency_2, 0.0f, 100000.0f);
	DDX_Text(pDX, IDC_GB_MASK_LATENCY_3_EDIT, m_mask_latency_3);
	DDV_MinMaxFloat(pDX, m_mask_latency_3, 0.0f, 100000.0f);
	DDX_Text(pDX, IDC_GB_MASK_LATENCY_4_EDIT, m_mask_latency_4);
	DDV_MinMaxFloat(pDX, m_mask_latency_4, 0.0f, 100000.0f);
	DDX_Text(pDX, IDC_GB_MASK_LATENCY_5_EDIT, m_mask_latency_5);
	DDV_MinMaxFloat(pDX, m_mask_latency_5, 0.0f, 100000.0f);
	DDX_Text(pDX, IDC_GB_MASK_LATENCY_6_EDIT, m_mask_latency_6);
	DDV_MinMaxFloat(pDX, m_mask_latency_6, 0.0f, 100000.0f);
	DDX_Text(pDX, IDC_GB_MASK_LATENCY_7_EDIT, m_mask_latency_7);
	DDV_MinMaxFloat(pDX, m_mask_latency_7, 0.0f, 100000.0f);
	DDX_Text(pDX, IDC_GB_NUM_MASKS_EDIT, m_num_masks);
	DDV_MinMaxUInt(pDX, m_num_masks, 1, 8);
	DDX_Text(pDX, IDC_TOTAL_GB_STATIC, m_total_gb_text);
	DDX_Text(pDX, IDC_TOTAL_GP_STATIC, m_total_gp_text);
	DDX_Text(pDX, IDC_NUM_GB_TURRETS_STATIC, m_num_turrets_text);
	DDX_Check(pDX, IDC_GP_MASK_7_7, m_gp77);
	DDX_Check(pDX, IDC_GP_MASK_7_6, m_gp76);
	DDX_Check(pDX, IDC_GP_MASK_7_5, m_gp75);
	DDX_Check(pDX, IDC_GP_MASK_7_4, m_gp74);
	DDX_Check(pDX, IDC_GP_MASK_7_3, m_gp73);
	DDX_Check(pDX, IDC_GP_MASK_7_2, m_gp72);
	DDX_Check(pDX, IDC_GP_MASK_7_1, m_gp71);
	DDX_Check(pDX, IDC_GP_MASK_7_0, m_gp70);
	DDX_Check(pDX, IDC_GP_MASK_6_7, m_gp67);
	DDX_Check(pDX, IDC_GP_MASK_6_6, m_gp66);
	DDX_Check(pDX, IDC_GP_MASK_6_5, m_gp65);
	DDX_Check(pDX, IDC_GP_MASK_6_4, m_gp64);
	DDX_Check(pDX, IDC_GP_MASK_6_3, m_gp63);
	DDX_Check(pDX, IDC_GP_MASK_6_2, m_gp62);
	DDX_Check(pDX, IDC_GP_MASK_6_1, m_gp61);
	DDX_Check(pDX, IDC_GP_MASK_6_0, m_gp60);
	DDX_Check(pDX, IDC_GP_MASK_5_6, m_gp56);
	DDX_Check(pDX, IDC_GP_MASK_5_7, m_gp57);
	DDX_Check(pDX, IDC_GP_MASK_5_5, m_gp55);
	DDX_Check(pDX, IDC_GP_MASK_5_4, m_gp54);
	DDX_Check(pDX, IDC_GP_MASK_5_3, m_gp53);
	DDX_Check(pDX, IDC_GP_MASK_5_2, m_gp52);
	DDX_Check(pDX, IDC_GP_MASK_5_1, m_gp51);
	DDX_Check(pDX, IDC_GP_MASK_5_0, m_gp50);
	DDX_Check(pDX, IDC_GP_MASK_4_7, m_gp47);
	DDX_Check(pDX, IDC_GP_MASK_4_6, m_gp46);
	DDX_Check(pDX, IDC_GP_MASK_4_5, m_gp45);
	DDX_Check(pDX, IDC_GP_MASK_4_4, m_gp44);
	DDX_Check(pDX, IDC_GP_MASK_4_3, m_gp43);
	DDX_Check(pDX, IDC_GP_MASK_4_2, m_gp42);
	DDX_Check(pDX, IDC_GP_MASK_4_1, m_gp41);
	DDX_Check(pDX, IDC_GP_MASK_4_0, m_gp40);
	DDX_Check(pDX, IDC_GP_MASK_3_7, m_gp37);
	DDX_Check(pDX, IDC_GP_MASK_3_6, m_gp36);
	DDX_Check(pDX, IDC_GP_MASK_3_5, m_gp35);
	DDX_Check(pDX, IDC_GP_MASK_3_4, m_gp34);
	DDX_Check(pDX, IDC_GP_MASK_3_3, m_gp33);
	DDX_Check(pDX, IDC_GP_MASK_3_2, m_gp32);
	DDX_Check(pDX, IDC_GP_MASK_3_1, m_gp31);
	DDX_Check(pDX, IDC_GP_MASK_3_0, m_gp30);
	DDX_Check(pDX, IDC_GP_MASK_2_7, m_gp27);
	DDX_Check(pDX, IDC_GP_MASK_2_6, m_gp26);
	DDX_Check(pDX, IDC_GP_MASK_2_5, m_gp25);
	DDX_Check(pDX, IDC_GP_MASK_2_4, m_gp24);
	DDX_Check(pDX, IDC_GP_MASK_2_3, m_gp23);
	DDX_Check(pDX, IDC_GP_MASK_2_2, m_gp22);
	DDX_Check(pDX, IDC_GP_MASK_2_1, m_gp21);
	DDX_Check(pDX, IDC_GP_MASK_2_0, m_gp20);
	DDX_Check(pDX, IDC_GP_MASK_1_7, m_gp17);
	DDX_Check(pDX, IDC_GP_MASK_1_6, m_gp16);
	DDX_Check(pDX, IDC_GP_MASK_1_5, m_gp15);
	DDX_Check(pDX, IDC_GP_MASK_1_4, m_gp14);
	DDX_Check(pDX, IDC_GP_MASK_1_3, m_gp13);
	DDX_Check(pDX, IDC_GP_MASK_1_2, m_gp12);
	DDX_Check(pDX, IDC_GP_MASK_1_1, m_gp11);
	DDX_Check(pDX, IDC_GP_MASK_1_0, m_gp10);
	DDX_Check(pDX, IDC_GP_MASK_0_7, m_gp07);
	DDX_Check(pDX, IDC_GP_MASK_0_6, m_gp06);
	DDX_Check(pDX, IDC_GP_MASK_0_5, m_gp05);
	DDX_Check(pDX, IDC_GP_MASK_0_4, m_gp04);
	DDX_Check(pDX, IDC_GP_MASK_0_3, m_gp03);
	DDX_Check(pDX, IDC_GP_MASK_0_2, m_gp02);
	DDX_Check(pDX, IDC_GP_MASK_0_1, m_gp01);
	DDX_Check(pDX, IDC_GP_MASK_0_0, m_gp00);
	DDX_Text(pDX, IDC_TURRET_REACTION_TIME_STATIC, m_turret_reaction_time_text);
	DDX_Text(pDX, IDC_TURRET_FOV_STATIC, m_turret_fov_text);
	DDX_Text(pDX, IDC_TURRET_SPR_STATIC, m_turret_spr_text);
	DDX_CBString(pDX, IDC_CURRET_TURRET_COMBO, m_current_turret_combo);
	DDX_Text(pDX, IDC_TURRET_SOBJ_STATIC, m_turret_sobj_text);
	DDX_Text(pDX, IDC_VIEW_CONE_ANGLE_EDIT, m_view_cone_angle);
	DDV_MinMaxFloat(pDX, m_view_cone_angle, 1.f, 359.f);
	DDX_Text(pDX, IDC_MAX_DISTANCE_EDIT, m_max_distance);
	DDV_MinMaxFloat(pDX, m_max_distance, 1.f, 1.e+007f);
	DDX_Text(pDX, IDC_XZ_PLANE_ANGLE_EDIT, m_xz_plane_angle);
	DDV_MinMaxFloat(pDX, m_xz_plane_angle, 1.f, 359.f);
	DDX_CBString(pDX, IDC_BARREL_0_COMBO, m_weapon_0_text);
	DDX_CBString(pDX, IDC_BARREL_1_COMBO, m_weapon_1_text);
	DDX_CBString(pDX, IDC_BARREL_2_COMBO, m_weapon_2_text);
	DDX_CBString(pDX, IDC_BARREL_4_COMBO, m_weapon_4_text);
	DDX_CBString(pDX, IDC_BARREL_3_COMBO, m_weapon_3_text);
	DDX_CBString(pDX, IDC_BARREL_5_COMBO, m_weapon_5_text);
	DDX_CBString(pDX, IDC_BARREL_6_COMBO, m_weapon_6_text);
	DDX_CBString(pDX, IDC_BARREL_7_COMBO, m_weapon_7_text);
	DDX_CBString(pDX, IDC_WB_FIRE0_SOUND_PULLDOWN, m_fire_sound_0_text);
	DDX_CBString(pDX, IDC_WB_FIRE1_SOUND_PULLDOWN, m_fire_sound_1_text);
	DDX_CBString(pDX, IDC_WB_FIRE2_SOUND_PULLDOWN, m_fire_sound_2_text);
	DDX_CBString(pDX, IDC_WB_FIRE3_SOUND_PULLDOWN, m_fire_sound_3_text);
	DDX_CBString(pDX, IDC_WB_FIRE4_SOUND_PULLDOWN, m_fire_sound_4_text);
	DDX_CBString(pDX, IDC_WB_FIRE5_SOUND_PULLDOWN, m_fire_sound_5_text);
	DDX_CBString(pDX, IDC_WB_FIRE6_SOUND_PULLDOWN, m_fire_sound_6_text);
	DDX_CBString(pDX, IDC_WB_FIRE7_SOUND_PULLDOWN, m_fire_sound_7_text);
	DDX_Text(pDX, IDC_ENERGY_USAGE_EDIT, m_energy_usage);
	DDV_MinMaxFloat(pDX, m_energy_usage, 0.0f, 10000.f);
	DDX_Text(pDX, IDC_AMMO_USAGE_EDIT, m_ammo_usage);
	DDV_MinMaxFloat(pDX, m_ammo_usage, 0.f, 10000.f);
	DDX_Check(pDX, IDC_SPRAY_CHECK, m_spray_check);
	DDX_Text(pDX, IDC_WB_FIRE0_CYCLE_TIME, m_fire0_cycle_time);
	DDX_Text(pDX, IDC_WB_FIRE1_CYCLE_TIME, m_fire1_cycle_time);
	DDX_Text(pDX, IDC_WB_FIRE2_CYCLE_TIME, m_fire2_cycle_time);
	DDX_Text(pDX, IDC_WB_FIRE3_CYCLE_TIME, m_fire3_cycle_time);
	DDX_Text(pDX, IDC_WB_FIRE4_CYCLE_TIME, m_fire4_cycle_time);
	DDX_Text(pDX, IDC_WB_FIRE5_CYCLE_TIME, m_fire5_cycle_time);
	DDX_Text(pDX, IDC_WB_FIRE6_CYCLE_TIME, m_fire6_cycle_time);
	DDX_Text(pDX, IDC_WB_FIRE7_CYCLE_TIME, m_fire7_cycle_time);
	DDX_Text(pDX, IDC_WB_FIRE0_END_TICK, m_fire0_end_tick);
	DDX_Text(pDX, IDC_WB_FIRE1_END_TICK, m_fire1_end_tick);
	DDX_Text(pDX, IDC_WB_FIRE2_END_TICK, m_fire2_end_tick);
	DDX_Text(pDX, IDC_WB_FIRE3_END_TICK, m_fire3_end_tick);
	DDX_Text(pDX, IDC_WB_FIRE4_END_TICK, m_fire4_end_tick);
	DDX_Text(pDX, IDC_WB_FIRE5_END_TICK, m_fire5_end_tick);
	DDX_Text(pDX, IDC_WB_FIRE6_END_TICK, m_fire6_end_tick);
	DDX_Text(pDX, IDC_WB_FIRE7_END_TICK, m_fire7_end_tick);
	DDX_Text(pDX, IDC_WB_FIRE0_FIRE_TICK, m_fire0_fire_tick);
	DDX_Text(pDX, IDC_WB_FIRE1_FIRE_TICK, m_fire1_fire_tick);
	DDX_Text(pDX, IDC_WB_FIRE2_FIRE_TICK, m_fire2_fire_tick);
	DDX_Text(pDX, IDC_WB_FIRE3_FIRE_TICK, m_fire3_fire_tick);
	DDX_Text(pDX, IDC_WB_FIRE4_FIRE_TICK, m_fire4_fire_tick);
	DDX_Text(pDX, IDC_WB_FIRE5_FIRE_TICK, m_fire5_fire_tick);
	DDX_Text(pDX, IDC_WB_FIRE6_FIRE_TICK, m_fire6_fire_tick);
	DDX_Text(pDX, IDC_WB_FIRE7_FIRE_TICK, m_fire7_fire_tick);
	DDX_Text(pDX, IDC_WB_FIRE0_START_TICK, m_fire0_start_tick);
	DDX_Text(pDX, IDC_WB_FIRE1_START_TICK, m_fire1_start_tick);
	DDX_Text(pDX, IDC_WB_FIRE2_START_TICK, m_fire2_start_tick);
	DDX_Text(pDX, IDC_WB_FIRE3_START_TICK, m_fire3_start_tick);
	DDX_Text(pDX, IDC_WB_FIRE4_START_TICK, m_fire4_start_tick);
	DDX_Text(pDX, IDC_WB_FIRE5_START_TICK, m_fire5_start_tick);
	DDX_Text(pDX, IDC_WB_FIRE6_START_TICK, m_fire6_start_tick);
	DDX_Text(pDX, IDC_WB_FIRE7_START_TICK, m_fire7_start_tick);
	DDX_Check(pDX, IDC_WB_RANDOM_CHECK, m_random_check);
	DDX_Check(pDX, IDC_GUIDED_CHECK, m_guided);
	DDX_Check(pDX, IDC_GP_MASK_Q_0, m_gpq0);
	DDX_Check(pDX, IDC_GP_MASK_Q_1, m_gpq1);
	DDX_Check(pDX, IDC_GP_MASK_Q_2, m_gpq2);
	DDX_Check(pDX, IDC_GP_MASK_Q_3, m_gpq3);
	DDX_Check(pDX, IDC_GP_MASK_Q_4, m_gpq4);
	DDX_Check(pDX, IDC_GP_MASK_Q_5, m_gpq5);
	DDX_Check(pDX, IDC_GP_MASK_Q_6, m_gpq6);
	DDX_Check(pDX, IDC_GP_MASK_Q_7, m_gpq7);
	DDX_Check(pDX, IDC_VIEW_CONE_ANGLE_CHECK, m_custom_view_cone_check);
	DDX_Check(pDX, IDC_ONOFF_CHECK, m_onoff_check);
	DDX_Check(pDX, IDC_MAX_DISTANCE_CHECK, m_custom_max_dist);
	DDX_Check(pDX, IDC_USER_TIMEOUT_CHECK, m_user_timeout);
	DDX_Check(pDX, IDC_FORCE_TO_FVEC_CHECK, m_force_fvec_check);
	DDX_Check(pDX, IDC_WBAIMFORWARD_CHECK, m_aim_fvec_check);
	DDX_Check(pDX, IDC_FORCE_TO_TARGET_CHECK, m_force_target_check);
	//}}AFX_DATA_MAP

	if(((CButton *)GetDlgItem(IDC_BARREL_0_RADIO))->GetCheck()) m_current_aiming_point = 0;
	if(((CButton *)GetDlgItem(IDC_BARREL_1_RADIO))->GetCheck()) m_current_aiming_point = 1;
	if(((CButton *)GetDlgItem(IDC_BARREL_2_RADIO))->GetCheck()) m_current_aiming_point = 2;
	if(((CButton *)GetDlgItem(IDC_BARREL_3_RADIO))->GetCheck()) m_current_aiming_point = 3;
	if(((CButton *)GetDlgItem(IDC_BARREL_4_RADIO))->GetCheck()) m_current_aiming_point = 4;
	if(((CButton *)GetDlgItem(IDC_BARREL_5_RADIO))->GetCheck()) m_current_aiming_point = 5;
	if(((CButton *)GetDlgItem(IDC_BARREL_6_RADIO))->GetCheck()) m_current_aiming_point = 6;
	if(((CButton *)GetDlgItem(IDC_BARREL_7_RADIO))->GetCheck()) m_current_aiming_point = 7;

	if(((CButton *)GetDlgItem(IDC_WB_NO_ANIM_RADIO))->GetCheck()) m_anim_type = 0;
	if(((CButton *)GetDlgItem(IDC_WB_LOCAL_ANIM_RADIO))->GetCheck()) m_anim_type = WBF_ANIM_LOCAL;
	if(((CButton *)GetDlgItem(IDC_WB_MODEL_ANIM_RADIO))->GetCheck()) m_anim_type = WBF_ANIM_FULL;
}


BEGIN_MESSAGE_MAP(RobotEditWeaponsDialog, CDialog)
	//{{AFX_MSG_MAP(RobotEditWeaponsDialog)
	ON_EN_KILLFOCUS(IDC_GB_NUM_MASKS_EDIT, OnKillfocusGbNumMasksEdit)
	ON_CBN_SELENDOK(IDC_CURRET_TURRET_COMBO, OnSelendokCurretTurretCombo)
	ON_CBN_SELENDOK(IDC_CURRENT_GB_COMBO, OnSelendokCurrentGbCombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// RobotEditWeaponsDialog message handlers


void RobotEditWeaponsDialog::UpdateDialog(void) 
{
	char str[200];
	
	if(!UpdateData(true)) return;	

	if(m_obj_pm->poly_wb[m_current_wb_index].num_turrets)
	{
		sscanf(m_current_turret_combo.GetBuffer(0), "%d", &m_current_turret_index);
		m_current_turret_index--;  // Accounts for base 1 to base 0 array conversion
	}
	else
		m_current_turret_index = 0;

	UpdateTurretInfo();
	UpdateData(false);

	if(m_obj_pm->poly_wb[m_current_wb_index].num_gps > 0)
		sprintf(str, "Gun Point %d", m_obj_pm->poly_wb[m_current_wb_index].gp_index[0]);
	else
		strcpy(str, "NA");
	((CButton *)GetDlgItem(IDC_BARREL_0_RADIO))->SetWindowText(str);

	if(m_obj_pm->poly_wb[m_current_wb_index].num_gps > 1)
		sprintf(str, "Gun Point %d", m_obj_pm->poly_wb[m_current_wb_index].gp_index[1]);
	else
		strcpy(str, "NA");
	((CButton *)GetDlgItem(IDC_BARREL_1_RADIO))->SetWindowText(str);

	if(m_obj_pm->poly_wb[m_current_wb_index].num_gps > 2)
		sprintf(str, "Gun Point %d", m_obj_pm->poly_wb[m_current_wb_index].gp_index[2]);
	else
		strcpy(str, "NA");
	((CButton *)GetDlgItem(IDC_BARREL_2_RADIO))->SetWindowText(str);

	if(m_obj_pm->poly_wb[m_current_wb_index].num_gps > 3)
		sprintf(str, "Gun Point %d", m_obj_pm->poly_wb[m_current_wb_index].gp_index[3]);
	else
		strcpy(str, "NA");
	((CButton *)GetDlgItem(IDC_BARREL_3_RADIO))->SetWindowText(str);

	if(m_obj_pm->poly_wb[m_current_wb_index].num_gps > 4)
		sprintf(str, "Gun Point %d", m_obj_pm->poly_wb[m_current_wb_index].gp_index[4]);
	else
		strcpy(str, "NA");
	((CButton *)GetDlgItem(IDC_BARREL_4_RADIO))->SetWindowText(str);

	if(m_obj_pm->poly_wb[m_current_wb_index].num_gps > 5)
		sprintf(str, "Gun Point %d", m_obj_pm->poly_wb[m_current_wb_index].gp_index[5]);
	else
		strcpy(str, "NA");
	((CButton *)GetDlgItem(IDC_BARREL_5_RADIO))->SetWindowText(str);

	if(m_obj_pm->poly_wb[m_current_wb_index].num_gps > 6)
		sprintf(str, "Gun Point %d", m_obj_pm->poly_wb[m_current_wb_index].gp_index[6]);
	else
		strcpy(str, "NA");
	((CButton *)GetDlgItem(IDC_BARREL_6_RADIO))->SetWindowText(str);

	if(m_obj_pm->poly_wb[m_current_wb_index].num_gps > 7)
		sprintf(str, "Gun Point %d", m_obj_pm->poly_wb[m_current_wb_index].gp_index[7]);
	else
		strcpy(str, "NA");
	((CButton *)GetDlgItem(IDC_BARREL_7_RADIO))->SetWindowText(str);

	((CButton *)GetDlgItem(IDC_GP_MASK_0_0))->EnableWindow(m_num_masks > 0 && m_obj_pm->poly_wb[m_current_wb_index].num_gps > 0);
	((CButton *)GetDlgItem(IDC_GP_MASK_0_1))->EnableWindow(m_num_masks > 0 && m_obj_pm->poly_wb[m_current_wb_index].num_gps > 1);
	((CButton *)GetDlgItem(IDC_GP_MASK_0_2))->EnableWindow(m_num_masks > 0 && m_obj_pm->poly_wb[m_current_wb_index].num_gps > 2);
	((CButton *)GetDlgItem(IDC_GP_MASK_0_3))->EnableWindow(m_num_masks > 0 && m_obj_pm->poly_wb[m_current_wb_index].num_gps > 3);
	((CButton *)GetDlgItem(IDC_GP_MASK_0_4))->EnableWindow(m_num_masks > 0 && m_obj_pm->poly_wb[m_current_wb_index].num_gps > 4);
	((CButton *)GetDlgItem(IDC_GP_MASK_0_5))->EnableWindow(m_num_masks > 0 && m_obj_pm->poly_wb[m_current_wb_index].num_gps > 5);
	((CButton *)GetDlgItem(IDC_GP_MASK_0_6))->EnableWindow(m_num_masks > 0 && m_obj_pm->poly_wb[m_current_wb_index].num_gps > 6);
	((CButton *)GetDlgItem(IDC_GP_MASK_0_7))->EnableWindow(m_num_masks > 0 && m_obj_pm->poly_wb[m_current_wb_index].num_gps > 7);
	((CButton *)GetDlgItem(IDC_GP_MASK_1_0))->EnableWindow(m_num_masks > 1 && m_obj_pm->poly_wb[m_current_wb_index].num_gps > 0);
	((CButton *)GetDlgItem(IDC_GP_MASK_1_1))->EnableWindow(m_num_masks > 1 && m_obj_pm->poly_wb[m_current_wb_index].num_gps > 1);
	((CButton *)GetDlgItem(IDC_GP_MASK_1_2))->EnableWindow(m_num_masks > 1 && m_obj_pm->poly_wb[m_current_wb_index].num_gps > 2);
	((CButton *)GetDlgItem(IDC_GP_MASK_1_3))->EnableWindow(m_num_masks > 1 && m_obj_pm->poly_wb[m_current_wb_index].num_gps > 3);
	((CButton *)GetDlgItem(IDC_GP_MASK_1_4))->EnableWindow(m_num_masks > 1 && m_obj_pm->poly_wb[m_current_wb_index].num_gps > 4);
	((CButton *)GetDlgItem(IDC_GP_MASK_1_5))->EnableWindow(m_num_masks > 1 && m_obj_pm->poly_wb[m_current_wb_index].num_gps > 5);
	((CButton *)GetDlgItem(IDC_GP_MASK_1_6))->EnableWindow(m_num_masks > 1 && m_obj_pm->poly_wb[m_current_wb_index].num_gps > 6);
	((CButton *)GetDlgItem(IDC_GP_MASK_1_7))->EnableWindow(m_num_masks > 1 && m_obj_pm->poly_wb[m_current_wb_index].num_gps > 7);
	((CButton *)GetDlgItem(IDC_GP_MASK_2_0))->EnableWindow(m_num_masks > 2 && m_obj_pm->poly_wb[m_current_wb_index].num_gps > 0);
	((CButton *)GetDlgItem(IDC_GP_MASK_2_1))->EnableWindow(m_num_masks > 2 && m_obj_pm->poly_wb[m_current_wb_index].num_gps > 1);
	((CButton *)GetDlgItem(IDC_GP_MASK_2_2))->EnableWindow(m_num_masks > 2 && m_obj_pm->poly_wb[m_current_wb_index].num_gps > 2);
	((CButton *)GetDlgItem(IDC_GP_MASK_2_3))->EnableWindow(m_num_masks > 2 && m_obj_pm->poly_wb[m_current_wb_index].num_gps > 3);
	((CButton *)GetDlgItem(IDC_GP_MASK_2_4))->EnableWindow(m_num_masks > 2 && m_obj_pm->poly_wb[m_current_wb_index].num_gps > 4);
	((CButton *)GetDlgItem(IDC_GP_MASK_2_5))->EnableWindow(m_num_masks > 2 && m_obj_pm->poly_wb[m_current_wb_index].num_gps > 5);
	((CButton *)GetDlgItem(IDC_GP_MASK_2_6))->EnableWindow(m_num_masks > 2 && m_obj_pm->poly_wb[m_current_wb_index].num_gps > 6);
	((CButton *)GetDlgItem(IDC_GP_MASK_2_7))->EnableWindow(m_num_masks > 2 && m_obj_pm->poly_wb[m_current_wb_index].num_gps > 7);
	((CButton *)GetDlgItem(IDC_GP_MASK_3_0))->EnableWindow(m_num_masks > 3 && m_obj_pm->poly_wb[m_current_wb_index].num_gps > 0);
	((CButton *)GetDlgItem(IDC_GP_MASK_3_1))->EnableWindow(m_num_masks > 3 && m_obj_pm->poly_wb[m_current_wb_index].num_gps > 1);
	((CButton *)GetDlgItem(IDC_GP_MASK_3_2))->EnableWindow(m_num_masks > 3 && m_obj_pm->poly_wb[m_current_wb_index].num_gps > 2);
	((CButton *)GetDlgItem(IDC_GP_MASK_3_3))->EnableWindow(m_num_masks > 3 && m_obj_pm->poly_wb[m_current_wb_index].num_gps > 3);
	((CButton *)GetDlgItem(IDC_GP_MASK_3_4))->EnableWindow(m_num_masks > 3 && m_obj_pm->poly_wb[m_current_wb_index].num_gps > 4);
	((CButton *)GetDlgItem(IDC_GP_MASK_3_5))->EnableWindow(m_num_masks > 3 && m_obj_pm->poly_wb[m_current_wb_index].num_gps > 5);
	((CButton *)GetDlgItem(IDC_GP_MASK_3_6))->EnableWindow(m_num_masks > 3 && m_obj_pm->poly_wb[m_current_wb_index].num_gps > 6);
	((CButton *)GetDlgItem(IDC_GP_MASK_3_7))->EnableWindow(m_num_masks > 3 && m_obj_pm->poly_wb[m_current_wb_index].num_gps > 7);
	((CButton *)GetDlgItem(IDC_GP_MASK_4_0))->EnableWindow(m_num_masks > 4 && m_obj_pm->poly_wb[m_current_wb_index].num_gps > 0);
	((CButton *)GetDlgItem(IDC_GP_MASK_4_1))->EnableWindow(m_num_masks > 4 && m_obj_pm->poly_wb[m_current_wb_index].num_gps > 1);
	((CButton *)GetDlgItem(IDC_GP_MASK_4_2))->EnableWindow(m_num_masks > 4 && m_obj_pm->poly_wb[m_current_wb_index].num_gps > 2);
	((CButton *)GetDlgItem(IDC_GP_MASK_4_3))->EnableWindow(m_num_masks > 4 && m_obj_pm->poly_wb[m_current_wb_index].num_gps > 3);
	((CButton *)GetDlgItem(IDC_GP_MASK_4_4))->EnableWindow(m_num_masks > 4 && m_obj_pm->poly_wb[m_current_wb_index].num_gps > 4);
	((CButton *)GetDlgItem(IDC_GP_MASK_4_5))->EnableWindow(m_num_masks > 4 && m_obj_pm->poly_wb[m_current_wb_index].num_gps > 5);
	((CButton *)GetDlgItem(IDC_GP_MASK_4_6))->EnableWindow(m_num_masks > 4 && m_obj_pm->poly_wb[m_current_wb_index].num_gps > 6);
	((CButton *)GetDlgItem(IDC_GP_MASK_4_7))->EnableWindow(m_num_masks > 4 && m_obj_pm->poly_wb[m_current_wb_index].num_gps > 7);
	((CButton *)GetDlgItem(IDC_GP_MASK_5_0))->EnableWindow(m_num_masks > 5 && m_obj_pm->poly_wb[m_current_wb_index].num_gps > 0);
	((CButton *)GetDlgItem(IDC_GP_MASK_5_1))->EnableWindow(m_num_masks > 5 && m_obj_pm->poly_wb[m_current_wb_index].num_gps > 1);
	((CButton *)GetDlgItem(IDC_GP_MASK_5_2))->EnableWindow(m_num_masks > 5 && m_obj_pm->poly_wb[m_current_wb_index].num_gps > 2);
	((CButton *)GetDlgItem(IDC_GP_MASK_5_3))->EnableWindow(m_num_masks > 5 && m_obj_pm->poly_wb[m_current_wb_index].num_gps > 3);
	((CButton *)GetDlgItem(IDC_GP_MASK_5_4))->EnableWindow(m_num_masks > 5 && m_obj_pm->poly_wb[m_current_wb_index].num_gps > 4);
	((CButton *)GetDlgItem(IDC_GP_MASK_5_5))->EnableWindow(m_num_masks > 5 && m_obj_pm->poly_wb[m_current_wb_index].num_gps > 5);
	((CButton *)GetDlgItem(IDC_GP_MASK_5_6))->EnableWindow(m_num_masks > 5 && m_obj_pm->poly_wb[m_current_wb_index].num_gps > 6);
	((CButton *)GetDlgItem(IDC_GP_MASK_5_7))->EnableWindow(m_num_masks > 5 && m_obj_pm->poly_wb[m_current_wb_index].num_gps > 7);
	((CButton *)GetDlgItem(IDC_GP_MASK_6_0))->EnableWindow(m_num_masks > 6 && m_obj_pm->poly_wb[m_current_wb_index].num_gps > 0);
	((CButton *)GetDlgItem(IDC_GP_MASK_6_1))->EnableWindow(m_num_masks > 6 && m_obj_pm->poly_wb[m_current_wb_index].num_gps > 1);
	((CButton *)GetDlgItem(IDC_GP_MASK_6_2))->EnableWindow(m_num_masks > 6 && m_obj_pm->poly_wb[m_current_wb_index].num_gps > 2);
	((CButton *)GetDlgItem(IDC_GP_MASK_6_3))->EnableWindow(m_num_masks > 6 && m_obj_pm->poly_wb[m_current_wb_index].num_gps > 3);
	((CButton *)GetDlgItem(IDC_GP_MASK_6_4))->EnableWindow(m_num_masks > 6 && m_obj_pm->poly_wb[m_current_wb_index].num_gps > 4);
	((CButton *)GetDlgItem(IDC_GP_MASK_6_5))->EnableWindow(m_num_masks > 6 && m_obj_pm->poly_wb[m_current_wb_index].num_gps > 5);
	((CButton *)GetDlgItem(IDC_GP_MASK_6_6))->EnableWindow(m_num_masks > 6 && m_obj_pm->poly_wb[m_current_wb_index].num_gps > 6);
	((CButton *)GetDlgItem(IDC_GP_MASK_6_7))->EnableWindow(m_num_masks > 6 && m_obj_pm->poly_wb[m_current_wb_index].num_gps > 7);
	((CButton *)GetDlgItem(IDC_GP_MASK_7_0))->EnableWindow(m_num_masks > 7 && m_obj_pm->poly_wb[m_current_wb_index].num_gps > 0);
	((CButton *)GetDlgItem(IDC_GP_MASK_7_1))->EnableWindow(m_num_masks > 7 && m_obj_pm->poly_wb[m_current_wb_index].num_gps > 1);
	((CButton *)GetDlgItem(IDC_GP_MASK_7_2))->EnableWindow(m_num_masks > 7 && m_obj_pm->poly_wb[m_current_wb_index].num_gps > 2);
	((CButton *)GetDlgItem(IDC_GP_MASK_7_3))->EnableWindow(m_num_masks > 7 && m_obj_pm->poly_wb[m_current_wb_index].num_gps > 3);
	((CButton *)GetDlgItem(IDC_GP_MASK_7_4))->EnableWindow(m_num_masks > 7 && m_obj_pm->poly_wb[m_current_wb_index].num_gps > 4);
	((CButton *)GetDlgItem(IDC_GP_MASK_7_5))->EnableWindow(m_num_masks > 7 && m_obj_pm->poly_wb[m_current_wb_index].num_gps > 5);
	((CButton *)GetDlgItem(IDC_GP_MASK_7_6))->EnableWindow(m_num_masks > 7 && m_obj_pm->poly_wb[m_current_wb_index].num_gps > 6);
	((CButton *)GetDlgItem(IDC_GP_MASK_7_7))->EnableWindow(m_num_masks > 7 && m_obj_pm->poly_wb[m_current_wb_index].num_gps > 7);

	((CButton *)GetDlgItem(IDC_GP_MASK_Q_0))->EnableWindow(m_obj_pm->poly_wb[m_current_wb_index].num_gps > 0);
	((CButton *)GetDlgItem(IDC_GP_MASK_Q_1))->EnableWindow(m_obj_pm->poly_wb[m_current_wb_index].num_gps > 1);
	((CButton *)GetDlgItem(IDC_GP_MASK_Q_2))->EnableWindow(m_obj_pm->poly_wb[m_current_wb_index].num_gps > 2);
	((CButton *)GetDlgItem(IDC_GP_MASK_Q_3))->EnableWindow(m_obj_pm->poly_wb[m_current_wb_index].num_gps > 3);
	((CButton *)GetDlgItem(IDC_GP_MASK_Q_4))->EnableWindow(m_obj_pm->poly_wb[m_current_wb_index].num_gps > 4);
	((CButton *)GetDlgItem(IDC_GP_MASK_Q_5))->EnableWindow(m_obj_pm->poly_wb[m_current_wb_index].num_gps > 5);
	((CButton *)GetDlgItem(IDC_GP_MASK_Q_6))->EnableWindow(m_obj_pm->poly_wb[m_current_wb_index].num_gps > 6);
	((CButton *)GetDlgItem(IDC_GP_MASK_Q_7))->EnableWindow(m_obj_pm->poly_wb[m_current_wb_index].num_gps > 7);

	((CEdit *)GetDlgItem(IDC_GB_MASK_LATENCY_0_EDIT))->EnableWindow(m_num_masks > 0);
	((CEdit *)GetDlgItem(IDC_GB_MASK_LATENCY_1_EDIT))->EnableWindow(m_num_masks > 1);
	((CEdit *)GetDlgItem(IDC_GB_MASK_LATENCY_2_EDIT))->EnableWindow(m_num_masks > 2);
	((CEdit *)GetDlgItem(IDC_GB_MASK_LATENCY_3_EDIT))->EnableWindow(m_num_masks > 3);
	((CEdit *)GetDlgItem(IDC_GB_MASK_LATENCY_4_EDIT))->EnableWindow(m_num_masks > 4);
	((CEdit *)GetDlgItem(IDC_GB_MASK_LATENCY_5_EDIT))->EnableWindow(m_num_masks > 5);
	((CEdit *)GetDlgItem(IDC_GB_MASK_LATENCY_6_EDIT))->EnableWindow(m_num_masks > 6);
	((CEdit *)GetDlgItem(IDC_GB_MASK_LATENCY_7_EDIT))->EnableWindow(m_num_masks > 7);

	((CComboBox *)GetDlgItem(IDC_BARREL_0_COMBO))->EnableWindow(m_obj_pm->poly_wb[m_current_wb_index].num_gps > 0);
	((CComboBox *)GetDlgItem(IDC_BARREL_1_COMBO))->EnableWindow(m_obj_pm->poly_wb[m_current_wb_index].num_gps > 1);
	((CComboBox *)GetDlgItem(IDC_BARREL_2_COMBO))->EnableWindow(m_obj_pm->poly_wb[m_current_wb_index].num_gps > 2);
	((CComboBox *)GetDlgItem(IDC_BARREL_3_COMBO))->EnableWindow(m_obj_pm->poly_wb[m_current_wb_index].num_gps > 3);
	((CComboBox *)GetDlgItem(IDC_BARREL_4_COMBO))->EnableWindow(m_obj_pm->poly_wb[m_current_wb_index].num_gps > 4);
	((CComboBox *)GetDlgItem(IDC_BARREL_5_COMBO))->EnableWindow(m_obj_pm->poly_wb[m_current_wb_index].num_gps > 5);
	((CComboBox *)GetDlgItem(IDC_BARREL_6_COMBO))->EnableWindow(m_obj_pm->poly_wb[m_current_wb_index].num_gps > 6);
	((CComboBox *)GetDlgItem(IDC_BARREL_7_COMBO))->EnableWindow(m_obj_pm->poly_wb[m_current_wb_index].num_gps > 7);

	((CButton *)GetDlgItem(IDC_BARREL_0_RADIO))->EnableWindow(m_obj_pm->poly_wb[m_current_wb_index].num_gps > 0);
	((CButton *)GetDlgItem(IDC_BARREL_1_RADIO))->EnableWindow(m_obj_pm->poly_wb[m_current_wb_index].num_gps > 1);
	((CButton *)GetDlgItem(IDC_BARREL_2_RADIO))->EnableWindow(m_obj_pm->poly_wb[m_current_wb_index].num_gps > 2);
	((CButton *)GetDlgItem(IDC_BARREL_3_RADIO))->EnableWindow(m_obj_pm->poly_wb[m_current_wb_index].num_gps > 3);
	((CButton *)GetDlgItem(IDC_BARREL_4_RADIO))->EnableWindow(m_obj_pm->poly_wb[m_current_wb_index].num_gps > 4);
	((CButton *)GetDlgItem(IDC_BARREL_5_RADIO))->EnableWindow(m_obj_pm->poly_wb[m_current_wb_index].num_gps > 5);
	((CButton *)GetDlgItem(IDC_BARREL_6_RADIO))->EnableWindow(m_obj_pm->poly_wb[m_current_wb_index].num_gps > 6);
	((CButton *)GetDlgItem(IDC_BARREL_7_RADIO))->EnableWindow(m_obj_pm->poly_wb[m_current_wb_index].num_gps > 7);

//	((CButton *)GetDlgItem(IDC_GP_MASK_7_7))->EnableWindow(m_num_masks > 7 && m_obj_pm->poly_wb[m_current_wb_index].num_gps > 7);
}

void RobotEditWeaponsDialog::UpdateTurretInfo() 
{
	char str[200];
	
	if(m_obj_pm->poly_wb[m_current_wb_index].num_turrets == 0)
	{
		m_turret_sobj_text = "0";
		m_turret_reaction_time_text = "0";
		m_turret_fov_text = "0";
		m_turret_spr_text = "0";

		return;
	}

	sprintf(str, "%d", m_obj_pm->poly_wb[m_current_wb_index].turret_index[m_current_turret_index]);
	m_turret_sobj_text = str;

	sprintf(str, "%.2f", m_obj_pm->submodel[m_obj_pm->poly_wb[m_current_wb_index].turret_index[m_current_turret_index]].think_interval);
	m_turret_reaction_time_text = str;
	
	sprintf(str, "%.2f", m_obj_pm->submodel[m_obj_pm->poly_wb[m_current_wb_index].turret_index[m_current_turret_index]].fov * 720.0);
	m_turret_fov_text = str;
	
	sprintf(str, "%.2f", (1.0f/m_obj_pm->submodel[m_obj_pm->poly_wb[m_current_wb_index].turret_index[m_current_turret_index]].rps));
	m_turret_spr_text = str;

}

BOOL RobotEditWeaponsDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	int i;
	char str[20];

	SendDlgItemMessage( IDC_BARREL_0_COMBO, CB_RESETCONTENT,0,0);
	for (i=0;i<MAX_WEAPONS;i++)
	{
		if (Weapons[i].used)
			SendDlgItemMessage( IDC_BARREL_0_COMBO, CB_ADDSTRING,0,(LPARAM) (LPCTSTR) Weapons[i].name);
	}
	SendDlgItemMessage( IDC_BARREL_0_COMBO, CB_SELECTSTRING,0,(LPARAM) (LPCTSTR) m_weapon_0_text);

	SendDlgItemMessage( IDC_BARREL_1_COMBO, CB_RESETCONTENT,0,0);
	for (i=0;i<MAX_WEAPONS;i++)
	{
		if (Weapons[i].used)
			SendDlgItemMessage( IDC_BARREL_1_COMBO, CB_ADDSTRING,0,(LPARAM) (LPCTSTR) Weapons[i].name);
	}
	SendDlgItemMessage( IDC_BARREL_1_COMBO, CB_SELECTSTRING,0,(LPARAM) (LPCTSTR) m_weapon_1_text);
	
	SendDlgItemMessage( IDC_BARREL_2_COMBO, CB_RESETCONTENT,0,0);
	for (i=0;i<MAX_WEAPONS;i++)
	{
		if (Weapons[i].used)
			SendDlgItemMessage( IDC_BARREL_2_COMBO, CB_ADDSTRING,0,(LPARAM) (LPCTSTR) Weapons[i].name);
	}
	SendDlgItemMessage( IDC_BARREL_2_COMBO, CB_SELECTSTRING,0,(LPARAM) (LPCTSTR) m_weapon_2_text);

	SendDlgItemMessage( IDC_BARREL_3_COMBO, CB_RESETCONTENT,0,0);
	for (i=0;i<MAX_WEAPONS;i++)
	{
		if (Weapons[i].used)
			SendDlgItemMessage( IDC_BARREL_3_COMBO, CB_ADDSTRING,0,(LPARAM) (LPCTSTR) Weapons[i].name);
	}
	SendDlgItemMessage( IDC_BARREL_3_COMBO, CB_SELECTSTRING,0,(LPARAM) (LPCTSTR) m_weapon_3_text);

	SendDlgItemMessage( IDC_BARREL_4_COMBO, CB_RESETCONTENT,0,0);
	for (i=0;i<MAX_WEAPONS;i++)
	{
		if (Weapons[i].used)
			SendDlgItemMessage( IDC_BARREL_4_COMBO, CB_ADDSTRING,0,(LPARAM) (LPCTSTR) Weapons[i].name);
	}
	SendDlgItemMessage( IDC_BARREL_4_COMBO, CB_SELECTSTRING,0,(LPARAM) (LPCTSTR) m_weapon_4_text);

		SendDlgItemMessage( IDC_BARREL_5_COMBO, CB_RESETCONTENT,0,0);
	for (i=0;i<MAX_WEAPONS;i++)
	{
		if (Weapons[i].used)
			SendDlgItemMessage( IDC_BARREL_5_COMBO, CB_ADDSTRING,0,(LPARAM) (LPCTSTR) Weapons[i].name);
	}
	SendDlgItemMessage( IDC_BARREL_5_COMBO, CB_SELECTSTRING,0,(LPARAM) (LPCTSTR) m_weapon_5_text);

	SendDlgItemMessage( IDC_BARREL_6_COMBO, CB_RESETCONTENT,0,0);
	for (i=0;i<MAX_WEAPONS;i++)
	{
		if (Weapons[i].used)
			SendDlgItemMessage( IDC_BARREL_6_COMBO, CB_ADDSTRING,0,(LPARAM) (LPCTSTR) Weapons[i].name);
	}
	SendDlgItemMessage( IDC_BARREL_6_COMBO, CB_SELECTSTRING,0,(LPARAM) (LPCTSTR) m_weapon_6_text);

	SendDlgItemMessage( IDC_BARREL_7_COMBO, CB_RESETCONTENT,0,0);
	for (i=0;i<MAX_WEAPONS;i++)
	{
		if (Weapons[i].used)
			SendDlgItemMessage( IDC_BARREL_7_COMBO, CB_ADDSTRING,0,(LPARAM) (LPCTSTR) Weapons[i].name);
	}
	SendDlgItemMessage( IDC_BARREL_7_COMBO, CB_SELECTSTRING,0,(LPARAM) (LPCTSTR) m_weapon_7_text);

	SendDlgItemMessage( IDC_WB_FIRE0_SOUND_PULLDOWN, CB_RESETCONTENT,0,0);
	for (i=0;i<MAX_SOUNDS;i++)
	{
		if (Sounds[i].used)
			SendDlgItemMessage( IDC_WB_FIRE0_SOUND_PULLDOWN, CB_ADDSTRING,0,(LPARAM) (LPCTSTR) Sounds[i].name);
	}
	SendDlgItemMessage( IDC_WB_FIRE0_SOUND_PULLDOWN, CB_SELECTSTRING,0,(LPARAM) (LPCTSTR) m_fire_sound_0_text);

	SendDlgItemMessage( IDC_WB_FIRE1_SOUND_PULLDOWN, CB_RESETCONTENT,0,0);
	for (i=0;i<MAX_SOUNDS;i++)
	{
		if (Sounds[i].used)
			SendDlgItemMessage( IDC_WB_FIRE1_SOUND_PULLDOWN, CB_ADDSTRING,0,(LPARAM) (LPCTSTR) Sounds[i].name);
	}
	SendDlgItemMessage( IDC_WB_FIRE1_SOUND_PULLDOWN, CB_SELECTSTRING,0,(LPARAM) (LPCTSTR) m_fire_sound_1_text);

	SendDlgItemMessage( IDC_WB_FIRE2_SOUND_PULLDOWN, CB_RESETCONTENT,0,0);
	for (i=0;i<MAX_SOUNDS;i++)
	{
		if (Sounds[i].used)
			SendDlgItemMessage( IDC_WB_FIRE2_SOUND_PULLDOWN, CB_ADDSTRING,0,(LPARAM) (LPCTSTR) Sounds[i].name);
	}
	SendDlgItemMessage( IDC_WB_FIRE2_SOUND_PULLDOWN, CB_SELECTSTRING,0,(LPARAM) (LPCTSTR) m_fire_sound_2_text);

	SendDlgItemMessage( IDC_WB_FIRE3_SOUND_PULLDOWN, CB_RESETCONTENT,0,0);
	for (i=0;i<MAX_SOUNDS;i++)
	{
		if (Sounds[i].used)
			SendDlgItemMessage( IDC_WB_FIRE3_SOUND_PULLDOWN, CB_ADDSTRING,0,(LPARAM) (LPCTSTR) Sounds[i].name);
	}
	SendDlgItemMessage( IDC_WB_FIRE3_SOUND_PULLDOWN, CB_SELECTSTRING,0,(LPARAM) (LPCTSTR) m_fire_sound_3_text);

	SendDlgItemMessage( IDC_WB_FIRE4_SOUND_PULLDOWN, CB_RESETCONTENT,0,0);
	for (i=0;i<MAX_SOUNDS;i++)
	{
		if (Sounds[i].used)
			SendDlgItemMessage( IDC_WB_FIRE4_SOUND_PULLDOWN, CB_ADDSTRING,0,(LPARAM) (LPCTSTR) Sounds[i].name);
	}
	SendDlgItemMessage( IDC_WB_FIRE4_SOUND_PULLDOWN, CB_SELECTSTRING,0,(LPARAM) (LPCTSTR) m_fire_sound_4_text);

	SendDlgItemMessage( IDC_WB_FIRE5_SOUND_PULLDOWN, CB_RESETCONTENT,0,0);
	for (i=0;i<MAX_SOUNDS;i++)
	{
		if (Sounds[i].used)
			SendDlgItemMessage( IDC_WB_FIRE5_SOUND_PULLDOWN, CB_ADDSTRING,0,(LPARAM) (LPCTSTR) Sounds[i].name);
	}
	SendDlgItemMessage( IDC_WB_FIRE5_SOUND_PULLDOWN, CB_SELECTSTRING,0,(LPARAM) (LPCTSTR) m_fire_sound_5_text);

	SendDlgItemMessage( IDC_WB_FIRE6_SOUND_PULLDOWN, CB_RESETCONTENT,0,0);
	for (i=0;i<MAX_SOUNDS;i++)
	{
		if (Sounds[i].used)
			SendDlgItemMessage( IDC_WB_FIRE6_SOUND_PULLDOWN, CB_ADDSTRING,0,(LPARAM) (LPCTSTR) Sounds[i].name);
	}
	SendDlgItemMessage( IDC_WB_FIRE6_SOUND_PULLDOWN, CB_SELECTSTRING,0,(LPARAM) (LPCTSTR) m_fire_sound_6_text);

	SendDlgItemMessage( IDC_WB_FIRE7_SOUND_PULLDOWN, CB_RESETCONTENT,0,0);
	for (i=0;i<MAX_SOUNDS;i++)
	{
		if (Sounds[i].used)
			SendDlgItemMessage( IDC_WB_FIRE7_SOUND_PULLDOWN, CB_ADDSTRING,0,(LPARAM) (LPCTSTR) Sounds[i].name);
	}
	SendDlgItemMessage( IDC_WB_FIRE7_SOUND_PULLDOWN, CB_SELECTSTRING,0,(LPARAM) (LPCTSTR) m_fire_sound_7_text);

	SendDlgItemMessage( IDC_CURRENT_GB_COMBO, CB_RESETCONTENT,0,0);
	for (i=0;i<m_obj_pm->num_wbs;i++)
	{
		sprintf(str, "%d", i+1);
		SendDlgItemMessage( IDC_CURRENT_GB_COMBO, CB_ADDSTRING,0,(LPARAM) (LPCTSTR) str);
	}
	itoa(m_current_wb_index + 1, str, 10);
	SendDlgItemMessage( IDC_CURRENT_GB_COMBO, CB_SELECTSTRING,0,(LPARAM) (LPCTSTR) str);

	SendDlgItemMessage( IDC_CURRET_TURRET_COMBO, CB_RESETCONTENT,0,0);
	for (i=0;i<m_obj_pm->poly_wb[m_current_wb_index].num_turrets;i++)
	{
		sprintf(str, "%d", i+1);
		SendDlgItemMessage( IDC_CURRET_TURRET_COMBO, CB_ADDSTRING,0,(LPARAM) (LPCTSTR) str);
	}
	SendDlgItemMessage( IDC_CURRET_TURRET_COMBO, CB_SELECTSTRING,0,(LPARAM) (LPCTSTR) "1");


	((CButton *)GetDlgItem(IDC_BARREL_0_RADIO))->SetCheck(m_current_aiming_point == 0);
	((CButton *)GetDlgItem(IDC_BARREL_1_RADIO))->SetCheck(m_current_aiming_point == 1);
	((CButton *)GetDlgItem(IDC_BARREL_2_RADIO))->SetCheck(m_current_aiming_point == 2);
	((CButton *)GetDlgItem(IDC_BARREL_3_RADIO))->SetCheck(m_current_aiming_point == 3);
	((CButton *)GetDlgItem(IDC_BARREL_4_RADIO))->SetCheck(m_current_aiming_point == 4);
	((CButton *)GetDlgItem(IDC_BARREL_5_RADIO))->SetCheck(m_current_aiming_point == 5);
	((CButton *)GetDlgItem(IDC_BARREL_6_RADIO))->SetCheck(m_current_aiming_point == 6);
	((CButton *)GetDlgItem(IDC_BARREL_7_RADIO))->SetCheck(m_current_aiming_point == 7);

	((CButton *)GetDlgItem(IDC_WB_NO_ANIM_RADIO))->SetCheck(m_anim_type == 0);
	((CButton *)GetDlgItem(IDC_WB_LOCAL_ANIM_RADIO))->SetCheck(m_anim_type == WBF_ANIM_LOCAL);
	((CButton *)GetDlgItem(IDC_WB_MODEL_ANIM_RADIO))->SetCheck(m_anim_type == WBF_ANIM_FULL);

	UpdateDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void RobotEditWeaponsDialog::OnKillfocusGbNumMasksEdit() 
{
	UpdateDialog();	
}


void RobotEditWeaponsDialog::OnSelendokCurretTurretCombo() 
{
	int cur;
	char name[200];

	if(!UpdateData(true)) return;	

	cur=SendDlgItemMessage( IDC_CURRET_TURRET_COMBO, CB_GETCURSEL,0,0);
	SendDlgItemMessage( IDC_CURRET_TURRET_COMBO, CB_GETLBTEXT,cur,(LPARAM) (LPCTSTR)name);

	m_current_turret_combo = name;

	UpdateData(false);

	UpdateDialog();
}

void RobotEditWeaponsDialog::GetData()
{
	int i;

	m_obj_static_wb[m_current_wb_index].aiming_gp_index = m_current_aiming_point;

	m_obj_static_wb[m_current_wb_index].flags &= ~WBF_ANIM_MASKS;
	m_obj_static_wb[m_current_wb_index].flags |= m_anim_type;

	m_obj_static_wb[m_current_wb_index].gp_fire_wait[0] = m_mask_latency_0;
	m_obj_static_wb[m_current_wb_index].gp_fire_wait[1] = m_mask_latency_1;
	m_obj_static_wb[m_current_wb_index].gp_fire_wait[2] = m_mask_latency_2;
	m_obj_static_wb[m_current_wb_index].gp_fire_wait[3] = m_mask_latency_3;
	m_obj_static_wb[m_current_wb_index].gp_fire_wait[4] = m_mask_latency_4;
	m_obj_static_wb[m_current_wb_index].gp_fire_wait[5] = m_mask_latency_5;
	m_obj_static_wb[m_current_wb_index].gp_fire_wait[6] = m_mask_latency_6;
	m_obj_static_wb[m_current_wb_index].gp_fire_wait[7] = m_mask_latency_7;

	m_obj_static_wb[m_current_wb_index].num_masks =	m_num_masks;

	// Clear the masks
	for(i = 0; i < 8; i++) m_obj_static_wb[m_current_wb_index].gp_fire_masks[i] = 0;

	m_obj_static_wb[m_current_wb_index].gp_fire_masks[7] |= m_gp77 << 7;
	m_obj_static_wb[m_current_wb_index].gp_fire_masks[7] |= m_gp76 << 6;
	m_obj_static_wb[m_current_wb_index].gp_fire_masks[7] |= m_gp75 << 5;
	m_obj_static_wb[m_current_wb_index].gp_fire_masks[7] |= m_gp74 << 4;
	m_obj_static_wb[m_current_wb_index].gp_fire_masks[7] |= m_gp73 << 3;
	m_obj_static_wb[m_current_wb_index].gp_fire_masks[7] |= m_gp72 << 2;
	m_obj_static_wb[m_current_wb_index].gp_fire_masks[7] |= m_gp71 << 1;
	m_obj_static_wb[m_current_wb_index].gp_fire_masks[7] |= m_gp70;
	m_obj_static_wb[m_current_wb_index].gp_fire_masks[6] |= m_gp67 << 7;
	m_obj_static_wb[m_current_wb_index].gp_fire_masks[6] |= m_gp66 << 6;
	m_obj_static_wb[m_current_wb_index].gp_fire_masks[6] |= m_gp65 << 5;
	m_obj_static_wb[m_current_wb_index].gp_fire_masks[6] |= m_gp64 << 4;
	m_obj_static_wb[m_current_wb_index].gp_fire_masks[6] |= m_gp63 << 3;
	m_obj_static_wb[m_current_wb_index].gp_fire_masks[6] |= m_gp62 << 2;
	m_obj_static_wb[m_current_wb_index].gp_fire_masks[6] |= m_gp61 << 1;
	m_obj_static_wb[m_current_wb_index].gp_fire_masks[6] |= m_gp60;
	m_obj_static_wb[m_current_wb_index].gp_fire_masks[5] |= m_gp57 << 7;
	m_obj_static_wb[m_current_wb_index].gp_fire_masks[5] |= m_gp56 << 6;
	m_obj_static_wb[m_current_wb_index].gp_fire_masks[5] |= m_gp55 << 5;
	m_obj_static_wb[m_current_wb_index].gp_fire_masks[5] |= m_gp54 << 4;
	m_obj_static_wb[m_current_wb_index].gp_fire_masks[5] |= m_gp53 << 3;
	m_obj_static_wb[m_current_wb_index].gp_fire_masks[5] |= m_gp52 << 2;
	m_obj_static_wb[m_current_wb_index].gp_fire_masks[5] |= m_gp51 << 1;
	m_obj_static_wb[m_current_wb_index].gp_fire_masks[5] |= m_gp50;
	m_obj_static_wb[m_current_wb_index].gp_fire_masks[4] |= m_gp47 << 7;
	m_obj_static_wb[m_current_wb_index].gp_fire_masks[4] |= m_gp46 << 6;
	m_obj_static_wb[m_current_wb_index].gp_fire_masks[4] |= m_gp45 << 5;
	m_obj_static_wb[m_current_wb_index].gp_fire_masks[4] |= m_gp44 << 4;
	m_obj_static_wb[m_current_wb_index].gp_fire_masks[4] |= m_gp43 << 3;
	m_obj_static_wb[m_current_wb_index].gp_fire_masks[4] |= m_gp42 << 2;
	m_obj_static_wb[m_current_wb_index].gp_fire_masks[4] |= m_gp41 << 1;
	m_obj_static_wb[m_current_wb_index].gp_fire_masks[4] |= m_gp40;
	m_obj_static_wb[m_current_wb_index].gp_fire_masks[3] |= m_gp37 << 7;
	m_obj_static_wb[m_current_wb_index].gp_fire_masks[3] |= m_gp36 << 6;
	m_obj_static_wb[m_current_wb_index].gp_fire_masks[3] |= m_gp35 << 5;
	m_obj_static_wb[m_current_wb_index].gp_fire_masks[3] |= m_gp34 << 4;
	m_obj_static_wb[m_current_wb_index].gp_fire_masks[3] |= m_gp33 << 3;
	m_obj_static_wb[m_current_wb_index].gp_fire_masks[3] |= m_gp32 << 2;
	m_obj_static_wb[m_current_wb_index].gp_fire_masks[3] |= m_gp31 << 1;
	m_obj_static_wb[m_current_wb_index].gp_fire_masks[3] |= m_gp30;
	m_obj_static_wb[m_current_wb_index].gp_fire_masks[2] |= m_gp27 << 7;
	m_obj_static_wb[m_current_wb_index].gp_fire_masks[2] |= m_gp26 << 6;
	m_obj_static_wb[m_current_wb_index].gp_fire_masks[2] |= m_gp25 << 5;
	m_obj_static_wb[m_current_wb_index].gp_fire_masks[2] |= m_gp24 << 4;
	m_obj_static_wb[m_current_wb_index].gp_fire_masks[2] |= m_gp23 << 3;
	m_obj_static_wb[m_current_wb_index].gp_fire_masks[2] |= m_gp22 << 2;
	m_obj_static_wb[m_current_wb_index].gp_fire_masks[2] |= m_gp21 << 1;
	m_obj_static_wb[m_current_wb_index].gp_fire_masks[2] |= m_gp20;
	m_obj_static_wb[m_current_wb_index].gp_fire_masks[1] |= m_gp17 << 7;
	m_obj_static_wb[m_current_wb_index].gp_fire_masks[1] |= m_gp16 << 6;
	m_obj_static_wb[m_current_wb_index].gp_fire_masks[1] |= m_gp15 << 5;
	m_obj_static_wb[m_current_wb_index].gp_fire_masks[1] |= m_gp14 << 4;
	m_obj_static_wb[m_current_wb_index].gp_fire_masks[1] |= m_gp13 << 3;
	m_obj_static_wb[m_current_wb_index].gp_fire_masks[1] |= m_gp12 << 2;
	m_obj_static_wb[m_current_wb_index].gp_fire_masks[1] |= m_gp11 << 1;
	m_obj_static_wb[m_current_wb_index].gp_fire_masks[1] |= m_gp10;
	m_obj_static_wb[m_current_wb_index].gp_fire_masks[0] |= m_gp07 << 7;
	m_obj_static_wb[m_current_wb_index].gp_fire_masks[0] |= m_gp06 << 6;
	m_obj_static_wb[m_current_wb_index].gp_fire_masks[0] |= m_gp05 << 5;
	m_obj_static_wb[m_current_wb_index].gp_fire_masks[0] |= m_gp04 << 4;
	m_obj_static_wb[m_current_wb_index].gp_fire_masks[0] |= m_gp03 << 3;
	m_obj_static_wb[m_current_wb_index].gp_fire_masks[0] |= m_gp02 << 2;
	m_obj_static_wb[m_current_wb_index].gp_fire_masks[0] |= m_gp01 << 1;
	m_obj_static_wb[m_current_wb_index].gp_fire_masks[0] |= m_gp00;

	m_obj_static_wb[m_current_wb_index].gp_quad_fire_mask = 0;
	m_obj_static_wb[m_current_wb_index].gp_quad_fire_mask |= m_gpq7 << 7;
	m_obj_static_wb[m_current_wb_index].gp_quad_fire_mask |= m_gpq6 << 6;
	m_obj_static_wb[m_current_wb_index].gp_quad_fire_mask |= m_gpq5 << 5;
	m_obj_static_wb[m_current_wb_index].gp_quad_fire_mask |= m_gpq4 << 4;
	m_obj_static_wb[m_current_wb_index].gp_quad_fire_mask |= m_gpq3 << 3;
	m_obj_static_wb[m_current_wb_index].gp_quad_fire_mask |= m_gpq2 << 2;
	m_obj_static_wb[m_current_wb_index].gp_quad_fire_mask |= m_gpq1 << 1;
	m_obj_static_wb[m_current_wb_index].gp_quad_fire_mask |= m_gpq0;

	m_obj_static_wb[m_current_wb_index].aiming_3d_dot = cos(m_view_cone_angle*PI/(360.0));
	m_obj_static_wb[m_current_wb_index].aiming_3d_dist = m_max_distance;
	m_obj_static_wb[m_current_wb_index].aiming_XZ_dot = cos(m_xz_plane_angle*PI/(360.0));

	m_obj_static_wb[m_current_wb_index].ammo_usage=m_ammo_usage;
	m_obj_static_wb[m_current_wb_index].energy_usage=m_energy_usage;

	m_obj_static_wb[m_current_wb_index].anim_time[0] = m_fire0_cycle_time;
	m_obj_static_wb[m_current_wb_index].anim_time[1] = m_fire1_cycle_time;
	m_obj_static_wb[m_current_wb_index].anim_time[2] = m_fire2_cycle_time;
	m_obj_static_wb[m_current_wb_index].anim_time[3] = m_fire3_cycle_time;
	m_obj_static_wb[m_current_wb_index].anim_time[4] = m_fire4_cycle_time;
	m_obj_static_wb[m_current_wb_index].anim_time[5] = m_fire5_cycle_time;
	m_obj_static_wb[m_current_wb_index].anim_time[6] = m_fire6_cycle_time;
	m_obj_static_wb[m_current_wb_index].anim_time[7] = m_fire7_cycle_time;
	m_obj_static_wb[m_current_wb_index].anim_end_frame[0] = m_fire0_end_tick;
	m_obj_static_wb[m_current_wb_index].anim_end_frame[1] = m_fire1_end_tick;
	m_obj_static_wb[m_current_wb_index].anim_end_frame[2] = m_fire2_end_tick;
	m_obj_static_wb[m_current_wb_index].anim_end_frame[3] = m_fire3_end_tick;
	m_obj_static_wb[m_current_wb_index].anim_end_frame[4] = m_fire4_end_tick;
	m_obj_static_wb[m_current_wb_index].anim_end_frame[5] = m_fire5_end_tick;
	m_obj_static_wb[m_current_wb_index].anim_end_frame[6] = m_fire6_end_tick;
	m_obj_static_wb[m_current_wb_index].anim_end_frame[7] = m_fire7_end_tick;
	m_obj_static_wb[m_current_wb_index].anim_fire_frame[0] = m_fire0_fire_tick;
	m_obj_static_wb[m_current_wb_index].anim_fire_frame[1] = m_fire1_fire_tick;
	m_obj_static_wb[m_current_wb_index].anim_fire_frame[2] = m_fire2_fire_tick;
	m_obj_static_wb[m_current_wb_index].anim_fire_frame[3] = m_fire3_fire_tick;
	m_obj_static_wb[m_current_wb_index].anim_fire_frame[4] = m_fire4_fire_tick;
	m_obj_static_wb[m_current_wb_index].anim_fire_frame[5] = m_fire5_fire_tick;
	m_obj_static_wb[m_current_wb_index].anim_fire_frame[6] = m_fire6_fire_tick;
	m_obj_static_wb[m_current_wb_index].anim_fire_frame[7] = m_fire7_fire_tick;
	m_obj_static_wb[m_current_wb_index].anim_start_frame[0] = m_fire0_start_tick;
	m_obj_static_wb[m_current_wb_index].anim_start_frame[1] = m_fire1_start_tick;
	m_obj_static_wb[m_current_wb_index].anim_start_frame[2] = m_fire2_start_tick;
	m_obj_static_wb[m_current_wb_index].anim_start_frame[3] = m_fire3_start_tick;
	m_obj_static_wb[m_current_wb_index].anim_start_frame[4] = m_fire4_start_tick;
	m_obj_static_wb[m_current_wb_index].anim_start_frame[5] = m_fire5_start_tick;
	m_obj_static_wb[m_current_wb_index].anim_start_frame[6] = m_fire6_start_tick;
	m_obj_static_wb[m_current_wb_index].anim_start_frame[7] = m_fire7_start_tick;

	m_obj_static_wb[m_current_wb_index].gp_weapon_index[0] =	FindWeaponName(m_weapon_0_text.GetBuffer(0));
	m_obj_static_wb[m_current_wb_index].gp_weapon_index[1] =	FindWeaponName(m_weapon_1_text.GetBuffer(0));
	m_obj_static_wb[m_current_wb_index].gp_weapon_index[2] =	FindWeaponName(m_weapon_2_text.GetBuffer(0));
	m_obj_static_wb[m_current_wb_index].gp_weapon_index[3] =	FindWeaponName(m_weapon_3_text.GetBuffer(0));
	m_obj_static_wb[m_current_wb_index].gp_weapon_index[4] =	FindWeaponName(m_weapon_4_text.GetBuffer(0));
	m_obj_static_wb[m_current_wb_index].gp_weapon_index[5] =	FindWeaponName(m_weapon_5_text.GetBuffer(0));
	m_obj_static_wb[m_current_wb_index].gp_weapon_index[6] =	FindWeaponName(m_weapon_6_text.GetBuffer(0));
	m_obj_static_wb[m_current_wb_index].gp_weapon_index[7] =	FindWeaponName(m_weapon_7_text.GetBuffer(0));

	m_obj_static_wb[m_current_wb_index].fm_fire_sound_index[0] =	FindSoundName(m_fire_sound_0_text.GetBuffer(0));
	m_obj_static_wb[m_current_wb_index].fm_fire_sound_index[1] =	FindSoundName(m_fire_sound_1_text.GetBuffer(0));
	m_obj_static_wb[m_current_wb_index].fm_fire_sound_index[2] =	FindSoundName(m_fire_sound_2_text.GetBuffer(0));
	m_obj_static_wb[m_current_wb_index].fm_fire_sound_index[3] =	FindSoundName(m_fire_sound_3_text.GetBuffer(0));
	m_obj_static_wb[m_current_wb_index].fm_fire_sound_index[4] =	FindSoundName(m_fire_sound_4_text.GetBuffer(0));
	m_obj_static_wb[m_current_wb_index].fm_fire_sound_index[5] =	FindSoundName(m_fire_sound_5_text.GetBuffer(0));
	m_obj_static_wb[m_current_wb_index].fm_fire_sound_index[6] =	FindSoundName(m_fire_sound_6_text.GetBuffer(0));
	m_obj_static_wb[m_current_wb_index].fm_fire_sound_index[7] =	FindSoundName(m_fire_sound_7_text.GetBuffer(0));

	if (m_spray_check)
		m_obj_static_wb[m_current_wb_index].flags|= WBF_SPRAY;
	else
		m_obj_static_wb[m_current_wb_index].flags&= ~WBF_SPRAY;

	if (m_guided)
		m_obj_static_wb[m_current_wb_index].flags|= WBF_GUIDED;
	else
		m_obj_static_wb[m_current_wb_index].flags&= ~WBF_GUIDED;

	if (m_user_timeout)
		m_obj_static_wb[m_current_wb_index].flags|= WBF_USER_TIMEOUT;
	else
		m_obj_static_wb[m_current_wb_index].flags&= ~WBF_USER_TIMEOUT;


	if (m_random_check)
		m_obj_static_wb[m_current_wb_index].flags|= WBF_RANDOM_FIRE_ORDER;
	else
		m_obj_static_wb[m_current_wb_index].flags&= ~WBF_RANDOM_FIRE_ORDER;

	if (m_force_fvec_check)
		m_obj_static_wb[m_current_wb_index].flags|= WBF_FIRE_FVEC;
	else
		m_obj_static_wb[m_current_wb_index].flags&= ~WBF_FIRE_FVEC;

	if (m_force_target_check)
		m_obj_static_wb[m_current_wb_index].flags|= WBF_FIRE_TARGET;
	else
		m_obj_static_wb[m_current_wb_index].flags&= ~WBF_FIRE_TARGET;

	if (m_aim_fvec_check)
		m_obj_static_wb[m_current_wb_index].flags|= WBF_AIM_FVEC;
	else
		m_obj_static_wb[m_current_wb_index].flags&= ~WBF_AIM_FVEC;

	if (m_onoff_check)
		m_obj_static_wb[m_current_wb_index].flags|= WBF_ON_OFF;
	else
		m_obj_static_wb[m_current_wb_index].flags&= ~WBF_ON_OFF;

	if (m_custom_view_cone_check)
		m_obj_static_wb[m_current_wb_index].flags|= WBF_USE_CUSTOM_FOV;
	else
		m_obj_static_wb[m_current_wb_index].flags&= ~WBF_USE_CUSTOM_FOV;

	if (m_custom_max_dist)
		m_obj_static_wb[m_current_wb_index].flags|= WBF_USE_CUSTOM_MAX_DIST;
	else
		m_obj_static_wb[m_current_wb_index].flags&= ~WBF_USE_CUSTOM_MAX_DIST;
}


void RobotEditWeaponsDialog::OnSelendokCurrentGbCombo() 
{
	int cur;
	char name[200];
	int f_save_info;

	if(!UpdateData(true)) return;	

	cur=SendDlgItemMessage( IDC_CURRENT_GB_COMBO, CB_GETCURSEL,0,0);
	SendDlgItemMessage( IDC_CURRENT_GB_COMBO, CB_GETLBTEXT,cur,(LPARAM) (LPCTSTR)name);

	m_current_gb = name;

	UpdateData(false);

	UpdateDialog();

	f_save_info = OutrageMessageBox (MBOX_YESNO, "Would you like to save changes to weapon battery %d\nNote: Changes will be lost if you do not save them now.", m_current_wb_index+1);
	if(f_save_info == IDYES)
	{
		GetData();
	}

	m_current_wb_index = atoi(m_current_gb) - 1;
	ASSERT(m_current_wb_index >= 0 && m_current_wb_index < m_obj_pm->num_wbs);

	NewWBSelected();	
	UpdateData(false);
	UpdateDialog();
}
