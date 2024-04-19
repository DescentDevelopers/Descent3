/*
* $Logfile: /DescentIII/Main/inetfile/inetgetfile.cpp $
* $Revision: 1.1.1.1 $
* $Date: 2003-08-26 03:58:00 $
* $Author: kevinb $
*
* InternetGetFile Class
*
* $Log: not supported by cvs2svn $
 * 
 * 13    3/26/00 11:28p Kevin
 * fixed case sensitivity of http: and ftp:
 * 
 * 12    3/26/00 10:33p Kevin
 * fixed non-win32 builds
 * 
 * 11    3/26/00 10:30p Kevin
 * MOD Downloader for 1.4 patch.
 * 
 * 10    10/21/99 9:28p Jeff
 * B.A. Macintosh code merge
 * 
 * 9     9/08/99 6:37p Jeff
 * fixed http/ftp downloading for Linux, should all work fine now.
 * 
 * 8     8/23/99 5:12p Kevin
 * Proxy support for http
 * 
 * 7     8/21/99 6:48a Jeff
 * Linux port
 * 
 * 6     4/14/99 1:20a Jeff
 * fixed case mismatched #includes
 * 
 * 5     7/31/98 11:57a Kevin
 * Added new functions for getting state
 * 
 * 4     7/31/98 11:40a Kevin
 * 
 * 3     7/31/98 11:17a Nate
 * Fixed memory leak and added memory alloc checking.
 * 
 * 2     6/01/98 10:10a Kevin
 * Added DLL connection interface and auto update DLL
 * 
 * 1     5/27/98 9:52a Kevin
 * 
 * 1     5/25/98 5:31p Kevin
 * Initial version
*
* $NoKeywords: $
*/

#if defined(WIN32)
#include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef MACINTOSH
#include "macsock.h"
#endif

// include inetgetfile.h before CFtp.h and Chttpget.h because it has some winsock defines
// to make Linux happy
#include "inetgetfile.h"
#include "CFtp.h"
#include "Chttpget.h"

extern char *Proxy_server;
extern short Proxy_port;


#ifdef __LINUX__
pthread_create_fp df_pthread_create = NULL;
pthread_exit_fp df_pthread_exit = NULL;
pthread_detach_fp df_pthread_detach = NULL;
pthread_self_fp df_pthread_self = NULL;

bool inet_LoadThreadLib(void)
{
	if(df_pthread_create)
		return true;

	void *lib;

	lib = dlopen("libpthread.so",RTLD_GLOBAL|RTLD_NOW);
	if(!lib)
	{
		return false;
	}

	df_pthread_create = (pthread_create_fp)dlsym(lib,"pthread_create");
	df_pthread_detach = (pthread_detach_fp)dlsym(lib,"pthread_detach");
	df_pthread_self = (pthread_self_fp)dlsym(lib,"pthread_self");

	if(!df_pthread_create || !df_pthread_detach || !df_pthread_self)
	{
		df_pthread_create = NULL;	//this variable I use to see if the library is loaded
		return false;
	}

	// don't close the library...we need it open for future use
	// and we can't close it on atexit() because linux will segfault for some stupid reason	
	return true;
}
#endif

void InetGetFile::AbortGet()
{
	if(m_bUseHTTP)
	{
		http->AbortGet();
	}
	else
	{
		ftp->AbortGet();
	}
}

InetGetFile::InetGetFile(char *URL,char *localfile,char *proxyip,short proxyport)
{
	m_HardError = 0;
	http=NULL;
	ftp=NULL;
	if((URL==NULL)||(localfile==NULL))
	{
		m_HardError = INET_ERROR_BADPARMS;
	}

	if(strstr(URL,"http:"))
	{
		m_bUseHTTP = true;
		http = new ChttpGet(URL,localfile,proxyip,proxyport);
		if(http==NULL)
			m_HardError = INET_ERROR_NO_MEMORY;
	}
	else if(strstr(URL,"ftp:"))
	{
		m_bUseHTTP = false;
		ftp = new CFtpGet(URL,localfile);
		if(ftp==NULL)
			m_HardError = INET_ERROR_NO_MEMORY;
	}
	else if(strstr(URL,"HTTP:"))
	{
		m_bUseHTTP = true;
		http = new ChttpGet(URL,localfile,proxyip,proxyport);
		if(http==NULL)
			m_HardError = INET_ERROR_NO_MEMORY;
	}
	else if(strstr(URL,"FTP:"))
	{
		m_bUseHTTP = false;
		ftp = new CFtpGet(URL,localfile);
		if(ftp==NULL)
			m_HardError = INET_ERROR_NO_MEMORY;
	}
	else
	{
		m_HardError = INET_ERROR_CANT_PARSE_URL;
	}
	Sleep(1000);
}

InetGetFile::InetGetFile(char *URL,char *localfile)
{
	m_HardError = 0;
	http=NULL;
	ftp=NULL;
	if((URL==NULL)||(localfile==NULL))
	{
		m_HardError = INET_ERROR_BADPARMS;
	}
	if(strstr(URL,"http:"))
	{
		m_bUseHTTP = true;
		http = new ChttpGet(URL,localfile);
		if(http==NULL)
			m_HardError = INET_ERROR_NO_MEMORY;
	}
	else if(strstr(URL,"ftp:"))
	{
		m_bUseHTTP = false;
		ftp = new CFtpGet(URL,localfile);
		if(ftp==NULL)
			m_HardError = INET_ERROR_NO_MEMORY;
	}
	else if(strstr(URL,"HTTP:"))
	{
		m_bUseHTTP = true;
		http = new ChttpGet(URL,localfile);
		if(http==NULL)
			m_HardError = INET_ERROR_NO_MEMORY;
	}
	else if(strstr(URL,"FTP:"))
	{
		m_bUseHTTP = false;
		ftp = new CFtpGet(URL,localfile);
		if(ftp==NULL)
			m_HardError = INET_ERROR_NO_MEMORY;
	}
	else
	{
		m_HardError = INET_ERROR_CANT_PARSE_URL;
	}
	Sleep(1000);
}

InetGetFile::~InetGetFile()
{
	if(http!=NULL) delete http;
	if(ftp!=NULL) delete ftp;
}

BOOL InetGetFile::IsConnecting()
{
	int state;
	if(m_bUseHTTP)
	{
		state = http->GetStatus();
	}
	else if(ftp)
	{
		state = ftp->GetStatus();
	}
	else
	{
		return false;
	}
	if(state == FTP_STATE_CONNECTING)
	{
		return true;
	}
	else
	{
		return false;
	}

}

BOOL InetGetFile::IsReceiving()
{
	int state;
	if(m_bUseHTTP)
	{
		state = http->GetStatus();
	}
	else if(ftp)
	{
		state = ftp->GetStatus();
	}
	if(state == FTP_STATE_RECEIVING)
	{
		return true;
	}
	else
	{
		return false;
	}
}

BOOL InetGetFile::IsFileReceived()
{
	int state;
	if(m_bUseHTTP)
	{
		state = http->GetStatus();
	}
	else if(ftp)
	{
		state = ftp->GetStatus();
	}
	if(state == FTP_STATE_FILE_RECEIVED)
	{
		return true;
	}
	else
	{
		return false;
	}
}

BOOL InetGetFile::IsFileError()
{
	int state;
	if(m_HardError) return true;
	if(m_bUseHTTP)
	{
		state = http->GetStatus();
	}
	else if(ftp)
	{
		state = ftp->GetStatus();
	}
	switch(state)
	{

	case FTP_STATE_URL_PARSING_ERROR:
	case FTP_STATE_HOST_NOT_FOUND:
	case FTP_STATE_DIRECTORY_INVALID:
	case FTP_STATE_FILE_NOT_FOUND:
	case FTP_STATE_CANT_CONNECT:
	case FTP_STATE_LOGIN_ERROR:
	case FTP_STATE_INTERNAL_ERROR:
	case FTP_STATE_SOCKET_ERROR:
	case FTP_STATE_UNKNOWN_ERROR:
	case FTP_STATE_RECV_FAILED:
	case FTP_STATE_CANT_WRITE_FILE:
		return true;
	case FTP_STATE_CONNECTING:
		return false;
	default:
		return false;
	}
}

int InetGetFile::GetErrorCode()
{
	int state;
	if(m_HardError) return m_HardError;
	if(m_bUseHTTP)
	{
		state = http->GetStatus();
	}
	else if(ftp)
	{
		state = ftp->GetStatus();
	}
	switch(state)
	{

	case FTP_STATE_URL_PARSING_ERROR:
		return INET_ERROR_CANT_PARSE_URL;

	case FTP_STATE_HOST_NOT_FOUND:
		return INET_ERROR_HOST_NOT_FOUND;


	case FTP_STATE_DIRECTORY_INVALID:
	case FTP_STATE_FILE_NOT_FOUND:
		return INET_ERROR_BAD_FILE_OR_DIR;

	case FTP_STATE_CANT_CONNECT:
	case FTP_STATE_LOGIN_ERROR:
	case FTP_STATE_INTERNAL_ERROR:
	case FTP_STATE_SOCKET_ERROR:
	case FTP_STATE_UNKNOWN_ERROR:
	case FTP_STATE_RECV_FAILED:

		return INET_ERROR_UNKNOWN_ERROR;

	case FTP_STATE_CANT_WRITE_FILE:
		return INET_ERROR_CANT_WRITE_FILE;
	default:
		return INET_ERROR_NO_ERROR;
	}
}

int InetGetFile::GetTotalBytes()
{
	if(m_bUseHTTP)
	{
		return http->GetTotalBytes();
	}
	else if(ftp)
	{
		return ftp->GetTotalBytes();
	}
	else return 0;
}

int InetGetFile::GetBytesIn()
{
	if(m_bUseHTTP)
	{
		return http->GetBytesIn();
	}
	else if(ftp)
	{
		return ftp->GetBytesIn();
	}
	else return 0;
}