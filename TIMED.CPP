// Timed.cpp : implementation file
//

#include "stdafx.h"
#include "Timed.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

char	but_capt[100];
extern	int get_identifier_line(LPCSTR id,LPSTR buf,LPCSTR sect);
extern LangID LANG_ID;

// =======================================================================
	CTimed::CTimed(CWnd* pParent ) : CDialog(CTimed::IDD, pParent)
// =======================================================================
{
	//{{AFX_DATA_INIT(CTimed)
	//}}AFX_DATA_INIT
}

// =======================================================================
	void CTimed::DoDataExchange(CDataExchange* pDX)
// =======================================================================
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTimed)
	DDX_Control(pDX, IDC_STATIC1, m_message);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTimed, CDialog)
	//{{AFX_MSG_MAP(CTimed)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// =======================================================================
	BOOL CTimed::OnInitDialog() 
// =======================================================================
{
char buf[3000];

	CDialog::OnInitDialog();
	*buf=0;
	get_identifier_line(msgid,buf,langstr("GERMAN","ENGLISH","RUSSIAN","FRENCH"));
	m_message.SetWindowText(buf);
	
	if (*but1)		GetDlgItem(IDOK)->SetWindowText(but1);
	if (*but2)		GetDlgItem(IDCANCEL)->SetWindowText(but2);
	if (delay>0)	SetTimer(2345,1000,NULL);

	GetDlgItem(defbut==1 ? IDOK : IDCANCEL)->GetWindowText(but_capt,100);
	GetDlgItem(defbut==1 ? IDOK : IDCANCEL)->SetWindowText(L("S_14",defbut==1 ? but1 : but2,delay));
	GetDlgItem(defbut==1 ? IDOK : IDCANCEL)->SetFocus();

	return TRUE;
}

// =======================================================================
	void CTimed::OnCancel() 
// =======================================================================
{
	KillTimer(2345);
	retval=2;
	CDialog::OnCancel();
}

// =======================================================================
	void CTimed::OnOK() 
// =======================================================================
{
	KillTimer(2345);
	retval=1;
	CDialog::OnOK();
}

// =======================================================================
	void CTimed::OnTimer(UINT nIDEvent) 
// =======================================================================
{
	if (delay<=0)
	{
		KillTimer(2345);
		retval=defbut;
		CDialog::OnCancel();
	}
	GetDlgItem(defbut==2 ? IDCANCEL : IDOK)->SetWindowText(L("S_14",but_capt,delay));
	delay--;
	CDialog::OnTimer(nIDEvent);
}							  
