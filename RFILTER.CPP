// rfilter.cpp : implementation file
// IDD_CFG_FILTER

#include "stdafx.h"
#include "lightdlg.h"
#include "floating.h"
#include "rfilter.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern CLightDlg *gpMain;
extern floating	 gFloating;
extern _gconfig  gc;

_rfilter oldfilter;
static char DlgName[]="IDD_CFG_FILTER";

// ================================================================
	rfilter::rfilter(CWnd* pParent ) : CDialog(rfilter::IDD, pParent)
// ================================================================
{
	//{{AFX_DATA_INIT(rfilter)
	//}}AFX_DATA_INIT
}

// ================================================================
	void rfilter::DoDataExchange(CDataExchange* pDX)
// ================================================================
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(rfilter)
	DDX_Control(pDX, IDC_PASTETO, m_pasteto);
	DDX_Control(pDX, IDC_PASTESUBJ, m_pastesubj);
	DDX_Control(pDX, IDC_PASTEFROM, m_pastefrom);
	DDX_Control(pDX, IDC_EDIT_SHOWLAST, m_edit_showlast);
	DDX_Control(pDX, IDC_CHECK1, m_check_last);
	DDX_Control(pDX, IDC_EDIT_TO, m_edit_to);
	DDX_Control(pDX, IDC_EDIT_SUBJECT, m_edit_subject);
	DDX_Control(pDX, IDC_EDIT_FROM, m_edit_from);
	DDX_Control(pDX, IDC_EDIT_HEADER, m_edit_header);
	DDX_Control(pDX, IDC_EDIT_DAYS, m_edit_days);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(rfilter, CDialog)
	//{{AFX_MSG_MAP(rfilter)
	ON_BN_CLICKED(IDC_C_IN_FROM, OnCInFrom)
	ON_BN_CLICKED(IDC_C_IN_SUBJECT, OnCInSubject)
	ON_BN_CLICKED(IDC_C_IN_TO, OnCInTo)
	ON_BN_CLICKED(IDC_NO_REPLY, OnNoReply)
	ON_BN_CLICKED(IDC_NOT_OLDER, OnNotOlder)
	ON_BN_CLICKED(IDC_OUTGOING, OnOutgoing)
	ON_BN_CLICKED(IDC_UNREAD, OnUnread)
	ON_BN_CLICKED(IDC_NOFILTER, OnNofilter)
	ON_BN_CLICKED(IDC_SINCE_LAST_POLL, OnSinceLastPoll)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_BN_CLICKED(IDDEFAULT, OnDefaultFilter)
	ON_BN_CLICKED(IDC_MARKEDONLY, OnMarkedonly)
	ON_BN_CLICKED(IDC_HEADERONLY, OnHeaderonly)
	ON_BN_CLICKED(IDC_FORZEN_ONLY, OnForzenOnly)
	ON_BN_CLICKED(IDC_PASTESUBJ, OnPasteSubj)
	ON_BN_CLICKED(IDC_CHECK1, OnCheck1)
	ON_BN_CLICKED(IDC_PASTEFROM, OnPasteFrom)
	ON_BN_CLICKED(IDC_PASTETO, OnPasteTo)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// ================================================================
	BOOL rfilter::OnInitDialog()
// ================================================================
{
int  lng[]={
			IDC_UNREAD,
			IDC_OUTGOING,
			IDC_NO_REPLY,
			IDC_NOT_OLDER,
			IDC_C_IN_FROM,
			IDC_C_IN_TO,
			IDC_C_IN_SUBJECT,
			IDOK,
			IDCANCEL,
			IDC_STATIC1,
			IDC_NOT,
			IDC_NOFILTER,
			IDC_SINCE_LAST_POLL,
			IDDEFAULT,
			IDC_MARKEDONLY,
			IDC_HEADERONLY,
			IDC_CHECK1,
			IDC_STATIC496,
			IDC_FORZEN_ONLY,
			IDHELP
			};

	CDialog::OnInitDialog();
    set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));
	memcpy(&oldfilter,&grfilter,sizeof(_rfilter));

	((CButton *) GetDlgItem(IDC_NOT))->SetCheck 			(grfilter.Inverse);
	((CButton *) GetDlgItem(IDC_NOFILTER))->SetCheck 		(grfilter.OnNoFilter);
	((CButton *) GetDlgItem(IDC_C_IN_FROM))->SetCheck 		(grfilter.OnCInFrom);
	((CButton *) GetDlgItem(IDC_C_IN_SUBJECT))->SetCheck 	(grfilter.OnCInSubject);
	((CButton *) GetDlgItem(IDC_C_IN_TO))->SetCheck 		(grfilter.OnCInTo);
	((CButton *) GetDlgItem(IDC_NO_REPLY))->SetCheck 		(grfilter.OnNoReply);
	((CButton *) GetDlgItem(IDC_NOT_OLDER))->SetCheck 		(grfilter.OnNotOlder);
	((CButton *) GetDlgItem(IDC_OUTGOING))->SetCheck 		(grfilter.OnOutgoing);
	((CButton *) GetDlgItem(IDC_UNREAD))->SetCheck			(grfilter.OnUnread);
	((CButton *) GetDlgItem(IDC_SINCE_LAST_POLL))->SetCheck	(grfilter.OnSinceLastPoll);
	((CButton *) GetDlgItem(IDC_MARKEDONLY))->SetCheck		(grfilter.OnMarkedOnly);
	((CButton *) GetDlgItem(IDC_HEADERONLY))->SetCheck		(grfilter.OnHeaderOnly);
	((CButton *) GetDlgItem(IDC_FORZEN_ONLY))->SetCheck		(grfilter.OnFrozenOnly);

	m_edit_to.SetWindowText		(grfilter.to);
	m_edit_subject.SetWindowText(grfilter.subject);
	m_edit_from.SetWindowText	(grfilter.from);
	m_edit_days.SetWindowText	(grfilter.days);
	m_edit_header.SetWindowText	(grfilter.header);
	m_edit_showlast.SetWindowText(grfilter.lastonly);
	m_check_last.SetCheck		(grfilter.ShowLastOnly);
	
	m_edit_to.EnableWindow(grfilter.OnCInTo);
	m_pasteto.EnableWindow(grfilter.OnCInTo);
	m_edit_subject.EnableWindow(grfilter.OnCInSubject);
	m_pastesubj.EnableWindow(grfilter.OnCInSubject);
	m_edit_from.EnableWindow(grfilter.OnCInFrom);
	m_pastefrom.EnableWindow(grfilter.OnCInFrom);
	m_edit_days.EnableWindow(grfilter.OnNotOlder);
	m_edit_header.EnableWindow(grfilter.OnHeaderOnly);
	m_edit_showlast.EnableWindow(grfilter.ShowLastOnly);
	return TRUE;
}

// ================================================================
	void rfilter::OnCancel()
// ================================================================
{
	memcpy(&grfilter,&oldfilter,sizeof(_rfilter));
	CDialog::OnCancel();
}

// ================================================================
	void rfilter::OnOK()
// ================================================================
{
	m_edit_to.GetWindowText(grfilter.to,39);
	m_edit_subject.GetWindowText(grfilter.subject,39);
	m_edit_from.GetWindowText(grfilter.from,39);
	m_edit_days.GetWindowText(grfilter.days,39);
	m_edit_header.GetWindowText(grfilter.header,39);
	grfilter.Inverse=((CButton *)GetDlgItem(IDC_NOT))->GetCheck();

	if (!grfilter.OnNoFilter && gc.structured_display)
	{
	  	gc.structured_display=0;
		show_msg("Structured display OFF","Отображение цепочек ответов ВЫКЛ");
	}

	grfilter.ShowLastOnly=m_check_last.GetCheck();
	m_edit_showlast.GetWindowText(grfilter.lastonly,39);
	gpMain->m_filter.SetPushed(!grfilter.OnNoFilter);
	gFloating.m_filter.SetPushed(!grfilter.OnNoFilter);
	if (grfilter.OnNoFilter)
		show_msg("Display filter OFF","Фильтр отображения ВЫКЛ");
	else
		show_msg("Display filter ON","Фильтр отображения ВКЛ");
	CDialog::OnOK();
}

// ================================================================
	void rfilter::OnNofilter()
// ================================================================
{
	remove_all_filters();
	disable_edits();
	grfilter.OnNoFilter=1;
}

// ================================================================
	void rfilter::OnPasteFrom() 
// ================================================================
{
CString str;

	gpMain->m_from.GetWindowText(str);
	m_edit_from.SetWindowText(str);
}

// ================================================================
	void rfilter::OnPasteTo() 
// ================================================================
{
CString str;

	gpMain->m_to.GetWindowText(str);
	m_edit_to.SetWindowText(str);
}
	
// ================================================================
	void rfilter::OnPasteSubj() 
// ================================================================
{
CString str;

	gpMain->m_subject.GetWindowText(str);
	m_edit_subject.SetWindowText(str);
}

// ================================================================
	void rfilter::OnCInFrom()
// ================================================================
{
	remove_all_filters();
	disable_edits();
	grfilter.OnCInFrom=1;
	m_edit_from.EnableWindow(1);
	m_pastefrom.EnableWindow(1);
}

// ================================================================
	void rfilter::OnCInSubject()
// ================================================================
{
	remove_all_filters();
	disable_edits();
	grfilter.OnCInSubject=1;
	m_edit_subject.EnableWindow(1);
	m_pastesubj.EnableWindow(1);
}

// ================================================================
	void rfilter::OnCInTo()
// ================================================================
{
	remove_all_filters();
	disable_edits();
	grfilter.OnCInTo=1;
	m_edit_to.EnableWindow(1);
	m_pasteto.EnableWindow(1);
}

// ================================================================
	void rfilter::OnNoReply()
// ================================================================
{
	remove_all_filters();
	disable_edits();
	grfilter.OnNoReply=1;
}

// ================================================================
	void rfilter::OnMarkedonly()
// ================================================================
{
	remove_all_filters();
	disable_edits();
	grfilter.OnMarkedOnly=1;
}

// ================================================================
	void rfilter::OnHeaderonly()
// ================================================================
{
	remove_all_filters();
	disable_edits();
	grfilter.OnHeaderOnly=1;
	m_edit_header.EnableWindow(1);
}

// ================================================================
	void rfilter::OnNotOlder()
// ================================================================
{
	remove_all_filters();
	disable_edits();
	grfilter.OnNotOlder=1;
	m_edit_days.EnableWindow(1);
}

// ================================================================
	void rfilter::OnOutgoing()
// ================================================================
{
	remove_all_filters();
	disable_edits();
	grfilter.OnOutgoing=1;
}
	
// ================================================================
	void rfilter::OnUnread()
// ================================================================
{
	remove_all_filters();
	disable_edits();
	grfilter.OnUnread=1;
}

// ================================================================
	void rfilter::OnSinceLastPoll()
// ================================================================
{
	remove_all_filters();
	disable_edits();
	grfilter.OnSinceLastPoll=1;
}

// ================================================================
	void rfilter::OnForzenOnly()
// ================================================================
{
	remove_all_filters();
	disable_edits();
	grfilter.OnFrozenOnly=1;
}

// ================================================================
	void rfilter::OnCheck1() 
// ================================================================
{
	m_edit_showlast.EnableWindow(m_check_last.GetCheck());
}
	
// ================================================================
	void rfilter::disable_edits()
// ================================================================
{
	m_edit_to.EnableWindow(0);
	m_edit_subject.EnableWindow(0);
	m_pastefrom.EnableWindow(0);
	m_pasteto.EnableWindow(0);
	m_pastesubj.EnableWindow(0);
	m_edit_from.EnableWindow(0);
	m_edit_days.EnableWindow(0);
	m_edit_header.EnableWindow(0);
	m_edit_showlast.EnableWindow(0);
}

// ================================================================
	void rfilter::OnDefaultFilter()
// ================================================================
{
FILE *fp;

	if (err_out("DY_RWSMFAD")!=IDYES)
	   return;

	m_edit_to.GetWindowText		(grfilter.to,39);
	m_edit_subject.GetWindowText(grfilter.subject,39);
	m_edit_from.GetWindowText	(grfilter.from,39);
	m_edit_days.GetWindowText	(grfilter.days,39);
	m_edit_header.GetWindowText	(grfilter.header,39);

	grfilter.ShowLastOnly=m_check_last.GetCheck();
	m_edit_showlast.GetWindowText(grfilter.lastonly,39);

	fp=fopen("mfilter.cfg","wb");
	if (fp)
	{
		fwrite(&grfilter,sizeof (grfilter),1,fp);
		fclose(fp);
	}
}

// ================================================================
	void remove_all_filters(void)
// ================================================================
{
	grfilter.OnNoFilter		=0;
	grfilter.OnCInFrom		=0;
	grfilter.OnCInSubject   =0;
	grfilter.OnCInTo        =0;
	grfilter.OnNoReply      =0;
	grfilter.OnNotOlder     =0;
	grfilter.OnOutgoing     =0;
	grfilter.OnUnread       =0;
	grfilter.OnSinceLastPoll=0;
	grfilter.OnMarkedOnly	=0;
	grfilter.OnHeaderOnly	=0;
	grfilter.OnFrozenOnly	=0;
}

// ================================================================
	void rfilter::OnHelp()
// ================================================================
{
	WinHelp(VHELP_CFG_FILTER);
}

BOOL rfilter::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return TRUE;
}
