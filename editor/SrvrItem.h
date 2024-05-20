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
*/

// SrvrItem.h : interface of the CEditorSrvrItem class
//

#if !defined(AFX_SRVRITEM_H__949F57DC_189E_11D1_BD9B_00A0C96ED595__INCLUDED_)
#define AFX_SRVRITEM_H__949F57DC_189E_11D1_BD9B_00A0C96ED595__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CEditorSrvrItem : public COleServerItem
{
	DECLARE_DYNAMIC(CEditorSrvrItem)

// Constructors
public:
	CEditorSrvrItem(CEditorDoc* pContainerDoc);

// Attributes
	CEditorDoc* GetDocument() const
		{ return (CEditorDoc*)COleServerItem::GetDocument(); }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditorSrvrItem)
	public:
	virtual BOOL OnDraw(CDC* pDC, CSize& rSize);
	virtual BOOL OnGetExtent(DVASPECT dwDrawAspect, CSize& rSize);
	//}}AFX_VIRTUAL

// Implementation
public:
	~CEditorSrvrItem();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SRVRITEM_H__949F57DC_189E_11D1_BD9B_00A0C96ED595__INCLUDED_)
