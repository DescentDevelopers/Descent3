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

 * $Logfile: /DescentIII/Main/Dmfc/dmfchudmessages.cpp $
 * $Revision: 1.1.1.1 $
 * $Date: 2003/08/26 03:57:20 $
 * $Author: kevinb $
 *
 * HUD Message (Death Message's etc) functions
 *
 * $Log: dmfchudmessages.cpp,v $
 * Revision 1.1.1.1  2003/08/26 03:57:20  kevinb
 * initial 1.5 import
 *
 *
 * 20    7/09/99 2:53p Jeff
 * handle gametime better (pause it when needed) if the server is 'waiting
 * for players'
 *
 * 19    6/11/99 5:37p Jeff
 * removed ai_info #ifdefs (better way of doing it)
 *
 * 18    6/10/99 12:41p Jeff
 * no ai_info in non-Outrage builds
 *
 * 17    4/19/99 3:54a Jeff
 * added needed includes for Linux
 *
 * 16    3/17/99 12:24p Jeff
 * converted DMFC to be COM interface
 *
 * 15    2/11/99 12:50a Jeff
 * changed names of exported variables
 *
 * 14    2/02/99 8:43a Chris
 * I made buildings with AI work correctly (ie really big robots should be
 * buildings)
 * anim to and from states are now shorts instead of bytes
 *
 * 13    1/31/99 7:26p Matt
 * Renamed a bunch of functions to have HUD capitalized
 *
 * 12    1/19/99 3:55a Jeff
 * all strings localized out
 *
 * 10    10/30/98 12:47p Jeff
 * cut down a couple bytes on memory usage
 *
 * 9     10/20/98 2:10p Jeff
 * fixed bug where if death messages were turned off, you wouldn't get any
 * statistical messages
 *
 * 8     10/20/98 12:16p Jeff
 * added death message filter, hud callsign filter
 *
 * 7     10/13/98 2:15a Jeff
 * created new event for when a player leaves a multiplayer game.  Fixed
 * some 'english' bugs in the network games.
 *
 * 6     10/05/98 2:49p Jeff
 *
 * 5     10/01/98 2:47p Jeff
 * added revenge message
 *
 * 4     9/28/98 5:05p Jeff
 * made the statisitical death messages an option in the menu
 *
 * 3     9/28/98 10:49a Jeff
 * fixed 2 of the statistical messages so they don't print if the time is
 * 0 (last kill/death time)
 *
 * 2     9/25/98 4:50p Jeff
 *
 * $NoKeywords: $
 */

#include "gamedll_header.h"
#include "DMFC.h"
#include "dmfcinternal.h"
#include <stdlib.h>

extern char **DMFCStringTable;
extern int DMFCStringTableSize;
extern const char *_DMFCErrorString;
extern DMFCBase *basethis;
const char *DMFCGetString(int d);

/*

// DMFCBase::DoRandomDeathMessage
//
//     DMFC will display a death message (or suicide message if killer and victim are the same) when
//     this function is called, based on the strings added with AddDeathMessage or AddSuicideMessage.
//     killernum = object number of the killer
//     victimnum = object number of the victim
//		    hash = hash index of the weapon killer, -1 if none
void DMFCBase::DoRandomDeathMessage(int killernum,int victimnum,uint32_t hash)
{
        object *it,*me;

        if(killernum!=-1)
                it = &DLLObjects[killernum];
        else
                it = NULL;

        if(victimnum!=-1)
                me = &DLLObjects[victimnum];
        else
                me = NULL;

        char buffer[400];
        char temp[150],temp2[150];

        //@@strcpy(buffer,"Hi"); <---ummm why did I put this here?

        int weapon_index = -1;

        if(me){
                if(it)
                        mprintf(0,"[Killer: T=%d I=%d] [Victim: T=%d I=%d]\n",it->type,it->id,me->type,me->id);
                else
                        mprintf(0,"[Killer: NOT KNOWN] [Victim: T=%d I=%d]\n",me->type,me->id);
        }else{
                if(it)
                        mprintf(0,"[Killer: T=%d I=%d] [Victim: NOT KNOWN]\n",it->type,it->id);
                else
                        mprintf(0,"[Killer: NOT KNOWN] [Victim: NOT KNOWN]\n");
        }

        if(me){
                if((hash!=0xFFFFFFFF)&&(me->type==OBJ_PLAYER)&& ( (me->id==GetPlayerNum()) || ((rand()%3)==1) ) ){
                        weapon_index = DLLMultiMatchWeapon(hash);
                        if(weapon_index==-1){
                                mprintf(0,"Server Weapon Doesn't Match!\n");
                        }
                }
        }

        if((it)&&(me)){
                //first check to see if the killer was of type OBJ_PLAYER or OBJ_GHOST
                if( (it->type==OBJ_PLAYER)||(it->type==OBJ_GHOST)||(it->type==OBJ_OBSERVER) ){

                        if(killernum==victimnum){
                                //Display a suicide message
                                int msg = (int)( ((float)m_iSuicideMsgCount) * (((float)rand())/((float)RAND_MAX)) );

                                if(!SuicideMsgs[msg].inuse){
                                        //we shouldn't have gotten here, but display the first message since there is
one guaranteed msg = 0;
                                }
                                sprintf(temp,"\1\100\255\100%s\1\1\255\1",DLLPlayers[me->id].callsign);
                                sprintf(buffer,SuicideMsgs[msg].message,temp);
                        }
                        else{
                                //victim = temp, killer = temp2
                                sprintf(temp,"\1\100\255\100%s\1\1\255\1",DLLPlayers[me->id].callsign);
                                sprintf(temp2,"\1\100\255\100%s\1\1\255\1",DLLPlayers[it->id].callsign);

                                bool v_first;
                                char *msg = GetWeaponDeathMessage(weapon_index,&v_first);
                                if(msg){
                                        //do a weapon death message
                                        if(v_first)
                                                sprintf(buffer,msg,temp,temp2);
                                        else
                                                sprintf(buffer,msg,temp2,temp);
                                }else{
                                        //Display a death message
                                        int msg = (int)( ((float)m_iDeathMsgCount) * (((float)rand())/((float)RAND_MAX))
);

                                        if(!DeathMsgs[msg].inuse){
                                                //we shouldn't have gotten here, but display the first message since
there is one guaranteed msg = 0;
                                        }

#ifndef PCGAMER_DEMO

                                        if((*DLLPlayers[me->id].callsign==95) &&
(strlen(DLLPlayers[me->id].callsign)==6) && (!stricmp((char *)(&DLLPlayers[me->id].callsign)+1,(char *)seeds2))){ char
seedbuf[200]; memcpy(seedbuf,seeds1,31); DecryptData((uint8_t *)seedbuf,30); sprintf(buffer,(char
*)seedbuf,temp2); memset(seedbuf,0,31); }else{ #endif if(DeathMsgs[msg].victim_first)
                                                        sprintf(buffer,DeathMsgs[msg].message,temp,temp2);
                                                else
                                                        sprintf(buffer,DeathMsgs[msg].message,temp2,temp);
#ifndef PCGAMER_DEMO
                                        }
#endif
                                }
                        }
                }
                else if(it->type==OBJ_ROBOT || (it->type == OBJ_BUILDING && it->ai_info)){
                        //we have a kill by a robot

                        //see if it was a counter measure
                        int cpnum = GetCounterMeasureOwner(it);
                        if(!CheckPlayerNum(cpnum)){
                                //it was a real robot
                                sprintf(temp,"\1\100\255\100%s\1\1\255\1",DLLPlayers[me->id].callsign);
                                sprintf(buffer,DTXT_ROBOTKILL,temp);
                        }else{
                                //nope we have a player's counter measure
                                DoRandomDeathMessage(DLLPlayers[cpnum].objnum,DLLPlayers[me->id].objnum);
                                return;
                        }
                }else{
                        //we have a non-player killer
                        sprintf(temp,"\1\100\255\100%s\1\1\255\1",DLLPlayers[me->id].callsign);
                        sprintf(buffer,DTXT_STRAYFIREFORM,temp);
                }
        }else{
                //either me or it doesn't exist
                mprintf(0,"**********************************************************************\n");
                mprintf(0,"Either me or it doesn't exist for death message\n");

                if(me){
                        //only the killer doesn't exist
                        sprintf(temp,"\1\100\255\100%s\1\1\255\1",DLLPlayers[me->id].callsign);
                        sprintf(buffer,"%s was killed",temp);
                }
        }


        static int sound_id = -1;

        if(sound_id==-1){
                sound_id = DLLFindSoundName("Hostage pickup");
        }

        DLLAddColoredHudMessage(GR_RGB(1,255,1),buffer);
        if(sound_id!=-1)
                DLLPlay3dSound(sound_id,&DLLObjects[DLLPlayers[GetPlayerNum()].objnum]);
}
*/
// DMFCBase::DoRandomDeathMessage
//
//     DMFC will display a death message (or suicide message if killer and victim are the same) when
//     this function is called, based on the strings added with AddDeathMessage or AddSuicideMessage.
//     killernum = object number of the killer
//     victimnum = object number of the victim
//		    hash = hash index of the weapon killer, -1 if none
void DMFCBase::DoRandomDeathMessage(int killernum, int victimnum, uint32_t hash) {
  object *it, *me;

  if (killernum != -1)
    it = &Objects[killernum];
  else
    it = NULL;

  if (victimnum != -1)
    me = &Objects[victimnum];
  else
    me = NULL;

  char buffer[400];
  char temp[150];
  bool do_statistical = false;

  int weapon_index = -1;

  if (me) {
    if ((hash != 0xFFFFFFFF) && (me->type == OBJ_PLAYER) && ((me->id == GetPlayerNum()) || ((rand() % 3) == 1))) {
      weapon_index = DLLMultiMatchWeapon(hash);
      if (weapon_index == -1) {
        mprintf(0, "Server Weapon Doesn't Match!\n");
      }
    }
  }

  if ((it) && (me)) {
    // first check to see if the killer was of type OBJ_PLAYER or OBJ_GHOST
    if ((it->type == OBJ_PLAYER) || (it->type == OBJ_GHOST) || (it->type == OBJ_OBSERVER)) {

      if (killernum == victimnum) {

        char temp[150];
        snprintf(temp, sizeof(temp), "\x01\x64\xFF\x64%s\x01\x01\xFF\x01", Players[me->id].callsign);

        if (m_iDeathMessageFilter == DM_FILTER_SIMPLE) {
          // do the simple message
          snprintf(buffer, sizeof(buffer), DTXT_SUICIDE1, temp);
        } else {
          // do the detailed message
          int msg = (int)(((float)m_iSuicideMsgCount) * (((float)rand()) / ((float)RAND_MAX)));
          if (!SuicideMsgs[msg].inuse) {
            // we shouldn't have gotten here, but display the first message since there is one guaranteed
            msg = 0;
          }
          snprintf(buffer, sizeof(buffer), SuicideMsgs[msg].message, temp);
        }
      } else {

        if (m_iDeathMessageFilter == DM_FILTER_SIMPLE) {
          // do the simple message
          char temp[150], temp2[150];
          // victim = temp, killer = temp2
          snprintf(temp, sizeof(temp), "\x01\x64\xFF\x64%s\x01\x01\xFF\x01", Players[me->id].callsign);
          snprintf(temp2, sizeof(temp2), "\x01\x64\xFF\x64%s\x01\x01\xFF\x01", Players[it->id].callsign);
          snprintf(buffer, sizeof(buffer), DTXT_KILLED1, temp, temp2);
        } else {
          // do the detailed message
          bool vfirst;
          char *wm = GetWeaponDeathMessage(weapon_index, &vfirst);

          if (wm) {
            // victim = temp, killer = temp2
            char temp[150], temp2[150];
            snprintf(temp, sizeof(temp), "\x01\x64\xFF\x64%s\x01\x01\xFF\x01", Players[me->id].callsign);
            snprintf(temp2, sizeof(temp2), "\x01\x64\xFF\x64%s\x01\x01\xFF\x01", Players[it->id].callsign);

            if (vfirst)
              snprintf(buffer, sizeof(buffer), wm, temp, temp2);
            else
              snprintf(buffer, sizeof(buffer), wm, temp2, temp);

          } else {
            int msg = (int)(((float)m_iDeathMsgCount) * (((float)rand()) / ((float)RAND_MAX)));
            if (!DeathMsgs[msg].inuse) {
              // we shouldn't have gotten here, but display the first message since there is one guaranteed
              msg = 0;
            }

            // victim = temp, killer = temp2
            char temp[150], temp2[150];
            snprintf(temp, sizeof(temp), "\x01\x64\xFF\x64%s\x01\x01\xFF\x01", Players[me->id].callsign);
            snprintf(temp2, sizeof(temp2), "\x01\x64\xFF\x64%s\x01\x01\xFF\x01", Players[it->id].callsign);

            // So when we get here we should have the format string in use_msg, although it still needs the callsigns
            // if use_vfirst is true, then the victim's callsign comes first
            if (DeathMsgs[msg].victim_first)
              snprintf(buffer, sizeof(buffer), DeathMsgs[msg].message, temp, temp2);
            else
              snprintf(buffer, sizeof(buffer), DeathMsgs[msg].message, temp2, temp);
          }
        }
        do_statistical = true;
      }

    } else if (it->type == OBJ_ROBOT || (it->type == OBJ_BUILDING && it->ai_info)) {
      // we have a kill by a robot

      // see if it was a counter measure
      int cpnum = GetCounterMeasureOwner(it);
      if (!CheckPlayerNum(cpnum)) {
        // it was a real robot
        snprintf(temp, sizeof(temp), "\x01\x64\xFF\x64%s\x01\x01\xFF\x01", Players[me->id].callsign);
        snprintf(buffer, sizeof(buffer), DTXT_ROBOTKILL, temp);
      } else {
        // nope we have a player's counter measure
        DoRandomDeathMessage(Players[cpnum].objnum, Players[me->id].objnum);
        return;
      }
    } else {
      // we have a non-player killer
      snprintf(temp, sizeof(temp), "\x01\x64\xFF\x64%s\x01\x01\xFF\x01", Players[me->id].callsign);
      snprintf(buffer, sizeof(buffer), DTXT_STRAYFIREFORM, temp);
    }
  } else {
    // either me or it doesn't exist
    mprintf(0, "**********************************************************************\n");
    mprintf(0, "Either me or it doesn't exist for death message\n");

    if (me) {
      // only the killer doesn't exist
      snprintf(temp, sizeof(temp), "\x01\x64\xFF\x64%s\x01\x01\xFF\x01", Players[me->id].callsign);
      snprintf(buffer, sizeof(buffer), DTXT_NOKILLERDEATHMSG, temp);
    }
  }

  static int sound_id = -1;

  if (m_iDeathMessageFilter != DM_FILTER_NONE) {

    if (sound_id == -1) {
      sound_id = DLLFindSoundName("Hostage pickup");
    }

    DLLAddColoredHUDMessage(GR_RGB(1, 255, 1), buffer);

    if (sound_id != -1)
      DLLPlay3dSound(sound_id, &Objects[Players[GetPlayerNum()].objnum], MAX_GAME_VOLUME, 0);
  }

  if (do_statistical && (m_iProtectedFlags & DMFC_PRF_DISPSTATHUDMSGS)) {
    GetStatisticalDeathMessage(it->id, me->id);
  }
}

//	DMFCBase::GetStatisticalSuicideMessage
//
//	Generates a statistical suicide message about the suicide
char *DMFCBase::GetStatisticalSuicideMessage(int pnum) {
  static char buffer[150];
  strcpy(buffer, DTXT_SIMPLESUICIDEMSG);
  return buffer;
}

//	DMFCBase::GetStatisticalDeathMessage
//
//	Generates a statistical death message about the deaths
void DMFCBase::GetStatisticalDeathMessage(int knum, int vnum) {
  char buffer[150];

  PInfo *vpi, *kpi;
  tPExtraInfo *vei, *kei;

  vpi = FindPInfo(vnum);
  kpi = FindPInfo(knum);

  player_record *kpr = GetPlayerRecordByPnum(knum);
  player_record *vpr = GetPlayerRecordByPnum(vnum);

  if (!vpi || !kpi || !kpr || !vpr)
    return;

  vei = vpi->GetExtraInfo();
  kei = kpi->GetExtraInfo();

  int message_type = 0;
  int retries = 0;

death_retry:
  retries++;
  message_type = (int)(rand() % 10);

  if (retries > 3)
    return;

  switch (message_type) {
  case 0: // Kills in a row
  {
    if (kei->kills_in_a_row <= 2)
      goto death_retry;
    snprintf(buffer, sizeof(buffer), DTXT_HM_KILLSINAROW, kei->kills_in_a_row, kpr->callsign);
  } break;
  case 1: // Deaths in a row
  {
    if (vei->deaths_in_a_row <= 2)
      goto death_retry;
    snprintf(buffer, sizeof(buffer), DTXT_HM_DEATHSINAROW, vei->deaths_in_a_row, vpr->callsign);
  } break;
  case 2: // Time since last kill
  {
    if (kei->last_kill_time == 0)
      goto death_retry;

    float a = ((RealGametime) - (kei->last_kill_time));
    if (a < 60.0f)
      goto death_retry;
    snprintf(buffer, sizeof(buffer), DTXT_HM_TIMESINCELASTKILL, kpr->callsign, GetTimeString(a));
  } break;
  case 3: // Time since last death
  {
    if (vei->last_death_time == 0)
      goto death_retry;

    float a = ((RealGametime) - (vei->last_death_time));
    if (a < 60.0f)
      goto death_retry;
    snprintf(buffer, sizeof(buffer), DTXT_HM_TIMESINCELASTDEATH, vpr->callsign, GetTimeString(a));
  } break;
  case 4: // Efficiency
  {
    float kills = ((float)kpr->dstats.kills[DSTAT_LEVEL]);
    float deaths = ((float)kpr->dstats.deaths[DSTAT_LEVEL]);
    float suicides = ((float)kpr->dstats.suicides[DSTAT_LEVEL]);
    float a = kills / (kills + deaths + suicides) * 100.0f;

    if (a < 55.0f)
      goto death_retry;
    if (kills < 5)
      goto death_retry;

    if (a > 75.0f)
      snprintf(buffer, sizeof(buffer), DTXT_HM_HIGHEFFICENCY, kpr->callsign, a);
    else
      snprintf(buffer, sizeof(buffer), DTXT_HM_GOODEFFICENCY, kpr->callsign, a);
  } break;
  case 5: // Ranking
  {
    goto death_retry;
  } break;
  case 7: // How many times you killed the victim
  {
    int slot = PRec_GetPlayerSlot(vnum);
    tPKillerInfo *ki = kpi->GetKillerInfo(slot);

    if (ki->kills < 3)
      goto death_retry;
    snprintf(buffer, sizeof(buffer), DTXT_HM_TIMESKILLED, kpr->callsign, vpr->callsign, ki->kills);
  } break;
  case 8: { // revenge death
    int kslot = PRec_GetPlayerSlot(knum);
    int vslot = PRec_GetPlayerSlot(vnum);
    if (kslot == -1 || vslot == -1)
      goto death_retry;

    if (vei->last_kill_num == kslot && kei->last_death_num == vslot && !kei->got_revenge) {
      // there has been revenge
      snprintf(buffer, sizeof(buffer), DTXT_HM_REVENGE, kpr->callsign, vpr->callsign);
      kei->got_revenge = 1;
    } else
      goto death_retry;
  } break;
  case 9: {
    goto death_retry;
  } break;
  default:
    goto death_retry;
  }

  DLLAddHUDMessage(buffer);
}
