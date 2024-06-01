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

 * $Logfile: /DescentIII/Main/D3Launch/3D_detect.cpp $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:56:51 $
 * $Author: kevinb $
 *
 * 
 *
 * $Log: not supported by cvs2svn $
 * 
 * 15    5/19/99 12:29p Nate
 * Fixed openGL crash, changed Network speed default, changed OpenGL
 * detection to default to unchecked, and removed config file parsing from
 * US version
 * 
 * 14    3/11/99 10:54a Nate
 * Changed "3Dfx Glide - Voodoo3" to only show up for Voodoo3 builds that
 * don't include all APIs
 * 
 * 13    3/09/99 11:57a Nate
 * Changed "Voodoo Graphics Family" to "Voodoo3" for OEM_VOODOO3 builds
 * 
 * 12    10/19/98 11:04a Nate
 * More fixes to eliminate detection of software-only OpenGL
 * 
 * 11    10/18/98 2:43p Nate
 * Undid some of the suggested Glide init "fixes", as they would sometimes
 * cause Direct3D enumeration to crash 
 * 
 * 10    10/16/98 3:18p Nate
 * 
 * 9     10/15/98 7:30p Nate
 * 
 * 8     10/13/98 3:03p Nate
 * More fixes and changes.
 * 
 * 7     10/12/98 7:13p Nate
 * Fixed several bugs.
 * 
 * 6     10/08/98 6:23p Nate
 * Fixed a few bugs.
 * 
 * 5     9/18/98 5:18p Nate
 * Added a card type to the 3dfx Glide board detection type.
 * 
 * 4     9/02/98 9:48a Nate
 * Fixed hardware accelerator display bug in the Speed Tab
 * 
 * 3     9/01/98 7:15p Nate
 * Major Revision #2
 * 
 * 2     8/05/98 11:54a Nate
 * Initial Version
 *
 * $NoKeywords: $
 */

#define DYNAHEADER	
#define DYNAHEADER_CREATE_STORAGE

#include "stdafx.h"
#include "dyna_glide.h"
#include "module.h"

#include "pserror.h"
#include "PsTypes.h"
#include "3d_detect.h"
#include "gl.h"

#include "D3Launch.h"
#include "VideoTab.h"

#undef D3D_OVERLOADS
#include "d3d.h"
#include "ddraw.h"

card3d Cards[MAX_CARDS];
int Num_cards = 0;

#define MAX_D2D_DEVICES 8
#define MAX_D3D_DEVICES 16

struct d3d_device {
	GUID	guid_2d;
	LPGUID	pguid_2d;

	GUID	guid_3d;
	LPGUID	pguid_3d;

	char	name[1024];
};

d3d_device D2D_devices[MAX_D2D_DEVICES];
d3d_device D3D_devices[MAX_D3D_DEVICES];

int Num_d2d_devices = 0;
int Num_d3d_devices = 0;

// The enumeration callback for D3D devices
HRESULT WINAPI gr_d3d_enum( LPGUID lpGUID,
							LPSTR lpDeviceDescription,
							LPSTR lpDeviceName,
							LPD3DDEVICEDESC lpHWDesc,
							LPD3DDEVICEDESC lpHELDesc, 
							LPVOID lpContext )
{
	int use_it = 0;
	
//	mprintf( "Found 3d device %s: %s\n",  lpDeviceName, lpDeviceDescription );

	if ( lpHWDesc && lpHWDesc->dwFlags != 0 )	{
		use_it = 1;
	} //else if ( lpHELDesc )	{

	
	if ( use_it )	{
		d3d_device *d2d = (d3d_device *)lpContext;
		d3d_device *d3d = (d3d_device *)&D3D_devices[Num_d3d_devices++];

		if ( lpGUID )	{
			memmove( &d3d->guid_3d, lpGUID, sizeof(GUID) );
			d3d->pguid_3d = &d3d->guid_3d;
		} else {
			memset( &d3d->guid_3d, 0, sizeof(GUID) );
			d3d->pguid_3d = NULL;
		}

		memmove( &d3d->guid_2d, &d2d->guid_2d, sizeof(GUID) );
		if ( d2d->pguid_2d )	{
			d3d->pguid_2d = &d3d->guid_2d;
		} else {
			d3d->pguid_2d = NULL;
		}

		//strcpy( d3d->name, "Direct 3D - " );
		strcpy( d3d->name, d2d->name );
	}

	return D3DENUMRET_OK;
}

// The enumeration callback for 2D  
BOOL WINAPI gr_d2d_enum( LPGUID lpGUID,
							LPSTR lpDeviceDescription,
							LPSTR lpDeviceName,
							LPVOID lpContext )
{
	
	d3d_device *d2d = (d3d_device *)&D2D_devices[Num_d2d_devices++];

//	mprintf( "Found 2d device %s: %s\n",  lpDeviceName, lpDeviceDescription );
	
	if ( lpGUID )	{
		memmove( &d2d->guid_2d, lpGUID, sizeof(GUID) );
		d2d->pguid_2d = &d2d->guid_2d;
	} else {
		memset( &d2d->guid_2d, 0, sizeof(GUID) );
		d2d->pguid_2d = NULL;
	}

	strcpy( d2d->name, lpDeviceDescription );
//	strcat( d2d->name, lpDeviceName );

	return D3DENUMRET_OK;
}

HRESULT (__stdcall *pfn_DirectDrawEnumerate) (LPDDENUMCALLBACK, LPVOID) = NULL;
HRESULT (__stdcall *pfn_DirectDrawCreate) (GUID FAR *, LPDIRECTDRAW FAR *, IUnknown FAR *) = NULL;

// Check for any Direct 3D devices
void check_direct3d()
{
	int i;
	HRESULT ddrval;
	LPDIRECTDRAW lpDD1=NULL;
	LPDIRECT3D2 lpD3D=NULL;

	Num_d2d_devices = 0;
	Num_d3d_devices = 0;

	if (!Dd_dll_handle)
		return;

	if (!pfn_DirectDrawCreate)
		pfn_DirectDrawCreate = (HRESULT (__stdcall *) (GUID FAR *, LPDIRECTDRAW FAR *, IUnknown FAR *)) GetProcAddress(Dd_dll_handle, "DirectDrawCreate");
	if (!pfn_DirectDrawCreate)
		goto D3DError;

	if (!pfn_DirectDrawEnumerate)
		pfn_DirectDrawEnumerate = (HRESULT (__stdcall *) (LPDDENUMCALLBACK, LPVOID)) GetProcAddress(Dd_dll_handle, "DirectDrawEnumerateA");
	if (!pfn_DirectDrawEnumerate)
		goto D3DError;

	ddrval = pfn_DirectDrawEnumerate( gr_d2d_enum, NULL );
	if ( ddrval != DD_OK ) {
		OutputDebugString( "GR_D3D_INIT: DirectDrawEnumerate failed.\n" );
		goto D3DError;
	}

	for ( i=0; i<Num_d2d_devices; i++)	{
		d3d_device *d2d = (d3d_device *)&D2D_devices[i];

		ddrval = pfn_DirectDrawCreate( d2d->pguid_2d, &lpDD1, NULL );
		if ( ddrval != DD_OK ) {
			OutputDebugString( "GR_D3D_INIT: DirectDrawCreate failed.\n" );
			goto D3DError;
		}

		ddrval = lpDD1->QueryInterface( IID_IDirect3D2, ( LPVOID *) &lpD3D );
		if ( ddrval != DD_OK ) {
			OutputDebugString( "GR_D3D_INIT: QueryInterface failed.\n" );
			goto D3DError;
		}

		ddrval = lpD3D->EnumDevices(gr_d3d_enum, d2d );
		if ( ddrval != DD_OK )	{
			OutputDebugString( "WIN_DD32: D3D enum devices failed. (0x%x)\n");//, ddrval );
		}

		lpD3D->Release();
		lpD3D = NULL;

		lpDD1->Release();
		lpDD1 = NULL;
	}

	// Store Direct3D devices in number of cards
	for ( i=0; i<Num_d3d_devices; i++)	{
		strcpy( Cards[Num_cards].name, D3D_devices[i].name );
		Cards[Num_cards].renderer_type=RENDERER_DIRECT3D;
		Num_cards++;
	}

	return;

D3DError:

	// If any devices haven't been freed, free them
	if(lpD3D!=NULL) {
		lpD3D->Release();
		lpD3D = NULL;
	}

	if(lpDD1!=NULL) {
		lpDD1->Release();
		lpDD1 = NULL;
	}
	//mprintf( "Direct3D Polling failed.\n" );

	return;
}

void check_glide()
{
	static GrHwConfiguration hwconfig;
	module *GlideDLLHandle;

	if ( !(GlideDLLHandle=LoadGlideDLL()) ) 	{
		//mprintf( "Glide DLL not found!\n" );
		return;
	}

	if (!GlideInited) {

		// Check if any 3dfx systems are present
		grSstQueryBoards(&hwconfig);
		if(hwconfig.num_sst<=0) {
			mod_FreeModule(GlideDLLHandle);
			return;
		}

		grGlideInit();
		GlideInited=TRUE;

		if (grSstQueryHardware(&hwconfig)==0 || hwconfig.num_sst<1)
		{
			// Glide is now only shutdown when the application exits
			//grGlideShutdown();
			mod_FreeModule(GlideDLLHandle);
			return;
		}

		grGlideShutdown();
	}

	if(hwconfig.num_sst<=0) {
		mod_FreeModule(GlideDLLHandle);
		return;
	}

	Cards[Num_cards].renderer_type=RENDERER_GLIDE;

	switch(hwconfig.SSTs[0].type) {
		case GR_SSTTYPE_VOODOO:
#if (defined(OEM_VOODOO3) && !defined(USE_ALL_VIDEO_OPTIONS))
			strcpy( Cards[Num_cards].name, "Voodoo3" );
#else
			strcpy( Cards[Num_cards].name, "Voodoo Graphics Family" );
#endif
			break;
		case GR_SSTTYPE_SST96:
			strcpy( Cards[Num_cards].name, "SST96" );
			break;
		case GR_SSTTYPE_AT3D:
			strcpy( Cards[Num_cards].name, "AT3D" );
			break;
		default:
			strcpy( Cards[Num_cards].name, "" );
			break;
	}
	Num_cards++;

	// Glide is now only shutdown when the application exits
	//grGlideShutdown();

	mod_FreeModule(GlideDLLHandle);
}

// Shuts down glide (if it has been initialized)
void shutdown_glide(void) 
{
	if(GlideInited) {
		module *GlideDLLHandle;

		if ( (GlideDLLHandle=LoadGlideDLL()) ) 	{
			grGlideShutdown();
			mod_FreeModule(GlideDLLHandle);
		}
	}
}

// OpenGL function pointer type definitions
typedef const GLubyte *(CALLBACK* LPFN_GLGETSTRING)(GLenum);
typedef BOOL (CALLBACK *LPFN_WGLMAKECURRENT)(HDC, HGLRC); 
typedef HGLRC (CALLBACK *LPFN_WGLCREATECONTEXT)(HDC); 
typedef BOOL (CALLBACK *LPFN_WGLDELETECONTEXT)(HGLRC); 

// Check for OpenGL support, and add it to list
void check_openGL()
{
	LPFN_GLGETSTRING pfn_glGetString=NULL;
	LPFN_WGLMAKECURRENT pfn_wglMakeCurrent=NULL;
	LPFN_WGLCREATECONTEXT pfn_wglCreateContext=NULL;
	LPFN_WGLDELETECONTEXT pfn_wglDeleteContext=NULL;

	// Check for the OpenGL dll, and open it
	if(opengl_dll_handle==NULL) {
		opengl_dll_handle=LoadLibrary("opengl32.dll");
		if(opengl_dll_handle==NULL) {
			OutputDebugString("OpenGL DLL not found.\n");
			return;
		}
	}

	// Get the functions to check the OpenGL renderer
	pfn_glGetString = (LPFN_GLGETSTRING)GetProcAddress(opengl_dll_handle,"glGetString");
	pfn_wglCreateContext = (LPFN_WGLCREATECONTEXT)GetProcAddress(opengl_dll_handle,"wglCreateContext");
	pfn_wglDeleteContext = (LPFN_WGLDELETECONTEXT)GetProcAddress(opengl_dll_handle,"wglDeleteContext");
	pfn_wglMakeCurrent = (LPFN_WGLMAKECURRENT)GetProcAddress(opengl_dll_handle,"wglMakeCurrent");
	if (!pfn_glGetString || !pfn_wglCreateContext || 
		!pfn_wglDeleteContext || !pfn_wglMakeCurrent)   {
		OutputDebugString("Could not obtain pfn_glGetString().\n");
		return;   
	}

	// Make sure the video tab has been initialized first
	if(CurrentVideoTab==NULL) {
		OutputDebugString("The Video Tab was not set.\n");
		return;   		
	}

	// Get the window device context
	// NOTE:the window is now set to the scanning window that is
	//		displayed just before detect_3dcards() is called, and
	//		destroyed afterwords - this function will fail if
	//		it is called in any other context.
	HDC hOpenGLDC = GetDC(CurrentVideoTab->m_MsgDlg.m_hWnd);
	if(hOpenGLDC==NULL) {
		OutputDebugString("GetDC() failed.\n");
		return;   
	}

	// Finds an acceptable pixel format to render to
	PIXELFORMATDESCRIPTOR pfd, pfd_copy;
	int pf;
	
	memset(&pfd, 0, sizeof(pfd));
	pfd.nSize        = sizeof(pfd);
	pfd.nVersion     = 1;
	pfd.dwFlags      = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | PFD_GENERIC_ACCELERATED;
	pfd.iPixelType   = PFD_TYPE_RGBA;
	pfd.cColorBits   = 16;
	pfd.cDepthBits   =16;
	
	// Find the user's "best match" PFD 
	pf = ChoosePixelFormat(hOpenGLDC, &pfd);
	if (pf == 0) 
	{
		OutputDebugString("ChoosePixelFormat() failed.\n");
		return;
	} 
 
	// Try and set the new PFD
	if (SetPixelFormat(hOpenGLDC, pf, &pfd) == FALSE) 
	{
		DWORD ret=GetLastError();
		OutputDebugString("SetPixelFormat() failed.\n");
		return;
	}

	// Get a copy of the newly set PFD
	if(DescribePixelFormat(hOpenGLDC, pf, sizeof(PIXELFORMATDESCRIPTOR), &pfd_copy)==0)
	{
		OutputDebugString("DescribePixelFormat() failed.\n");
		return;
	}

	// Check the returned PFD to see if it is hardware accelerated
	if((pfd_copy.dwFlags & PFD_GENERIC_ACCELERATED)==0 && (pfd_copy.dwFlags & PFD_GENERIC_FORMAT)!=0) {
		OutputDebugString("Returned OpenGL device is not hardware accelerated.\n");
		return;
	}

	// Create an OpenGL context, and make it the current context
	HGLRC ResourceContext;
	ResourceContext = pfn_wglCreateContext((HDC)hOpenGLDC);
	if(ResourceContext==NULL) {
		DWORD ret=GetLastError();
		OutputDebugString("wglCreateContext() failed.\n");
		return;   
	}
	pfn_wglMakeCurrent((HDC)hOpenGLDC, ResourceContext);

	// Add the OpenGL type to card list
	Cards[Num_cards].renderer_type=RENDERER_OPENGL;
	sprintf(Cards[Num_cards].name,"%s",pfn_glGetString(GL_RENDERER));
	Num_cards++;

	// Clean up
	pfn_wglMakeCurrent(NULL, NULL);
	pfn_wglDeleteContext(ResourceContext);
	ReleaseDC(CurrentVideoTab->m_hWnd,hOpenGLDC);
}


// Fills in Cards and Num_cards
void detect_3dcards(int detect_direct3d, int detect_glide, int detect_opengl)
{ 
	if(VideoCardsDetected) return;

	Num_cards = 0;

	//CString no_3d_msg;
	//no_3d_msg.LoadString(IDS_3D_DETECT_NO3D);
	//strcpy( Cards[Num_cards++].name, no_3d_msg );

	// Put in the "none" option
	Cards[Num_cards].renderer_type=RENDERER_NONE;
	strcpy(Cards[Num_cards].name,"");
	Num_cards++;

	// Check for the other types
	if(detect_opengl) check_openGL();
	if(detect_glide) check_glide();
	if(detect_direct3d) check_direct3d();

	//VideoCardsDetected=TRUE;
}

// Creates a string suitable for the list box 
// (combines the renderer_type and the device name)
char *GetFullName(card3d *card)
{
	static char name[1024];
	CString temp;

	// empty out the name
	strcpy(name,"");

	// Copy the renderer type in
	switch(card->renderer_type) {
		case RENDERER_SOFTWARE_8BIT:
			strcpy(name,"Software (8 bit)");
			break;
		case RENDERER_SOFTWARE_16BIT:
			strcpy(name,"Software (16 bit)");
			break;
		case RENDERER_OPENGL:
			strcpy(name,"OpenGL");
			break;
		case RENDERER_DIRECT3D:
			strcpy(name,"Direct3D");
			break;
		case RENDERER_GLIDE:
			strcpy(name,"3Dfx Glide");
			break;
		default:
			temp.LoadString(IDS_3D_DETECT_NO3D);
			strcpy(name,temp.GetBuffer(0));
			break;
	}

	// tack on the Device name (if there is one)
	if(strlen(card->name)>0) {
		strcat(name," - ");
		strcat(name,card->name);
	}

	return(name);
}
