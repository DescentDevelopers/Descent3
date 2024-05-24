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

 * $Source: $
 * $Revision: 27 $
 * $Author: Samir $
 * $Date: 4/29/99 2:18a $
 *
 * new NewUI Header
 *
 * $Log: /DescentIII/main/newui_core.h $
 *
 * 27    4/29/99 2:18a Samir
 * updated art for options style menu.
 *
 * 26    4/28/99 1:54a Samir
 * visual tweaks to text
 *
 * 25    4/27/99 2:13p Matt
 * Replaced the two (new) new UI fonts with one new font from Doug.
 *
 * 24    4/27/99 9:24a Matt
 * Added system for drawing title bars on dialogs.
 *
 * 23    4/26/99 7:46p Samir
 * newuiTiledWindow flags passed to create is an int now.
 *
 * 22    4/21/99 10:58a Samir
 * added changable text.
 *
 * 21    4/20/99 11:46a Samir
 * numerous ui 'feel' fixes.
 *
 * 20    4/18/99 7:55p Samir
 * fixed listbox double click and added functions to load and release
 * pertinent newui graphic data.
 *
 * 19    4/15/99 5:28p Samir
 * added advanced messagebox
 *
 * 18    4/05/99 5:13p Samir
 * fixed combo box asthetics.
 *
 */

#ifndef NEWUI_CORE_H
#define NEWUI_CORE_H

#include "ui.h"

#define N_NEWUI_BMPS 32  // number of ui bitmaps.
#define N_NEWUI_SHEETS 8 // number of sheets allowed in a menu.

#define NEWUI_MONITORFONT_COLOR GR_GREEN // GR_RGB(255,255,255)
#define NEWUI_GADGETFONT_COLOR GR_BLACK  // GR_RGB(255,255,255)

#define NEWUI_GADGETFONT_HICOLOR GR_RGB(192, 192, 0)

#ifndef MSGBOX_OK
//	types of message boxes
#define MSGBOX_NULL 0
#define MSGBOX_OK 1
#define MSGBOX_OKCANCEL 2
#define MSGBOX_YESNO 3
#endif

// used internally for ease of management
#define N_DIRECTIONS 12
#define N_PIECE_IDX 0
#define NE_PIECE_IDX 1
#define E_PIECE_IDX 2
#define SE_PIECE_IDX 3
#define S_PIECE_IDX 4
#define SW_PIECE_IDX 5
#define W_PIECE_IDX 6
#define NW_PIECE_IDX 7
#define C_PIECE_IDX 8
#define TL_PIECE_IDX 9
#define TC_PIECE_IDX 10
#define TR_PIECE_IDX 11

// types of 'arrow' buttons used in newuiArrowButton
#define NEWUI_ARROW_LEFT 0x1
#define NEWUI_ARROW_RIGHT 0x2
#define NEWUI_ARROW_UP 0x3
#define NEWUI_ARROW_DOWN 0x4
#define NEWUI_ARROW_CBUP 0x5
#define NEWUI_ARROW_CBDOWN 0x6

// initializes the core system for the newui
void newuiCore_Init();

// initializes the core system for the newui
void newuiCore_Close();

// touches all newui bitmaps, do before any newui menus open
void newuiCore_PageInBitmaps();

// call when NO newui menus open
void newuiCore_ReleaseBitmaps();

//	 C interface to load and free bitmap resources
UIBitmapItem *newui_LoadBitmap(const char *filename);

//	 C interface to load and free bitmap resources
void newui_FreeBitmap(UIBitmapItem *bmitem);

//	sets the callback for background rendering of desktop for UI
#define DEFAULT_UICALLBACK ((void (*)())1)

void SetUICallback(void (*fn)());
void (*GetUICallback())();

// predefined return values for DoUI or UI_frame_result.
#define NEWUIRES_FORCEQUIT -2

//	does a UI loop:  returns a result value for the current window in focus.
int DoUI();

// does one frame of UI input (returns result.)
void DoUIFrame();

//	does one frame of UI without polling input.
void DoUIFrameWithoutInput();

//	gets frame result of ui input.
int GetUIFrameResult();

//////////////////////////////////////////////////////////////////////////////

//	CLASS simple buttons

class newuiSlider;
class newuiCheckBox;
class newuiRadioButton;
class newuiButton;
class newuiMenu;
class newuiListBox;
class newuiEditBox;
class newuiComboBox;
class newuiHotspot;

#define DEFAULT_NEWUID -5

enum tAlignment { NEWUI_ALIGN_VERT, NEWUI_ALIGN_HORIZ };

#define SLIDER_UNITS_INT 0
#define SLIDER_UNITS_PERCENT 1
#define SLIDER_UNITS_FLOAT 2

// used optionally for sliders.
struct tSliderSettings {
  union {
    int i;
    float f;
  } min_val;
  union {
    int i;
    float f;
  } max_val;
  int type; // enumerated above in SLIDER_UNITS
};

#define NEWUI_EDIT_CANCELED_STR "\1" // if a newuiEditbox returns this, then we cancelled. invalid string

#define NEWUI_EDIT_BUFLEN 63

#define NEWUI_BTNF_FRAMED UIF_CUSTOMF      // when Adding a button, this button have 'framed' art.
#define NEWUI_BTNF_LONG (UIF_CUSTOMF << 1) // button will be a long version (with art)

//	CLASS contains gadgets

class newuiSheet {
  int m_sx, m_sy;           // origin of sheet gadgets relative to parent.
  short m_initial_focus_id; // gadget that will have focus upon realization.

public:
  newuiSheet();

  // called usually by other classes, used to initialize the sheet
  void Create(UIWindow *menu, const char *title, int n_items, int sx, int sy);

  // deallocates memory
  void Destroy();

  // get properties
  int X() const { return m_sx; };
  int Y() const { return m_sy; };

  // resets gadget list
  void Reset();

  // call this to initialize gadgets specified above in parent window
  void Realize();

  // call this to release gadgets specified above in parent window, retreives checkbox,radio,etc values too.
  void Unrealize();

  // ALL following functions that return a pointer, that pointer contains the value of the gadget added
  // i.e. checkbox will have a true or false value.  you can use this pointer to modify gadget values.
  // to validate these modifications, call 'UpdateChanges'. (done internally most of the time)

  // call these functions to determine if a pointer's value has changed after call to UpdateReturnValues (internal)
  bool HasChanged(bool *bptr);
  bool HasChanged(short *sptr);
  bool HasChanged(int *iptr);
  bool HasChanged(char *cptr);

  // obtain's pointer to gadget of respective item.  it is your responsibility to typecast the gadget
  // only works if you specified an ID!  Also the sheet must have been realized by now.
  UIGadget *GetGadget(short id);

  //	set focus on this gadget specified by id upon realization.
  void SetInitialFocusedGadget(short id);

  //	creates gadget list.pix_offset is the pixel offset from the group title to the first control.
  void NewGroup(const char *title, short x, short y, tAlignment align = NEWUI_ALIGN_VERT, short pix_offset = -1);

  //	adds standard button to current group. (flags see NEWUI_BTNF_xxxx)
  void AddButton(const char *title, short id, short flags = 0);

  // adds a long button (flags see NEWUI_BTNF_xxxx)
  void AddLongButton(const char *title, short id, short flags = 0);

  //	adds checkbox to current group.  initial state of checkbox can be set.
  bool *AddCheckBox(const char *title, bool init_state = false, short id = DEFAULT_NEWUID);

  //	adds long checkbox to current group.  initial state of checkbox can be set.
  bool *AddLongCheckBox(const char *title, bool init_state = false, short id = DEFAULT_NEWUID);

  // adds a radio button to current group.  initial state of radio may be set
  // pointer returned will return index of radio button in group currently selected.
  int *AddFirstRadioButton(const char *title, short id = DEFAULT_NEWUID);

  // adds a radio button to current group.  initial state of radio may be set
  void AddRadioButton(const char *title, short id = DEFAULT_NEWUID);

  // adds a radio button to current group.  initial state of radio may be set
  int *AddFirstLongRadioButton(const char *title, short id = DEFAULT_NEWUID);

  // adds a radio button to current group.  initial state of radio may be set
  void AddLongRadioButton(const char *title, short id = DEFAULT_NEWUID);

  // adds a slider, set the range for it too., returns two values, short[0] = position, short[1] = range
  short *AddSlider(const char *title, short range, short init_pos = 0, tSliderSettings *settings = NULL,
                   short id = DEFAULT_NEWUID);

  //	adds a static text item
  void AddText(const char *text, ...);

  // adds a static bitmap
  void AddBitmap(int bm_handle);

  //	adds a static text item
  char *AddChangeableText(int buflen);

  // adds a listbox
  newuiListBox *AddListBox(short w, short h, short id, ushort flags = 0);

  // adds a listbox
  newuiComboBox *AddComboBox(short id, ushort flags = 0);

  // adds an edit box
  char *AddEditBox(const char *title, short maxlen = NEWUI_EDIT_BUFLEN, short w = 0, short id = DEFAULT_NEWUID,
                   short flags = 0, bool return_on_esc = false);

  // adds a hotspot :(, should word wrap too.
  void AddHotspot(const char *title, short w, short h, short id, bool group = false);

  // THESE FUNCTIONS ARE CALLED BY FRAMEWORK, BUT IF YOU NEED TO DO SOME CUSTOM UI HANDLING, THESE
  // FUNCTIONS ARE MADE PUBLIC.
public:
  //	refreshes gadget states with values passed to the pointers returned by the below functions
  // depends on no direct manipulation of gadgets outside framework.
  void UpdateChanges();

  //	refreshes return values of gadgets, so they are accessible by the pointers returned by below functions
  // if a gadget has changed since last call to DoUI, the user can tell this by calling HasChanged
  void UpdateReturnValues();

private:
  struct t_gadget_desc {
    int8_t type;   // enumerated ui gadget type
    bool changed; // parameters are different than defaults?
    short id;     // id value
    char *title;  // title of gadget
    union {
      int i;
      short s[2];
      bool b;
      void *p;
    } parm; // parameter list.
    union {
      UIGadget *gadget;
      UIText *text;
      newuiSlider *slider;
      newuiCheckBox *chbox;
      newuiRadioButton *radio;
      newuiButton *button;
      newuiListBox *lb;
      newuiComboBox *cb;
      newuiEditBox *edit;
      newuiHotspot *hot;
    } obj;
    void *internal; // internal info.
  } *m_gadgetlist;

  UIWindow *m_parent;
  int m_ngadgets, m_gadgetlimit;
  bool m_realized;
  char *m_title;

  t_gadget_desc *AddGadget(short id, int8_t type, const char *title);

public:
  const char *GetTitle() { return m_title; };
};

//	CLASS new button class.

class newuiButton : virtual public UIButton {
public:
  newuiButton();

  // see (NEWUI_BTNF_xxx) flags
  void Create(UIWindow *wnd, short id, const char *name, short x, short y, short flags = 0);

protected:
  virtual void OnDraw();    // overridable draws the background first
  virtual void OnDestroy(); // override: frees memory and such.
  virtual void OnFormat();  // new formatting
  virtual void OnLostFocus();
  virtual void OnGainFocus();

  UIBitmapItem *m_bkg, *m_litbkg; // used to define visual characteristics of button
  UISnazzyTextItem *m_text;

  // this is used in multiple inheritance cases.(flags are additional attributes usually passed when creating gadget)
  void InitStates(const char *name, bool is_long, short flags = 0);

public:
  UISnazzyTextItem *GetTitle() { return m_text; };
};

//	CLASS a large option button used in menus.

class newuiMenuOptionButton : public newuiButton {
public:
  void Create(newuiMenu *menu, newuiMenuOptionButton *last, short id, const char *name, short x, short y,
              bool m_mono_press);

protected:
  void OnMouseBtnDown(int btn);
  void OnMouseBtnUp(int btn);
  virtual void OnKeyDown(int key);
  virtual void OnKeyUp(int key);

private:
  bool m_mono_press;
  newuiMenuOptionButton *m_prev_btn, *m_next_btn;
};

class newuiTinyButton : public newuiButton {
public:
  void Create(UIWindow *wnd, short id, const char *name, short x, short y);
};

//	CLASS creates an arrow button that is sensitive to touch (when down, always select)

class newuiArrowButton : public newuiButton {
public:
  newuiArrowButton(){};

  void Create(UIWindow *wnd, short id, short type, const char *name, short x, short y);
  void Show(bool show = true); // this will activate or deactivate a button.

protected:
  virtual void OnMouseBtnDown(int btn);
  virtual void OnMouseBtnUp(int btn);
  virtual void OnDraw();

private:
  float m_timer, m_selecttimer;
  bool m_hidden;
};

//	CLASS a new listbox. uses less memory than the old listbox hopefully.

#define SUBGADGET_UP 0x3
#define SUBGADGET_DOWN 0x4

class newuiListBox : public UIGadget {
public:
  newuiListBox();

  // creates listbox
  void Create(UIWindow *wnd, short id, short x, short y, short w, short h, ushort flags);

  // functions to add or remove items,
  void AddItem(const char *name);
  void RemoveItem(const char *name);
  void RemoveAll();

  // item information
  int GetCurrentIndex();
  void SetCurrentIndex(int index);
  bool GetCurrentItem(char *buffer, int buflen);
  bool GetItem(int index, char *buffer, int buflen);
  bool SetCurrentItem(const char *name);

  // set callbacks
  void SetSelectChangeCallback(void (*fn)(int));
  void SetSelectChangeCallback(void (*fn)(int, void *), void *ptr);

protected:
  virtual void OnLostFocus();               // override: behavior when gadget loses input focus.
  virtual void OnGainFocus();               // override: behavior when gadget gains input focus.
  virtual void OnDraw();                    // behavior when gadget is being drawn.
  virtual void OnKeyDown(int key);          //	behavior when key is pressed.
  virtual void OnKeyUp(int key);            // behavior when key is released.
  virtual void OnMouseBtnDown(int btn);     // behavior when mouse button is pressed.
  virtual void OnMouseBtnUp(int btn);       // behavior when mouse button is released.
  virtual void OnNotifySelect(UIGadget *g); // this function will handle when an arrow button was pressed
  virtual void OnSelect();                  // when a listbox item was double clicked (or entered)
  virtual void OnDestroy();                 // called when destroyed.
  virtual void OnUserProcess();             // override: behavior when gadget is processed
  virtual void OnAttachToWindow();          // when gadget is added to a window (AddGadget is called)
  virtual void OnDetachFromWindow();        // when gadget is detached from window

private:
  char **m_ItemList; // list of strings.
  int *m_Virt2Real;  // translates virtual(user) id to real index
  int *m_Real2Virt;  // translates real index into virtual(user) id

  void (*selectchange_fn)(int);            // callback when selection changes in listbox.
  void (*selectchange_id_fn)(int, void *); // callback when selection changes in listbox..also return the ID
  void *m_callbackptr;                     // user defined callback pointer

  int m_NumItems;        // number of items in list.
  int m_SelectedIndex;   // current selected index into listbox
  int m_Index;           // current index of visible items.
  int m_NumVisibleItems; // number of items visible in listbox
  int m_boffs;           // offset from edge to text region (border width)
  int m_mse_x, m_mse_y;  // mouse x and y of last click.
  int m_last_mse_x, m_last_mse_y;
  int m_last_selected_index;
  float m_click_time; // time in between clicks.
  int8_t m_nclicks;
  bool m_mse_clicked;

  newuiArrowButton m_up_btn; // buttons to scroll.
  newuiArrowButton m_down_btn;

  UIBitmapItem *m_pieces[N_DIRECTIONS]; // bitmap pieces

  void Offset(short offs);
  void SetInternalCurrentIndex(int index);
};

//	CLASS newuiComboBox

class newuiComboBox : public UIGadget {
public:
  newuiComboBox();

  // creates listbox
  void Create(UIWindow *wnd, short id, short x, short y, ushort flags);

  // functions to add or remove items,
  void AddItem(const char *name);
  void RemoveItem(const char *name);
  void RemoveAll();

  // item information
  int GetCurrentIndex();
  void SetCurrentIndex(int index);
  bool GetCurrentItem(char *buffer, int buflen);
  bool GetItem(int index, char *buffer, int buflen);
  bool SetCurrentItem(const char *name);

  // set callbacks
  void SetSelectChangeCallback(void (*fn)(int));

protected:
  virtual void OnDraw();                    // behavior when gadget is being drawn.
  virtual void OnKeyDown(int key);          //	behavior when key is pressed.
  virtual void OnNotifySelect(UIGadget *g); // this function will handle when an arrow button was pressed
  virtual void OnSelect();                  // when a listbox item was double clicked (or entered)
  virtual void OnDestroy();                 // called when destroyed.
  virtual void OnAttachToWindow();          // when gadget is added to a window (AddGadget is called)
  virtual void OnDetachFromWindow();        // when gadget is detached from window

private:
  char **m_ItemList; // list of strings.
  int *m_Virt2Real;  // translates virtual(user) id to real index
  int *m_Real2Virt;  // translates real index into virtual(user) id

  void (*selectchange_fn)(int); // callback when selection changes in listbox.

  int m_NumItems; // number of items in list.
  int m_Index;    // current index of visible items.
  int m_boffs;    // offset from edge to text region (border width)
  bool m_showup, m_showdown;

  newuiArrowButton m_up_btn; // buttons to scroll.
  newuiArrowButton m_down_btn;

  UIBitmapItem *m_barbmp; // bitmap pieces
};

// the new classic hotspot CLASS

class newuiHotspot : public UIGadget {
private:
  char *m_title;
  short m_alpha, m_alpha_adjust;
  float m_mse_timer;

public:
  newuiHotspot();

  void Create(UIWindow *wnd, short id, const char *title, short x, short y, short w, short h, short flags = 0);
  const char *GetTitle() const { return m_title; };

protected:
  virtual void OnDraw(); // this will use the newuiButton drawing scheme
  virtual void OnDestroy();
  virtual void OnMouseBtnDown(int btn);
  virtual void OnMouseBtnUp(int btn);
  virtual void OnGainFocus();
  virtual void OnLostFocus();
};

///////////////////////////////////////////////////////////////////////////////

//	CLASS, contains options buttons and sheets of ui stuff

#define NEWUIMENU_SMALL 0x10
#define NEWUIMENU_MEDIUM 0x20
#define NEWUIMENU_LARGE 0x40

class newuiMenu : public UIWindow {
public:
  newuiMenu();

  //	creates a menu
  void Create();

  //	adds an option to a menu, returns a newuiSheet object to add user interface to.
  newuiSheet *AddOption(short id, const char *title, int size = NEWUIMENU_SMALL, bool has_sheet = true, int yoff = 0);
  void AddSimpleOption(short id, const char *title, int yoff = 0);

  //	sets current option to display.
  void SetCurrentOption(short id);

  // returns current option
  short GetCurrentOption() const;

  // returns current sheet
  newuiSheet *GetCurrentSheet() const;

  // set master sheet,button locations
  void SetPlacements(int x, int y, int options_x, int options_y, int options_w, int options_h, int title_x, int title_y,
                     tAlignment align = NEWUI_ALIGN_VERT);

  // processes a menu
  int DoUI();

  // THESE FUNCTIONS DEAL WITH CALLBACK PROCEDURES
public:
  // when a new option is realized, this function will be called.
  // passed in: the menu object, the old option and the new option and an optional pointer respectively
  void SetOnOptionSwitchCB(void (*fn)(newuiMenu *, short, short, void *), void *data);

  // when a new option is ready.   at this point the system sets the focus on this option
  void SetOnOptionFocusCB(void (*fn)(newuiMenu *, short, void *), void *data);

  // equivalent of SetUICallback, called before gadgets are drawn
  void SetOnUIFrameCB(void (*fn)());

protected:
  virtual void OnDraw();    // overridable draws the window background before gadgets
  virtual void OnDestroy(); // overridable: called in Destroy

  UIBitmapItem *m_bkg; // background

private:
  newuiMenuOptionButton m_sheetbtn[N_NEWUI_SHEETS];
  newuiSheet m_sheets[N_NEWUI_SHEETS];
  bool m_hassheet[N_NEWUI_SHEETS];
  newuiSheet *m_cursheet;            // current sheet
  int m_nsheets, m_sheetx, m_sheety; // sheet characteristics
  int m_optionsw, m_optionsh;
  int m_optionsx, m_optionsy; // option button placements
  int m_titlex, m_titley;     // title location.
  tAlignment m_align;         // alignment of text
  short m_newoptionid;        // tells that a new option has been selected.
  short m_cursheetidx;
  bool m_refreshgadgets; // refreshes gadgets on current sheet.

  void (*m_activate_sheet_cb)(newuiMenu *, short, short, void *);
  void (*m_option_focus_cb)(newuiMenu *, short, void *);
  void (*m_uiframe_cb)();

  void *m_activate_sheet_cb_data;
  void *m_option_focus_cb_data;
};

//	CLASS, large (fullscreen) menu

class newuiLargeMenu : public newuiMenu {
public:
  //	creates a menu
  void Create();

  //	adds an option to a menu, returns a newuiSheet object to add user interface to.
  newuiSheet *AddOption(short id, const char *title, int size = NEWUIMENU_MEDIUM);
};

//	CLASS, small message box
#define NEWUI_MSGBOX_SHEET_W 200
#define NEWUI_MSGBOX_MAXBTNS 4

class newuiMessageBox : public UIWindow {
public:
  newuiMessageBox();

  void Create(const char *title, int type = MSGBOX_OK);

  void AddButton(const char *title, int slot, int key = 0);

  // grab the message box's sheet.
  newuiSheet *GetSheet();

  // do user interface, return UID_OK or UID_CANCEL or UID_??? (UID_OK and UID_CANCEL are predefined)
  int DoUI();

protected:
  virtual void OnDraw(); // overridable draws the window background before gadgets
  virtual void OnDestroy();

private:
  newuiSheet m_sheet;
  newuiButton m_btn[NEWUI_MSGBOX_MAXBTNS];

  UIBitmapItem *m_bkg;
};

// CLASS, sizable window

class newuiTiledWindow : public UIWindow {
public:
  newuiTiledWindow();

  void Create(const char *title, short x, short y, short w, short h, int flags = 0);

  // grab a newui interface from it.
  newuiSheet *GetSheet();

  // do user interface on it.
  int DoUI();

  // set callbacks, SetData keeps a pointer that is passed to all callbacks
  void SetData(void *data);

  // first called when DoUI is called.
  void SetOnStartUICB(void (*fn)(newuiTiledWindow *, void *));

  // called when DoUI returns
  void SetOnEndUICB(void (*fn)(newuiTiledWindow *, void *));

  // called before rend_Flip but after ui has drawn. (SetUICallback is before ui has been rendered)
  void SetOnDrawCB(void (*fn)(newuiTiledWindow *, void *));

  // called after DoUIFrame is called.   for immediate results.
  void SetOnUIFrameCB(void (*fn)(newuiTiledWindow *, void *));

protected:
  virtual void OnDraw();
  virtual void OnDestroy();

private:
  UIBitmapItem *m_pieces[N_DIRECTIONS]; // bitmap pieces
  newuiSheet m_sheet;
  char m_title[64];
  void *m_data;
  bool m_realized;

  void (*m_startui_cb)(newuiTiledWindow *, void *);
  void (*m_endui_cb)(newuiTiledWindow *, void *);
  void (*m_draw_cb)(newuiTiledWindow *, void *);
  void (*m_onframe_cb)(newuiTiledWindow *, void *);

  int GetTopTileIndex(int tx, int window_width);
};

// inlines and macros

#define F_APPROXIMATE(_f) ((_f) + 0.000001f)

inline short CALC_SLIDER_POS_FLOAT(float val, const tSliderSettings *settings, short range) {
  short curpos;
  curpos = (short)F_APPROXIMATE(((val - settings->min_val.f) * range) / (settings->max_val.f - settings->min_val.f));
  return curpos;
}

inline short CALC_SLIDER_POS_INT(int val, const tSliderSettings *settings, short range) {
  short curpos;
  float num = (float)((val - settings->min_val.i) * range);
  float dem = (float)((settings->max_val.i - settings->min_val.i));
  curpos = (short)F_APPROXIMATE(num / dem);
  return curpos;
}

inline float CALC_SLIDER_FLOAT_VALUE(short val, float min, float max, short range) {
  float retval;
  retval = F_APPROXIMATE((max - min) * val / range) + min;
  return retval;
}

inline int CALC_SLIDER_INT_VALUE(short val, int min, int max, short range) {
  int retval;
  float num = (float)((max - min) * val);
  retval = (int)(F_APPROXIMATE(num / range)) + min;
  return retval;
}

#endif
