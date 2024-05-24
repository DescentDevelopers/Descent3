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

 * $Logfile: /DescentIII/Main/Dmfc/dmfcfunctions.cpp $
 * $Revision: 1.1.1.1 $
 * $Date: 2003/08/26 03:57:20 $
 * $Author: kevinb $
 *
 * Function pointers
 *
 * $Log: dmfcfunctions.cpp,v $
 * Revision 1.1.1.1  2003/08/26 03:57:20  kevinb
 * initial 1.5 import
 *
 *
 * 26    8/16/99 5:38p Jeff
 * final updates for patch
 *
 * 25    8/15/99 4:36p Jeff
 * finished exporting all inventory class functions.  export object_info
 * array. added check for -nooutragelogo to display Outrage logo display.
 *
 * 24    8/11/99 5:31p Jeff
 * addon tablefile function returns a bool now
 *
 * 23    8/11/99 1:22p Jeff
 * exported needed functions for camera windows
 *
 * 22    7/20/99 11:27a Jeff
 * created functions to fire weapons
 *
 * 21    7/11/99 3:31p Jeff
 * exported game arguments, made command line option to specify
 * autoexec.dmfc
 *
 * 20    7/07/99 12:16p Jeff
 * all mangles symbol names fixed.  Finished making interface functions.
 *
 * 19    5/20/99 5:31p Jeff
 * exported PlayerStopSounds
 *
 * 18    5/19/99 11:49p Jeff
 * final preparations for dmfc version 1.0
 *
 * 17    5/19/99 5:27p Jeff
 * fixed some function prototypes
 *
 * 16    5/19/99 2:44a Jeff
 * CreateTextItem has a third parameter
 *
 * 15    5/03/99 8:39a Jeff
 * tons of Entropy fixes
 *
 * 14    4/30/99 7:36p Jeff
 * exported vis_effects to dmfc
 *
 * 13    3/27/99 4:53p Jeff
 * player rankings in multiplayer games implemented.  Fixed join message
 * so it doesn't get cut off
 *
 * 12    3/22/99 6:20p Jeff
 * added 2 more audio taunts.  a mulitplayer event when someone plays an
 * audio taunt.  option to disable audio taunts.
 *
 * 11    3/17/99 12:23p Jeff
 * converted DMFC to be COM interface
 *
 * 10    3/12/99 7:40p Jeff
 * removed enhanced objcreate, added enhanced objsetpos
 *
 * 9     3/11/99 6:30p Jeff
 * numerous fixes to demo system in multiplayer games (when
 * recording/playback a demo in a multiplayer game)
 *
 * 8     3/05/99 1:29p Jeff
 *
 * 7     2/25/99 8:54p Jeff
 * Inventory supports level change persistant items.  Inventory supports
 * time-out objects.  Inventory Reset changed (takes a level of reset
 * now).  Quad lasers stay across level change (single player).  Guidebot
 * bug fixed (now back in ship on level start).  Quads time out when
 * spewed.  Invulnerability and cloak powerups no longer use game
 * event/callbacks, so they can be saved in game saves (moved to
 * MakePlayerInvulnerable and MakeObjectInvisible)
 *
 * 6     2/10/99 11:36p Jeff
 * exported renderer and fvi functions
 *
 * 5     2/08/99 5:22p Jeff
 * exported rend_SetZBuffer...fixed up small view of stats
 *
 * 4     2/08/99 12:06a Jeff
 * exported some new functions and variables
 *
 * 3     2/06/99 6:59p Jeff
 * created RenderHUDGetTextLineWidth and RenderHUDGetTextHeight
 *
 * 2     2/03/99 4:09p Jeff
 * moved function pointers to seperate file. created autoexec.dmfc
 *
 * $NoKeywords: $
 */

#include "DMFC.h"
#include "dmfcinternal.h"
#include "dmfcinputcommands.h"
#include "gamedll_header.h"

// Determine if we are building the DLL, or not.  If we are building the DMFC
// DLL, than we need to define some things
#if defined(DMFC_DLL)
#define DMFCFUNCTION DLLFUNCEXPORT // for DMFC DLL library
#else
#define DMFCFUNCTION DLLFUNCIMPORT // for modules built for dmfc dll
#endif

DMFCFUNCTION void (*DLLGetGameAPI)(game_api *);
DMFCFUNCTION bool (*DLLAddHUDMessage)(const char *format, ...);
DMFCFUNCTION void (*DLLDebug_ConsolePrintf)(int n, const char *format, ...);
DMFCFUNCTION void (*DLLMultiSendClientExecuteDLL)(int eventnum, int me_objnum, int it_objnum, int to, dllinfo *info);
DMFCFUNCTION void (*DLLMultiSendObject)(object *obj, ubyte announce, ubyte demo_record);
DMFCFUNCTION void (*DLLMultiPaintGoalRooms)(int *texcolors);
DMFCFUNCTION void (*DLLMultiSendSpecialPacket)(int slot, ubyte *outdata, int size);
DMFCFUNCTION void (*DLLComputeRoomCenter)(vector *vp, room *rp);
DMFCFUNCTION int (*DLLGetGoalRoomForTeam)(int teamnum);
DMFCFUNCTION int (*DLLObjCreate)(ubyte type, ushort id, int roomnum, vector *pos, const matrix *orient,
                                 int parent_handle);
DMFCFUNCTION int (*DLLFindObjectIDName)(const char *name);
DMFCFUNCTION void (*DLLObjSetPosNoMark)(object *objp, vector *newpos, int roomnum, matrix *orient,
                                        bool f_update_attached_children);
DMFCFUNCTION void (*DLLObjSetPos)(object *objp, vector *newpos, int roomnum, matrix *orient,
                                  bool f_update_attached_children);
DMFCFUNCTION void (*DLLSetMaxTeams)(int num);
DMFCFUNCTION int (*DLLIncTeamScore)(int team, int amount);
DMFCFUNCTION bool (*DLLInvCheckItem)(int pnum, int type, int id);
DMFCFUNCTION bool (*DLLInvAddTypeID)(int pnum, int type, int id, int aux_type, int aux_id, int flags, const char *description);
DMFCFUNCTION bool (*DLLInvRemove)(int pnum, int type, int id);
DMFCFUNCTION void (*DLLPlayerSetLighting)(int slot, float dist, float r, float g, float b);
DMFCFUNCTION int (*DLLFindShipName)(const char *name);
DMFCFUNCTION void (*DLLPlayerSetRotatingBall)(int slot, int num, float speed, float *r, float *g, float *b);
DMFCFUNCTION void (*DLLPlayerChangeShip)(int slot, int ship_index);
DMFCFUNCTION int (*DLLInvGetTypeIDCount)(int playernum, int type, int id);
DMFCFUNCTION int (*DLLPlay3dSound)(int sound_index, object *cur_obj, float volume, int flags);
DMFCFUNCTION int (*DLLFindSoundName)(const char *str);
DMFCFUNCTION int (*DLLSpewCreate)(spewinfo *spew);
DMFCFUNCTION void (*DLLSpewClearEvent)(int handle, bool force);
DMFCFUNCTION int (*DLLbm_AllocLoadFileBitmap)(const char *filename, int mipped, int format);
DMFCFUNCTION void (*DLLbm_FreeBitmap)(int handle);
DMFCFUNCTION void (*DLLrend_DrawScaledBitmap)(int x1, int y1, int x2, int y2, int bm, float u0, float v0, float u1,
                                              float v1, float zval, int color, float *alphas);
DMFCFUNCTION void (*DLLgrtext_Printf)(int x, int y, const char *fmt, ...);
DMFCFUNCTION void (*DLLgrtext_Flush)(void);
DMFCFUNCTION void (*DLLgrtext_SetColor)(ddgr_color col);
DMFCFUNCTION void (*DLLgrtext_SetFancyColor)(ddgr_color col1, ddgr_color col2, ddgr_color col3, ddgr_color col4);
DMFCFUNCTION void (*DLLgrtext_SetAlpha)(ubyte alpha);
DMFCFUNCTION ubyte (*DLLgrtext_GetAlpha)(void);
DMFCFUNCTION void (*DLLgrtext_SetFont)(int font_handle);
DMFCFUNCTION int (*DLLgrtext_GetFont)(void);
DMFCFUNCTION int (*DLLgrtext_GetTextLineWidth)(const char *str);
DMFCFUNCTION int (*DLLgrfont_GetHeight)(int font);
DMFCFUNCTION void (*DLLgrtext_CenteredPrintf)(int xoff, int y, const char *fmt, ...);
DMFCFUNCTION bool (*DLLAddColoredHUDMessage)(ddgr_color color, const char *format, ...);
DMFCFUNCTION int (*DLLbm_h)(int handle, int miplevel);
DMFCFUNCTION int (*DLLbm_w)(int handle, int miplevel);
DMFCFUNCTION void (*DLLrend_DrawSimpleBitmap)(int bm_handle, int x, int y);
DMFCFUNCTION void (*DLLMultiClientSendSpecialPacket)(ubyte *outdate, int size);
DMFCFUNCTION bool (*DLLAddBlinkingHUDMessage)(char *format, ...);
DMFCFUNCTION void (*DLLInvReset)(int playernum, bool reset_all);
DMFCFUNCTION void (*DLLAddHUDItem)(tHUDItem *item);
DMFCFUNCTION void (*DLLRenderHUDQuad)(int x, int y, int w, int h, float u0, float v0, float u1, float v1, int bm,
                                      ubyte alpha, int sat_count);
DMFCFUNCTION void (*DLLRenderHUDText)(ddgr_color col, ubyte alpha, int sat_count, int x, int y, const char *fmt, ...);
DMFCFUNCTION void (*DLLMultiEndLevel)(void);
DMFCFUNCTION ushort *(*DLLbm_data)(int handle, int miplevel);
DMFCFUNCTION int (*DLLbm_AllocBitmap)(int w, int h, int add_mem);
DMFCFUNCTION void (*DLLrend_FillRect)(ddgr_color color, int x1, int y1, int x2, int y2);
DMFCFUNCTION bool (*DLLbm_CreateChunkedBitmap)(int bm_handle, chunked_bitmap *chunk);
DMFCFUNCTION void (*DLLbm_DestroyChunkedBitmap)(chunked_bitmap *chunk);
DMFCFUNCTION void (*DLLrend_DrawChunkedBitmap)(chunked_bitmap *chunk, int x, int y, ubyte alpha);
DMFCFUNCTION void (*DLLrend_DrawScaledChunkedBitmap)(chunked_bitmap *chunk, int x, int y, int neww, int newh,
                                                     ubyte alpha);
DMFCFUNCTION void (*DLLOpenCFILE)(CFILE **handle, const char *filename, const char *mode);
DMFCFUNCTION void (*DLLcfclose)(CFILE *cfp);
DMFCFUNCTION int (*DLLcfeof)(CFILE *cfp);
DMFCFUNCTION int (*DLLcfexist)(const char *filename);
DMFCFUNCTION int (*DLLcf_ReadBytes)(ubyte *buf, int count, CFILE *cfp);
DMFCFUNCTION int (*DLLcf_ReadInt)(CFILE *cfp);
DMFCFUNCTION short (*DLLcf_ReadShort)(CFILE *cfp);
DMFCFUNCTION sbyte (*DLLcf_ReadByte)(CFILE *cfp);
DMFCFUNCTION float (*DLLcf_ReadFloat)(CFILE *cfp);
DMFCFUNCTION double (*DLLcf_ReadDouble)(CFILE *cfp);
DMFCFUNCTION int (*DLLcf_ReadString)(char *buf, size_t n, CFILE *cfp);
DMFCFUNCTION int (*DLLcf_WriteBytes)(const ubyte *buf, int count, CFILE *cfp);
DMFCFUNCTION int (*DLLcf_WriteString)(CFILE *cfp, const char *buf);
DMFCFUNCTION void (*DLLcf_WriteInt)(CFILE *cfp, int i);
DMFCFUNCTION void (*DLLcf_WriteShort)(CFILE *cfp, short s);
DMFCFUNCTION void (*DLLcf_WriteByte)(CFILE *cfp, sbyte b);
DMFCFUNCTION void (*DLLcf_WriteFloat)(CFILE *cfp, float f);
DMFCFUNCTION void (*DLLcf_WriteDouble)(CFILE *cfp, double d);
DMFCFUNCTION bool (*DLLcf_CopyFile)(char *dest, const char *src);
DMFCFUNCTION bool (*DLLcf_Diff)(const char *a, const char *b);
DMFCFUNCTION void (*DLLMultiDisconnectPlayer)(int slot);
DMFCFUNCTION void (*DLLnw_GetNumbersFromHostAddress)(network_address *address, char *str);
DMFCFUNCTION int (*DLLnw_GetThisIP)(void);
DMFCFUNCTION bool (*DLLCreateStringTable)(const char *filename, char ***table, int *size);
DMFCFUNCTION void (*DLLDestroyStringTable)(char **table, int size);
DMFCFUNCTION void (*DLLRenderHUDTextFlags)(int flags, ddgr_color col, ubyte alpha, int sat_count, int x, int y, const char *fmt, ...);
DMFCFUNCTION void (*DLLPlayerSetHUDNameFOV)(int fov);
DMFCFUNCTION void (*DLLGetUltimateParentForObject)(object **parent, object *child);
DMFCFUNCTION void (*DLLSetObjectDeadFlagRaw)(object *obj, bool tell_clients_to_remove, bool tell_clients_to_play_sound);
DMFCFUNCTION void (*DLLSetObjectDeadFlag)(object *obj, bool tell_clients_to_remove, bool tell_clients_to_play_sound);
DMFCFUNCTION void (*FatalError)(const char *reason);
DMFCFUNCTION int (*DLLMultiMatchWeapon)(uint unique_id);
DMFCFUNCTION uint (*DLLMultiGetMatchChecksum)(int type, int id);
DMFCFUNCTION int (*DLLFindWeaponName)(const char *name);
DMFCFUNCTION int (*DLLcf_OpenLibrary)(const char *libname);
DMFCFUNCTION void (*DLLcf_CloseLibrary)(int handle);
DMFCFUNCTION void (*DLLMultiSendRequestToObserve)(int mode, int on, int objnum);
DMFCFUNCTION int (*DLLFindTextureName)(const char *name);
DMFCFUNCTION bool (*DLLApplyDamageToPlayer)(object *playerobj, object *killer, int damage_type, float damage_amount,
                                            int server_says, int weapon_id, bool playsound);
DMFCFUNCTION int (*DLLMultiMatchGeneric)(uint unique_id);
DMFCFUNCTION void (*DLLSetUITextItemText)(void *uit, char *newtext, uint32_t color);
DMFCFUNCTION void *(*DLLNewUIWindowCreate)(int x, int y, int w, int h, int flags);
DMFCFUNCTION void (*DLLNewUIWindowDestroy)(void *deswin);
DMFCFUNCTION void (*DLLNewUIWindowOpen)(void *deswin);
DMFCFUNCTION void (*DLLNewUIWindowClose)(void *deswin);
DMFCFUNCTION void *(*DLLTextCreate)(void *parentwin, void *textitem, int x, int y, int flags);
DMFCFUNCTION void *(*DLLEditCreate)(void *parentwin, int id, int x, int y, int w, int h, int flags);
DMFCFUNCTION void *(*DLLButtonCreate)(void *parentwin, int id, void *titleitem, int x, int y, int w, int h, int flags);
DMFCFUNCTION void *(*DLLListCreate)(void *parentwin, int id, int x, int y, int w, int h, int flags);
DMFCFUNCTION void (*DLLListRemoveAll)(void *item);
DMFCFUNCTION void (*DLLListAddItem)(void *item, void *uitext);
DMFCFUNCTION void (*DLLListRemoveItem)(void *item, void *txtitem);
DMFCFUNCTION void (*DLLListSelectItem)(void *item, void *txtitem);
DMFCFUNCTION char *(*DLLListGetItem)(void *item, int index);
DMFCFUNCTION int (*DLLListGetSelectedIndex)(void *item);
DMFCFUNCTION void (*DLLListSetSelectedIndex)(void *item, int index);
DMFCFUNCTION void (*DLLEditSetText)(void *item, const char *buff);
DMFCFUNCTION void (*DLLEditGetText)(void *item, char *buff, int len);
DMFCFUNCTION int (*DLLDoUI)(void);
DMFCFUNCTION int (*DLLDoMessageBox)(const char *title, const char *msg, int type, ddgr_color title_color,
                                    ddgr_color text_color);
DMFCFUNCTION void (*DLLDescentDefer)(void);
DMFCFUNCTION void *(*DLLNewUIGameWindowCreate)(int x, int y, int w, int h, int flags);
DMFCFUNCTION void (*DLLNewUIGameWindowDestroy)(void *item);
DMFCFUNCTION void (*DLLNewUIGameWindowOpen)(void *item);
DMFCFUNCTION void (*DLLNewUIGameWindowClose)(void *item);
DMFCFUNCTION void *(*DLLHotSpotCreate)(void *parentwin, int id, int key, void *txtitemoff, void *txtitemon, int x,
                                       int y, int w, int h, int flags);
DMFCFUNCTION int (*DLLPollUI)(void);
DMFCFUNCTION void (*DLLRemoveUITextItem)(void *item);
DMFCFUNCTION void *(*DLLCreateNewUITextItem)(const char *newtext, uint32_t color, int font);
DMFCFUNCTION void (*DLLRemoveUIBmpItem)(void *item);
DMFCFUNCTION void *(*DLLCreateNewUIBmpItem)(int handle, ubyte alpha);
DMFCFUNCTION void *(*DLLUIConsoleGadgetCreate)(void *parentid, int id, int x, int y, int font, int cols, int rows,
                                               int flags);
DMFCFUNCTION void (*DLLUIConsoleGadgetputs)(void *item, const char *str);
DMFCFUNCTION void (*DLLNewUIWindowSetFocusOnEditGadget)(void *item, void *parent);
DMFCFUNCTION void *(*DLLOldListCreate)(void *parentitem, int id, int x, int y, int w, int h, int flags);
DMFCFUNCTION void (*DLLOldListRemoveAll)(void *item);
DMFCFUNCTION void (*DLLOldListAddItem)(void *item, void *uitext);
DMFCFUNCTION void (*DLLOldListRemoveItem)(void *item, void *txtitem);
DMFCFUNCTION void (*DLLOldListSelectItem)(void *item, void *txtitem);
DMFCFUNCTION void *(*DLLOldEditCreate)(void *parentitem, int id, int x, int y, int w, int h, int flags);
DMFCFUNCTION void (*DLLOldEditGetText)(void *item, char *buff, int len);
DMFCFUNCTION void (*DLLOldEditSetText)(void *item, const char *newtext);
DMFCFUNCTION char *(*DLLOldListGetItem)(void *item, int index);
DMFCFUNCTION int (*DLLOldListGetSelectedIndex)(void *item);
DMFCFUNCTION void (*DLLToggleUICallback)(int state);
DMFCFUNCTION void (*DLLSetOldEditBufferLen)(void *item, int len);
DMFCFUNCTION void (*DLLNewUIWindowLoadBackgroundImage)(void *item, const char *image_name);
DMFCFUNCTION void (*DLLDeleteUIItem)(void *delitem);
DMFCFUNCTION void (*DLLHotSpotSetStates)(void *hs, void *texton, void *textoff);
DMFCFUNCTION void (*DLLSetUICallback)(void (*fn)());
DMFCFUNCTION void (*DLLGetUICallback)(void **fn);
DMFCFUNCTION void (*DLLSuspendControls)(void);
DMFCFUNCTION void (*DLLResumeControls)(void);
DMFCFUNCTION void (*DLLShowCursor)(void);
DMFCFUNCTION void (*DLLHideCursor)(void);
DMFCFUNCTION void (*DLLGameFrame)(void);
DMFCFUNCTION void (*DPrintf)(const char *format, ...);
DMFCFUNCTION void (*DLLassert)(int, const char *, const char *, int);
DMFCFUNCTION void (*DLLddio_SplitPath)(const char *srcPath, char *path, char *filename, char *ext);
DMFCFUNCTION void (*DLLddio_MakePath)(char *newPath, const char *absolutePathHeader, const char *subDir, ...);
DMFCFUNCTION int (*DLLPlay2dSound)(int sound_index, float volume);
DMFCFUNCTION void (*DLLTouchSound)(int sound_index);
DMFCFUNCTION bool (*DatabaseRead1)(const char *label, char *entry, int *entrylen);
DMFCFUNCTION bool (*DatabaseRead2)(const char *label, void *entry, int wordsize);
DMFCFUNCTION bool (*DatabaseRead3)(const char *label, bool *entry);
DMFCFUNCTION bool (*DatabaseWrite1)(const char *label, const char *entry, int entrylen);
DMFCFUNCTION bool (*DatabaseWrite2)(const char *label, int entry);
DMFCFUNCTION bool (*DLLAttachObject)(object *parent, char parent_ap, object *child, char child_ap, bool f_use_aligned);
DMFCFUNCTION bool (*DLLObjGet)(int handle, object **obj);
DMFCFUNCTION int (*DLLGetUIItemWidth)(void *item);
DMFCFUNCTION int (*DLLGetUIItemHeight)(void *item);
DMFCFUNCTION void *(*DLLSliderCreate)(void *parent, int id, int x, int y, int flags);
DMFCFUNCTION void (*DLLSliderSetRange)(void *slider, int range);
DMFCFUNCTION int (*DLLSliderGetRange)(void *slider);
DMFCFUNCTION void (*DLLSliderSetPos)(void *slider, int pos);
DMFCFUNCTION int (*DLLSliderGetPos)(void *slider);
DMFCFUNCTION void (*DLLSliderSetSelectChangeCallback)(void *slider, void (*fn)(int));
DMFCFUNCTION void (*DLLSliderSetSelectChangeCallbackWData)(void *slider, void (*fn)(int, void *), void *ptr);
DMFCFUNCTION void (*DLLTextSetTitle)(void *text, void *textitem);
DMFCFUNCTION bool (*DLLPPic_GetPilot)(ushort pilot_id, char *pilot_name, int buffersize);
DMFCFUNCTION int (*DLLPPic_GetBitmapHandle)(ushort pilot_id);
DMFCFUNCTION void (*DLLrend_DrawLine)(int x1, int y1, int x2, int y2);
DMFCFUNCTION void (*DLLrend_SetFlatColor)(ddgr_color color);
DMFCFUNCTION void (*DLLMultiSetLogoState)(bool state);
DMFCFUNCTION void (*DLLPlayerSpewInventory)(object *obj, bool spew_energy_and_shield, bool spew_nonspewable);
DMFCFUNCTION int (*DLLPlayerGetRandomStartPosition)(int slot);
DMFCFUNCTION void (*DLLInitPlayerNewShip)(int slot, int inven_reset);
DMFCFUNCTION void *(*DLLCheckBoxCreate)(void *parent, int id, void *title, int x, int y, int w, int h, int flags);
DMFCFUNCTION void (*DLLCheckBoxSetCheck)(void *cb, bool state);
DMFCFUNCTION bool (*DLLCheckBoxIsChecked)(void *cb);
DMFCFUNCTION uint32_t (*DLLnw_GetHostAddressFromNumbers)(char *str);
DMFCFUNCTION void (*TableFilesClear)(void);
DMFCFUNCTION bool (*TableFileAdd)(char *filename);
DMFCFUNCTION void (*DLLDebugBreak_callback_stop)(void);
DMFCFUNCTION void (*DLLDebugBreak_callback_resume)(void);
DMFCFUNCTION void (*DLLInt3MessageBox)(const char *file, const char *line);
DMFCFUNCTION void (*DLLGetUIItemPosition)(void *item, int *x, int *y, int *w, int *h);
DMFCFUNCTION bool (*DLLAttachObjectRadius)(object *parent, char parent_ap, object *child, float percent_rad);
DMFCFUNCTION bool (*DLLUnattachChildren)(object *parent);
DMFCFUNCTION bool (*DLLUnattachChild)(object *parent, char parent_ap);
DMFCFUNCTION bool (*DLLUnattachFromParent)(object *child);
DMFCFUNCTION float (*DLLvm_GetMagnitude)(vector *);
DMFCFUNCTION void (*DLLvm_MatrixMulVector)(vector *, vector *, matrix *);
DMFCFUNCTION void (*DLLphys_apply_force)(object *obj, vector *force_vec, short weapon_index);
DMFCFUNCTION void (*DLLphys_apply_rot)(object *obj, vector *force_vec);
DMFCFUNCTION void (*DLLvm_TransposeMatrix)(matrix *);
DMFCFUNCTION void (*DLLvm_CrossProduct)(vector *, vector *, vector *);
DMFCFUNCTION float (*DLLvm_NormalizeVector)(vector *);
DMFCFUNCTION void (*DLLConvertEulerToAxisAmount)(vector *e, vector *n, float *w);
DMFCFUNCTION void (*DLLConvertAxisAmountToEuler)(vector *n, float *w, vector *e);
DMFCFUNCTION float (*DLLvm_GetMagnitudeFast)(vector *);
DMFCFUNCTION void (*DLLvm_MakeIdentity)(matrix *);
DMFCFUNCTION void (*DLLvm_MakeVectorZero)(vector *v);
DMFCFUNCTION void (*DLLvm_MakeAngleZero)(angvec *a);
DMFCFUNCTION void (*DLLvm_VectorMulTMatrix)(vector *result, vector *v, matrix *m);
DMFCFUNCTION void (*DLLvm_MatrixMul)(matrix *, matrix *, matrix *);
DMFCFUNCTION void (*DLLvm_MatrixMulTMatrix)(matrix *dest, matrix *src0, matrix *src1);
DMFCFUNCTION float (*DLLvm_DotProduct)(vector *, vector *);
DMFCFUNCTION void (*DLLvm_SubVectors)(vector *, const vector *, const vector *);
DMFCFUNCTION void (*DLLvm_AddVectors)(vector *, vector *, vector *);
DMFCFUNCTION void (*DLLvm_AverageVector)(vector *, int);
DMFCFUNCTION void (*DLLvm_ScaleVector)(vector *, vector *, float);
DMFCFUNCTION void (*DLLvm_ScaleAddVector)(vector *d, vector *p, vector *v, float s);
DMFCFUNCTION void (*DLLvm_DivVector)(vector *, vector *, float);
DMFCFUNCTION float (*DLLvm_NormalizeVectorFast)(vector *);
DMFCFUNCTION void (*DLLvm_ClearMatrix)(matrix *);
DMFCFUNCTION void (*DLLvm_AnglesToMatrix)(matrix *, angle p, angle h, angle b);
DMFCFUNCTION void (*DLLvm_Orthogonalize)(matrix *m);
DMFCFUNCTION void (*DLLvm_VectorToMatrix)(matrix *m, vector *fvec, vector *uvec, vector *rvec);
DMFCFUNCTION void (*DLLvm_VectorAngleToMatrix)(matrix *m, vector *v, angle a);
DMFCFUNCTION void (*DLLvm_SinCos)(angle, float *, float *);
DMFCFUNCTION float (*DLLvm_GetSlope)(float, float, float, float);
DMFCFUNCTION void (*DLLvm_GetPerp)(vector *n, vector *a, vector *b, vector *c);
DMFCFUNCTION float (*DLLvm_GetNormal)(vector *n, vector *v0, vector *v1, vector *v2);
DMFCFUNCTION float (*DLLvm_VectorDistance)(const vector *a, const vector *b);
DMFCFUNCTION float (*DLLvm_VectorDistanceQuick)(vector *a, vector *b);
DMFCFUNCTION float (*DLLvm_GetNormalizedDir)(vector *dest, vector *end, vector *start);
DMFCFUNCTION float (*DLLvm_GetNormalizedDirFast)(vector *dest, vector *end, vector *start);
DMFCFUNCTION angvec *(*DLLvm_ExtractAnglesFromMatrix)(angvec *a, matrix *m);
DMFCFUNCTION angle (*DLLvm_DeltaAngVec)(vector *v0, vector *v1, vector *fvec);
DMFCFUNCTION angle (*DLLvm_DeltaAngVecNorm)(vector *v0, vector *v1, vector *fvec);
DMFCFUNCTION float (*DLLvm_DistToPlane)(vector *checkp, vector *norm, vector *planep);
DMFCFUNCTION float (*DLLvm_CalcDetValue)(matrix *det);
DMFCFUNCTION void (*DLLvm_MakeInverseMatrix)(matrix *dest);
DMFCFUNCTION void (*DLLvm_SinCosToMatrix)(matrix *m, float sinp, float cosp, float sinb, float cosb, float sinh,
                                          float cosh);
DMFCFUNCTION float (*DLLvm_GetCentroid)(vector *centroid, vector *src, int nv);
DMFCFUNCTION void (*DLLvm_MakeRandomVector)(vector *vec);
DMFCFUNCTION float (*DLLvm_ComputeBoundingSphere)(vector *center, vector *vecs, int num_verts);
DMFCFUNCTION float (*DLLvm_GetCentroidFast)(vector *centroid, vector *src, int nv);
DMFCFUNCTION int (*DLLRenderHUDGetTextLineWidth)(const char *string);
DMFCFUNCTION int (*DLLRenderHUDGetTextHeight)(const char *string);
DMFCFUNCTION void (*DLLStartFrame)(int x, int y, int x2, int y2, bool clear);
DMFCFUNCTION void (*DLLEndFrame)(void);
DMFCFUNCTION void (*DLLResetFacings)(void);
DMFCFUNCTION void (*DLLGameRenderWorld)(object *viewer, vector *viewer_eye, int viewer_roomnum, matrix *viewer_orient,
                                        float zoom, bool rear_view);
DMFCFUNCTION bool (*DLLGetFrameParameters)(int *x1, int *y1, int *x2, int *y2);
DMFCFUNCTION void (*DLLrend_SetZBufferState)(sbyte state);
DMFCFUNCTION void (*DLLrend_SetLighting)(light_state);
DMFCFUNCTION void (*DLLrend_SetColorModel)(color_model);
DMFCFUNCTION void (*DLLrend_SetTextureType)(texture_type);
DMFCFUNCTION void (*DLLrend_DrawPolygon)(int handle, g3Point **p, int nv, int map_type);
DMFCFUNCTION void (*DLLrend_SetMipState)(sbyte);
DMFCFUNCTION void (*DLLrend_SetFogState)(sbyte on);
DMFCFUNCTION void (*DLLrend_SetFiltering)(sbyte state);
DMFCFUNCTION void (*DLLrend_SetOverlayMap)(int handle);
DMFCFUNCTION void (*DLLrend_SetOverlayType)(ubyte type);
DMFCFUNCTION void (*DLLrend_ClearScreen)(ddgr_color color);
DMFCFUNCTION void (*DLLrend_SetPixel)(ddgr_color color, int x, int y);
DMFCFUNCTION ddgr_color (*DLLrend_GetPixel)(int x, int y);
DMFCFUNCTION void (*DLLrend_FillCircle)(ddgr_color col, int x, int y, int rad);
DMFCFUNCTION void (*DLLrend_DrawCircle)(int x, int y, int rad);
DMFCFUNCTION void (*DLLrend_SetAlphaType)(sbyte);
DMFCFUNCTION void (*DLLrend_SetAlphaValue)(ubyte val);
DMFCFUNCTION void (*DLLrend_SetWrapType)(wrap_type val);
DMFCFUNCTION void (*DLLrend_SetZBias)(float z_bias);
DMFCFUNCTION void (*DLLrend_SetZBufferWriteMask)(int state);
DMFCFUNCTION void (*DLLrend_GetLFBLock)(renderer_lfb *lfb);
DMFCFUNCTION void (*DLLrend_ReleaseLFBLock)(renderer_lfb *lfb);
DMFCFUNCTION void (*DLLrend_DrawLFBBitmap)(int sx, int sy, int w, int h, int dx, int dy, ushort *data, int rowsize);
DMFCFUNCTION void (*DLLrend_DrawSpecialLine)(g3Point *p0, g3Point *p1);
DMFCFUNCTION int (*DLLfvi_FindIntersection)(fvi_query *fq, fvi_info *hit_data, bool no_subdivision);
DMFCFUNCTION int (*DLLfvi_QuickDistFaceList)(int init_room_index, vector *pos, float rad,
                                             fvi_face_room_list *quick_fr_list, int max_elements);
DMFCFUNCTION int (*DLLfvi_QuickDistCellList)(int init_cell_index, vector *pos, float rad, int *quick_cell_list,
                                             int max_elements);
DMFCFUNCTION int (*DLLfvi_QuickDistObjectList)(vector *pos, int init_roomnum, float rad, short *object_index_list,
                                               int max_elements, bool f_lightmap_only, bool f_only_players_and_ais,
                                               bool f_include_non_collide_objects, bool f_stop_at_closed_doors);
DMFCFUNCTION bool (*DLLfvi_QuickRoomCheck)(vector *pos, room *cur_room, bool try_again);
DMFCFUNCTION bool (*DLLtaunt_AreEnabled)(void);
DMFCFUNCTION void (*DLLtaunt_Enable)(bool enable);
DMFCFUNCTION int (*GetPlayerRankIndex)(int pnum, char *rankbuf);
DMFCFUNCTION int (*DLLVisEffectAllocate)(void);
DMFCFUNCTION int (*DLLVisEffectFree)(int visnum);
DMFCFUNCTION int (*DLLVisEffectInitType)(vis_effect *vis);
DMFCFUNCTION int (*DLLVisEffectCreate)(ubyte type, ubyte id, int roomnum, vector *pos);
DMFCFUNCTION void (*DLLVisEffectLink)(int visnum, int roomnum);
DMFCFUNCTION void (*DLLVisEffectUnlink)(int visnum);
DMFCFUNCTION void (*DLLVisEffectRelink)(int visnum, int newroomnum);
DMFCFUNCTION void (*DLLVisEffectDelete)(int visnum);
DMFCFUNCTION void (*DLLCreateRandomSparks)(int num_sparks, vector *pos, int roomnum, int which_index,
                                           float force_scalar);
DMFCFUNCTION void (*DLLCreateRandomLineSparks)(int num_sparks, vector *pos, int roomnum, ushort color,
                                               float force_scalar);
DMFCFUNCTION int (*DLLVisEffectCreateControlled)(ubyte type, object *parent, ubyte id, int roomnum, vector *pos,
                                                 float lifetime, vector *velocity, int phys_flags, float size,
                                                 float mass, float drag, bool isreal);
DMFCFUNCTION void (*DLLCreateRandomParticles)(int num_sparks, vector *pos, int roomnum, int bm_handle, float size,
                                              float life);
DMFCFUNCTION void (*DLLAttachRandomNapalmEffectsToObject)(object *obj);
DMFCFUNCTION void (*DLLInitObjectScripts)(object *objp, bool do_evt_created);
DMFCFUNCTION void (*DLLg3_StartFrame)(vector *view_pos, matrix *view_matrix, float zoom);
DMFCFUNCTION void (*DLLg3_EndFrame)(void);
DMFCFUNCTION void (*DLLg3_GetViewPosition)(vector *vp);
DMFCFUNCTION void (*DLLg3_GetViewMatrix)(matrix *mat);
DMFCFUNCTION void (*DLLg3_GetUnscaledMatrix)(matrix *mat);
DMFCFUNCTION void (*DLLg3_StartInstanceMatrix)(vector *pos, matrix *orient);
DMFCFUNCTION void (*DLLg3_StartInstanceAngles)(vector *pos, angvec *angles);
DMFCFUNCTION void (*DLLg3_DoneInstance)();
DMFCFUNCTION bool (*DLLg3_CheckNormalFacing)(vector *v, vector *norm);
DMFCFUNCTION ubyte (*DLLg3_RotatePoint)(g3Point *dest, vector *src);
DMFCFUNCTION void (*DLLg3_ProjectPoint)(g3Point *point);
DMFCFUNCTION float (*DLLg3_CalcPointDepth)(vector *pnt);
DMFCFUNCTION void (*DLLg3_Point2Vec)(vector *v, short sx, short sy);
DMFCFUNCTION ubyte (*DLLg3_CodePoint)(g3Point *point);
DMFCFUNCTION vector *(*DLLg3_RotateDeltaX)(vector *dest, float dx);
DMFCFUNCTION vector *(*DLLg3_RotateDeltaY)(vector *dest, float dy);
DMFCFUNCTION vector *(*DLLg3_RotateDeltaZ)(vector *dest, float dz);
DMFCFUNCTION vector *(*DLLg3_RotateDeltaVec)(vector *dest, vector *src);
DMFCFUNCTION ubyte (*DLLg3_AddDeltaVec)(g3Point *dest, g3Point *src, vector *deltav);
DMFCFUNCTION int (*DLLg3_DrawPoly)(int nv, g3Point **pointlist, int bm, int map_type, g3Codes *clip_codes);
DMFCFUNCTION void (*DLLg3_DrawSphere)(ddgr_color color, g3Point *pnt, float rad);
DMFCFUNCTION void (*DLLg3_CheckAndDrawPoly)(int nv, g3Point **pointlist, int bm, vector *norm, vector *pnt);
DMFCFUNCTION void (*DLLg3_DrawLine)(ddgr_color color, g3Point *p0, g3Point *p1);
DMFCFUNCTION void (*DLLg3_DrawBitmap)(vector *pos, float width, float height, int bm, int color);
DMFCFUNCTION void (*DLLg3_DrawRotatedBitmap)(vector *pos, angle rot_angle, float width, float height, int bm,
                                             int color);
DMFCFUNCTION void (*DLLg3_DrawBox)(ddgr_color color, g3Point *pnt, float rad);
DMFCFUNCTION void (*DLLg3_SetCustomClipPlane)(ubyte state, vector *pnt, vector *normal);
DMFCFUNCTION void (*DLLg3_SetFarClipZ)(float z);
DMFCFUNCTION g3Point **(*DLLg3_ClipPolygon)(g3Point **pointlist, int *nv, g3Codes *cc);
DMFCFUNCTION void (*DLLg3_FreeTempPoints)(g3Point **pointlist, int nv);
DMFCFUNCTION void (*DLLg3_GetMatrixScale)(vector *matrix_scale);
DMFCFUNCTION void (*DLLg3_SetTriangulationTest)(int state);
DMFCFUNCTION void (*DLLg3_DrawSpecialLine)(g3Point *p0, g3Point *p1);
DMFCFUNCTION void (*DLLg3_DrawPlanarRotatedBitmap)(vector *pos, vector *norm, angle rot_angle, float width,
                                                   float height, int bm);
DMFCFUNCTION void (*DLLPlayerStopSounds)(int slot);
DMFCFUNCTION int (*DLLFindArg)(const char *which);
DMFCFUNCTION int (*DLLFireWeaponFromObject)(object *obj, int weapon_num, int gun_num, bool f_force_forward,
                                            bool f_force_target);
DMFCFUNCTION int (*DLLCreateAndFireWeapon)(vector *pos, vector *dir, object *parent, int weapon_num);
DMFCFUNCTION void (*DLLSelectNextCameraView)(int window);
DMFCFUNCTION bool (*Inven_Add)(Inventory *inven, int type, int id, object *parent, int aux_type, int aux_id, int flags,
                               char *description);
DMFCFUNCTION bool (*Inven_AddObject)(Inventory *inven, int object_handle, int flags, char *description);
DMFCFUNCTION bool (*Inven_AddCounterMeasure)(Inventory *inven, int id, int aux_type, int aux_id, int flags,
                                             char *description);
DMFCFUNCTION bool (*Inven_Remove)(Inventory *inven, int type, int id);
DMFCFUNCTION bool (*Inven_Use)(Inventory *inven, int type, int id, object *parent);
DMFCFUNCTION bool (*Inven_UseObjHandle)(Inventory *inven, int objhandle, object *parent);
DMFCFUNCTION int (*Inven_Size)(Inventory *inven);
DMFCFUNCTION bool (*Inven_CheckItem)(Inventory *inven, int type, int id);
DMFCFUNCTION void (*Inven_Reset)(Inventory *inven, bool in_game, int reset_stage);
DMFCFUNCTION void (*Inven_ResetPos)(Inventory *inven);
DMFCFUNCTION void (*Inven_NextPos)(Inventory *inven, bool skip);
DMFCFUNCTION void (*Inven_PrevPos)(Inventory *inven, bool skip);
DMFCFUNCTION bool (*Inven_GetPosTypeID)(Inventory *inven, int &type, int &id);
DMFCFUNCTION bool (*Inven_GetAuxPosTypeID)(Inventory *inven, int &type, int &id);
DMFCFUNCTION char *(*Inven_GetPosDescription)(Inventory *inven);
DMFCFUNCTION char *(*Inven_GetPosIconName)(Inventory *inven);
DMFCFUNCTION char *(*Inven_GetPosName)(Inventory *inven);
DMFCFUNCTION bool (*Inven_GetPosInfo)(Inventory *inven, ushort &iflags, int &flags);
DMFCFUNCTION int (*Inven_GetPosCount)(Inventory *inven);
DMFCFUNCTION bool (*Inven_AtBeginning)(Inventory *inven);
DMFCFUNCTION bool (*Inven_AtEnd)(Inventory *inven);
DMFCFUNCTION void (*Inven_GotoPos)(Inventory *inven, int newpos);
DMFCFUNCTION void (*Inven_GotoPosTypeID)(Inventory *inven, int type, int id);
DMFCFUNCTION bool (*Inven_UsePos)(Inventory *inven, object *parent);
DMFCFUNCTION int (*Inven_GetPos)(Inventory *inven);
DMFCFUNCTION void (*Inven_ValidatePos)(Inventory *inven, bool forward);
DMFCFUNCTION bool (*Inven_IsSelectable)(Inventory *inven);
DMFCFUNCTION bool (*Inven_IsUsable)(Inventory *inven);
DMFCFUNCTION int (*Inven_GetTypeIDCount)(Inventory *inven, int type, int id);
DMFCFUNCTION bool (*Inven_FindPos)(Inventory *inven, int type, int id);
DMFCFUNCTION int (*Inven_GetInventoryItemList)(Inventory *inven, tInvenList *list, int max_amount, int *cur_sel);
