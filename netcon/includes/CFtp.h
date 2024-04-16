/*
 * $Logfile: /DescentIII/Main/lib/CFtp.h $
 * $Revision: 1.3 $
 * $Date: 2004/02/09 04:14:51 $
 * $Author: kevinb $
 *
 * FTP Client class (get only)
 *
 * $Log: CFtp.h,v $
 * Revision 1.3  2004/02/09 04:14:51  kevinb
 * Added newlines to all headers to reduce number of warnings printed
 *
 * Made some small changes to get everything compiling.
 *
 * All Ready to merge the 1.5 tree.
 *
 * Revision 1.2  2000/06/03 14:30:33  icculus
 * 1.4 code merge and pthread->SDL thread conversion.
 *
 * Revision 1.1.1.1  2000/04/18 00:00:38  icculus
 * initial checkin
 *
 *
 * 4     8/21/99 6:48a Jeff
 * Linux port
 *
 * 3     7/31/98 11:40a Kevin
 *
 * 2     6/01/98 10:10a Kevin
 * Added DLL connection interface and auto update DLL
 *
 * 1     5/27/98 9:54a Kevin
 *
 * 1     5/25/98 5:31p Kevin
 * Initial version
 *
 * $NoKeywords: $
 */
#ifndef _CFTP_HEADER_
#define _CFTP_HEADER_

#define FTP_STATE_INTERNAL_ERROR 0
#define FTP_STATE_SOCKET_ERROR 1
#define FTP_STATE_URL_PARSING_ERROR 2
#define FTP_STATE_CONNECTING 3
#define FTP_STATE_HOST_NOT_FOUND 4
#define FTP_STATE_CANT_CONNECT 5
#define FTP_STATE_LOGGING_IN 6
#define FTP_STATE_LOGIN_ERROR 7
#define FTP_STATE_LOGGED_IN 8
#define FTP_STATE_DIRECTORY_INVALID 9
#define FTP_STATE_FILE_NOT_FOUND 10
#define FTP_STATE_RECEIVING 11
#define FTP_STATE_FILE_RECEIVED 12
#define FTP_STATE_UNKNOWN_ERROR 13
#define FTP_STATE_RECV_FAILED 14
#define FTP_STATE_CANT_WRITE_FILE 15
#define FTP_STATE_STARTUP 16

#ifdef __LINUX__
extern int FTPObjThread(void *obj);
#else
extern void FTPObjThread(void *obj);
#endif

class CFtpGet {

public:
  CFtpGet(char *URL, char *localfile, char *Username = NULL, char *Password = NULL);
  ~CFtpGet();
  int GetStatus();
  unsigned int GetBytesIn();
  unsigned int GetTotalBytes();
  void AbortGet();

  void WorkerThread();

protected:
  int ConnectControlSocket();
  int LoginHost();
  unsigned int SendFTPCommand(char *command);
  unsigned int ReadFTPServerReply();
  unsigned int GetFile();
  unsigned int IssuePort();
  unsigned int ReadDataChannel();
  void FlushControlChannel();

  unsigned int m_iBytesIn;
  unsigned int m_iBytesTotal;
  unsigned int m_State;

  bool m_Aborting;
  bool m_Aborted;

  char m_szUserName[100];
  char m_szPassword[100];
  char m_szHost[200];
  char m_szDir[200];
  char m_szFilename[100];

  char recv_buffer[1000];

  SOCKET m_ListenSock;
  SOCKET m_DataSock;
  SOCKET m_ControlSock;

  FILE *LOCALFILE;
};

#endif
