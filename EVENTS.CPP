// events.cpp : implementation file
// IDD_EVENTS

#include "stdafx.h"
#include "light.h"
#include "detmail.h"
#include "selevbos.h"
#include "events.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern CStrList eventlist;
extern detmail  gMailer;
extern CString	SelectedBoss;
extern _gconfig gc;

int		no_update=0;
int		restart_was_here;
static int	ret_val;
static char DlgName[]="IDD_EVENTS";

// ===============================================================
	events::events(CWnd* pParent ) : CDialog(events::IDD, pParent)
// ===============================================================
{
	//{{AFX_DATA_INIT(events)
	//}}AFX_DATA_INIT
}

// ===============================================================
	void events::DoDataExchange(CDataExchange* pDX)
// ===============================================================
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(events)
	DDX_Control(pDX, IDC_DELETE_AFTER_SUCCESS, m_delete_after_success);
	DDX_Control(pDX, IDC_LIST, m_list);
	DDX_Control(pDX, IDC_EDIT_TO, m_edit_to);
	DDX_Control(pDX, IDC_EDIT_OUTWAIT, m_edit_outwait);
	DDX_Control(pDX, IDC_EDIT_MAXTRY, m_edit_maxtry);
	DDX_Control(pDX, IDC_EDIT_INWAIT, m_edit_inwait);
	DDX_Control(pDX, IDC_EDIT_FROM2, m_edit_eventname);
	DDX_Control(pDX, IDC_EDIT_FROM, m_edit_from);
	DDX_Control(pDX, IDC_EDIT_CAUTION3, m_edit_action_if_ok);
	DDX_Control(pDX, IDC_EDIT_CAUTION2, m_edit_action_if_failed);
	DDX_Control(pDX, IDC_EDIT_CAUTION, m_edit_caution);
	DDX_Control(pDX, IDC_EDIT_BOSS3, m_edit_boss3);
	DDX_Control(pDX, IDC_EDIT_BOSS2, m_edit_boss2);
	DDX_Control(pDX, IDC_EDIT_BOSS1, m_edit_boss1);
	DDX_Control(pDX, IDC_CHECK_WEDNESDAY, m_check_we);
	DDX_Control(pDX, IDC_CHECK_THURSDAY, m_check_th);
	DDX_Control(pDX, IDC_CHECK_THUESDAY, m_check_tu);
	DDX_Control(pDX, IDC_CHECK_THIS_ENABLED, m_event_enabled);
	DDX_Control(pDX, IDC_CHECK_SUNDAY, m_check_su);
	DDX_Control(pDX, IDC_CHECK_SATURADAY, m_check_sa);
	DDX_Control(pDX, IDC_CHECK_MONDAY, m_check_mo);
	DDX_Control(pDX, IDC_CHECK_FRIDAY, m_check_fr);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(events, CDialog)
	//{{AFX_MSG_MAP(events)
	ON_BN_CLICKED(IDC_ADD, OnAddNewEntry)
	ON_BN_CLICKED(IDC_DELETE, OnDeleteEntry)
	ON_BN_CLICKED(IDC_DISABLE, OnDisableAllEvents)
	ON_BN_CLICKED(IDC_ENABLE, OnEnableAllEvents)
	ON_BN_CLICKED(IDC_FILE_ACTION_FAILED, OnChooseFileActionFailed)
	ON_BN_CLICKED(IDC_FILE_ACTION_OK, OnChooseFileActionOk)
	ON_BN_CLICKED(IDC_SET, OnSet)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_LBN_SELCHANGE(IDC_LIST, OnSelchangeList)
	ON_BN_CLICKED(IDC_RESTART_SEQUENCE, OnRestartSequence)
	ON_BN_CLICKED(IDSTATUSREPORT, OnStatusreport)
	ON_BN_CLICKED(IDC_ASDEFAULT, OnAsdefault)
	ON_BN_CLICKED(IDC_ABORT_CURRENT_EVENT, OnAbortCurrentEvent)
	ON_BN_CLICKED(IDC_BROWSE_BOSS, OnBrowseBoss)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// ===============================================================
	BOOL events::OnInitDialog()
// ===============================================================
{
CStrList lst;
CString	 str,line;
char		 buf[300],time1[5],time2[5];
int			 err,counter=0;
int			 tabs[]={300};
int	lng[]={
			IDC_CHECK_THIS_ENABLED,
			IDC_CHECK_MONDAY,
			IDC_CHECK_THUESDAY,
			IDC_CHECK_WEDNESDAY,
			IDC_CHECK_THURSDAY,
			IDC_CHECK_FRIDAY,
			IDC_CHECK_SATURADAY,
			IDC_CHECK_SUNDAY,
			IDC_SET,
			IDC_DELETE,
			IDC_ADD,
			IDHELP,
			IDC_ENABLE,
			IDC_DISABLE,
			IDC_STATIC3,
			IDC_STATIC4,
			IDC_STATIC5,
			IDC_STATIC6,
			IDC_STATIC7,
			IDC_STATIC8,
			IDC_STATIC9,
			IDC_STATIC78,
			IDC_STATIC55,
			IDC_STATIC345,
			IDC_STATIC542,
			IDC_STATIC57,
			IDC_STATIC198,
			IDC_STATIC199,
			IDC_STATIC3453,
			IDC_RESTART_SEQUENCE,
			IDC_STATIC197,
			IDC_DELETE_AFTER_SUCCESS,
			IDC_ABORT_CURRENT_EVENT,
			IDC_STATIC19,
			IDC_ASDEFAULT,
			IDSTATUSREPORT,
			IDOK,
			IDCANCEL
			};

	CDialog::OnInitDialog();
  set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));
	TABULATE_LB(IDC_LIST);

	restart_was_here=0;
 	eventlist.SaveToFile("events.cfg");
	gc.InEventDialogBox=1;

	UPDATE_LB(eventlist,IDC_LIST);
  SET_SELID(IDC_LIST,0);

	if (!scheduled)	OnSelchangeList();
 	enable_has_changed();

// came from FILEREQUEST
	if (scheduled)
	{
		lst.LoadFromFile("eventdef.cfg");
  	if (lst.GetCount()>=1)
		{
			line=lst.GetString(0);
			get_token(line,EVNT_from,str); 
			m_edit_from.SetWindowText(str);
			get_token(line,EVNT_to,str); 
			m_edit_to.SetWindowText(str);
			get_token(line,EVNT_mo,str);
			m_check_mo.SetCheck(STOB(str));
			get_token(line,EVNT_tu,str);
			m_check_tu.SetCheck(STOB(str));
			get_token(line,EVNT_we,str);
			m_check_we.SetCheck(STOB(str));
			get_token(line,EVNT_th,str);
			m_check_th.SetCheck(STOB(str));
			get_token(line,EVNT_fr,str);
			m_check_fr.SetCheck(STOB(str));
			get_token(line,EVNT_sa,str);
			m_check_sa.SetCheck(STOB(str));
			get_token(line,EVNT_su,str);
			m_check_su.SetCheck(STOB(str));
			get_token(line,EVNT_maxtry,str);  
			m_edit_maxtry.SetWindowText(str);
			get_token(line,EVNT_inwait,str);  
			m_edit_inwait.SetWindowText(str);
			get_token(line,EVNT_outwait,str); 
			m_edit_outwait.SetWindowText(str);
			get_token(line,EVNT_if_ok,str);  
			m_edit_action_if_ok.SetWindowText(str);
			get_token(line,EVNT_if_failed,str); 
			m_edit_action_if_failed.SetWindowText(str);
		}
		else
		{
			if (expand_address(fido_if_scheduled,str,line,1,1,1,err))
			{
				get_call_time(line,time1,time2);
				m_edit_from.SetWindowText(time1);
				m_edit_to.SetWindowText(time2);
			}
			if (*time1==0 || *time2==0)
			{
				m_edit_from.SetWindowText("1:00");
				m_edit_to.SetWindowText("7:00");
			}
			m_check_mo.SetCheck(1);
			m_check_tu.SetCheck(1);
			m_check_we.SetCheck(1);
			m_check_th.SetCheck(1);
			m_check_fr.SetCheck(1);
			m_check_sa.SetCheck(1);
			m_check_su.SetCheck(1);
			m_edit_maxtry.SetWindowText("10");
			m_edit_inwait.SetWindowText("10");
			m_edit_outwait.SetWindowText("30");
		}

		for (int i=0;i<eventlist.GetCount();i++)
		{
		 	line=eventlist.GetString(i);
			get_token(line,EVNT_event,str);
			if (!strncmp(str,"FREQ",11))	counter++;
 		}

 		sprintf(buf,"FREQ_%d",counter);
		m_edit_eventname.SetWindowText(buf);
		m_edit_boss1.SetWindowText(fido_if_scheduled);
	  m_event_enabled.SetCheck(1);
		m_delete_after_success.SetCheck(1);

		OnAddNewEntry();
		if (eventlist.GetCount() > 0)	SET_SELID(IDC_LIST,eventlist.GetCount()-1);
	}
	return TRUE;
}

// ===============================================================
	void events::OnAddNewEntry()
// ===============================================================
{
CString str;
char	buf[100];

	m_edit_eventname.GetWindowText(buf,99);
	trim_all(buf); 
	if (*buf==0)	ERR_MSG2_RET("E_NAMEREQ","EVENTNAME");
	set_tabbed_string(str,EVNT_event,buf);

	m_edit_from.GetWindowText(buf,99);
	trim_all(buf); 
	if (*buf==0)	ERR_MSG2_RET("E_NAMEREQ","FROM");
	set_tabbed_string(str,EVNT_from,buf);

	m_edit_to.GetWindowText(buf,99);
	trim_all(buf); 
	if (*buf==0)	ERR_MSG2_RET("E_NAMEREQ","TO");
	set_tabbed_string(str,EVNT_to,buf);

	m_edit_boss1.GetWindowText(buf,99);
	trim_all(buf); 
	if (*buf==0)	ERR_MSG2_RET("E_NAMEREQ","BOSS");
	set_tabbed_string(str,EVNT_boss1,buf);

	set_tabbed_string(str,EVNT_enabled,BTOS(m_event_enabled.GetCheck()));
	set_tabbed_string(str,EVNT_mo,BTOS(m_check_mo.GetCheck()));
	set_tabbed_string(str,EVNT_tu,BTOS(m_check_tu.GetCheck()));
	set_tabbed_string(str,EVNT_we,BTOS(m_check_we.GetCheck()));
	set_tabbed_string(str,EVNT_th,BTOS(m_check_th.GetCheck()));
	set_tabbed_string(str,EVNT_fr,BTOS(m_check_fr.GetCheck()));
	set_tabbed_string(str,EVNT_sa,BTOS(m_check_sa.GetCheck()));
	set_tabbed_string(str,EVNT_su,BTOS(m_check_su.GetCheck()));
	set_tabbed_string(str,EVNT_delete,BTOS(m_delete_after_success.GetCheck()));

  m_edit_boss2.GetWindowText(buf,99);       
	trim_all(buf); 
	set_tabbed_string(str,EVNT_boss2,buf);

  m_edit_boss3.GetWindowText(buf,99);       
	trim_all(buf); 
	set_tabbed_string(str,EVNT_boss3,buf);

  m_edit_maxtry.GetWindowText(buf,99);      
	trim_all(buf); 
	set_tabbed_string(str,EVNT_maxtry,buf);

  m_edit_caution.GetWindowText(buf,99);     
	trim_all(buf); 
	set_tabbed_string(str,EVNT_caution,buf);

  m_edit_inwait.GetWindowText(buf,99);      
	trim_all(buf); 
	set_tabbed_string(str,EVNT_inwait,buf);

  m_edit_outwait.GetWindowText(buf,99);     
	trim_all(buf); 
	set_tabbed_string(str,EVNT_outwait,buf);

  m_edit_action_if_ok.GetWindowText(buf,99); 
	trim_all(buf);     
	set_tabbed_string(str,EVNT_if_ok,buf);

  m_edit_action_if_failed.GetWindowText(buf,99); 
	trim_all(buf); 
	set_tabbed_string(str,EVNT_if_failed,buf);

	eventlist.AddTail(str);
	UPDATE_LB(eventlist,IDC_LIST);
	((CButton *)GetDlgItem(IDOK))->SetFocus();
}

// ===============================================================
	void events::OnDeleteEntry()
// ===============================================================
// Loschen eines Eintrags aus der List ...
{
int sel;

	sel=((CListBox *)GetDlgItem(IDC_LIST))->GetCurSel();
	if (sel==LB_ERR)
	{
		ret_val=0;
		return;
	}
	eventlist.Remove(sel);
	if (!no_update)
	{
		UPDATE_LB(eventlist,IDC_LIST);
		SET_SELID(IDC_LIST,0);
		OnSelchangeList();
	}
	ret_val=1;
}

// ===============================================================
	void events::OnDisableAllEvents()
// ===============================================================
{
	event_add_listbox(L("S_167"));	// disabling events
	gc.eventsenabled=0;
	enable_has_changed();
}

// ===============================================================
	void events::OnEnableAllEvents()
// ===============================================================
{
	gc.eventsenabled=1;
	show_events_status();
	enable_has_changed();
}

// ===============================================================
	void events::OnChooseFileActionFailed()
// ===============================================================
{
CString	str;
int		ret;

	str.LoadString(IDS_SCRIPTFILT);
	CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY,str);
	ret=dlg.DoModal();
	restore_base_path();
	if (ret!=IDOK)	return;
	m_edit_action_if_failed.SetWindowText(dlg.GetPathName());
}

// ===============================================================
	void events::OnChooseFileActionOk()
// ===============================================================
{
CString	str;
int		ret;

	str.LoadString(IDS_SCRIPTFILT);
	CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY,str);
	ret=dlg.DoModal();
	restore_base_path();
	if (ret!=IDOK)	return;
	m_edit_action_if_ok.SetWindowText(dlg.GetPathName());
}

// ===============================================================
	void events::OnSet()
// ===============================================================
{
	no_update=1;
	OnDeleteEntry();
	if (ret_val)
		OnAddNewEntry();
	no_update=0;
  SET_SELID(IDC_LIST,((CListBox *)GetDlgItem(IDC_LIST))->GetCount()-1);
	OnSelchangeList();
}

// ===============================================================
	void events::OnCancel()
// ===============================================================
{
	if (restart_was_here && err_out("DY_MSFERS")!=IDYES)	return;
	eventlist.LoadFromFile("events.cfg");
	gc.InEventDialogBox=0;
	CDialog::OnCancel();
}

// ===============================================================
	void events::OnOK()
// ===============================================================
{
 	eventlist.SaveToFile("events.cfg");
	gc.InEventDialogBox=0;
	CDialog::OnOK();
}

// ===============================================================
	void events::OnSelchangeList()
// ===============================================================
{
CString all;
int		sel;

	GET_SELID(IDC_LIST);
	all=eventlist.GetString(sel);
	set_wnd_text(all,EVNT_event,m_edit_eventname);
	set_btn_check(all,EVNT_enabled,m_event_enabled);
	set_wnd_text(all,EVNT_from,m_edit_from);
	set_wnd_text(all,EVNT_to,m_edit_to);
	set_btn_check(all,EVNT_mo,m_check_mo);
	set_btn_check(all,EVNT_tu,m_check_tu);
	set_btn_check(all,EVNT_we,m_check_we);
	set_btn_check(all,EVNT_th,m_check_th);
	set_btn_check(all,EVNT_fr,m_check_fr);
	set_btn_check(all,EVNT_sa,m_check_sa);
	set_btn_check(all,EVNT_su,m_check_su);
	set_btn_check(all,EVNT_delete,m_delete_after_success);
	set_wnd_text(all,EVNT_boss1,m_edit_boss1);
	set_wnd_text(all,EVNT_boss2,m_edit_boss2);
	set_wnd_text(all,EVNT_boss3,m_edit_boss3);
	set_wnd_text(all,EVNT_maxtry,m_edit_maxtry);
	set_wnd_text(all,EVNT_caution,m_edit_caution);
	set_wnd_text(all,EVNT_inwait,m_edit_inwait);
	set_wnd_text(all,EVNT_outwait,m_edit_outwait);
	set_wnd_text(all,EVNT_if_ok,m_edit_action_if_ok);
	set_wnd_text(all,EVNT_if_failed,m_edit_action_if_failed);
}

// ===============================================================
	void events::enable_has_changed	(void)
// ===============================================================
// enabling buttons in dialog
{
	if (gc.eventsenabled)
	{
		ENABLEID(IDC_DISABLE);
		DISABLEID(IDC_ENABLE);
	}
	else
	{
		DISABLEID(IDC_DISABLE);
		ENABLEID(IDC_ENABLE);
	}
	gMailer.SetButtonsState();
}

// ===============================================================
	void events::OnRestartSequence()
// ===============================================================
{
	restart_was_here=1;
	gc.eventsenabled=0;
	gc.restartevents=1;
	show_msg(L("S_209"));	// resetting events
	restart_events();
	gc.restartevents=0;
	enable_has_changed();
	gMailer.SetButtonsState();
}

// ===============================================================
	void events::OnStatusreport()
// ===============================================================
{
	show_events_status();
}

// ===============================================================
	void events::OnAsdefault()
// ===============================================================
// Hier werden die Defaults fuer einen SCHEDULED Request gespeichert ...
{
CStrList	lst;
CString		all,test;

	if (err_out("DY_SAVDSFR")!=IDYES)
	 	return;

	// From
    m_edit_from.GetWindowText(test);
	trim_all(test);
	set_tabbed_string(all,EVNT_from,PS test);

	// To
    m_edit_to.GetWindowText(test);
	trim_all(test);
	set_tabbed_string(all,EVNT_to,PS test);

	set_tabbed_string(all,EVNT_mo,BTOS(m_check_mo.GetCheck()));
	set_tabbed_string(all,EVNT_tu,BTOS(m_check_tu.GetCheck()));
	set_tabbed_string(all,EVNT_we,BTOS(m_check_we.GetCheck()));
	set_tabbed_string(all,EVNT_th,BTOS(m_check_th.GetCheck()));
	set_tabbed_string(all,EVNT_fr,BTOS(m_check_fr.GetCheck()));
	set_tabbed_string(all,EVNT_sa,BTOS(m_check_sa.GetCheck()));
	set_tabbed_string(all,EVNT_su,BTOS(m_check_su.GetCheck()));

	// Cautions
    m_edit_maxtry.GetWindowText(test);
	trim_all(test);
	set_tabbed_string(all,EVNT_maxtry,PS test);

	// Delays
    m_edit_inwait.GetWindowText(test);
	trim_all(test);
	set_tabbed_string(all,EVNT_inwait, PS test);
    m_edit_outwait.GetWindowText(test);
	trim_all(test);
	set_tabbed_string(all,EVNT_outwait,PS test);

	// Actions
    m_edit_action_if_ok.GetWindowText(test);
	trim_all(test);
	set_tabbed_string(all,EVNT_if_ok, PS test);
    m_edit_action_if_failed.GetWindowText(test);
	trim_all(test);
	set_tabbed_string(all,EVNT_if_failed,PS test);

	lst.RemoveAll();
	lst.AddTail(all);
	lst.SaveToFile("eventdef.cfg");

	show_msg("Saved","Сохранено",500);
}

// ===============================================================
	void events::OnAbortCurrentEvent()
// ===============================================================
{
	OnAbortCurrentEventWork();
}

// ===============================================================
	void OnAbortCurrentEventWork(void)
// ===============================================================
{
	gc.abort_current_event=1;
	show_msg("Aborting event","Событие прервано",300);
}

// ===============================================================
	void events::OnBrowseBoss()
// ===============================================================
{
CString str;

	selevbos dlg;
	dlg.DoModal();

	if (SelectedBoss.GetLength())
	{
		get_token(SelectedBoss,0,str);
		m_edit_boss1.SetWindowText(str);
	}
}

// ===============================================================
	void events::set_wnd_text(CString &str,int id,CWnd &wnd)
// ===============================================================
{
CString tmp;

	get_token(str,id,tmp);
	wnd.SetWindowText(tmp);
}

// ===============================================================
	void events::set_btn_check(CString &str,int id,CButton &btn)
// ===============================================================
{
CString tmp;

	get_token(str,id,tmp);
	btn.SetCheck(STOB(tmp));
}

// ===============================================================
	void events::OnHelp()
// ===============================================================
{
	WinHelp(VHELP_EVENTS);
}

BOOL events::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return TRUE;
}
