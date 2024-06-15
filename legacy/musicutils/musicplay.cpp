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

#include "musicplay.h"
#include "win\directx\dsound.h"
#include "ds3dlib.h"
#include "streamaudio.h"
#include "application.h"

static win_llsSystem Sound_system;
int paged_in_count = 0;

bool MusicPlayInit(oeApplication *app) {
  int retval = Sound_system.InitSoundLib(SOUND_MIXER_DS_8, app, 16);
  if (!retval)
    return false;

  AudioStream::InitSystem(&Sound_system);

  return true;
}

void MusicPlayClose() {
  AudioStream::Shutdown();
  Sound_system.DestroySoundLib();
}

void MusicPlayFrame() {
  Sound_system.SoundStartFrame();

  AudioStream::Frame();

  Sound_system.SoundEndFrame();
}
