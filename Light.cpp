// light.cpp : Defines the class behaviors for the application.

#include "stdafx.h"
#include "light.h"
#include "lightdlg.h"
#include "guard.h"
#include "cfg_dir.h"
#include "cfg_boss.h"
#include "cfg_edit.h"
#include "cfg_othr.h"
#include "cfg_area.h"
#include "cfg_pack.h"
#include "cfg_node.h"
#include "cfg_col.h"
#include "cfgsound.h"
#include "dettoss.h"
#include "detpurg.h"
#include "detmail.h"
#include "newmail.h"
#include "cfg_mode.h"
#include "cfg_scr.h"
#include "det_nl.h"
#include "cfg_time.h"
#include "cfg_carb.h"
#include "ecolst.h"
#include "traffic.h"
#include <io.h>
#include <direct.h>
#include "logdisp.h"
#include "logopti.h"
#include "cfg_aaka.h"
#include "cfg_conv.h"
#include "crospost.h"
#include "al_exist.h"
#include "outbound.h"
#include "cfg_tras.h"
#include "receiver.h"
#include "events.h"
#include "trepl.h"
#include "txttemp.h"
#include "cfg_inco.h"
#include "cfg_ring.h"
#include "exsearch.h"
#include "cfg_mult.h"
#include "insmm.h"
#include "adrbook.h"
#include "sub_adj.h"
#include "cfgmagic.h"
#include "cfgrpath.h"
#include "cfg_rmon.h"
#include "cfg_tics.h"
#include "hatch.h"
#include "exbrow.h"
#include "marksel.h"
#include "costuser.h"
#include "costanal.h"
#include "inouthis.h"
#include "cfg_fax.h"
#include "choosas.h"
#include "simppoll.h"
#include "gcomm.h"
#include "bbsgrp.h"
#include "bbsmain.h"
#include "bbsuser.h"
#include "bbsarea.h"
#include "valarea.h"
#include "floating.h"
#include "cfg_icon.h"
#include "again.h"
#include "wiedinf.h"
#include "kludges.h"
#include "quickset.h"
#include "supercom.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern CLightDlg *gpMain;
extern dettoss   gTosser;
extern detmail   gMailer;
extern detpurg   gPurger;
extern det_nl    gNLComp;
extern exsearch  gSearch;
extern floating  gFloating;

extern CStrList  gTimeouts;
extern CStrList  subject_list;
extern CStrList  actdisplist;
extern CStrList  eventlist;
extern CStrList  income_times;
extern CStrList  remote;

CStrList  isdnlist;
CStrList  gAkaList;
CString   isdnaccept;

extern int		building_subjlist;
extern int		current_mail_index;
extern int		font_specific_sizing_offset;
extern CString	actual_executed_script;
extern int		gLastUpdate;
extern LangID	LANG_ID;
extern _gcomm	gcomm;
extern int		came_from_button;

CWinThread *tp;
CWinThread *tM;
CWinThread *tt;
CWinThread *evtth;
CWinThread *UDPth;
CWinThread *Incometh;

extern	CWinThread *th_nl;
extern	CWinThread *script_thread;
extern	int tcpip_was_used;
extern  CStrList	gWebAddrList;

CString fwdstring;
CString StoredFido;
CString StoredFile;

extern	CString gLastCalled;
extern  CFont   font_labels;
extern  CFont   font_fields;
//extern  CFont   font_editor;
extern  CFont   font_mailtext;
extern  CFont   font_subject;
extern  CFont   font_ANSI;
extern  CFont   font_OEM;
int		numrcv=0;

_udp_packet rcvpacket;
_gconfig		gc;
_detincome	gincome;			// Holds all the global stuff for INCOME HAndling
_watchmail	watchmail;			// Die Mail nach der gekuckt werden soll ...
_magicid	magicid;			// MAGIC ID Struktur
_curmail	curmail;			// Infos's on current mail session
_rfilter	grfilter;       	// Globales Reader Filter
_ustat		gustat;				// Holds all the stuff for user-interaction    
HANDLE		mutex_tosspurg;     // Mutex to excutise TOSSER <-> PURGER
ULONG		*notbremse;     		// Notbremse array
char		typebuffer[25];			// for jokes searches
char		gMailbuf[2*MAX_MSG];	// current Mailtext (for expansion with '\n')
long		last_modemsession_time;	// Unixtime der letzten Anwahl ...
long		gColors[20];			// global colortable
int 		notbremscounter;		// Wieviele Eintraeg sind guetig ..
int			jumpstack[100];	        // Jump back stack
int			jumpcount;				// jump back stackcounter
int 		gLastSucessfulCallTime;	// Letzte erfolgreiche MAIL Session
int 		gNewAreaFromExtSearch;
char		oldpath[MAX_PATH];
char		uufname[MAX_PATH];

int			entry[100];
int			selcnt=0;
int			gSortCriteria=0;
int			gIncomeDevicesHaveChanged=0;
int			doexit=0;

UINT	UDPWorkerThread		(LPVOID p);
UINT	EventWorkerThread	(LPVOID p);
UINT	IncomeWorkerThread	(LPVOID p);
void	find_weblinks		(LPSTR text);
void	load_textwnd_colors	();
void	store_textwnd_colors();
void	save_mail_to_file	(BOOL selected);
void	check_sort			(int id);
int		prepare_repost		(int delay);
BOOL CALLBACK EnumWindowsProc(HWND hw,LPARAM lp);

CString ComPort1_IncoThread;
CString ComPort2_IncoThread;
CString ModemInit1_IncoThread;
CString ModemInit2_IncoThread;

// declares for UniPack calls
HINSTANCE hUni,hRar,hCab,hAce,hRes;
HANDLE  gEventToKillUDP=INVALID_HANDLE_VALUE;

// class for parsing command line

class MyCommandLineInfo : public CCommandLineInfo
{
public:
	void ParseParam(LPCTSTR lpszParam,BOOL bFlag,BOOL bLast);
};

void MyCommandLineInfo::ParseParam(LPCTSTR lpszParam,BOOL bFlag,BOOL bLast)
{
	if (!stricmp(lpszParam,"init"))
		gc.init=1;
	else if (!stricmp(lpszParam,"nosplash"))
		gc.nosplash=1;
	else if (!stricmp(lpszParam,"dl1"))
		gc.displaylevel=1;
	else if (!stricmp(lpszParam,"dl2"))
		gc.displaylevel=2;
	else if (!stricmp(lpszParam,"dl3"))
		gc.displaylevel=3;
	else if (!stricmp(lpszParam,"recycle"))
		gc.UseRecycled=1;
	else if (!stricmp(lpszParam,"ro"))
		gc.ReadOnly=1;
	else if (!strnicmp(lpszParam,"s",1))
		strcpy(gc.autoscript,lpszParam+1);
	else if (!strnicmp(lpszParam,"c",1))
		strcpy(gc.IniPath,lpszParam+1);
}

/////////////////////////////////////////////////////////////////////////////
// CLightApp

BEGIN_MESSAGE_MAP(CLightApp, CWinApp)
	//{{AFX_MSG_MAP(CLightApp)
	ON_COMMAND(ID_SCALE_10, OnScale10)
	ON_COMMAND(ID_SCALE_20, OnScale20)
	ON_COMMAND(ID_SCALE_30, OnScale30)
	ON_COMMAND(ID_SCALE_40, OnScale40)
	ON_COMMAND(ID_SCALE_50, OnScale50)
	ON_COMMAND(ID_SCALE_60, OnScale60)
	ON_COMMAND(ID_SCALE_70, OnScale70)
	ON_COMMAND(ID_SCALE_80, OnScale80)
	ON_COMMAND(ID_SCALE_90, OnScale90)
	ON_COMMAND(ID_SCALE_100, OnScale100)
	ON_COMMAND(ID_MENU_DIRECTRIES, OnCfgStation)
	ON_COMMAND(ID_MENU_BOSSCONFIG, OnCfgBosses)
	ON_COMMAND(ID_CFG_EDITOR, OnMenuEditor)
	ON_COMMAND(ID_MENU_AREAS, OnMenuAreas)
	ON_COMMAND(ID_MENU_PACKER, OnMenuPacker)
	ON_COMMAND(ID_STARTMAILER, OnStartmailer)
	ON_COMMAND(ID_STARTTOSSER, OnStarttosser)
	ON_COMMAND(ID_STARTPURGER, OnStartpurger)
	ON_COMMAND(ID_STARTNLCOMPILE, OnStartnlcompile)
	ON_COMMAND(ID_CFG_SYSTEMSOUNDS, OnSetupSystemsounds)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_SELECTALL, OnEditSelectall)
	ON_COMMAND(ID_MAILCOPY, OnMailCopy)
	ON_COMMAND(ID_CFG_NEWMAIL, OnSetupNewmail)
	ON_COMMAND(ID_MENU_MODEM, OnMenuModem)
	ON_COMMAND(ID_FILEREQUEST, OnFilerequest)
	ON_COMMAND(ID_MENU_NODELIST, OnMenuNodelist)
	ON_COMMAND(ID_WINDOWSTYLE, OnReadercolor)
	ON_COMMAND(ID_CFG_SCRIPTEDITOR, OnSetupScripteditor)
	ON_COMMAND(ID_FILE_EXIT, OnFileExit)
	ON_COMMAND(ID_CHANGEMAIL, OnActionChangecurrentmail)
	ON_COMMAND(ID_CFG_DIALINGTIMEOUTS, OnSetupDialingtimeouts)
	ON_COMMAND(ID_CARBONCOPY, OnSetupCarboncopystrings)
	ON_COMMAND(ID_ECHOLISTMANAGEMENT, OnActionEcholistmanagement)
	ON_COMMAND(ID_AREATRAFFICINFO, OnInfoAreatrafficinfo)
	ON_COMMAND(ID_HELPME, OnHelpMe)
	ON_COMMAND(ID_FIDONET, OnFIDONET)
	ON_COMMAND(ID_FIDOLANG, OnFIDOLANG)
	ON_COMMAND(ID_DICTIONARY, OnDICTIONARY)
	ON_COMMAND(ID_CFG_OTHERCONFIGS, OnConfigOtherconfigs)
	ON_COMMAND(ID_HELP_ABOUT, OnHelpAbout)
	ON_COMMAND(ID_ADDRESSINFO, OnInfoAddressinfo)
	ON_COMMAND(ID_FORWARDMAIL, OnForward)
	ON_COMMAND(ID_EDIT_PRINTMAIL, OnPrintMail)
	ON_COMMAND(ID_SAVESELTOFILE, OnSaveSelToFile)
	ON_COMMAND(ID_SAVEMAILTOFILE, OnSaveMailToFile)
	ON_COMMAND(ID_VIEWLOGFILES, OnInfoViewlogfiles)
	ON_COMMAND(ID_LOGFILE_CFG, OnSetupLogfilesettings)
	ON_COMMAND(ID_CFG_ADDITIONALAKAS, OnSetupAdditionalakas)
	ON_COMMAND(ID_CFG_DISPLAYFILTER, OnFilterDisplay)
	ON_COMMAND(ID_MARKALLASREAD, OnMarkAllMailsAsRead)
	ON_COMMAND(ID_CFG_SUBJLISTFONT, OnSetSubjectFont)
	ON_COMMAND(ID_MAILTEXTFONT_ANSI, OnSetMailtextFontANSI)
	ON_COMMAND(ID_MAILTEXTFONT_OEM, OnSetMailtextFontOEM)
	ON_COMMAND(ID_ABORTSCRIPT, OnActionAbortscript)
	ON_COMMAND(ID_NEVERPURGETHISMAIL, OnNeverDelete)
	ON_COMMAND(ID_QUOTE_NETMAIL, OnMailsAnswerasnetmail)
	ON_COMMAND(ID_RESCANMAIL, OnRescanmail)
	ON_COMMAND(ID_UMLAUTCONVERT, OnConvertSpecialChars)
	ON_COMMAND(ID_CROSSPOST, OnCrosspost)
	ON_COMMAND(ID_VIEWOUTBOUND, OnViewOutbound)
	ON_COMMAND(ID_HELPMAINSCREEN, OnHelp)
	ON_COMMAND(ID_CFG_TRASHCANFILTER, OnSetupTrashcanfilter)
	ON_COMMAND(ID_AUTOSAVEMAIL, OnAutosave)
	ON_COMMAND(ID_QUOTE_A, OnQuoteToArea)
	ON_COMMAND(ID_QUOTE, OnQuote)
	ON_COMMAND(ID_SWITCH_LEFTICONS, OnSwitchLeftIcons)
	ON_COMMAND(ID_SWITCH_MAILER, OnSwitchMailer)
	ON_COMMAND(ID_SWITCH_TOSSER, OnSwitchTosser)
	ON_COMMAND(ID_SWITCH_PURGER, OnSwitchPurger)
	ON_COMMAND(ID_SWITCH_NODELIST, OnSwitchNodelist)
	ON_COMMAND(ID_SWITCH_EXTSEARCH, OnSwitchExtSearch)
	ON_COMMAND(ID_SIMPLE_SEARCH, OnSimpleSearch)
	ON_COMMAND(ID_STARTSCRIPT1, OnActionsStartscript1)
	ON_COMMAND(ID_STARTSCRIPT2, OnActionsStartscript2)
	ON_COMMAND(ID_STARTSCRIPT3, OnActionsStartscript3)
	ON_COMMAND(ID_STARTSCRIPT4, OnActionsStartscript4)
	ON_COMMAND(ID_STARTSCRIPT5, OnActionsStartscript5)
	ON_COMMAND(ID_FREQFROMMAIL, OnFreqFromMail)
	ON_COMMAND(ID_MAILS_CREATENEWMAILINS, OnMailCreateNewMail)
	ON_COMMAND(ID_DELETE_ALL, OnDeleteAll)
	ON_COMMAND(ID_CFG_EVENTS, OnConfigEvents)
	ON_COMMAND(ID_MENU_MODEM2, OnMenuModem2)
	ON_COMMAND(ID_CFG_EDITORTYPEREPLACMENT, OnSetupEditortypereplacment)
	ON_COMMAND(ID_CFG_TEXTIMPORT, OnSetupTextimport)
	ON_COMMAND(ID_ABORTEVENT, OnActionsAbortcurrentevent)
	ON_COMMAND(ID_EXECUTEANYSCRIPT, OnActionsExecuteanyscript)
	ON_COMMAND(ID_MUL_AS_PRINT, OnMulAsPrint)
	ON_COMMAND(ID_MUL_AS_READ, OnMulAsRead)
	ON_COMMAND(ID_SAVE_MULTIPLE, OnSaveMultipleToFile)
	ON_COMMAND(ID_MUL_AUTOSAVE, OnMulAutosave)
	ON_COMMAND(ID_MUL_DELETE, OnMulDelete)
	ON_COMMAND(ID_MUL_NEVER_DELETE, OnMulNeverDelete)
	ON_COMMAND(ID_TOGGLEROT13, OnToggleRot13)
	ON_COMMAND(ID_CHECKDUPES_ALL, OnDupeCheckAll)
	ON_COMMAND(ID_CHECKDUPES_CURRENT, OnDupeCheck)
	ON_COMMAND(ID_INCOMINGDEVICES, OnConfigTimesAndDevices)
	ON_COMMAND(ID_INCOMINGNUMOFRINGS, OnIncommingcallsConfigNrofrings)
	ON_COMMAND(ID_MMDISPLAYOPTIONS, OnMultimediaDisplayoptions)
	ON_COMMAND(ID_MMAVAILABLEELEMENTS, OnMultimediaAccessibleelements)
	ON_COMMAND(ID_CAPTURE_RECIPIENT, OnCaptureRecipientAddr)
	ON_COMMAND(ID_CAPTURE_SENDER, OnCaptureSenderAddr)
	ON_COMMAND(ID_SUBJECT_ADJUST, OnSubjListAdjustment)
	ON_COMMAND(ID_REQUESTPATHS, OnSetupRequestPathes)
	ON_COMMAND(ID_MAGICS, OnSetupMagics)
	ON_COMMAND(ID_REMOTEMONITOR, OnIncomingcallsRemotemonitor)
	ON_COMMAND(ID_MARK_MAIL_FREEZE, OnMarkMailFreeze)
	ON_COMMAND(ID_CFG_TICECHOSETUP, OnSetupTicechosetup)
	ON_COMMAND(ID_HATCHAFILE, OnHatchFile)
	ON_COMMAND(ID_BEGINMULTIUUE, OnBeginMultiSelection)
	ON_COMMAND(ID_ENDMULTIUUE, OnEndMultiSelection)
	ON_COMMAND(ID_UUFROMSELECTED, OnUUFromSelected)
	ON_COMMAND(ID_STARTWWWBROWSER, OnStartWWWBrowser)
	ON_COMMAND(ID_UNMARKALLMESSAGES, OnUnmarkAllMessages)
	ON_COMMAND(ID_COSTCALCULATION, OnInfoCostcalculation)
	ON_COMMAND(ID_COSTANALYZE, OnInfoCostanalyze)
	ON_COMMAND(ID_FINDORIGINAL, OnFindOriginalCopy)
	ON_COMMAND(ID_INOUTCALLHISTORY, OnCallsHistory)
	ON_COMMAND(ID_CFG_FAX, OnFaxSetup)
	ON_COMMAND(ID_LOCALLOGIN, OnLocallogin)
	ON_COMMAND(ID_CFG_EDITORFONT, OnChooseFontEditor)
	ON_COMMAND(ID_CFG_LABELSFONT, OnFontMainLabels)
	ON_COMMAND(ID_CFG_FIELDSFONT, OnFontMainFields)
	ON_COMMAND(ID_ASAVETOAREA, OnAutosaveToArea)
	ON_COMMAND(ID_POLLASYSTEM, OnPollSystem)
	ON_COMMAND(ID_SAVEMARKEDTOAREA, OnSaveMarkedToSpecificArea)
	ON_COMMAND(ID_CFG_BBSGROUPS, OnBbsusergroupsetup)
	ON_COMMAND(ID_CFG_BBS, OnBbsmainconfig)
	ON_COMMAND(ID_BBSUSERCONFIG, OnBbsuserconfig)
	ON_COMMAND(ID_BBSAREACONFIG, OnBbsareaconfig)
	ON_COMMAND(ID_ORDERECHOES, OnOrderEchoes)
	ON_COMMAND(IDM_ALLAREASREAD, OnAllareasread)
	ON_COMMAND(ID_SORT_RECIEVE, OnSortRecieve)
	ON_COMMAND(ID_SORT_CREATE, OnSortCreate)
	ON_COMMAND(ID_SORT_FROM, OnSortFrom)
	ON_COMMAND(ID_SORT_TO, OnSortTo)
	ON_COMMAND(ID_SORT_SUBJECT, OnSortSubject)
	ON_COMMAND(ID_SORT_STATUS, OnSortStatus)
	ON_COMMAND(ID_SORT_ORDER, OnSortOrder)
	ON_COMMAND(ID_HSFLOATING, OnHsfloating)
	ON_COMMAND(ID_RESET_DLGPOS, OnResetdialogboxpos)
	ON_COMMAND(ID_CFG_FLOATING, OnCfgFloating)
	ON_COMMAND(ID_CFG_ADRESSBOOK, OnSetupAdressbook)
	ON_COMMAND(ID_INCOMING_MODEM1, OnIncomingModem1)
	ON_COMMAND(ID_INCOMING_MODEM2, OnIncomingModem2)
	ON_COMMAND(ID_MAILPRESENTAGAIN, OnMarkPresentAgain)
	ON_COMMAND(ID_PRESENTAGAIN, OnInfoPresentagain)
	ON_COMMAND(ID_DELETE_MAIL, OnDeleteMail)
	ON_COMMAND(ID_CFG_KLUDGES, OnCfgKludges)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// The one and only CLightApp object

CLightApp FipsApp;

/////////////////////////////////////////////////////////////////////////////
// CLightApp construction
	CLightApp::CLightApp(){}

/////////////////////////////////////////////////////////////////////////////
// CLightApp initialization
		
BOOL CLightApp::InitInstance()
{
HANDLE  hf;
CString str;
char	ipath[MAX_PATH],*p=NULL;
MyCommandLineInfo cmd;

	load_textwnd_colors();
	setlocale(LC_ALL,"ru");
	magicid.affe1	=0xAFFEAFFE;
	magicid.affe2	=0xAFFEAFFE;
	magicid.count1=0x01020304;
	magicid.count2=0x04030201;
	memset(&gc,0,sizeof(gc));   // Init global config struct
	ParseCommandLine(cmd);

// check presence of fips.ini
	if (gc.IniPath[0]==0)	get_exepath(gc.IniPath);	// cmd param is absent
	_mkdir(gc.IniPath);
	make_path(ipath,gc.IniPath,INIFILENAME);
	free((void*)m_pszProfileName);
	hf=CreateFile(ipath,GENERIC_READ,0,0,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
	//set_cfg(CFG_COMMON, "Floatbar", 1, TRUE);
	//set_cfg(CFG_COMMON, "Initialization", 1, TRUE);
	if (hf!=INVALID_HANDLE_VALUE)	CloseHandle(hf);
	m_pszProfileName=_tcsdup(ipath);

// load work pathes from ini to global struct, by default is the path to exe-module


// if FIPS already running, switch to it
	if (!access(m_pszProfileName,0))	// no errors while open
	{
		make_path(ipath,gc.IniPath,"MSGBASE");	// default message path at ini folder
		str=GetProfileString("Common","MsgBasePath",ipath);
		if (str.GetLength())
		{
			str+="\\00000000." EXT_HEADER;
			hf=CreateFile(str,GENERIC_READ,NULL,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
			if (GetLastError()==ERROR_SHARING_VIOLATION)
			{
				EnumWindows(&EnumWindowsProc,NULL);
				return FALSE;
			}
			if (hf!=INVALID_HANDLE_VALUE)	CloseHandle(hf);
		}
	}
	else
	{
		AfxMessageBox(IDS_CFGNOTAVAIL);
		return FALSE;
	}
	AfxInitRichEdit();			// initialization for RichEdit controls
	WNDCLASS wc;
	wc.style = 0;
	wc.lpfnWndProc = ::DefWindowProc;
	wc.cbClsExtra = wc.cbWndExtra = 0;
	wc.hInstance = m_hInstance;
	wc.hIcon = 0;
	wc.hCursor = ::LoadCursor(0, MAKEINTRESOURCE(IDC_IBEAM));
	wc.hbrBackground = 0;
	wc.lpszMenuName = 0;
	wc.lpszClassName = "FreeprogEditwnd";
	RegisterClass(&wc);
// Standard initialization
// Load color description table
//	Enable3dControls();
	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	GetCurrentDirectory(MAX_PATH,oldpath);
	SetCurrentDirectory(gc.IniPath);

// calling QUICK Setup IF not exists fips.ini OR is parameter 'init' in command line OR
// Initialization=1 in section Common of fips.ini OR empty message base path
	if (access(INIFILENAME,0) || get_cfg(CFG_COMMON,"Initialization",0)	|| gc.init) //gc.MsgBasePath[0]==0  ||
	{
		quickset dlg;
	    if (dlg.DoModal()==IDCANCEL)
		   return FALSE;
		else
		{
 			FipsApp.WriteProfileString(get_sect_name(CFG_COMMON),"Initialization",0);
			set_cfg(CFG_COMMON,"LeftToolbar",1,TRUE);
		}
	}
// check password
	if (get_cfg(CFG_COMMON,"Entry",0) != 0)
	{
		guard grd;
		grd.DoModal();
		if (!grd.m_allow)	return FALSE;
	}
	
	//migrated from 410 line
	get_folder_path(gc.IniPath, "", "BasePath", gc.BasePath);
	get_folder_path(gc.IniPath, "MsgBase", "MsgBasePath", gc.MsgBasePath);
	get_folder_path(gc.IniPath, "Outbound", "OutboundPath", gc.OutboundPath);
	get_folder_path(gc.IniPath, "Inbound", "InboundPath", gc.InboundPath);
	get_folder_path(gc.IniPath, "Multimed", "MultimedPath", gc.MultimedPath);
	get_folder_path(gc.IniPath, "Nodelist", "NodelistPath", gc.NodelistPath);
	get_folder_path(gc.IniPath, "Util", "UtilPath", gc.UtilPath);
	
	backup_wwd(AREADBF,AREADBFS,TRUE);	// backup afterstarting state
	if (db_open_area_descriptions(1)!=DB_OK)
		ERR_MSG_RET0("E_OPENADESC");
	if (db_create_default_areas()!=DB_OK)
		ERR_MSG_RET0("E_CANOTCRDEFAULT");
	
	hUni=LoadLibrary("UniPack");
	hRar=LoadLibrary("UnRar");
	hCab=LoadLibrary("Cabinet");
	hAce=LoadLibrary("UnAce");
	str=get_cfg(CFG_COMMON,"SkinPath","");
	if (str.GetLength())
		hRes=LoadLibrary(str);
	else
		hRes=FipsApp.m_hInstance;

// call main dialog
	CLightDlg dlg;
	m_pMainWnd = &dlg;
	dlg.DoModal();
	SetCurrentDirectory(oldpath);
	return FALSE;
}

void CLightApp::OnScale10()
{   gc.scale=gc.last_scale=10;  check_scale(ID_SCALE_10);  }
void CLightApp::OnScale20()
{   gc.scale=gc.last_scale=20;  check_scale(ID_SCALE_20);  }
void CLightApp::OnScale30()
{   gc.scale=gc.last_scale=30;  check_scale(ID_SCALE_30);  }
void CLightApp::OnScale40()
{   gc.scale=gc.last_scale=40;  check_scale(ID_SCALE_40);  }
void CLightApp::OnScale50()
{   gc.scale=gc.last_scale=50;  check_scale(ID_SCALE_50);  }
void CLightApp::OnScale60()
{   gc.scale=gc.last_scale=60;  check_scale(ID_SCALE_60);  }
void CLightApp::OnScale70()
{   gc.scale=gc.last_scale=70;  check_scale(ID_SCALE_70);  }
void CLightApp::OnScale80()
{   gc.scale=gc.last_scale=80;  check_scale(ID_SCALE_80);  }
void CLightApp::OnScale90()
{   gc.scale=gc.last_scale=90;  check_scale(ID_SCALE_90);  }
void CLightApp::OnScale100()
{   gc.scale=gc.last_scale=100; check_scale(ID_SCALE_100);}

// =====================================================================================
	void check_scale(int id)
// =====================================================================================
{
CMenu	*cm;
//RECT	re;

	cm=gpMain->GetMenu();
	cm->CheckMenuItem(ID_SCALE_10, MF_BYCOMMAND | MF_UNCHECKED);
	cm->CheckMenuItem(ID_SCALE_20, MF_BYCOMMAND | MF_UNCHECKED);
	cm->CheckMenuItem(ID_SCALE_30, MF_BYCOMMAND | MF_UNCHECKED);
	cm->CheckMenuItem(ID_SCALE_40, MF_BYCOMMAND | MF_UNCHECKED);
	cm->CheckMenuItem(ID_SCALE_50, MF_BYCOMMAND | MF_UNCHECKED);
	cm->CheckMenuItem(ID_SCALE_60, MF_BYCOMMAND | MF_UNCHECKED);
	cm->CheckMenuItem(ID_SCALE_70, MF_BYCOMMAND | MF_UNCHECKED);
	cm->CheckMenuItem(ID_SCALE_80, MF_BYCOMMAND | MF_UNCHECKED);
	cm->CheckMenuItem(ID_SCALE_90, MF_BYCOMMAND | MF_UNCHECKED);
	cm->CheckMenuItem(ID_SCALE_100, MF_BYCOMMAND | MF_UNCHECKED);
	cm->CheckMenuItem(id, MF_BYCOMMAND | MF_CHECKED);
	gc.scaleupdate=TRUE;
//	gpMain->GetClientRect(&re);
//	gpMain->InvalidateRect(&re);
	gpMain->Invalidate();
}

// =====================================================================================
	void check_sort(int id)
// =====================================================================================
{
CMenu *cm;
HICON hIcon;
char str[10];

	cm=gpMain->GetMenu();
	hIcon = FipsApp.LoadIcon(gc.SortCriteria & SORT_ORDER ? IDI_ARROWUP : IDI_ARROWDN);
	gpMain->m_statusbar.SetIcon(1, hIcon);
	cm->CheckMenuItem(id,MF_BYCOMMAND | (gc.SortCriteria & SORT_ORDER ? MF_CHECKED : MF_UNCHECKED));
	if (id != ID_SORT_ORDER)
	{
		cm->CheckMenuItem(ID_SORT_RECIEVE, MF_BYCOMMAND | MF_UNCHECKED);
		cm->CheckMenuItem(ID_SORT_CREATE, MF_BYCOMMAND | MF_UNCHECKED);
		cm->CheckMenuItem(ID_SORT_FROM, MF_BYCOMMAND | MF_UNCHECKED);
		cm->CheckMenuItem(ID_SORT_TO, MF_BYCOMMAND | MF_UNCHECKED);
		cm->CheckMenuItem(ID_SORT_SUBJECT, MF_BYCOMMAND | MF_UNCHECKED);
		cm->CheckMenuItem(ID_SORT_STATUS, MF_BYCOMMAND | MF_UNCHECKED);
		cm->CheckMenuItem(id, MF_BYCOMMAND | MF_CHECKED);
		switch(id) {
		case ID_SORT_RECIEVE:
			strcpy(str,L("S_375"));	// receive
			break;
		case ID_SORT_CREATE:
			strcpy(str,L("S_148"));	// creation
			break;
		case ID_SORT_FROM:
			strcpy(str,L("S_264"));	// from
			break;
		case ID_SORT_TO:
			strcpy(str,L("S_424"));	// to
			break;
		case ID_SORT_SUBJECT:
			strcpy(str,L("S_416"));	// subject
			break;
		case ID_SORT_STATUS:
			strcpy(str,L("S_311"));	// status
			break;
		default:
			*str=0;
		}
		gpMain->m_statusbar.SetText(str,1,0);
	}
}

// ==================================================
	void CLightApp::OnCfgStation()
// ==================================================
{
	cfg_dir dlg;
	dlg.DoModal();
}

// ==================================================
	void CLightApp::OnCfgBosses()
// ==================================================
{
	cfg_boss dlg;
	dlg.DoModal();
}

// ==================================================
	void CLightApp::OnMenuEditor()
// ==================================================
{
	cfg_edit dlg;
	if (dlg.DoModal()==IDOK)
		gpMain->show_mail(&gustat);

	gpMain->m_subjlist.SetFocus();
}

// ==================================================
	void CLightApp::OnMenuAreas()
// ==================================================
{
	cfg_area dlg;
	dlg.DoModal();
}

// ==================================================
	void CLightApp::OnMenuPacker()
// ==================================================
{
	cfg_pack dlg;
	dlg.DoModal();
}

// ==================================================
	UINT NlCompMainThread(LPVOID p)
// ==================================================
{
	gc.detnl.running=1;
	system_sound("NLCOMP_START");
    nl_compile();
	system_sound("NLCOMP_END");
	th_nl->Delete();
	gc.detnl.running=0;
	ExitThread(9);
	return 1;
}

// ==================================================
	UINT PurgerThread(LPVOID p)
// ==================================================
{
int num;

	gc.purger.running=1;
	gPurger.AddString(L("S_419"));	// synchronization
	WaitForSingleObject(mutex_tosspurg,INFINITE);

	while (gc.areasetup_active)
	{
		gPurger.AddString(L("S_462"));	// waiting for exit area setup
		Sleep(3000);
	}

	system_sound("PURGER_START");
	backup_wwd(AREADBF,AREADBFP,TRUE);	// backup prepurging state
	gpMain->m_anipurg.Play(0,-1,-1);
	gPurger.AddString(L("S_414"));	// start purge
	db_get_count_of_areas(&num);
	if (gc.purger.area_index<0)
		purge_one_or_more_areas(0,num-1);
	else if (gc.purger.area_index>=0 && gc.purger.area_index<num)
		purge_one_or_more_areas(gc.purger.area_index,gc.purger.area_index);
	else
		return 1;

	gPurger.AddString(L("S_453"));	// update info
	db_refresh_areas_info();

	gpMain->m_anipurg.Stop();
	gpMain->m_anipurg.Seek(0);

	ReleaseMutex(mutex_tosspurg);
    gPurger.AddString(L("S_193"));	// ended
    gPurger.AddString("");
	system_sound("PURGER_END");
	tp->Delete();
	gc.purger.running=0;
	ExitThread (10);
	return 1;
}

// ==================================================
	UINT ScriptWorkerThread(LPVOID p)
// ==================================================
{
    gc.detscript.abort=0;
	actual_executed_script.Empty();
	if (!gc.HideIcons)
		SHOW(gpMain->m_abortscript);
	gc.detscript.running=1;
	system_sound("SCRIPT_START");
	gpMain->execute_script(gc.detscript.scriptname);
	gc.detscript.disableupdate=0;
	if (!gc.HideIcons)
		HIDE(gpMain->m_abortscript);
	system_sound("SCRIPT_END");
	script_thread->Delete();
	gc.detscript.running=0;
	ExitThread(11);
	return 1;
}


// ==================================================
	UINT MailerThread(LPVOID p)
// ==================================================
{
_fidonumber nlnumber;
CString inform,line,number,phone,flaghelp,fidoadr;
CString help,help1,help2,help3,help4;
char	hbuf[300];
const char *p2;
int		ret,y,n,e,allcnt,trycount,tryc,waittime,loopcount,oldnum;
int		zone,net,node,point,zone1,net1,node1,point1;
int		nohydra=0,nozmod=0,nopoll=0,tcpip=0;

	gc.mailer.running=1;
	handle_mailer_ctls(gc.mailer.running);
  selcnt=0;
	trycount= atoi(gTimeouts.GetString(0));
	waittime= atoi(gTimeouts.GetString(1));
	loopcount=atoi(gTimeouts.GetString(2));

	WaitForSingleObject(mutex_tosspurg,INFINITE);
 	system_sound("MAILER_START");
	gpMain->m_animail.Play(0,-1,-1);

	allcnt=gMailer.m_queue.GetCount();
	if (allcnt==0)	goto endit;
	selcnt=gMailer.m_queue.GetSelItems(100,entry);
	if (selcnt==0)	// select entries if nothing selected
	{
		for (int i=0;i<allcnt;i++)
		{
			gMailer.m_queue.GetText(i,help);
			get_token(help,1,help1);
			if (is_work_time(help1))	gMailer.m_queue.SetSel(i,1);	// select only working stations
		}
		selcnt=gMailer.m_queue.GetSelItems(100,entry);
	}

	for (y=0;y<loopcount;y++)
	{
		if (selcnt==0)	break;

	 	for (int x=0;x<selcnt;x++)
		{
			if (selcnt==0)	break;

		  for (tryc=0;tryc<trycount;tryc++) // how often is each entry tried ...
			{
				if (selcnt==0)	break;
				if (!prepare_one_entry_for_dialing(entry[x],&curmail))	break;

				gMailer.m_queue.GetText(entry[x],line);
				get_token(line,0,number);
				get_token(line,1,fidoadr);
                trim_all(fidoadr);
				db_get_boss_flags(fidoadr,&nopoll,&nohydra,&nozmod,&tcpip);
				gLastCalled=fidoadr;

				oldnum=0;
				sscanf(number,"%d",&oldnum);
				set_queue_mark(entry[x],">>");
				highlight_current_selected();
				gc.mailer.skip_only=0;   // Reset Skip
				gAkaList.RemoveAll();

				phone=curmail.phone;
				if (replace_phone_number(phone))
					gMailer.OnAddToListBox(L("S_422",phone));	// TelNr. translated to:%s

				if (isdnlist.defaultindexnew)
				{
					get_token(line,1,help3);
					parse_address(help3,&zone1,&net1,&node1,&point1);
					if (nl_get_fido_by_number(zone1,net1,node1,point1,&nlnumber))
					{
						for (e=0;e<isdnlist.GetCount();e++)
						{
							help4=isdnlist.GetString(e);
							help4.MakeUpper();
							flaghelp=nlnumber.flags;
							flaghelp.MakeUpper();
							p2=strstr(flaghelp,help4);
							if (p2)
							{
								if (phone[0]!='$')
								    phone="$"+phone;
								modem_add_listbox(L("S_285"));	// ISDN Line identified
								break;
							}
						}
					}
				}

// mailer start
// to do: alternatively call external mailer here?
//        or use call it instead of the outer loop   for (y=0;y<loopcount;y++)  ?
				if (tcpip) 
					ret=binkd(line);
				else
			   	ret=outgoing_emsi_session(phone,curmail.passwd,
					curmail.basedir,curmail.myaka,nopoll,nohydra,nozmod);
				gcomm.ConnectedOn1Modem=gcomm.ConnectedOn2Modem=0;
				gcomm.DialingViaSecondModem=0;
//				free_capi_stuff();
//				gc.mailer.keyboard_break=0;
				if (ret != MODEM_OK)
				{
					// init-error: set all counter to limit -> no retry!
					if (ret==MODEM_CANNOT_INIT) goto endit;

					number.Format("%d",++oldnum);
					set_queue_mark(entry[x],number);

					if (!gc.mailer.keyboard_break)
						inform=L("S_461",waittime);
					highlight_current_selected();
					gMailer.OnAddToListBox(PS inform);

					trycount = atoi(gTimeouts.GetString(0));
					waittime = atoi(gTimeouts.GetString(1));
					loopcount= atoi(gTimeouts.GetString(2));
					for (int w=0;w<(waittime*2);w++)
					{
						if (!gc.mailer.keyboard_break)
							Sleep(500);
						else
						{
							gc.mailer.keyboard_break=3;
							if (gc.mailer.skip_only)
							{
								gMailer.OnAddToListBox(L("S_350"));	// Number skipped
								goto next_number;
							}
							else
								goto endit;
						}
				   }
				}
				else	// MODEM OK
				{
					write_lastcall_idx();
				    set_queue_mark(entry[x],"OK");
					remove_successful_call(x);

newloop:
					for (int xx=0;xx<selcnt;xx++)
					{
						gMailer.m_queue.GetText(entry[xx],help1);
						get_token(help1,1,help2);

						zone=node=net=point=0;
						parse_address(help2,&zone,&node,&net,&point);
						sprintf(hbuf,"%d:%d/%d.%d",zone,node,net,point);

						for (int a=0;a<gAkaList.GetCount();a++)
						{
						    if (!stricmp(gAkaList.GetString(a),hbuf))
							{
							    set_queue_mark(entry[xx],"OK");
								remove_successful_call(xx);
								goto newloop;
							}
						}
					}

				  highlight_current_selected();
					trycount=atoi(gTimeouts.GetString(0));
					waittime=atoi(gTimeouts.GetString(1));
					loopcount=atoi(gTimeouts.GetString(2));
					if (selcnt>0)
					{
						for (int w=0;w<(waittime*2);w++)
						{
							if (gc.mailer.skip_only)
							{
								gMailer.OnAddToListBox(L("S_465"));	// Waiting skipped
								gc.mailer.keyboard_break=0;
								gc.mailer.skip_only=0;
								break;
							}
							if (gc.mailer.keyboard_break)
							{
								gc.mailer.keyboard_break=3;
								goto endit;
							}
							Sleep(500);
						}
					}
				}

next_number:
				if (gc.mailer.skip_only)	 // Break here to dial next number
				{
					gc.mailer.skip_only=0;
					gc.mailer.keyboard_break=0;
				    break;
				}
			}

		}
	}
endit:

	gc.mailer.skip_only	  =0;
	gc.mailer.keyboard_break=0;
// reset selection and clear marks
	gMailer.m_queue.SetSel(-1,0);
	for (n=0;n<gMailer.m_queue.GetCount();n++)
		set_queue_mark(n,"");

	check_inbound();
	if (check_for_nodediffs())
	{
		if (gc.detscript.running)
			show_msg(L("S_605"),2000);	// new nodelist found
		else if (show_msg_t("DY_COMPNODE",1,10,L("S_286"),L("S_322")))	// yes, no
			gNLComp.OnStart();
	}

 	ReleaseMutex(mutex_tosspurg);
	gpMain->m_animail.Stop();
	gpMain->m_animail.Seek(0);
	system_sound("MAILER_END");
    gMailer.OnAddToListBox(L("S_194"));	// ending mailer
    gMailer.OnAddToListBox("");

	if (get_cfg(CFG_COMMON,"TosserStart",1))	// automatic tosser start
	   gTosser.OnStart();

	tM->Delete();

	// Free Memory for strings ...
	inform.Empty(); inform.FreeExtra();
	line.Empty(); line.FreeExtra();
	number.Empty(); number.FreeExtra();
	help.Empty(); help.FreeExtra();
	help1.Empty(); help1.FreeExtra();
	help2.Empty(); help2.FreeExtra();
	help3.Empty(); help3.FreeExtra();
	help4.Empty(); help4.FreeExtra();
	phone.Empty(); phone.FreeExtra();
	flaghelp.Empty(); flaghelp.FreeExtra();
	fidoadr.Empty(); fidoadr.FreeExtra();

//	(pml->m_startstop).SetFocus();
	gc.mailer.running=0;
	handle_mailer_ctls(gc.mailer.running);
	ExitThread(1);
	return 1;
}

// ==================================================
	void remove_successful_call(int ind)
// ==================================================
{
	for (int i=0;i<selcnt;i++)
		if (i>ind)
			entry[i-1]=entry[i];
	if (selcnt>0)
		selcnt--;
}

// ==================================================
	void highlight_current_selected(void)
// ==================================================
{
int x;

    gMailer.m_queue.SetRedraw(0);
 	for (x=0;x<gMailer.m_queue.GetCount();x++)
	   gMailer.m_queue.SetSel(x,0);

	for (x=0;x<selcnt;x++)
	   gMailer.m_queue.SetSel(entry[x],1);
    gMailer.m_queue.SetRedraw(1);
    gMailer.update_queue();
}

// ==================================================
	UINT TosserThread(LPVOID p)
// ==================================================
{
	gc.tosser.running=1;
	gTosser.AddString(L("S_419"));	// synchronization
	WaitForSingleObject(mutex_tosspurg,INFINITE);

	while (gc.areasetup_active)
	{
		gTosser.AddString(L("S_462"));	// waiting exit area setup
		Sleep(3000);
	}

	system_sound("TOSSER_START");
	backup_wwd(AREADBF,AREADBFT,TRUE);	// backup pretossing state
	gpMain->m_anitoss.Play(0,-1,-1);
	gc.tosser.running=1;

	strcpy(gc.tosser.m_status,L("S_243"));
	gc.tosser.m_packets=0;
	gc.tosser.m_mails=0;
	gc.tosser.m_bytes=0;
	gc.tosser.m_carbons=0;
	gc.tosser.m_echomails=0;
	gc.tosser.m_netmails=0;
	gc.tosser.m_twits=0;
	gTosser.UpdateValues();
	gTosser.AddString(L("S_412"));	// starting

	tossing_proc();
	gTosser.AddString(L("S_453"));	// update info
	db_refresh_areas_info();
	gTosser.AddString(L("S_254"));	// flushing files
	db_flush_area();
	gTosser.AddString(L("S_192"));	// end tossing
	gTosser.AddString("");
	ReleaseMutex(mutex_tosspurg);
	system_sound("TOSSER_END");
	strcpy(gc.tosser.m_status,L("S_271"));	// idle
	gTosser.UpdateValues();
	gpMain->m_anitoss.Stop();
	gpMain->m_anitoss.Seek(0);
	tt->Delete();
	if (!building_subjlist)
	{
		save_current_mail_index();
		gpMain->show_subjects(gustat.cur_area_handle);
		restore_current_mail_index();
	}
	gc.tosser.running=0;
	ExitThread(3); // _beginthread
	return 1;
}

// ==================================================
	void CLightApp::OnStartmailer()
// ==================================================
{
int tmp;

	if (gc.mailer.running || gMailer.m_queue.GetCount()==0)	return;
	gc.mailer.skip_file=0;
	tM=AfxBeginThread(MailerThread,&tmp);
}

// ==================================================
	void CLightApp::OnStarttosser()
// ==================================================
{
int tmp;

	if (gc.tosser.running)	return;
	gc.tosser.running=1;
	tt=AfxBeginThread(TosserThread,&tmp);
}

// ==================================================
	void CLightApp::OnStartpurger()
// ==================================================
{
int tmp;

	if (gc.purger.running)	return;
	gc.purger.area_index=-1;
	tp=AfxBeginThread(PurgerThread,&tmp);
}

// ==================================================
	void CLightApp::OnPurgeOne(int ind)
// ==================================================
{
int tmp;

	if (gc.purger.running)	return;
	gc.purger.area_index=ind;
	tp=AfxBeginThread(PurgerThread,&tmp);
}

// ==================================================
	void CLightApp::OnSetupSystemsounds()
// ==================================================
{
	cfgsound dlg;
	dlg.DoModal();
	gpMain->m_subjlist.SetFocus();
}


// ==================================================
	void CLightApp::OnSetupNewmail()
// ==================================================
{
	newmail dlg;
	dlg.DoModal();
	gpMain->m_subjlist.SetFocus();
}

// ==================================================
	void CLightApp::OnEditCopy() 	// Copy selection
// ==================================================
{
char  mail[2*MAX_MSG];

	*mail=0;
	if (!get_selected_lines(mail,1))	return;
	if (IsOem(&font_mailtext))	OemToChar(mail,mail);
	replace_chars(mail,'\01','@');
	copy_to_clipboard(mail);
}

// ==================================================
	void CLightApp::OnMailCopy()	// Copy entire mail
// ==================================================
{
char	mail[2*MAX_MSG],src[30],dst[30],fmt[1000];
mailheader &mh=gustat.act_mailh;

	gpMain->force_current_mail_update();
	CharToOem(L("S_628",gustat.act_area.echotag,gustat.act_area.description,"%s","%s","%s","%s","%s"),fmt);
	make_address(src,mh.srczone,mh.srcnet,mh.srcnode,mh.srcpoint);
	make_address(dst,mh.dstzone,mh.dstnet,mh.dstnode,mh.dstpoint);
	sprintf(mail,fmt,mh.fromname,src,mh.toname,dst,mh.subject);
	if (IsOem(&font_mailtext))
	{
		strnzcat(mail,gMailbuf,MAX_MSG-strlen(mail));
		OemToChar(mail,mail);
	}
	else
	{
		OemToChar(mail,mail);
		strnzcat(mail,gMailbuf,MAX_MSG-strlen(mail));
	}
	replace_chars(mail,'\01','@');
	copy_to_clipboard(mail);
}

// ==================================================
	void CLightApp::OnEditSelectall() 		 // Select All
// ==================================================
{
   gpMain->force_current_mail_update();
   gpMain->m_mailview.SelItemRange(TRUE,0,gpMain->m_mailview.GetCount()-1);
   gLastUpdate=0;
}

// ==================================================
	void CLightApp::OnMenuModem()
// ==================================================
{
	cfg_modem dlg;
	dlg.is_second=0;
	dlg.DoModal();
	gpMain->m_subjlist.SetFocus();
}

// ==================================================
	void CLightApp::OnMenuModem2()
// ==================================================
{
static int really_go_in=0;

	if (!really_go_in && err_out("DN_TDIONIHTM")!=IDYES)
		return;

	really_go_in=1;
	cfg_modem dlg;
	dlg.is_second=1;
	dlg.DoModal();
	gpMain->m_subjlist.SetFocus();
}

//// ==================================================
//	void focus_was_set_to_edit()
//// ==================================================
//{
//	gpMain->m_subjlist.SetFocus();
//}

// ==================================================
	void CLightApp::OnDeleteMail()
// ==================================================
{
	set_attributes(DB_DELETED);
	if (gc.golded_compatible)
		gpMain->SendDlgItemMessage(IDC_SUBJLIST,WM_KEYDOWN,VK_RIGHT,0);
	else
		gpMain->SendDlgItemMessage(IDC_SUBJLIST,WM_KEYDOWN,VK_DOWN,0);
	gpMain->m_subjlist.SetFocus();
}

// ============================================
	void CLightApp::OnDeleteAll()
// ============================================
{
	gpMain->MarkAllMailsAsDeleted();
	gpMain->m_subjlist.SetFocus();
}

// ==================================================
	void CLightApp::OnFilerequest()
// ==================================================
{
	gpMain->OnFilerequest();
}

// ==================================================
	void CLightApp::OnMenuNodelist()
// ==================================================
{
	cfg_node dlg;
	dlg.DoModal();
	gpMain->m_subjlist.SetFocus();
}

// ==================================================
	void CLightApp::OnReadercolor()
// ==================================================
{
	cfg_col dlg;
	if (dlg.DoModal()==IDOK)
	{
		gpMain->show_mail(&gustat);
		if (!gc.HideIcons)
			gpMain->resizing_on_hide_toolbox(1);
	}
	gpMain->m_subjlist.SetFocus();
}


// ==================================================
	void CLightApp::OnSetupScripteditor()
// ==================================================
{
   cfg_scr dlg;
   dlg.DoModal();
   gpMain->m_subjlist.SetFocus();
}

// ==================================================
	void CLightApp::OnStartnlcompile()
// ==================================================
{
	gNLComp.ShowWindow(SW_SHOWNORMAL);
}

// ==================================================
	int CLightApp::ExitInstance()
// ==================================================
{
	store_textwnd_colors();
	if (hUni)	FreeLibrary(hUni);
	if (hRar)	FreeLibrary(hRar);
	if (hCab)	FreeLibrary(hCab);
	if (hAce)	FreeLibrary(hAce);
	if (hRes)	FreeLibrary(hRes);
//	free((void *)m_pszProfileName);
	return CWinApp::ExitInstance();
}

// ==================================================
	void CLightApp::OnFileExit()
// ==================================================
{
	came_from_button=1;
	gpMain->OnExit();
}

/*/ ==================================================
	BOOL CLightApp::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
// ==================================================
{
	return CWinApp::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}
*/
// ==================================================
	void CLightApp::OnActionChangecurrentmail()
// ==================================================
// Allow change old mail from menu also
{
	gpMain->OnChangeCurrentMail();
}

// ==================================================
	void CLightApp::OnSetupDialingtimeouts()
// ==================================================
{
	cfg_time dlg;
	dlg.DoModal();
}

// ==================================================
	void CLightApp::OnSetupCarboncopystrings()
// ==================================================
{
	cfg_carb dlg;
	dlg.DoModal();
}

// ==================================================
	void CLightApp::OnActionEcholistmanagement()
// ==================================================
{
	ecolst dlg;
	dlg.DoModal();
}


// ==================================================
	void CLightApp::OnInfoAreatrafficinfo()
// ==================================================
{
	traffic dlg;
	dlg.DoModal();
}

// ============================================
	void CLightApp::OnHelp()
// ============================================
{
	WinHelp(VHELP_MAINWINDOW);
}

// ================================================
	void CLightApp::OnHelpMe()
// ================================================
{
	WinHelp(0,HELP_FINDER);
}

// ================================================
	void CLightApp::OnFIDONET()
// ================================================
{
CString	str;

	str=gc.BasePath;
	str+="\\Hаставление.htm";
	str.Replace("\\","\\\\");
	HINSTANCE i=ShellExecute(NULL,"open",str,NULL,NULL,SW_SHOWNORMAL);
}

// ================================================
	void CLightApp::OnFIDOLANG()
// ================================================
{
	WinHelp(VHELP_FIDOLANG);
}

// ================================================
	void CLightApp::OnDICTIONARY()
// ================================================
{
	WinHelp(VHELP_DICTIONARY);
}

// ================================================
	void CLightApp::OnConfigOtherconfigs()
// ================================================
{
	cfg_oth dlg;
	dlg.DoModal();
	gpMain->m_subjlist.SetFocus();
}

// ===========================================
	void CLightApp::OnHelpAbout()
// ===========================================
{
	gpMain->do_about();
}

// ===========================================
	void CLightApp::OnInfoAddressinfo()
// ===========================================
{
	gpMain->OnInfoAdress();
}

// ====================================================
	void CLightApp::OnPrintMail()
// ====================================================
{
FILE	*fp;
CString str;

	gpMain->BuildUserWish(str,TRUE);
	unlink(PRINT_FILE);
	fp=fopen(PRINT_FILE,"wt");
	if (fp)
	{
		if (IsOem(&font_mailtext) && get_cfg(CFG_PRINT,"AlwaysAnsi",1))	str.OemToAnsi();
		fwrite(str,str.GetLength(),1,fp);
		fclose(fp);
		str.Format(get_cfg(CFG_PRINT,"PrintCommand","start notepad.exe /p %s"),PRINT_FILE);
		WinExec(str,get_cfg(CFG_PRINT,"BackgroundPrint",1) ? SW_HIDE : SW_SHOWNORMAL);
	}
	else
		ERR_MSG_RET("E_COPSF");
}

// ====================================================
	void CLightApp::OnSaveSelToFile()
// ====================================================
{
	save_mail_to_file(TRUE);
}

// ====================================================
	void CLightApp::OnSaveMailToFile()
// ====================================================
{
	save_mail_to_file(FALSE);
}

// ====================================================
	void CLightApp::OnInfoViewlogfiles()
// ====================================================
{
	logdisp dlg;
	dlg.DoModal();
	gpMain->m_subjlist.SetFocus();
}

// ====================================================
	void CLightApp::OnSetupLogfilesettings()
// ====================================================
{
	logopti dlg;
	dlg.DoModal();
	gpMain->m_subjlist.SetFocus();
}

// ============================================
// choose ANSI font for the mailtext listbox
	void CLightApp::OnSetMailtextFontANSI()
// ============================================
{
LOGFONT lf;

	select_font(&font_ANSI,"ANSI");
	font_ANSI.GetLogFont(&lf);
	set_mailtext_font(&lf);
	gpMain->m_mailview.SetFont(&font_mailtext);
}
// ============================================
// choose OEM font for the mailtext listbox
	void CLightApp::OnSetMailtextFontOEM()
// ============================================
{
LOGFONT lf;

	select_font(&font_OEM,"OEM");
	font_OEM.GetLogFont(&lf);
	set_mailtext_font(&lf);
	gpMain->m_mailview.SetFont(&font_mailtext);
}
// ====================================================
// choose font for subjects listbox
	void CLightApp::OnSetSubjectFont()
// ====================================================
{
	select_font(&font_subject,"SUBJS");
	gpMain->m_subjlist.SetFont(&font_subject);
	gpMain->m_attrlist.SetFont(&font_subject);
}

// ====================================================
// choose font for labels in main dialog
	void CLightApp::OnFontMainLabels()
// ====================================================
{
	select_font(&font_labels,"LABELS");
}
// ====================================================
// choose font for fields in main dialog
	void CLightApp::OnFontMainFields()
// ====================================================
{
	select_font(&font_fields,"FIELDS");
}
// ============================================
// choose font for mail editor
	void CLightApp::OnChooseFontEditor()
// ============================================
{
//	select_font(&font_editor,"EDITOR");
}

// ============================================
	void CLightApp::OnSetupAdditionalakas()
// ============================================
{
	cfg_aaka dlg;
	dlg.DoModal();
	gpMain->m_subjlist.SetFocus();
}

// ============================================
	void CLightApp::OnFilterDisplay()
// ============================================
{
	gpMain->OnFilterSet();
	gpMain->m_subjlist.SetFocus();
}

// ============================================
	void CLightApp::OnMarkAllMailsAsRead()
// ============================================
{
	gpMain->MarkAllMailsAsRead();
}

// ============================================
	void CLightApp::OnActionAbortscript()
// ============================================
{
	gpMain->OnAbortscript();
	gpMain->m_subjlist.SetFocus();
}

// ============================================
	void CLightApp::OnNeverDelete()
// ============================================
{
	gpMain->OnNeverDelete();
}

// ============================================
	void CLightApp::OnMailsAnswerasnetmail()
// ============================================
{
	gpMain->m_subjlist.SetFocus();
	gpMain->OnAnswerAsNetmail();
}

// ============================================
	void CLightApp::OnRescanmail()
// ============================================
{
	if (!gc.mailer.running)
		gMailer.OnRescan();
	else
		err_out("W_MAIGAK");
	gpMain->m_subjlist.SetFocus();
}

// ============================================
	void CLightApp::OnConvertSpecialChars()
// ============================================
{
	cfg_conv dlg;
	dlg.DoModal();
	gpMain->m_subjlist.SetFocus();
}

// ============================================
	void CLightApp::OnCrosspost()
// ============================================
{
   crospost dlg;
   dlg.DoModal();
   gpMain->m_subjlist.SetFocus();
}

// ============================================
	void CLightApp::OnViewOutbound()
// ============================================
{
	if (gc.mailer.running)
		ERR_MSG_RET("W_STOPFIRST");
	outbound dlg;
	dlg.DoModal();
	gpMain->m_subjlist.SetFocus();
}

// ============================================
	void CLightApp::OnSetupTrashcanfilter()
// ============================================
{
	cfg_tras dlg;
	dlg.DoModal();
	gpMain->m_subjlist.SetFocus();
}

// ============================================
	void CLightApp::OnAutosave()
// ============================================
{
	gpMain->OnAsave();
	gpMain->m_subjlist.SetFocus();
}

// ===========================================
	void CLightApp::OnForward()
// ===========================================
{
areadef ad;
CString from,from_adr,to,to_adr,subj,create,name;

	if (db_get_area_by_index(gustat.cur_area_handle,&ad) != DB_OK)	return;

	get_fullname(name);
	gpMain->m_from.GetWindowText(from);
	gpMain->m_from_address.GetWindowText(from_adr);
	gpMain->m_to.GetWindowText(to);
	gpMain->m_to_address.GetWindowText(to_adr);
	gpMain->m_create.GetWindowText(create);
	gpMain->m_subject.GetWindowText(subj);

	fwdstring=
		"====================<Forward Summary>==========================\r\n";
	fwdstring+=
		"| FORWARDED BY "+name+" ("+ad.aka+")\r\n"+
		"| AREA: "+ad.echotag+" ("+ad.description+")\r\n"+
		"| FROM: "+from+", "+from_adr+" ("+create+")\r\n"+
		"| TO  : "+to+"\r\n"+
		"| SUBJ: "+subj+"\r\n"+
		"=====================<Begin Forward>===========================\r\n\r\n";

	gc.mode=MODE_FORWARD;
	gpMain->OnQuoteA();
	fwdstring.Empty();
}

// ============================================
	void CLightApp::OnQuoteToArea()
// ============================================
{
	gpMain->m_subjlist.SetFocus();
	gpMain->OnQuoteA();
}

// ============================================
	void CLightApp::OnQuote()
// ============================================
{
	gpMain->m_subjlist.SetFocus();
	gpMain->OnQuote();
}

// ============================================
	void CLightApp::OnSwitchLeftIcons()
// ============================================
{
    gpMain->OnHideicons();
	gpMain->m_subjlist.SetFocus();
}

// ============================================
	void CLightApp::OnSwitchMailer()
// ============================================
{
BOOL vis;

	vis=gMailer.IsWindowVisible();
	gMailer.ShowWindow(vis ? SW_HIDE : SW_SHOWNORMAL);
	if (!vis) gMailer.GraphicalStatusDisplay(1);
}
// ============================================
	void CLightApp::OnSwitchTosser()
// ============================================
{
	gTosser.ShowWindow(gTosser.IsWindowVisible() ? SW_HIDE : SW_SHOWNORMAL);
}
// ============================================
	void CLightApp::OnSwitchPurger()
// ============================================
{
	gPurger.ShowWindow(gPurger.IsWindowVisible() ? SW_HIDE : SW_SHOWNORMAL);
}
// ============================================
	void CLightApp::OnSwitchNodelist()
// ============================================
{
	gNLComp.ShowWindow(gNLComp.IsWindowVisible() ? SW_HIDE : SW_SHOWNORMAL);
}
// ============================================
	void CLightApp::OnSwitchExtSearch()
// ============================================
{
	gSearch.ShowWindow(gSearch.IsWindowVisible() ? SW_HIDE : SW_SHOWNORMAL);
}

// ============================================
	void CLightApp::OnHsfloating()
// ============================================
{
	gFloating.ShowWindow(gFloating.IsWindowVisible() ? SW_HIDE : SW_SHOWNORMAL);
	gpMain->m_subjlist.SetFocus();
}

// ============================================
	void CLightApp::OnSimpleSearch()
// ============================================
{
	gpMain->OnFind();
}

// ============================================
	void CLightApp::OnActionsStartscript1()
// ============================================
{
	gpMain->OnMacro1();
	gpMain->m_subjlist.SetFocus();
}

// ============================================
	void CLightApp::OnActionsStartscript2()
// ============================================
{
	gpMain->OnMacro2();
	gpMain->m_subjlist.SetFocus();
}

// ============================================
	void CLightApp::OnActionsStartscript3()
// ============================================
{
	gpMain->OnMacro3();
	gpMain->m_subjlist.SetFocus();
}

// ============================================
	void CLightApp::OnActionsStartscript4()
// ============================================
{
	gpMain->OnMacro4();
	gpMain->m_subjlist.SetFocus();
}

// ============================================
	void CLightApp::OnActionsStartscript5()
// ============================================
{
	gpMain->OnMacro5();
	gpMain->m_subjlist.SetFocus();
}

// ============================================
	void CLightApp::OnFreqFromMail()
// ============================================
{
	gpMain->OnFreq();
	gpMain->m_subjlist.SetFocus();
}

// ============================================
	void CLightApp::OnMailCreateNewMail()
// ============================================
{
	gpMain->OnNew();
//	gpMain->m_subjlist.SetFocus();
}

/*/ ============================================
	void CLightApp::OnIsdnConfigurations()
// ============================================
{
	cfg_isdn dlg;
	dlg.DoModal();
	gpMain->m_subjlist.SetFocus();
}
*/

/*/ ============================================
	void CLightApp::OnSetupRecieverlists()
// ============================================
{
	receiver dlg;
	dlg.DoModal();
	gpMain->m_subjlist.SetFocus();
}
*/
// ============================================
	void CLightApp::OnConfigEvents()
// ============================================
{
	events dlg;
	dlg.scheduled=0;
	dlg.DoModal();
}

// ==================================================
	void CLightApp::StartEventWorkerThread()
// ==================================================
{
DWORD tmp;

	evtth=AfxBeginThread(EventWorkerThread,(LPVOID)&tmp);
}

// ==================================================
	void CLightApp::StartUDPWorkerThread()
// ==================================================
{
DWORD tmp;

	UDPth=AfxBeginThread(UDPWorkerThread,(LPVOID)&tmp);
}


// ==================================================
	void CLightApp::StartIncomeWorkerThread()
// ==================================================
{
DWORD tmp;

	Incometh=AfxBeginThread(IncomeWorkerThread,(LPVOID)&tmp);
}


// ==================================================
	UINT IncomeWorkerThread(LPVOID p)
// ==================================================
// Der werkelt auch immer ...
{
OVERLAPPED ol1;
OVERLAPPED ol2;
int		   ret=0,waitres=0;
DWORD	   dwmask1=0,dwmask2=0;
HANDLE     hcomm1=INVALID_HANDLE_VALUE;
HANDLE     hcomm2=INVALID_HANDLE_VALUE;
HANDLE     waithand[4];
_modemcfg  mcfg;
DCB		   dc;
int		   devices,services,i,port,init_modem0_done,init_modem1_done,closeret;
int		   FlagIncomingISDNCall	=0;

	init_modem0_done=init_modem1_done=0;
	memset(&gincome,0,sizeof(gincome));
	gincome.EventToKillWaitCommEvent	=CreateEvent(0,TRUE,FALSE,0);
	ResetEvent(gincome.EventToKillWaitCommEvent);

	ret=WaitForSingleObject(gincome.EventToKillWaitCommEvent,2000);
	if (ret==WAIT_OBJECT_0)
		goto end_of_thread;

	while (1)
	{
next_try:
		if (gincome.MailerNeedsModem || gincome.InModemEmsiSession)
		{
			gcomm.AcceptOn1Modem=gcomm.AcceptOn2Modem=gcomm.AcceptOnIsdn=0;
			ret=WaitForSingleObject(gincome.EventToKillWaitCommEvent,1000);
			if (ret==WAIT_OBJECT_0)
				goto end_of_thread;
			continue;
		}

		get_devices_listen_for_call(devices,services);
		ISDNCallAcceptionOnOff((devices & 4)==4);

		if (!devices)
		{
wait_and_try_again:
			gincome.WaitingForMin1Device=0;
			ResetEvent(gincome.EventToKillWaitCommEvent);
			ret=WaitForSingleObject(gincome.EventToKillWaitCommEvent,10000);

			if (gc.ManualAccept)
			{
				port=gc.ManualAccept-1;
				goto DirektZumOlli;
			}

			if (gc.LocalBBSLogin)
			{
				gc.LocalBBSLogin=0;
				run_bbs((unsigned char )LOCALCOM);
				modem_add_listbox(L("S_299"));	// Local BBS Login ended
				continue;
			}

			if (ret==WAIT_OBJECT_0)
				goto end_of_thread;
			continue;
		}

		if (gincome.InModemEmsiSession)
		{
			gcomm.AcceptOn1Modem=gcomm.AcceptOn2Modem=gcomm.AcceptOnIsdn=0;
			goto wait_and_try_again;
		}
		gincome.WaitingForMin1Device=1;
		memset(&ol1,0,sizeof(OVERLAPPED));
		memset(&ol2,0,sizeof(OVERLAPPED));

		gincome.EventWaitCommEvent1		=CreateEvent(0,TRUE,FALSE,0);
		gincome.EventWaitCommEvent2		=CreateEvent(0,TRUE,FALSE,0);
		gincome.EventWaitCommEventISDN	=CreateEvent(0,TRUE,FALSE,0);
		ResetEvent(gincome.EventToKillWaitCommEvent);

		ASSERT(gincome.EventToKillWaitCommEvent);
		ASSERT(gincome.EventWaitCommEvent1);
		ASSERT(gincome.EventWaitCommEvent2);
		ASSERT(gincome.EventWaitCommEventISDN);

		ol1.hEvent=gincome.EventWaitCommEvent1;
		ol2.hEvent=gincome.EventWaitCommEvent2;

		for (i=0;i<2;i++)
		{
			if (!(devices & (1<<i)))	continue;
			load_modem_cfg(i+1,&mcfg);
			switch (i)
			{
				case 0:
							if (!init_modem0_done)
							{
								ret=callacc_initialize_modem(&mcfg);
								if (!ret)
								{
									MessageBeep(0);
									Sleep(2000);
									goto next_try;
								}
								init_modem0_done=1;
							}
 							ComPort1_IncoThread.Format("COM%d",mcfg.m_comport);
							ModemInit1_IncoThread.Format("baud=%d parity=N data=8 stop=1",mcfg.m_baud);

							if (gincome.MailerNeedsModem)
								goto next_try;

							hcomm1=CreateFile(ComPort1_IncoThread,GENERIC_READ | GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_FLAG_OVERLAPPED,NULL);
							if (hcomm1==INVALID_HANDLE_VALUE)
							{
								err_out("E_CAOPCPP",PS ComPort1_IncoThread);
								income_times.defaultindex=1; // Disable all further INCOMES
								goto next_try;
							}

							memset(&dc,0,sizeof(DCB)); 	dc.DCBlength=sizeof(DCB);
							ret=BuildCommDCB (ModemInit1_IncoThread,&dc);
							ret=ModifyCommDCB(&dc);
							ret=SetCommState(hcomm1,&dc);
							PurgeComm(hcomm1,PURGE_TXCLEAR | PURGE_RXCLEAR);

							ret=WaitForSingleObject(gincome.EventToKillWaitCommEvent,3000);
							if (ret==WAIT_OBJECT_0)
							{
								if (gc.LocalBBSLogin)
								{
									if (hcomm1!=INVALID_HANDLE_VALUE)
									{
										PurgeComm(hcomm1,PURGE_TXCLEAR | PURGE_RXCLEAR);
										CloseHandle(hcomm1);
										hcomm1=INVALID_HANDLE_VALUE;
									}
									goto lokal_login;
								}
								if (gincome.MailerNeedsModem)
									goto TheMailerNeedTheModem;

								goto end_of_thread;	// Ein Kill..
							}

							PurgeComm(hcomm1,PURGE_TXCLEAR | PURGE_RXCLEAR);
							ret=SetCommMask(hcomm1,EV_RING | EV_RXCHAR);
							ret=WaitCommEvent(hcomm1,&dwmask1,&ol1);
							break;
				case 1:
							if (!init_modem1_done)
							{
								ret=callacc_initialize_modem(&mcfg);
								if (!ret)
								{
									MessageBeep(0);
									Sleep(2000);
									goto next_try;
								}
								init_modem1_done=1;
							}

							ComPort2_IncoThread.Format("COM%d",mcfg.m_comport);
							ModemInit2_IncoThread.Format("baud=%d parity=N data=8 stop=1",mcfg.m_baud);

							if (gincome.MailerNeedsModem)	goto next_try;

							hcomm2=CreateFile(ComPort2_IncoThread,GENERIC_READ | GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_FLAG_OVERLAPPED,NULL);
							if (hcomm2==INVALID_HANDLE_VALUE)
							{
								err_out("E_CAOPCPS",PS ComPort2_IncoThread);
								income_times.defaultindex=1; // Disable all further INCOMES
								goto next_try;
							}

							memset(&dc,0,sizeof(DCB));
							dc.DCBlength=sizeof(DCB);
							ret=BuildCommDCB (PS ModemInit2_IncoThread,&dc);
							ret=ModifyCommDCB(&dc);
							ret=SetCommState(hcomm2,&dc);
							PurgeComm(hcomm2,PURGE_TXCLEAR | PURGE_RXCLEAR);

							ret=WaitForSingleObject(gincome.EventToKillWaitCommEvent,3000);
							if (ret==WAIT_OBJECT_0)
							{
								if (gc.LocalBBSLogin)
								{
									if (hcomm2!=INVALID_HANDLE_VALUE)
									{
										PurgeComm(hcomm2,PURGE_TXCLEAR | PURGE_RXCLEAR);
										CloseHandle(hcomm2);
										hcomm2=INVALID_HANDLE_VALUE;
									}
									goto lokal_login;
								}

								if (gincome.MailerNeedsModem)
									goto TheMailerNeedTheModem;

								goto end_of_thread;
							}

							PurgeComm(hcomm2,PURGE_TXCLEAR | PURGE_RXCLEAR);
							ret=SetCommMask(hcomm2,EV_RING | EV_RXCHAR);
							ret=WaitCommEvent(hcomm2,&dwmask2,&ol2);
							break;
				default:
							ASSERT(0);
							break;
			}

		}

		waithand[0]=gincome.EventToKillWaitCommEvent;
		waithand[1]=gincome.EventWaitCommEvent1;
		waithand[2]=gincome.EventWaitCommEvent2;
		waithand[3]=gincome.EventWaitCommEventISDN;

		// Falls wir auch auf den ISDN Kanal horchen sollen gehen wir nach einer
		// halben Sekunde wieder raus um auch den ISDN Kanal abhorchen zu koennen ...
		if (devices & 4)			// Auch auf ISDN Kanal horchen
		{
			while (1)
			{
				FlagIncomingISDNCall=0;
				waitres=WaitForMultipleObjects(4,waithand,FALSE,500);
				// Falls es ein Ring war, oder ein Kill dann verfahren wir ganz normal
				if (waitres!=WAIT_TIMEOUT)
				{
					// Line_Splitter Behandlung
					if (!LineSplitterDetectedRing(waitres))
						goto PollWaitHasDetectedRing;
				}
				if (HandleTestForIncomingISDNCall())
				{
					// Ein ISDN Anruf ...
					FlagIncomingISDNCall=1;
					goto PollWaitHasDetectedRing;
				}
				if (gIncomeDevicesHaveChanged)
				{
					gIncomeDevicesHaveChanged=0;
					get_devices_listen_for_call(devices,services);
					ISDNCallAcceptionOnOff((devices & 4)==4);

					// Falls die ISDN Anrufannahme mittlerweile gar nicht mehr
					// aktiv ist gehen wir den normalen Weg raus ...
					if (!(devices & 4))
						goto PollWaitHasDetectedRing;
				}
			}
		}
		else
			waitres=WaitForMultipleObjects(4,waithand,FALSE,10000);

PollWaitHasDetectedRing:
TheMailerNeedTheModem:
		// Egal welcher Grund, wir geben sofort die belegten Resourcen frei ...
		if (hcomm1!=INVALID_HANDLE_VALUE)
		{
			PurgeComm(hcomm1,PURGE_TXCLEAR | PURGE_RXCLEAR);
			closeret=CloseHandle(hcomm1);
			hcomm1=INVALID_HANDLE_VALUE;
		}
		if (hcomm2!=INVALID_HANDLE_VALUE)
		{
			PurgeComm(hcomm2,PURGE_TXCLEAR | PURGE_RXCLEAR);
			closeret=CloseHandle(hcomm2);
			hcomm2=INVALID_HANDLE_VALUE;
		}
		if (gincome.EventWaitCommEvent1)
		{
			CloseHandle(gincome.EventWaitCommEvent1);
			gincome.EventWaitCommEvent1=0;
		}
		if (gincome.EventWaitCommEvent2)
		{
			CloseHandle(gincome.EventWaitCommEvent2);
			gincome.EventWaitCommEvent2=0;
		}
		if (gincome.EventWaitCommEventISDN)
		{
			CloseHandle(gincome.EventWaitCommEventISDN);
			gincome.EventWaitCommEventISDN=0;
		}
		if (gc.ManualAccept)
		{
			port=gc.ManualAccept-1;
			goto DirektZumOlli;
		}
		if (gincome.MailerNeedsModem)
		{
			gincome.MailerNeedsModem=0;
			continue;
		}

lokal_login:
		if (gc.LocalBBSLogin)
		{
			gc.LocalBBSLogin=0;
			run_bbs((unsigned char)LOCALCOM);
			modem_add_listbox(L("S_299"));	// Local BBS Login ended
			continue;
		}

		if	(FlagIncomingISDNCall)	goto DirektZumOlli;
		if (waitres==WAIT_TIMEOUT)	continue;
		if (waitres==WAIT_OBJECT_0)	goto end_of_thread;

// Exit Thread
			port=-1;
			if (waitres==(WAIT_OBJECT_0+1))
			{
				modem_add_listbox(L("S_387"));	// Ring detected on 1st modem
				port=0;

				if (LineSplitterDetectedRing(waitres))
					continue;
			}
			if (waitres==(WAIT_OBJECT_0+2))
			{
				modem_add_listbox(L("S_388"));	// Ring detected on 2nd modem
				port=1;

				if (LineSplitterDetectedRing(waitres))
					continue;
			}
			if (waitres==(WAIT_OBJECT_0+3))
			{
				modem_add_listbox(L("S_389"));	// Ring detected on ISDN Port
				port=2;
			}

			ASSERT(port!=-1);

			if (!FlagIncomingISDNCall && !match_number_of_rings(port))
					continue;
			if (port!=-1)
			{

DirektZumOlli:

				if (gc.ManualAccept)
				{
					services=0xFFF;
					gc.ManualAccept=0;
				}
				gincome.OllisTurn=1;

				if (!FlagIncomingISDNCall)
					incoming_emsi_session(port+1,services);
				else
				{
					__try
					{
						StartCapiWorkerThread	();
						incoming_emsi_session(ISDNPORT,services);		// Incomming ISDN Call ..
						StopCapiWorkerThread	();
						capi_hangup				();
					}
					__except (1,EXCEPTION_EXECUTE_HANDLER)
					{
						Sleep(500);
						modem_add_listbox("SEH: incoming_emsi_session() failed");
						free_capi_stuff();
					}

				}
				gincome.OllisTurn=0;
				gc.LocalBBSLogin=0;
			}
	}
end_of_thread:

	gcomm.AcceptOn1Modem=gcomm.AcceptOn2Modem=gcomm.AcceptOnIsdn=0;

	if (hcomm1!=INVALID_HANDLE_VALUE)
	{
		PurgeComm(hcomm1,PURGE_TXCLEAR | PURGE_RXCLEAR);
		CloseHandle(hcomm1);
		hcomm1=INVALID_HANDLE_VALUE;
	}
	if (hcomm2!=INVALID_HANDLE_VALUE)
	{
		PurgeComm(hcomm2,PURGE_TXCLEAR | PURGE_RXCLEAR);
		CloseHandle(hcomm2);
		hcomm2=INVALID_HANDLE_VALUE;
	}
	if (gincome.EventWaitCommEvent1)
	{
		CloseHandle(gincome.EventWaitCommEvent1);
		gincome.EventWaitCommEvent1=0;
	}
	if (gincome.EventWaitCommEvent2)
	{
		CloseHandle(gincome.EventWaitCommEvent2);
		gincome.EventWaitCommEvent2=0;
	}
	if (gincome.EventWaitCommEventISDN)
	{
		CloseHandle(gincome.EventWaitCommEventISDN);
		gincome.EventWaitCommEventISDN=0;
	}

	CloseHandle(gincome.EventToKillWaitCommEvent);
	return 77;
}

#define WAIT100MS(a) 	for (int t=0;t<a;t++) \
	{ if (gc.ExitEventThread) goto exit; if (gc.restartevents) goto restart; Sleep(100); }

// ==================================================
	UINT EventWorkerThread(LPVOID p)
// ==================================================
{
	WAIT100MS(30)
restart:
	gc.restartevents=0;
	while (1) 				// forever ...
	{
		WAIT100MS(10)
		if (!gc.eventsenabled)	continue;

		for (int i=0;i<eventlist.GetCount();i++)
		{
			if (gc.InEventDialogBox)
			{
				event_add_listbox(L("S_228"));	// Events sleeping while in setup
				WAIT100MS(10)
				break;
			}
			gc.actual_event_index=i;
			event_pump(i);
			WAIT100MS(10)
		}
	}

exit:
	gc.ExitEventThread=0;
	ExitThread (13);
	return 1;
}

// ==================================================
	UINT UDPWorkerThread(LPVOID p)
// ==================================================
{
#define SOCKET_ERROR  -1
int	 ret;

//tw: #ifdef DEBUPD vor char dbuf[300] gesetzt

	#ifdef DEBUDP
		char dbuf[300];
		sprintf(dbuf,"UDP: Starting UDP Worker Thread\n"); OutputDebugString(dbuf);
	#endif

	// Wir kreiren den Event fuer eine Killen waehrend eines Waits ...
	gEventToKillUDP	  =CreateEvent(0,TRUE,FALSE,0);

	while (1)
	{
		// Falls remote disable ist
		if (!remote.defaultindex)
		{
			#ifdef DEBUDP
				sprintf(dbuf,"UDP: beforeWaitForSingleObject\n"); OutputDebugString(dbuf);
			#endif
			// Hier simulieren wir einen Sleep der von aussen gekilled werden kann ...
			ret=WaitForSingleObject(gEventToKillUDP,1500);
			#ifdef DEBUDP
				sprintf(dbuf,"UDP: WaitForSingleObject left=%d\n",ret); OutputDebugString(dbuf);
			#endif
			if (ret==WAIT_OBJECT_0)
				goto end_of_thread;	// Ein Kill..
		}
		else
		{
			// Falls aus irgendeinem Grund noch nicht initialisiert ist wareten
			// wir da drauf
			if (!tcpip_was_used)
			{
				#ifdef DEBUDP
					sprintf(dbuf,"UDP: Not yet initialized !\n"); OutputDebugString(dbuf);
				#endif
				Sleep(500);
			}
			else
			{
				// Horch was kommt von draussen rein ...
				ret=get_UDP_request(&rcvpacket);
				if (ret==SOCKET_ERROR)
				{
					if (gc.DisableRemote)
					{
						#ifdef DEBUDP
							sprintf(dbuf,"UDP: Gconfig.DisableRemote Set\n"); OutputDebugString(dbuf);
						#endif
						gc.DisableRemote=0;
						continue;
					}
					else
						goto end_of_thread;
				}
				gc.DisableRemote=0;

				// Wir merken uns wann der Request ankam ...
				gc.LastUDPCommand=time(NULL);
				switch (rcvpacket.command)
				{
					case UDP_RESCAN:
						::PostMessage(gMailer.m_hWnd,WM_COMMAND,IDC_RESCAN,0L);
						break;
					case UDP_ABORTSESSION:
						::PostMessage(gMailer.m_hWnd,WM_COMMAND,IDABORTSESSION,0L);
 						break;
					case UDP_START:
						::PostMessage(gMailer.m_hWnd,WM_COMMAND,IDSTART,0L);
						break;
					case UDP_EXITFIPS:
						::PostMessage(gpMain->m_hWnd,WM_COMMAND,IDC_EXIT,0L);
						break;
					case UDP_REMOTEANSWER:
						gc.ErrMsgBoxRemoteResponse=rcvpacket.subcommand;
						break;
					case UDP_REMSELECTIONCHANGE:
						int x;
						int *help;
						help=(int*)&rcvpacket.data;

						// Alle deselektieren
						for (x=0;x<gMailer.m_queue.GetCount();x++)
							gMailer.m_queue.SetSel(x,0);

						// Und die selektierten einschalten
						for (x=0;x<rcvpacket.subcommand;x++)
						{
							gMailer.m_queue.SetSel(*help,1);
							help++;
						}
						break;
				}
			}
		}
	}

end_of_thread:
	if (gEventToKillUDP!=INVALID_HANDLE_VALUE)
	{
		CloseHandle(gEventToKillUDP);
		gEventToKillUDP=INVALID_HANDLE_VALUE;
	}
	#ifdef DEBUDP
		sprintf(dbuf,"UDP: Ending UDP Worker Thread\n"); OutputDebugString(dbuf);
	#endif
	return 36;
}


// ============================================
	void CLightApp::OnSetupEditortypereplacment()
// ============================================
{
	trepl dlg;
	dlg.DoModal();
	gpMain->m_subjlist.SetFocus();
}

// ============================================
	void CLightApp::OnSetupTextimport()
// ============================================
{
	txttemp dlg;
	dlg.DoModal();
	gpMain->m_subjlist.SetFocus();
}

// ============================================
	void CLightApp::OnActionsAbortcurrentevent()
// ============================================
{
	OnAbortCurrentEventWork();
}

// ============================================
	void CLightApp::OnActionsExecuteanyscript()
// ============================================
{
CString	str;
int		ret;

	if (gc.detscript.running)
		ERR_MSG_RET("E_ALRRASF");

	str.LoadString(IDS_SCRIPTFILT);
	CFileDialog	dlg(TRUE,"fps",NULL,OFN_HIDEREADONLY,str);
	ret=dlg.DoModal();
	restore_base_path();
	if (ret!=IDOK)	return;
	strcpy(gc.detscript.scriptname,dlg.GetPathName());
	gc.detscript.running=1;
	script_thread=AfxBeginThread(ScriptWorkerThread,&dlg);
	gpMain->m_subjlist.SetFocus();
}

// ============================================
	void CLightApp::OnMulAsPrint()
// ============================================
{
	marksel dlg;
	if (dlg.DoModal()==IDCANCEL)	return;
	dlg.get_selection();

	if (show_first_selected())
	{
		do
			OnPrintMail();
		while (show_next_selected());
		show_msg(L("S_176"));
	}
	gpMain->m_subjlist.SetFocus();
}

// ============================================
	void CLightApp::OnMulAsRead()
// ============================================
{
	marksel dlg;
	if (dlg.DoModal()==IDCANCEL)	return;
	dlg.get_selection();

	if (show_first_selected())
	{
		while (show_next_selected());
		show_msg(L("S_176"));
	}
	gpMain->m_subjlist.SetFocus();
}

// ============================================
	void CLightApp::OnSaveMultipleToFile()
// ============================================
{
static CString path;
CString	str;
FILE	*fp;
int		ret;

	marksel mrk;
	if (mrk.DoModal()==IDCANCEL)	return;
	mrk.get_selection();
	str.LoadString(IDS_TEXTFILT);
	CFileDialog	dlg(FALSE,"txt",path,OFN_HIDEREADONLY,str);
	ret=dlg.DoModal();
	restore_base_path();
	if (ret!=IDOK)	return;
	path=dlg.GetPathName();
	if (access(path,0)==0)
	{
	   al_exist dlg;
	   ret=dlg.DoModal();
	   if (ret==IDCANCEL)	return;
	}
	if (show_first_selected())
	{
		fp=fopen(path,ret==IDAPPEND ? "at" : "wt");
		if (fp)
			fclose(fp);
		do
			gpMain->AppendMailTofile(path);
		while (show_next_selected());
		show_msg(L("S_176"));
	}
	gpMain->m_subjlist.SetFocus();
}

// ============================================
	void CLightApp::OnMulAutosave()
// ============================================
{
	marksel dlg;
	if (dlg.DoModal()==IDCANCEL) return;
	dlg.get_selection();

	if (show_first_selected())
	{
		do
			OnAutosave();
		while (show_next_selected());
		show_msg(L("S_176"));
	}
	gpMain->m_subjlist.SetFocus();
}

// ============================================
	void CLightApp::OnMulDelete()
// ============================================
{
	marksel dlg;
	if (dlg.DoModal()==IDCANCEL)	return;
	dlg.get_selection();
	set_attributes(DB_DELETED);
	gpMain->m_subjlist.SetFocus();
}

// ============================================
	void CLightApp::OnMulNeverDelete()
// ============================================
{
	marksel dlg;
	if (dlg.DoModal()==IDCANCEL) return;
	dlg.get_selection();
	set_attributes(DB_NEVER_DELETE);
	gpMain->m_subjlist.SetFocus();
}

/*/ ============================================
	void CLightApp::OnEditPrintconfiguration()
// ============================================
{
	cfg_print dlg;
	dlg.DoModal();
	gpMain->m_subjlist.SetFocus();
}*/

// ============================================
	void CLightApp::OnToggleRot13()
// ============================================
{
	gc.isrot13=!gc.isrot13;
	gpMain->m_subjlist.SetFocus();
	gc.SuppressModeSwitch=1;
    gpMain->OnDblclkList();
}

// =================================================================
	void CLightApp::OnDupeCheck()
// =================================================================
{
	gpMain->OnDupeCheck();
}

// ============================================
	void CLightApp::OnDupeCheckAll()
// =================================================================
{
	gpMain->OnDupeCheckAll();
}

// ============================================
	void CLightApp::OnConfigTimesAndDevices()
// ============================================
{
	cfg_inco dlg;
	dlg.DoModal();
	gpMain->m_subjlist.SetFocus();
}

// ============================================
	void CLightApp::OnIncommingcallsConfigNrofrings()
// ============================================
{
	cfg_ring dlg;
	dlg.DoModal();
	gpMain->m_subjlist.SetFocus();
}

// ============================================
	void CLightApp::OnMultimediaDisplayoptions()
// ============================================
{
	cfg_mult dlg;
	dlg.DoModal();
	gpMain->m_subjlist.SetFocus();
}

// ============================================
	void CLightApp::OnMultimediaAccessibleelements()
// ============================================
{
	gc.disable_insmm_items=1;
	insmm dlg;
	dlg.DoModal();
	gpMain->m_subjlist.SetFocus();
	gc.disable_insmm_items=0;
}

// ============================================
	void CLightApp::OnCaptureRecipientAddr()
// ============================================
{
adrbook dlg;

	gpMain->m_to.GetWindowText(dlg.name,99);
	if (gustat.cur_area_handle==0)
		gpMain->m_to_address.GetWindowText(dlg.address,99);
	dlg.DoModal();
	gpMain->m_subjlist.SetFocus();

}

// ============================================
	void CLightApp::OnCaptureSenderAddr()
// ============================================
{
	adrbook dlg;
	gpMain->m_from.GetWindowText(dlg.name,99);
	gpMain->m_from_address.GetWindowText(dlg.address,99);
	dlg.DoModal();
	gpMain->m_subjlist.SetFocus();
}

// ============================================
	void CLightApp::OnSubjListAdjustment()
// ============================================
{
	sub_adj dlg;
	dlg.DoModal();
	gpMain->m_subjlist.SetFocus();
}

void CLightApp::OnSetupRequestPathes()
{
	cfgrpath dlg;
	dlg.DoModal();
	gpMain->m_subjlist.SetFocus();
}

// ============================================
	void CLightApp::OnSetupMagics()
// ============================================
{
	cfgmagic dlg;
	dlg.DoModal();
	gpMain->m_subjlist.SetFocus();
}

// ============================================
	void CLightApp::OnIncomingcallsRemotemonitor()
// ============================================
{
	cfg_rmon dlg;
	dlg.DoModal();
	gpMain->m_subjlist.SetFocus();
}

// ============================================
	void CLightApp::OnMarkMailFreeze()
// ============================================
{
int sel,rsel,ret,changed=0;
CString line,index;

	if (!gustat.already_one_valid_mail)		return;
	if (!(gustat.act_mailh.status &  DB_MAIL_CREATED))	ERR_MSG_RET("E_NOMAILFROMYOU");
	if (gustat.act_mailh.status &  DB_MAIL_SCANNED)			ERR_MSG_RET("E_MLSALRSCANNED");

  sel=gpMain->m_subjlist.GetCurSel();
  LB_ERR_RET;
	rsel=sel;
	if (!gc.immediate_update)	   gpMain->force_current_mail_update();

	if (gustat.act_mailh.status & DB_FROZEN_MAIL)
	{
		gustat.act_mailh.status &= ~DB_FROZEN_MAIL;
		changed=1;
	}
	else
		gustat.act_mailh.status |= DB_FROZEN_MAIL;

	line=subject_list.GetString(sel);
	get_token(line,MAIL_INDEX,index);
	sscanf(index,"%d",&sel);

	ret=db_sethdr_by_index(gustat.cur_area_handle,gustat.act_mailh.index,&gustat.act_mailh);
	if (ret!=DB_OK)		ERR_MSG2_RET("E_DBSETHDRBYINDEX",ret);
	if (changed)	db_set_area_state(gustat.cur_area_handle,1);
	if (gustat.act_mailh.status & DB_FROZEN_MAIL)
		set_mailmark(rsel,'F');
	else
		set_mailmark(rsel,'P');

	gpMain->m_attrlist.Invalidate();
	gpMain->m_subjlist.SetFocus();
	if (gc.golded_compatible)
		gpMain->SendDlgItemMessage(IDC_SUBJLIST,WM_KEYDOWN,VK_RIGHT,0);
	else
		gpMain->SendDlgItemMessage(IDC_SUBJLIST,WM_KEYDOWN,VK_DOWN,0);
}

// ============================================
	void CLightApp::OnSetupTicechosetup()
// ============================================
{
	cfg_tics dlg;
	dlg.DoModal();
	gpMain->m_subjlist.SetFocus();
}

// ============================================
	void CLightApp::OnHatchFile()
// ============================================
{
	hatch dlg;
	dlg.DoModal();
	gpMain->m_subjlist.SetFocus();
}

// ============================================
	void CLightApp::OnBeginMultiSelection()
// ============================================
// UUDECODE Stuff
{
	if (err_out("DY_DYWBAMS")==IDNO)
		return;

	unlink(UUMULFILE);

	err_out("M_PCNMFUUD");
	gc.isMultipleSelektionMode=1;
	gpMain->m_subjlist.SetFocus();
}

// ============================================
	void CLightApp::OnEndMultiSelection()
// ============================================
// UUDECODE Stuff
{
	if (!gc.isMultipleSelektionMode)
	{
		MessageBeep(0);
		return;
	}

	gc.isMultipleSelektionMode=0;
	uudecode_from_file();
	gpMain->m_subjlist.SetFocus();
}

// ============================================
	BOOL add_mail_to_map(int ind,int &sect,int &mailno,int &nsect,BOOL &updated)
// ============================================
{
CString	tmp,str;
int		ret;
char	buf[MAX_MSG+1000],*p;

	str.Empty();
	sect=mailno=nsect=0;
	if (gpMain->m_subjlist.GetCount()<=ind)
		return 0;
	str=(LPSTR)gpMain->m_subjlist.GetItemDataPtr(ind);
	get_token(str,MAIL_INDEX,tmp);
	ret=sscanf(tmp,"%d",&mailno);
	ASSERT(ret);
    if (db_gethdr_by_index(gustat.ar2han,mailno,&(gustat.act_mailh))!=DB_OK)
		return 0;
// set DB_READ flag
	if (!(gustat.act_mailh.status & DB_MAIL_READ))
	{
		gustat.act_mailh.status|=DB_MAIL_READ;
		db_sethdr_by_index(gustat.ar2han,mailno,&(gustat.act_mailh));
		db_inc_read_counter(gustat.ar2han);
		updated |= 1;
	}
// trying find "(%d/%d)" in header
	p=strstr(gustat.act_mailh.subject,"(");
	if (p && sscanf(p,"(%d/%d)",&sect,&nsect)==2)
		return 1;	// found
// trying find "[%d/%d]" in header
	p=strstr(gustat.act_mailh.subject,"[");
	if (p && sscanf(p,"[%d/%d]",&sect,&nsect)==2)
		return 1;	// found
// trying find "section %d of %d" in mailtext
    if (db_get_mailtext(gustat.ar2han,&(gustat.act_mailh),buf,MAX_MSG)!=DB_OK)
		ERR_MSG2_RET0("DBGETMAILTEXT",gustat.ar2han);
	p=buf;
	do
	{
		p=strstr(p,"section ");
		if (!p)
			return 0;
		ret=sscanf(p,"section %d of %d",&sect,&nsect);
		p++;
	} while (ret<1);
	return 1;
}

#include "cuuendec.h"
#define RESULT_OK		0
#define RESULT_ERROR	1
#define RESULT_DONE		2
#define RESULT_CANCEL	3

// ============================================
	int decode_section(CUUDEC &uue,int mailno)
// ============================================
{
char	buf[MAX_MSG+1000],aline[1000],*pbeg,*pend;
int		state=0;

	db_gethdr_by_index(gustat.ar2han,mailno,&(gustat.act_mailh));
	db_get_mailtext(gustat.ar2han,&(gustat.act_mailh),buf,MAX_MSG);
	pbeg=buf;
	pend=pbeg;
	while (1)
	{
		if (*pend==0)	break;
		if (*pend=='\r')
		{
			if (pend-pbeg>=1000)
			{
				state=RESULT_ERROR;
				break;
			}
			memcpy(aline,pbeg,pend-pbeg+1);
			aline[pend-pbeg]=0;
			state=uue.DecodeLine(aline);
			if (state!=RESULT_OK)	break;
			pbeg=pend+1;
		}
		pend++;
	}
	return state;
}

// ============================================
// UUDECODE from selected Mails
	void CLightApp::OnUUFromSelected()
// ============================================
{
CMap<int,int,int,int>	seq;
CUUDEC	tmp(gpMain->m_hWnd);
CString	str,str1;
BOOL	sorted=FALSE,updated=FALSE;
int		count,selcnt,i,key,val,ret,ns,mailno,nsect=0,state=RESULT_ERROR;

	count=gpMain->m_subjlist.GetCount();
	selcnt=gpMain->m_subjlist.GetSelCount();
	if (selcnt==0)
		return;
	for (i=0;i<count;i++)	// trying build sorted map
	{
		if (gpMain->m_subjlist.GetSel(i))
		{
			if (!add_mail_to_map(i/*sel_mails[i]*/,key,val,ns,updated))
				continue;
			if (!nsect)
				nsect=ns;	// get number of sections
			seq[key]=val;
			sorted=TRUE;
		}
	}
	if (updated)			// refresh display after setting read status
	{
		save_mails_selection();
		gpMain->show_subjects(gustat.cur_area_handle);
		restore_mails_selection();
	}
	if (selcnt>1 && nsect!=seq.GetCount() && err_out("DY_UUEDIFFERNUM")!=IDYES)
		return;				// check number of sections if selected count >1
	for (i=1;i<=seq.GetCount();i++)	// check sequence of sections
	{
		if (!seq.Lookup(i,mailno))
		{
			sorted=FALSE;
			break;
		}
	}

	tmp.Reset();
	*uufname=0;
	if (sorted)	// in case of successful sorting
	{
		for (i=1;i<=seq.GetCount();i++)
		{
			seq.Lookup(i,mailno);
			state=decode_section(tmp,mailno);
			if (state!=RESULT_OK)
				break;
		}
	}
	else		// procceed unsorted list
	{
		for (i=0;i<count;i++)
		{
			if (gpMain->m_subjlist.GetSel(i))
			{
				str=(LPSTR)gpMain->m_subjlist.GetItemDataPtr(i);
				get_token(str,MAIL_INDEX,str1);
				ret=sscanf(str1,"%d",&mailno);
				ASSERT(ret);
				state=decode_section(tmp,mailno);
				if (state!=RESULT_OK)
					break;
			}
		}
	}
	if (state==RESULT_CANCEL)
		return;
	else if (state==RESULT_DONE)
		show_msg(L("S_176"));
	else if (state==RESULT_ERROR)
		ERR_MSG_RET("E_INCDECD")
	else
		ERR_MSG_RET("E_UUEINCOMPLETE")
}

//===================================================
	int ModifyCommDCB(LPDCB pdcb)
//===================================================
{
char buf[1000];
char command[100];
int  value;
int	 ret;

	pdcb->fDtrControl		=DTR_CONTROL_ENABLE;
	pdcb->fRtsControl		=RTS_CONTROL_ENABLE;
	pdcb->fDsrSensitivity	=FALSE;

	if (access("dcbinit.ctl",0)==0)
	{
		FILE *fp;

		fp=fopen("dcbinit.ctl","rt");
		while (fgets(buf,999,fp))
		{
			value=0;
			command[0]=0;
			ret=sscanf(buf,"%s %d",command,&value);
			if (ret==2 && (command[0]!=';'))
			{
				strupr(command);
				if (!strcmp(command,"OUTXCTS"))	pdcb->fOutxCtsFlow		=value;
				if (!strcmp(command,"OUTXDSR"))	pdcb->fOutxDsrFlow		=value;
				if (!strcmp(command,"DTR"))		pdcb->fDtrControl		=value;
				if (!strcmp(command,"DSRSEN"))	pdcb->fDsrSensitivity	=value;
				if (!strcmp(command,"RTS"))		pdcb->fRtsControl		=value;
			}
		}
		fclose(fp);
	}
	return 1 ;
}


// ============================================
	int callacc_initialize_modem(_modemcfg *mcfg)
// ============================================
// Hier inizialisieren wir zur Sicherheit das Modem 1 mal
{
UCHAR	lcom;
char	sbuf[300];
int		counter=40;

	while (counter>0)
	{
		if (gMailer.m_list.m_hWnd && IsWindow(gMailer.m_list.m_hWnd))
			break;

		Sleep(500);
		counter--;
	}
	if (!counter)
		return 0;

	modem_add_listbox(L("S_110"));
	lcom = mcfg->m_comport-1;				  // Supercom begins with 0
	sprintf(sbuf,L("S_281",lcom+1));
	modem_add_listbox(sbuf);

	ComInit(lcom);
	set_comport_options(lcom,mcfg);

	if (!init_modem(lcom,mcfg))	//->m_init1,mcfg->m_init2,mcfg->m_init3))
	{
		modem_add_listbox(L("S_444"));
		ComReset(lcom);
		return 0;
	}
	ComReset(lcom);
	return 1;
}

// ============================================
	void CLightApp::OnStartWWWBrowser()
// ============================================
{
char  mail[MAX_MSG];

	*mail=0;
	if (!get_selected_lines(mail,1) &&
		db_get_mailtext(gustat.ar2han,&gustat.act_mailh,mail,MAX_MSG)!=DB_OK)
		   return;

	find_weblinks(mail);
	exbrow dlg;
	dlg.DoModal();
	gpMain->m_subjlist.SetFocus();
}

// ============================================
	void CLightApp::OnUnmarkAllMessages()
// ============================================
{
	unmark_all_mails();
	gpMain->m_attrlist.Invalidate();
	gpMain->m_subjlist.SetFocus();
}

// ============================================
	void CLightApp::OnInfoCostcalculation()
// ============================================
{
	costuser dlg;
	dlg.DoModal();
	gpMain->m_subjlist.SetFocus();
}

// ============================================
	void CLightApp::OnInfoCostanalyze()
// ============================================
{
	costanal dlg;
	dlg.DoModal();
	gpMain->m_subjlist.SetFocus();
}

// ============================================
	void CLightApp::OnFindOriginalCopy()
// ============================================
{
	gpMain->FindOriginalMail();
}

// ============================================
	void CLightApp::OnCallsHistory()
// ============================================
{
	inouthis dlg;
	dlg.DoModal();
	gpMain->m_subjlist.SetFocus();
}

// ============================================
	void CLightApp::OnFaxSetup()
// ============================================
{
	cfg_fax dlg;
	dlg.DoModal();
	gpMain->m_subjlist.SetFocus();
}

// ============================================
	void CLightApp::OnLocallogin()
// ============================================
{
	gc.LocalBBSLogin=1;
	PulseEvent(gincome.EventToKillWaitCommEvent);
	modem_add_listbox(L("S_381"));	// Requesting BBS Local Login
}

// ============================================
	void CLightApp::OnAutosaveToArea()
// ============================================
{
	choosas dlg;
	if (dlg.DoModal()==IDCANCEL)	return;
	gpMain->OnAsave();
	gpMain->m_subjlist.SetFocus();
}

// ============================================
	void CLightApp::OnPollSystem()
// ============================================
{
	simppoll dlg;
	dlg.DoModal();
	((detmail *)gc.mailer.thisptr)->OnRescan();
}

// ============================================
	void CLightApp::OnSaveMarkedToSpecificArea()
// ============================================
{
	marksel dlg;
	if (dlg.DoModal()==IDCANCEL) return;
	choosas dlg1;
	if (dlg1.DoModal()==IDCANCEL) return;

	dlg.get_selection();
	if (show_first_selected())
	{
		do
			gpMain->OnAsave();
		while (show_next_selected());
	}
	gpMain->m_subjlist.SetFocus();
}

// ============================================
	void CLightApp::OnBbsusergroupsetup()
// ============================================
{
	bbsgrp dlg;
	dlg.DoModal();
	gpMain->m_subjlist.SetFocus();
}

// ============================================
	void CLightApp::OnBbsmainconfig()
// ============================================
{
	bbsmain dlg;
	dlg.DoModal();
	gpMain->m_subjlist.SetFocus();
}

// ============================================
	void CLightApp::OnBbsuserconfig()
// ============================================
{
	bbsuser dlg;
	dlg.DoModal();
	gpMain->m_subjlist.SetFocus();
}

// ============================================
	void CLightApp::OnBbsareaconfig()
// ============================================
{
	bbsarea dlg;
	dlg.DoModal();
	gpMain->m_subjlist.SetFocus();
}

// ============================================
	void CLightApp::OnOrderEchoes()
// ============================================
{
	valarea dlg;
	dlg.DoModal();
	gpMain->m_subjlist.SetFocus();
}

// ============================================
	void CLightApp::OnAllareasread()
// ============================================
{
	if (err_out("DN_DYRWMAMIAAAR") != IDYES)	return;
	gpMain->MarkAllAreasAsRead();
}

// ============================================
	void CLightApp::OnSortRecieve()
// ============================================
{
	gc.SortCriteria = (gc.SortCriteria & SORT_ORDER) | SORT_RECIEVE;
	gpMain->MailSort();
	check_sort(ID_SORT_RECIEVE);
}
// ============================================
	void CLightApp::OnSortCreate()
// ============================================
{
	gc.SortCriteria = (gc.SortCriteria & SORT_ORDER) | SORT_CREATE;
	gpMain->MailSort();
	check_sort(ID_SORT_CREATE);
}
// ============================================
	void CLightApp::OnSortFrom()
// ============================================
{
	gc.SortCriteria = (gc.SortCriteria & SORT_ORDER) | SORT_FROM;
	gpMain->MailSort();
	check_sort(ID_SORT_FROM);
}
// ============================================
	void CLightApp::OnSortTo()
// ============================================
{
	gc.SortCriteria = (gc.SortCriteria & SORT_ORDER) | SORT_TO;
	gpMain->MailSort();
	check_sort(ID_SORT_TO);
}
// ============================================
	void CLightApp::OnSortSubject()
// ============================================
{
	gc.SortCriteria = (gc.SortCriteria & SORT_ORDER) | SORT_SUBJECT;
	gpMain->MailSort();
	check_sort(ID_SORT_SUBJECT);
}
// ============================================
	void CLightApp::OnSortStatus()
// ============================================
{
	gc.SortCriteria = (gc.SortCriteria & SORT_ORDER) | SORT_STATUS;
	gpMain->MailSort();
	check_sort(ID_SORT_STATUS);
}
// ============================================
	void CLightApp::OnSortOrder()
// ============================================
{
	gc.SortCriteria ^= SORT_ORDER;
	gpMain->MailSort();
	check_sort(ID_SORT_ORDER);
}

// ============================================
	void CLightApp::OnResetdialogboxpos()
// ============================================
{
CString	str;

	if (err_out("DN_REARESDLGPOS") != IDYES)	return;
	str=get_cfg(CFG_DIALOGS,MAINWINID,"");
	del_cfg(CFG_DIALOGS);
	if (str.GetLength()>0)	set_cfg(CFG_DIALOGS,MAINWINID,str);
}

// ============================================
	void CLightApp::OnCfgFloating()
// ============================================
{
	cfg_icon dlg;
	dlg.DoModal();
	gpMain->m_subjlist.SetFocus();
}

// ============================================
	void CLightApp::OnSetupAdressbook()
// ============================================
{
	adrbook dlg;
	dlg.DoModal();
	gpMain->m_subjlist.SetFocus();
}


// ============================================
	void CLightApp::OnIncomingModem1()
// ============================================
{
	gpMain->OnIncomingModem1();
}

// ============================================
	void CLightApp::OnIncomingModem2()
// ============================================
{
	gpMain->OnIncomingModem2();
}

// ============================================
	int LineSplitterDetectedRing(int waitres)
// ============================================
// TOP: Diese Funktion testet ob der Line Splitter aktiviert
// wurde
// Returnwerte: 0 LineSplitter nicht aktiv, alles ganz normal behandeln.
//				1 LineSplitter aktiv, Ring nicht durchlassen.
{
static int last_1st=0;
static int last_2st=0;
int    now;
int    retval=0;
int    thediff=0;
char   buf[300];

	// Kein Line Splitter aktiv
	if (income_times.reserved3==0)
		return 0;

	now=time(NULL);
	switch (waitres)
	{
		case WAIT_OBJECT_0+1:
			// Splitter fuer dieses Modem nicht aktiv ..
			if (!(income_times.reserved3 & 1))
				return 0;

			thediff=(int)difftime(now,last_1st);
			if ((thediff >= income_times.reserved4) &&  (thediff <=income_times.reserved5))
			{
				retval=0;
				sprintf(buf,"Line-splitter: timediff=%d Sec.=>OK, accepting call",thediff);
				modem_add_listbox(buf);
			}
			else
			{
				retval=1;
				sprintf(buf,"Line-splitter enabled now for 1st modem");
				modem_add_listbox(buf);
				sprintf(buf,"Next_Ring_Intervall=[%d,%d]",income_times.reserved4,income_times.reserved5);
				modem_add_listbox(buf);
			}
			last_1st=now;
			break;
		case WAIT_OBJECT_0+2:
			// Splitter fuer dieses Modem nicht aktiv ..
			if (!(income_times.reserved3 & 2))
				return 0;

			thediff=(int)difftime(now,last_2st);
			if ((thediff >= income_times.reserved4) &&  (thediff <=income_times.reserved5))
			{
				retval=0;
				sprintf(buf,"Line-splitter: timediff=%d Sec.=>OK, accepting call",thediff);
				modem_add_listbox(buf);
			}
			else
			{
				retval=1;
				sprintf(buf,"Line-splitter enabled now for 2nd modem");
				modem_add_listbox(buf);
				sprintf(buf,"Next_Ring_Intervall=[%d,%d]",income_times.reserved4,income_times.reserved5);
				modem_add_listbox(buf);
			}
			last_2st=now;
			break;
	}
	return retval;
}

// =========================================================
	void CLightApp::OnMarkPresentAgain()
// =========================================================
{
	again dlg;
	dlg.DoModal();
	if (dlg.period < 0)	return;
	prepare_repost(dlg.period);
	show_msg(L("S_491",dlg.period));	// repost in %d days
	gpMain->m_subjlist.SetFocus();
}

// =========================================================
	void CLightApp::OnInfoPresentagain()
// =========================================================
{
	presinf dlg;
	dlg.DoModal();
	gpMain->m_subjlist.SetFocus();
}

// =============================================
	void CLightApp::OnCfgKludges() 
// =============================================
{
	kludges dlg;
	if (dlg.DoModal()==IDOK)
		gpMain->show_mail(&gustat);

	gpMain->m_subjlist.SetFocus();
}

// =============================================
// searches all WWW-links in form http://*,ftp://,www.*,ftp.*
	void find_weblinks(LPSTR text)
// =============================================
{
CString dm;
char msg[MAX_MSG];
char buf[300],tmpl[6];
char *p,*t;

	gWebAddrList.RemoveAll();
	strcpy(msg,text);
	strlwr(msg);

	for (int n=0;n<4;n++)
	{
		p=msg;
		if (n==0)
			strcpy(tmpl,"http:");
		if (n==1)
			strcpy(tmpl,"ftp:");
		if (n==2)
			strcpy(tmpl,"www.");
		if (n==3)
			strcpy(tmpl,"ftp.");

		while (p=strstr(p,tmpl))
		{
			if (p-msg>5 && !strncmp(p-6,"ftp://",6) ||
				p-msg>6 && !strncmp(p-7,"http://",7))
			{
				p+=strlen(tmpl);
				continue;
			}

			*buf=0;
			sscanf(p,"%s",buf);
			t=strchr(buf,',');
			if (t)	*t=0;
			t=strchr(buf,';');
			if (t)	*t=0;
			if (strlen(buf)>4 && gWebAddrList.FindString(buf,dm)<0)
				gWebAddrList.AddTail(buf);

			p+=strlen(buf);
		}
	}
}
	
BOOL CALLBACK EnumWindowsProc(HWND hw,LPARAM lp)
{
	char str[10];
	
	GetWindowText(hw,str,10);
	if (!strnicmp(str,"fips/",5))
	{
		if (IsIconic(hw))	ShowWindow(hw,SW_RESTORE);
		SetForegroundWindow(hw);
		return FALSE;
	}
	return TRUE;
}

// =========================================================
	void save_mail_to_file(BOOL selected)
// =========================================================
{
static CString path;
CString str,ename;
char		nname[100];
FILE		*fp;
int			ret;

	strcpy(nname,path);
	if (path.IsEmpty())
	{
		path=gustat.act_area.echotag;
		path.Replace('.','_');
		path.Replace("ASAVED:  ","");
		ename=path;
	}
	str.LoadString(IDS_TEXTFILT);
	CFileDialog	dlg(FALSE,"txt",path,OFN_HIDEREADONLY,str);
	ret=dlg.DoModal();
	restore_base_path();
	if (ret != IDOK)
	{
		path=nname;
		return;
	}
	path=dlg.GetPathName();
	if (!access(path,0))
	{
		al_exist dlg1;
		ret=dlg1.DoModal();
		if (ret==IDCANCEL)
		{
			path=nname;
			return;
		}
	}
	fp=fopen(path,ret==IDAPPEND ? "at" : "wt");
	if (fp)
	{
		gpMain->BuildUserWish(str,selected);
		str.Remove('\r');
		if (!IsOem(&font_mailtext))	str.AnsiToOem();
		fwrite(str,str.GetLength(),1,fp);
		fclose(fp);
		split_filename(path,0,0,nname,0);
		if (ename==nname) path="";
	}
	else
		ERR_MSG2_RET("E_COSSF",path);
}

// ====================================================
	int	prepare_repost(int delay)
// ====================================================
{
	_repost rp;
	FILE	*fp;
	char	base[MAX_PATH],filename[MAX_PATH],*p;
	UINT    now;
	int		ret;

	make_path(base,gc.MsgBasePath,REPOST_FLD); 
	mkdir(base);
	if (access(base,0))	ERR_MSG2_RET0("E_CANCDFW",base);

	_tzset();
	memset(&rp,0,sizeof(rp));
	strncpy(rp.echotag,gustat.act_area.echotag,99);
	rp.echotag[99]=0;
	rp.period=delay;
	rp.starttime=time(NULL)-_timezone;

	now=(rp.starttime/86400+rp.period)*86400;	// round to midnight
	do sprintf(filename,"%s\\%x",base,now++);
	while (!access(filename,0));

	fp=fopen(filename,"wb");
	if (!fp)	return 0;

	fwrite(&rp,sizeof(rp),1,fp);
	fwrite(&gustat.act_mailh,sizeof(mailheader),1,fp);

	p=(char *)malloc(MAX_MSG+100);
	if (!p)	ERR_MSG_RET0("E_MEM_OUT");

	ret=db_get_mailtext(gustat.cur_area_handle,&gustat.act_mailh,p,MAX_MSG);
	if (ret != DB_OK)
	{
		err_out("E_DBGETMAILTEXT",ret);
		free(p);
		return 0;
	}
	fwrite(p,gustat.act_mailh.text_length,1,fp);
	free(p);
	fclose(fp);
	return 1;
}
