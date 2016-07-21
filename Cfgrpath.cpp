// cfgrpath.cpp : implementation file
// IDD_CFG_REQPATH

#include "stdafx.h"
#include "resource.h"
#include "structs.h"
#include "cfgrpath.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CStrList rpathes;
static char DlgName[]="IDD_CFG_REQPATH";
CStrList rp;

// ============================================================
	cfgrpath::cfgrpath(CWnd* pParent ) 	: CSAPrefsSubDlg(cfgrpath::IDD, pParent)
// ============================================================
{
	//{{AFX_DATA_INIT(cfgrpath)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

// ============================================================
	void cfgrpath::DoDataExchange(CDataExchange* pDX)
// ============================================================
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(cfgrpath)
	DDX_Control(pDX, IDC_EDIT_PATH, m_edit_path);
	DDX_Control(pDX, IDC_EDIT_PASSWORD, m_edit_password);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(cfgrpath, CDialog)
	//{{AFX_MSG_MAP(cfgrpath)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_CHANGE, OnChange)
	ON_BN_CLICKED(IDC_IMPORT, OnImport)
	ON_LBN_SELCHANGE(IDC_LIST, OnSelchangeList)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// =====================================================================
LPCSTR cfgrpath::GetName(void)	{return DlgName;}
// =====================================================================

// ============================================================
	BOOL cfgrpath::OnInitDialog()
// ============================================================
{
int tabs[]={160,390};
int  lng[]={
			IDC_CHANGE,
			IDC_ADD,
			IDC_DELETE,
			IDC_STATIC2,
			IDC_STATIC3,
			IDC_BROWSE,
			IDC_STATIC1,
			IDC_IMPORT,
			};

	CDialog::OnInitDialog();
    set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));

	TABULATE_LB(IDC_LIST);
	EXTENT_LB(IDC_LIST,800);

	rp.LoadFromFile("rpathes.cfg");
	rp.Sort(0);
	UPDATE_LB(rp,IDC_LIST);
	SET_SELID(IDC_LIST,0);
	OnSelchangeList();
	return TRUE;
}


// ============================================================
	void cfgrpath::OnAdd()
// ============================================================
{
CString psw,path,str;

	m_edit_password.GetWindowText(psw);
	m_edit_path.GetWindowText(path);

    trim_all(psw);
    trim_all(path);

	if (path.GetLength()<1)
	{
		MessageBeep(0);
		return;
	}

	str.Format("%s\t%s\t",path,psw);
	rp.AddTail(str);
	UPDATE_LB(rp,IDC_LIST);
	m_edit_path.SetFocus();
}

// ============================================================
	void cfgrpath::OnBrowse()
// ============================================================
{
CString path;

	if (GetDirectory(path,m_hWnd))
		m_edit_path.SetWindowText(path);
}

// ============================================================
	void cfgrpath::OnDelete()
// ============================================================
{
int sel;

	GET_SELID(IDC_LIST);
	rp.Remove(sel);
	UPDATE_LB(rp,IDC_LIST);
	if (rp.GetCount()>0)
	{
		if (rp.GetCount()<=sel)
			sel=rp.GetCount()-1;
		SET_SELID(IDC_LIST,sel);
	}
}

// ============================================================
	void cfgrpath::OnChange()
// ============================================================
{
CString psw,path,str;
int		sel;

	GET_SELID(IDC_LIST);
	m_edit_password.GetWindowText(psw);
	m_edit_path.GetWindowText(path);
	str.Format("%s\t%s\t",path,psw);
    rp.Replace(sel,str);
	UPDATE_LB(rp,IDC_LIST);
	SET_SELID(IDC_LIST,sel);
}

// ============================================================
	void cfgrpath::OnOK()
// ============================================================
{
	rp.SaveToFile("rpathes.cfg");
	rpathes.LoadFromFile("rpathes.cfg");
	if(m_bCloseOnOk)
		CDialog::OnOK();
}

// ============================================================
	void cfgrpath::OnSelchangeList()
// ============================================================
{
CString str,psw,path;
int		sel;

	GET_SELID(IDC_LIST);
	str=rp.GetString(sel);
	get_token(str,0,path);
	get_token(str,1,psw);
	m_edit_path.SetWindowText(path);
	m_edit_password.SetWindowText(psw);
}

// ============================================================
	void cfgrpath::OnImport()
// ============================================================
// Imports Pathes from TIC and BBS Areas into FileReq-Pathes setup
// Only those Pathes are imported, which are not already present.
{
CStrList ticareas,bbsareas;
int		t,i,found;
char	tmp[300];
CString str,mtmp;

		if (err_out("DN_IMPBBSTIC") != IDYES)	return;
		ticareas.LoadFromFile("tics.cfg");
		bbsareas.LoadFromFile("bbsfare.cfg");
//		echo,uplink,description,dirpath,m_dontdelete

		// Get Tic-Areas
		for (t=0;t<ticareas.GetCount();t++)
		{
			str = ticareas.GetString(t);
			get_token(str,3,mtmp);
			strcpy(tmp,mtmp);
			addbackslash(tmp);
			found =0;
			for (i=0;i<rp.GetCount();i++)
			{
				char tmp2[200];
				CString stmp;

				stmp = rp.GetString(i);
				get_token(stmp,0,stmp);
				strcpy(tmp2,stmp);
				addbackslash(tmp2);
				if (strcmpi(tmp2,tmp) == 0)
				{
					found = 1 ;
					break;
				}
			}

			if (!found && mtmp.GetLength() >0)
			{
				CString mydum;
				mydum.Format("%s\t\t",mtmp);
				rp.AddTail(mydum);
				UPDATE_LB(rp,IDC_LIST);
			}
		}

//	pathforfiles,description,level,groups,filesbbs

		// Get BBS-Areas
		for (t=0;t<bbsareas.GetCount();t++)
		{
			str = bbsareas.GetString(t);
			get_token(str,0,mtmp);
			strcpy(tmp,mtmp);
			addbackslash(tmp);
			found =0;
			for (i=0;i<rp.GetCount();i++)
			{
				char tmp2[200];
				CString stmp;

				stmp = rp.GetString(i);
				get_token(stmp,0,stmp);
				strcpy(tmp2,stmp);
				addbackslash(tmp2);
				if (strcmpi(tmp2,tmp) == 0)
				{
					found = 1 ;
					break;
				}
			}

			if (!found && mtmp.GetLength() >0)
			{
				CString mydum;
				mydum.Format("%s\t\t",mtmp);
				rp.AddTail(mydum);
				UPDATE_LB(rp,IDC_LIST);
			}
		}
}

// ============================================================
	void cfgrpath::OnHelp()
// ============================================================
{
	WinHelp(VHELP_CFG_REQPATH);
}

BOOL cfgrpath::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return TRUE;
}
