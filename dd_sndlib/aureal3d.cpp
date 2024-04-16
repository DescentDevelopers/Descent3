/*
 * $Logfile: /DescentIII/Main/dd_sndlib/aureal3d.cpp $
 * $Revision: 18 $
 * $Date: 8/19/99 10:25a $
 * $Author: Samir $
 *
 * Aureal enhancements to sound library.
 *
 * $Log: /DescentIII/Main/dd_sndlib/aureal3d.cpp $
 *
 * 18    8/19/99 10:25a Samir
 * tweaks for Aureal as requested by Aureal.
 *
 * 17    8/13/99 2:00p Samir
 * more aureal and geometry fixes.
 *
 * 16    8/11/99 3:12p Samir
 * fixes for aureal support.
 *
 * 15    7/30/99 5:03p Samir
 * an attempt to get some aureal reflection support working.
 *
 * 14    7/20/99 4:09p Samir
 * fixed reflection support.  tag polygons for reuse.
 *
 * 13    5/03/99 12:35p Samir
 * added config file support for aureal features.
 *
 * 12    5/03/99 3:12a Samir
 * added event management routines.
 *
 * 11    4/29/99 3:01p Samir
 * added some more functions.
 *
 * 10    4/23/99 7:51p Samir
 * added flush function
 *
 * 9     4/13/99 4:09p Samir
 * more priority stuff.
 *
 * 8     4/06/99 8:30p Samir
 * added reflection support.
 *
 * 7     4/01/99 4:28p Samir
 * added some error checking.
 *
 * 6     3/29/99 10:53a Samir
 * added new Aureal 2.0 sdk support.
 *
 * 5     1/14/99 6:10p Samir
 * added DirectSound buffer duplication code.
 *
 * 4     1/08/99 6:31p Samir
 * added reverb
 *
 * 3     1/08/99 11:36a Samir
 * implemented basic Aureal 2.0 support.
 *
 * 2     12/23/98 11:48a Samir
 * basic functionality.
 *
 * 1     7/02/98 4:38p Samir
 * Initial revision.
 *
 * $NoKeywords: $
 */

#if 0

#include "auddev.h"

#include <objbase.h>
#include <initguid.h>

#include <stdio.h>
#include <stdlib.h>

#include "ia3dutil.h"
#include "ia3dapi.h"
#include "pserror.h"
#include "logfile.h"
#include "Macros.h"
#include "inffile.h"

typedef struct tA3D
{
	IA3d4 *a3d;
	IA3dListener *listener;
	IA3dGeom     *geom;							// Geometry object.
	IA3dList		 *list;							// list of geometry commands for a room.
	DWORD geom_mode_flags;						// active elements in A3d geometry engine.
	float global_gain;
	float reflect_time;
	float reflect_scale;							// reflection scale
	float reflect_gain;							// reflection game
	bool	do_occlusions;
	bool	do_reflections;
	sbyte reflection_update_interval;
}
tA3D;

static tA3D A3D = { NULL, NULL, NULL, NULL, 0,  };
static logfile a3dlog;
static bool A3D_first_init = false;

const DWORD A3D_FEATURES =   A3D_OCCLUSIONS | A3D_1ST_REFLECTIONS | A3D_DISABLE_SPLASHSCREEN;
const A3DVAL A3D_PRIORITY_BIAS = 0.60f;
const DWORD A3D_FALLBACK_SOURCES = 16;

int A3D_buffers_allocated=0;

void A3D_FinalShutdown();

#define A3DCMD_NUM 7
#define A3DCMD_GAIN 0
#define A3DCMD_OCC 1
#define A3DCMD_REF 2
#define A3DCMD_REFINT 3
#define A3DCMD_REFTIME 4
#define A3DCMD_REFSCALE 5
#define A3DCMD_REFGAIN 6

const char *A3DCommands[A3DCMD_NUM] = {
	"gain",
	"occlusions",
	"reflections",
	"reflectinterval",
	"reflecttime",
	"reflectscale",
	"reflectgain"
};



int A3DLex(const char *command)
{
	for (int i = 0; i < A3DCMD_NUM; i++)
	{
		if (strcmp(A3DCommands[i], command) == 0) 
			return i;
	}

	return INFFILE_ERROR;
}


//////////////////////////////////////////////////////////////////////////////
// Initialize object
bool A3D_Init(HWND hwnd)
{
	InfFile cfgfile;
	int hr;

	if (!A3D_first_init) {
		a3dlog.start("a3d.log", "Aureal 3D 2.0 Interface");
		atexit(A3D_FinalShutdown);
		A3D_first_init = true;
	}

	a3dlog.puts("initializing\n");

// Core Initialization
	hr = A3dInitialize();
	if (SUCCEEDED(hr)) {
		hr = A3dCreate(NULL, (void **)&A3D.a3d, NULL, A3D_FEATURES);
		if (hr != S_OK) {
			a3dlog.printf("failed to create a3d object (%x).\n", hr);
			A3dUninitialize();
			return false;
		}
	}

// okay read in config file.
	A3D.global_gain = 1.0f;
	A3D.do_occlusions = true;
	A3D.do_reflections = true;
	A3D.reflection_update_interval = 1;
	A3D.reflect_time = 1.0f;
	A3D.reflect_scale = 0.95f;
	A3D.reflect_gain = A3D.global_gain;
	
	if (cfgfile.Open("a3d.inf", NULL, A3DLex)) {
		a3dlog.puts("a3d.inf found:\n");
		while (cfgfile.ReadLine())
		{
			char operand[32];
			int cmd;
			while ((cmd = cfgfile.ParseLine(operand, INFFILE_LINELEN)) > INFFILE_ERROR) 
			{
				switch (cmd)
				{
					float fval;
					bool bval;
					int ival;

				case A3DCMD_GAIN:
					fval = atof(operand);
					if (fval < 0) fval = 0;
					else if (fval > 1.0f) fval = 1.0f;
					A3D.global_gain = fval;
					a3dlog.printf("gain = %.2f\n", fval);
					break;
				case A3DCMD_OCC:
					bval = (strcmpi(operand, "true")==0) ? true : false;
					A3D.do_occlusions = bval;
					a3dlog.printf("occlutions = %s\n", operand);
					break;
				case A3DCMD_REF:
					bval = (strcmpi(operand, "true")==0) ? true : false;
					A3D.do_reflections = bval;
					a3dlog.printf("reflections = %s\n", operand);
					break;
				case A3DCMD_REFINT:
					ival = atoi(operand);
					if (ival < 1) ival = 1;
					else if (ival > 6) ival = 6;
					A3D.reflection_update_interval = ival;
					a3dlog.printf("update interval = %d\n", ival);
					break;
				case A3DCMD_REFTIME:
					fval = atof(operand);
					if (fval < 0.0f) fval = 0.0f;
					A3D.reflect_time = fval;
					a3dlog.printf("max reflect time = %.2f", fval);
					break;
				case A3DCMD_REFSCALE:
					fval = atof(operand);
					if (fval < 0.0f) fval = 0.0f;
					A3D.reflect_scale = fval;
					a3dlog.printf("max reflect scale = %.2f", fval);
					break;
				case A3DCMD_REFGAIN:
					fval = atof(operand);
					if (fval < 0.0f) fval = 0.0f;
					A3D.reflect_gain = fval;
					a3dlog.printf("max reflect gain = %.2f", fval);
					break;
				}
			}
		}
		cfgfile.Close();
	}
	else {
		a3dlog.puts("no a3d.inf, using defaults.\n");
	}

	A3DCAPS_HARDWARE hwcaps;
	memset(&hwcaps, 0 , sizeof(hwcaps));
	hwcaps.dwSize = sizeof(hwcaps);
	hr = A3D.a3d->GetHardwareCaps(&hwcaps);
	if (hr != S_OK) {
	// failed???
		a3dlog.printf("failed hardware.\n");
		A3D.a3d->Release();
		A3D.a3d = NULL;
		A3dUninitialize();
		return false;
	}

	if (hwcaps.dwFlags & A3D_DIRECT_PATH_A3D) {
		bool occlusions = A3D.a3d->IsFeatureAvailable(A3D_OCCLUSIONS) == TRUE;
		bool reflections = A3D.a3d->IsFeatureAvailable(A3D_1ST_REFLECTIONS) == TRUE;

		if ((!occlusions && A3D.do_occlusions)) {
			a3dlog.printf("failed hardware occlusion test.\n");
			A3D.a3d->Release();
			A3D.a3d = NULL;
			A3dUninitialize();
			return false;
		}
		if (!reflections && A3D.do_reflections) {
			a3dlog.printf("failed hardware reflections test.\n");
			A3D.do_reflections = false;
		}
	}
	else {
		a3dlog.printf("aureal vortex support failed..\n");
		A3D.a3d->Release();
		A3D.a3d = NULL;
		A3dUninitialize();
		return false;
	}

	hr = A3D.a3d->SetCooperativeLevel(hwnd, A3D_CL_NORMAL);
	if (FAILED(hr)) {
		a3dlog.printf("failed to obtain access to device (%x).\n", hr);
		A3D.a3d->Release();
		A3D.a3d = NULL;
		A3dUninitialize();
		return false;
	}

// set number of fallback a2d sources, say to 20ish?
	hr = A3D.a3d->SetNumFallbackSources(A3D_FALLBACK_SOURCES);
	if (FAILED(hr)) {
		a3dlog.printf("failed to set fallback source count (%x).\n", hr);
	}

// we'll tend to lean towards priority since our code will more likely determine audibility.
	hr = A3D.a3d->SetRMPriorityBias(A3D_PRIORITY_BIAS);
	if (FAILED(hr)) {
		a3dlog.printf("failed to set priority bias (%x).\n", hr);
		A3D.a3d->Release();
		A3D.a3d = NULL;
		A3dUninitialize();
		return false;
	}

// we use left handed coordinate system!
	hr = A3D.a3d->SetCoordinateSystem(A3D_LEFT_HANDED_CS);
	if (hr != S_OK) {
		a3dlog.printf("couldn't set left handed coord system (%x).\n", hr);
		A3D.a3d->Release();
		A3D.a3d = NULL;
		A3dUninitialize();
		return false;
	}

	hr = A3D.a3d->SetOutputGain(A3D.global_gain);
	if (hr != S_OK) {
		a3dlog.printf("couldn't set gain (%x).\n", hr);
	}

	hr = A3D.a3d->SetMaxReflectionDelayTime(A3D.reflect_time);
	if (hr != S_OK) {
		a3dlog.printf("couldn't set reflect time (%x).\n", hr);
	}


	A3D.geom_mode_flags = 0;
	A3D_buffers_allocated = 0;
	A3D.a3d->Clear();

	a3dlog.update();

	return true;
}


// create or destroy geometry object
bool A3D_CreateGeometryInterface()
{
// Get geometry interface object.  We'll need this.
	if (!A3D.do_occlusions) {
		A3D.geom = NULL;
		return false;
	}

	HRESULT hr = A3D.a3d->QueryInterface(IID_IA3dGeom, (void **)&A3D.geom);
	if (hr != S_OK) {
		a3dlog.printf("failed to query geometry interface (%x).\n", hr);
		A3D.geom = NULL;
		return false;
	}

	A3D.geom->Enable(A3D_OCCLUSIONS);

	if (A3D.do_reflections) {
		A3D.geom->Enable(A3D_1ST_REFLECTIONS);
		A3D.geom->SetReflectionDelayScale(A3D.reflect_scale);
		A3D.geom->SetReflectionGainScale(A3D.reflect_gain);
		A3D.geom->SetReflectionUpdateInterval(A3D.reflection_update_interval);
	}

//	obtain a new list.
	hr = A3D.geom->NewList(&A3D.list);
	if (hr != S_OK) {
		a3dlog.printf("failed to create a list interface (%x).\n", hr);
		A3D.geom->Release();
		A3D.geom = NULL;
		return false;
	}

	A3D.geom_mode_flags = A3DPOLY_RENDERMODE_OCCLUSIONS;
	A3D.a3d->Clear();

	return true;
}


void A3D_DestroyGeometryInterface()
{
	if (A3D.list) {
		A3D.list->Release();
		A3D.list = NULL;
	}
	if (A3D.geom) {
		A3D.geom->Release();
		A3D.geom = NULL;
	}
}


// destroy object													
void A3D_Destroy()
{
	A3D_DestroyGeometryInterface();
	if (A3D.geom) A3D.geom->Release();
	if (A3D.listener) A3D.listener->Release();
	if (A3D.a3d) {
		A3D.a3d->Release();
		A3D.listener = NULL;
		A3D.a3d = NULL;
		A3dUninitialize();
		a3dlog.puts("shutdown\n\n");
		a3dlog.update();
	}
}


void A3D_FinalShutdown()
{
	a3dlog.puts("final shutdown\n");
	a3dlog.end();
}


// sets rolloff for all source objects
void A3D_SetRolloffFactor(float scalar)
{
	HRESULT hr;

	hr = A3D.a3d->SetDistanceModelScale(scalar);
	if (hr != S_OK) {
		a3dlog.printf("set distance model scalar failed (%x)\n", hr);
	}
}


// sets doppler for all source objects (from 0.0f to ???) (1.0f is default, 0.0f is none)
void A3D_SetDopplerFactor(float scalar)
{
	HRESULT hr = A3D.a3d->SetDopplerScale(scalar);
	if (hr != S_OK) {
		a3dlog.printf("set doppler scalar failed (%x)\n", hr);
	}
}


// sets units of measurement
void A3D_SetUnitsPerMeter(float units)
{
	HRESULT hr = A3D.a3d->SetUnitsPerMeter(units);
	if (hr != S_OK) {
		a3dlog.printf("couldn't set distance units (%x).\n", hr);
	}
}


//	create listener
bool A3D_CreateListener()
{
/* query for the listener */
	int hr = A3D.a3d->QueryInterface(IID_IA3dListener, (void **)&A3D.listener);
   if (hr != S_OK) {
		a3dlog.printf("create listener failed (%x)!\n", hr);
		return false;
	}
	a3dlog.puts("listener created.\n");
	return true;
}


// create source
void *A3D_CreateSSource(int size, int bps, bool stereo, bool streaming, ushort freq, bool is3d)
{
/* make a new source. at this stage the source is empty - no wave data */
	IA3dSource *a3dsrc=NULL;
	DWORD flags=0;

	if (is3d) {
		flags = A3DSOURCE_TYPEDEFAULT;
	}
	else {
		flags = A3DSOURCE_INITIAL_RENDERMODE_NATIVE;
	}
	if (streaming) {
		flags |= (A3DSOURCE_TYPESTREAMED);
//		a3dlog.printf("new streamed source 3d:%c\n", is3d ? 'y' : 'n');
	}
	else {
//		a3dlog.printf("new source type:%s\n", is3d ? "default[3d]" : "native[2d]");
	}

	int hr = A3D.a3d->NewSource( flags,	&a3dsrc);
   if (hr != S_OK) {
		a3dlog.printf("failed to create for source (%x)\n", hr);
		return NULL;
	}

/* set wave format */
	WAVEFORMATEX fmt;
					  
	fmt.cbSize = sizeof(fmt);
	fmt.wFormatTag = WAVE_FORMAT_PCM;
	fmt.nChannels = (stereo)?2:1;
	fmt.wBitsPerSample = bps;
	fmt.nSamplesPerSec = ((DWORD)freq);
	fmt.nBlockAlign = fmt.nChannels*(fmt.wBitsPerSample/8);
	fmt.nAvgBytesPerSec = ((DWORD)fmt.nSamplesPerSec)*((DWORD)fmt.nBlockAlign);

	hr = a3dsrc->SetWaveFormat(&fmt);
	if (hr != S_OK) {
		a3dlog.printf("failed to set format for source (%x)\n", hr);
		a3dsrc->Release();
		return NULL;
	}

	hr = a3dsrc->AllocateWaveData(size);
	if (hr != S_OK) {
		a3dlog.printf("failed to allocate wavedata for source [%d.%d.%c.%c.%c.%08x] (%x).\n", 
			size,bps,stereo ? 'S' : 'M', streaming ? 'Y' : 'N', is3d ? 'Y' : 'N', hr);
		a3dlog.update();
		a3dsrc->Release();
		return NULL;
	}

//	if (!CHECK_FLAG(flags, A3DSOURCE_TYPEUNMANAGED)) {
//		a3dsrc->SetPriority(0.5f);
//	}

	A3D_buffers_allocated++;

	return (void *)a3dsrc;
}


// duplicates source
void *A3D_DuplicateSource(void *src)
{
	IA3dSource *a3dsrc = (IA3dSource *)src;
	IA3dSource *new_src;
	int hr = A3D.a3d->DuplicateSource(a3dsrc, &new_src);
	if (hr != S_OK) {
		a3dlog.printf("duplicate source failed (%x)\n", hr);
		return NULL;
	}

	A3D_buffers_allocated++;

	return (void *)new_src;
}


// frees source
void A3D_FreeSSource(void *obj)
{
	if (obj) {
		IA3dSource *a3dsrc = (IA3dSource *)obj;
		a3dsrc->FreeWaveData();
		a3dsrc->Release();
		A3D_buffers_allocated--;
	}
}										  


// loads data into source
bool A3D_LoadSample(void *obj, void *src_data, int src_length)
{
	int hr=0;

	if (obj) {
		void *srcptr1=0, *srcptr2=0;
		DWORD len1, len2;
//		IA3dSource *a3dsrc = (IA3dSource *)obj;
//		hr = a3dsrc->LoadWaveData(src_data, (DWORD)src_length);	
//		if (SUCCEEDED(hr)) {
//			return true;
//		}
		if (A3D_Lock(obj, 0, src_length, &srcptr1, &len1, &srcptr2, &len2)) {
			memcpy(srcptr1, src_data, len1);
			if(srcptr2) {
				memcpy(srcptr2, ((char *)src_data) + len1, len2);
			}
			A3D_Unlock(obj, srcptr1, len1, srcptr2, len2);
			return true;
		}
	}

	a3dlog.printf("lock on load source failed (%x)\n", hr);

	return false;
}


// plays a sound
void A3D_Play(void *obj, bool looping)
{
	IA3dSource *a3dsrc = (IA3dSource *)obj;
	HRESULT hr;

	if (!a3dsrc) return;

	hr = a3dsrc->Play(looping ? A3D_LOOPED : A3D_SINGLE);
	if (hr == A3DERROR_NO_WAVE_DATA) {
		a3dlog.printf("play failed because no wave data associated with source!\n");
	}
}


// returns current position in buffer of playback
void A3D_GetCurrentPosition(void *obj,uint *playp)
{
	IA3dSource *a3dsrc = (IA3dSource *)obj;
	HRESULT hr;
	DWORD pos=0;

	if (!a3dsrc) return;

	hr = a3dsrc->GetWavePosition(&pos);
	if (hr != S_OK) {
		*playp = (uint)(-1);
		a3dlog.printf("get current position failed (%x)\n", hr);
	}
	else {
		*playp = (uint)pos;
	}
}

// sets current position in buffer
void A3D_SetCurrentPosition(void *obj, uint pos)
{
	IA3dSource *a3dsrc = (IA3dSource *)obj;
	HRESULT hr;

	if (!a3dsrc) return;

	hr = a3dsrc->SetWavePosition(pos);
	if (hr != S_OK) {
		a3dlog.printf("set wave position failed (%x)\n", hr);
	}
}


// stops a sound
void A3D_Stop(void *obj)
{
	IA3dSource *a3dsrc = (IA3dSource *)obj;
	HRESULT hr;
	DWORD pos=0;

	if (!a3dsrc) return;

	hr = a3dsrc->Stop();
}


// priority is from 0 to 1.0
void A3D_SetSourcePriority(void *obj, float priority)
{
	IA3dSource *a3dsrc = (IA3dSource *)obj;
	HRESULT hr;

	if (!a3dsrc) return;

	hr = a3dsrc->SetPriority((A3DVAL)priority);
}


// sets properties for source object
void A3D_SetSourceVolume(void *obj, float volume)
{
	IA3dSource *a3dsrc = (IA3dSource *)obj;
	HRESULT hr;

	if (!a3dsrc) return;

	hr = a3dsrc->SetGain((A3DVAL)volume);
}


float A3D_GetSourceVolume(void *obj)
{
	IA3dSource *a3dsrc = (IA3dSource *)obj;
	HRESULT hr;
	float gain;
	if (!a3dsrc) return 0.0f;

	hr = a3dsrc->GetGain(&gain);
	if (hr != S_OK) {
		a3dlog.printf("get gain failed (%x)\n",hr);
		return 0.0f;
	}

	return gain;
}


void A3D_SetSourceVelocity(void *obj, float x, float y, float z)
{
	IA3dSource *a3dsrc = (IA3dSource *)obj;
	HRESULT hr;

	if (!a3dsrc) return;

	hr = a3dsrc->SetVelocity3f(x,y,z);
	if (hr != S_OK) {
		a3dlog.printf("set source velocity failed (%x)\n", hr);
	}
}


void A3D_GetSourceVelocity(void *obj, vector *vel)
{
	IA3dSource *a3dsrc = (IA3dSource *)obj;
	HRESULT hr;

	if (!a3dsrc) return;

	hr = a3dsrc->GetVelocity3f(&vel->x,&vel->y,&vel->z);
	if (hr != S_OK) {
		a3dlog.printf("get source velocity failed (%x)\n", hr);
		vel->x = vel->y = vel->z = 0.0f;
	}
}


void A3D_SetSourceCone(void *obj, float x, float y, float z)
{
	IA3dSource *a3dsrc = (IA3dSource *)obj;
	HRESULT hr;

	if (!a3dsrc) return;

	hr = a3dsrc->SetCone(x,y,z);
}


void A3D_SetSourcePosition(void *obj, float x, float y, float z)
{
	IA3dSource *a3dsrc = (IA3dSource *)obj;
	HRESULT hr;

	if (!a3dsrc) return;
	
	hr = a3dsrc->SetPosition3f(x,y,z);
	if (hr != S_OK) {
		a3dlog.printf("set source position failed (%x)\n", hr);
	}
}


bool A3D_SetSourceWaveEvent(void *obj, DWORD offset, HANDLE hEvent)
{
	IA3dSource *a3dsrc = (IA3dSource *)obj;
	HRESULT hr;

	if (!a3dsrc) return false;
	
	hr = a3dsrc->SetWaveEvent(offset, hEvent);;
	if (hr != S_OK) {
		a3dlog.printf("set source waveevent failed (%x)\n", hr);
		return false;
	}

	return true;
}


bool A3D_ClearSourceWaveEvents(void *obj)
{
	IA3dSource *a3dsrc = (IA3dSource *)obj;
	HRESULT hr;

	if (!a3dsrc) return false;
	
	hr = a3dsrc->ClearWaveEvents();
	if (hr != S_OK) {
		a3dlog.printf("clear source waveevent failed (%x)\n", hr);
		return false;
	}

	return true;
}


void A3D_GetSourcePosition(void *obj, vector *pos)
{
	IA3dSource *a3dsrc = (IA3dSource *)obj;
	HRESULT hr;

	if (!a3dsrc) return;
	
	hr = a3dsrc->GetPosition3f(&pos->x,&pos->y,&pos->z);
	if (hr != S_OK) {
		a3dlog.printf("get source position failed (%x)\n", hr);
	}
}


void A3D_SetSourceOrientation(void *obj, const vector *fvec, const vector *uvec)
{
	IA3dSource *a3dsrc = (IA3dSource *)obj;
	HRESULT hr;

	if (!a3dsrc) return;

	hr = a3dsrc->SetOrientation6f(fvec->x, fvec->y, fvec->z, uvec->x, uvec->y, uvec->z);
	if (hr != S_OK) {
		a3dlog.printf("set source orientation failed (%x)\n", hr);
	}
}


void A3D_GetSourceOrientation(void *obj, matrix *orient)
{
	IA3dSource *a3dsrc = (IA3dSource *)obj;
	vector fvec, uvec;
	HRESULT hr;

	if (!a3dsrc) return;

	hr = a3dsrc->GetOrientation6f(&fvec.x, &fvec.y, &fvec.z, &uvec.x, &uvec.y, &uvec.z);
	if (hr == S_OK) {
		vm_VectorToMatrix(orient, &fvec, &uvec, NULL);
	}
	else {
		vm_MakeIdentity(orient);
		a3dlog.printf("get source orientation failed (%x)\n", hr);
	}
}


void A3D_SetMinMaxDistance(void *obj, float min, float max, bool mute_max)
{
	IA3dSource *a3dsrc = (IA3dSource *)obj;
	HRESULT hr;

	if (!a3dsrc) return;

	hr = a3dsrc->SetMinMaxDistance(min, max, mute_max ? A3D_MUTE : A3D_AUDIBLE);
	if (hr != S_OK) {
		a3dlog.printf("set minmaxdist failed (%x)\n", hr);
	}
}


void A3D_SetSourcePan(void *obj, float lpan, float rpan)
{
	IA3dSource *a3dsrc = (IA3dSource *)obj;
	HRESULT hr;
	A3DVAL pan[2] = { lpan, rpan };

	if (!a3dsrc) return;

	hr = a3dsrc->SetPanValues(2, pan); 
	if (hr != S_OK) {
		a3dlog.printf("set pan failed (%x)\n", hr);
	}
}


void A3D_GetSourcePan(void *obj, float *lpan, float *rpan)
{
	IA3dSource *a3dsrc = (IA3dSource *)obj;
	HRESULT hr;
	A3DVAL pan[2];

	if (!a3dsrc) return;

	hr = a3dsrc->GetPanValues(2, pan); 
	if (hr != S_OK) {
		a3dlog.printf("set pan failed (%x)\n", hr);
		*lpan = 0.0f;
		*rpan = 0.0f;
	}
	else {
		*lpan = pan[0];
		*rpan = pan[1];
	}
}


// sets properties for listener
void A3D_SetListenerVelocity(float x, float y, float z)
{
	HRESULT hr;

	if (!A3D.listener) return;

	hr = A3D.listener->SetVelocity3f(x,y,z);
	if (hr != S_OK) {
		a3dlog.printf("set listener velocity failed (%x)\n", hr);
	}
}


void A3D_SetListenerOrient(vector *fvec, vector *uvec)
{
	HRESULT hr;

	if (!A3D.listener) return;

	hr = A3D.listener->SetOrientation6f(fvec->x,fvec->y,fvec->z, uvec->x, uvec->y, uvec->z);
	if (hr != S_OK) {
		a3dlog.printf("set listener orient failed (%x)\n", hr);
	}
}


void A3D_SetListenerPosition(float x, float y, float z)
{
	HRESULT hr;

	if (!A3D.listener) return;

	hr = A3D.listener->SetPosition3f(x,y,z);
	if (hr != S_OK) {
		a3dlog.printf("set listener position failed (%x)\n", hr);
	}
}


// call at start of sound frame
void A3D_StartFrame()
{
	if (!A3D.a3d) return;

//	mprintf_at((1,5,20,"A3D:%2d", A3D_buffers_allocated));

// maybe we shouldn't clear here. flush should do the trick.
//	A3D.a3d->Clear();
}


void A3D_EndFrame()
{
	if (!A3D.a3d) return;

	A3D.geom_mode_flags = A3D_OCCLUSIONS;
	A3D.a3d->Flush();
	a3dlog.update();
}


void A3D_Flush()
{
	if (!A3D.a3d) return;
	A3D.a3d->Flush();
}


// returns status of source buffer
int A3D_GetSourceStatus(void *obj)
{
	IA3dSource *a3dsrc = (IA3dSource *)obj;
	HRESULT hr;
	DWORD status=0;
	int retval = 0;

	if (!a3dsrc) return 0;

	hr = a3dsrc->GetStatus(&status);
	if (SUCCEEDED(hr)) {
		if (status & A3DSTATUS_PLAYING) {
			retval |= A3D_STATUS_PLAYING;
		}
		if (status & A3DSTATUS_LOOPING) {
			retval |= A3D_STATUS_PLAYING;
		}
		if (status & A3DSTATUS_WAITING_FOR_FLUSH) {
			retval |= A3D_STATUS_PLAYING;
		}
//		if (status & A3DSTATUS_BUFFERLOST) {
//				mprintf((0, "A3D:buffer lost!\n"));
//			}
//			if (status & A3DSTATUS_LOOPING) {
//				mprintf((0, "A3D:looping!\n"));
//			}
//			if (status & A3DSTATUS_WAITING_FOR_FLUSH) {
//				mprintf((0, "A3D:flush wait!\n"));
//			}
	}

	return retval;
}


bool A3D_Lock(void *obj, DWORD dwWriteCursor, DWORD dwWriteBytes, void **ptr1, LPDWORD plen1, void **ptr2, LPDWORD plen2)
{
	IA3dSource *a3dsrc = (IA3dSource *)obj;
	HRESULT hr;

	if (!a3dsrc) return false;


	hr = a3dsrc->Lock(dwWriteCursor, dwWriteBytes,ptr1,plen1,ptr2,plen2,0);

	if (hr != S_OK) {
		return false;
	}

	return true;
}


bool A3D_Unlock(void *obj, void *ptr1, DWORD len1, void *ptr2, DWORD len2)
{
	IA3dSource *a3dsrc = (IA3dSource *)obj;
	HRESULT hr;

	if (!a3dsrc) return false;

	hr = a3dsrc->Unlock(ptr1,len1,ptr2,len2);
	if (hr != S_OK) {
		return false;
	}

	return true;
}


// sets up current global environment reverb
bool A3D_SetEnvironmentalReverb(float volume, float damping, float decay)
{
// geometry interface is on?  then lets emulate this function by setting reflection scalar.
	if (!A3D.geom) {
		return false;
	}

// here, decay = seconds.   since a hangar environment takes 10.0f seconds to decay, this is the most obvious
// a padded cell is 0.1 seconds, to the delay scale is minimal.
	float time = decay*4.0f/3.0f;
	A3D.a3d->SetMaxReflectionDelayTime(time);
//	A3D.geom->SetReflectionDelayScale(0.5f+decay*2.0f);

// here the volumes passed into our function seem a bit small.  we don't really want to diminish gain volumes,
// so we'll scale up of gain a bit.  at most it'll be around 1.5 to 2.0.
//	A3D.geom->SetReflectionGainScale(volume*3.0f/2.0f);

	return true;
}


// list = list id.  0 is only valid.
void A3D_GeomListStart(int list)
{
	if (!A3D.list) return;

	A3D.a3d->Clear();

// free old list, start new list.
	if (A3D.list) {
		A3D.list->Release();
		A3D.list = NULL;

		HRESULT hr = A3D.geom->NewList(&A3D.list);
		if (hr != S_OK) {
			a3dlog.printf("failed to create a list interface (%x).\n", hr);
			return;
		}
	}
	
	A3D.list->Begin();

// this will be used by geometry engine if on.
	if (A3D.geom) {
		A3D.geom->LoadIdentity();
		A3D.geom->SetRenderMode(A3D.geom_mode_flags);
	}
}


void A3D_GeomListEnd(int list)
{
	if (!A3D.list) return;

	A3D.list->End();
}


void A3D_GeomListExec(int list)
{
	if (!A3D.list) return;

	A3D.list->Call();
}


void A3D_GeomBindMaterial (void *obj)
{
	if (A3D.geom) {
		if (!obj) {
			if (A3D.geom_mode_flags & A3DPOLY_RENDERMODE_1ST_REFLECTIONS) {
				A3D.geom_mode_flags &= (~A3DPOLY_RENDERMODE_1ST_REFLECTIONS);
				A3D.geom->SetRenderMode(A3D.geom_mode_flags);
			}
		}
		else {
			if (A3D.do_reflections) {
				IA3dMaterial *matp = (IA3dMaterial *)obj;

				if (!(A3D.geom_mode_flags & A3DPOLY_RENDERMODE_1ST_REFLECTIONS)) {
					A3D.geom_mode_flags |= A3DPOLY_RENDERMODE_1ST_REFLECTIONS;
					A3D.geom->SetRenderMode(A3D.geom_mode_flags);
				}
				A3D.geom->BindMaterial(matp);
			}
		}
	}
}


// add primatives to geometry engine
void A3D_GeomAddTriangle(unsigned tag, vector **verts)
{
	HRESULT hr;

	if (!A3D.geom) return;

//	A3D.geom->PushMatrix();
		A3D.geom->Begin(A3D_TRIANGLES);
			A3D.geom->Tag(tag);
			hr = A3D.geom->Vertex3f(verts[0]->x, verts[0]->y, verts[0]->z);
			hr = A3D.geom->Vertex3f(verts[1]->x, verts[1]->y, verts[1]->z);
			hr = A3D.geom->Vertex3f(verts[2]->x, verts[2]->y, verts[2]->z);
		A3D.geom->End();
//	A3D.geom->PopMatrix();
}


void A3D_GeomAddQuad(unsigned tag, vector **verts)
{
	HRESULT hr;

	if (!A3D.geom) return;

//	A3D.geom->PushMatrix();
		A3D.geom->Begin(A3D_QUADS);
			A3D.geom->Tag(tag);
			hr = A3D.geom->Vertex3f(verts[0]->x, verts[0]->y, verts[0]->z);
			hr = A3D.geom->Vertex3f(verts[1]->x, verts[1]->y, verts[1]->z);
			hr = A3D.geom->Vertex3f(verts[2]->x, verts[2]->y, verts[2]->z);
			hr = A3D.geom->Vertex3f(verts[3]->x, verts[3]->y, verts[3]->z);
		A3D.geom->End();
//	A3D.geom->PopMatrix();
}


void A3D_GeomAddListener(vector *pos, matrix *orient)
{
	A3DMATRIX a3dorient;

	if (!A3D.geom) return;

	A3D.geom->PushMatrix();

		/* translate to position of listener (happens to be 0,0,0 here) */
		A3D.geom->Translate3f(pos->x, pos->y, pos->z);

		/* rotate by 'heading' degrees about the Y (up) axis */
		a3dorient[0] = orient->rvec.x;
		a3dorient[1] = orient->rvec.y;
		a3dorient[2] = orient->rvec.z;
		a3dorient[3] = 0;
		a3dorient[4] = orient->uvec.x;
		a3dorient[5] = orient->uvec.y;
		a3dorient[6] = orient->uvec.z;
		a3dorient[7] = 0;
		a3dorient[8] = orient->fvec.x;
		a3dorient[9] = orient->fvec.y;
		a3dorient[10] = orient->fvec.z;
		a3dorient[11] = 0;
		a3dorient[12] = 0;
		a3dorient[13] = 0;
		a3dorient[14] = 0;
		a3dorient[15] = 1;
		A3D.geom->MultMatrix(a3dorient);

		/* make these transformations apply to the listener */
		A3D.geom->BindListener();

	A3D.geom->PopMatrix();
}


void A3D_GeomAddSource(void *obj, vector *pos, matrix *orient)
{
	IA3dSource *a3dsrc = (IA3dSource *)obj;
	A3DMATRIX a3dorient;

	if (!A3D.geom) return;

	A3D.geom->PushMatrix();

		/* translate to position of listener (happens to be 0,0,0 here) */
		A3D.geom->Translate3f(pos->x, pos->y, pos->z);

		/* rotate by 'heading' degrees about the Y (up) axis */
		a3dorient[0] = orient->rvec.x;
		a3dorient[1] = orient->rvec.y;
		a3dorient[2] = orient->rvec.z;
		a3dorient[3] = 0;
		a3dorient[4] = orient->uvec.x;
		a3dorient[5] = orient->uvec.y;
		a3dorient[6] = orient->uvec.z;
		a3dorient[7] = 0;
		a3dorient[8] = orient->fvec.x;
		a3dorient[9] = orient->fvec.y;
		a3dorient[10] = orient->fvec.z;
		a3dorient[11] = 0;
		a3dorient[12] = 0;
		a3dorient[13] = 0;
		a3dorient[14] = 0;
		a3dorient[15] = 1;
		A3D.geom->MultMatrix(a3dorient);

		/* make these transformations apply to the listener */
		A3D.geom->BindSource(a3dsrc);

	A3D.geom->PopMatrix();
}


// used to create aureal 2.0 materials.
void *A3D_CreateMaterial(float xmit_gain, float xmit_highfreq, float reflect_gain, float reflect_highfreq)
{
	IA3dMaterial *matp;
	if (!A3D.geom) return NULL;

	A3D.geom->NewMaterial(&matp);

	matp->SetTransmittance(xmit_gain, xmit_highfreq);
	matp->SetReflectance(reflect_gain, reflect_highfreq);

	return (void *)matp;
}


void A3D_DestroyMaterial(void *obj)
{
	IA3dMaterial *matp = (IA3dMaterial *)obj;
	matp->Release();
}



void A3D_Clear()									// clears geometry info
{
	if (!A3D.a3d) return;
	A3D.a3d->Clear();
}

#endif
