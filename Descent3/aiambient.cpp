#include "aiambient.h"
#include "string.h"
#include "objinfo.h"
#include <stdlib.h>
#include "game.h"
#include "psrand.h"

ambient_life a_life;

#define AL_VERSION 1

void ambient_life::DoFrame(void)
{
}

void ambient_life::InitForLevel(void)
{
	char i;

	for(i = 0; i < MAX_AL_TYPES; i++)
	{
		ComputeNextSize(i);
		m_cur_num[i] = 0;
	   m_next_do_time[i] = Gametime;
	}

	DoFrame();
}

void ambient_life::GetALValue(char i, char field, void *ptr)
{
	switch(field)
	{
		case ALI_TYPE:
			*((int *)ptr) = m_type[i];
		break;
		case ALI_TOTAL:
			*((unsigned char *)ptr) = m_total[i];
		break;
		case ALI_MAX:
			*((unsigned char *)ptr) = m_max[i];
		break;
		case ALI_MIN:
			*((unsigned char *)ptr) = m_min[i];
		break;
		case ALI_FLAGS:
			*((unsigned char *)ptr) = m_flags[i];
		break;
	}
}

void ambient_life::ComputeNextSize(char i)
{
	char diff = m_max[i] - m_min[i];
	if(diff > 0)
	{
		char offset = ps_rand()%diff;
		m_next_size[i] = m_min[i] + offset;
	}
	else
	{
		m_next_size[i] = m_max[i];
	}
}

void ambient_life::SetALValue(char i, char field, void *ptr)
{
	switch(field)
	{
		case ALI_TYPE:
			m_type[i] = *((int *)ptr);
		break;
		case ALI_TOTAL:
			m_total[i] = *((unsigned char *)ptr);
		break;
		case ALI_MAX:
			m_max[i] = *((unsigned char *)ptr);
		break;
		case ALI_MIN:
			m_min[i] = *((unsigned char *)ptr);
		break;
		case ALI_FLAGS:
			m_flags[i] = *((unsigned char *)ptr);
		break;
	}

	//Jeff(Linux): Commented out because m_total[] is unsigned, so never negative
	//if(m_total[i] < 0)
	//	m_total[i] = 0;
	if(m_total[i] > MAX_ALS_PER_TYPE)
		m_total[i] = MAX_ALS_PER_TYPE;

	//Jeff(Linux): Commented out because m_max[] is unsigned, so never negative
	//if(m_max[i] < 0)
	//	m_max[i] = 0;
	if(m_max[i] > m_total[i])
		m_max[i] = m_total[i];

	//Jeff(Linux): Commented out because m_min[] is unsigned, so never negative
	//if(m_min[i] < 0)
	//	m_min[i] = 0;
	if(m_min[i] > m_max[i])
		m_min[i] = m_max[i];

	ComputeNextSize(i);
}

void ambient_life::ALReset()
{
	int i;
	int j;

	for(i = 0; i < MAX_AL_TYPES; i++)
	{
		m_type[i] = -1;

		for(j = 0; j < MAX_ALS_PER_TYPE; j++)
		{
			m_handle[i][j] = 0;
		}

		m_total[i] = 0;
		m_cur_num[i] = 0;
		m_flags[i] = 0;

		m_min[i] = 0;
		m_max[i] = 0;
		
		m_next_do_time[i] = 0;
		m_next_size[i] = 0;
	}
}

void ambient_life::SaveData(CFILE *fp)
{
	int i, j;
	short len;

	cf_WriteInt(fp, AL_VERSION);

	for(i = 0; i < MAX_AL_TYPES; i++)
	{
		int type = m_type[i];

		if(type >= 0)
		{
			len = strlen(Object_info[type].name) + 1; // Accounts for NULL charactor
			cf_WriteShort(fp, len);
			for(j = 0; j < len; j++)
			{
				cf_WriteByte(fp, Object_info[type].name[j]);
			}
		}
		else
		{
			cf_WriteShort(fp, 1);
			cf_WriteByte(fp, '\0');
		}

		cf_WriteByte(fp, m_total[i]);
		cf_WriteByte(fp, m_flags[i]);
		cf_WriteByte(fp, m_min[i]);
		cf_WriteByte(fp, m_max[i]);
		cf_WriteByte(fp, m_next_size[i]);
		cf_WriteFloat(fp, m_next_do_time[i]);
	}

	for(i = 0; i < MAX_AL_TYPES; i++)
	{
		cf_WriteByte(fp, m_cur_num[i]);

		for(j = 0; j < m_cur_num[i]; j++)
		{
			cf_WriteInt(fp, m_handle[i][j]);
		}
	}
}

void ambient_life::LoadData(CFILE *fp)
{
	int version = cf_ReadInt(fp);
	int i, j;
	int len;
	char temp_name[256];

	if(version < 1)
		return;

	for(i = 0; i < MAX_AL_TYPES; i++)
	{
		len =  cf_ReadShort(fp);
		for(j = 0; j < len; j++)
		{
			{
				temp_name[j] = cf_ReadByte(fp);
			}
		}

		m_type[i] = FindObjectIDName(temp_name);

		m_total[i] = cf_ReadByte(fp);
		m_flags[i] = cf_ReadByte(fp);
		m_min[i] = cf_ReadByte(fp);
		m_max[i] = cf_ReadByte(fp);
		m_next_size[i] = cf_ReadByte(fp);
		m_next_do_time[i] = cf_ReadFloat(fp);
	}

	for(i = 0; i < MAX_AL_TYPES; i++)
	{
		m_cur_num[i] = cf_ReadByte(fp);

		for(j = 0; j < m_cur_num[i]; j++)
		{
			m_handle[i][j] = cf_ReadInt(fp);
		}
	}
}