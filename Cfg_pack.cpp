// cfg_pack.cpp : implementation file
// IDD_CFG_PACKER

#include "stdafx.h"
#include "light.h"
#include "cfg_pack.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// cfg_pack dialog

BOOL verify_hexbytes(LPCSTR str);
BOOL verify_command(LPCSTR str);
void save_packlist(CStrList &lst);

static char DlgName[]="IDD_CFG_PACKER";

extern CLightApp FipsApp;
extern _packer	 Packers[];
extern HINSTANCE hUni;

CStrList packlist;
CString overwrite;
CString gstr;

cfg_pack::cfg_pack(CWnd* pParent /*=NULL*/)
	: CSAPrefsSubDlg(cfg_pack::IDD, pParent)
{
	//{{AFX_DATA_INIT(cfg_pack)
	m_unpack = _T("");
	m_pack = _T("");
	m_name = _T("");
	m_p1 = _T("");
	m_p10 = _T("");
	m_p2 = _T("");
	m_p3 = _T("");
	m_p4 = _T("");
	m_p5 = _T("");
	m_p6 = _T("");
	m_p7 = _T("");
	m_p8 = _T("");
	m_p9 = _T("");
	//}}AFX_DATA_INIT
}

void cfg_pack::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(cfg_pack)
	DDX_Control(pDX, IDC_BUILTIN, m_builtin);
	DDX_Control(pDX, IDC_LIST, m_list);
	DDX_Text(pDX, IDC_UNPACKCMD, m_unpack);
	DDX_Text(pDX, IDC_PACKCMD, m_pack);
	DDX_Text(pDX, IDC_NAME, m_name);
	DDX_Text(pDX, IDC_P1, m_p1);
	DDV_MaxChars(pDX, m_p1, 2);
	DDX_Text(pDX, IDC_P10, m_p10);
	DDV_MaxChars(pDX, m_p10, 2);
	DDX_Text(pDX, IDC_P2, m_p2);
	DDV_MaxChars(pDX, m_p2, 2);
	DDX_Text(pDX, IDC_P3, m_p3);
	DDV_MaxChars(pDX, m_p3, 2);
	DDX_Text(pDX, IDC_P4, m_p4);
	DDV_MaxChars(pDX, m_p4, 2);
	DDX_Text(pDX, IDC_P5, m_p5);
	DDV_MaxChars(pDX, m_p5, 2);
	DDX_Text(pDX, IDC_P6, m_p6);
	DDV_MaxChars(pDX, m_p6, 2);
	DDX_Text(pDX, IDC_P7, m_p7);
	DDV_MaxChars(pDX, m_p7, 2);
	DDX_Text(pDX, IDC_P8, m_p8);
	DDV_MaxChars(pDX, m_p8, 2);
	DDX_Text(pDX, IDC_P9, m_p9);
	DDV_MaxChars(pDX, m_p9, 2);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(cfg_pack, CDialog)
	//{{AFX_MSG_MAP(cfg_pack)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_CHANGE, OnChange)
	ON_LBN_SELCHANGE(IDC_LIST, OnSelchangeList)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_DEFAULT, OnDefault)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// cfg_pack message handlers

// =====================================================================
LPCSTR cfg_pack::GetName(void)	{return DlgName;}
// =====================================================================

// ============================================================================
	BOOL cfg_pack::OnInitDialog()
// ============================================================================
{
CString	str;
char tmp[10];
int tabs[]={50,150,900};
int i;
int lng[]={
			IDC_CHANGE,
			IDC_ADD,
			IDC_DELETE,
			IDC_DEFAULT,
			IDC_STATIC3,
			IDC_STATIC4,
			IDC_STATIC2,
			IDC_STATIC1,
			IDC_STATIC6,
			IDC_STATIC5,
			IDC_BUILTIN
			};

	CDialog::OnInitDialog();
    set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));

	i=0;
	packlist.RemoveAll();
	while (Packers[i].name[0])
	{
		str.Format("%s\t%s\t%s",Packers[i].name,Packers[i].pack_cmd,Packers[i].upck_cmd);
		for (int k=0;k<10;k++)
		{
			str+="\t";
			*tmp=0;
			if (Packers[i].sign[k]>=0)
				sprintf(tmp,"%02X",Packers[i].sign[k]);
			str+=tmp;
		}
		packlist.AddTail(str);
		i++;
	}
	if (!packlist.GetCount())
	   OnDefault();

	m_builtin.SetCheck(hUni!=0 ? get_cfg(CFG_TOSSER,"EnableBuiltinArc",0) : 0);
	m_builtin.EnableWindow(hUni!=0);
	UPDATE_LB(packlist,IDC_LIST);
	TABULATE_LB(IDC_LIST);
	m_list.SetCurSel(0);
	OnSelchangeList();
	return TRUE;
}

// ============================================================================
	void cfg_pack::OnAdd()
// ============================================================================
{
	make_string();
	if (!verify_hexbytes(gstr) || !verify_command(gstr))
		return;
	packlist.AddTail(gstr);
	UPDATE_LB(packlist,IDC_LIST);
	SET_SELID(IDC_LIST,m_list.GetCount()-1);
}

// ============================================================================
	void cfg_pack::OnChange()
// ============================================================================
{
CString str;
int		sel;

	GET_SELID(IDC_LIST);
	str=packlist.GetString(sel);
	get_token(str,0,overwrite);

	make_string();
	if (!verify_hexbytes(gstr) || !verify_command(gstr))
		return;
	overwrite.Empty();
	packlist.Replace(sel,gstr);
	UPDATE_LB(packlist,IDC_LIST);
	SET_SELID(IDC_LIST,sel);
}

// ============================================================================
	void cfg_pack::OnOK()
// ============================================================================
{
	UpdateData(1);
	save_packlist(packlist);
	set_cfg(CFG_TOSSER,"EnableBuiltinArc",m_builtin.GetCheck());
	if(m_bCloseOnOk)	CDialog::OnOK();
}

// ============================================================================
	void cfg_pack::make_string(void)
// ============================================================================
{
	UpdateData(1);
	if (overwrite.GetLength()>0)
	   m_name=overwrite;

	gstr=m_name+"\t"+m_pack+"\t"+m_unpack+"\t"+m_p1+"\t"+m_p2+"\t"+m_p3+"\t"+m_p4+"\t"
		+m_p5+"\t"+m_p6+"\t"+m_p7+"\t"+m_p8+"\t"+m_p9+"\t"+m_p10+"\t";
}

// ============================================================================
	void cfg_pack::OnSelchangeList()
// ============================================================================
{
CString str;
int		sel;

	GET_SELID(IDC_LIST);
	str=packlist.GetString(sel);
	get_token(str,0,m_name);
	get_token(str,1,m_pack);
	get_token(str,2,m_unpack);
	get_token(str,3,m_p1);
	get_token(str,4,m_p2);
	get_token(str,5,m_p3);
	get_token(str,6,m_p4);
	get_token(str,7,m_p5);
	get_token(str,8,m_p6);
	get_token(str,9,m_p7);
	get_token(str,10,m_p8);
	get_token(str,11,m_p9);
	get_token(str,12,m_p10);
	UpdateData(0);
}

// ============================================================================
	void cfg_pack::OnDelete()
// ============================================================================
{
int		sel;

	GET_SELID(IDC_LIST);
    packlist.Remove(sel);
	UPDATE_LB(packlist,IDC_LIST);

	if (m_list.GetCount()>=sel)
	   sel--;
	if (sel<0)
	   sel=0;

	SET_SELID(IDC_LIST,sel);
}

// ============================================================================
	void cfg_pack::OnDefault()
// ============================================================================
{
	get_packlist(TRUE);
	UPDATE_LB(packlist,IDC_LIST);
	if (packlist.GetCount()>0)
		SET_SELID(IDC_LIST,0);
	OnSelchangeList();
}

// ============================================================================
	void cfg_pack::OnHelp()
// ============================================================================
{
	WinHelp(VHELP_CFG_PACKER);
}

// ============================================================================
	BOOL cfg_pack::OnHelpInfo(HELPINFO* pHelpInfo) 
// ============================================================================
{
	OnHelp();
	return TRUE;
}

// ============================================================================
	BOOL verify_hexbytes(LPCSTR str)
// ============================================================================
{
char ch[100];

	for (int i=3;i<13;i++)
	{
		get_token(str,i,ch);
		if (*ch)
		{
			ch[2]=0;
			if (strspn(ch,"0123456789ABCDEF")!=2)
				ERR_MSG_RET0("E_NOTHEX");
		}
	}
	return 1;
}

// ============================================================================
	BOOL verify_command(LPCSTR str)
// ============================================================================
{
char tmp1[100],tmp2[100];

	get_token(str,0,tmp1);
	get_token(str,1,tmp2);
	if (!strstr(tmp2,"%f") || !strstr(tmp2,"%p"))
	{
		err_out("E_WRPACKCFG",tmp1,tmp2);
		return 0;
	}
	get_token(str,2,tmp2);
	if (!strstr(tmp2,"%f") || !strstr(tmp2,"%p"))
	{
		err_out("E_WRPACKCFG",tmp1,tmp2);
		return 0;
	}
	return 1;
}

// ===============================================================
// saves list of packers to fips.ini
	void save_packlist(CStrList &lst)
// ===============================================================
{
char buf[MAX_PACKLEN];

	make_inistr(lst,buf);
	WritePrivateProfileSection(get_sect_name(CFG_PACKERS),buf,FipsApp.m_pszProfileName);
}
