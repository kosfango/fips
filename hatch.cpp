// hatch.cpp : implementation file
// IDD_HATCHDLG

#include "stdafx.h"
#include <direct.h>
#include <io.h>
#include "cfido.h"
#include "hatch.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CStrList tics;
extern _gconfig gc;
static char DlgName[]="IDD_HATCHDLG";

// ==================================================================
	hatch::hatch(CWnd* pParent ) : CDialog(hatch::IDD, pParent)
// ==================================================================
{
	//{{AFX_DATA_INIT(hatch)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

// ==================================================================
	void hatch::DoDataExchange(CDataExchange* pDX)
// ==================================================================
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(hatch)
	DDX_Control(pDX, IDC_EDIT_MAGIC, m_edit_magic);
	DDX_Control(pDX, IDC_CHECK1, m_deleteafter);
	DDX_Control(pDX, IDC_LIST, m_list);
	DDX_Control(pDX, IDC_FILENAME, m_filename);
	DDX_Control(pDX, IDC_DESCRIPTION, m_description);
	DDX_Control(pDX, IDC_EDIT_REPLACE, m_replace);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(hatch, CDialog)
	//{{AFX_MSG_MAP(hatch)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_BN_CLICKED(IDC_IMPORT_DIZ, OnImportDiz)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// ==================================================================
	BOOL hatch::OnInitDialog()
// ==================================================================
{
int tabs[]={50,90,180,400,1900};
int lng[]={
			IDC_BROWSE,
			IDC_IMPORT_DIZ,
			IDHELP,
			IDCANCEL,
			IDOK,
			IDC_CHECK1,
			IDC_STATIC1,
			IDC_STATIC2,
			IDC_STATIC3,
			IDC_STATIC685,
			};

    CDialog::OnInitDialog();
    set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));
	UPDATE_LB(tics,IDC_LIST);
	TABULATE_LB(IDC_LIST);
	EXTENT_LB(IDC_LIST,800);
	return TRUE;
}

// ==================================================================
	void hatch::OnBrowse()
// ==================================================================
{
CString	str;
int		ret;

	str.LoadString(IDS_ALLFILES);
	CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY,str);
	ret=dlg.DoModal();
	restore_base_path();
	if (ret!=IDOK)	return;
	m_filename.SetWindowText(dlg.GetPathName());
}

// ==================================================================
	void hatch::OnCancel()
// ==================================================================
{
	CDialog::OnCancel();
}

// ==================================================================
	void hatch::OnOK()
// ==================================================================
{
CString str,areapath,filepath,magic,echotag,upl,desc;
int		zone,net,node,point,myzone,mynet,mynode,mypoint,sel,ret;
char	fpath[MAX_PATH],fname[MAX_PATH];

	sel=m_list.GetCurSel();
	if (sel==LB_ERR)
		ERR_MSG_RET("E_ETMBSEC");

	str=tics.GetString(sel);
	get_token(str,0,echotag);
	get_token(str,1,upl);
    trim_all(upl);
	get_token(str,3,areapath);

	m_description.GetWindowText(desc);
	trim_all(desc);
	desc.Replace("\r\n",". ");
//	if (strchr(desc,'\r') && err_out("DN_MULLDES")==IDNO)	return;

	m_filename.GetWindowText(filepath);
	if (filepath.GetLength()<2)
		ERR_MSG_RET("E_NOFILEGIVEN");

	m_edit_magic.GetWindowText(magic);
	get_filename(filepath,fname);
	make_path(fpath,areapath,fname);
	if (access(fpath,0) && err_out("DY_STFBCTDA",fpath)==IDYES)
	{
		if (!CopyFile(filepath,fpath,0))
		{
			err_out("E_CTFF",filepath,fpath);
			return;
		}
	}

	if (parse_address(upl,&zone,&net,&node,&point)<3)
		ERR_MSG2_RET("E_INVALUPL",upl);

	CFido fid=upl;
	char phone[300],psw[300],aka[300],ffixpsw[300],replace_buf[100];
	CString repl;

	memset(replace_buf,0,sizeof(replace_buf));
	fid.GetPhoneAndPwd(phone,psw,aka,ffixpsw);

	if (parse_address(aka,&myzone,&mynet,&mynode,&mypoint)<3)
		ERR_MSG2_RET("E_INVALUPO",upl);

	m_replace.GetWindowText(repl);
    trim_all(repl);
	if (repl.GetLength()>0)
		strcpy(replace_buf,repl);

	ret=make_tic_file(zone,net,node, point,myzone,mynet,mynode, mypoint,
		filepath,ffixpsw,desc,echotag,m_deleteafter.GetCheck(),
		replace_buf,magic);
	show_msg(ret ? L("S_176") : L("S_237"),200);	// Done/Error

	CDialog::OnOK();
}

// ==================================================================
	void hatch::OnImportDiz()
// ==================================================================
{
FILE	*fp;
CString desc;
CString fname;
char	buf[1000];
char	path[MAX_PATH];
char	fpath[MAX_PATH];
int		ret=0;

	m_filename.GetWindowText(fname);
    trim_all(fname);
	if (fname.GetLength()<2)
		ERR_MSG_RET("W_NOFFFD");

	make_path(path,gc.BasePath,"_TMP");
	make_path(fpath,path,"file_id.diz");
	_mkdir(path);

	if (!extract_file(fname,path))
	{
		err_out("E_EXTFAD",fname);
		ret=0;
		goto exit;
	}
	fp=fopen(fpath,"rt");
	if (fp)
	{
		while (fgets(buf,999,fp))
			desc+=buf;

		ret=1;
		fclose(fp);
	}

exit:
	SafeFileDelete(path,FALSE);

	if (!ret)
		ERR_MSG_RET("W_NOFIDDF");

	trim_all(desc);
	desc.Replace("\n"," ");
	desc.OemToAnsi();
	m_description.SetWindowText(desc);
}

// ==================================================================
	void hatch::OnHelp()
// ==================================================================
{
	WinHelp(VHELP_HATCHDLG);
}

BOOL hatch::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return TRUE;
}
