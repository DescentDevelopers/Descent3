/******************************************************************************\
* file   : ListCtrlEx.cpp
* created: 1997.09.22
*
* Zafir Anjum   - Original author.
* Mark Findlay  - IE4 fixes.
* Matthew Bells - Better TitleTips.
*
* description:
* A super CListControl.
* <P>features:
* <UL>
*   <LI>Title Tip item expantion
*   <LI>full row selection<BR>
*       <I>Note:</I> this is also a feature in IE 4.0 with
*       LVS_EX_FULLROWSELECT.
*   <LI>notifies parent selection has changed
*   <LI>supports column dragging (with IE4) which changes the column order
* </UL>
*
\******************************************************************************/
// ListCtrlEx.cpp : implementation of the CListCtrlEx class

#include "stdafx.h"
#include <assert.h>
#include "ListCtrlEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CListCtrlEx

IMPLEMENT_DYNCREATE(CListCtrlEx, CListCtrl)

BEGIN_MESSAGE_MAP(CListCtrlEx, CListCtrl)
	//{{AFX_MSG_MAP(CListCtrlEx)
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_KEYDOWN()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_MESSAGE(LVM_SETTEXTCOLOR, OnSetTextColor)
	ON_MESSAGE(LVM_SETTEXTBKCOLOR, OnSetTextBkColor)
	ON_MESSAGE(LVM_SETBKCOLOR, OnSetBkColor)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListCtrlEx construction/destruction

CListCtrlEx::CListCtrlEx()
{
	m_bFullRowSel = FALSE;
	m_bClientWidthSel = TRUE;

	m_clrText = ::GetSysColor(COLOR_WINDOWTEXT);
	m_clrTextBk = ::GetSysColor(COLOR_WINDOW);
	m_clrBkgnd = ::GetSysColor(COLOR_WINDOW);
}

CListCtrlEx::~CListCtrlEx()
{
}

// Make sure the control is owner drawn
BOOL CListCtrlEx::PreCreateWindow(CREATESTRUCT& cs)
{
	// default is report view and full row selection
	cs.style &= ~LVS_TYPEMASK;
	cs.style |= LVS_REPORT | LVS_OWNERDRAWFIXED;
	m_bFullRowSel = TRUE;

	return(CListCtrl::PreCreateWindow(cs));
}

BOOL CListCtrlEx::SetFullRowSel(BOOL bFullRowSel)
{
	// no painting during change
	LockWindowUpdate();

	m_bFullRowSel = bFullRowSel;

	BOOL bRet;

	if (m_bFullRowSel)
		bRet = ModifyStyle(0L, LVS_OWNERDRAWFIXED);
	else
		bRet = ModifyStyle(LVS_OWNERDRAWFIXED, 0L);

	// repaint window if we are not changing view type
	if (bRet && (GetStyle() & LVS_TYPEMASK) == LVS_REPORT)
		Invalidate();

	// repaint changes
	UnlockWindowUpdate();

	return(bRet);
}

BOOL CListCtrlEx::GetFullRowSel()
{
	return(m_bFullRowSel);
}

/////////////////////////////////////////////////////////////////////////////
// CListCtrlEx drawing

/*
* DrawItem() is called by the framework whenever an item needs to be drawn
* for owner drawn controls.
* Note:
* <UL>
*   <LI>LVS_SHOWSELALWAYS: non owner drawn controls show an item is
*     highlighted when the control does not have focus with a different
*     highlight color is (usually gray). This is not supported for
*     this control.
* </UL>
*/

void CListCtrlEx::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	int iSavedDC = pDC->SaveDC();             // Save DC state
        
	int iItem = lpDrawItemStruct->itemID;

	// Get item image and state info
	LV_ITEM lvi;
	lvi.mask = LVIF_IMAGE | LVIF_STATE;
	lvi.iItem = iItem;
	lvi.iSubItem = 0;
	lvi.stateMask = 0xFFFF;		// get all state flags
	GetItem(&lvi);

	bool bHighlight = (
		(lvi.state & LVIS_DROPHILITED) ||
		((lvi.state & LVIS_SELECTED) && ((GetFocus() == this) || (GetStyle() & LVS_SHOWSELALWAYS)))
		);

	// Get rectangles for drawing
	CRect rcBounds;
	CRect rcLabel;
	CRect rcIcon;
	GetItemRect(iItem, rcBounds, LVIR_BOUNDS);
	GetItemRect(iItem, rcLabel, LVIR_LABEL);
	GetItemRect(iItem, rcIcon, LVIR_ICON);
	CRect rcItem(rcBounds);

	CString sLabel = GetItemText(iItem, 0);

	// Labels are offset by a certain amount  
	// This offset is related to the width of a space character
	int offset = pDC->GetTextExtent(_T(" "), 1 ).cx*2;

	rcBounds.left = rcLabel.left;
	CRect rcWnd;
	GetClientRect(&rcWnd);
	if(m_bClientWidthSel && rcBounds.right<rcWnd.right)
		rcBounds.right = rcWnd.right;

	// Draw the background
	if(bHighlight)
	{
		pDC->SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
		pDC->SetBkColor(::GetSysColor(COLOR_HIGHLIGHT));
		pDC->FillRect(rcBounds, &CBrush(::GetSysColor(COLOR_HIGHLIGHT)));
	}
	else
	{
		pDC->FillRect(rcBounds, &CBrush(m_clrTextBk));
	}

	// Set clip region
	rcItem.right = rcItem.left + GetColumnWidth(0);
/*
	CRgn rgn;
	rgn.CreateRectRgnIndirect(&rcItem);
	pDC->SelectClipRgn(&rgn);
	rgn.DeleteObject();
*/

	// Draw state icon
	if(lvi.state & LVIS_STATEIMAGEMASK)
	{
		int nImage = ((lvi.state & LVIS_STATEIMAGEMASK)>>12) - 1;
		CImageList* pImageList = GetImageList(LVSIL_STATE);
		if(pImageList)
		{
			pImageList->Draw(pDC, nImage,
				CPoint(rcItem.left, rcItem.top), ILD_TRANSPARENT);
		}
	}

	// Draw normal and overlay icon
	CImageList* pImageList = GetImageList(LVSIL_SMALL);
	if(pImageList)
	{
		UINT nOvlImageMask = lvi.state & LVIS_OVERLAYMASK;
		pImageList->Draw(pDC, lvi.iImage, 
			CPoint(rcIcon.left, rcIcon.top),
			(bHighlight?ILD_BLEND50:0) | ILD_TRANSPARENT | nOvlImageMask );
	}

	// Draw item label - Column 0
	rcLabel.left += offset/2-1;
	rcLabel.right -= offset;
	pDC->DrawText(sLabel,-1,rcLabel,DT_LEFT | DT_SINGLELINE | DT_NOPREFIX
		| DT_VCENTER | DT_END_ELLIPSIS);

	// Draw labels for remaining columns
	LV_COLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH;

/*
// set clip region
 	rcBounds.right = rcHighlight.right > rcBounds.right ? rcHighlight.right :
	rcBounds.right;
	rgn.CreateRectRgnIndirect(&rcBounds);
	pDC->SelectClipRgn(&rgn);
*/
	for(int nColumn = 1; GetColumn(nColumn, &lvc); nColumn++)
	{
		rcItem.left = rcItem.right;
		rcItem.right += lvc.cx;

		sLabel = GetItemText(iItem, nColumn);

		// Get the text justification
		UINT nJustify = DT_LEFT;
		switch(lvc.fmt & LVCFMT_JUSTIFYMASK)
		{
		case LVCFMT_RIGHT:
			nJustify = DT_RIGHT;
			break;
		case LVCFMT_CENTER:
			nJustify = DT_CENTER;
			break;
		default:
			break;
		}

		rcLabel = rcItem;
		rcLabel.left += offset;
		rcLabel.right -= offset;

		pDC->DrawText(sLabel, -1, rcLabel,
			nJustify | DT_SINGLELINE | DT_NOPREFIX | DT_VCENTER | DT_END_ELLIPSIS);
	}

	// draw focus rectangle if item has focus
	if ((lvi.state & LVIS_FOCUSED) && (GetFocus() == this))
		pDC->DrawFocusRect(rcBounds);

	pDC->RestoreDC(iSavedDC);                 // Restore DC.
}

/////////////////////////////////////////////////////////////////////////////
// CListCtrlEx diagnostics

#ifdef _DEBUG

void CListCtrlEx::Dump(CDumpContext& dc) const
{
	CListCtrl::Dump(dc);

	dc << "m_bFullRowSel = " << m_bFullRowSel;
	dc << "\n";
}

#endif //_DEBUG


/**
* @param iRow    [in] row of cell
* @param iColunm [in] column of cell
* @return Rectangle corresponding to the given cell.
*/

CRect CListCtrlEx::GetCellRect(int iRow, int iColumn)const
{
	// Make sure that the ListView is in LVS_REPORT
	if((GetStyle() & LVS_TYPEMASK) != LVS_REPORT)
		return CRect(0,0,0,0);

	// Get the number of columns
	{
		CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
		int iColumnCount = pHeader->GetItemCount();
		assert(iColumn < iColumnCount);
	}

	CRect rect;
	GetItemRect(iRow, &rect, LVIR_BOUNDS);
	// Now find the column
	for(int colnum = 0; colnum < iColumn; colnum++)
	{
		rect.left += GetTrueColumnWidth(colnum);
	}

	// Found the column
	rect.right = rect.left + GetTrueColumnWidth(iColumn);

	RECT rectClient;
	GetClientRect(&rectClient);
	if(rect.right > rectClient.right)
		rect.right = rectClient.right;

	return rect;
}

/**
* @author Mark Findlay
*/

CString CListCtrlEx::GetTrueItemText(int row, int col)const
{
	// Get the header control 
	CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
	_ASSERTE(pHeader);
	
	// get the current number of columns 
	int nCount = pHeader->GetItemCount();
	
	// find the actual column requested. We will compare
	// against hi.iOrder
	for (int x=0; x< nCount; x++)
	{
		HD_ITEM hi = {0};
		hi.mask = HDI_ORDER;
		
		BOOL bRet = pHeader->GetItem(x,&hi);
		_ASSERTE(bRet);
		if (hi.iOrder == col)
		{
			// Found it, get the associated text
			return GetItemText(row,x);
		}
	}
	
	_ASSERTE(FALSE);
	return "We better never fall through to here!";
}

/**
* @author Mark Findlay
*/

int CListCtrlEx::GetTrueColumnWidth(int nCurrentPosition)const
{
	CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
	_ASSERTE(pHeader);
	
	int nCount = pHeader->GetItemCount();
	
	for (int x=0; x< nCount; x++)
	{
		HD_ITEM hi = {0};
		hi.mask = HDI_WIDTH | HDI_ORDER;
		
		BOOL bRet = pHeader->GetItem(x,&hi);
		_ASSERTE(bRet);
		if (hi.iOrder == nCurrentPosition)
			return hi.cxy;
	}
	
	_ASSERTE(FALSE);
	return 0; // We would never fall through to here!
}

void CListCtrlEx::HideTitleTip()
{
	m_titletip.ShowWindow(SW_HIDE);
}

/**
* @param point   [in]  point in client coordinates
* @param iRow    [out] row containing the point
* @param iColunm [out] column containing the point
*
* @author Matthew Bells
*/

bool CListCtrlEx::HitTestRowCol(CPoint& point, int& iRow, int& iColumn)const
{
	// Make sure that the ListView is in LVS_REPORT
	if((GetStyle() & LVS_TYPEMASK) != LVS_REPORT)
		return false;

	int iPosX = point.x;
	iRow = HitTest(point);

	// Get the number of columns
	CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
	int iColumnCount = pHeader->GetItemCount();

	for(iColumn = 0; iColumn < iColumnCount; ++iColumn)
	{
		iPosX -= GetTrueColumnWidth(iColumn);
		if(iPosX < 0)
			break;
	}
	if(iColumn == iColumnCount)
		iColumn = -1;

	return (iRow != -1 && iColumn != -1);
}

void CListCtrlEx::RepaintSelectedItems()
{
	CRect rcItem;
	CRect rcLabel;

	// Invalidate focused item so it can repaint

	int iItem = GetNextItem(-1, LVNI_FOCUSED);

	if(iItem != -1)
	{
		GetItemRect(iItem, rcItem, LVIR_BOUNDS);
		GetItemRect(iItem, rcLabel, LVIR_LABEL);
		rcItem.left = rcLabel.left;

		InvalidateRect(rcItem, FALSE);
	}

	// Invalidate selected items depending on LVS_SHOWSELALWAYS

	if(!(GetStyle() & LVS_SHOWSELALWAYS))
	{
		for(iItem = GetNextItem(-1, LVNI_SELECTED);
			iItem != -1; iItem = GetNextItem(iItem, LVNI_SELECTED))
		{
			GetItemRect(iItem, rcItem, LVIR_BOUNDS);
			GetItemRect(iItem, rcLabel, LVIR_LABEL);
			rcItem.left = rcLabel.left;

			InvalidateRect(rcItem, FALSE);
		}
	}

	UpdateWindow();
}

/////////////////////////////////////////////////////////////////////////////
// CListCtrlEx message handlers

void CListCtrlEx::OnDestroy() 
{
	m_titletip.DestroyWindow();

	CListCtrl::OnDestroy();
}

void CListCtrlEx::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	inherited::OnChar(nChar, nRepCnt, nFlags);
	HideTitleTip();
	SendSelChangedNotification();
}

void CListCtrlEx::OnKillFocus(CWnd* pNewWnd) 
{
	CListCtrl::OnKillFocus(pNewWnd);

	// This should be hidden no matter if another control is getting focus
	// or the edit box.
	HideTitleTip();

	// this really still has focus if one of its chilren (ie. the edit box)
	// has focus
	if(pNewWnd != NULL && pNewWnd->GetParent() == this)
		return;

	// repaint items that should change appearance
	if(m_bFullRowSel && (GetStyle() & LVS_TYPEMASK) == LVS_REPORT)
		RepaintSelectedItems();
}

void CListCtrlEx::OnLButtonDown(UINT nFlags, CPoint point)
{
	int iTest = GetKeyState(VK_LMENU);
	// Shortcut to editing.
	if((GetKeyState(VK_LMENU) & 0x8000) || (GetKeyState(VK_RMENU) & 0x8000))
	{
		int iRow;
		int iColumn;
		if(HitTestRowCol(point, iRow, iColumn))
		{
			SetFocus();
			PostMessage(LVM_EDITLABEL, (WPARAM)iRow, 0);
		}
	}
	else
	{
		inherited::OnLButtonDown(nFlags, point);
		ShowTitleTip(point);                    // Make sure TitleTip changes if needed.
		SendSelChangedNotification();
	}
}

void CListCtrlEx::OnMouseMove(UINT nFlags, CPoint point)
{
	if( nFlags == 0 )
	{
		ShowTitleTip(point);                    // Make sure TitleTip changes if needed.
	}
	
	CListCtrl::OnMouseMove(nFlags, point);
}

/*
* When the regular list view control repaints an item, it repaints only the
* area occupied by defined columns. If the last column does not extend to the
* end of the client area, then the space to the right of the last column is
* not repainted. If we are highlighting the full row then this area also needs
* to be invalidated so that the code in DrawItem() can add or remove the
* highlighting from this area.
*/

void CListCtrlEx::OnPaint() 
{
	// in full row select mode, we need to extend the clipping region
	// so we can paint a selection all the way to the right
	if (m_bClientWidthSel &&
		(GetStyle() & LVS_TYPEMASK) == LVS_REPORT && GetFullRowSel())
	{
		CRect rcAllLabels;
		GetItemRect(0, rcAllLabels, LVIR_BOUNDS);

		CRect rcClient;
		GetClientRect(&rcClient);
		if(rcAllLabels.right < rcClient.right)
		{
			// need to call BeginPaint (in CPaintDC c-tor)
			// to get correct clipping rect
			CPaintDC dc(this);

			CRect rcClip;
			dc.GetClipBox(rcClip);

			rcClip.left = min(rcAllLabels.right-1, rcClip.left);
			rcClip.right = rcClient.right;

			InvalidateRect(rcClip, FALSE);
			// EndPaint will be called in CPaintDC d-tor
		}
	}

	CListCtrl::OnPaint();
}

LRESULT CListCtrlEx::OnSetBkColor(WPARAM wParam, LPARAM lParam)
{
	m_clrBkgnd = (COLORREF)lParam;
	return(Default());
}

/*
* This is another step to mimic the default behaviour of the list view
* control. When the control loses focus, the focus rectangle around the
* selected (focus) item has to be removed. When the control gets back
* focus, then the focus rectangle has to be redrawn. Both these handlers
* call the RepaintSelectedItems() helper function. 
*/

void CListCtrlEx::OnSetFocus(CWnd* pOldWnd) 
{
	CListCtrl::OnSetFocus(pOldWnd);

	// check if we are getting focus from label edit box
//	if(pOldWnd!=NULL && pOldWnd->GetParent()==this)
//		return;

	// repaint items that should change appearance
	if(m_bFullRowSel && (GetStyle() & LVS_TYPEMASK)==LVS_REPORT)
		RepaintSelectedItems();
}

LRESULT CListCtrlEx::OnSetTextBkColor(WPARAM wParam, LPARAM lParam)
{
	m_clrTextBk = (COLORREF)lParam;
	return(Default());
}

LRESULT CListCtrlEx::OnSetTextColor(WPARAM wParam, LPARAM lParam)
{
	m_clrText = (COLORREF)lParam;
	return(Default());
}

void CListCtrlEx::PreSubclassWindow() 
{
	CListCtrl::PreSubclassWindow();

	m_titletip.Create(this);
	m_titletip.SetBackground(CBrush(GetBkColor()));
}

void CListCtrlEx::SendSelChangedNotification()
{
	NMHDR nmh;
	nmh.hwndFrom = *this;
	nmh.idFrom = GetDlgCtrlID();
	nmh.code = LVNU_SELCHANGED;
	GetParent()->SendMessage(WM_NOTIFY, GetDlgCtrlID(), (LPARAM)&nmh);
}

void CListCtrlEx::ShowTitleTip(CPoint point)
{
	int iRow;
	int iCol;

	if(HitTestRowCol(point, iRow, iCol))
	{
		CRect cellrect = GetCellRect(iRow, iCol);
		// offset is equal to TextExtent of 2 space characters.
		// Make sure you have the right font selected into the
		// device context before calling GetTextExtent.
		// You can save this value as a member variable.
		// offset = pDC->GetTextExtent(_T(" "), 1 ).cx*2;
		int offset = 6;
		/*if(iCol == 0)
		{
			CRect rcLabel;
			GetItemRect(iRow, &rcLabel, LVIR_LABEL );
			offset = rcLabel.left - cellrect.left + offset / 2 - 1;
		}*/

		if(iCol == 0)					// TBD: test this with IE4
			cellrect.left -= 2;	// Does it also move the first column???

		cellrect.top--;

		if(GetItemState(iRow, LVIS_SELECTED))
		{
			m_titletip.SetBkColor(::GetSysColor(COLOR_HIGHLIGHT));
			m_titletip.SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
		}
		else
		{
			m_titletip.SetBkColor(m_clrTextBk);
			m_titletip.SetTextColor(m_clrText);
		}

		m_titletip.Show(cellrect, GetTrueItemText(iRow, iCol), offset-1);
	}
}
