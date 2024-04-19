/*
 * $Source: /home/kevin/cvsstuff/descent3/descent3/Main/FontEditor/FontEditor.h,v $
 * $Revision: 1.1.1.1 $
 * $Author: kevinb $
 * $Date: 2003-08-26 03:57:45 $
 *
 * Font Editor
 *
 * $Log: not supported by cvs2svn $
 * 
 * 2     5/19/99 8:48p Samir
 * added kerning dump utilitiy.
 * 
 * 1     4/17/99 4:04p Samir
 * standard header.
 * 
 */

extern void FontDoIO();
extern void FontKern(const char *fnt_file_name);
extern void FontKernDump(const char *fnt_file_name);
extern void FontView(const char *fnt_file_name);
extern void FontCreate(const char *fnt_file_source, const char *fnt_file_dest, int min_ascii);

extern int System_font_handle;

