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
