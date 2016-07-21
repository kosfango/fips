// cfg_scr.cpp : implementation file
// IDD_SCRIPT_EDITOR

#include "stdafx.h"
#include "lightdlg.h"
#include "cfg_scr.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern _gconfig	 gc;
static char DlgName[]="IDD_SCRIPT_EDITOR";

// =====================================================================================
	cfg_scr::cfg_scr(CWnd* pParent ) : CDialog(cfg_scr::IDD, pParent)
// =====================================================================================
{
	//{{AFX_DATA_INIT(cfg_scr)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

// =====================================================================================
	void cfg_scr::DoDataExchange(CDataExchange* pDX)
// =====================================================================================
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(cfg_scr)
	DDX_Control(pDX, IDC_EDIT_UDEF_16, m_udef16);
	DDX_Control(pDX, IDC_EDIT_UDEF_15, m_udef15);
	DDX_Control(pDX, IDC_EDIT_UDEF_13, m_udef13);
	DDX_Control(pDX, IDC_LIST1, m_commands);
	DDX_Control(pDX, IDC_EDIT_UDEF_12, m_udef12);
	DDX_Control(pDX, IDC_EDIT_UDEF_11, m_udef11);
	DDX_Control(pDX, IDC_EDIT_UDEF_10, m_udef10);
	DDX_Control(pDX, IDC_EDIT_UDEF_9,  m_udef9);
	DDX_Control(pDX, IDC_EDIT_UDEF_8,  m_udef8);
	DDX_Control(pDX, IDC_EDIT_UDEF_7,  m_udef7);
	DDX_Control(pDX, IDC_EDIT_UDEF_6,  m_udef6);
	DDX_Control(pDX, IDC_EDIT_UDEF_5,  m_udef5);
	DDX_Control(pDX, IDC_EDIT_UDEF_4,  m_udef4);
	DDX_Control(pDX, IDC_EDIT_UDEF_3,  m_udef3);
	DDX_Control(pDX, IDC_EDIT_UDEF_2,  m_udef2);
	DDX_Control(pDX, IDC_EDIT_UDEF_1,  m_udef1);
	DDX_Control(pDX, IDC_EDIT, m_edit);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(cfg_scr, CDialog)
	//{{AFX_MSG_MAP(cfg_scr)
	ON_BN_CLICKED(IDC_LOAD, OnLoad)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_BN_CLICKED(IDC_UDEF_FIND_1, OnUdefFind1)
	ON_BN_CLICKED(IDC_UDEF_FIND_2, OnUdefFind2)
	ON_BN_CLICKED(IDC_UDEF_FIND_3, OnUdefFind3)
	ON_BN_CLICKED(IDC_UDEF_FIND_4, OnUdefFind4)
	ON_BN_CLICKED(IDC_UDEF_FIND_5, OnUdefFind5)
	ON_BN_CLICKED(IDC_UDEF_FIND_6, OnUdefFind6)
	ON_BN_CLICKED(IDC_UDEF_FIND_7, OnUdefFind7)
	ON_BN_CLICKED(IDC_UDEF_FIND_8, OnUdefFind8)
	ON_BN_CLICKED(IDC_UDEF_FIND_9, OnUdefFind9)
	ON_BN_CLICKED(IDC_UDEF_FIND_10, OnUdefFind10)
	ON_BN_CLICKED(IDC_UDEF_FIND_11, OnUdefFind11)
	ON_BN_CLICKED(IDC_UDEF_FIND_12, OnUdefFind12)
	ON_BN_CLICKED(IDC_UDEF_FIND_13, OnUdefFind13)
	ON_BN_CLICKED(IDC_UDEF_FIND_15, OnUdefFind15)
	ON_BN_CLICKED(IDC_UDEF_FIND_16, OnUdefFind16)
	ON_LBN_DBLCLK(IDC_LIST1, OnDblclkList1)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// =====================================================================================
	BOOL cfg_scr::OnInitDialog()
// =====================================================================================
{
CStrList lst;
CString	 scr;
char	 **p;
int  lng[]={
			IDC_LOAD,
			IDC_SAVE,
			IDOK,
			IDC_STATIC1,
			IDC_STATIC2,
			IDC_STATIC3,
			IDC_STATIC4,
			IDC_STATIC5,
			IDC_STATIC6,
			IDC_STATIC880,
			IDC_STATIC7,
			IDC_STATIC8,
			IDC_STATIC9,
			IDC_STATIC10,
			IDC_STATIC11,
			IDC_STATIC12,
			IDC_STATIC13,
			IDC_STATIC14,
			IDHELP,
			IDC_STATIC15,
			IDC_STATIC17,
			IDC_STATIC18,
			IDCANCEL
			};
char *strcmd[]={
	"Show(M | T | P | N | S | F | Q | E | R)",
	"Hide(M | T | P | N | S)","Move(M | T | P | N | S | F,x,y)","RestartEvents",
	"System(command)","PlayWave(filepath)","MsgBox(text)","Status(text)",
	"Wait(hour:min:sec)","Sleep(millisec)","Beep",
	"Call(scriptpath)","Rescan","Go(AREA|MAIL,FIRST|LAST|NEXT|PREV)",
	"StartMailer(Q1,Q2,..,Q10)","StartTosser","StartPurger","StartNLComp",
	"OpenFreqDialog","MarkAllAsRead","MarkAllAsDeleted",
	"Set(EV | TG | SB | EM | UD | TT , ON | OFF)","AutoSave",
	"Sort(FR | TO | SU | CR | RE | ST , INC | DEC)",
	"UUDecode","ViewOutbound","ResetEmergency","AppendToFile(filepath)",
	"Flash(msec,\"message\")","UpdateDisplay","CheckDupes(CUR | ALL)",
	"ESSelArea(area)","ESDeselArea(area)","ESShowFirst",
	"ESShowNext","ESClearQuery","ESClearResults",
	"ESLoadQuery(filepath)","ESLoadResults(filepath)",
	"ESSaveResults(filepath)","ESStart","CheckNodelists",
	"ToggleUserMarkForMail","RemoveUserMarkForAll","MailLimitOff","MailLimitOn",
	"Exit","CheckInbound","FindOriginal","FlushFiles","MarkAllAreasAsRead",
	"WinExec(command)","AcceptCall(1 | 2)","Print",
	"FileCreate(filepath)","FileDelete(filepath)","FileExist(filepath)",
	"FileExistDel(filepath)","FileImport(filepath)","FileCopy(sourcepath,destpath)",
	"ITOA(value)","GCVT(value)","ReadIniInt(sect,key,def,file)",
	"ReadIniStr(sect,key,def,file)","WriteIniStr(sect,key,def,file)",""
};

	CDialog::OnInitDialog();
    set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));
	lst.RemoveAll();
	p=strcmd;
	while(**p)
	{
		lst.AddTail(*p);
		p++;
	}

	UPDATE_LB(lst,IDC_LIST1);
	EXTENT_LB(IDC_LIST1,500);

	m_udef1.SetWindowText(get_cfg(CFG_SCRIPTS,"User1",""));
	m_udef2.SetWindowText(get_cfg(CFG_SCRIPTS,"User2",""));
	m_udef3.SetWindowText(get_cfg(CFG_SCRIPTS,"User3",""));
	m_udef4.SetWindowText(get_cfg(CFG_SCRIPTS,"User4",""));
	m_udef13.SetWindowText(get_cfg(CFG_SCRIPTS,"User5",""));
	m_udef5.SetWindowText(get_cfg(CFG_SCRIPTS,"Func2",""));
	m_udef6.SetWindowText(get_cfg(CFG_SCRIPTS,"Func3",""));
	m_udef7.SetWindowText(get_cfg(CFG_SCRIPTS,"Func4",""));
	m_udef8.SetWindowText(get_cfg(CFG_SCRIPTS,"Func5",""));
	m_udef9.SetWindowText(get_cfg(CFG_SCRIPTS,"Func6",""));
	m_udef10.SetWindowText(get_cfg(CFG_SCRIPTS,"Func7",""));
	m_udef11.SetWindowText(get_cfg(CFG_SCRIPTS,"Func8",""));
	m_udef12.SetWindowText(get_cfg(CFG_SCRIPTS,"Func9",""));
	m_udef15.SetWindowText(get_cfg(CFG_SCRIPTS,"Func11",""));
	m_udef16.SetWindowText(get_cfg(CFG_SCRIPTS,"Func12",""));
	scr=gc.ScriptName;
	if (scr.GetLength())
		load_script(scr);
	else
		GetDlgItem(IDC_SCRIPTNAME)->SetWindowText(L("S_326"));
	return TRUE;
}

// =====================================================================================
	void cfg_scr::OnLoad()
// =====================================================================================
{
CString	 str,scr;
int		 ret;

	str.LoadString(IDS_SCRIPTFILT);
	CFileDialog	dlg(TRUE,"fps",scr,OFN_HIDEREADONLY,str);
	ret=dlg.DoModal();
	restore_base_path();
	if (ret!=IDOK)	return;
	scr=dlg.GetPathName();
	load_script(scr);
}

// =====================================================================================
	void cfg_scr::load_script(CString &scr)
// =====================================================================================
{
CStrList lst;
CString	 str;
char	 fname[MAX_PATH];

	lst.FillAsEdit(scr);
	get_filename(scr,fname);
	GetDlgItem(IDC_SCRIPTNAME)->SetWindowText(fname);
	str.Empty();
	for (int i=0;i<lst.GetCount();i++)
		str=str+lst.GetString(i)+"\r\n";
	str.TrimRight();
	m_edit.SetWindowText(str);
}

// =====================================================================================
	void cfg_scr::OnSave()
// =====================================================================================
{
CStrList lst;
CString	 str,scr;
char	 line[300];
int      ret;

	GetDlgItem(IDC_SCRIPTNAME)->GetWindowText(scr);
	str.LoadString(IDS_SCRIPTFILT);
	CFileDialog	dlg(FALSE,"fps",scr,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,str);
	ret=dlg.DoModal();
	restore_base_path();
	if (ret!=IDOK)	return;
	for (int i=0;i<m_edit.GetLineCount();i++)
	{
		ret=m_edit.GetLine(i,line,299);
		line[ret]=0;
		lst.AddTail(line);
	}
	lst.SaveAsEdit(dlg.GetPathName());
}

// =====================================================================================
	void cfg_scr::OnOK() 
// =====================================================================================
{
CString scr;

	m_udef1.GetWindowText(scr);
	set_cfg(CFG_SCRIPTS,"User1",scr);
	m_udef2.GetWindowText(scr);  
	set_cfg(CFG_SCRIPTS,"User2",scr);
	m_udef3.GetWindowText(scr);  
	set_cfg(CFG_SCRIPTS,"User3",scr);
	m_udef4.GetWindowText(scr);  
	set_cfg(CFG_SCRIPTS,"User4",scr);
	m_udef13.GetWindowText(scr);  
	set_cfg(CFG_SCRIPTS,"User5",scr);

	m_udef5.GetWindowText(scr);   
	set_cfg(CFG_SCRIPTS,"Func2",scr);
	m_udef6.GetWindowText(scr);   
	set_cfg(CFG_SCRIPTS,"Func3",scr);
	m_udef7.GetWindowText(scr);   
	set_cfg(CFG_SCRIPTS,"Func4",scr);
	m_udef8.GetWindowText(scr);   
	set_cfg(CFG_SCRIPTS,"Func5",scr);
	m_udef9.GetWindowText(scr);   
	set_cfg(CFG_SCRIPTS,"Func6",scr);
	m_udef10.GetWindowText(scr);  
	set_cfg(CFG_SCRIPTS,"Func7",scr);
	m_udef11.GetWindowText(scr);  
	set_cfg(CFG_SCRIPTS,"Func8",scr);
	m_udef12.GetWindowText(scr);  
	set_cfg(CFG_SCRIPTS,"Func9",scr);
	m_udef15.GetWindowText(scr);  
	set_cfg(CFG_SCRIPTS,"Func11",scr);
	m_udef16.GetWindowText(scr);  
	set_cfg(CFG_SCRIPTS,"Func12",scr);

	CDialog::OnOK();
}

// =====================================================================================
	void cfg_scr::OnCancel()
// =====================================================================================
{
	CDialog::OnCancel();
}

void cfg_scr::OnUdefFind1() { select_script(m_udef1);}
void cfg_scr::OnUdefFind2() { select_script(m_udef2);}
void cfg_scr::OnUdefFind3() { select_script(m_udef3);}
void cfg_scr::OnUdefFind4() { select_script(m_udef4);}
void cfg_scr::OnUdefFind5() { select_script(m_udef5);}
void cfg_scr::OnUdefFind6() { select_script(m_udef6);}
void cfg_scr::OnUdefFind7() { select_script(m_udef7);}
void cfg_scr::OnUdefFind8() { select_script(m_udef8);}
void cfg_scr::OnUdefFind9() { select_script(m_udef9);}
void cfg_scr::OnUdefFind10() { select_script(m_udef10);}
void cfg_scr::OnUdefFind11() { select_script(m_udef11);}
void cfg_scr::OnUdefFind12() { select_script(m_udef12);}
void cfg_scr::OnUdefFind13() { select_script(m_udef13);}
void cfg_scr::OnUdefFind15() { select_script(m_udef15);}
void cfg_scr::OnUdefFind16() { select_script(m_udef16);}

// =========================================================
	void cfg_scr::OnDblclkList1()
// =========================================================
{
int		sel;
CString help;
char	buf[300];

    sel=m_commands.GetCurSel();
	LB_ERR_RET;
	m_commands.GetText(sel,buf);
	strcat(buf,"\r\n");
	m_edit.ReplaceSel(buf);
}

// =========================================================
	void cfg_scr::select_script(CWnd &ctl)
// =========================================================
{
CString str;
int		ret;

	str.LoadString(IDS_SCRIPTFILT);
	CFileDialog	dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY,str);
	ret=dlg.DoModal();
	restore_base_path();
	if (ret!=IDOK)	return;
	ctl.SetWindowText(dlg.GetPathName());
}

// =========================================================
	void cfg_scr::OnHelp()
// =========================================================
{
	WinHelp(VHELP_SCRIPT_EDITOR);
}

BOOL cfg_scr::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return TRUE;
}
