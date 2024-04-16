// Defines for indexes into the string table for in-code strings

#ifndef __STRING_TABLE____
#define __STRING_TABLE____

#define TXT(index) GetStringFromTable(index)

// Returns a pointer to the string at the index location from the string table
// if it is a bad index given, then the pointer to the error string "ERROR MISSING STRING" is given

char *GetStringFromTable(int index);

#define TXT_COOP TXT(0)          //"Coop"
#define TXT_STATGAMENAME TXT(1)  //"Co-op Descent 3"
#define TXT_PILOT TXT(2)         //"Pilot"
#define TXT_KILLS TXT(3)         //"Kills"
#define TXT_DEATHS TXT(4)        //"Deaths"
#define TXT_SUICIDES TXT(5)      //"Suicides"
#define TXT_PING TXT(6)          //"Ping"
#define TXT_WELCOME TXT(7)       //"Welcome to the Co-Op Game %s"
#define TXT_JOINED TXT(8)        //"%s has joined the game"
#define TXT_LEVELCOMPLETE TXT(9) //"Level Completed!"
#endif
