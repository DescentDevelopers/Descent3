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

 * $Source: /home/kevin/cvsstuff/descent3/descent3/Main/FontEditor/FontEditor.cpp,v $
 * $Revision: 1.1.1.1 $
 * $Author: kevinb $
 * $Date: 2003-08-26 03:57:45 $
 *
 * Font Editor main body
 *
 * $Log: not supported by cvs2svn $
 * 
 * 5     11/16/99 3:18p Samir
 * updated fonteditor to include tracking.
 * 
 * 4     5/19/99 8:48p Samir
 * added kerning dump utilitiy.
 * 
 * 3     4/24/99 5:43p Samir
 * incremental checkin
 * 
 * 2     4/17/99 4:05p Samir
 * complete font editor.
 * 
 */

// FontEditor.cpp : Defines the entry point for the application.
//

#include "stdafx.h"

#include "FontEditor.h"
#include "application.h"
#include "pserror.h"
#include "ddio.h"
#include "renderer.h"
#include "grtext.h"
#include "bitmap.h"
#include "mem.h"
#include "args.h"
#include "cfile.h"

#include <stdlib.h>

#define APPNAME "Font Kerner"

renderer_type Preferred_renderer = RENDERER_OPENGL;

void InitSystems(oeApplication *app, bool debug, bool mono, renderer_type renderer);
void InitIO(oeApplication *app);
void InitRenderer(oeApplication *app, renderer_type renderer);
extern void FontKern(const char *fnt_file_name);
extern void FontCreate(const char *fnt_file_source, const char *fnt_file_dest, int min_ascii, int max_ascii, int type);

class oeApplication *FontEditor = NULL;
int System_font_handle = -1;

class appFontKerner: public oeWin32Application
{
	bool shutdown;																				 

public:
  appFontKerner(HINSTANCE hinst): oeWin32Application(APPNAME, OEAPP_FULLSCREEN, hinst) { shutdown = false; };

//	returns 0 if we pass to default window handler.
  virtual int WndProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
			case WM_ACTIVATEAPP:
				if (wParam == (unsigned)(FALSE) && !shutdown && FontEditor) {
					shutdown = true;
					rend_Close();
					ddio_Close();
					ShowWindow((HWND)hwnd, SW_MINIMIZE);
					this->deactivate();
				}
				else if (wParam == (unsigned)(TRUE) && shutdown && FontEditor) {
					this->activate();
					ShowWindow((HWND)hwnd, SW_MAXIMIZE);
					InitIO(this);
					InitRenderer(this, Preferred_renderer);
					shutdown = false;
				}
				break;
			
		}

		return oeWin32Application::WndProc(hwnd, msg, wParam, lParam);
	};
};




int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
  appFontKerner fontkern(hInstance);
	int i;

	fontkern.init();

	GatherArgs(lpCmdLine);

	i = FindArg("-glide");
	if (i) Preferred_renderer = RENDERER_GLIDE;

	i = FindArg("-create");
	if (i) {
		int min_ascii = atoi(GameArgs[i+3]);
		InitSystems(&fontkern, false, false, Preferred_renderer);
		FontCreate(GameArgs[i+1], GameArgs[i+2], min_ascii);
		return 0;
	}

	i = FindArg("-kern");
	if (i) {
		InitSystems(&fontkern, false, false, Preferred_renderer);
		FontKern(GameArgs[i+1]);
		return 0;
	}

	i = FindArg("-view");
	if (i) {
		InitSystems(&fontkern, false, false, Preferred_renderer);
		FontView(GameArgs[i+1]);
		return 0;
	}

	i = FindArg("-kerndump");
	if (i) {
		error_Init(false, false, APPNAME);
		SetMessageBoxTitle(APPNAME " Message");
		mem_Init();
		FontEditor = &fontkern;
		InitIO(&fontkern);
		grfont_Reset();
		grtext_Init();
		FontKernDump(GameArgs[i+1]);
		return 0;
	}

 	MessageBox(NULL, "Need to specify a font to edit.", "Error",MB_OK);

	return 0;
}


void InitSystems(oeApplication *app, bool debug, bool mono, renderer_type renderer)
{
	error_Init(debug, mono, APPNAME);
	SetMessageBoxTitle(APPNAME " Message");

	mem_Init();																																  	InitIO(app);

	cf_OpenLibrary("fonteditor.hog");
	bm_InitBitmaps();
	grfont_Reset();
	grtext_Init();
	InitRenderer(app, renderer);

	System_font_handle = grfont_Load("system.fnt");
	if (System_font_handle < 0) {
		Error("Failed to load system font.");
	}

	FontEditor = app;
}


void InitIO(oeApplication *app)
{
	char path[1024];

	ddio_init_info ddio_init_data;

	ddio_init_data.obj = app;
	ddio_init_data.use_lo_res_time = true;
	ddio_init_data.key_emulation = true;
	ddio_init_data.joy_emulation = true;
	ddio_Init(&ddio_init_data);

	GetCurrentDirectory(sizeof(path), path);
	cf_SetSearchPath(path,NULL);
}


void InitRenderer(oeApplication *app, renderer_type renderer)
{
	renderer_preferred_state req_renderer_state;

	req_renderer_state.width = FIXED_SCREEN_WIDTH;
	req_renderer_state.height = FIXED_SCREEN_HEIGHT;
	req_renderer_state.bit_depth = 16;
	req_renderer_state.gamma = 1.6f;
	req_renderer_state.vsync_on = true;
	req_renderer_state.filtering = true;
	req_renderer_state.mipping = false;

		if (!rend_Init(renderer, app, &req_renderer_state)) {
			Error("Failed to initialize renderer.");
		}
}


void FontDoIO()
{
	FontEditor->defer();
	ddio_Frame();
}
