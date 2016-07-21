// guard.cpp : implementation file
// IDD_GUARD

#include "stdafx.h"
#include "structs.h"
#include "LIGHT.h"
#include "guard.h"


static char DlgName[]="IDD_GUARD";
// guard dialog

IMPLEMENT_DYNAMIC(guard, CDialog)
guard::guard(CWnd* pParent /*=NULL*/)
	: CDialog(guard::IDD, pParent)
	, m_password(_T(""))
{
	m_allow=TRUE;
}

guard::~guard()
{
}

void guard::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_PASSWORD, m_password);
	DDV_MaxChars(pDX, m_password, 20);
}


BEGIN_MESSAGE_MAP(guard, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// guard message handlers
BOOL guard::OnInitDialog()
{
int	lng[]={
			IDC_STATIC,
			IDOK,
			IDCANCEL
			};

	CDialog::OnInitDialog();
	set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));
	m_allow=FALSE;
	return TRUE;
}

void guard::OnBnClickedOk()
{
UINT m=0;

	UpdateData(1);
	m_password+="Phoenix";
	for(int i=0;i<=m_password.GetLength();i++)	m=(m<<1)+m_password[i];
	m_allow= get_cfg(CFG_COMMON,"Entry",0)==m;
	OnOK();
}
