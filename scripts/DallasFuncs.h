#pragma once
#include <variant>
#include <initializer_list>
#include <vector>

class user_var : public std::variant<float, int32_t> {
	private:
	using VarT = std::variant<float, int32_t>;

	public:
	// Required for compatibility with GCC 9/Ubuntu 20.04 that does not implement P2162R2,
	// using std::visit on classes inheriting from std::variant
	// (see https://gcc.gnu.org/bugzilla/show_bug.cgi?id=90943)
	// Move back to std::visit when we drop compatibility
	template <typename F>
	decltype(auto) visit(F&& f) & {
		return std::visit(std::forward<F>(f), static_cast<VarT&>(*this));
	}

	// For compatibility with existing code, override op= with one that
	// retains the active type, requiring instead set_type<> to change it.
	template<typename T> void operator=(T &&v) noexcept {
		VarT &self = *this;
		if (std::get_if<float>(this))
			self = static_cast<float>(v);
		else
			self = static_cast<int32_t>(v);
	}
	template<typename T> void set_type() noexcept { static_cast<VarT &>(*this) = T{}; }
	void operator++(int) noexcept { this->visit([](auto &&uv) { ++uv; }); }
	void operator--(int) noexcept { this->visit([](auto &&uv) { ++uv; }); }
	template<typename T> void operator+=(T &&r) noexcept { this->visit([&](auto &&uv) { uv += std::forward<T>(r); }); }
	template<typename T> void operator-=(T &&r) noexcept { this->visit([&](auto &&uv) { uv -= std::forward<T>(r); }); }
	template<typename T> bool operator==(T &&r) noexcept { return this->visit([&](auto &&uv) { return uv == std::forward<T>(r); }); }
	template<typename T> bool operator!=(T &&r) noexcept { return this->visit([&](auto &&uv) { return uv != std::forward<T>(r); }); }
	template<typename T> bool operator<(T &&r) noexcept { return this->visit([&](auto &&uv) { return uv < std::forward<T>(r); }); }
	template<typename T> bool operator>(T &&r) noexcept { return this->visit([&](auto &&uv) { return uv > std::forward<T>(r); }); }


};

#define MAX_USER_VARS 25 // make sure this value matches the USERTYPE definition
extern std::vector<user_var> User_vars;

#define MAX_SPEW_HANDLES 50 // make sure this value matches the USERTYPE definition
extern int Spew_handles[MAX_SPEW_HANDLES];

#define MAX_SAVED_OBJECT_HANDLES 20 // make sure this value matches the USERTYPE definition
extern int Saved_object_handles[MAX_SAVED_OBJECT_HANDLES];

//
//  System functions
//

extern void dfInit(const std::initializer_list<int> & = {});
extern void dfSave(void *fileptr);
extern void dfRestore(void *fileptr);

extern void aPortalRenderSet(int state, int portalnum, int roomnum, bool doublesided);
extern void aPortalBlockageSet(int state, int portalnum, int roomnum);
extern void aPortalRenderToggle(int portalnum, int roomnum, bool doublesided);
extern void aPortalBreakGlass(int portalnum, int roomnum);
extern void aRoomSetDamage(int roomnum, float damage, int soundtype);
extern void aShowHUDMessage(const char *format, ...);
extern void aShowHUDMessageObj(const char *format, int objhandle, ...);
extern void aShowColoredHUDMessage(int red, int green, int blue, const char *format, ...);
extern void aShowColoredHUDMessageObj(int red, int green, int blue, const char *format, int objhandle, ...);
extern void aAddGameMessage(const char *game_message, const char *hud_message);
extern void aRainTurnOn(float density);
extern void aRainTurnOff();
extern void aSnowTurnOn(float density);
extern void aSnowTurnOff();
extern void aLightningTurnOn(float check_delay, float prob);
extern void aLightningTurnOff();
extern void aLightningCreate(int objhandle1, int objhandle2, float lifetime, float thickness, int numtiles, int texture_id, float slidetime, int timesdrawn, int red, int green, int blue, bool autotile);
extern void aLightningCreateGunpoints(int gunpoint1, int gunpoint2, int objhandle, float lifetime, float thickness, int numtiles, int texture_id, float slidetime, int timesdrawn, int red, int green, int blue, bool autotile);
extern void aUserFlagSet(int flagnum, bool state);
extern void aUserVarSet(int varnum, float value);
extern void aUserVarInc(int varnum);
extern void aUserVarDec(int varnum);
extern void aUserVarAdd(float value, int varnum);
extern void aUserVarSub(float value, int varnum);
extern void aDoorLockUnlock(int state, int objref);
extern void aDoorActivate(int objref);
extern void aDoorSetPos(int objhandle, float pos);
extern void aDoorStop(int objhandle);
extern void aCreatePopupView(int gunpoint, int objref, float time, float zoom);
extern void aClosePopupView();
extern void aObjSetShields(int objref, float shields);
extern void aObjSetEnergy(int objref, float energy);
extern void aObjPlayAnim(int objref, int startframe, int endframe, float cycletime, bool looping);
extern void aObjApplyDamage(int objref, float damage);
extern void aObjKill(int objhandle, int delay_type, int expl_size, int death_flags, float min_delay, float max_delay);
extern void aObjDestroy(int objhandle);
extern void aObjDelete(int objhandle);
extern void aObjSetLightingDist(int objhandle, float dist);
extern void aObjSetLightingColor(int objhandle, float r, float g, float b);
extern void aObjGravityEnable(int enable, int objhandle);
extern void aObjSetMovementType(int objhandle, int mtype);
extern void aObjSetVelocity(int objhandle, float x, float y, float z, float speed);
extern void aRoomSetLightingStrobe(int state, int roomnum);
extern void aRoomSetLightingFlicker(int state, int roomnum);
extern void aRoomSetFuelcen(int state, int roomnum);
extern void aRoomSetLightingPulse(int roomnum, float time, float offset);
extern void aSetWaypoint(int number);
extern void aTurnOnSpew(int objref, int gunpoint, int effect_type, float mass, float drag, int gravity_type, uint8_t isreal, float lifetime, float interval, float longevity, float size, float speed, uint8_t random, int handle_slot);
extern void aTurnOffSpew(int handle_slot);
extern void aAttachObject(const char *objtypename, int childpoint, int objref, int parentpoint);
extern void aUnAttachObject(int objref);
extern void aDropObjects(int objref);
extern void aAttachExistingObject(int child_ref, int childpoint, int objref, int parentpoint);
extern void aMusicSetRegion(int region_num, int objhandle);
extern void aMusicSetRegionAll(int region_num);
extern void aSetObjectTimer(int objref, float time, int id);
extern void aSetLevelTimer(float time, int id);
extern void aCancelTimer(int id);
extern void aTimerShow(int id);
extern void aEndLevel();
extern void aFailLevel();
extern void aStartEndlevelSequence(int objhandle, int pathid, float time, const char *text);
extern void aStartEndlevelSequencePath(int camerapath, int pathid, float time, const char *text);
extern void aFadeWhiteAndEndlevel(float time, const char *text);
extern void aFadeAndMovePlayer(int Player);
extern void aMissionSetFlag(int flagnum, bool state);
extern void aMissionSetSecretFlag(bool state);
extern void aMissionSetLevelFlag(int flagnum, bool state);
extern void aSoundPlay2DObj(int soundnum, int objhandle, float volume);
extern void aSoundPlay2D(int soundnum, float volume);
extern void aSoundPlayObject(int soundnum, int objref, float volume);
extern void aSoundPlaySteaming(const char *soundname, float volume);

/*
$$ACTION
Sound && Music
Play streaming sound [a:SoundName] for all players, volume = [p:Volume=1.0:0.0|1.0]  (TEXT NAME VERSION)
aSoundPlaySteamingText
Play Steaming Sound
  Plays a streaming sound sample

Parameters:
  SoundName: the sound to play
  Volume: how loud to play the sound
$$END
*/
#define aSoundPlaySteamingText aSoundPlaySteaming

extern void aSoundPlaySteamingObj(const char *soundname, int objhandle, float volume);
extern void aSoundVolumeObj(int objhandle, float volume);
extern void aSoundStopObj(int objhandle);
extern void aObjGhostSet(int state, int objref);
extern void aObjHide(int objref);
extern void aObjMakeInvuln(int objref, int time);
extern void aObjMakeVulnerable(int objref);
extern void aObjDeform(int objhandle, float amount, float time);
extern void aObjSpark(int objhandle, float rate, float time);
extern void aMiscViewerShake(float amount);
extern void aMiscShakeArea(int objhandle, float amount, float dist);
extern void aRoomSetFaceTexture(int roomnum, int facenum, int texturenum);
extern void aMatcenSetState(int state, int matcen_id);
extern void aMatcenSetEnableState(int state, int matcen_id);
extern void aMatcenSetValues(int matcen_id, int max_produced, float multiplier, int max_alive);
extern void aRoomSetFog(int roomnum, float r, float g, float b, float depth);
extern void aRoomChangeFog(int roomnum, float r, float g, float b, float depth, float time);
extern void aRoomFogSetState(int state, int roomnum);
extern void aRoomToggleFog(int roomnum);
extern void aRoomSetWind(int roomnum, float x, float y, float z, float speed);
extern void aRoomChangeWind(int roomnum, float x, float y, float z, float speed, float time);
extern void aSetScriptedDeath(int objhandle, int state);
extern void aAISetState(int state, int objhandle);
extern void aAISetFOV(int objhandle, float fov);
extern void aAISetTarget(int objhandle, int targethandle);
extern void aAISetTeam(int team, int objhandle);
extern void aAIClearGoal(int slot, int handle);
extern void aAISetMode(int objhandle, int modetype);
extern void aAISetMaxSpeed(int objhandle, float max_speed);

// Used for all AI Goal calls
#define DEFAULT_INFLUENCE 1.0

extern void aAIFlags(int set, int flags, int handle);
extern void aAIGoalSetCircleDistance(int objhandle, int slot, float distance);
extern void aAIGoalGotoRoom(int objhandle, int roomnum, int slot, int flags, int goalid);
extern void aAIGoalGotoObject(int objhandle, int target, int slot, int flags, int goalid);
extern void aAIGoalFollowPathSimple(int objhandle, int pathid, int flags, int goalid, int priority = 0);
extern void aAIGoalFollowPath(int objhandle, int pathid, int firstnode, int lastnode, int startnode, int slot, int flags, int goalid);
extern void aAIGoalPickUpObject(int pickerhandle, int pickerpoint, int pickeehandle, int pickeepoint, bool aligned, int slot, int flags, int goalid);
extern void aAIGoalSetObjectOnObject(int pickerhandle, int pickerpoint, int pickeehandle, int pickeepoint, int slot, int flags, int goalid);
extern void aAIGoalLandOnObject(int pickerhandle, int pickerpoint, int pickeehandle, int pickeepoint, int slot, int flags, int goalid);
extern void aAIGoalPickUpObjectRad(int pickerhandle, int pickerpoint, int pickeehandle, float radius, int slot, int flags, int goalid);
extern void aPhysFlags(int set, int flags, int handle);
extern void aCinematicSimple(int pathid, const char *Text, int Target, float Seconds, bool FadeIn = false);
extern void aCinematicIntro(int camera_path, const char *Text, int NoLongerUserTarget, int PlayerPath, float Seconds);
extern void aCinematicStop();
extern void aComplexCinematicStart();
extern void aComplexCinematicEnd(const char *Text, float Seconds);
extern void aComplexCinematicText(float Start, float End);
extern void aComplexCinematicTrack(int Target, float Start, float End);
extern void aComplexCinematicPlayerDisabled(float Start, float End);
extern void aComplexCinematicCameraView(float Start, float End);
extern void aComplexCinematicQuickExit(float Start, float End);
extern void aComplexCinematicEndTrans(int End);
extern void aComplexCinematicStartTrans(bool Enable);
extern void aComplexCinematicCameraOnPath(int Path);
extern void aComplexCinematicCameraAtPoint(vector *Position, int Room);
extern void aComplexCinematicCameraAtStoredPt(int Room);
extern void aComplexCinematicScreenMode(int Mode);
extern void aComplexCinematicTextMode(int Mode);
extern void aComplexCinematicTextLayoutMode(int Mode);
extern void aComplexCinematicFlags(int flags);
extern void aEnableShip(const char *Ship);
extern void aDisableShip(const char *Ship);
extern void aTriggerSetState(int state, int trigger_num);
extern void aAddObjectToInventory(int Object, int PlayerObject, bool Spewable);
extern void aAddObjectToInventoryNamed(int Object, int PlayerObject, const char *name, bool Spewable);
extern void aRemoveObjectFromInventory(int Object, int PlayerObject);
extern void aStoreObjectInPositionClipboard(int Object);
extern void aMoveObjectToPositionClipboard(int Object);
extern void aCloakObject(int Object, float Seconds);
extern void aUnCloakObject(int Object);
extern void aCloakAllPlayers(float Seconds);
extern void aUnCloakAllPlayers(void);
extern void aSetObjectOnFire(int obj, float seconds, float dps);
extern void aObjFireWeapon(const char *weapon_name, int gun_num, int objhandle);

/*
$$ACTION
Objects
Fire flare from gun number [i:GunNum] of object [o:Object]
aObjFireFlare
Fire flare
  Fires a flare from the given gun number of an object

Parameters:
  GunNum - the gun number to fire from, or -1 to fire from the object's center
  Object - the object to fire the flare
$$END
*/
#define aObjFireFlare(gun_num, objhandle) aObjFireWeapon("Yellow flare", gun_num, objhandle)

extern void aDestroyAllRobotsInit();
extern void aDestroyAllRobotsSpareType(const char *name);
extern void aDestroyAllRobotsSpareHandle(int handle);
extern void aDestroyAllRobotsEnd(void);
extern void aEnableVirusInfection(int enable, int handle);
extern void aEnableNegativeLighting(int enable, int handle);
extern void aStripWeaponsEnergy(int Object);
extern void aStripWeaponsEnergyFromAll(void);
extern void aRemovePowerupsInRoom(int Room);
extern bool qVirusInfected(int handle);
extern bool qNegativeLight(int handle);
extern int qGetDifficulty(void);
extern float qObjectCloakTime(int Object);
extern float qObjectPosition(int handle, int axis);
extern bool qAICloseToTarget(int me);
extern bool qAIIsObjectAware(int me);
extern int qAIGetTarget(int me);
extern bool qHasObjectInInventory(int PlayerObject, int Object);
extern void aObjSaveHandle(int objhandle, int slot);
extern void aGoalCompleted(int goal_index, int completed);
extern void aGoalItemCompleted(int goal_index, int item_index, int completed);
extern void aGoalFailed(int goal_index, int failed);
extern void aGoalEnableDisable(int enabled, int goal_index);
extern void aGoalSetPriority(int goal_index, int priority);
extern void aGoalSetCompletionMessage(int goal_index, const char *message);
extern void aGoalSetGBKnowledge(int enabled, int goal_index);
extern bool qObjExists(int objref);
extern bool qObjIsPlayer(int objref);
extern bool qObjIsPlayerWeapon(int handle);
extern bool qObjIsPlayerOrPlayerWeapon(int handle);
extern bool qObjIsEnergyWeapon(int handle);
extern bool qObjIsType(int objhandle, int type);
extern bool qObjCanSeeObj(int handletarget, int cone, int handlesrc);
extern bool qObjCanSeeObjAdvanced(int handletarget, int cone, int handlesrc, int fvi_flags);
extern int qObjParent(int objhandle);
extern int qGetAtachedChild(int objhandle, int attachpoint);
extern int qObjRoom(int objref);
extern bool qObjOnTerrain(int objhandle);
extern int qObjType(int objref);
extern float qUserVarValue(int varnum);
extern int qUserVarValueInt(int varnum);
extern bool qUserFlag(int flagnum);
extern float qObjShields(int objref);
extern float qObjEnergy(int objref);
extern float qObjShieldsOriginal(int objref);
extern bool qDoorLocked(int objref);
extern bool qDoorOpenable(int door_handle, int opener_handle);
extern float qDoorGetPos(int objhandle);
extern bool aMissionGetFlag(int flagnum);
extern bool aMissionGetLevelFlag(int flagnum);
extern float qObjAnimFrame(int objref);
extern bool qRoomFogOn(int roomnum);
extern void aTogglePlayerObjControl(int enable, int controlmask, int objhandle);
extern void aTogglePlayerObjAllControls(int enable, int objhandle);
extern void aObjectPlayerGiveKey(int player_handle, int key_handle, int key_num, const char *key_name, int show_on_hud);
extern void aObjectPlayerGiveInvisibleKey(int player_handle, int key_num);
extern float qObjGetLightingDist(int objhandle);
extern float qMathAddFloat(float f0, float f1);
extern float qMathSubFloat(float f0, float f1);
extern float qMathMulFloat(float f0, float f1);
extern float qMathIntToFloat(int i);
extern float qMathAddPercent(float f0, float f1);
extern float qMathSubPercent(float f0, float f1);
extern int qMathAddInt(int f0, int f1);
extern int qMathSubInt(int f0, int f1);
extern float qMathPercentage(float f0, float f1);
extern float qObjGetDistance(int objhandle0, int objhandle1);
extern bool qPortalIsOn(int portalnum, int roomnum);
extern bool qRandomChance(float prob);
extern float qRandomValue(float low, float high);
extern bool qObjCanSeePlayer(int cone, int handlesrc, float max_distance);
extern bool qObjCanSeePlayerAdvanced(int cone, int handlesrc, float max_distance, int fvi_flags);
extern bool qObjCanSeePlayerAdvancedWithStore(int slot, int cone, int handlesrc, float max_distance, int fvi_flags);
extern bool qRoomHasPlayer(int roomnum);
extern bool qIsShipEnabled(const char *Ship);
extern float qRoomGetDamage(int roomnum);
extern int qObjSavedHandle(int slot);
extern bool qTriggerGetState(int trigger_num);
extern int qObjCountTypeID(int type, const char *idname);

/*
$$ACTION
Misc
Show HUD message [s:Message] [f:Float]
aShowHUDMessageF
Show HUD message
  Shows a HUD message for all players with a float parameter

Parameters:
  Message:  The message to show, with %f marking where the number should go
$$END
*/
#define aShowHUDMessageF aShowHUDMessage

/*
$$ACTION
Misc
Show HUD message [s:Message] for player [o:PlayerObject=IT] with parm [f:Float]
aShowHUDMessageObjF
Show HUD message to one player
  Shows a HUD message for one players with a float parameter

Parameters:
  Message:  The message to show, with %f marking where the number should go
  PlayerObject: the player who sees the message
  Float: the number to show
$$END
*/
#define aShowHUDMessageObjF aShowHUDMessageObj

/*
$$ACTION
Misc
Show HUD message [s:Message] [i:Integer]
aShowHUDMessageI
Show HUD message
  Shows a HUD message for all players with an integer parameter

Parameters:
  Message:  The message to show, with %d marking where the number should go
$$END
*/
#define aShowHUDMessageI aShowHUDMessage

extern bool qGoalPrimariesComplete();
extern bool qGoalEnabled(int goal_index);
extern bool qGoalCompleted(int goal_index);
extern bool qGoalFailed(int goal_index);
extern int qPlayerClosest(int objhandle, int varnum);
extern float qObjDamage(int handle);
extern float qFrametime();
extern float qAIQueryMaxSpeed(int objhandle);
