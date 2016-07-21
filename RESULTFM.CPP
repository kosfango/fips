// resultfm.cpp : implementation file
// IDD_RESULT_FORMAT

#include "stdafx.h"
#include "resultfm.h"
#include "exsearch.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CStrList resultdisp;

static int	hinweiscounter=0;
static char DlgName[]="IDD_RESULT_FORMAT";

// A = Area
// S = Sender  	  Name
// E = Empfaenger Name
// T = TabStopp
// - = Space
// I = Subject
// D = Empfangsdatum
// K = Erstelldatum
// X = Sender Fido
// Y = Empfaenger Fido

// ================================================
	resultfm::resultfm(CWnd* pParent ) : CDialog(resultfm::IDD, pParent)
// ================================================
{
	//{{AFX_DATA_INIT(resultfm)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

// ================================================
	void resultfm::DoDataExchange(CDataExchange* pDX)
// ================================================
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(resultfm)
	DDX_Control(pDX, IDC_STATIC1, m_warning);
	DDX_Control(pDX, IDC_EDIT_TABS, m_edit_tabs);
	DDX_Control(pDX, IDC_EDIT_FORMAT, m_edit_format);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(resultfm, CDialog)
	//{{AFX_MSG_MAP(resultfm)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_BN_CLICKED(IDDEFAULT, OnDefault)
	ON_BN_CLICKED(IDAPPLY, OnApply)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// resultfm message handlers

// ================================================
	void resultfm::OnOK()
// ================================================
{
char buf[1000];
char tabs[1000];
int	 i;

	if (!TestConsistency())
		return;

	m_edit_format.GetWindowText(buf,999);
	for (i=0;i<(int)strlen(buf);i++)
		if (!strchr(" ASETIDKXY",buf[i]))
				ERR_MSG2_RET("E_ILLRCCIS",buf[i]);

	m_edit_tabs.GetWindowText(tabs,999);
	for (i=0;i<(int)strlen(tabs);i++)
		if (tabs[i]!=',' &&	!isdigit(tabs[i]))
			ERR_MSG2_RET("E_ILLRSCCIS",tabs[i]);

	resultdisp.RemoveAll();
	resultdisp.AddTail(buf);
	resultdisp.AddTail(tabs);
	resultdisp.SaveToFile("resudisp.cfg");

	KillTimer(2345);
	CDialog::OnOK();
}


// ================================================
	void resultfm::OnDefault()
// ================================================
{
	m_edit_format.SetWindowText("ATSTI");
	m_edit_tabs.SetWindowText("72,154");
	resultdisp.RemoveAll();
	resultdisp.AddTail("ATSTI");
	resultdisp.AddTail("72,154");
	resultdisp.SaveToFile("resudisp.cfg");
}

// ================================================
	void resultfm::OnCancel()
// ================================================
{
	resultdisp.LoadFromFile("resudisp.cfg");
	KillTimer(2345);
	CDialog::OnCancel();
}

// ================================================
	BOOL resultfm::OnInitDialog()
// ================================================
{
int  lng[]={
			IDOK,
			IDCANCEL,
			IDC_STATIC1,
			IDHELP,
			IDDEFAULT,
			IDAPPLY
			};

	CDialog::OnInitDialog();
    set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));
	hinweiscounter=0;
	m_edit_format.SetWindowText(resultdisp.GetString(0));
	m_edit_tabs.SetWindowText(resultdisp.GetString(1));
	SetTimer(2345,400,0);
	return TRUE;
}

// ================================================
	void resultfm::OnApply()
// ================================================
{
char tabs[1000];

	m_edit_tabs.GetWindowText(tabs,999);
	for (int i=0;i<(int)strlen(tabs);i++)
		if (tabs[i]!=',' && !isdigit(tabs[i]))
			ERR_MSG2_RET("E_ILLRSCCIS",tabs[i]);
}


// ================================================
	int resultfm::TestConsistency()
// ================================================
{
CString all;
char edit[1000];
char tabs[1000];
int  tabx[20];
char *p;
int	 anzahl=0;
int	 last,ret;

	m_edit_format.GetWindowText(edit,999);
	m_edit_tabs.GetWindowText(tabs,999);
	// Ist der letzte Char ein T  ?

	last=strlen(edit)-1;
	if (last<0)
		last=0;

	if (edit[last]=='T')
		ERR_MSG_RET0("E_LASCHAMNBT");

	// dann zaehlen wir die TAB Sepicfier ..
	anzahl=0;
	p=edit;
	while (*p)
	{
		if (*p=='T')
			anzahl++;
		p++;
	}

	// Nun schauen wieviele TABS angegeben sind
	ret=sscanf(tabs,"%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
		&tabx[0],&tabx[1],&tabx[2],&tabx[3],&tabx[4],
		&tabx[5],&tabx[6],&tabx[7],&tabx[8],&tabx[9],
		&tabx[10],&tabx[11],&tabx[12],&tabx[13],&tabx[14]
	);

	if (ret!=anzahl)
		ERR_MSG_RET0("E_COUNTBDNM");

	return 1;
}

// ================================================
	void resultfm::OnHelp()
// ================================================
{
	WinHelp(VHELP_EXT_SEARCH);
}

// ================================================
	void resultfm::OnTimer(UINT nIDEvent)
// ================================================
{
static int toggle=1;

	if (hinweiscounter++ >5)
	{
		m_warning.ShowWindow(SW_SHOWNORMAL);
		KillTimer(2345);
		return;
	}

	if (toggle)
	{
		toggle=0;
		m_warning.ShowWindow(SW_HIDE);
	}
	else
	{
		m_warning.ShowWindow(SW_SHOWNORMAL);
		toggle=1;
	}
	CDialog::OnTimer(nIDEvent);
}
