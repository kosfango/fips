// marksel.cpp : implementation file
// IDD_CHOOSE_MARK

#include "stdafx.h"
#include "lightdlg.h"
#include "marksel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CByteArray AttrListBuffer;
extern CLightDlg *gpMain;

static char DlgName[]="IDD_CHOOSE_MARK";

/////////////////////////////////////////////////////////////////////////////
// marksel dialog

marksel::marksel(CWnd* pParent /*=NULL*/)
	: CDialog(marksel::IDD, pParent)
{
	//{{AFX_DATA_INIT(marksel)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void marksel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(marksel)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(marksel, CDialog)
	//{{AFX_MSG_MAP(marksel)
	ON_BN_CLICKED(IDC_ON_SELECTED, OnOnSelected)
	ON_BN_CLICKED(IDC_ON_MARKED, OnOnMarked)
	ON_BN_CLICKED(IDC_ON_BOTH, OnOnBoth)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// marksel message handlers

BOOL marksel::OnInitDialog()
{
int  lng[]={
			IDCANCEL,
			IDC_ON_SELECTED,
			IDC_ON_MARKED,
			IDC_ON_BOTH
			};

    CDialog::OnInitDialog();
    set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));
	m_seltype=0;
	return TRUE;
}

void marksel::OnCancel()
{
	m_seltype=0;
	CDialog::OnCancel();
}

void marksel::OnOnSelected() 
{
	m_seltype=0;	// 0=sel 1=marked 2=both
	CDialog::OnOK();
}

void marksel::OnOnMarked() 
{
	m_seltype=1;	// 0=sel 1=marked 2=both
	CDialog::OnOK();
}

void marksel::OnOnBoth() 
{
	m_seltype=2;	// 0=sel 1=marked 2=both
	CDialog::OnOK();
}

// ===============================================
	void marksel::get_selection(void )
// ===============================================
{
int i;

	switch (m_seltype)
	{
		case 1:	// for all marked
			gpMain->m_subjlist.SetSel(-1,0);
			for (i=0;i<AttrListBuffer.GetCount();i++)
			{
				if (AttrListBuffer[i] & 0x80)
					gpMain->m_subjlist.SetSel(i,1);
			}
			break;
		case 2:	// for both
			for (i=0;i<AttrListBuffer.GetCount();i++)
			{
				if (AttrListBuffer[i] & 0x80)
					gpMain->m_subjlist.SetSel(i,1);
			}
			break;
		default:// nothing to do or for selected
			m_seltype=0;	// 0=sel 1=marked 2=both
	}
}

