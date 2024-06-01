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

--- HISTORICAL COMMENTS FOLLOW ---

 * $Logfile: /DescentIII/Main/directplay/directplay.cpp $
 * $Revision: 12 $
 * $Date: 4/14/99 1:12a $
 * $Author: Jeff $
 *
 *	Directplay API
 *
 * $Log: /DescentIII/Main/directplay/directplay.cpp $
 *
 * 12    4/14/99 1:12a Jeff
 * fixed case mismatched #includes
 *
 * 11    2/16/99 12:36a Kevin
 * Fixes for release builds of OEM V3 and KAtmai
 *
 * 10    2/15/99 1:22p Kevin
 * Changes for GameGauge
 *
 * 9     1/21/99 11:15p Jeff
 * pulled out some structs and defines from header files and moved them
 * into seperate header files so that multiplayer dlls don't require major
 * game headers, just those new headers.  Side effect is a shorter build
 * time.  Also cleaned up some header file #includes that weren't needed.
 * This affected polymodel.h, object.h, player.h, vecmat.h, room.h,
 * manage.h and multi.h
 *
 * 8     10/08/98 5:20p Kevin
 * Fixed dependancies and fixed demo config
 *
 * 7     9/02/98 6:54p Kevin
 * Fixed general directplay support up, and got modem-modem working
 *
 * 6     8/24/98 10:55a Kevin
 * new directplay stuff
 *
 * 5     8/19/98 11:49a Kevin
 * Got DirectPlay IPX working, and localized connection DLLs
 *
 * 4     8/14/98 4:54p Kevin
 * More directplay stuff
 *
 * 3     8/13/98 6:36p Kevin
 * checked that  lpDirectPlay4A was non null
 *
 * 2     8/13/98 6:32p Kevin
 * Initial implementation of directplay API
 *
 * 1     8/13/98 6:25p Kevin
 *
 */

#include <windows.h>
#include <winsock.h>
#include <wsipx.h>
#include <ras.h>
#include <objbase.h>

#include "descent.h"
#include "player.h"
#include "appdatabase.h"

#include "pstypes.h"
#include "pserror.h"
#include "mono.h"
#include "networking.h"
#include "ddio.h"
#include "mem.h"
#include "multi.h"

#include "directplay.h"

bool Use_DirectPlay = false;

DPSESSIONDESC2 Directplay_sessions[MAX_DP_GAMES];
int Num_directplay_games = 0;

char Directplay_session_desc[MAX_DP_GAMES][200];
bool Directplay_lobby_launched_game = false;
void *lpAddress = NULL;

// char Directplay_modem_list[MAX_MODEMS][200];
modem_list Modems_found[MAX_MODEMS];
int Num_modems_found = 0;

LPVOID lpConnectionBuffer;
LPDIRECTPLAY4A lpDirectPlay4A = NULL;
LPDIRECTPLAYLOBBY3A lpDirectPlayLobby3A = NULL;
LPDPLCONNECTION lpdplconnection = NULL;

// {915CE160-3125-11d2-BB3D-00104B27BFF0}
static const GUID DPD3_GUID = {0x915ce160, 0x3125, 0x11d2, {0xbb, 0x3d, 0x0, 0x10, 0x4b, 0x27, 0xbf, 0xf0}};

uint32_t Pending_dp_conn[MAX_PENDING_NEW_CONNECTIONS];

struct dpconnections {
  char name[100];
  void *conn;
};

dpconnections dpconns[MAX_DIRECTPLAY_CONNECTIONS];

BOOL FAR PASCAL DirectPlayEnumConnectionsCallback(LPCGUID lpguidSP, LPVOID lpConnection, DWORD dwConnectionSize,
                                                  LPCDPNAME lpName, DWORD dwFlags, LPVOID lpContext) {

  mprintf(0, "Directplay connection found: %s\n", lpName->lpszShortNameA);
  // Do something with the list of connections
  for (int i = 0; i < MAX_DIRECTPLAY_CONNECTIONS; i++) {
    if (dpconns[i].conn == NULL) {
      dpconns[i].conn = mem_malloc(dwConnectionSize);
      memcpy(dpconns[i].conn, lpConnection, dwConnectionSize);
      strcpy(dpconns[i].name, lpName->lpszShortNameA);
      return (TRUE);
    }
  }
  mprintf(0, "No space for Directplay entry.\n");

  return (TRUE);
}

BOOL FAR PASCAL EnumSessionsCallback(LPCDPSESSIONDESC2 lpSessionDesc, LPDWORD lpdwTimeOut, DWORD dwFlags,
                                     LPVOID lpContext) {
  if (!lpSessionDesc)
    return (FALSE);

  if (Num_directplay_games < MAX_DP_GAMES) {
    mprintf(0, "Found Directplay game[%d]: %s\n", Num_directplay_games, lpSessionDesc->lpszSessionNameA);
    memcpy(&Directplay_sessions[Num_directplay_games], lpSessionDesc, sizeof(DPSESSIONDESC2));
    strcpy(Directplay_session_desc[Num_directplay_games], lpSessionDesc->lpszSessionNameA);
    Directplay_sessions[Num_directplay_games].lpszSessionNameA = Directplay_session_desc[Num_directplay_games];
/*
    mprintf(0,"Found Directplay game[%d]: %s\n",
             Num_directplay_games,
             Directplay_sessions[Num_directplay_games].lpszSessionNameA);
*/
    Num_directplay_games++;
  } else
    return FALSE;

  return (TRUE);
}

int dp_SelectDirectPlayConnection(char *name) {
  HRESULT hr;
  void *connection = NULL;
  int i;
  if (!lpDirectPlay4A)
    return 0;
  if (name) {
    if (lpAddress)
      mem_free(lpAddress);
    lpAddress = NULL;
    for (i = 0; i < MAX_DIRECTPLAY_CONNECTIONS; i++) {
      if (stricmp(dpconns[i].name, name) == 0) {
        mprintf(0, "Found DirectPlay connection: %s\n", name);
        connection = dpconns[i].conn;
        break;
      }
    }
    if (!connection) {
      mprintf(0, "Unable to find DirectPlay connection: %s\n", name);
      return 0;
    }
  } else
    connection = lpAddress;

  memset(&dpconns, 0, sizeof(dpconnections) * MAX_DIRECTPLAY_CONNECTIONS);

  hr = lpDirectPlay4A->InitializeConnection(connection, 0);

  for (i = 0; i < MAX_DIRECTPLAY_CONNECTIONS; i++) {
    if (dpconns[i].conn != NULL) {
      mem_free(dpconns[i].conn);
      dpconns[i].conn = NULL;
      dpconns[i].name[0] = NULL;
    }
  }

  if (FAILED(hr)) {
    mprintf(0, "lpDirectPlay4A->InitializeConnection() failed.\n");
    switch (hr) {
    case DPERR_ALREADYINITIALIZED:
      mprintf(0, "Error: DPERR_ALREADYINITIALIZED\n");
      break;
    case DPERR_INVALIDFLAGS:
      mprintf(0, "Error: DPERR_INVALIDFLAGS\n");
      break;
    case DPERR_INVALIDPARAMS:
      mprintf(0, "Error: DPERR_INVALIDPARAMS\n");
      break;
    case DPERR_UNAVAILABLE:
      mprintf(0, "Error: DPERR_UNAVAILABLE\n");
      break;
    default:
      mprintf(0, "Error: Unknown\n");
    }

    return 0;
  } else
    return 1;
}

int dp_InitDirectPlay(char *conn_name, void *parms, int num_elements) {
  DWORD dwAddressSize;
  HRESULT hr;
  char *conn = conn_name;
  for (int i = 0; i < MAX_PENDING_NEW_CONNECTIONS; i++)
    Pending_dp_conn[i] = DPID_UNKNOWN;

  // Delete any previous directplay instance
  if (lpDirectPlay4A)
    lpDirectPlay4A->Release();
  if (SUCCEEDED(CoCreateInstance(CLSID_DirectPlay, NULL, CLSCTX_INPROC_SERVER, IID_IDirectPlay4A,
                                 (LPVOID *)&lpDirectPlay4A))) {
    lpDirectPlay4A->EnumConnections(&DPD3_GUID, DirectPlayEnumConnectionsCallback, 0, 0);
  } else {
    lpDirectPlay4A = NULL;
  }
  if (parms) {
    if (lpDirectPlayLobby3A) {
      lpDirectPlayLobby3A->Release();
      lpDirectPlayLobby3A = NULL;
    }

    hr = CoCreateInstance(CLSID_DirectPlayLobby, NULL, CLSCTX_INPROC_SERVER, IID_IDirectPlayLobby3A,
                          (LPVOID *)&lpDirectPlayLobby3A);
    if (SUCCEEDED(hr)) {
      hr = lpDirectPlayLobby3A->CreateCompoundAddress((LPDPCOMPOUNDADDRESSELEMENT)parms, num_elements, NULL,
                                                      &dwAddressSize);
      if (hr == DPERR_BUFFERTOOSMALL) {
        if (lpAddress)
          mem_free(lpAddress);
        lpAddress = mem_malloc(dwAddressSize);
        hr = lpDirectPlayLobby3A->CreateCompoundAddress((LPDPCOMPOUNDADDRESSELEMENT)parms, num_elements, lpAddress,
                                                        &dwAddressSize);
        if (FAILED(hr)) {
          mprintf(0, "lpDirectPlayLobby3A->CreateCompoundAddress() failed. DirectPlay *NOT* initialized!\n");
          return 0;
        } else {
          mprintf(0, "lpDirectPlayLobby3A->CreateCompoundAddress() succeeded!\n");
        }
      } else {
        mprintf(0, "lpDirectPlayLobby3A->CreateCompoundAddress() buffer size request failed. DirectPlay *NOT* initialized!\n");
      }
    }
  }
  if (dp_SelectDirectPlayConnection(conn)) {
    DP_active = TRUE;
    mprintf(0, "DirectPlay Initialized\n");
    return 1;
  }
  return 0;
}

// Everyone should call this when leaving the game.
void dp_EndGame() {
  mprintf(0, "Ending DirectPlay game.\n");
  if ((!DP_active) || (!lpDirectPlay4A))
    return;
  lpDirectPlay4A->Close();
  lpDirectPlay4A->Release();
  lpDirectPlay4A = NULL;
}

// This function gets called when a game is started, so Direcplay knows the title, and to listen
int dp_StartGame(char *gamename) {
  HRESULT hres;
  DPNAME server_player;
  DPSESSIONDESC2 sessionDesc;

  mprintf(0, "Starting DirectPlay game: %s\n", gamename);

  if ((!DP_active) || (!lpDirectPlay4A)) {
    mprintf(0, "Can't start game because DirectPlay isn't initialized!\n");
    return 0;
  }
  // Host a new session.
  ZeroMemory(&sessionDesc, sizeof(DPSESSIONDESC2));
  sessionDesc.dwSize = sizeof(DPSESSIONDESC2);
  sessionDesc.dwFlags =
      DPSESSION_CLIENTSERVER | DPSESSION_KEEPALIVE | DPSESSION_OPTIMIZELATENCY | DPSESSION_DIRECTPLAYPROTOCOL;
  sessionDesc.guidApplication = DPD3_GUID;
  sessionDesc.dwMaxPlayers = MAX_NET_PLAYERS;
  sessionDesc.dwCurrentPlayers = 1; // The server is a player
  sessionDesc.lpszSessionNameA = gamename;

  ZeroMemory(&server_player, sizeof(DPNAME));
  server_player.dwSize = sizeof(DPNAME);
  server_player.lpszShortNameA = Players[Player_num].callsign;
  server_player.lpszLongNameA = NULL;

  // if(lpAddress)
  //{
  //	hres = lpDirectPlay4A->Open((DPSESSIONDESC2 *)lpAddress, DPOPEN_CREATE | DPOPEN_RETURNSTATUS);
  //}
  // else
  //{
  hres = lpDirectPlay4A->Open(&sessionDesc, DPOPEN_CREATE | DPOPEN_RETURNSTATUS);
  //}

  if (SUCCEEDED(hres)) {
    NetPlayers[Player_num].hPlayerEvent = CreateEvent(NULL,  // no security
                                                      FALSE, // auto reset
                                                      FALSE, // initial event reset
                                                      NULL); // no name
    if (NetPlayers[Player_num].hPlayerEvent == NULL) {
      mprintf(0, "Failed to create directplay notification event!\n");
      dp_EndGame();
      DP_active = false;
      return 0;
    }
    hres = lpDirectPlay4A->CreatePlayer(&NetPlayers[Player_num].dpidPlayer, &server_player,
                                        NetPlayers[Player_num].hPlayerEvent, NULL, 0, DPPLAYER_SERVERPLAYER);
    if (FAILED(hres)) {
      mprintf(0, "Failed to create directplay player in dp_StartGame()\n");
      switch (hres) {
      case DPERR_CANTADDPLAYER:
        mprintf(0, "Unable to create player. Error code: DPERR_CANTADDPLAYER\n");
        break;
      case DPERR_CANTCREATEPLAYER:
        mprintf(0, "Unable to create player. Error code: DPERR_CANTCREATEPLAYER\n");
        break;
      case DPERR_CONNECTIONLOST:
        mprintf(0, "Unable to create player. Error code: DPERR_CONNECTIONLOST\n");
        break;
      case DPERR_INVALIDFLAGS:
        mprintf(0, "Unable to create player. Error code: DPERR_INVALIDFLAGS\n");
        break;
      case DPERR_INVALIDPARAMS:
        mprintf(0, "Unable to create player. Error code: DPERR_INVALIDPARAMS\n");
        break;
      case DPERR_NOCONNECTION:
        mprintf(0, "Unable to create player. Error code: DPERR_NOCONNECTION\n");
        break;
      default:
        mprintf(0, "Unable to create player. Error code: Unknown\n");
        break;
      }
      return 0;
    }
    // Success
    return 1;
  } else {
    switch (hres) {

    case DPERR_ACCESSDENIED:
      mprintf(0, "Failed to create Directplay session (DPERR_ACCESSDENIED)!\n");
      break;
    case DPERR_ALREADYINITIALIZED:
      mprintf(0, "Failed to create Directplay session (DPERR_ALREADYINITIALIZED)!\n");
      break;
    case DPERR_AUTHENTICATIONFAILED:
      mprintf(0, "Failed to create Directplay session (DPERR_AUTHENTICATIONFAILED)!\n");
      break;
    case DPERR_CANNOTCREATESERVER:
      mprintf(0, "Failed to create Directplay session (DPERR_CANNOTCREATESERVER)!\n");
      break;
    case DPERR_CANTLOADCAPI:
      mprintf(0, "Failed to create Directplay session (DPERR_CANTLOADCAPI)!\n");
      break;
    case DPERR_CANTLOADSECURITYPACKAGE:
      mprintf(0, "Failed to create Directplay session (DPERR_CANTLOADSECURITYPACKAGE)!\n");
      break;
    case DPERR_CANTLOADSSPI:
      mprintf(0, "Failed to create Directplay session (DPERR_CANTLOADSSPI)!\n");
      break;
    case DPERR_CONNECTING:
      mprintf(0, "Failed to create Directplay session (DPERR_CONNECTING)!\n");
      break;
    case DPERR_CONNECTIONLOST:
      mprintf(0, "Failed to create Directplay session (DPERR_CONNECTIONLOST)!\n");
      break;
    case DPERR_ENCRYPTIONFAILED:
      mprintf(0, "Failed to create Directplay session (DPERR_ENCRYPTIONFAILED)!\n");
      break;
    case DPERR_ENCRYPTIONNOTSUPPORTED:
      mprintf(0, "Failed to create Directplay session (DPERR_ENCRYPTIONNOTSUPPORTED)!\n");
      break;
    case DPERR_INVALIDFLAGS:
      mprintf(0, "Failed to create Directplay session (DPERR_INVALIDFLAGS)!\n");
      break;
    case DPERR_INVALIDPARAMS:
      mprintf(0, "Failed to create Directplay session (DPERR_INVALIDPARAMS)!\n");
      break;
    case DPERR_INVALIDPASSWORD:
      mprintf(0, "Failed to create Directplay session (DPERR_INVALIDPASSWORD)!\n");
      break;
    case DPERR_LOGONDENIED:
      mprintf(0, "Failed to create Directplay session (DPERR_LOGONDENIED)!\n");
      break;
    case DPERR_NOCONNECTION:
      mprintf(0, "Failed to create Directplay session (DPERR_NOCONNECTION)!\n");
      break;
    case DPERR_NONEWPLAYERS:
      mprintf(0, "Failed to create Directplay session (DPERR_NONEWPLAYERS)!\n");
      break;
    case DPERR_NOSESSIONS:
      mprintf(0, "Failed to create Directplay session (DPERR_NOSESSIONS)!\n");
      break;
    case DPERR_SIGNFAILED:
      mprintf(0, "Failed to create Directplay session (DPERR_SIGNFAILED)!\n");
      break;
    case DPERR_TIMEOUT:
      mprintf(0, "Failed to create Directplay session (DPERR_TIMEOUT)!\n");
      break;
    case DPERR_UNINITIALIZED:
      mprintf(0, "Failed to create Directplay session (DPERR_UNINITIALIZED)!\n");
      break;
    case DPERR_USERCANCEL:
      mprintf(0, "Failed to create Directplay session (DPERR_USERCANCEL)!\n");
      break;
    default:
      mprintf(0, "Failed to create Directplay session (unknown)!\n");
    }
  }
  return 0;
}

int dp_ListDirectPlayGames() {
  DPSESSIONDESC2 sessionDesc;
  HRESULT hr;
  if (!DP_active)
    return DPERR_EXCEPTION;
  if (!lpDirectPlay4A)
    return DPERR_EXCEPTION;

  Num_directplay_games = 0;
  // Search for this kind of session.
  ZeroMemory(&sessionDesc, sizeof(DPSESSIONDESC2));
  sessionDesc.dwSize = sizeof(DPSESSIONDESC2);
  sessionDesc.guidApplication = DPD3_GUID;

  hr = lpDirectPlay4A->EnumSessions(&sessionDesc, 0, EnumSessionsCallback, 0,
                                    DPENUMSESSIONS_AVAILABLE | DPENUMSESSIONS_RETURNSTATUS | DPENUMSESSIONS_ASYNC);
  if (hr == DPERR_CONNECTING) {
    mprintf(0, "Waiting for EnumSessions() to connect.\n");
    return DPERR_CONNECTING;
  }

  if FAILED (hr) {
    mprintf(0, "Unable to EnumSessions() for Directplay.\n");
    switch (hr) {
    case DPERR_CONNECTING:
      mprintf(0, "Error code: DPERR_CONNECTING\n");
      break;
    case DPERR_CONNECTIONLOST:
      mprintf(0, "Error code: DPERR_CONNECTIONLOST\n");
      break;
    case DPERR_EXCEPTION:
      mprintf(0, "Error code: DPERR_EXCEPTION\n");
      break;
    case DPERR_GENERIC:
      mprintf(0, "Error code: DPERR_GENERIC\n");
      break;
    case DPERR_INVALIDOBJECT:
      mprintf(0, "Error code: DPERR_INVALIDOBJECT\n");
      break;
    case DPERR_INVALIDPARAMS:
      mprintf(0, "Error code: DPERR_INVALIDPARAMS\n");
      break;
    case DPERR_NOCONNECTION:
      mprintf(0, "Error code: DPERR_NOCONNECTION\n");
      break;
    case DPERR_UNINITIALIZED:
      mprintf(0, "Error code: DPERR_UNINITIALIZED\n");
      break;
    case DPERR_USERCANCEL:
      mprintf(0, "Error code: DPERR_USERCANCEL\n");
      break;
    default:
      mprintf(0, "Error code: Unknown\n");
      break;
    }
    return DPERR_NOCONNECTION;
  } else
    mprintf(0, "EnumSessions() returned DP_OK\n");

  return DP_OK;
}

// This function will look for incoming messages, and dispatch them accordingly
void dp_DirectPlayDispatch() {
  HRESULT hr;
  uint8_t packet_data[32000];              // MAX_PACKET_SIZE+1];
  DWORD dwMsgBufferSize = 32000; // MAX_PACKET_SIZE;
  DPID idFrom = 0;
  DPID idTo = 0;
  network_address from_addr;
  LPDPMSG_CREATEPLAYERORGROUP new_player_msg;
  // Put messages in the buffers
  if ((!DP_active) || (!lpDirectPlay4A)) {
    mprintf(0, "Can't dispatch DirectPlay message because DirectPlay isn't active!\n");
    return;
  }

  hr = lpDirectPlay4A->Receive(&idFrom, &idTo, DPRECEIVE_ALL, packet_data, &dwMsgBufferSize);
  if (FAILED(hr)) {
    switch (hr) {
    case DPERR_BUFFERTOOSMALL:
      mprintf(0, "Error: lpDirectPlay4A->Receive() returned DPERR_BUFFERTOOSMALL -- needs %d\n", dwMsgBufferSize);
      break;
    case DPERR_GENERIC:
      mprintf(0, "Error: lpDirectPlay4A->Receive() returned DPERR_GENERIC\n");
      break;
    case DPERR_INVALIDOBJECT:
      mprintf(0, "Error: lpDirectPlay4A->Receive() returned DPERR_INVALIDOBJECT\n");
      break;
    case DPERR_INVALIDPARAMS:
      mprintf(0, "Error: lpDirectPlay4A->Receive() returned DPERR_INVALIDPARAMS\n");
      break;
    case DPERR_INVALIDPLAYER:
      mprintf(0, "Error: lpDirectPlay4A->Receive() returned DPERR_INVALIDPLAYER\n");
      break;
    case DPERR_NOMESSAGES:
      // Not really an error....
      // mprintf(0,"Error: lpDirectPlay4A->Receive() returned DPERR_NOMESSAGES\n");
      break;
    default:
      mprintf(0, "Unknown error code from DirectPlay::Receive() -- %x\n", hr);
    }
  } else {
    // Now we have actual data -- do something with it.
    if (idFrom == DPID_SYSMSG) {
      // This is a system message
      LPDPMSG_GENERIC lpMsg = (LPDPMSG_GENERIC)packet_data;
      switch (lpMsg->dwType) {
      case DPSYS_CREATEPLAYERORGROUP:
        // New player joined the game
        {
          mprintf(0, "Got a DPSYS_CREATEPLAYERORGROUP packet!\n");
          new_player_msg = (LPDPMSG_CREATEPLAYERORGROUP)lpMsg;
          // Store this player's info
          for (int i = 0; i < MAX_PENDING_NEW_CONNECTIONS; i++) {
            if (Pending_dp_conn[i] == DPID_UNKNOWN) {
              // new connection slot
              Pending_dp_conn[i] = new_player_msg->dpId;
              break;
            }
          }
        }
        break;
      case DPSYS_DESTROYPLAYERORGROUP:
        // Player left the game
        break;
      case DPSYS_ADDPLAYERTOGROUP:
      case DPSYS_DELETEPLAYERFROMGROUP:
      case DPSYS_SESSIONLOST:
      case DPSYS_HOST:
      case DPSYS_SETPLAYERORGROUPDATA:
      case DPSYS_SETPLAYERORGROUPNAME:
      case DPSYS_SENDCOMPLETE:
        break;
      default:
        Int3(); // Get Kevin
        mprintf(0, "Unknown system message from DirectPlay! %d\n", lpMsg->dwType);
      }
    } else {
      if (dwMsgBufferSize >= MAX_PACKET_SIZE) {
        mprintf(0, "Error: Packet too large for D3 to handle! Discarding!!!!!\n");
      } else {
        // Presumably this is from a player we know about...
        memset(&from_addr, 0, sizeof(network_address));
        from_addr.connection_type = NP_DIRECTPLAY;
        memcpy(from_addr.address, &idFrom, sizeof(DPID));
        nw_psnet_buffer_packet((uint8_t *)packet_data, dwMsgBufferSize, &from_addr);
      }
    }
  }
}

//	Send a packet to a direct play user
int dp_DirectPlaySend(network_address *who_to, uint8_t *data, int len, bool reliable) {
  DPID idTo;
  HRESULT hr;
  uint32_t send_flags;
  if ((!DP_active) || (!lpDirectPlay4A)) {
    mprintf(0, "Can't send DirectPlay message because DirectPlay isn't active!\n");
    return 0;
  }

  ASSERT(who_to->connection_type == NP_DIRECTPLAY);

  send_flags = DPSEND_ASYNC;
  if (reliable)
    send_flags |= DPSEND_GUARANTEED;

  memcpy(&idTo, who_to->address, sizeof(DPID));
  hr = lpDirectPlay4A->SendEx(NetPlayers[Player_num].dpidPlayer, idTo, send_flags, data, len, 0, 0, NULL, NULL);

  if (FAILED(hr))
    return 0;
  else
    return 1;
}

void dp_DirectPlayDestroyPlayer(DPID who) {
  HRESULT hr;

  if ((!DP_active) || (!lpDirectPlay4A))
    return;

  hr = lpDirectPlay4A->DestroyPlayer(who);
  if (FAILED(hr)) {
    switch (hr) {
    case DPERR_ACCESSDENIED:
      mprintf(0, "lpDirectPlay4A->DestroyPlayer() returned DPERR_ACCESSDENIED.\n");
      break;
    case DPERR_CONNECTIONLOST:
      mprintf(0, "lpDirectPlay4A->DestroyPlayer() returned DPERR_CONNECTIONLOST.\n");
      break;
    case DPERR_INVALIDOBJECT:
      mprintf(0, "lpDirectPlay4A->DestroyPlayer() returned DPERR_INVALIDOBJECT.\n");
      break;
    case DPERR_INVALIDPLAYER:
      mprintf(0, "lpDirectPlay4A->DestroyPlayer() returned DPERR_INVALIDPLAYER.\n");
      break;
    case DPERR_UNAVAILABLE:
      mprintf(0, "lpDirectPlay4A->DestroyPlayer() returned DPERR_UNAVAILABLE.\n");
      break;
    default:
      mprintf(0, "Unknown error returned by nw_DirectPlayDestroyPlayer() -- %x\n", hr);
    }
  }
}

// if(Use_DirectPlay)

int dp_DirectPlayJoinGame(LPDPSESSIONDESC2 session) {
  HRESULT hr;
  do {
    hr = lpDirectPlay4A->Open(session, DPOPEN_JOIN | DPOPEN_RETURNSTATUS);
    if (DPERR_CONNECTING == hr) {
      mprintf(0, "Waiting for Directplay session to start(DPERR_CONNECTING)!\n");
      Sleep(50);
    }
  } while (DPERR_CONNECTING == hr);
  if (FAILED(hr)) {
    switch (hr) {

    case DPERR_ACCESSDENIED:
      mprintf(0, "Failed to create Directplay session (DPERR_ACCESSDENIED)!\n");
      break;
    case DPERR_ALREADYINITIALIZED:
      mprintf(0, "Failed to create Directplay session (DPERR_ALREADYINITIALIZED)!\n");
      break;
    case DPERR_AUTHENTICATIONFAILED:
      mprintf(0, "Failed to create Directplay session (DPERR_AUTHENTICATIONFAILED)!\n");
      break;
    case DPERR_CANNOTCREATESERVER:
      mprintf(0, "Failed to create Directplay session (DPERR_CANNOTCREATESERVER)!\n");
      break;
    case DPERR_CANTLOADCAPI:
      mprintf(0, "Failed to create Directplay session (DPERR_CANTLOADCAPI)!\n");
      break;
    case DPERR_CANTLOADSECURITYPACKAGE:
      mprintf(0, "Failed to create Directplay session (DPERR_CANTLOADSECURITYPACKAGE)!\n");
      break;
    case DPERR_CANTLOADSSPI:
      mprintf(0, "Failed to create Directplay session (DPERR_CANTLOADSSPI)!\n");
      break;
    case DPERR_CONNECTIONLOST:
      mprintf(0, "Failed to create Directplay session (DPERR_CONNECTIONLOST)!\n");
      break;
    case DPERR_ENCRYPTIONFAILED:
      mprintf(0, "Failed to create Directplay session (DPERR_ENCRYPTIONFAILED)!\n");
      break;
    case DPERR_ENCRYPTIONNOTSUPPORTED:
      mprintf(0, "Failed to create Directplay session (DPERR_ENCRYPTIONNOTSUPPORTED)!\n");
      break;
    case DPERR_INVALIDFLAGS:
      mprintf(0, "Failed to create Directplay session (DPERR_INVALIDFLAGS)!\n");
      break;
    case DPERR_INVALIDPARAMS:
      mprintf(0, "Failed to create Directplay session (DPERR_INVALIDPARAMS)!\n");
      break;
    case DPERR_INVALIDPASSWORD:
      mprintf(0, "Failed to create Directplay session (DPERR_INVALIDPASSWORD)!\n");
      break;
    case DPERR_LOGONDENIED:
      mprintf(0, "Failed to create Directplay session (DPERR_LOGONDENIED)!\n");
      break;
    case DPERR_NOCONNECTION:
      mprintf(0, "Failed to create Directplay session (DPERR_NOCONNECTION)!\n");
      break;
    case DPERR_NONEWPLAYERS:
      mprintf(0, "Failed to create Directplay session (DPERR_NONEWPLAYERS)!\n");
      break;
    case DPERR_NOSESSIONS:
      mprintf(0, "Failed to create Directplay session (DPERR_NOSESSIONS)!\n");
      break;
    case DPERR_SIGNFAILED:
      mprintf(0, "Failed to create Directplay session (DPERR_SIGNFAILED)!\n");
      break;
    case DPERR_TIMEOUT:
      mprintf(0, "Failed to create Directplay session (DPERR_TIMEOUT)!\n");
      break;
    case DPERR_UNINITIALIZED:
      mprintf(0, "Failed to create Directplay session (DPERR_UNINITIALIZED)!\n");
      break;
    case DPERR_USERCANCEL:
      mprintf(0, "Failed to create Directplay session (DPERR_USERCANCEL)!\n");
      break;
    default:
      mprintf(0, "Failed to create Directplay session (unknown)!\n");
    }
    // return 0;
  } else {
    HRESULT hres;
    DPNAME client_player;
    ZeroMemory(&client_player, sizeof(DPNAME));
    client_player.dwSize = sizeof(DPNAME);
    client_player.lpszShortNameA = Players[Player_num].callsign;
    client_player.lpszLongNameA = NULL;

    mprintf(0, "Creating local player...\n");
    NetPlayers[Player_num].hPlayerEvent = CreateEvent(NULL,  // no security
                                                      FALSE, // auto reset
                                                      FALSE, // initial event reset
                                                      NULL); // no name
    if (NetPlayers[Player_num].hPlayerEvent == NULL) {
      mprintf(0, "Failed to create directplay notification event!\n");
      dp_EndGame();
      DP_active = false;
      return 0;
    }
    hres = lpDirectPlay4A->CreatePlayer(&NetPlayers[Player_num].dpidPlayer, &client_player,
                                        NetPlayers[Player_num].hPlayerEvent, NULL, 0, 0);
    if (FAILED(hres)) {
      mprintf(0, "Failed to create directplay player in dp_DirectPlayJoinGame()\n");
      switch (hres) {
      case DPERR_CANTADDPLAYER:
        mprintf(0, "Unable to create player. Error code: DPERR_CANTADDPLAYER\n");
        break;
      case DPERR_CANTCREATEPLAYER:
        mprintf(0, "Unable to create player. Error code: DPERR_CANTCREATEPLAYER\n");
        break;
      case DPERR_CONNECTIONLOST:
        mprintf(0, "Unable to create player. Error code: DPERR_CONNECTIONLOST\n");
        break;
      case DPERR_INVALIDFLAGS:
        mprintf(0, "Unable to create player. Error code: DPERR_INVALIDFLAGS\n");
        break;
      case DPERR_INVALIDPARAMS:
        mprintf(0, "Unable to create player. Error code: DPERR_INVALIDPARAMS\n");
        break;
      case DPERR_NOCONNECTION:
        mprintf(0, "Unable to create player. Error code: DPERR_NOCONNECTION\n");
        break;
      default:
        mprintf(0, "Unable to create player. Error code: Unknown\n");
        break;
      }
      return 0;
    }
    return 1;
  }
  return 1;
}

void dp_ShutdownDirectPlay() {
  int i;
  mprintf(0, "Freeing DirectPlay memory\n");
  for (i = 0; i < MAX_DIRECTPLAY_CONNECTIONS; i++) {
    if (dpconns[i].conn != NULL) {
      mem_free(dpconns[i].conn);
      dpconns[i].conn = NULL;
      dpconns[i].name[0] = NULL;
    }
  }
  if (lpdplconnection)
    mem_free(lpdplconnection);
  if (lpAddress)
    mem_free(lpAddress);
  lpAddress = NULL;
  lpdplconnection = NULL;
  // Delete any previous directplay instance
  mprintf(0, "Releasing DirectPlay COM object (lpDirectPlay4A)\n");
  // if(lpDirectPlay4A)
  //	lpDirectPlay4A->Release();
  // lpDirectPlay4A = NULL;
  mprintf(0, "Done shutting down DirectPlay.\n");
}

// ---------------------------------------------------------------------------
// EnumModemAddress
// ---------------------------------------------------------------------------
// Description: Enumeration callback called by DirectPlayLobby.
//              Enumerates the DirectPlay address chunks. If the
//              chunk contains modem strings, add them to the control.
// Arguments:
//  REFGUID     [in] GUID of the address type
//  DWORD       [in] size of chunk
//  LPVOID      [in] pointer to chunk
//  LPVOID      [in] user-defined context
// Returns:
//  BOOL        FALSE to stop enumerating after the first callback
BOOL FAR PASCAL EnumModemAddress(REFGUID lpguidDataType, DWORD dwDataSize, LPCVOID lpData, LPVOID lpContext) {
  HWND hWnd = (HWND)lpContext;
  LPSTR lpszStr = (LPSTR)lpData;

  // modem
  if (IsEqualGUID(lpguidDataType, DPAID_Modem)) {
    // loop over all strings in list
    while (strlen(lpszStr)) {
      mprintf(0, "Found modem: %s\n", lpszStr);
      if (Num_modems_found < MAX_MODEMS) {
        // store modem name
        strcpy(Modems_found[Num_modems_found].name, lpszStr);
        Num_modems_found++;
      }
      // skip to next string
      lpszStr += strlen(lpszStr) + 1;
    }
  }
  return (TRUE);
}

// char Directplay_modem_list[MAX_MODEMS][200];
// int Num_modems_found = 0;

// Call this function with size set to 0, and it will fill in size with
// the amount of buffer space you need
// Otherwise, it will fill in the buffer with a bunch of null delimited
// strings, with a double null at the end.
int dp_GetModemChoices(char *buffer, LPDWORD size) {
  LPDIRECTPLAY4A lpTempDP4;
  HRESULT hr;
  void *connection = NULL;
  ;
  int i;
  Num_modems_found = 0;
  if (SUCCEEDED(
          CoCreateInstance(CLSID_DirectPlay, NULL, CLSCTX_INPROC_SERVER, IID_IDirectPlay4A, (LPVOID *)&lpTempDP4))) {

    lpTempDP4->EnumConnections(&DPD3_GUID, DirectPlayEnumConnectionsCallback, 0, 0);

    for (i = 0; i < MAX_DIRECTPLAY_CONNECTIONS; i++) {
      if (stricmp(dpconns[i].name, "Modem Connection For DirectPlay") == 0) {
        // mprintf(0,"Found DirectPlay connection: %s\n",name);
        connection = dpconns[i].conn;
        break;
      }
    }
    if (!connection) {
      mprintf(0, "Unable to find DirectPlay Modem connection.\n");
      return 0;
    }

    memset(&dpconns, 0, sizeof(dpconnections) * MAX_DIRECTPLAY_CONNECTIONS);

    hr = lpTempDP4->InitializeConnection(connection, 0);

    for (i = 0; i < MAX_DIRECTPLAY_CONNECTIONS; i++) {
      if (dpconns[i].conn != NULL) {
        mem_free(dpconns[i].conn);
        dpconns[i].conn = NULL;
        dpconns[i].name[0] = NULL;
      }
    }

    if (FAILED(hr)) {
      mprintf(0, "lpTempDP4->InitializeConnection() failed.\n");
      switch (hr) {
      case DPERR_ALREADYINITIALIZED:
        mprintf(0, "Error: DPERR_ALREADYINITIALIZED\n");
        break;
      case DPERR_INVALIDFLAGS:
        mprintf(0, "Error: DPERR_INVALIDFLAGS\n");
        break;
      case DPERR_INVALIDPARAMS:
        mprintf(0, "Error: DPERR_INVALIDPARAMS\n");
        break;
      case DPERR_UNAVAILABLE:
        mprintf(0, "Error: DPERR_UNAVAILABLE\n");
        break;
      default:
        mprintf(0, "Error: Unknown\n");
      }

      return 0;
    }
    lpTempDP4->GetPlayerAddress(0, buffer, size);
    if (buffer) {
      if (lpDirectPlayLobby3A) {
        lpDirectPlayLobby3A->Release();
        lpDirectPlayLobby3A = NULL;
      }
      hr = CoCreateInstance(CLSID_DirectPlayLobby, NULL, CLSCTX_INPROC_SERVER, IID_IDirectPlayLobby3A,
                            (LPVOID *)&lpDirectPlayLobby3A);
      if (SUCCEEDED(hr))
        lpDirectPlayLobby3A->EnumAddress(EnumModemAddress, buffer, *size, 0);
      else
        mprintf(0, "Couldn't create IID_IDirectPlayLobby3A COM Instance!\n");
    }

    lpTempDP4->Release();
    return 1;
  }
  return 0;
}

// Register a DirectPlay lobby aware application
// Use this so a directplay lobby provider such as zone.com can launch the game
//
// Parameters:
// appname		The non-localized name of the application (ie. "Descent 3") DON'T LOCALIZE THIS! IT IS AN ID
//	exefile		Executable file name (without path)
//	exepath		Path to executable file
//	arguments	Any command line arguments the app needs
//	workingdir	The Working directory for the application
//	description	Localized description of the application
void dp_RegisterLobbyApplication(char *appname, char *exefile, char *exepath, char *arguments, char *workingdir,
                                 char *description) {
  static DPAPPLICATIONDESC AppDesc;

  AppDesc.dwSize = sizeof(DPAPPLICATIONDESC);
  AppDesc.dwFlags = 0;
  AppDesc.lpszApplicationNameA = appname;
  AppDesc.guidApplication = DPD3_GUID;
  AppDesc.lpszFilenameA = exefile;
  AppDesc.lpszCommandLineA = arguments;
  AppDesc.lpszPathA = exepath;
  AppDesc.lpszCurrentDirectoryA = workingdir;
  AppDesc.lpszDescriptionA = description;

  HRESULT hr;

  mprintf(0, "Registering DirectPlay Lobby application:\n");
  mprintf(0, "AppDesc.lpszApplicationNameA = %s\n", appname);
  mprintf(0, "AppDesc.lpszFilenameA = %s\n", exefile);
  mprintf(0, "AppDesc.lpszCommandLineA = %s\n", arguments);
  mprintf(0, "AppDesc.lpszPathA = %s\n", exepath);
  mprintf(0, "AppDesc.lpszCurrentDirectoryA = %s\n", workingdir);
  mprintf(0, "AppDesc.lpszDescriptionA = %s\n", description);

  hr = CoCreateInstance(CLSID_DirectPlayLobby, NULL, CLSCTX_INPROC_SERVER, IID_IDirectPlayLobby3A,
                        (LPVOID *)&lpDirectPlayLobby3A);
  if (SUCCEEDED(hr)) {
    hr = lpDirectPlayLobby3A->RegisterApplication(0, &AppDesc);
    if (FAILED(hr)) {
      mprintf(0, "IDirectPlayLobby3::RegisterApplication() failed.\n");
      if (hr == DPERR_INVALIDFLAGS)
        mprintf(0, "Error = DPERR_INVALIDFLAGS\n");
      else if (hr == DPERR_INVALIDPARAMS)
        mprintf(0, "Error = DPERR_INVALIDPARAMS\n");
      else
        mprintf(0, "Error = ??????????? %x\n", hr);
    } else {
      mprintf(0, "IDirectPlayLobby3::RegisterApplication() succeded.\n");
    }
  } else {
    mprintf(0, "IDirectPlayLobby3 CoCreateInstance() failed.\n");
    return;
  }

  // lpDirectPlayLobby3A->Release();
}

// Returns TRUE if the game was launched from a lobby
bool dp_DidLobbyLaunchGame() {
  HRESULT hr;
  DWORD buffersize = sizeof(DPLCONNECTION);

  Directplay_lobby_launched_game = false;
  if (lpDirectPlayLobby3A) {
    lpDirectPlayLobby3A->Release();
    lpDirectPlayLobby3A = NULL;
  }
  CoInitialize(NULL);
  hr = CoCreateInstance(CLSID_DirectPlayLobby, NULL, CLSCTX_INPROC_SERVER, IID_IDirectPlayLobby3A,
                        (LPVOID *)&lpDirectPlayLobby3A);
  if (SUCCEEDED(hr)) {
    hr = lpDirectPlayLobby3A->GetConnectionSettings(0, NULL, &buffersize);
    if (FAILED(hr)) {
      if (hr == DPERR_NOTLOBBIED) {
        mprintf(0, "Application not started from a lobby.\n");
        lpDirectPlayLobby3A->Release();
        lpDirectPlayLobby3A = NULL;
        return false;
      }
      mprintf(0, "Unable to determine DirectPlay lobby connection info buffer size!!!\n");
      lpDirectPlayLobby3A->Release();
      lpDirectPlayLobby3A = NULL;
      return false;
    }
    lpdplconnection = (LPDPLCONNECTION)mem_malloc(buffersize);
    hr = lpDirectPlayLobby3A->GetConnectionSettings(0, lpdplconnection, &buffersize);
    lpDirectPlayLobby3A->Release();
    lpDirectPlayLobby3A = NULL;
    if (FAILED(hr)) {
      mprintf(0, "IDirectPlayLobby3::GetConnectionSettings() failed.\n");
      if (hr == DPERR_NOTLOBBIED) {
        mprintf(0, "Application not started from a lobby.\n");
        return false;
      } else {
        mprintf(0, "Unknown error from IDirectPlayLobby3::GetConnectionSettings().\n");
        return false;
      }
    } else {
      mprintf(0, "Application was started from a lobby.\n");
      Directplay_lobby_launched_game = true;
      return true;
    }
  } else {
    mprintf(0, "IDirectPlayLobby3 CoCreateInstance() failed.\n");
  }
  return false;
}

// Autoconnects to a game or starts one, based on the directplay lobby stuff
bool dp_AutoConnect() {
  if (!Directplay_lobby_launched_game)
    return false;

  if (lpdplconnection->dwFlags == DPLCONNECTION_CREATESESSION) {
    //	We are hosting a game
    Use_DirectPlay = true;
    // What level do we use? what dll? what pps, etc....

  } else if (lpdplconnection->dwFlags == DPLCONNECTION_JOINSESSION) {
    //	We are joining a game
    Use_DirectPlay = true;
  }

  return false;
}
