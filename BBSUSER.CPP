// bbsuser.cpp : implementation file
// IDD_BBS_USERS

#include "stdafx.h"
#include "resource.h"
#include "structs.h"
#include "bbsuser.h"
#include "supercom.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static CStrList bbs_user;
extern int bbs_language_list(CStrList &);

static int	haschanged=0;
static int	remainchanged=0;
static CString savedlogin;
static char DlgName[]="IDD_BBS_USERS";

// =================================================================================
	bbsuser::bbsuser(CWnd *pParent ) : CSAPrefsSubDlg(bbsuser::IDD, pParent)
// =================================================================================
{
	//{{AFX_DATA_INIT(bbsuser)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

// =================================================================================
	void bbsuser::DoDataExchange(CDataExchange* pDX)
// =================================================================================
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(bbsuser)
	DDX_Control(pDX, IDC_EDIT_REMAIN, m_edit_remain);
	DDX_Control(pDX, IDC_LIST, m_list);
	DDX_Control(pDX, IDC_EDIT_TOWN, m_edit_town);
	DDX_Control(pDX, IDC_EDIT_TEL, m_edit_tel);
	DDX_Control(pDX, IDC_EDIT_STREET, m_edit_street);
	DDX_Control(pDX, IDC_EDIT_PASSWORD, m_edit_password);
	DDX_Control(pDX, IDC_EDIT_NAME, m_edit_name);
	DDX_Control(pDX, IDC_EDIT_MODEMFAX, m_edit_modemfax);
	DDX_Control(pDX, IDC_EDIT_LINES, m_edit_lines);
	DDX_Control(pDX, IDC_EDIT_GROUPS, m_edit_groups);
	DDX_Control(pDX, IDC_DD_LEVEL, m_dd_level);
	DDX_Control(pDX, IDC_DD_LANGUAGE, m_dd_language);
	DDX_Control(pDX, IDC_CHECK_ANSI, m_check_ansi);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(bbsuser, CDialog)
	//{{AFX_MSG_MAP(bbsuser)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_CHANGE, OnChange)
	ON_LBN_SELCHANGE(IDC_LIST, OnSelchangeList)
	ON_EN_CHANGE(IDC_EDIT_PASSWORD, OnChangeEditPassword)
	ON_EN_CHANGE(IDC_EDIT_REMAIN, OnChangeEditRemain)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// =====================================================================
LPCSTR bbsuser::GetName(void)	{return DlgName;}
// =====================================================================

// =================================================================================
	BOOL bbsuser::OnInitDialog()
// =================================================================================
{
int tabs[]={60,100,120,160,230,300,340,380,400,410,450,700};
//name,language,level,groups,street,town,tel,modem,linesper,ansi,crc,remain,lastlogin
int  lng[]={
			IDC_STATIC1,
			IDC_ADD,
			IDC_CHANGE,
			IDC_DELETE,
			IDC_STATIC2,
			IDC_STATIC9,
			IDC_STATIC3,
			IDC_STATIC4,
			IDC_STATIC5,
			IDC_STATIC6,
			IDC_STATIC8,
			IDC_CHECK_ANSI,
			IDC_STATIC11,
			IDC_STATIC10,
			IDC_STATIC7,
			IDC_STATIC12
			};
    CDialog::OnInitDialog();
    set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));
	m_list.SetTabStops((sizeof(tabs)/sizeof(int)),tabs);
	EXTENT_LB(IDC_LIST,1000);
	bbs_user.LoadFromFile("bbsuser.cfg",1);
	bbs_user.Sort(0);
	FillDropDowns();

	UPDATE_LB(bbs_user,IDC_LIST);
	SET_SELID(IDC_LIST,0);
	OnSelchangeList();
	return TRUE;
}

// =================================================================================
	int bbsuser::FillDropDowns(void)
// =================================================================================
{
CStrList	xx;
CString		help,help1;

	bbs_language_list(xx);

	m_dd_language.ResetContent();
	for (int i=0;i<xx.GetCount();i++)
	{
		help=xx.GetString(i);
		m_dd_language.AddString(help);
	}

	xx.LoadFromFile("bbsgrps.cfg");

	m_dd_level.ResetContent();
	for (int ii=0;ii<xx.GetCount();ii++)
	{
		help=xx.GetString(ii);
		get_token(help,0,help1);
		m_dd_level.AddString(help1);
	}
	return 1;
}

// =================================================================================
	int bbsuser::MakeString(CString &line)
// =================================================================================
{
CString name,crc,level,groups,street,town,tel,modem,linesper,ansi,language,remain;
char	lastlogin[300];
int     help,ret;
ULONG  newcrc;

	m_edit_name.GetWindowText(name);
	m_edit_password.GetWindowText(crc);

	if (haschanged)
	{
		newcrc=0;
		RS_GetCRC_CCITT32 (PS crc,crc.GetLength(),&newcrc);
		crc.Format("%08x",newcrc);
	}

	ret=m_dd_level.GetCurSel();
	if (ret==LB_ERR)
	{
		MessageBeep(0);
		return 0;
	}
	m_dd_level.GetLBText(ret,level);

	m_edit_groups.GetWindowText		(groups);
	m_edit_street.GetWindowText		(street);
	m_edit_town.GetWindowText		(town);
	m_edit_tel.GetWindowText		(tel);
	m_edit_modemfax.GetWindowText	(modem);
	m_edit_lines.GetWindowText		(linesper);
	m_edit_remain.GetWindowText		(remain);
	ansi=m_check_ansi.GetCheck() ? "1":"0";

	sscanf(remain,"%d",&help);
	help=help*60;
	remain.Format("%d",help);

	ret=m_dd_language.GetCurSel();
	if (ret==LB_ERR)
	{
		MessageBeep(0);
		return 0;
	}

	if (remainchanged)
		_strdate(lastlogin);
	else
		strcpy(lastlogin,savedlogin);

	m_dd_language.GetLBText(ret,language);
	line.Format("%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s",
				name,language,level,groups,street,town,tel,modem,linesper,ansi,
				crc,remain,lastlogin);
	return 1;
}



// =================================================================================
	int bbsuser::DispString(CString line)
// =================================================================================
{
CString name;
CString crc;
CString level;
CString groups;
CString street;
CString town;
CString tel;
CString modem;
CString linesper;
CString	ansi;
CString language;
CString help;
CString ln;
CString remain;
int     xhelp;

		get_token(PS line,0,name);
		get_token(PS line,10,crc);
		get_token(PS line,11,remain);
		get_token(PS line,2,level);
		get_token(PS line,3,groups);
		get_token(PS line,4,street);
		get_token(PS line,5,town);
		get_token(PS line,6,tel);
		get_token(PS line,7,modem);
		get_token(PS line,8,linesper);
		get_token(PS line,9,ansi);
		get_token(PS line,1,language);
		get_token(PS line,12,savedlogin);

		m_edit_name.SetWindowText(PS name);
		m_edit_password.SetWindowText(PS crc);

		m_dd_level.SetCurSel(0);
		for (int i=0;i<m_dd_level.GetCount();i++)
		{
			m_dd_level.GetLBText(i,ln);
			if (ln==level)
			{
				m_dd_level.SetCurSel(i);
				break;
			}
		}

		m_edit_groups.SetWindowText(PS groups);
		m_edit_street.SetWindowText(PS street);
		m_edit_town.SetWindowText(PS town);
		m_edit_tel.SetWindowText(PS tel);
		m_edit_modemfax.SetWindowText(PS modem);
		m_edit_lines.SetWindowText(PS linesper);
		m_check_ansi.SetCheck(ansi=="1");

		m_dd_language.SetCurSel(0);
		for (int ii=0;ii<m_dd_language.GetCount();ii++)
		{
			m_dd_language.GetLBText(ii,ln);
			if (ln==language)
			{
				m_dd_language.SetCurSel(ii);
				break;
			}
		}

		sscanf(PS remain,"%d",&xhelp);
		xhelp=xhelp/60;
		remain.Format("%d",xhelp);
		m_edit_remain.SetWindowText(PS remain);
		return 1;
}


// =================================================================================
	void bbsuser::OnAdd()
// =================================================================================
{
CString buf;

	if (!MakeString(buf))	return;
	bbs_user.AddTail(buf);
	UPDATE_LB(bbs_user,IDC_LIST);
}

// =================================================================================
	void bbsuser::OnDelete()
// =================================================================================
{
int sel;

	GET_SELID(IDC_LIST);
	bbs_user.Remove(sel);
	UPDATE_LB(bbs_user,IDC_LIST);
	sel=sel-1;
	if (sel<0)	sel=0;
	SET_SELID(IDC_LIST,sel);
}

// =================================================================================
	void bbsuser::OnChange()
// =================================================================================
{
int		sel;
CString buf;

	GET_SELID(IDC_LIST);
	if (!MakeString(buf))	return;
    bbs_user.Replace(sel,buf);
	UPDATE_LB(bbs_user,IDC_LIST);
	SET_SELID(IDC_LIST,sel);
}

// =================================================================================
	void bbsuser::OnOK()
// =================================================================================
{
	bbs_user.SaveToFile("bbsuser.cfg",-1);
	if(m_bCloseOnOk)
		CDialog::OnOK();
}

// =================================================================================
	void bbsuser::OnSelchangeList()
// =================================================================================
{
int		sel;
CString actual;

	GET_SELID(IDC_LIST);
	actual=bbs_user.GetString(sel);
	DispString(actual);
	haschanged=remainchanged=0;
}

// =================================================================================
	void bbsuser::OnChangeEditPassword()
// =================================================================================
{
	haschanged=1;
}

// =================================================================================
	void bbsuser::OnChangeEditRemain()
// =================================================================================
{
	remainchanged=1;
}

// =================================================================================
	void bbsuser::OnHelp()
// =================================================================================
{
	WinHelp(VHELP_BBS_USERS);
}

BOOL bbsuser::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return TRUE;
}
