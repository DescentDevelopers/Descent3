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

#include "pstypes.h"
#include "pserror.h"
#include "game.h"
#include "game2dll.h"
#include "room.h"
#include "object.h"
#include "terrain.h"
#include "player.h"
#include "mono.h"
#include "hud.h"
#include "Inventory.h"
#include "multi_server.h"
#include "ship.h"
#include "DllWrappers.h"
#include "soundload.h"
#include "spew.h"
#include "objinfo.h"
#include "module.h"
#include "localization.h"
#include "weapon.h"
#include "voice.h"
#include "gametexture.h"
#include "Mission.h"
#include "damage.h"
#include "ui.h"
#include "newui.h"
#include "multi_dll_mgr.h"
#include "controls.h"
#include "gameloop.h"
#include "gamesequence.h"
#include "dedicated_server.h"
#include "attach.h"
#include "PilotPicsAPI.h"
#include "vclip.h"
#include "osiris_dll.h"
#include "manage.h"
#include "PHYSICS.H"
#include "collide.h"
#include "render.h"
#include "audiotaunts.h"
#include "polymodel.h"
#include "osiris_share.h"
#include "viseffect.h"
#include "ObjScript.h"
#include "args.h"

#include <algorithm>

void SelectNextCameraView(int window);
#define NUM_CAMERA_VIEWS 3
extern int Camera_view_mode[NUM_CAMERA_VIEWS];

//	Osiris_CreateModuleInitStruct
//	Purpose:
//		This function initializes a Module Init Struct with all the needed data to get sent
//	to the module during initialization.
extern void Osiris_CreateModuleInitStruct(tOSIRISModuleInit *mi);
module GameDLLHandle = {NULL};
extern ddgr_color Player_colors[];
struct game_api {
  int *objs;
  int *rooms;
  int *terrain;
  int *players;
  int *netgame;
  int *netplayers;
  int *ships;
  int *weapons;
  int *Current_mission;
  int *GameTextures;
  int *GameVClips;
  int *fp[450]; // function pointers
  int *vp[50];  // variable pointers
  tOSIRISModuleInit *osiris_functions;
};
// The exported DLL function call prototypes
#if defined(POSIX)
typedef void DLLFUNCCALL (*DLLGameCall_fp)(int eventnum, dllinfo *data);
typedef void DLLFUNCCALL (*DLLGameInit_fp)(int *api_func, uint8_t *all_ok, int num_teams_to_use);
typedef void DLLFUNCCALL (*DLLGameClose_fp)();
typedef void DLLFUNCCALL (*DLLGameGetName_fp)(char *buffer, int maxsize);
typedef void DLLFUNCCALL (*DLLGameGetDesc_fp)(char **buffer, int maxsize, int lines);
typedef void DLLFUNCCALL (*DLLGetGameInfo_fp)(tDLLOptions *options);
#else
typedef void(DLLFUNCCALL *DLLGameCall_fp)(int eventnum, dllinfo *data);
typedef void(DLLFUNCCALL *DLLGameInit_fp)(int *api_func, uint8_t *all_ok, int num_teams_to_use);
typedef void(DLLFUNCCALL *DLLGameClose_fp)();
typedef void(DLLFUNCCALL *DLLGameGetName_fp)(char *buffer, int maxsize);
typedef void(DLLFUNCCALL *DLLGameGetDesc_fp)(char **buffer, int maxsize, int lines);
typedef void(DLLFUNCCALL *DLLGetGameInfo_fp)(tDLLOptions *options);
#endif
DLLGameCall_fp DLLGameCall = NULL;
DLLGameInit_fp DLLGameInit = NULL;
DLLGameClose_fp DLLGameClose = NULL;
DLLGetGameInfo_fp DLLGetGameInfo = NULL;
dllinfo DLLInfo;
tOSIRISModuleInit Multi_d3m_osiris_funcs;
char Multi_game_dll_name[_MAX_PATH * 2];

static void DUMMYrend_DrawScaledBitmap(int x1, int y1, int x2, int y2, int bm, float u0, float v0, float u1, float v1,
                                       float zval, int color, float *alphas) {
  ASSERT(zval >= 0.0f && zval <= 1.0f);
  rend_DrawScaledBitmap(x1, y1, x2, y2, bm, u0, v0, u1, v1, color, alphas);
}

void GetGameAPI(game_api *api) {
  api->objs = (int *)Objects;
  api->rooms = (int *)Rooms;
  api->terrain = (int *)Terrain_seg;
  api->players = (int *)Players;
  api->netgame = (int *)&Netgame;
  api->netplayers = (int *)&NetPlayers;
  api->ships = (int *)&Ships;
  api->weapons = (int *)&Weapons;
  api->Current_mission = (int *)&Current_mission;
  api->GameTextures = (int *)GameTextures;
  api->GameVClips = (int *)GameVClips;

  // Fill in function pointers here.  The order here must match the order on the
  // DLL side (dmfcbase.cpp)

  api->fp[0] = (int *)AddHUDMessage;
  api->fp[1] = (int *)MultiSendClientExecuteDLL;
  api->fp[2] = (int *)FindObjectIDName;
  api->fp[3] = (int *)GetGoalRoomForTeam;
  api->fp[4] = (int *)SetMaxTeams;
  api->fp[5] = (int *)ComputeRoomCenter;
  api->fp[6] = (int *)ObjCreate;
  api->fp[7] = (int *)MultiSendObject;
  api->fp[8] = (int *)MultiPaintGoalRooms;
  api->fp[9] = (int *)MultiSendSpecialPacket;
  api->fp[10] = (int *)IncTeamScore;
  api->fp[11] = (int *)MonoPrintf;
  api->fp[12] = (int *)ObjSetPos;
  api->fp[13] = (int *)InvCheckItem;
  api->fp[14] = (int *)InvAddTypeID;
  api->fp[15] = (int *)InvRemove;
  api->fp[16] = (int *)PlayerSetLighting;
  api->fp[17] = (int *)FindShipName;
  api->fp[18] = (int *)PlayerSetRotatingBall;
  api->fp[19] = (int *)ChangePlayerShipIndex;
  api->fp[20] = (int *)InvGetTypeIDCount;
  api->fp[21] = (int *)D3W_Play3dSound;
  api->fp[22] = (int *)FindSoundName;
  api->fp[23] = (int *)SpewCreate;
  api->fp[24] = (int *)SpewClearEvent;
  api->fp[25] = (int *)bm_AllocLoadFileBitmap;
  api->fp[26] = (int *)bm_FreeBitmap;
  api->fp[27] = (int *)DUMMYrend_DrawScaledBitmap;
  api->fp[28] = (int *)grtext_Printf;
  api->fp[29] = (int *)grtext_Flush;
  api->fp[30] = (int *)grtext_SetColor;
  api->fp[31] = (int *)grtext_SetFancyColor;
  api->fp[32] = (int *)grtext_SetAlpha;
  api->fp[33] = (int *)grtext_GetAlpha;
  api->fp[34] = (int *)grtext_SetFont;
  api->fp[35] = (int *)grtext_GetFont;
  api->fp[36] = (int *)grtext_GetTextLineWidth;
  api->fp[37] = (int *)grfont_GetHeight;
  api->fp[38] = (int *)grtext_CenteredPrintf;
  api->fp[39] = (int *)AddColoredHUDMessage;
  api->fp[40] = (int *)bm_w;
  api->fp[41] = (int *)bm_h;
  api->fp[42] = (int *)rend_DrawSimpleBitmap;
  api->fp[43] = (int *)MultiClientSendSpecialPacket;
  api->fp[44] = (int *)AddBlinkingHUDMessage;
  api->fp[45] = (int *)InvReset;
  api->fp[46] = (int *)AddHUDItem;
  api->fp[47] = (int *)RenderHUDQuad;
  api->fp[48] = (int *)RenderHUDText;
  api->fp[49] = (int *)MultiEndLevel;
  api->fp[50] = (int *)bm_data;
  api->fp[51] = (int *)bm_AllocBitmap;
  api->fp[52] = (int *)rend_FillRect;
  api->fp[53] = (int *)bm_CreateChunkedBitmap;
  api->fp[54] = (int *)bm_DestroyChunkedBitmap;
  api->fp[55] = (int *)rend_DrawChunkedBitmap;
  api->fp[56] = (int *)rend_DrawScaledChunkedBitmap;
  api->fp[57] = (int *)OpenCFILE;
  api->fp[58] = (int *)cfclose;
  api->fp[59] = (int *)cfeof;
  api->fp[60] = (int *)cfexist;
  api->fp[61] = (int *)cf_ReadBytes;
  api->fp[62] = (int *)cf_ReadInt;
  api->fp[63] = (int *)cf_ReadShort;
  api->fp[64] = (int *)cf_ReadByte;
  api->fp[65] = (int *)cf_ReadFloat;
  api->fp[66] = (int *)cf_ReadDouble;
  api->fp[67] = (int *)cf_ReadString;
  api->fp[68] = (int *)cf_WriteBytes;
  api->fp[69] = (int *)cf_WriteString;
  api->fp[70] = (int *)cf_WriteInt;
  api->fp[71] = (int *)cf_WriteShort;
  api->fp[72] = (int *)cf_WriteByte;
  api->fp[73] = (int *)cf_WriteFloat;
  api->fp[74] = (int *)cf_WriteDouble;
  api->fp[75] = (int *)cf_CopyFile;
  api->fp[76] = (int *)cf_Diff;
  api->fp[77] = (int *)MultiDisconnectPlayer;
  api->fp[78] = (int *)nw_GetNumbersFromHostAddress;
  api->fp[79] = (int *)nw_GetThisIP;
  api->fp[80] = (int *)CreateStringTable;
  api->fp[81] = (int *)DestroyStringTable;
  api->fp[82] = (int *)RenderHUDTextFlags;
  api->fp[83] = (int *)PlayerSpewInventory;
  api->fp[84] = (int *)PlayerSetHUDNameFOV;
  api->fp[85] = (int *)GetUltimateParentForObject;
  api->fp[86] = (int *)SetObjectDeadFlagDLL;
  api->fp[87] = (int *)DLLFatalError;
  api->fp[88] = (int *)assertdll;
  api->fp[89] = (int *)MultiMatchWeapon;
  api->fp[90] = (int *)MultiGetMatchChecksum;
  api->fp[91] = (int *)FindWeaponName;
  api->fp[92] = (int *)cf_OpenLibrary;
  api->fp[93] = (int *)cf_CloseLibrary;
  api->fp[94] = (int *)MultiSendRequestToObserve;
  api->fp[95] = (int *)FindTextureName;
  api->fp[96] = (int *)ApplyDamageToPlayer;
  api->fp[97] = (int *)MultiMatchGeneric;
  api->fp[98] = (int *)SetUITextItemText;
  api->fp[99] = (int *)NewUIWindowCreate;
  api->fp[100] = (int *)NewUIWindowDestroy;
  api->fp[101] = (int *)NewUIWindowOpen;
  api->fp[102] = (int *)NewUIWindowClose;
  api->fp[103] = (int *)TextCreate;
  api->fp[104] = (int *)EditCreate;
  api->fp[105] = (int *)ButtonCreate;
  api->fp[106] = (int *)ListCreate;
  api->fp[107] = (int *)ListRemoveAll;
  api->fp[108] = (int *)ListAddItem;
  api->fp[109] = (int *)ListRemoveItem;
  api->fp[110] = (int *)ListSelectItem;
  api->fp[111] = (int *)ListGetItem;
  api->fp[112] = (int *)ListGetSelectedIndex;
  api->fp[113] = (int *)EditSetText;
  api->fp[114] = (int *)EditGetText;
  api->fp[115] = (int *)DoUI;
  api->fp[116] = (int *)DoMessageBox;
  api->fp[117] = (int *)DescentDefer;
  api->fp[118] = (int *)NewUIGameWindowCreate;
  api->fp[119] = (int *)NewUIGameWindowDestroy;
  api->fp[120] = (int *)NewUIGameWindowOpen;
  api->fp[121] = (int *)NewUIGameWindowClose;
  api->fp[122] = (int *)HotSpotCreate;
  api->fp[123] = (int *)PollUI;
  api->fp[124] = (int *)RemoveUITextItem;
  api->fp[125] = (int *)CreateNewUITextItem;
  api->fp[126] = (int *)UIConsoleGadgetCreate;
  api->fp[127] = (int *)UIConsoleGadgetputs;
  api->fp[128] = (int *)NewUIWindowSetFocusOnEditGadget;
  api->fp[129] = (int *)OldEditCreate;
  api->fp[130] = (int *)OldListCreate;
  api->fp[131] = (int *)OldListRemoveAll;
  api->fp[132] = (int *)OldListAddItem;
  api->fp[133] = (int *)OldListRemoveItem;
  api->fp[134] = (int *)OldListSelectItem;
  api->fp[135] = (int *)OldListGetItem;
  api->fp[136] = (int *)OldListGetSelectedIndex;
  api->fp[137] = (int *)OldEditSetText;
  api->fp[138] = (int *)OldEditGetText;
  api->fp[139] = (int *)ToggleUICallback;
  api->fp[140] = (int *)SetOldEditBufferLen;
  api->fp[141] = (int *)NewUIWindowLoadBackgroundImage;
  api->fp[142] = (int *)DeleteUIItem;
  api->fp[143] = (int *)HotSpotSetStates;
  api->fp[144] = (int *)SetUICallback;
  api->fp[145] = (int *)RetrieveUICallback;
  api->fp[146] = (int *)SuspendControls;
  api->fp[147] = (int *)ResumeControls;
  api->fp[148] = (int *)ui_ShowCursor;
  api->fp[149] = (int *)ui_HideCursor;
  api->fp[150] = (int *)GameFrame;
  api->fp[151] = (int *)PrintDedicatedMessage;
  api->fp[152] = (int *)ddio_MakePath;
  api->fp[153] = (int *)ddio_SplitPath;
  api->fp[154] = (int *)D3W_Play2dSound;
  api->fp[155] = (int *)D3W_TouchSound;
  api->fp[156] = (int *)dDatabaseRead1;
  api->fp[157] = (int *)dDatabaseRead2;
  api->fp[158] = (int *)dDatabaseRead3;
  api->fp[159] = (int *)dDatabaseWrite1;
  api->fp[160] = (int *)dDatabaseWrite2;
  api->fp[161] = (int *)dAttachObject;
  api->fp[162] = (int *)dObjGet;
  api->fp[163] = (int *)ListSetSelectedIndex;
  api->fp[164] = (int *)RemoveUIBmpItem;
  api->fp[165] = (int *)CreateNewUIBmpItem;
  api->fp[166] = (int *)GetUIItemWidth;
  api->fp[167] = (int *)GetUIItemHeight;
  api->fp[168] = (int *)SliderCreate;
  api->fp[169] = (int *)SliderSetRange;
  api->fp[170] = (int *)SliderGetRange;
  api->fp[171] = (int *)SliderSetPos;
  api->fp[172] = (int *)SliderGetPos;
  api->fp[173] = (int *)SliderSetSelectChangeCallback;
  api->fp[174] = (int *)SliderSetSelectChangeCallbackWData;
  api->fp[175] = (int *)TextSetTitle;
  api->fp[176] = (int *)PPic_GetPilot;
  api->fp[177] = (int *)PPic_GetBitmapHandle;
  api->fp[178] = (int *)rend_DrawLine;
  api->fp[179] = (int *)rend_SetFlatColor;
  api->fp[180] = (int *)MultiSetLogoState;
  api->fp[181] = (int *)PlayerGetRandomStartPosition;
  api->fp[182] = (int *)InitPlayerNewShip;
  api->fp[183] = (int *)CheckBoxCreate;
  api->fp[184] = (int *)CheckBoxSetCheck;
  api->fp[185] = (int *)CheckBoxIsChecked;
  api->fp[186] = (int *)nw_GetHostAddressFromNumbers;
  api->fp[187] = (int *)mng_ClearAddonTables;
  api->fp[188] = (int *)mng_SetAddonTable;
  api->fp[189] = (int *)DebugBreak_callback_stop;
  api->fp[190] = (int *)DebugBreak_callback_resume;
  api->fp[191] = (int *)Int3MessageBox;
  api->fp[192] = (int *)GetUIItemPosition;
  api->fp[193] = (int *)dAttachObjectRadius;
  api->fp[194] = (int *)dUnattachChildren;
  api->fp[195] = (int *)dUnattachChild;
  api->fp[196] = (int *)dUnattachFromParent;
  api->fp[197] = (int *)vm_GetMagnitude;
  api->fp[198] = (int *)vm_MatrixMulVector;
  api->fp[199] = (int *)phys_apply_force;
  api->fp[200] = (int *)phys_apply_rot;
  api->fp[201] = (int *)vm_TransposeMatrix;
  api->fp[202] = (int *)vm_CrossProduct;
  api->fp[203] = (int *)vm_NormalizeVector;
  api->fp[204] = (int *)ConvertEulerToAxisAmount;
  api->fp[205] = (int *)ConvertAxisAmountToEuler;
  api->fp[206] = (int *)vm_GetMagnitudeFast;
  api->fp[207] = (int *)vm_MakeIdentity;
  api->fp[208] = (int *)dvm_MakeZeroVector;
  api->fp[209] = (int *)dvm_MakeZeroAngle;
  api->fp[210] = (int *)vm_VectorMulTMatrix;
  api->fp[211] = (int *)vm_MatrixMul;
  api->fp[212] = (int *)vm_MatrixMulTMatrix;
  api->fp[213] = (int *)vm_DotProduct;
  api->fp[214] = (int *)vm_SubVectors;
  api->fp[215] = (int *)vm_AddVectors;
  api->fp[216] = (int *)vm_AverageVector;
  api->fp[217] = (int *)vm_ScaleVector;
  api->fp[218] = (int *)vm_ScaleAddVector;
  api->fp[219] = (int *)vm_DivVector;
  api->fp[220] = (int *)vm_NormalizeVectorFast;
  api->fp[221] = (int *)vm_ClearMatrix;
  api->fp[222] = (int *)vm_AnglesToMatrix;
  api->fp[223] = (int *)vm_Orthogonalize;
  api->fp[224] = (int *)vm_VectorToMatrix;
  api->fp[225] = (int *)vm_VectorAngleToMatrix;
  api->fp[226] = (int *)vm_SinCos;
  api->fp[227] = (int *)vm_GetSlope;
  api->fp[228] = (int *)vm_GetPerp;
  api->fp[229] = (int *)vm_GetNormal;
  api->fp[230] = (int *)vm_VectorDistance;
  api->fp[231] = (int *)vm_VectorDistanceQuick;
  api->fp[232] = (int *)vm_GetNormalizedDir;
  api->fp[233] = (int *)vm_GetNormalizedDirFast;
  api->fp[234] = (int *)vm_ExtractAnglesFromMatrix;
  api->fp[235] = (int *)vm_DeltaAngVec;
  api->fp[236] = (int *)vm_DeltaAngVecNorm;
  api->fp[237] = (int *)vm_DistToPlane;
  api->fp[238] = (int *)calc_det_value;
  api->fp[239] = (int *)vm_MakeInverseMatrix;
  api->fp[240] = (int *)vm_SinCosToMatrix;
  api->fp[241] = (int *)vm_GetCentroid;
  api->fp[242] = (int *)vm_MakeRandomVector;
  api->fp[243] = (int *)vm_ComputeBoundingSphere;
  api->fp[244] = (int *)vm_GetCentroidFast;
  api->fp[245] = (int *)RenderHUDGetTextLineWidth;
  api->fp[246] = (int *)RenderHUDGetTextHeight;
  api->fp[247] = (int *)dStartFrame;
  api->fp[248] = (int *)EndFrame;
  api->fp[249] = (int *)ResetFacings;
  api->fp[250] = (int *)GameRenderWorld;
  api->fp[251] = (int *)GetFrameParameters;
  api->fp[252] = (int *)rend_SetZBufferState;
  api->fp[253] = (int *)rend_SetLighting;
  api->fp[254] = (int *)rend_SetColorModel;
  api->fp[255] = (int *)rend_SetTextureType;
  api->fp[256] = (int *)rend_DrawPolygon3D;
  api->fp[257] = (int *)rend_SetMipState;
  api->fp[258] = (int *)rend_SetFogState;
  api->fp[259] = (int *)rend_SetFiltering;
  api->fp[260] = (int *)rend_SetOverlayMap;
  api->fp[261] = (int *)rend_SetOverlayType;
  api->fp[262] = (int *)rend_ClearScreen;
  api->fp[263] = (int *)rend_SetPixel;
  api->fp[264] = (int *)rend_GetPixel;
  api->fp[265] = (int *)rend_FillCircle;
  api->fp[266] = (int *)rend_DrawCircle;
  api->fp[267] = (int *)rend_SetAlphaType;
  api->fp[268] = (int *)rend_SetAlphaValue;
  api->fp[269] = (int *)rend_SetWrapType;
  api->fp[270] = (int *)rend_SetZBias;
  api->fp[271] = (int *)rend_SetZBufferWriteMask;
  api->fp[272] = (int *)rend_GetLFBLock;
  api->fp[273] = (int *)rend_ReleaseLFBLock;
  api->fp[274] = (int *)rend_DrawLFBBitmap;
  api->fp[275] = (int *)rend_DrawSpecialLine;
  api->fp[276] = (int *)fvi_FindIntersection;
  api->fp[277] = (int *)fvi_QuickDistFaceList;
  api->fp[278] = (int *)fvi_QuickDistCellList;
  api->fp[279] = (int *)fvi_QuickDistObjectList;
  api->fp[280] = (int *)fvi_QuickRoomCheck;
  api->fp[281] = (int *)ObjSetPosAndMarkMoved;
  api->fp[282] = (int *)dSetObjectDeadFlagWDemo;
  api->fp[283] = (int *)taunt_AreEnabled;
  api->fp[284] = (int *)dSetAudioTaunts;
  api->fp[285] = (int *)GetRankIndex;
  api->fp[286] = (int *)VisEffectAllocate;
  api->fp[287] = (int *)VisEffectFree;
  api->fp[288] = (int *)VisEffectInitType;
  api->fp[289] = (int *)VisEffectCreate;
  api->fp[290] = (int *)VisEffectLink;
  api->fp[291] = (int *)VisEffectUnlink;
  api->fp[292] = (int *)VisEffectRelink;
  api->fp[293] = (int *)VisEffectDelete;
  api->fp[294] = (int *)CreateRandomSparks;
  api->fp[295] = (int *)CreateRandomLineSparks;
  api->fp[296] = (int *)VisEffectCreateControlled;
  api->fp[297] = (int *)CreateRandomParticles;
  api->fp[298] = (int *)AttachRandomNapalmEffectsToObject;
  api->fp[299] = (int *)InitObjectScripts;
  api->fp[300] = (int *)g3_StartFrame;
  api->fp[301] = (int *)g3_EndFrame;
  api->fp[302] = (int *)g3_GetViewPosition;
  api->fp[303] = (int *)g3_GetViewMatrix;
  api->fp[304] = (int *)g3_GetUnscaledMatrix;
  api->fp[305] = (int *)g3_StartInstanceMatrix;
  api->fp[306] = (int *)g3_StartInstanceAngles;
  api->fp[307] = (int *)g3_DoneInstance;
  api->fp[308] = (int *)g3_CheckNormalFacing;
  api->fp[309] = (int *)g3_RotatePoint;
  api->fp[310] = (int *)g3_ProjectPoint;
  api->fp[311] = (int *)g3_CalcPointDepth;
  api->fp[312] = (int *)g3_Point2Vec;
  api->fp[313] = (int *)g3_CodePoint;
  api->fp[314] = (int *)g3_RotateDeltaX;
  api->fp[315] = (int *)g3_RotateDeltaY;
  api->fp[316] = (int *)g3_RotateDeltaZ;
  api->fp[317] = (int *)g3_RotateDeltaVec;
  api->fp[318] = (int *)g3_AddDeltaVec;
  api->fp[319] = (int *)g3_DrawPoly;
  api->fp[320] = (int *)g3_DrawSphere;
  api->fp[321] = (int *)g3_CheckAndDrawPoly;
  api->fp[322] = (int *)g3_DrawLine;
  api->fp[323] = (int *)g3_DrawBitmap;
  api->fp[324] = (int *)g3_DrawRotatedBitmap;
  api->fp[325] = (int *)g3_DrawBox;
  api->fp[326] = (int *)g3_SetCustomClipPlane;
  api->fp[327] = (int *)g3_SetFarClipZ;
  api->fp[328] = (int *)g3_ClipPolygon;
  api->fp[329] = (int *)g3_FreeTempPoints;
  api->fp[330] = (int *)g3_GetMatrixScale;
  api->fp[331] = (int *)g3_SetTriangulationTest;
  api->fp[332] = (int *)g3_DrawSpecialLine;
  api->fp[333] = (int *)g3_DrawPlanarRotatedBitmap;
  api->fp[334] = (int *)PlayerStopSounds;
  api->fp[335] = (int *)FindArg;
  api->fp[336] = (int *)FireWeaponFromObject;
  api->fp[337] = (int *)CreateAndFireWeapon;
  api->fp[338] = (int *)SelectNextCameraView;
  api->fp[339] = (int *)dInven_Add;
  api->fp[340] = (int *)dInven_AddObject;
  api->fp[341] = (int *)dInven_AddCounterMeasure;
  api->fp[342] = (int *)dInven_Remove;
  api->fp[343] = (int *)dInven_Use;
  api->fp[344] = (int *)dInven_UseObjHandle;
  api->fp[345] = (int *)dInven_Size;
  api->fp[346] = (int *)dInven_CheckItem;
  api->fp[347] = (int *)dInven_Reset;
  api->fp[348] = (int *)dInven_ResetPos;
  api->fp[349] = (int *)dInven_NextPos;
  api->fp[350] = (int *)dInven_PrevPos;
  api->fp[351] = (int *)dInven_GetPosTypeID;
  api->fp[352] = (int *)dInven_GetAuxPosTypeID;
  api->fp[353] = (int *)dInven_GetPosDescription;
  api->fp[354] = (int *)dInven_GetPosIconName;
  api->fp[355] = (int *)dInven_GetPosName;
  api->fp[356] = (int *)dInven_GetPosInfo;
  api->fp[357] = (int *)dInven_GetPosCount;
  api->fp[358] = (int *)dInven_AtBeginning;
  api->fp[359] = (int *)dInven_AtEnd;
  api->fp[360] = (int *)dInven_GotoPos;
  api->fp[361] = (int *)dInven_GotoPosTypeID;
  api->fp[362] = (int *)dInven_UsePos;
  api->fp[363] = (int *)dInven_GetPos;
  api->fp[364] = (int *)dInven_ValidatePos;
  api->fp[365] = (int *)dInven_IsSelectable;
  api->fp[366] = (int *)dInven_IsUsable;
  api->fp[367] = (int *)dInven_GetTypeIDCount;
  api->fp[368] = (int *)dInven_FindPos;
  api->fp[369] = (int *)dInven_GetInventoryItemList;

  // Variable pointers
  api->vp[0] = (int *)&Player_num;
  api->vp[1] = (int *)&Highest_room_index;
  api->vp[2] = (int *)&Game_window_w;
  api->vp[3] = (int *)&Game_window_h;
  api->vp[4] = (int *)&Game_fonts;
  api->vp[5] = (int *)&Frametime;
  api->vp[6] = (int *)&Gametime;
  api->vp[7] = (int *)&Multi_additional_damage;
  api->vp[8] = (int *)&Game_interface_mode;
  api->vp[9] = (int *)LocalD3Dir;
  api->vp[10] = (int *)&Game_is_master_tracker_game;
  api->vp[11] = (int *)Local_object_list;
  api->vp[12] = (int *)Server_object_list;
  api->vp[13] = (int *)&Dedicated_server;
  api->vp[14] = (int *)Player_colors;
  api->vp[15] = (int *)&Hud_aspect_x;
  api->vp[16] = (int *)&Hud_aspect_y;
  api->vp[17] = (int *)&Viewer_object;
  api->vp[18] = (int *)&Render_zoom;
  api->vp[19] = (int *)&Game_window_x;
  api->vp[20] = (int *)&Game_window_y;
  api->vp[21] = (int *)&Poly_models;
  api->vp[22] = (int *)VisEffects;
  api->vp[23] = (int *)&Highest_vis_effect_index;
  api->vp[24] = (int *)&Multi_next_level;
  api->vp[25] = (int *)&Level_info;
  api->vp[26] = (int *)GameArgs;
  api->vp[27] = (int *)Camera_view_mode;
  api->vp[28] = (int *)Object_info;

  api->osiris_functions = &Multi_d3m_osiris_funcs;
  Osiris_CreateModuleInitStruct(&Multi_d3m_osiris_funcs);
  Multi_d3m_osiris_funcs.string_table = NULL;
  Multi_d3m_osiris_funcs.string_count = 0;
  Multi_d3m_osiris_funcs.module_identifier = 0xEDF7;
  Multi_d3m_osiris_funcs.module_is_static = false;
  Multi_d3m_osiris_funcs.game_checksum = Osiris_game_checksum;
  Multi_d3m_osiris_funcs.script_identifier = Netgame.scriptname;
}
// Closes and deletes any tempfiles for the game module
void CloseGameModule(module *mod) {
  // Clear out error queue
  mod_GetLastError();
  mod_FreeModule(mod);
  if (Multi_game_dll_name[0] != '\0') {
    // Try deleting the file now!
    if (!ddio_DeleteFile(Multi_game_dll_name)) {
      mprintf(0, "Couldn't delete the tmp dll");
    }
  }
  mod->handle = NULL;
}
// this function will load up the DLL, but not get any symbols
bool InitGameModule(const char *name, module *mod) {
  char lib_name[_MAX_PATH * 2];
  char dll_name[_MAX_PATH * 2];
  char tmp_dll_name[_MAX_PATH * 2];
  // Make the hog filename
  ddio_MakePath(lib_name, Base_directory, "netgames", name, NULL);
  strcat(lib_name, ".d3m");
// Make the dll filename
#if defined(WIN32)
  snprintf(dll_name, sizeof(dll_name), "%s.dll", name);
#else
  snprintf(dll_name, sizeof(dll_name), "%s.so", name);
#endif

  // Open the hog file
  if (!cf_OpenLibrary(lib_name)) {
    ddio_MakePath(tmp_dll_name, Base_directory, "netgames", name, NULL);
    strcat(tmp_dll_name, ".d3m");
    Multi_game_dll_name[0] = '\0';
    goto loaddll;
  }
  // get a temp file name
  if (!ddio_GetTempFileName(Descent3_temp_directory, "d3m", tmp_dll_name)) {
    return false;
  }
  // Copy the DLL
  if (!cf_CopyFile(tmp_dll_name, dll_name)) {
    mprintf(0, "DLL copy failed!\n");
    return false;
  }
  strcpy(Multi_game_dll_name, tmp_dll_name);
loaddll:
  // Clear out error queue
  mod_GetLastError();
  if (!mod_LoadModule(mod, tmp_dll_name)) {
    int err = mod_GetLastError();
    mprintf(0, "You are missing the DLL %s!\n", name);
    return false;
  }
  return true;
}
// Frees the dll if its in memory
void FreeGameDLL() {
  if (!GameDLLHandle.handle)
    return;
  if (DLLGameClose)
    DLLGameClose();
  Osiris_UnloadMissionModule();
  CloseGameModule(&GameDLLHandle);
  DLLGameCall = NULL;
  DLLGameInit = NULL;
  DLLGameClose = NULL;
  DLLGetGameInfo = NULL;
}
// Loads the game dll.  Returns 1 on success, else 0 on failure
int LoadGameDLL(const char *name, int num_teams_to_use) {
  static int first = 1;
  // char lib_name[_MAX_PATH*2];
  // char dll_name[_MAX_PATH*2];
  // char tmp_dll_name[_MAX_PATH*2];
  // char dll_path_name[_MAX_PATH*2];
  // char fulldll[_MAX_PATH*2];
  // ddio_MakePath(fulldll,Base_directory,"netgames",name,NULL);
  if (GameDLLHandle.handle)
    FreeGameDLL();
  if (num_teams_to_use == -1) {
    int mint, maxt;
    if (GetDLLNumTeamInfo(name, &mint, &maxt)) {
      // multi team game
      num_teams_to_use = mint;
    } else {
      // non-team game
      num_teams_to_use = 1;
    }
  }
  mprintf(0, "Loading '%s', setting up for %d teams\n", name, num_teams_to_use);
  if (!InitGameModule(name, &GameDLLHandle))
    return 0;

  // Clear out error queue
  mod_GetLastError();
  DLLGameInit = (DLLGameInit_fp)mod_GetSymbol(&GameDLLHandle, "DLLGameInit", 12);
  if (!DLLGameInit) {
    int err = mod_GetLastError();
    mprintf(0, "Couldn't get a handle to the dll function DLLGameInit!\n");
    Int3();
    FreeGameDLL();
    return 0;
  }
  // Clear out error queue
  mod_GetLastError();
  DLLGameCall = (DLLGameCall_fp)mod_GetSymbol(&GameDLLHandle, "DLLGameCall", 8);
  if (!DLLGameCall) {
    int err = mod_GetLastError();
    mprintf(0, "Couldn't get a handle to the dll function DLLGameCall!\n");
    Int3();
    FreeGameDLL();
    return 0;
  }
  // Clear out error queue
  mod_GetLastError();
  DLLGameClose = (DLLGameClose_fp)mod_GetSymbol(&GameDLLHandle, "DLLGameClose", 0);
  if (!DLLGameClose) {
    int err = mod_GetLastError();
    mprintf(0, "Couldn't get a handle to the dll function DLLGameClose!\n");
    Int3();
    FreeGameDLL();
    return 0;
  }
  // Clear out error queue
  mod_GetLastError();
  DLLGetGameInfo = (DLLGetGameInfo_fp)mod_GetSymbol(&GameDLLHandle, "DLLGetGameInfo", 4);
  if (!DLLGetGameInfo) {
    int err = mod_GetLastError();
    mprintf(0, "Couldn't get a handle to the dll function DLLGetGameInfo!\n");
    Int3();
    FreeGameDLL();
    return 0;
  }
  if (first) {
    // Jeff: Linux for some reason dies if you try to
    // free a DLL/so on atexit, they should be freed
    // anyway during game sequencing
    // atexit (FreeGameDLL);
    first = 0;
  }
  void *api_fp;
  uint8_t ok = 1;
  api_fp = (void *)GetGameAPI;

  DLLGameInit((int *)api_fp, &ok, num_teams_to_use);
  if (!ok) {
    // The DLL said no to the load
    mprintf(0, "DLLGameInit returned false, couldn't init DLL\n");
    Int3();
    FreeGameDLL();
    return 0;
  }
  Osiris_LoadMissionModule(&GameDLLHandle, name);

  return 1;
}
// If this function is called than the DLL is to be closed, because there was an error running it
// if reason is not NULL than that is the reason why
void DLLFatalError(const char *reason) {
  mprintf(0, "============================\n");
  mprintf(0, "= DLL Fatal Error          =\n");
  mprintf(0, "============================\n");
  mprintf(0, "%s\n", (reason) ? reason : "<No Reason Given>");
  Netgame.flags |= NF_EXIT_NOW;
  Int3();
}
// The chokepoint function to call the dll function
void CallGameDLL(int eventnum, dllinfo *data) {
  if (GameDLLHandle.handle && DLLGameCall) {
    data->iRet = 0;
    DLLGameCall(eventnum, data);
  }
}
// Call this function right after a player connects to the game to see if a player is banned
bool GameDLLIsAddressBanned(network_address *addr, const char *tracker_id) {
  ASSERT(addr);
  DLLInfo.special_data = (uint8_t *)addr;
  // This used to be tracker_id, but storing a pointer as an int is a problem in 64 bit
  // and no code ever was populating the tracker id that I know of. -Kevin
  DLLInfo.iParam = 0;
  DLLInfo.iRet = 0;
  CallGameDLL(EVT_GAMECHECKBAN, &DLLInfo);
  if (DLLInfo.iRet)
    return true;
  else
    return false;
}
// Call this function to get the team that a connecting player should be placed on
//	Callsign and network address of the player MUST be filled in at this point
//	PXO id must also be set if mastertracker game
//
//	Return values:
//		-2:	GameDLL can't determine a team (user specified that they want to handle), so pick a random team
//		-1:	Dedicated Server
//		 0-4: Team
int GameDLLGetConnectingPlayersTeam(int slot) {
  ASSERT(slot >= 0 && slot < MAX_PLAYERS);
  DLLInfo.iParam = slot;
  DLLInfo.iRet = 0;
  CallGameDLL(EVT_GAME_GET_PLAYER_TEAM, &DLLInfo);
  return DLLInfo.iRet;
}
// Call this function to get information/options from a unloaded mod
bool GetDLLGameInfo(const char *name, tDLLOptions *options) {
  module mod = {NULL};
  memset(options, 0, sizeof(tDLLOptions));
  DLLGetGameInfo_fp modGetGameInfo;
  if (!InitGameModule(name, &mod))
    return false;
  // Clear out error queue
  mod_GetLastError();
  modGetGameInfo = (DLLGetGameInfo_fp)mod_GetSymbol(&mod, "DLLGetGameInfo", 4);
  if (!modGetGameInfo) {
    int err = mod_GetLastError();
    mprintf(0, "Couldn't get a handle to the dll function DLLGetGameInfo!\n");
    Int3();
    CloseGameModule(&mod);
    return false;
  }
  modGetGameInfo(options);
  CloseGameModule(&mod);
  return true;
}
// Call this function to get the list of requirements that the given module needs in order
// to be playable.  Returns the number of requirements it needs...-1 on error.
int GetDLLRequirements(const char *name, char *requirements, int buflen) {
  ASSERT(buflen >= 0);
  ASSERT(requirements);
  tDLLOptions opt;
  if (!GetDLLGameInfo(name, &opt)) {
    mprintf(0, "Unable to get %s's requirements\n", name);
    return -1;
  }
  strncpy(requirements, opt.requirements, buflen - 1);
  requirements[buflen - 1] = '\0';
  uint32_t opt_req_len = strlen(opt.requirements);
  if (opt_req_len > strlen(requirements)) {
    // too small of a buffer!
    mprintf(0, "Too small of a buffer to fill in all requirements!...need %d\n", opt_req_len + 1);
    Int3();
    // cut off last requirement (which is shortened)
    char *p = strrchr(requirements, ',');
    if (p) {
      *p = '\0';
    } else {
      *requirements = '\0';
    }
  }
  // go through requirements and count em
  char *p = requirements;
  int count = 0;
  while (*p) {
    if (*p == ',')
      count++;
    p++;
  }
  count++;
  return count;
}
// Call this function to get information about the number of teams for the game
// Returns true if it's a team game...false if it's a non-team game.
// If it returns true, then min is filled in with the minumum number of teams needed for the game
// and max is filled in with the maximum number of teams for the game...if they are the same
// value, then it is the only number of teams supported.
bool GetDLLNumTeamInfo(const char *name, int *mint, int *maxt) {
  tDLLOptions dllo;
  if (!GetDLLGameInfo(name, &dllo)) {
    *mint = 1;
    *maxt = 1;
    return false;
  }
  if (!(dllo.flags & DOF_MAXTEAMS)) {
    *mint = 1;
    *maxt = 1;
    return false;
  }
  *maxt = (dllo.max_teams == 0 || dllo.max_teams == 1 || dllo.max_teams < 0) ? 1 : std::min(dllo.max_teams, 4);
  if ((*maxt) == 1) {
    *mint = 1;
  } else {
    *mint = 2;
  }
  if (dllo.flags & DOF_MINTEAMS && dllo.min_teams >= 0) {
    *mint = (dllo.min_teams == 0 || dllo.min_teams == 1) ? 1 : std::min(*maxt, dllo.min_teams);
  }
  return ((*maxt) == 1) ? false : true;
}
