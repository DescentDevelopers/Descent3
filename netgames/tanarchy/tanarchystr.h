//Defines for indexes into the string table for in-code strings

#ifndef __STRING_TABLE____
#define __STRING_TABLE____

#define TXT(index)	GetStringFromTable(index)

//Returns a pointer to the string at the index location from the string table
//if it is a bad index given, then the pointer to the error string "ERROR MISSING STRING" is given

char *GetStringFromTable(int index);

#define	TXT_DEATH1			TXT(0)	//"%s got blasted by %s"
#define	TXT_DEATH2			TXT(1)	//"%s knows %s is his god"
#define	TXT_DEATH3			TXT(2)	//"%s sucks %s's milk"
#define	TXT_DEATH4			TXT(3)	//"%s realizes %s's power"
#define	TXT_DEATH5			TXT(4)	//"%s got killed by %s"
#define	TXT_DEATH6			TXT(5)	//"%s begs for %s's mercy"
#define	TXT_DEATH7			TXT(6)	//"%s realizes %s is a better player"
#define	TXT_DEATH8			TXT(7)	//"%s was no match for %s"
#define	TXT_DEATH9			TXT(8)	//"%s wishes he was as good as %s"
#define	TXT_DEATH10			TXT(9)	//"%s got messed up by %s"
#define	TXT_SUICIDE1			TXT(10)	//"%s blasts himself"
#define	TXT_SUICIDE2			TXT(11)	//"%s Bursts his own bubble"
#define	TXT_SUICIDE3			TXT(12)	//"%s doesn't know his own strength"
#define	TXT_SUICIDE4			TXT(13)	//"No prize for %s"
#define	TXT_SUICIDE5			TXT(14)	//"%s doesn't wish to live anymore"
#define	TXT_SUICIDE6			TXT(15)	//"%s SUCKS!"
#define	TXT_STATS			TXT(16)	//"Stats"
#define	TXT_TEAM			TXT(17)	//"%s Team"
#define	TXT_TEAMSCORE			TXT(18)	//"%s Team: %d"
#define	TXT_WELCOME			TXT(19)	//"Welcome to Team Anarchy %s!"
#define	TXT_TEAMJOINMSG			TXT(20)	//"You're on the %s Team"
#define	TXT_JOINED			TXT(21)	//"%s has joined the %s Team"
#define	TXT_PILOT			TXT(22)	//"Pilot"
#define	TXT_KILLS			TXT(23)	//"Kills"
#define	TXT_DEATHS			TXT(24)	//"Deaths"
#define	TXT_SUICIDES			TXT(25)	//"Suicides"
#define	TXT_POINTS			TXT(26)	//"Points"
#define	TXT_GAMENAME			TXT(27)	//"Team Anarchy"
#define	TXT_SCORE			TXT(28)	//"Score"
#define	TXT_PING			TXT(29)	//"Ping"
#define	TXT_SAVESTATSA			TXT(30)	//"Team Anarchy\nGame: %s\nLevel: %d\n"
#define	TXT_SAVESTATSB			TXT(31)	//"Current Level Rankings\n"
#define	TXT_SAVESTATSC			TXT(32)	//"Rank Name                         Score       Kills       Deaths     Suicides"
#define	TXT_SAVESTATSD			TXT(33)	//"\nIndividual Stats\n"
#define	TXT_TIMEINGAME			TXT(34)	//"Total Time In Game: %s"
#define	TXT_SAVESTATSE			TXT(35)	//"Callsign:                     Kills:    Deaths:"
#define	TXT_STATSSAVED			TXT(36)	//"Stats saved to %s"
#define TXT_STATSGAMENAME		TXT(37)	//"Team Anarchy"
#define TXT_KILLS_SHORT		TXT(38)	//"K"
#define TXT_DEATHS_SHORT	TXT(39)	//"D"
#define TXT_SUICIDES_SHORT	TXT(40)	//"S"

#endif
