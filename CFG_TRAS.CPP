// cfg_tras.cpp : implementation file
// IDD_CFG_TWITS

#include "stdafx.h"
#include "resource.h"
#include "structs.h"
#include "cfg_tras.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

static char DlgName[]="IDD_CFG_TWITS";
CStrList twits;

// =====================================================================
	cfg_tras::cfg_tras(CWnd* pParent ) : CSAPrefsSubDlg(cfg_tras::IDD, pParent)
// =====================================================================
{
	//{{AFX_DATA_INIT(cfg_tras)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

// =====================================================================
	void cfg_tras::DoDataExchange(CDataExchange* pDX)
// =====================================================================
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(cfg_tras)
	DDX_Control(pDX, IDC_USEINTHISAREAS, m_areas);
	DDX_Control(pDX, IDC_LIST, m_list);
	DDX_Control(pDX, IDC_EDIT, m_edit);
	DDX_Control(pDX, IDC_TO, m_to);
	DDX_Control(pDX, IDC_SUBJECT, m_subject);
	DDX_Control(pDX, IDC_FROM, m_from);
	DDX_Control(pDX, IDC_FROMADDR, m_fromaddr);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(cfg_tras, CDialog)
	//{{AFX_MSG_MAP(cfg_tras)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_LBN_SELCHANGE(IDC_LIST, OnSelchangeList)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_CHANGE, OnBnClickedChange)
END_MESSAGE_MAP()

// =====================================================================
LPCSTR cfg_tras::GetName(void)	{return DlgName;}
// =====================================================================

// =====================================================================
	BOOL cfg_tras::OnInitDialog()
// =====================================================================
{
int  tabs[]={100,130,300};
int  lng[]={
			IDC_STATIC1,
			IDC_STATIC2,
			IDC_FROM,
			IDC_TO,
			IDC_SUBJECT,
			IDC_STATIC4,
			IDC_ADD,
			IDC_DELETE,
			IDC_STATIC5,
			IDC_STATIC3,
			IDC_CHANGE,
			IDC_FROMADDR
			};

	CDialog::OnInitDialog();
    set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));

	TABULATE_LB(IDC_LIST);
	twits.LoadFromFile("twitfilt.cfg");
	if (twits.GetCount()>0)
		UPDATE_LB(twits,IDC_LIST);

	return TRUE;
}

// =====================================================================
	void cfg_tras::OnAdd()
// =====================================================================
{
CString str;

	make_line(str);
	if (!str.IsEmpty()) 
	{
		twits.AddTail(str);
		UPDATE_LB(twits,IDC_LIST);
		m_list.SetCurSel(m_list.GetCount()-1);
	}
}

void cfg_tras::OnBnClickedChange()
{
CString str;
int sel;

	GET_SELID(IDC_LIST);
	make_line(str);
	if (!str.IsEmpty()) 
	{
		twits.Replace(sel,str);
		UPDATE_LB(twits,IDC_LIST);
		m_list.SetCurSel(sel);
	}
}

// =====================================================================
	void cfg_tras::OnDelete()
// =====================================================================
{
int sel;

	GET_SELID(IDC_LIST);
	twits.Remove(sel);
	UPDATE_LB(twits,IDC_LIST);
}

// =====================================================================
	void cfg_tras::OnOK()
// =====================================================================
{
	twits.SaveToFile("twitfilt.cfg");
	if(m_bCloseOnOk)
		CDialog::OnOK();
}

// =====================================================================
	void cfg_tras::OnSelchangeList()
// =====================================================================
{
CString str,text,val,areas;
int		sel,valx=0;

	GET_SELID(IDC_LIST);
	str=twits.GetString(sel);

	// text\flags\areas\val ...
	get_token(str,0,text);
	get_token(str,2,areas);
	get_token(str,3,val);
	sscanf(val,"%d",&valx);

	m_from.SetCheck(valx & 0x1);
	m_to.SetCheck(valx & 0x2);
	m_subject.SetCheck(valx & 0x4);
	m_fromaddr.SetCheck(valx & 0x8);
	m_areas.SetWindowText(areas);
	m_edit.SetWindowText(text);

}

// =====================================================================
	void cfg_tras::OnHelp()   // HELP Button
// =====================================================================
{
	WinHelp(VHELP_CFG_TRAHS);
}

// =====================================================================
	BOOL cfg_tras::OnHelpInfo(HELPINFO* pHelpInfo) 
// =====================================================================
{
	OnHelp();
	return TRUE;
}

// =====================================================================
	void cfg_tras::make_line(CString &line)
// =====================================================================
{
CString str1,str2;
int		val=0;

	line.Empty();
	m_edit.GetWindowText(str1);
	if (str1.IsEmpty())	return;
	m_areas.GetWindowText(str2);
	if (str2.IsEmpty())	str2="*";
	str1+="\t";
	if (m_from.GetCheck())
	{
// FROM
		str1+=L("S_265")[0];
		val|=1;
	}
	if (m_to.GetCheck())
	{
// TO
		str1+=L("S_424")[0];
		val|=2;
	}
	if (m_subject.GetCheck())
	{
// SUBJECT
		str1+=L("S_416")[0];
		val|=4;
	}
	if (m_fromaddr.GetCheck())
	{
// FROM-ADDR
		str1+=L("S_464")[0];
		val|=8;
	}
	if (val==0)	ERR_MSG_RET("E_ATLOGBMBS");
	line.Format("%s\t%s\t%d",str1,str2,val);
}
