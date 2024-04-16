/*
 * joystick header
 *
 * $NoKeywords: $
 */
#include "joystick.h"
#include "pserror.h"
#include "pstypes.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
typedef struct tJoystickRecord {
  bool valid;    // is this a valid device.
  bool remote;   // is this device on another computer
  tJoystick joy; // joystick
  union {
    int joyid;
    // SOCKET joysock;
  } id;          // information on accessing the device
  tJoyInfo caps; // controller capabilities
  tJoyPos pos;   // current position
} tJoystickRecord;
//	---------------------------------------------------------------------------
//	globals
static tJoystickRecord Joysticks[MAX_JOYSTICKS];
int Joy_initialized = 0; // is our joystick system initialized?
void joy_Close();
bool joy_InitRemoteStick(tJoystick joy, char *server_adr, tJoystickRecord *stick);
void joy_GetState(tJoystick stick, tJoyPos *pos);
void joy_GetRemoteState(tJoystick stick, tJoyPos *pos);
int joyGetNumDevs(void);
//	closes a stick
//		closes connection with controller.
void joy_CloseStick(tJoystick joy);
//	---------------------------------------------------------------------------
//	functions
//	joystick system initialization
bool joy_Init(bool remote) {
  int i;

  //	reinitialize joystick if already initialized.
  if (Joy_initialized) {
    joy_Close();
  } else {
    atexit(joy_Close);
  }

  //	check if this OS supports joysticks
  if (!joyGetNumDevs() && !remote) {
    Joy_initialized = 0;
    return false;
  }

  //	initialize joystick list
  for (i = 0; i < MAX_JOYSTICKS; i++)
    Joysticks[i].valid = false;

  Joy_initialized = 1;

  return true;
}
void joy_Close() {
  int i;
  for (i = 0; i < MAX_JOYSTICKS; i++)
    if (Joysticks[i].valid)
      joy_CloseStick((i == 0) ? JOYSTICK_1 : (i == 1) ? JOYSTICK_2 : JOYSTICK_1);
}
//	initializes a joystick
//		if server_adr is valid, a link is opened to another machine with a controller.
bool joy_InitStick(tJoystick joy, char *server_adr) {
  ASSERT(Joy_initialized);
  //	close down already open joystick.
  if (Joysticks[(int)joy].valid)
    joy_CloseStick(joy);
  //	okay, now if this is a remote joystick, open it
  if (server_adr) {
    return joy_InitRemoteStick(joy, server_adr, &Joysticks[(int)joy]);
  } else if (joyGetNumDevs() == 0) {
    return false;
  } else {
    // OPEN JOYSTICK HERE!
    // Setup struct
    return true;
  }

  return false;
}
// this function takes a server address and tries to initialize a stick that's on a machine
// running the remote control server.
bool joy_InitRemoteStick(tJoystick joy, char *server_adr, tJoystickRecord *stick) {
  /*
  sockaddr_in servaddr;
  int adr[4];

  ASSERT(server_adr);

  Joysticks[(int)joy].id.joysock = socket(AF_INET, SOCK_STREAM, 0);
  if (Joysticks[(int)joy].id.joysock == INVALID_SOCKET)
    return false;

  sscanf(server_adr, "%d.%d.%d.%d", &adr[0], &adr[1], &adr[2], &adr[3]);
  mprintf((0, "Connecting to remote control on %d.%d.%d.%d...\n", adr[0],adr[1],adr[2],adr[3]));

  servaddr.sin_family = AF_INET;
  servaddr.sin_port = JOY_PORT;
  servaddr.sin_addr.S_un.S_un_b.s_b1 = (ubyte)adr[0];
  servaddr.sin_addr.S_un.S_un_b.s_b2 = (ubyte)adr[1];
  servaddr.sin_addr.S_un.S_un_b.s_b3 = (ubyte)adr[2];
  servaddr.sin_addr.S_un.S_un_b.s_b4 = (ubyte)adr[3];

  if (connect(Joysticks[(int)joy].id.joysock, (struct sockaddr *)&servaddr, sizeof(servaddr)) == SOCKET_ERROR) {
    shutdown(Joysticks[(int)joy].id.joysock, 1);
    closesocket(Joysticks[(int)joy].id.joysock);
    Joysticks[(int)joy].valid = false;
    return false;
  }
  else {
    tJoyPacket packet;
    LINGER linger;
    int res;

    linger.l_onoff = 1;
    linger.l_linger = 2;
    setsockopt(Joysticks[(int)joy].id.joysock, SOL_SOCKET, SO_LINGER, (char *)&linger, sizeof(linger));

    Joysticks[(int)joy].valid = true;
    Joysticks[(int)joy].remote = true;
    Joysticks[(int)joy].joy = joy;

    //	now wait to receive acknowledgement packet (joystick info will be stored here in a tJoyInfo format) "INFO"
    res = recv(Joysticks[(int)joy].id.joysock, (char *)&packet, sizeof(tJoyInfo) + strlen(JOY_INFO),0);

    if ( res == SOCKET_ERROR || !res) {
      shutdown(Joysticks[(int)joy].id.joysock, 1);
      closesocket(Joysticks[(int)joy].id.joysock);
      Joysticks[(int)joy].valid = false;
      return false;
    }
    mprintf((0, "RECV %c%c%c%c \n", packet.coda[0], packet.coda[1], packet.coda[2], packet.coda[3]));
    if (memcmp(packet.coda, JOY_INFO, strlen(JOY_INFO)) != 0) {
      mprintf((0, "Received non information packet from control server.\n"));
      shutdown(Joysticks[(int)joy].id.joysock, 1);
      closesocket(Joysticks[(int)joy].id.joysock);
      Joysticks[(int)joy].valid = false;
      return false;
    }
    memcpy(&Joysticks[(int)joy].caps, packet.buf, sizeof(tJoyInfo));

    //	start polling. send "POLL"
    memcpy(packet.coda, JOY_POLL, strlen(JOY_POLL));
    send(Joysticks[(int)joy].id.joysock, (char *)&packet, strlen(JOY_POLL), 0);

    // start packet read thread.
    //@@		Thread_running[(int)joy] = false;
    //@@		unsigned thread_id;
    //@@		unsigned long thread_handle = _beginthreadex(NULL, 0, joy_ReadRemotePacket, &Joysticks[(int)joy],
  1, &thread_id);
    //@@		if (thread_handle == 0) {
    //@@			Error("Remote joystick thread creation failed.");
    //@@		}
    //@@
    //@@		if (SetThreadPriority((HANDLE)thread_handle, THREAD_PRIORITY_NORMAL) == FALSE)
    //@@			Error("Remote joystick  thread prioritization failed.");
  }
  */
  return true;
}
//	closes a stick
//		closes connection with controller.
void joy_CloseStick(tJoystick joy) {
  ASSERT(Joysticks[(int)joy].valid);
  if (Joysticks[(int)joy].remote) {
    //@@		u_long arg = 1;
    //@@		ioctlsocket(Joysticks[(int)joy].id.joysock, FIONBIO, &arg);
    //@@		Thread_running[(int)joy] = false;
    //@@		Sleep(200);
    // shutdown(Joysticks[(int)joy].id.joysock, 1);
    // closesocket(Joysticks[(int)joy].id.joysock);
  }
  // CLOSE joystick here
  Joysticks[(int)joy].valid = false;
}
//	returns true if joystick valid
bool joy_IsValid(tJoystick joy) {
  if (Joysticks[(int)joy].valid)
    return true;
  else
    return false;
}
//	retreive information about joystick.
void joy_GetJoyInfo(tJoystick joy, tJoyInfo *info) {
  ASSERT(Joy_initialized);
  if (Joysticks[(int)joy].valid) {
    memcpy(info, &Joysticks[(int)joy].caps, sizeof(tJoyInfo));
  } else
    Int3(); // This should never happen.
}
//	retreive uncalibrated position of joystick
void joy_GetRawPos(tJoystick joy, tJoyPos *pos) {
  joy_GetPos(joy, pos);
  pos->x = (pos->x + 128) * JOYAXIS_RANGE;
  pos->y = (pos->y + 128) * JOYAXIS_RANGE;
  pos->z = (pos->z + 128) * JOYAXIS_RANGE;
  pos->r = (pos->r + 128) * JOYAXIS_RANGE;
  pos->u = (pos->u + 128) * JOYAXIS_RANGE;
  pos->v = (pos->v + 128) * JOYAXIS_RANGE;
}
//	returns the state of a stick, remote or otherwise
void joy_GetPos(tJoystick stick, tJoyPos *pos) {
  //  ASSERT(Joy_initialized);

  pos->x = 0;
  pos->y = 0;
  pos->z = 0;
  pos->r = 0;
  pos->u = 0;
  pos->v = 0;
  pos->pov[0] = 0;
  pos->pov[1] = 0;
  pos->pov[2] = 0;
  pos->pov[3] = 0;
  pos->buttons = 0;
  pos->btn = 0;

  if (!Joysticks[(int)stick].valid)
    return;

  //	retrieve joystick info from the net, or locally.
  if (Joysticks[(int)stick].remote) {
    joy_GetRemoteState(stick, pos);
  } else {
    // READ JOYSTICK HERE!!!!!!!!!!!
  }
}
//	returns the position of a remote stick.
void joy_GetRemoteState(tJoystick stick, tJoyPos *pos) {
  /*
  ASSERT(Joysticks[(int)stick].remote);

  tJoyPacket packet;
  //	get next POSI packet
  recv(Joysticks[(int)stick].id.joysock, (char *)packet.coda, strlen(JOY_POS),0);
  if (memcmp(packet.coda, JOY_POS, strlen(JOY_POS)) == 0) {
    recv(Joysticks[(int)stick].id.joysock, (char *)packet.buf, sizeof(tJoyPos),0);
    memcpy(&Joysticks[(int)stick].pos, packet.buf, sizeof(tJoyPos));
  }
  pos->x = (int)((Joysticks[(int)stick].pos.x<<8)/(Joysticks[(int)stick].caps.maxx - Joysticks[(int)stick].caps.minx)) -
  128; pos->y = (int)((Joysticks[(int)stick].pos.y<<8)/(Joysticks[(int)stick].caps.maxy -
  Joysticks[(int)stick].caps.miny)) - 128; if (Joysticks[(int)stick].caps.axes_mask & JOYFLAG_ZVALID) pos->z =
  (int)((Joysticks[(int)stick].pos.z<<8)/(Joysticks[(int)stick].caps.maxz - Joysticks[(int)stick].caps.minz)) - 128;
  else
    pos->z = 0;
  if (Joysticks[(int)stick].caps.axes_mask & JOYFLAG_RVALID)
    pos->r = (int)((Joysticks[(int)stick].pos.r<<8)/(Joysticks[(int)stick].caps.maxr - Joysticks[(int)stick].caps.minr))
  - 128; else pos->r = 0; if (Joysticks[(int)stick].caps.axes_mask & JOYFLAG_UVALID) pos->u =
  (int)((Joysticks[(int)stick].pos.u<<8)/(Joysticks[(int)stick].caps.maxu - Joysticks[(int)stick].caps.minu)) - 128;
  else
    pos->u = 0;
  if (Joysticks[(int)stick].caps.axes_mask & JOYFLAG_VVALID)
    pos->v = (int)((Joysticks[(int)stick].pos.v<<8)/(Joysticks[(int)stick].caps.maxv - Joysticks[(int)stick].caps.minv))
  - 128; else pos->v = 0; if (Joysticks[(int)stick].caps.axes_mask & JOYFLAG_POVVALID) { pos->pov =
  (Joysticks[(int)stick].pos.pov == JOY_POVBACKWARD) ? JOYPOV_DOWN : (Joysticks[(int)stick].pos.pov == JOY_POVFORWARD) ?
  JOYPOV_UP : (Joysticks[(int)stick].pos.pov == JOY_POVLEFT) ? JOYPOV_LEFT : (Joysticks[(int)stick].pos.pov ==
  JOY_POVRIGHT) ? JOYPOV_RIGHT : JOYPOV_CENTER;
  }
  else
    pos->pov = JOYPOV_CENTER;
  pos->buttons = (unsigned)Joysticks[(int)stick].pos.buttons;
  pos->btn = (unsigned)Joysticks[(int)stick].pos.btn;

  //	request poll for next packet POLL
  memcpy(packet.coda, JOY_POLL, strlen(JOY_POLL));
  send(Joysticks[(int)stick].id.joysock, (char *)&packet, strlen(JOY_POLL), 0);
  */
}
void ddio_InternalJoySuspend(void) {}
void ddio_InternalJoyResume(void) {}
int joyGetNumDevs(void) { return 0; }
void ddio_InternalJoyFrame(void) {}
