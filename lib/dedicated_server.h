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

#ifndef DEDICATED_SERVER_H
#define DEDICATED_SERVER_H

#define CVAR_GAMEINIT 0x0001 // this variable can be set/changed during server init
#define CVAR_GAMEPLAY 0x0002 // this variable can be set/changed during game play

enum cvar_type {
  CVAR_TYPE_INT,
  CVAR_TYPE_FLOAT,
  CVAR_TYPE_STRING,
  CVAR_TYPE_NONE,
};

struct cvar_entry {
  const char *varname;
  cvar_type type;
  void *dest_variable;
  int var_min, var_max;
  int16_t permissions;
};

extern bool Dedicated_server;

// Sets the value for a cvar INT type
void SetCVarInt(int index, int val);

// Sets the value for a cvar FLOAT type
void SetCVarFloat(int index, float val);

// Sets the value for a cvar string type
void SetCVarString(int index, const char *val);

// The accessor function that sets the value of a cvar
void SetCVar(const char *cvar_string, const char *cvar_argument, bool in_game_init);

// The accessor function that sets the value of a cvar...INT only
void SetCVar(const char *cvar_string, int cvar_argument);

// The accessor function that sets the value of a cvar...FLOAT only
void SetCVar(const char *cvar_string, float cvar_argument);

// Starts a dedicated server and loads the server config file
void StartDedicatedServer();

// Reads in the server config file for a dedicated server
// Returns true if everything is ok
int LoadServerConfigFile();

// Called once per frame for the dedicated server
void DoDedicatedServerFrame();

// Prints a message to the console if the dedicated server is active
void PrintDedicatedMessage(const char *fmt, ...);

// Reads incoming data from the telnet connection to the server
void DedicatedReadTelnet(void);

// Sends a string to all connected and logged in clients
void DedicatedSocketputs(char *str);

// Look for incoming connections
void ListenDedicatedSocket(void);

// Init the socket and start listening
void InitDedicatedSocket(uint16_t port);

#endif
