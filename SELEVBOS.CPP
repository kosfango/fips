// selevbos.cpp : implementation file
// IDD_SELECT_EV_BOSS

#include "stdafx.h"
#include "selevbos.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CString		SelectedBoss;
static char DlgName[]="IDD_SELECT_EV_BOSS";

selevbos::selevbos(CWnd* pParent /*=NULL*/)
	: CDialog(selevbos::IDD, pParent)
{
	//{{AFX_DATA_INIT(selevbos)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void selevbos::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(selevbos)
	DDX_Control(pDX, IDC_LIST, m_list);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(selevbos, CDialog)
	//{{AFX_MSG_MAP(selevbos)
	ON_LBN_DBLCLK(IDC_LIST, OnDblclkList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// =============================================
	BOOL selevbos::OnInitDialog()
// =============================================
{
CString	str,str1;
int	 tabs[]={50};
char buf[MAX_BOSSLEN];
int	 i=0;
int  lng[]={
			IDOK,
			IDCANCEL,
			IDC_STATIC1,
			};

    CDialog::OnInitDialog();
    set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));
	TABULATE_LB(IDC_LIST);
	while (db_get_boss_by_index(i++,buf))
	{
		get_token(buf,BOSSSYST,str);
		get_token(buf,BOSSADDR,str1);
		str1+="\t"+str;
		m_list.AddString(str1);
	}
	SelectedBoss.Empty();
	return TRUE;
}

// =============================================
	void selevbos::OnCancel()
// =============================================
{
	CDialog::OnCancel();
}

// =============================================
	void selevbos::OnOK()
// =============================================
{
int sel;

	GET_SELID(IDC_LIST);
	m_list.GetText(sel,SelectedBoss);
	CDialog::OnOK();
}

// =============================================
	void selevbos::OnDblclkList()
// =============================================
{
	OnOK();
}
