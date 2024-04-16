/*
 * $Logfile: /DescentIII/Main/mac/MACAPP.H $
 * $Revision: 1.1.1.1 $
 * $Date: 2003/08/26 03:58:14 $
 * $Author: kevinb $
 *
 *	Application object for Mac
 *
 * $Log: MACAPP.H,v $
 * Revision 1.1.1.1  2003/08/26 03:58:14  kevinb
 * initial 1.5 import
 *
 * 
 * 3     4/12/00 7:08p Matt
 * From Duane for 1.4
 * 
 * 2     10/21/99 1:55p Kevin
 * Mac Merge!
 * 
 * 1     7/28/99 2:31p Kevin
 * Mac only stuff
 * 
 * 
 * $NoKeywords: $
 */
#ifndef MACAPP_H
#define MACAPP_H
#define MAX_MSG_FUNCTIONS			64
#ifndef CW_USEDEFAULT
#define CW_USEDEFAULT	0
#endif
#include "Application.h"
//	This structure is used to retrieve and set 
typedef struct tMacAppInfo {
	unsigned flags;							// Application Flags
	CWindowPtr hwnd;									// Window Handle
	int wnd_x, wnd_y, wnd_w, wnd_h;		// Window dimensions
} tMacAppInfo;
/*	Mac Application Object
		This object entails initialization and cleanup of all operating system
		elements, as well as data that libraries may need to initialize their 
		systems.  
	The Mac Application object creates the application window and housekeeps
	the window and instance handle for the application.
	We also allow the option of setting these handles from outside the Application object.
  
tOEMacMsgCallback:
	Callbacks return a 0 if we don't want to call the default action for the message, otherwise return 1
	Callbacks are executed in the Window's message procedure, so the calling program need not manually run
	the handler (although, I give you a function to do just that, if you can't depend on the WndProc).
	NOTE: the callbacks are executed in the window's root procedure and not in the inherited WndProc function.
	This means, that callbacks have priority over the application defined WndProc.
	General priority of message handling.
		Outrage Window Procedure:
			takes care of window creation, destruction and system menus.
			if message's window not registered then 
				calls Mac Window Procedure
			else 
				runs OEMacMsgCallbacks for that message
				if OEMacMsgCallback functions return 0 then
					leave Outrage Window Procedure
				calls oeMacApplication->WndProc hierarchy which
					optionally calls Mac Window Procedure.
			endif
*/
typedef int (*tOEMacMsgCallback)(WindowPtr,unsigned,unsigned,long);
class oeMacApplication: public oeApplication
{
#if defined(OEAPP_INTERNAL_MODULE)
public:
#else
private:
#endif
//	WindowPtr	m_hWnd;
	CWindowPtr	m_hWnd;
	
	bool m_WasCreated;						// Tells us if this app created the window handle or not.
	int m_NumMsgFn;							// Number of message functions.
	struct {										// assign functions to messages.
		unsigned msg;
		tOEMacMsgCallback fn;
	}
	m_MsgFn[MAX_MSG_FUNCTIONS];
	void (*m_DeferFunc)(bool);				// function to call when deffering to OS (OnIdle for instance)
	char m_WndName[64];						// name of window.
	static bool os_initialized;			// is the OS check initialized?
	static bool first_time;					// first time init?
private:
	int defer_block();						// real defer code.
public:
//	Creates the window handle
	oeMacApplication(const char *name, unsigned flags);	  
	oeMacApplication(tMacAppInfo *appinfo);	  
	
	oeMacApplication(void) {};	  
	virtual ~oeMacApplication();
//	initializes the object
	virtual void init();
//	Function to retrieve information from object through a platform defined structure.
	virtual void get_info(void *appinfo);
//	defer returns some flags.   essentially this function defers program control to OS.
	virtual unsigned defer();
//	set a function to run when deferring to OS.
	virtual void set_defer_handler(void (*func)(bool));
//	delays app for a certain amount of time
	virtual void delay(float secs);
//	Sizes the displayable region of the app (the window)
	void set_sizepos(int x, int y, int w, int h);
//	These functions allow you to add message handlers.
	bool add_handler(unsigned msg, tOEMacMsgCallback fn);
// These functions remove a handler
	bool remove_handler(unsigned msg, tOEMacMsgCallback fn);
// Run handler for message (added by add_handler)
	bool run_handler(WindowPtr wPtr, unsigned msg, unsigned wParam, long lParam);
//	clears handler list
	void clear_handlers();
//	These variables are only accessable to modules that have DD_ACCESS.
#if defined(DD_ACCESS_RING)
public:
#else
private:
#endif
	unsigned m_Flags;
	int m_X, m_Y, m_W, m_H;				// window dimensions.
private:
	void os_init();						// initializes OS components.
};

#endif

//void SetupScreen (GDHandle *hGD, CGrafPtr *m_hWnd, short *numDevices);
//void SetupScreen (GDHandle *hGD, CGrafPtr *m_hWnd);
void SetupScreen (CGrafPtr *m_hWnd);
void ShutdownScreen(CGrafPtr *m_hWnd);

void SwitchDSpContex(int newContext);

void PauseDSpContext();
void ResumeDSpContext();