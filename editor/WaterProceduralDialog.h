#if !defined(AFX_WATERPROCEDURALDIALOG_H__DAC35720_3108_11D2_9798_004095AA4E37__INCLUDED_)
#define AFX_WATERPROCEDURALDIALOG_H__DAC35720_3108_11D2_9798_004095AA4E37__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// WaterProceduralDialog.h : header file
//

#include "gr.h"
#include "gametexture.h"

/////////////////////////////////////////////////////////////////////////////
// CWaterProceduralDialog dialog

class CWaterProceduralDialog : public CDialog
{
// Construction
public:
	CWaterProceduralDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWaterProceduralDialog)
	enum { IDD = IDD_WATER_PROCEDURAL };
	CSliderCtrl	m_light_slider;
	CSliderCtrl	m_thickness_slider;
	CSliderCtrl	m_size_slider;
	CSliderCtrl	m_height_slider;
	CSliderCtrl	m_frequency_slider;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWaterProceduralDialog)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWaterProceduralDialog)
	afx_msg void OnSelendokProceduralPulldown();
	afx_msg void OnTimer(UINT nIDEvent);
	virtual BOOL OnInitDialog();
	afx_msg void OnClearProcedurals();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKillfocusProcEvalTimeEdit();
	afx_msg void OnKillfocusProcOscTimeEdit();
	afx_msg void OnKillfocusProcOscValueEdit();
	afx_msg void OnCopyProcedural();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void UpdateDialog(void);
	void UpdateProcView(void);
	void HarvestValue(int whichbox,ubyte *val);
	void PrintValue (int,ubyte);
	static_proc_element *CreateElement(int,int);
	
private:
	grHardwareSurface m_TextureSurf;
	int m_window_bitmap;
	int m_button_down;
	int m_proc_type;
	int m_save_x,m_save_y;
	short m_height;
	ubyte m_frequency;
	ubyte m_size;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WATERPROCEDURALDIALOG_H__DAC35720_3108_11D2_9798_004095AA4E37__INCLUDED_)
