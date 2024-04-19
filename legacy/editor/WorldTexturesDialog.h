// WorldTexturesDialog.h : header file
//

#include "SoundListCombo.h"
#include "gr.h"

/////////////////////////////////////////////////////////////////////////////
// CWorldTexturesDialog dialog

class CWorldTexturesDialog : public CDialog
{
// Construction
public:
	CWorldTexturesDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWorldTexturesDialog)
	enum { IDD = IDD_WORLDTEXTURES };
	CSoundListCombo	m_ambient_sound_combo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWorldTexturesDialog)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:

	// Generated message map functions
	//{{AFX_MSG(CWorldTexturesDialog)
	afx_msg void OnWtexdlgAddnew();
	afx_msg void OnPaint();
	afx_msg void OnNext();
	afx_msg void OnPrevious();
	afx_msg void OnSelendokTexList();
	afx_msg void OnLock();
	afx_msg void OnCheckin();
	afx_msg void OnRcsStatus();
	afx_msg void OnDelete();
	afx_msg void OnVolatile();
	afx_msg void OnWater();
	afx_msg void OnForcefield();
	afx_msg void OnCheckAnimate();
	afx_msg void OnLoadBitmap();
	afx_msg void OnCheckDestroy();
	afx_msg void OnKillfocusReflect();
	afx_msg void OnKillfocusSlideu();
	afx_msg void OnKillfocusSlidev();
	afx_msg void OnKillfocusDamage();
	afx_msg void OnKillfocusTexNum();
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTextureCurrent();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnMineTexture();
	afx_msg void OnObjectTexture();
	afx_msg void OnTerrainTexture();
	afx_msg void OnKillfocusBlueLighting();
	afx_msg void OnKillfocusGreenLighting();
	afx_msg void OnKillfocusRedLighting();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnAddNewSmall();
	afx_msg void OnAddNewTiny();
	afx_msg void OnKillfocusAlphaEdit();
	afx_msg void OnTextureFlyThruCheck();
	afx_msg void OnPingPong();
	afx_msg void OnTextureChangeName();
	afx_msg void OnSelendokDestroyPulldown();
	afx_msg void OnSaturate();
	afx_msg void OnOverride();
	afx_msg void OnSaveAsTga();
	afx_msg void OnConvertToNormal();
	afx_msg void OnConvertToSmall();
	afx_msg void OnConvertToTiny();
	afx_msg void OnKillfocusSpeedEdit();
	afx_msg void OnImportBump();
	afx_msg void OnGenerateOmni();
	afx_msg void OnSrcBump();
	afx_msg void OnBump1();
	afx_msg void OnBump2();
	afx_msg void OnBump3();
	afx_msg void OnBump4();
	afx_msg void OnMetalCheck();
	afx_msg void OnMarbleCheck();
	afx_msg void OnPlasticCheck();
	afx_msg void OnEffectTexture();
	afx_msg void OnHudCockpitTexture();
	afx_msg void OnLightTexture();
	afx_msg void OnProceduralCheck();
	afx_msg void OnProceduralButton();
	afx_msg void OnWaterProceduralCheck();
	afx_msg void OnForceLightmaps();
	afx_msg void OnCoronaA();
	afx_msg void OnCoronaB();
	afx_msg void OnCoronaC();
	afx_msg void OnCoronaD();
	afx_msg void OnMultichecktex();
	afx_msg void OnImport4444();
	afx_msg void OnSaturateLightmap();
	afx_msg void OnAddNewHuge();
	afx_msg void OnLavaCheckbox();
	afx_msg void OnRubbleCheckbox();
	afx_msg void OnSelendokTextureAmbientSoundPulldown();
	afx_msg void OnKillfocusTextureAmbientSoundVolume();
	afx_msg void OnSmoothSpecCheck();
	afx_msg void OnAmbientSoundSelect();
	afx_msg void OnCheckBreakable();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void UpdateDialog(void);
	void UpdateTextureViews (int);
	void SaveTexturesOnClose (void);
	void TextureListDown(int rows);
	void TextureListUp(int rows);
	int LoadITLFile (char *filename,int type);
	void DrawSwatch(int handle,float r,float g,float b);
	void ChangeSize (int w,int h);

private:
	grHardwareSurface m_TextureSurf;
	int m_TexStart;						// Start of texture list.
	int m_TexListPos;					// Current position of textture list.
	int m_TexPageLen;					// length of one view of textures in listbox
	int m_bumpmap;
	bool m_RedrawListbox;
	int m_CheckX, m_CheckY, m_CheckForTextureList;
	
};
