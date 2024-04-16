/*
 * $Logfile: /DescentIII/Main/ddio_lnx/lnxcdrom.cpp $
 * $Revision: 1.12 $
 * $Date: 2004/12/05 04:00:20 $
 * $Author: ryan $
 *
 * Linux cdrom routines (borrowed some code from SDL)
 *
 * $Log: lnxcdrom.cpp,v $
 * Revision 1.12  2004/12/05 04:00:20  ryan
 * MacOS X patches.
 *
 * Revision 1.11  2004/03/21 17:11:39  kevinb
 * Fixes so linux will compile again. Tested with gcc-2.96
 *
 * Revision 1.10  2004/02/25 00:04:06  ryan
 * Removed loki_utils dependency and ported to MacOS X (runs, but incomplete).
 *
 * Revision 1.9  2001/01/11 11:41:29  icculus
 * Mounting fix.
 *
 * Revision 1.8  2000/07/07 01:45:32  icculus
 * Reinit functionality.
 *
 * Revision 1.7  2000/07/06 22:15:49  icculus
 * Changed to work with GetMultiCDPath()'s new behaviour.
 *
 * Revision 1.6  2000/06/29 08:57:34  icculus
 * Took out the Mount() and Unmount() code, 'cause it was retarded. Do it
 * outside the program.
 *
 * Revision 1.5  2000/06/29 06:41:23  icculus
 * mad commits.
 *
 * Revision 1.4  2000/06/24 01:15:15  icculus
 * patched to compile.
 *
 * Revision 1.3  2000/04/27 11:16:34  icculus
 * Took out SDL_Init() call. It's handled in $/Main/lnxmain.cpp, now.
 *
 * Revision 1.2  2000/04/24 03:18:00  icculus
 * SDLified and loki_utilified this code. Lots of paradigm fixes, too. It's
 * VERY different now.
 *
 * Revision 1.1.1.1  2000/04/18 00:00:33  icculus
 * initial checkin
 *
 *
 * 5     12/01/99 3:30p Jeff
 * ddio_GetCDDrive now returns NULL instead of ""
 *
 * 4     10/05/99 12:54p Jeff
 * removed system() call
 *
 * 3     10/02/99 6:50p Jeff
 * pretty solid cd-rom support now.  Tested and seems to work.  Need to
 * figure out a better way of handlng /proc/mounts
 *
 * 2     10/02/99 3:00p Jeff
 * slightly rewritten due to some early design flaws (namely multiple
 * cd-rom drives, and the drive would be mounted when you try to switch
 * CDs).  This should be better (though only 1 cd-rom drive is supported
 * now).  May not compile yet.
 *
 * 1     10/01/99 7:44p Jeff
 * initial checkin
 *
 * $NoKeywords: $
 */

#include <sys/mount.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <SDL.h>
#include "mem.h"
#include "mono.h"
#include "ddio.h"
#include "debug.h"
#include "pserror.h"
#include "loki_utils.h"

#define VD_LSN 16         // first logical sector of volume descriptor table
#define CDROM_LSECSZ 2048 // initial logical sector size of a CDROM
#define CDROM_LBS CDROM_LSECSZ

class system_cdroms {
public:
  system_cdroms();
  ~system_cdroms();

  bool Mount(void);
  bool UnMount(void);

  // Given a device specified by the user, it will make it the
  // default device for the system.
  bool SetDefaultDevice(char *device);

  // resets the device mountpoint.  If the mountpoint was created, it will
  // unmount the drive and remove the mountpoint directory
  void ResetMountpoint(void);

  // This function will check the default device to see if it
  // has a CD in it with the given volume.  If it doesn't
  // then it will unmount the device (if it is mounted), in preparation
  // for a CD switch.  If it does have the correct CD in it, then
  // it wil be mounted (if it isn't already) and the mount position
  // will be given in mountpoint.  If it can't mount it will return false;
  bool PrepareForVolume(char *volume, char *mountpoint);

  // Returns the device name of the default drive
  const char *GetDeviceName(void);

  // Returns the volume name of the default drive
  const char *GetVolumeName(void);

  // Returns true if a CD is in the default device drive
  bool IsCdInDrive(void);

private:
  bool IsCdInDrive(int drive);
  const char *CDName(int drive);
  const char *GetVolume(int drive);
  int GetNumCDRoms(void);

  // checks to see if the given cd-rom device is mounted
  // if so, it returns true and fills in mountpoint with the
  // directory it is mounted in
  bool IsDeviceMounted(const char *device, char *mountpoint, int max_size);
  // Check a drive to see if it is a CD-ROM
  int CheckDrive(char *drive, struct stat *stbuf);
  // Add a CD-ROM drive to our list of valid drives
  void AddDrive(char *drive, struct stat *stbuf);
  void Shutdown(void);

  // Locks the CD-ROM
  void LockDevice(void);
  // UnLocks the CD-ROM
  void UnLockDevice(void);
  // This function will go through our list of devices
  // when it comes across the first CD-ROM (which can only be
  // detected when a CD is in the drive), it will make it the default
  // device.
public:
  void FindDefaultDevice(void);
  void reinit(void);

private:
  //	Queries the settings for the default drive
  //	First it checks to see if the drive is already mounted
  //	If it is, it's mountpoint is saved and we continue
  //	If it isn't mounted, the it will create a mountpoint for the device
  //	and mount it there.
  void QueryDefaultDevice(void);

  bool m_CreatedMountPoint;
  bool m_DefaultMounted;

public: // !!! hack.  Make an accessor method.
  char m_MountedDir[_MAX_PATH];

private:
  char m_MountDirectory[_MAX_PATH];
  int m_NumCDRoms;
  int m_DeviceToUse;
};

system_cdroms CDROM_Devices;

// Given a device/drive, this marks it as the default CD-ROM drive
void ddio_MarkDefaultCDDrive(char *drive) {
  bool ret = CDROM_Devices.SetDefaultDevice(drive);
  /*
         if(!ret)
         {
                 Error("Unable to set %s as the default CD-ROM drive\n",drive);
         }
  */
}

void ddio_ReinitCDSystem(void) { CDROM_Devices.reinit(); }

// Give a volume label to look for, and if it's found returns a path
// If it isn't found, return NULL
char *ddio_GetCDDrive(char *vol) {
  static char drivepath[_MAX_PATH];

  /*
          if(*vol=='\0')
                  return NULL;

          bool ret = CDROM_Devices.PrepareForVolume(vol,drivepath);
          if(!ret)
          {
                  // nope...not yet
                  return NULL;
          }
  */

  CDROM_Devices.FindDefaultDevice(); // returns if already set.

  if (CDROM_Devices.Mount() == false)
    return (NULL);

  strcpy(drivepath, CDROM_Devices.m_MountedDir);
  return drivepath;
}

void cdrom_system_shutdown(void) { CDROM_Devices.ResetMountpoint(); }

void cdrom_set_system_shutdown(void) {
  static bool set = false;
  if (set)
    return;
  set = true;
  atexit(cdrom_system_shutdown);
}

system_cdroms::system_cdroms(void) { reinit(); }

void system_cdroms::reinit(void) {
  SDL_QuitSubSystem(SDL_INIT_CDROM);
  SDL_Init(SDL_INIT_CDROM);
  m_CreatedMountPoint = false;
  m_NumCDRoms = 0;
  m_DeviceToUse = -1;
  m_DefaultMounted = false;

  m_NumCDRoms = GetNumCDRoms();
} // reinit

system_cdroms::~system_cdroms(void) {
  //    SDL_QuitSubSystem(SDL_INIT_CDROM);
}

int system_cdroms::GetNumCDRoms(void) { return (SDL_CDNumDrives()); }

// Returns the device name of the default drive
const char *system_cdroms::GetDeviceName(void) { return CDName(m_DeviceToUse); }

// Returns the volume name of the default drive
const char *system_cdroms::GetVolumeName(void) { return GetVolume(m_DeviceToUse); }

const char *system_cdroms::CDName(int drive) { return (SDL_CDName(drive)); }

const char *system_cdroms::GetVolume(int drive) {
#if 1 // MACOSX
  assert(0);
#else
  static char buffer[256];

  if (drive < 0 || drive >= m_NumCDRoms)
    return NULL;

  struct iso_primary_descriptor *ipd;
  int cdfd, cdromfmt = 0;

  if (!IsCdInDrive(drive))
    return NULL;

  cdfd = open(CDName(drive), (O_RDONLY | O_EXCL | O_NONBLOCK), 0);
  if (cdfd < 0)
    return NULL;

  // locate at the beginning of the descriptor table
  lseek(cdfd, VD_LSN * CDROM_LSECSZ, SEEK_SET);

  ipd = (struct iso_primary_descriptor *)buffer;

  // walk descriptor table
  for (;;) {
    unsigned char type;
    read(cdfd, buffer, sizeof(buffer));

    // make sure it's ISO format
    if (cdromfmt == 0) {
      if (strncmp(ipd->id, ISO_STANDARD_ID, sizeof(ipd->id)) != 0) {
        // not an ISO9660 cdrom
        close(cdfd);
        return NULL;
      } else {
        // ISO9660
        cdromfmt = 1;
      }
    }

    // type of descriptor
    type = (unsigned char)ipd->type[0];

    // terminating volume
    if (type == ISO_VD_END) {
      break;
    }

    // ISO 9660 filestructure
    if (cdromfmt == 1 && type == ISO_VD_PRIMARY && !strncmp(ipd->id, ISO_STANDARD_ID, sizeof(ipd->id))) {
      char *ptr;
      int len = sizeof(ipd->volume_id);
      bcopy(ipd->volume_id, buffer, len);
      buffer[len] = '\0';

      for (ptr = buffer + len - 1; ptr > buffer; ptr--) {
        if (*ptr == ' ') {
          *ptr = 0;
        }
      }

      close(cdfd);
      return buffer;
    }
  }

  close(cdfd);
#endif
  return NULL;
}

// Returns true if a CD is in the default device drive
bool system_cdroms::IsCdInDrive(void) { return IsCdInDrive(m_DeviceToUse); }

bool system_cdroms::IsCdInDrive(int drive) {
  //	int cdfd,is_in=0;
  //	struct cdrom_subchnl info;
  //	struct stat stbuf;
  CDstatus cdStat;
  SDL_CD cdInfo;
  bool retVal = true;

  if (drive < 0 || drive >= m_NumCDRoms)
    retVal = false;
  else {
    cdStat = SDL_CDStatus(&cdInfo);
    if ((cdStat == CD_TRAYEMPTY) || (cdStat == CD_ERROR))
      retVal = false;
  } // else
  return (retVal);
}

static int read_file_string(FILE *fd, char *buffer, int maxsize) {
  int ret;
  int size = 0;

  ret = fgetc(fd);
  while (ret != EOF && size < maxsize) {
    if (ret == '\n') {
      break;
    } else {
      // check for a number
      if (ret == '\\') {
        // we have a number coming
        char val = 0;
        char c[3];
        c[0] = fgetc(fd);
        c[1] = fgetc(fd);
        c[2] = fgetc(fd);
        val = ((c[0] - '0') * 64) + ((c[1] - '0') * 8) + (c[0] - '0');
        *buffer = val;
        buffer++;
      } else {
        *buffer = ret;
        buffer++;
      }
      size++;
    }
    ret = fgetc(fd);
  }

  *buffer = '\0';

  return size;
}

static bool cdrom_CopyFile(char *dst_path, const char *src_path) {
  char *buf;
  int buf_size;
  int dest_desc;
  int source_desc;
  int n_read;
  struct stat sb;
  char *cp;
  int *ip;
  bool return_val = true;
  off_t n_read_total = 0;
  int last_write_made_hole = 0;
  int make_holes = 0;

  source_desc = open(src_path, O_RDONLY);
  if (source_desc < 0) {
    return false;
  }

  /* Create the new regular file with small permissions initially,
  to not create a security hole.  */
  dest_desc = open(dst_path, O_WRONLY | O_CREAT | O_TRUNC, 0666);
  if (dest_desc < 0) {
    return_val = false;
    goto ret2;
  }

  /* Find out the optimal buffer size.  */
  if (fstat(dest_desc, &sb)) {
    return_val = false;
    goto ret;
  }
  buf_size = sb.st_blksize;

  /* Make a buffer with space for a sentinel at the end.  */
  buf = (char *)alloca(buf_size + sizeof(int));

  for (;;) {
    n_read = read(source_desc, buf, buf_size);
    if (n_read < 0) {
      return_val = false;
      goto ret;
    }
    if (n_read == 0)
      break;

    n_read_total += n_read;

    ip = 0;

    if (write(dest_desc, buf, n_read) < 0) {
      return_val = false;
      goto ret;
    }
  }

ret:
  if (close(dest_desc) < 0) {
    return_val = false;
  }
ret2:
  if (close(source_desc) < 0) {
    return_val = false;
  }

  return return_val;
}

static bool checkDevMount(const char *device, const char *mntfname, char *mountpoint) {
  bool retVal = false;
#if 1 // MACOSX
  assert(0);
#else
  bool getOut = false;
  struct mntent *mntEntry;

  FILE *mountFile = setmntent(mntfname, "r");
  if (mountFile == NULL) {
    mprintf((0, "setmntent(\"%s\", \"r\"); failed!  errno == (%d).", mntfname, errno));
    Int3();
    return (false);
  } // if

  do {
    mntEntry = getmntent(mountFile);
    if (mntEntry == NULL)
      getOut = true;
    else {
      if (strcmp(device, mntEntry->mnt_fsname) == 0) // found device?
      {
        strcpy(mountpoint, mntEntry->mnt_dir); //!!! overflows?
        getOut = retVal = true;
      } // if
    }   // else
  } while (getOut == false);

  endmntent(mountFile);
#endif
  return (retVal);
}

bool system_cdroms::IsDeviceMounted(const char *device, char *mountpoint, int max_size) {
#if 1 // MACOSX
  int rc = loki_getmountpoint(device, mountpoint, max_size);
  return (rc > 0);
#else
  return false;
#endif
}

bool system_cdroms::Mount(void) {
  if (m_DeviceToUse == -1)
    return false;

  // make sure the atexit is set so we can properly clear all created tmp dirs
  cdrom_set_system_shutdown();

  // first check to see if it is mounted
  const char *name = CDName(m_DeviceToUse);
  char mountpoint[_MAX_PATH];
  if (IsDeviceMounted(name, mountpoint, sizeof(mountpoint))) {
    strcpy(m_MountedDir, mountpoint);
    m_DefaultMounted = true;
    return true;
  }

  /*  don't do this.
  // Not mounted? Do it ourselves.
      // so we now have our directory, try to mount
      unsigned long int rwflag = MS_MGC_VAL|MS_RDONLY;
      int ret = mount(name,m_MountedDir,"iso9660",rwflag,NULL);
      if(ret!=0)
      {
              switch(errno)
              {
              case EPERM:
                      fprintf(stdout,"Unable to mount %s, you are not superuser\n",name);
                      break;
              case ENODEV:
                      fprintf(stdout,"iso9660 filesystem not supported by kernel\n");
                      break;
              case ENOTBLK:
                      fprintf(stdout,"Device %s not a block device\n",name);
                      break;
              case EBUSY:
                      fprintf(stdout,"Device %s already mounted\n",name);
                      Int3();
                      break;
              case ENAMETOOLONG:
                      fprintf(stdout,"Attempting to mount %s to %s (but name too long)\n",name,m_MountedDir);
                      break;
              case ENOTDIR:
                      fprintf(stdout,"%s not a directory\n",m_MountedDir);
                      break;
              default:
              //case EINVAL:
              //case EFAULT:
              //case ENOMEM:
              //case EMXIO:
              //case EMFILE:
              //case ENOENT:
              //case EACCES:
                      fprintf(stdout,"Unknown error %d trying to mount %s\n",errno,name);
                      break;
              }
      }else
      {
              // we mounted!
              m_DefaultMounted = true;
              LockDevice();//make sure it is locked
              return true;
      }
  */
  return false;
}

bool system_cdroms::UnMount(void) {
  if (m_DeviceToUse == -1)
    return false;
  /*
          // find the drive that is mounted
          char mountpoint[_MAX_PATH];
      const char *name = CDName(m_DeviceToUse);
          if (IsDeviceMounted(name,mountpoint,sizeof (mountpoint)) == false)
      {
                  m_DefaultMounted = false;
                  UnLockDevice();//make sure it is unlocked
                  return true;//drive isn't mounted anywhere
          }

          UnLockDevice();//make sure it is unlocked
          int ret = umount(name);
          if(ret==0)
          {
                  m_DefaultMounted = false;
                  return true;
          }else
          {
                  mprintf((0,"Error %d trying to unmount %s\n",errno,name));
                  //lock it again...something went wrong
                  LockDevice();
                  return false;
          }
  */
  return false;
}

// Given a device specified by the user, it will make it the
// default device for the system.
bool system_cdroms::SetDefaultDevice(char *device) {
  /*
          if(m_DeviceToUse!=-1)
          {
                  Int3();
                  return false;
          }
  */
  // first find which id the device is
  int i, dev_id = -1;

  for (i = 0; i < m_NumCDRoms; i++) {
    if (strcmp(device, CDName(i)) == 0) {
      dev_id = i;
      break;
    }
  }

  if (dev_id == -1) {
    //		printf("CDROM: Default device (%s) not found\n",device);
    return false;
  }

  m_DeviceToUse = dev_id;
  QueryDefaultDevice();
  return true;
}

// This function will check the default device to see if it
// has a CD in it with the given volume.  If it doesn't
// then it will unmount the device (if it is mounted), in preparation
// for a CD switch.  If it does have the correct CD in it, then
// it wil be mounted (if it isn't already) and the mount position
// will be given in mountpoint.  If it can't mount it will return false;
bool system_cdroms::PrepareForVolume(char *volume, char *mountpoint) {
  if (m_DeviceToUse == -1) {
    // uh oh, we don't even know what device to use yet...
    // so, we'll look at everything we have, checking for volumes
    // the first volume successfully returned will be are default
    // device.
    FindDefaultDevice();
    if (m_DeviceToUse == -1) {
      // still nothing
      return false;
    }
  }

  char *vol = (char *)GetVolumeName();
  if (!vol) {
    // no CD in the default device drive
    return false;
  }

  // compare the CD in the drive to what we want
  if (!strcmp(vol, volume)) {
    // this is the CD we want!
    // lets make sure it is mounted
    bool ret = Mount();
    if (ret) {
      strcpy(mountpoint, m_MountedDir);
      return true;
    } else {
      // we couldn't mount!
      mprintf((0, "CDROM: Unable to mount drive though we must have it mounted\n"));
      Int3();
      return false;
    }
  }

  // ok, the CD in there is NOT what we want
  // lets unmount the drive so the user can switch CDs
  bool ret = UnMount();
  if (!ret) {
    mprintf((0, "Unable to unmount drive\n"));
  }

  return false;
}

// This function will go through our list of devices
// when it comes across the first CD-ROM (which can only be
// detected when a CD is in the drive), it will make it the default
// device.
void system_cdroms::FindDefaultDevice(void) {
  if (m_DeviceToUse != -1)
    return;

  // 0 is SDL's default CD-ROM drive id. But use -1 if there are none.
  if (GetNumCDRoms() <= 0) {
    m_DeviceToUse = -1;
    mprintf((0, "CDROM: No drives found to be made default!"));
    return;
  }    // if
  else // there WAS at least one drive...
  {
    m_DeviceToUse = 0; // default device.
    mprintf((0, "CDROM: Making device \"%s\" default CD-ROM drive.", CDName(m_DeviceToUse)));
  } // else

  // we found the device we want to use for our CD-ROM.
  // now we need to query some information about the default drive
  // like if it's currently mounted (if so where, and save that info),
  // and if it's not mounted, then we need to create a mountpoint for it and
  // use that
  QueryDefaultDevice();
}

//	Queries the settings for the default drive
//	First it checks to see if the drive is already mounted
//	If it is, it's mountpoint is saved and we continue
//	If it isn't mounted, the it will create a mountpoint for the device
//	and mount it there.
void system_cdroms::QueryDefaultDevice(void) {
  if (m_DeviceToUse == -1) {
    Int3();
    return;
  }

  // first check to see if it is already mounted
  char mountpoint[_MAX_PATH];
  bool ismounted = false;
  const char *device = CDName(m_DeviceToUse);

  if (IsDeviceMounted(device, mountpoint, sizeof(mountpoint))) {
    mprintf((0, "CDROM: Default device already mounted at %s...\n", mountpoint));

    // save the mountpoint for use
    strcpy(m_MountedDir, mountpoint);
    m_DefaultMounted = true;
    return;
  }

#if 0 // rcg10052000  BAD! BADBADBAD!
        // see if there's a default dir for mounting this thing.
    if (checkDevMount(device, "/etc/fstab", m_MountedDir) == false)
    {
        	// otherwise, we'll make a mountpoint and mount it
            //  remember that the current working directory, unless overridden
            //  by the user, is "$HOME/.loki/", so this is "okay", I guess.
        strcpy(mountpoint, loki_getprefpath());
    	char *dptr = mountpoint + strlen(mountpoint);
        char *sptr = (char *) device;
    	*dptr = 'T'; dptr++;
    	*dptr = 'M'; dptr++;
    	*dptr = 'P'; dptr++;
    	while(*sptr)
    	{
    		if(*sptr!='/')
    		{
    			*dptr = *sptr;
    			dptr++;
    		}
    		sptr++;
    	}
    	*dptr = '0';dptr++;
    	*dptr = '\0';	
	
    	dptr = mountpoint + strlen(mountpoint) - 1;

    	// now keep attempting to make the directory until we do or can't even try
    	bool bail = true;
    	while( ((*dptr)-'0')<=9 )
    	{
    		if(!ddio_DirExists(mountpoint))
    		{
    			if(ddio_CreateDir(mountpoint))
    			{
    				// we did it
    				bail = false;
    				break;
    			}
    		}else
    		{
    			*dptr = (*dptr)++;
    		}
    	}

    	if(bail)
    	{
    		Error("Unable to create a mountpoint for device %s\n", device);
    		exit(1);
    		return;
    	}

    	// save the mountpoint for use
    	mprintf((0,"CDROM: Mountpoint created for device %s at %s\n",device,mountpoint));
    	strcpy(m_MountedDir,mountpoint);
    	m_CreatedMountPoint = true;
    } // if

	Mount();
#endif
}

// resets the device mountpoint.  If the mountpoint was created, it will
// unmount the drive and remove the mountpoint directory
void system_cdroms::ResetMountpoint(void) {
  if (!m_CreatedMountPoint)
    return;

  UnMount();
  ddio_RemoveDir(m_MountedDir);
}

// Locks the CD-ROM
void system_cdroms::LockDevice(void) {
  /*
          if(m_DeviceToUse==-1)
                  return;

          int retval;
          int fileid;
          fileid = open(m_CDList[m_DeviceToUse]->name, (O_RDONLY|O_EXCL|O_NONBLOCK), 0);
          if(fileid>=0)
          {
                  int arg = 1;
                  //retval = ioctl(fileid,CDROM_LOCKDOOR,arg);
                  retval = ioctl(fileid,CDC_LOCK,arg);
                  if(retval<0)
                  {
                          mprintf((0,"CDROM: ioctl error locking drive (%d)\n",retval));
                  }
                  close(fileid);
          }
  */
}

// UnLocks the CD-ROM
void system_cdroms::UnLockDevice(void) {
  /*
          if(m_DeviceToUse==-1)
                  return;

          int retval;
          int fileid;
          fileid = open(m_CDList[m_DeviceToUse]->name, (O_RDONLY|O_EXCL|O_NONBLOCK), 0);
          if(fileid>=0)
          {
                  int arg = 0;

                  //retval = ioctl(fileid,CDROM_LOCKDOOR,arg);
                  retval = ioctl(fileid,CDC_LOCK,arg);
                  if(retval<0)
                  {
                          mprintf((0,"CDROM: ioctl error unlocking drive (%d)\n",retval));
                  }
                  close(fileid);
          }
  */
}
