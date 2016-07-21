// IDD_ADD_FILELIST

#include "stdafx.h"
#include <direct.h>
#include "cfg_nfll.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern _gconfig	gc;
static char DlgName[]="IDD_ADD_FILELIST";

// ========================================================================
	cfg_nfll::cfg_nfll(CWnd* pParent) : CDialog(cfg_nfll::IDD, pParent)
// ========================================================================
{
	//{{AFX_DATA_INIT(cfg_nfll)
	//}}AFX_DATA_INIT
}

// ========================================================================
	void cfg_nfll::DoDataExchange(CDataExchange* pDX)
// ========================================================================
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(cfg_nfll)
	DDX_Control(pDX, IDC_FPATH, m_path);
	DDX_Control(pDX, IDC_DIRECT, m_direct);
	DDX_Control(pDX, IDC_LIST2, m_extract);
	DDX_Control(pDX, IDC_LIST1, m_dir);
	DDX_Control(pDX, IDC_EDIT1, m_fido);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(cfg_nfll, CDialog)
	//{{AFX_MSG_MAP(cfg_nfll)
	ON_BN_CLICKED(IDC_EXTRACT, OnExtract)
	ON_LBN_DBLCLK(IDC_LIST1, OnDblclkList1)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_BN_CLICKED(IDC_SEARCH, OnSearch)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_BN_CLICKED(IDC_FLIST_PATH, OnFlistPath)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// ========================================================================
	BOOL cfg_nfll::OnInitDialog()
// ========================================================================
{
CStrList	dir;
CString		tmp;
char		path[MAX_PATH];
int  lng[]={
			IDC_EXTRACT,
			IDC_SEARCH,
			IDOK,
			IDC_STATIC1,
			IDC_STATIC2,
			IDCANCEL,
			IDHELP,
			IDC_STATIC3
			};

	CDialog::OnInitDialog();
    set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));
	tmp=get_cfg(CFG_COMMON,"FilelistPath",gc.InboundPath);
	m_path.SetWindowText(tmp);
	make_path(path,tmp,"*.*");
	dir.FillWithFiles(path);
	UPDATE_LB(dir,IDC_LIST1);
	return TRUE;
}

// ========================================================================
	void cfg_nfll::OnOK()
// ========================================================================
{
CString ename,fido,brname;
char	fidoname[100],utildir[MAX_PATH],srcname[MAX_PATH];
int		sel,zone,net,node,point;

	m_direct.GetWindowText(brname);
	if (brname.GetLength()==0)
	{
		sel=m_extract.GetCurSel();
		LB_ERR_RET;
		m_extract.GetText(sel,ename);
	}
	else
	{
		sel=m_extract.GetCurSel();
		if (sel!=LB_ERR)
			ERR_MSG_RET("E_BOTHDAEX");
		ename=brname;
	}

	m_fido.GetWindowText(fido);
	if (parse_address(fido,&zone,&net,&node,&point)<3)
		ERR_MSG2_RET("E_NOT_FCOR",fido);

	make_fidodir(fidoname,zone,net,node,point);
	make_path(utildir,gc.UtilPath,fidoname);
	strcat(utildir,".filelist");
	make_path(srcname,gc.BasePath,"tmp\\flst\\");
	strcat(srcname,ename);

	if (brname.GetLength()>0)
		strcpy(srcname,brname);

	CopyFile(srcname,utildir,0);
	prepare_tmp_dir();
	CDialog::OnOK();
}


// ========================================================================
	void cfg_nfll::OnExtract()
// ========================================================================
{
CStrList dir;
CString text;
CString path;
char	tmp[MAX_PATH];
char	fname[MAX_PATH];
int		sel;

	CDialog::OnInitDialog();
	sel=m_dir.GetCurSel();
	LB_ERR_RET;
	m_dir.GetText(sel,text);
	prepare_tmp_dir();
	m_path.GetWindowText(path);
	make_path(fname,path,text);
	make_path(tmp,gc.BasePath,"tmp\\flst");
	if (!extract_file(fname,tmp))	return;

	strcat(tmp,"\\*.*");
	dir.FillWithFiles(tmp);
	UPDATE_LB(dir,IDC_LIST2);
}

// ========================================================================
	void prepare_tmp_dir()
// ========================================================================
{
CStrList lst;
char	tmp[MAX_PATH],tmp1[MAX_PATH];

	make_path(tmp,gc.BasePath,"tmp");
	mkdir(tmp);
	make_path(tmp1,tmp,"flst");
	mkdir(tmp1);
 	make_path(tmp,tmp1,"*.*");

	lst.RemoveAll();
	lst.FillWithFiles(tmp);
 	for (int i=0;i<lst.GetCount();i++)
	{
		make_path(tmp,tmp1,lst.GetString(i));
		SafeFileDelete(tmp);
	}
}

// ========================================================================
	void cfg_nfll::OnCancel()
// ========================================================================
{
	prepare_tmp_dir();
	CDialog::OnCancel();
}

// ========================================================================
	void cfg_nfll::OnDblclkList1()
// ========================================================================
{
	OnExtract();
}

// ========================================================================
	void cfg_nfll::OnBrowse()
// ========================================================================
{
CString	str;
int		ret;

	str.LoadString(IDS_TEXTFILT);
	CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY,str);
	ret=dlg.DoModal();
	restore_base_path();
	if (ret!=IDOK)	return;
	m_direct.SetWindowText(dlg.GetPathName());
}

// ========================================================================
	void cfg_nfll::OnSearch() 	  // Try to expand a incomplete Fido Adress
// ========================================================================
{
CString fido,info,full;
int     err,ret;

	m_fido.GetWindowText(fido);
	ret=expand_address(fido,full,info,1,1,0,err);
	if (ret)
		m_fido.SetWindowText(full);
	else
		if (err!=7)	// EXP_NO_USERSEL
			ERR_MSG2_RET("E_CANBCFA",fido);
}

// ========================================================================
	void cfg_nfll::OnFlistPath() 
// ========================================================================
{
CStrList dir;
CString  path;
char	 tmp[MAX_PATH];

	if (GetDirectory(path,m_hWnd))
	{
		m_path.SetWindowText(path);
		set_cfg(CFG_COMMON,"FilelistPath",path);
		make_path(tmp,path,"*.*");
		dir.FillWithFiles(tmp);
		UPDATE_LB(dir,IDC_LIST1);
	}
}
	
// ========================================================================
	void cfg_nfll::OnHelp()
// ========================================================================
{
	WinHelp(VHELP_FREQ);
}

BOOL cfg_nfll::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return TRUE;
}
