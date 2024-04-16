#ifndef MULTI_WORLD_STATE_H
#define MULTI_WORLD_STATE_H

#define WS_ROOM_WIND 0
#define WS_ROOM_FOG 1
#define WS_ROOM_LIGHTING 2
#define WS_ROOM_TEXTURE 3
#define WS_ROOM_PORTAL_RENDER 4
#define WS_WEATHER 5
#define WS_WAYPOINT 6
#define WS_DOOR 7
#define WS_LIGHT_DISTANCE 8
#define WS_SPEW 9
#define WS_PLAYERBF 10
#define WS_ROOM_DAMAGE 11
#define WS_ROOM_REFUEL 12
#define WS_LIGHT_COLOR 13
#define WS_ROOM_GLASS 14
#define WS_BUDDYBOTS 15
#define WS_BUDDYBOTUPDATE 16
#define WS_ROOM_PORTAL_BLOCK 17
#define WS_LEVELGOAL 18
#define WS_ROOM_GOALSPECFLAG 19
#define WS_OBJECT_ATTACH 20
#define WS_NO_RENDER 21
#define WS_OBJECT_PHYS_FLAGS 22
#define WS_MOTD 23
#define WS_END 100

// WEATHER!
// WAYPOINT!

// Room change flags
#define RCF_WIND 1
#define RCF_FOG (1 << 1)
#define RCF_AMBIENT_SOUND (1 << 2)
#define RCF_LIGHTING (1 << 3)
#define RCF_CHANGING_WIND_FOG (1 << 4)
#define RCF_TEXTURE (1 << 5)
#define RCF_PORTAL_RENDER (1 << 6)
#define RCF_DAMAGE (1 << 7)
#define RCF_REFUEL (1 << 8)
#define RCF_PORTAL_BLOCK (1 << 9)
#define RCF_GOALSPECIAL_FLAGS (1 << 10) // Goal flags/special 1-6 flags have changed

// Object change flags
#define OCF_NO_RENDER 1
#define OCF_LIGHT_DISTANCE (1 << 1)
#define OCF_LIGHT_COLOR (1 << 2)
#define OCF_PHYS_FLAGS (1 << 3)

#endif