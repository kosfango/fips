#include "stdafx.h"
#include "csobj.h"

CSobj::CSobj() { }
CSobj::~CSobj(){ }

// ================================================
	void CSobj::BuildSearchStuff(const char *buf)
// ================================================
{
CString help;

	mask	=0;
	state	=0;
	get_token(buf,0,help);
	if (help==L("S_83"))	{ mask=EXS_ALL;			goto found; }
	if (help==L("S_270"))	{ mask=EXS_HEADER;		goto found; }
	if (help==L("S_82"))	{ mask=EXS_ADDRESS;		goto found; }
	if (help==L("S_264"))	{ mask=EXS_FROM;		goto found; }
	if (help==L("S_424"))	{ mask=EXS_TO;			goto found; }
	if (help==L("S_416"))	{ mask=EXS_SUBJECT;		goto found; }
	if (help==L("S_312"))	{ mask=EXS_MAILTEXT;	goto found; }
	if (help==L("S_375"))	{ mask=EXS_RECEIVE;		goto found; }
	if (help==L("S_148"))	{ mask=EXS_CREATION;	goto found; }
	if (help==L("S_292"))	{ mask|=EXS_LASTCALL;	goto found; }
	if (help==L("S_311"))	{ mask=EXS_MAILSTATUS;	goto found; }
	mask=EXS_ALL;
	
found:
	get_token(buf,1,help);
	if (help=="-")
		mask|=EXS_NOT;

	get_token(buf,2,help);
	if (help==L("S_140"))	{ mask|=EXS_CONTAINS;		goto found1; }
	if (help==L("S_102"))	{ mask|=EXS_BEGINS;			goto found1; }
	if (help==L("S_473"))	{ mask|=EXS_YOUNGER;		goto found1; }
	if (help==L("S_395"))	{ mask|=EXS_SCANNED;		goto found1; }
	if (help==L("S_325"))	{ mask|=EXS_NEW_OWN_MAIL;	goto found1; }
	if (help==L("S_85"))	{ mask|=EXS_ALREADY_READ;	goto found1; }
	if (help==L("S_324"))	{ mask|=EXS_NEVER_DELETE;	goto found1; }
	if (help==L("S_267"))	{ mask|=EXS_FROZEN;			goto found1; }
	if (help==L("S_458"))	{ mask|=EXS_USERMARKED;		goto found1; }
	if (help==L("S_313"))	{ mask|=EXS_MARKEDREAD;		goto found1; }
	mask|=EXS_CONTAINS;

found1:
	get_token(buf,3,help);
	if (help=="+")			mask|=EXS_CASESENS;
	mailtext_needed=(mask & EXS_TEXTNEEDED);

	get_token(buf,4,help);
	help.AnsiToOem();
	searchtext=help;
}

// ================================================
	int  CSobj::WasSearchSuccessfull(struct mailheader *mh,char *mailtext)
// ================================================
// Search in accordance with Flags
{
#ifdef DEBSEARCH
	char ddbuf[500];
#endif

	if (mask & EXS_ALL)
	{
		if ((mask & EXS_FROM) && handle_combined_search(mh->fromname))
			return !(mask & EXS_NOT);

		if ((mask & EXS_TO) && handle_combined_search(mh->toname))
			return !(mask & EXS_NOT);

		if ((mask & EXS_SUBJECT) && handle_combined_search(mh->subject))
			return !(mask & EXS_NOT);

		if ((mask & EXS_MAILTEXT) && handle_combined_search(mailtext))
			return !(mask & EXS_NOT);

		return (mask & EXS_NOT);
	}

	if (mask & (EXS_RECEIVE | EXS_CREATION | EXS_LASTCALL))
		return(handle_datums_check(mh));

	if (mask & EXS_MAILSTATUS)
		return(handle_status_check(mh));


	// Wenn wir hierherkommen war kein AbfrageBit gesetzt, und das sollte nicht
	// passieren

	#ifdef DEBSEARCH
		sprintf(ddbuf,"SearchObj:\tKein SerachBIT gefunden \n");
		OutputDebugString(ddbuf);
	#endif

	ASSERT(0);
	return 0; // Suche war nicht erfolgreich ...
}

// ================================================
	int  CSobj::handle_combined_search(char *serchin)
// ================================================
// TOP: Diese Funktion sucht im uebergebenen String
// Ausgewertet werden BEGINS,CONTAINS, und CASE-SENSITIVE
// DAs EXS_NOT Flag wird in hier nicht behandelt, da es in den uebergeordneten 
// Funktionen behandelt wird ...
{
CString tosearch;
CString pattern;

	if (mask & EXS_BEGINS)
	{//	"BEGINS"
		if (mask & EXS_CASESENS)
			return !strncmp(serchin,searchtext,searchtext.GetLength()); 
		else
			return !strnicmp(serchin,searchtext,searchtext.GetLength());
	}
	else
	{//	CONTAINS
		if (mask & EXS_CASESENS)
			return (strstr(serchin,searchtext)!=0);
		else
		{
			tosearch=serchin;
			pattern	=searchtext;
			pattern.MakeUpper();
			tosearch.MakeUpper();
			return (tosearch.Find(pattern)!=-1);
 		}
	}
	return 0;
}

// ================================================
int  CSobj::handle_datums_check	(struct mailheader *mh)
// ================================================
// TOP: Diese Funktion behandelt EXS_RECEIVE, EXS_CREATION
// und dabei nur das Feld YOUNGER 
{
int		retval=-1;
int		seconds=0;
int		days=0;
long	now=0;
	
	if (mask & (EXS_CREATION | EXS_RECEIVE))
	{
		sscanf(searchtext,"%d",&days);
		if (!days)
			return 0;

		seconds=days * 86400;
		now=time(NULL);
	}

	if (mask & EXS_CREATION)
	{
		// Falls die Mail keine MAILID hat
		// geben wir FLASE zurueck ...
		if (!mh->mailid)
			return 0; 

		// Ansonsten testen wir auf das Alter
		retval=((now-parse_time(mh->datetime)) < seconds); 
		goto endit;
	}

	if (mask & EXS_RECEIVE)
	{
		// Falls die Mail keine RECEIVE ID hat
		// geben wir FLASE zurueck ...
		if (!mh->recipttime)
			return 0; 

		// Ansonsten testen wir auf das Alter
		retval=((now-mh->recipttime) < seconds); 
		goto endit;
	}

	if (mask & EXS_LASTCALL)
	{
		// Falls die Mail keine MAILID ID hat
		// geben wir FLASE zurueck ...
		if (!mh->recipttime)
			return 0; 

		// Ansonsten schauen wir ob das Empfangsalter groesser als das
		// Datum der letzten ModemSession war ..
		retval=(mh->recipttime > last_modemsession_time); 
		goto endit;
	}

endit:
	ASSERT (retval!=-1);
	if (mask & EXS_NOT)
		retval=!retval;
	return retval;
}

// ================================================
int  CSobj::handle_status_check	(struct mailheader *mh)
// ================================================
// TOP: Diese Funktion behandelt die Mailstati Abrfragen 
{
int retval	=-1;

	if (mask & EXS_SCANNED)
	{
		retval=(mh->status & DB_MAIL_SCANNED); 
		goto endit;
	}
	if (mask & EXS_NEW_OWN_MAIL)
	{
		retval=(mh->status & (DB_MAIL_CREATED | DB_MAIL_SCANNED)); 
		goto endit;
	}
	if (mask & EXS_ALREADY_READ)
	{
		retval=(mh->status & DB_MAIL_READ); 
		goto endit;
	}
	if (mask & EXS_NEVER_DELETE)
	{
		retval=(mh->status & DB_NEVER_DELETE); 
		goto endit;
	}
	if (mask & EXS_FROZEN)
	{
		retval=(mh->status & DB_FROZEN_MAIL); 
		goto endit;
	}
	if (mask & EXS_USERMARKED)
	{
		retval=(mh->status & DB_USERMARKED); 
		goto endit;
	}
	if (mask & EXS_MARKEDREAD)
	{
		retval=(mh->status & DB_MAIL_READ); 
		goto endit;
	}

endit:
	ASSERT (retval!=-1);
	if (mask & EXS_NOT)
		retval=!retval;
	return retval ? TRUE : FALSE;
}











