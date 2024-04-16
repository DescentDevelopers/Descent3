#ifndef MULTI_UI_H
#define MULTI_UI_H

// The first multiplayer menu that the user will see...all multiplayer stuff is
// reached from this menu
// Returns true if we're starting a multiplayer game
int MainMultiplayerMenu ();
int SearchMasterTrackerGameMenu ();
int LoginMasterTrackerGameMenu ();
int AutoConnectPXO();
int AutoConnectLANIP();
int AutoConnectHeat();
void DoMultiAllowed(void);
extern int MultiDLLGameStarting;
void MultiDoConfigLoad(void);
void MultiDoConfigSave(void);
void MultiGameOptionsMenu (int alloptions);
int ReturnMultiplayerGameMenu(void);
int MultiLevelSelection(void);
bool DoPlayerMouselookCheck(unsigned int flags);
#endif