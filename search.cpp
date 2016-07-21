// search.cpp : implementation file
// IDD_SIMPLE_SEARCH

#include "stdafx.h"
#include "lightdlg.h"
#include "exsearch.h"
#include "search.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern LangID			LANG_ID;
extern CStrList		subject_list;
extern CLightDlg	*gpMain;
extern exsearch		gSearch;
extern _gconfig gc;

static char DlgName[]="IDD_SIMPLE_SEARCH";

/////////////////////////////////////////////////////////////////////////////
// search dialog

search::search(CWnd* pParent /*=NULL*/)
	: CDialog(search::IDD, pParent)
{
	//{{AFX_DATA_INIT(search)
	m_pattern = _T("");
	m_multiselect = FALSE;
	//}}AFX_DATA_INIT
}

void search::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(search)
	DDX_Control(pDX, IDC_STATUS, m_status);
	DDX_Radio(pDX, IDC_UP, m_direction);
	DDX_Check(pDX, IDC_CONVERT, m_convert);
	DDX_Check(pDX, IDC_HEADER, m_header);
	DDX_Check(pDX, IDC_HIGHLIGHT, m_highlight);
	DDX_Check(pDX, IDC_IGNORECASE, m_ignorecase);
	DDX_Check(pDX, IDC_MESSAGETEXT, m_messagetext);
	DDX_Text(pDX, IDC_PATTERN, m_pattern);
	DDX_Check(pDX, IDC_MULTI, m_multiselect);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(search, CDialog)
	//{{AFX_MSG_MAP(search)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_BN_CLICKED(IDEXTSEARCH, OnExtsearch)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// ===============================================
	BOOL search::OnInitDialog()
// ===============================================
{
int  lng[]={
			IDHELP,
			IDOK,
			IDC_HEADER,
			IDC_MESSAGETEXT,
			IDC_IGNORECASE,
			IDC_HIGHLIGHT,
			IDCANCEL,
			IDC_STATIC1,
			IDEXTSEARCH,
			IDC_CONVERT,
			IDC_STATIC2,
			IDC_DIRECTION,
			IDC_UP,
			IDC_DOWN,
			IDC_ALL,
			IDC_MULTI
			};

	CDialog::OnInitDialog();
    set_dlg_language(this,DlgName,lng,sizeof lng/sizeof(int));

	m_pattern=strcpy(gc.search_pattern,get_cfg(CFG_SEARCH,"SearchPattern",""));
	m_highlight=gc.show_result=get_cfg(CFG_SEARCH,"ShowResults",1);
	m_ignorecase=gc.ignorecase=get_cfg(CFG_SEARCH,"IgnoreCase",1);
	m_header=gc.inheader=get_cfg(CFG_SEARCH,"InHeader",1);
	m_messagetext=gc.inmessage=get_cfg(CFG_SEARCH,"InText",1);
	m_convert=gc.userconv=get_cfg(CFG_SEARCH,"UserConvert",0);
	m_direction=get_cfg(CFG_SEARCH,"Direction",2);
	m_multiselect=get_cfg(CFG_SEARCH,"Multiselect",0);

	if (LANG_ID==LANG_RUS)	
		SHOWID(IDC_CONVERT)
	else
		HIDEID(IDC_CONVERT);
	if (gc.next_find)
	{
		gc.next_find=0;
		OnOK();
	}
	UpdateData(FALSE);
	return TRUE;
}

// ===============================================
	void search::OnCancel()
// ===============================================
{
	UpdateData(TRUE);
	set_cfg(CFG_SEARCH,"SearchPattern",strcpy(gc.search_pattern,m_pattern));
	set_cfg(CFG_SEARCH,"ShowResults",gc.show_result=m_highlight);
	set_cfg(CFG_SEARCH,"IgnoreCase",gc.ignorecase=m_ignorecase);
	set_cfg(CFG_SEARCH,"InHeader",gc.inheader=m_header);
	set_cfg(CFG_SEARCH,"InText",gc.inmessage=m_messagetext);
	set_cfg(CFG_SEARCH,"UserConvert",gc.userconv=m_convert);
	set_cfg(CFG_SEARCH,"Direction",m_direction);
	set_cfg(CFG_SEARCH,"Multiselect",m_multiselect);
//	CDialog::OnCancel();
	CDialog::DestroyWindow();
}

// ===============================================
// Begin search
	void search::OnOK()
// ===============================================
{
mailheader mh;
CString line;
int			offset,sel,cnt,ind;
char    *p,*p2;

	UpdateData(TRUE);
	m_status.SetWindowText(NULL);
	cnt=gpMain->m_subjlist.GetCount();
	if (cnt==0 || m_pattern.IsEmpty())	return;
	sel=gpMain->m_subjlist.GetCurSel();
	if (sel==LB_ERR || m_direction==2)	sel=0;

	p=(LPSTR)malloc(MAX_MSG);
	p2=(LPSTR)malloc(2*MAX_MSG);	// for expansion with '\n'
	if (!p || !p2)
	{
		err_out("E_MEM_OUT");
		goto end_close;
	}

	BeginWaitCursor();
	strnzcpy(gc.search_pattern,m_pattern,100);
new_search:
	if (m_direction==0)	sel--;	else	sel++;
	if (sel>=cnt || sel<0)
	{
		m_status.SetWindowText(L("S_347"));	// not found
		goto end_open;
	}
	get_token(subject_list.GetString(sel),MAIL_INDEX,line);
	sscanf(line,"%d",&ind);
	memset(&mh,0,sizeof(mailheader));
  if (db_gethdr_by_index(gustat.ar2han,ind,&mh) != DB_OK)	goto end_close;
	
	if (m_header)	// search in header
	{
		if (find_next_entry(mh.fromname,!m_ignorecase,m_pattern,&offset,1,m_convert))
		{
			m_status.SetWindowText(L("S_259"));	// found in from
			show_result(p2,&mh,sel,offset);
			goto end_open;
		}
		if (find_next_entry(mh.toname,!m_ignorecase,m_pattern,&offset,1,m_convert))
		{
			m_status.SetWindowText(L("S_262"));	// found in to
			show_result(p2,&mh,sel,offset);
			goto end_open;
		}
		if (find_next_entry(mh.subject,!m_ignorecase,m_pattern,&offset,1,m_convert))
		{
			m_status.SetWindowText(L("S_261"));	// found in subject
			show_result(p2,&mh,sel,offset);
			goto end_open;
		}
	}
	if (m_messagetext)	// search in mailtext
	{
	  if (db_get_mailtext(gustat.ar2han,&mh,p,MAX_MSG)!=DB_OK)
		{
			err_out("DBGETMAILTEXT",gustat.ar2han);
			goto end_close;
		}
		filter_mailtext(p,p2);
		if (find_next_entry(p2,!m_ignorecase,m_pattern,&offset,1,m_convert))
		{
			m_status.SetWindowText(L("S_260"));	// found in text
			show_result(p2,&mh,sel,offset);
			goto end_open;
		}
	}
	goto new_search;

end_open:
	if (p) free(p); 
	if (p2) free(p2);
	EndWaitCursor();
	GetDlgItem(IDOK)->SetFocus();
	return;

end_close:
	if (p) free(p); 
	if (p2) free(p2);
	EndWaitCursor();
//	CDialog::OnOK();
	CDialog::DestroyWindow();
}

// ====================================================================
	void search::OnExtsearch()
// ====================================================================
{
	gSearch.m_edit.SetWindowText(gc.search_pattern);
	gSearch.ShowWindow(SW_SHOWNORMAL);
	OnCancel();
}

// ====================================================================
	void search::show_result(char *st,mailheader *pmh,int sel,int offs)
// ====================================================================
{
int line;
char *p;
RECT re;

	gc.display_this_mail_only=2;
	if (!m_multiselect)
	{
		gpMain->m_subjlist.SetSel(-1,0);
		gpMain->m_subjlist.GetClientRect(&re);
		gpMain->m_subjlist.InvalidateRect(&re);
	}
	gpMain->m_subjlist.SetSel(sel,1);
	gpMain->m_subjlist.SetTopIndex(sel==0 ? 0 : sel-1);
	gc.no_dblclick=1;
	gpMain->OnDblclkList();
	line=0;
	p=st;
	while (*p && (p-st)<offs)
	{
		if (*p=='\r')	line++;
		p++;
	}
	gpMain->m_mailview.SetTopIndex(line);
	if (m_direction!=0)
	{
		m_direction=1;
		UpdateData(FALSE);
	}
}

// ====================================================================
	void search::OnHelp()
// ====================================================================
{
	WinHelp(VHELP_SIMPLE_SEARCH);
}

BOOL search::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return TRUE;
}
