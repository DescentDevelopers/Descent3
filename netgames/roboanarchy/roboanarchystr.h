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

#ifndef __ANARCHY_STRING_TABLE_H_
#define __ANARCHY_STRING_TABLE_H_

#define TXT(d) GetString(d)

#define TXT_DEATH1 TXT(0)        //"%s got blasted by %s"
#define TXT_DEATH2 TXT(1)        //"%s knows %s is his god"
#define TXT_DEATH3 TXT(2)        //"%s sucks %s's milk"
#define TXT_DEATH4 TXT(3)        //"%s realizes %s's power"
#define TXT_DEATH5 TXT(4)        //"%s got killed by %s"
#define TXT_DEATH6 TXT(5)        //"%s begs for %s's mercy"
#define TXT_DEATH7 TXT(6)        //"%s realizes %s is a better player"
#define TXT_DEATH8 TXT(7)        //"%s was no match for %s"
#define TXT_DEATH9 TXT(8)        //"%s wishes he was as good as %s"
#define TXT_DEATH10 TXT(9)       //"%s got messed up by %s"
#define TXT_SUICIDE1 TXT(10)     //"%s blasts himself"
#define TXT_SUICIDE2 TXT(11)     //"%s Bursts his own bubble"
#define TXT_SUICIDE3 TXT(12)     //"%s doesn't know his own strength"
#define TXT_SUICIDE4 TXT(13)     //"No prize for %s"
#define TXT_SUICIDE5 TXT(14)     //"%s doesn't wish to live anymore"
#define TXT_SUICIDE6 TXT(15)     //"%s SUCKS!"
#define TXT_PILOT TXT(16)        //"Pilot"
#define TXT_KILLS TXT(17)        //"Kills"
#define TXT_DEATHS TXT(18)       //"Deaths"
#define TXT_SUICIDES TXT(19)     //"Suicides"
#define TXT_SCORE TXT(20)        //"Score"
#define TXT_STATS TXT(21)        //"Stats"
#define TXT_WELCOME TXT(22)      //"Welcome to the Anarchy %s!"
#define TXT_JOINED TXT(23)       //"%s has joined the Anarchy"
#define TXT_POINTS TXT(24)       //"Points"
#define TXT_HUDD_NONE TXT(25)    //"Anarchy: HUD Display set to None"
#define TXT_HUDD_SCORES TXT(26)  //"Anarchy: HUD Display set to Scores"
#define TXT_HUDD_EFFIC TXT(27)   //"Anarchy: HUD Display set to Efficiency"
#define TXT_MNU_HUDSTYLE TXT(28) //"HUD Display"
#define TXT_NONE TXT(29)         //"None"
#define TXT_SCOREHD TXT(30)      //"Score"
#define TXT_EFFICIENCY TXT(31)   //"Efficiency"
#define TXT_MNU_HUDCOLOR TXT(32) //"HUD Score Colors"
#define TXT_PLAYERCOLORS TXT(33) //"Player Colors"
#define TXT_NORMAL TXT(34)       //"Normal"
#define TXT_PING TXT(35)         //"Ping"
#define TXT_SAVE_HEADER TXT(36)  //"Anarchy\r\nGame: %s\r\nLevel: %d\r\n"
#define TXT_SAVE_HEADERB                                                                                               \
  TXT(37) //"[Rank] [Name]                      [Score]     [Kills]     [Deaths]   [Suicides] [Time In Game]"
#define TXT_SAVE_HEADERC TXT(38)    //"\r\nIndividual Stats\r\n"
#define TXT_SAVE_TIMEINGAME TXT(39) //"Total Time In Game: %s"
#define TXT_SAVE_KILLERLIST TXT(40) //"Callsign:                     Kills:    Deaths:"
#define TXT_MSG_SAVESTATS TXT(41)   //"Stats saved to file"
#define TXT_MSG_COLORPLR TXT(42)    //"Anarchy HUD Color Model: Player Colors"
#define TXT_MSG_COLORNORM TXT(43)   //"Anarchy HUD Color Model: Normal"
#define TXT_KILLS_SHORT TXT(44)     //"K"
#define TXT_DEATHS_SHORT TXT(45)    //"D"
#define TXT_SUICIDES_SHORT TXT(46)  //"S"

#endif
