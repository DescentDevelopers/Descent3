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

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "inetgetfile.h"
#include "CFtp.h"
#include "Chttpget.h"


InetGetFile::InetGetFile(char *URL,char *localfile)
{
}

InetGetFile::~InetGetFile()
{
}

BOOL InetGetFile::IsFileReceived()
{
	return false;
}
BOOL InetGetFile::IsFileError()
{
	return true;
}

BOOL InetGetFile::IsConnecting()
{
	return true;
}

BOOL InetGetFile::IsReceiving()
{
	return false;
}

int InetGetFile::GetErrorCode()
{
	return 0;
}

int InetGetFile::GetBytesIn()
{
	return 0;
}

int InetGetFile::GetTotalBytes()
{
	return 0;
}

void InetGetFile::AbortGet()
{
}

////////////////////////////////////////////////////////////////////

CFtpGet::CFtpGet(char *URL,char *localfile,char *Username,char *Password){}
CFtpGet::~CFtpGet(){}
int CFtpGet::GetStatus(){return 0;}
uint32_t CFtpGet::GetBytesIn(){return 0;}
uint32_t CFtpGet::GetTotalBytes(){return 0;}
void CFtpGet::AbortGet(){}
void CFtpGet::WorkerThread(){}
int CFtpGet::ConnectControlSocket(){return 0;}
int CFtpGet::LoginHost(){return 0;}
uint32_t CFtpGet::SendFTPCommand(char *command){return 0;}
uint32_t CFtpGet::ReadFTPServerReply(){return 0;}
uint32_t CFtpGet::GetFile(){return 0;}
uint32_t CFtpGet::IssuePort(){return 0;}
uint32_t CFtpGet::ReadDataChannel(){return 0;}
void CFtpGet::FlushControlChannel(){}
/////////////////////////////////////////////////////////////////////
ChttpGet::ChttpGet(char *URL,char *localfile){}
ChttpGet::~ChttpGet(){}
int ChttpGet::GetStatus(){return 0;}
uint32_t ChttpGet::GetBytesIn(){return 0;}
uint32_t GetTotalBytes(){return 0;}
void AbortGet(){}
void WorkerThread(){}
int ConnectSocket(){return 0;}
char *GetHTTPLine(){return "ERR";}
uint32_t ReadDataChannel(){return 0;}
	
