
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

char *__PHYSFS_platformCurrentDir(void) {
  int allocSize = 0;
  char *retval = NULL;
  char *ptr;

  do {
    allocSize += 100;
    ptr = (char *)realloc(retval, allocSize);
    if (ptr == NULL) {
      if (retval != NULL)
        free(retval);
      BAIL_MACRO(ERR_OUT_OF_MEMORY, NULL);
    } /* if */

    retval = ptr;
    ptr = getcwd(retval, allocSize);
  } while (ptr == NULL && errno == ERANGE);

  if (ptr == NULL && errno) {
    /*
     * getcwd() failed for some reason, for example current
     * directory not existing.
     */
    if (retval != NULL)
      free(retval);
    BAIL_MACRO(ERR_NO_SUCH_FILE, NULL);
  } /* if */

  return (retval);
} /* __PHYSFS_platformCurrentDir */

/*
 * See where program (bin) resides in the $PATH specified by (envr).
 *  returns a copy of the first element in envr that contains it, or NULL
 *  if it doesn't exist or there were other problems. PHYSFS_SetError() is
 *  called if we have a problem.
 *
 * (envr) will be scribbled over, and you are expected to free() the
 *  return value when you're done with it.
 */
static char *findBinaryInPath(const char *bin, char *envr) {
  size_t alloc_size = 0;
  char *exe = NULL;
  char *start = envr;
  char *ptr;

  BAIL_IF_MACRO(bin == NULL, ERR_INVALID_ARGUMENT, NULL);
  BAIL_IF_MACRO(envr == NULL, ERR_INVALID_ARGUMENT, NULL);

  do {
    size_t size;
    ptr = strchr(start, ':'); /* find next $PATH separator. */
    if (ptr)
      *ptr = '\0';

    size = strlen(start) + strlen(bin) + 2;
    if (size > alloc_size) {
      char *x = (char *)realloc(exe, size);
      if (x == NULL) {
        if (exe != NULL)
          free(exe);
        BAIL_MACRO(ERR_OUT_OF_MEMORY, NULL);
      } /* if */

      alloc_size = size;
      exe = x;
    } /* if */

    /* build full binary path... */
    strcpy(exe, start);
    if ((exe[0] == '\0') || (exe[strlen(exe) - 1] != '/'))
      strcat(exe, "/");
    strcat(exe, bin);

    if (access(exe, X_OK) == 0) /* Exists as executable? We're done. */
    {
      strcpy(exe, start); /* i'm lazy. piss off. */
      return (exe);
    } /* if */

    start = ptr + 1; /* start points to beginning of next element. */
  } while (ptr != NULL);

  if (exe != NULL)
    free(exe);

  return (NULL); /* doesn't exist in path. */
} /* findBinaryInPath */

char *__PHYSFS_platformCopyEnvironmentVariable(const char *varname) {
  const char *envr = getenv(varname);
  char *retval = NULL;

  if (envr != NULL) {
    retval = (char *)malloc(strlen(envr) + 1);
    if (retval != NULL)
      strcpy(retval, envr);
  } /* if */

  return (retval);
} /* __PHYSFS_platformCopyEnvironmentVariable */

static char *getUserNameByUID(void) {
  uid_t uid = getuid();
  struct passwd *pw;
  char *retval = NULL;

  pw = getpwuid(uid);
  if ((pw != NULL) && (pw->pw_name != NULL)) {
    retval = (char *)malloc(strlen(pw->pw_name) + 1);
    if (retval != NULL)
      strcpy(retval, pw->pw_name);
  } /* if */

  return (retval);
} /* getUserNameByUID */

static char *getUserDirByUID(void) {
  uid_t uid = getuid();
  struct passwd *pw;
  char *retval = NULL;

  pw = getpwuid(uid);
  if ((pw != NULL) && (pw->pw_dir != NULL)) {
    retval = (char *)malloc(strlen(pw->pw_dir) + 1);
    if (retval != NULL)
      strcpy(retval, pw->pw_dir);
  } /* if */

  return (retval);
} /* getUserDirByUID */

char *__PHYSFS_platformGetUserName(void) {
  char *retval = getUserNameByUID();
  if (retval == NULL)
    retval = __PHYSFS_platformCopyEnvironmentVariable("USER");
  return (retval);
} /* __PHYSFS_platformGetUserName */

char *__PHYSFS_platformGetUserDir(void) {
  char *retval = __PHYSFS_platformCopyEnvironmentVariable("HOME");
  if (retval == NULL)
    retval = getUserDirByUID();
  return (retval);
} /* __PHYSFS_platformGetUserDir */

static int appendDirSep(char **dir) {
  const char *dirsep = "/";
  char *ptr;

  if (strcmp((*dir + strlen(*dir)) - strlen(dirsep), dirsep) == 0)
    return (1);

  ptr = (char *)realloc(*dir, strlen(*dir) + strlen(dirsep) + 1);
  if (!ptr) {
    free(*dir);
    return (0);
  } /* if */

  strcat(ptr, dirsep);
  *dir = ptr;
  return (1);
} /* appendDirSep */

static char *unixCalcBaseDir(const char *argv0) {
  /* If there isn't a path on argv0, then look through the $PATH for it. */

  char *retval;
  char *envr;

  if (strchr(argv0, '/') != NULL) /* default behaviour can handle this. */
    return (NULL);

  envr = __PHYSFS_platformCopyEnvironmentVariable("PATH");
  BAIL_IF_MACRO(!envr, NULL, NULL);
  retval = findBinaryInPath(argv0, envr);
  free(envr);
  return (retval);
}

static char *calcBaseDir(const char *argv0) {
  const char *dirsep = "/";
  char *retval;
  char *ptr;

  /*
   * See if the platform driver wants to handle this for us...
   */
  retval = unixCalcBaseDir(argv0);
  if (retval != NULL)
    return (retval);

  /*
   * Determine if there's a path on argv0. If there is, that's the base dir.
   */
  ptr = strstr(argv0, dirsep);
  if (ptr != NULL) {
    char *p = ptr;
    size_t size;
    while (p != NULL) {
      ptr = p;
      p = strstr(p + 1, dirsep);
    } /* while */

    size = (size_t)(ptr - argv0);
    retval = (char *)malloc(size + 1);
    if (retval == NULL)
      return NULL;
    memcpy(retval, argv0, size);
    retval[size] = '\0';
    return (retval);
  } /* if */

  /*
   * Last ditch effort: it's the current working directory. (*shrug*)
   */
  retval = __PHYSFS_platformCurrentDir();
  if (retval != NULL) {
    return (retval);
  }

  /*
   * Ok, current directory doesn't exist, use the root directory.
   * Not a good alternative, but it only happens if the current
   * directory was deleted from under the program.
   */
  retval = (char *)malloc(strlen(dirsep) + 1);
  strcpy(retval, dirsep);
  return (retval);
}

static char *basepath = NULL;
static char *prefpath = NULL;

const char *loki_getdatapath(void) { return basepath; }

const char *loki_getprefpath(void) { return prefpath; }

void loki_initialize(int argc, char **argv, char *desc) {
  char resolved_path[MAXPATHLEN];
  char *ptr = __PHYSFS_platformGetUserDir();
  if (!ptr) {
    fprintf(stderr, "ERROR: environment variable HOME not set?!");
    exit(42);
  }

#if MACOSX
  const char *extra = "Library/Application Support/Descent 3";
#else
  const char *extra = ".loki/descent3";
#endif

  prefpath = (char *)malloc(strlen(ptr) + strlen(extra) + 3);
  strcpy(prefpath, ptr);
  if (prefpath[strlen(prefpath) - 1] != '/')
    strcat(prefpath, "/");

  free(ptr);

  strcat(prefpath, extra);
  ptr = prefpath;
  while ((ptr = strchr(ptr, '/')) != NULL) {
    *ptr = '\0';
    mkdir(prefpath, S_IRWXU);
    *ptr = '/';
    ptr++;
  }
  mkdir(prefpath, S_IRWXU);

  basepath = calcBaseDir(argv[0]);
  if (basepath == NULL) {
    fprintf(stderr, "ERROR: Couldn't find game directory!\n");
    exit(43);
  }

  if (realpath(prefpath, resolved_path)) {
    ptr = (char *)realloc(prefpath, strlen(resolved_path) + 1);
    if (ptr) {
      strcpy(ptr, resolved_path);
      prefpath = ptr;
    }
  }

  if (realpath(basepath, resolved_path)) {
    ptr = (char *)realloc(basepath, strlen(resolved_path) + 1);
    if (ptr) {
      strcpy(ptr, resolved_path);
      basepath = ptr;
    }
  }

// Strip out Application Bundle...
#if MACOSX
  {
    // !!! FIXME
  }
#endif

  // printf("base path: %s\n", basepath);
  // printf("pref path: %s\n", prefpath);

  printf("%s\n", desc);
} /* loki_initialize */

/* end of loki_utils.c ... */
