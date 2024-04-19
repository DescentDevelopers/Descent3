/*
 * $Logfile: /descent3/main/editor/PictListBox.h $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:57:38 $
 * $Author: kevinb $
 *
 * Editor Generic Picture List Box.  You must declare your own custom
 * box.
 *
 * $Log: not supported by cvs2svn $
 * 
 * 4     4/02/97 12:21p Samir
 * Moved some protected members to public to make list boxes more
 * managable by parents.
 * 
 * 3     3/18/97 12:12p Samir
 * Parent window accessable to child classes.
 * 
 * 2     2/25/97 1:24p Samir
 * Made SelectItem a public.
 *
 * $NoKeywords: $
 */

//	D3editor Pictoral ListBox Control

#ifndef PICTLISTBOX_H
#define PICTLISTBOX_H

#include "gr.h"

class editorPictListBox
{
protected:
	CWnd *m_ParentWnd;

private:
	int m_StaticID;
	int m_ScrollbarID;
	RECT m_ListRect;					// listbox rect
	int m_ItemStart;					// Initial item (not index into listbox, but into object list)
	int m_ListPos;						// Current position of textture list.
	int m_PageLen;						// length of one view of textures in listbox
	int m_ItemsPerRow;					// textures per row
	int m_ItemsMaxRow;					// maximum number of rows.
	int m_ItemsMax;
	bool m_Redraw;
	int m_CheckX, m_CheckY, m_CheckForSelect;
	int m_OldItemCount;					// texture count saved from last update.
	int m_ItemSelected;					// currently selected texture.
	int m_SurfDim;

private:
	void ListDown(int rows);
	void ListUp(int rows);

public:
	editorPictListBox();				// constructor

// attach to a window.  it will take over that window
	void Create(CWnd *parent, int static_id, int scroll_id, int item_dim, int init_item);	
	void Invalidate();					// forces a redraw. (calls Update);
	void SelectItem(int item);
	int GetSelectedItem() const { return m_ItemSelected; };
	int GetItemStart() const { return m_ItemStart; };
	void SetItemStart(int start) { m_ItemStart = start; };

protected:
//	Handlers
	void SetItemMax(int max) { m_ItemsMax = max; };

public:
	void OnLButtonDown(CPoint point);	// called by parent window.
	void OnVScroll(UINT nSBCode, int nPos, CScrollBar *pScrollBar);

//	Handlers used by editorPictListBox.
protected:
	virtual void StartIterateListItems() = 0;
	virtual int  NextListItem(int curitem) = 0;
	virtual int  PrevListItem(int curitem) = 0;
	virtual void DrawItem(int x, int y, int item) = 0;
	virtual void OnItemSelected(int item) = 0;
	virtual void Update();				// updates the display of the listbox if needed.

protected:
	grHardwareSurface m_ItemSurf, m_ItemSurfSel;

};

#endif
		