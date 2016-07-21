// msgbox.cpp : implementation file
// IDD_MESSAGEBOX

#include "stdafx.h"
#include "msgbox.h"					  

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// msgbox dialog

msgbox::msgbox(CWnd* pParent /*=NULL*/)
	: CDialog(msgbox::IDD, pParent)
{
	//{{AFX_DATA_INIT(msgbox)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	delay_mult=0;
}

void msgbox::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(msgbox)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(msgbox, CDialog)
	//{{AFX_MSG_MAP(msgbox)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// msgbox message handlers

void msgbox::OnTimer(UINT nIDEvent) 
{
	KillTimer(2635);
	CDialog::OnTimer(nIDEvent);
	OnCancel();
}

BOOL msgbox::OnInitDialog() 
{
	CDialog::OnInitDialog();
	if (title.GetLength() >0)
	   SetWindowText(title);
	CenterWindow(GetDesktopWindow());
	SetTimer(2635,400+delay_mult,0);
	return TRUE;
}

