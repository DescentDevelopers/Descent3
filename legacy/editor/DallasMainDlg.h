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

--- HISTORICAL COMMENTS FOLLOW ---

* $Logfile: /DescentIII/Main/editor/DallasMainDlg.h $
* $Revision: 1.1.1.1 $
* $Date: 2003-08-26 03:57:37 $
* $Author: kevinb $
*
* This is the header for the main dialog of the DALLAS graphical scripting interface
*
* $Log: not supported by cvs2svn $
 * 
 * 67    8/30/99 4:11p Nate
 * Added copy/paste for conditions, log ops, and nested if-then clauses
 * 
 * 66    5/04/99 6:53p Jeff
 * added event for when a player dies
 * 
 * 65    4/27/99 4:27p Jeff
 * added player respawn osiris event
 * 
 * 64    4/15/99 9:47p Nate
 * Upped max number of enum values to 100
 * 
 * 63    4/14/99 6:03p Nate
 * Added Event Type specification to Dallas Script Highlighting system.
 * 
 * 62    4/02/99 2:16p Nate
 * Added AI Init event
 * 
 * 61    3/30/99 4:47p Jeff
 * added level events for when IGC occurs for a player
 * 
 * 60    3/11/99 10:51a Nate
 * Added deletion of "Untitled" Dallas files when doing a New from editor
 * 
 * 59    2/22/99 1:17p Nate
 * Added 'level goal item complete' event
 * 
 * 58    2/22/99 1:19a Jeff
 * added handling for evt_use
 * 
 * 57    2/20/99 6:07p Nate
 * Added Level Goal events
 * 
 * 56    2/19/99 5:35p Nate
 * Added new types and events
 * 
 * 55    2/17/99 4:14p Nate
 * Added condition query shortcuts
 * 
 * 54    2/08/99 3:53p Nate
 * Added new event types
 * 
 * 53    2/07/99 4:43p Nate
 * Added OWNER selection to Door parameter types
 * 
 * 52    2/05/99 11:52a Nate
 * Added importing/exporting of scripts
 * 
 * 51    2/03/99 7:20p Nate
 * Fixed clipboard bug and added script chaining option
 * 
 * 50    2/03/99 2:19p Nate
 * Added cool drag-n-drop support
 * 
 * 49    2/03/99 11:57a Nate
 * Added script highlighting interface
 * 
 * 48    2/02/99 7:41p Nate
 * Added columnization of popup menus
 * 
 * 47    2/01/99 2:42p Nate
 * Added title bar display of level and modified indicator
 * 
 * 46    1/29/99 4:41p Nate
 * A few minor tweaks and fixes
 * 
 * 45    1/28/99 9:41p Nate
 * Added tons of new stuff
 * 
 * 44    1/26/99 1:15p Nate
 * Added UserTypes Workshop implementation
 * 
 * 43    1/25/99 7:16p Nate
 * Added UserTypes dialog (non-functional)
 * 
 * 42    1/25/99 2:38p Nate
 * Added Valid Flag Mask range support.
 * 
 * 41    1/25/99 9:52a Nate
 * Enabled Save Format Version 1
 * 
 * 40    1/24/99 4:49p Nate
 * Added code for Dallas Save Format Version 1, but didn't enable it yet
 * 
 * 39    1/23/99 5:52p Nate
 * Added flag support
 * 
 * 38    1/21/99 8:57p Nate
 * Added warnings when saving objects
 * 
 * 37    1/20/99 3:51p Nate
 * Added Specific Name parameter type 'a'
 * 
 * 36    1/19/99 7:37p Nate
 * Added sound selection prompt dialog
 * 
 * 35    1/19/99 12:18p Nate
 * Made it so Owner objects can now be "other"
 * 
 * 34    1/19/99 10:35a Nate
 * Fixed Other object submenu bug
 * 
 * 33    1/18/99 2:29p Nate
 * Added support for default parameters and valid range specification
 * 
 * 32    1/17/99 6:29p Nate
 * Layed groundwork for default parameter values and valid parameter
 * ranges
 * 
 * 31    1/15/99 7:31p Nate
 * Added some more interface features/fixes
 * 
 * 30    1/15/99 2:05p Nate
 * Added collapse/expand all, fixed me and delete problems, made dlg
 * longer
 * 
 * 29    1/15/99 10:37a Nate
 * Fixed highest room index bug
 * 
 * 28    1/14/99 6:11p Nate
 * Added Trigger Script support and many other things, too numerous to
 * count.
 * 
 * 27    1/13/99 7:28p Nate
 * Added message file reading from created script
 * 
 * 26    1/13/99 10:50a Nate
 * Fixed up copy/pase, added highlighting of all scripts matching
 * specified owner
 * 
 * 25    1/12/99 7:32p Nate
 * Added copy and paste support
 * 
 * 24    1/12/99 4:45p Nate
 * Added max exec time support, added more interface features
 * 
 * 23    1/11/99 8:42p Nate
 * Added script parameter support
 * 
 * 22    1/11/99 6:40p Nate
 * Added ENUM support
 * 
 * 21    1/11/99 10:19a Nate
 * Fixed AND/OR insertion bug
 * 
 * 20    1/08/99 12:32p Nate
 * Added glue function validation upon loading scripts
 * 
 * 19    1/07/99 10:17p Nate
 * Added first round of script loading code...
 * 
 * 18    1/06/99 7:09p Nate
 * Added saving of scripts
 * 
 * 17    1/06/99 1:13p Nate
 * Added support for IT objects and ELSE clauses
 * 
 * 16    1/05/99 8:00p Nate
 * Added conditional code creation... fixed up interface yet a little
 * more.
 * 
 * 15    1/05/99 2:03p Nate
 * Fixed up events and conditional interface a little
 * 
 * 14    1/04/99 7:34p Nate
 * Added rough interface for Conditions
 * 
 * 13    12/23/98 6:44p Nate
 * Added reading in of queries
 * 
 * 12    12/23/98 4:03p Nate
 * Added code to implement as a modeless dialog
 * 
 * 11    12/23/98 12:35p Nate
 * Added use of level name in Dallas script file naming system.
 * 
 * 10    12/22/98 3:55p Nate
 * Added object and room assignments
 * 
 * 9     12/21/98 8:01p Nate
 * Added creation of shell code, started conditional interface.
 * 
 * 8     12/20/98 4:29p Nate
 * Added script grouping code (based upon owner, then event type)
 * 
 * 7     12/18/98 6:54p Nate
 * Added more interface features.
 * 
 * 6     12/17/98 9:48p Nate
 * Added editing of paramaters
 * 
 * 5     12/16/98 8:45p Nate
 * Added loading of Actions
 * 
 * 4     12/15/98 7:47p Nate
 * Added inserting and deleting of default script trees.
 * 
 * 3     12/13/98 6:08p Nate
 * Implemented the Message List interface
 * 
 * 2     12/11/98 6:45p Nate
 * Initial Version
*
* $NoKeywords: $
*/


#if !defined(AFX_DALLASMAINDLG_H__93285122_90E1_11D2_A4E0_00A0C96ED60D__INCLUDED_)
#define AFX_DALLASMAINDLG_H__93285122_90E1_11D2_A4E0_00A0C96ED60D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DallasMainDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// The Current Save Version Number
/////////////////////////////////////////////////////////////////////////////
#define DALLAS_SAVE_VERSION			3

/////////////////////////////////////////////////////////////////////////////
// Message List structure and constants
/////////////////////////////////////////////////////////////////////////////

#define MAX_MESSAGE_LIST_ENTRIES		500
#define MAX_MESSAGE_LEN					256
#define MAX_MESSAGE_NAME_LEN			45

#define NO_MSG_LIST_ERROR				0
#define MSG_LIST_FULL_ERROR			1
#define MSG_LIST_DUP_NAME_ERROR		2

#define DEFAULT_MESSAGE_NAME			"Message"
#define DEFAULT_MESSAGE_TEXT			""

struct tMessageListEntry {
	char	name[MAX_MESSAGE_NAME_LEN+1];		// the named identifier of the message
	char	message[MAX_MESSAGE_LEN+1];		// the message text
};


/////////////////////////////////////////////////////////////////////////////
// Tree Data Node structure and constants
/////////////////////////////////////////////////////////////////////////////

#define NOT_SPECIFIED_TYPE		-1
#define OBJECT_NONE_HANDLE		-2
#define OBJECT_IT_HANDLE		-3
#define OBJECT_ME_HANDLE		-4
#define ENUM_GOALID_VALUE		-5
#define ENUM_TIMERID_VALUE		-6
#define ENUM_MATCENID_VALUE	-7
#define MATCEN_ID_VALUE			-8
#define LEVEL_GOAL_ID_VALUE	-9

// The types of Tree Nodes
#define SCRIPT_HEADER_NODE					0
#define SCRIPT_OWNER_NODE					1
#define SCRIPT_EVENT_NODE					2
#define CONDITIONAL_HEADER_NODE			3
#define ACTIONS_HEADER_NODE				4
#define CONDITIONAL_STATEMENT_NODE		5
#define EXPRESSION_NODE						6
#define EXPRESSION_OPERATOR_NODE			7
#define ACTION_STATEMENT_NODE				8
#define LOGICAL_OPERATOR_NODE				9
#define PARAMETER_NODE						10
#define PARAMETER_OPERATOR_NODE			11
#define CLIPBOARD_HEADER_NODE				12
#define UNKNOWN_NODE							13

// Logical Operator Types
#define AND_TYPE					0
#define OR_TYPE					1

#define MAX_LOGICAL_OPERATORS				2

// Conditional Statement Types
#define ALWAYS_STATEMENT				0
#define BINARY_STATEMENT				1
#define COMPARISON_STATEMENT			2

#define MAX_CONDITION_TYPES			3

#define ALWAYS_STRING					"ALWAYS"

// Parameter Types
#define DOOR_PARAMETER_TYPE			0
#define OBJECT_PARAMETER_TYPE			1
#define ROOM_PARAMETER_TYPE			2
#define TRIGGER_PARAMETER_TYPE		3
#define INT_PARAMETER_TYPE				4
#define BOOL_PARAMETER_TYPE			5
#define FLOAT_PARAMETER_TYPE			6
#define VECTOR_PARAMETER_TYPE			7
#define STRING_PARAMETER_TYPE			8
#define PERCENTAGE_PARAMETER_TYPE	9
#define ENUM_PARAMETER_TYPE			10
#define SCRIPT_PARAMETER_TYPE			11
#define SOUND_PARAMETER_TYPE			12
#define SPECNAME_PARAMETER_TYPE		13
#define TEXTURE_PARAMETER_TYPE		14
#define FLAG_PARAMETER_TYPE			15
#define PATH_PARAMETER_TYPE			16
#define MATCEN_PARAMETER_TYPE			17
#define LEVEL_GOAL_PARAMETER_TYPE	18
#define STRM_AUDIO_PARAMETER_TYPE	19
#define UNKNOWN_PARAMETER_TYPE		20

#define MAX_LITERALS						20

#define ANY_PARAMETER_TYPE				21

// Expression Types
#define LITERAL_VALUE_TYPE				0
#define QUERY_TYPE						1

// Expression Operator Types
#define BINARY_OPERATOR_TYPE			0
#define COMPARISON_OPERATOR_TYPE		1

// Expression Operators (Comparison)
#define EQUAL_TO							0
#define NOT_EQUAL_TO						1
#define GREATER_THAN						2
#define LESS_THAN							3
#define GREATER_THAN_OR_EQUAL_TO		4
#define LESS_THAN_OR_EQUAL_TO			5

// Expression Operators (Binary)
#define IS_TRUE							6
#define IS_FALSE							7

#define MAX_EXPRESSION_OPERATORS		8

// Struct for storing expression operator information
struct ExpOpInfoItem {
	int			type;				// expression operator (see above)
	char			*name;			// the displayed name
	char			*menu_name;		// the menu name
	char			*code_name;		// DEFINE string to identify event in code
	int			op_type;			// whether it is a binary or comparison op
};


// Script Owner Types
#define TRIGGER_TYPE				0
#define OBJECT_TYPE				1
#define LEVEL_TYPE				2

#define ROOM_TYPE					3	// Not an owner type, but it is a named type
#define SOUND_TYPE				4	// also not an owner type
#define PATH_TYPE					5	// also not an owner type
#define MATCEN_TYPE				6	// also not an owner type
#define LEVEL_GOAL_TYPE			7	// also not an owner type

#define ALL_OBJECTS_TYPE		10		// Used for highlighting purposes only
#define ALL_TRIGGERS_TYPE		11		// Used for highlighting purposes only
#define NONE_TYPE					12		// Used for highlighting purposes only
#define ALL_OWNERS_TYPE			13		// Used for highlighting purposes only

// Script Event Types - ONLY ADD TO END OF LIST!!!!
// (NOTE: when making changes, remember to update event_info[] in the main source file!)
#define COLLIDE_EVENT_TYPE					0
#define ACTIVATED_EVENT_TYPE				1
#define CREATED_EVENT_TYPE					2
#define DAMAGED_EVENT_TYPE					3
#define DESTROYED_EVENT_TYPE				4
#define FRAME_INTERVAL_EVENT_TYPE		5
#define LEVEL_START_EVENT_TYPE			6
#define LEVEL_END_EVENT_TYPE				7
#define CHANGE_SEGMENT_EVENT_TYPE		8
#define TIMER_EVENT_TYPE					9
#define AIN_OBJKILLED_EVENT_TYPE			10
#define AIN_SEEPLAYER_EVENT_TYPE			11
#define AIN_WHITOBJECT_EVENT_TYPE		12
#define AIN_GOALCOMPLETE_EVENT_TYPE		13
#define AIN_GOALFAIL_EVENT_TYPE			14
#define AIN_MELHIT_EVENT_TYPE				15
#define AIN_MELATTACKFRAME_EVENT_TYPE	16
#define AIN_MOVIE_STARTED_EVENT_TYPE	17
#define AIN_MOVIE_ENDED_EVENT_TYPE		18
#define DOOR_CLOSED_EVENT_TYPE			19
#define DOOR_ACTIVATED_EVENT_TYPE		20
#define MATCEN_NOTIFY_EVENT_TYPE			21
#define CHILD_DIED_EVENT_TYPE				22
#define LGOAL_COMPLETED_EVENT_TYPE		23
#define ALL_LGOALS_COMPLETE_EVENT_TYPE	24	
#define USED_EVENT_TYPE						25
#define LGOAL_ITEM_COMPLETE_EVENT_TYPE	26
#define PLAYER_MOVIE_START_TYPE			27
#define PLAYER_MOVIE_END_TYPE				28
#define AI_INIT_EVENT_TYPE					29
#define PLAYER_RESPAWN_TYPE					30
#define PLAYER_DIES_TYPE					31
#define MAX_EVENT_TYPES						32

#define ALL_EVENT_TYPES						99		// Used for highlighting purposes only

// Struct for storing event information
struct EventInfoItem {
	int			type;				// event type (see above)
	char			*name;			// the displayed name
	char			*code_name;		// DEFINE string to identify event in code
	char			*data_line;		// The line for this event's data struct
	int			flags;			// owner and various flags
};

// Constants for masking what events go with what owner
#define OBJECT_MASK			0x001
#define TRIGGER_MASK			0x002
#define LEVEL_MASK			0x004

#define HAS_IT_MASK			0x008
#define HAS_GOALID_MASK		0x010
#define HAS_TIMERID_MASK	0x020
#define HAS_MATCENID_MASK	0x040

#define DOOR_OBJECT_MASK	0x080

#define HAS_LEVGOALID_MASK	0x100


// Struct for easily creating literal menu
struct ParamMenuItem {
	int type;		// the type of literal (parameter type)
	char *name;		// menu name of the literal
};


// Nested Types
#define TOP_LEVEL		0
#define NESTED			1

// Clause Types
#define THEN_CLAUSE	0
#define ELSE_CLAUSE	1

// Max Execution Time options
#define ENTER_EXEC_TIMES			0
#define EXECUTE_INFINITELY			1
#define EXECUTE_ONCE					2
#define EXECUTE_TWICE				3

#define MAX_EXEC_TIME_OPTIONS		4

// Continue Chain Options
#define CONTINUE_SCRIPT_CHAIN		0
#define BREAK_SCRIPT_CHAIN			1

#define MAX_CHAIN_OPTIONS			2

// Object Parameter Indicators
#define USE_OBJECT_HANDLE	0
#define USE_IT_HANDLE		1
#define USE_ME_HANDLE		2

// Enum Parameter Indicators
#define USE_ENUM_VALUE		0
#define USE_GOALID_VALUE	1
#define USE_TIMERID_VALUE	2
#define USE_MATCENID_VALUE	3

// Matcen Parameter Indicators
#define USE_MATCEN_VALUE		0
#define USE_MATCEN_EVENT_ID	1

// Level Goal Parameter Indicators
#define USE_LEVEL_GOAL_VALUE	0
#define USE_LEVEL_GOAL_ID		1

#define DEFAULT_SCRIPT_HEADER_STRING	"Script Description Goes Here"
#define NOT_SPECIFIED_STRING				"NOT SPECIFIED"
#define NONE_STRING							"None"

#define MAX_NDATA_NAME_LEN			64

#define MAX_STRING_DISPLAY_LEN	20

struct tTreeNodeData {
	int			type;											// the node type (see above list)
	char			name[MAX_NDATA_NAME_LEN+1];			// multipurpose name string
	int			ID;											// multipurpose identifier
	int			subID;										// multipurpose sub-identifier
	int			int_val;										// stores an integer value
	float			float_val1;									// stores a float value type (Vector X)
	float			float_val2;									// stores a float value type (Vector Y)
	float			float_val3;									// stores a float value type (Vector Z)
	char			str_val[MAX_MESSAGE_NAME_LEN+1];		// stores the named identifier of a message
	int			flags;										// various flags
};

/**********************************************************
* Tree Node Data Specifications
***********************************************************

	SCRIPT_HEADER_NODE
		name:				the script header description entered by user
		ID:				the ID number given to the script

	SCRIPT_OWNER_NODE	
		ID:				the type of owner (object, trigger, or level)
		int_val:			the handle or ID number for the owner

	SCRIPT_EVENT_NODE
		ID:				the type of event

	CONDITIONAL_HEADER_NODE
		ID:				whether it is TOP_LEVEL or NESTED
		
	ACTIONS_HEADER_NODE
		ID:				whether it is TOP_LEVEL or NESTED
		for TOP_LEVEL:
			subID:		max times to execute actions (only valid for TOP_LEVEL, 0 implies infinity)
			int_val:		whether it should
		for NESTED:
			subID:		whether it is a THEN_CLAUSE or an ELSE_CLAUSE
			int_val:		unused

	CONDITIONAL_STATEMENT_NODE
		ID:				the type of conditional (always, binary, or comparison statement)

	EXPRESSION_NODE
		ID:				stores index into the Query Database
		name:				the parameter name accepting the query's return value

	EXPRESSION_OPERATOR_NODE
		ID:				the type of operation (binary or comparison)
		subID:			the specific operation (<,=,>, etc)

	ACTION_STATEMENT_NODE
		ID:				index into the Action Database (or DO_NOTHING_ID)

	LOGICAL_OPERATOR_NODE
		ID:				the type of operator (AND, OR)

	PARAMETER_NODE
		name:				the parameter name for this value
		ID:				the type of parameter this node is storing
		for DOOR_PARAMETER_TYPE:
			subID:		indicates whether object should USE_OBJECT_HANDLE or USE_IT_HANDLE or USE_ME_HANDLE
			int_val:		stores the object handle for the door
			str_val:		stores the name of the door
		for OBJECT_PARAMETER_TYPE:
			subID:		indicates whether object should USE_OBJECT_HANDLE or USE_IT_HANDLE or USE_ME_HANDLE
			int_val:		stores the object handle
			str_val:		stores the name of the object
		for ROOM_PARAMETER_TYPE:
			int_val:		stores an index/ID for the room
			str_val:		stores the name of the room
		for TRIGGER_PARAMETER_TYPE:
			int_val:		stores an index/ID for the trigger
			str_val:		stores the name of the trigger
		for INT_PARAMETER_TYPE:
			int_val:		stores an integer value
		for BOOL_PARAMETER_TYPE:
			int_val:		stores either TRUE or FALSE
		for FLOAT_PARAMETER_TYPE:
			float_val1:	stores a float value
		for VECTOR_PARAMETER_TYPE:
			float_val1:	stores X value
			float_val2:	stores Y value
			float_val3:	stores Z value
		for STRING_PARAMETER_TYPE:
			str_val:		stores the name (identifier) of a string in the message list
		for PERCENTAGE_PARAMETER_TYPE:
			float_val1:	stores a percentage as a float value (ie 0.5 == 50%)	
		for ENUM_PARAMETER_TYPE:
			subID:		indicates whether enum should USE_ENUM_VALUE or USE_GOALID_VALUE or USE_TIMERID_VALUE or USE_MATCENID_VALUE
			int_val:		stores the value for the enumeration (keyed on the value in name)
		for SCRIPT_PARAMETER_TYPE:
			int_val:		stores the script ID
		for SOUND_PARAMETER_TYPE:
			int_val:		stores the sound array index position
			str_val:		stores the name of the sound
		for SPECNAME_PARAMETER_TYPE:
			str_val:		stores the specific string
		for TEXTURE_PARAMETER_TYPE:
			int_val:		stores the texture array index position
			str_val:		stores the name of the sound
		for FLAG_PARAMETER_TYPE:
			int_val:		stores the value for the flags (keyed on the value in name)
		for PATH_PARAMETER_TYPE:
			int_val:		stores an index/ID for the path
			str_val:		stores the name of the path
		for MATCEN_PARAMETER_TYPE:
			subID:		indicates whether matcen should USE_MATCEN_VALUE or USE_MATCEN_EVENT_ID
			int_val:		stores an index/ID for the matcen
			str_val:		stores the name of the matcen
		for LEVEL_GOAL_PARAMETER_TYPE:
			subID:		stores whether goal should USE_LEVEL_GOAL_VALUE or USE_LEVEL_GOAL_ID
			int_val:		stores an index/ID for the goal
			str_val:		stores the name of the goal
		for STRM_AUDIO_PARAMETER_TYPE:
			str_val:		stores the name of the streaming audio file

	PARAMETER_OPERATOR_NODE

	CLIPBOARD_HEADER_NODE

	UNKNOWN_NODE
*************************************************************/


/////////////////////////////////////////////////////////////////////////////
// Enumeration structures and constants
/////////////////////////////////////////////////////////////////////////////

#define MAX_ENUMS				50
#define MAX_ENUM_VALUES		100

#define TAG_ENUM				"$$ENUM"
#define TAG_USERTYPE			"$$USERTYPE"

#define USERTYPE_NONE		"None"

#define INVALID_ENUM			-1

struct tEnumValueEntry {
	char	*name;		// the name of this enum value
	int	value;		// the value bound to this name
};

struct tEnumDBEntry {
	char					*name;							// the name for this enumeration type
	tEnumValueEntry	values[MAX_ENUM_VALUES];	// the enumerated value storage
	int					num_values;						// the number of values stored
	bool					is_user_type;					// indicates if it's a user type
	int					max_values;						// max allowed value for user types
};


/////////////////////////////////////////////////////////////////////////////
// Flag structures and constants
/////////////////////////////////////////////////////////////////////////////

#define MAX_FLAGS				50
#define MAX_FLAG_VALUES		32

#define TAG_FLAG				"$$FLAG"

#define INVALID_FLAG			-1

struct tFlagValueEntry {
	char	*name;		// the name of this flag value
	int	value;		// the value bound to this name
};

struct tFlagDBEntry {
	char					*name;							// the name for this flag type
	tFlagValueEntry	values[MAX_FLAG_VALUES];	// the flag value storage
	int					num_values;						// the number of values stored
};


/////////////////////////////////////////////////////////////////////////////
// Action and Query Database structures and constants
/////////////////////////////////////////////////////////////////////////////

#define DO_NOTHING_ID				-1
#define DO_NOTHING_STRING			"DO NOTHING"

#define MAX_ACTIONS					500
#define MAX_QUERIES					500
#define MAX_CATEGORIES				32

#define INVALID_CATEGORY			-1

#define INVALID_FUNCTION_NAME		"InvalidFunctionName"
#define INVALID_FUNCTION_ID		-2

#define TAG_CAT						"$$CATEGORIES"
#define TAG_ACTION					"$$ACTION"
#define TAG_QUERY						"$$QUERY"
#define TAG_END						"$$END"

#define NO_MEM_ERR					0		// out of memory (malloc failed)
#define UEOF_ERR						1		// unexpected end of file
#define INV_CAT_ERR					2		// invalid category assigned
#define INV_ENUM_ERR					3		// invalid enum assigned (duplicate name)
#define INV_ENUM_VALUE_ERR			4		// invalid enum value assigned (duplicate)
#define MAX_ENUM_ERR					5		// the max number of enums has been reached
#define MAX_ENUM_VALUES_ERR		6		// the max number of enum values has been reached
#define INV_FLAG_ERR					7		// invalid flag assigned (duplicate name)
#define INV_FLAG_VALUE_ERR			8		// invalid flag value assigned (duplicate)
#define MAX_FLAG_ERR					9		// the max number of flags has been reached
#define MAX_FLAG_VALUES_ERR		10		// the max number of flag values has been reached

//Structure that defines an action
struct tActionDBEntry {
	int  category;	//Which group this action belongs to
	char *desc;		//What the user sees
	char *func;		//The glue function to call for this action
	char *help;		//The help info for this function
};

//Structure that defines a query
struct tQueryDBEntry {
	int  category;	//Which group this query belongs to
	char *desc;		//What the user sees
	char *func;		//The glue function to call for this query
	char *help;		//The help info for this function
};

/////////////////////////////////////////////////////////////////////////////
// Script Sorting/Organization Structures and Constants
/////////////////////////////////////////////////////////////////////////////

struct tEventSection {
	HTREEITEM	*script_node_list;		// The list of scripts of this event type
	int			num_script_nodes;			// The number of scripts in the list
};


struct tScriptOwnerGroup {
	int				owner_type;								// The owner type for this script group
	int				owner_handle;							// The specific handle/ID for this script group
	tEventSection	event_sections[MAX_EVENT_TYPES];	// The event section script lists
};


/////////////////////////////////////////////////////////////////////////////
// File parsing constants
/////////////////////////////////////////////////////////////////////////////

// Message File Related
#define MAX_MSG_FILE_BUFFER_LEN		(MAX_MESSAGE_LEN+MAX_MESSAGE_NAME_LEN)+1024
#define NEXT_MSG_ID_NUM_KEYWORD		"NEXT_MESSAGE_ID_NUMBER"

// Usertype value block tags
#define USERTYPE_VALS_START_TAG		"$$UTYPE_VALS_START"
#define USERTYPE_VALS_END_TAG			"$$UTYPE_VALS_END"

// Script Source File Related
#define SCRIPT_BLOCK_START_TAG		"$$SCRIPT_BLOCK_START"
#define SCRIPT_BLOCK_END_TAG			"$$SCRIPT_BLOCK_END"
#define CHILD_BLOCK_START_TAG			"$$CHILD_BLOCK_START"
#define CHILD_BLOCK_END_TAG			"$$CHILD_BLOCK_END"

// Name List Constants
#define DOOR_LIST_START_TAG			"$$DOOR_LIST_START"
#define DOOR_LIST_END_TAG				"$$DOOR_LIST_END"

#define OBJECT_LIST_START_TAG			"$$OBJECT_LIST_START"
#define OBJECT_LIST_END_TAG			"$$OBJECT_LIST_END"

#define ROOM_LIST_START_TAG			"$$ROOM_LIST_START"
#define ROOM_LIST_END_TAG				"$$ROOM_LIST_END"

#define TRIGGER_LIST_START_TAG		"$$TRIGGER_LIST_START"
#define TRIGGER_LIST_END_TAG			"$$TRIGGER_LIST_END"

#define SOUND_LIST_START_TAG			"$$SOUND_LIST_START"
#define SOUND_LIST_END_TAG				"$$SOUND_LIST_END"

#define TEXTURE_LIST_START_TAG		"$$TEXTURE_LIST_START"
#define TEXTURE_LIST_END_TAG			"$$TEXTURE_LIST_END"

#define SPECNAME_LIST_START_TAG		"$$SPECNAME_LIST_START"
#define SPECNAME_LIST_END_TAG			"$$SPECNAME_LIST_END"

#define PATH_LIST_START_TAG			"$$PATH_LIST_START"
#define PATH_LIST_END_TAG				"$$PATH_LIST_END"

#define MATCEN_LIST_START_TAG			"$$MATCEN_LIST_START"
#define MATCEN_LIST_END_TAG			"$$MATCEN_LIST_END"

#define GOAL_LIST_START_TAG			"$$GOAL_LIST_START"
#define GOAL_LIST_END_TAG				"$$GOAL_LIST_END"

#define STRM_AUDIO_LIST_START_TAG	"$$STRM_AUDIO_LIST_START"
#define STRM_AUDIO_LIST_END_TAG		"$$STRM_AUDIO_LIST_END"


// Custom Source Related
#define CUSTOM_FUNCTION_PROTOTYPE_BEGIN_TAG				"<CUSTOM_FUNCTION_PROTOTYPE_START>"
#define CUSTOM_FUNCTION_PROTOTYPE_END_TAG					"<CUSTOM_FUNCTION_PROTOTYPE_END>"

// Script Library File Parsing
#define SCRIPT_START_TAG				"$$SCRIPT"
#define SCRIPT_END_TAG					"$$END"

// Script Parsing Errors
#define UEOS_ERR				0			// The script file ended unexpectedly
#define INVALID_FUNC_ERR	1			// An invalid function name was encountered
#define INVALID_NODE_ERR	2			// A node was not added due to parse error(s)
#define MODIFIED_FUNC_ERR	3			// A function is now different than the one read in
#define INVALID_MSGID_ERR	4			// An invalid message ID was encountered

// Custom Script Block Lines
#define CUSTOM_SCRIPT_BLOCK_START_TAG		"/**{CUSTOM_SCRIPT_BLOCK_START}** DO NOT EDIT! **/"
#define CUSTOM_SCRIPT_BLOCK_END_TAG			"/**{CUSTOM_SCRIPT_BLOCK_END}**** DO NOT EDIT! **/"


/////////////////////////////////////////////////////////////////////////////
// Floating Menu Command Ranges
/////////////////////////////////////////////////////////////////////////////
// IMPORTANT NOTE:
//
//		When adding a new command to the following menu system, it is vital
//		that you tack on the number of ID's used to the following section's
//		STARTING_COMMAND_ID!!!!
//
//		This was done due to the compiler's limit on enclosing parentheses
/////////////////////////////////////////////////////////////////////////////

// Define maximum ranges of values for the different lists
#define MAX_NAMED_OBJECTS					1000
#define MAX_NAMED_TRIGGERS					100
#define MAX_NAMED_DOORS						500
#define MAX_NAMED_ROOMS						500
#define MAX_NAMED_SCRIPTS					500
#define MAX_NAMED_SOUNDS					700
#define MAX_NAMED_TEXTURES					500
#define MAX_NAMED_PATHS						250
#define MAX_NAMED_MATCENS					250
#define MAX_NAMED_GOALS						250
#define MAX_NAMED_STRM_AUDIO				700
#define MAX_SPECNAMES						300

// Define command ID starting point
#define STARTING_COMMAND_ID							(WM_USER+10000)

// Define external DALLAS commands
#define WM_HIGHLIGHT_SCRIPTS							STARTING_COMMAND_ID
#define WM_ADD_SCRIPT									(WM_HIGHLIGHT_SCRIPTS+1)
#define WM_ADD_SCRIPT_AND_HIGHLIGHT					(WM_ADD_SCRIPT+1)

// Define universal CANCEL command
#define CANCEL_COMMAND_ID								(WM_ADD_SCRIPT_AND_HIGHLIGHT+1)

// Define the command ID range for a "Assign" operation
#define ASSIGN_STARTING_COMMAND_ID					(STARTING_COMMAND_ID+4)
#define ASSIGN_COMMAND_RANGE_START					ASSIGN_STARTING_COMMAND_ID
#define ASSIGN_SCRIPT_ID_RANGE_START				ASSIGN_COMMAND_RANGE_START
#define ASSIGN_SCRIPT_ID_RANGE_END					(ASSIGN_SCRIPT_ID_RANGE_START+MAX_NAMED_SCRIPTS)
#define ASSIGN_EXEC_TIME_ID_RANGE_START			(ASSIGN_SCRIPT_ID_RANGE_END+1)
#define ASSIGN_EXEC_TIME_ID_RANGE_END				(ASSIGN_EXEC_TIME_ID_RANGE_START+MAX_EXEC_TIME_OPTIONS)
#define ASSIGN_CHAIN_ID_RANGE_START					(ASSIGN_EXEC_TIME_ID_RANGE_END+1)
#define ASSIGN_CHAIN_ID_RANGE_END					(ASSIGN_CHAIN_ID_RANGE_START+MAX_CHAIN_OPTIONS)
#define ASSIGN_SPECIFIC_VALUE_ID						(ASSIGN_CHAIN_ID_RANGE_END+1)
#define ASSIGN_TRUE_ID									(ASSIGN_SPECIFIC_VALUE_ID+1)
#define ASSIGN_FALSE_ID									(ASSIGN_TRUE_ID+1)
#define ASSIGN_MESSAGE_ID_RANGE_START				(ASSIGN_FALSE_ID+1)
#define ASSIGN_MESSAGE_ID_RANGE_END					(ASSIGN_MESSAGE_ID_RANGE_START+MAX_MESSAGE_LIST_ENTRIES)
#define ASSIGN_EVENT_ID_RANGE_START					(ASSIGN_MESSAGE_ID_RANGE_END+1)
#define ASSIGN_EVENT_ID_RANGE_END					(ASSIGN_EVENT_ID_RANGE_START+MAX_EVENT_TYPES)
#define ASSIGN_LEVEL_ID									(ASSIGN_EVENT_ID_RANGE_END+1)
#define ASSIGN_TRIGGER_ID_RANGE_START				(ASSIGN_LEVEL_ID+1)
#define ASSIGN_TRIGGER_ID_RANGE_END					(ASSIGN_TRIGGER_ID_RANGE_START+MAX_NAMED_TRIGGERS)
#define ASSIGN_OBJECT_ME_ID							(ASSIGN_TRIGGER_ID_RANGE_END+1)
#define ASSIGN_OBJECT_IT_ID							(ASSIGN_OBJECT_ME_ID+1)
#define ASSIGN_OBJECT_NONE_ID							(ASSIGN_OBJECT_IT_ID+1)
#define ASSIGN_OBJECT_ID_RANGE_START				(ASSIGN_OBJECT_NONE_ID+1)
#define ASSIGN_OBJECT_ID_RANGE_END					(ASSIGN_OBJECT_ID_RANGE_START+MAX_NAMED_OBJECTS)
#define ASSIGN_ROOM_ID_RANGE_START					(ASSIGN_OBJECT_ID_RANGE_END+1)
#define ASSIGN_ROOM_ID_RANGE_END						(ASSIGN_ROOM_ID_RANGE_START+MAX_NAMED_ROOMS)
#define ASSIGN_EXPOP_ID_RANGE_START					(ASSIGN_ROOM_ID_RANGE_END+1)
#define ASSIGN_EXPOP_ID_RANGE_END					(ASSIGN_EXPOP_ID_RANGE_START+MAX_EXPRESSION_OPERATORS)
#define ASSIGN_ENUM_GOAL_ID							(ASSIGN_EXPOP_ID_RANGE_END+1)
#define ASSIGN_ENUM_TIMER_ID							(ASSIGN_ENUM_GOAL_ID+1)
#define ASSIGN_ENUM_MATCEN_ID							(ASSIGN_ENUM_TIMER_ID+1)
#define ASSIGN_ENUM_VALUE_ID_RANGE_START			(ASSIGN_ENUM_MATCEN_ID+1)
#define ASSIGN_ENUM_VALUE_ID_RANGE_END				(ASSIGN_ENUM_VALUE_ID_RANGE_START+MAX_ENUM_VALUES)
#define ASSIGN_PATH_ID_RANGE_START					(ASSIGN_ENUM_VALUE_ID_RANGE_END+1)
#define ASSIGN_PATH_ID_RANGE_END						(ASSIGN_PATH_ID_RANGE_START+MAX_NAMED_PATHS)
#define ASSIGN_MATCEN_EVENT_ID						(ASSIGN_PATH_ID_RANGE_END+1)
#define ASSIGN_MATCEN_ID_RANGE_START				(ASSIGN_MATCEN_EVENT_ID+1)
#define ASSIGN_MATCEN_ID_RANGE_END					(ASSIGN_MATCEN_ID_RANGE_START+MAX_NAMED_MATCENS)
#define ASSIGN_LEVEL_GOAL_ID							(ASSIGN_MATCEN_ID_RANGE_END+1)
#define ASSIGN_LEVEL_GOAL_ID_RANGE_START			(ASSIGN_LEVEL_GOAL_ID+1)
#define ASSIGN_LEVEL_GOAL_ID_RANGE_END				(ASSIGN_LEVEL_GOAL_ID_RANGE_START+MAX_NAMED_GOALS)
#define ASSIGN_COMMAND_RANGE_END						(ASSIGN_LEVEL_GOAL_ID_RANGE_END+1)

// Define all command ID range for an "Add" (also serves as "Insert") operation
#define ADD_STARTING_COMMAND_ID						(ASSIGN_STARTING_COMMAND_ID+MAX_NAMED_SCRIPTS+MAX_EXEC_TIME_OPTIONS+MAX_CHAIN_OPTIONS+MAX_MESSAGE_LIST_ENTRIES+MAX_EVENT_TYPES+MAX_NAMED_TRIGGERS+MAX_NAMED_OBJECTS+MAX_NAMED_ROOMS+MAX_EXPRESSION_OPERATORS+MAX_ENUM_VALUES+MAX_NAMED_PATHS+MAX_NAMED_MATCENS+MAX_NAMED_GOALS+26)
#define ADD_COMMAND_RANGE_START						ADD_STARTING_COMMAND_ID		
#define ADD_IF_THEN_CLAUSE_ID							ADD_COMMAND_RANGE_START
#define ADD_ELSE_CLAUSE_ID								(ADD_IF_THEN_CLAUSE_ID+1)
#define ADD_IF_THEN_ELSE_CLAUSE_ID					(ADD_ELSE_CLAUSE_ID+1)
#define ADD_DO_NOTHING_ACTION_ID						(ADD_IF_THEN_ELSE_CLAUSE_ID+1)
#define ADD_ACTION_ID_RANGE_START					(ADD_DO_NOTHING_ACTION_ID+1)
#define ADD_ACTION_ID_RANGE_END						(ADD_ACTION_ID_RANGE_START+MAX_ACTIONS)
#define ADD_LOGOP_ID_RANGE_START						(ADD_ACTION_ID_RANGE_END+1)
#define ADD_LOGOP_ID_RANGE_END						(ADD_LOGOP_ID_RANGE_START+MAX_LOGICAL_OPERATORS)
#define INSERT_LOGOP_ID_RANGE_START					(ADD_LOGOP_ID_RANGE_END+1)
#define INSERT_LOGOP_ID_RANGE_END					(INSERT_LOGOP_ID_RANGE_START+MAX_LOGICAL_OPERATORS)
#define ADD_CONDITION_ID_RANGE_START				(INSERT_LOGOP_ID_RANGE_END+1)
#define ADD_CONDITION_ID_RANGE_END					(ADD_CONDITION_ID_RANGE_START+MAX_CONDITION_TYPES)
#define ADD_COND_QBIN_ID_RANGE_START				(ADD_CONDITION_ID_RANGE_END+1)
#define ADD_COND_QBIN_ID_RANGE_END					(ADD_COND_QBIN_ID_RANGE_START+MAX_QUERIES)
#define ADD_COND_QCOMP_ID_RANGE_START				(ADD_COND_QBIN_ID_RANGE_END+1)
#define ADD_COND_QCOMP_ID_RANGE_END					(ADD_COND_QCOMP_ID_RANGE_START+MAX_QUERIES)
#define ADD_COMMAND_RANGE_END							(ADD_COND_QCOMP_ID_RANGE_END+1)

// Define all command ID range for a "Replace" operation
#define REPLACE_STARTING_COMMAND_ID					(ADD_STARTING_COMMAND_ID+MAX_ACTIONS+MAX_LOGICAL_OPERATORS+MAX_LOGICAL_OPERATORS+MAX_CONDITION_TYPES+MAX_QUERIES+MAX_QUERIES+11)
#define REPLACE_COMMAND_RANGE_START					REPLACE_STARTING_COMMAND_ID
#define REPLACE_QUERY_ID_RANGE_START				REPLACE_COMMAND_RANGE_START
#define REPLACE_QUERY_ID_RANGE_END					(REPLACE_QUERY_ID_RANGE_START+MAX_QUERIES)	
#define REPLACE_LITERAL_ID_RANGE_START				(REPLACE_QUERY_ID_RANGE_END+1)
#define REPLACE_LITERAL_ID_RANGE_END				(REPLACE_LITERAL_ID_RANGE_START+MAX_LITERALS)
#define REPLACE_LIT_ENUM_ID_RANGE_START			(REPLACE_LITERAL_ID_RANGE_END+1)
#define REPLACE_LIT_ENUM_ID_RANGE_END				(REPLACE_LIT_ENUM_ID_RANGE_START+MAX_ENUMS)
#define REPLACE_LIT_FLAG_ID_RANGE_START			(REPLACE_LIT_ENUM_ID_RANGE_END+1)
#define REPLACE_LIT_FLAG_ID_RANGE_END				(REPLACE_LIT_FLAG_ID_RANGE_START+MAX_FLAGS)
#define REPLACE_DO_NOTHING_ACTION_ID				(REPLACE_LIT_FLAG_ID_RANGE_END+1)
#define REPLACE_ACTION_ID_RANGE_START				(REPLACE_DO_NOTHING_ACTION_ID+1)
#define REPLACE_ACTION_ID_RANGE_END					(REPLACE_ACTION_ID_RANGE_START+MAX_ACTIONS)
#define REPLACE_LOGOP_ID_RANGE_START				(REPLACE_ACTION_ID_RANGE_END+1)
#define REPLACE_LOGOP_ID_RANGE_END					(REPLACE_LOGOP_ID_RANGE_START+MAX_LOGICAL_OPERATORS)
#define REPLACE_CONDITION_ID_RANGE_START			(REPLACE_LOGOP_ID_RANGE_END+1)
#define REPLACE_CONDITION_ID_RANGE_END				(REPLACE_CONDITION_ID_RANGE_START+MAX_CONDITION_TYPES)
#define REPLACE_COND_QBIN_ID_RANGE_START			(REPLACE_CONDITION_ID_RANGE_END+1)
#define REPLACE_COND_QBIN_ID_RANGE_END				(REPLACE_COND_QBIN_ID_RANGE_START+MAX_QUERIES)
#define REPLACE_COND_QCOMP_ID_RANGE_START			(REPLACE_COND_QBIN_ID_RANGE_END+1)
#define REPLACE_COND_QCOMP_ID_RANGE_END			(REPLACE_COND_QCOMP_ID_RANGE_START+MAX_QUERIES)
#define REPLACE_COMMAND_RANGE_END					(REPLACE_COND_QCOMP_ID_RANGE_END+1)

// Define all command ID range for a "Delete" operation
#define DELETE_STARTING_COMMAND_ID					(REPLACE_STARTING_COMMAND_ID+MAX_QUERIES+MAX_LITERALS+MAX_ENUMS+MAX_FLAGS+MAX_ACTIONS+MAX_LOGICAL_OPERATORS+MAX_CONDITION_TYPES+MAX_QUERIES+MAX_QUERIES+11)
#define DELETE_COMMAND_RANGE_START					DELETE_STARTING_COMMAND_ID
#define DELETE_ALL_ID									DELETE_COMMAND_RANGE_START
#define DELETE_CHILDREN_ONLY_ID						(DELETE_ALL_ID+1)
#define DELETE_COMMAND_RANGE_END						(DELETE_CHILDREN_ONLY_ID+1)


/////////////////////////////////////////////////////////////////////////////
// CDallasMainDlg dialog
/////////////////////////////////////////////////////////////////////////////

typedef char *char_ptr;

class CDallasMainDlg : public CDialog
{
private:

	// Storage for the Loaded script filename
	CString			m_ScriptFilename;
	CString			m_ScriptDLLFilename;
	CString			m_ScriptMessagesFilename;
	CString			m_DallasFunctionsFilename;

	// Modified variable storage
	bool				m_Modified;

	// Declare the image list for the tree control
	CImageList		m_TreeImageList;

	// Store the next message ID to be allocated
	int				m_NextScriptID;

	// The list of Message entries
	tMessageListEntry		*m_MessageEntryList[MAX_MESSAGE_LIST_ENTRIES];
	int						m_MessageListErrorCode;
	int						m_NextMessageID;

	// The enumeration database
	tEnumDBEntry			m_EnumDatabase[MAX_ENUMS];
	int						m_NumEnums;

	// The flag database
	tFlagDBEntry			m_FlagDatabase[MAX_FLAGS];
	int						m_NumFlags;

	// The Action function database
	char						*m_FunctionCategories[MAX_CATEGORIES];
	tActionDBEntry			m_ActionDatabase[MAX_ACTIONS];
	tQueryDBEntry			m_QueryDatabase[MAX_QUERIES];
	int						m_NumFunctionCategories;
	int						m_NumActions;
	int						m_NumQueries;

	// Storage for list of object handles (objects with custom names only)
	int						m_ObjectHandleList[MAX_NAMED_OBJECTS];
	int						m_NumObjectHandles;

	// The list for organizing scripts into owner/event groups (prior to creating source)
	tScriptOwnerGroup		*m_ScriptGroupingList;
	int						m_NumScriptGroups;

	// Clipboard data
	HTREEITEM				m_ClipboardNode;

	// Custom Script Storage Data
	char						**m_CustomScriptLines;
	int						m_NumCustomScriptLines;
	int						m_MaxNumCustomScriptLines;

	// Name Lists
	char						*m_DoorList[MAX_NAMED_DOORS];
	int						m_DoorListSize;
	char						*m_ObjectList[MAX_NAMED_OBJECTS];
	int						m_ObjectListSize;
	char						*m_RoomList[MAX_NAMED_ROOMS];
	int						m_RoomListSize;
	char						*m_TriggerList[MAX_NAMED_TRIGGERS];
	int						m_TriggerListSize;
	char						*m_SoundList[MAX_NAMED_SOUNDS];
	int						m_SoundListSize;
	char						*m_TextureList[MAX_NAMED_TEXTURES];
	int						m_TextureListSize;
	char						*m_SpecnameList[MAX_SPECNAMES];
	int						m_SpecnameListSize;
	char						*m_PathList[MAX_NAMED_PATHS];
	int						m_PathListSize;
	char						*m_MatcenList[MAX_NAMED_MATCENS];
	int						m_MatcenListSize;
	char						*m_GoalList[MAX_NAMED_GOALS];
	int						m_GoalListSize;
	char						*m_StrmAudioList[MAX_NAMED_STRM_AUDIO];
	int						m_StrmAudioListSize;
	char						*m_MessageNameList[MAX_MESSAGE_LIST_ENTRIES];
	int						m_MessageNameListSize;

	// Drag and Drop Vars
	CImageList*				m_pDragImage;	
	BOOL						m_bLDragging;
	HTREEITEM				m_hitemDrag,m_hitemDrop;

public:
	// Construction
	CDallasMainDlg(CWnd* pParent = NULL);   // standard constructor

	///////////////////////////////////
	// Public Interface Data
	///////////////////////////////////
	int m_ScriptOwnerType;
	int m_ScriptOwnerHandle;

	///////////////////////////////////
	// General Refresh functions
	///////////////////////////////////
	void InitAll(void);
	void ClearAll(void);
	void LoadAll(void);
	void SetAllFilenames(void);
	void SetAllFilenamesToThis(char *level_path);
	void Refresh(void);

	///////////////////////////////////
	// Manage System functions
	///////////////////////////////////
	void CheckinScriptFiles(void);
	void CheckoutScriptFiles(void);
	void DeleteScriptFiles(void);

	///////////////////////////////////
	// Tree Control functions
	///////////////////////////////////
	void InitTree(void);		
	void ClearTree(void);

	void UpdateTreeText(HTREEITEM parent);

	void UpdateNodeText(HTREEITEM node);
	void UpdateAllParentNodesText(HTREEITEM node);
	void FormatTreeText(CString &text, tTreeNodeData *data, HTREEITEM node=NULL);
	void FormatConditionText(CString &text, HTREEITEM condition_node);
	void FormatGenericExpressionText(CString &text, HTREEITEM gen_exp_node);
	void FormatParameterValueText(CString &text, tTreeNodeData *data);
	void FormatActionNodeText(CString &text, HTREEITEM action_node);
	void FormatQueryNodeText(CString &text, HTREEITEM query_node);

	void SetBoldNodeText(HTREEITEM node, bool bold_on); 

	void FreeTreeItem(HTREEITEM item);
	void FreeTreeItemChildren(HTREEITEM item);

	void ExpandAll(HTREEITEM node, UINT nCode);		// Works like CTreeCtrl::Expand(), except on child nodes as well

	HTREEITEM AddNodeToTree(HTREEITEM parent, HTREEITEM insertbefore, HTREEITEM src_node, bool expand=FALSE);
	HTREEITEM AddNodeToTree(HTREEITEM parent, HTREEITEM insertbefore, tTreeNodeData *data_node, bool expand=FALSE);

	HTREEITEM CopyTree(HTREEITEM dest_parent, HTREEITEM dest_insert_before, HTREEITEM src_node);
	HTREEITEM CopyChildren(HTREEITEM dest_parent, HTREEITEM src_parent);

	int ConvertParamCharToType(int param_char_ID);
	int ConvertParamTypeToChar(int param_type);

	bool ParentIsComparisonConditional(HTREEITEM node);
	int GetValidParamType(HTREEITEM node, CString &name);
	int GetParamType(HTREEITEM node, CString &name);

	void SetTreeNodeImage(HTREEITEM node);			// Sets the image for a node (based on node data)

	char *GetEventTypeString(int type);
	char *GetEventCodeName(int type);
	char *GetEventDataLine(int type);
	void GetLogicalOperatorTypeString(int type, CString &string);
	char *GetExpressionOperatorTypeString(int type);
	char *GetExpressionOperatorCodeName(int type);
	char *GetLiteralName(int type);

	bool ScriptHasAnIt(HTREEITEM script_node);
	bool ScriptHasAMe(HTREEITEM script_node);
	bool ScriptHasADoorMe(HTREEITEM script_node);
	bool ScriptHasATimerID(HTREEITEM script_node);
	bool ScriptHasAGoalID(HTREEITEM script_node);
	bool ScriptHasAMatcenID(HTREEITEM script_node);
	bool ScriptHasALevelGoalID(HTREEITEM script_node);
	bool NodeIsIfClause(HTREEITEM node);
	bool NodeIsClauseOfType(HTREEITEM node, int type);
	bool CanAppendElseToNode(HTREEITEM node);

	int GetChildPosition(HTREEITEM child);
	HTREEITEM GetNthChild(HTREEITEM parent, int n);
	int GetChildCount(HTREEITEM parent);
	int GetScriptID(HTREEITEM script_node);
	HTREEITEM FindScriptIDNode(int scriptID);
	int GetLowestUnusedScriptID(void);
	int GetNodeType(HTREEITEM node);
	int GetScriptOwnerType(HTREEITEM script_node);
	int GetScriptEventType(HTREEITEM script_node);
	bool SetScriptEventType(HTREEITEM script_node, int type);

	HTREEITEM GetParentNodeOfType(HTREEITEM child_node, int node_type);
	HTREEITEM GetParameterParentNode(HTREEITEM param_node);
	HTREEITEM GetScriptOwnerNode(HTREEITEM node);
	HTREEITEM GetScriptEventNode(HTREEITEM node);
	HTREEITEM GetConditionalHeaderNode(HTREEITEM node);
	HTREEITEM GetActionHeaderNode(HTREEITEM node);

	tTreeNodeData *GetNearestFunctionNode(HTREEITEM node);
	void DisplayFloatingPopupMenu(HTREEITEM node, POINT &pt);
	int DeletePrompt(char *msg);
	void ConfirmAfterDelete(HTREEITEM parent);

	void HighlightAllScripts(void);			// Highlights scripts according to public owner data
	void HighlightScript(HTREEITEM node);	// Highlights a given script according to public owner data

	int ModifiedPrompt(void);			// Displays the "are you sure?" dialog
	void SetModified(bool value);
	void SetTitleBar(void);

	///////////////////////////////////
	// Message List functions
	///////////////////////////////////
	void InitMessageList(void);									// initializes the message list
	void ClearMessageList(void);									// clears the message list
	bool AddToMessageList(char *name, char *message);		// adds a message to the list
	char *FindMessageInList(char *name);						// returns a message matching given name
	int DeleteMessageListEntry(char *name);					// Deletes a message entry
	tMessageListEntry *GetEmptyMessageListEntry(void);		// returns an available entry slot

	///////////////////////////////////////
	// Name List functions
	///////////////////////////////////////
	void InitNameLists(void);
	void ClearNameLists(void);
	int FillNameListsFromTree(HTREEITEM parent, bool show_notspec_warnings);
	int AddNameToListFromTreeNode(HTREEITEM node, bool show_notspec_warnings);

	// Funcs for displaying Indexed value messages
	void InvSpecParamMsg(int scriptID, char *type_name);
	void IndValNotSpecMsg(int scriptID, char *type_name);
	void InvIndValMsg(int scriptID, char *type_name, int index, char *name);
	int InvIndValPrompt(int scriptID, char *type_name, int index, char *name, int new_index);
	void InvNameIndValMsg(int scriptID, char *type_name, int index, char *name, char *new_name);
	int InvNameIndValPrompt(int scriptID, char *type_name, int index, char *name, char *new_name, int new_index);

	// Funcs for displaying object messages
	void InvObjMsg(int scriptID, int handle, char *name);
	int InvObjPrompt(int scriptID, int handle, char *name, int new_handle);
	void InvNameObjMsg(int scriptID, int handle, char *name, char *new_name);
	int InvNameObjPrompt(int scriptID, int handle, char *name, char *new_name, int new_handle);

	// Add/Find funcs for the individual name lists
	int AddDoorToList(char *name);
	int FindDoorInList(char *name);

	int AddObjectToList(char *name);
	int FindObjectInList(char *name);

	int AddRoomToList(char *name);
	int FindRoomInList(char *name);

	int AddTriggerToList(char *name);
	int FindTriggerInList(char *name);

	int AddSoundToList(char *name);
	int FindSoundInList(char *name);

	int AddTextureToList(char *name);
	int FindTextureInList(char *name);

	int AddSpecnameToList(char *name);
	int FindSpecnameInList(char *name);

	int AddPathToList(char *name);
	int FindPathInList(char *name);

	int AddMatcenToList(char *name);
	int FindMatcenInList(char *name);

	int AddGoalToList(char *name);
	int FindGoalInList(char *name);

	int AddStrmAudioToList(char *name);
	int FindStrmAudioInList(char *name);

	int AddMessageNameToList(char *name);
	int FindMessageNameInList(char *name);

	///////////////////////////////////////
	// User Type Workshop Functions
	///////////////////////////////////////
	int FillUserTypeBox(CComboBox *box);
	int FillValuesBox(CListBox *box, char *utype_name);
	int AddUserTypeValue(char *utype_name, char *value_name);
	int DeleteUserTypeValue(char *utype_name, char *value_name);
	int ChangeValueName(char *utype_name, char *old_name, char *new_name);

	///////////////////////////////////////
	// Enum Database functions
	///////////////////////////////////////
	void InitEnumDatabase(void);									// Initializes the enum DB for use
	void ClearEnumDatabase(void);									// Clears any allocated data for the enums
	int GetEnumID(char *name);										// Returns the DB slot matching the given enum type name
	char *GetEnumValueName(char *name, int value);			// Returns the name bound to an enum value

	bool GetEnumValue(char *name, char *value_name, int &value);	// obtains value for given value name

	int FillEnumTypesMenu(CMenu *enum_menu, int command_offset, char *valid_name);
	int FillEnumValuesMenu(CMenu *enum_menu, int command_offset, char *enum_name);

	///////////////////////////////////////
	// Flag Database functions
	///////////////////////////////////////
	void InitFlagDatabase(void);									// Initializes the flag DB for use
	void ClearFlagDatabase(void);									// Clears any allocated data for the flags
	int GetFlagID(char *name);										// Returns the DB slot matching the given flag type name
	char *GetFlagValueName(char *name, int value);			// Returns the name bound to an flag value


	bool FormatFlagValueNames(char *name, int value, CString &text);		// Returns the name bound to an flag value
	bool GetFlagValue(char *name, char *value_name, int &value);			// obtains value for given value name

	int FillFlagTypesMenu(CMenu *flag_menu, int command_offset, char *valid_name);
	int FillFlagValuesBox(CCheckListBox *box, char *flag_name, int flags_value, int valid_flags_mask);

	///////////////////////////////////////
	// Action and Query Database functions
	///////////////////////////////////////
	void InitFunctionDatabases(void);			// Initialize the Action and Query Database Arrays
	void ClearFunctionDatabases(void);			// Frees up any allocated memory
	void ParseFunctionFile(char *filename, bool show_errors=TRUE);	// Parses a file, and adds any Actions and Querys to the database
	char *GetActionDesc(int ID);					// Returns the Action description
	char *GetActionHelp(int ID);					// Returns the Action help text
	char *GetActionFunc(int ID);					// Returns the Action function name
	int GetActionFuncID(char *func_name);		// Searches action list for action matching given function name
	char *GetQueryDesc(int ID);					// Returns the Query description
	char *GetQueryHelp(int ID);					// Returns the Query help text
	char *GetQueryFunc(int ID);					// Returns the Query function name
	int GetQueryFuncID(char *func_name);		// Searches query list for query matching given function name
	int GetQueryReturnType(int ID, CString &name);	// Returns the Query's return type (parameter type)
	int GetFunctionCategoryID(char *catname);	// Matches an integer ID to a given category name

	void FunctionFileParseError(int error_code, int linenum, const char *filename);		// Handle Parse Errors
	void ParseOutActionVarTypes(char *new_desc, char *old_desc);	// Returns a description copy with the parameter type info removed
	void ParseOutQueryVarTypes(char *new_desc, char *old_desc);		// Returns a description copy with the parameter type info removed
	bool ValidateActionNode(HTREEITEM node, int linenum);
	bool ValidateQueryNode(HTREEITEM node, int linenum);
	void FillActionMenu(CMenu *action_menu, int command_offset);
	void FillQueryMenu(CMenu *query_menu, int command_offset, int valid_return_type, char *valid_return_name);
	bool ConformParamNode(HTREEITEM parent_node, HTREEITEM &param_node, int type, char *name, char *def_value=NULL);

	int ParseNthParam(HTREEITEM func_node, int n, CString &name_text, CString &default_text, CString &range_text);
	int ParseNthParam(char *desc, int n, CString &name_text, CString &default_text, CString &range_text);
	int ParseParamBlock(char_ptr &line, CString &name_text, CString &default_text, CString &range_text);

	bool VerifyIntegerRange(int value, char *range_desc);
	bool VerifyFloatRange(float value, char *range_desc);

	///////////////////////////////////
	// Script Grouping List functions
	///////////////////////////////////
	void InitScriptGroupingList(void);
	void ClearScriptGroupingList(void);
	int AddNodeToScriptOwnerGroup(int pos, HTREEITEM script_node);
	int AddNodeToScriptGroupingList(HTREEITEM script_node);
	int BuildScriptGroupingList(void);

	char *CreateScriptConstantName(int pos);
	char *CreateScriptClassName(int pos);
	char *CreateScriptGlobalCtrName(int ID);

	///////////////////////////////////
	// Source File Parsing Functions
	///////////////////////////////////
	int ParseSourceScript(char *filename);						// does high-level parsing of CPP file
	int ParseMsgTableFile(char *filename);						// reads in the message list from a file

	HTREEITEM ParseScriptNodeLine_v0(char *line, int linenum, HTREEITEM parent, bool &skip_all_children, HTREEITEM insert_before=TVI_LAST);
	HTREEITEM ParseScriptNodeLine_v1U(char *line, int linenum, HTREEITEM parent, bool &skip_all_children, int version, HTREEITEM insert_before=TVI_LAST);
	void ScriptFileParseError(int error_code, int linenum, int script_ID, char *name);
	void SpecialScriptFileParseError(int linenum, int script_ID, char *type_name, char *name);
	bool ValidateFunctionNode(HTREEITEM node, int linenum);

	///////////////////////////////////
	// Custom Script Block Functions
	///////////////////////////////////
	void InitCustomScriptStorage(void);
	void ClearCustomScriptStorage(void);
	int CountCustomScriptLines(CFILE *infile);
	int ParseCustomScriptFile(char *filename, bool show_errors=TRUE);
	void WriteCustomScriptBlock(void);

	///////////////////////////////////
	// Source File Creation Functions
	///////////////////////////////////
	int CreateMsgTableFile(char *filename);										// writes message list to file
	void TabOver(void);																	// writes series of tabs
	int CreateScriptFile(char *filename);											// creates the new script file
	void CreateLevelEventCases(tScriptOwnerGroup *owner_group);
	void CreateEventCases(tScriptOwnerGroup *owner_group);					// writes the valid event cases
	void WriteScriptCode(HTREEITEM script_node);									// writes script code for a script node
	void WriteConditionalCodeBlock(HTREEITEM conditional_header_node);	// writes a conditional block segment
	void WriteLogicalOpExpression(HTREEITEM logop_node);						// writes out a logical operation block
	void WriteConditionalStatement(HTREEITEM condition_node);				// writes out a conditional statement
	void WriteQueryFunctionCall(HTREEITEM query_node);							// writes out query function call
	void WriteActionCodeBlock(HTREEITEM action_header_node);					// writes an action block segment
	void WriteActionFunctionCall(HTREEITEM action_node);						// writes out action function call
	void WriteFunctionParameter(HTREEITEM param_node);							// writes out a parameter
	void WriteScriptSaveBlock(void);													// creates the tree block (for saving scripts)
	void WriteScriptTreeDump(void);
	void WriteUserTypeVals(void);
	void WriteNameLists(void);
	void WriteNameListArrays(void);
	void WriteNameArrayLookupCode(void);
	void WriteScriptChildrenDump(HTREEITEM parent, bool filter_on=FALSE);
	void WriteScriptNodeDump_v0(HTREEITEM parent);
	void WriteScriptNodeDump_v1U(HTREEITEM parent, bool filter_on=FALSE);

	///////////////////////////////////
	// Object Handle List Functions
	///////////////////////////////////
	void InitObjectHandleList(void);
	void ClearObjectHandleList(void);
	int AddToObjectHandleList(int handle);

	///////////////////////////////////
	// Floating Menu Creation Functions
	///////////////////////////////////
	void FillObjectMenu(CMenu *object_menu, int command_offset, bool show_other);
	int FillObjectTypeMenu(CMenu *object_menu, int command_offset, int obj_type);
	int FillRoomMenu(CMenu *room_menu, int command_offset);
	int FillSoundsMenu(CMenu *sound_menu, int command_offset);
	int FillTriggerMenu(CMenu *trigger_menu, int command_offset);
	int FillPathMenu(CMenu *path_menu, int command_offset);
	int FillMatcenMenu(CMenu *matcen_menu, int command_offset);
	int FillLevelGoalMenu(CMenu *level_goal_menu, int command_offset);
	int FillScriptMenu(CMenu *script_menu, int command_offset);
	int FillLogOpMenu(CMenu *log_op_menu, int command_offset);
	int FillExpOpMenu(CMenu *exp_op_menu, int command_offset, int op_type);
	void FillConditionMenu(CMenu *condition_menu, int command_offset);
	int FillLiteralMenu(CMenu *literal_menu, int command_offset, int enum_command_offset, int flag_command_offset, int valid_type, char *valid_name);

	void DisplayScriptHeaderNodeMenu(POINT *point);
	void DisplayScriptOwnerNodeMenu(POINT *point);
	void DisplayScriptEventNodeMenu(POINT *point, int owner_type, bool has_a_door_me);
	void DisplayLogicalOperatorNodeMenu(POINT *point);
	void DisplayConditionalStatementNodeMenu(POINT *point);
	void DisplayExpressionNodeMenu(POINT *point, int valid_return_type, char *valid_return_name);
	void DisplayExpressionOperatorNodeMenu(POINT *point, int op_type);
	void DisplayActionStatementNodeMenu(POINT *point);
	void DisplayActionHeaderNodeMenu(POINT *point, int level_type, int clause_type, bool can_add_else);
	void DisplayParameterNodeMenu(POINT *point, int param_type, char *param_name, int valid_return_type, char *valid_return_name, bool has_an_it, bool has_a_me, bool has_a_door_me, bool has_a_goalID, bool has_a_timerID, bool has_a_matcenID, bool has_a_levgoalID);

	void ColumnizePopupMenu(CMenu *menu);

	///////////////////////////////////
	// Functions to Assign Values
	///////////////////////////////////
	void AssignScriptID(int pos);
	void AssignExecTime(int ID);
	void AssignChainOption(int ID);
	void AssignSpecificValue(void);
	void AssignNamedValue(int type, int handle);
	void AssignEventType(int type);
	void AssignBooleanValue(int type);
	void AssignMessageName(int msg_ID);
	void AssignExpOpType(int type);
	void AssignEnumValueType(int type);

	///////////////////////////////////
	// Functions to Add Tree Nodes
	///////////////////////////////////
	void AddNestedIfThenClause(void);
	void AddNestedElseClause(void);
	void AddNestedIfThenElseClause(void);
	void AddConditionalStatementNode(int type, int query_id=-1);
	void AddActionStatementNode(int action_ID);
	void AddActionParameterNodes(HTREEITEM action_node);
	void AddQueryParameterNodes(HTREEITEM query_node);
	void AddLogicalOperatorNode(int type);
	void InsertLogicalOperatorNode(int type);

	///////////////////////////////////
	// Functions to Replace Tree Nodes
	///////////////////////////////////
	void ReplaceWithQueryNode(int query_ID);
	void ReplaceWithLiteralNode(int literal_type, int index);
	void DoComparisonMatchup(HTREEITEM node);
	void ReplaceWithActionNode(int action_ID);
	void ReplaceWithLogOpNode(int type);
	void ReplaceWithConditionNode(int type, int query_id=-1);

	///////////////////////////////////
	// Functions to Add Default Trees
	///////////////////////////////////
	HTREEITEM CreateDefaultScriptTree(int script_ID, HTREEITEM insert_before=NULL);
	HTREEITEM CreateDefaultIfThenClause(HTREEITEM parent, HTREEITEM insert_before, int type);
	HTREEITEM CreateDefaultElseClause(HTREEITEM parent, HTREEITEM insert_before);
	HTREEITEM CreateDefaultConditionalStatementNode(HTREEITEM parent);
	HTREEITEM CreateDefaultConditionalStatementSubtree(HTREEITEM parent, int type, int query_id=-1);
	HTREEITEM CreateDefaultExpressionOperatorNode(HTREEITEM parent, int type);
	HTREEITEM CreateDefaultActionStatementNode(HTREEITEM parent);
	HTREEITEM CreateDefaultParameterNode(HTREEITEM parent, HTREEITEM insert_before, int param_type, char *name, char *def_value=NULL);
	HTREEITEM CreateDefaultClipboardNode(void);

	///////////////////////////////////
	// Functions to Handle Copy/Paste
	///////////////////////////////////
	void PerformScriptCopy(void);
	void PerformActionCopy(HTREEITEM action_node);
	void PerformConditionalCopy(HTREEITEM conditional_node);
	void PerformLogOpCopy(HTREEITEM logop_node);
	void PerformClauseCopy(HTREEITEM clause_node);

	void PerformScriptPaste(void);
	void PerformActionPaste(HTREEITEM src_action_node);
	void PerformConditionalPaste(HTREEITEM src_conditional_node);
	void PerformLogOpPaste(HTREEITEM src_logop_node);
	void PerformClausePaste(HTREEITEM src_clause_node);

	bool NodeIsInClipboard(HTREEITEM node);

	///////////////////////////////////////////////
	// Functions to search tree and replace values
	///////////////////////////////////////////////
	int UpdateStringParams(HTREEITEM root, char *old_name, char *new_name);

	///////////////////////////////////////////
	// Functions to Handle Script Drag-n-Drop
	///////////////////////////////////////////
	bool IsDropSource(HTREEITEM item);
	HTREEITEM GetDropTarget(HTREEITEM item);

	///////////////////////////////////////////
	// Functions to Handle Highlight Interface
	///////////////////////////////////////////
	void ClearHighlightRadioButtons(void);
	void FillHighlightEventList(void);
	int GetHighlightedEvent(void);
	void SetHighlightedEvent(int type);

	/////////////////////////////////////
	// Functions to Handle Import/Export
	/////////////////////////////////////
	bool ScriptLibraryFilePrompt(CString &filename, bool use_import_msg);
	bool ImportScriptFromFile(char *filename, char *script_name);
	bool ExportScriptToFile(char *filename, char *script_name);


// Dialog Data
	//{{AFX_DATA(CDallasMainDlg)
	enum { IDD = IDD_DALLAS_MAIN_DIALOG };
	CComboBox	m_EventList;
	CTreeCtrl	m_ScriptTree;
	CEdit	m_HelpEdit;
	CListBox	m_MessageList;
	CEdit	m_MessageEdit;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDallasMainDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDallasMainDlg)
	afx_msg void OnNewMessageButton();
	afx_msg void OnDeleteMessageButton();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeMessageList();
	afx_msg void OnChangeMessageEdit();
	afx_msg void OnDblclkMessageList();
	afx_msg void OnChangeMessageNameButton();
	virtual void OnOK();
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	virtual void OnCancel();
	afx_msg void OnNewScriptButton();
	afx_msg void OnInsertScriptButton();
	afx_msg void OnDeleteScriptButton();
	afx_msg void OnRclickEventTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkScriptTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangedScriptTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSaveButton();
	afx_msg void OnKeydownEventTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCopyButton();
	afx_msg void OnPasteButton();
	afx_msg void OnHighlightButton();
	afx_msg void OnExpandAllButton();
	afx_msg void OnCollapseAllButton();
	afx_msg void OnUtypesButton();
	afx_msg void OnBegindragEventTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnAllobjRadio();
	afx_msg void OnAlltrigRadio();
	afx_msg void OnNoneRadio();
	afx_msg void OnSpecificRadio();
	afx_msg void OnLevelRadio();
	afx_msg void OnImportButton();
	afx_msg void OnExportButton();
	afx_msg void OnEventRadio();
	afx_msg void OnSelchangeEventCombo();
	//}}AFX_MSG
	afx_msg void OnActivate( UINT nState, CWnd* pWndOther, BOOL bMinimized );
	afx_msg LONG OnHighlightScripts(UINT, LONG);
	afx_msg LONG OnAddScript(UINT, LONG);
	afx_msg LONG OnAddScriptAndHighlight(UINT, LONG);
	afx_msg void OnMenuSelectionOfTypeAssign(UINT nID);
	afx_msg void OnMenuSelectionOfTypeAdd(UINT nID);
	afx_msg void OnMenuSelectionOfTypeReplace(UINT nID);
	afx_msg void OnMenuSelectionOfTypeDelete(UINT nID);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DALLASMAINDLG_H__93285122_90E1_11D2_A4E0_00A0C96ED60D__INCLUDED_)
