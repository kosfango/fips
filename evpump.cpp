#include "stdafx.h"
#include <io.h>
#include "lightdlg.h"
#include "cfido.h"
#include "detmail.h"
#include "gcomm.h"

extern CStrList		eventlist;
extern CStrList		isdnlist;
extern CStrList		alt_phone;
extern CLightDlg	*gpMain;
extern detmail 		gMailer;
extern _gconfig		gc;

static char ddbuf[300];
static int lastslicestatus=0;

static CString	entry;
static CString	fidoadr;
extern CString	gLastCalled;
extern _gcomm	gcomm;

void	build_event_infomail(evstruct &evs, int ok);
void	delete_current_event(void);
void	set_event_status		(int index,int status);
int		parse_event					(LPCSTR entry,evstruct &evs);
int		execute_event				(LPCSTR addr,evstruct &evs);
int		is_event_permitted	(evstruct &evs);
void	reset_dialindex			(int index);
void	reset_trycounter		(int index);
void	increment_trycounter(int index);
void	set_trycounter			(int index);	// set flag to prevent reply "FLAG" event
void	increment_dialindex	(int index,evstruct &evs);

// =================================
// is called from EventWorkerThread and execute event number 'index'
	int event_pump(int index)
// =================================
{
evstruct evs;
int 	 nn,ret,ret0,ret1,allowed;

	if (index >= eventlist.GetCount())	return 0;
	entry=eventlist.GetString(index);
	parse_event(entry,evs);

	switch (evs.status)
	{
		case	EV_TODO:
			allowed=is_event_permitted(evs);
			if (allowed > 0)	// event enabled
			{
				lastslicestatus=1;
				fidoadr=evs.boss1;

// INWAIT STUFF
				if (evs.inwait)
				{
					event_add_listbox(L("S_221",evs.event,evs.inwait));	// pause
					for (int n=0;n<(evs.inwait*10);n++)
					{
						if (gc.ExitEventThread || gc.restartevents)	goto endit;
						Sleep(100);
					}
					event_add_listbox(L("S_224",evs.event));	// waiting finished
				}
				if (gc.mailer.running)
				{
					event_add_listbox(L("S_220",evs.event));	// modem busy
					goto endit;
				}

				gc.mailer.running=1;
				handle_mailer_ctls(TRUE);
				ret=execute_event(fidoadr,evs);	// executing current event
				gc.mailer.running=0;
				handle_mailer_ctls(FALSE);

				if (gc.abort_current_event)	// Abort button pressed
   			{
					gc.abort_current_event=0;
					event_add_listbox(L("S_210",evs.event));	// aborted
					set_event_status(index,EV_FAILED);
					goto endit;
				}

// OUTWAIT STUFF
				if (evs.outwait)
				{
					event_add_listbox(L("S_221",evs.event,evs.outwait));	// pause
					for (nn=0;nn<(evs.outwait*10);nn++)
					{
						if (gc.ExitEventThread || gc.restartevents)	goto endit;
						Sleep(100);
					}
					event_add_listbox(L("S_224",evs.event));	// end waiting
				}
// analyse result of event execution
				switch(ret)
				{
					case MODEM_NOTFOUND:
						event_add_listbox(L("S_211",evs.event));	// address not found
						set_event_status(index,EV_FAILED);
						break;
					case MODEM_FAIL:
					case MODEM_CANNOT_INIT:
					default:
						increment_trycounter(index);
						increment_dialindex(index,evs);
						event_add_listbox(L("S_213",evs.event,fidoadr));	// event failed
						break;
					case MODEM_OK:   	// event finished OK
						if (!stricmp(fidoadr,"SCRIPT"))
						{
							write_lastcall_idx();
							event_add_listbox(L("S_215",evs.event));	// script finished
							if (*evs.if_failed)
							{
								event_add_listbox(L("S_219",evs.event,evs.if_failed));	// start F-script
								gpMain->execute_script(evs.if_failed);
							}
							if (*evs.if_ok)
							{
								event_add_listbox(L("S_218",evs.event,evs.if_ok));	// start O-script
								gpMain->execute_script(evs.if_ok);
 							}
							set_event_status(index,EV_OK);
						}
						else if (!stricmp(fidoadr,"FLAG"))
						{
							set_trycounter(index);	// prevent repeat this event in current period
							write_lastcall_idx();
							if (evs.maxtry==0)
								set_event_status(index,EV_OK);
							else
								set_event_status(index,EV_TODO);
						}
						else
						{
							build_event_infomail(evs,1);
							set_event_status(index,EV_OK);
							check_inbound();
						}
						if (evs.del_after)	delete_current_event();
						break;
				}
			}
			else if (allowed==0)   // max tries is reached
			{
				if (!evs.enabled)	break;
				if (is_event_permitted(evs)==-1)
				{
					set_event_status (index,EV_TODO);
					reset_trycounter(index);
					reset_dialindex(index);
				}

				lastslicestatus=0;
				if (evs.acttry >= evs.maxtry)
				{
					set_event_status (index,EV_FAILED);
					if (strlen(evs.if_failed)>1)
					{
						event_add_listbox(L("S_218",evs.event,evs.if_failed));	// executing script
						gpMain->execute_script(evs.if_failed);
					}
					build_event_infomail(evs,0);
					break;
				}
			}
			else if (allowed == -1)	// out of period
			{
				lastslicestatus=0;
				if (abs((int)difftime(time(NULL),evs.lasttouch)) <60)
				{
					if (evs.lasttouchstatus==1)
					{
						set_event_status (index,EV_FAILED);
						if (*evs.if_failed)
						{
							event_add_listbox(L("S_170",evs.event,evs.if_failed));
							gpMain->execute_script(evs.if_failed);
						}
						build_event_infomail(evs,0);
					}
					break;
				}
			}
			else if (allowed==-2)	// && is_event_permitted(evs)==-1)
			{
				set_event_status(index,EV_TODO);
				reset_trycounter(index);
				reset_dialindex(index);
			}
			else if (allowed==-3)
				reset_trycounter(index);

			break;
		case	EV_FAILED:
		case	EV_OK:
			// Es gibt zwei Zustaende die aus dem OK in den TODO ueberfuehren:
			// 1: wir sind OUT-OF-TIME
			// 2: Diff zum Lasttouch ist groesser als Eventdauer
			ret0=is_event_permitted(evs);
			ret1=diff_to_lasttouch_to_big(evs);

			if (ret0==-1 || ret1)
			{
				set_event_status (index,EV_TODO);
				reset_trycounter    (index);
				reset_dialindex (index);
			}
			break;
	 	default:
			ASSERT(0);	// illegal Event state
	}
endit:
	if (!gc.no_event_update)	touch_actual_time_to_eventstring(index);
	gc.no_event_update=0;
	return 1;
}

// =======================================================================
// parses event string to event structure
	int parse_event(LPCSTR entry,evstruct &evs)
// =======================================================================
{
CString help;

	memset(&evs,0,sizeof(evs));
	get_token(entry,EVNT_event,evs.event);
	get_token(entry,EVNT_from,evs.from);
	get_token(entry,EVNT_to,evs.to);
	get_token(entry,EVNT_boss1,evs.boss1);
	get_token(entry,EVNT_boss2,evs.boss2);
	get_token(entry,EVNT_boss3,evs.boss3);
	get_token(entry,EVNT_enabled,help); if (help=="1") evs.enabled=1;
	get_token(entry,EVNT_mo,help); sscanf(help,"%d",&evs.check_mo);
	get_token(entry,EVNT_tu,help); sscanf(help,"%d",&evs.check_tu);
	get_token(entry,EVNT_we,help); sscanf(help,"%d",&evs.check_we);
	get_token(entry,EVNT_th,help); sscanf(help,"%d",&evs.check_th);
	get_token(entry,EVNT_fr,help); sscanf(help,"%d",&evs.check_fr);
	get_token(entry,EVNT_sa,help); sscanf(help,"%d",&evs.check_sa);
	get_token(entry,EVNT_su,help); sscanf(help,"%d",&evs.check_su);
	get_token(entry,EVNT_delete,help); sscanf(help,"%d",&evs.del_after);
	get_token(entry,EVNT_inwait,help); sscanf(help,"%d",&evs.inwait);
	get_token(entry,EVNT_outwait,help); sscanf(help,"%d",&evs.outwait);
	get_token(entry,EVNT_maxtry,help); sscanf(help,"%d",&evs.maxtry);
	get_token(entry,IDTOacttry,help);	  sscanf(help,"%d",&evs.acttry);
	get_token(entry,IDTOdialindex,help);  sscanf(help,"%d",&evs.dialindex);
	get_token(entry,EVNT_caution,evs.condition);
	get_token(entry,EVNT_if_ok,evs.if_ok);
	get_token(entry,EVNT_if_failed,evs.if_failed);
	get_token(entry,IDTOlasttouch,help);	sscanf(help,"%lx",&evs.lasttouch);
	get_token(entry,IDTOlaststatus,help);	sscanf(help,"%d",&evs.lasttouchstatus);
	get_token(entry,IDTOstatus,help);	    sscanf(help,"%d",&evs.status);
	return 1;
}

// struct evstruct
// {
// 	int  enabled;
// 	char event			[100];
// 	char boss1 				[30];
// 	char boss2 				[30];
// 	char boss3 				[30];
// 	char from 				[30];
// 	char to 				[30];
// 	char condition 			[100];
// 	char if_ok 		[300];
// 	char if_failed	[300];
// 	int	 check_mo;
// 	int	 check_tu;
// 	int	 check_we;
// 	int	 check_th;
// 	int	 check_fr;
// 	int	 check_sa;
// 	int	 check_su;
// 	int	 maxtry;
// 	int	 inwait;
// 	int	 outwait;
// 	long lasttouch;
// 	int  status;
// };

// ==================================================================================
// checks if event is permitted now
// returns 0 if max tries exhausted, -1 if out of allowed period, -2
//				 >0 if event is enabled
	int is_event_permitted(evstruct &evs)
// ==================================================================================
{
CString line,name,status;
char 	cau_name[300],cau_bed[300];
time_t	unixtime;
tm		*tx;
int 	i,numstat,start_hr,start_min,stop_hr,stop_min,beg_time,end_time,cur_time,diff;
int   in=0,found=0;

	if (!evs.enabled)	return 0;
	if (stricmp(evs.boss1,"FLAG") && evs.maxtry && evs.acttry >= evs.maxtry)	return 0;
	unixtime=time(NULL);
	tx=localtime(&unixtime);
	switch (tx->tm_wday)
	{
		   case 0:  if (!evs.check_su) return -1;
		   case 1:  if (!evs.check_mo) return -1;
		   case 2:  if (!evs.check_tu) return -1;
		   case 3:  if (!evs.check_we) return -1;
		   case 4:  if (!evs.check_th) return -1;
		   case 5:  if (!evs.check_fr) return -1;
		   case 6:  if (!evs.check_sa) return -1;
	}
	sscanf(evs.from,"%d:%d",&start_hr,&start_min);
	sscanf(evs.to,	"%d:%d",&stop_hr, &stop_min);
	cur_time=tx->tm_hour*60+tx->tm_min;
	beg_time=start_hr*60+start_min;
	end_time=stop_hr*60+stop_min;
// check allowed period
	if (beg_time <= end_time)
	{
		if (beg_time > cur_time || cur_time >= end_time)	return -1;	// not in normal period
	}
	else
	{
		if (cur_time > end_time && beg_time > cur_time)		return -1;	// not in revert period
	}
// check repeated interval
	if (!stricmp(evs.boss1,"FLAG"))
	{
		diff=cur_time-beg_time;
		if (diff < 0) diff+=24*60;
		if (evs.maxtry > 0)	// execute repeatedly?
		{
			if (diff % evs.maxtry != 0)	return -3;	// not in period, return value to reset acttry
			if (evs.acttry==1) return -4;	// skip repeated calls in one period
		}
	}

// checking condition in format EVENTNAME STATUS, for example MYPOLL OK
	if (*evs.condition > 0)
	{
		if (sscanf(evs.condition,"%s %s",cau_name,cau_bed) != 2)	err_out("W_WREVTCAUTION",evs.condition);
		for (i=0;i<eventlist.GetCount();i++)
		{
			line=eventlist.GetString(i);
			get_token(line,EVNT_event,name);
			if (!stricmp(name,cau_name))
			{
			  found=1;
				numstat=0;
				get_token(line,IDTOstatus,status);
				sscanf(status,"%d",&numstat);
				switch (numstat)
				{
				 	case -1:	if (stricmp(cau_bed,"FAILED")) return -2;
				 	case 0:		if (stricmp(cau_bed,"TODO"))   return -2;
				 	case 1:		if (stricmp(cau_bed,"OK"))	   return -2;
					default:	ASSERT(0);	 // invalid status
				}
			}
		}
		if (!found)	ERR_MSG2_RET0("W_WREVTNNF",cau_name)
	}
	return 1;		// OK, event permitted
}

// ==================================================================================
	int touch_actual_time_to_eventstring (int index)
// ==================================================================================
// Diese Function 'touched' den aktuellen Eintrag im 'i-ten' Index ..
{
CString	line;
char	buf[300];

	line=eventlist.GetString(index);
	sprintf(buf,"%lx",time(NULL));
	set_tabbed_string(line,IDTOlasttouch,buf);
	sprintf(buf,"%d",lastslicestatus);
	set_tabbed_string(line,IDTOlaststatus,buf);
	eventlist.Replace(index,line);
	return 1;
}

// ==================================================================================
// increments dial index
	void increment_dialindex(int index,evstruct &evs)
// ==================================================================================
{
CString	line;
char	buf[300];
int		cnt=0;

	if (strlen(evs.boss2)<2)	return;
	switch (evs.dialindex)
	{
		 case	0:
					cnt=1;
		 			break;
		 case	1:
					if (strlen(evs.boss3)>1)
						cnt=2;
					else
						cnt=0;
		 			break;
		 case	2:
					cnt=0;
		 			break;
		 default:
		 			ASSERT(0);			// Illegal dialindex ...
					break;
	}
	line=eventlist.GetString(index);
	sprintf(buf,"%d",cnt);
	set_tabbed_string(line,IDTOdialindex,buf);
	eventlist.Replace(index,line);
}

// ==================================================================================
// increments try counter
	void increment_trycounter(int index)
// ==================================================================================
{
CString	line,str;
int 	acttry;

	line=eventlist.GetString(index);
	get_token(line,IDTOacttry,str);
	acttry=0;
	sscanf(str,"%d",&acttry);
	str.Format("%d",++acttry);
	set_tabbed_string(line,IDTOacttry,str);
	eventlist.Replace(index,line);
}

// ==================================================================================
// sets flag to prevent repeat "FLAG" event in one period
	void set_trycounter(int index)
// ==================================================================================
{
CString	line;

	line=eventlist.GetString(index);
	set_tabbed_string(line,IDTOacttry,"1");
	eventlist.Replace(index,line);
}

// ==================================================================================
// resets try counter for event
	void reset_trycounter(int index)
// ==================================================================================
{
CString	line;

	line=eventlist.GetString(index);
	set_tabbed_string(line,IDTOacttry,"0");
	eventlist.Replace(index,line);
}

// ==================================================================================
// resets index for dialing
	void reset_dialindex(int index)
// ==================================================================================
{
CString	line;

	line=eventlist.GetString(index);
	set_tabbed_string(line,IDTOdialindex,"0");
	eventlist.Replace(index,line);
}

// ==================================================================================
	void set_event_status(int index,int status)
// ==================================================================================
{
CString	line,str;
char	buf[10];

	line=eventlist.GetString(index);
	get_token(line,EVNT_event,str);
	event_add_listbox(L("S_208",str,status==0 ? "TODO" : (status==1 ? "OK" : "FAILED")));	// event status
	sprintf(buf,"%d",status);
	set_tabbed_string(line,IDTOstatus,buf);
	eventlist.Replace(index,line);
}

// ==================================================================================
// execute one FIDO Call for required Fido Address
	int execute_event(LPCSTR fidoadr,evstruct &evs)
// ==================================================================================
// Returns result from outgoing_emsi_session():
// MODEM_NOTFOUND:  Address not found
// MODEM_FAIL:		Session failed
// MODEM_OK:		Session finished OK
{
_fidonumber nlnumber,result;
CString fido,phon,pass,aka,asdir,line,flaghelp,str;
char	outbdir[MAX_PATH],buf[MAX_BOSSLEN],tmp[300],akaret[500];
int		zone1,net1,node1,point1,zone2,net2,node2,point2;
int		ret,i,found,nopoll=0,nohydra=0,nozmod=0;

// script event
	if (!stricmp(fidoadr,"SCRIPT"))
	{
		if (*(evs.if_failed))
		{
			Sleep(500);
			event_add_listbox(L("S_218",evs.event,evs.if_failed));	// executing script
			gpMain->execute_script(evs.if_failed);
		}
		if (*(evs.if_ok))
		{
			Sleep(500);
			event_add_listbox(L("S_218",evs.event,evs.if_ok));	// executing script
			gpMain->execute_script(evs.if_ok);
		}
		event_add_listbox(L("S_215",evs.event));	// script finished
		return MODEM_OK;
	}
// flag event
	if (!stricmp(fidoadr,"FLAG") && *(evs.boss2))
	{
		time_t unixtime=time(NULL);
		tm *tx=localtime(&unixtime);

		event_add_listbox(L("S_630",evs.event,evs.boss2));		// flag checking
		if (!access(evs.boss2,0))
		{
			event_add_listbox(L("S_631",evs.event,evs.boss2));	// flag found
			if (*(evs.if_ok))
			{
				event_add_listbox(L("S_218",evs.event,evs.if_ok));// executing OK script
				gpMain->execute_script(evs.if_ok);
			}
		}
		else
		{
			event_add_listbox(L("S_632",evs.event,evs.boss2));	// flag don't found
			if (*(evs.if_failed))
			{
				event_add_listbox(L("S_218",evs.event,evs.if_failed));	// executing FAILED script
				gpMain->execute_script(evs.if_failed);
			}
		}
		return MODEM_OK;
	}
// search among bosses
	i=0;
	found=0;
	while (db_get_boss_by_index(i++,buf))
	{
		get_token(buf,BOSSADDR,fido);	  			// boss address
		if (!stricmp(fido,fidoadr))
		{
			get_token(buf,BOSSPHON,phon);  			// phone
			get_token(buf,SESSPASW,pass);  			// password
			get_token(buf,MAINAKA,aka);					// AKA for this Boss
			nopoll=get_token_int(buf,NOPOLL);		// no poll-packet
			nohydra=get_token_int(buf,NOHYDRA);	// no hydra
			nozmod=get_token_int(buf,NOZMODEM);	// no zmodem
		 	found=1;
			break;
		}
	}
// if not found, search in nodlist
	if (!found)
	{
		parse_address(fidoadr,&zone2,&net2,&node2,&point2);
		if (nl_get_fido_by_number(zone2,net2,node2,point2,&result))
		{
			strcpy(tmp,result.phone);
			translate_phone_number(tmp);
			phon=tmp;
			pass.Empty();
			*akaret=0; 
			db_build_akas(akaret,TRUE);	// build aka-string for all possible aka's
			aka=akaret;
			found=1;
		}
	}

	if (!found)
	{
		event_add_listbox(L("S_223",evs.event,fido));	// system not found
		return MODEM_NOTFOUND;
	}

	switch (evs.dialindex)
	{
		default:
		case 0:
		 	break;				// default number
		case 1:
			phon=evs.boss2;		// 1 alternative number
		 	break;
		case 2:
			phon=evs.boss3;		// 2 alternative number
		 	break;
	}

	CFido fd(fidoadr);
	asdir=fd.GetAsDir(0);
	make_path(outbdir,gc.OutboundPath,asdir);
	replace_phone_number(phon);
//ISDN
	if (isdnlist.defaultindexnew)
	{
		parse_address(fidoadr,&zone1,&net1,&node1,&point1);
		if (nl_get_fido_by_number(zone1,net1,node1,point1,&nlnumber))
		{
			for (i=0;i<isdnlist.GetCount();i++)
			{
				str=isdnlist.GetString(i);  
				str.MakeUpper();
				flaghelp=nlnumber.flags;	  
				flaghelp.MakeUpper();
				if (strstr(flaghelp,str))
				{
					if (*phon != '$')	phon="$"+phon;
					break;
				}
			}
		}
	}

	gc.ineventaction=1;
	gpMain->m_animail.Play(0,-1,-1);
	for (i=0;i<10;i++)	// pause 1 sec
	{
		if (gc.ExitEventThread || gc.restartevents)
		{
			gpMain->m_animail.Stop();
			gpMain->m_animail.Seek(0);
			return MODEM_OK;
		}
		Sleep(100);
	}

	system_sound("EVENT_DIALING");
	event_add_listbox(L("S_212",evs.event,phon));	// calling phone
	gLastCalled=fidoadr;
	alt_phone.RemoveAll();
	ret=outgoing_emsi_session(phon,pass,outbdir,aka,nopoll,nohydra,nozmod);
	gcomm.ConnectedOn1Modem=gcomm.ConnectedOn2Modem=0;
	gcomm.DialingViaSecondModem=0;
	gpMain->m_animail.Stop();
	gpMain->m_animail.Seek(0);
	gc.ineventaction=0;
	return ret;
}

// ==================================================================================
	int diff_to_lasttouch_to_big (struct evstruct &evs)
// ==================================================================================
{
time_t starttime,stoptime,nowtime;
int    start_hr,start_min,stop_hr,stop_min,evperiod;
tm		 txstart,txstop,*tx;

	sscanf(evs.from,"%d:%d",&start_hr,&start_min);
	sscanf(evs.to,	"%d:%d",&stop_hr, &stop_min);

	nowtime=time(NULL);
	tx=localtime(&nowtime);

	memcpy(&txstart,tx,sizeof(tm));
	memcpy(&txstop,tx,sizeof(tm));

	txstart.tm_hour=start_hr;
	txstart.tm_min =start_min;
	starttime=mktime(&txstart);

	txstop.tm_hour=stop_hr;
	txstop.tm_min =stop_min;
	stoptime=mktime(&txstop);

	evperiod=abs((int)difftime(starttime,stoptime));

	return ((nowtime-evs.lasttouch) > evperiod);
}

// ==================================================================================
// sets all events in TODO state and clears try counter
	void restart_events(void)
// ==================================================================================
{
CString line,str;

	event_add_listbox(L("S_384"));	// events restart
	for (int i=0;i<eventlist.GetCount();i++)
	{
		line=eventlist.GetString(i);
		get_token(line,EVNT_enabled,str);	
		if (str=="1")
		{
			set_event_status(i,EV_TODO);
			reset_trycounter(i);
			reset_dialindex(i);
		}
	}
}

// ==================================================================================
// output for EventPump
	int	event_add_listbox(LPCSTR text)
// ==================================================================================
{
char	buf[300],buf2[300];
int		cnt;

	if (gc.ExitEventThread)	return 0;
	sprintf(buf,"%s: %s",_strtime(buf2),text);
	gMailer.m_eventlog.AddString(buf);
	gMailer.m_eventlog.SetRedraw(0);
	for (int i=0;i<gMailer.m_eventlog.GetCount()-MAX_EVENTLOG-20;i++)
		gMailer.m_eventlog.DeleteString(0);
	gMailer.m_eventlog.SetRedraw(1);
	cnt=gMailer.m_eventlog.GetCount()-2;
	gMailer.m_eventlog.SetTopIndex(cnt<0 ? 0 : cnt);
	writelog(LOG_EVENTS,text);
 	return 1;
}

// ==================================================================================
	void show_events_status(void)
// ==================================================================================
{
CString line,name,status,str;
int  	stat,tries;

	event_add_listbox(L("S_217"));	// events list beginning
	for (int i=0;i<eventlist.GetCount();i++)
	{
		line=eventlist.GetString(i);
		get_token(line,EVNT_enabled,str);	
		if (str=="1")
		{
			stat=get_token_int(line,IDTOstatus);	
			tries=get_token_int(line,IDTOacttry);	
			get_token(line,EVNT_event,name);
			switch (stat)
			{
		 		case -1:
					status="FAILED";
					break;
		 		case 0:
					status="TODO";
					break;
		 		case 1:
					status="OK";
					break;
		 		default:
					status="ILLEGAL";
					break;
			}
			event_add_listbox(L("S_208",name,status));	//,tries event status
		}
	}
	event_add_listbox(L("S_4"));	// end of listing
}

// ==================================================================================
	void delete_current_event(void)
// ==================================================================================
{
CString line,name;

	gc.restartevents=gc.no_event_update=1;
	line=eventlist.GetString(gc.actual_event_index);
	get_token(line,EVNT_event,name);
	strcpy(ddbuf,L("S_222",name));	// event removed
	event_add_listbox(ddbuf);
	eventlist.Remove(gc.actual_event_index);
}

// ==================================================================================
	void build_event_infomail(evstruct &evs,int ok)
// ==================================================================================
{
CString sender,mailtext,subject,name,event,completed;
char    x1[20],x2[20],now[50];

	if (get_cfg(CFG_COMMON,"DontGenInfo",0))
	{
		event_add_listbox(L("S_280"));	// infomail disabled
		return;
	}
	_strtime(x1);
	_strdate(x2);
	sprintf(now,"%s %s",x1,x2);
	event.Format("%s",evs.event);
	get_fullname(name);
	sender=L("S_226");	// event manager
	subject=L("S_302");	// local notification
	completed.Format(ok ? L("S_42") : L("S_41"));	// successful/failed
	mailtext=L("S_32",event,evs.from,evs.to,evs.boss1,completed,now,evs.acttry);	// event...
	build_routed_netmail(sender,"",name,"",subject,mailtext,0,0,1);
	event_add_listbox(L("S_300"));	// infomail generated
}

