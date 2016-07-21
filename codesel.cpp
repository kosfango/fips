// codesel.cpp : implementation file
// IDD_CODESEL

#include "stdafx.h"
#include "LIGHT.h"
#include "codesel.h"

static char DlgName[]="IDD_CODESEL";
// codesel dialog

IMPLEMENT_DYNAMIC(codesel, CDialog)
codesel::codesel(CWnd* pParent /*=NULL*/)
	: CDialog(codesel::IDD, pParent)
{
}

void codesel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(codesel, CDialog)
	ON_BN_CLICKED(IDC_OEM, OnOem)
	ON_BN_CLICKED(IDC_ANSI, OnAnsi)
END_MESSAGE_MAP()

BOOL codesel::OnInitDialog()
{
int  lng[]={
		IDC_OEM,
		IDC_ANSI
		};

	CDialog::OnInitDialog();
	set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));
	return TRUE;
}

void codesel::OnOem()
{
	CDialog::EndDialog(IDC_OEM);
}

void codesel::OnAnsi()
{
	CDialog::EndDialog(IDC_ANSI);
}
