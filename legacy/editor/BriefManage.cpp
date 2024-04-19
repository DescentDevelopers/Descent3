// BriefManage.cpp : implementation file
//

#include "stdafx.h"
#include "BriefManage.h"
#include "editor.h"
#include "ddio.h"
#include "manage.h"
#include "pserror.h"
#include "pstypes.h"
#include "gamefile.h"
#include "soundpage.h"
#include "editlinedialog.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBriefManage dialog


CBriefManage::CBriefManage(CWnd* pParent /*=NULL*/)
	: CDialog(CBriefManage::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBriefManage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CBriefManage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBriefManage)
	DDX_Control(pDX, IDC_LIST, m_List);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBriefManage, CDialog)
	//{{AFX_MSG_MAP(CBriefManage)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBriefManage message handlers
bool AddNewGameFile(char *fullpath,char *directory);
bool CheckInGamefile(char *tempbuffer,bool show_ok_confirmation);
void CBriefManage::OnAdd() 
{
	if(!Network_up){
		MessageBox("You need to say YES on the data update when starting the editor");
		return;
	}

	int sel = m_List.GetCurSel();
	if(sel==LB_ERR){
		MessageBox("Please select the file you want to add");
		return;
	}

	char full_buffer[_MAX_PATH+100];
	m_List.GetText(sel,full_buffer);

	int page_type = -1;
	char *ptr;

	//break it down
	ptr = full_buffer + strlen(full_buffer);

	while( ptr>full_buffer && (*ptr)!='|') ptr--;

	if(*ptr!='|'){
		MessageBox("Internal Error");
		Int3();//get jeff;
		return;
	}

	*ptr = '\0';
	ptr++;

	if(page_type==-1 && strstr(ptr,"[sound]"))
		page_type=PAGETYPE_SOUND;
	if(page_type==-1 && strstr(ptr,"[file]"))
		page_type=PAGETYPE_GAMEFILE;

	if(page_type==-1){
		MessageBox("Internal Error");
		Int3();	//get Jeff
		return;
	}

	if(page_type==PAGETYPE_SOUND){
		MessageBox("You must add sounds in the sound dialog");
		return;
	}

	switch(page_type)
	{
	case PAGETYPE_GAMEFILE:
		{
			char filename[255];
			char real_filename[_MAX_PATH];

			strcpy(filename,full_buffer);

			// Get the filename of the representing image
			CString filter = "";

			if (!OpenFileDialog(this, (LPCTSTR)filter, filename, Current_files_dir, sizeof(Current_files_dir))) 
				return;

			char directory[32],ext[32];
			bool know_dir = false;

			ddio_SplitPath(filename,NULL,real_filename,ext);
			strcat(real_filename,ext);

			if(!know_dir && !stricmp(ext,".ogf")){
				know_dir = true;
				strcpy(directory,"graphics");
			}

			if(!know_dir && !stricmp(ext,".tga")){
				know_dir = true;
				strcpy(directory,"graphics");
			}

			if(!know_dir && !stricmp(ext,".pcx")){
				know_dir = true;
				strcpy(directory,"graphics");
			}
			
			if(!know_dir && !stricmp(ext,".oof")){
				know_dir = true;
				strcpy(directory,"models");
			}

			if(!know_dir){
				CEditLineDialog dlg("What directory does this belong in?","Directory", "graphics", false, this);
				if(dlg.DoModal()!=IDOK)
					return;

				strcpy(directory,dlg.GetText());

				bool is_ok = false;

				if(!is_ok && !stricmp(directory,"briefings") )
					is_ok = true;

				if(!is_ok && !stricmp(directory,"graphics") )
					is_ok = true;

				if(!is_ok && !stricmp(directory,"levels") )
					is_ok = true;

				if(!is_ok && !stricmp(directory,"misc") )
					is_ok = true;

				if(!is_ok && !stricmp(directory,"models") )
					is_ok = true;

				if(!is_ok && !stricmp(directory,"music") )
					is_ok = true;

				if(!is_ok && !stricmp(directory,"scripts") )
					is_ok = true;

				if(!is_ok && !stricmp(directory,"sounds") )
					is_ok = true;

				if(!is_ok){
					MessageBox("Invalid directory");
					return;
				}
			}

			if(!AddNewGameFile(filename,directory)){
				char buffer[_MAX_PATH*2];
				sprintf(buffer,"Unable to add:\r\n%s\r\nto %s",filename,directory);
				MessageBox(buffer);
				return;
			}

			//auto checkin 
			if (!mng_MakeLocker()){
				return;
			}

			if(!CheckInGamefile(real_filename,true)){
				MessageBox("Unable to auto-checkin");
			}

			mng_EraseLocker();
		}break;
	}

	UpdateDialog();	
}

BOOL CBriefManage::OnInitDialog() 
{
	CDialog::OnInitDialog();

	UpdateDialog();

	return TRUE;
}

void CBriefManage::AddString(char *filename,int pagetype)
{
	char buffer[_MAX_PATH+100];
	char real_filename[_MAX_PATH];
	char page_type[100];
	char *pagename = filename;

	switch(pagetype)
	{
	case PAGETYPE_SOUND:
		{
			//look for the page in the manage system
			bool found;

			found = false;		

			mngs_sound_page page;

			if(mng_FindSpecificSoundPage(filename,&page )>0)
			{
				found = true;
			}

			if(!found){
				//ok there wasn't one in the gamefiles make sure there isn't a tracklocked
				if(mng_FindTrackLock(filename,PAGETYPE_SOUND)!=-1){
					//there isn't one in the track lock...this is our first compile!
					found = true;
				}
			}

			if(found){
				//we already have this in the manage system
				return;
			}


			strcpy(page_type,"[sound]");
		}break;
	case PAGETYPE_GAMEFILE:
		{
			//look for the page in the manage system
			bool found;
			int i;

			char ext[32];
			ddio_SplitPath(filename,NULL,real_filename,ext);
			strcat(real_filename,ext);
			pagename = real_filename;

			found = false;		
			for (i=0;i<MAX_GAMEFILES;i++){
				if (Gamefiles[i].used && (!stricmp(real_filename,Gamefiles[i].name)) ){
					found = true;
					break;
				}
			}

			if(!found){
				//ok there wasn't one in the gamefiles make sure there isn't a tracklocked
				if(mng_FindTrackLock(real_filename,PAGETYPE_GAMEFILE)!=-1){
					//there isn't one in the track lock...this is our first compile!
					found = true;
				}
			}

			if(found){
				//we already have this in the manage system
				return;
			}


			strcpy(page_type,"[file]");
		}		
		break;
	default:
		Int3();
		return;
	}

	sprintf(buffer,"%s|%s",pagename,page_type);

	//see if it's already in the list
	if(m_List.FindStringExact(-1,buffer)==LB_ERR)
	{
		m_List.AddString(buffer);
	}
}

void CBriefManage::UpdateDialog(void)
{
	m_List.ResetContent();

	int curr_screen = Briefing_root_screen;
	int curr_effect;
	tBriefScreen *screen;
	tBriefEffect *effect;
	
	//go through and see what we have
	while(curr_screen!=-1)
	{
		screen = &Briefing_screens[curr_screen];

		ASSERT(screen->used);

		if(screen->layout[0]!='\0'){
			//add the layout screen
			AddString(screen->layout);
		}

		//go through all the effects for this screen
		curr_effect = screen->root_effect;
		while(curr_effect!=-1)
		{
			effect = &screen->effects[curr_effect];
			ASSERT(effect->used);

			switch(effect->type){
			case BE_MOVIE:
			case BE_TEXT:
			case BE_BKG:
				break;	//no files in here

			case BE_BMP:
				if(*effect->bmp_desc.filename)
				{
					AddString(effect->bmp_desc.filename);
				}
				break;
		
			case BE_POLY:
				if(*effect->poly_desc.polyname)
				{
					AddString(effect->poly_desc.polyname);
				}
				break;
			case BE_SND:
				if(*effect->snd_desc.filename)
				{
					AddString(effect->snd_desc.filename,PAGETYPE_SOUND);
				}
				break;
			case BE_BUTTON:
				if(*effect->button_desc.filename)
				{
					AddString(effect->button_desc.filename);
				}

				if(*effect->button_desc.filename_focus)
				{
					AddString(effect->button_desc.filename_focus);
				}

				if(*effect->button_desc.flash_filename)
				{
					AddString(effect->button_desc.flash_filename);
				}

				if(*effect->button_desc.flash_filename_focus)
				{
					AddString(effect->button_desc.flash_filename_focus);
				}

				break;
			default:
				break;
			}


			curr_effect = effect->next;			
		}	

		curr_screen = screen->next;
	}

	m_List.SetCurSel(0);
}