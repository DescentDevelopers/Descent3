// InfFile.h: interface for the InfFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INFFILE_H__D8F94664_216E_11D2_AF2D_0060089A8025__INCLUDED_)
#define AFX_INFFILE_H__D8F94664_216E_11D2_AF2D_0060089A8025__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "pstypes.h"
#include "psclass.h"

struct CFILE;

#define INFFILE_LINELEN 256
#define INFFILE_CUSTOM -128
#define INFFILE_ERROR -1
#define INFFILE_COMMENT -2
#define INFFILE_EOL -3
#define INFFILE_SYMBOL 1024

class InfFile {
public:
  InfFile();
  virtual ~InfFile();

  //	opens an inf file, pass in a lexical analyzer that will return a command index.
  //	tag_check is a string that must appear at the very beginning of the file.
  //	lexfn is a function to match a command to a number.   num <= 0 and num >=1024 are taken.
  bool Open(const char *filename, const char *tag_check, int (*lexfn)(const char *command));

  //	closes the file
  void Close();

  //	read a command line. returns false on eof.
  bool ReadLine();

  //	parses a line of 'code'.  return value from the lexfn
  //	check INFFILE constants above. (any value less than 0, you should read in a new line.)
  int ParseLine(char *operand, int oprlen);

private:
  CFILE *m_fp;                  //	file object.
  int (*m_lexfn)(const char *); // lexical analyzer.
  char m_linetext[INFFILE_LINELEN];
  char *m_lineptr; // pointer to current text line.
  char *m_endptr;  // pointer to last character in text line
  int m_line;      // line #

  struct sym_info // full runtime symbol
  {
    char name[PSFILENAME_LEN + 1];
    char *text;
  };

  // inf file symbol list
  tList<sym_info> m_sym_list; // symbol list.

private:
  void AddSymbol(const char *name, const char *text);
  const char *GetSymbolText(const char *name);
  void FreeSymbols();

public:
  // line number
  int line() const { return m_line; };
};

#endif // !defined(AFX_INFFILE_H__D8F94664_216E_11D2_AF2D_0060089A8025__INCLUDED_)
