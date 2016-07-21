// quickset.cpp : implementation file
// IDD_QUICK_SETUP

#include "stdafx.h"
#include "resource.h"
#include "quickset.h"
#include "structs.h"
#include "supercom.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern _gconfig gc;
extern LangID LANG_ID;
int check_field(CWnd &wnd,const char *msg=0);

static char DlgName[]="IDD_QUICK_SETUP";
int  lng[]={
			IDC_STATIC0,
			IDC_STATIC1,
			IDC_STATIC2,
			IDC_STATIC3,
			IDC_STATIC4,
			IDC_STATIC5,
			IDC_STATIC6,
			IDC_STATIC7,
			IDC_STATIC8,
			IDC_STATIC9,
			IDC_STATIC10,
			IDC_STATIC11,
			IDC_STATIC12,
			IDC_STATIC13,
			IDC_STATIC14,
			IDC_STATIC15,
			IDC_STATIC16,
			IDC_STATIC17,
			IDC_STATIC18,
			IDC_STATIC19,
			IDHELP,
			IDCANCEL,
			IDOK
			};

// ================================================================
	quickset::quickset(CWnd* pParent ) : CDialog(quickset::IDD, pParent)
// ================================================================
{
	//{{AFX_DATA_INIT(quickset)
	//}}AFX_DATA_INIT
}

// ================================================================
	void quickset::DoDataExchange(CDataExchange* pDX)
// ================================================================
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(quickset)
	DDX_Control(pDX, IDC_LANG, m_language);
	DDX_Control(pDX, IDC_SYSOPNAME, m_bossname);
	DDX_Control(pDX, IDC_SESSIONPASSWORD, m_sessionpsw);
	DDX_Control(pDX, IDC_PHONENUMBER, m_bossphone);
	DDX_Control(pDX, IDC_PACKERTYPE2, m_packertype);
	DDX_Control(pDX, IDC_MYPOINTNUMBER, m_point);
	DDX_Control(pDX, IDC_INIT1, m_init1);
	DDX_Control(pDX, IDC_FIDOADDRESS, m_bossnode);
	DDX_Control(pDX, IDC_DIAL_COMMAND, m_dialcommand);
	DDX_Control(pDX, IDC_COMPORT, m_comport);
	DDX_Control(pDX, IDC_cfg_telephone, m_phone);
	DDX_Control(pDX, IDC_cfg_systemname, m_systname);
	DDX_Control(pDX, IDC_cfg_prename, m_name);
	DDX_Control(pDX, IDC_cfg_aftername, m_surname);
	DDX_Control(pDX, IDC_cfg_location, m_location);
	DDX_Control(pDX, IDC_SYSBOSS, m_bosssyst);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(quickset, CDialog)
	//{{AFX_MSG_MAP(quickset)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_EN_KILLFOCUS(IDC_MYPOINTNUMBER, OnKillfocusPoint)
	ON_EN_KILLFOCUS(IDC_FIDOADDRESS, OnKillfocusNode)
	ON_WM_HELPINFO()
	ON_CBN_SELENDOK(IDC_LANG, OnSelendokLang)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// ================================================================
	BOOL quickset::OnInitDialog()
// ================================================================
{
char buf[10];
int	 lang;

	CDialog::OnInitDialog();
	GetLocaleInfo(LOCALE_USER_DEFAULT,LOCALE_ICOUNTRY,buf,9);
	if (!strcmp(buf,"7"))
	{
		LANG_ID=LANG_RUS;
		lang=0;
	}
	else if (!strcmp(buf,"49"))
	{
		LANG_ID=LANG_GER;
		lang=2;
	}
	else //default
	{
		LANG_ID=LANG_ENG;
		lang=1;
	}
	m_language.SetCurSel(lang);
	load_language();
//	load_dlg_language(LANG_ID);
    set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));
	m_init1.SetWindowText("ATZ");
	m_dialcommand.SetWindowText("ATDP");
	m_comport.SetCurSel(1);
	m_systname.SetWindowText(get_cfg(CFG_COMMON,"SystemName",""));
	m_name.SetWindowText(get_cfg(CFG_COMMON,"FirstName",""));
	m_surname.SetWindowText(get_cfg(CFG_COMMON,"SecondName",""));
	m_location.SetWindowText(get_cfg(CFG_COMMON,"Location",""));
	m_phone.SetWindowText(get_cfg(CFG_COMMON,"Telephone",""));
	m_packertype.SetCurSel(0);

	return TRUE;
}

// ================================================================
	void quickset::OnOK()
// ================================================================
{
_modemcfg mcfg;
CString str,packstring,depack,prename,aftname,systemname,location,telephone;
CString bosssyst,bossname,bossnode,bossphone,myname,point,psw,packertype;
char	exepath[MAX_PATH],path[MAX_PATH];

// set language
	m_language.GetLBText(m_language.GetCurSel(),str);
	set_cfg(CFG_COMMON,"Language",str);
// check my settings
	if (!check_field(m_name,L("S_253")) ||	// First Name		
		!check_field(m_surname,L("S_400")) ||	// Second Name
		!check_field(m_systname,L("S_475")) ||	// Your System Name
		!check_field(m_dialcommand,L("S_163")))	// Dial command
			return;

// check Boss settings
	if (!check_field(m_bosssyst,L("S_106")) ||	// Boss System Name
		!check_field(m_bossname,L("S_104")) ||	// Boss Name
		!check_field(m_bossnode,L("S_103")) ||	// Boss address
		!check_field(m_bossphone,L("S_105")) ||	// Boss Phone number
		!check_field(m_point,L("S_474")) ||	// Your pointnumber
		!check_field(m_sessionpsw))
			return;
	m_bossnode.GetWindowText(str);
	if (!is_valid_address(str))	return;
	m_point.GetWindowText(str);
	if (!is_valid_address(str))	return;
// fill global struct
	m_name.GetWindowText(prename);       
	strnzcpy(gc.FirstName,trim_all(prename),100);
	prename=gc.FirstName;
	m_surname.GetWindowText(aftname);     
	strnzcpy(gc.SecondName,trim_all(aftname),100);
	aftname=gc.SecondName;

	m_systname.GetWindowText(systemname); 
	trim_all(systemname);
	m_location.GetWindowText(location);     
	trim_all(location);
	m_phone.GetWindowText(telephone);   
	trim_all(telephone);
// make directories
    get_exepath(exepath);
	if (*exepath)
	{
		strupr(exepath);
		set_cfg(CFG_COMMON,"BasePath",exepath);
		make_path(path,exepath,"Util");
		create_path(path);
		set_cfg(CFG_COMMON,"UtilPath",path);
		make_path(path,exepath,"Outbound");
		create_path(path);
		set_cfg(CFG_COMMON,"OutboundPath",path);
		make_path(path,exepath,"Nodelist");
		create_path(path);
		set_cfg(CFG_COMMON,"NodelistPath",path);
		make_path(path,exepath,"Msgbase");
		create_path(path);
		set_cfg(CFG_COMMON,"MsgBasePath",path);
		make_path(path,exepath,"Inbound");
		create_path(path);
		set_cfg(CFG_COMMON,"InboundPath",path);
		make_path(path,exepath,"Multimed");
		create_path(path);
		set_cfg(CFG_COMMON,"MultimedPath",path);
		make_path(path,exepath,"Ticbasic");
		create_path(path);
		set_cfg(CFG_COMMON,"TicbasicPath",path);
		
		set_cfg(CFG_COMMON,"FirstName",prename);
		set_cfg(CFG_COMMON,"SecondName",aftname);
		set_cfg(CFG_COMMON,"SystemName",systemname);
		set_cfg(CFG_COMMON,"Location",location);
		set_cfg(CFG_COMMON,"Telephone",telephone);
	}
	else
		ERR_MSG_RET("E_BASESFA");

	m_bosssyst.GetWindowText(bosssyst);
	m_bossname.GetWindowText(bossname);
	m_bossnode.GetWindowText(bossnode);
    trim_all(bossnode);
	m_bossphone.GetWindowText(bossphone);
	m_point.GetWindowText(point);
    trim_all(point);
	m_sessionpsw.GetWindowText(psw);
	m_packertype.GetWindowText(packertype);
// build boss-line for ini
	str.Format(
		"%s;%s;%s;%s;%s;%s;%s;echolist.txt;allfiles.zip;allfiles.fls;"
		"newfiles.zip;newfiles.fls;AreaFix;%s;%d;FileFix;%s;%d;%s;*;0;0;0;0",
		bosssyst,bossnode,bossphone,bossname,point,psw,packertype,psw,0,psw,0,psw);
	set_cfg(CFG_BOSSES,"Boss0",str);

	memset(&mcfg,0,sizeof(_modemcfg));
	mcfg.m_comport=1;
	mcfg.m_flow=SIGNAL_CTS;
	mcfg.m_data=8;
	mcfg.m_baud=57600;
	mcfg.m_parity='N';
	mcfg.m_stop=1;
	strcpy(mcfg.m_answer,"AT S0=2");
	m_dialcommand.GetWindowText(str); 	
	strcpy(mcfg.m_dial,str);
	m_init1.GetWindowText(str);
	strcpy(mcfg.m_init1,str);
	mcfg.m_comport=m_comport.GetCurSel()+1;
	save_modem_cfg(1,&mcfg);
	CDialog::OnOK();
}

// ================================================================
	int is_exe_available(LPCSTR name)
// ================================================================
{
CString str;
char buf[MAX_PATH];
char *p;

	str=name; 				
	if (SearchPath(NULL,str,NULL,MAX_PATH-1,buf,&p)) return 1;
	str=name; str+=".exe";	
	if (SearchPath(NULL,str,NULL,MAX_PATH-1,buf,&p)) return 1;
	str=name; str+=".bat";	
	if (SearchPath(NULL,str,NULL,MAX_PATH-1,buf,&p)) return 1;
	str=name; str+=".com";	
	if (SearchPath(NULL,str,NULL,MAX_PATH-1,buf,&p)) return 1;
	str=name; str+=".cmd";	
	if (SearchPath(NULL,str,NULL,MAX_PATH-1,buf,&p)) return 1;
	str=name; str+=".pif";	
	if (SearchPath(NULL,str,NULL,MAX_PATH-1,buf,&p)) return 1;
	return 0;
}

// ================================================================
	void quickset::OnKillfocusNode() 
// ================================================================
{
CString	str;

	m_bossnode.GetWindowText(str);
	is_valid_address(str);
}

// ================================================================
	void quickset::OnKillfocusPoint()
// ================================================================
{
CString point,boss,str;

	m_point.GetWindowText(point);
	m_bossnode.GetWindowText(boss);
	if (!strchr(point,'.'))
	{
		str=boss+"."+point;
		str=str.Left(19);	// max ZZZZ:NNNN/FFFF.PPPP
		m_point.SetWindowText(str);
	}
}

// ================================================================
	int check_field(CWnd &wnd,const char *msg)
// ================================================================
{
CString	buf,str;

    wnd.GetWindowText(buf);   
	trim_all(buf);
	if (buf.IsEmpty() && msg)
	{
		str.Format(IDS_W_FILLIT,msg);
		AfxMessageBox(str);
		return 0;
	}
	if (buf.Find(';')>=0)
	{
		str.LoadString(IDS_INVALIDCHAR);
		AfxMessageBox(str);
		return 0;
	}
	return 1;
}
	
// ================================================================
	void quickset::OnHelp()
// ================================================================
{
	WinHelp(VHELP_QUICK_SETUP);
}

BOOL quickset::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return TRUE;
}

void quickset::OnSelendokLang() 
{
	switch (m_language.GetCurSel())
	{
		case LANG_GER:
			LANG_ID=LANG_GER;
			break;
		case LANG_RUS:
			LANG_ID=LANG_RUS;
			break;
		case LANG_FRE:
			LANG_ID=LANG_FRE;
			break;
		case LANG_ENG:
		default:
			LANG_ID=LANG_ENG;
	}
	load_language();
//	load_dlg_language(LANG_ID);
    set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));
}
