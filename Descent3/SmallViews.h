/*
 * $Logfile: /DescentIII/Main/SmallViews.h $
 * $Revision: 11 $
 * $Date: 5/01/99 12:41a $
 * $Author: Matt $
 *
 * Header for SmallViews.cpp
 *
 * $Log: /DescentIII/Main/SmallViews.h $
 *
 * 11    5/01/99 12:41a Matt
 * Added static when a small viewer object dies.
 *
 * 10    4/19/99 1:31p Matt
 * Made guided missile small view use title and crosshairs flag, instead
 * of doing special check for guided missile.
 *
 * 9     4/12/99 1:10a Matt
 * Added labels for small views.
 *
 * 8     3/22/99 4:26p Samir
 * added toggles for guided missile view and reticle.
 *
 * 7     2/10/99 1:47p Matt
 * Changed object handle symbolic constants
 *
 * 6     1/13/99 12:42p Matt
 * Added code to close a popup view
 *
 * 5     5/26/98 11:37a Matt
 * Changed small view system to allow the popup window in any of the three
 * positions, to allow any window to be the "bigger" size, and to restore
 * the old view when a popup view goes away.
 *
 * 4     2/09/98 3:19p Matt
 * Added function to return the viewer object for a small view
 *
 * 3     2/04/98 12:50a Matt
 * Added ability to view from a gun point in small views.
 * Made pop-up view separate from and bigger than other small views.
 * Added real keys to set view mode in the three small views.
 * Changed rendering functions a bit to accommodate smallview changes.
 *
 * 2     1/30/98 2:55p Matt
 * Added SmallViews system
 *
 * 1     1/29/98 11:15a Matt
 *
 */

#include "descent.h"

// Values for the small view windows
#define SVW_LEFT 0
#define SVW_CENTER 1
#define SVW_RIGHT 2

// Small view flags
#define SVF_POPUP 1       // This is a temporary window
#define SVF_BIGGER 2      // This window is drawn a little bigger than the normal window
#define SVF_REARVIEW 4    // Draw looking backward from the viewer
#define SVF_TIMED 8       // This window is timer-based. DO NOT USE THIS FLAG WHEN CALLING CreateSmallView().
#define SVF_CROSSHAIRS 16 // This window has crosshairs
#define SVF_STATIC 32     // Window is showing static.  The object handle is unused

// if guided missile smallview is up, this will be true.
extern bool Guided_missile_smallview;

// Create a new small view.  If there is already a view in the given window, the old view gets blown away.
// Parameters:	window - which window to open.  See constants in SmallViews.h
//					objhandle - handle for the object to view from
//					flags - various view attributes.  See defines in header file.
//					time - how long to keep the window up.  If 0, keep up indefinitely
//					zoom - the zoom for this window.  If 0, use the default zoom
//					gun_num - which gun to view from. if -1, use viewer's center and orientation.
//					label - the label for the window
// Returns:		which window was opened, or -1 if window couldn't be created
int CreateSmallView(int window, int objhandle, int flags = 0, float time = 0.0, float zoom = D3_DEFAULT_ZOOM,
                    int gun_num = -1, char *label = NULL);

// Called to get rid of all the small views & init system
void ResetSmallViews();

// Draw all the active small views
void DrawSmallViews();

// Returns the viewer object for the specified small view.
// If the view isn't active, returns OBJECT_HANDLE_NONE.
int GetSmallViewer(int window);

// Get rid of a small view
// Parameters:	window - the window to get rid of
void CloseSmallView(int window);

// Get rid of a small view if it's a popup window
// Parameters:	window - the window to get rid of
void ClosePopupView(int window);
