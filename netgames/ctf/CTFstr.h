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

#ifndef __CTF_STR_H_
#define __CTF_STR_H_

#define TXT(d) GetString(d)

#define TXT_DEATH1 TXT(0)          //"%s got blasted by %s"
#define TXT_DEATH2 TXT(1)          //"%s knows %s is his god"
#define TXT_DEATH3 TXT(2)          //"%s sucks %s's milk"
#define TXT_DEATH4 TXT(3)          //"%s realizes %s's power"
#define TXT_DEATH5 TXT(4)          //"%s got killed by %s"
#define TXT_DEATH6 TXT(5)          //"%s begs for %s's mercy"
#define TXT_DEATH7 TXT(6)          //"%s realizes %s is a better player"
#define TXT_DEATH8 TXT(7)          //"%s was no match for %s"
#define TXT_DEATH9 TXT(8)          //"%s wishes he was as good as %s"
#define TXT_DEATH10 TXT(9)         //"%s got messed up by %s"
#define TXT_SUICIDE1 TXT(10)       //"%s blasts himself"
#define TXT_SUICIDE2 TXT(11)       //"%s Bursts his own bubble"
#define TXT_SUICIDE3 TXT(12)       //"%s doesn't know his own strength"
#define TXT_SUICIDE4 TXT(13)       //"No prize for %s"
#define TXT_SUICIDE5 TXT(14)       //"%s doesn't wish to live anymore"
#define TXT_SUICIDE6 TXT(15)       //"%s SUCKS!"
#define TXT_TEAMSCORE TXT(16)      //"%s Team Scores %d Points [%d]"
#define TXT_PICKUPFLAGSPEW TXT(17) //"%s (%s) finds the %s Flag among some debris!"
#define TXT_PICKUPFLAG TXT(18)     //"%s (%s) picks up the %s flag"
#define TXT_STATS TXT(19)          //"Stats"
#define TXT_TEAMSCOREFORM TXT(20)  //"%s Team %d[%d]"
#define TXT_TEAMFORM TXT(21)       //"%s Team"
#define TXT_WELCOME TXT(22)        //"Welcome to Capture The Flag %s!"
#define TXT_JOINED TXT(23)         //"%s has joined CTF"
#define TXT_PILOT TXT(24)          //"Pilot"
#define TXT_KILLS TXT(25)          //"Kills"
#define TXT_DEATHS TXT(26)         //"Deaths"
#define TXT_SUICIDES TXT(27)       //"Suicides"
#define TXT_POINTS TXT(28)         //"Points"
#define TXT_CTF TXT(29)            //"Capture The Flag"
#define TXT_PING TXT(30)           //"Ping"
#define TXT_HATTRICK TXT(31)       //"%s has achieved a Hat Trick!!!"
#define TXT_HATTRICKFIRST TXT(32)  //"%s is the first to get a Hat Trick!!!"
#define TXT_CAPTURE TXT(33)        //"%s (%s) captures the %s Flag!"
#define TXT_CAPTURETWO TXT(34)     //"%s (%s) captures the %s and %s Flags!"
#define TXT_CAPTURETHREE TXT(35)   //"%s (%s) captures the %s, %s and %s Flags!"
#define TXT_RETURN TXT(36)         //"%s (%s) returns the %s Flag!"
#define TXT_STAT_HEADING TXT(37)   //"Capture The Flag\r\nGame: %s\r\nLevel: %d\r\n"
#define TXT_STAT_HEADINGA                                                                                              \
  TXT(38) //"Rank Team   Name                         Score       Kills       Deaths     Suicides  Time In Game"
#define TXT_STAT_HEADINGB TXT(39)  //"\r\nIndividual Stats\r\n"
#define TXT_STAT_TIG TXT(40)       //"Total Time In Game: %s"
#define TXT_STAT_HEADINGC TXT(41)  //"Callsign:                     Kills:    Deaths:"
#define TXT_STAT_SAVED TXT(42)     //"Stats saved to file"
#define TXT_KILLS_SHORT TXT(43)    //"K"
#define TXT_DEATHS_SHORT TXT(44)   //"D"
#define TXT_SUICIDES_SHORT TXT(45) //"S"

#endif
