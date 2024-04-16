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
