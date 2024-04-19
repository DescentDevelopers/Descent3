// D3Splash.cpp : implementation file
//

#include "stdafx.h"
#include "editor.h"
#include "D3Splash.h"

#include <stdarg.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


const int SPLASHTEXT_LEFT = 10;
const int SPLASHTEXT_TOP = 180;
const int SPLASHTEXT_RIGHT = 310;
const int SPLASHTEXT_BOTTOM = 230;

CD3Splash *D3_splash_screen;

/////////////////////////////////////////////////////////////////////////////
// CD3Splash dialog


CD3Splash::CD3Splash(CWnd* pParent /*=NULL*/)
	: CDialog(CD3Splash::IDD, pParent)
{
	//{{AFX_DATA_INIT(CD3Splash)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_ParentWnd = pParent;
}


void CD3Splash::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CD3Splash)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BOOL CD3Splash::Create()
{
	m_TextLines[0][0] = 0;
	m_TextLines[1][0] = 0;
	m_TextLines[2][0] = 0;
	m_CurLine = 0;

	D3_splash_screen = this;

	m_SplashBmp.LoadBitmap(IDB_D3SPLASH);

	return CDialog::Create(CD3Splash::IDD, m_ParentWnd);
}


void CD3Splash::PutText(char *buf)
{
	RECT rect;

	if (m_CurLine == 3) {
		for (int i = 0; i < 2; i++)
			lstrcpy(&m_TextLines[i][0], &m_TextLines[i+1][0]);
	}
	else m_CurLine++;

	lstrcpy(&m_TextLines[m_CurLine-1][0], buf);

//	mprintf((1, "%d:%s",m_CurLine-1, m_TextLines[m_CurLine-1]));
	
	SetRect(&rect, SPLASHTEXT_LEFT, SPLASHTEXT_TOP, SPLASHTEXT_RIGHT, SPLASHTEXT_BOTTOM);
	InvalidateRect(&rect, FALSE);
	UpdateWindow();
}



BEGIN_MESSAGE_MAP(CD3Splash, CDialog)
	//{{AFX_MSG_MAP(CD3Splash)
	ON_WM_PAINT()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CD3Splash message handlers

void CD3Splash::OnCancel()
{
//	Absolutely nothing
}


void CD3Splash::OnOK()
{
//	Absolutely nothing
}


void CD3Splash::PostNcDestroy() 
{
//	needed for modeless dialogs
	delete this;
}

void CD3Splash::OnPaint() 
{
	CPaintDC dc(this);	// device context for painting
	CDC sdc;			// source dc
	CBitmap *bmp;
	CFont *fnt;
	CSize textdim;
	BITMAP bm;
	RECT uprect;
	int x, y;

	GetClientRect(&uprect);

	m_SplashBmp.GetObject(sizeof(bm), &bm);
	
	sdc.CreateCompatibleDC(NULL);
	bmp = sdc.SelectObject(&m_SplashBmp);
	dc.StretchBlt(uprect.left+1,uprect.top+1,uprect.right-uprect.left-2,uprect.bottom-uprect.top-2, &sdc, 0,0,bm.bmWidth,bm.bmHeight,SRCCOPY); 
	sdc.SelectObject(bmp);

	fnt = dc.SelectObject(GetFont());
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(RGB(255,255,255));
			
	textdim = dc.GetTextExtent(&m_TextLines[0][0], lstrlen(m_TextLines[0]));

	y = uprect.bottom-(4*(textdim.cy+2));

	for (int r = 0; r < 3; r++)
	{
		if (lstrlen(m_TextLines[r])) {
			textdim = dc.GetTextExtent(&m_TextLines[r][0], lstrlen(m_TextLines[r]));
			x = ((uprect.right-uprect.left) - textdim.cx)/2;
			dc.TextOut(x,y,&m_TextLines[r][0], lstrlen(m_TextLines[r]));
			y += textdim.cy +2;
		}
	}
	dc.SelectObject(fnt);
}


void CD3Splash::OnDestroy() 
{
	CDialog::OnDestroy();

	D3_splash_screen = NULL;
}
