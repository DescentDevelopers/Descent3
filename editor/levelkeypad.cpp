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

// levelkeypad.cpp : implementation file
//

#include "stdafx.h"
#include "editor.h"
#include "levelkeypad.h"
#include "physics.h"
#include "levelgoal.h"
#include "boa.h"
#include "terrain.h"
#include "aiambient.h"
#include "objinfo.h"
#include "game.h"
#include "soundload.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// levelkeypad dialog

levelkeypad::levelkeypad(CWnd* pParent /*=NULL*/)
	: CKeypadDialog(levelkeypad::IDD, pParent)
{
	//{{AFX_DATA_INIT(levelkeypad)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void levelkeypad::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(levelkeypad)
	DDX_Control(pDX, IDC_LEVELGOALTRIGGER_COMBO, m_trigger_combo);
	DDX_Control(pDX, IDC_LEVELGOALOBJECT_COMBO, m_object_combo);
	DDX_Control(pDX, IDC_LEVELGOAL_ROOM_COMBO, m_room_combo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(levelkeypad, CDialog)
	//{{AFX_MSG_MAP(levelkeypad)
	ON_WM_PAINT()
	ON_EN_KILLFOCUS(IDC_LEVEL_GRAVITY_EDIT, OnKillfocusLevelGravityEdit)
	ON_BN_CLICKED(IDC_LEVEL_PREV_GOAL_BUTTON, OnLevelPrevGoalButton)
	ON_BN_CLICKED(IDC_LEVEL_NEXT_GOAL_BUTTON, OnLevelNextGoalButton)
	ON_BN_CLICKED(IDC_LEVEL_NEW_GOAL_BUTTON, OnLevelNewGoalButton)
	ON_EN_KILLFOCUS(IDC_LEVEL_GOAL_NAME_EDIT, OnKillfocusLevelGoalNameEdit)
	ON_EN_KILLFOCUS(IDC_LEVEL_GOAL_LOC_NAME_EDIT, OnKillfocusLevelGoalLocNameEdit)
	ON_EN_KILLFOCUS(IDC_LEVEL_GOAL_DESC_EDIT, OnKillfocusLevelGoalDescEdit)
	ON_BN_CLICKED(IDC_LEVEL_GOAL_NEW_ITEM_BUTTON, OnLevelGoalNewItemButton)
	ON_BN_CLICKED(IDC_LEVEL_GOAL_NEXT_ITEM_BUTTON, OnLevelGoalNextItemButton)
	ON_BN_CLICKED(IDC_LEVEL_GOAL_PREV_ITEM_BUTTON, OnLevelGoalPrevItemButton)
	ON_BN_CLICKED(IDC_LEVEL_GOAL_DELETE_ITEM_BUTTON, OnLevelGoalDeleteItemButton)
	ON_BN_CLICKED(IDC_LEVEL_GOAL_ROOM_ITEM_RADIO, OnLevelGoalRoomItemRadio)
	ON_BN_CLICKED(IDC_LEVEL_GOAL_CELL_ITEM_RADIO, OnLevelGoalCellItemRadio)
	ON_BN_CLICKED(IDC_LEVEL_GOAL_OBJ_ITEM_RADIO, OnLevelGoalObjItemRadio)
	ON_BN_CLICKED(IDC_GOAL_AUTO_COMPLETE_CHECK, OnGoalAutoCompleteCheck)
	ON_BN_CLICKED(IDC_LEVEL_GOAL_SEC_GOAL_CHECK, OnLevelGoalSecGoalCheck)
	ON_BN_CLICKED(IDC_LEVEL_GOAL_ENABLED_CHECK, OnLevelGoalEnabledCheck)
	ON_BN_CLICKED(IDC_LEVEL_GOAL_COMPLETED_CHECK, OnLevelGoalCompletedCheck)
	ON_BN_CLICKED(IDC_LEVEL_GOAL_TLG_CHECK, OnLevelGoalTlgCheck)
	ON_BN_CLICKED(IDC_LEVEL_GOAL_GDKL_CHECK, OnLevelGoalGdklCheck)
	ON_BN_CLICKED(IDC_LEVEL_GOAL_NLB_CHECK, OnLevelGoalNlbCheck)
	ON_BN_CLICKED(IDC_LEVEL_GOAL_PASTE_OBJREF_ITEM_BUTTON, OnLevelGoalPasteObjrefItemButton)
	ON_BN_CLICKED(IDC_LEVEL_GOAL_GOTO_ITEM_BUTTON, OnLevelGoalGotoItemButton)
	ON_BN_CLICKED(IDC_LEVEL_DELETE_GOAL_BUTTON, OnLevelDeleteGoalButton)
	ON_EN_KILLFOCUS(IDC_LEVEL_GOAL_ITEM_EDIT, OnKillfocusLevelGoalItemEdit)
	ON_EN_KILLFOCUS(IDC_LEVEL_GOAL_PRIORITY_EDIT1, OnKillfocusLevelGoalPriorityEdit1)
	ON_BN_CLICKED(IDC_LEVEL_GOAL_END_LEVEL_CHECK, OnLevelGoalEndLevelCheck)
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_SIZE()
	ON_CBN_SELENDOK(IDC_AMB_TYPE1, OnSelendokAmbType1)
	ON_CBN_SELENDOK(IDC_AMB_TYPE2, OnSelendokAmbType2)
	ON_CBN_SELENDOK(IDC_AMB_TYPE3, OnSelendokAmbType3)
	ON_CBN_SELENDOK(IDC_AMB_TYPE4, OnSelendokAmbType4)
	ON_CBN_SELENDOK(IDC_AMB_TYPE5, OnSelendokAmbType5)
	ON_CBN_SELENDOK(IDC_AMB_TYPE6, OnSelendokAmbType6)
	ON_EN_KILLFOCUS(IDC_AMB_MAX1, OnKillfocusAmbMax1)
	ON_EN_KILLFOCUS(IDC_AMB_MAX2, OnKillfocusAmbMax2)
	ON_EN_KILLFOCUS(IDC_AMB_MAX3, OnKillfocusAmbMax3)
	ON_EN_KILLFOCUS(IDC_AMB_MAX4, OnKillfocusAmbMax4)
	ON_EN_KILLFOCUS(IDC_AMB_MAX5, OnKillfocusAmbMax5)
	ON_EN_KILLFOCUS(IDC_AMB_MAX6, OnKillfocusAmbMax6)
	ON_EN_KILLFOCUS(IDC_AMB_MIN1, OnKillfocusAmbMin1)
	ON_EN_KILLFOCUS(IDC_AMB_MIN2, OnKillfocusAmbMin2)
	ON_EN_KILLFOCUS(IDC_AMB_MIN3, OnKillfocusAmbMin3)
	ON_EN_KILLFOCUS(IDC_AMB_MIN4, OnKillfocusAmbMin4)
	ON_EN_KILLFOCUS(IDC_AMB_MIN5, OnKillfocusAmbMin5)
	ON_EN_KILLFOCUS(IDC_AMB_MIN6, OnKillfocusAmbMin6)
	ON_EN_KILLFOCUS(IDC_AMB_N_GROUPS1, OnKillfocusAmbNGroups1)
	ON_EN_KILLFOCUS(IDC_AMB_N_GROUPS2, OnKillfocusAmbNGroups2)
	ON_EN_KILLFOCUS(IDC_AMB_N_GROUPS3, OnKillfocusAmbNGroups3)
	ON_EN_KILLFOCUS(IDC_AMB_N_GROUPS4, OnKillfocusAmbNGroups4)
	ON_EN_KILLFOCUS(IDC_AMB_N_GROUPS5, OnKillfocusAmbNGroups5)
	ON_EN_KILLFOCUS(IDC_AMB_N_GROUPS6, OnKillfocusAmbNGroups6)
	ON_BN_CLICKED(IDC_AMB_OUTSIDE1, OnAmbOutside1)
	ON_BN_CLICKED(IDC_AMB_OUTSIDE2, OnAmbOutside2)
	ON_BN_CLICKED(IDC_AMB_OUTSIDE3, OnAmbOutside3)
	ON_BN_CLICKED(IDC_AMB_OUTSIDE4, OnAmbOutside4)
	ON_BN_CLICKED(IDC_AMB_OUTSIDE5, OnAmbOutside5)
	ON_BN_CLICKED(IDC_AMB_OUTSIDE6, OnAmbOutside6)
	ON_EN_KILLFOCUS(IDC_LEVEL_GOAL_COMPLETION_EDIT, OnKillfocusLevelGoalCompletionEdit)
	ON_BN_CLICKED(IDC_LG_ACTIVATE_RADIO, OnLgActivateRadio)
	ON_BN_CLICKED(IDC_LG_ENTER_RADIO, OnLgEnterRadio)
	ON_BN_CLICKED(IDC_LG_DESTROYED_RADIO, OnLgDestroyedRadio)
	ON_BN_CLICKED(IDC_LG_PPW_COLLIDE_RADIO, OnLgPpwCollideRadio)
	ON_BN_CLICKED(IDC_LG_LC_RADIO, OnLgLcRadio)
	ON_BN_CLICKED(IDC_LG_DALLAS_RADIO, OnLgDallasRadio)
	ON_BN_CLICKED(IDC_TRIGGER_RADIO, OnTriggerRadio)
	ON_BN_CLICKED(IDC_LEVEL_GOAL_MINE_RADIO, OnLevelGoalMineRadio)
	ON_CBN_SELENDOK(IDC_LEVELGOAL_ROOM_COMBO, OnSelendokLevelgoalRoomCombo)
	ON_CBN_SELENDOK(IDC_LEVELGOALOBJECT_COMBO, OnSelendokLevelgoalobjectCombo)
	ON_CBN_SELENDOK(IDC_LEVELGOALTRIGGER_COMBO, OnSelendokLevelgoaltriggerCombo)
	ON_EN_KILLFOCUS(IDC_EDIT57, OnKillfocusEdit57)
	ON_BN_CLICKED(IDC_LEVEL_CEILING_EVERYWHERE, OnLevelCeilingEverywhere)
	ON_EN_KILLFOCUS(IDC_LEVEL_CEILING_EDIT, OnKillfocusLevelCeilingEdit)
	ON_CBN_SELENDOK(IDC_FFOS_COMBO, OnSelendokFfosCombo)
	ON_CBN_SELENDOK(IDC_BGOS_COMBO, OnSelendokBgosCombo)
	ON_EN_KILLFOCUS(IDC_FFT1_EDIT, OnKillfocusFft1Edit)
	ON_EN_KILLFOCUS(IDC_FFT2_EDIT, OnKillfocusFft2Edit)
	ON_CBN_SELENDOK(IDC_FFT1_COMBO, OnSelendokFft1Combo)
	ON_CBN_SELENDOK(IDC_FFT2_COMBO, OnSelendokFft2Combo)
	ON_EN_KILLFOCUS(IDC_FIXED_FFT1_EDIT, OnKillfocusFixedFft1Edit)
	ON_CBN_SETFOCUS(IDC_FFT1_COMBO, OnSetfocusFft1Combo)
	ON_CBN_SETFOCUS(IDC_FFT2_COMBO, OnSetfocusFft2Combo)
	ON_BN_CLICKED(IDC_LEVEL_PIW, OnLevelPiw)
	ON_EN_KILLFOCUS(IDC_FFT3_EDIT, OnKillfocusFft3Edit)
	ON_CBN_SELENDOK(IDC_FFT3_COMBO, OnSelendokFft3Combo)
	ON_CBN_SETFOCUS(IDC_FFT3_COMBO, OnSetfocusFft3Combo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// levelkeypad message handlers


void levelkeypad::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	if (!m_Active) return;
 
	UpdateDialog();
}

bool levelkeypad::ValidateCurrentGoal()
{
	int num_goals = Level_goals.GetNumGoals();

	if(num_goals > 0)
	{
		if(m_current_goal < 0 || m_current_goal >= num_goals)
			m_current_goal = 0;

		return true;
	}
	else
	{
		m_current_goal = -1;
		return false;
	}
}

bool levelkeypad::ValidateCurrentItem()
{
	if(ValidateCurrentGoal())
	{
		int num_items = Level_goals.GoalGetNumItems(m_current_goal);
		if(num_items > 0)
		{
			if(m_current_item < 0 || m_current_item >= num_items)
				m_current_item = 0;

			return true;
		}
	}

	m_current_item = -1;
	return false;
}

void make_goal_dialog_text(char *big_text_message)
{
	int i;
	int j;

	big_text_message[0] = '\0';

	int num_goals[2 * MAX_GOAL_LISTS];
	int goals[2 * MAX_GOAL_LISTS][MAX_LEVEL_GOALS];
	int p[2 * MAX_GOAL_LISTS][MAX_LEVEL_GOALS];

	for(i = 0; i < 2 * MAX_GOAL_LISTS; i++)
	{
		num_goals[i] = 0;
	}

	int n = Level_goals.GetNumGoals();

	for(i = 0; i < n; i++)
	{
		int priority;
		char list;
		int flags;

		Level_goals.GoalPriority(i, LO_GET_SPECIFIED, &priority);
		Level_goals.GoalGoalList(i, LO_GET_SPECIFIED, &list);
		Level_goals.GoalStatus(i, LO_GET_SPECIFIED, &flags, true);

		if(flags & LGF_SECONDARY_GOAL)
		{
			list += MAX_GOAL_LISTS;
		}

		int insert = num_goals[list];

		for(j = 0; j < num_goals[list]; j++)
		{
			if(priority < p[list][j])
			{
				insert = j;
				break;
			}
		}

		for(j = num_goals[list]; j > insert; j--)
		{
			goals[list][j] = goals[list][j - 1];
			p[list][j] = p[list][j - 1];
		}

		goals[list][insert] = i;
		p[list][insert] = priority;

		num_goals[list]++;
	}

	for(i = 0; i < 2 * MAX_GOAL_LISTS; i++)
	{
		strcat(big_text_message, "\r\n---------------------\r\n");

		if(i < MAX_GOAL_LISTS)
		{
			char cur[50];
			sprintf(cur, "Primary List %d\r\n", i);
			strcat(big_text_message, cur);
		}
		else
		{
			char cur[50];
			sprintf(cur, "Secondary List %d\r\n", i - MAX_GOAL_LISTS);
			strcat(big_text_message, cur);
		}

		strcat(big_text_message, "---------------------\r\n");

		for(j = 0; j < num_goals[i]; j++)
		{
			char name[41];
			char p_text[5];
			
			strcat(big_text_message, "   ");
			Level_goals.GoalGetName(goals[i][j], name, 40);

			int flags;
			Level_goals.GoalStatus(goals[i][j], LO_GET_SPECIFIED, &flags, true);
			if(flags & LGF_TELCOM_LISTS)
			{
				strcat(big_text_message, "(Obj) ");
			}
			strcat(big_text_message, name);
			sprintf(p_text, " (%d)", p[i][j]);
			strcat(big_text_message, p_text);
			strcat(big_text_message, "\r\n");
		}
	}
}

//	call this to update all the text entries in the keypad.
void levelkeypad::UpdateDialog()
{
	CButton *bbox;
	int flags;
	int lgflags;
	int priority;
	char g_list;
	char name[256];
	char iname[256];
	char desc[2560];
	char message[2560];
	int num_goals;
	int num_items = 0;
	int i;
	char big_text_message[MAX_LEVEL_GOALS*5*40];

	char item_type;
	int item_handle;

	SendDlgItemMessage( IDC_BGOS_COMBO, CB_RESETCONTENT,0,0);
	for (i = 0;i < MAX_SOUNDS; i++)
	{
		if (Sounds[i].used)
			SendDlgItemMessage( IDC_BGOS_COMBO, CB_ADDSTRING,0,(LPARAM) (LPCTSTR) Sounds[i].name);
	}
	if(sound_override_glass_breaking != -1)
		SendDlgItemMessage( IDC_BGOS_COMBO, CB_SELECTSTRING,0,(LPARAM) (LPCTSTR) Sounds[sound_override_glass_breaking].name);
	else
		SendDlgItemMessage( IDC_BGOS_COMBO, CB_SELECTSTRING,0,(LPARAM) (LPCTSTR) "Default");


	if(force_field_bounce_texture[0] != -1)
		SendDlgItemMessage( IDC_FFT1_COMBO, CB_SELECTSTRING,0,(LPARAM) (LPCTSTR) GameTextures[force_field_bounce_texture[0]].name);
	else
		SendDlgItemMessage( IDC_FFT1_COMBO, CB_SELECTSTRING,0,(LPARAM) (LPCTSTR) "Not Assigned");
	if(force_field_bounce_texture[1] != -1)
		SendDlgItemMessage( IDC_FFT2_COMBO, CB_SELECTSTRING,0,(LPARAM) (LPCTSTR) GameTextures[force_field_bounce_texture[1]].name);
	else
		SendDlgItemMessage( IDC_FFT2_COMBO, CB_SELECTSTRING,0,(LPARAM) (LPCTSTR) "Not Assigned");
	if(force_field_bounce_texture[2] != -1)
		SendDlgItemMessage( IDC_FFT3_COMBO, CB_SELECTSTRING,0,(LPARAM) (LPCTSTR) GameTextures[force_field_bounce_texture[2]].name);
	else
		SendDlgItemMessage( IDC_FFT3_COMBO, CB_SELECTSTRING,0,(LPARAM) (LPCTSTR) "Not Assigned");


	SendDlgItemMessage( IDC_FFOS_COMBO, CB_RESETCONTENT,0,0);
	for (i = 0;i < MAX_SOUNDS; i++)
	{
		if (Sounds[i].used)
			SendDlgItemMessage( IDC_FFOS_COMBO, CB_ADDSTRING,0,(LPARAM) (LPCTSTR) Sounds[i].name);
	}
	if(sound_override_force_field != -1)
		SendDlgItemMessage( IDC_FFOS_COMBO, CB_SELECTSTRING,0,(LPARAM) (LPCTSTR) Sounds[sound_override_force_field].name);
	else
		SendDlgItemMessage( IDC_FFOS_COMBO, CB_SELECTSTRING,0,(LPARAM) (LPCTSTR) "Default");

	int idc_amb_type[6] = {IDC_AMB_TYPE1, IDC_AMB_TYPE2, IDC_AMB_TYPE3, IDC_AMB_TYPE4, IDC_AMB_TYPE5, IDC_AMB_TYPE6};

	int idc_chars[6][4] =  {{IDC_AMB_N_GROUPS1,IDC_AMB_MAX1,IDC_AMB_MIN1},
									{IDC_AMB_N_GROUPS2,IDC_AMB_MAX2,IDC_AMB_MIN2},
									{IDC_AMB_N_GROUPS3,IDC_AMB_MAX3,IDC_AMB_MIN3},
									{IDC_AMB_N_GROUPS4,IDC_AMB_MAX4,IDC_AMB_MIN4},
									{IDC_AMB_N_GROUPS5,IDC_AMB_MAX5,IDC_AMB_MIN5},
									{IDC_AMB_N_GROUPS6,IDC_AMB_MAX6,IDC_AMB_MIN6}};

	int idc_outside[6] = {IDC_AMB_OUTSIDE1, IDC_AMB_OUTSIDE2, IDC_AMB_OUTSIDE3, 
		                   IDC_AMB_OUTSIDE4, IDC_AMB_OUTSIDE5, IDC_AMB_OUTSIDE6};

	for(i = 0; i < 6; i++)
	{
		int j;
		uint8_t x;

		for(j = 1; j <= 3; j++)
		{
			a_life.GetALValue(i, j, &x);
			PrintToDlgItem(this, idc_chars[i][j - 1], "%d", (int)x);
		}

		int n_id = idc_outside[i];
		a_life.GetALValue(i, ALI_FLAGS, &x);

		bbox = (CButton *)GetDlgItem(n_id);
		bbox->SetCheck(x != 0);
	}

	for(i = 0; i < 6; i++)
	{
		int n_id = idc_amb_type[i];
		int j;
		int x;

		SendDlgItemMessage( n_id, CB_RESETCONTENT,0,0);
		SendDlgItemMessage( n_id, CB_ADDSTRING,0,(LPARAM) (LPCTSTR) "None");

		for (j=0;j<MAX_OBJECT_IDS;j++)
		{
			if (Object_info[j].type != OBJ_NONE)
			{
				SendDlgItemMessage(n_id, CB_ADDSTRING,0,(LPARAM) (LPCTSTR) Object_info[j].name);
			}
		}
	
		a_life.GetALValue(i, ALI_TYPE, &x);	
		if(x >= 0 && x < MAX_OBJECT_IDS && Object_info[x].type != OBJ_NONE)
			SendDlgItemMessage( n_id, CB_SELECTSTRING,0,(LPARAM) (LPCTSTR) Object_info[x].name);
		else
			SendDlgItemMessage( n_id, CB_SELECTSTRING,0,(LPARAM) (LPCTSTR) "None");
	}

	Level_goals.LGStatus(LO_GET_SPECIFIED, &lgflags);

	if(ValidateCurrentGoal())
	{
		num_goals = Level_goals.GetNumGoals();
		num_items = Level_goals.GoalGetNumItems(m_current_goal);

		Level_goals.GoalStatus(m_current_goal, LO_GET_SPECIFIED, &flags, true);
		Level_goals.GoalPriority(m_current_goal, LO_GET_SPECIFIED, &priority);
		Level_goals.GoalGoalList(m_current_goal, LO_GET_SPECIFIED, &g_list);

		Level_goals.GoalGetName(m_current_goal, name, sizeof(name));
		Level_goals.GoalGetItemName(m_current_goal, iname, sizeof(iname));
		Level_goals.GoalGetDesc(m_current_goal, desc, sizeof(desc));
		Level_goals.GoalGetCompletionMessage(m_current_goal, message, sizeof(message));

		make_goal_dialog_text(big_text_message);
	}
	else
	{
		num_goals = 0;
		num_items = 0;

		flags = 0;
		priority = 0;
		g_list = 0;

		strcpy(name, "No goals");
		strcpy(iname, "No goals");
		strcpy(desc, "No goals");
		strcpy(message, "No goals");

		strcpy(big_text_message, "No goals");
	}

	if(ValidateCurrentItem())
	{
		Level_goals.GoalItemInfo(m_current_goal, m_current_item, LO_GET_SPECIFIED, &item_type, &item_handle, NULL);
	}
	else
	{
		item_type = LIT_OBJECT;
		item_handle = OBJECT_HANDLE_NONE;
	}

	PrintToDlgItem(this, IDC_FIXED_FFT1_EDIT, "%f", force_field_bounce_multiplier[0]);
	PrintToDlgItem(this, IDC_FFT2_EDIT, "%f", force_field_bounce_multiplier[1]);
	PrintToDlgItem(this, IDC_FFT3_EDIT, "%f", force_field_bounce_multiplier[2]);
	PrintToDlgItem(this, IDC_LEVEL_GRAVITY_EDIT, "%f", -Gravity_strength);
	PrintToDlgItem(this, IDC_LEVEL_GOAL_NUM_GOALS_STATIC, "Number of goals: %d", num_goals);
	PrintToDlgItem(this, IDC_LEVEL_GOAL_CUR_GOAL_STATIC, "Current goal: %d", m_current_goal + 1);
	PrintToDlgItem(this, IDC_LEVEL_GOAL_NUM_ITEMS_STATIC, "Number of items: %d", num_items);
	PrintToDlgItem(this, IDC_LEVEL_GOAL_CUR_ITEM_STATIC, "Current item: %d", m_current_item + 1);
	PrintToDlgItem(this, IDC_GOAL_TABLE_EDIT, big_text_message);
	PrintToDlgItem(this, IDC_LEVEL_CEILING_EDIT, "%f", Ceiling_height);

//	IDC_LEVEL_PREV_GOAL_BUTTON
//	IDC_LEVEL_NEXT_GOAL_BUTTON
//	IDC_LEVEL_NEW_GOAL_BUTTON
	PrintToDlgItem(this, IDC_LEVEL_GOAL_NAME_EDIT, "%s", name);
	PrintToDlgItem(this, IDC_LEVEL_GOAL_LOC_NAME_EDIT, "%s", iname);
	PrintToDlgItem(this, IDC_LEVEL_GOAL_DESC_EDIT, "%s", desc);
	PrintToDlgItem(this, IDC_LEVEL_GOAL_COMPLETION_EDIT, "%s", message);
//	IDC_LEVEL_GOAL_NEW_ITEM_BUTTON
//	IDC_LEVEL_GOAL_NEXT_ITEM_BUTTON
//	IDC_LEVEL_GOAL_PREV_ITEM_BUTTON
//	IDC_LEVEL_GOAL_DELETE_ITEM_BUTTON
	bbox = (CButton *)GetDlgItem(IDC_LEVEL_GOAL_ROOM_ITEM_RADIO);
	bbox->SetCheck(item_type == LIT_INTERNAL_ROOM);
	bbox = (CButton *)GetDlgItem(IDC_LEVEL_GOAL_CELL_ITEM_RADIO);
	bbox->SetCheck(item_type == LIT_TERRAIN_CELL);
	bbox = (CButton *)GetDlgItem(IDC_LEVEL_GOAL_OBJ_ITEM_RADIO);
	bbox->SetCheck(item_type == LIT_OBJECT);
	bbox = (CButton *)GetDlgItem(IDC_TRIGGER_RADIO);
	bbox->SetCheck(item_type == LIT_TRIGGER);
//	bbox = (CButton *)GetDlgItem(IDC_GOAL_AUTO_COMPLETE_CHECK);
//	bbox->SetCheck(flags & LGF_AUTO_COMPLETION);
	bbox = (CButton *)GetDlgItem(IDC_LEVEL_GOAL_SEC_GOAL_CHECK);
	bbox->SetCheck(flags & LGF_SECONDARY_GOAL);
	bbox = (CButton *)GetDlgItem(IDC_LEVEL_GOAL_ENABLED_CHECK);
	bbox->SetCheck(flags & LGF_ENABLED);
	bbox = (CButton *)GetDlgItem(IDC_LEVEL_GOAL_COMPLETED_CHECK);
	bbox->SetCheck(flags & LGF_COMPLETED);
	bbox = (CButton *)GetDlgItem(IDC_LEVEL_GOAL_TLG_CHECK);
	bbox->SetCheck(flags & LGF_TELCOM_LISTS);
	bbox = (CButton *)GetDlgItem(IDC_LEVEL_GOAL_GDKL_CHECK);
	bbox->SetCheck(flags & LGF_GB_DOESNT_KNOW_LOC);
	bbox = (CButton *)GetDlgItem(IDC_LEVEL_GOAL_NLB_CHECK);
	bbox->SetCheck(flags & LGF_NOT_LOC_BASED);
//	bbox = (CButton *)GetDlgItem(IDC_LEVEL_GOAL_END_LEVEL_CHECK);
//	bbox->SetCheck(lgflags & LGF_AUTO_END_LEVEL);

	bbox = (CButton *)GetDlgItem(IDC_LG_ACTIVATE_RADIO);
	bbox->SetCheck((flags & LGF_COMP_MASK) == LGF_COMP_ACTIVATE);

	bbox = (CButton *)GetDlgItem(IDC_LG_ENTER_RADIO);
	bbox->SetCheck((flags & LGF_COMP_MASK) == LGF_COMP_ENTER);

	bbox = (CButton *)GetDlgItem(IDC_LEVEL_CEILING_EVERYWHERE);
	bbox->SetCheck(FVI_always_check_ceiling);

	bbox = (CButton *)GetDlgItem(IDC_LEVEL_PIW);
	bbox->SetCheck(Level_powerups_ignore_wind);

	bbox = (CButton *)GetDlgItem(IDC_LG_DESTROYED_RADIO);
	bbox->SetCheck((flags & LGF_COMP_MASK) == LGF_COMP_DESTROY);
	
	bbox = (CButton *)GetDlgItem(IDC_LG_PPW_COLLIDE_RADIO);
	bbox->SetCheck((flags & LGF_COMP_MASK) == LGF_COMP_PLAYER_WEAPON);
	
	bbox = (CButton *)GetDlgItem(IDC_LG_LC_RADIO);
	bbox->SetCheck((flags & LGF_COMP_MASK) == LGF_COMP_PLAYER);
	
	bbox = (CButton *)GetDlgItem(IDC_LG_DALLAS_RADIO);
	bbox->SetCheck((flags & LGF_COMP_MASK) == LGF_COMP_DALLAS);

	if(item_type == LIT_OBJECT)
		m_object_combo.SetSelected(item_handle);
	else
		m_object_combo.SetSelected(-1);

	if(item_type == LIT_INTERNAL_ROOM)
		m_room_combo.SetSelected(item_handle);
	else
		m_room_combo.SetSelected(-1);

	if(item_type == LIT_TRIGGER)
		m_trigger_combo.SetSelected(item_handle);
	else
		m_trigger_combo.SetSelected(-1);

	//	IDC_LEVEL_GOAL_PASTE_OBJREF_ITEM_BUTTON
//	IDC_LEVEL_GOAL_GOTO_ITEM_BUTTON
//	IDC_LEVEL_DELETE_GOAL_BUTTON
//	PrintToDlgItem(this, IDC_LEVEL_GOAL_ITEM_EDIT, "%d", item_handle);
	PrintToDlgItem(this, IDC_LEVEL_GOAL_PRIORITY_EDIT1, "%d", priority);
	PrintToDlgItem(this, IDC_EDIT57, "%d", g_list);

}

void levelkeypad::UpdateKeypad(int mask)
{
	if (mask & KUF_LEVEL_LOADED) {
		m_trigger_combo.Init(-1);
		m_object_combo.Init(OBJ_NONE, OBJECT_HANDLE_NONE);
		m_room_combo.Init(-1);

		m_current_goal = m_current_item = 0;

		UpdateDialog();
	}
}

void levelkeypad::OnKillfocusLevelGravityEdit() 
{
	char str[21];
	((CEdit *) GetDlgItem(IDC_LEVEL_GRAVITY_EDIT))->GetWindowText(str,20);

	Gravity_strength = -atof(str);

	UpdateDialog();	
}

BOOL levelkeypad::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_room_combo.Init(-1);
	m_trigger_combo.Init(-1);
	m_object_combo.Init(OBJ_NONE, OBJECT_HANDLE_NONE);
	UpdateDialog();	
	
	int i;
	SendDlgItemMessage( IDC_FFT1_COMBO, CB_RESETCONTENT,0,0);
	SendDlgItemMessage( IDC_FFT2_COMBO, CB_RESETCONTENT,0,0);
	SendDlgItemMessage( IDC_FFT3_COMBO, CB_RESETCONTENT,0,0);

	SendDlgItemMessage( IDC_FFT1_COMBO, CB_ADDSTRING,0,(LPARAM) (LPCTSTR) "Not Assigned");
	SendDlgItemMessage( IDC_FFT2_COMBO, CB_ADDSTRING,0,(LPARAM) (LPCTSTR) "Not Assigned");
	SendDlgItemMessage( IDC_FFT3_COMBO, CB_ADDSTRING,0,(LPARAM) (LPCTSTR) "Not Assigned");

	for (i = 0;i < MAX_TEXTURES; i++)
	{
		if (GameTextures[i].used)
		{
			SendDlgItemMessage( IDC_FFT1_COMBO, CB_ADDSTRING,0,(LPARAM) (LPCTSTR) GameTextures[i].name);
			SendDlgItemMessage( IDC_FFT2_COMBO, CB_ADDSTRING,0,(LPARAM) (LPCTSTR) GameTextures[i].name);
			SendDlgItemMessage( IDC_FFT3_COMBO, CB_ADDSTRING,0,(LPARAM) (LPCTSTR) GameTextures[i].name);
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void levelkeypad::OnLevelPrevGoalButton() 
{
	int num_goals = Level_goals.GetNumGoals();

	if(m_current_goal > 0)
	{
		m_current_goal--;
	}
	else
	{
		m_current_goal = num_goals - 1;
	}
	m_current_item = 0;

	UpdateDialog();
}

void levelkeypad::OnLevelNextGoalButton() 
{
	int num_goals = Level_goals.GetNumGoals();

	if(m_current_goal < num_goals - 1)
	{
		m_current_goal++;
	}
	else
	{
		m_current_goal = 0;
	}
	m_current_item = 0;

	UpdateDialog();
}

void levelkeypad::OnLevelNewGoalButton() 
{
	m_current_goal = Level_goals.AddGoal(true);
	m_current_item = 0;

	UpdateDialog();

	OnLevelGoalNewItemButton();
}

void levelkeypad::OnLevelDeleteGoalButton() 
{
	Level_goals.DeleteGoal(m_current_goal);
	m_current_goal = 0;
	m_current_item = 0;

	UpdateDialog();
}

void levelkeypad::OnKillfocusLevelGoalNameEdit() 
{
	char str[256];
	
	((CEdit *) GetDlgItem(IDC_LEVEL_GOAL_NAME_EDIT))->GetWindowText(str,255);
	Level_goals.GoalSetName(m_current_goal, str);

	UpdateDialog();
}

void levelkeypad::OnKillfocusLevelGoalLocNameEdit() 
{
	char str[256];
	
	((CEdit *) GetDlgItem(IDC_LEVEL_GOAL_LOC_NAME_EDIT))->GetWindowText(str,255);
	Level_goals.GoalSetItemName(m_current_goal, str);

	UpdateDialog();
}

void levelkeypad::OnKillfocusLevelGoalDescEdit() 
{
	char str[2560];
	
	((CEdit *) GetDlgItem(IDC_LEVEL_GOAL_DESC_EDIT))->GetWindowText(str,2559);
	Level_goals.GoalSetDesc(m_current_goal, str);

	UpdateDialog();
}

void levelkeypad::OnLevelGoalNewItemButton() 
{
	int item;

	item = Level_goals.GoalAddItem(m_current_goal);
	if(item != -1)
	{
		m_current_item = item;
	}
	else
	{
		OutrageMessageBox(MBOX_OK, "Sorry: No more items are allowed for this goal.  See Chris.");
	}

	UpdateDialog();
}

void levelkeypad::OnLevelGoalNextItemButton() 
{
	if(m_current_item < Level_goals.GoalGetNumItems(m_current_goal) - 1)
		m_current_item++;
	else
		m_current_item = 0;

	UpdateDialog();
}

void levelkeypad::OnLevelGoalPrevItemButton() 
{
	if(m_current_item > 0)
		m_current_item--;
	else
		m_current_item = Level_goals.GoalGetNumItems(m_current_goal) - 1;

	UpdateDialog();
}

void levelkeypad::OnLevelGoalDeleteItemButton() 
{
	if(Level_goals.GoalGetNumItems(m_current_goal))
	{
		Level_goals.GoalDeleteItem(m_current_goal, m_current_item);
		m_current_item = 0;
	}

	UpdateDialog();

}

void levelkeypad::OnLevelGoalRoomItemRadio() 
{
	char type = LIT_INTERNAL_ROOM;
	int handle = -1;

	Level_goals.GoalItemInfo(m_current_goal, m_current_item, LO_SET_SPECIFIED, &type, &handle, NULL);
	
	UpdateDialog();
}

void levelkeypad::OnLevelGoalCellItemRadio() 
{
	char type = LIT_TERRAIN_CELL;
	int handle = -1;

	Level_goals.GoalItemInfo(m_current_goal, m_current_item, LO_SET_SPECIFIED, &type, &handle, NULL);
	UpdateDialog();
}

void levelkeypad::OnLevelGoalObjItemRadio() 
{
	char type = LIT_OBJECT;
	int handle = -1;

	Level_goals.GoalItemInfo(m_current_goal, m_current_item, LO_SET_SPECIFIED, &type, &handle, NULL);
	UpdateDialog();
}

void levelkeypad::OnTriggerRadio() 
{
	char type = LIT_TRIGGER;
	int handle = -1;

	Level_goals.GoalItemInfo(m_current_goal, m_current_item, LO_SET_SPECIFIED, &type, &handle, NULL);
	
	UpdateDialog();
}

void levelkeypad::OnLevelGoalMineRadio() 
{
	char type = LIT_ANY_MINE;
	int handle = -1;

	Level_goals.GoalItemInfo(m_current_goal, m_current_item, LO_SET_SPECIFIED, &type, &handle, NULL);
	
	UpdateDialog();
}

void levelkeypad::DoGoalFlagToggle(int flag)
{
	char op = LO_CLEAR_SPECIFIED;
	int flags;
	bool f_set;

	Level_goals.GoalStatus(m_current_goal, LO_GET_SPECIFIED, &flags, true);
	f_set = ((flags & flag) == 0);

	if(f_set)
	{
		op = LO_SET_SPECIFIED;
	}

	Level_goals.GoalStatus(m_current_goal, op, &flag, true);
	UpdateDialog();
}

void levelkeypad::OnGoalAutoCompleteCheck() 
{
	DoGoalFlagToggle(LGF_COMPLETED);
}

void levelkeypad::OnLevelGoalSecGoalCheck() 
{
	DoGoalFlagToggle(LGF_SECONDARY_GOAL);
}

void levelkeypad::OnLevelGoalEnabledCheck() 
{
	DoGoalFlagToggle(LGF_ENABLED);
}

void levelkeypad::OnLevelGoalCompletedCheck() 
{
	DoGoalFlagToggle(LGF_COMPLETED);
}

void levelkeypad::OnLevelGoalTlgCheck() 
{
	DoGoalFlagToggle(LGF_TELCOM_LISTS);
}

void levelkeypad::OnLevelGoalGdklCheck() 
{
	DoGoalFlagToggle(LGF_GB_DOESNT_KNOW_LOC);
}

void levelkeypad::OnLevelGoalNlbCheck() 
{
	DoGoalFlagToggle(LGF_NOT_LOC_BASED);
}

void levelkeypad::OnLevelGoalPasteObjrefItemButton() 
{
	int handle = Copied_object_id;

	Level_goals.GoalItemInfo(m_current_goal, m_current_item, LO_SET_SPECIFIED, NULL, &handle, NULL);
	UpdateDialog();
}

void levelkeypad::OnLevelGoalGotoItemButton() 
{
	char type;
	int handle;

	if(Level_goals.GoalItemInfo(m_current_goal, m_current_item, LO_GET_SPECIFIED, &type, &handle, NULL))
	{
		bool valid = false;
		vector pnt;
		int roomnum;

		MakeBOA();

		switch(type)
		{
			case LIT_OBJECT:
			{
				object *obj = ObjGet(handle);
				if(obj)
				{
					pnt = obj->pos;
					roomnum = obj->roomnum;
					valid = true;
				}
			}
			break;
			case LIT_INTERNAL_ROOM:
			{
				if(handle >= 0 && handle <= Highest_room_index && Rooms[handle].used)
				{
					pnt = Rooms[handle].path_pnt;
					roomnum = handle;
					valid = true;
				}
			}
			break;
			case LIT_TERRAIN_CELL:
			{
				if(handle >= 0 && handle < TERRAIN_WIDTH * TERRAIN_DEPTH)
				{
					ComputeTerrainSegmentCenter(&pnt, handle);
					roomnum = MAKE_ROOMNUM(handle);
					valid = true;
				}
			}
			break;
		}

		if(valid)
		{
			ObjSetPos(Viewer_object, &pnt, roomnum,NULL,false);
			Viewer_moved = 1;
		}
		else
		{
			OutrageMessageBox(MBOX_OK, "Cannot move viewer: This item is not valid.");
		}
	}
	else
	{
		OutrageMessageBox(MBOX_OK, "Cannot move viewer: This item is not defined.");
	}
	
	UpdateDialog();
}


void levelkeypad::OnKillfocusLevelGoalItemEdit() 
{
	int handle;
	char str[256];
	
	((CEdit *) GetDlgItem(IDC_LEVEL_GOAL_ITEM_EDIT))->GetWindowText(str,255);
	sscanf(str, "%d", &handle);
	
	Level_goals.GoalItemInfo(m_current_goal, m_current_item, LO_SET_SPECIFIED, NULL, &handle, NULL);
	UpdateDialog();
}

void levelkeypad::OnKillfocusLevelGoalPriorityEdit1() 
{
	int priority;
	char str[256];
	
	((CEdit *) GetDlgItem(IDC_LEVEL_GOAL_PRIORITY_EDIT1))->GetWindowText(str,255);
	sscanf(str, "%d", &priority);
	
	Level_goals.GoalPriority(m_current_goal, LO_SET_SPECIFIED, &priority);
	UpdateDialog();
}

void levelkeypad::OnLevelGoalEndLevelCheck() 
{
	int flag = 0;//LGF_AUTO_END_LEVEL;
	char op = LO_CLEAR_SPECIFIED;
	int flags;
	bool f_set;

	Level_goals.LGStatus(LO_GET_SPECIFIED, &flags);
	f_set = ((flags & flag) == 0);

	if(f_set)
	{
		op = LO_SET_SPECIFIED;
	}

	Level_goals.LGStatus(op, &flag);
	UpdateDialog();
}

void levelkeypad::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CKeypadDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

void levelkeypad::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CKeypadDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void levelkeypad::OnSize(UINT nType, int cx, int cy) 
{
	CKeypadDialog::OnSize(nType, cx, cy);
}

void levelkeypad::SetAmbType(int i, int n_id)
{
	int cur;
	char name[200];

	cur=SendDlgItemMessage( n_id, CB_GETCURSEL,0,0);
	SendDlgItemMessage( n_id, CB_GETLBTEXT,cur,(LPARAM) (LPCTSTR)name);

	int obj_id = FindObjectIDName(name);
	a_life.SetALValue(i, ALI_TYPE, &obj_id);
		
	UpdateDialog();
}

void levelkeypad::OnSelendokAmbType1() 
{
	SetAmbType(0, IDC_AMB_TYPE1);
}

void levelkeypad::OnSelendokAmbType2() 
{
	SetAmbType(1, IDC_AMB_TYPE2);
}

void levelkeypad::OnSelendokAmbType3() 
{
	SetAmbType(2, IDC_AMB_TYPE3);
}

void levelkeypad::OnSelendokAmbType4() 
{
	SetAmbType(3, IDC_AMB_TYPE4);
}

void levelkeypad::OnSelendokAmbType5() 
{
	SetAmbType(4, IDC_AMB_TYPE5);
}

void levelkeypad::OnSelendokAmbType6() 
{
	SetAmbType(5, IDC_AMB_TYPE6);
}

void levelkeypad::SetAmbCharVal(char index, char item, int n_id)
{
	int ival;
	uint8_t val;
	char str[256];
	
	((CEdit *) GetDlgItem(n_id))->GetWindowText(str,255);
	sscanf(str, "%d", &ival);
	val = ival;

	a_life.SetALValue(index, item, &val);
	UpdateDialog();
}

void levelkeypad::OnKillfocusAmbMax1() 
{
	SetAmbCharVal(0, ALI_MAX, IDC_AMB_MAX1);
}

void levelkeypad::OnKillfocusAmbMax2() 
{
	SetAmbCharVal(1, ALI_MAX, IDC_AMB_MAX2);
}

void levelkeypad::OnKillfocusAmbMax3() 
{
	SetAmbCharVal(2, ALI_MAX, IDC_AMB_MAX3);
}

void levelkeypad::OnKillfocusAmbMax4() 
{
	SetAmbCharVal(3, ALI_MAX, IDC_AMB_MAX4);
}

void levelkeypad::OnKillfocusAmbMax5() 
{
	SetAmbCharVal(4, ALI_MAX, IDC_AMB_MAX5);
}

void levelkeypad::OnKillfocusAmbMax6() 
{
	SetAmbCharVal(5, ALI_MAX, IDC_AMB_MAX6);
}

void levelkeypad::OnKillfocusAmbMin1() 
{
	SetAmbCharVal(0, ALI_MIN, IDC_AMB_MIN1);
}

void levelkeypad::OnKillfocusAmbMin2() 
{
	SetAmbCharVal(1, ALI_MIN, IDC_AMB_MIN2);
}

void levelkeypad::OnKillfocusAmbMin3() 
{
	SetAmbCharVal(2, ALI_MIN, IDC_AMB_MIN3);
}

void levelkeypad::OnKillfocusAmbMin4() 
{
	SetAmbCharVal(3, ALI_MIN, IDC_AMB_MIN4);
}

void levelkeypad::OnKillfocusAmbMin5() 
{
	SetAmbCharVal(4, ALI_MIN, IDC_AMB_MIN5);
}

void levelkeypad::OnKillfocusAmbMin6() 
{
	SetAmbCharVal(5, ALI_MIN, IDC_AMB_MIN6);
}

void levelkeypad::OnKillfocusAmbNGroups1() 
{
	SetAmbCharVal(0, ALI_TOTAL, IDC_AMB_N_GROUPS1);
}

void levelkeypad::OnKillfocusAmbNGroups2() 
{
	SetAmbCharVal(1, ALI_TOTAL, IDC_AMB_N_GROUPS2);
}

void levelkeypad::OnKillfocusAmbNGroups3() 
{
	SetAmbCharVal(2, ALI_TOTAL, IDC_AMB_N_GROUPS3);
}

void levelkeypad::OnKillfocusAmbNGroups4() 
{
	SetAmbCharVal(3, ALI_TOTAL, IDC_AMB_N_GROUPS4);
}

void levelkeypad::OnKillfocusAmbNGroups5() 
{
	SetAmbCharVal(4, ALI_TOTAL, IDC_AMB_N_GROUPS5);
}

void levelkeypad::OnKillfocusAmbNGroups6() 
{
	SetAmbCharVal(5, ALI_TOTAL, IDC_AMB_N_GROUPS6);
}

void levelkeypad::OnAmbOutside1() 
{
	CButton *bbox;
	uint8_t x;

	bbox = (CButton *)GetDlgItem(IDC_AMB_OUTSIDE1);
	x = bbox->GetCheck();

	a_life.SetALValue(0, ALI_FLAGS, &x);
}

void levelkeypad::OnAmbOutside2() 
{
	CButton *bbox;
	uint8_t x;

	bbox = (CButton *)GetDlgItem(IDC_AMB_OUTSIDE2);
	x = bbox->GetCheck();

	a_life.SetALValue(1, ALI_FLAGS, &x);
}

void levelkeypad::OnAmbOutside3() 
{
	CButton *bbox;
	uint8_t x;

	bbox = (CButton *)GetDlgItem(IDC_AMB_OUTSIDE3);
	x = bbox->GetCheck();

	a_life.SetALValue(2, ALI_FLAGS, &x);
}

void levelkeypad::OnAmbOutside4() 
{
	CButton *bbox;
	uint8_t x;

	bbox = (CButton *)GetDlgItem(IDC_AMB_OUTSIDE4);
	x = bbox->GetCheck();

	a_life.SetALValue(3, ALI_FLAGS, &x);
}

void levelkeypad::OnAmbOutside5() 
{
	CButton *bbox;
	uint8_t x;

	bbox = (CButton *)GetDlgItem(IDC_AMB_OUTSIDE5);
	x = bbox->GetCheck();

	a_life.SetALValue(4, ALI_FLAGS, &x);
}

void levelkeypad::OnAmbOutside6() 
{
	CButton *bbox;
	uint8_t x;

	bbox = (CButton *)GetDlgItem(IDC_AMB_OUTSIDE6);
	x = bbox->GetCheck();

	a_life.SetALValue(5, ALI_FLAGS, &x);
}

void levelkeypad::OnKillfocusLevelGoalCompletionEdit() 
{
	char str[2560];
	
	((CEdit *) GetDlgItem(IDC_LEVEL_GOAL_COMPLETION_EDIT))->GetWindowText(str,2559);
	Level_goals.GoalSetCompletionMessage(m_current_goal, str);

	UpdateDialog();
}

void levelkeypad::OnLgActivateRadio() 
{
	int flags = LGF_COMP_MASK;
	Level_goals.GoalStatus(m_current_goal, LO_CLEAR_SPECIFIED, &flags, true);

	flags = LGF_COMP_ACTIVATE;
	Level_goals.GoalStatus(m_current_goal, LO_SET_SPECIFIED, &flags, true);
}

void levelkeypad::OnLgEnterRadio() 
{
	int flags = LGF_COMP_MASK;
	Level_goals.GoalStatus(m_current_goal, LO_CLEAR_SPECIFIED, &flags, true);

	flags = LGF_COMP_ENTER;
	Level_goals.GoalStatus(m_current_goal, LO_SET_SPECIFIED, &flags, true);
}

void levelkeypad::OnLgDestroyedRadio() 
{
	int flags = LGF_COMP_MASK;
	Level_goals.GoalStatus(m_current_goal, LO_CLEAR_SPECIFIED, &flags, true);

	flags = LGF_COMP_DESTROY;
	Level_goals.GoalStatus(m_current_goal, LO_SET_SPECIFIED, &flags, true);
}

void levelkeypad::OnLgPpwCollideRadio() 
{
	int flags = LGF_COMP_MASK;
	Level_goals.GoalStatus(m_current_goal, LO_CLEAR_SPECIFIED, &flags, true);

	flags = LGF_COMP_PLAYER_WEAPON;
	Level_goals.GoalStatus(m_current_goal, LO_SET_SPECIFIED, &flags, true);
}

void levelkeypad::OnLgLcRadio() 
{
	int flags = LGF_COMP_MASK;
	Level_goals.GoalStatus(m_current_goal, LO_CLEAR_SPECIFIED, &flags, true);

	flags = LGF_COMP_PLAYER;
	Level_goals.GoalStatus(m_current_goal, LO_SET_SPECIFIED, &flags, true);
}

void levelkeypad::OnLgDallasRadio() 
{
	int flags = LGF_COMP_MASK;
	Level_goals.GoalStatus(m_current_goal, LO_CLEAR_SPECIFIED, &flags, true);

	flags = LGF_COMP_DALLAS;
	Level_goals.GoalStatus(m_current_goal, LO_SET_SPECIFIED, &flags, true);
}

void levelkeypad::OnSelendokLevelgoalRoomCombo() 
{
	int handle = m_room_combo.GetSelected();

	Level_goals.GoalItemInfo(m_current_goal, m_current_item, LO_SET_SPECIFIED, NULL, &handle, NULL);
	UpdateDialog();
}

void levelkeypad::OnSelendokLevelgoalobjectCombo() 
{
	int handle = m_object_combo.GetSelected();

	Level_goals.GoalItemInfo(m_current_goal, m_current_item, LO_SET_SPECIFIED, NULL, &handle, NULL);
	UpdateDialog();
}

void levelkeypad::OnSelendokLevelgoaltriggerCombo() 
{
	int handle = m_trigger_combo.GetSelected();

	Level_goals.GoalItemInfo(m_current_goal, m_current_item, LO_SET_SPECIFIED, NULL, &handle, NULL);
	UpdateDialog();
}
/*
void levelkeypad::OnKillfocusLevelGoalListEdit() 
{
	int int_g_list;
	char g_list;
	char str[256];
	
	((CEdit *) GetDlgItem(IDC_LEVEL_GOAL_LIST_EDIT))->GetWindowText(str,255);
	sscanf(str, "%d", &int_g_list);

	g_list = (char)int_g_list;
	
	Level_goals.GoalGoalList(m_current_goal, LO_SET_SPECIFIED, &g_list);
	UpdateDialog();
}
*/

void levelkeypad::OnKillfocusEdit57() 
{
	int int_g_list;
	char g_list;
	char str[256];
	
	((CEdit *) GetDlgItem(IDC_EDIT57))->GetWindowText(str,255);
	sscanf(str, "%d", &int_g_list);

	g_list = (char)int_g_list;
	
	Level_goals.GoalGoalList(m_current_goal, LO_SET_SPECIFIED, &g_list);
	UpdateDialog();
}
void levelkeypad::OnLevelCeilingEverywhere() 
{
	if(FVI_always_check_ceiling)
		FVI_always_check_ceiling = false;
	else
		FVI_always_check_ceiling = true;

	UpdateDialog();
}

void levelkeypad::OnKillfocusLevelCeilingEdit() 
{
	char str[21];
	((CEdit *) GetDlgItem(IDC_LEVEL_CEILING_EDIT))->GetWindowText(str,20);

	Ceiling_height = atof(str);

	UpdateDialog();	
}

void levelkeypad::OnSelendokFfosCombo() 
{
	char name[200];

	int cur = SendDlgItemMessage(IDC_FFOS_COMBO,CB_GETCURSEL,0,0);
	SendDlgItemMessage(IDC_FFOS_COMBO,CB_GETLBTEXT,cur,(LPARAM) (LPCTSTR)name);

	if(stricmp(name, "Default") != 0)
		sound_override_force_field = FindSoundName(name);
	else
		sound_override_force_field = -1;

	UpdateDialog();
}

void levelkeypad::OnSelendokBgosCombo() 
{
	char name[200];

	int cur = SendDlgItemMessage(IDC_BGOS_COMBO,CB_GETCURSEL,0,0);
	SendDlgItemMessage(IDC_BGOS_COMBO,CB_GETLBTEXT,cur,(LPARAM) (LPCTSTR)name);

	if(stricmp(name, "Default") != 0)
		sound_override_glass_breaking = FindSoundName(name);
	else
		sound_override_glass_breaking = -1;

	UpdateDialog();
}

void levelkeypad::OnKillfocusFft1Edit() 
{
}

void levelkeypad::OnKillfocusFft2Edit() 
{
	char str[21];
	((CEdit *) GetDlgItem(IDC_FFT2_EDIT))->GetWindowText(str,20);

	force_field_bounce_multiplier[1] = atof(str);

	UpdateDialog();	
}

void levelkeypad::OnSelendokFft1Combo() 
{
	char name[200];

	int cur = SendDlgItemMessage(IDC_FFT1_COMBO,CB_GETCURSEL,0,0);
	SendDlgItemMessage(IDC_FFT1_COMBO,CB_GETLBTEXT,cur,(LPARAM) (LPCTSTR)name);

	if(stricmp(name, "Not Assigned") != 0)
		force_field_bounce_texture[0] = FindTextureName(name);
	else
		force_field_bounce_texture[0] = -1;

	UpdateDialog();
}

void levelkeypad::OnSelendokFft2Combo() 
{
	char name[200];

	int cur = SendDlgItemMessage(IDC_FFT2_COMBO,CB_GETCURSEL,0,0);
	SendDlgItemMessage(IDC_FFT2_COMBO,CB_GETLBTEXT,cur,(LPARAM) (LPCTSTR)name);

	if(stricmp(name, "Not Assigned") != 0)
		force_field_bounce_texture[1] = FindTextureName(name);
	else
		force_field_bounce_texture[1] = -1;

	UpdateDialog();
}

void levelkeypad::OnKillfocusFixedFft1Edit() 
{
	char str[21];
	((CEdit *) GetDlgItem(IDC_FIXED_FFT1_EDIT))->GetWindowText(str,20);

	force_field_bounce_multiplier[0] = atof(str);

	UpdateDialog();	
}

void levelkeypad::OnSetfocusFft1Combo() 
{
	int i;
	
	SendDlgItemMessage( IDC_FFT1_COMBO, CB_RESETCONTENT,0,0);
	SendDlgItemMessage( IDC_FFT1_COMBO, CB_ADDSTRING,0,(LPARAM) (LPCTSTR) "Not Assigned");
	for (i = 0;i < MAX_TEXTURES; i++)
	{
		if (GameTextures[i].used)
		{
			SendDlgItemMessage( IDC_FFT1_COMBO, CB_ADDSTRING,0,(LPARAM) (LPCTSTR) GameTextures[i].name);
		}
	}
}

void levelkeypad::OnSetfocusFft2Combo() 
{
	int i;

	SendDlgItemMessage( IDC_FFT2_COMBO, CB_RESETCONTENT,0,0);
	SendDlgItemMessage( IDC_FFT2_COMBO, CB_ADDSTRING,0,(LPARAM) (LPCTSTR) "Not Assigned");
	for (i = 0;i < MAX_TEXTURES; i++)
	{
		if (GameTextures[i].used)
		{
			SendDlgItemMessage( IDC_FFT2_COMBO, CB_ADDSTRING,0,(LPARAM) (LPCTSTR) GameTextures[i].name);
		}
	}
}

void levelkeypad::OnLevelPiw() 
{
	if(Level_powerups_ignore_wind)
		Level_powerups_ignore_wind = false;
	else
		Level_powerups_ignore_wind = true;

	UpdateDialog();
}

void levelkeypad::OnKillfocusFft3Edit() 
{
	char str[21];
	((CEdit *) GetDlgItem(IDC_FFT3_EDIT))->GetWindowText(str,20);

	force_field_bounce_multiplier[2] = atof(str);

	UpdateDialog();	
}

void levelkeypad::OnSelendokFft3Combo() 
{
	char name[200];

	int cur = SendDlgItemMessage(IDC_FFT3_COMBO,CB_GETCURSEL,0,0);
	SendDlgItemMessage(IDC_FFT3_COMBO,CB_GETLBTEXT,cur,(LPARAM) (LPCTSTR)name);

	if(stricmp(name, "Not Assigned") != 0)
		force_field_bounce_texture[2] = FindTextureName(name);
	else
		force_field_bounce_texture[2] = -1;

	UpdateDialog();
}

void levelkeypad::OnSetfocusFft3Combo() 
{
	int i;

	SendDlgItemMessage( IDC_FFT3_COMBO, CB_RESETCONTENT,0,0);
	SendDlgItemMessage( IDC_FFT3_COMBO, CB_ADDSTRING,0,(LPARAM) (LPCTSTR) "Not Assigned");
	for (i = 0;i < MAX_TEXTURES; i++)
	{
		if (GameTextures[i].used)
		{
			SendDlgItemMessage( IDC_FFT3_COMBO, CB_ADDSTRING,0,(LPARAM) (LPCTSTR) GameTextures[i].name);
		}
	}
}
