/*
 * $Logfile: /DescentIII/Main/ui/UIWindow.cpp $
 * $Revision: 39 $
 * $Date: 5/02/99 2:09a $
 * $Author: Samir $
 *
 *	Interface code.
 *
 * $Log: /DescentIII/Main/ui/UIWindow.cpp $
 *
 * 39    5/02/99 2:09a Samir
 * don't process input on disabled gadgets.
 *
 * 38    4/26/99 9:11p Matt
 * Took out saturation used by old newui.
 *
 * 37    4/22/99 4:28p Samir
 * made gadget clip check include the right and bottom boundaries of the
 * screen.
 *
 * 36    4/21/99 12:42p Samir
 * clip gadgets on screen.
 *
 * 35    4/14/99 1:54a Jeff
 * fixed case mismatched #includes
 *
 * 34    3/01/99 6:56p Samir
 * saturate text for old ui menus (UIF_PROCESS_ALL)
 *
 * 32    3/01/99 5:00a Samir
 * whenever m_GadgetCur is being NULLed, do a lose focus on it first.
 *
 * 31    2/26/99 2:10a Samir
 * added accelerator table.
 *
 * 30    2/22/99 8:13p Samir
 * added slave gadget system that only uses mouse input from window, but
 * defers keys to its master.
 *
 * 29    2/21/99 6:36p Samir
 * focusing changes and key input changes to ui.,
 *
 * 28    2/17/99 8:32p Samir
 * fixed bugs in remove gadget, added callbacks for attaching and
 * detaching gadgets.
 *
 * 27    2/16/99 11:58a Samir
 * small fixes and added gadget notification.
 *
 * 26    10/19/98 12:18p Samir
 * made tabbing between edit controls possible.
 *
 * 25    10/13/98 8:23p Samir
 * communication between gadget and window and edit system to work
 * together.
 *
 * 24    10/06/98 7:31p Samir
 * both tab keys and arrow keys scroll through gadgets.
 *
 * 23    9/10/98 10:20a Samir
 * when removing a gadget from a window, check if it's being used by the
 * user intefrace, if it is then remove it from that.
 *
 * 22    8/31/98 12:36p Samir
 * implemented dialogversion of window.
 *
 * 21    8/27/98 2:52p Samir
 * changing window system to work in two modes.
 *
 * 20    8/26/98 12:25p Samir
 * fixed the whole editbox, keyboard access mess.
 *
 * 19    8/25/98 7:08p Samir
 * select how keys are sent to a gadget.
 *
 * 18    8/25/98 6:33p Kevin
 * PXO screens
 *
 * 17    8/25/98 5:27p Samir
 * new focusing rules for keyboard.
 *
 * 16    6/05/98 5:35p Samir
 * massive improvement in UI keyboard interface.
 *
 * 15    3/10/98 12:48p Samir
 * Added window centering ability.
 *
 * 14    3/09/98 2:57p Samir
 * Remove gadget from window called from gadget destroy.
 *
 * 13    3/05/98 6:38p Samir
 * Fixed key presses within windows.
 *
 * 12    2/15/98 7:07p Samir
 * Revamped way keys like up and down are handled in window system.
 *
 * 11    2/13/98 6:35p Samir
 * Fixed some gadget creation/destruction inconsistancies
 *
 * 10    2/02/98 7:36p Samir
 * Added UIObject::Destroy.
 *
 * 9     1/30/98 7:05p Samir
 * Each gadget now has their own coordinate system.
 *
 * 8     1/26/98 10:54a Samir
 * Fixed bug in UIWindow returning -2 always.
 *
 * 7     1/23/98 5:45p Samir
 * Call OnUserProcess after default Window Process.
 *
 * 6     1/18/98 4:23p Samir
 * Moved object destruction code to a virtual OnDestroy function for
 * gadgets.  Must destroy window too.
 *
 * 5     1/13/98 4:26p Samir
 * Changed process of setting and getting focus.
 *
 * 4     1/08/98 12:17p Samir
 * Added TitledWindow and modified the UI interface for polling.
 *
 * 3     1/05/98 11:10a Jason
 * FROM SAMIR: Draw background rectangle for menus
 *
 * 2     1/02/98 12:48p Samir
 * Redid setting and releasing keyboard/mouse focus for gadgets.
 *
 * 1     12/30/97 4:36p Samir
 * Initial revision
 *
 * $NoKeywords: $
 */

#include "UIlib.h"
#include "Macros.h"

//	the window font for all windows
int UIWindow::m_WindowFont = 0;

//	----------------------------------------------------------------------------
//	UIWindow class

UIWindow::UIWindow() : UIObject() {
  m_GadgetHead = NULL;
  m_GadgetTail = NULL;
  m_GadgetCur = NULL;
  m_BackItem = NULL;
  m_HoldHotkeys = false;
  m_CurGadgetInGroup = false;
  m_ResetCurGadget = false;
  m_naccels = 0;
}

UIWindow::~UIWindow() {
  if (m_GadgetTail) {
    Destroy();
  }
}

//	create a window at the specified coords.
void UIWindow::Create(int x, int y, int w, int h, int flags) {
  m_FontHandle = m_WindowFont;
  m_LockedGadget = NULL;
  m_BackItem = UIPrimativeItem(TRANSPARENT_COLOR32).CopyUIItem();
  m_BackColor = GR_RGB(0, 0, 0);
  m_Flags = flags;
  m_GadgetTail = NULL;
  m_GadgetHead = NULL;
  m_LastKey = 0;
  m_LastKeyTime = 0.0f;
  m_HoldHotkeys = false;
  m_ResetCurGadget = false;
  m_CurGadgetInGroup = false;
  m_naccels = 0;

  if (m_Flags & UIF_CENTER) {
    x = UI_screen_width / 2 - w / 2;
    y = UI_screen_height / 2 - h / 2;
  }

  UIObject::Create(x, y, w, h);
}

void UIWindow::Destroy() {
  OnDestroy();

  while (m_GadgetTail) {
    UIGadget *gadget = m_GadgetTail;
    gadget->Destroy(); // gadget must be destroyed AFTER it's removed from list.
  }
  m_GadgetTail = NULL;
  m_GadgetHead = NULL;

  SetBackItem(NULL);

  UIObject::Destroy();
}

//	interface functions
// adds a UIObject to the interface's draw list.
void UIWindow::AddGadget(UIGadget *gadget) {
  ASSERT(IsCreated());
  ASSERT(gadget->IsCreated());

  //	perform a linked list add.
  if (!m_GadgetHead) {
    m_GadgetHead = gadget;
    m_GadgetTail = m_GadgetHead;
  } else {
    m_GadgetTail->m_Next = gadget;
    gadget->m_Prev = m_GadgetTail;
    gadget->m_Next = NULL;
    m_GadgetTail = gadget;
  }

  if (m_GadgetCur) {
    m_GadgetCur->LostFocus();
    m_GadgetCur = NULL;
  }
  m_ResetCurGadget = true;
  m_CurGadgetInGroup = false;

  gadget->m_Wnd = this;

  gadget->OnAttachToWindow();
}

// removes a UIObject from the draw list.
void UIWindow::RemoveGadget(UIGadget *gadget) {
  if (!gadget)
    return;

  ASSERT(IsCreated());
  ASSERT(gadget->IsCreated());
  ASSERT(gadget->m_Wnd == this);

  gadget->OnDetachFromWindow();

  //	remove object from list.
  if (gadget->m_Prev)
    gadget->m_Prev->m_Next = gadget->m_Next;
  if (gadget->m_Next)
    gadget->m_Next->m_Prev = gadget->m_Prev;

  if (gadget == m_GadgetTail)
    m_GadgetTail = gadget->m_Prev;

  if (gadget == m_GadgetHead)
    m_GadgetHead = gadget->m_Next;

  if (gadget == m_GadgetCur) {
    m_GadgetCur->LostFocus();
    m_GadgetCur = NULL;
    m_ResetCurGadget = true;
    m_CurGadgetInGroup = false;
  }

  if (gadget == m_LockedGadget)
    m_LockedGadget = NULL;

  //	break link.
  gadget->m_Prev = NULL;
  gadget->m_Next = NULL;
  gadget->m_Wnd = NULL;
}

// forces a particular gadget into focus, always until unlocked.
void UIWindow::LockFocusOnGadget(UIGadget *gadget) { m_LockedGadget = gadget; }

void UIWindow::UnlockFocusOnGadget() { m_LockedGadget = NULL; }

// forces all keyinput onto one gadget.
void UIWindow::LockKeyFocusOnGadget(UIGadget *gadget) {}

//	Process
//		run this to handle user input inside an interface
//		sets focus on a control
//			m_GadgetCur is gadget in focus.

int UIWindow::Process() {
  ASSERT(IsCreated());
  bool no_process = false; // don't do input for disabled gadgets.

  if (m_ResetCurGadget) {
    // find first valid gadget to set focus to.
    UIGadget *start_gadget, *gadget;
    start_gadget = gadget = m_GadgetHead;
    if (gadget) {
      bool in_group = false;
      do {
        if (CHECK_FLAG(gadget->GetFlags(), UIF_GROUP_START)) {
          in_group = true;
        }
        if (!gadget->IsDisabled() && !CHECK_FLAG(gadget->GetFlags(), UIF_SLAVE)) {
          if (m_GadgetCur) {
            m_GadgetCur->LostFocus();
          }
          m_GadgetCur = gadget;
          m_GadgetCur->GainFocus();
          m_CurGadgetInGroup = in_group;
          break;
        }
        if (CHECK_FLAG(gadget->GetFlags(), UIF_GROUP_END)) {
          in_group = false;
        }
        gadget = (gadget->m_Next) ? gadget->m_Next : m_GadgetHead;
      } while (gadget != start_gadget);
    }

    m_ResetCurGadget = false;
  }

  //	setup default output values
  UI_output.gadget = NULL;
  UI_output.id = -1;

  // we can do focus processing if no one gadget has locked the keyboard, mouse focus.
  if (!m_LockedGadget) {
    UIGadget *test_gadget = NULL;
    if (UI_input.b1_status == UIMSEBTN_PRESSED || UI_input.b1_status == UIMSEBTN_CLICKED) {
      // start from top most gadget down for mouse focus check.
      bool in_group = false;
      for (test_gadget = m_GadgetTail; test_gadget != NULL; test_gadget = test_gadget->m_Prev) {
        if (CHECK_FLAG(test_gadget->GetFlags(), UIF_GROUP_END)) { // going from end of list to start, so reverse
          in_group = true;
        }
        if (PT_IN_GADGET(this, test_gadget, UI_input.mx, UI_input.my)) {
          if (!CHECK_FLAG(test_gadget->GetFlags(), UIF_SLAVE)) {
            if (!test_gadget->IsDisabled()) {
              break;
            } else {
              test_gadget = NULL;
              no_process = true;
              break;
            }
          }
        }

        if (CHECK_FLAG(test_gadget->GetFlags(), UIF_GROUP_START)) {
          in_group = false;
        }
      }

      // switch focus from old gadget to new.
      if (test_gadget) {
        if (m_GadgetCur && m_GadgetCur != test_gadget) {
          m_GadgetCur->LostFocus();
          m_GadgetCur = NULL;
        }
        if (!m_GadgetCur) {
          m_GadgetCur = test_gadget;
          m_GadgetCur->GainFocus();
          m_CurGadgetInGroup = in_group;
        }
        test_gadget->CheckFocusOnSlaves(UI_input.mx, UI_input.my);
      }

    } else {
      // check for keyboard switching.
      if (UI_input.key_status == UIKEY_PRESSED) {
        if (TrapKey(UI_input.key)) {
          OnKeyDown(UI_input.key);
        }
      } else if (UI_input.key_status == UIKEY_RELEASED) {
        UntrapKey(UI_input.key);
        OnKeyUp(UI_input.key);
      } else if (UI_input.key_status == UIKEY_CLICKED) {
        if (TrapKey(UI_input.key)) {
          OnKeyDown(UI_input.key);
        }
        UntrapKey(UI_input.key);
        OnKeyUp(UI_input.key);
      }
    }
  } else {
    // always assert that current gadget is locked gadget.
    // if not, then m_CurGadgetInGroup can be wrong.
    //	ASSERT(m_GadgetCur == m_LockedGadget);
    //	m_GadgetCur = m_LockedGadget;
  }

  //	process input for current gadget in focus.
  if (m_LockedGadget) {
    m_LockedGadget->Process(true);
  } else if (m_GadgetCur && !m_GadgetCur->IsDisabled()) {
    // check what slave gadget a mouse event occurs inside m_GadgetCur
    if (!no_process) {
      if (m_GadgetCur->m_CurrentSlave) {
        m_GadgetCur->m_CurrentSlave->Process(false, true);
      }
      m_GadgetCur->Process(true, m_GadgetCur->m_CurrentSlave ? false : true);
    }
  }

  // perform user processing of window input.
  OnUserProcess();

  return UI_output.id;
}

// overridable: runs after window is processed, to supplement it.
void UIWindow::OnUserProcess() {}

//	behavior when key is pressed.
void UIWindow::OnKeyDown(int key) {
  UIGadget *gadget, *start_gadget;
  bool in_group;

  //	advance down gadget list.
  if (key == KEY_TAB) {
    if (!m_GadgetCur) {
      m_ResetCurGadget = true;
      return;
    }
    start_gadget = gadget = m_GadgetCur;
    in_group = m_CurGadgetInGroup;
    do {
      if (CHECK_FLAG(gadget->GetFlags(), UIF_GROUP_START)) {
        in_group = true;
      }
      if (CHECK_FLAG(gadget->GetFlags(), UIF_GROUP_END)) {
        in_group = false;
      }
      gadget = (gadget->m_Next) ? gadget->m_Next : m_GadgetHead;
      if (!gadget->IsDisabled() && !CHECK_FLAG(gadget->GetFlags(), UIF_SLAVE) && !in_group) {
        start_gadget->LostFocus();
        gadget->GainFocus();
        m_CurGadgetInGroup = (CHECK_FLAG(gadget->GetFlags(), UIF_GROUP_START) > 0);
        m_GadgetCur = gadget;
        break;
      }
    } while (gadget != start_gadget);
  }
  if (key == (KEY_SHIFTED + KEY_TAB)) {
    if (!m_GadgetCur) {
      m_ResetCurGadget = true;
      return;
    }
    start_gadget = gadget = m_GadgetCur;
    in_group = m_CurGadgetInGroup;
    do {
      gadget = (gadget->m_Prev) ? gadget->m_Prev : m_GadgetTail;
      if (CHECK_FLAG(gadget->GetFlags(), UIF_GROUP_END)) {
        in_group = true;
      }
      if (CHECK_FLAG(gadget->GetFlags(), UIF_GROUP_START)) {
        in_group = false;
      }
      if (!gadget->IsDisabled() && !CHECK_FLAG(gadget->GetFlags(), UIF_SLAVE) && !in_group) {
        start_gadget->LostFocus();
        gadget->GainFocus();
        m_CurGadgetInGroup =
            (CHECK_FLAG(gadget->GetFlags(), UIF_GROUP_END)) || (CHECK_FLAG(gadget->GetFlags(), UIF_GROUP_START));
        m_GadgetCur = gadget;
        break;
      }
    } while (gadget != start_gadget);
  }

  // handle direction keys only in a group
  if (m_CurGadgetInGroup) {

    if (key == KEY_UP || key == KEY_LEFT) {
      if (!m_GadgetCur) {
        m_ResetCurGadget = true;
        return;
      }
      start_gadget = gadget = m_GadgetCur;
      do {
        if (CHECK_FLAG(gadget->GetFlags(), UIF_GROUP_START)) {
          break;
        }
        gadget = (gadget->m_Prev) ? gadget->m_Prev : m_GadgetTail;
        if (!gadget->IsDisabled() && !CHECK_FLAG(gadget->GetFlags(), UIF_SLAVE)) {
          start_gadget->LostFocus();
          gadget->GainFocus();
          m_GadgetCur = gadget;
          break;
        }
      } while (start_gadget != gadget);
    }
    if (key == KEY_DOWN || key == KEY_RIGHT) {
      if (!m_GadgetCur) {
        m_ResetCurGadget = true;
        return;
      }
      start_gadget = gadget = m_GadgetCur;
      do {
        if (CHECK_FLAG(gadget->GetFlags(), UIF_GROUP_END)) {
          break;
        }
        gadget = (gadget->m_Next) ? gadget->m_Next : m_GadgetHead;
        if (!gadget->IsDisabled() && !CHECK_FLAG(gadget->GetFlags(), UIF_SLAVE)) {
          start_gadget->LostFocus();
          gadget->GainFocus();
          m_GadgetCur = gadget;
          break;
        }
      } while (start_gadget != gadget);
    }
  }

  //	determine if any hotkey was pressed
  if (!m_HoldHotkeys) {
    int i;

    for (gadget = m_GadgetHead; gadget != NULL; gadget = gadget->m_Next) {
      if (!gadget->IsDisabled()) {
        if (gadget->GetID() == UID_CANCEL && key == KEY_ESC)
          gadget->OnSelect();
      }
    }

    // do hotkeys
    for (gadget = m_GadgetHead; gadget != NULL; gadget = gadget->m_Next) {
      if (!gadget->IsDisabled() && UI_input.key && ISKEYPRESSED(gadget->m_Hotkey))
        gadget->OnSelect();
    }

    // do window accelerators.
    for (i = 0; i < m_naccels; i++) {
      if (UI_input.key && ISKEYPRESSED(m_Accelerators[i].key) && UI_output.id < 0) {
        UI_output.gadget = NULL;
        UI_output.id = m_Accelerators[i].id;
        break;
      }
    }
  }
}

// behavior when key is released.
void UIWindow::OnKeyUp(int key) {}

//	redraw all gadgets
void UIWindow::Render() {
  UIGadget *gadget = m_GadgetHead;

  //	draw background of window.
  ui_StartDraw(m_X, m_Y, m_X + m_W, m_Y + m_H);
  ui_DrawSetFont(m_FontHandle);

  OnDraw();
  ui_EndDraw();

  while (gadget) {
    int l = m_X + gadget->X();
    int t = m_Y + gadget->Y();
    int r = l + gadget->W();
    int b = t + gadget->H();

    //	reformat gadget
    gadget->OnFormat();

    // draw gadget
    if (l >= 0 && r <= UI_screen_width && t >= 0 && b <= UI_screen_height) {
      ui_StartDraw(l, t, r, b);
      ui_DrawSetFont(m_FontHandle);
      gadget->OnDraw();
      ui_EndDraw();
    } else {
      mprintf((0, "WINDOW: Gadget clipped on screen border (%d,%d,%d,%d)\n", l, t, r, b));
      //			Int3();
    }

    // proeceed...
    gadget = gadget->m_Next;
  }

  UITextItem::SetSaturationFactor(0);

// draw ugly looking focus box around current gadget.
#if 0
	if (m_GadgetCur && !CHECK_FLAG(m_GadgetCur->m_Flags, UIF_SLAVE) ) {
		int l = m_X+m_GadgetCur->X();
		int t = m_Y+m_GadgetCur->Y();
		int r = l+m_GadgetCur->W();
		int b = t+m_GadgetCur->H();

		l--; t--; r++;	b++;
		if (l < 0) l = 0;
		if (t < 0) t = 0;
		if (r >(m_X+m_W)) r = m_X+m_W;
		if (b >(m_Y+m_H)) b = m_Y+m_H;
		ui_StartDraw(l,t,r,b);
		ui_DrawBox(GR_RGB(128,128,128),0,0,(r-l),(b-t));
		ui_EndDraw();
	}
#endif
}

// adds window to ui list.
void UIWindow::Open() { ui_AddWindow(this); }

// removes window from ui list.
void UIWindow::Close() { ui_RemoveWindow(this); }

//	settings
void UIWindow::SetFont(int handle) { m_FontHandle = handle; }

// overridable draws the window background before gadgets
void UIWindow::OnDraw() {
  if (m_BackItem)
    m_BackItem->draw(0, 0, m_W, m_H);
}

// used to manage keypresses so repeating isn't quick.
bool UIWindow::TrapKey(int key) {
  float time;
  time = timer_GetTime();

  //	take care of multiple keypresses and key down repeats. handle timing
  // so it feels 'natural'
  if (m_LastKey && m_LastKey == key) {
    if (m_LastKeyCount == 1) {
      if ((time - m_LastKeyTime) < KEYDOWN_FIRST_DELAY)
        return false;
      m_LastKeyCount++;
    } else if ((time - m_LastKeyTime) < KEYDOWN_REPEAT_DELAY) {
      return false;
    }
    m_LastKeyCount++;
  } else {
    m_LastKeyCount = 1;
  }

  m_LastKey = key;
  m_LastKeyTime = time;

  return true;
}

void UIWindow::UntrapKey(int key) {
  //	if the key previously pressed went up, then
  //	turn off key repeat management.
  if (key == m_LastKey) {
    m_LastKeyCount = 0;
    m_LastKey = 0;
  }
}

void UIWindow::SetFocusOnGadget(UIGadget *gadget, bool key) {
  if (m_GadgetCur) {
    m_GadgetCur->LostFocus();
  }
  m_GadgetCur = gadget;
  if (m_GadgetCur) {
    m_GadgetCur->GainFocus();
    m_ResetCurGadget = false;
  }
}

// used to mark key pressed to id numbers
void UIWindow::AddAcceleratorKey(int key, int id) {
  if (m_naccels == N_WINDOW_ACCELS) {
    // window has asked for too many accelerators.  will not add this one. it's okay to continue, but tell someone
    Int3();
    return;
  }
  m_Accelerators[m_naccels].id = id;
  m_Accelerators[m_naccels].key = key;
  m_naccels++;
}

void UIWindow::ResetAcceleratorKey() { m_naccels = 0; }

//	----------------------------------------------------------------------------
//	UITitledWindow class

UITitledWindow::UITitledWindow() {}

UITitledWindow::~UITitledWindow() {}

void UITitledWindow::Create(UITextItem &title, int x, int y, int w, int h) {
  m_Title = title;
  m_BorderThickness = 2;
  m_CaptionColor = GR_DARKGRAY;

  UIWindow::Create(x, y, w, h);
  UIWindow::SetBackItem(&UIPrimativeItem(GR_LIGHTGRAY));
}

//	inheritable actions

//	draws the window background before gadgets
void UITitledWindow::OnDraw() {
  UIWindow::OnDraw();
  int m_CaptionHeight; // caption height

  //	set font for title.
  if (m_Title.get_font() > -1)
    ui_DrawSetFont(m_Title.get_font());

  m_CaptionHeight = ui_GetFontHeight() + ui_GetFontHeight() / 2;

  //	draw border.
  for (int i = 0; i < m_BorderThickness; i++)
    ui_DrawBox(GR_RGB(0, 0, 0), m_X + i, m_Y + i, m_X + m_W - i, m_Y + m_H - i);

  //	draw caption.
  int x, y;
  ui_DrawRect(m_CaptionColor, m_BorderThickness, m_BorderThickness, m_W - m_BorderThickness, m_CaptionHeight);
  x = m_W / 2 - ui_GetTextWidth(m_Title) / 2;
  y = m_BorderThickness + m_CaptionHeight / 2 - ui_GetFontHeight() / 2;

  ui_SetCharAlpha(m_Title.get_alpha());
  ui_DrawString(m_Title.get_color(), x, y, m_Title);
}