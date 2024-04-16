//Defines for indexes into the string table for in-code strings

#ifndef __STRING_TABLE____
#define __STRING_TABLE____

#define TXT(index)	GetStringFromTable(index)

//Returns a pointer to the string at the index location from the string table
//if it is a bad index given, then the pointer to the error string "ERROR MISSING STRING" is given

char *GetStringFromTable(int index);

#define	TXT_DEATH1			TXT(0)	//"%s got butchered by %s"
#define	TXT_SUICIDE1			TXT(1)	//"%s shags himself"
#define	TXT_PILOT			TXT(2)	//"Pilot"
#define	TXT_KILLS			TXT(3)	//"Kills"
#define	TXT_DEATHS			TXT(4)	//"Deaths"
#define	TXT_SUICIDES			TXT(5)	//"Suicides"
#define	TXT_SCORE			TXT(6)	//"Score"
#define	TXT_LOSTORB			TXT(7)	//"%s Lost The HyperOrb!!!"
#define	TXT_GOTORB			TXT(8)	//"%s Has The HyperOrb!!!"
#define	TXT_SCOREPOINTS			TXT(9)	//"%s racks up another %d points!"
#define	TXT_STATS			TXT(10)	//"Stats"
#define	TXT_WELCOME			TXT(11)	//"Welcome to Hyper Anarchy %s!"
#define	TXT_JOINED			TXT(12)	//"%s has joined the Hyper Anarchy!"
#define	TXT_GAMENAME			TXT(13)	//"Hyper-Anarchy"
#define	TXT_STATSGAMENAME			TXT(14)	//"Anarchy"
#define	TXT_PING			TXT(15)	//"Ping"
#define	TXT_HUDSCORECOLORS			TXT(16)	//"HUD Score Colors"
#define	TXT_PLAYERCOLORS			TXT(17)	//"Player Colors"
#define	TXT_NORMAL			TXT(18)	//"Normal"
#define	TXT_SAVESTATSA			TXT(19)	//"Hyper-Anarchy\r\nGame: %s\r\nLevel: %d\r\n"
#define	TXT_SAVESTATSB			TXT(20)	//"Current Level Rankings\r\n"
#define	TXT_SAVESTATSC			TXT(21)	//"[Rank] [Name]                     [Score]     [Kills]     [Deaths]   [Suicides] [Time In Game]"
#define	TXT_SAVESTATSD			TXT(22)	//"\r\nIndividual Stats\r\n"
#define	TXT_TIMEINGAME			TXT(23)	//"Total Time In Game: %s"
#define	TXT_SAVESTATSE			TXT(24)	//"Callsign:                     Kills:    Deaths:"
#define	TXT_STATSAVED			TXT(25)	//"Stats saved to file"
#define	TXT_KILLS_SHORT			TXT(26)	//"K"
#define	TXT_DEATHS_SHORT		TXT(27)	//"D"
#define	TXT_SUICIDES_SHORT		TXT(28)	//"S"
#endif
