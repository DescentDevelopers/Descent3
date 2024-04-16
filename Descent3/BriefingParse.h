/*
* $Logfile: /DescentIII/main/BriefingParse.h $
* $Revision: 7 $
* $Date: 4/17/99 6:15p $
* $Author: Samir $
*
* 
*
* $Log: /DescentIII/main/BriefingParse.h $
 * 
 * 7     4/17/99 6:15p Samir
 * replaced gr.h with grdefs.h and fixed resulting compile bugs.
 * 
 * 6     4/16/99 1:51a Jeff
 * include linux fix for stricmp
 * 
 * 5     4/14/99 3:56a Jeff
 * fixed case mismatch in #includes
 * 
 * 4     1/04/99 12:32p Jeff
 * added support for mission flag parsing
 * 
 * 3     9/11/98 6:06p Jeff
 * Briefing Editor completed
 * 
 * 2     9/09/98 7:02p Jeff
 * Initial Creation
*
* $NoKeywords: $
*/


#ifndef __BRIEFPARSE_H_
#define __BRIEFPARSE_H_

#if defined(__LINUX__)
#include "linux/linux_fix.h"//for stricmp
#endif

#include "TelComEfxStructs.h"
#include "grdefs.h"

//	Return codes of ParseBriefing
#define PBERR_FILENOTEXIST	-1
#define PBERR_NOERR			0


typedef struct{
	void (*AddTextEffect)(LPTCTEXTDESC desc,char *text,char *description,int id);
	void (*AddBmpEffect)(LPTCBMPDESC desc,char *description);
	void (*AddMovieEffect)(LPTCMOVIEDESC desc,char *description);
	void (*AddBkgEffect)(LPTCBKGDESC desc,char *description);
	void (*AddPolyEffect)(LPTCPOLYDESC desc,char *description);
	void (*AddSoundEffect)(LPTCSNDDESC desc,char *description);
	void (*AddButtonEffect)(LPTCBUTTONDESC desc,char *description,int id);
	void (*StartScreen)(int screen_num,char *description,char *layout,uint mask_set,uint mask_unset);
	void (*EndScreen)();
	bool (*LoopCallback)();
	void (*SetTitle)(char *title);
	void (*SetStatic)(float amount);
	void (*SetGlitch)(float amount);
	void (*AddVoice)(char *filename,int flags,char *description);
}tBriefParseCallbacks;

typedef struct{
	TCTEXTDESC textdesc;
	int text_id;
	int teffect;
	char description[128];
}tTextBufferDesc;

class CBriefParse
{
public:
	CBriefParse();
	~CBriefParse();
	void SetCallbacks(tBriefParseCallbacks *cb);
	int ParseBriefing(char *filename);
private:
	void (*AddTextEffect)(LPTCTEXTDESC desc,char *text,char *description,int id);
	void (*AddBmpEffect)(LPTCBMPDESC desc,char *description);
	void (*AddMovieEffect)(LPTCMOVIEDESC desc,char *description);
	void (*AddBkgEffect)(LPTCBKGDESC desc,char *description);
	void (*AddPolyEffect)(LPTCPOLYDESC desc,char *description);
	void (*AddSoundEffect)(LPTCSNDDESC desc,char *description);
	void (*AddButtonEffect)(LPTCBUTTONDESC desc,char *description,int id);
	void (*StartScreen)(int screen_num,char *desc,char *layout,uint mask_set,uint mask_unset);
	void (*EndScreen)();
	bool (*LoopCallback)();
	void (*SetTitle)(char *title);
	void (*SetStatic)(float amount);
	void (*SetGlitch)(float amount);
	void (*AddVoice)(char *filename,int flags,char *description);

	void ParseError(char *msg,char *p=NULL);
	char *ParseComma(char *p);
	char *ParseFloat(char *p,float *f);
	char *ParseInt(char *p,int *i);
	char *ParseString(char *p,char *buf,int bufsize);
	char *ParseToken(char *p,char *buf,int bufsize);
	char *SkipWhite(char *p);
	char *ParseKeyword(char *p,int *keyword_id);
	bool ParseButtonEffect(char *p);
	bool ParseTextEffect(char *p,tTextBufferDesc *tbd);

	int linenum;
	bool parse_error;
};


#endif