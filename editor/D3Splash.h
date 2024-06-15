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

// D3Splash.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CD3Splash dialog

class CD3Splash : public CDialog {
  // Construction
public:
  CD3Splash(CWnd *pParent = NULL); // standard constructor

  BOOL Create();

  // Dialog Data
  //{{AFX_DATA(CD3Splash)
  enum { IDD = IDD_D3SPLASH };
  // NOTE: the ClassWizard will add data members here
  //}}AFX_DATA

  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CD3Splash)
protected:
  virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
  virtual void PostNcDestroy();
  //}}AFX_VIRTUAL

  //	Operations
public:
  void PutText(char *buf);

private:
  char m_TextLines[3][40];
  int m_CurLine;

  // Implementation
protected:
  virtual void OnCancel();
  virtual void OnOK();

  // Generated message map functions
  //{{AFX_MSG(CD3Splash)
  afx_msg void OnPaint();
  afx_msg void OnDestroy();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()

private:
  CWnd *m_ParentWnd;
  CBitmap m_SplashBmp;
};

extern CD3Splash *D3_splash_screen;
