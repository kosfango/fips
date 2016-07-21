// sedesta.cpp : implementation file
// IDD_SEARCH_AREA_SELECTION

#include "stdafx.h"
#include "sedesta.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CString gSearchDestArea;
static char DlgName[]="IDD_SEARCH_AREA_SELECTION";

// ====================================
	sedesta::sedesta(CWnd* pParent ) : CDialog(sedesta::IDD, pParent)
// ====================================
{
	//{{AFX_DATA_INIT(sedesta)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

// ====================================
	void sedesta::DoDataExchange(CDataExchange* pDX)
// ====================================
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(sedesta)
	DDX_Control(pDX, IDC_LIST1, m_list);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(sedesta, CDialog)
	//{{AFX_MSG_MAP(sedesta)
	ON_BN_CLICKED(IDUSE, OnUse)
	ON_LBN_DBLCLK(IDC_LIST1, OnDblclkList1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// ====================================
	BOOL sedesta::OnInitDialog()
// ====================================
{
areadef adef;
int		ret,i;
char	bufxx[300],buf[300],*p;
int		tabs[]={150,190,220,400};
int		lng[]={
				IDCANCEL,
				IDUSE
				};

	CDialog::OnInitDialog();
    set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));
	gSearchDestArea.Empty();

	TABULATE_LB(IDC_LIST1);
 	EXTENT_LB(IDC_LIST1,1000);

	for (i=0;;i++)
	{
		ret=db_get_area_by_index(i,&adef);
		if (ret!=DB_OK)
		   break;
		if (!adef.localmail)
			continue;

		sprintf(bufxx,"%s",adef.description);
		p=strchr(bufxx,'\n'); if (p) *p=0;
		p=strchr(bufxx,'\r'); if (p) *p=0;
		sprintf(buf,"%s\t%s",adef.echotag,bufxx);
		m_list.AddString(buf);
	}
	return TRUE;
}

// ====================================
	void sedesta::OnCancel()
// ====================================
{
	gSearchDestArea.Empty();
	CDialog::OnCancel();
}

// ====================================
	void sedesta::OnUse()
// ====================================
{
CString help;
int		sel;

	gSearchDestArea.Empty();
	GET_SELID(IDC_LIST1);
	m_list.GetText(sel,help);
	get_token(help,0,gSearchDestArea);
	CDialog::OnOK();
}

// ====================================
	void sedesta::OnDblclkList1()
// ====================================
{
	OnUse();
}
