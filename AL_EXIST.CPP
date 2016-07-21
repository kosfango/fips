// al_exist.cpp : implementation file
// IDD_CHOOSE_SAVE

#include "stdafx.h"
#include "al_exist.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

static char DlgName[]="IDD_CHOOSE_SAVE";

al_exist::al_exist(CWnd* pParent /*=NULL*/)
	: CDialog(al_exist::IDD, pParent)
{
	//{{AFX_DATA_INIT(al_exist)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void al_exist::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(al_exist)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(al_exist, CDialog)
	//{{AFX_MSG_MAP(al_exist)
	ON_BN_CLICKED(IDAPPEND, OnAppend)
	ON_BN_CLICKED(IDOVER, OnOver)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL al_exist::OnInitDialog()
{
int  lng[]={
			IDAPPEND,
			IDOVER,
			IDCANCEL
			};

	CDialog::OnInitDialog();
	set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));
	return TRUE;
}

void al_exist::OnCancel()
{
	CDialog::OnCancel();
}

void al_exist::OnAppend()
{
	CDialog::EndDialog(IDAPPEND);
}

void al_exist::OnOver()
{
	CDialog::EndDialog(IDOVER);
}
