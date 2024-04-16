#include "MacHeaders.h"

#include <stdlib.h>
#include <unix.h>
#include <ctype.h>
#include <math.h>

// Constants
#define MACINTOSH

#include "pserror.h"
#include "Macros.h"

#define RELEASE
//#define mprintf(args) //Debug_ConsolePrintf args

//#pragma options align packed
#define MEM_USE_RTL 1
#define and andVar
#define or orVar
#define _fstat fstat
#define _filelength filelength
#define __cdecl
#define _MAX_PATH 1024
#define _MAX_FNAME 64 /* max. length of path component*/
#define _MAX_EXT 64   /* max. length of extension component*/
#define TRANSPARENT_COLOR 0
// These are other error types defined in Visual C++.
// The values are made up and hopefullydon't conflict JCA
#define EACCES 102
#define EDEADLOCK 104
#define EMFILE 105
#define ENOENT 106
#define ENFILE 107
#define ENOSPC 108
#define ENOSYS 112

#define _finite(a) finite(a)
#define strcmpi(a, b) stricmp(a, b)
#define strcmpni(a, b, c) strnicmp(a, b, c)
#define _vsnprintf(a, b, c, d) vsnprintf(a, b, c, d)
#define HeapSize(a, b, c) 0
#define HeapDestroy(a)

int stricmp(const char *inX, const char *inY);
int strnicmp(const char *inX, const char *inY, int len);
char *strdup(const char *str);

#if 0
///////////////////////////
// From Winsock.h
///////////////////////////

/*
 * Address families.
 */
#define AF_UNSPEC 0     /* unspecified */
#define AF_UNIX 1       /* local to host (pipes, portals) */
#define AF_INET 2       /* internetwork: UDP, TCP, etc. */
#define AF_IMPLINK 3    /* arpanet imp addresses */
#define AF_PUP 4        /* pup protocols: e.g. BSP */
#define AF_CHAOS 5      /* mit CHAOS protocols */
#define AF_IPX 6        /* IPX and SPX */
#define AF_NS 6         /* XEROX NS protocols */
#define AF_ISO 7        /* ISO protocols */
#define AF_OSI AF_ISO   /* OSI is ISO */
#define AF_ECMA 8       /* european computer manufacturers */
#define AF_DATAKIT 9    /* datakit protocols */
#define AF_CCITT 10     /* CCITT protocols, X.25 etc */
#define AF_SNA 11       /* IBM SNA */
#define AF_DECnet 12    /* DECnet */
#define AF_DLI 13       /* Direct data link interface */
#define AF_LAT 14       /* LAT */
#define AF_HYLINK 15    /* NSC Hyperchannel */
#define AF_APPLETALK 16 /* AppleTalk */
#define AF_NETBIOS 17   /* NetBios-style addresses */
#define AF_VOICEVIEW 18 /* VoiceView */
#define AF_FIREFOX 19   /* FireFox */
#define AF_UNKNOWN1 20  /* Somebody is using this! */
#define AF_BAN 21       /* Banyan */
#define AF_MAX 22

/*
 * Types
 */
#define SOCK_STREAM 1    /* stream socket */
#define SOCK_DGRAM 2     /* datagram socket */
#define SOCK_RAW 3       /* raw-protocol interface */
#define SOCK_RDM 4       /* reliably-delivered message */
#define SOCK_SEQPACKET 5 /* sequenced packet stream */
/*
 * Basic system type definitions, taken from the BSD file sys/types.h.
 */
typedef unsigned char   u_char;
typedef unsigned short  u_short;
typedef unsigned int    u_int;
typedef unsigned long   u_long;
/*
 * The new type to be used in all
 * instances which refer to sockets.
 */
typedef u_int           SOCKET;
/*
 * Socket address, internet style.
 */
struct sockaddr {
        unsigned short sa_family;              /* address family */
        char    sa_data[14];            /* up to 14 bytes of direct address */
};
struct in_addr {
        union {
                struct { u_char s_b1,s_b2,s_b3,s_b4; } S_un_b;
                struct { u_short s_w1,s_w2; } S_un_w;
                u_long S_addr;
        } S_un;
#define s_addr S_un.S_addr
                                /* can be used for most tcp & ip code */
#define s_host S_un.S_un_b.s_b2
                                /* host on imp */
#define s_net S_un.S_un_b.s_b1
                                /* network */
#define s_imp S_un.S_un_w.s_w2
                                /* imp */
#define s_impno S_un.S_un_b.s_b4
                                /* imp # */
#define s_lh S_un.S_un_b.s_b3
                                /* logical host */
};
struct sockaddr_in {
        short   sin_family;
        u_short sin_port;
        struct  in_addr sin_addr;
        char    sin_zero[8];
};
#endif

// Types
typedef unsigned char BOOL;
typedef void **HANDLE;
typedef Fixed fix;
typedef long long INT64;
typedef void *HGLOBAL;
// Function Definitions
#define _finite(a) finite(a)
#define strcmpi(a, b) stricmp(a, b)
#define strcmpni(a, b, c) strnicmp(a, b, c)
#define _vsnprintf(a, b, c, d) vsnprintf(a, b, c, d)
#define HeapSize(a, b, c) 0
#define HeapDestroy(a)
int stricmp(const char *inX, const char *inY);
int strnicmp(const char *inX, const char *inY, int len);
char *strdup(const char *str);
void GlobalFree(void *);
void *GlobalAlloc(int flags, int size);
void *GlobalLock(HGLOBAL hMem);
void Sleep(int millis);
char *itoa(int value, char *string, int radix);
char *strupr(char *string);
inline int finite(double a) { return (a != HUGE_VAL); }

#if 0
///////////////////////////
// From Winsock.h
///////////////////////////
SOCKET accept (SOCKET s, struct sockaddr *addr,
                          int *addrlen);
int bind (SOCKET s, const struct sockaddr *addr, int namelen);
int closesocket (SOCKET s);
int connect (SOCKET s, const struct sockaddr *name, int namelen);
int ioctlsocket (SOCKET s, long cmd, u_long *argp);
int getpeername (SOCKET s, struct sockaddr *name, int * namelen);
int getsockname (SOCKET s, struct sockaddr *name, int * namelen);
int getsockopt (SOCKET s, int level, int optname, char * optval, int *optlen);
#define htonl(hostlong) (hostlong)
#define htons(hostshort) (hostshort)
unsigned long inet_addr (const char * cp);
char * inet_ntoa (struct in_addr in);
int listen (SOCKET s, int backlog);
u_long ntohl (u_long netlong);
u_short ntohs (u_short netshort);
int recv (SOCKET s, char * buf, int len, int flags);
int recvfrom (SOCKET s, char * buf, int len, int flags, struct sockaddr *from, int * fromlen);
//int select (int nfds, fd_set *readfds, fd_set *writefds,
//                       fd_set *exceptfds, const struct timeval *timeout);
int send (SOCKET s, const char * buf, int len, int flags);
int sendto (SOCKET s, const char * buf, int len, int flags, const struct sockaddr *to, int tolen);
int setsockopt (SOCKET s, int level, int optname, const char * optval, int optlen);
int shutdown (SOCKET s, int how);
SOCKET socket (int af, int type, int protocol);
#endif