// pnotf.cpp : implementation file
//

#include "stdafx.h"
#include "light.h"
#include "pnotf.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// pnotf dialog


pnotf::pnotf(CWnd* pParent /*=NULL*/)
	: CDialog(pnotf::IDD, pParent)
{
	//{{AFX_DATA_INIT(pnotf)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void pnotf::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(pnotf)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(pnotf, CDialog)
	//{{AFX_MSG_MAP(pnotf)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// pnotf message handlers


BOOL pnotf::OnInitDialog() 
{
	RECT re;
	CDialog::OnInitDialog();
	SetTimer(2635,1000,0);
	GetWindowRect(&re);
	re.left-=150;
	re.right-=150;
	re.bottom-=150;
	re.top-=150;
	MoveWindow(&re,1);
	MessageBeep(0);	
	return TRUE;
}

void pnotf::OnTimer(UINT nIDEvent) 
{
	KillTimer(2635);
	
	CDialog::OnTimer(nIDEvent);
	OnCancel();
}
