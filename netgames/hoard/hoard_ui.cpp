/*
* $Logfile: /DescentIII/Main/hoard/hoard_ui.cpp $
* $Revision: 1.1.1.1 $
* $Date: 2003/08/26 03:57:53 $
* $Author: kevinb $
*
* Hoard UI Dialogs
*
* $Log: hoard_ui.cpp,v $
* Revision 1.1.1.1  2003/08/26 03:57:53  kevinb
* initial 1.5 import
*
 * 
 * 10    5/19/99 2:42a Jeff
 * call correct NewUIGameWindow functions
 * 
 * 9     4/24/99 11:13p Jeff
 * added flags for creating a newui window for a title bar
 * 
 * 8     4/14/99 1:17a Jeff
 * fixed case mismatched #includes
 * 
 * 7     4/03/99 9:26p Jeff
 * changed dialogs that weren't using UID_OK and UID_CANCEL to use and
 * handle them properly
 * 
 * 6     3/17/99 12:23p Jeff
 * converted DMFC to be COM interface
 * 
 * 5     2/11/99 3:54p Jeff
 * localized
 * 
 * 4     2/07/99 1:18a Jeff
 * peppered UI dialogs that were missing NEWUIRES_FORCEQUIT to handle it
 * 
 * 3     11/20/98 5:52p Jeff
 * updated hyper anarchy and hoard a little
 * 
 * 2     11/19/98 5:56p Jeff
 * added slider exported and improved Hoard
*
* $NoKeywords: $
*/

#include "gamedll_header.h"
#include <string.h>
#include "idmfc.h"
#include "hoardstr.h"
#include "hoardaux.h"
extern IDMFC *DMFCBase;

// Displays the Hoard game configuration dialog (Server)
void DisplayHoardConfigDialog(tGameConfig *config)
{
#define WINDOW_H	152
#define WINDOW_W	352

	if(DMFCBase->GetLocalRole()!=LR_SERVER)
		return;

	if(DMFCBase->IAmDedicatedServer())
		return;

	if(config->min_hoard<1) config->min_hoard = 1;
	if(config->min_hoard>12) config->min_hoard = 12;

	//DMFCBase->MakeClientsWait(true);

	// variable declarations
	// ---------------------
	bool exit_menu = false;
	char buffer[10];
	sprintf(buffer,"%d",config->min_hoard);

	// text items
	// ----------
	void *start_game_text_off	= DLLCreateNewUITextItem(TXT_RETURNTOGAME,UICOL_HOTSPOT_LO,-1);
	void *start_game_text_on	= DLLCreateNewUITextItem(TXT_RETURNTOGAME,UICOL_HOTSPOT_HI,-1);
	void *window_title			= DLLCreateNewUITextItem(TXT_GAMECONFIG,UICOL_WINDOW_TITLE,-1);
	void *min_count_label_ti	= DLLCreateNewUITextItem(TXT_MINIMUMORBS,UICOL_TEXT_NORMAL,-1);
	void *slider_val_ti			= DLLCreateNewUITextItem(buffer,UICOL_TEXT_NORMAL,-1);

	// main window
	// -----------
	void *main_wnd			= DLLNewUIGameWindowCreate(0,0,WINDOW_W,WINDOW_H,UIF_PROCESS_ALL|UIF_CENTER|NUWF_TITLEMED);

	// item placement
	// --------------
	int cury = 35;
	int left_x_col = 20;
	
	void *start_game_hs		= DLLHotSpotCreate(main_wnd,UID_OK,K_ENTER,start_game_text_off,start_game_text_on,40,WINDOW_H - OKCANCEL_YOFFSET,130,15,UIF_CENTER);
	void *window_title_text = DLLTextCreate(main_wnd,window_title,0,7,UIF_CENTER|UIF_FIT);
	void *min_count_label	= DLLTextCreate(main_wnd,min_count_label_ti,left_x_col,cury,UIF_FIT|UIF_CENTER); cury+=15;
	void *count_slider		= DLLSliderCreate(main_wnd,31,0,cury,UIF_CENTER|UIF_FIT); cury+=30;
	DLLSliderSetRange(count_slider,11);
	DLLSliderSetPos(count_slider,config->min_hoard-1);
	void *slider_val		= DLLTextCreate(main_wnd,slider_val_ti,left_x_col,cury,UIF_FIT|UIF_CENTER);

	// show window
	// -----------
	DLLNewUIGameWindowOpen(main_wnd);

	// process
	// -------
	while (!exit_menu) 
	{
		int res;
		res = DLLPollUI();

		if(res==-1)
			continue;

		switch(res){
		case NEWUIRES_FORCEQUIT:
			exit_menu = true;
			break;
		case UID_OK:
			exit_menu = true;
			break;
		case 31:
			{//update slider
				DLLRemoveUITextItem(slider_val_ti);
				char buff[10];
				int val = DLLSliderGetPos(count_slider);
				sprintf(buff,"%d",val+1);
				slider_val_ti = DLLCreateNewUITextItem(buff,UICOL_TEXT_NORMAL,-1);
				DLLTextSetTitle(slider_val,slider_val_ti);
			}break;
		}
	}

	config->min_hoard = DLLSliderGetPos(count_slider) + 1;

	// close window
	// ------------
	DLLNewUIGameWindowClose(main_wnd);

	// free up resources
	// -----------------
	DLLNewUIGameWindowDestroy(main_wnd);

	DLLRemoveUITextItem(start_game_text_on);
	DLLRemoveUITextItem(start_game_text_off);
	DLLRemoveUITextItem(window_title);
	DLLRemoveUITextItem(min_count_label_ti);
	DLLRemoveUITextItem(slider_val_ti);

	DLLDeleteUIItem(main_wnd);
	DLLDeleteUIItem(start_game_hs);
	DLLDeleteUIItem(window_title_text);
	DLLDeleteUIItem(min_count_label);
	DLLDeleteUIItem(count_slider);
	DLLDeleteUIItem(slider_val);

	// we're done
	// ----------
	//DMFCBase->MakeClientsWait(false);

	SendGameConfig(SP_ALL);
} 