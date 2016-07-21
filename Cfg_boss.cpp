// cfg_boss.cpp : implementation file
// IDD_CFG_BOSS

#include "stdafx.h"
#include "resource.h"
#include "structs.h"
#include "cfg_boss.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern CStrList Bosses;
extern _packer Packers[];
static char DlgName[]="IDD_CFG_BOSS";
CStrList bosslst;

/////////////////////////////////////////////////////////////////////////////
// cfg_boss dialog

cfg_boss::cfg_boss(CWnd* pParent /*=NULL*/)
	: CSAPrefsSubDlg(cfg_boss::IDD, pParent)
{
	//{{AFX_DATA_INIT(cfg_boss)
	m_afix_name = _T("");
	m_afix_password = _T("");
	m_afix_plus = FALSE;
	m_bossname = _T("");
	m_echofilename = _T("");
	m_ffix_name = _T("");
	m_ffix_password = _T("");
	m_ffix_plus = FALSE;
	m_fidoaddress = _T("");
	m_fl_packedname = _T("");
	m_fl_unpackedname = _T("");
	m_mypointnumber = _T("");
	m_new_packedname = _T("");
	m_new_unpackedname = _T("");
	m_phonenumber = _T("");
	m_sessionpassword = _T("");
	m_sysopname = _T("");
	m_arc_password = _T("");
	m_routing = _T("");
	m_nopoll = FALSE;
	m_nomail = FALSE;
	//}}AFX_DATA_INIT
}


void cfg_boss::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(cfg_boss)
	DDX_Control(pDX, IDC_TCPIP, m_tcpip);
	DDX_Control(pDX, IDC_PACKERTYPE, m_packertype);
	DDX_Control(pDX, IDC_ZMODEM, m_zmod);
	DDX_Control(pDX, IDC_HYDRA, m_hydra);
	DDX_Text(pDX, IDC_AFIX_NAME, m_afix_name);
	DDV_MaxChars(pDX, m_afix_name, 12);
	DDX_Text(pDX, IDC_AFIX_PASSWORD, m_afix_password);
	DDV_MaxChars(pDX, m_afix_password, 8);
	DDX_Check(pDX, IDC_AFIX_PLUS, m_afix_plus);
	DDX_Text(pDX, IDC_BOSSNAME, m_bossname);
	DDX_Text(pDX, IDC_ECHOFILENAME, m_echofilename);
	DDV_MaxChars(pDX, m_echofilename, 12);
	DDX_Text(pDX, IDC_FFIX_NAME, m_ffix_name);
	DDV_MaxChars(pDX, m_ffix_name, 12);
	DDX_Text(pDX, IDC_FFIX_PASSWORD, m_ffix_password);
	DDV_MaxChars(pDX, m_ffix_password, 8);
	DDX_Check(pDX, IDC_FFIX_PLUS, m_ffix_plus);
	DDX_Text(pDX, IDC_FIDOADDRESS, m_fidoaddress);
	DDX_Text(pDX, IDC_FL_PACKEDNAME, m_fl_packedname);
	DDV_MaxChars(pDX, m_fl_packedname, 12);
	DDX_Text(pDX, IDC_FL_UNPACKEDNAME, m_fl_unpackedname);
	DDV_MaxChars(pDX, m_fl_unpackedname, 12);
	DDX_Text(pDX, IDC_MYPOINTNUMBER, m_mypointnumber);
	DDX_Text(pDX, IDC_NEW_PACKEDNAME, m_new_packedname);
	DDV_MaxChars(pDX, m_new_packedname, 12);
	DDX_Text(pDX, IDC_NEW_UNPACKEDNAME, m_new_unpackedname);
	DDV_MaxChars(pDX, m_new_unpackedname, 12);
	DDX_Text(pDX, IDC_PHONENUMBER, m_phonenumber);
	DDX_Text(pDX, IDC_SESSIONPASSWORD, m_sessionpassword);
	DDV_MaxChars(pDX, m_sessionpassword, 8);
	DDX_Text(pDX, IDC_SYSOPNAME, m_sysopname);
	DDX_Text(pDX, IDC_ARCFILENAME, m_arc_password);
	DDV_MaxChars(pDX, m_arc_password, 8);
	DDX_Text(pDX, IDC_ROUTE, m_routing);
	DDV_MaxChars(pDX, m_routing, 200);
	DDX_Check(pDX, IDC_NOPOLL, m_nopoll);
	DDX_Check(pDX, IDC_NOMAIL, m_nomail);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(cfg_boss, CDialog)
	//{{AFX_MSG_MAP(cfg_boss)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_LBN_SELCHANGE(IDC_LIST, OnSelchangeList)
	ON_BN_CLICKED(IDC_CHANGE, OnChange)
	ON_EN_KILLFOCUS(IDC_MYPOINTNUMBER, OnKillFocusPoint)
	ON_BN_CLICKED(IDC_HYDRA, OnHydra)
	ON_BN_CLICKED(IDC_ZMODEM, OnZmodem)
	ON_BN_CLICKED(IDC_TCPIP, OnBinkD)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// cfg_boss message handlers

// =====================================================================
	LPCSTR cfg_boss::GetName(void)	{return DlgName;}
// =====================================================================

// ==========================================================
	BOOL cfg_boss::OnInitDialog()
// ==========================================================
{
int	 tabs[]={100,140,180,400,1900};
char buf[MAX_BOSSLEN];
int	 i;
int  lng[]={
			IDC_AFIX_PLUS,
			IDC_FFIX_PLUS,
			IDC_CHANGE,
			IDC_ADD,
			IDC_DELETE,
			IDC_STATIC3,
			IDC_STATIC5,
			IDC_STATIC6,
			IDC_STATIC7,
			IDC_STATIC8,
			IDC_STATIC22,
			IDC_STATIC21,
			IDC_STATIC15,
			IDC_STATIC10,
			IDC_STATIC16,
			IDC_STATIC9,
			IDC_STATIC24,
			IDC_STATIC25,
			IDC_STATIC23,
			IDC_STATIC14,
			IDC_STATIC18,
			IDC_STATIC19,
			IDC_STATIC17,
			IDC_STATIC20,
			IDC_STATIC2,
			IDC_STATIC1,
			IDC_STATIC4,
			IDC_STATIC11,
			IDC_STATIC12,
			IDC_NOPOLL,
			IDC_NOMAIL,
			IDC_STATIC88,
			IDC_HYDRA,
			IDC_ZMODEM
			};

	CDialog::OnInitDialog();
    set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));
	TABULATE_LB(IDC_LIST);
// read current available packer types
	m_packertype.ResetContent();
	i=0;
	while (Packers[i].name[0])
		m_packertype.AddString(strupr(Packers[i++].name));

	bosslst.RemoveAll();
	i=0;
	while (db_get_boss_by_index(i++,buf))
		 bosslst.AddTail(buf);

	UPDATE_LB(bosslst,IDC_LIST);
	SET_SELID(IDC_LIST,0);
	show_selection();
	return TRUE;
}

// ==========================================================
	void cfg_boss::OnOK()
// ==========================================================
{
	Bosses.RemoveAll();
	for (int i=0;i<bosslst.GetCount();i++)
		Bosses.AddTail(bosslst.GetString(i));

	del_cfg(CFG_BOSSES);
	save_bosslist(Bosses);
	if(m_bCloseOnOk)
		CDialog::OnOK();
}

// ==========================================================
	void cfg_boss::MakeString(CString &str)
// ==========================================================
{
CString tmp;

	UpdateData(1);
    trim_all(m_bossname);
    trim_all(m_fidoaddress);
    trim_all(m_phonenumber);
    trim_all(m_sysopname);
    trim_all(m_mypointnumber);
    trim_all(m_sessionpassword);
	m_packertype.GetWindowText(tmp);
    trim_all(tmp);
    trim_all(m_echofilename);
    trim_all(m_fl_packedname);
    trim_all(m_fl_unpackedname);
    trim_all(m_new_packedname);
    trim_all(m_new_unpackedname);
    trim_all(m_afix_name);
    trim_all(m_afix_password);
    trim_all(m_ffix_name);
    trim_all(m_ffix_password);
    trim_all(m_arc_password);
    trim_all(m_routing);

	str.Format("%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t"
		"%s\t%s\t%s\t%s\t%d\t%s\t%s\t%d\t%s\t%s\t%d\t%d\t%d\t%d\t%d",
		m_bossname,m_fidoaddress,m_phonenumber,m_sysopname,m_mypointnumber,
		m_sessionpassword,tmp,m_echofilename,m_fl_packedname,m_fl_unpackedname,
		m_new_packedname,m_new_unpackedname,m_afix_name,m_afix_password,m_afix_plus,
		m_ffix_name,m_ffix_password,m_ffix_plus,m_arc_password,m_routing,m_nopoll,
		m_nomail,m_hydra.GetCheck(),m_zmod.GetCheck(),m_tcpip.GetCheck());
}

// ==========================================================
	void cfg_boss::OnAdd()
// ==========================================================
{
CString buf;

	MakeString(buf);
	bosslst.AddTail(buf);
	UPDATE_LB(bosslst,IDC_LIST);
}

// ==========================================================
	void cfg_boss::OnDelete()
// ==========================================================
{
int sel;

	GET_SELID(IDC_LIST);
	bosslst.Remove(sel);
	UPDATE_LB(bosslst,IDC_LIST);
}

// ==========================================================
	void cfg_boss::OnChange()
// ==========================================================
{
CString buf;
int		sel;

	GET_SELID(IDC_LIST);
	MakeString(buf);
	bosslst.Replace(sel,buf);
	UPDATE_LB(bosslst,IDC_LIST);
	SET_SELID(IDC_LIST,sel);
}

// ==========================================================
	void cfg_boss::show_selection()
// ==========================================================
{
CString str,tmp;
int		sel;

	GET_SELID(IDC_LIST);
	str=bosslst.GetString(sel);

	get_token(str,BOSSSYST,m_bossname);
	get_token(str,BOSSADDR,m_fidoaddress);
	get_token(str,BOSSPHON,m_phonenumber);
	get_token(str,BOSSNAME,m_sysopname);
	get_token(str,MAINAKA,m_mypointnumber);
	get_token(str,SESSPASW,m_sessionpassword);
	get_token(str,ARCHIVER,tmp);
	m_packertype.SelectString(-1,tmp);
	get_token(str,ECHOFILE,m_echofilename);
	get_token(str,FLISTARC,m_fl_packedname);
	get_token(str,FLISTUNA,m_fl_unpackedname);
	get_token(str,NFLSTARC,m_new_packedname);
	get_token(str,NFLSTUNA,m_new_unpackedname);
	get_token(str,AREAFIX,m_afix_name);
	get_token(str,AFIXPASW,m_afix_password);
	m_afix_plus=get_token_int(str,AFIXPLUS);
	get_token(str,FILEFIX,m_ffix_name);
	get_token(str,FFIXPASW,m_ffix_password);
	m_ffix_plus=get_token_int(str,FFIXPLUS);
	get_token(str,ARCMPASW,m_arc_password);
	get_token(str,ROUTING,m_routing);
	m_nopoll=get_token_int(str,NOPOLL);
	m_nomail=get_token_int(str,NOTSEND);
	m_hydra.SetCheck(get_token_int(str,NOHYDRA));
	m_zmod.SetCheck(get_token_int(str,NOZMODEM));
	m_tcpip.SetCheck(get_token_int(str,TCPIP));
	UpdateData(0);
}

// ==========================================================
	void cfg_boss::OnCancel()
// ==========================================================
{
	CDialog::OnCancel();
}

// ==========================================================
	void cfg_boss::OnSelchangeList()
// ==========================================================
{
	show_selection();
}

// ==========================================================
	void cfg_boss::OnKillFocusPoint()
// ==========================================================
{
	 UpdateData(1);
	 if (!strchr(m_mypointnumber,'.'))
	 {
		 m_mypointnumber=m_fidoaddress+"."+m_mypointnumber;
		 UpdateData(0);
	 }
}

// ==========================================================
	void cfg_boss::OnHydra() 
// ==========================================================
{
	if (m_hydra.GetCheck() && m_zmod.GetCheck())
		m_zmod.SetCheck(0);
	m_tcpip.SetCheck(0);
}

// ==========================================================
	void cfg_boss::OnZmodem() 
// ==========================================================
{
	if (m_hydra.GetCheck() && m_zmod.GetCheck())
		m_hydra.SetCheck(0);
	m_tcpip.SetCheck(0);
}

// ==========================================================
	void cfg_boss::OnBinkD() 
// ==========================================================
{
	if (m_tcpip.GetCheck())
	{
		m_zmod.SetCheck(1);
		m_hydra.SetCheck(1);
	}
	else
	{
		m_zmod.SetCheck(0);
		m_hydra.SetCheck(1);
	}
}

// ==========================================================
	void cfg_boss::OnHelp()
// ==========================================================
{
	WinHelp(VHELP_BOSS_CONFIG);
}

// ==========================================================
	BOOL cfg_boss::OnHelpInfo(HELPINFO* pHelpInfo) 
// ==========================================================
{
	OnHelp();
	return TRUE;
}
