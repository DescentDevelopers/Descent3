/*
 *
 * Database functions
 *
 *
 * $NoKeywords: $
 */
#include "appdatabase.h"
#include <string.h>
#include "mem.h"
#include "cfile.h"
#include "ddio.h"
#include "ddio_mac.h"
#include "descent.h"
#include "ddebug/debug.h"
#include "pserror.h"

char RECORD_FORMAT_STR[] = ":%s";
char ENTRY_FORMAT_STR[] = "-\t%s = %s";

#define STR_BUFF_SIZE 1024

void oeMacAppDatabase::DBReadIn(void) {
  char str[64];
  DBRecord *record = NULL;
  DBEntry *entry = NULL;

  this->data = NULL;

  while (fgets(str, STR_BUFF_SIZE, this->file)) {
    if (str[0] == RECORD_FORMAT_STR[0]) {
      if (record) {
        record->next = (DBRecord *)mem_malloc(sizeof(DBRecord));
        record = record->next;
      } else {
        record = (DBRecord *)mem_malloc(sizeof(DBRecord));
        this->data = record;
      }
      sscanf(str, RECORD_FORMAT_STR, record->name);
      record->entries = NULL;
      record->next = NULL;
      entry = NULL;
    } else if (str[0] == ENTRY_FORMAT_STR[0]) {
      if (!record) {
        DebugStr("\pD3 Error: Bad Database Format");
        return;
      }
      if (entry) {
        entry->next = (DBEntry *)mem_malloc(sizeof(DBEntry));
        entry = entry->next;
      } else {
        entry = (DBEntry *)mem_malloc(sizeof(DBEntry));
        record->entries = entry;
      }
      // DAJ			sscanf(str, ENTRY_FORMAT_STR, entry->label, entry->entry);
      // DAJ			entry->next = NULL;

      char temp_label[STR_BUFF_SIZE], temp_entry[STR_BUFF_SIZE];
      if (strlen(str) < sizeof(temp_entry)) {
        sscanf(str, ENTRY_FORMAT_STR, temp_label, temp_entry);
        entry->label = mem_strdup(temp_label);
        entry->entry = mem_strdup(temp_entry);
        entry->next = NULL;
      } else {
        mprintf((2, "DBReadIn: entry string to long\n"));
        Int3();
      }
    }

    // no error for other line starts
    // allow other lines as comments or blanks
  }
}
void oeMacAppDatabase::DBWriteOut(void) {
  char str[STR_BUFF_SIZE];
  DBRecord *record;
  DBEntry *entry;
  fseek(this->file, 0, SEEK_SET);

  record = this->data;

  while (record) {
    sprintf(str, RECORD_FORMAT_STR, record->name);
    fputs(str, this->file);
    fputc('\n', this->file);

    entry = record->entries;
    while (entry) {
      sprintf(str, ENTRY_FORMAT_STR, entry->label, entry->entry);
      fputs(str, this->file);
      fputc('\n', this->file);

      entry = entry->next;
    }
    record = record->next;
  }
}

// Construction and destruction.
oeMacAppDatabase::oeMacAppDatabase() {
  // Open up the database file, for reading, read in all data and keep it in memory
  // then close the database
  OSErr err;
  short foundVRefNum;
  long foundDirID;
  FSSpec spec;

  err = FindFolder(kOnSystemDisk, 'pref', kDontCreateFolder, &foundVRefNum, &foundDirID);
  if (err)
    mprintf((2, "unable to find preferences folder\n"));

  err = FSMakeFSSpec(foundVRefNum, foundDirID, "\pDescent3 Prefs", &spec);
  if (!(err == fnfErr || err == noErr))
    mprintf((2, "unable to make FSspec for Descent3 Prefs\n"));

  this->file = FSp_fopen(&spec, "rt", 'pref');
  this->data = NULL;
  this->current = NULL;
  if (this->file) {
    DBReadIn();
    fclose(this->file);
    this->file = NULL;
  }
  create_record("Version");
}
oeMacAppDatabase::oeMacAppDatabase(oeMacAppDatabase *parent) {
  this->file = NULL;
  this->data = parent->data;
  this->current = NULL;
}
oeMacAppDatabase::~oeMacAppDatabase() {
  OSErr err;
  short foundVRefNum;
  long foundDirID;
  FSSpec spec;

  err = FindFolder(kOnSystemDisk, 'pref', kDontCreateFolder, &foundVRefNum, &foundDirID);
  if (err)
    mprintf((2, "unable to find preferences folder\n"));

  err = FSMakeFSSpec(foundVRefNum, foundDirID, "\pDescent3 Prefs", &spec);
  if (!(err == fnfErr || err == noErr))
    mprintf((2, "unable to make FSspec for Descent3 Prefs\n"));

  this->file = FSp_fopen(&spec, "wt", 'pref');

  if (this->file) {
    DBWriteOut();
    fclose(this->file);
    this->file = NULL;
  }
}
// Record functions
// these are actual folders of information
// creates an empty classification or structure where you can store information
bool oeMacAppDatabase::create_record(const char *pathname) {
  DBRecord *last = NULL;
  DBRecord *record = NULL;
  int len = strlen(pathname);

  record = this->data;

  while (record) {
    if (len == strlen(record->name) && stricmp(pathname, record->name) == 0) {
      current = record;
      return true;
    }
    last = record;
    record = record->next;
  }
  if (last) {
    last->next = (DBRecord *)mem_malloc(sizeof(DBRecord));
    this->current = record = last->next;
  } else {
    this->data = (DBRecord *)mem_malloc(sizeof(DBRecord));
    this->current = record = this->data;
  }

  strcpy(record->name, pathname);
  record->entries = NULL;
  record->next = NULL;

  return true;
}
// set current database focus to a particular record
bool oeMacAppDatabase::lookup_record(const char *pathname) {
  DBRecord *record;
  int len = strlen(pathname);

  record = this->data;

  while (record) {
    if (len == strlen(record->name) && stricmp(pathname, record->name) == 0) {
      current = record;
      return true;
    }
    record = record->next;
  }
  return false;
}
// read either a string from the current record
bool oeMacAppDatabase::read(const char *label, char *entry, int *entrylen) {
  DBEntry *record;
  int len = strlen(label);

  record = this->current->entries;

  while (record) {
    if (len == strlen(record->label) && stricmp(label, record->label) == 0) {
      strcpy(entry, record->entry);
      *entrylen = strlen(entry);
      return true;
    }
    record = record->next;
  }
  return false;
}
// read a variable-sized integer from the current record
bool oeMacAppDatabase::read(const char *label, void *entry, int wordsize) {
  DBEntry *record;
  int len = strlen(label);

  record = this->current->entries;

  while (record) {
    if (len == strlen(record->label) && stricmp(label, record->label) == 0) {
      int value;

      sscanf(record->entry, "%d", &value);
      switch (wordsize) {
      case 1:
        *((unsigned char *)entry) = (unsigned char)value;
        break;
      case 2:
        *((unsigned short *)entry) = (unsigned short)value;
        break;
      case 4:
        *((unsigned int *)entry) = (unsigned int)value;
        break;
      default:
        DebugStr("\pD3 Error: Unable to read key, unsupported size");
        return false;
        break;
      }
      return true;
    }
    record = record->next;
  }
  return false;
}
bool oeMacAppDatabase::read(const char *label, bool *entry) {
  bool value;
  if (!read(label, &value, sizeof(bool)))
    return false;
  *entry = (value != 0) ? true : false;
  return true;
}
// write either an integer or string to a record.
bool oeMacAppDatabase::write(const char *label, const char *entry, int entrylen) {
  DBEntry *record, *last = NULL;
  int len = strlen(label);

  record = this->current->entries;

  while (record) {
    if (len == strlen(record->label) && stricmp(label, record->label) == 0) {
      strcpy(record->entry, entry);
      return true;
    }
    last = record;
    record = record->next;
  }
  if (last) {
    last->next = (DBEntry *)mem_malloc(sizeof(DBEntry));
    record = last->next;
  } else {
    this->current->entries = (DBEntry *)mem_malloc(sizeof(DBEntry));
    record = this->current->entries;
  }

  record->label = mem_strdup(label);
  record->entry = mem_strdup(entry);
  record->next = NULL;
  return true;
}
bool oeMacAppDatabase::write(const char *label, int entry) {
  char str[32];

  sprintf(str, "%d", entry);
  return (write(label, str, strlen(str)));
}
// get the current user's name from the os
void oeMacAppDatabase::get_user_name(char *buffer, ulong *size) {
#ifdef FIXED
  char temp[STR_BUFF_SIZE];
  if (cuserid(temp))
    strncpy(buffer, temp, (*size) - 1);
  else
    strncpy(buffer, "Unknown", (*size) - 1);
  buffer[(*size) - 1] = '\0';
  *size = strlen(buffer);
#endif // FIXED
  strncpy(buffer, "Unknown", (*size) - 1);
  buffer[(*size) - 1] = '\0';
  *size = strlen(buffer);
}
