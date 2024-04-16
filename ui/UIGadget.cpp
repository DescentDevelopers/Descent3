/*
 * $Logfile: /DescentIII/Main/ui/UIGadget.cpp $
 * $Revision: 28 $
 * $Date: 10/21/99 3:31p $
 * $Author: Kevin $
 *
 *	Gadget code
 *
 * $Log: /DescentIII/Main/ui/UIGadget.cpp $
 * 
 * 28    10/21/99 3:31p Kevin
 * mac merge & fix for slider
 * 
 * 27    4/14/99 1:52a Jeff
 * fixed case mismatched #includes
 * 
 * 26    3/23/99 9:02p Samir
 * always null hotkey when creating gadget!
 * 
 * 25    3/01/99 7:29p Samir
 * fixed an issue with slaves getting freed after their master was
 * destroyed for sliders.
 * 
 * 24    2/22/99 8:13p Samir
 * added slave gadget system that only uses mouse input from window, but
 * defers keys to its master.
 * 
 * 23    2/21/99 6:36p Samir
 * focusing changes and key input changes to ui.,
 * 
 * 22    2/17/99 8:32p Samir
 * fixed bugs in remove gadget, added callbacks for attaching and
 * detaching gadgets.
 * 
 * 21    2/16/99 11:58a Samir
 * small fixes and added gadget notification.
 * 
 * 20    10/19/98 12:18p Samir
 * made tabbing between edit controls possible.
 * 
 * 19    10/13/98 8:23p Samir
 * communication between gadget and window and edit system to work
 * together.
 * 
 * 18    8/26/98 12:25p Samir
 * fixed the whole editbox, keyboard access mess.
 * 
 * 17    8/25/98 7:08p Samir
 * select how keys are sent to a gadget.
 * 
 * 16    6/22/98 7:30p Samir
 * set focus on gadget when calling LockFocus.
 * 
 * 15    6/05/98 5:35p Samir
 * massive improvement in UI keyboard interface.
 * 
 * 14    5/25/98 8:18p Samir
 * Enter keys are passed to gadgets too.
 * 
 * 13    4/24/98 2:43a Samir
 * added IsLocked function.
 * 
 * 12    3/24/98 4:26p Samir
 * fixed minor bug in TrapKey.
 * 
 * 11    3/10/98 7:24p Samir
 * Save original width and height.
 * 
 * 10    3/09/98 2:57p Samir
 * Remove gadget from window called from gadget destroy.
 * 
 * 9     2/15/98 7:07p Samir
 * Revamped way keys like up and down are handled in window system.
 * 
 * 8     2/13/98 6:35p Samir
 * Fixed some gadget creation/destruction inconsistancies
 * 
 * 7     1/30/98 7:05p Samir
 * Each gadget now has their own coordinate system.
 * 
 * 6     1/23/98 5:34p Samir
 * Allow for nonpersistant UIItems.
 * 
 * 5     1/18/98 4:22p Samir
 * Implemented new UIItem system.
 * 
 * 4     1/13/98 4:27p Samir
 * Keyboard timing delay stored UIGadget.  Moved from UIEdit.
 * 
 * 3     1/05/98 4:36p Samir
 * Moved centering and fittext control flags to UIGadget class.
 * 
 * 2     1/02/98 12:43p Samir
 * Gadget processing functional.
 * 
 * 1     12/30/97 4:36p Samir
 * Initial revision
 * 
 * $NoKeywords: $
 */

#include "UIlib.h"
#include "Macros.h"
#include <string.h>


int UIGadget::m_LastKey = 0;
int UIGadget::m_LastKeyCount = 0;
float UIGadget::m_LastKeyTime = 0.0f;


UIGadget::UIGadget()
	:UIObject()
{
	m_ID = 0;
	m_Wnd = NULL;
	m_Prev = m_PrevSlave = NULL;
	m_Next = m_NextSlave = NULL;
	m_MasterGadget = NULL;
	m_SlaveGadgets = NULL;
	m_CurrentSlave = NULL;
	m_infocus = false;
}


UIGadget::~UIGadget()
{
}


//	ALL Gadgets must have a parent.
void UIGadget::Create(UIWindow *wnd, int id, int x, int y, int w, int h, int flags)
{
	ASSERT(!IsCreated());
	ASSERT(wnd);

	m_ID = id;
	m_Wnd = wnd;

	m_Disabled = false;
	m_Flags = flags;
	m_infocus = false;

//	add to interface's list.
	UIObject::Create(x, y, w, h);

//	save original width and height
	m_SavedW = m_W;
	m_SavedH = m_H;
	m_Hotkey = 0;

//	handle formatting issues.
	OnFormat();
	m_MasterGadget = NULL;
	m_SlaveGadgets = NULL;
	m_CurrentSlave = NULL;
	m_NextSlave  = m_PrevSlave = NULL;
	m_Datum = 0;

	m_Wnd->AddGadget(this);
}

void UIGadget::Destroy()			
{
// slaves are freed or destroyed by now.
	m_SlaveGadgets = NULL;

// remove gadget from windw list.
	if (m_Wnd) {
		m_Wnd->RemoveGadget(this);
	}
	OnDestroy();
	UIObject::Destroy();
}


//	sets hotkey for gadget
void UIGadget::SetHotkey(int key)
{
	m_Hotkey = key;
}


void UIGadget::SetFlag(int flag)
{
	m_Flags |= flag;
}


void UIGadget::ClearFlag(int flag)
{
	m_Flags &= (~flag);
}


//	locks input on this gadget
void UIGadget::LockFocus()
{
//	m_Wnd->SetFocusOnGadget(this);
	m_Wnd->LockFocusOnGadget(this);
}

void UIGadget::UnlockFocus()
{
	m_Wnd->UnlockFocusOnGadget();
}


//	called before drawing usually.  sets x,y,w,h optionally.
void UIGadget::OnFormat()
{
	if (m_Flags & UIF_CENTER) 
		m_X = m_Wnd->W()/2 - m_W/2;
}


//	interprets input on this gadget.  this gadget is in focus.
void UIGadget::Process(bool do_keys, bool do_mouse, bool do_user)
{
// do mouse processing on this gadget if and only if no slave processing was done.
	if (UI_input.b1_status == UIMSEBTN_PRESSED && do_mouse) {
		OnMouseBtnDown(UILMSEBTN);
		UI_input.b1_last_status = UIMSEBTN_PRESSED;		//DAJ use to latch the UI mouse button
	} 
	else if (UI_input.b1_status == UIMSEBTN_RELEASED && do_mouse) {
		OnMouseBtnUp(UILMSEBTN);
		UI_input.b1_last_status = UIMSEBTN_RELEASED;	//DAJ use to latch the UI mouse button
	} 
	else if (UI_input.b1_status == UIMSEBTN_CLICKED && do_mouse) {
		OnMouseBtnDown(UILMSEBTN);
		OnMouseBtnUp(UILMSEBTN);
	} 
	else if (do_keys) {
	// keyboard processing.
		switch (UI_input.key)
		{
			case KEY_ENTER:
				if ((UI_input.key_status == UIKEY_PRESSED) || 
					(UI_input.key_status == UIKEY_CLICKED)) {
					OnSelect();
				}
				break;
		}

		if (UI_input.key_status == UIKEY_PRESSED) {
			if (TrapKey(UI_input.key)) 
				OnKeyDown(UI_input.key);
		}
		else if (UI_input.key_status == UIKEY_RELEASED) {
			UntrapKey(UI_input.key);
			OnKeyUp(UI_input.key);
		}
		else if (UI_input.key_status == UIKEY_CLICKED) {
			if (TrapKey(UI_input.key)) 
				OnKeyDown(UI_input.key);
			UntrapKey(UI_input.key);
			OnKeyUp(UI_input.key);
		}
	}

// run gadget's user process
	if (do_user) {
		OnUserProcess();
	}
}


//	when gadget is selected, this function will AT LEAST be called.
//	typically this function is called by a child class when we want
//	to tell the caller that this gadget was selected for something.
void UIGadget::OnSelect()
{
	if (m_MasterGadget && CHECK_FLAG(m_Flags, UIF_NOTIFYMASTERSEL)) {
		m_MasterGadget->OnNotifySelect(this);
	}
	else {
		UI_output.id = m_ID;
		UI_output.gadget = this;
	}
}



void UIGadget::AttachSlaveGadget(UIGadget *gadget)
{
// attach to list.
	ASSERT(gadget);
	ASSERT(!m_MasterGadget);			// slaves can't own each other (sorry, been reading Uncle Tom's Cabin in class)

	gadget->m_Flags |= UIF_SLAVE;
	gadget->m_MasterGadget = this;
	if (m_SlaveGadgets) {
		m_SlaveGadgets->m_NextSlave = gadget;
	}
	gadget->m_PrevSlave = m_SlaveGadgets;
	gadget->m_NextSlave = NULL;
	m_SlaveGadgets = gadget;
}


void UIGadget::DetachSlaveGadget(UIGadget *gadget)
{
	if (!gadget) {
		return;
	}

	ASSERT(gadget->m_MasterGadget == this);
	ASSERT(!m_MasterGadget);			// slaves can't own each other (sorry, been reading Uncle Tom's Cabin in class)

	if (gadget == m_SlaveGadgets) {
		m_SlaveGadgets = gadget->m_PrevSlave;
	}
	if (gadget == m_CurrentSlave) {
		m_CurrentSlave->LostFocus();
		m_CurrentSlave = gadget->m_PrevSlave ? gadget->m_PrevSlave : gadget->m_NextSlave;
	}
	if (gadget->m_PrevSlave) {
		gadget->m_PrevSlave->m_NextSlave = gadget->m_NextSlave;
	}
	if (gadget->m_NextSlave) {
		gadget->m_NextSlave->m_PrevSlave = gadget->m_PrevSlave;
	}

	gadget->m_MasterGadget = NULL;
	gadget->m_Flags &= (~UIF_SLAVE);
}


///////////////////////////////////////////////////////////////////////////////
//	PRIVATE FUNCTIONS

// called by window system
void UIGadget::LostFocus()
{
	this->OnLostFocus();					// should invoke gadget's OnLostFocus function
	if (m_CurrentSlave) {
		ASSERT(m_SlaveGadgets);
		m_CurrentSlave->LostFocus();
		m_CurrentSlave = NULL;
	}
	m_infocus = false;
}


// called by window system 
void UIGadget::GainFocus()
{
//	m_LastKey = 0;
//	m_LastKeyTime = 0.0f;

	m_infocus = true;
	m_CurrentSlave = NULL;
	this->OnGainFocus();					// should invoke gadget's OnGainFocus function
}


void UIGadget::CheckFocusOnSlaves(int mx, int my)
{
	UIGadget *slave = m_SlaveGadgets;

	if (CHECK_FLAG(m_Flags, UIF_SLAVE)) 
		return;

	while (slave)
	{
		if (PT_IN_GADGET(m_Wnd, slave, mx, my)) {
			if (m_CurrentSlave) {
				m_CurrentSlave->LostFocus();
			}
			m_CurrentSlave = slave;
			m_CurrentSlave->GainFocus();
			break;
		}
		slave = slave->m_PrevSlave;
	}

	if (!slave) {
		if (m_CurrentSlave) {
			m_CurrentSlave->LostFocus();
			m_CurrentSlave = NULL;
		}
	}
}


// used to manage keypresses so repeating isn't quick. 
bool UIGadget::TrapKey(int key)
{
	float time;
	time = timer_GetTime();

//	take care of multiple keypresses and key down repeats. handle timing
// so it feels 'natural'
	if (m_LastKey && m_LastKey == key) {
		if (m_LastKeyCount == 1) {
			if ((time - m_LastKeyTime) < KEYDOWN_FIRST_DELAY) 
				return false;
		}
		else if ((time - m_LastKeyTime) < KEYDOWN_REPEAT_DELAY) {
			return false;
		}
		m_LastKeyCount++;
	}
	else {
		m_LastKeyCount = 1;
	}

	m_LastKey = key;
	m_LastKeyTime = time;

	return true;
}


void UIGadget::UntrapKey(int key)
{
//	if the key previously pressed went up, then 
//	turn off key repeat management.
	if (key == m_LastKey) {
		m_LastKeyCount = 0;
		m_LastKey = 0;
	}
}


// is this gadget's focus locked?
bool UIGadget::IsLocked() const
{
	if (m_Wnd->m_LockedGadget == this)
		return true;
	else
		return false;
}


UIGadget *UIGadget::GetPrevUsableGadget()
{
	UIGadget *gadget = this;
	UIGadget *next_gadget = NULL;

	while (gadget->m_Prev)
	{
		gadget = gadget->m_Prev;
		if (!gadget->IsDisabled()) {
			next_gadget = gadget;
			break;
		}
	}

	return next_gadget;
}

	
UIGadget *UIGadget::GetNextUsableGadget()
{
	UIGadget *gadget = this;
	UIGadget *next_gadget = NULL;

	while (gadget->m_Next)
	{
		gadget = gadget->m_Next;
		if (!gadget->IsDisabled()) {
			next_gadget = gadget;
			break;
		}
	}

	return next_gadget;
}

