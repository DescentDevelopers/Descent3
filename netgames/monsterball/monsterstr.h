// Defines for indexes into the string table for in-code strings

#ifndef __STRING_TABLE____
#define __STRING_TABLE____

#define TXT(index) GetStringFromTable(index)

// Returns a pointer to the string at the index location from the string table
// if it is a bad index given, then the pointer to the error string "ERROR MISSING STRING" is given

char *GetStringFromTable(int index);

#define TXT_GAMENAME TXT(0)           //"Monsterball"
#define TXT_KILLEDA TXT(1)            //"%s was killed by %s"
#define TXT_SUICIDEA TXT(2)           //"%s lost control"
#define TXT_PILOT TXT(3)              //"Pilot"
#define TXT_POINTS TXT(4)             //"Points"
#define TXT_BLUNDERS TXT(5)           //"Blunders"
#define TXT_KILLSSHORT TXT(6)         //"K"
#define TXT_DEATHSSHORT TXT(7)        //"D"
#define TXT_SUICIDESSHORT TXT(8)      //"S"
#define TXT_PING TXT(9)               //"Ping"
#define TXT_SCOREONE TXT(10)          //"%s Team Scores 1 point!"
#define TXT_SCOREMULTI TXT(11)        //"%s Team Scores %d points!!"
#define TXT_BLUNDERSCORE TXT(12)      //"%s accidently scores a point for the %s team!"
#define TXT_BLUNDERMULTISCORE TXT(13) //"%s accidently scores %d points for the %s team!"
#define TXT_PLAYERSCORE TXT(14)       //"%s (%s) knocks the ball in for a point!"
#define TXT_PLAYERMULTISCORE TXT(15)  //"%s (%s) knocks the ball in for %d points!"
#define TXT_HUDDISPLAY TXT(16)        //"%s Team: %d"
#define TXT_KILLS TXT(17)             //"Kills"
#define TXT_DEATHS TXT(18)            //"Deaths"
#define TXT_SUICIDES TXT(19)          //"Suicides"
#define TXT_SAVESTATSA TXT(20)        //"Monsterball\r\nGame: %s\nLevel: %d"
#define TXT_SAVESTATSTEAM TXT(21)     //"%s Team"
#define TXT_SAVESTATSB TXT(22)        //"Current Level Rankings\r\n"
#define TXT_SAVESTATSC                                                                                                 \
  TXT(23) //"Rank Name                         Points  Blunders     Kills       Deaths     Suicides"
#define TXT_SAVESTATSD TXT(24)   //"\r\nIndividual Stats\r\n"
#define TXT_TIMEINGAME TXT(25)   //"Total Time In Game: %s"
#define TXT_SAVESTATSE TXT(26)   //"Team: Callsign:                     Kills:    Deaths:"
#define TXT_STATSSAVED TXT(27)   //"Stats saved to file"
#define TXT_HUDSCORE TXT(28)     //"[%d]%s Team"
#define TXT_WELCOME TXT(29)      //"Welcome To Monsterball %s"
#define TXT_ONTEAM TXT(30)       //"You're On The %s Team"
#define TXT_JOINED TXT(31)       //"%s Has Joined The %s Team"
#define TXT_STATGAMENAME TXT(32) //"Monsterball"
#endif
