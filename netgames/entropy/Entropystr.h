// Defines for indexes into the string table for in-code strings

#ifndef __STRING_TABLE____
#define __STRING_TABLE____

#define TXT(index) GetStringFromTable(index)

// Returns a pointer to the string at the index location from the string table
// if it is a bad index given, then the pointer to the error string "ERROR MISSING STRING" is given

char *GetStringFromTable(int index);

#define TXT_GAMENAME TXT(0)        //"Entropy"
#define TXT_KILLA TXT(1)           //"%s was killed by %s"
#define TXT_SUICIDEA TXT(2)        //"%s killed himself"
#define TXT_PILOT TXT(3)           //"Pilot"
#define TXT_SCORE TXT(4)           //"Score"
#define TXT_KILLS TXT(5)           //"Kills"
#define TXT_DEATHS TXT(6)          //"Deaths"
#define TXT_SUICIDES TXT(7)        //"Suicides"
#define TXT_PING TXT(8)            //"Ping"
#define TXT_CANTCARRY TXT(9)       //"You can't carry this virus yet"
#define TXT_VIRUSDESTROYED TXT(10) //"Virus Destroyed"
#define TXT_CANTKILLVIRUS TXT(11)  //"You can't kill this virus"
#define TXT_TAKEOVER TXT(12)       //"%s Took Over A %s Team's %s Room"
#define TXT_NONAME TXT(13)         //"Someone"
#define TXT_HUDDISPLAY TXT(14)     //"%s Team: %d"
#define TXT_SAVESTATSA TXT(15)     //"Entropy\nGame: %s\nLevel: %d\n"
#define TXT_TEAMINFO TXT(16)       //"%s Team"
#define TXT_CURRRANKINGS TXT(17)   //"Current Level Rankings\n"
#define TXT_SAVESTATSB TXT(18)     //"Rank Name                         Score       Kills       Deaths     Suicides"
#define TXT_SAVESTATSC TXT(19)     //"\nIndividual Stats\n"
#define TXT_TIMEINGAME TXT(20)     //"Total Time In Game: %s"
#define TXT_SAVESTATSD TXT(21)     //"Callsign:                     Kills:    Deaths:"
#define TXT_HUDSCORE TXT(22)       //"%s Team(%d)"
#define TXT_VIRUSLOAD TXT(23)      //"Virus Load: %d/%d"
#define TXT_WELCOME TXT(24)        //"Welcome To Entropy, %s"
#define TXT_TEAMJOIN TXT(25)       //"You're on the %s Team"
#define TXT_JOIN TXT(26)           //"%s Has Joined The %s Team"
#define TXT_SAVED TXT(27)          //"Stats saved"
#define TXT_STARTGAME TXT(28)      //"Start Game"
#define TXT_VIRUSCREDIT TXT(29)    //"Virus Credit/Kill"
#define TXT_STATSGAMENAME TXT(30)  //"Entropy"
#define TXT_VIRUS TXT(31)          //"Virus"
#define TXT_PICKUPVIRUS TXT(32)    //"You picked up a virus"
#define TXT_KILLS_SHORT TXT(33)    //"K"
#define TXT_DEATHS_SHORT TXT(34)   //"D"
#define TXT_SUICIDES_SHORT TXT(35) //"S"

#endif
