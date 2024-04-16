#ifndef GAMEEVENT_H
#define GAMEEVENT_H

#include "game.h"
#include "object.h"

#define MAX_EVENTS 500

// Game event types
#define OBJECT_EVENT 1
#define RENDER_EVENT 2

// IDs
#define UNKNOWN_EVENT 0
#define FUSION_EFFECT 1
#define DAMAGE_EFFECT 2
#define SCREEN_BLEND 3
#define BLAST_RING_EVENT 4
#define FOV_CHANGE_EVENT 5
#define EDRAIN_EFFECT 6

#define D3X_TIMED_EVENT 256

typedef struct game_event {
  int type;
  int id;
  int objhandle_detonator; // watch this object, if it dies/gets killed than cancel this game event
  ubyte used;
  float start_time, end_time;
  int frame_born;

  void *data;

  void (*subfunction)(int, void *);
} game_event;

// Adds and event to the list. The event will trigger at Gametime+length
int CreateNewEvent(int type, int id, float length, void *data, int size, void (*subfunction)(int eventnum, void *data),
                   int objhandle_detonator = OBJECT_HANDLE_NONE);

// Processes all pending events, removing the ones that are expired
void ProcessNormalEvents();
void ProcessRenderEvents();

// Does a specific action according to an event
void HandleEvent(game_event *);

// Clears the event list
void ClearAllEvents();

// Returns -1 if can't find event, else index of event int GameEvent array
int FindEventID(int id);

// Frees an event for use by others
void FreeEvent(int index);

extern game_event GameEvent[MAX_EVENTS];
extern int Num_events;

#endif