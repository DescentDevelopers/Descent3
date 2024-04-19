#ifndef __DMFC_APP_H_
#define __DMFC_APP_H_

// DMFCApp Class
//    This is the class you actually make an instance of.
//    It will handle processing of events, calling defaults where needed, or calling your overrides
//    To create an event handler override, add it's prototype to this class, and then define the function
//    in your multiplayer source file.  See examples to do this.  The prototype must be exactly the same 
//    as it is in DMFCBase class.
class DMFCApp : public DMFCBase
{
public:
	//Setup and processing functions
	void GameInit(int teams);
	void GameClose(void);

	//!!!!!!!ADD YOUR EVENT HANDLER OVERRIDES HERE!!!!!!!!!!!
	bool OnCanChangeTeam(int pnum,int newteam);
	void OnClientPlayerDisconnect(int player_num);
	void OnPlayerEntersObserver(int pnum,object *piggy);
	void HandlePowerballLoss(int pnum,bool spew_shields);

	//void OnServerWeaponCollide(object *pobj,object *wobj,vector *point,vector *normal,bool is_electrical);
	void OnGameStateRequest(int pnum);
	void OnClientPlayerKilled(object *killer_obj,int victim_pnum);
	void OnServerGameCreated(void);
	void OnClientLevelStart(void);
	void OnClientLevelEnd(void);
	void OnPlayerConnect(int player_num);
	void OnInterval(void);
	void OnHUDInterval(void);
	void OnKeypress(int key);
	void OnServerCollide(object *me_obj,object *it_obj);
	void OnClientCollide(object *me_obj,object *it_obj);
	void OnServerPlayerChangeSegment(int player_num,int newseg,int oldseg);
	void OnClientPlayerChangeSegment(int player_num,int newseg,int oldseg);
	void OnSaveStatsToFile(void);
	void OnPLRInterval(void);
	void OnPLRInit(void);
private:
};


/*
========================================================================================================
							DMFCApp Members
*/

/*
 * If you need to do some specific initialization when the DLL loads, put that in DMFCApp::GameInit,
 * likewise, special deallocation/deinitialization should go in DMFCApp::GameClose.  Most of the time
 * you won't need to add anything here.
 */

// DMFCApp::GameInit
//   
//   Sets up all the DLL functions and pointers and preps the class for use.  This ABSOLUTLY must be 
//   called, so if you override DMFCApp::GameInit, make sure that you put a call to this somewhere in
//   the override.
void DMFCApp::GameInit(int teams)
{
	DMFCBase::GameInit(teams);
}

// DMFCBase::GameClose
//
//   Closes up any needed DLL and DMFC tasks.  This ABSOLUTLY must be called, so if you override
//   DMFCApp::GameClose, make sure that you put a call to this somewhere in the override
void DMFCApp::GameClose(void)
{
	DMFCBase::GameClose();
}

//Declare the instance of the multiplayer game
DMFCApp PBall;

/*************************************************************************************************
 *The following functions and declaration are needed to connect the DLL to the game.  These must *
 *stay here and must call the functions that are in them.  You can not delete from here, but you *
 *can add to it no problem																								 *
 *************************************************************************************************
*/

// These next two function prototypes MUST appear in the extern "C" block if called
// from a CPP file.
extern "C"
{
	DLLEXPORT void DLLFUNCCALL DLLGameInit (int *api_func,ubyte *all_ok);
	DLLEXPORT void DLLFUNCCALL DLLGameCall (int eventnum,dllinfo *data);
	DLLEXPORT void DLLFUNCCALL DLLGameClose ();
	DLLEXPORT void DLLFUNCCALL DLLGetGameInfo (tDLLOptions *options);
	DLLEXPORT int DLLFUNCCALL GetGOScriptID(char *name,ubyte isdoor);
	DLLEXPORT void DLLFUNCCALLPTR CreateInstance(int id);
	DLLEXPORT void DLLFUNCCALL DestroyInstance(int id,void *ptr);
	DLLEXPORT short DLLFUNCCALL CallInstanceEvent(int id,void *ptr,int event,tOSIRISEventInfo *data);
	DLLEXPORT int DLLFUNCCALL SaveRestoreState( void *file_ptr, ubyte saving_state );
}

// The main entry point where the game calls the dll
void DLLFUNCCALL DLLGameCall (int eventnum,dllinfo *data)
{
	if((eventnum<EVT_CLIENT_INTERVAL) && (PBall.GetLocalRole()!=LR_SERVER)){
		return;
	}

	PBall.TranslateEvent(eventnum,data);
}

//	GetGOScriptID
//	Purpose:
//		Given the name of the object (from it's pagename), this function will search through it's
//	list of General Object Scripts for a script with a matching name (to see if there is a script
//	for that type/id of object within this DLL).  If a matching scriptname is found, a UNIQUE ID
//	is to be returned back to Descent 3.  This ID will be used from here on out for all future
//	interaction with the DLL.  Since doors are not part of the generic object's, it's possible
//	for a door to have the same name as a generic object (OBJ_POWERUP, OBJ_BUILDING, OBJ_CLUTTER
//	or OBJ_ROBOT), therefore, a 1 is passed in for isdoor if the given object name refers to a
//	door, else it is a 0.  The return value is the unique identifier, else -1 if the script
//	does not exist in the DLL.
int DLLFUNCCALL GetGOScriptID(char *name,ubyte isdoor)
{
	return -1;
}

//	CreateInstance
//	Purpose:
//		Given an ID from a call to GetGOScriptID(), this function will create a new instance for that
//	particular script (by allocating and initializing memory, etc.).  A pointer to this instance
//	is to be returned back to Descent 3.  This pointer will be passed around, along with the ID
//	for CallInstanceEvent() and DestroyInstance().  Return NULL if there was an error.
void DLLFUNCCALLPTR CreateInstance(int id)
{
	return NULL;
}

//	DestroyInstance
//	Purpose:
//		Given an ID, and a pointer to a particular instance of a script, this function will delete and
//	destruct all information associated with that script, so it will no longer exist.
void DLLFUNCCALL DestroyInstance(int id,void *ptr)
{
}

//	CallInstanceEvent
//	Purpose:
//		Given an ID, a pointer to a script instance, an event and a pointer to the struct of
//	information about the event, this function will translate who this event belongs to and
//	passes the event to that instance of the script to be handled.  Return a combination of
//	CONTINUE_CHAIN and CONTINUE_DEFAULT, to give instructions on what to do based on the
//	event. CONTINUE_CHAIN means to continue through the chain of scripts (custom script, level
//	script, mission script, and finally default script).  If CONTINUE_CHAIN is not specified,
//	than the chain is broken and those scripts of lower priority will never get the event.  Return
//	CONTINUE_DEFAULT in order to tell D3 if you want process the normal action that is built into
//	the game for that event.  This only pertains to certain events.  If the chain continues
//	after this script, than the CONTINUE_DEFAULT setting will be overridden by lower priority
//	scripts return value.
short DLLFUNCCALL CallInstanceEvent(int id,void *ptr,int event,tOSIRISEventInfo *data)
{
	return CONTINUE_CHAIN|CONTINUE_DEFAULT;
}

//	SaveRestoreState
//	Purpose:
//		This function is called when Descent 3 is saving or restoring the game state.  In this function
//	you should save/restore any global data that you want preserved through load/save (which includes
//	demos).  You must be very careful with this function, corrupting the file (reading or writing too
//	much or too little) may be hazardous to the game (possibly making it impossible to restore the
//	state).  It would be best to use version information to keep older versions of saved states still
//	able to be used.  IT IS VERY IMPORTANT WHEN SAVING THE STATE TO RETURN THE NUMBER OF _BYTES_ WROTE
//	TO THE FILE.  When restoring the data, the return value is ignored.  saving_state is 1 when you should
//	write data to the file_ptr, 0 when you should read in the data.
int DLLFUNCCALL SaveRestoreState( void *file_ptr, ubyte saving_state )
{
	return 0;
}

#endif