#ifndef _AIAMBIENT_H_
#define _AIAMBIENT_H_

#include "cfile/cfile.h"

#define MAX_AL_TYPES 6
#define MAX_ALS_PER_TYPE 130

#define ALF_INSIDE 0x01

#define ALI_TYPE 0
#define ALI_TOTAL 1
#define ALI_MAX 2
#define ALI_MIN 3
#define ALI_FLAGS 4

#define CHECK_INTERVAL_MIN 5.0f
#define CHECK_INTERVAL_MAX 10.0f

class ambient_life {
  // Editor settable values
  int m_type[MAX_AL_TYPES];
  unsigned char m_total[MAX_AL_TYPES];
  unsigned char m_max[MAX_AL_TYPES];
  unsigned char m_min[MAX_AL_TYPES];
  unsigned char m_flags[MAX_AL_TYPES];

  // These are never set or gotten from outside of the class
  unsigned char m_cur_num[MAX_AL_TYPES];
  int m_handle[MAX_AL_TYPES][MAX_ALS_PER_TYPE];

  // Don't save these...
  unsigned char m_next_size[MAX_AL_TYPES];
  float m_next_do_time[MAX_AL_TYPES];

  void ComputeNextSize(char index);

public:
  ambient_life() { ALReset(); }

  void GetALValue(char index, char field, void *ptr);
  void SetALValue(char index, char field, void *ptr);
  void SaveData(CFILE *fptr);
  void LoadData(CFILE *fptr);

  void DoFrame();
  void InitForLevel(void);
  void ALReset();
};

extern ambient_life a_life;

#endif