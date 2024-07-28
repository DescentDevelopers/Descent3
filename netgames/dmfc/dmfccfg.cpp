/*
* Descent 3 
* Copyright (C) 2024 Parallax Software
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.

--- HISTORICAL COMMENTS FOLLOW ---

 * $Logfile: /DescentIII/Main/dmfc/dmfccfg.cpp $
 * $Revision: 1.1.1.1 $
 * $Date: 2003/08/26 03:57:20 $
 * $Author: kevinb $
 *
 * Functions for a D3M config file
 *
 * $Log: dmfccfg.cpp,v $
 * Revision 1.1.1.1  2003/08/26 03:57:20  kevinb
 * initial 1.5 import
 *
 *
 * 7     9/12/99 12:06a Jeff
 * fixed atoxi bug (stupid bug)
 *
 * 6     5/17/99 2:34p Ardussi
 * changed to compile on Mac
 *
 * 5     5/13/99 4:55p Ardussi
 * changes for compiling on the Mac
 *
 * 4     3/17/99 12:23p Jeff
 * converted DMFC to be COM interface
 *
 * 3     9/21/98 7:11p Jeff
 * made InputCommand interface API and moved existing input commands to
 * the interface.  Changed mprintf/ASSERT so they are valid in DMFC
 *
 * $NoKeywords: $
 */

#include "gamedll_header.h"
#include "DMFC.h"
#include "dmfcinternal.h"


CRegistry *reg = NULL;

// DMFCBase::CFGOpen
//
//	Opens the registry/cfg information for the DMFC game, see error codes for return values
int DMFCBase::CFGOpen(char *filename) {
  if (!filename)
    return CFG_NOCFGFILE;

  if (reg)
    return CFG_ALREADYOPEN;

  reg = new CRegistry(filename);
  if (!reg)
    return CFG_OUTOFMEMORY;

  reg->Import();

  return CFG_NOERROR;
}

// DMFCBase::CFGClose
//
//	Closes the registry information for the game.
void DMFCBase::CFGClose(void) {
  if (reg) {
    reg->Export();
    delete reg;
    reg = NULL;
  }
}

// DMFCBase::CFGFlush
//
//	Flushes out the registry information to fall
int DMFCBase::CFGFlush(void) {
  if (reg) {
    reg->Export();
    return CFG_NOERROR;
  } else
    return CFG_NOTOPEN;
}

// DMFCBase::CFGCreateKey
//
//  Creates a key in the registry
int DMFCBase::CFGCreateKey(char *name) {
  if (reg) {
    reg->CreateKey(name);
    return CFG_NOERROR;
  } else
    return CFG_NOTOPEN;
}

// DMFCBase::CFGLookupKey
//
//   Sets the active key in the registry
int DMFCBase::CFGLookupKey(char *name) {
  if (reg) {
    if (reg->LookupKey(name))
      return CFG_NOERROR;
    else
      return CFG_KEYNOTFOUND;
  } else
    return CFG_NOTOPEN;
}

// DMFCBase::CFGLookupRecord
//
//	Looks up a record in the active key
int DMFCBase::CFGLookupRecord(char *record, void *data) {
  if (reg) {
    if (reg->LookupRecord(record, data))
      return CFG_NOERROR;
    else
      return CFG_RECORDNOTFOUND;
  } else
    return CFG_NOTOPEN;
}

// DMFCBase::CFGCreateRecord
//
// Create/overwrites a record in the active key. Type is either REGT_DWORD or REGT_STRING
int DMFCBase::CFGCreateRecord(char *name, char type, void *data) {
  if (reg) {
    if (reg->CreateRecord(name, type, data))
      return CFG_NOERROR;
    else
      return CFG_CANTCREATE;
  } else
    return CFG_NOTOPEN;
}

// Convert a string that represents a hex value into an int
    int
    axtoi(char *p) {
  int value = 0;
  while ((p) && (*p)) {
    *p = toupper(*p);
    if ((*p >= '0') && (*p <= '9'))
      value = (value * 16) + ((*p) - '0');
    else if ((*p >= 'A') && (*p <= 'F'))
      value = (value * 16) + (((*p) - 'A') + 10);
    else
      return 0;
    p++;
  }
  return value;
}

// Removes whitespace from the start of the given string.
// Returns a pointer to the first non-white character
char *CRegistry::SkipWhite(char *p) {
  while (isspace(*p))
    p++;
  return p;
}

// Parses a quoted string
// Returns true if got string ok, else false
char *CRegistry::ParseString(char *p, char *buf, int bufsize, char sdelim, char edelim) {
  char *save_p;

  p = SkipWhite(p);

  save_p = p;

  if (*p != sdelim) {
    return NULL;
  } else {
    p++; // skip initial quote
  }

  // Copy chars until endquote or out of space
  while (*p && (*p != edelim) && --bufsize)
    *buf++ = *p++;

  // Check for buffer overflow
  if (bufsize <= 0) {
    return NULL;
  }

  // Check for missing endquote
  if (!*p) {
    return NULL;
  }

  // Write terminator
  *buf = 0;

  // Return new pointer (move over a char for end quote)
  return p + 1;
}

// Parses a sequence of non-space characters
char *CRegistry::ParseToken(char *p, char *buf, int bufsize) {
  char *save_p;
  p = SkipWhite(p);
  save_p = p;

  while (!isspace(*p) && (*p != ',') && *p && --bufsize)
    *buf++ = *p++;

  *buf = 0;

  // Check for buffer overflow
  if (bufsize <= 0) {
    return NULL;
  }

  return p;
}

#define PARSE_KEY(buf)                                                                                                 \
  do {                                                                                                                 \
    ptr = ParseString(ptr, buf, sizeof(buf), '[', ']');                                                                \
  } while (0)
#define PARSE_STRING(buf)                                                                                              \
  do {                                                                                                                 \
    ptr = ParseString(ptr, buf, sizeof(buf), '"', '"');                                                                \
  } while (0)
#define PARSE_TOKEN(buf)                                                                                               \
  do {                                                                                                                 \
    ptr = ParseToken(ptr, buf, sizeof(buf));                                                                           \
  } while (0)

CRegistry::CRegistry(const char *str) {
  currentkey = root = NULL;
  strcpy(name, str);
}

CRegistry::~CRegistry() { Destroy(); }

void CRegistry::Destroy(void) {
  tKey *curr, *next;
  curr = next = root;

  while (curr) {
    next = curr->next;
    DestroyKey(curr);
    curr = next;
  }
  root = NULL;
}

void CRegistry::DestroyKey(tKey *key) {
  tRecord *curr, *next;
  curr = next = key->records;

  while (curr) {
    next = curr->next;
    DestroyRecord(curr);
    curr = next;
  }

  free(key);
}

void CRegistry::DestroyRecord(tRecord *record) {
  if (record->data) {
    free(record->data);
    record->data = NULL;
  }
  free(record);
}

void CRegistry::Export() {
  tKey *curr, *next;
  curr = next = root;
  CFILE *file;
  DLLOpenCFILE(&file, name, "wt");

  if (!file)
    return;

  while (curr) {
    next = curr->next;
    ExportKey(curr, file);
    curr = next;
  }
  DLLcfclose(file);
}

void CRegistry::ExportKey(tKey *key, CFILE *file) {
  tRecord *curr, *next;
  curr = next = key->records;

  // write out name
  char buffer[258];
  snprintf(buffer, sizeof(buffer), "[%s]", key->name);
  DLLcf_WriteString(file, buffer);
  while (curr) {
    next = curr->next;
    ExportRecord(curr, file);
    curr = next;
  }
}

void CRegistry::ExportRecord(tRecord *record, CFILE *file) {
  int *dw;
  char *st;
  char buffer[512];
  switch (record->type) {
  case REGT_STRING: {
    st = (char *)record->data;
    snprintf(buffer, sizeof(buffer), "\"%s\"=\"%s\"", record->name, st);
  } break;
  case REGT_DWORD: {
    dw = (int *)record->data;
    snprintf(buffer, sizeof(buffer), "\"%s\"=dword:%X", record->name, *dw);
  } break;
  };
  DLLcf_WriteString(file, buffer);
}

bool CRegistry::Import() {
  char buffer[500];
  char newbuff[500];
  CFILE *file;
  char *ptr;

  DLLOpenCFILE(&file, name, "rt");

  if (!file) {
    mprintf(0, "Unable to import %s\n", name);
    return false;
  }
  mprintf(0, "Importing %s\n", name);
  Destroy();

  bool oktocreate;
  // loop till we are done
  while (!DLLcfeof(file)) {
    oktocreate = true;
    char type = REGT_STRING;

    // read in the string
    DLLcf_ReadString(buffer, 500, file);
    if (strlen(buffer) <= 1)
      continue;

    ptr = buffer;
    if (DLLcfeof(file)) // we are at the end of the file so there isn't data to continue
      oktocreate = false;

    // see what we read, a key or record, if the first character is a [ than a key " is record
    if (oktocreate) {
      if (buffer[0] == '[') {
        // Create a key!
        PARSE_KEY(newbuff);
        mprintf(0, "Found Key: |%s|\n", newbuff);
        CreateKey(newbuff);
      } else if (buffer[0] == '\"') {
        // Create a record
        // see what type of record by looking at whats after the =
        char *p;
        p = buffer;
        bool done = false;
        type = REGT_STRING;
        while (!done) {
          if ((!p) || (!*p))
            done = true;
          if ((p) && (*p == '=')) {
            if (*(p + 1) == 'd')
              type = REGT_DWORD;
            else
              type = REGT_STRING;
            done = true;
          }
          p++;
        }

        // now we "SHOULD" know the type, parse the info
        char data[300];
        int idata;

        switch (type) {
        case REGT_STRING:
          PARSE_STRING(newbuff);
          ptr++; // blow by =
          PARSE_STRING(data);
          if (!CreateRecord(newbuff, REGT_STRING, data)) {
            mprintf(0, "Unable to create String record: %s\n", newbuff);
          } else {
            mprintf(0, "Created String record %s = %s\n", newbuff, data);
          }
          break;
        case REGT_DWORD:
          PARSE_STRING(newbuff);
          ptr += 7; // blow by =dword:
          PARSE_TOKEN(data);
          idata = axtoi(data);

          if (!CreateRecord(newbuff, REGT_DWORD, &idata)) {
            mprintf(0, "Unable to create dword record: %s\n", newbuff);
          } else {
            mprintf(0, "Created dword record %s = %X\n", newbuff, idata);
          }
          break;
        }
      } else {
        mprintf(0, "Expected [ or \"\n");
      }
    }
  }
  DLLcfclose(file);
  return true;
}

void CRegistry::CreateKey(const char *name) {
  tKey *curr;
  if (LookupKey(name)) {
    mprintf(0, "Key: %s already exists\n", name);
    return;
  }

  if (!root) {
    root = (tKey *)malloc(sizeof(tKey));
    if (!root)
      return;
    curr = root;
  } else {
    curr = root;
    while (curr->next) {
      curr = curr->next;
    }

    curr->next = (tKey *)malloc(sizeof(tKey));
    if (!curr->next)
      return;
    curr = curr->next;
  }

  curr->next = NULL;
  strcpy(curr->name, name);
  curr->records = NULL;
  currentkey = curr;
}

bool CRegistry::LookupKey(const char *name) {
  tKey *curr;
  curr = root;
  while (curr) {
    if (!stricmp(name, curr->name)) {
      // found a match
      currentkey = curr;
      return true;
    }
    curr = curr->next;
  }
  return false;
}

tRecord *CRegistry::LookupRecord(const char *record, void *data) {
  if (!currentkey)
    return NULL;

  tRecord *curr;
  curr = currentkey->records;
  while (curr) {
    if (!stricmp(record, curr->name)) {
      // found the record
      switch (curr->type) {
      case REGT_STRING:
        char *st;
        st = (char *)curr->data;
        strcpy((char *)data, st);
        break;
      case REGT_DWORD:
        int *dw;
        dw = (int *)curr->data;
        *((int *)data) = *dw;
        break;
      };
      return curr;
    }
    curr = curr->next;
  }
  return NULL;
}

int CRegistry::GetDataSize(const char *record) {
  if (!currentkey)
    return false;
  tRecord *curr;
  curr = currentkey->records;
  while (curr) {
    if (!stricmp(record, curr->name)) {
      // found the record
      switch (curr->type) {
      case REGT_STRING:
        return strlen((char *)curr->data) + 1;
        break;
      case REGT_DWORD:
        return sizeof(int);
        break;
      };
      return 0;
    }
    curr = curr->next;
  }
  return 0;
}

bool CRegistry::CreateRecord(const char *name, char type, void *data) {
  if (!currentkey)
    return false;
  tRecord *curr;

  // first see if the record exists under this key
  int datasize = GetDataSize(name);
  if (datasize) {
    char *olddata = (char *)malloc(datasize);
    if (olddata) {
      curr = LookupRecord(name, olddata);
      if (curr) {
        // ok we have an old value, replace it!
        mprintf(0, "Replacing %s\n", name);
        if (curr->data)
          free(curr->data);
        free(olddata);
        curr->type = type;

        switch (type) {
        case REGT_STRING:
          curr->data = malloc(strlen((char *)data) + 1);
          strcpy((char *)curr->data, (char *)data);
          break;
        case REGT_DWORD:
          curr->data = malloc(sizeof(int));
          *((int *)curr->data) = *((int *)data);
          break;
        }
        return true;
      }
    }
  }

  // it is a new record
  if (currentkey->records) {
    curr = currentkey->records;
    while (curr->next)
      curr = curr->next;

    curr->next = (tRecord *)malloc(sizeof(tRecord));
    if (!curr->next)
      return false;
    curr = curr->next;
  } else {
    currentkey->records = (tRecord *)malloc(sizeof(tRecord));
    if (!currentkey->records)
      return false;
    curr = currentkey->records;
  }

  curr->next = NULL;
  strcpy(curr->name, name);
  switch (type) {
  case REGT_STRING:
    curr->data = malloc(strlen((char *)data) + 1);
    curr->type = REGT_STRING;
    strcpy((char *)curr->data, (char *)data);
    break;
  case REGT_DWORD:
    curr->data = malloc(sizeof(int));
    curr->type = REGT_DWORD;
    *((int *)curr->data) = *((int *)data);
    break;
  };
  return true;
}
