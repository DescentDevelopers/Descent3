// DeathDialog.cpp : implementation file
//

#include "stdafx.h"
#include "editor.h"
#include "DeathDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//Stuff for Copy/Paste
static death_info Paste_data;
static bool Paste_data_used = 0;

/////////////////////////////////////////////////////////////////////////////
// CDeathDialog dialog


CDeathDialog::CDeathDialog(death_info *death_info,CWnd* pParent /*=NULL*/)
	: CDialog(CDeathDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDeathDialog)
	m_explosion_size_radio = -1;
	m_delay_max = 0.0f;
	m_delay_min = 0.0f;
	//}}AFX_DATA_INIT

	m_death_info_ptr = death_info;

	m_flags = death_info->flags;
	m_delay_min = death_info->delay_min;
	m_delay_max = death_info->delay_max;
}


void CDeathDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDeathDialog)
	DDX_Radio(pDX, IDC_DEATH_EXPLOSION_SMALL, m_explosion_size_radio);
	DDX_Text(pDX, IDC_DEATH_DELAY_MAX, m_delay_max);
	DDV_MinMaxFloat(pDX, m_delay_max, 0.f, 10.f);
	DDX_Text(pDX, IDC_DEATH_DELAY_MIN, m_delay_min);
	DDV_MinMaxFloat(pDX, m_delay_min, 0.f, 10.f);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDeathDialog, CDialog)
	//{{AFX_MSG_MAP(CDeathDialog)
	ON_BN_CLICKED(IDC_DEATH_BREAKS_APART, OnDeathBreaksApart)
	ON_BN_CLICKED(IDC_DEATH_COMMON_ANIMAL, OnDeathCommonAnimal)
	ON_BN_CLICKED(IDC_DEATH_COMMON_DESCENT, OnDeathCommonDescent)
	ON_BN_CLICKED(IDC_DEATH_COMMON_QUICK_EXPLOSION, OnDeathCommonQuickExplosion)
	ON_BN_CLICKED(IDC_DEATH_COMMON_R2D2, OnDeathCommonR2D2)
	ON_BN_CLICKED(IDC_DEATH_COMMON_SHOOT_UP, OnDeathCommonShootUp)
	ON_BN_CLICKED(IDC_DEATH_CONTACT_BREAKS_APART, OnDeathContactBreaksApart)
	ON_BN_CLICKED(IDC_DEATH_CONTACT_EXPLODES, OnDeathContactExplodes)
	ON_BN_CLICKED(IDC_DEATH_CONTACT_REMAINS, OnDeathContactRemains)
	ON_BN_CLICKED(IDC_DEATH_COPY, OnDeathCopy)
	ON_BN_CLICKED(IDC_DEATH_DEBRIS_CONTACT_EXPLODES, OnDeathDebrisContactExplodes)
	ON_BN_CLICKED(IDC_DEATH_DEBRIS_CONTACT_REMAINS, OnDeathDebrisContactRemains)
	ON_BN_CLICKED(IDC_DEATH_DELAY_FIREBALLS, OnDeathDelayFireballs)
	ON_BN_CLICKED(IDC_DEATH_DELAY_FLYING, OnDeathDelayFlying)
	ON_BN_CLICKED(IDC_DEATH_DELAY_LOSES_ANTIGRAV, OnDeathDelayLosesAntigrav)
	ON_EN_CHANGE(IDC_DEATH_DELAY_MAX, OnChangeDeathDelayMax)
	ON_EN_CHANGE(IDC_DEATH_DELAY_MIN, OnChangeDeathDelayMin)
	ON_BN_CLICKED(IDC_DEATH_DELAY_SPARKS, OnDeathDelaySparks)
	ON_BN_CLICKED(IDC_DEATH_EXPLODES, OnDeathExplodes)
	ON_BN_CLICKED(IDC_DEATH_EXPLOSION_LARGE, OnDeathExplosionLarge)
	ON_BN_CLICKED(IDC_DEATH_EXPLOSION_MEDIUM, OnDeathExplosionMedium)
	ON_BN_CLICKED(IDC_DEATH_EXPLOSION_SMALL, OnDeathExplosionSmall)
	ON_BN_CLICKED(IDC_DEATH_PASTE, OnDeathPaste)
	ON_BN_CLICKED(IDC_DEATH_BLAST_RING, OnDeathBlastRing)
	ON_BN_CLICKED(IDC_DEATH_CONTACT_BLAST_RING, OnDeathContactBlastRing)
	ON_BN_CLICKED(IDC_DEATH_CONTACT_FIREBALL, OnDeathContactFireball)
	ON_BN_CLICKED(IDC_DEATH_DEBRIS_CONTACT_BLAST_RING, OnDeathDebrisContactBlastRing)
	ON_BN_CLICKED(IDC_DEATH_DEBRIS_CONTACT_FIREBALL, OnDeathDebrisContactFireball)
	ON_BN_CLICKED(IDC_DEATH_DEBRIS_SMOKES, OnDeathDebrisSmokes)
	ON_BN_CLICKED(IDC_DEATH_DELAY_FROM_ANIM, OnDeathDelayFromAnim)
	ON_BN_CLICKED(IDC_DEATH_DELAY_SMOKE, OnDeathDelaySmoke)
	ON_BN_CLICKED(IDC_DEATH_FIREBALL, OnDeathFireball)
	ON_BN_CLICKED(IDC_DEATH_LOSES_ANTIGRAV, OnDeathLosesAntigrav)
	ON_BN_CLICKED(IDC_DEATH_REMAINS, OnDeathRemains)
	ON_BN_CLICKED(IDC_DEATH_DELAY_FADE_AWAY, OnDeathDelayFadeAway)
	ON_BN_CLICKED(IDC_DEATH_FADES_AWAY, OnDeathFadesAway)
	ON_BN_CLICKED(IDC_DEATH_DELAY_SOUND, OnDeathDelaySound)
	ON_BN_CLICKED(IDC_DEATH_DELAY_NO_TUMBLE, OnDeathDelayNoTumble)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDeathDialog message handlers


BOOL CDeathDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	UpdateDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//Handy macros
#define SET_CHECK(id,state)	((CButton *) GetDlgItem(id))->SetCheck((state) != 0)
#define GET_CHECK(id)			((CButton *) GetDlgItem(id))->GetCheck()
#define ENABLE(id,state)		((CButton *) GetDlgItem(id))->EnableWindow((state) != 0)

void CDeathDialog::UpdateDialog() 
{
	//Set member variables for explosion size radio button
	m_explosion_size_radio = (m_flags & DF_EXPL_SIZE_MASK) >> DF_EXPL_SIZE_SHIFT;

	//Update the dialog from member variables
	UpdateData(false);

	//Set the delay checkboxes
	SET_CHECK(IDC_DEATH_DELAY_FROM_ANIM,(m_flags & DF_DELAY_FROM_ANIM));
	SET_CHECK(IDC_DEATH_DELAY_LOSES_ANTIGRAV,(m_flags & DF_DELAY_LOSES_ANTIGRAV));
	SET_CHECK(IDC_DEATH_DELAY_SPARKS,(m_flags & DF_DELAY_SPARKS));
	SET_CHECK(IDC_DEATH_DELAY_SMOKE,(m_flags & DF_DELAY_SMOKES));
	SET_CHECK(IDC_DEATH_DELAY_FLYING,(m_flags & DF_DELAY_FLYING));
	SET_CHECK(IDC_DEATH_DELAY_FIREBALLS,(m_flags & DF_DELAY_FIREBALL));
	SET_CHECK(IDC_DEATH_DELAY_FADE_AWAY,(m_flags & DF_DELAY_FADE_AWAY));
	SET_CHECK(IDC_DEATH_DELAY_SOUND,(m_flags & DF_DELAY_SOUND));
	SET_CHECK(IDC_DEATH_DELAY_NO_TUMBLE,(m_flags & DF_DELAY_NO_TUMBLE_FLY));

	//Set the death checkboxes
	SET_CHECK(IDC_DEATH_FIREBALL,(m_flags & DF_FIREBALL));
	SET_CHECK(IDC_DEATH_BREAKS_APART,(m_flags & DF_BREAKS_APART));
	SET_CHECK(IDC_DEATH_FADES_AWAY,(m_flags & DF_FADE_AWAY));
	SET_CHECK(IDC_DEATH_BLAST_RING,(m_flags & DF_BLAST_RING));
	SET_CHECK(IDC_DEATH_REMAINS,(m_flags & DF_REMAINS));
	SET_CHECK(IDC_DEATH_LOSES_ANTIGRAV,(m_flags & DF_LOSES_ANTIGRAV));

	//Set the contact checkboxes
	SET_CHECK(IDC_DEATH_CONTACT_FIREBALL,(m_flags & DF_CONTACT_FIREBALL));
	SET_CHECK(IDC_DEATH_CONTACT_BREAKS_APART,(m_flags & DF_CONTACT_BREAKS_APART));
	SET_CHECK(IDC_DEATH_CONTACT_BLAST_RING,(m_flags & DF_CONTACT_BLAST_RING));
	SET_CHECK(IDC_DEATH_CONTACT_REMAINS,(m_flags & DF_CONTACT_REMAINS));

	//Set the debris checkbox
	SET_CHECK(IDC_DEATH_DEBRIS_SMOKES,(m_flags & DF_DEBRIS_SMOKES));

	//Set the debris contact checkboxes
	SET_CHECK(IDC_DEATH_DEBRIS_CONTACT_FIREBALL,(m_flags & DF_DEBRIS_FIREBALL));
	SET_CHECK(IDC_DEATH_DEBRIS_CONTACT_BLAST_RING,(m_flags & DF_DEBRIS_BLAST_RING));
	SET_CHECK(IDC_DEATH_DEBRIS_CONTACT_REMAINS,(m_flags & DF_DEBRIS_REMAINS));

	//Enable/disable delay options
	bool delay = ((m_flags & DF_DELAY_FROM_ANIM) || (m_delay_min != 0.0) || (m_delay_max != 0.0));
	ENABLE(IDC_DEATH_DELAY_GROUP,delay);
	ENABLE(IDC_DEATH_DELAY_LOSES_ANTIGRAV,delay);
	ENABLE(IDC_DEATH_DELAY_SPARKS,delay);
	ENABLE(IDC_DEATH_DELAY_SMOKE,delay);
	ENABLE(IDC_DEATH_DELAY_FLYING,delay);
	ENABLE(IDC_DEATH_DELAY_FIREBALLS,delay);
	ENABLE(IDC_DEATH_DELAY_FADE_AWAY,delay);
	ENABLE(IDC_DEATH_DELAY_SOUND,delay);
	ENABLE(IDC_DEATH_DELAY_NO_TUMBLE,delay && (m_flags & DF_DELAY_FLYING));

	//Enable/disable delay times
	bool user_delay = !(m_flags & DF_DELAY_FROM_ANIM);
	ENABLE(IDC_DEATH_DELAY_MIN,user_delay);
	ENABLE(IDC_DEATH_DELAY_MAX,user_delay);
	ENABLE(IDC_DEATH_DELAY_MIN_LABEL,user_delay);
	ENABLE(IDC_DEATH_DELAY_MAX_LABEL,user_delay);

	//Enable/disable contact options
	ENABLE(IDC_DEATH_CONTACT_FIREBALL,delay);
	ENABLE(IDC_DEATH_CONTACT_BREAKS_APART,delay);
	ENABLE(IDC_DEATH_CONTACT_BLAST_RING,delay);
	ENABLE(IDC_DEATH_CONTACT_REMAINS,delay);
	ENABLE(IDC_DEATH_CONTACT_GROUP,delay);

	//Enable/disable explosion options
	bool explodes = ((m_flags & DF_FIREBALL) != 0);
	ENABLE(IDC_DEATH_EXPLOSION_SMALL,explodes);
	ENABLE(IDC_DEATH_EXPLOSION_MEDIUM,explodes);
	ENABLE(IDC_DEATH_EXPLOSION_LARGE,explodes);
	ENABLE(IDC_DEATH_EXPLOSION_OPTIONS_GROUP,explodes);

	//Enable/disable debris options
	bool breaks_apart = ((m_flags & DF_BREAKS_APART) != 0);
	ENABLE(IDC_DEATH_DEBRIS_CONTACT_FIREBALL,breaks_apart);
	ENABLE(IDC_DEATH_DEBRIS_CONTACT_BLAST_RING,breaks_apart);
	ENABLE(IDC_DEATH_DEBRIS_CONTACT_REMAINS,breaks_apart);
	ENABLE(IDC_DEATH_DEBRIS_CONTACT_GROUP,breaks_apart);
	ENABLE(IDC_DEATH_DEBRIS_SMOKES,breaks_apart);
	ENABLE(IDC_DEATH_DEBRIS_OPTIONS_GROUP,breaks_apart);

	//Enable/disable Paste
	ENABLE(IDC_DEATH_PASTE,Paste_data_used);
}

void CDeathDialog::OnDeathDelayFromAnim() 
{
	m_flags ^= DF_DELAY_FROM_ANIM;

	UpdateDialog();
}

void CDeathDialog::OnChangeDeathDelayMin() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	//Update the member variables from the dialog
	UpdateData(true);

	UpdateDialog();
}

void CDeathDialog::OnChangeDeathDelayMax() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	//Update the member variables from the dialog
	UpdateData(true);

	UpdateDialog();
}

void CDeathDialog::OnDeathDelaySparks() 
{
	m_flags ^= DF_DELAY_SPARKS;

	UpdateDialog();
}

void CDeathDialog::OnDeathDelayFireballs() 
{
	m_flags ^= DF_DELAY_FIREBALL;

	UpdateDialog();
}

void CDeathDialog::OnDeathDelaySmoke() 
{
	m_flags ^= DF_DELAY_SMOKES;

	UpdateDialog();
}

void CDeathDialog::OnDeathDelayFlying() 
{
	m_flags ^= DF_DELAY_FLYING;

	UpdateDialog();
}

void CDeathDialog::OnDeathDelayLosesAntigrav() 
{
	m_flags ^= DF_DELAY_LOSES_ANTIGRAV;

	UpdateDialog();
}

void CDeathDialog::OnDeathDelayFadeAway() 
{
	m_flags ^= DF_DELAY_FADE_AWAY;

	UpdateDialog();
}

void CDeathDialog::OnDeathExplodes() 
{
	m_flags ^= DF_FIREBALL;

	UpdateDialog();
}

void CDeathDialog::OnDeathBreaksApart() 
{
	m_flags ^= DF_BREAKS_APART;

	UpdateDialog();
}

void CDeathDialog::OnDeathExplosionSmall() 
{
	m_flags = (m_flags & ~DF_EXPL_SIZE_MASK) | DF_EXPL_SMALL;

	UpdateDialog();
}

void CDeathDialog::OnDeathExplosionMedium() 
{
	m_flags = (m_flags & ~DF_EXPL_SIZE_MASK) | DF_EXPL_MEDIUM;

	UpdateDialog();
}

void CDeathDialog::OnDeathExplosionLarge() 
{
	m_flags = (m_flags & ~DF_EXPL_SIZE_MASK) | DF_EXPL_LARGE;

	UpdateDialog();
}

void CDeathDialog::OnDeathBlastRing() 
{
	m_flags ^= DF_BLAST_RING;

	UpdateDialog();
}

void CDeathDialog::OnDeathContactBreaksApart() 
{
	m_flags ^= DF_CONTACT_BREAKS_APART;

	UpdateDialog();
}

void CDeathDialog::OnDeathContactExplodes() 
{
	m_flags ^= DF_CONTACT_FIREBALL;

	UpdateDialog();
}

void CDeathDialog::OnDeathContactRemains() 
{
	m_flags ^= DF_CONTACT_REMAINS;

	UpdateDialog();
}

void CDeathDialog::OnDeathDebrisContactExplodes() 
{
	m_flags ^= DF_DEBRIS_FIREBALL;

	UpdateDialog();
}

void CDeathDialog::OnDeathDebrisContactRemains() 
{
	m_flags ^= DF_DEBRIS_REMAINS;

	UpdateDialog();
}

void CDeathDialog::OnDeathCommonDescent() 
{
	m_flags = DF_DELAY_FIREBALL +
						DF_FIREBALL +
						DF_BREAKS_APART +
						DF_DEBRIS_FIREBALL +
						DF_EXPL_MEDIUM;
	m_delay_min = m_delay_max = 2.0;

	UpdateDialog();
}

void CDeathDialog::OnDeathCommonQuickExplosion() 
{
	m_flags = DF_FIREBALL +
						DF_BREAKS_APART +
						DF_DEBRIS_FIREBALL +
						DF_EXPL_MEDIUM;
	m_delay_min = m_delay_max = 0.0;
	
	UpdateDialog();
}

void CDeathDialog::OnDeathCommonShootUp() 
{
	m_flags = DF_DELAY_FLYING +
						DF_FIREBALL +
						DF_BREAKS_APART +
						DF_DEBRIS_FIREBALL +
						DF_EXPL_MEDIUM;
	m_delay_min = 1.0;
	m_delay_max = 3.0;
	
	UpdateDialog();
}

void CDeathDialog::OnDeathCommonR2D2() 
{
	m_flags = DF_DELAY_SPARKS +
						DF_FIREBALL +
						DF_BREAKS_APART +
						DF_DEBRIS_FIREBALL +
						DF_EXPL_MEDIUM;
	m_delay_min = 2.0;
	m_delay_max = 4.0;
	
	UpdateDialog();
}

void CDeathDialog::OnDeathCommonAnimal() 
{
	m_flags = DF_CONTACT_REMAINS;
	m_delay_min = m_delay_max = 0.0;
	
	UpdateDialog();
}

void CDeathDialog::OnDeathCopy() 
{
	Paste_data.flags = m_flags;
	Paste_data.delay_min = m_delay_min;
	Paste_data.delay_max = m_delay_max;

	Paste_data_used = 1;

	UpdateDialog();
}

void CDeathDialog::OnDeathPaste() 
{
	m_flags = Paste_data.flags;
	m_delay_min = Paste_data.delay_min;
	m_delay_max = Paste_data.delay_max;

	UpdateDialog();
}

#include "pserror.h"

void CDeathDialog::OnOK() 
{
	if (m_delay_max < m_delay_min) {
		EditorMessageBox("Max delay must be greater than or equal to min delay.");
		return;
	}

	//Copy data into input struct
	m_death_info_ptr->flags = m_flags;
	m_death_info_ptr->delay_min = m_delay_min;
	m_death_info_ptr->delay_max = m_delay_max;
	
	CDialog::OnOK();
}

void CDeathDialog::OnDeathContactBlastRing() 
{
	m_flags ^= DF_CONTACT_BLAST_RING;

	UpdateDialog();
}

void CDeathDialog::OnDeathContactFireball() 
{
	m_flags ^= DF_CONTACT_FIREBALL;

	UpdateDialog();
}

void CDeathDialog::OnDeathDebrisContactBlastRing() 
{
	m_flags ^= DF_DEBRIS_BLAST_RING;

	UpdateDialog();
}

void CDeathDialog::OnDeathDebrisContactFireball() 
{
	m_flags ^= DF_DEBRIS_FIREBALL;

	UpdateDialog();
}

void CDeathDialog::OnDeathDebrisSmokes() 
{
	m_flags ^= DF_DEBRIS_SMOKES;

	UpdateDialog();
}

void CDeathDialog::OnDeathFireball() 
{
	m_flags ^= DF_FIREBALL;

	UpdateDialog();
}

void CDeathDialog::OnDeathLosesAntigrav() 
{
	m_flags ^= DF_LOSES_ANTIGRAV;

	UpdateDialog();
}

void CDeathDialog::OnDeathRemains() 
{
	m_flags ^= DF_REMAINS;

	UpdateDialog();
}

void CDeathDialog::OnDeathFadesAway() 
{
	m_flags ^= DF_FADE_AWAY;

	UpdateDialog();
}

void CDeathDialog::OnDeathDelaySound() 
{
	m_flags ^= DF_DELAY_SOUND;

	UpdateDialog();
}

void CDeathDialog::OnDeathDelayNoTumble() 
{
	m_flags ^= DF_DELAY_NO_TUMBLE_FLY;

	UpdateDialog();
}
