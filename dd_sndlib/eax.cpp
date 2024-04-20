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
*/

/*
 * $Source: $
 * $Revision: 10 $
 * $Author: Samir $
 * $Date: 9/27/99 5:38p $
 *
 *
 *
 * $Log: /DescentIII/Main/dd_sndlib/eax.cpp $
 *
 * 10    9/27/99 5:38p Samir
 * EAX 2.0->1.0 compatibility checkin.
 *
 * 9     8/24/99 3:42p Samir
 * load EAX dynamically
 *
 * 8     8/24/99 1:47p Samir
 * updated header file.
 *
 * 7     8/23/99 5:29p Samir
 * incremental EAX 2.0 checkin
 *
 * 6     1/11/99 5:54p Samir
 * made environment a hangar.
 *
 * 5     1/11/99 5:52p Samir
 * updated EAX support.
 *
 * 4     1/08/99 6:31p Samir
 * added reverb
 *
 * 3     12/23/98 11:50a Samir
 *
 * 2     12/23/98 11:48a Samir
 * basic functionality.
 *
 * 1     12/21/98 7:06p Samir
 * Creative Labs EAX
 *
 */

#include "auddev.h"

#include <objbase.h>
#include <initguid.h>

#include "eax.h"
#include "eax2.h"
#include "pserror.h"

// DATA
#define EAX_ENVIRONMENTS_AVAILABLE (1 << 0)
#define VOICE_MANAGMENT_AVAILABLE (1 << 1)

struct {
  HINSTANCE m_dll;
  LPDIRECTSOUND m_lpds;
  LPDIRECTSOUNDBUFFER m_lpdsb;
  LPKSPROPERTYSET m_lpksps;
  DWORD m_dwSoundProperties;
  VmMode m_vmode;
  EAX_REVERBPROPERTIES m_preset;
} EAX = {NULL, NULL, 0, 0, 0};

const EAX_REVERBPROPERTIES EAX_Environments[EAX_ENVIRONMENT_COUNT] = {
    {EAX_PRESET_GENERIC},    {EAX_PRESET_PADDEDCELL},    {EAX_PRESET_ROOM},       {EAX_PRESET_BATHROOM},
    {EAX_PRESET_LIVINGROOM}, {EAX_PRESET_STONEROOM},     {EAX_PRESET_AUDITORIUM}, {EAX_PRESET_CONCERTHALL},
    {EAX_PRESET_CAVE},       {EAX_PRESET_ARENA},         {EAX_PRESET_HANGAR},     {EAX_PRESET_CARPETEDHALLWAY},
    {EAX_PRESET_HALLWAY},    {EAX_PRESET_STONECORRIDOR}, {EAX_PRESET_ALLEY},      {EAX_PRESET_FOREST},
    {EAX_PRESET_CITY},       {EAX_PRESET_MOUNTAINS},     {EAX_PRESET_QUARRY},     {EAX_PRESET_PLAIN},
    {EAX_PRESET_PARKINGLOT}, {EAX_PRESET_SEWERPIPE},     {EAX_PRESET_UNDERWATER}, {EAX_PRESET_DRUGGED},
    {EAX_PRESET_DIZZY},      {EAX_PRESET_PSYCHOTIC}};

//	FUNCTIONS

HRESULT(FAR PASCAL *DLLEAXDirectSoundCreate)(GUID *, LPDIRECTSOUND *, IUnknown FAR *) = NULL;

//	EAX 1.0 support.
bool EAX_SetEnvironmentPreset(unsigned environment);
bool EAX_SetEnvironmentalReverb(float volume, float damping, float decay);

//	CODE
bool EAX_Create(GUID *pGuid, LPDIRECTSOUND *lpds) {
  HRESULT hr;

  EAX.m_dll = LoadLibrary("eax.dll");
  if (EAX.m_dll) {
    DLLEAXDirectSoundCreate = (LPEAXDIRECTSOUNDCREATE)GetProcAddress(EAX.m_dll, "EAXDirectSoundCreate");
    if (!DLLEAXDirectSoundCreate) {
      Error("EAX DLL doesn't contain latest code for 2.0 functionality.");
      return false;
    }

    mprintf((0, "EAX 2.0 support detected.\n"));
    hr = (*DLLEAXDirectSoundCreate)(pGuid, &EAX.m_lpds, NULL);
  } else {
    mprintf((0, "EAX 1.0 support detected.\n"));
    hr = DirectSoundCreate(pGuid, &EAX.m_lpds, NULL);
  }

  if (hr != DS_OK) {
    *lpds = NULL;
    return false;
  }
  EAX.m_lpdsb = NULL;
  EAX.m_dwSoundProperties = 0;

  *lpds = EAX.m_lpds;

  return true;
}

void EAX_Destroy() {
  if (EAX.m_lpds) {
    if (EAX.m_lpksps != NULL) {
      EAX.m_lpksps->Release();
      EAX.m_lpksps = NULL;
    }
    if (EAX.m_lpdsb) {
      EAX.m_lpdsb->Release();
      EAX.m_lpdsb = NULL;
    }
    EAX.m_lpds->Release();
    EAX.m_lpds = NULL;
  }
  if (EAX.m_dll) {
    FreeLibrary(EAX.m_dll);
    EAX.m_dll = NULL;
    DLLEAXDirectSoundCreate = NULL;
  }
  EAX.m_dwSoundProperties = 0;
}

// returns EAX caps
int EAX_Caps() { return EAX.m_dwSoundProperties; }

// initializes EAX specific interfaces.
bool EAX_SetPrimaryBuffer() {
  HRESULT hr;
  DWORD support;
  bool retval = true;
  WAVEFORMATEX wave;
  DSBUFFERDESC dsbdesc;
  EAX_REVERBPROPERTIES props = {EAX_PRESET_HANGAR};

  ASSERT(EAX.m_lpds);

  memset(&wave, 0, sizeof(WAVEFORMATEX));
  wave.wFormatTag = WAVE_FORMAT_PCM;
  wave.nChannels = 2;
  wave.nSamplesPerSec = 22050;
  wave.wBitsPerSample = 16;
  wave.nBlockAlign = wave.wBitsPerSample / 8 * wave.nChannels;
  wave.nAvgBytesPerSec = wave.nSamplesPerSec * wave.nBlockAlign;

  memset(&dsbdesc, 0, sizeof(DSBUFFERDESC));
  dsbdesc.dwSize = sizeof(DSBUFFERDESC);
  dsbdesc.dwFlags = DSBCAPS_STATIC | DSBCAPS_CTRL3D;
  dsbdesc.dwBufferBytes = DSBSIZE_MIN * 2;
  dsbdesc.lpwfxFormat = &wave;

  if (FAILED(EAX.m_lpds->CreateSoundBuffer(&dsbdesc, &EAX.m_lpdsb, NULL))) {
    return false;
  }

  if (EAX.m_lpksps == NULL) {
    if (FAILED(EAX.m_lpdsb->QueryInterface(IID_IKsPropertySet, (LPVOID *)&EAX.m_lpksps))) {
      mprintf((0, "EAX: Error failed to query property set interface.\n"));
      Int3();
      retval = false;
      goto error_sub;
    }
  }

  //	now, query support depending on EAX 2.0 availability
  if (EAX.m_dll) {
    hr = EAX.m_lpksps->QuerySupport(DSPROPSETID_EAX_ListenerProperties, DSPROPERTY_EAXLISTENER_ENVIRONMENT, &support);
    if (SUCCEEDED(hr)) {
      if ((support & (KSPROPERTY_SUPPORT_SET | KSPROPERTY_SUPPORT_GET)) ==
          (KSPROPERTY_SUPPORT_SET | KSPROPERTY_SUPPORT_GET)) {
        EAX.m_dwSoundProperties |= EAX_ENVIRONMENTS_AVAILABLE;
      }
    }

    hr = EAX.m_lpksps->QuerySupport(DSPROPSETID_EAX_BufferProperties, DSPROPERTY_EAXBUFFER_OBSTRUCTION, &support);
    if (SUCCEEDED(hr)) {
      if ((support & (KSPROPERTY_SUPPORT_SET | KSPROPERTY_SUPPORT_GET)) ==
          (KSPROPERTY_SUPPORT_SET | KSPROPERTY_SUPPORT_GET)) {
        EAX.m_dwSoundProperties |= EAXF_SOURCE_OBSTRUCTION;
      }
    }
  } else {
    hr = EAX.m_lpksps->QuerySupport(DSPROPSETID_EAX_ReverbProperties, DSPROPERTY_EAX_ALL, &support);
    if (SUCCEEDED(hr)) {
      if ((support & KSPROPERTY_SUPPORT_SET | KSPROPERTY_SUPPORT_GET) ==
          (KSPROPERTY_SUPPORT_SET | KSPROPERTY_SUPPORT_GET)) {
        EAX.m_dwSoundProperties |= EAX_ENVIRONMENTS_AVAILABLE;
        /*
           Here the reverb environment is initialized to off.
   */
        EAX_SetEnvironmentPreset(EAX_ENVIRONMENT_HANGAR);
      }
    }
  }

  retval = (EAX.m_dwSoundProperties & EAX_ENVIRONMENTS_AVAILABLE) ? true : false;
  EAX_SetEnvironmentalReverb(props.fVolume, props.fDamping, props.fDecayTime_sec);

error_sub:
  if (retval == false) {
    mprintf((0, "EAX: Error failed to query environmental support.\n"));
    Int3();
    if (EAX.m_lpksps) {
      EAX.m_lpksps->Release();
      EAX.m_lpksps = NULL;
    }
  }

  return retval;
}

/*

  This routine can be used to change the current EAX preset environment. The environment applies
  to all 3D buffers.

*/
// sets up current global environment reverb
bool EAX_SetEnvironmentalReverb(float volume, float damping, float decay) {
  if (EAX.m_dwSoundProperties & EAX_ENVIRONMENTS_AVAILABLE) {
    int i;

    EAX.m_preset.environment = EAX_ENVIRONMENT_GENERIC;
    EAX.m_preset.fVolume = volume;
    EAX.m_preset.fDecayTime_sec = decay;
    EAX.m_preset.fDamping = damping;

    if (EAX.m_dll) {
      for (i = 0; i < EAX_ENVIRONMENT_COUNT; i++) {
        if (volume == EAX_Environments[i].fVolume && damping == EAX_Environments[i].fDamping &&
            decay == EAX_Environments[i].fDecayTime_sec) {
          EAX.m_preset.environment = (ulong)i;
          break;
        }
      }
      if (FAILED(EAX.m_lpksps->Set(DSPROPSETID_EAX_ListenerProperties, DSPROPERTY_EAXLISTENER_ENVIRONMENT, NULL, 0,
                                   &EAX.m_preset.environment, sizeof(ulong)))) {
        return false;
      }
    } else {
      if (FAILED(EAX.m_lpksps->Set(DSPROPSETID_EAX_ReverbProperties, DSPROPERTY_EAX_VOLUME, NULL, 0,
                                   &EAX.m_preset.fVolume, sizeof(float)))) {
        return false;
      }
      if (FAILED(EAX.m_lpksps->Set(DSPROPSETID_EAX_ReverbProperties, DSPROPERTY_EAX_DECAYTIME, NULL, 0,
                                   &EAX.m_preset.fDecayTime_sec, sizeof(float)))) {
        return false;
      }
      if (FAILED(EAX.m_lpksps->Set(DSPROPSETID_EAX_ReverbProperties, DSPROPERTY_EAX_DAMPING, NULL, 0,
                                   &EAX.m_preset.fDamping, sizeof(float)))) {
        return false;
      }
    }

    return true;
  }

  return false;
}

// intializes a sound source for EAX
bool EAX_InitSource(LPDIRECTSOUND3DBUFFER lpBuffer3D, LPKSPROPERTYSET *plpksp) {
  if (!lpBuffer3D)
    return true;

  if (SUCCEEDED(lpBuffer3D->QueryInterface(IID_IKsPropertySet, (void **)plpksp))) {
    return true;
  }

  return false;
}

//	frees an eax sound source
void EAX_FreeSource(LPKSPROPERTYSET lpksp) {
  if (lpksp) {
    lpksp->Release();
  }
}

//	sets source properties
void EAX_SetSourceProperties(LPKSPROPERTYSET lpksp, float obstruction) {
  if (!lpksp)
    return;

  if (EAX.m_dwSoundProperties & EAXF_SOURCE_OBSTRUCTION) {
    LONG lValue = (DWORD)(-10000 * obstruction);
    lpksp->Set(DSPROPSETID_EAX_BufferProperties, DSPROPERTY_EAXBUFFER_OBSTRUCTION, NULL, 0, &lValue, sizeof(LONG));
  }
}

/*

  This routine can be used to change the current EAX preset environment. The environment applies
  to all 3D buffers.

*/
// sets up current global environment reverb
bool EAX_SetEnvironmentPreset(unsigned environment) {
  EAX.m_preset.environment = environment;

  if (FAILED(EAX.m_lpksps->Set(DSPROPSETID_EAX_ReverbProperties, DSPROPERTY_EAX_ENVIRONMENT, NULL, 0,
                               &EAX.m_preset.environment, sizeof(float)))) {
    return false;
  }

  return true;
}
