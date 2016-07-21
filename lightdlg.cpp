// Lightdlg.cpp : implementation file
// IDD_MAIN_WINDOW, IDD_ABOUTBOX

#include "stdafx.h"
#include "supercom.h"
#include "bind.h"
#include <io.h>
#include "light.h"
#include "lightdlg.h"
#include "search.h"
#include "detpurg.h"
#include "detmail.h"
#include "dettoss.h"
#include "det_nl.h"
#include "writmail.h"
#include "areasel.h"
#include "filereq.h"
#include "reqsel.h"
#include "info_adr.h"
#include "rfilter.h"
#include "exsearch.h"
#include "bmpview.h"
#include "splash.h"
#include "gcomm.h"
#include "floating.h"
#include "badxfer.h"
#include "SAPrefsDialog.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern int	gLastUpdate;
extern	"C" { int set_gcomm_adress (void *); }
extern LangID LANG_ID;

extern CStrList   subject_list;
extern CWinThread *evtth;
extern CWinThread *Incometh;
extern HBRUSH hbackground;
extern HBRUSH hbackground_subject;
extern char		orgbuf[MAX_MSG+1000];
int		came_from_button=0;

CRITICAL_SECTION GlobalLogFileAccess;

CListBox 	*m_globedit;
detpurg		gPurger;
detmail 	gMailer;
dettoss 	gTosser;
floating 	gFloating;
bmpview 	gBmpView;
det_nl 	  gNLComp;
writmail 	gEditor;
search		gFind;
exsearch 	gSearch;

extern _gconfig gc;
struct _gcomm gcomm;

extern CWinThread *evtth;
CWinThread				*script_thread;

CStrList	LangStrList,SoundLst,HeaderList,OriginList,FooterList;
CStrList	converts,teltrans,dialbrk,trepls,jokes,income_times,resultdisp;
CStrList	Agroups,UserdefAreaList,AreasList,EMails,Aliases,AddKludges,HideKludges;
CStrList	eventlist,costbaselist,costuserlist,gatecfg,exfrcfg,faxcfg;
CStrList	Abook,magics,rpathes,remote,tics;
CMap<__int64,__int64,_tzone,_tzone&>	Tzutc;	// array of timezone params
CByteArray		AttrListBuffer;			// buffer for attributes list

int current_mail_line=-1;				// line position in current Mail
int current_mail_index=-1;				// index of current Mail
int current_mail_index_which=-1;		// index of current Mail
int cached_current_mail_index=-1;		// index of current cached Mail
int cached_current_mail_index_which=-1;	// index of current cached Mail
int last_area=-1;						// last area selected

int		IconOrder[MAX_ICONS];
_packer Packers[MAX_PACKERS];

CLightDlg *gpMain;
extern CLightApp FipsApp;
extern CStrList  gTimeouts;
extern CStrList  patterns_list;
extern CStrList  isdnlist;
extern CString  isdnaccept;
extern CStrList  actdisplist;
extern CFont	 font_mailtext;
extern CFont	 font_ANSI;
extern CFont	 font_OEM;
extern int		 DelayedAreas[100];
extern HANDLE	 gEventToKillUDP;
extern HINSTANCE hRes;

extern pareadef gAreadef;
extern int	colors_changed;

extern "C" {int capi_set_error_window_handle(HWND); }

CStrList	Akas;
CStrList	Nodelists;
CStrList	FoundAddresses;
CStrList	found_filenames;

int		selections_count,selections[1000];
int		SubjPos[3];
HACCEL	hAccel;
CRITICAL_SECTION NodeListAcces;

void load_timeouts_cfg	(void);
void load_cities_def	(void);
void compose_mail_dlg	(void);
int	 convert_settings	(void);
void sel_next_in_chain	(int direction);
int  remove_brackets	(LPSTR text);
BOOL city_from_nodelist	(int zone,int net,int node,int point,CString &city);
void add_subdlg			(CSAPrefsDialog &dlg,CSAPrefsSubDlg &page,LPCSTR name,CSAPrefsSubDlg *pnt=NULL);
void check_scales		(void);

static char DlgName[]=MAINWINID;

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	CStatic	m_backgrd;
	//}}AFX_DATA

// Implementation
protected:
	HCURSOR	hOldCursor;
	HCURSOR	hMailTo;
	HCURSOR	hGotoUrl;
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CLightDlg dialog

CLightDlg::CLightDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLightDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLightDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon=::LoadIcon(hRes,MAKEINTRESOURCE(IDI_FIPS));
	pTray=NULL;
}

// 	============================================================================
	void CLightDlg::DoDataExchange(CDataExchange* pDX)
// 	============================================================================

{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLightDlg)
	DDX_Control(pDX, IDC_ECHOTAG, m_echotag);
	DDX_Control(pDX, IDC_COUNTER, m_counter);
	DDX_Control(pDX, IDC_FROM, m_from);
	DDX_Control(pDX, IDC_SUBJECT, m_subject);
	DDX_Control(pDX, IDC_TO, m_to);
	DDX_Control(pDX, IDC_CITY, m_city);
	DDX_Control(pDX, IDC_CREATE, m_create);
	DDX_Control(pDX, IDC_FROM_ADDRESS, m_from_address);
	DDX_Control(pDX, IDC_RECEIVE, m_receive);
	DDX_Control(pDX, IDC_TO_ADDRESS, m_to_address);
	//	DDX_Control(pDX, IDC_TOPBOX, m_topbox);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CLightDlg, CDialog)
	//{{AFX_MSG_MAP(CLightDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_CTLCOLOR()
	ON_WM_CLOSE()
	ON_WM_RBUTTONDOWN()
	ON_WM_HELPINFO()
	ON_EN_UPDATE(IDC_TO_ADDRESS, OnUpdateToAddress)
	ON_LBN_SETFOCUS(IDC_ATTRLIST, OnSetfocusSmall)
	ON_LBN_DBLCLK(IDC_MAILTEXT, OnDblclkMailtext)
	ON_LBN_DBLCLK(IDC_SUBJLIST, OnDblclkList)
	ON_LBN_SELCHANGE(IDC_SUBJLIST, OnSelchangeList)
	ON_COMMAND(IDC_NEXT_AVAIL_SER, OnNextAvailSer)
	ON_COMMAND(IDC_FINDNEXT, OnFindNext)
	ON_COMMAND(ID_COPY_FROM, OnCopyFromToClipboard)
	ON_COMMAND(ID_COPY_TO, OnCopyToToClipboard)
	ON_COMMAND(ID_COPY_SUBJECT, OnCopySubjectToClipboard)
	ON_COMMAND(ID_INFO_SENDER, OnInfoSender)
	ON_COMMAND(ID_INFO_RECIPIENT, OnInfoRecipient)
	ON_COMMAND(IDC_LAST_SCALE, OnLastScale)
	ON_COMMAND(IDC_DELIVTIME, OnDeliveryTime)
	ON_COMMAND(ID_TOGGLE_USERMARK, OnToggleUsermark)
	ON_COMMAND(ID_CTLRETURN, OnCtrlReturn)
	ON_COMMAND(ID_SWITCH_ATTR, OnSwitchAttr)
	ON_COMMAND(ID_SWITCH_KLUDGE, OnSwitchKludge)
	ON_COMMAND(ID_SWITCH_DEL, OnSwitchDel)
	ON_COMMAND(ID_SPACEPRESSED, OnSpacePressed)
	ON_COMMAND(ID_PLUSPRESSED, OnPlusPressed)
	ON_COMMAND(ID_MINUSPRESSED, OnMinusPressed)
	ON_COMMAND(ID_MULTIPLYPRESSED, OnMultiplyPressed)
	ON_COMMAND(ID_DIVIDEPRESSED, OnDividePressed)
	ON_COMMAND(ID_LIST_BADXFER, OnBadXfer)
	ON_COMMAND(ID_SWITCH_CODING, OnSwitchCoding)
	ON_COMMAND(ID_SAVEORIGIN, OnSaveOrigin)
	ON_COMMAND(ID_TOPREVAREA,select_prevarea)
	ON_COMMAND(ID_TONEXTAREA,select_nextarea)
	ON_COMMAND(ID_DELETE_SELECTED, OnDeleteSelected)
	ON_COMMAND(ID_TOGGLE_STRUCT, OnToggleStruct)
	ON_COMMAND(ID_AREA, OnArea)
	ON_COMMAND(ID_SET_ENG, OnSetEng)
	ON_COMMAND(ID_SET_GER, OnSetGer)
	ON_COMMAND(ID_SET_RUS, OnSetRus)
	ON_COMMAND(ID_SET_FRE, OnSetFre)
	ON_COMMAND(ID_CONFIG, OnConfig)
	ON_COMMAND(ID_RESTORE, OnRestore)
	ON_BN_CLICKED(IDC_SHOW_ATTRIBUTES, OnShowAttributes)
	ON_BN_CLICKED(IDC_SHOW_HIDDEN, OnShowHidden)
	ON_BN_CLICKED(IDC_SHOW_DELETED, OnShowDeleted)
	ON_BN_CLICKED(IDC_ECHOTAG, OnEchotag)
	ON_BN_CLICKED(IDC_COUNTER, OnCounter)
	ON_BN_CLICKED(IDC_DESCRIPTION, OnDescription)
	ON_BN_CLICKED(IDC_ANIMAIL, OnAnimail)
	ON_BN_CLICKED(IDC_ANIPURGE, OnAnipurge)
	ON_BN_CLICKED(IDC_ANITOSS, OnAnitoss)
	ON_BN_CLICKED(IDC_TEXT_FROM, OnTextFrom)
	ON_BN_CLICKED(IDC_TEXT_TO, OnTextTo)
	ON_BN_CLICKED(IDC_NEVERDELETE, OnNeverDelete)
	ON_BN_CLICKED(IDC_NEXT, OnNext)
	ON_BN_CLICKED(IDC_PREV, OnPrevious)
	ON_BN_CLICKED(IDC_QUOTE, OnQuote)
	ON_BN_CLICKED(IDC_FILEREQUEST, OnFilerequest)
	ON_BN_CLICKED(IDC_FILT, OnFilter)
	ON_BN_CLICKED(ID_TOGGLE_SUB_DISP, OnToggleStruct)
	ON_BN_CLICKED(IDC_FIND, OnFind)
	ON_BN_CLICKED(IDC_MACRO1, OnMacro1)
	ON_BN_CLICKED(IDC_MACRO2, OnMacro2)
	ON_BN_CLICKED(IDC_MACRO3, OnMacro3)
	ON_BN_CLICKED(IDC_MACRO4, OnMacro4)
	ON_BN_CLICKED(IDC_MACRO5, OnMacro5)
	ON_BN_CLICKED(IDC_FREQ, OnFreq)
	ON_BN_CLICKED(IDC_ASAVE, OnAsave)
	ON_BN_CLICKED(IDC_ABORT, OnAbortscript)
	ON_BN_CLICKED(IDC_EXIT, OnExit)
	ON_BN_CLICKED(IDC_QUOTEA, OnQuoteA)
	ON_BN_CLICKED(IDC_NEW, OnNew)
	ON_BN_CLICKED(IDC_FILTER, OnFilterSet)
	ON_BN_CLICKED(IDC_CROSSJUMP, FindOriginalMail)
	ON_BN_CLICKED(IDC_TEXT_SUBJECT, OnTextSubject)
	ON_MESSAGE(UDM_EDITEND,OnRefreshAfterEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLightDlg message handlers

// 	============================================================================
	BOOL CLightDlg::OnInitDialog()
// 	============================================================================
{
WINDOWPLACEMENT wp;
CString	str;
int		ret;
int		tablist[]={10,110,200,400,1900};
CSize	chw;

	CDialog::OnInitDialog();
	hAccel=LoadAccelerators(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_ACCELS));
	SetIcon(m_hIcon,TRUE);
	SetIcon(m_hIcon,FALSE);
	gc.scaleupdate=FALSE;

#ifdef WATCHERROR
extern __WatchError(void);
	__WatchError();
#endif

	convert_settings();	// do convertion from cfg to ini if needed
	if (!gc.nosplash)
	{
		CSplashWnd::EnableSplashScreen(1);
		CSplashWnd::ShowSplashScreen(this);
	}
	gpMain=this;
	startup_init();

	gMailer.Create(IDD_STAT_MAILER);
	gTosser.Create(IDD_STAT_TOSS);
	gPurger.Create(IDD_STAT_PURGE);
	gNLComp.Create(IDD_STAT_NLCOMP);
	gBmpView.Create(IDD_BMP_VIEW);
	gSearch.Create(IDD_EXT_SEARCH);
	gFloating.Create(IDD_FLOATING_BAR);
	m_globedit=&m_mailview;
	
// init toolbars buttons
	create_button(m_autosave,IDC_ASAVE,IDI_ASAVE);
	create_button(m_prevarea,IDC_PREV,IDI_PREV);
	create_button(m_nextarea,IDC_NEXT,IDI_NEXT);
	create_button(m_find,IDC_FIND,IDI_FIND);
	create_button(m_newmail,IDC_NEW,IDI_NEW);
	create_button(m_quote,IDC_QUOTE,IDI_QUOTE);
	create_button(m_quotea,IDC_QUOTEA,IDI_QUOTEA);
	create_button(m_freq,IDC_FREQ,IDI_FREQ);
	create_button(m_udef1,IDC_MACRO1,IDI_MACRO1);
	create_button(m_udef2,IDC_MACRO2,IDI_MACRO2);
	create_button(m_udef3,IDC_MACRO3,IDI_MACRO3);
	create_button(m_udef4,IDC_MACRO4,IDI_MACRO4);
	create_button(m_udef5,IDC_MACRO5,IDI_MACRO5);
	create_button(m_exit,IDC_EXIT,IDI_EXIT);
	create_button(m_abortscript,IDC_ABORT,IDI_ABORT);
	create_button(m_filter,IDC_FILT,IDI_FILT);
	create_button(m_animail,IDC_ANIMAIL,IDI_MAILER);
	create_button(m_anitoss,IDC_ANITOSS,IDI_TOSSER);
	create_button(m_anipurg,IDC_ANIPURGE,IDI_PURGER);
	set_images();
// init switches
	create_switch(m_show_attributes,IDC_SHOW_ATTRIBUTES);
	create_switch(m_show_hidden,IDC_SHOW_HIDDEN);
	create_switch(m_show_deleted,IDC_SHOW_DELETED);
	m_show_attributes.SetPushed(gc.display_attributes);
	m_show_hidden.SetPushed(gc.show_seenby || gc.show_tags);
	m_show_deleted.SetPushed(!gc.hide_deleted);
	create_switch(m_text_from,IDC_TEXT_FROM);
	create_switch(m_text_to,IDC_TEXT_TO);
	create_switch(m_text_subject,IDC_TEXT_SUBJECT);
	
	VERIFY(m_subjlist.SubclassDlgItem(IDC_SUBJLIST,this));
	m_subjlist.type=SUBJ_LIST;
	VERIFY(m_attrlist.SubclassDlgItem(IDC_ATTRLIST,this));
	m_attrlist.type=ATTR_LIST;
	VERIFY(m_mailview.SubclassDlgItem(IDC_MAILTEXT,this));
	VERIFY(m_statusbar.CreateEx(SBARS_SIZEGRIP | SBARS_TOOLTIPS,WS_CHILD | WS_VISIBLE | CBRS_BOTTOM,
		CRect(0,0,0,0),this,IDC_STATUS_BAR));
	int sbparts[] = {0,0,0,0};
	m_statusbar.SetParts(sizeof(sbparts)/sizeof(int),sbparts);
	m_statusbar.SetText(gc.ViewOem ? "DOS" : "WIN",3,0);
// tooltips
	tip.Create(this);
	tip.SetMaxTipWidth(0xFFFFFF);
	tip.AddTool(&m_attrlist,"");
	tip.AddTool(&m_autosave,"");
	tip.AddTool(&m_find,"");
	tip.AddTool(&m_newmail,"");
	tip.AddTool(&m_quote,"");
	tip.AddTool(&m_quotea,"");
	tip.AddTool(&m_freq,"");
	tip.AddTool(&m_udef1,"");
	tip.AddTool(&m_udef2,"");
	tip.AddTool(&m_udef3,"");
	tip.AddTool(&m_udef4,"");
	tip.AddTool(&m_udef5,"");
	tip.AddTool(&m_exit,"");
	tip.AddTool(&m_abortscript,"");
	tip.AddTool(&m_filter,"");
	tip.AddTool(&m_animail,"");
	tip.AddTool(&m_anitoss,"");
	tip.AddTool(&m_anipurg,"");
	tip.AddTool(&m_prevarea,"");
	tip.AddTool(&m_nextarea,"");
	tip.AddTool(&m_counter,"");
	//tip.AddTool(&m_coding,"");
	tip.AddTool(&m_echotag,"");
	tip.AddTool(&m_show_attributes,"");
	tip.AddTool(&m_show_hidden,"");
	tip.AddTool(&m_show_deleted,"");
	tip.AddTool(&m_text_from,"");
	tip.AddTool(&m_text_to,"");
	tip.AddTool(&m_text_subject,"");
	tip.AddTool(&m_from,"");
	tip.AddTool(&m_to,"");
	tip.AddTool(&m_subject,"");
	tip.AddTool(&m_from_address,"");
	tip.AddTool(&m_to_address,"");
	tip.AddTool(&m_city,"");
	tip.AddTool(&m_create,"");
	tip.AddTool(&m_receive,"");
// enabling tooltips
	gc.NoTooltips=get_cfg(CFG_COMMON,"NoTooltips",0);
	tip.Activate(!gc.NoTooltips);
	EnableToolTips(!gc.NoTooltips);

	set_language();
	CenterWindow();
	InitializeFonts();
	AdjustItemSizes();	// Adjust widths of controls
	ForceBitmapPosition();

	m_mailview.itemhigh	=gColors[TEXT_LINE_DISTANCE];
	m_subjlist.itemhigh	=gColors[SUBJ_LINE_DISTANCE];
	m_attrlist.itemhigh	=gColors[SUBJ_LINE_DISTANCE];
	m_mailview.type=1;
	m_subjlist.SetTabStops(sizeof(tablist)/sizeof(int),tablist);
	get_fontsize(&font_mailtext,&m_mailview,chw);
	m_mailview.SetHorizontalExtent(chw.cx*82);
	m_subjlist.SetItemHeight(0,gColors[SUBJ_LINE_DISTANCE]);
	m_attrlist.SetItemHeight(0,gColors[SUBJ_LINE_DISTANCE]);
	m_mailview.SetItemHeight(0,gColors[TEXT_LINE_DISTANCE]);

	SetTimer(5463,250,NULL);

	AttrListBuffer.SetSize(100,100);		// init attributes array

// restore mail filter
	memset(&grfilter,0,sizeof(grfilter));
	grfilter.OnNoFilter=1;
	FILE *fpf=fopen("mfilter.cfg","rb");
	if (fpf)
	{
		int sizef;
		sizef=_filelength(_fileno(fpf));
		if (sizef==sizeof(grfilter))
		{
			fread(&grfilter,sizeof(grfilter),1,fpf);
			fclose(fpf);
			if (!grfilter.OnNoFilter)	gc.structured_display=0;
		}
		else
		{
			fclose(fpf);
			unlink("mfilter.cfg");
		}
	}

// get placement of main window
	if (!IsIconic())
	{
		str=get_cfg(CFG_DIALOGS,DlgName,"");
		ret=sscanf(str,"%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",&wp.length,&wp.flags,&wp.showCmd,
			&wp.ptMinPosition.x,&wp.ptMinPosition.y,&wp.ptMaxPosition.x,&wp.ptMaxPosition.y,
			&wp.rcNormalPosition.left,&wp.rcNormalPosition.top,
			&wp.rcNormalPosition.right,&wp.rcNormalPosition.bottom);
		if (ret==11)
			SetWindowPlacement(&wp);
	}
	resizing_on_hide_toolbox(1);
	if (get_cfg(CFG_COMMON,"LeftToolbar",0) == 0)	OnHideicons();
// displaying area

	gustat.cur_area_handle=0;
	select_area_by_index(gustat.cur_area_handle);

	if (get_cfg(CFG_COMMON,"RescanStart",1))
		gMailer.OnRescan();
	else
		modem_add_listbox(L("S_97"));	// autorescan disabled

	check_scales();

	gc.eventsenabled=get_cfg(CFG_COMMON,"EnableEvents",1);
	if (gc.eventsenabled)	gMailer.SetButtonsState();

	handle_autoexec();	// start 'autoexec.fps' if exists

// start threads
	FipsApp.StartEventWorkerThread();
	FipsApp.StartIncomeWorkerThread();
	FipsApp.StartUDPWorkerThread();
	return TRUE;
}

// 	============================================================================
	void CLightDlg::OnSysCommand(UINT nID, LPARAM lParam)
// 	============================================================================
{
static BOOL state=FALSE;
char buf[100];

	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else if (gc.MinToTray && nID == SC_MINIMIZE)	//minimizing
	{
		state=this->IsZoomed();
		ShowWindow(SW_HIDE);
		pTray = new CTrayIcon(this,m_hIcon,get_versioninfo(buf,1),IDI_PHOENIX);
		return;
	}
	else if (gc.MinToTray && nID == SC_RESTORE && lParam==0)	//restoring
	{
		delete pTray;
		pTray=NULL;
		ShowWindow(state ? SW_SHOWMAXIMIZED : SW_SHOW);
		return;
	}
	CDialog::OnSysCommand(nID, lParam);
}

// =============================================================
	void CLightDlg::OnDestroy()
// =============================================================
{
int		ret;

	set_cfg(CFG_COMMON,"LeftToolbar",!gc.HideIcons);
	gc.ExitEventThread=1;
	gc.SetOnFipsExit	=1;
// Inform Incoming-EMSI Thread that we are exiting ...
	gc.exitincomingemsi =1;

	ret=WaitForSingleObject(evtth->m_hThread,7000);
	if (ret	!= WAIT_OBJECT_0)
		TerminateThread(evtth->m_hThread,0);

	if (gEventToKillUDP!=INVALID_HANDLE_VALUE)
		SetEvent(gEventToKillUDP);

	SetEvent(gincome.EventToKillWaitCommEvent);
	ret=WaitForSingleObject(Incometh->m_hThread,1000);
	if (ret	!= WAIT_OBJECT_0)
		TerminateThread(Incometh->m_hThread,0);

	eventlist.SaveToFile("events.cfg");
// purge locked areas
	if (DelayedAreas[0]!=-1)
	{
		show_msg(L("S_439"),100);	// purging locked areas
		delayed_purge_on_exit();
	}

	free_all_stuff();
	WinHelp(0L, HELP_QUIT);
	evtth->Delete();
	CDialog::OnDestroy();
}

// =============================================================
// initialisation of global variables and structures
	void CLightDlg::startup_init(void)
// =============================================================
{
CString  str;
int		 now,i;

	memset(&gcomm,0,sizeof(gcomm));
// inform Supercom about address of structure
	set_gcomm_adress((PVOID)&gcomm);

	memset(typebuffer,0,sizeof(typebuffer));
	unlink(PRINT_FILE);
	now=GetTickCount();
	srand(now);	           	// randomize
	notbremse=0;
	gNewAreaFromExtSearch=0;
	notbremscounter=0;
	gc.NotBremseOn=0;
	gLastSucessfulCallTime=0;

	FILE *fpx=fopen(LastSucessfulCall,"rb");
	if (fpx)
	{
		fread(&gLastSucessfulCallTime,1,sizeof(int),fpx);
		fclose(fpx);
	}
	get_colors();
	memset(&DelayedAreas,-1,sizeof(DelayedAreas));

  InitializeCriticalSection(&GlobalLogFileAccess);

  *gMailbuf=0;
	jumpcount=0;
	memset(&gustat,0,sizeof(_ustat));			// Init global user status struct

	gc.scale=get_cfg(CFG_COMMON,"Scale",40);	// Init with 40 % Screen Size
	init_top_offsets();							// set initial top for labels
    load_editor_cfg();
	mutex_tosspurg=CreateMutex(0,FALSE,0);
	ASSERT(mutex_tosspurg!=INVALID_HANDLE_VALUE);
	get_soundlist(SoundLst);
	load_headers(HeaderList);
	load_footers(FooterList);
	load_origins(OriginList);
	load_timeouts_cfg();
	isdnlist.LoadFromDelimString(get_cfg(CFG_HWARE,"IsdnStrings",""),'\\');
	isdnaccept=get_cfg(CFG_HWARE,"IsdnAccept","");
	Agroups.LoadFromFile("receiver.cfg");
	eventlist.LoadFromFile("events.cfg");
	dialbrk.LoadFromFile("dialbrk.cfg");
	trepls.LoadFromDelimText("TextRepl.def",0,'\t');
	income_times.LoadFromFile("incomet.cfg");
	resultdisp.LoadFromFile("resudisp.cfg");
	exfrcfg.LoadFromFile("exfrtool.cfg");
	UserdefAreaList.LoadFromFile("udefdef.cfg");
	gatecfg.LoadFromFile("ingate.cfg");
	load_multimed_cfg();
	tics.LoadFromFile("tics.cfg");
// loading floatbar icons order
	str=get_cfg(CFG_COMMON,"FloatBar","");
	trim_all(str);
//	if (str.IsEmpty())	return;
	memset(IconOrder,0,MAX_ICONS*sizeof(int));
	i=0;
	do 
	{
		IconOrder[i]=get_token_int(str,i,',');
	} while(IconOrder[i++]>0);
	faxcfg.LoadFromFile("faxcfg.cfg");
	load_cities_def();
	EMails.LoadFromDelimText("Emails.def",0,',');
	Akas.LoadFromDelimString(get_cfg(CFG_COMMON,"Akas",""));
	split_string(get_cfg(CFG_COMMON,"Aliases",""),Aliases);
	get_packlist();

	while (faxcfg.GetCount()<4)
		faxcfg.AddTail("");

	costbaselist.LoadFromFile("costbase.cfg");
	costuserlist.LoadFromFile("costuser.cfg");
	if (costuserlist.GetCount()==0)
		costuserlist.AddTail("Default\tBitte дndern! Change me! Измените!");

	remote.LoadFromFile("remote.cfg");
	if (!init_tcpip_stuff())
		handle_tcpip_load_error();

	if (resultdisp.GetCount()==0)
	{
		resultdisp.RemoveAll();
		resultdisp.AddTail("ATSTI");
		resultdisp.AddTail("72,154");
		resultdisp.SaveToFile("resudisp.cfg");
	}

	Abook.LoadFromFile("adrbook.cfg");
	Nodelists.LoadFromFile("nodelist.cfg");
	converts.LoadFromDelimText("converts.def",0,'\t');
	teltrans.LoadFromFile("teltrans.cfg");
	initialize_jokes_list();

	InitializeCriticalSection(&NodeListAcces);	// tosser init
	system_sound("APPLICATION_START");
	load_modem_cfg(1);
	gc.ViewOem=get_cfg(CFG_COLORS,"ViewOem",0);
	gc.DisplayUserdefArea=get_cfg(CFG_COMMON,"ShortAreaList",0);
	gc.NoBlinkScrollLock=get_cfg(CFG_COMMON,"NoBlinkScrollLock",0);
	gc.MinToTray=get_cfg(CFG_COMMON,"MinToTray",0);
	gc.RepeatQuote=get_cfg(CFG_EDIT,"RepeatQuote",1);
	gc.MultiSpace=get_cfg(CFG_EDIT,"MultiSpace",0);
	gc.NdlPrefer=get_cfg(CFG_EDIT,"NdlPrefer",0);
	gc.DefaultConv=get_cfg(CFG_EDIT,"DefaultConv",1);
	strcpy(gc.FirstName,get_lim_cfg(CFG_COMMON,"FirstName","",99));
	strcpy(gc.SecondName,get_lim_cfg(CFG_COMMON,"SecondName","",99));
	SubjPos[0]=get_cfg(CFG_COLORS,"RecipientPos",200);
	SubjPos[1]=get_cfg(CFG_COLORS,"SubjectPos",400);
	SubjPos[2]=get_cfg(CFG_COLORS,"SizePos",600);
	get_hiddenkludges(HideKludges);
	gc.HiddenMode=get_cfg(CFG_EDIT,"HiddenMode",0);
	AddKludges.LoadFromDelimString(get_cfg(CFG_EDIT,"ExtraKludges",""),';');

	if (access("subcol.cfg",0))
	{
		init_empty_patterns();
		patterns_list.SaveToFile("subcol.cfg");
	}
	else
		patterns_list.LoadFromFile("subcol.cfg");
}

// =============================================================
	HCURSOR CLightDlg::OnQueryDragIcon()
// =============================================================
// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
{
	return (HCURSOR) m_hIcon;
}

// =====================================================================================
	void check_scales(void)
// =====================================================================================
// TOP: Diese Funktion setzt nach dem Laden die Checkboxes fuer
// die gc.scale richtig.
{
	if (gc.scale==10)		check_scale(ID_SCALE_10);
	if (gc.scale==20)		check_scale(ID_SCALE_20);
	if (gc.scale==30)		check_scale(ID_SCALE_30);
	if (gc.scale==40)		check_scale(ID_SCALE_40);
	if (gc.scale==50)		check_scale(ID_SCALE_50);
	if (gc.scale==60)		check_scale(ID_SCALE_60);
	if (gc.scale==70)		check_scale(ID_SCALE_70);
	if (gc.scale==80)		check_scale(ID_SCALE_80);
	if (gc.scale==90)		check_scale(ID_SCALE_90);
	if (gc.scale==100)	check_scale(ID_SCALE_100);
	if (gc.scale!=100)	gc.last_scale=gc.scale;
}

// =================================================
	void CLightDlg::OnClose()
// =================================================
{
WINDOWPLACEMENT wp;
CString	str;

	if (!confirm_aborting())	return;
	wp.length = sizeof(WINDOWPLACEMENT);
	GetWindowPlacement(&wp);
	str.Format("%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",wp);
	set_cfg(CFG_DIALOGS,DlgName,str);
	gNLComp.SavePosition();
	gMailer.SavePosition();
	gTosser.SavePosition();
	gPurger.SavePosition();
	gFloating.SavePosition();
	gSearch.SavePosition();
	if (pTray)	delete pTray;
	CDialog::OnClose();
}

// 	============================================================================
// Cancel handler. Kill dispatcher Thread
	void CLightDlg::OnExit()
// 	============================================================================
{
	if (!confirm_aborting())	return;
	OnClose();
	came_from_button=1;
	CLightDlg::OnCancel();
}

// ==================================================
	void CLightDlg::OnRestore()
// ==================================================
{
	PostMessage(WM_SYSCOMMAND, SC_RESTORE);	// do not change to ShowWindow!
}

// 	============================================================================
	int confirm_aborting()
// 	============================================================================
{
   if ((gc.detnl.running && err_out("DN_ABORTNLCOMP") != IDYES) ||
	   (gc.tosser.running && err_out("DN_ABORTTOSSER")!= IDYES) ||
	   (gc.purger.running && err_out("DN_ABORTPURGER")!= IDYES) ||
	   (gc.ineventaction && err_out("DN_ABORTEVENTS")  != IDYES) ||
	   (gc.mailer.running && !gc.NotBremseActive && err_out("DN_ABORTMAILER") != IDYES))
		     return 0;
   return 1;
}

// 	============================================================================
	void CLightDlg::free_all_stuff(void)
// 	============================================================================
{
	SetRollLock(0);
	reset_Notbremse();

	set_cfg(CFG_COMMON,"Scale",gc.scale);
 	unlink(PRINT_FILE);

	if (gc.LongUUBasePointer)
	{
		free(gc.LongUUBasePointer);
		gc.LongUUBasePointer=0;
	}

	ISDNCallAcceptionOnOff	(0);
	free_capi_stuff();
	free_dib_stuff();
	DeleteCriticalSection(&NodeListAcces);	// tosser free
	db_close_area_descriptions();
	db_close_all_areas();
	CloseHandle(mutex_tosspurg);
	mutex_tosspurg=INVALID_HANDLE_VALUE;
	free_tcpip_stuff();
	if (hbackground)
		DeleteObject(hbackground);
	if (hbackground_subject)
		DeleteObject(hbackground_subject);
	system_sound("APPLICATION_END");
	DeleteCriticalSection(&GlobalLogFileAccess);
}

// Focus handler
//void CLightDlg::OnEditFocus()		{ m_mailview.SetFocus(); }
//void CLightDlg::OnSubjectFocus()	{ m_subjlist.SetFocus(); }

// 	============================================================================
	void CLightDlg::OnTimer(UINT nIDEvent)
// 	============================================================================
{
//CClientDC	dc(this);
char		buf[300];
static int	x=0;
static int timerwashere=0;
static int blinkcnt=15;
static char lastcount[100];
static int	blink=0;
int			notread,cnt;
areadef		ad;

	if (gc.SetOnFipsExit)	return;
	gMailer.GraphicalStatusDisplay();
	if (gc.UpdateOnCancel==2)	// need Update?
	{
		gLastUpdate=0;
		gc.SuppressModeSwitch=1;
		gc.UpdateOnCancel=0;
		OnOK();
	}

	if (gLastUpdate && (GetTickCount()-gLastUpdate)>50)
	{
		gLastUpdate=0;
		if (gc.AreaSelectionMode)
		{
			gc.SuppressModeSwitch=0;
			gc.UpdateOnCancel=1;
		}
		else
		{
			gc.SuppressModeSwitch=1;
			OnOK();
		}
	}
	if (gc.ExitFips && gc.ExitFips++ >= 10)
	{
		KillTimer(5463);
		show_msg(L("S_607"));	// exiting
		OnExit();
	}

	timerwashere++;
	if (timerwashere==2 && strlen(gc.autoscript))
	{
		handle_startup_script(gc.autoscript);
		gc.autoscript[0]=0;
	}

	if (m_attrlist.GetTopIndex() != m_subjlist.GetTopIndex())
	   m_attrlist.SetTopIndex(m_subjlist.GetTopIndex());

	if (!x)
	{
		x=1;
		if (!gc.detscript.running)
		{
			db_get_area_by_index(gustat.act_area.index,&ad);
			cnt=m_subjlist.GetCount();
			notread=ad.number_of_mails-ad.number_of_read;
			if (strcmp(ad.echotag,BBSMAIL))
				if (cnt)
					if (notread>0)
						sprintf(buf,"%d/%d(%d)",m_subjlist.GetCurSel()+1,cnt,notread);
					else
						sprintf(buf,"%d/%d",m_subjlist.GetCurSel()+1,cnt);
				else
					if (notread>0)
						sprintf(buf,"(%d)",m_subjlist.GetCurSel()+1,cnt,notread);
					else
						*buf=0;
			else
				if (cnt)
					sprintf(buf,"%d",cnt);
				else
					*buf=0;

			if (strcmp(buf,lastcount))
			{
				m_counter.SetWindowText(buf);
				strcpy(lastcount,buf);
			}
		}
	}
	else
	   x=0;

// proccessing ScrollLock state
	if (!gc.NoBlinkScrollLock)
	{
		blinkcnt++;
		if (!(blinkcnt % 20))
		{
			blinkcnt=0;
			blink=gc.NoBlinkScrollLock && IsThereAInterestingEvent();
		}
		if (!(blinkcnt % 2))
		{
			gc.LastBlinkState=blink && ~gc.LastBlinkState;
			SetRollLock(gc.LastBlinkState);
		}
	}
	CDialog::OnTimer(nIDEvent);
}

// 	============================================================================
// left click - previous mail
	void CLightDlg::OnPrevious()
// 	============================================================================
{
	SendDlgItemMessage(IDC_SUBJLIST,WM_KEYDOWN,gc.golded_compatible ? VK_LEFT : VK_UP,0);
}

// 	============================================================================
// left click - next mail
	void CLightDlg::OnNext()
// 	============================================================================
{
	SendDlgItemMessage(IDC_SUBJLIST,WM_KEYDOWN,gc.golded_compatible ? VK_RIGHT : VK_DOWN,0);
}

// =====================================================
	void CLightDlg::OnRButtonDown(UINT nFlags, CPoint point)
// =====================================================
{
CRect	re;

	m_animail.GetWindowRect(&re);
	if (re.PtInRect(point))
	{
		if (gMailer.m_queue.GetCount()==0)
			gMailer.fill_queuelist();
		if (gc.mailer.running)
			gMailer.OnAbortsession();
		else
			gMailer.OnStart();
		return;
	}
	m_anitoss.GetWindowRect(&re);
	if (re.PtInRect(point))
	{
		FipsApp.OnStarttosser();
		return;
	}
	m_anipurg.GetWindowRect(&re);
	if (re.PtInRect(point))
	{
		if (gc.purger.running)
			gc.purger.breakit=1;
		else
			FipsApp.OnStartpurger();
		return;
	}
	m_prevarea.GetWindowRect(&re);
	if (re.PtInRect(point))
	{
		select_prevarea();
		return;
	}
	m_nextarea.GetWindowRect(&re);
	if (re.PtInRect(point))
	{
		select_nextarea();
		return;
	}
	CDialog::OnRButtonDown(nFlags, point);
}
	
// ============================================================================
// previous area selection
	void CLightDlg::select_prevarea()
// ============================================================================
{
int area,count;

	db_get_count_of_areas(&count);
	if (count<=0)	return;
	area=gustat.cur_area_handle-1;
	if (gc.DisplayUserdefArea)
	{
		check_udef_existence();
		area=get_next_area(-1);
		if (area==-1)	return;
	}
	if (area>=0)
	{
		gustat.cur_area_handle=area;
		select_area(TRUE,FALSE);
	}
}

// ============================================================================
// next area selection
	void CLightDlg::select_nextarea()
// ============================================================================
{
int area,count;

	db_get_count_of_areas(&count);
	if (count<=0)	return;
	area=gustat.cur_area_handle+1;
	if (gc.DisplayUserdefArea)
	{
		check_udef_existence();
		area=get_next_area(1);
		if (area==-1)	return;
	}
	if (area<count)
	{
		gustat.cur_area_handle=area;
		select_area(TRUE,FALSE);
	}
}

// ============================================================================
// new area selection
	void CLightDlg::OnArea()
// ============================================================================
{
	select_area(FALSE,FALSE);
}

// 	============================================================================
	void CLightDlg::select_first_mail_in_area()
// 	============================================================================
{
int sel=0;

	if (gc.first_non_read)
	{
		if (gustat.first_non_read_mail!=-1)
		{
			if (m_subjlist.GetCount() >gustat.first_non_read_mail)
			   sel=gustat.first_non_read_mail;
		}
		else
		{
		   sel=m_subjlist.GetCount()-1;
		   if (sel<0)  sel=0;
		}
	}
	else   // Display the last read mail
	{
		if (gustat.last_already_read_mail!=-1)
		{
			if (m_subjlist.GetCount()>gustat.last_already_read_mail)
			{
			   sel=gustat.last_already_read_mail;
			   if (sel<0)  sel=0;
			}
		}
		else
		   sel=0;
	}
    m_subjlist.SetSel(sel,1);
	m_subjlist.SetTopIndex(sel==0 ? 0 : (sel-1));
    OnSelchangeList();
}
	
// 	============================================================================
	void CLightDlg::OnSelchangeList()
// 	============================================================================
{
int sel;

	if (gc.immediate_update)	show_mail(&gustat);
	if (gc.autoscroll)	// Autoscroll
	{
		sel=m_subjlist.GetCurSel();
		m_subjlist.SetTopIndex(sel==0 ? 0 : (sel-1));
	}
	m_attrlist.SetTopIndex(m_subjlist.GetTopIndex());
	m_attrlist.SetCurSel(-1);
}

// 	============================================================================
// quick switch between 0% and 100% height of subject list
	void CLightDlg::OnOK()
// 	============================================================================
{
	if (gc.SuppressModeSwitch)
		gc.SuppressModeSwitch=0;
	else if (gc.scale==100)
		FipsApp.OnScale10();
	else if (gc.scale==10)
		FipsApp.OnScale100();
	else if (gc.immediate_update)
	{
		OnSelchangeList();
		return;
	}

	gLastUpdate=0;
	show_mail(&gustat);
	m_attrlist.SetTopIndex(m_subjlist.GetTopIndex());
	m_attrlist.SetCurSel(-1);
// 	CDialog::OnOK();
}

// 	============================================================================
// quick switch between 0% and 100% scale of subjects list
	void CLightDlg::OnDblclkList()
// 	============================================================================
{
	if (!gc.no_dblclick)
	{
		if (gc.SuppressModeSwitch)
			gc.SuppressModeSwitch=0;
		else if (gc.scale==100)
			FipsApp.OnScale10();
		else if (gc.scale==10)
			FipsApp.OnScale100();
	}
    gc.no_dblclick=0;
	show_mail(&gustat);
	m_attrlist.SetTopIndex(m_subjlist.GetTopIndex());
	m_attrlist.SetCurSel(-1);
}

// 	============================================================================
	void CLightDlg::OnSetfocusSmall()
// 	============================================================================
{
	m_subjlist.SetFocus();
}

// 	============================================================================
// AUTOSAVE Button
	void CLightDlg::OnAsave()
// 	============================================================================
{
	if(*gc.AutoSaveDestArea==0 && !strcmp(gustat.act_area.group,ASAVED_GROUP))
//		(gustat.act_area.membership & AUTOCREATEAD_ASAVE)
			ERR_MSG_RET("E_NOASAVEINAGROUP");

	autosave_mail();
	m_subjlist.SetFocus();
}

// 	============================================================================
// FIND Button
	void CLightDlg::OnFind()
// 	============================================================================
{
	if (gFind.m_hWnd)
		gFind.SetFocus();
	else
	{
		gFind.Create(IDD_SIMPLE_SEARCH);
		gFind.ShowWindow(SW_SHOWNORMAL);
	}
}

// 	============================================================================
// MACRO1 Button
	void CLightDlg::OnMacro1()
// 	============================================================================
{
	exec_script_key("User",1);
	m_subjlist.SetFocus();
}

// 	============================================================================
	void CLightDlg::OnMacro2()	// UDEF 2
// 	============================================================================
{
 	exec_script_key("User",2);
	m_subjlist.SetFocus();
}

// 	============================================================================
	void CLightDlg::OnMacro3() 	 // UDEF 3
// 	============================================================================
{
	exec_script_key("User",3);
	m_subjlist.SetFocus();
}

// 	============================================================================
	void CLightDlg::OnMacro4() 	 //  UDEF4
// 	============================================================================
{
	exec_script_key("User",4);
	m_subjlist.SetFocus();
}

// 	============================================================================
	void CLightDlg::OnMacro5() 	 //  UDEF5
// 	============================================================================
{
	exec_script_key("User",5);
	m_subjlist.SetFocus();
}

// 	============================================================================
	int CLightDlg::exec_script_key(LPCSTR pref,int num)
// 	============================================================================
{
CString	scr,name;
DWORD	dm;

   if (gc.detscript.running)
       return 0;

	name.Format("%s%d",pref,num);
	scr=get_cfg(CFG_SCRIPTS,name,"");
	if (scr.GetLength()<2)
		return 0;
	strcpy(gc.detscript.scriptname,scr);

	gc.detscript.running=1;
	script_thread=AfxBeginThread(ScriptWorkerThread,&dm);
	return 1;
}

// 	============================================================================
// searches in user selection for fido-addresses, phone-numbers and filenames
	void CLightDlg::OnFreq()    // File Request from MAIL
// 	============================================================================
{
CString	 dm;
char	 buf[MAX_MSG],tmp[1000];
char 	 *p,*x;
int  	 ret,w,len;

  FoundAddresses.RemoveAll();
  found_filenames.RemoveAll();
	*buf=0;
	if (!get_selected_lines(buf,1))	strcpy(buf,gMailbuf);   // get selection or full text
	m_from_address.GetWindowText(tmp,20);
	tmp[19]=0;
	FoundAddresses.AddTail(tmp);
	p=buf;
	while (1)
	{
		ret=0;
		if (sscanf(p,"%s%n",tmp,&ret)!=1)
		   break;
		p+=ret;
		// look for FIDO address
		remove_brackets(tmp);
		if (is_full_address(tmp))
		{
			if (FoundAddresses.FindString(tmp,dm)<0)
				FoundAddresses.AddTail(tmp);
			continue;
		}
		// look for MAGIC (all chars are capital)
		len=strlen(tmp);
		if (len>2)
		{
			for (w=0;w<len;w++)
			{
				if (!isupper(tmp[w]))
				    goto next;
			}
		    if (found_filenames.FindString(tmp,dm)<0)
				found_filenames.AddTail(tmp);
			continue;
		}
next:
		// look for Filename (*.*);
		x=strchr(tmp,'.');
		if (x>tmp && 1<tmp+len-x && tmp+len-x<5)
		{
			int b=true;
			for (w=0;w<len;w++)
			{
				if (tmp[w]=='.' && tmp+w==x)
					continue;	// second period
				if (!isalpha(tmp[w]) && !isdigit(tmp[w]) && !strchr("_^$~!#%&-{}@`'()",tmp[w]))
				{
					b=false;
				    break;
				}
			}
		    if (b && found_filenames.FindString(tmp,dm)<0)
				found_filenames.AddTail(tmp);
		}
	}
	if (found_filenames.GetCount()==0)
		show_msg("No files for FREQ was found!","Имена файлов в письме не найдены!");
	else
	{
		reqsel req;
		ret=req.DoModal();
		if	(ret==IDOK)
			OnFilerequest();
	}
	m_subjlist.SetFocus();
}

// 	============================================================================
// removes '(' and ')' from the string
	int remove_brackets(char *text)
// 	============================================================================
{
char tmp[300];
char *p;

   p=strchr(text,'(');
   if (p)
   {
   	   strcpy(tmp,p+1);
	   strcpy(text,tmp);
   }

   p=strchr(text,')');
   if (p)
      *p=0;

   return 1;
}

// 	============================================================================
// creates new mail
	void CLightDlg::OnNew()
// 	============================================================================
{
	if (gc.writemail_active)	return;
// no new mail in BBS and CarbonCopy areas
	if (gustat.act_area.localmail && strcmp(gustat.act_area.echotag,BBSMAIL))
		ERR_MSG_RET("E_NOREASON");
	if (!gc.immediate_update)	show_mail(&gustat);

	gustat.dest_area_handle=gustat.cur_area_handle;
	save_current_mail_index();
	gc.mode=MODE_NEW;
	compose_mail_dlg();
}

// 	============================================================================
// creates reply
	void CLightDlg::OnQuote()
// 	============================================================================
{
char	area[100],str[100],toptext[1000],*x;
long	hnd;

	if (gc.writemail_active)	return;
	if (!gc.immediate_update)	show_mail(&gustat);
	mark_as_answered(0);
	if (gustat.act_area.localmail && strcmp(gustat.act_area.echotag,BBSMAIL))
	{
// quote in CarbonCopy area
		if (db_get_mailtext(gustat.ar2han,&gustat.act_mailh,toptext,1000) != DB_OK)	return;
		CharToOem(langstr(CC_ENG,CC_RUS),str); // look for signature "CarbonCopy from area"
		x=strstr(toptext,str);
		if (!x || sscanf(x,"%*s %*s %*s %s",area) != 1)	ERR_MSG_RET("E_NOCCSIGN");
		if (db_get_area_handle(area,&hnd,0) != DB_OK)	ERR_MSG2_RET("GETAHANDLE",area);
		show_msg(L("S_372",area));	// Quoting to Area: %s
		gustat.dest_area_handle=hnd;
	}
	else
		gustat.dest_area_handle=gustat.cur_area_handle;

	save_current_mail_index();
	gc.mode=MODE_QUOTE;
	compose_mail_dlg();
}

// 	============================================================================
// QUOTE to another area
	void CLightDlg::OnQuoteA()
// 	============================================================================
{
CAreasel areasel;
areadef	 pa;
long     dm;
int		 ret;

	if (gc.writemail_active)	return;
	mark_as_answered(0);
	if (!gc.immediate_update)	force_current_mail_update();

	gc.mode=MODE_QUOTE;
	gc.AreaSelectionMode=1;
	gc.asel_as_list=FALSE;
	areasel.DoModal();
	gc.AreaSelectionMode=0;
	if (gc.selected_area<0)	return;
	ret=db_get_area_by_index(gustat.dest_area_handle,&pa);
	if (ret != DB_OK)	ERR_MSG2_RET("E_DBGETAREABYINDEX",ret);
	ret=db_get_area_handle(pa.echotag,&dm,0);
	if (ret != DB_OK)	ERR_MSG2_RET("E_DBGETAREAHANDLE",ret);
	save_current_mail_index();
	compose_mail_dlg();
}

// 	============================================================================
	void CLightDlg::FindOriginalMail(void)
// 	============================================================================
{
CString indexl,str;
char area[300],*p,*x;
int  index,top,sel,ret,hnd=0;

// Search only in LocalMail and CarbonCopy areas
	if (gustat.act_area.localmail == 1)// gustat.cur_area_handle==1)
	{
		ALLOC(p,MAX_MSG+100);
	  if (db_get_mailtext(gustat.ar2han,&gustat.act_mailh,p,MAX_MSG) != DB_OK)
	  {
		  free(p);
		  return;
		}
		OemToChar(p,p);
// Format: CarbonCopy from area WIN95.GER
 		x=strstr(p,CC_ENG);
 		if (!x)
			x=strstr(p,CC_RUS);
		if (!x)
		{
			free(p);
  		ERR_MSG_RET("E_NOCCMAIL");
		}
		sscanf(x,"%*s %*s %*s %s",area);
		free(p);
		if (db_get_area_handle(area,(long *)&hnd,0) != DB_OK)
			ERR_MSG2_RET("GETAHANDLE",area);

		GET_SEL(m_subjlist);
		str=subject_list.GetString(sel);
		get_token(str,MAIL_INDEX,indexl);
		ret=sscanf(indexl,"%d",&index);
		gustat.cur_area_handle=hnd;
		begin_area_selection(TRUE);
		gc.WatchDisplayMails=1;
		watchmail.dispoff=-1;
		watchmail.isCrossfind=1;
		memcpy(&watchmail.mh,&gustat.act_mailh,sizeof(gustat.act_mailh));
		show_subjects(hnd);
		gc.WatchDisplayMails=0;
		watchmail.isCrossfind=0;

		if (watchmail.dispoff==-1)
		{
			show_msg(L("S_614"),500);
			return;
		}
		m_subjlist.SetSel(-1,0);
		m_subjlist.SetSel(watchmail.dispoff,1);

		top=watchmail.dispoff-3;
		if (top < 0)	top=0;

		m_subjlist.SetTopIndex(top);
		OnSelchangeList();
		m_subjlist.SetFocus();
	}
	else
		ERR_MSG_RET("W_TFIOAIL");
}

// 	============================================================================
	void CLightDlg::OnFilerequest()
// 	============================================================================
{
	filereq dlg;
	dlg.DoModal();
	m_subjlist.SetFocus();
}

// =======================================================================
	void CLightDlg::OnFindNext()   // The hidden find next button
// =======================================================================
{
	gc.next_find=1;
	OnFind();
}

// =======================================================================
	HBRUSH CLightDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
// =======================================================================
{
HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	if (!gc.colored)
	    return hbr;

	if (!hbackground)
	   hbackground=CreateSolidBrush(gColors[TEXT_BACKGR_COLOR]);

	if (!hbackground_subject)
	   hbackground_subject=CreateSolidBrush(gColors[SUBJ_BACKGR_COLOR]);

	if (CTLCOLOR_LISTBOX==nCtlColor)
	{
	   if (pWnd==&m_mailview)
		   return hbackground;

	   if (pWnd==&m_subjlist || pWnd==&m_attrlist)
		   return hbackground_subject;

	}
	return hbr;
}

// =================================================
	void CLightDlg::OnAbortscript()
// =================================================
// Abort a running script
{
	gc.detscript.abort=1;
	gc.detscript.disableupdate=0;
	m_abortscript.SetWindowText("wait");
}

// =================================================
	void CLightDlg::OnAnswerAsNetmail()
// =================================================
{
    mark_as_answered(0);
	if (!gc.immediate_update)
	   force_current_mail_update();

	show_msg("Quote as NETMAIL","Ответ в NETMAIL");
	gustat.dest_area_handle=0;
	gc.mode=MODE_QUOTE;
	compose_mail_dlg();
}

// =================================================
	void CLightDlg::OnChangeCurrentMail() 	// CHANGE current mail
// =================================================
{
	if (!gc.immediate_update)
	   force_current_mail_update();

	if (!(gustat.act_mailh.status & DB_MAIL_CREATED))	// is own mail
		ERR_MSG_RET("E_NOMAILFROMYOU");

	if (gustat.act_mailh.status &  DB_MAIL_SCANNED)		// is scanned
	{
		if (err_out("DY_MAILSCANNED") != IDYES)   return;
	    gustat.act_mailh.status |=  DB_MAIL_CREATED;
	    gustat.act_mailh.status &=  ~DB_MAIL_SCANNED;
	    gustat.act_mailh.status &=  ~DB_MAIL_READ;
	}

	gc.mode=MODE_CHANGE;
	compose_mail_dlg();
}

// =================================================================
	void compose_mail_dlg(void)
// =================================================================
{
	if (gc.writemail_active)	return;
	gc.writemail_active=1;
	save_current_mail_index(1);
	gEditor.Create(IDD_WRITMAIL640);
	gEditor.ShowWindow(SW_SHOWNORMAL);
	gEditor.SetActiveWindow();
	gc.mode=0;
}

// =================================================
	void CLightDlg::force_current_mail_update(void)
// =================================================
{
int old,count,lines;
int arr[3000];

	count=m_mailview.GetSelItems(2999,arr);
	lines=m_mailview.GetCount();
	old=gc.immediate_update;
	gc.immediate_update=1;
	gc.detscript.disableupdate=1;
	OnSelchangeList();
	gc.detscript.disableupdate=0;
	gc.immediate_update=old;

	if (lines==m_mailview.GetCount())
	{
		for (int i=0;i<count;i++)
			m_mailview.SetSel(arr[i],TRUE);
	}
}

// 	============================================================================
	void  CLightDlg::MailSlotAlternateWrite(char *buf)
// 	============================================================================
{
int		cur,max,ret;
char	*p;

	if (!strcmp(buf,"UPDATE_MAILER"))
		gMailer.OnUpdateValues();
	else if (!strncmp(buf,"TOSSER_PROZ",11))	//	TOSSER_PROZ curent maximum
	{
		ret=sscanf(buf,"%*s %d %d",&cur,&max);
		if (ret==2)	gTosser.m_proz.SetRange32(1,max);
		gTosser.m_proz.SetPos(cur);
	}
	else if (!strncmp(buf,"MAILER_ADD_LISTBOX",18))	// MAILER_ADD_LISTBOX string
	{
		p=strchr(buf,' ');
		gMailer.OnAddToListBox(p+1);
	}
	else if (!strncmp(buf,"TOSSER_ADD_LISTBOX",18))	// TOSSER_ADD_LISTBOX string
	{
		p=strchr(buf,' ');
		gTosser.AddString(p+1);
	}
	else if (!strncmp(buf,"PAL",3))	// PURGE_ADD_LISTBOX string
	{
		p=strchr(buf,' ');
		gPurger.AddString(p+1);
	}
	else if (!strncmp(buf,"NL_ADD_LISTBOX",14))	// NL_ADD_LISTBOX string
	{
		p=strchr(buf,' ');
		gNLComp.AddString(p+1);
	}
}

// 	============================================================================
	void  CLightDlg::do_about()
// 	============================================================================
{
	CAboutDlg tmp;
	tmp.DoModal();
}

// 	============================================================================
	void CLightDlg::OnUpdateToAddress()
// 	============================================================================
{
	m_from_address.GetWindowText(gc.AddressForInfo,sizeof(gc.AddressForInfo)-1);
}

// 	============================================================================
	void CLightDlg::OnInfoAdress()
// 	============================================================================
{
	info_adr tmp;
	tmp.DoModal();
	m_subjlist.SetFocus();
}

// 	============================================================================
	void CLightDlg::OnCancel()
// 	============================================================================
{
	if (!came_from_button)
	   return;

	CDialog::OnCancel();
}

LRESULT CLightDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
 	if (message==WM_SYSCOMMAND && wParam==61536)
		  came_from_button=1;
	return CDialog::DefWindowProc(message, wParam, lParam);
}

// 	============================================================================
// toggles display mode between structured and normal display and refreshes display
	void CLightDlg::OnToggleStruct()
// 	============================================================================
{
	if (gc.structured_display)
		show_msg(L("S_593"));
	else
	{
		show_msg(L("S_592"));
		if (!grfilter.OnNoFilter)
		{
			remove_all_filters();
			grfilter.OnNoFilter=1;
	  	show_msg(L("S_591"));
			m_filter.SetPushed(0);
			gFloating.m_filter.SetPushed(0);
		}
	}

	gc.structured_display=!gc.structured_display;

	BeginWaitCursor();
	save_current_mail_index();
	show_subjects(gustat.cur_area_handle);
  restore_current_mail_index();
	EndWaitCursor();
	m_subjlist.SetFocus();
}

// ==================================================
// SPACE KEY: goto next unread Mail
	void CLightDlg::OnSpacePressed()
// ==================================================
{
mailheader mh;
int sel,ret,selected,scount;
CString str,index;
int		sitems[1000];

	sel=m_subjlist.GetCurSel();
	LB_ERR_RET;

	sel++;
	BeginWaitCursor();
	while (sel < m_subjlist.GetCount())
	{
		get_token(subject_list.GetString(sel),MAIL_INDEX,index);
		ret=sscanf(index,"%d",&selected);
		ASSERT(ret==1);

		if (db_gethdr_by_index(gustat.ar2han,selected,&mh)!=DB_OK)
		{
			EndWaitCursor();
			return;
		}

		if (mh.status & DB_MAIL_READ)	// skip read mail
		{
		   sel++;
		   continue;
		}
		if (!gc.MultiSpace)	// clear previous selection
		{
			scount=m_subjlist.GetSelItems(1000,sitems);
			for (int i=0;i<scount;i++)
				m_subjlist.SetSel(sitems[i],FALSE);
		}

		m_subjlist.SetSel(sel,TRUE);
		OnSelchangeList();
		m_subjlist.Invalidate();
		EndWaitCursor();
		return;
	}
	EndWaitCursor();

	if (get_cfg(CFG_COMMON,"LastAreaSelection",1))
		OnArea();
}

// ==================================================
// PLUS KEY: searches reply forward
	void CLightDlg::OnPlusPressed()
// ==================================================
{
	sel_next_in_chain(1);
}

// ==================================================
// MINUS KEY: searches original mail backward
	void CLightDlg::OnMinusPressed()
// ==================================================
{
	sel_next_in_chain(-1);
}

// ==================================================
// MULTIPLY KEY: searches replies with the same level
	void CLightDlg::OnMultiplyPressed()
// ==================================================
{
	sel_next_in_chain(0);
}

// ==================================================
// DIVIDE KEY: clearing selection marks
	void CLightDlg::OnDividePressed()
// ==================================================
{
}

// ================================================
	void CLightDlg::OnFilterSet()
// ================================================
{
	rfilter dlg;
	dlg.DoModal();
	save_current_mail_index();
	show_subjects(gustat.cur_area_handle);
    restore_current_mail_index();
	m_subjlist.SetFocus();
}

// ================================================
	void CLightDlg::OnEchotag()
// ================================================
{
	select_area(FALSE,TRUE);
}
// ================================================
	void CLightDlg::OnCounter()
// ================================================
{
	select_area(FALSE,TRUE);
}
// ================================================
	void CLightDlg::OnDescription()
// ================================================
{
	select_area(FALSE,TRUE);
}

// ================================================
// marks all mails in current area as read
	void CLightDlg::MarkAllMailsAsRead()
// ================================================
{
	set_attributes(0,TRUE);
	show_msg(L("S_176"));
	m_subjlist.SetFocus();
}

// ================================================
// marks all mails in all areas as read
	void CLightDlg::MarkAllAreasAsRead()
// ================================================
{
	BeginWaitCursor();
	db_mark_all_areas_as_read();
	set_attributes(0,TRUE);
	EndWaitCursor();
	show_msg(L("S_176"));
	m_subjlist.SetFocus();
}

// ================================================
// marks all mails as deleted
	void CLightDlg::MarkAllMailsAsDeleted()
// ================================================
{
	if (err_out("DY_DELALLMAILS")!=IDYES)	return;
	set_attributes(DB_DELETED,TRUE);
	show_msg(L("S_176"));
	m_subjlist.SetFocus();
}

// ================================================
	void CLightDlg::OnFilter()
// ================================================
{
static _rfilter oldfilter;

	gFloating.m_filter.SetPushed(m_filter.IsPushed());
	if (!m_filter.IsPushed())
	{
		oldfilter=grfilter;
		remove_all_filters();
		grfilter.OnNoFilter=1;
		show_msg(L("S_591"));	// filter off
	}
	else
	{
		grfilter=oldfilter;
		show_msg(L("S_590"));	// filter on
	}
	save_current_mail_index();
	show_subjects(gustat.cur_area_handle);
    restore_current_mail_index();
	m_subjlist.SetFocus();
}

// ==================================================
	void sel_next_in_chain(int direction)
// ==================================================
// movements in Replies chain
// if direction<0, searchs upwards
// if direction>0, searchs downwards
// if direction==0, searchs on same level
{
mailheader mh;
CString index;
int		sel,ret,selected,cnt,step;
ULONG	searchid;

	sel=gpMain->m_subjlist.GetCurSel();
	LB_ERR_RET;

	get_token(subject_list.GetString(sel),MAIL_INDEX,index);
	ret=sscanf(index,"%d",&selected);
	ASSERT(ret==1);

	if (db_gethdr_by_index(gustat.ar2han,selected,&mh)!=DB_OK)
		return;

	if (direction>0)
	{
		searchid=mh.mailid;
		step=1;
	}
	else
	{
		searchid=mh.replyid;
		step=-1;
	}
	
	if (searchid)
	{
		gpMain->BeginWaitCursor();
		show_msg("Looking for mail...","Поиск письма...");
		gpMain->m_subjlist.SetFocus();
		cnt=gpMain->m_subjlist.GetCount();
		for (int i=0;i<cnt;i++)
		{
			sel+=step;
			if (sel>=cnt)	sel=0;
			if (sel<0)		sel=cnt-1;
			get_token(subject_list.GetString(sel),MAIL_INDEX,index);
			sscanf(index,"%d",&selected);
			if (db_gethdr_by_index(gustat.ar2han,selected,&mh)!=DB_OK)
				break;

			if ((direction<0 && mh.mailid==searchid) ||
				(direction>0 && mh.replyid==searchid) ||
				(direction==0 && mh.replyid==searchid))
			{
				gpMain->m_subjlist.SetSel(sel,1);
				gpMain->OnSelchangeList();
				current_mail_index=-1;
				gpMain->EndWaitCursor();
				return;
			}
		}
		gpMain->EndWaitCursor();
	}
	show_msg(L("S_347"));	// not found
}

// =========================================================
	void save_mails_selection(void)
// =========================================================
{
	selections_count=gpMain->m_subjlist.GetSelItems(1000,selections);
	save_current_mail_index();
}

// =========================================================
	void restore_mails_selection(void)
// =========================================================
{
	for (int i=0;i<selections_count;i++)
		gpMain->m_subjlist.SetSel(selections[i]);
	restore_current_mail_index();
}
	
// =========================================================
	void save_current_mail_index(int which)
// =========================================================
{
mailheader mh;
CString str,index;
int sel,sel1,ret;

	sel=gpMain->m_subjlist.GetCurSel();
	if (sel==LB_ERR)
	{
		if (which)
			current_mail_index_which=-1;
		else
			current_mail_index=-1;
		return;
	}
	if (subject_list.GetCount() <= sel)	return;
	current_mail_line=gpMain->m_mailview.GetTopIndex();
	str=subject_list.GetString(sel);
	get_token(str,MAIL_INDEX,index);
	sscanf(index,"%d",&ret);
	if (which)
		current_mail_index_which=ret;
	else
		current_mail_index=ret;

	memset(&mh,0,sizeof(mailheader));

	if (which)
		db_gethdr_by_index(gustat.ar2han,current_mail_index_which,&mh);
	else
		db_gethdr_by_index(gustat.ar2han,current_mail_index,&mh);
// look for first non-deleted mail
	if ((mh.status & DB_DELETED) && gc.hide_deleted)
	{
// look forward
		sel1=sel+1;
		while (sel1 < gpMain->m_subjlist.GetCount())
		{
			str=subject_list.GetString(sel1);
			get_token(str,MAIL_INDEX,index);
			sscanf(index,"%d",&ret);
			db_gethdr_by_index(gustat.ar2han,ret,&mh);
			if (!(mh.status & DB_DELETED))
			{
				if (which)
					current_mail_index_which=ret;
				else
					current_mail_index=ret;

				return;
			}
		    sel1++;
		}
// look backward
		sel1=sel-1;
		while (sel1 >=0)
		{
			str=subject_list.GetString(sel1);
			get_token(str,MAIL_INDEX,index);
			sscanf(index,"%d",&ret);
			db_gethdr_by_index(gustat.ar2han,ret,&mh);
			if (!(mh.status & DB_DELETED))
			{
				if (which)
					current_mail_index_which=ret;
				else
					current_mail_index=ret;
				return;
			}
		    sel1--;
		}
		if (which)
			current_mail_index_which=-1;
		else
			current_mail_index=-1;
	}
}

// =========================================================
// sets focus to previously selected mail or last available
	void restore_current_mail_index(int which)
// =========================================================
{
mailheader mh;
CString	line;
int		sel,i;
ULONG   ind;

	if (which)
	{
		ind=current_mail_index_which;
		if (ind!=-1 && cached_current_mail_index_which!=-1)
		{
			gpMain->show_mail_by_index(cached_current_mail_index_which,which);
			goto exitrest;
		}
	}
	else
	{
		ind=current_mail_index;
		if (ind!=-1 && cached_current_mail_index!=-1)
		{
			gpMain->show_mail_by_index(cached_current_mail_index,which);
			goto exitrest;
		}
	}

	if (ind==-1)	return;
	gpMain->BeginWaitCursor();
	for (i=0;i<gpMain->m_subjlist.GetCount();i++)
	{
		get_token(subject_list.GetString(i),MAIL_INDEX,line);
		sscanf(line,"%d",&sel);
		db_gethdr_by_index(gustat.ar2han,sel,&mh);
		if (mh.index==ind)
		{
			gpMain->show_mail_by_index(i,which);
			goto exitrest;
		}
	}
	if (gpMain->m_subjlist.GetCount()>0)
		gpMain->show_mail_by_index(0,which);

exitrest:
	if (which)
	{
		current_mail_index_which=-1;
		cached_current_mail_index_which=-1;
	}
	else
	{
		current_mail_index=-1;
		cached_current_mail_index=-1;
	}
	gpMain->EndWaitCursor();
}

// =================================================
	void CLightDlg::show_mail_by_index(int ind,int mode)
// =================================================
{
	m_subjlist.SetSel(ind);
	m_subjlist.SetTopIndex(ind-1);
	m_attrlist.SetTopIndex(ind-1);
	show_mail(&gustat);
	m_mailview.SetTopIndex(current_mail_line);

	if (mode)
	{
		current_mail_index_which=-1;
		cached_current_mail_index_which=-1;
	}
	else
	{
		current_mail_index=-1;
		cached_current_mail_index=-1;
	}
}

// =================================================
	void CLightDlg::OnNeverDelete() 	  // ALT-L Key
// =================================================
{
CString line;
CString index;
int		sel,rsel,ret;

	if (!gustat.already_one_valid_mail)
	   return;

	sel=m_subjlist.GetCurSel();
	LB_ERR_RET;
	rsel=sel;

	if (!gc.immediate_update)
	   force_current_mail_update();

	line=subject_list.GetString(sel);

	get_token(line,MAIL_INDEX,index);
	ret=sscanf(index,"%d",&sel);
	ASSERT(ret);

	gustat.act_mailh.status |= DB_NEVER_DELETE;
	gustat.act_mailh.status &= ~DB_DELETED;
	ret=db_sethdr_by_index(gustat.cur_area_handle,gustat.act_mailh.index,&gustat.act_mailh);
	if (ret!=DB_OK)
		ERR_MSG2_RET("E_DBSETHDRBYINDEX",ret);

	set_mailmark(rsel,'A');
	m_attrlist.Invalidate();

	m_subjlist.SetFocus();
	if (gc.golded_compatible)
		SendDlgItemMessage(IDC_SUBJLIST,WM_KEYDOWN,VK_RIGHT,0);
	else
		SendDlgItemMessage(IDC_SUBJLIST,WM_KEYDOWN,VK_DOWN,0);
}

// =================================================
	void CLightDlg::SetNewMenu()
// =================================================
{
static CString curlang="";
FILE	*fp;
CMenu	sm;
char	buf[100],capt[100],*ch;
CString newlang,str;
int scan=0;

	switch (LANG_ID)
	{
		case LANG_GER:
			newlang="@SECTION GERMAN";
			break;
		case LANG_RUS:
			newlang="@SECTION RUSSIAN";
			break;
		case LANG_FRE:
			newlang="@SECTION FRENCH";
			break;
		default:
		case LANG_ENG:
			newlang="@SECTION ENGLISH";
			break;
	}
	if (curlang != newlang)
	{
		fp=fopen(FIPSMENUS,"rt"); if (!fp) ERR_MSG2_RET("E_CANLOLANF",FIPSMENUS);
		if (m_menu.GetSafeHmenu())	m_menu.DestroyMenu();
		m_menu.CreateMenu();
		fgets(buf,99,fp);
		do
		{
			if (*buf==0 || *buf==';' || *buf=='\n' || strchr(buf,'|'))
				goto nxt;
			if (scan==0 && !strnicmp(buf,newlang,strlen(newlang)))
			{
				scan=1;
				goto nxt;
			}
			if (scan==1 && !strnicmp(buf,"$MENU_MAIN",10))
			{
				scan=2;
				goto nxt;
			}
			if ((scan && !strnicmp(buf,"@section ",9)) || 
				(scan==2 && *buf=='$'))	break;
			if (scan==2)
			{
				strncpy(capt,buf,99);
				capt[99]=0;
				trim_all(capt);
				sm.CreatePopupMenu();
				load_submenu(buf,fp,&sm);
				m_menu.AppendMenu(MF_POPUP | MF_STRING | MF_ENABLED,(UINT)sm.m_hMenu,capt);
				sm.DestroyMenu();
				ch=buf;
				continue;
			}
nxt:		ch=fgets(buf,99,fp);
		}while(ch);
		fclose(fp);
	}
	if (scan)	SetMenu(&m_menu);	// if menu section was found
	curlang=newlang;
}

// =================================================================
	int CLightDlg::switch_left_icons(void)
// =================================================================
{
int status;

	status=gc.HideIcons ? SW_HIDE : SW_SHOWNORMAL;

	m_filter.ShowWindow(status);
	m_autosave.ShowWindow(status);
	m_find.ShowWindow(status);
	m_newmail.ShowWindow(status);
	m_quote.ShowWindow(status);
	m_quotea.ShowWindow(status);
	m_freq.ShowWindow(status);
	m_udef1.ShowWindow(status);
	m_udef2.ShowWindow(status);
	m_udef3.ShowWindow(status);
	m_udef4.ShowWindow(status);
	m_udef5.ShowWindow(status);
	m_exit.ShowWindow(status);

	if (gc.detscript.running)
		m_abortscript.ShowWindow(status);

	return 1;
}

// =================================================================
	void CLightDlg::OnHideicons()
// =================================================================
{
	gc.HideIcons=!gc.HideIcons;
	switch_left_icons();
	resizing_on_hide_toolbox(0);
	Invalidate();
//	redraw_all_elements();
}

// =================================================================
// updates main screen
	void CLightDlg::OnUpdateScreenDisplay()
// =================================================================
{
	save_current_mail_index();
	show_subjects(gustat.cur_area_handle);
    restore_current_mail_index();
	EndWaitCursor();
	m_subjlist.SetFocus();
}

// =================================================================
	LRESULT CLightDlg::OnRefreshAfterEdit(WPARAM w,LPARAM l)
// =================================================================
{
//	if (gustat.cur_area_handle==g2wustat.cur_area_handle)
//	{
		show_subjects(gustat.cur_area_handle);
	  restore_current_mail_index(1);
//	}
	m_subjlist.SetFocus();
	return 0;
}

// =================================================================
	void CLightDlg::OnDupeCheck()
// =================================================================
{
int	count;

	BeginWaitCursor();
	save_current_mail_index();
	count=check_dupes_for_area(gustat.cur_area_handle);
	db_refresh_area_info(gustat.cur_area_handle);
	EndWaitCursor();
	show_msg(L("S_6",count));
	tosser_add_listbox(L("S_6",count));
	show_subjects(gustat.cur_area_handle);
    restore_current_mail_index();
	m_subjlist.SetFocus();
}

// ============================================
	void CLightDlg::OnDupeCheckAll(void)
// ============================================
{
int	numar=0,count=0;

	db_get_count_of_areas(&numar);
	BeginWaitCursor();
	save_current_mail_index();
	for (int i=0;i<numar;i++)
	{
		db_open_simple(i);
		count+=check_dupes_for_area(i);
	}
	db_refresh_areas_info();
	EndWaitCursor();
	show_msg(L("S_6",count));	// deleted n dupes
	tosser_add_listbox(L("S_6",count));
	show_subjects(gustat.cur_area_handle);
    restore_current_mail_index();
	m_subjlist.SetFocus();
}

// ============================================
	void CLightDlg::OnNextAvailSer()
// ============================================
{
	if (gSearch.GetSafeHwnd())
		gSearch.DisplayNextResult();
}

// ============================================
	void CLightDlg::OnDblclkMailtext()
// ============================================
{
int		sel;
CString line;

	 sel=m_mailview.GetCurSel();
	 LB_ERR_RET;
	 line=actdisplist.GetString(sel);

	 // Doubleclick on .AWD line
	 if (strstr(line,"_Hinweis_") || strstr(line,"_Hint_") || strstr(line,"_Примечание_"))
	 {
		 handle_fax_doubleclick(PS line);
		 return;
	 }

	 // Doubleclick on .FAX line
	 if (strstr(line,".FAX"))
	 {
		 handle_fax_with_bgfax(PS line);
		 return;
	 }

	 handle_mm_stuff(PS line,3);
}

// ============================================
	BOOL CLightDlg::PreTranslateMessage(MSG *pMsg)
// ============================================
{
	if (pMsg)
	{
		if (tip)	tip.RelayEvent(pMsg);
		if (TranslateAccelerator(m_hWnd, hAccel, pMsg))	return TRUE;
		if (handle_function_key(pMsg))	return TRUE;
		if (pMsg->message==WM_RBUTTONDOWN && HandleRightButton(pMsg))	return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}

// ============================================
	void CLightDlg::OnCopyFromToClipboard()
// ============================================
{
char buf[100];

	m_from.GetWindowText(buf,100);
	copy_to_clipboard(buf);
	m_subjlist.SetFocus();
}

// ============================================
	void CLightDlg::OnCopyToToClipboard()
// ============================================
{
char buf[100];

	m_to.GetWindowText(buf,100);
	copy_to_clipboard(buf);
	m_subjlist.SetFocus();
}

// ============================================
	void CLightDlg::OnCopySubjectToClipboard()
// ============================================
{
char buf[100];

	m_subject.GetWindowText(buf,100);
	copy_to_clipboard(buf);
	m_subjlist.SetFocus();
}

// ============================================
	void CLightDlg::OnInfoSender()
// ============================================
{
	m_from_address.GetWindowText(gc.AddressForInfo,sizeof(gc.AddressForInfo)-1);
	gc.RequestFromMenu=1;
	info_adr dlg;
	dlg.DoModal();
	gc.RequestFromMenu=0;
	m_subjlist.SetFocus();

}

// ============================================
	void CLightDlg::OnInfoRecipient()
// ============================================
{
char buf[MAX_MSG];
char *p;
int	 n=0;

	if (gustat.act_area.index>1)
	{
// trying look for REPLY kludge
		db_get_mailtext(gustat.ar2han,&(gustat.act_mailh),buf,MAX_MSG);
		p=strstr(buf,"\001REPLY: ");
		if (p)
			n=strcspn(p+8," @\r");
		if (n==0)
			ERR_MSG_RET("M_INFONOTDEF");
		strncpy(gc.AddressForInfo,p+8,n);
		gc.AddressForInfo[n]=0;
	}
	else
		m_to_address.GetWindowText(gc.AddressForInfo,sizeof(gc.AddressForInfo)-1);
	gc.RequestFromMenu=1;
	info_adr dlg;
	dlg.DoModal();
	gc.RequestFromMenu=0;
	m_subjlist.SetFocus();
}

// ============================================
	void CLightDlg::OnLastScale()
// ============================================
{
	switch (gc.last_scale)
	{
		case 10:
			FipsApp.OnScale10();
			break;
		case 20:
			FipsApp.OnScale20();
			break;
		case 30:
			FipsApp.OnScale30();
			break;
		case 40:
			FipsApp.OnScale40();
			break;
		case 50:
			FipsApp.OnScale50();
			break;
		case 60:
			FipsApp.OnScale60();
			break;
		case 70:
			FipsApp.OnScale70();
			break;
		case 80:
			FipsApp.OnScale80();
			break;
		case 90:
			FipsApp.OnScale90();
			break;
		case 100:
			FipsApp.OnScale100();
			break;
	}
	m_subjlist.SetFocus();
}

// ============================================
	void CLightDlg::OnDeliveryTime()
// ============================================
{
CString create,receive;
int		diff,day,hour,min,create_time=0,receive_time=0;

	m_create.GetWindowText(create);
	m_receive.GetWindowText(receive);

	create_time=parse_time(create,TRUE);
	receive_time=parse_time(receive,TRUE);

	if (!create_time || !receive_time || create_time>receive_time)
		ERR_MSG_RET("E_TIMEINVAL");

	diff=receive_time-create_time;
	day=diff/86400;
	diff=diff%86400;
	hour=diff/3600;
	diff=diff%3600;
	min=diff/60;
	if (!day && !hour && !min)
		ERR_MSG_RET("M_EQUALTIME");

	err_out("M_MDELTIM",day,hour,min);
}

// ============================================
// toggle user mark for selected mails
	void CLightDlg::OnToggleUsermark()
// ============================================
{
pmailheader pm;
int		count,ret,mailno;
char	tmp[20];

	count=m_subjlist.GetCount();
	if (count<1)	return;
	pm=&gustat.act_mailh;
	for (int i=0;i<count;i++)
	{
		if (m_subjlist.GetSel(i))
		{
			get_token((LPCSTR)m_subjlist.GetItemDataPtr(i),MAIL_INDEX,tmp);
			ret=sscanf(tmp,"%d",&mailno);
			db_gethdr_by_index(gustat.cur_area_handle,mailno,pm);
			pm->status ^= DB_USERMARKED;
			AttrListBuffer[i] ^= 0x80;
			if (db_sethdr_by_index(gustat.cur_area_handle,mailno,pm) != DB_OK)
				ERR_MSG2_RET("E_DBSETHDRBYINDEX",ret);
		}
	}
	m_attrlist.Invalidate();
	m_subjlist.SetFocus();
}

// ============================================
// toggle displaying attributes
	void CLightDlg::OnSwitchAttr()
// ============================================
{
	m_show_attributes.SetPushed(!m_show_attributes.IsPushed());
	OnShowAttributes();
}

// ============================================
// toggle displaying kludges
	void CLightDlg::OnSwitchKludge()
// ============================================
{
	m_show_hidden.SetPushed(!m_show_hidden.IsPushed());
	OnShowHidden();
}

// ============================================
// toggle displaying deleted lines
	void CLightDlg::OnSwitchDel()
// ============================================
{
	m_show_deleted.SetPushed(!m_show_deleted.IsPushed());
	OnShowDeleted();
}

// ============================================
// display attributes
	void CLightDlg::OnShowAttributes()
// ============================================
{
	gc.display_attributes=m_show_attributes.IsPushed();
	set_cfg(CFG_EDIT,"ShowAttributes",gc.display_attributes);
	gc.disablectl=1;
	current_mail_line=m_mailview.GetTopIndex();
	show_mail(&gustat);
	m_mailview.SetTopIndex(current_mail_line);
	gc.disablectl=0;
}

// ============================================
// display hidden lines
	void CLightDlg::OnShowHidden()
// ============================================
{
	gc.show_seenby=gc.show_tags=m_show_hidden.IsPushed();
	set_cfg(CFG_EDIT,"HideSeenBy",!gc.show_seenby);
	set_cfg(CFG_EDIT,"HideTags",!gc.show_tags);
	gc.disablectl=1;
	show_mail(&gustat);
	gc.disablectl=0;
}

// ============================================
// display deleted mails
	void CLightDlg::OnShowDeleted()
// ============================================
{
	gc.hide_deleted=!m_show_deleted.IsPushed();
	set_cfg(CFG_EDIT,"HideDeleted",gc.hide_deleted);
	save_current_mail_index();
	show_subjects(gustat.cur_area_handle);
    restore_current_mail_index();
}

// ============================================
// delete selected mails
	void CLightDlg::OnDeleteSelected()
// ============================================
{
	FipsApp.OnDeleteMail();
}

// ============================================
// handling for Ctrl-Enter
	void CLightDlg::OnCtrlReturn()
// ============================================
{
	m_subjlist.SetFocus();
	if (gustat.act_mailh.status & DB_MAIL_CREATED)
		OnChangeCurrentMail();
	else
		OnQuote();
}
	
// ============================================
	void CLightDlg::RefreshTips(void) 
// ============================================
{
	tip.UpdateTipText(L("S_596"),&m_attrlist);
	tip.UpdateTipText(L("S_515"),&m_newmail);
	tip.UpdateTipText(L("S_516"),&m_quote);
	tip.UpdateTipText(L("S_517"),&m_quotea);
	tip.UpdateTipText(L("S_518"),&m_autosave);
	tip.UpdateTipText(L("S_519"),&m_find);
	tip.UpdateTipText(L("S_520"),&m_freq);
	tip.UpdateTipText(get_script_desc(1),&m_udef1);
	tip.UpdateTipText(get_script_desc(2),&m_udef2);
	tip.UpdateTipText(get_script_desc(3),&m_udef3);
	tip.UpdateTipText(get_script_desc(4),&m_udef4);
	tip.UpdateTipText(get_script_desc(5),&m_udef5);
	tip.UpdateTipText(L("S_521"),&m_exit);
	tip.UpdateTipText(L("S_588"),&m_abortscript);
	tip.UpdateTipText(L("S_530"),&m_filter);
	tip.UpdateTipText(L("S_527"),&m_animail);
	tip.UpdateTipText(L("S_528"),&m_anitoss);
	tip.UpdateTipText(L("S_529"),&m_anipurg);
	tip.UpdateTipText(L("S_522"),&m_prevarea);
	tip.UpdateTipText(L("S_523"),&m_nextarea);
	tip.UpdateTipText(L("S_524"),&m_counter);
	//tip.UpdateTipText(L("S_599"),&m_coding);
	tip.UpdateTipText(L("S_524"),&m_echotag);
	tip.UpdateTipText(L("S_531"),&m_show_attributes);
	tip.UpdateTipText(L("S_532"),&m_show_hidden);
	tip.UpdateTipText(L("S_533"),&m_show_deleted);
	tip.UpdateTipText(L("S_600"),&m_text_from);
	tip.UpdateTipText(L("S_600"),&m_text_to);
	tip.UpdateTipText(L("S_600"),&m_text_subject);
}
	
// ============================================
	void CLightDlg::RefreshInfoTips(void) 
// ============================================
{
char tmp[100];

	tip.UpdateTipText(get_deliverytime("S_525"),&m_create);
	tip.UpdateTipText(get_deliverytime("S_526"),&m_receive);
	tip.UpdateTipText(get_info(m_from_address),&m_from);
	tip.UpdateTipText(get_info(m_from_address),&m_from_address);
	tip.UpdateTipText(get_info(m_to_address),&m_to);
	tip.UpdateTipText(get_info(m_to_address),&m_to_address);
	m_subject.GetWindowText(tmp,72);
	tmp[71]=0;
	tip.UpdateTipText(tmp,&m_subject);
}
	
// ====================================================
	void CLightDlg::BuildUserWish(CString &result,BOOL usesel)
// ====================================================
{
CString area,from,from_adr,to,to_adr,subj,create;
char	mail[MAX_MSG];
	
	area.Format("%s (%s)",gustat.act_area.echotag,gustat.act_area.description);
	m_from.GetWindowText(from);
	m_from_address.GetWindowText(from_adr);
	m_to.GetWindowText(to);
	m_to_address.GetWindowText(to_adr);
	m_subject.GetWindowText(subj);
	m_create.GetWindowText(create);
	
	sprintf(mail,"AREA: %s\r\nFROM: %s  %s\r\nDATE: %s\r\nTO  : %s  %s\r\nSUBJ: %s\r\n\r\n",
		area,from,from_adr,create,to,to_adr,subj);
	
	if (gc.ViewOem)
		CharToOem(mail,mail);
	
	if (!usesel || !get_selected_lines(mail,1))
		strcat(mail,gMailbuf);
	
	strcat(mail,"\r\n");
	result=mail;
	result.Replace('\01','@');
}
	
// ====================================================
	void CLightDlg::AppendMailTofile(LPCSTR path)
// ====================================================
{
CString str;
FILE	*fp;
	
	BuildUserWish(str,TRUE);
	if (!gc.ViewOem)
		str.AnsiToOem();
	fp=fopen(path,"at");
	if (fp)
	{
		str.Remove('\r');
		fwrite(str,str.GetLength(),1,fp);
		fclose(fp);
	}
	else
		ERR_MSG2_RET("E_COSSF",path);
}
	
// ============================================
	void CLightDlg::MailSort(int from_script_file)
// ============================================
{
long last=GetTickCount();

	// waiting 30 seconds for finishing Tosser or Purger
	if (gc.tosser.running || gc.purger.running)
	{
		if (GetTickCount()-last > 30000)
		{
			if (!from_script_file)	err_out("W_WAITFORTPTFAILED");
			return;
		}
		show_msg(L("S_419"));	// sync threads...
		Sleep(500);
		m_subjlist.SetFocus();
	}

	BeginWaitCursor();
	save_current_mail_index();
	db_sort_area(gustat.cur_area_handle,gustat.cur_area_handle);
//	show_msg(L("S_420"));	// sorting finished
	show_subjects(gustat.cur_area_handle);
	restore_current_mail_index();
	EndWaitCursor();
	m_subjlist.SetFocus();
}

// ============================================
	void CLightDlg::OnIncomingModem1()
// ============================================
{
	gc.ManualAccept=1;
	PulseEvent(gincome.EventToKillWaitCommEvent);
}

// ============================================
	void CLightDlg::OnIncomingModem2()
// ============================================
{
	gc.ManualAccept=2;
	PulseEvent(gincome.EventToKillWaitCommEvent);
}

	void load_timeouts_cfg(void)
{
	gTimeouts.RemoveAll();
	gTimeouts.AddTail(get_cfg(CFG_TIMEOUT,"TryCounter","999"));
	gTimeouts.AddTail(get_cfg(CFG_TIMEOUT,"WaitTime","3"));
	gTimeouts.AddTail(get_cfg(CFG_TIMEOUT,"LoopCounter","100"));
}

// ===========================================
	void CLightDlg::SetLanguage(void)
// ===========================================
{
char str[100];
int  lng[]={
			IDC_TEXT_FROM,
			IDC_TEXT_TO,
			IDC_TEXT_SUBJECT,
			IDC_SHOW_ATTRIBUTES,
			IDC_SHOW_HIDDEN,
			IDC_SHOW_DELETED
			};
    set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));
	SetWindowText(get_versioninfo(str,0));
}

// ============================================
	void CLightDlg::select_area_by_index(int ind)
// ============================================
{
areadef ad;
int	count;

	db_get_count_of_areas(&count);
	if (count<=0 || ind<0 || ind>=count)
	   return;
	if (db_get_area_by_index(ind,&ad)!=DB_OK)
		ERR_MSG2_RET("E_AREANOTFOUND",ind);

	gustat.cur_area_handle=ind;
	m_attrlist.ResetContent();
	AttrListBuffer.RemoveAll();
	begin_area_selection(TRUE);
	show_subjects(gustat.cur_area_handle);
	m_attrlist.Invalidate();
	select_first_mail_in_area();
	m_subjlist.SetFocus();
}

// ============================================
	void CLightDlg::select_area_by_name(LPCSTR str)
// ============================================
{
CString buf;
int		sel;

	get_token(str,0,buf,' ');
	if (buf=="ASAVED:")
	{
		get_token(str+9,0,buf,' ');
		buf=ASAVED_PREFIX+buf;
	}
	sel=db_get_area_by_name(buf);
	if (sel==CB_ERR)
		return;
	select_area_by_index(sel);
}

// ============================================
	void CLightDlg::get_areainfo(areadef &ad,CString &str,int current)
// ============================================
{
int notread;

	notread=ad.number_of_mails-ad.number_of_read;
	if (notread<0)
		notread=0;

	if (!strcmp(ad.echotag,BBSMAIL))
	{
		str.Format("%-30s %5d         %s",ad.echotag,ad.number_of_mails,L("S_101"));	// BBS Local Mailarea
		return;
	}
	if (current==0)
	{
		if (notread)
			str.Format("%-30s %5d(%d) %s",
				ad.echotag,ad.number_of_mails,notread,ad.description);
		else
			str.Format("%-30s %5d         %s",
				ad.echotag,ad.number_of_mails,ad.description);
	}
	else
	{
		if (notread)
			str.Format("%-30s %5d/%5d(%-5d) %s",
				ad.echotag,current,ad.number_of_mails,notread,ad.description);
		else
			str.Format("%-30s %5d/%5d         %s",
				ad.echotag,current,ad.number_of_mails,ad.description);
	}
}

// ============================================
	void CLightDlg::select_area(BOOL skipdlg,BOOL as_list)
// ============================================
{
	if (!begin_area_selection(skipdlg,as_list))	return;
	show_subjects(gustat.cur_area_handle);
	m_mailview.ResetContent();
	select_first_mail_in_area();
	m_subjlist.SetFocus();
}

// 	============================================================================
	void CLightDlg::OnAnimail() 
// 	============================================================================
{
	FipsApp.OnSwitchMailer();
}
// 	============================================================================
	void CLightDlg::OnAnitoss() 
// 	============================================================================
{
	FipsApp.OnSwitchTosser();
}
// 	============================================================================
	void CLightDlg::OnAnipurge() 
// 	============================================================================
{
	FipsApp.OnSwitchPurger();
}

// =============================================
	void CLightDlg::OnBadXfer() 
// =============================================
{
	badxfer dlg;
	dlg.DoModal();
	m_subjlist.SetFocus();
}

// =============================================
	void CLightDlg::OnSwitchCoding() 
// =============================================
{
LOGFONT lf;

	gc.ViewOem=!gc.ViewOem;
	if (!IsOem(&font_mailtext))
	{
		font_OEM.GetLogFont(&lf);
		m_statusbar.SetText("DOS",3,0);
	}
	else
	{
		font_ANSI.GetLogFont(&lf);
		m_statusbar.SetText("WIN",3,0);
	}
	
	set_mailtext_font(&lf);
	m_mailview.SetFont(&font_mailtext);
	show_mail(&gustat);
	m_subjlist.SetFocus();
}

// ============================================
	void CLightDlg::create_button(SButton &ctl,int id,int resid)
// ============================================
{
	ctl.SubclassWindow(GetDlgItem(id)->GetSafeHwnd());
	ctl.SetStyleFlat(TRUE);
	ctl.SetFrameWidth(SButton::THICK_FRAME);
}
	
// ============================================
	void CLightDlg::set_images(void)
// ============================================
{
	m_autosave.SetImages(IDI_ASAVE);
	m_prevarea.SetImages(IDI_PREV);
	m_nextarea.SetImages(IDI_NEXT);
	m_find.SetImages(IDI_FIND);
	m_newmail.SetImages(IDI_NEW);
	m_quote.SetImages(IDI_QUOTE);
	m_quotea.SetImages(IDI_QUOTEA);
	m_freq.SetImages(IDI_FREQ);
	m_udef1.SetImages(IDI_MACRO1);
	m_udef2.SetImages(IDI_MACRO2);
	m_udef3.SetImages(IDI_MACRO3);
	m_udef4.SetImages(IDI_MACRO4);
	m_udef5.SetImages(IDI_MACRO5);
	m_exit.SetImages(IDI_EXIT);
	m_abortscript.SetImages(IDI_ABORT);
	m_filter.SetImages(IDI_FILT);
	m_animail.SetImages(IDI_MAILER);
	m_animail.SetAnimation(IDR_MAILER);
	m_anitoss.SetImages(IDI_TOSSER);
	m_anitoss.SetAnimation(IDR_TOSSER);
	m_anipurg.SetImages(IDI_PURGER);
	m_anipurg.SetAnimation(IDR_PURGER);
	Invalidate();
}

// ============================================
	void CLightDlg::create_switch(SButton &ctl,int id)
// ============================================
{
	ctl.SubclassWindow(GetDlgItem(id)->GetSafeHwnd());
	ctl.SetImages(NULL);
	ctl.SetStyleFlat(TRUE);
	ctl.SetStyleToggle(TRUE);
	ctl.SetStyleHotText(TRUE);
}

//===================================================
	BOOL CLightDlg::OnHelpInfo(HELPINFO* pHelpInfo) 
//===================================================
{
	FipsApp.OnHelpMe();
	return TRUE;
}

// ============================================
	void CLightDlg::OnSaveOrigin()
// ============================================
{
CString tmp,str,orig;
char *p,*s,*t;
char buf[80];

	p=(char *)malloc(MAX_MSG);
	if (!p)
		ERR_MSG_RET("E_MEM_OUT");

	if (db_get_mailtext(gustat.ar2han,&gustat.act_mailh,p,MAX_MSG)==DB_OK)
	{
		s=strstr(p,"\r * Origin: ");
		if (s)
		{
			strncpy(buf,s+12,79);
			buf[79]=0;
			t=strchr(buf,'(');
			if (t)
				*t=0;
			str=buf;
			str.OemToAnsi();
			russian_conversion(str);
			trim_all(str);
			if (OriginList.FindString(str,tmp)<0)
			{
				OriginList.AddTail(str);
				orig=get_cfg(CFG_EDIT,"OriginFile","");
				if (orig.IsEmpty() || access(orig,0))
					OriginList.SaveAsEdit("origins.def");
				else
					OriginList.SaveAsEdit(orig);
				show_msg("Origin was saved","Ориджн сохранен");
			}
		}
	}
   free(p);
}

//===================================================
// checks and converts old cfg-settings to ini-ones
	int convert_settings(void)
//===================================================
{
CString	val,str,fname,sect,key,ind;
CStrList uf,cf;
FILE	*fp;
char	*t;
int		ret,k,b,typ=0;
BOOL	isf=FALSE;
char	tmp[100];
long	la[20];

	if (access("update.ini",0))	return FALSE;
	ret=uf.LoadFromDelimText("update.ini",0,',');
	set_cfg(CFG_COMMON,"Language","Russian",TRUE);
	for (int i=0;i<uf.GetCount();i++)
	{
		uf.GetString(i,val);
		if (val.Find('\t')<0)
		{
			isf=FALSE;
			ret=sscanf(val,"%s %d",fname,&typ);
			ASSERT(ret==2);
			if (access(fname,0))
				continue;
			if (typ==3)	// SPACE-delimited string
			{
				fp=fopen(fname,"rt");
				if (!fp)
					continue;
				t=fgets(tmp,100,fp);
				fclose(fp);
				if (!t)
					continue;
				str=tmp;
			}
			else if (typ==4)	// Binary array of longs
			{
				fp=fopen(fname,"rb");
				if (!fp)
					continue;
				fread(la,1,sizeof(la),fp);
				fclose(fp);
			}
			else
			{
				cf.LoadFromFile(fname);
				if (cf.GetCount()==0)
					continue;
				cf.GetString(0,str);
			}
			isf=TRUE;
		}
		else if (isf)
		{
			get_token(val,0,sect);
			get_token(val,1,key);
			get_token(val,2,ind);
			k=atoi(ind);
			val.Empty();
			switch (typ)
			{
				case 1:	// TAB-delimited cfg
					if (key=="Akas")
						cf.SaveToDelimString(val);	// LF-delimited -> ','-delimited
					else
						get_token(str,k,val);
					break;
				case 2:	// SPACE-delimited cfg
				case 3:	// SPACE-delimited string
					get_token(str,k,val,' ');
					break;
				case 4:	// Binary array[20] of longs
					if (k<20)
						val.Format("%d",la[k]);
					else
						goto exitloop;
					break;
				default:// LF-delimited cfg
					if (k<cf.GetCount())
					{
						cf.GetString(k,val);
						if (sect=="Bosses")
						{
							val.Replace("\t",";");
							val+="0;0";	// add no-HYDRA & no-ZMODEM options
						}
						else if (sect=="Packers")
						{
							get_token(val,0,key);
							val.Replace("\t",";");
							key.MakeUpper();	// key=name of archiver
						}
						else
							val.Replace("\t",",");
					}
					else
						goto exitloop;
			}
			if (FipsApp.GetProfileString(sect,key,"nokey")=="nokey")
				FipsApp.WriteProfileString(sect,key,val);
exitloop:;
		}
	}
	set_cfg(CFG_COMMON,"DefaultFreeze",0,TRUE);
	set_cfg(CFG_COMMON,"CharConversion",0,TRUE);
	b=!access("udefdisp.cfg",0) ? 1 : 0;
	set_cfg(CFG_COMMON,"ShortAreaList",b,TRUE);
	b=!access("hideicon.cfg",0) ? 0 : 1;
	set_cfg(CFG_COMMON,"LeftToolbar",b,TRUE);
	set_cfg(CFG_EDIT,"AltColored",1,TRUE);
	set_cfg(CFG_EDIT,"ClearTearline",0,TRUE);
	set_cfg(CFG_EDIT,"NetmailOrigin",0,TRUE);
	set_cfg(CFG_EDIT,"MaxUueLines",340,TRUE);
	set_cfg(CFG_EDIT,"Tearline",get_versioninfo(tmp,0),TRUE);
	set_cfg(CFG_PURGER,"DeleteSeenBy",0,TRUE);
	set_cfg(CFG_PURGER,"NotProcAsaved",0,TRUE);
	MoveFileEx("update.ini","update.bak",MOVEFILE_REPLACE_EXISTING);
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Control(pDX, IDC_ABOUT, m_backgrd);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// ============================================
	BOOL CAboutDlg::OnInitDialog()
// ============================================
{
RECT	re;
int		w,h,w1,h1;

	CDialog::OnInitDialog();
	hOldCursor=GetCursor();
	hMailTo=::LoadCursor(hRes,MAKEINTRESOURCE(IDC_MAILTO));
	hGotoUrl=::LoadCursor(hRes,MAKEINTRESOURCE(IDC_GOTOURL));
	set_transparent_bmp(&m_backgrd,IDB_ABOUT);
	GETRECT(m_backgrd,re);
	h=re.bottom-re.top;
	w=re.right-re.left;
	GetWindowRect(&re);
	h1=re.bottom-re.top;
	w1=re.right-re.left;
	re.top=re.top-(h-h1)/2;
	re.left=re.left-(w-w1)/2;
	re.bottom=re.top+h+6;
	re.right=re.left+w+6;
	MoveWindow(&re);
	return TRUE;
}

#define TMARG 0.95
#define EMARG 2/5
#define UMARG 4/6
// ============================================
	void CAboutDlg::OnLButtonUp(UINT nFlags, CPoint point)
// ============================================
{
RECT	re;
int		w;
	
	GetWindowRect(&re);
	ScreenToClient(&re);
	w=re.right-re.left;
	if (point.y>re.bottom*TMARG)
	{
		if (point.x<w*EMARG)
			ShellExecute(0,"open","mailto:FIPS-feedback<forfips@mail.ru>",0,0,SW_SHOWNORMAL);
		else if (point.x>w*UMARG)
			ShellExecute(0,"open","http://www.fips.tk/",0,0,SW_SHOWNORMAL);
		else
			OnCancel();
	}
	else
		OnCancel();
}

// ============================================
	void CAboutDlg::OnMouseMove(UINT nFlags, CPoint point)
// ============================================
{
RECT	re;
int		w;
	
	GetWindowRect(&re);
	ScreenToClient(&re);
	w=re.right-re.left;
	if (point.y>re.bottom*TMARG)
	{
		if (point.x<w*EMARG)
			SetCursor(hMailTo);
		else if (point.x>w*UMARG)
			SetCursor(hGotoUrl);
		else
			SetCursor(hOldCursor);
	}
	else
		SetCursor(hOldCursor);

	CDialog::OnMouseMove(nFlags, point);
}

// ===========================================
   void set_mailmark(int ind,char s)
// ===========================================
{
	if (ind >= AttrListBuffer.GetCount())
		AttrListBuffer.SetAtGrow(ind,s);
	else
		AttrListBuffer[ind] = s | (AttrListBuffer[ind] & 0x80);
}

// ===========================================
	void CLightDlg::OnSetEng(void)
// ===========================================
{
	set_cfg(CFG_COMMON,"Language","English");
	set_language();
}

// ===========================================
	void CLightDlg::OnSetGer(void)
// ===========================================
{
	set_cfg(CFG_COMMON,"Language","German");
	set_language();
}

// ===========================================
	void CLightDlg::OnSetRus(void)
// ===========================================
{
	set_cfg(CFG_COMMON,"Language","Russian");
	set_language();
}

// ===========================================
	void CLightDlg::OnSetFre(void)
// ===========================================
{
	set_cfg(CFG_COMMON,"Language","French");
	set_language();
}

extern CStrList LangDlgList;
#include "cfg_aaka.h"
#include "cfg_dir.h"
#include "cfg_boss.h"
#include "cfg_pack.h"
#include "bbsmain.h"
#include "bbsarea.h"
#include "bbsgrp.h"
#include "bbsuser.h"
#include "cfg_ring.h"
#include "cfg_inco.h"
#include "cfgrpath.h"
#include "cfgmagic.h"
#include "cfg_othr.h"
#include "cfg_fax.h"
#include "cfg_edit.h"
#include "kludges.h"
#include "trepl.h"
#include "txttemp.h"
#include "newmail.h"
#include "cfg_mult.h"
#include "cfg_serv.h"
#include "cfg_advn.h"
#include "cfg_node.h"
#include "cfg_apno.h"
#include "cfg_icon.h"
#include "cfg_col.h"
#include "cfg_mode.h"
#include "cfg_carb.h"
#include "cfg_tras.h"
#include "logopti.h"
#include "cfg_time.h"
#include "costuser.h"
#include "receiver.h"
#include "cfg_gate.h"

// ===========================================
	void CLightDlg::OnConfig() 
// ===========================================
{
RECT	re;
#define REDRAW(ctl)	{ctl.GetClientRect(&re); ctl.InvalidateRect(&re);}
HCURSOR oc;

	SetCursor(oc=LoadCursor(NULL, IDC_WAIT));
	CSAPrefsDialog dlg;
	dlg.SetConstantText("Phoenix");
	dlg.SetValidateBeforeChangingPages(true);

	cfg_aaka dlg1;
	add_subdlg(dlg,dlg1,dlg1.GetName());
	cfg_modem dlg28;
	add_subdlg(dlg,dlg28,dlg28.GetName());
	cfg_dir	dlg2;
	add_subdlg(dlg,dlg2,dlg2.GetName());
	cfg_serv dlg22;
	add_subdlg(dlg,dlg22,dlg22.GetName());
	cfg_boss dlg3;
	add_subdlg(dlg,dlg3,dlg3.GetName());
//	cfg_tics dlg29;
//	add_subdlg(dlg,dlg29,dlg29.GetName());
	cfg_carb dlg30;
	add_subdlg(dlg,dlg30,dlg30.GetName());
	cfg_tras dlg31;
	add_subdlg(dlg,dlg31,dlg31.GetName());
	cfg_pack dlg5;
	add_subdlg(dlg,dlg5,dlg5.GetName());
	cfg_time dlg33;
	add_subdlg(dlg,dlg33,dlg33.GetName());
	cfg_edit dlg6;
	add_subdlg(dlg,dlg6,dlg6.GetName());
		kludges dlg7;
		add_subdlg(dlg,dlg7,dlg7.GetName(),&dlg6);
		trepl dlg8;
		add_subdlg(dlg,dlg8,dlg8.GetName(),&dlg6);
		txttemp dlg9;
		add_subdlg(dlg,dlg9,dlg9.GetName(),&dlg6);
		newmail dlg10;
		add_subdlg(dlg,dlg10,dlg10.GetName(),&dlg6);
	cfg_node dlg21;
	add_subdlg(dlg,dlg21,dlg21.GetName());
		cfg_apno dlg23;
		add_subdlg(dlg,dlg23,dlg23.GetName(),&dlg21);
		receiver dlg4;
		add_subdlg(dlg,dlg4,dlg4.GetName(),&dlg21);
		cfg_gate dlg34;
		add_subdlg(dlg,dlg34,dlg34.GetName(),&dlg21);
		cfg_advn dlg24;
		add_subdlg(dlg,dlg24,dlg24.GetName(),&dlg21);
	cfg_ring dlg11;
	add_subdlg(dlg,dlg11,dlg11.GetName());
		cfg_inco dlg12;
		add_subdlg(dlg,dlg12,dlg12.GetName(),&dlg11);
		cfgrpath dlg13;
		add_subdlg(dlg,dlg13,dlg13.GetName(),&dlg11);
		cfgmagic dlg14;
		add_subdlg(dlg,dlg14,dlg14.GetName(),&dlg11);
		cfg_fax dlg15;
		add_subdlg(dlg,dlg15,dlg15.GetName(),&dlg11);
	bbsmain dlg16;
	add_subdlg(dlg,dlg16,dlg16.GetName());
		bbsarea dlg17;
		add_subdlg(dlg,dlg17,dlg17.GetName(),&dlg16);
		bbsgrp dlg18;
		add_subdlg(dlg,dlg18,dlg18.GetName(),&dlg16);
		bbsuser dlg19;
		add_subdlg(dlg,dlg19,dlg19.GetName(),&dlg16);
	cfg_mult dlg20;
	add_subdlg(dlg,dlg20,dlg20.GetName());
		cfg_col dlg26;
		add_subdlg(dlg,dlg26,dlg26.GetName(),&dlg20);
		cfg_icon dlg27;
		add_subdlg(dlg,dlg27,dlg27.GetName(),&dlg20);
	logopti dlg32;
	add_subdlg(dlg,dlg32,dlg32.GetName());
	costuser dlg35;
	add_subdlg(dlg,dlg35,dlg35.GetName());
	cfg_oth dlg25;
	add_subdlg(dlg,dlg25,dlg25.GetName());

	SetCursor(oc);
	dlg.DoModal();
	REDRAW(m_autosave);
	REDRAW(m_prevarea);
	REDRAW(m_nextarea);
	REDRAW(m_find);
	REDRAW(m_newmail);
	REDRAW(m_quote);
	REDRAW(m_quotea);
	REDRAW(m_freq);
	REDRAW(m_udef1);
	REDRAW(m_udef2);
	REDRAW(m_udef3);
	REDRAW(m_udef4);
	REDRAW(m_udef5);
	REDRAW(m_exit);
//	REDRAW(m_coding);
	REDRAW(m_abortscript);
	REDRAW(m_filter);
	REDRAW(m_animail);
	REDRAW(m_anitoss);
	REDRAW(m_anipurg);
}

// ===========================================
	void add_subdlg(CSAPrefsDialog &dlg,CSAPrefsSubDlg &page,LPCSTR name,CSAPrefsSubDlg *pnt)
// ===========================================
{
CString str;
int		k;
	
	k=LangDlgList.FindString(name,str);
	if (k!=LB_ERR)	str=LangDlgList.GetString(++k);
	dlg.AddPage(page,str,pnt);
}

void CLightDlg::OnTextFrom() 
{
static _rfilter oldfilter;
char str[40];
	
	m_from.GetWindowText(str,39);
	str[39]=0;
	if (!m_text_from.IsPushed())
		grfilter=oldfilter;
	else
	{
		oldfilter=grfilter;
		if (*str) 
		{
			remove_all_filters();
			strcpy(grfilter.from,str);
			grfilter.OnCInFrom=1;
		}
	}
	RefreshSubjects();
}

void CLightDlg::OnTextTo() 
{
static _rfilter oldfilter;
char str[40];
	
	m_to.GetWindowText(str,39);
	str[39]=0;
	if (!m_text_to.IsPushed())
		grfilter=oldfilter;
	else
	{
		oldfilter=grfilter;
		if (*str) 
		{
			remove_all_filters();
			strcpy(grfilter.to,str);
			grfilter.OnCInTo=1;
		}
	}
	RefreshSubjects();
}

void CLightDlg::OnTextSubject() 
{
static _rfilter oldfilter;
char str[40];
	
	m_subject.GetWindowText(str,39);
	str[39]=0;
	if (!m_text_subject.IsPushed())
		grfilter=oldfilter;
	else
	{
		oldfilter=grfilter;
		if (*str) 
		{
			remove_all_filters();
			strcpy(grfilter.subject,str);
			grfilter.OnCInSubject=1;
		}
	}
	RefreshSubjects();
}

void CLightDlg::RefreshSubjects(void)
{
	save_current_mail_index();
	show_subjects(gustat.cur_area_handle);
    restore_current_mail_index();
	m_subjlist.SetFocus();
}

void load_cities_def(void)
{
FILE	*fp;
_tzone	TZone;
char	buf[1000],*p,*s;
int		zone,net,node,point,k;
	
	fp=fopen("cities.def","rt");	if (!fp)	return;
	while (fgets(buf,999,fp))
	{
		if (*buf==';' || *buf=='\n')	continue;
		p=strchr(buf,'\n');	if (p)	*p=0;
		p=strchr(buf,';');	if (!p)	continue;
//		p=strchr(buf,',');	if (!p)	continue;
		zone=net=node=point=0;
		k=sscanf(buf,"%d:%d/%d.%d",&zone,&net,&node,&point);
		if (k<2)
		{
			zone=0;
			k=sscanf(buf,"%d/%d.%d",&net,&node,&point);
		}
		if (k<1)	continue;
		s=strchr(p+1,';');	if (!s)	continue;
		k=sscanf(s+1,"%d;%d",&TZone.winter,&TZone.summer);
		if (k<2)	continue;
//		s=strstr(buf,"TZUTC-W");if (!s)	continue;
//		sscanf(s,"TZUTC-W %d TZUTC-S %d",&TZone.winter,&TZone.summer);
		TZone.winter/=100;
		TZone.summer/=100;
//		while (*(--s) && (*s==' ' || *s=='\t'));
//		k=min(sizeof(TZone.city),s-p);
		k=min(sizeof(TZone.city),s-p-1);
		strncpy(TZone.city,p+1,k);
		TZone.city[k]=0;
		OemToChar(TZone.city,TZone.city);
		Tzutc.SetAt(zone*1000000000000+net*100000000+node*10000+point,TZone);
	}
	fclose(fp);
}

// 	============================================================================
// display selected mail
	void CLightDlg::show_mail(_ustat *gu)
// 	============================================================================
{
_tzone tz;
mailheader &amh=gu->act_mailh;
CString tmp,line,index,cbuf,sender,fromfido,recipient,tofido,subject,mailtext,city,name1,name2;
static long last_area=-1,last_mail=-1;
int		sel,rsel,saved_index;
char	buf[400],buf1[100],msgcp[MAX_MSG+1000];
char	*msg,*p,*b,*e;

	if (!gc.disablectl && (GetKeyState(VK_CONTROL) & 0x80000000))
	{
		gLastUpdate=GetTickCount();
		return;
	}

	m_from.SetWindowText(NULL);
	m_to.SetWindowText(NULL);
	m_from_address.SetWindowText(NULL);
	m_to_address.SetWindowText(NULL);
	m_subject.SetWindowText(NULL);
	m_create.SetWindowText(NULL);
	m_receive.SetWindowText(NULL);
	m_city.SetWindowText(NULL);

	GET_SELID(IDC_SUBJLIST);
	rsel=sel;
	gustat.lastfindoffset=0;
	if (subject_list.GetCount()==0)	return;
	line=subject_list.GetString(sel);

	get_token(line,MAIL_INDEX,index);
	sscanf(index,"%d",&sel);
	gAreadef[gu->cur_area_handle].lastread=sel;	// save current mail position
	gc.sound_already_done=0;
	hide_mm_bitmap();
	gc.bmps_already_done=0;

	if (last_area == -1)
	{
		last_area=gu->cur_area_handle;
		last_mail=sel;
	}
	if (last_area != gu->cur_area_handle || last_mail != sel)
	{
		gc.display_this_mail_only--;
		if (gc.display_this_mail_only < 0)	gc.display_this_mail_only=0;
	}
	last_area=gu->cur_area_handle;
	last_mail=saved_index=sel;

	if (db_gethdr_by_index(gu->ar2han,sel,&amh) != DB_OK)	return;
	gu->already_one_valid_mail=1;

	if (!(amh.status & DB_MAIL_READ))
		db_inc_read_counter(gu->ar2han);

	if (!(amh.status & (DB_MAIL_READ | DB_DELETED | DB_MAIL_SCANNED)))
	{
		amh.status |= DB_MAIL_READ;
		if (db_sethdr_by_index(gu->ar2han,sel,&amh) != DB_OK)
			ERR_MSG2_RET("E_DBSETHDRBYINDEX",gu->ar2han);

		UCHAR tr;
		tr=AttrListBuffer[rsel] & 0x7F;	// clear usermark bit
		if (!strchr("PDAF",tr))	set_mailmark(rsel,'x');
		set_usermark(rsel,amh.status & DB_USERMARKED);
		m_attrlist.Invalidate();
	}
	else
	{
		amh.status |= DB_MAIL_READ;
		db_sethdr_by_index(gu->ar2han,sel,&amh);
	}

	if (db_get_mailtext(gu->ar2han,&amh,orgbuf,MAX_MSG+999) != DB_OK)
		ERR_MSG2_RET("DBGETMAILTEXT",gu->ar2han);

	if (gc.isMultipleSelektionMode)// UUDECODE selection mode
	{
		FILE *fp;
		fp=fopen(UUMULFILE,"at");
		if (fp)
		{
			char *x;
			int  ii;

			msgcp[0]=0;
			memcpy(msgcp,orgbuf,amh.text_length);
			msgcp[amh.text_length]='\n';
			msgcp[amh.text_length+1]=0;
			x=msgcp;
			for (ii=0;ii<amh.text_length;ii++)
			{
				if (*x=='\r')	*x='\n';
				x++;
			}
			fwrite(msgcp,amh.text_length+1,1,fp);
			fclose(fp);
			show_msg(L("S_616"),-350);	// mail proceeded
		}
	}

	filter_mailtext(orgbuf,gMailbuf);
	handle_mm_stuff(gMailbuf,2);
	if (!gc.ViewOem)	OemToChar(gMailbuf,gMailbuf);
	make_string_list(gMailbuf,actdisplist);
	m_globedit->InitStorage(actdisplist.GetCount(),actdisplist.GetCount()*80);
	actdisplist.UpdateListBox(m_globedit);

	// display to-name
	OemToChar(amh.toname,buf1);
	m_to.SetWindowText(buf1);

	get_fullname(name1,FALSE);
	get_fullname(name2,TRUE);
	if (!name1.CompareNoCase(buf1) || !name2.CompareNoCase(buf1))	system_sound("MAIL_TO_ME");

	// display from-name
	OemToChar(amh.fromname,buf1);
	m_from.SetWindowText(buf1);

	// display subject
	OemToChar(amh.subject,buf1);
	m_subject.SetWindowText(buf1);

	// display creation date
	m_create.SetWindowText(langtime(amh.datetime,buf1));

	// display reception date (only for received mails)
	if (amh.status & DB_MAIL_CREATED || !amh.recipttime)	// local mail or time is null
		m_receive.SetWindowText(NULL);
	else
	{
		unix_time_to_fido(amh.recipttime,buf);
		m_receive.SetWindowText(langtime(buf,buf1));
	}

	// display from-address
	if (amh.srczone>0)
	{
		make_address(buf,amh.srczone,amh.srcnet,amh.srcnode,amh.srcpoint);
		m_from_address.SetWindowText(buf);
		m_from_address.ShowWindow(SW_SHOWNORMAL);
	}
	else
		m_from_address.ShowWindow(SW_HIDE);// hide for local mails

	// display to-address
	if (amh.dstnet>0 && gu->ar2han==0)
	{
		make_address(buf,amh.dstzone,amh.dstnet,amh.dstnode,amh.dstpoint);
		m_to_address.SetWindowText(buf);
		m_to_address.ShowWindow(SW_SHOWNORMAL);
	}
	else
	{
		m_to_address.SetWindowText(NULL);
		m_to_address.ShowWindow(SW_HIDE);// hide for zero-net 0:0/0.0 and echoes
	}

	// display city
	m_city.SetWindowText(NULL);
	if (amh.srcnet>0)
	{
		if (gc.NdlPrefer)
			city_from_nodelist(amh.srczone,amh.srcnet,amh.srcnode,amh.srcpoint,city);
		else
		{
			get_timezone_inf(amh.srczone,amh.srcnet,amh.srcnode,amh.srcpoint,tz);
			city=tz.city;
		}
		if (city.IsEmpty())
		{
			if (gc.NdlPrefer)
			{
				get_timezone_inf(amh.srczone,amh.srcnet,amh.srcnode,amh.srcpoint,tz);
				city=tz.city;
			}
			else
				city_from_nodelist(amh.srczone,amh.srcnet,amh.srcnode,amh.srcpoint,city);
		}
		trim_all(city);
		if (!city.IsEmpty())
		{
			if (strnicmp(city,"internet",8) || strnicmp(city,"интернет",8))
			{
				msg=orgbuf;
				p=strstr(msg,"\001REPLYADDR: ");
				if (p)
				{
					p=p+12;
					msg=p;
					while (*p && *p++!='\r');
					if (p>msg)
					{
						strncpy(buf,msg,p-msg);
						OemToChar(buf,buf);
						b=strchr(buf,'<');
						e=strchr(buf,'>');
						if (b && e)
						{
							*e=0;
							strcpy(buf,b+1);
						}
						else
							buf[p-msg-1]=0;
						EMails.FindString(buf,line);
						if (line.GetLength())
							get_token(line,1,city);
						else
							city=buf;
					}
				}
			}
		}
		m_city.SetWindowText(city);
	}
	m_attrlist.Invalidate();
	// netmail && recipt-request
	if (gu->ar2han==0 && (amh.attrib & MSGRRQ) &&  
		!(amh.status & (DB_MAIL_CREATED | DB_MAIL_SCANNED)))
	{
		if (err_out("DY_GENRESP")==IDYES)
		{
			show_msg(L("S_603"),200);	// generating confirmation
			sender=amh.toname;
			recipient=amh.fromname;
			make_address(fromfido,amh.dstzone,amh.dstnet,amh.dstnode,amh.dstpoint);
			make_address(tofido,amh.srczone,amh.srcnet,amh.srcnode,amh.srcpoint);

			subject=L("S_96");	// automatic recipt confirmation
			OemToChar(amh.subject,buf);
			sender.OemToAnsi();
			recipient.OemToAnsi();
			mailtext=L("S_35",amh.datetime,buf,sender);	// recipt text

			handle_recipt_template(mailtext);
			build_routed_netmail(sender,fromfido,recipient,tofido,subject,mailtext,0,1,0);
		}
		amh.attrib &= ~MSGRRQ; // clear request flag always
		if (db_sethdr_by_index(gu->ar2han,sel,&amh) != DB_OK)
			ERR_MSG2_RET("E_DBSETHDRBYINDEX",gu->ar2han);
	}
	RefreshInfoTips();
}

// =================================================================
	void CLightDlg::RefreshAreatag(void)
// =================================================================
{
char buf[200],buf1[300];

	if (strcmp(gustat.act_area.echotag,BBSMAIL))
		strcpy(buf,gustat.act_area.description);
	else
		strcpy(buf,L("S_101")); // BBS area
	if (*buf==0)
		strcpy(buf1,gustat.act_area.echotag);
	else
		sprintf(buf1,"%s (%s)",gustat.act_area.echotag,buf);
	m_echotag.SetWindowText(buf1);
}

// ==========================================================
	BOOL city_from_nodelist(int zone,int net,int node,int point,CString &city)
// ==========================================================
{
_fidonumber number;

	city.Empty();
	if (nl_get_fido_by_number(zone,net,node,point,&number))
	{
		city=number.location;
		return TRUE;
	}
	return FALSE;
}

