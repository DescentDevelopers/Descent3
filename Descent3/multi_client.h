#ifndef MULTI_CLIENT_H

// Do client stuff for this frame
void MultiDoClientFrame();

// Set the local machine as a client
void MultiStartClient(char *scriptname);

#define MAX_SAVED_MOVES 100

typedef struct {
  float timestamp;
  vector thrust, rotthrust;

} saved_move;

extern saved_move SavedMoves[];
extern int Current_saved_move;

// Alloc and free saved move
extern int MultiAllocSavedMove();
extern void MultiFreeSavedMove(int);

#endif