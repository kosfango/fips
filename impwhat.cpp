// IDD_CHOOSE_IMPORT

#include "stdafx.h"
#include "writmail.h"
#include "impwhat.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern writmail *gEditor;

static char DlgName[]="IDD_CHOOSE_IMPORT";

// ====================================================================
	impwhat::impwhat(CWnd* pParent ) : CDialog(impwhat::IDD, pParent)
// ====================================================================
{
	//{{AFX_DATA_INIT(impwhat)
	//}}AFX_DATA_INIT
}

// ====================================================================
	void impwhat::DoDataExchange(CDataExchange* pDX)
// ====================================================================
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(impwhat)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(impwhat, CDialog)
	//{{AFX_MSG_MAP(impwhat)
	ON_BN_CLICKED(IDC_OEMTEXT, OnOemText)
	ON_BN_CLICKED(IDC_ANSITEXT, OnAnsiText)
	ON_BN_CLICKED(IDC_UUENCODE, OnUuencode)
	ON_BN_CLICKED(IDC_MULTIM, OnMultim)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// ====================================================================
	BOOL impwhat::OnInitDialog()
// ====================================================================
{
int  lng[]={
			IDCANCEL,
			IDC_OEMTEXT,
			IDC_ANSITEXT,
			IDC_UUENCODE,
			IDC_MULTIM
			};

	CDialog::OnInitDialog();
    set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));
	return TRUE;
}

// ====================================================================
	void impwhat::OnMultim()
// ====================================================================
{
	gEditor->OnMultimediaInsert();
	OnCancel();
}

// ====================================================================
	void impwhat::OnOemText()
// ====================================================================
{
	gEditor->TextInsert(TRUE);
	OnCancel();
}

// ====================================================================
	void impwhat::OnAnsiText()
// ====================================================================
{
	gEditor->TextInsert(FALSE);
	OnCancel();
}

// ====================================================================
	void impwhat::OnUuencode()
// ====================================================================
{
	gEditor->OnBinaryInsert();
	OnCancel();
}

// ====================================================================
	void impwhat::OnCancel()
// ====================================================================
{
	CDialog::OnCancel();
}

