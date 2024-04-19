// editorDoc.h : interface of the CEditorDoc class
//
/////////////////////////////////////////////////////////////////////////////

class CEditorSrvrItem;

class CEditorDoc : public COleServerDoc
{
protected: // create from serialization only
	CEditorDoc();
	DECLARE_DYNCREATE(CEditorDoc)

// Attributes
public:
//	ADDED BY SAMIR FOR OLE SUPPORT
	CEditorSrvrItem* GetEmbeddedItem()
		{ return (CEditorSrvrItem*)COleServerDoc::GetEmbeddedItem(); }
//	ADDED BY SAMIR FOR OLE SUPPORT

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditorDoc)
	protected:
	virtual COleServerItem* OnGetEmbeddedItem();
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual void OnCloseDocument();
	protected:
	virtual BOOL SaveModified();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CEditorDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CEditorDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
