// This File contains the whole stuff for ListBox filling
// Areaselection and so on
#include "stdafx.h"
#include "lightdlg.h"
#include "areasel.h"
#include "detpurg.h"
#include "search.h"
#include <io.h>

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern _gconfig gc;
extern detpurg  gPurger;
extern CByteArray AttrListBuffer;	 // buffer for attributes list
extern int		DelayedAreas[100];
extern CFont	font_subject;
extern int		SubjPos[3];
extern int		last_area;
char			orgbuf[MAX_MSG+1000];

int	gLastUpdate=0;
int	building_subjlist=0;

CStrList actdisplist;
CStrList subject_list;

int hlistcount=0;
int globalnest=0;
int globalfather=0;

struct _tree
{
	ULONG msgid;
	ULONG unixtime;
	_tree *preptr;
	_tree *nextptr;
	_tree *replyptr;
	mailheader *mheader;
};

_tree *basetree;
_tree *root;
mailheader **hlist;

int  append_to_tree					(mailheader *ph,_tree **tbase);
int  PreSortForDateTime 			(const void *a1,const void *a2);
int  build_hlist					(_tree **tbase);
int  should_this_mail_be_displayed 	(mailheader *mh);
void build_mail_size				(CString &sizeinfo);
void save_delayed_purge				(int hand);

extern int cached_current_mail_index;
extern int cached_current_mail_index_which;
extern int current_mail_index;
extern int current_mail_index_which;
extern CStrList EMails;
extern CListBox *m_globedit;

//  ==============================================================================
// creates area selection dialog in modal or modeless mode
	int CLightDlg::begin_area_selection(BOOL skip_dlg,BOOL aslist)
//  ==============================================================================
{
CAreasel areasel;
areadef ad;
//static int lastarea=-1;
int		trycount;

	if(!skip_dlg)
	{
		gc.asel_as_list=aslist;
		areasel.DoModal();
		if (gc.selected_area<0)	return 0;
	}

	jumpcount=0;		// clear Jumpstack Counter
	if (last_area != -1)	db_unlock_area(last_area);
// waiting unlocking area
	trycount=5;
	while (db_lock_area(gustat.cur_area_handle)==DB_AREA_ALREADY_LOCKED)
	{
		Sleep(500);
		if (--trycount <= 0)	ERR_MSG_RET0("W_DBCURLOCKED");
	}
	last_area=gustat.cur_area_handle;
	if (db_get_area_by_index(gustat.cur_area_handle,&ad) != DB_OK)
		ERR_MSG2_RET0("E_AREANOTFOUND",gustat.cur_area_handle);

    m_subjlist.SetFocus();
    return 1;
}

// ============================================================================
// shows all subjects for area in Subjects list
	int CLightDlg::show_subjects(int area_nr)
// ============================================================================
{
mailheader *pheader=0,*phbase=0,*phjwbase=0;
CString sizeinfo,str;
CSize	sz;
char	buf[300],*p;
int		indexcounter=0,dummymailcounter=0,cnt_headers=0,deleted_counter=0,counter_of_deleted=0;
int		i,e,oldcount,maxlev,curlev,retval=1,limit_nr_to_display=-1;

	get_fontsize(&font_subject,&m_subjlist,sz);
	maxlev=SubjPos[0]/sz.cx;

	gustat.first_non_read_mail=-1;
	gustat.last_already_read_mail=-1;
	cached_current_mail_index=-1;
	cached_current_mail_index_which=-1;

	AttrListBuffer.RemoveAll();
	m_subjlist.ResetContent();
	m_attrlist.ResetContent();
//	m_attrlist.Invalidate();

//m_subjtree.DeleteAllItems();

	if (grfilter.ShowLastOnly)
	{
		sscanf(grfilter.lastonly,"%d",&limit_nr_to_display);
		if (limit_nr_to_display<=0)
			limit_nr_to_display=-1;
	}

	if (building_subjlist)	return 1;
	building_subjlist=1;

  if (db_get_area_by_index(area_nr,&gustat.act_area) != DB_OK)
  {
	  db_unlock_area(area_nr);
		building_subjlist=0;
		ERR_MSG2_RET0("E_DBGETAREABYINDEX",area_nr);
	}
  if (db_get_area_handle(gustat.act_area.echotag,&gustat.ar2han,1) != DB_OK)
  {
		building_subjlist=0;
		ERR_MSG2_RET0("E_DBGETAREAHANDLE",gustat.act_area.echotag);
	}
	RefreshAreatag();

// ********************* NORMAL DISPLAY *********************************************
	if (gc.structured_display)	goto structured_display_enabled;

	BeginWaitCursor();
  if (db_getfirsthdr(gustat.ar2han,&gustat.act_mailh) != DB_OK)	 // empty area
	{
		building_subjlist=0;
		db_unlock_area(area_nr);
		EndWaitCursor();
		return 0;
	}
	if (limit_nr_to_display>=0 &&
		gustat.act_mailh.index<=(ULONG)(gustat.act_area.number_of_mails-limit_nr_to_display))
	{
		dummymailcounter++;
		subject_list.RemoveAll();
		goto skip_first_mail;
	}

	if ((gustat.act_mailh.status & DB_DELETED) && gc.hide_deleted ) 
		counter_of_deleted++;

	subject_list.RemoveAll();

// check the last read positions
	if (gustat.act_mailh.status & DB_MAIL_READ)
		gustat.last_already_read_mail=subject_list.GetCount();
	else if (gustat.first_non_read_mail==-1)
		gustat.first_non_read_mail=subject_list.GetCount();

	kill_tabs(gustat.act_mailh.fromname);
	kill_tabs(gustat.act_mailh.toname);
	kill_tabs(gustat.act_mailh.subject);
	build_mail_size(sizeinfo);

// form line in subject list
	sprintf(buf," \t%s\t%s\t%s\t%d, %s\t%d\t",gustat.act_mailh.fromname,gustat.act_mailh.toname,
		gustat.act_mailh.subject,++dummymailcounter,sizeinfo,gustat.act_mailh.index);	
	OemToChar(buf,buf);
	handle_save_of_cached_display_index(gustat.act_mailh.index);

	if (should_this_mail_be_displayed(&gustat.act_mailh))
	{
		test_watchstuff(0,&gustat.act_mailh);
		subject_list.AddTail(buf);
		add_marker();
	}

skip_first_mail:
	for (;;)
	{
		indexcounter++;
		if (db_getnexthdr(gustat.ar2han,&gustat.act_mailh) != DB_OK)
		{
			UPDATE_LB(subject_list,IDC_SUBJLIST);
			EndWaitCursor();
			building_subjlist=0;
		  return 1;
		}

		if (limit_nr_to_display>=0 && 
			(gustat.act_mailh.index+1)<=(ULONG)(gustat.act_area.number_of_mails-limit_nr_to_display))
		{
			dummymailcounter++;
			continue;
		}
		if ((gustat.act_mailh.status & DB_DELETED) && gc.hide_deleted ) 
			counter_of_deleted++;

// check the last read positions
		if (gustat.act_mailh.status & DB_MAIL_READ)
			gustat.last_already_read_mail=subject_list.GetCount();
		else if (gustat.first_non_read_mail==-1)
			gustat.first_non_read_mail=subject_list.GetCount();

		kill_tabs(gustat.act_mailh.fromname);
		kill_tabs(gustat.act_mailh.toname);
		kill_tabs(gustat.act_mailh.subject);
		build_mail_size(sizeinfo);
// form line in subject list
		sprintf(buf," \t%s\t%s\t%s\t%d, %s\t%d\t",gustat.act_mailh.fromname,gustat.act_mailh.toname,
			gustat.act_mailh.subject,++dummymailcounter,sizeinfo,gustat.act_mailh.index);	
		OemToChar(buf,buf);

		handle_save_of_cached_display_index(gustat.act_mailh.index);
	    if (should_this_mail_be_displayed(&gustat.act_mailh))
		{
			test_watchstuff(indexcounter,&gustat.act_mailh);
			subject_list.AddTail(buf);
			add_marker();
		}
	}

// ***************** STRUCTURED DISPLAY *****************************************
structured_display_enabled:

	if (db_getallheaders(area_nr,&pheader,&cnt_headers,TRUE) != DB_OK)
	{
		retval=0;
		goto free_all;
	}
	phjwbase=pheader;
	if (grfilter.ShowLastOnly)
	{
		int to_add;
		to_add=cnt_headers-limit_nr_to_display;
		if (limit_nr_to_display>=0 && to_add>0)
		{
			pheader+=to_add;
			cnt_headers-=to_add;
		}
	}

	hlist=(mailheader **)malloc(cnt_headers*sizeof(mailheader *)+100);
	if (!hlist)
	{
		err_out("E_MEM_OUT");
		retval=0;
		goto free_all;
	}

	phbase=pheader;
	BeginWaitCursor();
	qsort(pheader,cnt_headers,sizeof(mailheader),PreSortForDateTime);
	basetree=NULL;
	root=NULL;
	oldcount=0;
	for (e=0;e<cnt_headers;e++)
	{
		basetree=root; // basetree vom root-ptr holen
		{
			if (!gc.hide_deleted || !(pheader->status & DB_DELETED))
			{
				if (!append_to_tree(pheader,&basetree))
				{
					pheader->replyid=0;
					basetree=root;
					if (!append_to_tree(pheader,&basetree))
						oldcount++;
				}
			}
			else
				deleted_counter++;

		}

		if (root == NULL) // wenn root noch NULL ptr, dann holen wir uns den basetree als root
		root=basetree;
		pheader++;
	}

   pheader=phbase;
   hlistcount=globalnest=globalfather=0;
   basetree=root;
   build_hlist(&basetree);

   ASSERT((cnt_headers-deleted_counter)==hlistcount);

	subject_list.RemoveAll();	   // << SEE
	gustat.first_non_read_mail	=-1;
	gustat.last_already_read_mail=-1;
	for (i=0;i<(cnt_headers-deleted_counter); i++)
	{
		memcpy(&gustat.act_mailh,hlist[i],sizeof(mailheader));
		add_marker();
		// Check the last read positions

		if ((gustat.act_mailh.status & DB_DELETED) && gc.hide_deleted ) 
			counter_of_deleted++;

		if (gustat.act_mailh.status & DB_MAIL_READ)
			gustat.last_already_read_mail=subject_list.GetCount();
		else if (gustat.first_non_read_mail==-1)
			gustat.first_non_read_mail=subject_list.GetCount();

		kill_tabs(gustat.act_mailh.fromname);
		kill_tabs(gustat.act_mailh.toname);
		kill_tabs(gustat.act_mailh.subject);

// form line in subject list
		p=buf;
		*p++='\t';
		if (gustat.act_mailh.nestlevel>0)	// add nest level signs
		{
			curlev=gustat.act_mailh.nestlevel>maxlev ? maxlev : gustat.act_mailh.nestlevel;
			for (int n=0;n<curlev;n++)
				*p++='#';
		}

		build_mail_size(sizeinfo);
		sprintf(p,"%s\t%s\t%s\t%d, %s\t%d\t",gustat.act_mailh.fromname,gustat.act_mailh.toname,
			gustat.act_mailh.subject,++dummymailcounter,sizeinfo,gustat.act_mailh.index);	

		handle_save_of_cached_display_index(gustat.act_mailh.index);

		OemToChar(buf,buf);
		test_watchstuff(i,&gustat.act_mailh);
		subject_list.AddTail(buf);
	}

	UPDATE_LB(subject_list,IDC_SUBJLIST);
	EndWaitCursor();

free_all:
	// Free all stuff
	if (phjwbase)
	{
		free(phjwbase);
		pheader=phjwbase=0;
	}
	if (hlist)
	{
		free(hlist);
		hlist=0;
	}
	m_attrlist.Invalidate();
	building_subjlist=0;
	return retval;
}

// 	============================================================================
// sets mail marker in attributes list
	void CLightDlg::add_marker(void)
// 	============================================================================
{
int cnt;
char mrk[2];

	mrk[1]=0;
	mrk[0]=get_mail_marker(gustat.act_mailh.status);
	AttrListBuffer.Add(mrk[0]);
	m_attrlist.AddString(mrk);
	cnt=m_attrlist.GetCount();
	if (cnt>0)
		set_usermark(cnt-1,gustat.act_mailh.status & DB_USERMARKED);
}

// =================================================================
// purges areas range
	void purge_one_or_more_areas(int from,int to)
// =================================================================
{
areadef  ad;
int		 na;

	na=get_cfg(CFG_PURGER,"NotProcAsaved",0);
	gPurger.AddMessage(L("S_411"));		// begin purging
	gc.purger.text_only=0;						// Update Text only
	gc.purger.area_counter=0;
	gc.purger.number_of_areas=to-from+1;
	gc.purger.mails_total=0;
	gc.purger.mails_removed=0;
	gc.purger.bytes_total=0;
	gc.purger.bytes_removed=0;

	for (int i=from;i<=to;i++)
	{
		if (gc.purger.breakit) 
		{
			gc.purger.breakit=0;
			gPurger.AddMessage(L("S_76"));	// aborted
			break;
		}
		gc.purger.area_counter++;
		memset(&ad,0,sizeof(areadef));
		db_get_area_by_index(i,&ad);
		if (na && !strnicmp(ad.echotag,ASAVED_PREFIX,strlen(ASAVED_PREFIX)))
		{
			gPurger.AddMessage(L("S_408",ad.echotag));	// skipped
			continue;
		}
		if (db_lock_area(i)==DB_AREA_ALREADY_LOCKED)
		{
			save_delayed_purge(i);
			gc.purger.text_only=1;
			gPurger.AddMessage(L("S_303",ad.echotag));	// locked
			continue;
		}

		if (db_purge_area(i))
			gPurger.AddMessage(L("S_368",ad.echotag));	// purged
		else
			gPurger.AddMessage(L("S_238",ad.echotag));	// failed
		db_unlock_area(i);
	}

	gc.purger.text_only=1;
	gPurger.AddMessage(L("S_367"));	// ended
}

// =================================================================
	int autosave_mail(void)
// =================================================================
{
mailheader	mh;
areadef		ad;
long			hnd;
char			buf[200],tmp[MAX_MSG];
int				fullmail;

	if (gc.tosser.running)	ERR_MSG_RET0("E_NOSAVETOSSRUN");
	if (gc.AutoSaveDestArea[0])
		sprintf(buf,"%s",gc.AutoSaveDestArea);
	else if (get_cfg(CFG_COMMON,"GlobalASave",1))	  // save to global?
		sprintf(buf,"%sGLOBAL",ASAVED_PREFIX);
	else
		sprintf(buf,"%s%s",ASAVED_PREFIX,gustat.act_area.echotag);

	if (!db_get_area_handle(buf,&hnd,1))	ERR_MSG2_RET0("E_ASAVE",buf);

// Marks this Area as ASAVED
  db_get_area_by_index(hnd,&ad);
	strcpy(ad.group,ASAVED_GROUP);
  db_change_area_properties(&ad);

	sprintf(orgbuf,"%s\r",L("S_130",gustat.act_area.echotag));	// copy from area
	if (gc.ViewOem)	CharToOem(orgbuf,orgbuf);
	fullmail=get_selected_lines(tmp,2,FALSE);
	if (fullmail)
	{
		if (!gc.ViewOem)	CharToOem(tmp,tmp);
	}
	else 
		if (db_get_mailtext(gustat.ar2han,&gustat.act_mailh,tmp,MAX_MSG) != DB_OK)	return 0;

	strcat(orgbuf,tmp);
	memcpy(&mh,&gustat.act_mailh,sizeof(mh));
	mh.mail_text=orgbuf;
	mh.text_length=strlen(orgbuf)+1;
	mh.status &= ~(DB_MAIL_SCANNED | DB_USERMARKED);
	mh.status |= DB_MAIL_READ; //DB_MAIL_CREATED;

	if (!db_append_new_message(hnd,&mh))	ERR_MSG_RET0("E_AMAILSAVE");
	show_msg(fullmail ? L("S_172") : L("S_170"));	// mail/selection saved into ASAVED
	db_refresh_area_info(hnd);
	return 1;
}

//===================================================
// sort array of headers on their timestamps
// if timestamps are equal, then compares their mailid's
	int PreSortForDateTime (const void *a1,const void *a2)
//===================================================
{
mailheader *p1,*p2;

	p1=(mailheader *)a1;
	p2=(mailheader *)a2;

	if (p1->unixtime==p2->unixtime)
	{
		if (p1->mailid==p2->mailid)
			return 0;
		else if (p1->mailid < p2->mailid)
			return -1;
		else
			return 1;
	}
	else if (p1->unixtime < p2->unixtime)
		return -1;
	else
		return 1;
}

// ================================================================
	int append_to_tree(mailheader *ph,_tree **tbase)
// ================================================================
// olis geniale rekursive funktion zum aufbau eines binaeren baums ;-)
{
	// wenn der uebergebene pointer NULL ist, so muessen wir einen neuen eintrag erstellen.
	if (!(*tbase))
	{
		*tbase=(_tree *)malloc(sizeof(_tree));
		if (!(*tbase))
			ERR_MSG_RET0("E_MEM_OUT");

		(*tbase)->msgid=ph->mailid;
		(*tbase)->preptr=NULL;
		(*tbase)->nextptr=NULL;
		(*tbase)->replyptr=NULL;
		(*tbase)->mheader=ph;
		(*tbase)->unixtime=parse_time(ph->datetime);
		return 1;
	}

	// wenn keine replyidvorhanden, dann haengen wir es an den nextptr (rekursiv versteht sich ;-)
	if (ph->replyid==0)
		return append_to_tree(ph,&(*tbase)->nextptr);

	// wenn replyid, dann muessen wir nachgucken ob wir eine msgid im baum finden auf die es passt
	if (ph->replyid != 0)
	{
		// wenn eine msgid im baum gefunden wurde, und der replyptr != NULL ist, so muessen
		// wir den neuen eintrag an den nextptr des replyptrs haengen
		if ((*tbase)->msgid == ph->replyid && (*tbase)->replyptr != NULL)
		{
			struct _tree *tt;

			tt = (*tbase)->replyptr;
			while (tt->nextptr) // hier war ein ; der mich 4 stunden kostete ;-(
				tt = tt->nextptr;		// hier suchen wir den naechsten freienh nextptr

			if (append_to_tree(ph,&tt->nextptr)) // und jetzt den eintrag anhaengen
				return 1;
		}

		// wenn die msgid und replyid passen und der replyptr NULL ist, dann haengen wir den
		// neuen eintrag einfach in den replyptr
		if ((*tbase)->msgid == ph->replyid && !(*tbase)->replyptr)
			return append_to_tree(ph,&(*tbase)->replyptr);

		// wenn msgid und replyid nicht passen, und der aktuelle replyptr != NULL ist
		// dann muessen wir im ast des replyptrs suchen ob nicht evtl. dort eine
		// passende msgid liegt
		// wenn das gut geht, return 1
		if ((*tbase)->msgid != ph->replyid && 
			(*tbase)->replyptr && 
			append_to_tree(ph,&(*tbase)->replyptr))
				return 1;

		// im ast der replyptr's wurde nichts gefunden (sonst haetten wir schon returned)
		// also suchen wir im aktuellen nextptr weiter wenn dieser != NULL ist
		// ansonsten returnen wir am ende der funktion 0. die aufrufende
		// funktion weiss dann, dass keine passende msgid gefunden wurde
		// und somit fuer diese reply keine originalmail vorhanden ist
		if ((*tbase)->msgid != ph->replyid &&
			(*tbase)->nextptr &&
			append_to_tree(ph,&(*tbase)->nextptr))
				return 1;

		// nichts passendes fuer replyid gefunden
		return 0;
	}
	return 0;
}

// ================================================================
	int build_hlist(struct _tree **tbase)
// ================================================================
{
_tree *tt;

	tt=(*tbase); 				// hier merken wir uns den aktuellen tree-pointer
	if (!tt)						// Keine Mails in dieser Area
	    return 1;

	ASSERT(tt->mheader); 	// wenn der NULL ist, dann haben wir das recht kraeftig zu scheppern

	hlist[hlistcount] = tt->mheader; // hlist setzen
	ASSERT(hlist[hlistcount]);
	hlistcount++;

	(tt->mheader)->nestlevel = globalnest; // nestlevel sichern

	if ((*tbase)->replyptr) // der replyptr != NULL, also eine eben tiefer
	{								// und wir rufen uns auf dem replptr rekursiv auf (das machen wir gerne)
		globalnest++;
		globalfather++;
		build_hlist(&tt->replyptr);
		globalnest--;
		globalfather--;
	}

	tt=(*tbase); // wieder sichern

	if (tt->nextptr != NULL) 		// wenn im nextptr was steht, dann rufen wir diesen wiederum rekursiv auf
	{									// wir machen das noch immer gerne
		build_hlist(&tt->nextptr);
	}

	free((*tbase)); // jetzt den eintrag freigeben und mit 1 returnen (0 geht ja eigentlich gar ned)
	*tbase=0;
	return 1;
}

// ==================================================================0
	void build_mail_size(CString &sizeinfo)
// ==================================================================0
{
   sizeinfo.Format("%5.1fK",gustat.act_mailh.text_length/1024.);
}

// ==================================================================
	int should_this_mail_be_displayed(mailheader *mh)
// ==================================================================
// Hier wird festgelegt ob eine Mail dargestellt werden soll oder ned
{
CString	str;
int		inv,st,days;
int		offsets[300];

	inv=grfilter.Inverse;
	st=mh->status;
	if (gc.hide_deleted && st & DB_DELETED)  // hide deleted mails
		return 0;
	if (grfilter.OnNoFilter)
		return 1;
	if (grfilter.OnMarkedOnly)
		return ((st & DB_USERMARKED && 1) ^ inv);
	if (grfilter.OnFrozenOnly)
		return ((st & DB_FROZEN_MAIL && 1) ^ inv);
	if (grfilter.OnUnread)
		return !((st & DB_MAIL_READ && 1) ^ inv);
	if (grfilter.OnNoReply)
		return !((mh->replyid && 1) ^ inv);
	if (grfilter.OnOutgoing)
		return ((st & DB_MAIL_CREATED && 1) ^ inv);
	if (grfilter.OnNotOlder)
	{
		sscanf(grfilter.days,"%d",&days);
		return ((difftime(time(NULL),mh->recipttime)<(24*60*60*days) && 1) ^ inv);
	}
	if (grfilter.OnSinceLastPoll)  // Since last poll
		return ((mh->recipttime>gLastSucessfulCallTime && 1) ^ inv);

	if (grfilter.OnCInFrom)  // In FROM
		return ((find_next_entry(mh->fromname,0,grfilter.from,offsets,1,1) && 1) ^ inv);

	if (grfilter.OnCInTo)  // In TO
		return ((find_next_entry(mh->toname,0,grfilter.to,offsets,1,1) && 1) ^ inv);

	if (grfilter.OnCInSubject)  // In SUBJECT
		return ((find_next_entry(mh->subject,0,grfilter.subject,offsets,1,1) && 1) ^ inv);

	if (grfilter.OnHeaderOnly)  // In Header
	{
		if (find_next_entry(mh->fromname,0,grfilter.header,offsets,1,1) ||
			find_next_entry(mh->toname,0,grfilter.header,offsets,1,1) ||
			find_next_entry(mh->subject,0,grfilter.header,offsets,1,1))
				return !inv;
		return inv;
	}
	return !inv;
}

// ==============================================
	void mark_all_mails_as_read(void)
// ==============================================
{
mailheader mh;

	for (int i=0;;i++)
	{
		if (db_gethdr_by_index(gustat.cur_area_handle,i,&mh) != DB_OK)	break;
		if (mh.status & DB_MAIL_READ)	continue;
		mh.status |= DB_MAIL_READ;
		if (db_sethdr_by_index(gustat.cur_area_handle,i,&mh) != DB_OK)
			ERR_MSG2_RET("E_DBSETHDRBYINDEX",gustat.cur_area_handle);

		set_mailmark(i,'x');
	}
	db_set_area_to_read(gustat.cur_area_handle);
}

// ==============================================
	void unmark_all_mails(void)
// ==============================================
{
mailheader mh;

	for (int i=0;;i++)
	{
	  if (db_gethdr_by_index(gustat.cur_area_handle,i,&mh) != DB_OK)	break;
		if (!(mh.status & DB_USERMARKED))
		{
			set_usermark(i,0);
			continue;
		}
		mh.status &= ~DB_USERMARKED;
    if (db_sethdr_by_index(gustat.cur_area_handle,i,&mh)!=DB_OK)
			ERR_MSG2_RET("E_DBSETHDRBYINDEX",gustat.cur_area_handle);

		set_usermark(i,0);
	 }
}

// ==============================================
	void mark_all_mails_as_deleted(void)
// ==============================================
{
mailheader mh;

	for (int i=0;;i++)
	{
    if (db_gethdr_by_index(gustat.cur_area_handle,i,&mh) != DB_OK)	break;
		if (mh.status & (DB_DELETED | DB_NEVER_DELETE))	continue;
		mh.status |= DB_DELETED | DB_MAIL_READ;
	  if (db_sethdr_by_index(gustat.cur_area_handle,i,&mh) != DB_OK)
			ERR_MSG2_RET("E_DBSETHDRBYINDEX",gustat.cur_area_handle);

		set_mailmark(i,'D');
	 }
}

// ==============================================
	void test_watchstuff(int index,struct mailheader *mh)
// ==============================================
{
	if (gc.WatchDisplayMails)
	{
		if (!watchmail.isCrossfind)
		{
			if (mh->index==watchmail.index && mh->mailid==watchmail.mailid)
				watchmail.dispoff=subject_list.GetCount();
		}
		else
		{
			if (watchmail.mh.mailid==mh->mailid &&
				!strcmp(watchmail.mh.fromname,mh->fromname) &&
				!strcmp(watchmail.mh.toname,mh->toname) &&
				!strcmp(watchmail.mh.subject,mh->subject))
					watchmail.dispoff=subject_list.GetCount();
		}
	}
}

// ==========================================================
	void handle_save_of_cached_display_index(int mailindex)
// ==========================================================
{
	if (current_mail_index==mailindex)
		cached_current_mail_index=subject_list.GetCount();
	if (current_mail_index_which==mailindex)
		cached_current_mail_index_which=subject_list.GetCount();
}

// ==========================================================
	void handle_recipt_template(CString &mailtext)
// ==========================================================
{
CStrList lst;
char	 tmp[MAX_PATH];

	make_path(tmp,gc.BasePath,RECIPTTEMPLATE);
	if (access(tmp,0))	return;

	lst.FillAsEdit(tmp);
	for (int i=0;i<lst.GetCount();i++)
		mailtext=mailtext+lst.GetString(i)+"\r";
}

// =================================================================
	void save_delayed_purge(int hand)
// =================================================================
{
	for (int i=0;i<100;i++)
	{
		if (DelayedAreas[i]==-1)
		{
		    DelayedAreas[i]=hand;
			return;
		}
		if (DelayedAreas[i]==hand)
			return;

	}
}
