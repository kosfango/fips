// outbound.cpp : implementation file
// IDD_OUTBOUND_VIEW

#include "stdafx.h"
#include "cfido.h"
#include <io.h>
#include <direct.h>
#include "filereq.h"
#include "cha_xfer.h"
#include "pkt_view.h"
#include "ticedit.h"
#include "lightdlg.h"
#include "outbound.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

static  CStrList list;
char gXferFile[300];
CString SelectedPktOutdir;
CString gEditTICPath;
extern _gconfig gc;

static char DlgName[]="IDD_OUTBOUND_VIEW";

// ===================================================================================
	outbound::outbound(CWnd* pParent ) 	: CDialog(outbound::IDD, pParent)
// ===================================================================================
{
	//{{AFX_DATA_INIT(outbound)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

// ===================================================================================
	void outbound::DoDataExchange(CDataExchange* pDX)
// ===================================================================================
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(outbound)
	DDX_Control(pDX, IDC_LIST, m_list);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(outbound, CDialog)
	//{{AFX_MSG_MAP(outbound)
	ON_BN_CLICKED(IDC_BUTTON1, OnFileAttach)
	ON_BN_CLICKED(IDC_BUTTON2, OnMessages)
	ON_BN_CLICKED(IDC_BUTTON12, OnFileRequest)
	ON_BN_CLICKED(IDC_BUTTON18, OnDeleteThisOutbound)
	ON_LBN_SELCHANGE(IDC_LIST, OnSelchangeList)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_BN_CLICKED(IDC_TICFILES, OnTicfiles)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// ======================================================================
	BOOL outbound::OnInitDialog()
// ======================================================================
{
int  tabs[]={100,270};
int  lng[]={
			IDHELP,
			IDOK,
			IDC_STATIC_01,
			IDC_BUTTON1,
			IDC_BUTTON2,
			IDC_BUTTON12,
			IDC_STATIC_02,
			IDC_BUTTON18,
			IDC_TICFILES
			};

	CDialog::OnInitDialog();
    set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));

	TABULATE_LB(IDC_LIST);
	EXTENT_LB(IDC_LIST,400);
	refresh_list();
	m_list.SetCurSel(0);
	OnSelchangeList();
	return TRUE;
}

// ======================================================================
	void outbound::OnFileAttach()
// ======================================================================
{
CString path;

 	get_selected_dir(path);
	make_path(gXferFile,path,XFER_LIST);
	cha_xfer dlg;
	dlg.DoModal();
	refresh_list();
}

// ======================================================================
	void outbound::OnMessages()
// ======================================================================
{
	get_selected_dir(SelectedPktOutdir);
	pkt_view dlg;
	dlg.DoModal();
	SetForegroundWindow();
	refresh_list();
}

// ======================================================================
	void outbound::OnFileRequest()
// ======================================================================
{
	filereq dlg;
	dlg.DoModal();
	refresh_list();
}

// ======================================================================
	void outbound::OnOK()
// ======================================================================
{
	SelectedPktOutdir.Empty();
	CDialog::OnOK();
}

// ======================================================================
	void outbound::OnDeleteThisOutbound()
// ======================================================================
{
CString	 path;
char	 fname[MAX_PATH];

	get_selected_dir(path);
	if (path.GetLength()<2)
	{
	    MessageBeep(0);
		return;
	}

	if (err_out("DN_ASKFORDELDIR",path)!=IDYES)
	   return;

	make_path(fname,path,"*.*");
	SafeFileDelete(fname);
	refresh_list();
}

// ======================================================================
	void outbound::list_outbound_systems(void)
// ======================================================================
{
CStrList dirs,files;
CString adir,full,asfido,info,packs,str;
char	path[MAX_PATH],fname[MAX_PATH];
int 	mailfound,errorcode,i;
CFido	cf;

	list.RemoveAll();
	dirs.FillWithDirs(gc.OutboundPath);
	dirs.DeleteEmptySubDirs(gc.OutboundPath);
	dirs.FillWithDirs(gc.OutboundPath);
	for (i=0;i<dirs.GetCount();i++)	// add netmal pathes
	{
		make_path(fname,dirs.GetString(i),"NETMAIL");
		make_path(path,gc.OutboundPath,fname);
		if (!access(path,0))
			dirs.AddTail(fname);
	}
	for (i=0;i<dirs.GetCount();i++)
	{
		adir=dirs.GetString(i);
		make_path(path,gc.OutboundPath,adir);
		make_path(fname,path,"*.*");
		files.FillWithFiles(fname); 
		if (files.GetCount()==0)   continue;	// skip empty uplinks

		packs.Empty();
		cf=PS adir;
		asfido=cf.Get();
		expand_address(asfido,full,info,0,1,0,errorcode);

		make_path(fname,path,XFER_LIST);
		if (!access(fname,0))
		   	packs+="TRANSFER";

		make_path(fname,path,FREQ_LIST);
		if (!access(fname,0))
		{
			if (packs.GetLength()>0) packs+=","; 
		   	packs+="FILEREQ";
		}

		make_path(fname,path,"*.tic");
		files.FillWithFiles(fname);
		if (files.GetCount()>0)
		{
			if (packs.GetLength()>0) packs+=","; 
		   	packs+="TIC";
		}

		mailfound=0;
		make_path(fname,path,"*.*");
		files.FillWithFiles(fname);
		for (int n=0;n<files.GetCount();n++)
		{
			strcpy(fname,files.GetString(n));
			strlwr(fname);
			if (strcmp(fname,XFER_LIST) && strcmp(fname,FREQ_LIST) && !strstr(fname,".tic"))
			{
				if (packs.GetLength()>0) packs+=",";
		   		packs+="MAIL";
				break;
			}
		}

		str=adir+"\t"+info+"\t"+packs;
	    list.AddTail(str);
	}
	UPDATE_LB(list,IDC_LIST);
}

// ======================================================================
	void outbound::OnSelchangeList()
// ======================================================================
{
CString str,packs;
int		sel;

	GET_SELID(IDC_LIST);
	disable_buttons();
	m_list.GetText(sel,str);
	get_token(str,2,packs);

	if (strstr(packs,"TRANSFER"))	ENABLEID(IDC_BUTTON1);	// Button: File Attach
	if (strstr(packs,"FILEREQ"))	ENABLEID(IDC_BUTTON12);	// Button: File Request
	if (strstr(packs,"MAIL"))			ENABLEID(IDC_BUTTON2);	// Button: Messages
	if (strstr(packs,"TIC"))			ENABLEID(IDC_TICFILES);	// Button: TIC files
	ENABLEID(IDC_BUTTON18);		// Button: Delete Outbound
}

// ======================================================================
	void outbound::get_selected_dir(CString &path)
// ======================================================================
{
CString str,tmp;
int		sel;

	 GET_SELID(IDC_LIST);
	 m_list.GetText(sel,str);
	 get_token(str,0,tmp);
	 path.Format("%s\\%s",gc.OutboundPath,tmp);
}

// ======================================================================
	void outbound::disable_buttons(void)
// ======================================================================
{
	DISABLEID(IDC_BUTTON1);   // Button: File Attach
	DISABLEID(IDC_BUTTON2);   // Button: Messages
	DISABLEID(IDC_BUTTON12);  // Button: File Request
	DISABLEID(IDC_BUTTON18);  // Button: Delete Outbound
	DISABLEID(IDC_TICFILES);  // Button: TIC Files
}

// ======================================================================
	void outbound::OnTicfiles()
// ======================================================================
{
int		sel;
CString str,fido;
char	path[MAX_PATH];
CFido	cf;

	GET_SELID(IDC_LIST);
	str=list.GetString(sel);
	get_token(str,0,fido);
	cf=PS system;
	make_path(path,gc.OutboundPath,cf.GetAsDir(0));
	_mkdir(path);
//	tmp.AssureDirPath(path);
	gEditTICPath=path;
	ticedit dlg;
	dlg.DoModal();
	refresh_list();
}

// ======================================================================
	void outbound::refresh_list(void)
// ======================================================================
{
int	sel;

	sel=m_list.GetCurSel();
	disable_buttons();
	list_outbound_systems();
	if (sel!=LB_ERR && sel<m_list.GetCount())
	{
		m_list.SetCurSel(sel);
		OnSelchangeList();
	}
}

// ======================================================================
	void outbound::OnHelp()
// ======================================================================
{
	WinHelp(VHELP_OUTBOUND_VIEW);
}

BOOL outbound::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return TRUE;
}
