/*
* $Logfile: /DescentIII/Main/linux/registry.h $
* $Revision: 1.2 $
* $Date: 2004/02/09 04:14:51 $
* $Author: kevinb $
*
* Linux registry header
*
* $Log: registry.h,v $
* Revision 1.2  2004/02/09 04:14:51  kevinb
* Added newlines to all headers to reduce number of warnings printed
*
* Made some small changes to get everything compiling.
*
* All Ready to merge the 1.5 tree.
*
* Revision 1.1.1.1  2000/04/18 00:00:39  icculus
* initial checkin
*
 * 
 * 4     7/14/99 9:09p Jeff
 * added comment header
*
* $NoKeywords: $
*/

#ifndef __REGISTRY_H_
#define __REGISTRY_H_

#define MAX_RECORD_NAME	256
#define REGT_STRING	0
#define REGT_DWORD	1

#include <stdio.h>

typedef struct tRecord
{
  char name[MAX_RECORD_NAME];
  char type;
  void *data;
  tRecord *next;
}tRecord;

typedef struct tKey
{
  char name[MAX_RECORD_NAME];
  tKey *next;
  tRecord *records;
}tKey;


class CRegistry
{
public:
  CRegistry(char *name);
  ~CRegistry();
  void Export();
  bool Import();
  void CreateKey(char *name);
  bool LookupKey(char *name);
  bool CreateRecord(char *name,char type,void *data);
  tRecord *LookupRecord(char *record,void *data);
  int GetDataSize(char *record);
  void GetSystemName(char *name);
  void SetSystemName(char *name);
private:
  void Destroy(void);
  void DestroyKey(tKey *key);
  void DestroyRecord(tRecord *record);
  void ExportKey(tKey *key,FILE *file);
  void ExportRecord(tRecord *record,FILE *file);
  char name[MAX_RECORD_NAME];
  tKey *root;
  tKey *currentkey;
};

#endif


