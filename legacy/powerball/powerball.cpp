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

#include "windows.h"
#include "gamedll_header.h"
#include <string.h>
//########################Start DMFC Specifics##########################
#include "DMFC.h"
#include "powerball.h"
//#include "tanarchystr.h"
//##########################End DMFC Specifics###########################

//////////////////////////////////
// defines
#define SPID_NEWPLAYER		0
#define SPID_COLLIDE		1

#define POWERBALL_ID_NAME		"Powerball"
#define POWERBALL_SND_SCORE		"Extra life"
#define POWERBALL_SND_PICKUP	""

/*
$$TABLE_GENERIC "Powerball"
$$TABLE_SOUND "Extra life"
*/

//////////////////////////////////
// Structs
struct tPlayerStat {
	int Score[2];
};

//////////////////////////////////
// Globals

int SortedPLRPlayers[DLLMAX_TEAMS][DLLMAX_PLAYERS];
int TeamScores[DLLMAX_TEAMS];
bool DisplayScoreScreen = false;
int NumOfTeams = 2;
int SortedTeams[DLLMAX_TEAMS];
int SortedPlayers[DLLMAX_PLAYERS];
bool players_sorted = false;
int PowerBallID = -1;
int PowerBallRoom = -1;
int PowerBallObjnum = -1;
vector PowerBallPos;
bool DisplayPowerBBlink = true;
bool DisplayScoreBlink = true;
int WhoJustScored = -1,WhoJustScoredTimer=-1;
int WhoJustPowerB = -1,WhoJustPowerBTimer=-1;
int ShieldRegenTimer = -1;
int WhoHasPowerBall = -1;
int RegenTimer = -1;
int GoalRooms[DLLMAX_TEAMS];
int PowerBallIcon = -1;
int PBallHazehandle = -1;
CDmfcStats stat;

//////////////////////////////////
// Prototypes
void DisplayHUDScores(struct tHUDItem *hitem);
void DisplayStats(void);
void GetGameStartPacket(uint8_t *data);
void SendGameStartPacket(int pnum);
bool GetPowerBallInfo(int id);
void UpdatePowerBallEffectOnPlayer(int pnum,bool pickedup);
void SortTeams(void);
void DisplayWelcomeMessage(int player_num);
void OnTimerScore(void);
void OnTimer(void);
void OnTimerScoreKill(void);
void OnTimerKill(void);
void OnTimerRegen(void);
void OnTimerRegenKill(void);
void SaveEndOfLevelStats(void);
void SaveStatsToFile(char *filename);
void HandlePlayerCollideWithPowerball(object *pobj,object *pball);
void HandlePlayerLosingPowerball(object *pball);
void HandleWeaponCollide(object *me_obj,object *it_obj);
void GetCollideInfo(uint8_t *data);
void SendCollideInfo(object *pobj,object *wobj,int towho,vector *point,vector *normal);

void DetermineScore(int precord_num,int column_num,char *buffer,int buffer_size)
{
	player_record *pr = PBall.GetPlayerRecord(precord_num);
	if(!pr || pr->state==STATE_EMPTY){
		buffer[0] = '\0';
		return;
	}

	tPlayerStat *stat = (tPlayerStat *)pr->user_info;
	sprintf(buffer,"%d[%d]",(stat)?stat->Score[DSTAT_LEVEL]:0,(stat)?stat->Score[DSTAT_OVERALL]:0);
}

void TeamScoreCallback(int team,char *buffer,int buffer_size)
{
	ASSERT(team>=0 && team<DLLMAX_TEAMS);

	sprintf(buffer," %d",TeamScores[team]);
}

void ShowStatBitmap(int precord_num,int column_num,int x,int y,int w,int h,uint8_t alpha_to_use)
{
	player_record *pr = PBall.GetPlayerRecord(precord_num);
	
	if(pr->state==STATE_INGAME && WhoHasPowerBall==pr->pnum){
		DLLRenderHUDQuad(x+2,y,10,10,0,0,1,1,PowerBallIcon,alpha_to_use);
	}
}


// This function gets called by the game when it wants to learn some info about the game
void DLLFUNCCALL DLLGetGameInfo (tDLLOptions *options)
{
	options->flags		= DOF_MAXTEAMS;
	options->max_teams	= 2;
	strcpy(options->game_name,"PowerBall");
}

// Initializes the game function pointers
void DLLFUNCCALL DLLGameInit (int *api_func,uint8_t *all_ok)
{
	*all_ok = 1;
	PBall.LoadFunctions(api_func);
	PBall.GameInit(NumOfTeams);
	//DLLCreateStringTable("tanarchy.str",&StringTable,&StringTableSize);
	//DLLmprintf(0,"%d strings loaded from string table\n",StringTableSize);

	//add the death and suicide messages
	PBall.AddDeathMessage("%s was killed by %s",true);

	PBall.AddSuicideMessage("%s is suicidal");

	//setup the Playerstats struct so DMFC can handle it automatically when a new player enters the game
	PBall.SetupPlayerRecord(sizeof(tPlayerStat));

	//register special packet receivers
	PBall.RegisterPacketReceiver(SPID_NEWPLAYER,GetGameStartPacket);
	PBall.RegisterPacketReceiver(SPID_COLLIDE,GetCollideInfo);

	PBall.SetNumberOfTeams(NumOfTeams);

	PBall.AddHUDItemCallback(HI_TEXT,DisplayHUDScores);

	PowerBallIcon = DLLbm_AllocLoadFileBitmap("PowerBallIcon.ogf",0);
	if(PowerBallIcon==-1)
		PowerBallIcon = BAD_BITMAP_HANDLE;

	DisplayScoreScreen = false;

	//TableFileAdd("PowerBall.Gam");


	// Initialize the Stats Manager
	// ----------------------------

	tDmfcStatsInit tsi;
	tDmfcStatsColumnInfo pl_col[7];
	char gname[20];
	strcpy(gname,"Powerball");

	tsi.flags = DSIF_SHOW_PIC|DSIF_SHOW_OBSERVERICON|DSIF_SEPERATE_BY_TEAM;
	tsi.cColumnCountDetailed = 0;
	tsi.cColumnCountPlayerList = 7;
	tsi.clbDetailedColumnBMP = NULL;
	tsi.clbDetailedColumn = NULL;
	tsi.clbPlayerColumn = DetermineScore;
	tsi.clbPlayerColumnBMP = ShowStatBitmap;	
	tsi.DetailedColumns = NULL;
	tsi.GameName = gname;
	tsi.MaxNumberDisplayed = NULL;
	tsi.PlayerListColumns = pl_col;
	tsi.SortedPlayerRecords = SortedPlayers;
	tsi.clTeamLine = TeamScoreCallback;

	pl_col[0].color_type = DSCOLOR_TEAM;
	pl_col[0].title[0] = '\0';
	pl_col[0].type = DSCOL_BMP;
	pl_col[0].width = 15;

	pl_col[1].color_type = DSCOLOR_TEAM;
	strcpy(pl_col[1].title,"Pilot");
	pl_col[1].type = DSCOL_PILOT_NAME;
	pl_col[1].width = 120;

	pl_col[2].color_type = DSCOLOR_TEAM;
	strcpy(pl_col[2].title,"Score");
	pl_col[2].type = DSCOL_CUSTOM;
	pl_col[2].width = 47;

	pl_col[3].color_type = DSCOLOR_TEAM;
	strcpy(pl_col[3].title,"Kills");
	pl_col[3].type = DSCOL_KILLS_BOTH;
	pl_col[3].width = 47;

	pl_col[4].color_type = DSCOLOR_TEAM;
	strcpy(pl_col[4].title,"Deaths");
	pl_col[4].type = DSCOL_DEATHS_BOTH;
	pl_col[4].width = 57;

	pl_col[5].color_type = DSCOLOR_TEAM;
	strcpy(pl_col[5].title,"Suicides");
	pl_col[5].type = DSCOL_SUICIDES_BOTH;
	pl_col[5].width = 65;

	pl_col[6].color_type = DSCOLOR_TEAM;
	strcpy(pl_col[6].title,"Ping");
	pl_col[6].type = DSCOL_PING;
	pl_col[6].width = 40;

	stat.Initialize(&tsi);

	PBallHazehandle = DLLbm_AllocBitmap(32,32,0);
	if(PBallHazehandle>BAD_BITMAP_HANDLE){
		uint16_t *data = DLLbm_data(PBallHazehandle,0);
		for(int i=0;i<32*32;i++){
			data[i] = GR_RGB16(80,200,255)|OPAQUE_FLAG;
		}
	}else{
		Int3();
	}
}

// Called when the DLL is shutdown
void DLLFUNCCALL DLLGameClose ()
{
	if(PowerBallIcon>BAD_BITMAP_HANDLE)
		DLLbm_FreeBitmap(PowerBallIcon);

	if(PBallHazehandle>BAD_BITMAP_HANDLE)
		DLLbm_FreeBitmap(PBallHazehandle);

	if(RegenTimer!=-1)
		PBall.KillTimer(RegenTimer);
	PBall.GameClose();
	//DLLDestroyStringTable(StringTable,StringTableSize);
}

void DMFCApp::OnClientPlayerDisconnect(int player_num)
{
	if(player_num==WhoHasPowerBall)
		HandlePowerballLoss(player_num,true);
	DMFCBase::OnClientPlayerDisconnect(player_num);
}

void DMFCApp::OnPlayerEntersObserver(int pnum,object *piggy)
{
	if(pnum==WhoHasPowerBall)
		HandlePowerballLoss(pnum,false);
	DMFCBase::OnPlayerEntersObserver(pnum,piggy);
}

bool DMFCApp::OnCanChangeTeam(int pnum,int newteam)
{
	if(!DMFCBase::OnCanChangeTeam(pnum,newteam))
		return false;
	
	if(WhoHasPowerBall==pnum){
		AnnounceTeamChangeDeny(pnum);
		return false;
	}

	return true;	
}

void DMFCApp::OnHUDInterval(void)
{
	stat.DoFrame();
	DisplayOutrageLogo();

	DMFCBase::OnHUDInterval();
}

void DMFCApp::OnInterval(void)
{
	GetSortedPlayerSlots(SortedPlayers,DLLMAX_PLAYERS);
	players_sorted = true;
	SortTeams();

	DMFCBase::OnInterval();
}

void DMFCApp::OnKeypress(int key)
{
	switch(key){
	case K_F7:
		DisplayScoreScreen = !DisplayScoreScreen;
		PBall.EnableOnScreenMenu(false);
		stat.Enable(DisplayScoreScreen);
		break;
	case K_PAGEDOWN:
		if(DisplayScoreScreen){
			stat.ScrollDown();
			Data->iRet = 1;
		}
		break;
	case K_PAGEUP:
		if(DisplayScoreScreen){
			stat.ScrollUp();
			Data->iRet = 1;
		}
		break;
	case K_F6:
		DisplayScoreScreen = false;
		stat.Enable(false);		
		break;
	}

	DMFCBase::OnKeypress(key);
}

// The server has just started, so clear out all the stats and game info
void DMFCApp::OnServerGameCreated(void)
{
	DMFCBase::OnServerGameCreated();
	player_record *pr;
	tPlayerStat *stat;
	for(int i=0;i<MAX_PLAYER_RECORDS;i++){
		pr = GetPlayerRecord(i);
		stat = (tPlayerStat *)pr->user_info;
		if(stat){
			stat->Score[DSTAT_LEVEL] = stat->Score[DSTAT_OVERALL] = 0;
		}
	}
	for(i=0;i<NumOfTeams;i++){
		TeamScores[i] = 0;
	}
}

void DMFCApp::OnServerCollide(object *me_obj,object *it_obj)
{
	if( !me_obj || !it_obj )
		return;
	if(it_obj->type==OBJ_PLAYER && me_obj->type==OBJ_POWERUP && me_obj->id==PowerBallID && -1==WhoHasPowerBall){
		CallClientEvent(EVT_CLIENT_GAMECOLLIDE,GetMeObjNum(),GetItObjNum(),-1);
		OnClientCollide(me_obj,it_obj);
	}
	DMFCBase::OnServerCollide(me_obj,it_obj);
}

void DMFCApp::OnClientCollide(object *me_obj,object *it_obj)
{
	static int sound = -1;
	WhoHasPowerBall = it_obj->id;
	if(WhoHasPowerBall==GetPlayerNum()){
		//we need to turn off movement
		Players[GetPlayerNum()].movement_scalar = 0;
	}

	if(sound==-1)
		sound=DLLFindSoundName(IGNORE_TABLE(POWERBALL_SND_SCORE));
	if(sound!=-1)
		DLLPlay3dSound(sound,&Objects[Players[GetPlayerNum()].objnum]);

	//Set a Timer to display
	if(WhoJustPowerBTimer!=-1)
		KillTimer(WhoJustPowerBTimer);
	WhoJustPowerBTimer = SetTimerInterval(OnTimer,0.5f,5.0f,OnTimerKill);
	WhoJustPowerB = GetPlayerTeam(it_obj->id);

	DLLAddHUDMessage("%s Has Picked Up The PowerBall!",Players[it_obj->id].callsign);

	HandlePlayerCollideWithPowerball(it_obj,&Objects[PowerBallObjnum]);

	DMFCBase::OnClientCollide(me_obj,it_obj);
}

void DMFCApp::OnServerPlayerChangeSegment(int player_num,int newseg,int oldseg)
{
	if(player_num==-1)
		return;
	if((WhoHasPowerBall==player_num)&&(newseg==GoalRooms[GetPlayerTeam(player_num)])){
		CallClientEvent(EVT_CLIENT_GAMEPLAYERCHANGESEG,GetMeObjNum(),GetItObjNum(),-1);
		OnClientPlayerChangeSegment(player_num,newseg,oldseg);
	}
}

void DMFCApp::OnClientPlayerChangeSegment(int player_num,int newseg,int oldseg)
{
	static int sound = -1;
	//if we got here than the player with the powerball has entered his team's goal!

	if(IsPlayerDedicatedServer(player_num))
		return;	//dedicated server

	HandlePowerballLoss(player_num,false);
	DLLObjSetPos(&Objects[PowerBallObjnum],&PowerBallPos,PowerBallRoom,NULL,false);	
	WhoHasPowerBall = -1;

	DLLAddHUDMessage("%s Team Scores!!",GetTeamString(Players[player_num].team));

	tPlayerStat *stat = (tPlayerStat *)GetPlayerRecordData(player_num);
	if(stat){
		stat->Score[DSTAT_LEVEL]++;
		stat->Score[DSTAT_OVERALL]++;
	}
	
	TeamScores[Players[player_num].team]++;

	if(sound==-1)
		sound=DLLFindSoundName(IGNORE_TABLE(POWERBALL_SND_SCORE));
	if(sound!=-1)
		DLLPlay3dSound(sound,&Objects[Players[GetPlayerNum()].objnum]);

	//do killgoal check
	int goal;
	if((GetScoreLimit(&goal))&&(GetLocalRole()==LR_SERVER)){
		if(TeamScores[Players[player_num].team]>=goal)
			EndLevel();
	}

	//Set a Timer to display
	if(WhoJustScoredTimer!=-1)
		KillTimer(WhoJustScoredTimer);
	WhoJustScoredTimer = SetTimerInterval(OnTimerScore,0.5f,5.0f,OnTimerScoreKill);
	WhoJustScored = Players[player_num].team;
}


// The server has started a new level, so clear out any scores needed to be reset
void DMFCApp::OnClientLevelStart(void)
{
	DMFCBase::OnClientLevelStart();

	player_record *pr;
	tPlayerStat *stat;
	for(int i=0;i<MAX_PLAYER_RECORDS;i++){
		pr = GetPlayerRecord(i);
		if(pr)
			stat = (tPlayerStat *)pr->user_info;
		else
			stat = NULL;
		if(stat)
			stat->Score[DSTAT_LEVEL] = 0;
	}

	for(i=0;i<NumOfTeams;i++){
		TeamScores[i] = 0;
		GoalRooms[i] = DLLGetGoalRoomForTeam(i);
	}

	DLLMultiPaintGoalRooms();

	DLLmprintf(0,"Getting powerball info\n");
	if(!GetPowerBallInfo(DLLFindObjectIDName(IGNORE_TABLE(POWERBALL_ID_NAME)))){
		FatalError("Error finding Powerball room\n");
	}

	if(GetLocalRole()==LR_SERVER){
		PowerBallObjnum = DLLObjCreate(OBJ_POWERUP,PowerBallID,PowerBallRoom,&PowerBallPos,NULL);
		if(PowerBallObjnum==-1)
			FatalError("Unable to create PowerBall");
		DLLMultiSendObject(&Objects[PowerBallObjnum],1);
		if(RegenTimer!=-1)
			KillTimer(RegenTimer);
		RegenTimer = SetTimerInterval(OnTimerRegen,0.1f,3600.0f,OnTimerRegenKill);
	}else{
		RequestGameState();
	}
	players_sorted = false;
}

void DMFCApp::OnClientLevelEnd(void)
{
	SaveEndOfLevelStats();
	DMFCBase::OnClientLevelEnd();
}

// A New Player has entered the game, so we want to send him a game status packet that
// has information about the game
void DMFCApp::OnGameStateRequest(int player_num)
{
	SendGameStartPacket(player_num);
	DMFCBase::OnGameStateRequest(player_num);	
}


// A new player has entered the game, zero their stats out
void DMFCApp::OnPlayerConnect(int player_num)
{
	tPlayerStat *stat = (tPlayerStat *)GetPlayerRecordData(player_num);
	if(stat){
		stat->Score[DSTAT_LEVEL] = 0;
		stat->Score[DSTAT_OVERALL] = 0;
	}	

	DMFCBase::OnPlayerConnect(player_num);
	DisplayWelcomeMessage(player_num);
}

// We need to adjust the scores
void DMFCApp::OnClientPlayerKilled(object *killer_obj,int victim_pnum)
{
	int kpnum;
		
	if(killer_obj){
		if((killer_obj->type==OBJ_PLAYER)||(killer_obj->type==OBJ_GHOST))
			kpnum = killer_obj->id;
		else if(killer_obj->type==OBJ_ROBOT || (killer_obj->type == OBJ_BUILDING && killer_obj->ai_info)){
			//countermeasure kill
			kpnum = GetCounterMeasureOwner(killer_obj);
		}else{
			kpnum = -1;
		}
	}else
		kpnum = -1;

	HandlePowerballLoss(victim_pnum,(bool)(victim_pnum!=kpnum && kpnum!=-1));	

	DMFCBase::OnClientPlayerKilled(killer_obj,victim_pnum);
}

void DMFCApp::HandlePowerballLoss(int pnum,bool spew_shields)
{
	static int shield_id = -1;
	if(WhoHasPowerBall==-1)
		return;

	if(pnum == WhoHasPowerBall){
		DLLAddHUDMessage("%s Lost The PowerBall!",Players[pnum].callsign);
		if(WhoHasPowerBall==GetPlayerNum()){
			//remove movement restriction
			Players[GetPlayerNum()].movement_scalar = 1;
		}
	}

	HandlePlayerLosingPowerball(&Objects[PowerBallObjnum]);

	if(GetLocalRole()==LR_SERVER && spew_shields){
		//add a bunch of shield powerups into the inventory so they spew out
		if(shield_id==-1){
			shield_id = DLLFindObjectIDName("Shield");
		}
		if(shield_id!=-1){
			int max = rand()%4 + 1;
			for(int i=0;i<max;i++)
				DLLInvAddTypeID(pnum,OBJ_POWERUP,shield_id);
		}
	}

	WhoHasPowerBall = -1;
	UpdatePowerBallEffectOnPlayer(pnum,false);
}

bool compare_slots(int a,int b)
{
	int ascore,bscore;
	player_record *apr,*bpr;
	tPlayerStat *astat,*bstat;

	apr = PBall.GetPlayerRecord(a);
	bpr = PBall.GetPlayerRecord(b);
	if( !apr )
		return true;
	if( !bpr )
		return false;
	if( apr->state==STATE_EMPTY )
		return true;
	if( bpr->state==STATE_EMPTY )
		return false;
	astat = (tPlayerStat *)apr->user_info;
	bstat = (tPlayerStat *)bpr->user_info;

	if( (apr->state==STATE_INGAME) && (bpr->state==STATE_INGAME) ){
		//both players were in the game
		ascore = (astat)?astat->Score[DSTAT_LEVEL]:0;
		bscore = (bstat)?bstat->Score[DSTAT_LEVEL]:0;
		return (ascore<bscore);
	}
	if( (apr->state==STATE_INGAME) && (bpr->state==STATE_DISCONNECTED) ){
		//apr gets priority since he was in the game on exit
		return false;
	}
	if( (apr->state==STATE_DISCONNECTED) && (bpr->state==STATE_INGAME) ){
		//bpr gets priority since he was in the game on exit
		return true;
	}
	//if we got here then both players were disconnected
	ascore = (astat)?astat->Score[DSTAT_LEVEL]:0;
	bscore = (bstat)?bstat->Score[DSTAT_LEVEL]:0;
	return (ascore<bscore);
}

void DMFCApp::OnPLRInit(void)
{
	int tempsort[MAX_PLAYER_RECORDS];
	int i,t,j;

	for(i=0;i<MAX_PLAYER_RECORDS;i++){
		tempsort[i] = i;
	}

	for(i=1;i<=MAX_PLAYER_RECORDS-1;i++){
		t=tempsort[i];
		// Shift elements down until
		// insertion point found.
		for(j=i-1;j>=0 && compare_slots(tempsort[j],t); j--){
			tempsort[j+1] = tempsort[j];
		}
		// insert
		tempsort[j+1] = t;
	}

	//copy the array over
	memcpy(SortedPlayers,tempsort,DLLMAX_PLAYERS*sizeof(int));

	//Now fill in the final structure of sorted names
	int TeamCount[DLLMAX_TEAMS];
	player_record *pr;
	int team;

	for(i=0;i<NumOfTeams;i++)
		TeamCount[i] = 0;
	for(i=0;i<DLLMAX_PLAYERS;i++){
		int slot = SortedPlayers[i];
		pr = GetPlayerRecord(slot);
		if(pr->state!=STATE_EMPTY){

			if(IsPlayerDedicatedServer(pr))
				continue;//skip dedicated server

			team = (pr->state==STATE_INGAME)?Players[pr->pnum].team:pr->team;

			if(team>=NumOfTeams)
				team = 0;
			SortedPLRPlayers[team][TeamCount[team]] = slot;
			TeamCount[team]++;
		}
	}
	for(i=0;i<NumOfTeams;i++){
		if(TeamCount[i]<DLLMAX_PLAYERS)
			SortedPLRPlayers[i][TeamCount[i]] = -1;
	}

	DMFCBase::OnPLRInit();
}

void DMFCApp::OnPLRInterval(void)
{
	DMFCBase::OnPLRInterval();

	int TeamCol = 100;
	int NameCol = 210;
	int KillsCol = 350;
	int DeathsCol = 400;
	int SuicidesCol = 450;
	int y = 110;
	int slot;
	player_record *pr;
	tPlayerStat *stat;

	DLLgrtext_SetFont(Game_fonts[HUD_FONT_INDEX]);
	int height = DLLgrfont_GetHeight(Game_fonts[HUD_FONT_INDEX]) + 1;

	//print out header
	DLLgrtext_SetColor(GR_RGB(255,40,40));
	DLLgrtext_Printf(NameCol,y,"Pilot");
	DLLgrtext_Printf(KillsCol,y,"Kills");
	DLLgrtext_Printf(DeathsCol,y,"Deaths");
	DLLgrtext_Printf(SuicidesCol,y,"Suicides");
	y+=height;


	for(int team=0;team<MAX_TEAMS;team++){
		//process this team
		if(SortedPLRPlayers[team][0]!=-1){
			//is there anyone on this team?
			DLLgrtext_SetColor(GetTeamColor(team));
			DLLgrtext_Printf(TeamCol,y,"%s Team: %d",GetTeamString(team),TeamScores[team]);
		}

		for(int index=0;index<DLLMAX_PLAYERS;index++){
			//get the player num
			slot = SortedPLRPlayers[team][index];
			pr = GetPlayerRecord(slot);
			if(slot==-1)//we are done with this team
				break;
			if(pr && pr->state!=STATE_EMPTY){

				if(IsPlayerDedicatedServer(pr))
					continue;//skip dedicated server

				//valid player
				stat = (tPlayerStat *)pr->user_info;
				DLLgrtext_Printf(NameCol,y,"%s %d[%d]:",pr->callsign,(stat)?stat->Score[DSTAT_LEVEL]:0,(stat)?stat->Score[DSTAT_OVERALL]:0);
				DLLgrtext_Printf(KillsCol,y,"%d[%d]",pr->dstats.kills[DSTAT_LEVEL],pr->dstats.kills[DSTAT_OVERALL]);
				DLLgrtext_Printf(DeathsCol,y,"%d[%d]",pr->dstats.deaths[DSTAT_LEVEL],pr->dstats.deaths[DSTAT_OVERALL]);
				DLLgrtext_Printf(SuicidesCol,y,"%d[%d]",pr->dstats.suicides[DSTAT_LEVEL],pr->dstats.suicides[DSTAT_OVERALL]);
				y+=height;
			}
		}//end for
		//on to the next team
	}//end for
}


void SaveStatsToFile(char *filename)
{
	CFILE *file;
	DLLOpenCFILE(&file,filename,"wt");
	if(!file){
		DLLmprintf(0,"Unable to open output file\n");
		return;
	}

	//write out game stats
	#define BUFSIZE	150
	char buffer[BUFSIZE];
	char tempbuffer[25];
	int sortedslots[MAX_PLAYER_RECORDS];
	player_record *pr,*dpr;
	tPInfoStat stat;
	tPlayerStat *st;
	int count,length,p;

	//sort the stats
	PBall.GetSortedPlayerSlots(sortedslots,MAX_PLAYER_RECORDS);
	SortTeams();
	count = 1;

	sprintf(buffer,"PowerBall\nGame: %s\nLevel: %d\n",PBall.Netgame->name,PBall.Current_mission->cur_level);
	DLLcf_WriteString(file,buffer);

	for(p=0;p<NumOfTeams;p++){
		int team_i = SortedTeams[p];
		memset(buffer,' ',BUFSIZE);
		sprintf(tempbuffer,"%s Team",PBall.GetTeamString(team_i));
		memcpy(&buffer[0],tempbuffer,strlen(tempbuffer));
		sprintf(tempbuffer,"[%d]",TeamScores[team_i]);
		memcpy(&buffer[20],tempbuffer,strlen(tempbuffer));
		DLLcf_WriteString(file,buffer);
	}

	sprintf(buffer,"Current Level Rankings\n");
	DLLcf_WriteString(file,buffer);

	sprintf(buffer,"Rank Name                         Score       Kills       Deaths     Suicides");
	DLLcf_WriteString(file,buffer);
	sprintf(buffer,"-----------------------------------------------------------------------------");
	DLLcf_WriteString(file,buffer);


	for(p=0;p<MAX_PLAYER_RECORDS;p++){
		pr = PBall.GetPlayerRecord(sortedslots[p]);
		if( pr && pr->state!=STATE_EMPTY) {

			if(PBall.IsPlayerDedicatedServer(pr))
				continue;//skip dedicated server

			st = (tPlayerStat *)pr->user_info;
			memset(buffer,' ',BUFSIZE);

			sprintf(tempbuffer,"%d)",count);
			memcpy(&buffer[0],tempbuffer,strlen(tempbuffer));

			sprintf(tempbuffer,"%s%s",(pr->state==STATE_INGAME)?"":"*",pr->callsign);
			memcpy(&buffer[5],tempbuffer,strlen(tempbuffer));

			sprintf(tempbuffer,"%d[%d]",(st)?st->Score[DSTAT_LEVEL]:0,(st)?st->Score[DSTAT_OVERALL]:0);
			memcpy(&buffer[34],tempbuffer,strlen(tempbuffer));

			sprintf(tempbuffer,"%d[%d]",pr->dstats.kills[DSTAT_LEVEL],pr->dstats.kills[DSTAT_OVERALL]);
			memcpy(&buffer[46],tempbuffer,strlen(tempbuffer));

			sprintf(tempbuffer,"%d[%d]",pr->dstats.deaths[DSTAT_LEVEL],pr->dstats.deaths[DSTAT_OVERALL]);
			memcpy(&buffer[58],tempbuffer,strlen(tempbuffer));

			sprintf(tempbuffer,"%d[%d]",pr->dstats.suicides[DSTAT_LEVEL],pr->dstats.suicides[DSTAT_OVERALL]);
			memcpy(&buffer[69],tempbuffer,strlen(tempbuffer));
	
			int pos;
			pos = 69 + strlen(tempbuffer) + 1;
			if(pos<BUFSIZE)
				buffer[pos] = '\0';
							
			buffer[BUFSIZE-1] = '\0';
			DLLcf_WriteString(file,buffer);
			count++;
		}
	}

	DLLcf_WriteString(file,"\nIndividual Stats\n");

	count =1;
	for(p=0;p<MAX_PLAYER_RECORDS;p++){
		pr = PBall.GetPlayerRecord(p);
		if( pr && pr->state!=STATE_EMPTY) {

			if(PBall.IsPlayerDedicatedServer(pr))
				continue;//skip dedicated server

			//Write out header
			sprintf(buffer,"%d) %s%s",count,(pr->state==STATE_INGAME)?"":"*",pr->callsign);
			DLLcf_WriteString(file,buffer);
			length = strlen(buffer);
			memset(buffer,'=',length);
			buffer[length] = '\0';
			DLLcf_WriteString(file,buffer);
			
			//time in game
			sprintf(buffer,"Total Time In Game: %s",basethis->GetTimeString(basethis->GetTimeInGame(p)));
			DLLcf_WriteString(file,buffer);

			if(PBall.FindPInfoStatFirst(p,&stat)){
				sprintf(buffer,"Callsign:                     Kills:    Deaths:");
				DLLcf_WriteString(file,buffer);
				
				if(stat.slot!=p){
					memset(buffer,' ',BUFSIZE);
					dpr = PBall.GetPlayerRecord(stat.slot);
					int pos;

					sprintf(tempbuffer,"%s",dpr->callsign);
					memcpy(buffer,tempbuffer,strlen(tempbuffer));

					sprintf(tempbuffer,"%d",stat.kills);
					memcpy(&buffer[30],tempbuffer,strlen(tempbuffer));

					sprintf(tempbuffer,"%d",stat.deaths);
					memcpy(&buffer[40],tempbuffer,strlen(tempbuffer));

					pos = 40 + strlen(tempbuffer) + 1;
					if(pos<BUFSIZE)
						buffer[pos] = '\0';

					buffer[BUFSIZE-1] = '\0';
					DLLcf_WriteString(file,buffer);
				}
						

				while(PBall.FindPInfoStatNext(&stat)){															
					if(stat.slot!=p){
						int pos;
						memset(buffer,' ',BUFSIZE);
						dpr = PBall.GetPlayerRecord(stat.slot);
						sprintf(tempbuffer,"%s",dpr->callsign);
						memcpy(buffer,tempbuffer,strlen(tempbuffer));

						sprintf(tempbuffer,"%d",stat.kills);
						memcpy(&buffer[30],tempbuffer,strlen(tempbuffer));

						sprintf(tempbuffer,"%d",stat.deaths);
						memcpy(&buffer[40],tempbuffer,strlen(tempbuffer));

						pos = 40 + strlen(tempbuffer) + 1;
						if(pos<BUFSIZE)
							buffer[pos] = '\0';

						buffer[BUFSIZE-1] = '\0';
						DLLcf_WriteString(file,buffer);
					}		
				}
			}
			PBall.FindPInfoStatClose();
			DLLcf_WriteString(file,"");	//skip a line
			count++;
		}
	}

	//done writing stats
	DLLcfclose(file);
	DLLAddHUDMessage("Stats saved to %s",filename);
}

#define ROOTFILENAME	"PowerBall"
int stat_filecounter = 0;
void DMFCApp::OnSaveStatsToFile(void)
{
	char filename[100];	
	sprintf(filename,"%s%d.stats",ROOTFILENAME,stat_filecounter); stat_filecounter++;
	SaveStatsToFile(filename);
}

void SaveEndOfLevelStats(void)
{
	int level = PBall.Current_mission->cur_level;
	char *name = PBall.Netgame->name;
	char filename[256];
	sprintf(filename,"%s_%s_%d.stats",ROOTFILENAME,name,level);
	SaveStatsToFile(filename);
}



//////////////////////////////////////////////
// NON-DMFC Functions
//////////////////////////////////////////////
void DisplayHUDScores(struct tHUDItem *hitem)
{
	if(!players_sorted)
		return;
	int height = DLLgrfont_GetHeight(PBall.Game_fonts[HUD_FONT_INDEX]) + 3;
	int y,x,team;


	if(WhoHasPowerBall==PBall.GetPlayerNum()){
		if(PBallHazehandle>BAD_BITMAP_HANDLE){
			//DLLRenderHUDQuad(0,0,640,480,0,0,1,1,PBallHazehandle,100);
		}else{
			Int3();
		}

		int w,h;
		w = DLLbm_w(PowerBallIcon,0);
		h = DLLbm_h(PowerBallIcon,0);
		x = 530;
		y = 10;
		DLLRenderHUDQuad(x,y,w,h,0,0,1,1,PowerBallIcon,255);
	}	

	y = 180;
	x = 520;

	team = PBall.GetMyTeam();
	DLLRenderHUDText(PBall.GetTeamColor(team),255,0,x,0,"%s Team",PBall.GetTeamString(team));

	int powerteam = -1;
	if(WhoHasPowerBall!=-1)
		powerteam = PBall.GetPlayerTeam(WhoHasPowerBall);

	for(int i=0;i<NumOfTeams;i++){
		team = SortedTeams[i];
		if( ((WhoJustScored!=team)&&(WhoJustPowerB!=team)) || (DisplayPowerBBlink)&&(DisplayScoreBlink)){
			if(powerteam==team)
				DLLRenderHUDQuad(x-12,y,10,10,0,0,1,1,PowerBallIcon,255);
			DLLRenderHUDText(PBall.GetTeamColor(team),255,0,x,y,"%s Team",PBall.GetTeamString(team));
			DLLRenderHUDText(PBall.GetTeamColor(team),255,0,615,y,"[%d]",TeamScores[team]);
		}
		y+=height;
	}
}

void GetGameStartPacket(uint8_t *data)
{
	int size = 0;

	//team scores
	memcpy(TeamScores,&data[size],sizeof(int)*DLLMAX_TEAMS);	size+= (sizeof(int)*DLLMAX_TEAMS);

	//who has the powerball
	int8_t temp;
	memcpy(&temp,&data[size],sizeof(char));	size+=sizeof(char);
	WhoHasPowerBall = temp;

	memcpy(&temp,&data[size],sizeof(char));	size+=sizeof(char);
	NumOfTeams = temp;

	//Now based on what we have from the server set up our info
	if(WhoHasPowerBall!=-1){
		UpdatePowerBallEffectOnPlayer(WhoHasPowerBall,true);
	}
	
	//we need to find the objnum of the PowerBall...its there somewhere
	DLLmprintf(0,"Looking for powerball in level\n");
	for(int i=0;i<MAX_OBJECTS;i++){
		if( (PBall.Objects[i].type==OBJ_POWERUP) && (PBall.Objects[i].id==PowerBallID) ){
			//here it is
			PowerBallObjnum = i;
			break;
		}
	}
	DLLmprintf(0,"Powerball %s\n",(PowerBallObjnum==-1)?"Not Found":"Found");
	if(PowerBallObjnum==-1){
		FatalError("Couldn't Find PowerBall when it should be there");
	}
	
}

void SendGameStartPacket(int pnum)
{
	int maxsize = sizeof(int)*DLLMAX_TEAMS + 2*sizeof(char);
	int size = 0;

	uint8_t *data = PBall.StartPacket(maxsize,SPID_NEWPLAYER);
	memset(data,0,maxsize);

	//add the team scores
	memcpy(&data[size],TeamScores,sizeof(int)*DLLMAX_TEAMS); size += (sizeof(int)*DLLMAX_TEAMS);

	int8_t temp;
	//who has the powerball if anyone
	temp = WhoHasPowerBall;
	memcpy(&data[size],&temp,sizeof(char));	size+= sizeof(char);

	//number of teams
	temp = NumOfTeams;
	memcpy(&data[size],&temp,sizeof(char));	size+=sizeof(char);

	//we're done
	DLLmprintf(0,"Sending Game State to %s\n",PBall.Players[pnum].callsign);
	PBall.SendPacket(maxsize,pnum);
}

void UpdatePowerBallEffectOnPlayer(int pnum,bool pickedup)
{
	float red[3],green[3],blue[3];
	red[0] = 1; red[1] = 0; red[2] = 0;
	green[0] = 0;green[1] = 1; green[2] = 0;
	blue[0] = 0;blue[1] = 0; blue[2] = 1;

	if(pickedup){
		DLLPlayerSetRotatingBall(pnum,3,0,red,green,blue);
	}else{
		DLLPlayerSetRotatingBall(pnum,0,0,NULL,NULL,NULL);
	}
}

bool GetPowerBallInfo(int id)
{
	if(id==-1)
		return false;

	PowerBallID = id;

	//find the room with the RF_SPECIAL1
	for(int i=0;i<=*PBall.Highest_room_index;i++){
		if(PBall.Rooms[i].flags&RF_SPECIAL1){
			//here's the powerball!
			PowerBallRoom = i;
			DLLComputeRoomCenter(&PowerBallPos,&PBall.Rooms[i]);
			return true;
		}
	}
	return false;	
}

#define compGT(a,b) (a < b)
// insert sort
void SortTeams(void)
{
	int t;
    int i, j;

	//copy scores into scoreinfo array
	for(i=0;i<DLLMAX_TEAMS;i++)
	{
		SortedTeams[i] = i;
	}

    for(i=1;i<=DLLMAX_TEAMS-1;i++) 
	{
		t=SortedTeams[i];

        // Shift elements down until 
        // insertion point found.
        for(j=i-1;j>=0 && compGT(TeamScores[SortedTeams[j]],TeamScores[t]); j--)
		{
			SortedTeams[j+1] = SortedTeams[j];
		}

        // insert 
        SortedTeams[j+1] = t;
    }
}

void DisplayWelcomeMessage(int player_num)
{
	if(player_num==PBall.GetPlayerNum())
	{
		int team = PBall.GetMyTeam();
		if(team==-1)
			return;

		DLLAddHUDMessage("Welcome To Powerball %s",PBall.Players[PBall.GetPlayerNum()].callsign);
		DLLAddColoredHUDMessage(PBall.GetTeamColor(team),"You're On The %s Team",PBall.GetTeamString(team));
	}
	else
	{
		int team = PBall.Players[player_num].team;
		if(team==-1)
			return;

		DLLAddColoredHUDMessage(PBall.GetTeamColor(team),"%s Has Joined The %s Team",PBall.Players[player_num].callsign,PBall.GetTeamString(team));
	}
}

void OnTimerScore(void)
{
	DisplayScoreBlink = !DisplayScoreBlink;
}
void OnTimerScoreKill(void)
{
	DisplayScoreBlink = true;
	WhoJustScored = WhoJustScoredTimer = -1;
}

void OnTimer(void)
{
	DisplayPowerBBlink = !DisplayPowerBBlink;
}
void OnTimerKill(void)
{
	DisplayPowerBBlink = true;
	WhoJustPowerB = WhoJustPowerBTimer = -1;
}

void OnTimerRegen(void)
{
	if(PBall.GetLocalRole()!=LR_SERVER)
		return;
	if(WhoHasPowerBall==-1)
		return;

	float amount = 0;

	amount = 100 - PBall.Objects[PBall.Players[WhoHasPowerBall].objnum].shields;

	if( amount > 1 )
		amount = 1;
	if( amount < 0)
		return;

	PBall.ShieldDelta[WhoHasPowerBall] -= amount;
	PBall.Objects[PBall.Players[WhoHasPowerBall].objnum].shields += amount;
}

void OnTimerRegenKill(void)
{
	RegenTimer = PBall.SetTimerInterval(OnTimerRegen,0.1f,3600.0f,OnTimerRegenKill);
}

void HandlePlayerCollideWithPowerball(object *pobj,object *pball)
{
	if(PBall.GetLocalRole()!=LR_SERVER)
		return;

	// Attach the flag	
	int ret = DLLAttachObject(pobj,0,pball,0,true);
	if(!ret){
		//couldn't attach the flag
		mprintf(0,"PBALL: COULDN'T ATTACH BALL TO PLAYER\n");
	}
}

void HandlePlayerLosingPowerball(object *pball)
{
	if(PBall.GetLocalRole()!=LR_SERVER)
		return;

	DLLUnattachFromParent(pball);
}
	
void HandleWeaponCollide(object *me_obj,object *it_obj)
{
	mprintf(0,"Weapon Collide\n");
}


void bump_two_objects(object *object0, vector *rotvel, vector *velocity, vector *pos, matrix *orient, float mass, float size,vector *collision_point,vector  *collision_normal,float rot_scale,float vel_scale);
/*
void DMFCApp::OnServerWeaponCollide(object *pobj,object *wobj,vector *point,vector *normal,bool is_electrical)
{
	if(is_electrical){
		DMFCBase::OnServerWeaponCollide(pobj,wobj,point,normal,is_electrical);
		return;
	}

	if(pobj->id == WhoHasPowerBall){
		if(pobj->id!=GetPlayerNum())
			SendCollideInfo(pobj,wobj,pobj->id,point,normal);
		else
			bump_two_objects(pobj,&wobj->mtype.phys_info.rotvel,&wobj->mtype.phys_info.velocity,
							&wobj->pos,&wobj->orient,wobj->mtype.phys_info.mass,wobj->size,point,normal,0.1f,10.0f);
	}
	DMFCBase::OnServerWeaponCollide(pobj,wobj,point,normal,is_electrical);
}
*/

void SendCollideInfo(object *pobj,object *wobj,int towho,vector *point,vector *normal)
{
	int maxsize = (sizeof(float)*26) + (sizeof(uint16_t)*1);
	int size = 0;

	uint8_t *data = PBall.StartPacket(maxsize,SPID_COLLIDE);
	memset(data,0,maxsize);

	memcpy(&data[size],&wobj->mtype.phys_info.rotvel,3*sizeof(float)); size+=(sizeof(float)*3);
	memcpy(&data[size],&wobj->mtype.phys_info.velocity,3*sizeof(float)); size+=(sizeof(float)*3);
	memcpy(&data[size],&wobj->pos,3*sizeof(float)); size+=(sizeof(float)*3);
	memcpy(&data[size],&wobj->mtype.phys_info.mass,sizeof(float)); size+=sizeof(float);
	memcpy(&data[size],&wobj->size,sizeof(float)); size+=sizeof(float);
	memcpy(&data[size],&wobj->orient.fvec,sizeof(float)*3); size+=(sizeof(float)*3);
	memcpy(&data[size],&wobj->orient.rvec,sizeof(float)*3); size+=(sizeof(float)*3);
	memcpy(&data[size],&wobj->orient.uvec,sizeof(float)*3); size+=(sizeof(float)*3);
	memcpy(&data[size],point,sizeof(float)*3); size+=(sizeof(float)*3);
	memcpy(&data[size],normal,sizeof(float)*3); size+=(sizeof(float)*3);
	uint16_t id = (pobj-PBall.Objects);
	memcpy(&data[size],&id,sizeof(uint16_t)); size+= sizeof(uint16_t);

	//we're done
	PBall.SendPacket(maxsize,towho);
}

void GetCollideInfo(uint8_t *data)
{
	int size = 0;
	vector rotvel,velocity,pos,point,normal;
	matrix orient;
	float mass,fsize;
	uint16_t id;
	int objnum;

	memcpy(&rotvel,&data[size],3*sizeof(float)); size+=(sizeof(float)*3);
	memcpy(&velocity,&data[size],3*sizeof(float)); size+=(sizeof(float)*3);
	memcpy(&pos,&data[size],3*sizeof(float)); size+=(sizeof(float)*3);
	memcpy(&mass,&data[size],sizeof(float)); size+=sizeof(float);
	memcpy(&fsize,&data[size],sizeof(float)); size+=sizeof(float);
	memcpy(&orient.fvec,&data[size],sizeof(float)*3); size+=(sizeof(float)*3);
	memcpy(&orient.rvec,&data[size],sizeof(float)*3); size+=(sizeof(float)*3);
	memcpy(&orient.uvec,&data[size],sizeof(float)*3); size+=(sizeof(float)*3);	
	memcpy(&point,&data[size],sizeof(float)*3); size+=(sizeof(float)*3);
	memcpy(&normal,&data[size],sizeof(float)*3); size+=(sizeof(float)*3);

	memcpy(&id,&data[size],sizeof(uint16_t)); size+= sizeof(uint16_t);

	objnum = PBall.ConvertServerToLocalObjnum(id);
	ASSERT(objnum!=-1);
	if(objnum!=-1){
		bump_two_objects(&PBall.Objects[objnum],&rotvel,&velocity,&pos,&orient,mass,fsize,&point,&normal,0.1f,10.0f);
	}
}

void bump_two_objects(object *object0, vector *rotvel, vector *velocity, vector *pos, matrix *orient, float mass, float size,vector *collision_point,vector  *collision_normal,float rot_scalar,float vel_scalar)
{
//	vector force;	//dv, 
	object *t = NULL;
	object *other = NULL;

	ASSERT(_finite(rotvel->x) != 0);
	ASSERT(_finite(rotvel->y) != 0);
	ASSERT(_finite(rotvel->z) != 0);
	ASSERT(_finite(object0->mtype.phys_info.rotvel.x) != 0);
	ASSERT(_finite(object0->mtype.phys_info.rotvel.y) != 0);
	ASSERT(_finite(object0->mtype.phys_info.rotvel.z) != 0);
	ASSERT(_finite(velocity->x) != 0);
	ASSERT(_finite(velocity->y) != 0);
	ASSERT(_finite(velocity->z) != 0);
	ASSERT(_finite(object0->mtype.phys_info.velocity.x) != 0);
	ASSERT(_finite(object0->mtype.phys_info.velocity.y) != 0);
	ASSERT(_finite(object0->mtype.phys_info.velocity.z) != 0);

	vector r1 = *collision_point - object0->pos;
	vector r2 = *collision_point - (*pos);
	vector w1;
	vector w2;
	vector n1;
	vector n2;
	float temp1;
	float temp2;

	float j;

	matrix o_t1 = object0->orient;
	matrix o_t2 = *orient;

	DLLvm_TransposeMatrix(&o_t1);
	DLLvm_TransposeMatrix(&o_t2);

	vector cmp1 = object0->mtype.phys_info.rotvel * o_t1;
	vector cmp2 = (*rotvel) * o_t2;

	DLLConvertEulerToAxisAmount(&cmp1, &n1, &temp1);
	DLLConvertEulerToAxisAmount(&cmp2, &n2, &temp2);
	
	n1 *= temp1;
	n2 *= temp2;

	if(temp1 != 0.0f)
	{
		DLLvm_CrossProduct(&w1, &n1, &r1);
	}
	else
	{
		w1.x = 0;
		w1.y = 0;
		w1.z = 0;
	}

	if(temp2 != 0.0f)
	{
		DLLvm_CrossProduct(&w2, &n2, &r2);
	}
	else
	{
		w2.x = 0;
		w2.y = 0;
		w2.z = 0;
	}

	vector p1 = object0->mtype.phys_info.velocity + w1;
	vector p2 = (*velocity) + w2;
	float v_rel;

	float m1 = object0->mtype.phys_info.mass;
	float m2 = mass;

	ASSERT(m1 != 0.0f && m2 != 0.0f);

	v_rel = *collision_normal * (p1 - p2);

	float e;
	e = vel_scalar;

	vector c1;
	vector c2;
	vector cc1;
	vector cc2;
	float cv1;
	float cv2;

//	matrix i1;
//	matrix i2;
	float i1 = (2.0f/5.0f)*m1*object0->size*object0->size;
	float i2 = (2.0f/5.0f)*m2*size;

	if(i1 < .0000001)
		i1 = .0000001f;
	if(i2 < .0000001)
		i2 = .0000001f;

	DLLvm_CrossProduct(&c1, &r1, collision_normal);
	DLLvm_CrossProduct(&c2, &r2, collision_normal);

	c1 = c1/i1;
	c2 = c2/i2;
	
	DLLvm_CrossProduct(&cc1, &c1, &r1);
	DLLvm_CrossProduct(&cc2, &c2, &r2);

	cv1 = (*collision_normal)*c1;
	cv2 = (*collision_normal)*c2;

	j = (-(1.0f + e))*v_rel;
	j /= (1/m1 + 1/m2 + cv1 + cv2);

	//apply the force to the player
	object0->mtype.phys_info.velocity += ((j*(*collision_normal))/m1);
	
	vector jcn = j * (*collision_normal);

	DLLvm_CrossProduct(&c1, &r1, &jcn);

	n1 = (c1)/i1;

	temp1 = DLLvm_NormalizeVector(&n1);

	vector txx1;
	DLLConvertAxisAmountToEuler(&n1, &temp1, &txx1);

	float rotscale1;
	rotscale1 = rot_scalar;

	//change the player's rotational velocity
	object0->mtype.phys_info.rotvel += (txx1*object0->orient) * rotscale1;
	
	ASSERT(_finite(object0->mtype.phys_info.rotvel.x) != 0);
	ASSERT(_finite(object0->mtype.phys_info.rotvel.y) != 0);
	ASSERT(_finite(object0->mtype.phys_info.rotvel.z) != 0);
	ASSERT(_finite(object0->mtype.phys_info.velocity.x) != 0);
	ASSERT(_finite(object0->mtype.phys_info.velocity.y) != 0);
	ASSERT(_finite(object0->mtype.phys_info.velocity.z) != 0);
}
