// Defines for indexes into the string table for in-code strings

#ifndef __STRING_TABLE____
#define __STRING_TABLE____

#define TXT(index) GetStringFromTable(index)

// Returns a pointer to the string at the index location from the string table
// if it is a bad index given, then the pointer to the error string "ERROR MISSING STRING" is given

char *GetStringFromTable(int index);

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
#define TXT_SCOREONE TXT(16)       //"%s scores 1 point [%d]"
#define TXT_SCOREMULTI TXT(17)     //"%s scores %d points [%d]"
#define TXT_GOTHOARD TXT(18)       //"You got a hoard orb"
#define TXT_MAXHOARDS TXT(19)      //"You've got all the orbs you can handle"
#define TXT_PILOT TXT(20)          //"Pilot"
#define TXT_LEVEL TXT(21)          //"Level"
#define TXT_POINTS TXT(22)         //"Points"
#define TXT_HIGH TXT(23)           //"High"
#define TXT_GOAL TXT(24)           //"Goal"
#define TXT_KILLS TXT(25)          //"Kills"
#define TXT_DEATHS TXT(26)         //"Deaths"
#define TXT_SUICIDES TXT(27)       //"Suicides"
#define TXT_WELCOME TXT(28)        //"Welcome to the Hoard %s!"
#define TXT_JOINED TXT(29)         //"%s has joined the Hoard"
#define TXT_STATS TXT(30)          //"Stats"
#define TXT_GAMENAME TXT(31)       //"Hoard"
#define TXT_SCORE TXT(32)          //"Score"
#define TXT_HIGHEST TXT(33)        //"Highest"
#define TXT_PING TXT(34)           //"Ping"
#define TXT_HUDSCORECOLORS TXT(35) //"HUD Score Colors"
#define TXT_PLAYERCOLORS TXT(36)   //"Player Colors"
#define TXT_NORMAL TXT(37)         //"Normal"
#define TXT_GAMECONFIG TXT(38)     //"Game Configuration"
#define TXT_NEEDMOREOBS TXT(39)    //"You need at least %d orbs to score"
#define TXT_STATSSAVEA TXT(40)     //"Hoard\r\nGame: %s\r\nLevel: %d\r\n"
#define TXT_SAVESTATSB TXT(41)     //"Current Level Rankings\r\n"
#define TXT_SAVESTATSC                                                                                                 \
  TXT(42) //"[Rank] [Name]                     [Score]     [Kills]     [Deaths]   [Suicides] [Highest Goal] [Time In
          //Game]"
#define TXT_SAVESTATSD TXT(43)     //"\r\nIndividual Stats\r\n"
#define TXT_TIMEINGAME TXT(44)     //"Total Time In Game: %s"
#define TXT_SAVESTATSE TXT(45)     //"Callsign:                     Kills:    Deaths:"
#define TXT_STATSSAVED TXT(46)     //"Stats saved to file"
#define TXT_STATGAMENAME TXT(47)   //"Hoard"
#define TXT_RETURNTOGAME TXT(48)   //"Back to Game"
#define TXT_MINIMUMORBS TXT(49)    //"Minimum Hoard Orbs To Score"
#define TXT_KILLS_SHORT TXT(50)    //"K"
#define TXT_DEATHS_SHORT TXT(51)   //"D"
#define TXT_SUICIDES_SHORT TXT(52) //"S"
#endif
