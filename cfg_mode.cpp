// cfg_mode.cpp : implementation file
// IDD_CFG_HARDWARE

#include "stdafx.h"
#include <io.h>
#include "cfg_mode.h"
#include "supercom.h"
#include "tapisel.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern CStrList  isdnlist;
extern CString  isdnaccept;
CStrList	modemkeys;
CString		gTapiModem;
int			curmod=1;	// current modem
_modemcfg	mc[2];

static char	DlgName[]="IDD_CFG_HARDWARE";

int GetBinaryRegistryDCBData(HKEY whichtree,char *treepath,char *bindata);

// =====================================================
		cfg_modem::cfg_modem(CWnd* pParent /*=NULL*/)
		: CSAPrefsSubDlg(cfg_modem::IDD, pParent)
// =====================================================
, m_useinit1(FALSE)
, m_useinit2(FALSE)
, m_useinit3(FALSE)
, m_usecmd1(FALSE)
, m_usecmd2(FALSE)
, m_usecmd3(FALSE)
		{
	//{{AFX_DATA_INIT(cfg_modem)
	m_answer = _T("");
	m_dial = _T("");
	m_init1 = _T("");
	m_init2 = _T("");
	m_init3 = _T("");
	m_predial1 = _T("");
	m_predial2 = _T("");
	m_predial3 = _T("");
	m_databits = -1;
	m_parity = -1;
	m_stopbits = -1;
	m_flowctl = -1;
	m_use_second = FALSE;
	m_isdn_enable = FALSE;
	m_msn = _T("");
	m_newline = _T("");
	//}}AFX_DATA_INIT
}

void cfg_modem::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(cfg_modem)
	DDX_Control(pDX, IDC_LIST, m_isdnlist);
	DDX_Control(pDX, IDC_BAUD, m_baud_control);
	DDX_Control(pDX, IDC_COMPORT, m_comport_control);
	DDX_Text(pDX, IDC_ANSWER_COMMAND, m_answer);
	DDV_MaxChars(pDX, m_answer, 100);
	DDX_Text(pDX, IDC_DIAL_COMMAND, m_dial);
	DDV_MaxChars(pDX, m_dial, 100);
	DDX_Text(pDX, IDC_INIT1, m_init1);
	DDV_MaxChars(pDX, m_init1, 100);
	DDX_Text(pDX, IDC_INIT2, m_init2);
	DDV_MaxChars(pDX, m_init2, 100);
	DDX_Text(pDX, IDC_INIT3, m_init3);
	DDV_MaxChars(pDX, m_init3, 100);
	DDX_Text(pDX, IDC_INITDIAL1, m_predial1);
	DDX_Text(pDX, IDC_INITDIAL2, m_predial2);
	DDX_Text(pDX, IDC_INITDIAL3, m_predial3);
	DDX_Radio(pDX, IDC_DATA8, m_databits);
	DDX_Radio(pDX, IDC_PNONE, m_parity);
	DDX_Radio(pDX, IDC_STOP1, m_stopbits);
	DDX_Radio(pDX, IDC_RTS, m_flowctl);
	DDX_Check(pDX, IDC_USEINIT1, m_useinit1);
	DDX_Check(pDX, IDC_USEINIT2, m_useinit2);
	DDX_Check(pDX, IDC_USEINIT3, m_useinit3);
	DDX_Check(pDX, IDC_USECMD1, m_usecmd1);
	DDX_Check(pDX, IDC_USECMD2, m_usecmd2);
	DDX_Check(pDX, IDC_USECMD3, m_usecmd3);
	DDX_Check(pDX, IDC_USE2MODEM, m_use_second);
	DDX_Check(pDX, IDC_ISDNENABLE, m_isdn_enable);
	DDX_Text(pDX, IDC_MSN, m_msn);
	DDX_Text(pDX, IDC_NEWLINE, m_newline);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(cfg_modem, CDialog)
	//{{AFX_MSG_MAP(cfg_modem)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_BN_CLICKED(IDC_IMPORT, OnImport)
	ON_WM_HELPINFO()
	ON_BN_CLICKED(IDC_MOD1SET, OnMod1set)
	ON_BN_CLICKED(IDC_MOD2SET, OnMod2set)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// =====================================================================
LPCSTR cfg_modem::GetName(void)	{return DlgName;}
// =====================================================================

// =====================================================
	BOOL cfg_modem::OnInitDialog()
// =====================================================
{
int		lng[]={
				IDC_PNONE,
				IDC_PODD,
				IDC_PEVEN,
				IDC_IMPORT,
				IDC_DELETE,
				IDC_ADD,
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
				IDC_ISDNENABLE,
				IDC_USE2MODEM,
				IDC_STATIC14,
				IDC_USEINIT1,
				IDC_USEINIT2,
				IDC_USEINIT3,
				IDC_USECMD1,
				IDC_USECMD2,
				IDC_USECMD3,
				};

	CDialog::OnInitDialog();
    set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));
// modems part
	load_modem_cfg(1,&mc[0]);
	load_modem_cfg(2,&mc[1]);
	from_mc(&mc[0]);
	CheckRadioButton(IDC_MOD1SET,IDC_MOD2SET,IDC_MOD1SET);
// isdn part
	m_isdn_enable=isdnlist.defaultindexnew;
	m_use_second=isdnlist.defaultindex;
	m_msn=isdnaccept;
	UPDATE_LB(isdnlist,IDC_LIST);
	UpdateData(0);
	return TRUE;
}

// =====================================================
	void cfg_modem::OnOK()
// =====================================================
{
CString str;
	
	UpdateData(1);
	if (((CButton*)GetDlgItem(IDC_MOD1SET))->GetCheck())  
		to_mc(&mc[0]);
	save_modem_cfg(1,&mc[0]);
	if (((CButton*)GetDlgItem(IDC_MOD2SET))->GetCheck()) 
	{
		to_mc(&mc[1]);
		save_modem_cfg(2,&mc[1]);
	}
	load_modem_cfg(1);
	isdnlist=m_isdnlist;
	isdnlist.defaultindexnew=m_isdn_enable;
	isdnlist.defaultindex	=m_use_second;
	set_cfg(CFG_HWARE,"IsdnEnable",m_isdn_enable);
	set_cfg(CFG_HWARE,"UseSecond",m_use_second);
	set_cfg(CFG_HWARE,"IsdnAccept",m_msn);
	isdnlist.SaveToDelimString(str,'\\');
	set_cfg(CFG_HWARE,"IsdnStrings",str);
	str=get_cfg(CFG_HWARE,"M2Port","");
	if (m_use_second && str.IsEmpty())	ERR_MSG_RET("W_NSMCY");
	if(m_bCloseOnOk)
		CDialog::OnOK();
}

// =====================================================
// sets controls from config struct
	void cfg_modem::from_mc(_modemcfg *pmc)
// =====================================================
{
char buf[20];

	sprintf(buf,"COM%d",pmc->m_comport);
	m_comport_control.SelectString(-1,buf);
	m_answer=pmc->m_answer;
	m_dial=pmc->m_dial;
	m_useinit1=pmc->m_useinit1;
	m_useinit2=pmc->m_useinit2;
	m_useinit3=pmc->m_useinit3;
	m_init1=pmc->m_init1;
	m_init2=pmc->m_init2;
	m_init3=pmc->m_init3;
	m_usecmd1=pmc->m_usecmd1;
	m_usecmd2=pmc->m_usecmd2;
	m_usecmd3=pmc->m_usecmd3;
	m_predial1=pmc->m_predial1;
	m_predial2=pmc->m_predial2;
	m_predial3=pmc->m_predial3;
	sprintf(buf,"%d",pmc->m_baud);
	m_baud_control.SelectString(-1,buf);
	m_databits=pmc->m_data==7;
	m_parity=pmc->m_parity=='N' ? 0 : (pmc->m_parity=='O' ? 1 : 2);
	m_stopbits=pmc->m_stop==2;
	m_flowctl=pmc->m_flow==SIGNAL_XON;
	UpdateData(0);
}

// =====================================================
	void cfg_modem::to_mc(_modemcfg *pmc)
// =====================================================
{
char buf[20];

	UpdateData(1);
	memset(pmc,0,sizeof(_modemcfg));
	m_comport_control.GetWindowText(buf,20);
	sscanf(buf,"COM%d",&pmc->m_comport);
	strcpy(pmc->m_answer,m_answer);
	strcpy(pmc->m_dial,m_dial);
	pmc->m_useinit1=m_useinit1;
	pmc->m_useinit2=m_useinit2;
	pmc->m_useinit3=m_useinit3;
	strcpy(pmc->m_init1,m_init1);
	strcpy(pmc->m_init2,m_init2);
	strcpy(pmc->m_init3,m_init3);
	pmc->m_usecmd1=m_usecmd1;
	pmc->m_usecmd2=m_usecmd2;
	pmc->m_usecmd3=m_usecmd3;
	strcpy(pmc->m_predial1,m_predial1);
	strcpy(pmc->m_predial2,m_predial2);
	strcpy(pmc->m_predial3,m_predial3);
	m_baud_control.GetWindowText(buf,20);
	sscanf(buf,"%d",&pmc->m_baud);
	pmc->m_data=m_databits==0 ? 8 : 7;
	pmc->m_parity=m_parity==0 ? 'N' : m_parity==1 ? 'O' : 'E';
	pmc->m_stop=m_stopbits==0 ? 1 : 2;
	pmc->m_flow=m_flowctl==0 ? SIGNAL_CTS : SIGNAL_XON;
}
	
// =====================================================
	void cfg_modem::get_sys_modems()
// =====================================================
{
CStrList branches,keys,modementries,modemvals;
CString	 result;
char	 branch[100],str[300],key[300];
int		 n,isWinnt;

	isWinnt=GetVersion() < 0x80000000;
	strcpy(branch,"SYSTEM\\CurrentControlSet\\");
	if (isWinnt)
		strcat(branch,"Control\\Class");
	else
		strcat(branch,"Services\\Class");

	modemkeys.RemoveAll();
	gTapiModem.Empty();
	branches.EnumRegistryKeys(HKEY_LOCAL_MACHINE,branch);

	for (int i=0;i<branches.GetCount();i++)
	{
		make_path(str,branch,branches.GetString(i));
		keys.EnumStringKeyValues(HKEY_LOCAL_MACHINE,str);

		if (keys.FindString("Modem",result,1)!=-1 || keys.FindString("Модем",result,1)!=-1)
		{
			modementries.EnumRegistryKeys(HKEY_LOCAL_MACHINE,str);
			for (n=0;n<modementries.GetCount();n++)
			{
				make_path(key,str,modementries.GetString(n));
				modemvals.EnumStringKeyValues(HKEY_LOCAL_MACHINE,key);

				if (isWinnt)
					modemvals.FindString("ResponsesKeyName",result,0);
				else
					modemvals.FindString("Model",result,0);
				
				get_token(result,1,str); 
				strcat(str,"\t");
				strcat(str,key);
				modemkeys.AddTail(str);
			}

			if (modemkeys.GetCount()==0) goto ModemNotFound;
			tapisel dlg;
			if (dlg.DoModal() != IDOK)	return;

			if (gTapiModem.GetLength()>1)
			{
				FetchAndSetModemData();
				return;
			}
			break;
		}
	}

ModemNotFound:
	ERR_MSG_RET("E_NOTAPIFOUND");
}

// =====================================================
        void cfg_modem::OnImport()
// =====================================================
{
    __try
    {
            get_sys_modems();
    }
    __except (1,EXCEPTION_EXECUTE_HANDLER)
    {
            Sleep(500);
            err_out("E_MODDETFAILU");
    }
}

// =====================================================
	void cfg_modem::FetchAndSetModemData	(void)
// =====================================================
{
CStrList xx,yy;
char key[100],work[100];
CString result,line,help,additional;
CString CompressionOn,ErrorControlOn,FlowControlHard,SpeakerModeDial;
char    cr[100],attached[100],buf[100];
int		i,ret;
DCB     dcb;

	memset(cr,0,sizeof(cr));
	get_token(gTapiModem,1,key);
	yy.EnumStringKeyValues(HKEY_LOCAL_MACHINE,key);
	make_path(work,key,"Settings");
	xx.EnumStringKeyValues(HKEY_LOCAL_MACHINE,work);
	xx.FindString("Terminator",result,0);
	if (result.GetLength()<=0)
		strcpy(cr,"<cr>");
	else
	{
		get_token(result,1,help);
		strcpy(cr,help);
	}

	if (ret=xx.FindString("AttachedTo",result)<0)
		ret=yy.FindString("AttachedTo",result);
	if (ret<0)
		strcpy(attached,"COM2");
	else
	{
		get_token(result,1,help);
		strcpy(attached,help);
	}

	xx.FindString("Compression_On",result,0);
	if (!result.IsEmpty())
		get_token(result,1,CompressionOn);

	xx.FindString("ErrorControl_On",result,0);
	if (!result.IsEmpty())
		get_token(result,1,ErrorControlOn);

	xx.FindString("FlowControl_Hard",result,0);
	if (!result.IsEmpty())
		get_token(result,1,FlowControlHard);

	xx.FindString("SpeakerMode_Dial",result,0);
	if (!result.IsEmpty())
		get_token(result,1,SpeakerModeDial);

	m_comport_control.SelectString(-1,attached);
	memset(&dcb,0,sizeof(dcb));
	if (GetBinaryRegistryDCBData(HKEY_LOCAL_MACHINE,PS key,(char *)&dcb))
	{
		help.Format("%d",dcb.BaudRate);
		m_baud_control.SelectString(-1,help);
	}

	make_path(work,key,"Init");
	xx.EnumStringKeyValues(HKEY_LOCAL_MACHINE,work);
	m_init1.Empty();
	m_init2.Empty();
	m_init3.Empty();
	for (i=1;i<=3;i++)
	{
		sprintf(buf,"%d",i);
		xx.FindString(buf,result,0);
		if (result.GetLength()>0)
		{
			char *p;
			get_token(result,1,help);
			// strip <cr> ...
			p= (char *) strstr(help,cr);
			if (p)	*p=0;
			if (help.IsEmpty())	break;
			if (i==1)		m_init1=help;
			if (i==2)		m_init2=help;
			if (i==3)		m_init3=help;
		}
	}

	additional=CompressionOn+" "+ErrorControlOn+" "+FlowControlHard+" "+SpeakerModeDial;

	m_useinit1=m_useinit2=m_useinit3=TRUE;
	if (m_init1.IsEmpty())
		m_init1="AT "+additional;
	else if (m_init2.IsEmpty())
		m_init2="AT "+additional;
	else if (m_init3.IsEmpty())
		m_init3="AT "+additional;
	else
		m_init1+=" "+additional;

	make_path(work,key,"Answer");
	xx.EnumStringKeyValues(HKEY_LOCAL_MACHINE,work);
	xx.FindString("1",result,0);
	if (result.GetLength()>0)
	{
		char *p;
		get_token(result,1,help);
		// strip <cr> ...
		p= (char *) strstr(help,cr);
		if (p)	*p=0;
		if (!help.IsEmpty())		m_answer=help;
	}
	UpdateData(0);
}

// =========================================================================
	int GetBinaryRegistryDCBData(HKEY whichtree,char *treepath,char *bindata)
// =========================================================================
{
HKEY	hkey=0;
int		ret=0;
ULONG	datasize,valuesize,type;
char	values[500],data[500];

	if (RegOpenKeyEx(whichtree,treepath,0,KEY_ALL_ACCESS,&hkey)!=ERROR_SUCCESS)
		return 0;

	for (int i=0;;i++)
	{
		datasize=sizeof(data)-1;
		valuesize=sizeof(values)-1;
		type=0;
		memset(values,0,sizeof(values));
		memset(data,0,sizeof(data));
		if (RegEnumValue(hkey,i,values,&valuesize,0,&type,(UCHAR *)data,&datasize)!=ERROR_SUCCESS)
			break;

		if (type==REG_BINARY && !strcmp(values,"DCB"))
		{
			ret=1;
			memcpy(bindata,data,20);
			break;
		}
	}
	if (hkey)
	{
		RegCloseKey (hkey);
		hkey=0;
	}
	return ret;
}

void cfg_modem::OnMod1set()
{
	if (curmod==1)	return;
	curmod=1;
	to_mc(&mc[1]);
	from_mc(&mc[0]);
	UpdateData(0);
}

void cfg_modem::OnMod2set() 
{
	if (curmod==2)	return;
	curmod=2;
	to_mc(&mc[0]);
	from_mc(&mc[1]);
	UpdateData(0);
}
	
// =====================================================
	void cfg_modem::OnHelp()
// =====================================================
{
	WinHelp(VHELP_MODEMS);
}

BOOL cfg_modem::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return TRUE;
}

void cfg_modem::OnAdd() 
{
CString buf;

	UpdateData(1);
	buf=m_newline;
	trim_all(buf);
	if (buf.IsEmpty())	ERR_MSG_RET("E_EMPTSANA");
	m_isdnlist.AddString(buf);
	m_newline.Empty();
	UpdateData(0);
	GetDlgItem(IDC_NEWLINE)->SetFocus();
}

void cfg_modem::OnDelete() 
{
int sel;
	
	GET_SELID(IDC_LIST);
	m_isdnlist.DeleteString(sel);
}
