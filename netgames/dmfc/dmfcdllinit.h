/*
* $Logfile: /DescentIII/Main/Dmfc/dmfcdllinit.h $
* $Revision: 1.1.1.1 $
* $Date: 2003/08/26 03:57:20 $
* $Author: kevinb $
*
* Function and variable initialization
*
* $Log: dmfcdllinit.h,v $
* Revision 1.1.1.1  2003/08/26 03:57:20  kevinb
* initial 1.5 import
*
 * 
 * 64    9/28/99 10:55a Jeff
 * changed size of pointer for mastertracker game from ubyte* to int*
 * 
 * 63    8/15/99 4:36p Jeff
 * finished exporting all inventory class functions.  export object_info
 * array. added check for -nooutragelogo to display Outrage logo display.
 * 
 * 62    8/11/99 1:22p Jeff
 * exported needed functions for camera windows
 * 
 * 61    7/20/99 11:26a Jeff
 * fire weapon functions
 * 
 * 60    7/11/99 3:31p Jeff
 * exported game arguments, made command line option to specify
 * autoexec.dmfc
 * 
 * 59    5/22/99 1:12a Jeff
 * correctly handle Viewer_object
 * 
 * 58    5/20/99 5:31p Jeff
 * exported PlayerStopSounds
 * 
 * 57    5/19/99 11:49p Jeff
 * final preparations for dmfc version 1.0
 * 
 * 56    5/19/99 5:26p Jeff
 * fixed some exported functions.  Removed no longer used functions
 * 
 * 55    5/03/99 8:39a Jeff
 * tons of Entropy fixes
 * 
 * 54    4/30/99 10:53p Jeff
 * added $warp command
 * 
 * 53    4/30/99 7:36p Jeff
 * exported vis_effects to dmfc
 * 
 * 52    3/30/99 9:01p Jeff
 * exported polymodels
 * 
 * 51    3/27/99 4:53p Jeff
 * player rankings in multiplayer games implemented.  Fixed join message
 * so it doesn't get cut off
 * 
 * 50    3/22/99 6:20p Jeff
 * added 2 more audio taunts.  a mulitplayer event when someone plays an
 * audio taunt.  option to disable audio taunts.
 * 
 * 49    3/17/99 12:23p Jeff
 * converted DMFC to be COM interface
 * 
 * 48    3/12/99 7:40p Jeff
 * removed enhanced objcreate, added enhanced objsetpos
 * 
 * 47    3/11/99 6:30p Jeff
 * numerous fixes to demo system in multiplayer games (when
 * recording/playback a demo in a multiplayer game)
 * 
 * 46    2/25/99 8:54p Jeff
 * Inventory supports level change persistant items.  Inventory supports
 * time-out objects.  Inventory Reset changed (takes a level of reset
 * now).  Quad lasers stay across level change (single player).  Guidebot
 * bug fixed (now back in ship on level start).  Quads time out when
 * spewed.  Invulnerability and cloak powerups no longer use game
 * event/callbacks, so they can be saved in game saves (moved to
 * MakePlayerInvulnerable and MakeObjectInvisible)
 * 
 * 45    2/11/99 12:50a Jeff
 * changed names of exported variables
 * 
 * 43    2/08/99 5:22p Jeff
 * exported rend_SetZBuffer...fixed up small view of stats
 * 
 * 42    2/08/99 12:05a Jeff
 * exported some new functions and variables
 * 
 * 41    2/06/99 6:59p Jeff
 * created RenderHUDGetTextLineWidth and RenderHUDGetTextHeight
 * 
 * 40    1/31/99 7:26p Matt
 * Renamed a bunch of functions to have HUD capitalized
 * 
 * 39    1/21/99 11:16p Jeff
 * exported vecmat functions
 * 
 * 38    1/19/99 5:34p Jeff
 * updated monsterball
 * 
 * 37    1/15/99 8:29p Jeff
 * updates to powerball
 * 
 * 36    1/15/99 3:54a Jeff
 * exported a couple more functions.  Added event handlers for Weapon
 * collide event
 * 
 * 35    1/12/99 2:55p Jeff
 * added/finished the waiting for player's to join dialog
 * 
 * 34    1/07/99 5:01p Jeff
 * added Int3's and updated all net games to use stats manager...correctly
 * too
 * 
 * 33    1/04/99 2:19p Jeff
 * exported table file management functions
 * 
 * 32    1/04/99 12:21p Jeff
 * added support for hosts.allow/deny and updates stats manager a little
 * 
 * 31    12/08/98 3:28p Jeff
 * created and exported UI Checkbox wrappers
 * 
 * 30    12/08/98 12:17p Jeff
 * various changes that include an improved Team Control dialog (doesn't
 * switch teams until exit..) and spew/respawn players that change teams
 * 
 * 29    12/04/98 7:04p Jeff
 * almost finished up dmfc stat manager
 * 
 * 28    12/01/98 6:56p Jeff
 * put in quick and dirty implementation of pilot pics for testing
 * 
 * 27    11/19/98 5:56p Jeff
 * added slider exported and improved Hoard
 * 
 * 26    11/09/98 11:54a Jeff
 * Added Player_colors
 * 
 * 25    10/29/98 7:01p Jeff
 * creation of team placement dialog.  Moved TranslateEvent into DMFC
 * 
 * 24    10/18/98 7:59p Jeff
 * functions added to dmfc for client->server objnum matching.  Banning
 * now uses tracker id when available.
 * 
 * 23    10/13/98 6:01p Jeff
 * added attaching
 * 
 * 22    10/08/98 3:37p Jeff
 * general improvements (Netgame info things, save to registry).  Changes
 * so it would send packets on NETSEQ_OBJECTS
 * 
 * 21    10/02/98 6:11p Jeff
 * 
 * 20    9/30/98 3:50p Jeff
 * general improvements (many)
 * 
 * 19    9/21/98 7:11p Jeff
 * made InputCommand interface API and moved existing input commands to
 * the interface.  Changed mprintf/ASSERT so they are valid in DMFC
*
* $NoKeywords: $
*/




//This file contains the DLLInit function that is to be used across all multiplayer
//DLLs.  So all changes can be done in one place.  This file should only be included where
//the DLLGameInit function should be.

	DLLGetGameAPI=(GetGameAPI_fp)api_func;
	DLLGetGameAPI (&API);

	Objects=(object *)API.objs;
	Rooms=(room *)API.rooms;
	Terrain_seg=(terrain_segment *)API.terrain;
	Players=(player *)API.players;
	Netgame=(netgame_info *)API.netgame;
	NetPlayers=(netplayer *)API.netplayers;
	Ships = (ship *)API.ships;
	Weapons = (weapon *)API.weapons;
	Current_mission = (tMission *)API.Current_mission;
	GameTextures = (texture *)API.GameTextures;
	GameVClips = (vclip *)API.GameVClips;

	int i = 0;

	// Do functions, variables
	// Note, these must match the ordering on the D3 side
	DLLAddHUDMessage=(AddHUDMessage_fp)API.fp[i++];
	DLLMultiSendClientExecuteDLL=(MultiSendClientExecuteDLL_fp)API.fp[i++];
	DLLFindObjectIDName=(FindObjectIDName_fp)API.fp[i++];
	DLLGetGoalRoomForTeam=(GetGoalRoomForTeam_fp)API.fp[i++];
	DLLSetMaxTeams=(SetMaxTeams_fp)API.fp[i++];
	DLLComputeRoomCenter=(ComputeRoomCenter_fp)API.fp[i++];
	DLLObjCreate=(ObjCreate_fp)API.fp[i++];
	DLLMultiSendObject=(MultiSendObject_fp)API.fp[i++];
	DLLMultiPaintGoalRooms=(MultiPaintGoalRooms_fp)API.fp[i++];
	DLLMultiSendSpecialPacket=(MultiSendSpecialPacket_fp)API.fp[i++];
	DLLIncTeamScore=(IncTeamScore_fp)API.fp[i++];
	DLLDebug_ConsolePrintf=(Debug_ConsolePrintf_fp)API.fp[i++];
	DLLObjSetPosNoMark=(ObjSetPosNoMark_fp)API.fp[i++];
	DLLInvCheckItem=(InvCheckItem_fp)API.fp[i++];
	DLLInvAddTypeID=(InvAddTypeID_fp)API.fp[i++];
	DLLInvRemove=(InvRemove_fp)API.fp[i++];
	DLLPlayerSetLighting=(PlayerSetLighting_fp)API.fp[i++];
	DLLFindShipName=(FindShipName_fp)API.fp[i++];
	DLLPlayerSetRotatingBall=(PlayerSetRotatingBall_fp)API.fp[i++];
	DLLPlayerChangeShip=(PlayerChangeShip_fp)API.fp[i++];
	DLLInvGetTypeIDCount = (InvGetTypeIDCount_fp)API.fp[i++];
	DLLPlay3dSound = (D3W_Play3dSound_fp)API.fp[i++];
	DLLFindSoundName = (FindSoundName_fp)API.fp[i++];
	DLLSpewCreate = (SpewCreate_fp)API.fp[i++];
	DLLSpewClearEvent = (SpewClearEvent_fp)API.fp[i++];
	DLLbm_AllocLoadFileBitmap=(bm_AllocLoadFileBitmap_fp)API.fp[i++];
	DLLbm_FreeBitmap=(bm_FreeBitmap_fp)API.fp[i++];
	DLLrend_DrawScaledBitmap=(rend_DrawScaledBitmap_fp)API.fp[i++];
	DLLgrtext_Printf=(grtext_Printf_fp)API.fp[i++];
	DLLgrtext_Flush=(grtext_Flush_fp)API.fp[i++];
	DLLgrtext_SetColor=(grtext_SetColor_fp)API.fp[i++];
	DLLgrtext_SetFancyColor=(grtext_SetFancyColor_fp)API.fp[i++];
	DLLgrtext_SetAlpha=(grtext_SetAlpha_fp)API.fp[i++];
	DLLgrtext_GetAlpha=(grtext_GetAlpha_fp)API.fp[i++];
	DLLgrtext_SetFont=(grtext_SetFont_fp)API.fp[i++];
	DLLgrtext_GetFont=(grtext_GetFont_fp)API.fp[i++];
	DLLgrtext_GetTextLineWidth=(grtext_GetTextLineWidth_fp)API.fp[i++];
	DLLgrfont_GetHeight=(grfont_GetHeight_fp)API.fp[i++];
	DLLgrtext_CenteredPrintf=(grtext_CenteredPrintf_fp)API.fp[i++];
	DLLAddColoredHUDMessage=(AddColoredHUDMessage_fp)API.fp[i++];
	DLLbm_w=(bm_w_fp)API.fp[i++];
	DLLbm_h=(bm_h_fp)API.fp[i++];
	DLLrend_DrawSimpleBitmap=(rend_DrawSimpleBitmap_fp)API.fp[i++];
	DLLMultiClientSendSpecialPacket=(MultiClientSendSpecialPacket_fp)API.fp[i++];
	DLLAddBlinkingHUDMessage = (AddBlinkingHUDMessage_fp)API.fp[i++];
	DLLInvReset = (InvReset_fp)API.fp[i++];
	DLLAddHUDItem = (AddHUDItem_fp)API.fp[i++];
	DLLRenderHUDQuad = (RenderHUDQuad_fp)API.fp[i++];
	DLLRenderHUDText = (RenderHUDText_fp)API.fp[i++];
	DLLMultiEndLevel = (MultiEndLevel_fp)API.fp[i++];
	DLLbm_data = (bm_data_fp)API.fp[i++];
	DLLbm_AllocBitmap = (bm_AllocBitmap_fp)API.fp[i++];
	DLLrend_FillRect = (rend_FillRect_fp)API.fp[i++];
	DLLbm_CreateChunkedBitmap = (bm_CreateChunkedBitmap_fp)API.fp[i++];
	DLLbm_DestroyChunkedBitmap = (bm_DestroyChunkedBitmap_fp)API.fp[i++];
	DLLrend_DrawChunkedBitmap = (rend_DrawChunkedBitmap_fp)API.fp[i++];
	DLLrend_DrawScaledChunkedBitmap = (rend_DrawScaledChunkedBitmap_fp)API.fp[i++];
	DLLOpenCFILE = (OpenCFILE_fp)API.fp[i++];
	DLLcfclose = (cfclose_fp)API.fp[i++];
	DLLcfeof = (cfeof_fp)API.fp[i++];
	DLLcfexist = (cfexist_fp)API.fp[i++];
	DLLcf_ReadBytes = (cf_ReadBytes_fp)API.fp[i++];
	DLLcf_ReadInt = (cf_ReadInt_fp)API.fp[i++];
	DLLcf_ReadShort = (cf_ReadShort_fp)API.fp[i++];
	DLLcf_ReadByte = (cf_ReadByte_fp)API.fp[i++];
	DLLcf_ReadFloat = (cf_ReadFloat_fp)API.fp[i++];
	DLLcf_ReadDouble = (cf_ReadDouble_fp)API.fp[i++];
	DLLcf_ReadString = (cf_ReadString_fp)API.fp[i++];
	DLLcf_WriteBytes = (cf_WriteBytes_fp)API.fp[i++];
	DLLcf_WriteString = (cf_WriteString_fp)API.fp[i++];
	DLLcf_WriteInt = (cf_WriteInt_fp)API.fp[i++];
	DLLcf_WriteShort = (cf_WriteShort_fp)API.fp[i++];
	DLLcf_WriteByte = (cf_WriteByte_fp)API.fp[i++];
	DLLcf_WriteFloat = (cf_WriteFloat_fp)API.fp[i++];
	DLLcf_WriteDouble = (cf_WriteDouble_fp)API.fp[i++];
	DLLcf_CopyFile = (cf_CopyFile_fp)API.fp[i++];
	DLLcf_Diff = (cf_Diff_fp)API.fp[i++];
	DLLMultiDisconnectPlayer = (MultiDisconnectPlayer_fp)API.fp[i++];
	DLLnw_GetNumbersFromHostAddress = (nw_GetNumbersFromHostAddress_fp)API.fp[i++];
	DLLnw_GetThisIP = (nw_GetThisIP_fp)API.fp[i++];
	DLLCreateStringTable = (CreateStringTable_fp)API.fp[i++];
	DLLDestroyStringTable = (DestroyStringTable_fp)API.fp[i++];
	DLLRenderHUDTextFlags = (RenderHUDTextFlags_fp)API.fp[i++];
	DLLPlayerSpewInventory = (PlayerSpewInventory_fp)API.fp[i++];
	DLLPlayerSetHUDNameFOV = (PlayerSetHUDNameFOV_fp)API.fp[i++];
	DLLGetUltimateParentForObject = (GetUltimateParentForObject_fp)API.fp[i++];
	DLLSetObjectDeadFlagRaw = (SetObjectDeadFlagRaw_fp)API.fp[i++];
	FatalError = (DLLFatalError_fp)API.fp[i++];
	DLLassert = (assertdll_fp)API.fp[i++];
	DLLMultiMatchWeapon = (MultiMatchWeapon_fp)API.fp[i++];
	DLLMultiGetMatchChecksum = (MultiGetMatchChecksum_fp)API.fp[i++];
	DLLFindWeaponName = (FindWeaponName_fp)API.fp[i++];
	DLLcf_OpenLibrary = (cf_OpenLibrary_fp)API.fp[i++];
	DLLcf_CloseLibrary = (cf_CloseLibrary_fp)API.fp[i++];
	DLLMultiSendRequestToObserve = (MultiSendRequestToObserve_fp)API.fp[i++];
	DLLFindTextureName = (FindTextureName_fp)API.fp[i++];
	DLLApplyDamageToPlayer = (ApplyDamageToPlayer_fp)API.fp[i++];
	DLLMultiMatchGeneric = (MultiMatchGeneric_fp)API.fp[i++];
	DLLSetUITextItemText = (SetUITextItemText_fp)API.fp[i++];
	DLLNewUIWindowCreate = (NewUIWindowCreate_fp)API.fp[i++];
	DLLNewUIWindowDestroy= (NewUIWindowDestroy_fp)API.fp[i++];
	DLLNewUIWindowOpen = (NewUIWindowOpen_fp)API.fp[i++];
	DLLNewUIWindowClose = (NewUIWindowClose_fp)API.fp[i++];
	DLLTextCreate = (TextCreate_fp)API.fp[i++];
	DLLEditCreate = (EditCreate_fp)API.fp[i++];
	DLLButtonCreate = (ButtonCreate_fp)API.fp[i++];
	DLLListCreate = (ListCreate_fp)API.fp[i++];
	DLLListRemoveAll = (ListRemoveAll_fp)API.fp[i++];
	DLLListAddItem = (ListAddItem_fp)API.fp[i++];
	DLLListRemoveItem = (ListRemoveItem_fp)API.fp[i++];
	DLLListSelectItem = (ListSelectItem_fp)API.fp[i++];
	DLLListGetItem = (ListGetItem_fp)API.fp[i++];
	DLLListGetSelectedIndex = (ListGetSelectedIndex_fp)API.fp[i++];
	DLLEditSetText = (EditSetText_fp)API.fp[i++];
	DLLEditGetText = (EditGetText_fp)API.fp[i++];
	DLLDoUI = (DoUI_fp)API.fp[i++];
	DLLDoMessageBox = (DoMessageBox_fp)API.fp[i++];
	DLLDescentDefer = (DescentDefer_fp)API.fp[i++];
	DLLNewUIGameWindowCreate = (NewUIGameWindowCreate_fp)API.fp[i++];
	DLLNewUIGameWindowDestroy = (NewUIGameWindowDestroy_fp)API.fp[i++];
	DLLNewUIGameWindowOpen = (NewUIGameWindowOpen_fp)API.fp[i++];
	DLLNewUIGameWindowClose = (NewUIGameWindowClose_fp)API.fp[i++];
	DLLHotSpotCreate = (HotSpotCreate_fp)API.fp[i++];
	DLLPollUI = (PollUI_fp)API.fp[i++];
	DLLRemoveUITextItem = (RemoveUITextItem_fp)API.fp[i++];
	DLLCreateNewUITextItem = (CreateNewUITextItem_fp)API.fp[i++];
	DLLUIConsoleGadgetCreate = (UIConsoleGadgetCreate_fp)API.fp[i++];
	DLLUIConsoleGadgetputs = (UIConsoleGadgetputs_fp)API.fp[i++];
	DLLNewUIWindowSetFocusOnEditGadget = (NewUIWindowSetFocusOnEditGadget_fp)API.fp[i++];
	DLLOldEditCreate = (OldEditCreate_fp)API.fp[i++];
	DLLOldListCreate = (OldListCreate_fp)API.fp[i++];
	DLLOldListRemoveAll = (OldListRemoveAll_fp)API.fp[i++];
	DLLOldListAddItem = (OldListAddItem_fp)API.fp[i++];
	DLLOldListRemoveItem = (OldListRemoveItem_fp)API.fp[i++];
	DLLOldListSelectItem = (OldListSelectItem_fp)API.fp[i++];
	DLLOldListGetItem = (OldListGetItem_fp)API.fp[i++];
	DLLOldListGetSelectedIndex = (OldListGetSelectedIndex_fp)API.fp[i++];
	DLLOldEditSetText = (OldEditSetText_fp)API.fp[i++];
	DLLOldEditGetText = (OldEditGetText_fp)API.fp[i++];
	DLLToggleUICallback = (ToggleUICallback_fp)API.fp[i++];
	DLLSetOldEditBufferLen = (SetOldEditBufferLen_fp)API.fp[i++];
	DLLNewUIWindowLoadBackgroundImage = (NewUIWindowLoadBackgroundImage_fp)API.fp[i++];
	DLLDeleteUIItem = (DeleteUIItem_fp)API.fp[i++];
	DLLHotSpotSetStates = (HotSpotSetStates_fp)API.fp[i++];
	DLLSetUICallback = (SetUICallback_fp)API.fp[i++];
	DLLGetUICallback = (RetrieveUICallback_fp)API.fp[i++];
	DLLSuspendControls = (SuspendControls_fp)API.fp[i++];
	DLLResumeControls = (ResumeControls_fp)API.fp[i++];
	DLLShowCursor = (ui_ShowCursor_fp)API.fp[i++];
	DLLHideCursor = (ui_HideCursor_fp)API.fp[i++];
	DLLGameFrame = (GameFrame_fp)API.fp[i++];
	DPrintf = (DPrintf_fp)API.fp[i++];
	DLLddio_MakePath = (ddio_MakePath_fp)API.fp[i++];
	DLLddio_SplitPath = (ddio_SplitPath_fp)API.fp[i++];
	DLLPlay2dSound = (Play2dSound_fp)API.fp[i++];
	DLLTouchSound = (TouchSound_fp)API.fp[i++];
	DatabaseRead1 = (dDatabaseRead_fp1)API.fp[i++];
	DatabaseRead2 = (dDatabaseRead_fp2)API.fp[i++];
	DatabaseRead3 = (dDatabaseRead_fp3)API.fp[i++];
	DatabaseWrite1 = (dDatabaseWrite_fp1)API.fp[i++];
	DatabaseWrite2 = (dDatabaseWrite_fp2)API.fp[i++];
	DLLAttachObject = (AttachObject_fp)API.fp[i++];
	DLLObjGet = (ObjGet_fp)API.fp[i++];
	DLLListSetSelectedIndex = (ListSetSelectedIndex_fp)API.fp[i++];
	DLLRemoveUIBmpItem = (RemoveUIBmpItem_fp)API.fp[i++];
	DLLCreateNewUIBmpItem = (CreateNewUIBmpItem_fp)API.fp[i++];
	DLLGetUIItemWidth = (GetUIItemWidth_fp)API.fp[i++];
	DLLGetUIItemHeight = (GetUIItemHeight_fp)API.fp[i++];
	DLLSliderCreate = (SliderCreate_fp)API.fp[i++];
	DLLSliderSetRange = (SliderSetRange_fp)API.fp[i++];
	DLLSliderGetRange = (SliderGetRange_fp)API.fp[i++];
	DLLSliderSetPos = (SliderSetPos_fp)API.fp[i++];
	DLLSliderGetPos = (SliderGetPos_fp)API.fp[i++];
	DLLSliderSetSelectChangeCallback = (SliderSetSelectChangeCallback_fp)API.fp[i++];
	DLLSliderSetSelectChangeCallbackWData = (SliderSetSelectChangeCallbackWData_fp)API.fp[i++];
	DLLTextSetTitle = (TextSetTitle_fp)API.fp[i++];
	DLLPPic_GetPilot = (PPic_GetPilot_fp)API.fp[i++];
	DLLPPic_GetBitmapHandle = (PPic_GetBitmapHandle_fp)API.fp[i++];
	DLLrend_DrawLine = (rend_DrawLine_fp)API.fp[i++];
	DLLrend_SetFlatColor = (rend_SetFlatColor_fp)API.fp[i++];
	DLLMultiSetLogoState = (MultiSetLogoState_fp)API.fp[i++];
	DLLPlayerGetRandomStartPosition = (PlayerGetRandomStartPosition_fp)API.fp[i++];
	DLLInitPlayerNewShip = (InitPlayerNewShip_fp)API.fp[i++];
	DLLCheckBoxCreate = (CheckBoxCreate_fp)API.fp[i++];
	DLLCheckBoxSetCheck = (CheckBoxSetCheck_fp)API.fp[i++];
	DLLCheckBoxIsChecked = (CheckBoxIsChecked_fp)API.fp[i++];
	DLLnw_GetHostAddressFromNumbers	= (nw_GetHostAddressFromNumbers_fp)API.fp[i++];
	TableFilesClear = (TableFilesClear_fp)API.fp[i++];
	TableFileAdd = (TableFileAdd_fp)API.fp[i++];
	DLLDebugBreak_callback_stop = (DebugBreak_callback_stop_fp)API.fp[i++];
	DLLDebugBreak_callback_resume = (DebugBreak_callback_resume_fp)API.fp[i++];
	DLLInt3MessageBox = (Int3MessageBox_fp)API.fp[i++];
	DLLGetUIItemPosition = (GetUIItemPosition_fp)API.fp[i++];
	DLLAttachObjectRadius = (AttachObjectRadius_fp)API.fp[i++];
	DLLUnattachChildren = (UnattachChildren_fp)API.fp[i++];
	DLLUnattachChild = (UnattachChild_fp)API.fp[i++];
	DLLUnattachFromParent = (UnattachFromParent_fp)API.fp[i++];
	DLLvm_GetMagnitude = (vm_GetMagnitude_fp)API.fp[i++];
	DLLvm_MatrixMulVector = (vm_MatrixMulVector_fp)API.fp[i++];
	DLLphys_apply_force = (phys_apply_force_fp)API.fp[i++];
	DLLphys_apply_rot = (phys_apply_rot_fp)API.fp[i++];
	DLLvm_TransposeMatrix = (vm_TransposeMatrix_fp)API.fp[i++];
	DLLvm_CrossProduct = (vm_CrossProduct_fp)API.fp[i++];
	DLLvm_NormalizeVector = (vm_NormalizeVector_fp)API.fp[i++];
	DLLConvertEulerToAxisAmount = (ConvertEulerToAxisAmount_fp)API.fp[i++];
	DLLConvertAxisAmountToEuler = (ConvertAxisAmountToEuler_fp)API.fp[i++];
	DLLvm_GetMagnitudeFast = (vm_GetMagnitudeFast_fp)API.fp[i++];
	DLLvm_MakeIdentity = (vm_MakeIdentity_fp)API.fp[i++];
	DLLvm_MakeVectorZero = (vm_MakeVectorZero_fp)API.fp[i++];
	DLLvm_MakeAngleZero = (vm_MakeAngleZero_fp)API.fp[i++];
	DLLvm_VectorMulTMatrix = (vm_VectorMulTMatrix_fp)API.fp[i++];
	DLLvm_MatrixMul = (vm_MatrixMul_fp)API.fp[i++];
	DLLvm_MatrixMulTMatrix = (vm_MatrixMulTMatrix_fp)API.fp[i++];
	DLLvm_DotProduct = (vm_DotProduct_fp)API.fp[i++];
	DLLvm_SubVectors = (vm_SubVectors_fp)API.fp[i++];
	DLLvm_AddVectors = (vm_AddVectors_fp)API.fp[i++];
	DLLvm_AverageVector = (vm_AverageVector_fp)API.fp[i++];
	DLLvm_ScaleVector = (vm_ScaleVector_fp)API.fp[i++];
	DLLvm_ScaleAddVector = (vm_ScaleAddVector_fp)API.fp[i++];
	DLLvm_DivVector = (vm_DivVector_fp)API.fp[i++];
	DLLvm_NormalizeVectorFast = (vm_NormalizeVectorFast_fp)API.fp[i++];
	DLLvm_ClearMatrix = (vm_ClearMatrix_fp)API.fp[i++];
	DLLvm_AnglesToMatrix = (vm_AnglesToMatrix_fp)API.fp[i++];
	DLLvm_Orthogonalize = (vm_Orthogonalize_fp)API.fp[i++];
	DLLvm_VectorToMatrix = (vm_VectorToMatrix_fp)API.fp[i++];
	DLLvm_VectorAngleToMatrix = (vm_VectorAngleToMatrix_fp)API.fp[i++];
	DLLvm_SinCos = (vm_SinCos_fp)API.fp[i++];
	DLLvm_GetSlope = (vm_GetSlope_fp)API.fp[i++];
	DLLvm_GetPerp = (vm_GetPerp_fp)API.fp[i++];
	DLLvm_GetNormal = (vm_GetNormal_fp)API.fp[i++];
	DLLvm_VectorDistance = (vm_VectorDistance_fp)API.fp[i++];
	DLLvm_VectorDistanceQuick = (vm_VectorDistanceQuick_fp)API.fp[i++];
	DLLvm_GetNormalizedDir = (vm_GetNormalizedDir_fp)API.fp[i++];
	DLLvm_GetNormalizedDirFast = (vm_GetNormalizedDirFast_fp)API.fp[i++];
	DLLvm_ExtractAnglesFromMatrix = (vm_ExtractAnglesFromMatrix_fp)API.fp[i++];
	DLLvm_DeltaAngVec = (vm_DeltaAngVec_fp)API.fp[i++];
	DLLvm_DeltaAngVecNorm = (vm_DeltaAngVecNorm_fp)API.fp[i++];
	DLLvm_DistToPlane = (vm_DistToPlane_fp)API.fp[i++];
	DLLvm_CalcDetValue = (vm_CalcDetValue_fp)API.fp[i++];
	DLLvm_MakeInverseMatrix = (vm_MakeInverseMatrix_fp)API.fp[i++];
	DLLvm_SinCosToMatrix = (vm_SinCosToMatrix_fp)API.fp[i++];
	DLLvm_GetCentroid = (vm_GetCentroid_fp)API.fp[i++];
	DLLvm_MakeRandomVector = (vm_MakeRandomVector_fp)API.fp[i++];
	DLLvm_ComputeBoundingSphere = (vm_ComputeBoundingSphere_fp)API.fp[i++];
	DLLvm_GetCentroidFast = (vm_GetCentroidFast_fp)API.fp[i++];
	DLLRenderHUDGetTextLineWidth = (RenderHUDGetTextLineWidth_fp)API.fp[i++];
	DLLRenderHUDGetTextHeight = (RenderHUDGetTextHeight_fp)API.fp[i++];
	DLLStartFrame = (StartFrame_fp)API.fp[i++];
	DLLEndFrame = (EndFrame_fp)API.fp[i++];
	DLLResetFacings = (ResetFacings_fp)API.fp[i++];
	DLLGameRenderWorld = (GameRenderWorld_fp)API.fp[i++];
	DLLGetFrameParameters = (GetFrameParameters_fp)API.fp[i++];
	DLLrend_SetZBufferState = (rend_SetZBufferState_fp)API.fp[i++];
	DLLrend_SetLighting = (rend_SetLighting_fp)API.fp[i++];
	DLLrend_SetColorModel = (rend_SetColorModel_fp)API.fp[i++];
	DLLrend_SetTextureType = (rend_SetTextureType_fp)API.fp[i++];
	DLLrend_DrawPolygon = (rend_DrawPolygon_fp)API.fp[i++];
	DLLrend_SetMipState = (rend_SetMipState_fp)API.fp[i++];
	DLLrend_SetFogState = (rend_SetFogState_fp)API.fp[i++];
	DLLrend_SetFiltering = (rend_SetFiltering_fp)API.fp[i++];
	DLLrend_SetOverlayMap = (rend_SetOverlayMap_fp)API.fp[i++];
	DLLrend_SetOverlayType = (rend_SetOverlayType_fp)API.fp[i++];
	DLLrend_ClearScreen = (rend_ClearScreen_fp)API.fp[i++];
	DLLrend_SetPixel = (rend_SetPixel_fp)API.fp[i++];
	DLLrend_GetPixel = (rend_GetPixel_fp)API.fp[i++];
	DLLrend_FillCircle = (rend_FillCircle_fp)API.fp[i++];
	DLLrend_DrawCircle = (rend_DrawCircle_fp)API.fp[i++];
	DLLrend_SetAlphaType = (rend_SetAlphaType_fp)API.fp[i++];
	DLLrend_SetAlphaValue = (rend_SetAlphaValue_fp)API.fp[i++];
	DLLrend_SetWrapType = (rend_SetWrapType_fp)API.fp[i++];
	DLLrend_SetZBias = (rend_SetZBias_fp)API.fp[i++];
	DLLrend_SetZBufferWriteMask = (rend_SetZBufferWriteMask_fp)API.fp[i++];
	DLLrend_GetLFBLock = (rend_GetLFBLock_fp)API.fp[i++];
	DLLrend_ReleaseLFBLock = (rend_ReleaseLFBLock_fp)API.fp[i++];
	DLLrend_DrawLFBBitmap = (rend_DrawLFBBitmap_fp)API.fp[i++];
	DLLrend_DrawSpecialLine = (rend_DrawSpecialLine_fp)API.fp[i++];
	DLLfvi_FindIntersection = (fvi_FindIntersection_fp)API.fp[i++];
	DLLfvi_QuickDistFaceList = (fvi_QuickDistFaceList_fp)API.fp[i++];
	DLLfvi_QuickDistCellList = (fvi_QuickDistCellList_fp)API.fp[i++];
	DLLfvi_QuickDistObjectList = (fvi_QuickDistObjectList_fp)API.fp[i++];
	DLLfvi_QuickRoomCheck = (fvi_QuickRoomCheck_fp)API.fp[i++];
	DLLObjSetPos = (ObjSetPos_fp)API.fp[i++];
	DLLSetObjectDeadFlag = (SetObjectDeadFlag_fp)API.fp[i++];
	DLLtaunt_AreEnabled = (taunt_AreEnabled_fp)API.fp[i++];
	DLLtaunt_Enable = (taunt_Enable_fp)API.fp[i++];
	GetPlayerRankIndex = (GetPlayerRankIndex_fp)API.fp[i++];
	DLLVisEffectAllocate = (VisEffectAllocate_fp)API.fp[i++];
	DLLVisEffectFree = (VisEffectFree_fp)API.fp[i++];
	DLLVisEffectInitType = (VisEffectInitType_fp)API.fp[i++];
	DLLVisEffectCreate = (VisEffectCreate_fp)API.fp[i++];
	DLLVisEffectLink = (VisEffectLink_fp)API.fp[i++];
	DLLVisEffectUnlink = (VisEffectUnlink_fp)API.fp[i++];
	DLLVisEffectRelink = (VisEffectRelink_fp)API.fp[i++];
	DLLVisEffectDelete = (VisEffectDelete_fp)API.fp[i++];
	DLLCreateRandomSparks = (CreateRandomSparks_fp)API.fp[i++];
	DLLCreateRandomLineSparks = (CreateRandomLineSparks_fp)API.fp[i++];
	DLLVisEffectCreateControlled = (VisEffectCreateControlled_fp)API.fp[i++];
	DLLCreateRandomParticles = (CreateRandomParticles_fp)API.fp[i++];
	DLLAttachRandomNapalmEffectsToObject = (AttachRandomNapalmEffectsToObject_fp)API.fp[i++];
	DLLInitObjectScripts = (InitObjectScripts_fp)API.fp[i++];
	DLLg3_StartFrame = (g3_StartFrame_fp)API.fp[i++];
	DLLg3_EndFrame = (g3_EndFrame_fp)API.fp[i++];
	DLLg3_GetViewPosition = (g3_GetViewPosition_fp)API.fp[i++];
	DLLg3_GetViewMatrix = (g3_GetViewMatrix_fp)API.fp[i++];
	DLLg3_GetUnscaledMatrix = (g3_GetUnscaledMatrix_fp)API.fp[i++];
	DLLg3_StartInstanceMatrix = (g3_StartInstanceMatrix_fp)API.fp[i++];
	DLLg3_StartInstanceAngles = (g3_StartInstanceAngles_fp)API.fp[i++];
	DLLg3_DoneInstance = (g3_DoneInstance_fp)API.fp[i++];
	DLLg3_CheckNormalFacing = (g3_CheckNormalFacing_fp)API.fp[i++];
	DLLg3_RotatePoint = (g3_RotatePoint_fp)API.fp[i++];
	DLLg3_ProjectPoint = (g3_ProjectPoint_fp)API.fp[i++];
	DLLg3_CalcPointDepth = (g3_CalcPointDepth_fp)API.fp[i++];
	DLLg3_Point2Vec = (g3_Point2Vec_fp)API.fp[i++];
	DLLg3_CodePoint = (g3_CodePoint_fp)API.fp[i++];
	DLLg3_RotateDeltaX = (g3_RotateDeltaX_fp)API.fp[i++];
	DLLg3_RotateDeltaY = (g3_RotateDeltaY_fp)API.fp[i++];
	DLLg3_RotateDeltaZ = (g3_RotateDeltaZ_fp)API.fp[i++];
	DLLg3_RotateDeltaVec = (g3_RotateDeltaVec_fp)API.fp[i++];
	DLLg3_AddDeltaVec = (g3_AddDeltaVec_fp)API.fp[i++];
	DLLg3_DrawPoly = (g3_DrawPoly_fp)API.fp[i++];
	DLLg3_DrawSphere = (g3_DrawSphere_fp)API.fp[i++];
	DLLg3_CheckAndDrawPoly = (g3_CheckAndDrawPoly_fp)API.fp[i++];
	DLLg3_DrawLine = (g3_DrawLine_fp)API.fp[i++];
	DLLg3_DrawBitmap = (g3_DrawBitmap_fp)API.fp[i++];
	DLLg3_DrawRotatedBitmap = (g3_DrawRotatedBitmap_fp)API.fp[i++];
	DLLg3_DrawBox = (g3_DrawBox_fp)API.fp[i++];
	DLLg3_SetCustomClipPlane = (g3_SetCustomClipPlane_fp)API.fp[i++];
	DLLg3_SetFarClipZ = (g3_SetFarClipZ_fp)API.fp[i++];
	DLLg3_ClipPolygon = (g3_ClipPolygon_fp)API.fp[i++];
	DLLg3_FreeTempPoints = (g3_FreeTempPoints_fp)API.fp[i++];
	DLLg3_GetMatrixScale = (g3_GetMatrixScale_fp)API.fp[i++];
	DLLg3_SetTriangulationTest = (g3_SetTriangulationTest_fp)API.fp[i++];
	DLLg3_DrawSpecialLine = (g3_DrawSpecialLine_fp)API.fp[i++];
	DLLg3_DrawPlanarRotatedBitmap = (g3_DrawPlanarRotatedBitmap_fp)API.fp[i++];
	DLLPlayerStopSounds = (PlayerStopSounds_fp)API.fp[i++];
	DLLFindArg = (FindArg_fp)API.fp[i++];
	DLLFireWeaponFromObject = (FireWeaponFromObject_fp)API.fp[i++];
	DLLCreateAndFireWeapon = (CreateAndFireWeapon_fp)API.fp[i++];
	DLLSelectNextCameraView = (SelectNextCameraView_fp)API.fp[i++];
	Inven_Add = (dInven_Add_fp)API.fp[i++];
	Inven_AddObject = (dInven_AddObject_fp)API.fp[i++];
	Inven_AddCounterMeasure = (dInven_AddCounterMeasure_fp)API.fp[i++];
	Inven_Remove = (dInven_Remove_fp)API.fp[i++];
	Inven_Use = (dInven_Use_fp)API.fp[i++];
	Inven_UseObjHandle = (dInven_UseObjHandle_fp)API.fp[i++];
	Inven_Size = (dInven_Size_fp)API.fp[i++];
	Inven_CheckItem = (dInven_CheckItem_fp)API.fp[i++];
	Inven_Reset = (dInven_Reset_fp)API.fp[i++];
	Inven_ResetPos = (dInven_ResetPos_fp)API.fp[i++];
	Inven_NextPos = (dInven_NextPos_fp)API.fp[i++];
	Inven_PrevPos = (dInven_PrevPos_fp)API.fp[i++];
	Inven_GetPosTypeID = (dInven_GetPosTypeID_fp)API.fp[i++];
	Inven_GetAuxPosTypeID = (dInven_GetAuxPosTypeID_fp)API.fp[i++];
	Inven_GetPosDescription = (dInven_GetPosDescription_fp)API.fp[i++];
	Inven_GetPosIconName = (dInven_GetPosIconName_fp)API.fp[i++];
	Inven_GetPosName = (dInven_GetPosName_fp)API.fp[i++];
	Inven_GetPosInfo = (dInven_GetPosInfo_fp)API.fp[i++];
	Inven_GetPosCount = (dInven_GetPosCount_fp)API.fp[i++];
	Inven_AtBeginning = (dInven_AtBeginning_fp)API.fp[i++];
	Inven_AtEnd = (dInven_AtEnd_fp)API.fp[i++];
	Inven_GotoPos = (dInven_GotoPos_fp)API.fp[i++];
	Inven_GotoPosTypeID = (dInven_GotoPosTypeID_fp)API.fp[i++];
	Inven_UsePos = (dInven_UsePos_fp)API.fp[i++];
	Inven_GetPos = (dInven_GetPos_fp)API.fp[i++];
	Inven_ValidatePos = (dInven_ValidatePos_fp)API.fp[i++];
	Inven_IsSelectable = (dInven_IsSelectable_fp)API.fp[i++];
	Inven_IsUsable = (dInven_IsUsable_fp)API.fp[i++];
	Inven_GetTypeIDCount = (dInven_GetTypeIDCount_fp)API.fp[i++];
	Inven_FindPos = (dInven_FindPos_fp)API.fp[i++];
	Inven_GetInventoryItemList = (dInven_GetInventoryItemList_fp)API.fp[i++];

	// Do variables
	Player_num=(int *)API.vp[0];
	Highest_room_index=(int *)API.vp[1];
	Game_window_w=(int *)API.vp[2];
	Game_window_h=(int *)API.vp[3];
	Game_fonts=(int *)API.vp[4];
	Frametime=(float *)API.vp[5];
	Gametime=(float *)API.vp[6];
	ShieldDelta=(float *)API.vp[7];
	Game_interface_mode = (int *)API.vp[8];
	LocalD3Dir = (char *)API.vp[9];
	m_bTrackerGame = (int *)API.vp[10];
	Local_object_list = (ushort *)API.vp[11];
	Server_object_list = (ushort *)API.vp[12];
	Dedicated_server = (bool *)API.vp[13];
	Player_colors = (ddgr_color *)API.vp[14];
	Hud_aspect_x = (float *)API.vp[15];
	Hud_aspect_y = (float *)API.vp[16];
	Viewer_object = (object **)API.vp[17];
	Render_zoom = (float *)API.vp[18];
	Game_window_x = (int *)API.vp[19];
	Game_window_y = (int *)API.vp[20];
	Poly_models = (poly_model *)API.vp[21];
	VisEffects = (vis_effect *)API.vp[22];
	Highest_vis_effect_index = (int *)API.vp[23];
	Multi_next_level = (int *)API.vp[24];
	Level_info = (level_info *)API.vp[25];
	GameArgs = (void *)API.vp[26];
	Camera_view_mode = (int *)API.vp[27];
	Object_info = (object_info *)API.vp[28];
