// cfgmagic.cpp : implementation file
// IDD_CFG_MAGIC

#include "stdafx.h"
#include "resource.h"
#include "structs.h"
#include "cfgmagic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CStrList magics;
static char DlgName[]="IDD_CFG_MAGIC";
CStrList mg;

cfgmagic::cfgmagic(CWnd* pParent ) 	: CSAPrefsSubDlg(cfgmagic::IDD, pParent)
{
	//{{AFX_DATA_INIT(cfgmagic)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void cfgmagic::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(cfgmagic)
	DDX_Control(pDX, IDC_PATH, m_path);
	DDX_Control(pDX, IDC_PASSWORD, m_password);
	DDX_Control(pDX, IDC_MAGIC, m_magic);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(cfgmagic, CDialog)
	//{{AFX_MSG_MAP(cfgmagic)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_CHANGE, OnChange)
	ON_LBN_SELCHANGE(IDC_LIST, OnSelchangeList)
	ON_BN_CLICKED(IDC_EXPORT,OnExport)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// =====================================================================
LPCSTR cfgmagic::GetName(void)	{return DlgName;}
// =====================================================================

// =====================================================================
	BOOL cfgmagic::OnInitDialog()
// =====================================================================
{
int tabs[]={60,180,260,400,1900};
int lng[]={
			IDC_CHANGE,
			IDC_ADD,
			IDC_DELETE,
			IDC_STATIC1,
			IDC_STATIC2,
			IDC_STATIC3,
			IDC_BROWSE,
			IDC_EXPORT,
			};

	CDialog::OnInitDialog();
    set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));
	TABULATE_LB(IDC_LIST);
	EXTENT_LB(IDC_LIST,800);

	mg.LoadFromFile("magics.cfg");
	mg.Sort(0);
	UPDATE_LB(mg,IDC_LIST);
	SET_SELID(IDC_LIST,0);
	OnSelchangeList();
	return TRUE;
}

// ============================================================
	void cfgmagic::OnAdd()
// ============================================================
{
CString magic,psw,path,str;

	m_magic.GetWindowText(magic);
	m_password.GetWindowText(psw);
	m_path.GetWindowText(path);

    trim_all(magic);
    trim_all(psw);
    trim_all(path);

	if (!magic.GetLength() || !path.GetLength())	return;

	str.Format("%s\t%s\t%s\t",magic,path,psw);
	mg.AddTail(str);
	UPDATE_LB(mg,IDC_LIST);
	m_magic.SetFocus();
}

// ============================================================
	void cfgmagic::OnBrowse()
// ============================================================
{
CString	str;
int		ret;

	str.LoadString(IDS_TEXTFILT);
	CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY,str);
	ret=dlg.DoModal();
	restore_base_path();
	if (ret!=IDOK)	return;
	m_path.SetWindowText(dlg.GetPathName());
}

// ============================================================
	void cfgmagic::OnDelete()
// ============================================================
{
int sel,count;

	GET_SELID(IDC_LIST);
	mg.Remove(sel);
	UPDATE_LB(mg,IDC_LIST);
	count=mg.GetCount();
	if (count>0)
		SET_SELID(IDC_LIST,count>sel ? sel : (count-1));
}

// ============================================================
	void cfgmagic::OnChange()
// ============================================================
{
CString magic,psw,path,str;
int		sel;

	GET_SELID(IDC_LIST);
	m_magic.GetWindowText(magic);
	m_password.GetWindowText(psw);
	m_path.GetWindowText(path);

	str.Format("%s\t%s\t%s\t",magic,path,psw);
    mg.Replace(sel,str);
	UPDATE_LB(mg,IDC_LIST);
	SET_SELID(IDC_LIST,sel);
}

// ============================================================
	void cfgmagic::OnOK()
// ============================================================
{
	mg.SaveToFile("magics.cfg");
	magics.LoadFromFile("magics.cfg");
	if(m_bCloseOnOk)
		CDialog::OnOK();
}

// ============================================================
	void cfgmagic::OnSelchangeList()
// ============================================================
{
CString str,magic,psw,path;
int		sel;

	GET_SELID(IDC_LIST);
	str=mg.GetString(sel);
	get_token(str,0,magic);
	get_token(str,1,path);
	get_token(str,2,psw);

	m_path.SetWindowText(path);
	m_password.SetWindowText(psw);
	m_magic.SetWindowText(magic);
}

// ============================================================
//magic filename !psw
	void cfgmagic::OnExport()
// ============================================================
{
CString path,magic,str,psw;
FILE	*fp;
int		ret;

	str.LoadString(IDS_TEXTFILT);
	CFileDialog dlg(FALSE,NULL,NULL,OFN_HIDEREADONLY,str);
	ret=dlg.DoModal();
	restore_base_path();
	if (ret!=IDOK)	return;
	path=dlg.GetPathName();
	fp=fopen(path,"wt");
	if (fp)
	{
		for (int i=0;i<mg.GetCount();i++)
		{
			str=mg.GetString(i);
			get_token(str,0,magic);
			get_token(str,1,path);
			get_token(str,2,psw);
			if (psw.GetLength()>1)
				psw="!"+psw;
			fprintf(fp,"%-20s %-55s %s\n",magic,path,psw);
		}
		fclose(fp);
		show_msg(L("S_176"));
	}
	else
		show_msg(L("S_237"));
}

// ============================================================
	void cfgmagic::OnHelp()
// ============================================================
{
	WinHelp(VHELP_CFG_REQPATH);
}

BOOL cfgmagic::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return TRUE;
}
