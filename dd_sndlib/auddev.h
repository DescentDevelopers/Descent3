/*
 * $Source: $
 * $Revision: 16 $
 * $Author: Samir $
 * $Date: 8/23/99 5:29p $
 *
 * 
 *
 * $Log: /DescentIII/Main/dd_sndlib/auddev.h $
 * 
 * 16    8/23/99 5:29p Samir
 * incremental EAX 2.0 checkin
 * 
 * 15    8/13/99 2:00p Samir
 * more aureal and geometry fixes.
 * 
 * 14    8/11/99 3:12p Samir
 * fixes for aureal support.
 * 
 * 13    5/03/99 3:12a Samir
 * added event management routines.
 * 
 * 12    4/29/99 3:01p Samir
 * added some more functions.
 * 
 * 11    4/23/99 7:51p Samir
 * added flush function.
 * 
 * 10    4/13/99 4:09p Samir
 * more priority stuff.
 * 
 * 9     4/06/99 8:30p Samir
 * added reflection support.
 * 
 * 8     3/29/99 11:02a Samir
 * added geometry supoprt.
 * 
 * 7     1/14/99 6:10p Samir
 * added DirectSound buffer duplication code.
 * 
 * 6     1/11/99 5:52p Samir
 * updated EAX support.
 * 
 * 5     1/08/99 6:31p Samir
 * added reverb
 * 
 * 4     1/08/99 11:36a Samir
 * implemented basic Aureal 2.0 support.
 * 
 * 3     12/23/98 11:50a Samir
 * 
 * 2     12/23/98 11:48a Samir
 * basic functionality.
 * 
 * 1     12/22/98 11:56a Samir
 * 
 */

#ifndef AUDDEV_H
#define AUDDEV_H

#include "dsound.h"
#include "vmanpset.h"
#include "pstypes.h"
#include "vecmat.h"

#define SOUND_DECAY_MAXTIME		100.0f
#define SOUND_GAIN_MAXSCALAR		1.0f

//#define SUPPORT_AUREAL

// Creative labs Environmental Audio Extension Interface

//	starts up standard audio device (DirectSound)
bool EAX_Create(GUID *pguid, LPDIRECTSOUND *lpds);								

//	shutdown standard audio device (DirectSound) 
void EAX_Destroy();													

// returns EAX caps
#define EAXF_SOURCE_OBSTRUCTION		16
int EAX_Caps();

// initializes EAX specific interfaces.
bool EAX_SetPrimaryBuffer();		

// sets up current global environment reverb
bool EAX_SetEnvironmentalReverb(float volume, float damping, float decay);

// intializes a sound source for EAX
bool EAX_InitSource(LPDIRECTSOUND3DBUFFER lpBuffer3D, LPKSPROPERTYSET *plpksp);

//	frees an eax sound source
void EAX_FreeSource(LPKSPROPERTYSET lpksp);

//	sets source properties
void EAX_SetSourceProperties(LPKSPROPERTYSET lpksp, float obstruction);



#ifdef SUPPORT_AUREAL
//////////////////////////////////////////////////////////////////////////////
// Aureal 2.0 Direct Path Interface

// Initialize object
bool A3D_Init(HWND hwnd);	 

// destroy object													
void A3D_Destroy();

// sets units of measurement
void A3D_SetUnitsPerMeter(float units);

// sets rolloff for all source objects (from 0.0f to ???)
void A3D_SetRolloffFactor(float scalar);

// sets doppler for all source objects (from 0.0f to ???) (1.0f is default, 0.0f is none)
void A3D_SetDopplerFactor(float scalar);

// create or destroy geometry object
bool A3D_CreateGeometryInterface();
void A3D_DestroyGeometryInterface();

//	create listener
bool A3D_CreateListener();

// create source
void *A3D_CreateSSource(int size, int bps, bool stereo, bool streaming, ushort freq =22050,bool is3d=false);

// duplicates source
void *A3D_DuplicateSource(void *src);

// frees source
void A3D_FreeSSource(void *obj);

// loads data into source
bool A3D_LoadSample(void *obj, void *src_data, int src_length);

// plays a sound
void A3D_Play(void *obj, bool looping);

// stops a sound
void A3D_Stop(void *obj);

// returns current position in buffer of playback
void A3D_GetCurrentPosition(void *obj,uint *playp);

// sets current position in buffer
void A3D_SetCurrentPosition(void *obj, uint pos);

// sets properties for source object
// priority is from 0 to 1.0
void A3D_SetSourcePriority(void *obj, float priority);

// pan from 0 to 1.0f
void A3D_SetSourcePan(void *obj, float lpan, float rpan);
void A3D_GetSourcePan(void *obj, float *lpan, float *rpan);

// volume from 0 to 1.0f
void A3D_SetSourceVolume(void *obj, float volume);
float A3D_GetSourceVolume(void *obj);

void A3D_SetSourceCone(void *obj, float x, float y, float z);

void A3D_SetSourceVelocity(void *obj, float x, float y, float z);
void A3D_GetSourceVelocity(void *obj, vector *vel);

void A3D_SetSourcePosition(void *obj, float x, float y, float z);
void A3D_GetSourcePosition(void *obj, vector *pos);

void A3D_SetSourceOrientation(void *obj, const vector *fvec, const vector *uvec);
void A3D_GetSourceOrientation(void *obj, matrix *orient);

bool A3D_SetSourceWaveEvent(void *obj, DWORD offset, HANDLE hEvent);
bool A3D_ClearSourceWaveEvents(void *obj);

void A3D_SetMinMaxDistance(void *obj, float min, float max, bool mute_max=false);

// sets properties for listener
void A3D_SetListenerVelocity(float x, float y, float z);
void A3D_SetListenerOrient(vector *fvec, vector *uvec);
void A3D_SetListenerPosition(float x, float y, float z);

// call at start of sound frame
void A3D_StartFrame();
void A3D_EndFrame();
void A3D_Flush();

// gets status of current source buffer
#define A3D_STATUS_PLAYING 0x1

int A3D_GetSourceStatus(void *obj);

bool A3D_Lock(void *obj, DWORD dwWriteCursor, DWORD dwWriteBytes, void **ptr1, LPDWORD plen1, void **ptr2, LPDWORD plen2);
bool A3D_Unlock(void *obj, void *ptr1, DWORD len1, void *ptr2, DWORD len2);


// sets up current global environment reverb
bool A3D_SetEnvironmentalReverb(float volume, float damping, float decay);

// room builder
// list = list id.  0 is only valid.
void A3D_GeomListStart(int list);
void A3D_GeomListEnd(int list);
void A3D_GeomListExec(int list);
void A3D_Clear();									// clears geometry info

// all below functions act on the current list.
void A3D_GeomAddListener(vector *pos, matrix *orient);
void A3D_GeomAddSource(void *obj, vector *pos, matrix *orient);
void A3D_GeomBindMaterial (void *obj);
void A3D_GeomAddTriangle(unsigned tag, vector **verts);
void A3D_GeomAddQuad(unsigned tag, vector **verts);

// used to create aureal 2.0 materials.
// gain and highfreq values range from 0 to 1.0
void *A3D_CreateMaterial(float xmit_gain, float xmit_highfreq, float reflect_gain, float reflect_highfreq);
void A3D_DestroyMaterial(void *obj);
#endif

#endif