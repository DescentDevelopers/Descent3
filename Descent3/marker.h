#ifndef MARKER_H
#define MARKER_H

#include "player.h"

#define MAX_MARKER_MESSAGE_LENGTH 40

// Drops a marker at the players current position
void DropMarker(char *message);

// Resets markers before a level stars
void ResetMarkers();

// Inits markers at game load
void InitMarkers();

extern int Marker_polynum;
extern int Marker_message;

extern char MarkerMessages[MAX_PLAYERS * 2][MAX_MARKER_MESSAGE_LENGTH];

#endif