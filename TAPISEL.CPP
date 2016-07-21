// tapisel.cpp : implementation file
// IDD_SELECT_MODEM

#include "stdafx.h"
#include "tapisel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CString gTapiModem;
extern CStrList modemkeys;
static char DlgName[]="IDD_SELECT_MODEM";

// ===================================================================
	tapisel::tapisel(CWnd* pParent ) : CDialog(tapisel::IDD, pParent)
// ===================================================================
{
	//{{AFX_DATA_INIT(tapisel)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void tapisel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(tapisel)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(tapisel, CDialog)
	//{{AFX_MSG_MAP(tapisel)
	ON_LBN_DBLCLK(IDC_LIST1, OnDblclkList1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// ===================================================================
	BOOL tapisel::OnInitDialog()
// ===================================================================
{
int tabs[]={2000};
int  lng[]={
			IDOK,
			IDCANCEL,
			IDC_STATIC1
			};

    CDialog::OnInitDialog();
	TABULATE_LB(IDC_LIST1);
    set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));
	UPDATE_LB(modemkeys,IDC_LIST1);
	return TRUE;
}

// ===================================================================
	void tapisel::OnOK()
// ===================================================================
{
int sel;

	GET_SELID(IDC_LIST1);
	gTapiModem=modemkeys.GetString(sel);
	CDialog::OnOK();
}

// ===================================================================
	void tapisel::OnCancel()
// ===================================================================
{
	CDialog::OnCancel();
}

// ===================================================================
	void tapisel::OnDblclkList1()
// ===================================================================
{
	OnOK();
}
