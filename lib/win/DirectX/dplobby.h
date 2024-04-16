/*==========================================================================;
 *
 *  Copyright (C) 1996-1997 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       dplobby.h
 *  Content:    DirectPlayLobby include file
 *@@BEGIN_MSINTERNAL
 *  History:
 *	Date		By		Reason
 *	===========	=======	==========
 *	4/13/96		myronth	Created it
 *	6/20/96		myronth	Cleaned it up
 *	6/24/96		myronth	More cleanup
 *	7/1/96		myronth	Changed DPLERR_ to DPERR_
 *  7/9/96      ajayj   Cleaned up formatting to match dplay.h
 *  7/15/96     ajayj   Added DPADDRESS chunk tags for dplaunch
 *                      sample app.
 *	7/16/96		kipo	changed address types to be GUIDs instead of 4CC
 *  8/1/96      sohailm Added IDirectPlayLobby interface macros
 *  8/10/96     sohailm Bug#2227: declared pointers in callbacks as const.
 *  8/15/96     sohailm Added type definition for LPCDPLCONNECTION
 *                      Changed LPGUIDs to REFGUIDs in IDirectPlayLobby interface 
 *                      Made function parameters constant where appropriate
 *	8/16/96		andyco	took out some const and ref stuff for ship
 *	8/16/96		myronth	Added SetConnectionSettings & Connect methods
 *  8/21/96     ajayj   replace tabs with spaces
 *	8/21/96		kipo	Added DPCOMPORTADDRESS structure
 *	9/5/96		myronth	Wrapped DPLCAPS with MSINTERNAL
 *	10/23/96	myronth	Added first cut of client/server methods
 *	10/25/96	myronth	Added DX5 methods
 *	12/13/96	myronth	Changed DPLAD_SYSTEM to DPLMSG_SYSTEM
 *	1/30/97		myronth	Added MSINTERNAL's to DX5 stuff for meltdown
 *	2/12/97		myronth	Removed meltdown changes
 *	2/12/97		myronth	Mass DX5 changes
 *	2/18/97		myronth	Implemented GetObjectCaps
 *	3/5/97		andyco	added DPAID_TotalSize
 *	3/12/97		myronth	Added DPAID_LobbyProvider chunk
 *	3/17/97		kipo	Added data structures for CreateCompoundAddress() and
 *						added guids for Unicode phone numbers and IP addresses
 *	3/24/97		kipo	Added support for IDirectPlayLobby2 interface
 *	4/11/97		myronth	Added CLSID_DirectPlayLobby clsid
 *	5/8/97		myronth	Moved DPLCONNECTION structure definition to dplay.h
 *  5/21/97		ajayj	Added structures and definitions for standard lobby 
 *						messages
 *  5/23/97		ajayj	Added DPLDATA_PLAYERGUID structure
 *  5/30/97		ajayj	Added DPLPROPERTY_MessagesSupported GUID
 *	6/7/97		myronth	Fixed CreateCompoundAddress macros
 *	6/6/97		kipo	Added "FAR" to all structures
 *	8/19/97		myronth	Added DPLMSG_NEWSESSIONHOST structure
 *	11/13/97	myronth	Added guidInstance to lobby system messages (#10944)
 *	12/2/97		myronth	Added Register/UnregisterApplication
 *	12/4/97		myronth	Added ConnectEx
 *	1/20/98		myronth	Added WaitForConnectionSettings
 *	1/29/98		sohailm	Added DPAID_INetPort.
 *	2/11/98		a-peterz Clarify that DPAID_INetPort value is a word (#18210)
 *	4/15/98		a-peterz Fix IDirectPlayLobby_CreateCompoundAddress C++ version
 *@@END_MSINTERNAL
 ***************************************************************************/
#ifndef __DPLOBBY_INCLUDED__
#define __DPLOBBY_INCLUDED__

#include "dplay.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * GUIDS used by DirectPlay objects
 */

/* {AF465C71-9588-11cf-A020-00AA006157AC} */
DEFINE_GUID(IID_IDirectPlayLobby, 0xaf465c71, 0x9588, 0x11cf, 0xa0, 0x20, 0x0, 0xaa, 0x0, 0x61, 0x57, 0xac);
/* {26C66A70-B367-11cf-A024-00AA006157AC} */
DEFINE_GUID(IID_IDirectPlayLobbyA, 0x26c66a70, 0xb367, 0x11cf, 0xa0, 0x24, 0x0, 0xaa, 0x0, 0x61, 0x57, 0xac);
/* {0194C220-A303-11d0-9C4F-00A0C905425E} */
DEFINE_GUID(IID_IDirectPlayLobby2, 0x194c220, 0xa303, 0x11d0, 0x9c, 0x4f, 0x0, 0xa0, 0xc9, 0x5, 0x42, 0x5e);
/* {1BB4AF80-A303-11d0-9C4F-00A0C905425E} */
DEFINE_GUID(IID_IDirectPlayLobby2A, 0x1bb4af80, 0xa303, 0x11d0, 0x9c, 0x4f, 0x0, 0xa0, 0xc9, 0x5, 0x42, 0x5e);
/* {2DB72490-652C-11d1-A7A8-0000F803ABFC} */
DEFINE_GUID(IID_IDirectPlayLobby3, 0x2db72490, 0x652c, 0x11d1, 0xa7, 0xa8, 0x0, 0x0, 0xf8, 0x3, 0xab, 0xfc);
/* {2DB72491-652C-11d1-A7A8-0000F803ABFC} */
DEFINE_GUID(IID_IDirectPlayLobby3A, 0x2db72491, 0x652c, 0x11d1, 0xa7, 0xa8, 0x0, 0x0, 0xf8, 0x3, 0xab, 0xfc);
/* {2FE8F810-B2A5-11d0-A787-0000F803ABFC} */
DEFINE_GUID(CLSID_DirectPlayLobby, 0x2fe8f810, 0xb2a5, 0x11d0, 0xa7, 0x87, 0x0, 0x0, 0xf8, 0x3, 0xab, 0xfc);


/****************************************************************************
 *
 * IDirectPlayLobby Structures
 *
 * Various structures used to invoke DirectPlayLobby.
 *
 ****************************************************************************/

typedef struct IDirectPlayLobby     FAR *LPDIRECTPLAYLOBBY;
typedef struct IDirectPlayLobby     FAR *LPDIRECTPLAYLOBBYA;
typedef struct IDirectPlayLobby     IDirectPlayLobbyA;

typedef struct IDirectPlayLobby2    FAR *LPDIRECTPLAYLOBBY2;
typedef struct IDirectPlayLobby2    FAR *LPDIRECTPLAYLOBBY2A;
typedef struct IDirectPlayLobby2    IDirectPlayLobby2A;

typedef struct IDirectPlayLobby3    FAR *LPDIRECTPLAYLOBBY3;
typedef struct IDirectPlayLobby3    FAR *LPDIRECTPLAYLOBBY3A;
typedef struct IDirectPlayLobby3    IDirectPlayLobby3A;


/*
 * DPLAPPINFO
 * Used to hold information about a registered DirectPlay
 * application
 */
typedef struct DPLAPPINFO
{
    DWORD       dwSize;             // Size of this structure
    GUID        guidApplication;    // GUID of the Application
    union
    {
        LPSTR   lpszAppNameA;       // Pointer to the Application Name
        LPWSTR  lpszAppName;
    };

} DPLAPPINFO, FAR *LPDPLAPPINFO;

/*
 * LPCDPLAPPINFO
 * A constant pointer to DPLAPPINFO
 */
typedef const DPLAPPINFO FAR *LPCDPLAPPINFO;

/*
 * DPCOMPOUNDADDRESSELEMENT
 *
 * An array of these is passed to CreateCompoundAddresses()
 */
typedef struct DPCOMPOUNDADDRESSELEMENT
{
    GUID                guidDataType;
    DWORD               dwDataSize;
	LPVOID				lpData;
} DPCOMPOUNDADDRESSELEMENT, FAR *LPDPCOMPOUNDADDRESSELEMENT;

/*
 * LPCDPCOMPOUNDADDRESSELEMENT
 * A constant pointer to DPCOMPOUNDADDRESSELEMENT
 */
typedef const DPCOMPOUNDADDRESSELEMENT FAR *LPCDPCOMPOUNDADDRESSELEMENT;

/*
 * LPDPAPPLICATIONDESC
 * Used to register a DirectPlay application
 */
typedef struct DPAPPLICATIONDESC
{
    DWORD       dwSize;
    DWORD       dwFlags;
    union
    {
        LPSTR       lpszApplicationNameA;
        LPWSTR      lpszApplicationName;
    };
    GUID        guidApplication;
    union
    {
        LPSTR       lpszFilenameA;
        LPWSTR      lpszFilename;
    };
    union
    {
        LPSTR       lpszCommandLineA;
        LPWSTR      lpszCommandLine;
    };
    union
    {
        LPSTR       lpszPathA;
        LPWSTR      lpszPath;
    };
    union
    {
        LPSTR       lpszCurrentDirectoryA;
        LPWSTR      lpszCurrentDirectory;
    };
    LPSTR       lpszDescriptionA;
    LPWSTR      lpszDescriptionW;
} DPAPPLICATIONDESC, *LPDPAPPLICATIONDESC;


/****************************************************************************
 *
 * Enumeration Method Callback Prototypes
 *
 ****************************************************************************/

/*
 * Callback for EnumAddress()
 */
typedef BOOL (FAR PASCAL *LPDPENUMADDRESSCALLBACK)(
    REFGUID         guidDataType,
    DWORD           dwDataSize,
    LPCVOID         lpData,
    LPVOID          lpContext);

/*
 * Callback for EnumAddressTypes()
 */
typedef BOOL (FAR PASCAL *LPDPLENUMADDRESSTYPESCALLBACK)(
    REFGUID         guidDataType,
    LPVOID          lpContext,
    DWORD           dwFlags);

/*
 * Callback for EnumLocalApplications()
 */
typedef BOOL (FAR PASCAL * LPDPLENUMLOCALAPPLICATIONSCALLBACK)(
    LPCDPLAPPINFO   lpAppInfo,
    LPVOID          lpContext,
    DWORD           dwFlags);


/****************************************************************************
 *
 * DirectPlayLobby API Prototypes
 *
 ****************************************************************************/
#ifdef UNICODE
#define DirectPlayLobbyCreate   DirectPlayLobbyCreateW
#else
#define DirectPlayLobbyCreate   DirectPlayLobbyCreateA
#endif /* UNICODE */

extern HRESULT WINAPI DirectPlayLobbyCreateW(LPGUID, LPDIRECTPLAYLOBBY *, IUnknown *, LPVOID, DWORD );
extern HRESULT WINAPI DirectPlayLobbyCreateA(LPGUID, LPDIRECTPLAYLOBBYA *, IUnknown *, LPVOID, DWORD );


/****************************************************************************
 *
 * IDirectPlayLobby (and IDirectPlayLobbyA) Interface
 *
 ****************************************************************************/
#undef INTERFACE
#define INTERFACE IDirectPlayLobby
DECLARE_INTERFACE_( IDirectPlayLobby, IUnknown )
{
    /*  IUnknown Methods	*/
    STDMETHOD(QueryInterface)       (THIS_ REFIID riid, LPVOID * ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef)        (THIS) PURE;
    STDMETHOD_(ULONG,Release)       (THIS) PURE;

    /*  IDirectPlayLobby Methods	*/
    STDMETHOD(Connect)              (THIS_ DWORD, LPDIRECTPLAY2 *, IUnknown FAR *) PURE;
    STDMETHOD(CreateAddress)        (THIS_ REFGUID, REFGUID, LPCVOID, DWORD, LPVOID, LPDWORD) PURE;
    STDMETHOD(EnumAddress)          (THIS_ LPDPENUMADDRESSCALLBACK, LPCVOID, DWORD, LPVOID) PURE;
    STDMETHOD(EnumAddressTypes)     (THIS_ LPDPLENUMADDRESSTYPESCALLBACK, REFGUID, LPVOID, DWORD) PURE;
    STDMETHOD(EnumLocalApplications)(THIS_ LPDPLENUMLOCALAPPLICATIONSCALLBACK, LPVOID, DWORD) PURE;
    STDMETHOD(GetConnectionSettings)(THIS_ DWORD, LPVOID, LPDWORD) PURE;
    STDMETHOD(ReceiveLobbyMessage)  (THIS_ DWORD, DWORD, LPDWORD, LPVOID, LPDWORD) PURE;
    STDMETHOD(RunApplication)       (THIS_ DWORD, LPDWORD, LPDPLCONNECTION, HANDLE) PURE;
    STDMETHOD(SendLobbyMessage)     (THIS_ DWORD, DWORD, LPVOID, DWORD) PURE;
    STDMETHOD(SetConnectionSettings)(THIS_ DWORD, DWORD, LPDPLCONNECTION) PURE;
    STDMETHOD(SetLobbyMessageEvent) (THIS_ DWORD, DWORD, HANDLE) PURE;

};

/****************************************************************************
 *
 * IDirectPlayLobby2 (and IDirectPlayLobby2A) Interface
 *
 ****************************************************************************/
#undef INTERFACE
#define INTERFACE IDirectPlayLobby2
DECLARE_INTERFACE_( IDirectPlayLobby2, IDirectPlayLobby )
{
    /*  IUnknown Methods	*/
    STDMETHOD(QueryInterface)       (THIS_ REFIID riid, LPVOID * ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef)        (THIS) PURE;
    STDMETHOD_(ULONG,Release)       (THIS) PURE;

    /*  IDirectPlayLobby Methods	*/
    STDMETHOD(Connect)              (THIS_ DWORD, LPDIRECTPLAY2 *, IUnknown FAR *) PURE;
    STDMETHOD(CreateAddress)        (THIS_ REFGUID, REFGUID, LPCVOID, DWORD, LPVOID, LPDWORD) PURE;
    STDMETHOD(EnumAddress)          (THIS_ LPDPENUMADDRESSCALLBACK, LPCVOID, DWORD, LPVOID) PURE;
    STDMETHOD(EnumAddressTypes)     (THIS_ LPDPLENUMADDRESSTYPESCALLBACK, REFGUID, LPVOID, DWORD) PURE;
    STDMETHOD(EnumLocalApplications)(THIS_ LPDPLENUMLOCALAPPLICATIONSCALLBACK, LPVOID, DWORD) PURE;
    STDMETHOD(GetConnectionSettings)(THIS_ DWORD, LPVOID, LPDWORD) PURE;
    STDMETHOD(ReceiveLobbyMessage)  (THIS_ DWORD, DWORD, LPDWORD, LPVOID, LPDWORD) PURE;
    STDMETHOD(RunApplication)       (THIS_ DWORD, LPDWORD, LPDPLCONNECTION, HANDLE) PURE;
    STDMETHOD(SendLobbyMessage)     (THIS_ DWORD, DWORD, LPVOID, DWORD) PURE;
    STDMETHOD(SetConnectionSettings)(THIS_ DWORD, DWORD, LPDPLCONNECTION) PURE;
    STDMETHOD(SetLobbyMessageEvent) (THIS_ DWORD, DWORD, HANDLE) PURE;

    /*  IDirectPlayLobby2 Methods	*/
    STDMETHOD(CreateCompoundAddress)(THIS_ LPCDPCOMPOUNDADDRESSELEMENT,DWORD,LPVOID,LPDWORD) PURE;
};

/****************************************************************************
 *
 * IDirectPlayLobby3 (and IDirectPlayLobby3A) Interface
 *
 ****************************************************************************/
#undef INTERFACE
#define INTERFACE IDirectPlayLobby3
DECLARE_INTERFACE_( IDirectPlayLobby3, IDirectPlayLobby )
{
    /*  IUnknown Methods	*/
    STDMETHOD(QueryInterface)       (THIS_ REFIID riid, LPVOID * ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef)        (THIS) PURE;
    STDMETHOD_(ULONG,Release)       (THIS) PURE;

    /*  IDirectPlayLobby Methods	*/
    STDMETHOD(Connect)              (THIS_ DWORD, LPDIRECTPLAY2 *, IUnknown FAR *) PURE;
    STDMETHOD(CreateAddress)        (THIS_ REFGUID, REFGUID, LPCVOID, DWORD, LPVOID, LPDWORD) PURE;
    STDMETHOD(EnumAddress)          (THIS_ LPDPENUMADDRESSCALLBACK, LPCVOID, DWORD, LPVOID) PURE;
    STDMETHOD(EnumAddressTypes)     (THIS_ LPDPLENUMADDRESSTYPESCALLBACK, REFGUID, LPVOID, DWORD) PURE;
    STDMETHOD(EnumLocalApplications)(THIS_ LPDPLENUMLOCALAPPLICATIONSCALLBACK, LPVOID, DWORD) PURE;
    STDMETHOD(GetConnectionSettings)(THIS_ DWORD, LPVOID, LPDWORD) PURE;
    STDMETHOD(ReceiveLobbyMessage)  (THIS_ DWORD, DWORD, LPDWORD, LPVOID, LPDWORD) PURE;
    STDMETHOD(RunApplication)       (THIS_ DWORD, LPDWORD, LPDPLCONNECTION, HANDLE) PURE;
    STDMETHOD(SendLobbyMessage)     (THIS_ DWORD, DWORD, LPVOID, DWORD) PURE;
    STDMETHOD(SetConnectionSettings)(THIS_ DWORD, DWORD, LPDPLCONNECTION) PURE;
    STDMETHOD(SetLobbyMessageEvent) (THIS_ DWORD, DWORD, HANDLE) PURE;

    /*  IDirectPlayLobby2 Methods	*/
    STDMETHOD(CreateCompoundAddress)(THIS_ LPCDPCOMPOUNDADDRESSELEMENT,DWORD,LPVOID,LPDWORD) PURE;

    /*  IDirectPlayLobby3 Methods	*/
    STDMETHOD(ConnectEx)            (THIS_ DWORD, REFIID, LPVOID *, IUnknown FAR *) PURE;
    STDMETHOD(RegisterApplication)  (THIS_ DWORD, LPDPAPPLICATIONDESC) PURE;
    STDMETHOD(UnregisterApplication)(THIS_ DWORD, REFGUID) PURE;
    STDMETHOD(WaitForConnectionSettings)(THIS_ DWORD) PURE;
};

/****************************************************************************
 *
 * IDirectPlayLobby interface macros
 *
 ****************************************************************************/

#if !defined(__cplusplus) || defined(CINTERFACE)

#define IDirectPlayLobby_QueryInterface(p,a,b)              (p)->lpVtbl->QueryInterface(p,a,b)
#define IDirectPlayLobby_AddRef(p)                          (p)->lpVtbl->AddRef(p)
#define IDirectPlayLobby_Release(p)                         (p)->lpVtbl->Release(p)
#define IDirectPlayLobby_Connect(p,a,b,c)                   (p)->lpVtbl->Connect(p,a,b,c)
#define IDirectPlayLobby_ConnectEx(p,a,b,c,d)               (p)->lpVtbl->ConnectEx(p,a,b,c,d)
#define IDirectPlayLobby_CreateAddress(p,a,b,c,d,e,f)       (p)->lpVtbl->CreateAddress(p,a,b,c,d,e,f)
#define IDirectPlayLobby_CreateCompoundAddress(p,a,b,c,d)   (p)->lpVtbl->CreateCompoundAddress(p,a,b,c,d)
#define IDirectPlayLobby_EnumAddress(p,a,b,c,d)             (p)->lpVtbl->EnumAddress(p,a,b,c,d)
#define IDirectPlayLobby_EnumAddressTypes(p,a,b,c,d)        (p)->lpVtbl->EnumAddressTypes(p,a,b,c,d)
#define IDirectPlayLobby_EnumLocalApplications(p,a,b,c)     (p)->lpVtbl->EnumLocalApplications(p,a,b,c)
#define IDirectPlayLobby_GetConnectionSettings(p,a,b,c)     (p)->lpVtbl->GetConnectionSettings(p,a,b,c)
#define IDirectPlayLobby_ReceiveLobbyMessage(p,a,b,c,d,e)   (p)->lpVtbl->ReceiveLobbyMessage(p,a,b,c,d,e)
#define IDirectPlayLobby_RegisterApplication(p,a,b)         (p)->lpVtbl->RegisterApplication(p,a,b)
#define IDirectPlayLobby_RunApplication(p,a,b,c,d)          (p)->lpVtbl->RunApplication(p,a,b,c,d)
#define IDirectPlayLobby_SendLobbyMessage(p,a,b,c,d)        (p)->lpVtbl->SendLobbyMessage(p,a,b,c,d)
#define IDirectPlayLobby_SetConnectionSettings(p,a,b,c)		(p)->lpVtbl->SetConnectionSettings(p,a,b,c)
#define IDirectPlayLobby_SetLobbyMessageEvent(p,a,b,c)	    (p)->lpVtbl->SetLobbyMessageEvent(p,a,b,c)
#define IDirectPlayLobby_UnregisterApplication(p,a,b)       (p)->lpVtbl->UnregisterApplication(p,a,b)
#define IDirectPlayLobby_WaitForConnectionSettings(p,a)     (p)->lpVtbl->WaitForConnectionSettings(p,a)

#else /* C++ */

#define IDirectPlayLobby_QueryInterface(p,a,b)              (p)->QueryInterface(a,b)
#define IDirectPlayLobby_AddRef(p)                          (p)->AddRef()
#define IDirectPlayLobby_Release(p)                         (p)->Release()
#define IDirectPlayLobby_Connect(p,a,b,c)                   (p)->Connect(a,b,c)
#define IDirectPlayLobby_ConnectEx(p,a,b,c,d)               (p)->ConnectEx(a,b,c,d)
#define IDirectPlayLobby_CreateAddress(p,a,b,c,d,e,f)       (p)->CreateAddress(a,b,c,d,e,f)
#define IDirectPlayLobby_CreateCompoundAddress(p,a,b,c,d)   (p)->CreateCompoundAddress(a,b,c,d)
#define IDirectPlayLobby_EnumAddress(p,a,b,c,d)             (p)->EnumAddress(a,b,c,d)
#define IDirectPlayLobby_EnumAddressTypes(p,a,b,c,d)        (p)->EnumAddressTypes(a,b,c,d)
#define IDirectPlayLobby_EnumLocalApplications(p,a,b,c)     (p)->EnumLocalApplications(a,b,c)
#define IDirectPlayLobby_GetConnectionSettings(p,a,b,c)     (p)->GetConnectionSettings(a,b,c)
#define IDirectPlayLobby_ReceiveLobbyMessage(p,a,b,c,d,e)   (p)->ReceiveLobbyMessage(a,b,c,d,e)
#define IDirectPlayLobby_RegisterApplication(p,a,b)         (p)->RegisterApplication(a,b)
#define IDirectPlayLobby_RunApplication(p,a,b,c,d)          (p)->RunApplication(a,b,c,d)
#define IDirectPlayLobby_SendLobbyMessage(p,a,b,c,d)        (p)->SendLobbyMessage(a,b,c,d)
#define IDirectPlayLobby_SetConnectionSettings(p,a,b,c)		(p)->SetConnectionSettings(a,b,c)
#define IDirectPlayLobby_SetLobbyMessageEvent(p,a,b,c)	    (p)->SetLobbyMessageEvent(a,b,c)
#define IDirectPlayLobby_UnregisterApplication(p,a,b)       (p)->UnregisterApplication(a,b)
#define IDirectPlayLobby_WaitForConnectionSettings(p,a)     (p)->WaitForConnectionSettings(a)

#endif

/****************************************************************************
 *
 * DirectPlayLobby Flags
 *
 ****************************************************************************/

/*
 *	This flag is used by IDirectPlayLobby->WaitForConnectionSettings to
 *	cancel a current wait that is in progress.
 */
#define DPLWAIT_CANCEL                  0x00000001


/*
 *	This is a message flag used by ReceiveLobbyMessage.  It can be
 *	returned in the dwMessageFlags parameter to indicate a message from
 *	the system.
 */
#define DPLMSG_SYSTEM					0x00000001

/*
 *	This is a message flag used by ReceiveLobbyMessage and SendLobbyMessage.
 *  It is used to indicate that the message is a standard lobby message.
 *  DPLMSG_SETPROPERTY, DPLMSG_SETPROPERTYRESPONSE, DPLMSG_GETPROPERTY,
 *	DPLMSG_GETPROPERTYRESPONSE
 */
#define DPLMSG_STANDARD					0x00000002


/****************************************************************************
 *
 * DirectPlayLobby messages and message data structures
 *
 * All system messages have a dwMessageFlags value of DPLMSG_SYSTEM returned
 * from a call to ReceiveLobbyMessage.
 *
 * All standard messages have a dwMessageFlags value of DPLMSG_STANDARD returned
 * from a call to ReceiveLobbyMessage.
 *
 ****************************************************************************/

/*
 * DPLMSG_GENERIC
 * Generic message structure used to identify the message type.
 */
typedef struct _DPLMSG_GENERIC
{
    DWORD       dwType;         // Message type
} DPLMSG_GENERIC, FAR *LPDPLMSG_GENERIC;

/*
 * DPLMSG_SYSTEMMESSAGE
 * Generic message format for all system messages --
 * DPLSYS_CONNECTIONSETTINGSREAD, DPLSYS_DPLYCONNECTSUCCEEDED,
 * DPLSYS_DPLAYCONNECTFAILED, DPLSYS_APPTERMINATED, DPLSYS_NEWCONNECTIONSETTINGS
 */
typedef struct _DPLMSG_SYSTEMMESSAGE
{
    DWORD       dwType;         // Message type
    GUID        guidInstance;   // Instance GUID of the dplay session the message corresponds to
} DPLMSG_SYSTEMMESSAGE, FAR *LPDPLMSG_SYSTEMMESSAGE;

/*
 *  DPLMSG_SETPROPERTY
 *  Standard message sent by an application to a lobby to set a
 *  property
 */
typedef struct _DPLMSG_SETPROPERTY
{
	DWORD	dwType;				// Message type
	DWORD	dwRequestID;		// Request ID (DPL_NOCONFIRMATION if no confirmation desired)
	GUID	guidPlayer;			// Player GUID
	GUID	guidPropertyTag;	// Property GUID
	DWORD	dwDataSize;			// Size of data
	DWORD	dwPropertyData[1];	// Buffer containing data
} DPLMSG_SETPROPERTY, FAR *LPDPLMSG_SETPROPERTY;

#define	DPL_NOCONFIRMATION			0

/*
 *  DPLMSG_SETPROPERTYRESPONSE
 *  Standard message returned by a lobby to confirm a 
 *  DPLMSG_SETPROPERTY message.
 */
typedef struct _DPLMSG_SETPROPERTYRESPONSE
{
	DWORD	dwType;				// Message type
	DWORD	dwRequestID;		// Request ID
	GUID	guidPlayer;			// Player GUID
	GUID	guidPropertyTag;	// Property GUID
	HRESULT	hr;					// Return Code
} DPLMSG_SETPROPERTYRESPONSE, FAR *LPDPLMSG_SETPROPERTYRESPONSE;

/*
 *  DPLMSG_GETPROPERTY
 *  Standard message sent by an application to a lobby to request
 *	the current value of a property
 */
typedef struct _DPLMSG_GETPROPERTY
{
	DWORD	dwType;				// Message type
	DWORD	dwRequestID;		// Request ID
	GUID	guidPlayer;			// Player GUID
	GUID	guidPropertyTag;	// Property GUID
} DPLMSG_GETPROPERTY, FAR *LPDPLMSG_GETPROPERTY;

/*
 *  DPLMSG_GETPROPERTYRESPONSE
 *  Standard message returned by a lobby in response to a
 *	DPLMSG_GETPROPERTY message.
 */
typedef struct _DPLMSG_GETPROPERTYRESPONSE
{
	DWORD	dwType;				// Message type
	DWORD	dwRequestID;		// Request ID
	GUID	guidPlayer;			// Player GUID
	GUID	guidPropertyTag;	// Property GUID
	HRESULT	hr;					// Return Code
	DWORD	dwDataSize;			// Size of data
	DWORD	dwPropertyData[1];	// Buffer containing data
} DPLMSG_GETPROPERTYRESPONSE, FAR *LPDPLMSG_GETPROPERTYRESPONSE;

/*
 *  DPLMSG_NEWSESSIONHOST
 *  Standard message returned by a lobby in response to a
 *	the session host migrating to a new client
 */
typedef struct _DPLMSG_NEWSESSIONHOST
{
    DWORD   dwType;             // Message type
    GUID    guidInstance;       // GUID Instance of the session
} DPLMSG_NEWSESSIONHOST, FAR *LPDPLMSG_NEWSESSIONHOST;


/******************************************
 *
 *	DirectPlay Lobby message dwType values
 *
 *****************************************/

/*
 *  The application has read the connection settings.
 *  It is now O.K. for the lobby client to release
 *  its IDirectPlayLobby interface.
 */
#define DPLSYS_CONNECTIONSETTINGSREAD   0x00000001

/*
 *  The application's call to DirectPlayConnect failed
 */
#define DPLSYS_DPLAYCONNECTFAILED       0x00000002

/*
 *  The application has created a DirectPlay session.
 */
#define DPLSYS_DPLAYCONNECTSUCCEEDED    0x00000003

/*
 *  The application has terminated.
 */
#define DPLSYS_APPTERMINATED            0x00000004

/*
 *  The message is a DPLMSG_SETPROPERTY message.
 */
#define DPLSYS_SETPROPERTY				0x00000005

/*
 *  The message is a DPLMSG_SETPROPERTYRESPONSE message.
 */
#define DPLSYS_SETPROPERTYRESPONSE		0x00000006

/*
 *  The message is a DPLMSG_GETPROPERTY message.
 */
#define DPLSYS_GETPROPERTY				0x00000007

/*
 *  The message is a DPLMSG_GETPROPERTYRESPONSE message.
 */
#define DPLSYS_GETPROPERTYRESPONSE		0x00000008

/*
 *  The message is a DPLMSG_NEWSESSIONHOST message.
 */
#define DPLSYS_NEWSESSIONHOST           0x00000009

/*
 *  New connection settings are available.
 */
#define DPLSYS_NEWCONNECTIONSETTINGS    0x0000000A


/****************************************************************************
 *
 * DirectPlay defined property GUIDs and associated data structures
 *
 ****************************************************************************/

/*
 * DPLPROPERTY_MessagesSupported
 *
 * Request whether the lobby supports standard.  Lobby with respond with either
 * TRUE or FALSE or may not respond at all.
 * 
 * Property data is a single BOOL with TRUE or FALSE
 */
// {762CCDA1-D916-11d0-BA39-00C04FD7ED67}
DEFINE_GUID(DPLPROPERTY_MessagesSupported, 
0x762ccda1, 0xd916, 0x11d0, 0xba, 0x39, 0x0, 0xc0, 0x4f, 0xd7, 0xed, 0x67);

/*
 * DPLPROPERTY_LobbyGuid
 *
 * Request the GUID that identifies the lobby software that the application
 * is communicating with.
 *
 * Property data is a single GUID.
 */
// {F56920A0-D218-11d0-BA39-00C04FD7ED67}
DEFINE_GUID(DPLPROPERTY_LobbyGuid, 
0xf56920a0, 0xd218, 0x11d0, 0xba, 0x39, 0x0, 0xc0, 0x4f, 0xd7, 0xed, 0x67);

/*
 * DPLPROPERTY_PlayerGuid
 *
 * Request the GUID that identifies the player on this machine for sending
 * property data back to the lobby.
 *
 * Property data is the DPLDATA_PLAYERDATA structure
 */
// {B4319322-D20D-11d0-BA39-00C04FD7ED67}
DEFINE_GUID(DPLPROPERTY_PlayerGuid, 
0xb4319322, 0xd20d, 0x11d0, 0xba, 0x39, 0x0, 0xc0, 0x4f, 0xd7, 0xed, 0x67);

/*
 * DPLDATA_PLAYERGUID
 *
 * Data structure to hold the GUID of the player and player creation flags
 * from the lobby.
 */
typedef struct _DPLDATA_PLAYERGUID
{
	GUID	guidPlayer;
	DWORD	dwPlayerFlags;
} DPLDATA_PLAYERGUID, FAR *LPDPLDATA_PLAYERGUID;

/*
 * DPLPROPERTY_PlayerScore
 *
 * Used to send an array of long integers to the lobby indicating the 
 * score of a player.
 *
 * Property data is the DPLDATA_PLAYERSCORE structure.
 */
// {48784000-D219-11d0-BA39-00C04FD7ED67}
DEFINE_GUID(DPLPROPERTY_PlayerScore, 
0x48784000, 0xd219, 0x11d0, 0xba, 0x39, 0x0, 0xc0, 0x4f, 0xd7, 0xed, 0x67);

/*
 * DPLDATA_PLAYERSCORE
 *
 * Data structure to hold an array of long integers representing a player score.
 * Application must allocate enough memory to hold all the scores.
 */
typedef struct _DPLDATA_PLAYERSCORE
{
	DWORD	dwScoreCount;
	LONG	Score[1];
} DPLDATA_PLAYERSCORE, FAR *LPDPLDATA_PLAYERSCORE;

/****************************************************************************
 *
 * DirectPlay Address ID's
 *
 ****************************************************************************/

/* DirectPlay Address
 *
 * A DirectPlay address consists of multiple chunks of data, each tagged
 * with a GUID signifying the type of data in the chunk. The chunk also
 * has a length so that unknown chunk types can be skipped.
 *
 * The EnumAddress() function is used to parse these address data chunks.
 */

/*
 * DPADDRESS
 *
 * Header for block of address data elements
 */
typedef struct _DPADDRESS
{
    GUID                guidDataType;
    DWORD               dwDataSize;
} DPADDRESS;

typedef DPADDRESS FAR *LPDPADDRESS;

/*
 * DPAID_TotalSize
 *
 * Chunk is a DWORD containing size of entire DPADDRESS structure
 */

// {1318F560-912C-11d0-9DAA-00A0C90A43CB}
DEFINE_GUID(DPAID_TotalSize, 
0x1318f560, 0x912c, 0x11d0, 0x9d, 0xaa, 0x0, 0xa0, 0xc9, 0xa, 0x43, 0xcb);

/*
 * DPAID_ServiceProvider
 *
 * Chunk is a GUID describing the service provider that created the chunk.
 * All addresses must contain this chunk.
 */

// {07D916C0-E0AF-11cf-9C4E-00A0C905425E}
DEFINE_GUID(DPAID_ServiceProvider, 
0x7d916c0, 0xe0af, 0x11cf, 0x9c, 0x4e, 0x0, 0xa0, 0xc9, 0x5, 0x42, 0x5e);

/*
 * DPAID_LobbyProvider
 *
 * Chunk is a GUID describing the lobby provider that created the chunk.
 * All addresses must contain this chunk.
 */

// {59B95640-9667-11d0-A77D-0000F803ABFC}
DEFINE_GUID(DPAID_LobbyProvider, 
0x59b95640, 0x9667, 0x11d0, 0xa7, 0x7d, 0x0, 0x0, 0xf8, 0x3, 0xab, 0xfc);

/*
 * DPAID_Phone and DPAID_PhoneW
 *
 * Chunk is a string containing a phone number (i.e. "1-800-555-1212")
 * in ANSI or UNICODE format
 */

// {78EC89A0-E0AF-11cf-9C4E-00A0C905425E}
DEFINE_GUID(DPAID_Phone, 
0x78ec89a0, 0xe0af, 0x11cf, 0x9c, 0x4e, 0x0, 0xa0, 0xc9, 0x5, 0x42, 0x5e);

// {BA5A7A70-9DBF-11d0-9CC1-00A0C905425E}
DEFINE_GUID(DPAID_PhoneW, 
0xba5a7a70, 0x9dbf, 0x11d0, 0x9c, 0xc1, 0x0, 0xa0, 0xc9, 0x5, 0x42, 0x5e);

/*
 * DPAID_Modem and DPAID_ModemW
 *
 * Chunk is a string containing a modem name registered with TAPI
 * in ANSI or UNICODE format
 */

// {F6DCC200-A2FE-11d0-9C4F-00A0C905425E}
DEFINE_GUID(DPAID_Modem, 
0xf6dcc200, 0xa2fe, 0x11d0, 0x9c, 0x4f, 0x0, 0xa0, 0xc9, 0x5, 0x42, 0x5e);

// {01FD92E0-A2FF-11d0-9C4F-00A0C905425E}
DEFINE_GUID(DPAID_ModemW, 
0x1fd92e0, 0xa2ff, 0x11d0, 0x9c, 0x4f, 0x0, 0xa0, 0xc9, 0x5, 0x42, 0x5e);

/*
 * DPAID_Inet and DPAID_InetW
 *
 * Chunk is a string containing a TCP/IP host name or an IP address
 * (i.e. "dplay.microsoft.com" or "137.55.100.173") in ANSI or UNICODE format
 */

// {C4A54DA0-E0AF-11cf-9C4E-00A0C905425E}
DEFINE_GUID(DPAID_INet, 
0xc4a54da0, 0xe0af, 0x11cf, 0x9c, 0x4e, 0x0, 0xa0, 0xc9, 0x5, 0x42, 0x5e);

// {E63232A0-9DBF-11d0-9CC1-00A0C905425E}
DEFINE_GUID(DPAID_INetW, 
0xe63232a0, 0x9dbf, 0x11d0, 0x9c, 0xc1, 0x0, 0xa0, 0xc9, 0x5, 0x42, 0x5e);

/*
 * DPAID_InetPort
 *
 * Chunk is the port number used for creating the apps TCP and UDP sockets.
 * WORD value (i.e. 47624).
 */
 
// {E4524541-8EA5-11d1-8A96-006097B01411}
DEFINE_GUID(DPAID_INetPort, 
0xe4524541, 0x8ea5, 0x11d1, 0x8a, 0x96, 0x0, 0x60, 0x97, 0xb0, 0x14, 0x11);

#ifdef BIGMESSAGEDEFENSE
//@@BEGIN_MSINTERNAL
/*
 * DPAID_MaxMessageSize
 *
 * Tells DPLAY what the maximum allowed message size is.  Enables SPs to
 *	combat Denial of Service attacks
 */
 
 // this terrible hack is needed so the SP can work with the Elmer build.
 // it can be removed when the MSINTERNAL stuff is removed
 #define MAXMSGSIZEGUIDDEFINED

// {F5D09980-F0C4-11d1-8326-006097B01411}
DEFINE_GUID(DPAID_MaxMessageSize, 
0xf5d09980, 0xf0c4, 0x11d1, 0x83, 0x26, 0x0, 0x60, 0x97, 0xb0, 0x14, 0x11);
//@@END_MSINTERNAL
#endif

/*
 * DPCOMPORTADDRESS
 *
 * Used to specify com port settings. The constants that define baud rate,
 * stop bits and parity are defined in WINBASE.H. The constants for flow
 * control are given below.
 */

#define DPCPA_NOFLOW        0           // no flow control
#define DPCPA_XONXOFFFLOW   1           // software flow control
#define DPCPA_RTSFLOW       2           // hardware flow control with RTS
#define DPCPA_DTRFLOW       3           // hardware flow control with DTR
#define DPCPA_RTSDTRFLOW    4           // hardware flow control with RTS and DTR

typedef struct _DPCOMPORTADDRESS
{
    DWORD   dwComPort;                  // COM port to use (1-4)
    DWORD   dwBaudRate;                 // baud rate (100-256k)
    DWORD   dwStopBits;                 // no. stop bits (1-2)
    DWORD   dwParity;                   // parity (none, odd, even, mark)
    DWORD   dwFlowControl;              // flow control (none, xon/xoff, rts, dtr)
} DPCOMPORTADDRESS;

typedef DPCOMPORTADDRESS FAR *LPDPCOMPORTADDRESS;

/*
 * DPAID_ComPort
 *
 * Chunk contains a DPCOMPORTADDRESS structure defining the serial port.
 */

// {F2F0CE00-E0AF-11cf-9C4E-00A0C905425E}
DEFINE_GUID(DPAID_ComPort, 
0xf2f0ce00, 0xe0af, 0x11cf, 0x9c, 0x4e, 0x0, 0xa0, 0xc9, 0x5, 0x42, 0x5e);

/****************************************************************************
 *
 * 	dplobby 1.0 obsolete definitions
 *	Included for compatibility only.
 *
 ****************************************************************************/
#define DPLAD_SYSTEM          DPLMSG_SYSTEM


#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif /* __DPLOBBY_INCLUDED__ */
