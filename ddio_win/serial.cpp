/*
 * $Source: $
 * $Revision: 2 $
 * $Author: Jeff $
 * $Date: 5/11/99 11:28a $
 *
 * COM port interface
 *
 * $Log: /DescentIII/Main/ddio_win/serial.cpp $
 * 
 * 2     5/11/99 11:28a Jeff
 * added serial support (SAMIR)
 * 
 * 1     5/10/99 9:27p Jeff
 * 
 */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "ddio_win.h"
#include "mem.h"
#include "pserror.h"
#include "ddio.h"

#include <stdio.h>

#define ASCII_XON       0x11
#define ASCII_XOFF      0x13


#define N_SERIAL_PORTS		4

// internal struct
typedef struct tWin32SerialPort
{
	sbyte port;
	sbyte connect;
	HANDLE hFile;
	DCB dcb;
}tWin32SerialPort;


// takes port number 1-4, returns a port object
tSerialPort ddio_SerialOpenPort(int port_number, int baud)
{
	COMMTIMEOUTS ctimeouts;
	char filename[16];
	tWin32SerialPort port_obj;

	ASSERT(port_number >= 1 && port_number <= N_SERIAL_PORTS);

	if (port_number < 1 || port_number > N_SERIAL_PORTS) {
		return NULL;
	}

	sprintf(filename, "COM%d", port_number);
  											
	port_obj.hFile = CreateFile(filename, 
								GENERIC_READ | GENERIC_WRITE,
								0,
								NULL,
								OPEN_EXISTING, 
								FILE_ATTRIBUTE_NORMAL, NULL);

	if (port_obj.hFile == INVALID_HANDLE_VALUE)	{
		return NULL;
	}
	port_obj.port = (sbyte)port_number;
	
//	 Modem COMport is open.
	SetCommMask(port_obj.hFile, 0);
	SetupComm(port_obj.hFile, 1024, 1024);
	PurgeComm(port_obj.hFile, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR ) ;

//	Timeout after 10 sec.
	ctimeouts.ReadIntervalTimeout = 0xffffffff;
	ctimeouts.ReadTotalTimeoutMultiplier = 0;
	ctimeouts.ReadTotalTimeoutConstant = 10000;
	ctimeouts.WriteTotalTimeoutMultiplier = 0;
	ctimeouts.WriteTotalTimeoutConstant = 10000;
	SetCommTimeouts(port_obj.hFile, &ctimeouts);

//	Setup parameters for Connection
//	38400 8N1
	port_obj.dcb.DCBlength = sizeof(DCB);
	GetCommState(port_obj.hFile, &port_obj.dcb);

	port_obj.dcb.BaudRate = baud;

	mprintf((0, "COM%d (%d) is opened.\n", port_obj.port,  baud));

	port_obj.dcb.fBinary			= 1;
	port_obj.dcb.Parity 			= NOPARITY;
	port_obj.dcb.fNull 			= 0;
   port_obj.dcb.XonChar 		= ASCII_XON;
   port_obj.dcb.XoffChar 		= ASCII_XOFF;
   port_obj.dcb.XonLim 			= 1024;
   port_obj.dcb.XoffLim 		= 1024;
   port_obj.dcb.EofChar 		= 0;
   port_obj.dcb.EvtChar 		= 0;
	port_obj.dcb.fDtrControl	= DTR_CONTROL_ENABLE;// dtr=on
	port_obj.dcb.fRtsControl	= RTS_CONTROL_ENABLE;

	port_obj.dcb.ByteSize 		= 8;
	port_obj.dcb.StopBits 		= ONESTOPBIT;
	port_obj.dcb.fParity			= FALSE;

	port_obj.dcb.fOutxDsrFlow	= FALSE;
	port_obj.dcb.fOutxCtsFlow	= FALSE;					// rts/cts off

// obj->dcb.fInX = obj->dcb.fOutX = 1;				// Software flow control XON/XOFF
  	
	if (SetCommState(port_obj.hFile, &port_obj.dcb) == TRUE) 
	{
	//	Send DTR
		EscapeCommFunction(port_obj.hFile, SETDTR);
		port_obj.connect = 1;
	}
	else {
		mprintf((1, "COMM: Unable to set CommState: (%x)\n", GetLastError()));
		CloseHandle(port_obj.hFile);
		port_obj.connect = 0;

		return NULL;
	}


	tWin32SerialPort *obj = (tWin32SerialPort *)mem_malloc(sizeof(tWin32SerialPort));
	if (obj) {
		memcpy(obj, &port_obj, sizeof(port_obj));
	}
	else {
		CloseHandle(port_obj.hFile);
		port_obj.connect = 0;
		return NULL;
	}

	return obj;
}


// takes port structure 
void ddio_SerialClosePort(tSerialPort port)
{
	tWin32SerialPort *obj = (tWin32SerialPort *)port;

	if (!port) return;

	if (obj->connect) {
		SetCommMask(obj->hFile, 0);
		EscapeCommFunction(obj->hFile, CLRDTR);
		PurgeComm( obj->hFile, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR ) ;
		CloseHandle(obj->hFile);

		obj->connect = 0;
	}

	obj->hFile = NULL;
	mprintf((0, "COM%d closed.\n", obj->port));

	mem_free(obj);
}


bool ddio_SerialWriteByte(tSerialPort port, ubyte b)
{
	tWin32SerialPort *obj = (tWin32SerialPort *)port;
	DWORD length;

	if (!obj->connect) return false;

	if (!WriteFile(obj->hFile, &b, 1, &length,NULL)) {
		mprintf((0, "Failed to write byte to COM%d\n", obj->port));
		return false;
	}

	return true;
}