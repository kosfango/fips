// searepl.cpp : implementation file
// IDD_SEARCH_REPLACE

#include "stdafx.h"
#include "writmail.h"
#include "searepl.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern CFont   	font_mailtext;
extern writmail *gEditor;
CStrList		sea_repl;
static char DlgName[]="IDD_SEARCH_REPLACE";

// =======================================================================
	searepl::searepl(CWnd* pParent ) : CDialog(searepl::IDD, pParent)
// =======================================================================
, m_bRegexp(FALSE)
	{
	//{{AFX_DATA_INIT(searepl)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

// =======================================================================
	void searepl::DoDataExchange(CDataExchange* pDX)
// =======================================================================
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(searepl)
	DDX_Control(pDX, IDC_USERCONV, m_userconv);
	DDX_Control(pDX, IDC_IGNCASE, m_igncase);
	DDX_Control(pDX, IDC_EDIT_SEARCH, m_search);
	DDX_Control(pDX, IDC_EDIT_REPLACE, m_replace);
	//}}AFX_DATA_MAP
	DDX_Check(pDX, IDC_REGEXP, m_bRegexp);
}

BEGIN_MESSAGE_MAP(searepl, CDialog)
	//{{AFX_MSG_MAP(searepl)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// =======================================================================
	BOOL searepl::OnInitDialog()
// =======================================================================
{
int  lng[]={
			IDC_IGNCASE,
			IDC_SEARCH,
			IDC_REPLACE,
			IDC_USERCONV,
			IDOK,
			IDCANCEL
			};

	CDialog::OnInitDialog();
  set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));
	m_search.SetWindowText(get_cfg(CFG_SEARCH,"SearchPattern",""));
	m_replace.SetWindowText(get_cfg(CFG_SEARCH,"ReplacePattern",""));
	m_igncase.SetCheck(get_cfg(CFG_SEARCH,"IgnoreCase",1));
	m_userconv.SetCheck(get_cfg(CFG_SEARCH,"UserConvert",1));
	return TRUE;
}

// =======================================================================
	void searepl::OnOK()
// =======================================================================
{
CString pattern,replace;
int igncase,userconv;

  m_search.GetWindowText(pattern);
	m_replace.GetWindowText(replace);

	if (strstr(replace,pattern))	ERR_MSG_RET("E_SWBRE");

	igncase=m_igncase.GetCheck();
	userconv=m_userconv.GetCheck();
	set_cfg(CFG_SEARCH,"SearchPattern",pattern);
	set_cfg(CFG_SEARCH,"ReplacePattern",replace);
	set_cfg(CFG_SEARCH,"IgnoreCase",igncase);
	set_cfg(CFG_SEARCH,"UserConvert",userconv);

	CPoint found;
	int foundlen = 0;
	char *old_loc;
	if (IsOem(&font_mailtext))	//gEditor->m_mailtext.IsOemCharset())
	{
		old_loc = strdup(setlocale(LC_ALL, ".866"));
		pattern.AnsiToOem();
	}
	else 
		old_loc = strdup(setlocale(LC_ALL, "ru"));
	if (FindStringHelper(&gEditor->m_buffer, !igncase, m_bRegexp, pattern, found, foundlen))
	{
		gEditor->m_mailtext.HighlightText(found, foundlen, FALSE);
    ReplaceSelection(&gEditor->m_buffer, &gEditor->m_mailtext, replace);
	}
	setlocale(LC_ALL, old_loc);
	free(old_loc);

	CDialog::OnOK();
}
