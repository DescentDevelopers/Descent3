/*
 * $Logfile: /DescentIII/Main/mission_download.h $
 * $Revision: 5 $
 * $Date: 3/26/00 10:29p $
 * $Author: Kevin $
 *
 *  Mission file downloading system
 *
 *
 * $Log: /DescentIII/Main/mission_download.h $
 *
 * 5     3/26/00 10:29p Kevin
 * MOD Downloader for 1.4 patch.
 *
 * 4     4/15/99 1:40a Jeff
 * changes for linux compile
 *
 * 3     12/30/98 12:15p Kevin
 * Auto Mission Download system
 *
 * 2     12/28/98 2:22p Kevin
 * Initial mission downloading system
 *
 * 1     12/28/98 11:42a Kevin
 *
 *
 */

#define MAX_MISSION_URL_LEN 300
#define MAX_MISSION_URL_COUNT 5

#define URL_ASK_POLL_TIME 2

extern int Got_url;

typedef struct {
  char msnname[_MAX_PATH];
  char URL[MAX_MISSION_URL_COUNT][MAX_MISSION_URL_LEN];
  // Possibly some quality of service flags
} msn_urls;

// Function prototypes
void msn_DoAskForURL(ubyte *indata, network_address *net_addr);
void msn_DoCurrMsnURLs(ubyte *data, network_address *net_addr);
int msn_CheckGetMission(network_address *net_addr, char *filename);
int msn_ShowDownloadChoices(msn_urls *urls);
int msn_DownloadWithStatus(char *url, char *filename);
void msn_ClipURLToWidth(int width, char *string);
char *msn_SecondsToString(int time_sec);

int ModDownloadWithStatus(char *url, char *filename);
int CheckGetD3M(char *d3m);