#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "inetgetfile.h"
#include "CFtp.h"
#include "Chttpget.h"

InetGetFile::InetGetFile(char *URL, char *localfile) {}

InetGetFile::~InetGetFile() {}

BOOL InetGetFile::IsFileReceived() { return false; }
BOOL InetGetFile::IsFileError() { return true; }

BOOL InetGetFile::IsConnecting() { return true; }

BOOL InetGetFile::IsReceiving() { return false; }

int InetGetFile::GetErrorCode() { return 0; }

int InetGetFile::GetBytesIn() { return 0; }

int InetGetFile::GetTotalBytes() { return 0; }

void InetGetFile::AbortGet() {}

////////////////////////////////////////////////////////////////////

CFtpGet::CFtpGet(char *URL, char *localfile, char *Username, char *Password) {}
CFtpGet::~CFtpGet() {}
int CFtpGet::GetStatus() { return 0; }
unsigned int CFtpGet::GetBytesIn() { return 0; }
unsigned int CFtpGet::GetTotalBytes() { return 0; }
void CFtpGet::AbortGet() {}
void CFtpGet::WorkerThread() {}
int CFtpGet::ConnectControlSocket() { return 0; }
int CFtpGet::LoginHost() { return 0; }
unsigned int CFtpGet::SendFTPCommand(char *command) { return 0; }
unsigned int CFtpGet::ReadFTPServerReply() { return 0; }
unsigned int CFtpGet::GetFile() { return 0; }
unsigned int CFtpGet::IssuePort() { return 0; }
unsigned int CFtpGet::ReadDataChannel() { return 0; }
void CFtpGet::FlushControlChannel() {}
/////////////////////////////////////////////////////////////////////
ChttpGet::ChttpGet(char *URL, char *localfile) {}
ChttpGet::~ChttpGet() {}
int ChttpGet::GetStatus() { return 0; }
unsigned int ChttpGet::GetBytesIn() { return 0; }
unsigned int GetTotalBytes() { return 0; }
void AbortGet() {}
void WorkerThread() {}
int ConnectSocket() { return 0; }
char *GetHTTPLine() { return "ERR"; }
unsigned int ReadDataChannel() { return 0; }
