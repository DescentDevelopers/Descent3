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


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <dirent.h>
#include <time.h>
#include <errno.h>
#include <sys/mount.h>

#include "SDL.h"

#include "loki_utils.h"

#define BAIL_IF_MACRO(x, y, z)                                                                                         \
  if (x)                                                                                                               \
    return (z);
#define BAIL_MACRO(x, z) return (z);

#if MACOSX

#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/storage/IOMedia.h>
#include <IOKit/storage/IOCDMedia.h>
#include <IOKit/storage/IODVDMedia.h>

/*
 * Code based on sample from Apple Developer Connection:
 *  http://developer.apple.com/samplecode/Sample_Code/Devices_and_Hardware/Disks/VolumeToBSDNode/VolumeToBSDNode.c.htm
 */

static int darwinIsWholeMedia(io_service_t service) {
  int retval = 0;
  CFTypeRef wholeMedia;

  if (!IOObjectConformsTo(service, kIOMediaClass))
    return (0);

  wholeMedia = IORegistryEntryCreateCFProperty(service, CFSTR(kIOMediaWholeKey), kCFAllocatorDefault, 0);
  if (wholeMedia == NULL)
    return (0);

  retval = CFBooleanGetValue(wholeMedia);
  CFRelease(wholeMedia);

  return retval;
} /* darwinIsWholeMedia */

static int darwinIsMountedDisc(char *bsdName, mach_port_t masterPort) {
  int retval = 0;
  CFMutableDictionaryRef matchingDict;
  kern_return_t rc;
  io_iterator_t iter;
  io_service_t service;

  if ((matchingDict = IOBSDNameMatching(masterPort, 0, bsdName)) == NULL)
    return (0);

  rc = IOServiceGetMatchingServices(masterPort, matchingDict, &iter);
  if ((rc != KERN_SUCCESS) || (!iter))
    return (0);

  service = IOIteratorNext(iter);
  IOObjectRelease(iter);
  if (!service)
    return (0);

  rc = IORegistryEntryCreateIterator(service, kIOServicePlane,
                                     kIORegistryIterateRecursively | kIORegistryIterateParents, &iter);

  if (!iter)
    return (0);

  if (rc != KERN_SUCCESS) {
    IOObjectRelease(iter);
    return (0);
  } /* if */

  IOObjectRetain(service); /* add an extra object reference... */

  do {
    if (darwinIsWholeMedia(service)) {
      if ((IOObjectConformsTo(service, kIOCDMediaClass)) || (IOObjectConformsTo(service, kIODVDMediaClass))) {
        retval = 1;
      } /* if */
    }   /* if */
    IOObjectRelease(service);
  } while ((service = IOIteratorNext(iter)) && (!retval));

  IOObjectRelease(iter);
  IOObjectRelease(service);

  return (retval);
} /* darwinIsMountedDisc */

int loki_getmountpoint(const char *device, char *mountpoint, int max_size) {
  const char *devPrefix = "/dev/";
  int prefixLen = strlen(devPrefix);
  const char *dev = device + prefixLen;
  mach_port_t masterPort = 0;
  struct statfs *mntbufp;
  int i, mounts;

  if (IOMasterPort(MACH_PORT_NULL, &masterPort) != KERN_SUCCESS)
    return (0);

  mounts = getmntinfo(&mntbufp, MNT_WAIT); /* NOT THREAD SAFE! */
  for (i = 0; i < mounts; i++) {
    char *dev = mntbufp[i].f_mntfromname;
    char *mnt = mntbufp[i].f_mntonname;

    if (strcmp(dev, device) != 0)
      continue;

    dev += prefixLen;
    if (darwinIsMountedDisc(dev, masterPort)) {
      strncpy(mountpoint, mnt, max_size);
      mountpoint[max_size - 1] = '\0';
      return 1;
    } /* if */
  }   /* for */

  return 0;
} /* loki_getmountpoint */

#else // not MACOSX

#include <mntent.h>

int loki_getmountpoint(const char *device, char *mountpoint, int max_size) {
  FILE *mounts = NULL;
  struct mntent *ent = NULL;
  int retval = 0;

  mounts = setmntent("/etc/mtab", "r");
  if (mounts == NULL)
    return 0;

  while ((ent = getmntent(mounts)) != NULL) {
    if (strcmp(ent->mnt_dir, device) == 0) {
      strncpy(mountpoint, ent->mnt_fsname, max_size);
      mountpoint[max_size - 1] = '\0';
      retval = 1;
      break;
    }
  } /* while */

  endmntent(mounts);

  return (retval);
}
#endif


static char *basepath = NULL;
static char *prefpath = NULL;

const char *loki_getdatapath(void) { return basepath; }

const char *loki_getprefpath(void) { return prefpath; }

void loki_initialize(int argc, char **argv, char *desc) {
  basepath = SDL_GetBasePath();
  if (basepath == NULL) {
    fprintf(stderr, "ERROR: Couldn't find game directory!\n");
    exit(43);
  }

  prefpath = SDL_GetPrefPath("Outrage Entertainment", "Descent 3");
  if (prefpath == NULL) {
    fprintf(stderr, "ERROR: Couldn't find preference directory!\n");
    exit(43);
  }

  // printf("base path: %s\n", basepath);
  // printf("pref path: %s\n", prefpath);

} /* loki_initialize */

/* end of loki_utils.c ... */
