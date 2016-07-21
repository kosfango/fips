#include "stdafx.h"
#include <io.h>
#include "light.h"
#include "detmail.h"
#include "dettoss.h"
#include "detpurg.h"
#include "det_nl.h"
#include "exsearch.h"
#include "floating.h"
#include "nodesel.h"
#include "mmsystem.h"
#include "lightdlg.h"
#include "msgbox.h"
#include "cuuendec.h"
#include "timed.h"
#include "gcomm.h"
#include "direct.h"
#include "shlobj.h" 
#include <afxtempl.h>
#include "sup\supercom.h"
#include "mime\cb64endec.h"
#include "regexp.h"

// EMAIL - Tosser Stuff
static char   *mail =0;
static char   *mbuf2=0;
static long   hnd=0;
static mailheader 	header;
static int    counter=0;

// variables for Multiple Selection
int 	count_of_selected=0;
int		current_selected=0;
int   selected_entries[MAX_SELMAILS];	// indexes for multiple selection
char	gStr[300],gTmp[300];

const char	*emonth[]={"january","february","march","april","may","june","july","august","september","october","november","december"};
const char	*rmonth[]={"янваpя","февpаля","маpта","апpеля","мая","июня","июля","августа","сентябpя","октябpя","ноябpя","декабpя"};

extern CLightApp FipsApp;
extern CLightDlg *gpMain;
extern dettoss	gTosser;
extern detmail  gMailer;
extern detpurg  gPurger;
extern det_nl   gNLComp;
extern exsearch gSearch;
extern floating gFloating;
extern CStrList	LangStrList;
extern CStrList SoundLst;
extern CStrList converts;
extern CStrList teltrans;
extern CStrList actdisplist;
extern CStrList dialbrk;
extern CStrList trepls;
extern CStrList jokes;
extern CStrList income_times;
extern CStrList UserdefAreaList;
extern CStrList gatecfg;
extern CStrList costbaselist;
extern CStrList costuserlist;
extern CString isdnaccept;
extern CStrList AddKludges;
extern CStrList HideKludges;
extern _gcomm	gcomm;
extern _gconfig gc;
extern _modemcfg gModemCfg;
extern _packer	Packers[];
extern LangID	LANG_ID;
extern HINSTANCE hRes;
extern CMap<__int64,__int64,_tzone,_tzone&>	Tzutc;

CFont  font_mailtext;
CFont  font_mail_under;
CFont  font_mail_italic;
CFont  font_under_italic;
//CFont	 font_editor;

char *subject_help_buffer		=0;
char *subject_help_actual_char	=0;

CStrList LangDlgList;
CStrList LangMnuList;
extern CByteArray AttrListBuffer;

int	MenuIDs[]={
/*1*/ID_EDIT_SELECTALL,ID_EDIT_COPY,ID_MAILCOPY,ID_SAVEMAILTOFILE,ID_AUTOSAVEMAIL,
/*6*/ID_ASAVETOAREA,ID_CAPTURE_SENDER,ID_CAPTURE_RECIPIENT,ID_EDIT_PRINTMAIL,ID_PRINT_CFG,
/*11*/ID_FILE_EXIT,ID_MENU_DIRECTRIES,ID_MENU_BOSSCONFIG,ID_MENU_PACKER,ID_CFG_ADDITIONALAKAS,
/*16*/ID_MENU_MODEM,ID_MENU_MODEM2,IDD_ISDN_CONFIGURATIONS,ID_CFG_DIALINGTIMEOUTS,ID_WINDOWSTYLE,
/*21*/ID_SUBJECT_ADJUST,ID_SCALE_10,ID_SCALE_20,ID_SCALE_30,ID_SCALE_40,
/*26*/ID_SCALE_50,ID_SCALE_60,ID_SCALE_70,ID_SCALE_80,ID_SCALE_90,
/*31*/ID_SCALE_100,ID_RESET_DLGPOS,ID_CFG_SUBJLISTFONT,ID_MAILTEXTFONT_ANSI,ID_MAILTEXTFONT_OEM,
/*36*/ID_CFG_EDITORFONT,ID_CFG_LABELSFONT,ID_CFG_FIELDSFONT,ID_CFG_NEWMAIL,ID_CFG_TEXTIMPORT,
/*41*/ID_CFG_EDITORTYPEREPLACMENT,ID_CFG_KLUDGES,ID_CFG_EDITOR,ID_CFG_TRASHCANFILTER,ID_CARBONCOPY,
/*46*/ID_MENU_AREAS,ID_MENU_NODELIST,ID_CFG_TICECHOSETUP,ID_CFG_ADRESSBOOK,ID_MMDISPLAYOPTIONS,
/*51*/ID_MMAVAILABLEELEMENTS,ID_CFG_SYSTEMSOUNDS,ID_CFG_FLOATING,ID_CFG_EVENTS,ID_LOGFILE_CFG,
/*56*/ID_COSTCALCULATION,ID_CFG_OTHERCONFIGS,ID_MAILS_CREATENEWMAILINS,ID_CHANGEMAIL,ID_QUOTE,
/*61*/ID_QUOTE_NETMAIL,ID_QUOTE_A,ID_FORWARDMAIL,ID_CROSSPOST,ID_SIMPLE_SEARCH,
/*66*/ID_FINDORIGINAL,ID_SWITCH_EXTSEARCH,ID_SORT_FROM,0,ID_SORT_TO,
/*71*/0,ID_SORT_SUBJECT,0,ID_SORT_CREATE,0,
/*76*/ID_SORT_RECIEVE,0,ID_SORT_STATUS,ID_SORT_ORDER,ID_CFG_DISPLAYFILTER,
/*81*/ID_MUL_AUTOSAVE,ID_SAVEMARKEDTOAREA,ID_SAVE_MULTIPLE,ID_MUL_DELETE,ID_MUL_AS_READ,
/*86*/ID_MUL_NEVER_DELETE,ID_MAILPRESENTAGAIN,ID_MUL_AS_PRINT,ID_MARKALLASREAD,ID_DELETE_ALL,
/*91*/IDM_ALLAREASREAD,ID_MARK_MAIL_FREEZE,ID_NEVERPURGETHISMAIL,ID_DELETE_MAIL,ID_TOGGLE_USERMARK,
/*96*/ID_UNMARKALLMESSAGES,ID_UUFROMSELECTED,ID_BEGINMULTIUUE,ID_ENDMULTIUUE,ID_CHECKDUPES_CURRENT,
/*101*/ID_CHECKDUPES_ALL,ID_TOGGLEROT13,ID_INCOMINGDEVICES,ID_INCOMINGNUMOFRINGS,ID_REQUESTPATHS,
/*106*/ID_MAGICS,ID_CFG_FAX,ID_LOCALLOGIN,ID_CFG_BBS,ID_CFG_BBSGROUPS,
/*111*/ID_BBSUSERCONFIG,ID_BBSAREACONFIG,ID_INCOMING_MODEM1,ID_INCOMING_MODEM2,ID_REMOTEMONITOR,
/*116*/ID_RESCANMAIL,ID_STARTMAILER,ID_STARTTOSSER,ID_STARTPURGER,ID_STARTNLCOMPILE,
/*121*/ID_FILEREQUEST,ID_FREQFROMMAIL,ID_POLLASYSTEM,ID_HATCHAFILE,ID_ECHOLISTMANAGEMENT,
/*126*/ID_ORDERECHOES,ID_CFG_SCRIPTEDITOR,ID_STARTSCRIPT1,ID_STARTSCRIPT2,ID_STARTSCRIPT3,
/*131*/ID_STARTSCRIPT4,ID_STARTSCRIPT5,ID_EXECUTEANYSCRIPT,ID_ABORTSCRIPT,ID_ABORTEVENT,
/*136*/ID_STARTWWWBROWSER,ID_SWITCH_MAILER,ID_SWITCH_TOSSER,ID_SWITCH_PURGER,ID_SWITCH_NODELIST,
/*141*/ID_SWITCH_LEFTICONS,ID_HSFLOATING,ID_VIEWOUTBOUND,ID_PRESENTAGAIN,ID_VIEWLOGFILES,
/*146*/ID_INOUTCALLHISTORY,ID_AREATRAFFICINFO,ID_COSTANALYZE,ID_ADDRESSINFO,ID_HELPME,
/*151*/ID_HELPMAINSCREEN,ID_FIDOLANG,ID_DICTIONARY,ID_HELP_ABOUT,ID_UMLAUTCONVERT,
/*156*/ID_SCALE_30,ID_SCALE_100,IDC_LAST_SCALE,ID_AREA,ID_TOGGLE_SUB_DISP,
/*161*/IDC_CROSSJUMP,ID_TOGGLE_USERMARK,ID_MARKALLASREAD,IDC_NEVERDELETE,ID_DELETE_SELECTED,
/*166*/ID_DELETE_ALL,ID_SAVE_MULTIPLE,ID_SAVEMARKEDTOAREA,ID_UUFROMSELECTED,ID_FREQFROMMAIL,
/*171*/ID_STARTWWWBROWSER,ID_EDIT_COPY,ID_ASAVETOAREA,IDC_QUOTE,IDC_QUOTEA,
/*176*/ID_QUOTE_NETMAIL,ID_FORWARDMAIL,ID_ORDERECHOES,ID_COPY_FROM,ID_COPY_TO,
/*181*/ID_COPY_SUBJECT,ID_CAPTURE_SENDER,ID_CAPTURE_RECIPIENT,ID_INFO_SENDER,ID_INFO_RECIPIENT,
/*186*/IDC_CLIP_PASTE,IDC_CLIP_COPY,IDC_CLIP_CUT,ID_OEMTEXT,IDC_BINARY_INSERT,
/*191*/IDC_MULTIMEDIA_INSERT,IDC_REPLACEDIALOG,IDC_ROT13,IDC_FINDINET,IDC_USERECIPIENT,
/*196*/ID_TO_PASTE,ID_ADDR_PASTE,ID_SUBJ_PASTE,ID_SORT_NAME,ID_SORT_ADRESS,
/*201*/ID_SORT_COMMENT,IDC_NAME,IDC_ADDRESS,IDC_TOWN,IDC_BBSNAME,
/*206*/IDC_PHONE,IDC_FLAGS,ID_SET_ENG,ID_SET_GER,ID_SET_RUS,
/*211*/ID_SET_FRE,ID_CONFIG,ID_ANSITEXT,ID_RESTORE,IDC_EXIT
};

// ===============================================================================
	int system_command(LPCSTR cmd,			// command to execute
						LPSTR title,		// title of process
						LPCSTR startupdir,	// 'HOME' directory for new process
						int	 showwindow,	// How to start
						int	 Timeout,		// Allowed timeout to complete(msec, or INFINITE)
						int  *retval,       // The return value from the executed cmd
						int	 suppress_error	// return error ?
						)
// ===============================================================================
{
STARTUPINFO			startUpInfo;
PROCESS_INFORMATION	procInfo;
BOOL	success=0;
int		ret=0,help=0,iswinnt=0;
char    exectool[400],exefile[300],cmddir[MAX_PATH];
char	*p;

	memset(&startUpInfo,0,sizeof(startUpInfo));
	memset(&procInfo,	0,sizeof(procInfo));

    GetStartupInfo(&startUpInfo);
	startUpInfo.lpTitle=title;
	startUpInfo.wShowWindow=showwindow;
	startUpInfo.dwFlags=STARTF_USESHOWWINDOW;

	// Kill trailing backslashes
	strcpy(cmddir,startupdir);
	p=cmddir+strlen(cmddir)-1;
	while (p>=cmddir && *p=='\\')
		*p--=0;

	*exefile=0;
	sscanf(cmd,"%s",exefile);
	if (!is_exe_available(exefile))
	{
		// Cannot find this FILE, .EXE .COM .PIF .CMD or .BAT in Path
		if (!suppress_error)
			err_out("E_EXENFOUND",cmd);
		return 0;
	}

	sprintf(exectool,"exectool.exe 1 \"%s\"\\fipstoss.log \"%s\" \"%s\"",gc.BasePath,cmddir,cmd);
    success=CreateProcess(0,exectool,0,0,FALSE,CREATE_NEW_CONSOLE,0,0,&startUpInfo,&procInfo);
    if (!success)
	{
		if (!suppress_error)
			err_out("E_CANOTEXEC");
		return 0;
	}

	ret=WaitForSingleObject(procInfo.hProcess,Timeout);
	if (ret	!= WAIT_OBJECT_0)
	{// Error
		CloseHandle(procInfo.hProcess);
		CloseHandle(procInfo.hThread);
		if (!suppress_error)
			err_out("E_PRTIMOUT",cmd);
		return 0;
	}

	help=GetExitCodeProcess(procInfo.hProcess,(unsigned long *)retval);	
	ASSERT(help);
	CloseHandle(procInfo.hProcess);
	CloseHandle(procInfo.hThread);

	if (*retval)
	{
		if (*retval>RETVAL_BASE)	// EXECTOOL ERROR ?
			tosser_add_listbox(L("S_232",*retval));
		else
			tosser_add_listbox(L("S_366",*retval));
		return 0;
	}
	return 1;
}

// =======================================================
	void load_editor_cfg(void)
// =======================================================
// this function loads editors config into global struct
//
{
	gc.autoscroll			=get_cfg(CFG_EDIT,"Autoscroll",1);
	gc.structured_display	=get_cfg(CFG_EDIT,"StructDisplay",0);
	gc.display_attributes	=get_cfg(CFG_EDIT,"ShowAttributes",1);
	gc.first_non_read		=get_cfg(CFG_EDIT,"FirstNonRead",1);
	gc.golded_compatible	=get_cfg(CFG_EDIT,"GoldedComp",1);
	gc.hide_leading_blank	=get_cfg(CFG_EDIT,"HideLeading",1);
	gc.hide_deleted		=get_cfg(CFG_EDIT,"HideDeleted",1);
	gc.show_seenby			=!get_cfg(CFG_EDIT,"HideSeenBy",1);
	gc.show_tags			=!get_cfg(CFG_EDIT,"HideTags",1);
	gc.immediate_update	=get_cfg(CFG_EDIT,"ImmUpdate",1);
	gc.colored				=get_cfg(CFG_EDIT,"Colored",1);
	gc.altcolored			=get_cfg(CFG_EDIT,"AltColored",1);
	gc.clear_tearline		=get_cfg(CFG_EDIT,"ClearTearline",0);
	gc.MaxUULines=get_cfg(CFG_EDIT,"MaxUueLines",340);
	if (gc.MaxUULines<10 || gc.MaxUULines>1000)
		gc.MaxUULines=340;
	gc.EditorWidth=get_cfg(CFG_EDIT,"EditorWidth",80);
	if (gc.EditorWidth<20 || gc.EditorWidth>100)
		gc.EditorWidth=80;
}

// ======================================================================
	void test_sound(LPCSTR soundname)
// ======================================================================
{
   	PlaySound(soundname,0,SND_ASYNC);
}

// ======================================================================
	void stop_test_sound(void)
// ======================================================================
{
	PlaySound(0,0,SND_ASYNC);
}

// ======================================================================
	int system_sound(LPSTR soundid,int forcesync)
// ======================================================================
{
int		i;
char	buf[2000];
char	*p;

   for (i=0;i<SoundLst.GetCount();i++)
   {
	  strcpy(buf,SoundLst.GetString(i));
	  p=strchr(buf,'\t');
	  if (!strncmp(buf,soundid,p-buf))
	  {
 		 if (!strcmp(p+1,"NONE") || !strcmp(p+1,"Не задано"))	// no sound
			return 1;

		 if (!strcmp(p+1,"STOP"))	    // stop sound
		 {
			 PlaySound(0,0,SND_ASYNC);
			 return 1;
		 }

		 if (stricmp(buf,"APPLICATION_END") || forcesync)
			 PlaySound(p+1,0,SND_SYNC);
		 else
			 PlaySound(p+1,0,SND_ASYNC);
		 return 1;
	  }
   }
   return 0;
}

// ========================================================================================
// search pattern in text
	int find_next_entry(LPCSTR text,int casesens,LPCSTR pattern,LPINT offsets,int maxret,int userconv,BOOL charset)
// ========================================================================================
// returns number of matches, offsets[] contains offsets of the matches
// wildcard '?' may be used in pattern, charset of text:  0 - OEM, 1 - ANSI
{
CString	patrn;
int			plen,slen,found=0;
char		*pe,*pp;
LPCSTR	se,ps,ptextin;
char		cs[]="\0\0";
char    cp[]="\0\0";

	slen=strlen(text);
	plen=strlen(pattern);
	if (slen==0 || plen==0 || slen<plen)	return 0;
	patrn=pattern;
	if (LANG_ID==LANG_RUS && userconv)
	{
		patrn.Replace('Н','H');
		patrn.Replace('р','p');
	}
	se=text+slen;		// End of Text
	pe=PS patrn+plen;	// End of Pattern
	ps=text;

next_char:
	while (ps<se)
	{
		ptextin=ps;
		pp=PS patrn;
		while (pp<pe)
		{
			*cs=*ps;
			*cp=*pp;
			if (*cp=='?')
			{
				ps++;
				pp++;
				continue;
			}
			if (!charset)	OemToChar(cs,cs);
			if (!casesens)	
			{  // bugfixing no-case comparing for Russian language
				*cs |= 0x20;
				*cp |= 0x20;
			}
			if (*cs==*cp)
			{
				ps++;
				pp++;
				continue;
			}
			else
			{
				ps=ptextin+1;
				goto next_char;
			}
		}
		found++;
		*offsets++=ptextin-text;
		if (found >= maxret)	return found;
	}
	return found;
}

// =============================================================
	int unix_time_to_fido(time_t unixtime,char *result)
// =============================================================
{
char   names[12][4]={"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
struct tm *tx;

	tx=localtime(&unixtime);

	if (tx->tm_year > 99)
		tx->tm_year -= 100;

	sprintf(result,"%02d %s %02d  %02d:%02d:%02d",
		tx->tm_mday,names[tx->tm_mon],tx->tm_year,tx->tm_hour,tx->tm_min,tx->tm_sec);
	return 1;
}

// =============================================================
	void kill_tabs(char *text)
// =============================================================
{
char *p;

   p=text;

   while (*p)
   {
	  if (*p=='\t')
	     *p=' ';
	  p++;
   }
}

// =============================================================
	int play_sound_file(char *filename)
// =============================================================
{
   PlaySound(filename,0,SND_SYNC);
   return 1;
}


/*/ ===========================================================
	int dbout(char *buf)
// ===========================================================
//	Diese Funktion oeffnet ein Console Window und schreibt den Inhalt von 'buf' raus
//  falls sich im Verzeichniss eine Date 'debug.on' befindet..
//  Falls sich auc noch eine Datei namens 'write.on' im Verzeichniss
//	befindet wird das LOG in die Datei 'fips.deb' mitgeschrieben
//  Die HeaderDatei <io.h> muss included sein...
{
// Prototypes, falls die entsprechenden headers nicht included sind ...
int			ret;
static int	first=1;
static int	write=0;
static		HANDLE hcon=INVALID_HANDLE_VALUE;
DWORD		written;
COORD		cord;
char		line[1000];
static   	FILE  *fp=NULL;

	if (first)
	{
		first=0;
		if (access("debug.on",0)==0)
		{
			if (access("write.on",0)==0)
			{
			   write=1;
			   fp=fopen("fips.deb","at");
		       fprintf(fp,"----------- New Log ------------\n");
			}

			AllocConsole();
			hcon=GetStdHandle(STD_OUTPUT_HANDLE);
			cord.X=80;
			cord.Y=2000;
			if (hcon!=INVALID_HANDLE_VALUE)
				SetConsoleScreenBufferSize(hcon,cord);
			else
				return 0;

		}
	}
	if (hcon!=INVALID_HANDLE_VALUE)
	{
		_strtime(line);
		strcat(line," >");
		strcat(line,buf);

		ret=WriteConsole(
						    hcon,			// handle of a console screen buffer
						    line,			// address of buffer to write from
						    strlen(line),	// number of characters to write
						    &written,		// address of number of characters written
						    NULL 			// reserved
					   );
	   if (write)
	      fprintf(fp,"%s",line);			// No close, for speed!

	}
	return 1;
}
*/
// ===========================================
	void restore_base_path(void)
// ===========================================
{
 	SetCurrentDirectory(gc.BasePath);
}

// ===========================================
	void load_language(void)
// ===========================================
{
CString sect;
FILE	*fp;
char	buf[400];
	
	switch (LANG_ID)
	{
	case LANG_GER:
		sect="@SECTION GERMAN";
		break;
	case LANG_RUS:
		sect="@SECTION RUSSIAN";
		break;
	case LANG_FRE:
		sect="@SECTION FRENCH";
		break;
	default:
	case LANG_ENG:
		sect="@SECTION ENGLISH";
		break;
	}

// dialogs
	fp=fopen(FIPSDIALOGS,"rt"); if (!fp) ERR_MSG2_RET("E_CANLOLANF",FIPSDIALOGS);
	LangDlgList.RemoveAll();
	*buf=0;
	while (fgets(buf,399,fp))
	{
		trim_all(buf);
		if (*buf==0 || *buf==';' ||	strnicmp(buf,sect,strlen(sect)))	continue;
		while (fgets(buf,399,fp))
		{
			trim_all(buf);
			if (*buf==0 || *buf==';')			continue;
			if (!strnicmp(buf,"@section ",9))	goto end_dl;	// end scanning
			if (*buf=='$')
				LangDlgList.AddTail(buf+1);		// new dialog
			else
				LangDlgList.AddTail(buf);		// new element
		}
	}
end_dl:
	fclose(fp);
	
// strings
	fp=fopen(FIPSSTRINGS,"rt"); if (!fp) ERR_MSG2_RET("E_CANLOLANF",FIPSSTRINGS);
	LangStrList.RemoveAll();
	*buf=0;
	while (fgets(buf,399,fp))
	{
		trim_all(buf);
		if (*buf==0 || *buf==';' ||	strnicmp(buf,sect,strlen(sect)))	continue;
		while (fgets(buf,399,fp))
		{
			trim_all(buf);
			if (*buf==0 || *buf==';')			continue;
			if (!strnicmp(buf,"@section ",9))	goto end_sl;	// end scanning
			LangStrList.AddTail(buf);
		}
	}
end_sl:
	fclose(fp);
}
	
// ===========================================
	void set_language(void)
// ===========================================
{
CString str;
	
	str=get_cfg(CFG_COMMON,"Language","English");
	if (str=="English")			LANG_ID=LANG_ENG;
	else if (str=="German")		LANG_ID=LANG_GER;
	else if (str=="Russian")	LANG_ID=LANG_RUS;
	else if (str=="French")		LANG_ID=LANG_FRE;
	
	gpMain->SetNewMenu();
	switch(LANG_ID) 
	{
	case LANG_GER:
		(gpMain->m_menu).CheckMenuItem(ID_SET_GER, MF_BYCOMMAND | MF_CHECKED);
		break;
	case LANG_RUS:
		(gpMain->m_menu).CheckMenuItem(ID_SET_RUS, MF_BYCOMMAND | MF_CHECKED);
		break;
	case LANG_FRE:
		(gpMain->m_menu).CheckMenuItem(ID_SET_FRE, MF_BYCOMMAND | MF_CHECKED);
		break;
	default:
	case LANG_ENG:
		(gpMain->m_menu).CheckMenuItem(ID_SET_ENG, MF_BYCOMMAND | MF_CHECKED);
		break;
	}
	load_language();
	gpMain->SetLanguage();
	gMailer.SetLanguage();
	gTosser.SetLanguage();
	gPurger.SetLanguage();
	gNLComp.SetLanguage();
	gSearch.SetLanguage();
	if (!gc.NoTooltips) 
	{
		gpMain->RefreshTips();
		gMailer.RefreshTips();
		gFloating.RefreshTips();
	}
}
	
// ================================================
    void set_dlg_language(CDialog *cw,LPCSTR id,int *ids,int count)
// ================================================
{
CString str;
int		k;

	k=LangDlgList.FindString(id,str);
	if (k!=LB_ERR)
	{
		for (int i=0;i<=count;i++)
		{
			str=LangDlgList.GetString(++k);
			trim_all(str);
			if (str.GetLength()) 
			{
				str.Replace("\\","\r\n");
				if (i==0)
					cw->SetWindowText(str);
				else
				{
					(cw->GetDlgItem(*ids))->SetWindowText(str);
					ids++;
				}
			}
		}
	}
}

// =================================================================
	void create_dirs_always(void)
// =================================================================
{
CString path;

	path=get_cfg(CFG_COMMON,"InboundPath","");
    if (access(path,0))
    	create_path(path);
	path=get_cfg(CFG_COMMON,"OutboundPath","");
    if (access(path,0))
    	create_path(path);
	path=get_cfg(CFG_COMMON,"UtilPath","");
    if (access(path,0))
    	create_path(path);
	path=get_cfg(CFG_COMMON,"NodelistPath","");
    if (access(path,0))
    	create_path(path);
	path=get_cfg(CFG_COMMON,"MsgBasePath","");
    if (access(path,0))
    	create_path(path);
	path=get_cfg(CFG_COMMON,"MultimedPath","");
    if (access(path,0))
    	create_path(path);
}

// =================================================================
	int replace_phone_number(CString &number)
// =================================================================
{
CString num,trans,buf;

	for (int i=0;i<teltrans.GetCount();i++)
	{
		buf=teltrans.GetString(i);
		if (strlen(buf)<1)
		   continue;

		get_token(buf,0,num);
		if (!strncmp(number,num,strlen(num)))
		{
			get_token(buf,1,trans);
 			trans+=number.Mid(strlen(num));
			number=trans;
			return 1;
		}
	}
	return 0;
}

// =================================================================
	void ScriptUudecode (void)
// =================================================================
{
char line[1000];

	CUUDEC uue(gpMain->m_hWnd);
	uue.Reset();
	for (int i=0;i<actdisplist.GetCount();i++)
	{
		strnzcpy(line,actdisplist.GetString(i),1000);
	  uue.DecodeLine(line);
	}
	show_msg(L("S_176"));	// Done
}

// ===========================================================================
	int build_routed_netmail(LPCSTR from_name,LPCSTR from_fido,LPCSTR to_name,LPCSTR to_fido,
													LPCSTR subject,LPCSTR mailtext,int crash,int recret,int local_only)
// ===========================================================================
{
mailheader mh;
CString		 mailout,INTL,FMPT,TOPT,MSGID,PID,tearline;
char			 buf[1000],text[MAX_MSG];

	gpMain->BeginWaitCursor();
	memset(&mh,0,sizeof(mailheader));
	mh.structlen=sizeof(mailheader);
	mh.status=DB_MAIL_CREATED;
	CharToOem(subject,mh.subject);
	strcpy(mh.datetime,build_fido_time(buf));
	CharToOem(to_name,mh.toname);
	CharToOem(from_name,mh.fromname);
	mh.recipttime=mh.mailid=time(NULL);
	CharToOem(mailtext,text);

	if (crash)			mh.attrib |= MSGCRASH;
	if (recret)			mh.attrib |= MSGCPT;
	if (local_only)	mh.status |= DB_MAIL_SCANNED;

	parse_address(from_fido,&mh.srczone,&mh.srcnet,&mh.srcnode,&mh.srcpoint);
	parse_address(to_fido,&mh.dstzone,&mh.dstnet,&mh.dstnode,&mh.dstpoint);

	INTL.Format("\001INTL %d:%d/%d %d:%d/%d\r",mh.dstzone,mh.dstnet,mh.dstnode,mh.srczone,
		mh.srcnet,mh.srcnode);
	FMPT.Empty();
	if (mh.srcpoint)	FMPT.Format	("\001FMPT %d\r",mh.srcpoint);
	TOPT.Empty();
	if (mh.dstpoint)	TOPT.Format	("\001TOPT %d\r",mh.dstpoint);
	PID.Format("\001PID: %s\r",get_versioninfo(buf,0));
	MSGID.Format("\001MSGID: %d:%d/%d.%d %x\r",mh.srczone,mh.srcnet,mh.srcnode,mh.srcpoint,mh.mailid);

	mailout=INTL+FMPT+TOPT+MSGID+PID+"\r"+text+get_tearline(tearline,TRUE)+"\r";
	expand_tabs(mailout);
	mh.text_length=mailout.GetLength()+1;
	mh.mail_text=PS mailout;

	if (db_append_new_message(0,&mh) != DB_OK)
	{
	   gpMain->EndWaitCursor();
	   ERR_MSG_RET0("E_APPMFAILED");
	}

	if (!local_only && db_set_area_state(0,1) != DB_OK)
	{
		gpMain->EndWaitCursor();
		ERR_MSG_RET0("E_MODFLAGSET");
	}
	gpMain->EndWaitCursor();
	return 1;
}

// ===================================================================
// inserts 'text' into 'index' tabbed position in 'str'
	void set_tabbed_string(CString &str,int index,LPCSTR text)
// ===================================================================
{
int  len=0,cnt=0;
char buf[30000];
LPCSTR p;

	*buf=0;
	p=str;
	while (*p && cnt < index)
	{
	  if (*p=='\t')	cnt++;
		if (cnt==index)	break;
		p++;
		len++;
	}
  if (cnt < index)
	{
  	while (cnt<index)
		{
			  str+="\t";
			  len++;
			  cnt++;
		}
	}
	if (index > 0)
	{
		strncpy(buf,str,len+1);
		buf[len+1]=0;
	}
	strcat(buf,text);
	p=str;
	cnt=0;
	index++;
	while (*p && cnt < index)
	{
	  if (*p=='\t')	cnt++;
		if (cnt==index)	break;
		p++;
	}
	strcat(buf,*p==0 ? "\t" : p);
	str=buf;
}

// ===================================================================
	void load_modem_cfg(int num,_modemcfg *pmcfg)
// ===================================================================
// 1 = first modem
// 2 = second modem
// if pmcfg==0 then loading to global modem cfg, otherwise to pmcfg
{
_modemcfg *pm;
CString line;
char	str[20];

	pm=pmcfg ? pmcfg : &gModemCfg;
	memset(pm,0,sizeof(pm));
	pm->m_comport=1;
	pm->m_baud=57600;
	pm->m_data=8;
	pm->m_stop=1;
	pm->m_parity='N';
	pm->m_flow=SIGNAL_CTS;// SIGNAL_XON
	sprintf(str,"M%dPort",num);
	if (num==1)
		line=get_cfg(CFG_HWARE,str,"1,57600,8,1,N,16");
	else
		line=get_cfg(CFG_HWARE,str,"0,57600,8,1,N,16");
	sscanf(line,"%d,%d,%d,%d,%c,%d",&(pm->m_comport),&(pm->m_baud),
		&(pm->m_data),&(pm->m_stop),&(pm->m_parity),&(pm->m_flow));
	sprintf(str,"M%dUseInit1",num);
	pm->m_useinit1=get_cfg(CFG_HWARE,str,1);
	sprintf(str,"M%dInit1",num);
	strcpy(pm->m_init1,get_cfg(CFG_HWARE,str,"ATZ"));
	sprintf(str,"M%dUseInit2",num);
	pm->m_useinit2=get_cfg(CFG_HWARE,str,1);
	sprintf(str,"M%dInit2",num);
	strcpy(pm->m_init2,get_cfg(CFG_HWARE,str,""));
	sprintf(str,"M%dUseInit3",num);
	pm->m_useinit3=get_cfg(CFG_HWARE,str,1);
	sprintf(str,"M%dInit3",num);
	strcpy(pm->m_init3,get_cfg(CFG_HWARE,str,""));
	sprintf(str,"M%dDial",num);
	strcpy(pm->m_dial,get_cfg(CFG_HWARE,str,"ATDP"));
	sprintf(str,"M%dAnswer",num);
	strcpy(pm->m_answer,get_cfg(CFG_HWARE,str,"ATA"));
	sprintf(str,"M%dUseCmd1",num);
	pm->m_usecmd1=get_cfg(CFG_HWARE,str,0);
	sprintf(str,"M%dPredial1",num);
	strcpy(pm->m_predial1,get_cfg(CFG_HWARE,str,""));
	sprintf(str,"M%dUseCmd2",num);
	pm->m_usecmd2=get_cfg(CFG_HWARE,str,0);
	sprintf(str,"M%dPredial2",num);
	strcpy(pm->m_predial2,get_cfg(CFG_HWARE,str,""));
	sprintf(str,"M%dUseCmd3",num);
	pm->m_usecmd3=get_cfg(CFG_HWARE,str,0);
	sprintf(str,"M%dPredial3",num);
	strcpy(pm->m_predial3,get_cfg(CFG_HWARE,str,""));
}

	void save_modem_cfg(int num,_modemcfg *pmcfg)
{
_modemcfg *pm;
char str[20],str1[100];

	pm=pmcfg ? pmcfg : &gModemCfg;
	sprintf(str,"M%dPort",num);
	sprintf(str1,"%d,%d,%d,%d,%c,%d",pm->m_comport,pm->m_baud,
		pm->m_data,pm->m_stop,pm->m_parity,pm->m_flow);
	set_cfg(CFG_HWARE,str,str1);
	sprintf(str,"M%dUseInit1",num);
	set_cfg(CFG_HWARE,str,pm->m_useinit1);
	sprintf(str,"M%dInit1",num);
	set_cfg(CFG_HWARE,str,pm->m_init1);
	sprintf(str,"M%dUseInit2",num);
	set_cfg(CFG_HWARE,str,pm->m_useinit2);
	sprintf(str,"M%dInit2",num);
	set_cfg(CFG_HWARE,str,pm->m_init2);
	sprintf(str,"M%dUseInit3",num);
	set_cfg(CFG_HWARE,str,pm->m_useinit3);
	sprintf(str,"M%dInit3",num);
	set_cfg(CFG_HWARE,str,pm->m_init3);
	sprintf(str,"M%dDial",num);
	set_cfg(CFG_HWARE,str,pm->m_dial);
	sprintf(str,"M%dAnswer",num);
	set_cfg(CFG_HWARE,str,pm->m_answer);
	sprintf(str,"M%dUseCmd1",num);
	set_cfg(CFG_HWARE,str,pm->m_usecmd1);
	sprintf(str,"M%dPredial1",num);
	set_cfg(CFG_HWARE,str,pm->m_predial1);
	sprintf(str,"M%dUseCmd2",num);
	set_cfg(CFG_HWARE,str,pm->m_usecmd2);
	sprintf(str,"M%dPredial2",num);
	set_cfg(CFG_HWARE,str,pm->m_predial2);
	sprintf(str,"M%dUseCmd3",num);
	set_cfg(CFG_HWARE,str,pm->m_usecmd3);
	sprintf(str,"M%dPredial3",num);
	set_cfg(CFG_HWARE,str,pm->m_predial3);
}

// ===================================================================
	int test_Notbremse (void)
// ===================================================================
{
CString help;
ULONG tvalue;
int counter=0,secs=0,maxtry=0,backcounter=0,badcounter=0,i;
long    jetzt,diff;

// check emergense break
	if (!dialbrk.defaultindex)	return 1;
	if (gc.NotBremseOn)	return 0;
  	jetzt=time(NULL);
	help=dialbrk.GetString(1);
	sscanf(help,"%d",&secs);

	help=dialbrk.GetString(0);
	sscanf(help,"%d",&maxtry);

	for (i=notbremscounter-1;i>=0;i--)
	{
	   tvalue=*(notbremse+i);
	   diff=abs((int)difftime(jetzt,tvalue));

	   if (diff < secs)
	  	  badcounter++;
	   else
		  return 1;

	   if (badcounter >=maxtry)
	   {
		  gc.NotBremseOn=1;
		  return 0;
	   }

	}
    return 1;
}

// ===================================================================
	int reset_Notbremse (void)
// ===================================================================
{
	if (notbremse)
	{
	 	free(notbremse);
		notbremse=0;
	}
	notbremscounter=0;
	gc.NotBremseOn=0;
	return 1;
}

// ===================================================================
	int is_excluded_area(LPCSTR line,LPCSTR areaname)
// ===================================================================
{
CString	 excludes,oneexcl;
CStrList excllist;
char 	 lwrarea[300],lwrexcl[3000],*p;
int 	 i;

	get_token(line,3,excludes);
	strcpy(lwrarea,areaname);
	strcpy(lwrexcl,excludes);
	strlwr(lwrarea);
	strlwr(lwrexcl);

	p=strstr(lwrexcl,lwrarea);	if (!p)	return 0;
	excllist.SplitWords(line);
	for (i=0;i<excllist.GetCount();i++)
	{
	 	oneexcl=excllist.GetString(i);
	 	if (!stricmp(oneexcl,areaname))
	 		return 1;
	}
	return 0;
}

// ==================================================================
	int show_first_selected(void)
// ==================================================================
{
	disable_multimedia(TRUE);
    count_of_selected=gpMain->m_subjlist.GetSelItems(MAX_SELMAILS-1,selected_entries);
	current_selected=0;
	return show_next_selected();
}

// ==================================================================
	int show_next_selected(void)
// ==================================================================
{
	if (current_selected >= count_of_selected)
	{
		disable_multimedia(FALSE);
		return 0;
	}
	gpMain->m_subjlist.SetSel(selected_entries[current_selected],1);
	gpMain->show_mail(&gustat);
	current_selected++;
	return 1;
}

// ==================================================================
	int show_msg_t(LPCSTR msgid,int defbut,int delay,LPCSTR pushYES,LPCSTR pushNO)
// ==================================================================
{
	CTimed tmp;
	strcpy(tmp.but1,pushYES);	// Caption of Pushbutton = 1
	strcpy(tmp.but2,pushNO);	// Caption of Pushbutton = 2
	strcpy(tmp.msgid,msgid);	// Message ID in fips.msg file
	tmp.defbut=defbut;			// Default Button
	tmp.delay=delay;			// Wait time
	tmp.DoModal();
	return tmp.retval;
}

// ==================================================================
// checks for dupes in one area
	int check_dupes_for_area(int area_handle)
// ==================================================================
{
mailheader *tp,*mh=0;
mailheader *tph[40000];
int		i,k,kk,cnt=0,count=0;

	if (gc.purger.running)	return 0;
	gc.purger.running=1;
	if (db_getallheaders(area_handle,&mh,&cnt)==DB_OK)
	{
		if (cnt >39990)			cnt=39990;
		tp=mh;
		for (i=0;i<cnt;i++)
		{
			tph[i]=tp;
			tp++;
		}
// sort on MSGID
		qsort(tph,cnt,sizeof(mailheader *),SortMailHeaderForDupeCheck);
// skip starting deleted mails
		for (k=0;k<cnt;k++)
			if (!(tph[k]->status & (/*DB_DUPE_MAIL |*/ DB_DELETED)))	break;

		for (i=k+1;i<cnt;i++)
		{
// dupes condition: equal MSGID,FROM,TO,SUBJECT
			if (tph[i]->status & (/*DB_DUPE_MAIL |*/ DB_DELETED))		continue;
			if (tph[i]->mailid!=tph[k]->mailid)	// next MSGID
			{
				for (kk=k+1;kk<=i;kk++)	// look for non-deleted mail
					if (!(tph[kk]->status & DB_DELETED))				break;
				k=kk;
				i=k;
				continue;
			}
			if (strcmp(tph[i]->fromname,tph[k]->fromname) ||
				strcmp(tph[i]->toname,tph[k]->toname) ||
				strcmp(tph[i]->subject,tph[k]->subject))
					continue;	// look for next mail with different FROM,TO,SUBJECT
			if (tph[i]->status & DB_NEVER_DELETE)
			{
				tph[k]->status |= DB_DELETED /*| DB_DUPE_MAIL*/ | DB_MAIL_READ;
				k=i;
			}
			else
				tph[i]->status |= DB_DELETED /*| DB_DUPE_MAIL*/ | DB_MAIL_READ;
			count++;
		}

		if (db_set_all_headers(area_handle,mh,cnt)!=DB_OK)
		{
			err_out("E_WRITEAHFAI");
			goto free_all;
		}

	}

free_all:
	if (mh)
	{
		free(mh);
		mh=0;
	}
	gc.purger.running=0;
	return count;
}

//===================================================
	int SortMailHeaderForDupeCheck (const void *a1,const void *a2)
//===================================================
{
mailheader *p1,*p2;
mailheader **p1h,**p2h;

	p1h=(mailheader **)a1;
	p2h=(mailheader **)a2;

	p1=*p1h;
	p2=*p2h;

	if (p1->mailid < p2->mailid)
		return -1;

  	if (p1->mailid > p2->mailid)
		return 1;

	return 0;
}

//===================================================
	void initialize_jokes_list(void)
//===================================================
{
	jokes.RemoveAll();
	jokes.AddTail("OS/2\tEqual Win*2");
	jokes.AddTail("Hornet\tHornet against Phoenix? Curious...");
	jokes.AddTail("Beer\tFIDO without beer = Internet");
	jokes.AddTail("Windows XP\tRulez forever!");
	jokes.AddTail("Delphi\tLanguage for weaklings!");
	jokes.AddTail("Duke\tNukem? Must Die!");
	jokes.AddTail("Doom\tGot you !");
	jokes.AddTail("NFS\tGood game, bad filesystem!");
	jokes.AddTail("Quake\tFrogs do it in the night");
	jokes.AddTail("C&C\tYes Sir!");
	jokes.AddTail("Phoenix\tIt's only a legend...");
	jokes.AddTail("USR\tIt's a modem, NOT A TRAIN !");
	jokes.AddTail("Piter\tIt's a Real Capital!");
	jokes.AddTail("Star Trek\tScotch me up beamy !");

}

//===================================================
	int	get_devices_listen_for_call(int &devices,int &services)
//===================================================
// TOP: Diese Funktion gibt alle Devices zurueck die aktuell auf einen
// Call lauern sollen ...
// devices are Bitcoding:
// BIT 0 = first Modem
// BIT 1 = second Modem
// BIT 2 = ISDN (CAPI )Line

// Services are Bitcoding:
// BIT 0 = EMSI
// BIT 1 = MAILBOX
// BIT 2 = FAX
// BIT 3 = VOICE
{
CString	line,help;
time_t	unixtime;
tm		*tx;
int     startmul,stopmul,actmul,start_hr,start_min,stop_hr,stop_min,erg,i;
int		ret_val=0;
static  int DoesKeyAllowIncome=-1;

	devices=services=0;

	if (DoesKeyAllowIncome==-1)
			DoesKeyAllowIncome=1;

	if (!DoesKeyAllowIncome)
		return 1;


	if (income_times.defaultindex)		// All income-times disabled ?
	{
		gcomm.AcceptOn1Modem=gcomm.AcceptOn2Modem=gcomm.AcceptOnIsdn=0;
		return 1;
	}
	for (i=0;i<income_times.GetCount();i++)
	{
		line=income_times.GetString(i);

		// Als erstes testen wir den Wochentag
		unixtime=time(NULL);
		tx=localtime(&unixtime);
		switch (tx->tm_wday)
		{

			   case 0:
							get_token(PS line,IDINCCm_su,help);
							if (help!="1") continue; break;
			   case 1:
							get_token(PS line,IDINCCm_mo,help);
							if (help!="1") continue; break;
			   case 2:
							get_token(PS line,IDINCCm_tu,help);
							if (help!="1") continue; break;
			   case 3:
							get_token(PS line,IDINCCm_we,help);
							if (help!="1") continue; break;
			   case 4:
							get_token(PS line,IDINCCm_th,help);
							if (help!="1") continue; break;
			   case 5:
							get_token(PS line,IDINCCm_fr,help);
							if (help!="1") continue; break;
			   case 6:
							get_token(PS line,IDINCCm_sa,help);
							if (help!="1") continue; break;	// Sonntag ..
		}

		get_token(line,IDINCCm_edit_from,help);
		sscanf(help,"%d:%d",&start_hr,&start_min);
		get_token(line,IDINCCm_edit_to,help);
		sscanf(help,"%d:%d",&stop_hr, &stop_min);

		// |<-------------------- x=start_mul
		// |<---------------------------------------- x=act_mul
		// |<------------------------------------------------------------x=stop_mul

		startmul=(start_hr	* 60)	+start_min;
		stopmul =(stop_hr 	* 60)	+stop_min;
		actmul  =(tx->tm_hour*60) 	+tx->tm_min;

		erg=0;
		if (startmul <= stopmul)   // Normalfall
		{
			if ((actmul >= startmul) && (actmul < stopmul))
				erg=1;
			else
				erg=0;
		}
		else		  				// Event geht ueber Mitternacht ...
		{
			if ((actmul <= stopmul) || (actmul >= startmul))
				erg=1;
			else
				erg=0;
		}
		if (erg==1)
		{
			// FIRSTMODEM
			get_token(line,IDINCCm_firstmodem,help);
			if (help=="1")
			{
				devices|=1;
				get_token(line,IDINCCm_service_emsi,help);
				if (help=="1") 	services|=1;
				get_token(line,IDINCCm_service_mailbox,help);
				if (help=="1") 	services|=2;
				get_token(line,IDINCCm_service_fax,help);
				if (help=="1") 	services|=4;
//				get_token(line,IDINCCm_service_voice,help);
//				if (help=="1") 	services|=8;
			}

			// SECONDMODEM
			get_token(line,IDINCCm_secondmodem,help);
			if (help=="1")
			{
				devices|=2;
				get_token(line,IDINCCm_service_emsi,help);
				if (help=="1") 	services|=1;
				get_token(line,IDINCCm_service_mailbox,help);
				if (help=="1") 	services|=2;
				get_token(line,IDINCCm_service_fax,help);
				if (help=="1") 	services|=4;
//				get_token(line,IDINCCm_service_voice,help);
//				if (help=="1") 	services|=8;
			}

			// ISDNLINE
			get_token(line,IDINCCm_isdnline,help);
			if (help=="1")
			{
				devices|=4;
				get_token(line,IDINCCm_service_emsi,help);
				if (help=="1") 	services|=1;
				get_token(line,IDINCCm_service_mailbox,help);
				if (help=="1") 	services|=2;
				get_token(line,IDINCCm_service_emsi,help);
				if (help=="1") 	services|=4;
//				get_token(line,IDINCCm_service_voice,help);
//				if (help=="1") 	services|=8;
			}
		}
	}

	if (devices & 1)
		gcomm.AcceptOn1Modem=1;
	else
		gcomm.AcceptOn1Modem=0;

	if (devices & 2)
		gcomm.AcceptOn2Modem=1;
	else
		gcomm.AcceptOn2Modem=0;

	if (devices & 4)
		gcomm.AcceptOnIsdn=1;
	else
		gcomm.AcceptOnIsdn=0;

	return 1;
}

//===================================================
	int	match_number_of_rings(int port)
//===================================================
// Port=0 -> first  Modem
// Port=1 -> second Modem
// Port=2 -> ISDN   Port
{
static int	callcounter[3]	={0,0,0};
static long	lastcalltimes[3]={0,0,0};
CString		line,help;
int			count,blanktime;
long		now;

	ASSERT(port>=0 && port<=2);
	switch (port)
	{
		case 0:
			line=get_cfg(CFG_INCOMIN,"Modem1","3,10");
		case 1:
			line=get_cfg(CFG_INCOMIN,"Modem2","3,10");
		case 2:
			line=get_cfg(CFG_INCOMIN,"ISDN","3,10");
	}
	count=blanktime=0;
	sscanf(line,"%d,%d",&count,&blanktime);
	now=time(NULL);

	if ((now-lastcalltimes[port]) > blanktime)
	{
		callcounter[port]=1;
		lastcalltimes[port]=now;
		if(callcounter[port]>=count)
		{
			callcounter[port]=0;
			lastcalltimes[port]=0;
			return 1;
		}
	}
	else
	{
		callcounter[port]++;
		lastcalltimes[port]=now;
		if(callcounter[port]>=count)
		{
			callcounter[port]=0;
			lastcalltimes[port]=0;
			return 1;
		}
	}
	return 0;
}

//===================================================
	void check_udef_existence (void)
//===================================================
{
areadef adef;
char	buf[300];
int		notread=0;

	if (UserdefAreaList.GetCount()==0)
	{
		for (int i=0;;i++)
		{
			if (db_get_area_by_index(i,&adef)!=DB_OK)
			   break;
			notread=adef.number_of_mails-adef.number_of_read;
			if (notread<0)
		   		notread=0;
			sprintf(buf,"%s\t%d\t%d\t%s",adef.echotag,adef.number_of_mails,notread,adef.description);
			UserdefAreaList.AddTail(buf);
		}
	}
}

//===================================================
	int	get_next_area(int direction)
//===================================================
{
areadef ad;
CString str;
int		ind;
char    buf[100];

	if (db_get_area_by_index(gustat.cur_area_handle,&ad)!=DB_OK)
		return -1;

	ind=UserdefAreaList.FindString(ad.echotag,str);
	if (ind==-1)
		return -1;

next:
	if (direction<0)
		ind--;
	if (direction>0)
		ind++;

	if (ind<0 || ind>=UserdefAreaList.GetCount())
		return -1;

	str=UserdefAreaList.GetString(ind);

	if (*str==';')
		goto next;

	get_first_value(str,buf);

	return db_get_area_by_name(buf);
}

//===================================================
	void get_first_value(const char *source,char *ua)
//===================================================
{
CString str;

	get_token(source,0,str);
	strcpy(ua,str);
}

//===================================================
	int handle_ingate_translation(CString &to,CString &fido,CString &subject,CString &aka,CString &mailtext,CString &inaddr)
//===================================================
{
CString c_to;
CString c_fido;
CString c_subject;
CString c_aka;
CString c_mailtext;
CString c_inaddr;
CString format;
int		ret;

	c_to		=to;
	c_fido		=fido;
	c_subject  =subject;
	c_aka		=aka;
	c_mailtext =mailtext;
	c_inaddr	=inaddr;

	if (gatecfg.GetCount()<5)
		ERR_MSG_RET0("E_INVALEGATEDEF");

	format=gatecfg.GetString(0);
	insert_LFCR(PS format);
	ret=interpret_format(to,format,c_to,c_fido,c_subject,c_aka,c_mailtext,c_inaddr);
	if (!ret)
		return 0;

	format=gatecfg.GetString(1);
	insert_LFCR(PS format);
	ret=interpret_format(fido,format,c_to,c_fido,c_subject,c_aka,c_mailtext,c_inaddr);
	if (!ret)
		return 0;

	format=gatecfg.GetString(2);
	insert_LFCR(PS format);
	ret=interpret_format(subject,format,c_to,c_fido,c_subject,c_aka,c_mailtext,c_inaddr);
	if (!ret)
		return 0;

	format=gatecfg.GetString(3);
	insert_LFCR(PS format);
	ret=interpret_format(aka,format,c_to,c_fido,c_subject,c_aka,c_mailtext,c_inaddr);
	if (!ret)
		return 0;

	format=gatecfg.GetString(4);
	insert_LFCR(PS format);
	ret=interpret_format(mailtext,format,c_to,c_fido,c_subject,c_aka,c_mailtext,c_inaddr);
	if (!ret)
		return 0;

	return 1;
}

//===================================================
	int interpret_format(	CString &tag,
							CString &format,
							CString &c_to,
							CString &c_fido,
							CString &c_subject,
							CString &c_aka,
							CString &c_mailtext,
							CString &c_inaddr)
//===================================================
{
char buf[1000],*formp,*expp;
int  c;

	 tag.Empty();
	 if (format=="EMPTY")
		return 1;
	 buf[0]=0;

	 formp =PS format;
	 expp  =buf;

	 c=*formp;
	 while (c)
	 {
		 if (c=='?')
		 {
			  switch (*(formp+1))
			  {
				   case 'E': // Empfaenger
							 strcpy(expp,c_to); 
							 expp+=c_to.GetLength()-1;
							 formp++;
							 break;
				   case 'F': // Empfaenger Fido
							 strcpy(expp,c_fido); 
							 expp+=c_fido.GetLength()-1;
							 formp++;
							 break;
				   case 'S': // Subject
							 strcpy(expp,c_subject); 
							 expp+=c_subject.GetLength()-1;
							 formp++;
							 break;
				   case 'A': // AKA
							 strcpy(expp,c_aka); 
							 expp+=c_aka.GetLength()-1;
							 formp++;
							 break;
				   case 'I': // InternetAdresse from 'to' or 'to_fido'
							 strcpy(expp,c_inaddr); 
							 expp+=c_inaddr.GetLength()-1;
							 formp++;
							 break;
			  }
		 }
		 else
			 *expp=*formp;								// Copy Byte

		 formp++;
		 expp++;
		 c=*formp;
	 }
	 *expp=0;
	 tag=buf;
	 return 1;
}


//===================================================
	int secure_delete(char *path)
//===================================================
{
	unlink(path);
	if (access(path,0)==0)
		ERR_MSG2_RET0("E_CANDCF",path);

	return 1;
}

//===================================================
    int rename_to_short_filenames(const char *path,CStrList &files,CStrList &renamelist)
//===================================================
{
CString dest,tmp,help1,help2;
FILE	*fp;
char	src[MAX_PATH],wpath[MAX_PATH];
int		i;

	make_path(wpath,path,"long2shr.inf");

	if (access(wpath,0)==0)
		ERR_MSG2_RET0("E_OLNGRFE",path);

	renamelist.RemoveAll();
	for (i=0;i<files.GetCount();i++)
	{
		help1=files.GetString(i);
		get_token(help1,0,help2);
		make_path(src,path,help2);
		dest.Format("%s\\%08d.lst",path,i);
		if (!access(dest,0))
			unlink(dest);
		rename(src,dest);
		tmp=src; 
		tmp+='\t'+dest;
		renamelist.AddTail(tmp);
	}

	fp=fopen(wpath,"wt");
	if (!fp)
		ERR_MSG2_RET0("E_EWWTFX",wpath);

	for (i=0;i<renamelist.GetCount();i++)
	{
		tmp=renamelist.GetString(i);
		fprintf(fp,"%s\n",tmp);
	}
	fclose(fp);
	return 1;
}

//===================================================
	int rename_back_to_long_filenames(char *path)
//===================================================
{
int		ret;
CString wpath;
FILE	*fp;
char	buf[500];
char    src[400];
char    dest[400];


	wpath=path;	wpath+="\\"; wpath+="long2shr.inf";

	if (access(PS wpath,0)!=0)
		return 1;				 // Keine INFO File da ...

	// Ansonsten lesen wir das rein ...
	fp=fopen(PS wpath,"rt");
	if (fp)
	{
		while (fgets(buf,499,fp))
		{
			ret=sscanf(buf,"%s %s",src,dest);
			if (ret!=2)
				continue;
			rename(dest,src);
		}
	}
	else
		return 0;

	fclose(fp);

	// Nun loeschen wir diese Datei ...
	unlink(wpath);
	if (access(wpath,0)==0)
		ERR_MSG2_RET0("E_CANDELFL",PS wpath);

	return 1;
}

//===================================================
	int handle_tcpip_load_error	(void)
//===================================================
{
	switch (gc.tcpipLoadError)
	{
		case 1:
				err_out("E_TCPIP_WSA");			// WSAStartup failed
				break;
		case 2:
				err_out("E_TCPIP_SOCKET");		// SOCKET failed
				break;

		case 3:
				err_out("E_TCPIP_SETSOCK");		// SETSOCKETOPTIONS failed
				break;

		case 4:
				err_out("E_TCPIP_BIND");		// BIND failed
				break;

		case 5:
				err_out("E_TCPIP_LOADLIB");		// LoadLibrary
				break;

  		case 6:
				err_out("E_TCPIP_DYNABIND");	// Dynabind
				break;
	}
	return 1;
}

#define RESULT_ERROR 1
#define RESULT_DONE  2
//===================================================
	void uudecode_from_file(char *filename)
//===================================================
// uudecode from temp file on disk
{
char aline[3000];
int  state=0;
FILE *fp=fopen(UUMULFILE,"rt");

	if (!fp)
		ERR_MSG2_RET("E_CANFUTFP",UUMULFILE)
	else
	{
		CUUDEC tmp(gpMain->m_hWnd);
		tmp.Reset();
		while (fgets(aline,2999,fp) && state!=RESULT_DONE && state!=RESULT_ERROR)
			state=tmp.DecodeLine(aline);
		fclose(fp);

		if (state==RESULT_DONE)
			show_msg(L("S_176"));
		else if (state==RESULT_ERROR)
			ERR_MSG_RET("E_INCDECD")
		else
			ERR_MSG_RET("E_UUEINCOMPLETE")
	}
}

// ===============================================
// temporary disable multimedia settings
	void disable_multimedia(BOOL save)
// ===============================================
{
static int mm_sound;
static int mm_bmps;
static int mm_no_error;

	if (save)
	{
		mm_sound=gc.sound_enabled;
		mm_bmps	=gc.bmps_enabled;
		mm_no_error=gc.no_error;
		gc.sound_enabled=0;
		gc.bmps_enabled=0;
		gc.no_error=1;

	}
	else
	{
		gc.sound_enabled=mm_sound;
		gc.bmps_enabled=mm_bmps;
		gc.no_error=mm_no_error;
	}
}

// ===============================================
	void handle_usermark_bit(int command)   // 0=clear 1=set -1=toggle
// ===============================================
{
int sel,ret,oldval,newval=0;

	if (!gustat.already_one_valid_mail)
		return;
    sel=(gpMain->m_subjlist).GetCurSel();
    LB_ERR_RET;

	if (!gc.immediate_update)
	   gpMain->force_current_mail_update();

	oldval=gustat.act_mailh.status & DB_USERMARKED;
	if ((command==0 && !oldval) || (command==1 && oldval))
	{
		set_usermark(sel,oldval);
		return;
	}
	switch (command)
	{
		case 0:				// clear
			gustat.act_mailh.status &= ~DB_USERMARKED;
			newval=0;
			break;
		case 1:				// set
			gustat.act_mailh.status |= DB_USERMARKED;
			newval=1;
			break;
		case -1:			// toggle
			gustat.act_mailh.status ^= DB_USERMARKED;
			newval=gustat.act_mailh.status & DB_USERMARKED;
			break;
	}

	ret=db_sethdr_by_index (gustat.cur_area_handle,gustat.act_mailh.index,&gustat.act_mailh);
	if (ret!=DB_OK)
		ERR_MSG2_RET("E_DBSETHDRBYINDEX",ret);

    set_usermark(sel,newval);
	gpMain->m_attrlist.Invalidate();
	gpMain->m_subjlist.SetFocus();
}

// ===========================================
   void set_usermark(int ind,int on)
// ===========================================
{
	if (ind >= AttrListBuffer.GetCount())	return;
	if (on)
		AttrListBuffer[ind] |= 0x80;
	else
		AttrListBuffer[ind] &= ~0x80;
}

// ===============================================
	int handle_function_key(MSG *pMsg)
// ===============================================
{
	if (pMsg->message==WM_KEYDOWN && pMsg->wParam>=VK_F2 && pMsg->wParam<=VK_F12)
	{
		gpMain->exec_script_key("Func",pMsg->wParam-VK_DIVIDE);
		return 1;
	}
	return 0;
}

// ===============================================================================
// append cost info to logfile
	int append_to_cost_file(struct _costinfo *cost)
// ===============================================================================
{
FILE	*fp;
char	tmp[MAX_PATH];

	if (!costuserlist.defaultindex)
		return 1;

	make_path(tmp,gc.BasePath,COSTFILENAME);
	fp=fopen(tmp,"ab");
	if (fp)
	{
		fwrite(cost,sizeof (struct _costinfo),1,fp);
		fclose(fp);
		return 1;
	}
	return 0;
}

// ===============================================================================
// returns the count of comparing chars in two strings
	int get_equal_size(LPCSTR str1,LPCSTR str2)
// ===============================================================================
{
int same=0;
LPCSTR p1=str1;
LPCSTR p2=str2;

	while (*p1 && *p2)
	{
		if (*p1 == *p2)
		{
			same++;
			p1++;
			p2++;
		}
		else
			return same;
	}
	return same;
}

// ===============================================================================
// Priority Boost
	void	set_fips_priority (int flag)
// ===============================================================================
{
static int	current_priority=0;		// Normal Priority

	switch (flag)
	{
		case 0:				// back to Normal
			if (current_priority==0)
				return;
			SetPriorityClass(GetCurrentProcess(),NORMAL_PRIORITY_CLASS);
			current_priority=0;
			break;
		case 1:				// burst
			if (current_priority==1)
				return;

			SetPriorityClass(GetCurrentProcess(),HIGH_PRIORITY_CLASS);
			modem_add_listbox(L("S_279"));	// Increasing Priority ...
			current_priority=1;
			break;
	}
}


// ===============================================================================
	void write_lastcall_idx(void)
// ===============================================================================
{
FILE	*fpx;
long	xtime;
	// Als allererstes schreiben wir uns die Zeit des letzten
	// successfull calls in die Datei LastSucessfulCall
	fpx=fopen(LastSucessfulCall,"wb");
	if (fpx)
	{
		gLastSucessfulCallTime=xtime=time(NULL);
		fwrite(&xtime,sizeof(int),1,fpx);
		fclose(fpx);
	}
}


/*/ ===============================================================================
	int modify_origin_aka_for_area(int areahandle,CString &origin)
// ===============================================================================
// Diese Funktion modifiziert die AKA in einer Origin Zeile falls notwendig
// fuer die Zielarea
{
CString orgneu;
char	*p;
int		ii;
char	boss[300];
char	point[300];

	// Test ob das auch wirklich eine Origin Zeile ist ...
	p=strstr(origin,"* Origin:");
	if (!p)
		return 0;	// no Origin

	ii=origin.ReverseFind('(');
	if (ii==-1)
		return 0;

	if (db_get_uplink_for_area(areahandle,boss,point)!=DB_OK)
		return 0;						// Keine gueltige Point Adresse

	// Nun basteln wir die neue Origin ...
	// Mal schauen ob das ganze zu lang wird.

	int len=ii+1+strlen(point)+2;  // Laenge Anfang + point + )
	while (len>79 && ii>0)
	{
		ii--;
		len--;
	}

	orgneu=origin.Left(ii+1);
	if (orgneu[ii]!='(')
		orgneu+="(";
	orgneu+=point;
	orgneu+=")";

	// Die neue Origin ...
	origin=orgneu;
	return 1;
}*/

// ===============================================================================
	int broesel_msns	(void)
// ===============================================================================
// TOP: Diese Funktion zerbroeselt die Eingabe in der ISDN Config in die Nr1 bis Nr3.
{
CString help;
CStrList xx;
char    buf[300],buf2[300],*p=0,*p2=0;

	strcpy(gcomm.Nr1,"");
	strcpy(gcomm.Nr2,"");
	strcpy(gcomm.Nr3,"");

	help=isdnaccept;
    trim_all(help);
	if (help.GetLength()<=0)
		return 0;

	xx.SplitTabs(PS help,',');

	for (int k=0;k<(xx.GetCount()-1);k++)
	{
		help=xx.GetString(k);
        trim_all(help);
		if (help.GetLength()>39)
		{
			modem_add_listbox("MSN input specification too long!");
			help.Empty();
		}

		strcpy(buf,help);
		p=buf;
		memset(buf2,0,sizeof(buf2));
		p2=buf2;

		while (*p)
		{
			if (isdigit(*p))
			{
				*p2=*p;
				p2++;
			}
			p++;
		}

		switch (k)
		{

			case 0:
					strcpy(gcomm.Nr1,buf2);
					break;

			case 1:
					strcpy(gcomm.Nr2,buf2);
					break;

			case 2:
					strcpy(gcomm.Nr3,buf2);
					break;

			case 3:
					// BUGBUG in der SplitTabs Funktion
					break;

			default:
					modem_add_listbox("Maximum of 3 MSN exceeded");
					return 0;
					break;

		}
	}
	return 1;
}

// ===========================================================================
	void DynamicAdjustItemSizes(CWnd *wx,struct _DlgItemsSize *sf, int count, int newx, int newy)
// ===========================================================================
{
struct _DlgItemsSize *sfi;
int		i;
CWnd	*wptr;
int		diff;

	for (i=1;i<count;i++)
	{
		sfi=sf+i;
		sfi->tomove.top		=sfi->orgrect.top;
		sfi->tomove.left	=sfi->orgrect.left;
		sfi->tomove.right	=sfi->orgrect.right;
		sfi->tomove.bottom	=sfi->orgrect.bottom;
		sfi->mustmove=0;
	}

// if there are horisontal changes of Dialog
	if (newx >	sf[0].orgrect.right)
	{
		diff=newx - sf[0].orgrect.right;
		for (i=1;i<count;i++)
		{
			sfi=sf+i;
			if (sfi->left & HO)
			{
				sfi->tomove.left = sfi->orgrect.left + diff;
				sfi->mustmove=1;
			}
			if (sfi->right & HO)
			{
				sfi->tomove.right = sfi->orgrect.right + diff;
				sfi->mustmove=1;
			}
		}
	}

// if there are vertical changes of Dialog
	if (newy >	sf[0].orgrect.bottom)
	{
		diff=newy - sf[0].orgrect.bottom;
		for (i=1;i<count;i++)
		{
			sfi=sf+i;
			if (sfi->top & VE)
			{
				sfi->tomove.top = sfi->orgrect.top + diff;
				sfi->mustmove=1;
			}
			if (sfi->bottom & VE)
			{
				sfi->tomove.bottom = sfi->orgrect.bottom + diff;
				sfi->mustmove=1;
			}
		}
	}

	for (i=1;i<count;i++)
	{
		sfi=sf+i;
		if (sfi->mustmove)
		{
			wptr=(CWnd *)wx->GetDlgItem(sfi->id);
			if (wptr)
			{
				RECT re;
				re.top		=sfi->tomove.top;
				re.left		=sfi->tomove.left;
				re.right	=sfi->tomove.right;
				re.bottom	=sfi->tomove.bottom;
				wptr->MoveWindow(&re,FALSE);
			}
		}
	}
}

// ===========================================================================
	void StoreInitDialogSize(char *name,CWnd *wx)
// ===========================================================================
{
CString str;
RECT	re;

	if (!wx)	return;
	wx->GetWindowRect(&re);
	str.Format("%d,%d,%d,%d,%d",re,wx->IsWindowVisible());
	set_cfg(CFG_DIALOGS,name,str);
}

// ===========================================================================
	void StoreOrgItemSizes(char *name,CWnd *wx,struct _DlgItemsSize *sf, int count)
// ===========================================================================
{
int		i,ret;
int     show=0;
CWnd	*wptr;
RECT	re;
RECT	re1;
CString	str;

	wx->GetClientRect(&(sf->orgrect));
	wx->GetWindowRect(&re);
	wx->GetClientRect(&re1);
	wx->ClientToScreen(&re1);

	for (i=1;i<count;i++)
	{
		if (!((sf+i)->id))
			continue;

		wptr=wx->GetDlgItem((sf+i)->id);
		if (wptr)
		{
			wptr->GetWindowRect(&((sf+i)->orgrect));
			wx->ScreenToClient(&((sf+i)->orgrect));
		}
	}

	str=get_cfg(CFG_DIALOGS,name,"");
	ret=sscanf(str,"%d,%d,%d,%d,%d",&re.left,&re.top,&re.right,&re.bottom,&show);
	if (ret>=4)
	{
		wx->MoveWindow(&re);
		if (ret==5 && show==1)
			wx->ShowWindow(SW_SHOWNORMAL);
	}
}

// ======================================================
	void DrawRightBottomKnubble(CWnd *wptr,CPaintDC &dc)
// ======================================================
{
int  height;
int  width;
int  x,y;
RECT re;

	wptr->GetClientRect(&re);

	height=re.bottom-re.top;
	width =re.right-re.left;
	x=re.left+width-7;
	y=re.top+height-7;

	dc.MoveTo(x,re.bottom);
	dc.LineTo(re.right,y);
	x++;y++;
	dc.MoveTo(x,re.bottom);
	dc.LineTo(re.right,y);
	x++;y++;
	dc.MoveTo(x,re.bottom);
	dc.LineTo(re.right,y);
	x++;y++;
	dc.MoveTo(x,re.bottom);
	dc.LineTo(re.right,y);
	x++;y++;
	dc.MoveTo(x,re.bottom);
	dc.LineTo(re.right,y);
	x++;y++;
	dc.MoveTo(x,re.bottom);
	dc.LineTo(re.right,y);
	x++;y++;
	dc.MoveTo(x,re.bottom);
	dc.LineTo(re.right,y);
}

// ============================================
	int IsThereAInterestingEvent (void)
// ============================================
// check Inbound on interesting events
{
CString		basedir;
CString		line;
CStrList	xx;
CStrList	yy;
char	tmppath[300];
int			ret;
int			notread;
int         i;
struct areadef adef;

	ret=db_get_area_by_index(0,&adef);
	if (ret!=DB_OK)
		return 0;

	notread=adef.number_of_mails-adef.number_of_read;
	if (notread<0)
		notread=0;

	if (notread>0)
		return 1;

	basedir.Format("%s\\*.pkt",gc.InboundPath);
	xx.FillWithFiles(basedir);

	if (xx.GetCount()>0)
		return 1;

	make_path(tmppath,gc.BasePath,OTHERPLACES);
	xx.FillAsEdit(tmppath);
	for (i=0;i<xx.GetCount();i++)
	{
		line=xx.GetString(i);
		if (line.GetLength()<2)
			continue;

		yy.FillWithFiles(line);
		if (yy.GetCount()>0)
			return 1;
	}

	return 0;
}

// ============================================
	void SetRollLock( BOOL bState )
// ============================================
{
BYTE keyState[256];

   GetKeyboardState((LPBYTE)&keyState);
   if((bState && !(keyState[VK_SCROLL] & 1)) || (!bState && (keyState[VK_SCROLL] & 1)))
   {
   // Simulate a key press
      keybd_event(VK_SCROLL,0x45,KEYEVENTF_EXTENDEDKEY | 0,0);
   // Simulate a key release
      keybd_event(VK_SCROLL,0x45,KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP,0);
   }
}


// ============================================
	void mark_as_answered(int action)
// ============================================
// 0 = Store
// 1 = Mark
{
static _ustat lastustat;
mailheader	mh;

	if (action==0)
	{
		memcpy(&lastustat,&gustat,sizeof(_ustat));
		gc.ShouldMarkAsAnsweredOnSave=1;
		return;
	}

	if (action==1)
	{
		if (gc.ShouldMarkAsAnsweredOnSave)
		{
			db_gethdr_by_index(lastustat.cur_area_handle,lastustat.act_mailh.index,&mh);
			mh.status |= DB_ANSWERED;
			db_sethdr_by_index(lastustat.cur_area_handle,lastustat.act_mailh.index,&mh,1);
		}
		gc.ShouldMarkAsAnsweredOnSave=0;
	}
}

// ====================================================
	int	check_repost(void)
// ====================================================
{
_repost rp;
FILE	*fp;
mailheader mh;
CStrList   lst;
char	base[MAX_PATH],path[MAX_PATH],npath[MAX_PATH],buf[100],*p;
long    now,ft,hnd=0;
int     i,cnt=0;

	make_path(base,gc.MsgBasePath,REPOST_FLD); 
	mkdir(base);
	make_path(path,base,"*.");
	lst.FillWithFiles(path);
	p=(char *)malloc(MAX_MSG+100);
	if (!p)	ERR_MSG_RET0("E_MEM_OUT");
	_tzset();
	now=time(NULL)-_timezone;
	for (i=0;i<lst.GetCount();i++)
	{
		strcpy(buf,lst.GetString(i));
		sscanf(buf,"%x",&ft);
		if (now < ft)	continue;
		make_path(path,base,buf);
		fp=fopen(path,"rb");
		if (fp)
		{
			memset(&rp,0,sizeof(rp));
			fread(&rp,sizeof(rp),1,fp);
			memset(&mh,0,sizeof(mh));
			fread(&mh,sizeof(mh),1,fp);
			fread(p,mh.text_length,1,fp);
			mh.mail_text=p;
			mh.status |= DB_MAIL_READ | DB_MAIL_CREATED;
			//mh.status &= ~DB_MAIL_READ;
			//mh.status &= ~DB_ANSWERED;

			if (db_get_area_handle(rp.echotag,&hnd,1)==DB_OK)
				if (db_append_new_message(hnd,&mh)==DB_OK)
					cnt++;

			fclose(fp);
			now=((time(NULL)-_timezone)/86400+rp.period)*86400;	// round to midnight
			do sprintf(npath,"%s\\%x",base,now++);
			while (!access(npath,0));
			rename(path,npath);
			//SafeFileDelete(path);
		}
	}
	free(p);
	if (cnt)	tosser_add_listbox(L("S_7",cnt));	// repost mails
	return 1;
}

//vk: added functions
// ====================================================
	void show_msg(LPCSTR msg, int delay)
// ====================================================
{
msgbox tmp;

	tmp.delay_mult=delay;
	tmp.title=msg;
	tmp.DoModal();
}

// ====================================================
	void show_msg(LPCSTR eng,LPCSTR rus,int delay)
// ====================================================
{
msgbox tmp;

	tmp.delay_mult=delay;
	tmp.title= LANG_ID==LANG_RUS ? rus : eng;
	tmp.DoModal();
}

// ====================================================
	void show_msg(LPCSTR eng,LPCSTR ger,LPCSTR rus,int delay)
// ====================================================
{
msgbox tmp;

	tmp.delay_mult=delay;
	tmp.title= LANG_ID==LANG_GER ? ger : (LANG_ID==LANG_RUS ? rus : eng);
	tmp.DoModal();
}

// ====================================================
	void show_msg(LPCSTR eng,LPCSTR ger,LPCSTR rus,LPCSTR fre,int delay)
// ====================================================
{
msgbox tmp;

	tmp.delay_mult=delay;
	tmp.title= LANG_ID==LANG_GER ? ger : (LANG_ID==LANG_RUS ? rus : 
		(LANG_ID==LANG_FRE ? fre : eng));
	tmp.DoModal();
}

// ====================================================
	CString get_cfg(int id_sect,LPCSTR key,LPCSTR defval)
// ====================================================
{
	return FipsApp.GetProfileString(get_sect_name(id_sect),key,defval);
}

// ====================================================
// returns limited-length CString cfg
	CString get_lim_cfg(int id_sect,LPCSTR key,LPCSTR defval,int lim)
// ====================================================
{
CString	str;

	str=FipsApp.GetProfileString(get_sect_name(CFG_COMMON),key,defval);
	str.Left(lim);
	return str;
}

// ====================================================
	int get_cfg(int id_sect,LPCSTR key,int defval)
// ====================================================
{
	return FipsApp.GetProfileInt(get_sect_name(id_sect),key,defval);
}

// ====================================================
	void set_cfg(int id_sect,LPCSTR key,LPCSTR val,BOOL ifnew)
// ====================================================
{
CString	sect;

	sect=get_sect_name(id_sect);
	if (ifnew && FipsApp.GetProfileString(sect,key,"nokey")!="nokey")
		return;

	FipsApp.WriteProfileString(sect,key,val);
}

// ====================================================
	void set_cfg(int id_sect,LPCSTR key,int val,BOOL ifnew)
// ====================================================
{
CString	sect;

	sect=get_sect_name(id_sect);
	if (ifnew && FipsApp.GetProfileInt(sect,key,-999999)!=-999999)
		return;

	FipsApp.WriteProfileInt(sect,key,val);
}
// ====================================================
	void del_cfg(int id_sect)
// ====================================================
{
	WritePrivateProfileSection(get_sect_name(id_sect),NULL,FipsApp.m_pszProfileName);
}
// ====================================================
	void del_cfg(int id_sect,LPCSTR key)
// ====================================================
{
	FipsApp.WriteProfileString(get_sect_name(id_sect),key,NULL);
}

// ====================================================
	CString get_sect_name(int id_sect)
// ====================================================
{
	switch (id_sect)
	{
	case CFG_COMMON:
		return	"Common";
	case CFG_EDIT:
		return	"Editor";
	case CFG_TEXTIMP:
		return	"TextImport";
	case CFG_COLORS:
		return	"Colors";
	case CFG_PURGER:
		return	"Purger";
	case CFG_TOSSER:
		return	"Tosser";
	case CFG_MM:
		return	"Multimedia";
	case CFG_PRINT:
		return	"Print";
	case CFG_BITMAP:
		return	"Picture";
	case CFG_ESEARCH:
		return	"ExtSearch";
	case CFG_TIMEOUT:
		return	"Timeouts";
	case CFG_SCRIPTS:
		return	"Scripts";
	case CFG_DIALOGS:
		return	"Dialogs";
	case CFG_BOSSES:
		return	"Bosses";
	case CFG_SEARCH:
		return	"Search";
	case CFG_PACKERS:
		return	"Packers";
	case CFG_PREFIX:
		return	"Prefixes";
	case CFG_LOGS:
		return	"Logs";
	case CFG_SOUNDS:
		return	"Sounds";
	case CFG_INCOMIN:
		return	"Incoming";
	case CFG_FONTS:
		return	"Fonts";
	case CFG_HWARE:
		return	"Hardware";
	case CFG_MAILED:
		return	"MailEditor";
	default:
		return	"General";
	}
}

// ============================================
	char *langstr(LPCSTR eng,LPCSTR rus)
// ============================================
{
	return (LPSTR)(LANG_ID==LANG_RUS ? rus : eng);
}

// ============================================
	char *langstr(LPCSTR ger,LPCSTR eng,LPCSTR rus)
// ============================================
{
	return (LPSTR)(LANG_ID==LANG_RUS ? rus : (LANG_ID==LANG_GER ? ger : eng));
}

// ============================================
	char *langstr(LPCSTR ger,LPCSTR eng,LPCSTR rus,LPCSTR fre)
// ============================================
{
	return (LPSTR)(LANG_ID==LANG_RUS ? rus : 
		(LANG_ID==LANG_GER ? ger : (LANG_ID==LANG_FRE ? fre : eng)));
}

// ==================================================
	void set_first_token(CString &str,LPCSTR text)
// ==================================================
{
CString help;
int		idx;

   idx=str.Find('\t');
   help=text+str.Mid(idx);
   str=help;
}

// ===============================================================
	int is_address(char *fido)
// ===============================================================
{
char *p;

	p=fido;
	while (*p)
	{
		if (*p!=':' && *p!='/' && *p!='.' && !isdigit(*p))
			return FALSE;
		p++;
	}
	return TRUE;
}

// 	============================================================================
	int is_full_address(char *addr)
// 	============================================================================
{
	if (!strchr(addr,':') || !strchr(addr,'/'))	return FALSE;
    return is_address(addr);
}

// ===============================================
	void resize_wnd(CWnd *wnd,_DlgItemsSize *items,UINT size)
// ===============================================
{
RECT re;

	wnd->GetClientRect(&re);	
	DynamicAdjustItemSizes(wnd,items,size/sizeof(_DlgItemsSize),re.right,re.bottom);
	wnd->GetClientRect(&re);	
	wnd->InvalidateRect(&re);
}

// ===============================================
	void make_address(LPSTR buf,int zone,int net,int node,int point)
// ===============================================
{
	if (node && point)
		sprintf(buf,"%d:%d/%d.%d",zone,net,node,point);
	else if (node)
		sprintf(buf,"%d:%d/%d",zone,net,node);
	else
		sprintf(buf,"%d:%d",zone,net);
}

// ===============================================
	void make_address(CString &str,int zone,int net,int node,int point)
// ===============================================
{
	if (node && point)
		str.Format("%d:%d/%d.%d",zone,net,node,point);
	else if (node)
		str.Format("%d:%d/%d",zone,net,node);
	else
		str.Format("%d:%d",zone,net);
}

// ===============================================
	int parse_address(LPCSTR buf,int *pzone,int *pnet,int *pnode,int *ppoint)
// ===============================================
{
int ret;

	*pzone=*pnet=0;
	if (pnode && ppoint)
	{
		*pnode=*ppoint=0;
		ret=sscanf(buf,"%d:%d/%d.%d",pzone,pnet,pnode,ppoint);
	}
	else if (pnode)
	{
		*pnode=0;
		ret=sscanf(buf,"%d:%d/%d",pzone,pnet,pnode);
	}
	else
		ret=sscanf(buf,"%d:%d",pzone,pnet);
	return ret;
}

// ===============================================
	int parse_address(LPCSTR buf,USHORT *pzone,USHORT *pnet,USHORT *pnode,USHORT *ppoint)
// ===============================================
{
int ret;

	*pzone=*pnet=0;
	if (pnode && ppoint)
	{
		*pnode=*ppoint=0;
		ret=sscanf(buf,"%hu:%hu/%hu.%hu",pzone,pnet,pnode,ppoint);
	}
	else if (pnode)
	{
		*pnode=0;
		ret=sscanf(buf,"%hu:%hu/%hu",pzone,pnet,pnode);
	}
	else
		ret=sscanf(buf,"%hu:%hu",pzone,pnet);
	return ret;
}

// ===============================================
	void make_fidodir(LPSTR buf,int zone,int net,int node,int point)
// ===============================================
{
	if (point)
		sprintf(buf,"%d_%d_%d_%d",zone,net,node,point);
	else
		sprintf(buf,"%d_%d_%d",zone,net,node);
}

// ===============================================
	int split_fidodir(LPCSTR buf,int *pzone,int *pnet,int *pnode,int *ppoint)
// ===============================================
{
int ret;

	*pzone=*pnet=*pnode=0;
	if (ppoint)
	{
		*ppoint=0;
		ret=sscanf(buf,"%d_%d_%d_%d",pzone,pnet,pnode,ppoint);
	}
	else
		ret=sscanf(buf,"%d_%d_%d",pzone,pnet,pnode);

	return ret;
}

// ===========================================
// strips path from pathname, returns only filename //in uppercase
	void get_filename(LPCSTR in,LPSTR out)
// ===========================================
{
LPCSTR p;

	*out=0;
	if (!in || !in[0])
		return;

	p=strrchr(in,'\\');
	if (!p) p=strrchr(in,':');
	if (!p)	p=in-1;
	strcpy(out,p+1);
//	strupr(out);
}

// ==================================================
	LPSTR get_script_desc(int ind)
// ==================================================
{
CString	scr,name;
char	dm[MAX_PATH];

	name.Format("User%d",ind);
	scr=get_cfg(CFG_SCRIPTS,name,"");
	if (scr.GetLength()>1)
	{
		strcpy(gTmp,L("S_291"));
		strcat(gTmp," ");
		_splitpath(scr,dm,dm,gTmp+4,dm);
	}
	else
		strcpy(gTmp,L("S_397"));
	strcat(gTmp,L("S_34"));
	return gTmp;
}

// ==================================================
	BOOL GetDirectory(CString &path,HWND parent) 
// ==================================================
{ 
BOOL			ret; 
TCHAR			szPath[MAX_PATH]; 
char			szDir[MAX_PATH];
LPITEMIDLIST	pidl; 
LPMALLOC		lpMalloc; 
BROWSEINFO bi = {parent,NULL,szPath,L("S_402"),BIF_RETURNONLYFSDIRS,NULL,0L,0}; 
 
	bi.pidlRoot = 0;
	pidl = SHBrowseForFolder(&bi); 
 
	if (pidl)
	{
		ret = SHGetPathFromIDList(pidl, szDir);
		path=szDir;
// Get the shell's allocator to free PIDLs 
		if (!SHGetMalloc(&lpMalloc) && lpMalloc) 
		{ 
			lpMalloc->Free(pidl); 
			lpMalloc->Release(); 
		}
	}
	else 
		ret = FALSE; 
 
	return ret; 
} 
 
// ============================================
	void MakeColumns(CListCtrl &list,_listcol *listcol,int count,LPCSTR key,int ind)
// ============================================
{
LV_COLUMN	col;
char str[100],buf[100];
int	 i;

	memset(&col,0,sizeof(col));
	list.SetExtendedStyle(LVS_EX_FULLROWSELECT);// | LVS_EX_GRIDLINES);
	col.mask=LVCF_TEXT | LVCF_WIDTH;
	*str=0;
	if (key > 0)
	{
		sprintf(buf,"%s_L%d",key,ind);
		strcpy(str,get_cfg(CFG_DIALOGS,buf,""));
	}
	for (i=0;i<count;i++)
	{
		col.cx=*str ? get_token_int(str,i,';') : col.cx=listcol[i].width;
		col.pszText=listcol[i].hdr;
		list.InsertColumn(i,&col);
	}
}

// ============================================
	void SaveColumnsWidth(CListCtrl &lst,LPCSTR key,int ind)
// ============================================
{
char str[100],buf[100];

	*str=0;
	for (int i=0;i<lst.GetHeaderCtrl()->GetItemCount();i++)
	{
		sprintf(buf,"%d;",lst.GetColumnWidth(i));
		strcat(str,buf);
	}
	str[strlen(str)-1]=0;
	sprintf(buf,"%s_L%d",key,ind);
	set_cfg(CFG_DIALOGS,buf,str);
}

// ============================================
	void RestoreColumnsWidth(CListCtrl &lst,LPCSTR key,int ind)
// ============================================
{
char str[100],buf[100];
int w;

	sprintf(buf,"%s_L%d",key,ind);
	strcpy(str,get_cfg(CFG_DIALOGS,buf,""));
	if (*str == 0)	return;
	for (int i=0;i<lst.GetHeaderCtrl()->GetItemCount();i++)
	{
		w=get_token_int(str,i,';');
		lst.SetColumnWidth(i,w);
	}
}

// ============================================
	void RefreshColumns(CListCtrl &list,char **hdrs)
// ============================================
{
CHeaderCtrl *ph;
HDITEM		hi;
int	i,j;

	ph=list.GetHeaderCtrl();
	for (i=0;i<ph->GetItemCount();i++)
	{
		j=i*2+(LANG_ID==LANG_RUS);
		hi.pszText=hdrs[j];
		hi.cchTextMax=strlen(hdrs[j])+1;
		hi.mask=HDI_TEXT;
		ph->SetItem(i,&hi);
	}
}

// ============================================
	void SelectRow(CListCtrl &list,int ind)
// ============================================
{
	if (ind<0 || ind>=list.GetItemCount())	return;
	list.SetItemState(ind,LVIS_FOCUSED | LVIS_SELECTED,0x000F);
	list.EnsureVisible(ind,FALSE);
}

// ============================================
	void DeselectRow(CListCtrl &list,int ind)
// ============================================
{
	if (ind<0 || ind>=list.GetItemCount())	return;
	list.SetItemState(ind,LVIS_FOCUSED,0x000F);
}

// ============================================
	void AddRow(CListCtrl &list,const char *line)
// ============================================
{
LV_ITEM	item;
CString	str;
int		i,j;

	memset(&item,0,sizeof(item));
	item.mask=LVIF_TEXT;
	j=list.GetItemCount();
	item.iItem=j;
	list.InsertItem(&item);
	for (i=0;i<list.GetHeaderCtrl()->GetItemCount();i++)
	{
		get_token(line,i,str);
		list.SetItemText(j,i,str);
	}
}

// ============================================
	void ReplaceRow(CListCtrl &list,int ind,LPCSTR line)
// ============================================
{
CString	str;
	
	if (ind>=list.GetItemCount())	return;
	for (int i=0;i<list.GetHeaderCtrl()->GetItemCount();i++)
	{
		get_token(line,i,str);
		list.SetItemText(ind,i,str);
	}
}
	
// ============================================
	void GetRow(CListCtrl &list,int ind,CString &line)
// ============================================
{
	line.Empty();
	if (ind<0 || ind>=list.GetItemCount())	return;
	for (int i=0;i<list.GetHeaderCtrl()->GetItemCount();i++)
		line+=list.GetItemText(ind,i)+'\t';
}

// ============================================
	int GetSelectedItem(CListCtrl &list)
// ============================================
{
POSITION pos;

	pos=list.GetFirstSelectedItemPosition();
	return (pos==NULL ? LB_ERR : list.GetNextSelectedItem(pos));
}

// ====================================================
// choose a new font
	int select_font(CFont *pf,LPCSTR fname)
// ====================================================
{
LOGFONT lf;
	
	get_font(&lf,fname);
	CFontDialog dlg(&lf);
	if (dlg.DoModal() == IDOK)
	{
		save_font(&lf,fname);
		if (pf)
		{
			pf->DeleteObject();
			pf->CreateFontIndirect(&lf);
		}
		return 1;
	}
	return 0;
}

// ====================================================
// read font
	void get_font(LOGFONT *plf,LPCSTR fname,BOOL bOEM)
// ====================================================
{
CString str,tmp;

	if (bOEM)
		::GetObject(GetStockObject(OEM_FIXED_FONT),sizeof(LOGFONT),plf);
	else
		::GetObject(GetStockObject(ANSI_FIXED_FONT),sizeof(LOGFONT),plf);
	plf->lfHeight=-13;
	str=get_cfg(CFG_FONTS,fname,"");
	if (str.IsEmpty())	return;
	get_token(str,0,tmp,',');
	plf->lfHeight=atoi(tmp);
	get_token(str,1,tmp,',');
	plf->lfWidth=atoi(tmp);
	get_token(str,2,tmp,',');
	plf->lfWeight=atoi(tmp);
	get_token(str,3,tmp,',');
	plf->lfItalic=atoi(tmp);
	get_token(str,4,tmp,',');
	plf->lfUnderline=atoi(tmp);
	get_token(str,5,tmp,',');
	plf->lfCharSet=atoi(tmp);
	get_token(str,6,tmp,',');
	plf->lfOutPrecision=atoi(tmp);
	get_token(str,7,tmp,',');
	plf->lfClipPrecision=atoi(tmp);
	get_token(str,8,tmp,',');
	plf->lfQuality=atoi(tmp);
	get_token(str,9,tmp,',');
	plf->lfPitchAndFamily=atoi(tmp);
	get_token(str,10,plf->lfFaceName,',');
}
	
// ============================================================
	void save_font(LOGFONT *plf,LPCSTR fname)
// ============================================================
{
char str[300];

	sprintf(str,"%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%s",
		plf->lfHeight,plf->lfWidth,plf->lfWeight,plf->lfItalic,plf->lfUnderline,
		plf->lfCharSet,plf->lfOutPrecision,plf->lfClipPrecision,plf->lfQuality,
		plf->lfPitchAndFamily,plf->lfFaceName);
	set_cfg(CFG_FONTS,fname,str);
}
	
// ============================================================
	void SaveToDelimText(CListCtrl &list,const char *fname,int conv,char delim)
// ============================================================
// conv=0 - without conversion
//     >0 - OEM to ANSI
//     <0 - ANSI to OEM
{
int		i;
FILE	*fp;
CString buf;
	
	fp=fopen(fname,"wb");
	if (!fp)
		return;
	for (i=0;i<list.GetItemCount();i++)
	{
		GetRow(list,i,buf);
		buf.Replace('\t',delim);
		if (conv>0) buf.OemToAnsi();
		if (conv<0) buf.AnsiToOem();
		fprintf(fp,"%s\n",buf);
	}
	fclose(fp);
}
	
// ============================================================
	int LoadFromDelimText(CListCtrl &list,const char *fname,int conv,char delim)
// ============================================================
// conv=0 - without conversion
//     >0 - OEM to ANSI
//     <0 - ANSI to OEM
{
FILE	*fp;
char	buf[1000];
CString line;
	
	list.DeleteAllItems();
	fp=fopen(fname,"rt");
	if (fp)
	{
		while (fgets(buf,999,fp))
		{
			line=buf;
			line.Replace(delim,'\t');
			line.Replace('\n','\0');
			if (conv>0) line.OemToAnsi();
			if (conv<0) line.AnsiToOem();
			AddRow(list,line);
		}
		fclose(fp);
		return 1;
	}
	else
		return 0;
}

// =====================================================================
// remove leading and trailing whitespaces
    CString &trim_all(CString &text)
// =====================================================================
{
	return text.Trim();
}

// =====================================================================
// remove leading and trailing whitespaces (codes < 32)
    LPSTR trim_all(LPSTR str)
// =====================================================================
{
char *p,*t;

	if (*str)
	{
		p=str;
		t=str+strlen(str)-1;
		while (*p && (UCHAR)*p<=' ')
			p++;
		while (t>=p && (UCHAR)*t<=' ')
			t--;
		strncpy(str,p,t-p+1);
		str[t-p+1]=0;
	}
	return str;
}

// ============================================================
// returns string representation of calltime in nodelist
	void get_call_time(const char *str,char *time1,char *time2)
// ============================================================
{
const char *p;

	*time1=*time2=0;
	p=strstr(str,"CM");
	if (p)
	{
		strcpy(time1,"00:00");
		strcpy(time2,"23:59");
		return;
	}
	p=strstr(str,",T");
	if (p)
	{
		decode_time(*(p+2),time1);
		decode_time(*(p+3),time2);
	}
	if (*time1 && *time2)
		return;
	*time1=*time2=0;
}

// =================================================================
// decodes char code of calltime
	void decode_time(const char chtime,char *sztime)
// =================================================================
{
TIME_ZONE_INFORMATION tz;
div_t	dd;
int		mn=0,hr=0;
char	ch=chtime;

	*sztime=0;
	GetTimeZoneInformation(&tz);
	if (ch>='a' && ch<='x')
		mn=30;
	ch=toupper(ch);
	if (ch<'A' && ch>'X')
		return;
	hr=ch-'A';
	dd = div((int)(hr * 60 + mn - tz.Bias), (int)60);
	hr=dd.quot;
	mn=dd.rem;
	if (hr>=24)
		hr-=24;
	sprintf(sztime,"%02d:%02d",hr,mn);
}

// =================================================================
	void FuncOnActionsB64decodemail (void)
// =================================================================
{
int i;
CString help;

	CB64DEC tmp(gpMain->m_hWnd);
	tmp.Reset();

	for (i=0;i<actdisplist.GetCount();i++)
	{
		help=actdisplist.GetString(i);
	    if(help.GetLength())tmp.DecodeLine(PS help);
	}
        tmp.DecodeLine("");
	show_msg(L("S_176"));
}

//===================================================
	int b64decode_from_tmp_file(char *filename)
//===================================================
// Hier uudecoden wir das tmp file welches auf der platte liegen sollte ...
{
char aline[3000];
 
	FILE *fp;
	int i;

	fp=fopen(UUMULFILE,"rt");
	if (!fp)
		ERR_MSG2_RET0("E_CANFUTFP",UUMULFILE)
	else
	{
		CB64DEC tmp(gpMain->m_hWnd);
		tmp.Reset();
		while (fgets(aline,2999,fp)) 
		{i=strlen(aline);
		 if ((i>0) && (aline[i-1]==10)) aline[i-1]='\0';
		 if (*aline) tmp.DecodeLine(aline);
		}
		  tmp.DecodeLine("");

	}

	fclose(fp);
	return 1;
}

//===================================================
	void parse_name(LPCSTR name,CString &first,CString &second)
//===================================================
{
char	dm0[100],dm1[100],dm2[100];
int		i,ret;

	first.Empty();
	second.Empty();
	*dm0=0;
	*dm1=0;
	*dm2=0;
	ret=sscanf(name,"%s %s %s",dm0,dm1,dm2);
	for (i=0;i<ret;i++)
	{
		if (i==0)
			first=dm0;
		if (i==1)
			second=dm1;
		if (i==2)
			second=second+" "+dm2;
	}
}

// ==================================================================
// extract token number n (zero based) from dl-delimited string
// returns  0 if not found, 1 if found, founded token returns in dst
	BOOL get_token(LPCSTR src,int n,CString &dst,char delim)
// ==================================================================
{
char	buf[1000];
const char 	*beg,*end;
int 	i=0;

	dst.Empty();
	if (*src==0)
		return n==0;
	beg=src;
	do
	{
		end=strchr(beg,delim);
		if (!end)
			end=src+strlen(src);
		if (i==n)
			break;
		i++;
		beg=end+1;
	}while(*end);
	
	if (beg<=end)
	{
		strncpy(buf,beg,end-beg);
		buf[end-beg]=0;
		dst=buf;
		return 1;
	}
	return 0;
}

// ==================================================================
// extract token number n (zero based) from delim-delimited string
// returns  0 if not found, 1 if found, founded token returns in dst
	BOOL get_token(LPCSTR src,int n,LPSTR dst,char delim)
// ==================================================================
{
const char 	*beg,*end;
int 	i=0;

	*dst=0;
	if (strlen(src)==0)
		return n==0;
	beg=src;
	do
	{
		end=strchr(beg,delim);
		if (!end)
			end=src+strlen(src);
		if (i==n)
			break;
		i++;
		beg=end+1;
	}while(*end);
	
	if (beg<=end)
	{
		strncpy(dst,beg,end-beg);
		dst[end-beg]=0;
		return 1;
	}
	return 0;
}

// ==================================================================
// extract token number n (zero based) from delim-delimited string
// returns token converted to integer
	int get_token_int(LPCSTR src,int ind,char delim)
// ==================================================================
{
CString buf;

	get_token(src,ind,buf,delim);
	return atoi(buf);
}

// ==================================================================
	int count_tokens(LPCSTR src,LPCSTR delim)
// ==================================================================
{
LPCSTR p;
int cnt=0,n;

	if (*src==0)	return 0;
	p=src;
	do
	{
		n=strspn(p,delim);
		p+=n;
		if (*p==0)	break;
		n=strcspn(p,delim);
		cnt++;
		p+=n;
	} while(*p);
	return cnt;
}

// ===========================================
	BOOL IsValidDir(LPCSTR path) 
// ===========================================
{ 
DWORD dwAttrib; 

	dwAttrib = GetFileAttributes(path); 
	if (dwAttrib == -1) 
			return 0; 

	if (dwAttrib & FILE_ATTRIBUTE_DIRECTORY)
			return 1; 

	return 0; 
}

//// ==================================================================
//	void make_user_conversion(CString &text)
//// ==================================================================
//{
//CString ptrn;
//CString	repl;
//int    	i;
//
//	for (i=0;i<trepls.GetCount();i++)
//	{
//		get_token(trepls.GetString(i),0,ptrn);
//		get_token(trepls.GetString(i),1,repl);
//		text.Replace(ptrn,repl);
//	}
//}

// ==================================================================
	void get_colors(void)
// ==================================================================
{
	gColors[SUBJ_BACKGR_COLOR]=get_cfg(CFG_COLORS,"SubjBgrColor",16384/*0x4000*/);
	gColors[TEXT_BACKGR_COLOR]=get_cfg(CFG_COLORS,"TextBgrColor",16384/*0x4000*/);
	gColors[SUBJ_COLOR]=get_cfg(CFG_COLORS,"SubjColor",16777215/*0xFFFFFF*/);
	gColors[TEXT_COLOR]=get_cfg(CFG_COLORS,"TextColor",16777215/*0xFFFFFF*/);
	gColors[ODD_QUOTE_COLOR]=get_cfg(CFG_COLORS,"OddColor",65535/*0xFFFF*/);
	gColors[EVEN_QUOTE_COLOR]=get_cfg(CFG_COLORS,"EvenColor",33023/*0x80FF*/);
	gColors[KLUDGE_COLOR]=get_cfg(CFG_COLORS,"KludgeColor",16776960/*0xFFFF00*/);
	gColors[ORIGIN_COLOR]=get_cfg(CFG_COLORS,"OriginColor",33023/*0x80FF*/);
	gColors[BOLD_COLOR]=get_cfg(CFG_COLORS,"BoldColor",10461183/*0x9F9FFF*/);
	gColors[SUBJ_LINE_DISTANCE]=get_cfg(CFG_COLORS,"SubjLineDist",14);
	gColors[TEXT_LINE_DISTANCE]=get_cfg(CFG_COLORS,"TextLineDist",14);
	gColors[NEST_COLOR]=get_cfg(CFG_COLORS,"NestColor",65535/*0xFFFF*/);
	gColors[WRITMAILSIZE]=get_cfg(CFG_COLORS,"WriteEditSize",900);
}
	
// ==================================================================
	int floor(int num,int div)
// ==================================================================
{
int r;

	r=num/div;
	if (num % div)
		r++;
	return r;
}
	
// ================================================================
// parse char date 'DD MMMM YYYY HH:MM:SS' to unixtime
// if lang==TRUE, parse language specific month names
	long parse_time(const char *time,BOOL lang)
// ================================================================
{
char	month[20];
const char	**monthes;
struct	tm mail_tm;

	memset(&mail_tm,0,sizeof(mail_tm));

	sscanf(time,"%d %s %d %d:%d:%d",&mail_tm.tm_mday,month,&mail_tm.tm_year,
		&mail_tm.tm_hour,&mail_tm.tm_min,&mail_tm.tm_sec);

	if (mail_tm.tm_year < 80) 
		mail_tm.tm_year += 100;
	if (mail_tm.tm_year >= 1900) 
		mail_tm.tm_year -= 1900;

	if (lang && LANG_ID==LANG_RUS)
		monthes=rmonth;
	else
		monthes=emonth;

	for (int i=0;i<12;i++)
		if (!strnicmp(month,monthes[i],strlen(month)))
		{ 
			mail_tm.tm_mon=i; 
			break; 
		}
	return mktime(&mail_tm);
}

// ================================================================
// returns language translated string
	char *langtime(const char *src,char *dst)
// ================================================================
{
char	month[20];
const char	*s;

	strncpy(dst,src,16);
	dst[16]=0;
	if (LANG_ID!=LANG_RUS)
		return dst;	// for non-Russian

	if (sscanf(src,"%*d %s %*d %*d:%*d:%*d",month)==1)
	{
		s=strstr(src,month);
		for (int i=0;i<12;i++)
			if (!strnicmp(month,emonth[i],strlen(month)))
			{ 
				strcpy(dst+(s-src),rmonth[i]);
				strncpy(dst+(s-src)+strlen(rmonth[i]),s+strlen(month),10);
				dst[strlen(rmonth[i])+13]=0;
				break; 
			}
	}
	return dst;
}

// ================================================================
// returns language specific date string
	char *langdate(time_t tim,char *dst,BOOL fullmonth, BOOL withtime)
// ================================================================
{
char month[20];
tm  *st;
int	 year;

	st=localtime(&tim);
	if (LANG_ID==LANG_RUS)
		strcpy(month,rmonth[st->tm_mon]);
	else
		strcpy(month,emonth[st->tm_mon]);

	if (!fullmonth)
		month[3]=0;

	year=st->tm_year;
	if (year>=100)
		year-=100;

	if (withtime)
		sprintf(dst,"%02d %s %02d  %02d:%02d",
			st->tm_mday,month,year,st->tm_hour,st->tm_min);
	else
		sprintf(dst,"%02d %s %02d",st->tm_mday,month,year);
	
	return dst;
}

// ================================================================
// remove extra white spaces
	void rtrim(char *str)
// ================================================================
{
	if (!str)
		return;
	char *p=str+strlen(str)-1;
	while (p>=str && ISSPACE(*p))
		*p--=0;
}

// ================================================================
// returns tearline according settings
	CString &get_tearline(CString &str,BOOL Oem)
// ================================================================
{
char tmp[100];

	str="---";
	if (!get_cfg(CFG_EDIT,"ClearTearline",0))
		str+=" "+get_cfg(CFG_EDIT,"Tearline",get_versioninfo(tmp,0));
	if (Oem)
		str.AnsiToOem();
	return str;
}

// ================================================================
// returns full name or reversed full name
	void get_fullname(CString &str,BOOL inv)
// ================================================================
{
	str.Empty();
	if (inv)
		str=str+gc.SecondName+" "+gc.FirstName;
	else
		str=str+gc.FirstName+" "+gc.SecondName;
}

// ================================================================
// get mailtext by index in subjects list in main dialog
	void get_mailtext(char *buf,int ind,BOOL kludge)
// ================================================================
{
CString str;
char line[1000];
int	 mailno,ret;
char *pbeg,*pend,*p;

	strncpy(line,(LPSTR)gpMain->m_subjlist.GetItemDataPtr(ind),999);
	line[999]=0;
	get_token(line,MAIL_INDEX,str);
	ret=sscanf(str,"%d",&mailno);
	ASSERT(ret);
	db_gethdr_by_index(gustat.ar2han,mailno,&(gustat.act_mailh));
	db_get_mailtext(gustat.ar2han,&(gustat.act_mailh),buf,MAX_MSG);
	if (kludge)
		return;
	pbeg=buf;
	p=strchr(buf,'\001');
	while (p)	// look for last kludge in header
	{
		if (!strncmp(p,"\001VIA",4) || !strncmp(p,"\001PATH",5))
		{
			pend=p;
			break;
		}
		pbeg=p+1;
		p=strchr(pbeg,'\r');	// look for end of kludge
		if (p)
			pbeg=p+1;
		p=strchr(pbeg,'\001');
	}
	if (pend)
		strncpy(buf,pbeg,pend-pbeg);
	else if (buf != pbeg)
		strcpy(buf,pbeg);
}

// ================================================================
// returns merged filepath, pointer to filename may be NULL
	char *make_path(LPSTR fpath,LPCSTR path,LPCSTR fname)
// ================================================================
{
	strcpy(fpath,path);
	if (fpath[strlen(fpath)-1] != '\\' && *fname != '\\')	strcat(fpath,"\\");
	strcat(fpath,fname);
	strupr(fpath);
	return fpath;
}

// ===============================================================
// returns tab-delimited string list of bosses info from fips.ini
	void load_bosslist(CStrList &bosslist)
// ===============================================================
{
CString str;
CString name;
int i=0;

	bosslist.RemoveAll();
	name.Format("Boss%d",i);
	str=get_cfg(CFG_BOSSES,name,"");
	while (!str.IsEmpty())
	{
		i++;
		str.Replace(';','\t');
		bosslist.AddTail(str);
		name.Format("Boss%d",i);
		str=get_cfg(CFG_BOSSES,name,"");
	}
}

// ===============================================================
// saves tab-delimited string list of bosses info to fips.ini
	void save_bosslist(CStrList &lst)
// ===============================================================
{
CString str;
CString name;
POSITION pos;
int i=0;

	for (pos=lst.GetHeadPosition();pos;i++)
	{
		str=lst.GetNext(pos);
		str.Replace('\t',';');
		name.Format("Boss%d",i);
		set_cfg(CFG_BOSSES,name,str);
	}
}

// ===============================================================
// increments dial counter in event line in mailer window
	void set_queue_mark(int ind,const char *txt)
// ===============================================================
{
CString line;

	gMailer.m_queue.GetText(ind,line);
	set_first_token(line,txt);
	gMailer.m_queue.InsertString(ind,line);
	gMailer.m_queue.DeleteString(ind+1);
}

// ============================================
	LPSTR L(LPCSTR id,...)
// ============================================
{
va_list arg_ptr;
CString str,str2;
	
	*gStr=0;
	if (LangStrList.FindString(id,str)<0)	return gStr;
	get_token(str,1,str2);
	str2.Replace("\\","\r\n");
	va_start(arg_ptr,id);
	vsprintf(gStr,str2,arg_ptr);
	va_end(arg_ptr);
	return gStr;
}

// ====================================================
// load headers from cfg or text file
	void load_headers(CStrList &lst)
// ====================================================
{
	lst.RemoveAll();
	if (!access("headers.def",0))
		lst.LoadFromDelimText("headers.def",0);
	else
		lst.LoadFromFile("header.cfg");
	lst.defaultindex=get_cfg(CFG_EDIT,"HeaderQ",0);
	lst.defaultindexnew=get_cfg(CFG_EDIT,"HeaderN",0);
	if (!access("header.cfg",0) && access("headers.def",0))
	{
		lst.SaveAsEdit("headers.def");
		MoveFile("header.cfg","header.bak");
		set_cfg(CFG_EDIT,"HeaderQ",lst.defaultindex);
		set_cfg(CFG_EDIT,"HeaderN",lst.defaultindexnew);
	}
}

// ====================================================
// load footers from cfg or text file
	void load_footers(CStrList &lst)
// ====================================================
{
	lst.RemoveAll();
	if (!access("footers.def",0))
		lst.LoadFromDelimText("footers.def",0);
	else
		lst.LoadFromFile("footer.cfg");
	lst.defaultindex=get_cfg(CFG_EDIT,"FooterQ",0);
	lst.defaultindexnew=get_cfg(CFG_EDIT,"FooterN",0);
	if (!access("footer.cfg",0) && access("footers.def",0))
	{
		lst.SaveAsEdit("footers.def");
		MoveFile("footer.cfg","footer.bak");
		set_cfg(CFG_EDIT,"FooterQ",lst.defaultindex);
		set_cfg(CFG_EDIT,"FooterN",lst.defaultindexnew);
	}
}

// ====================================================
// load origins from cfg or text file
	void load_origins(CStrList &lst)
// ====================================================
{
CString str;

	lst.RemoveAll();
	str=get_cfg(CFG_EDIT,"OriginFile","");
	if (!str.IsEmpty() && !access(str,0))
		lst.LoadFromDelimText(str,0);
	else if (!access("origins.def",0))
		lst.LoadFromDelimText("origins.def",0);
	else
		lst.LoadFromFile("origin.cfg");
	lst.defaultindex=get_cfg(CFG_EDIT,"OriginQ",NO_ENTRY_SELECTED);
	lst.defaultindexnew=get_cfg(CFG_EDIT,"OriginN",NO_ENTRY_SELECTED);
	if (!access("origin.cfg",0) && access("origins.def",0))
	{
		lst.SaveAsEdit("origins.def");
		MoveFile("origin.cfg","origin.bak");
		set_cfg(CFG_EDIT,"OriginQ",lst.defaultindex);
		set_cfg(CFG_EDIT,"OriginN",lst.defaultindexnew);
	}
}

extern CStrList	OriginList,HeaderList,FooterList;
// ===============================================================
// returns header according settings
	int get_header(areadef *pad,BOOL fornew,CString &header,BOOL Oem)
// ===============================================================
{
int sel,cnt;

	header.Empty();
	if (HeaderList.GetCount()==0)	load_headers(HeaderList);
	if (fornew)
	{
		if (pad->header_new > 0)
			sel=pad->header_new;
		else
			sel=HeaderList.defaultindexnew;
	}
	else
	{
		if (pad->header_quote > 0)
			sel=pad->header_quote;
		else
			sel=HeaderList.defaultindex;
	}
	cnt=HeaderList.GetCount();
	if (cnt == 0)	return -1;
	if (sel >= cnt)	sel=0;
	header=HeaderList.GetString(sel);
	if (Oem)	header.AnsiToOem();
	return sel;
}

// ===============================================================
// returns footer according settings
	int get_footer(areadef *pad,BOOL fornew,CString &footer,BOOL Oem)
// ===============================================================
{
int sel,cnt;

	footer.Empty();
	if (FooterList.GetCount()==0)	load_footers(FooterList);
	if (fornew)
	{
		if (pad->footer_new > 0)
			sel=pad->footer_new;
		else
			sel=FooterList.defaultindexnew;
	}
	else
	{
		if (pad->footer_quote > 0)
			sel=pad->footer_quote;
		else
			sel=FooterList.defaultindex;
	}
	cnt=FooterList.GetCount();
	if (cnt == 0)	return -1;
	if (sel >= cnt)	sel=0;
	footer=FooterList.GetString(sel);
	if (Oem)	footer.AnsiToOem();
	return sel;
}

// ===============================================================
// returns origin according settings
	int get_origin(areadef *pad,BOOL fornew,CString &origin,BOOL Oem)
// ===============================================================
{
int sel,cnt;

	origin.Empty();
	if (OriginList.GetCount()==0)	load_origins(OriginList);
	if (fornew)
	{
		if (pad->origin_new > 0)
			sel=pad->origin_new;
		else
			sel=OriginList.defaultindexnew;
	}
	else
	{
		if (pad->origin_quote > 0)
			sel=pad->origin_quote;
		else
			sel=OriginList.defaultindex;
	}
	//sel=fornew ? OriginList.defaultindexnew : OriginList.defaultindex;
	cnt=OriginList.GetCount();
	if (cnt==0)	return -1;

	srand((unsigned)time(NULL));
	if 	(sel<=NO_ENTRY_SELECTED || sel>=cnt)
		sel=OriginList.GetCount()*rand()/(RAND_MAX+1);

	origin=OriginList.GetString(sel);
	if (Oem)	origin.AnsiToOem();
	return sel;
}

// ====================================================
// delete file phisycally or to recycled bin (customized in settings)
	void SafeFileDelete(LPCSTR fpath,BOOL filesonly)
// ====================================================
{
SHFILEOPSTRUCT sh;
CStrList files;
char	 str[MAX_PATH];
char	 dir[MAX_PATH];
int		 len,n;

	if (!fpath || !fpath[0])
		return;
	if (gc.UseRecycled)
	{
		memset(&sh,0,sizeof(SHFILEOPSTRUCT));
		len=strlen(fpath);
		strcpy(str,fpath);
		str[len+1]=0;
		sh.hwnd=NULL;
		sh.wFunc=FO_DELETE;
		sh.pFrom=str;
		sh.fFlags=FOF_NOCONFIRMATION | FOF_ALLOWUNDO;
		if (filesonly)	sh.fFlags|=FOF_FILESONLY;
		sh.fAnyOperationsAborted=FALSE;
		SHFileOperation(&sh);
	}
	else
	{
		if (strchr(fpath,'*') || strchr(fpath,'?'))
		{
			files.FillWithFiles(fpath);
			get_path(fpath,dir);
			for (n=0;n<files.GetCount();n++)
			{
				make_path(str,dir,files.GetString(n));
				unlink(str);
			}
			Sleep(100);
			if (!filesonly)
				RemoveDirectory(dir);
		}
		else
			unlink(fpath);
	}
}

// ====================================================
// returns path to exe module
	void get_exepath(char *path)
// ====================================================
{
char *p;

	strcpy(path,_pgmptr);
	p=path+strlen(path);
	while (p>path)
	{
		if (*p=='\\')	break;
		else if (*p==':')
		{	
			p++;
			break;
		}
		p--;
	}
	*p=0;
}

// ============================================
	void get_folder_path(LPCSTR ipath,LPCSTR fpath,LPCSTR key,LPSTR gcstr)
// ============================================
{
char tmp[MAX_PATH];

	strncpy(tmp,get_cfg(CFG_COMMON,key,ipath),MAX_PATH-1);
	tmp[MAX_PATH-1]=0;
	if (*tmp==0)
		strcpy(gcstr,ipath);
	else if (strchr(tmp,':')==0)
		make_path(gcstr,ipath,tmp);
	else
		strcpy(gcstr,tmp);
}

// ============================================
	void set_transparent_bmp(CStatic *ctl,int id)
// ============================================
{
HBITMAP hb;

	hb=(HBITMAP)LoadImage(hRes,MAKEINTRESOURCE(id),IMAGE_BITMAP,0,0,LR_LOADMAP3DCOLORS);
	if (hb)	ctl->SetBitmap(hb);
}

// ====================================================
// returns path, not ending with '\'
	void get_path(LPCSTR src,LPSTR dst)
// ====================================================
{
LPCSTR p;

	*dst=0;
	p=strrchr(src,'\\');
	if (!p)
	{
		p=strrchr(src,':');
		if (p)	p++;
	}
	if (p)
	{
		strncpy(dst,src,p-src);
		dst[p-src]=0;
	}
}

// ============================================================
    void load_lbx_from_str(CListBox &lbx,const char *lst)
// ============================================================
{
char str[300];
int	 i=0;
BOOL ret;
	
	lbx.SetRedraw(0);
	lbx.ResetContent();
	if (*lst)
	{
		do
		{
			ret=get_token(lst,i++,str);
			lbx.AddString(str);
		} while (ret);
	}
	lbx.SetRedraw(1);
}

// ===============================================================
// returns tab-delimited string list of packers info from fips.ini
	void get_packlist(BOOL def)
// ===============================================================
{
CString str;
int		num;
char	buf[1000];
char	*p,*q;

	memset(Packers,0,MAX_PACKERS*sizeof(_packer));
	num=GetPrivateProfileSection(get_sect_name(CFG_PACKERS),buf,1000,FipsApp.m_pszProfileName);
	if (!num || def)
	{
		add_packer(0,"PKZIP\tpkzip -m %f %p\tpkunzip -o %f %p\t50\t4B");
		add_packer(1,"RAR\trar m -ep %f %p\trar e -ep -o+ -w%p %f\t52\t61\t72\t21\t1A\t07\t00");
		add_packer(2,"ARJ\tarj m %f %p\tarj e -y %f %p\t60\tEA");
		add_packer(3,"LHARC\tlha m -m %f %p\tlha e -w%p %f\t\t\t2D\t6C\t68");
		add_packer(4,"LIM\tlim a %f -d%p\tlim e -y %f -d%p\t4C\t4D\t1A\t08\t00\t10\t10\t00\t80");
		add_packer(5,"ARC-I\tarc m %f %p\tarc eo %f %p\\*.*\t1A\t08");
		add_packer(6,"ARC-II\tarc m %f %p\tarc eo %f %p\\*.*\t1A\t09");
		add_packer(7,"ARC-III\tarc m %f %p\tarc eo %f %p\\*.*\t1A\t0B");
		add_packer(8,"PKPAK\tpkpak m %f %p\tpkunpak e -y %f %p\t1A\t0B");
		add_packer(9,"SQEEZE\tsqz m %f %p\tsqz e %f %p\\ *.*\t48\t4C\t53\t51\t5A");
	}
	else
	{
		q=buf;
		num=0;
		for (;;)
		{
			p=q+strlen(q);
			if (p==q)	 return;
			str=strchr(q,'=')+1;
			str.Replace(';','\t');
			add_packer(num,str);
			num++;
			if (!*(p+1)) return;
			q=p+1;
		}
	}
}

// ============================================================
// adds new packer to packers array
	void add_packer(int ind,LPCSTR str)
// ============================================================
{
CString tmp;
int		hex;

	get_token(str,0,tmp);
	strnzcpy(Packers[ind].name,tmp,20);
	get_token(str,1,tmp);
	strnzcpy(Packers[ind].pack_cmd,tmp,MAX_PATH);
	get_token(str,2,tmp);
	strnzcpy(Packers[ind].upck_cmd,tmp,MAX_PATH);
	for (int i=0;i<10;i++)
	{
		get_token(str,i+3,tmp);
		hex=0;
		if (*tmp)
		{
			sscanf(tmp,"%x",&hex);
			Packers[ind].sign[i]=hex;
		}
		else
			Packers[ind].sign[i]=-1;
	}
}

// ============================================================
// returns list of sounds
	void get_soundlist(CStrList &lst,BOOL def)
// ============================================================
{
CString str;
int		num;
char	buf[60000];
char	*p,*q;

	lst.RemoveAll();
	num=GetPrivateProfileSection(get_sect_name(CFG_SOUNDS),buf,60000,FipsApp.m_pszProfileName);
	if (!num || def)
	{
		str.LoadString(IDS_DEFSOUNDS);
		lst.LoadFromDelimString(str,'\n');
	}
	else
	{
		q=p=buf;
		num=0;
		for (;;)
		{
			if (!*p)
			{
				num++;
				str=q;
				str.Replace('=','\t');
				lst.AddTail(str);
				if (!*(p+1))	break;
				q=p+1;
			}
			p++;
		}
	}
}

// ============================================================
// saves the list of sounds
	void save_soundlist(CStrList &lst)
// ============================================================
{
CString key;
CString val;

	for (int i=0;i<lst.GetCount();i++)
	{
		get_token(lst.GetString(i),0,key);
		get_token(lst.GetString(i),1,val);
		set_cfg(CFG_SOUNDS,key,val);
	}
}

// ============================================================
// returns list of kludges
	void get_hiddenkludges(CStrList &lst,BOOL def)
// ============================================================
{
static char defk[]="INTL;FMPT;TOPT;MSGID;REPLY;PATH;Via;Recd;PID;RFC-;CHRS;TZUTC;TID";
CString str;

	lst.RemoveAll();
	str=get_cfg(CFG_EDIT,"HiddenKludges",defk);
	trim_all(str);
	if (str.IsEmpty() || def)
		str=defk;

	lst.LoadFromDelimString(str,';');
}

// ============================================================
// saves the list of kludges
	void save_hiddenkludges(CStrList &lst)
// ============================================================
{
CString str;

	str.Empty();
	for (int i=0;i<lst.GetCount();i++)
		str+=lst.GetString(i);

	set_cfg(CFG_EDIT,"HiddenKludges",str);
}

// ============================================================
// checks validity of FIDO-address
	BOOL is_valid_address(const char *str)
// ============================================================
{
CString buf;
int		i;

	if (parse_address(str,&i,&i,&i,&i)>=3)	return TRUE;
	buf.Format(IDS_E_NOTCOR,str);
	AfxMessageBox(buf);
	return FALSE;
}

// ============================================================
// returns sizes of char 'W' in required font
	void get_fontsize(CFont *fnt,CWnd *ctl,CSize &chs)
// ============================================================
{
CDC		*cdc;
CFont *ofnt;

	cdc=ctl->GetDC();
	ofnt=cdc->SelectObject(fnt);
	chs=cdc->GetTextExtent("W",1);
	cdc->SelectObject(ofnt);
	ctl->ReleaseDC(cdc);
}

// ================================================================================
	LPSTR get_info(CStatic &ctl)
// ================================================================================
{
_fidonumber number;
CString str,addr;
int		zone,net,node,point=0,n=0;
char	time1[5],time2[5],buf[1000];
char	*p;

	*buf=0;
	ctl.GetWindowText(addr);
	if (strlen(addr)==0)
	{
// trying look for REPLY kludge
		db_get_mailtext(gustat.ar2han,&(gustat.act_mailh),buf,HDR_LEN);
		p=strstr(buf,"\001REPLY: ");
		if (p)
			n=strcspn(p+8," @\r");
		if (n)
		{
			strncpy(buf,p+8,n);
			buf[n]=0;
		}
	}
	else
		strcpy(buf,addr);
	parse_address(buf,&zone,&net,&node,&point);
	if (zone && net && node && nl_get_fido_by_number(zone,net,node,point,&number))
	{
		p=strchr(number.flags,'\n');
		if (p) 
			*p=0;
		sprintf(gTmp,L("S_421",number.user));	// sysop name
		if (!strcmp(number.flags,L("S_81")))
			strcat(gTmp,L("S_1"));			// work time
		else
		{
			strcat(gTmp,L("S_100",number.bbsname,number.phone,number.location,
				number.flags));				// location
			get_call_time(number.flags,time1,time2);
			if (*time1 && *time2)
				strcat(gTmp,L("S_468",time1,time2));
			else
				strcat(gTmp,L("S_467"));	// worktime not defined
		}
	}
	else
		strcpy(gTmp,L("S_79"));
	return gTmp;
}

// ============================================
// returns formatted string of additional kludges
	CString get_extrakludges(BOOL addLF)
// ============================================
{
CString str;

	str.Empty();
	for (int i=0;i<AddKludges.GetCount();i++)
		str=str+'\001'+AddKludges.GetString(i)+(addLF ? "\r\n" : "\r");

	return str;
}

// ============================================
// returns formatted string of delivery time
	LPSTR get_deliverytime(LPCSTR str)
// ============================================
{
CString create,receive;
int		create_time=0,receive_time=0;
int		diff,day,hour,min;

	gpMain->m_create.GetWindowText(create);
	gpMain->m_receive.GetWindowText(receive);

	create_time=parse_time(create,TRUE);
	receive_time=parse_time(receive,TRUE);
	strcpy(gTmp,L(str));

	if (!create_time || !receive_time || create_time>receive_time)
	{	
		strcat(gTmp,L("S_283"));
		return gTmp;
	}

	diff=receive_time-create_time;
	day=diff/86400;
	diff=diff%86400;
	hour=diff/3600;
	diff=diff%3600;
	min=diff/60;
	if (!day && !hour && !min)
	{	
		strcat(gTmp,L("S_293"));
		return gTmp;
	}

	strcat(gTmp,L("S_294",day,hour,min));
	return gTmp;
}

// ============================================
	CString &russian_conversion(CString &str,BOOL oem)
// ============================================
{
	if (LANG_ID==LANG_RUS && gc.DefaultConv)
	{
		if (oem)
		{
			str.Replace('\x8D','H');
			str.Replace('\xE0','p');
		}
		else
		{
			str.Replace('Н','H');
			str.Replace('р','p');
		}
	}
	return str;
}

// ============================================
	LPSTR russian_conversion(LPSTR str,BOOL oem)
// ============================================
{
	if (LANG_ID==LANG_RUS && gc.DefaultConv)
	{
		if (oem)
		{
			replace_chars(str,'\x8D','H');
			replace_chars(str,'\xE0','p');
		}
		else
		{
			replace_chars(str,'Н','H');
			replace_chars(str,'р','p');
		}
	}
	return str;
}

// ============================================
// makes string for WriteProfile from string list
// first token in each line is the name of parameter
	void make_inistr(CStrList &lst,char *buf)
// ============================================
{
char	*p;
CString str;
CString name;
POSITION pos;

	p=buf;
	for (pos=lst.GetHeadPosition();pos;)
	{
		str=lst.GetNext(pos);
		get_token(str,0,name);
		name.MakeUpper();
		str.Replace('\t',';');
		strcpy(p,name);
		strcat(p,"=");
		strcat(p,str);
		p+=name.GetLength()+str.GetLength()+2;
	}
	*p=0;
}

// 	============================================================================
	char get_mail_marker(long status)
// 	============================================================================
{
	if (status & DB_DELETED)
		return 'D';
	else if (status & DB_FROZEN_MAIL)
		return 'F';
	else if (status & DB_NEVER_DELETE)
		return 'A';
	else if (status & DB_ANSWERED)
		return 'B';
/*	else if (status & DB_DUPE_MAIL)
		return '2';*/
	else if (status & DB_MAIL_SCANNED)
		return 'S';
	else if (status & DB_MAIL_CREATED)
		return 'P';
	else if (status & DB_MAIL_READ)
		return 'x';
	else
		return ' ';
}

// ==============================================================================
	void parse_textimp_str(LPCSTR key,CString &file,int &cod)
// ==============================================================================
{
char str[MAX_PATH+10],*p;

	cod=0;
	file.Empty();
	strcpy(str,get_cfg(CFG_TEXTIMP,key,""));
	p=strchr(str,',');
	if (p)
	{
		sscanf(p+1,"%d",&cod);
		*p=0;
	}
	file=str;
}

// ============================================================
    void split_string(LPCSTR str,CStrList &lst)
// ============================================================
{
const char *p,*t;
char	buf[1000];
	
	lst.RemoveAll();
	p=t=str;
	while (*p)
	{
		if (*p==',')
		{
			strncpy(buf,t,p-t);
			buf[p-t]=0;
			trim_all(buf);
			lst.AddTail(buf);
			p++;
			t=p;
		}
		else if (*p=='(')
			while (*p && *p++!=')');
		else
			p++;
	}
	if (p!=t)
	{
		strncpy(buf,t,p-t);
		buf[p-t]=0;
		trim_all(buf);
		lst.AddTail(buf);
	}
}

// ============================================================
    void collect_string(CString &str,CListBox &lst)
// ============================================================
{
CString tmp;

	str.Empty();
	for(int i=0;i<lst.GetCount();i++)
	{
		lst.GetText(i,tmp);
		trim_all(tmp);
		str=str+tmp+",";
	}
	if (!str.IsEmpty())	str.Delete(str.GetLength()-1);
}
		
// ============================================
// switch attributes for selected mails and set READ attribute
	void set_attributes(int attr,BOOL all)
// ============================================
{
pmailheader pm;
int		count,ret,mailno,del,ask=1;
long	st;
CString	tmp;

  count=gpMain->m_subjlist.GetCount();
	if (count<1)	return;
	pm=&gustat.act_mailh;
	for (int i=0;i<count;i++)
	{
		if (all || gpMain->m_subjlist.GetSel(i))
		{
			get_token((LPCSTR)gpMain->m_subjlist.GetItemDataPtr(i),MAIL_INDEX,tmp);
			ret=sscanf(tmp,"%d",&mailno);
			ASSERT(ret);
			db_gethdr_by_index(gustat.cur_area_handle,mailno,pm);
			st=pm->status;
			if (attr==DB_DELETED &&	!(st & DB_DELETED) && (st & DB_NEVER_DELETE))
			{
				if (ask)
				{
					ask=0;
					del=(err_out("DY_REDTMES")==IDYES);
				}
				if (!del)	continue;
				st &= ~DB_NEVER_DELETE;
			}
			pm->status =(st ^ attr) | DB_MAIL_READ;	// set attributes
			if (db_sethdr_by_index(gustat.cur_area_handle,mailno,pm) != DB_OK)
				ERR_MSG2_RET("E_DBSETHDRBYINDEX",ret);
			set_mailmark(i,get_mail_marker(pm->status));
		}
	}
	gpMain->m_attrlist.Invalidate();
	db_refresh_area_info(gustat.cur_area_handle);
}

// ============================================
	void handle_mailer_ctls(BOOL state)
// ============================================
{
detmail *pml=(detmail *)gc.mailer.thisptr;

	pml->GetDlgItem(IDC_LIST)->SetFocus();
	pml->GetDlgItem(IDC_SKIPCALL)->EnableWindow(state);
	pml->GetDlgItem(IDC_REMOVEFILE)->EnableWindow(state);
	pml->GetDlgItem(IDC_SKIPFILE)->EnableWindow(state);
	pml->GetDlgItem(IDABORTSESSION)->EnableWindow(state);
	pml->GetDlgItem(IDC_RESCAN)->EnableWindow(!state);
	pml->GetDlgItem(IDC_REMOVECALL)->EnableWindow(!state);
	pml->GetDlgItem(IDSTART)->EnableWindow(!state);
}

// ============================================
	void set_mailtext_font(LOGFONT *plf)
// ============================================
{
	font_mailtext.DeleteObject();
	font_mailtext.CreateFontIndirect(plf);
	plf->lfItalic=1;
	font_mail_italic.DeleteObject();
	font_mail_italic.CreateFontIndirect(plf);
	plf->lfUnderline=1;
	font_under_italic.DeleteObject();
	font_under_italic.CreateFontIndirect(plf);
	plf->lfItalic=0;
	font_mail_under.DeleteObject();
	font_mail_under.CreateFontIndirect(plf);
	plf->lfUnderline=0;
}

// ============================================
// replaces TABs with spaces
	CString &replace_tabs(CString &str)
// ============================================
{
	str.Replace('\t',' ');
	return str;
}

// ============================================
// replaces TABs with spaces
	LPSTR replace_tabs(LPSTR str)
// ============================================
{
char *p;

	p=str;
	while (*p)
	{
		if (*p=='\t')	*p=' ';
		p++;
	}
	return str;
}

// ====================================================
// replace "\r\n" with "/n"
	void remove_LFCR(char *text)
// ====================================================
{
char *p;

   p=text;
   for (size_t i=0;i<strlen(text);i++)
   {
	  if (*p=='\r' && *(p+1)=='\n')
	  {
	     *p='/';
		 *(p+1)='n';
	  }
	  p++;
   }
}

// ====================================================
// replace "/n" with "\r\n"
	void insert_LFCR(char *text)
// ====================================================
{
char *p;

	p=text;
	for (size_t i=0;i<strlen(text);i++)
	{
		if (*p=='/' && *(p+1)=='n')
		{
			*p='\r';
			*(p+1)='\n';
		}
		p++;
	}
}

// =========================================================================
// returns mailtext with filtered out lines
	void filter_mailtext(LPCSTR src,LPSTR dst)
// =========================================================================
{
CString str;
LPCSTR	s;
LPSTR	d;
int		c,proccess_it=0;
BOOL	hide;

	s=src;
	d=dst;
	memset(dst,0,MAX_MSG);
	while (d<dst+2*MAX_MSG-1)
	{
		c=*s;
		switch (c)
		{
		    case 0:					// EOF
				*d=0;
				return;
			case 1:					// kludges & tags
				if (gc.show_tags)
					*d=c;
				else
				{
					if (gc.HiddenMode)	// kludge hidden mode
					{
						hide=FALSE;
						for (int i=0;i<HideKludges.GetCount();i++)
						{
							str=HideKludges.GetString(i);
							if(!strnicmp(s+1,str,str.GetLength()))
							{
								hide=TRUE;
								break;
							}
						}
					}
					else
						hide=TRUE;
					
					if (hide)
					{
						while(*s && *s!='\r') 
							s++; 
						if (!*s)	//found EOF
						{
							*d=0;
							return;
						}
						proccess_it=0;
						continue;
					}
					else
						*d=c;
				}
				break;
			case '\r':				// CR
				if (gc.hide_leading_blank && !proccess_it)
				{
					s++;
					continue;
				}

				*d='\r';
				d++;
				*d='\n';
				break;
			 default:
				 if (!gc.show_seenby && !strncmp(s,"SEEN-BY:",8)) // remove SEEN-BY ? 
				 {
					while(*s && *s!='\r') 
						s++; 
					if (!*s) //found EOF
					{
						*d=0;
						return;
					}
					proccess_it=0;
					continue;
				 }

				 *d=c;
				 if (*d!=' ' && *d!='\t')
					proccess_it=1;
				 break;
		}

		d++; 
		s++;
	}
	*d=0;
}

// =========================================================================
// returns number of lines (count of '\r')
	int count_lines(LPCSTR src)
// =========================================================================
{
LPCSTR	p;
int		n=0;
	
	p=strchr(src,'\r');
	while (p)
	{
		n++;
		p=strchr(++p,'\r');
	}
	return n;
}

// =======================================================================================
	int limit_text_len(char *txt,int maxpt,CDC *pDC)
// =======================================================================================
{
SIZE sz;
int	 len=strlen(txt);

	GetTextExtentPoint32(pDC->m_hDC,txt,len,&sz);
	if (maxpt<=0 || len==0)
	{
		*txt=0;
		return 0;
	}
	while (sz.cx>maxpt)
		GetTextExtentPoint32(pDC->m_hDC,txt,len--,&sz);

	if (sz.cx<=0 || len<0)	len=0;
	txt[len]=0;
	return len;
}

// =========================================================================
// change action of mm-signature from immediate to dblclick
	void change_mm_action(LPSTR src)
// =========================================================================
{
LPSTR p;
int n;

	p=src-1;
	while (p=strstr(p+1,MM_PREFIX))
	{
		n=0;
		if (sscanf(p+3,"%*1[WB]%*1[12]%d",&n) && n)	*(p+=4)='4';
	}
}

/*/ =========================================================================
struct fnctab 
{
	LPCSTR find;
	LPCSTR repl;
} fnctab[] = 
{
	{".tar.gz", ".tgz"},
	{".tar.bz2",".tbz"},
	{".html",   ".htm"},
	{".desc",   ".dsc"},
	{NULL,      NULL  }
};
// =========================================================================
// converts long filename to short one (8.3)
	LPSTR get_shortname(LPSTR dst,LPCSTR src)
// =========================================================================
{
LPSTR p,t;

	strcpy(dst,src);
// replace common extensions
	for(int i=0;fnctab[i].find;i++)
	{
		if(p=strstr(dst, fnctab[i].find))
		{
			*p=0;
			strcat(dst,fnctab[i].repl);
			strcat(dst,p+strlen(fnctab[i].find));
			break;
		}
	}
// find extension
	p=strrchr(dst,'.');
	if (p)
	{
		for(t=p+1;*t && t-p<4;t++)
			if ((UCHAR)(*t)<=' ' || *t=='*' || *t=='?')
				*t='_';
		*t=0;
	}
	else
		p=dst+strlen(dst);
// replace illegal chars
	for (t=dst;t<p;t++)
		if ((UCHAR)(*t)<=' ' || strchr(".*?",*t))
			*t='_';
// shorten name
	if (t-dst>8)
	{
		*(dst+5)='~';
		p-=2;
		t=dst+6;
	}
	strcpy(t,p);
	return dst;
}
*/
// =========================================================================
// returns key string
	char *get_versionkey(LPSTR tmp,int ind)
// =========================================================================
{
char	path[MAX_PATH],key[20];
LPVOID	buf=NULL,lpvMem=NULL;
DWORD	dwVerHnd,dwVerInfoSize; 
UINT	szbuf;

	*tmp=0;
	GetModuleFileName(NULL,path,sizeof(path));
	if (dwVerInfoSize=GetFileVersionInfoSize(path,&dwVerHnd))
	{
		lpvMem=(void *)malloc(dwVerInfoSize);
		GetFileVersionInfo(path, dwVerHnd, dwVerInfoSize,lpvMem);
		strcpy(key,"\\StringFileInfo\\040904B0\\");
		switch (ind)
		{
			default:
			case 1:
				strcat(key,"ProductName");
				break;
			case 2:
				strcat(key,"FileVersion");
				break;
			case 3:
				strcat(key,"PrivateBuild");
				break;
			case 4:
				strcat(key,"SpecialBuild");
				break;
			case 6:
				strcat(key,"ProductVersion");
				break;
			case 7:
				strcat(key,"ProductVersion");
		}
		VerQueryValue(lpvMem,key,&buf,&szbuf);
		memcpy(tmp,buf,szbuf);
		tmp[szbuf]=0;
		free(lpvMem);
	}
	return tmp;
}

// =========================================================================
// returns version information
	LPSTR get_versioninfo(LPSTR str,int ind)
// =========================================================================
// parameters:
// 0 - full product info	"FIPS/Phoenix <build ...>"
// 1 - product name			"FIPS/Phoenix"
// 2 - build number			"01.11"
// 3 - private build		"beta"
// 4 - special build		"3"
// 5 - full build info		"<build ...>"
// 6 - main build			hex value in first char
// 7 - sub build			hex value in first char
{
char tmp[100];

	*str=0;
	if (ind==0)
	{
		strcpy(str,get_versionkey(tmp,1));
		strcat(str," ");
	}
	else
		strcpy(str,get_versionkey(tmp,ind));
	if (ind==5 || ind==0)
	{
		strcat(str,"<build ");
		for(int i=2;i<=4;i++)
			strcat(str,get_versionkey(tmp,i));
		strcat(str,">");
	}
	else if (ind==6)
		sscanf(str,"%d.",str);
	else if (ind==7)
		sscanf(str,"%*d.%d",str);

	return str;
}

// =========================================================================
	void replace_chars(LPSTR str,char s,char d)
// =========================================================================
{
	for(LPSTR p=str;*p;p++)	if (*p==s)	*p=d;
}

//	======================================================================
// expands partial FidoAddress and extract all info from Nodelist
	int expand_address(LPCSTR tail,		// tail of Address
					CString &full,		// full FIDO-address
					CString &info,		// info if exists
					int usersel,		// allow user selection if multivalence
					int get_nl_stuff,	// whole Nodelist Stuff
					int must_listed,	// Muss der eintrag in der Nodelist stehen
					int &errorcode,		// error code
					int patternsearch,	// pattern search (default=0)
					int nlsearch		// nodelist search (default=0)
					)
//	=====================================================================
// Available Formats: 2:2494/17, 2494/17, 17, hornet, #0878192116
/*
#define EXP_FIDO_ILLEGAL_CHARS_INNUMBER	1
#define EXP_FIDO_NUMBER_NEVER_LISTED	2
#define EXP_FIDO_INVALID_BOSS_CFG	    3
#define EXP_FIDO_NO_FIDO_FOR_STRING		4
#define EXP_EMPTY						5
#define EXP_NOUSERSELALL				6
#define EXP_NO_USERSEL					7
#define EXP_NO_INFO_IN_NL				8
*/
{
_fidonumber number,*result;
CString str,help2,help3,bossadress;
char	bossbuf[MAX_BOSSLEN],tmp[300];
int		zone,net,node,point,bosszone,bossnet,bossnode,ret,i;
int		dont_ask=0;

	errorcode=0;
	strcpy(tmp,tail);
	trim_all(tmp);
	if (*tmp == 0)
	{
		errorcode=5; // EXP_EMPTY
		return 0;
	}

	if (nlsearch)	goto nl_search_only;
	if (*tmp=='/')	strcpy(tmp,tmp+1);
	if (*tmp=='#')	// phone number ?
	{
		if (strspn(tmp,"-/$#0123456789" ALT_TEL_DELIM)<strlen(tmp))
		{
			errorcode=1; // EXP_FIDO_ILLEGAL_CHARS_INNUMBER
			return 0;
		}
		full=tmp;
		info=L("S_18",tmp);
		return 1;
	}

	if (strspn(tmp,":/.0123456789")<strlen(tmp))	// maybe name
	{
		i=0;
		while (db_get_boss_by_index(i++,bossbuf))	// check bosses names first
		{
			get_token(bossbuf,BOSSNAME,str);
			if (!stricmp(tmp,str))
			{
				get_token(bossbuf,BOSSADDR,full);
				goto full_adr;
			}
		}

nl_search_only:	//	search name in nodelist
		gustat.findresult=(_fidonumber *)malloc(1500*sizeof(_fidonumber));
		if (!gustat.findresult)	ERR_MSG_RET0("E_MEM_OUT");
		gustat.findcount=1499;
		if (nlsearch)
		{
			gpMain->BeginWaitCursor();
			if (nl_get_fido_by_partname_ext(tmp,&gustat.findcount,gustat.findresult))
			{
				gpMain->EndWaitCursor();
				usersel=1;
				goto found_any_stuff;
			}
			free(gustat.findresult);
			gpMain->EndWaitCursor();
		    return 0;
		}

		if (nl_get_fido_by_name(tmp,&gustat.findcount,gustat.findresult))
		{
found_any_stuff:
			if (gustat.findcount==1)
			{
				result=gustat.findresult;
				make_address(full,result->zone,result->net,result->node,result->point);
				free(gustat.findresult);
				gustat.findresult=0;
				goto full_adr;
			}

			if (!usersel)
			{
				free(gustat.findresult);
				gustat.findresult=0;
				errorcode=6; // EXP_NOUSERSELALL
				return 0;
			}

			nodesel tmp;
			tmp.DoModal();

			if (gustat.selected.zone!=0xFFFF)	// user selected item
			{
				make_address(full,gustat.selected.zone,gustat.selected.net,gustat.selected.node,gustat.selected.point);
				free(gustat.findresult);
				gustat.findresult=0;
				goto full_adr;
			}
			else
			{
				free(gustat.findresult);
				gustat.findresult=0;
				errorcode=7; // EXP_NO_USERSEL
				return 0;
			}
		}
		else
		{
			if (patternsearch)
			{
				if (strncmp(tmp,"::",2))
					dont_ask=get_cfg(CFG_COMMON,"NoDetSearch",0);// detailed search?
				else
					dont_ask=1;

				if (!dont_ask)
				{
					if (err_out("DY_EXTSEARCH")==IDYES)
					{
						gustat.findcount=1499;
						gpMain->BeginWaitCursor();
						if (nl_get_fido_by_partname(tmp,&gustat.findcount,gustat.findresult))
						{
							gpMain->EndWaitCursor();
							goto found_any_stuff;
						}
						gpMain->EndWaitCursor();
					}
				}
			}
			errorcode=4; // EXP_FIDO_NO_FIDO_FOR_STRING
			free(gustat.findresult);
			gustat.findresult=0;
			return 0;
		}
   }
   else
   {	// FIDO-address
		if (db_get_boss_by_index(0,bossbuf)!=DB_OK)
		{
			 errorcode=3; // EXP_FIDO_INVALID_BOSS_CFG
			 return 0;
		}
		get_token(bossbuf,BOSSADDR,bossadress);
		parse_address(bossadress,&bosszone,&bossnet,&bossnode);
// sample like ZZ:NNNN/FFFF or ZZ:NNNN/FFFF.PPPP
		zone=net=node=point=0;
		ret=parse_address(tmp,&zone,&net,&node,&point);
		if (ret>=3)
		{
		   full=tmp;
		   goto full_adr;
		}
// sample like NNNN/FFFF or NNNN/FFFF.PPPP
		net=node=point=0;
		ret=sscanf(tmp,"%d/%d.%d",&net,&node,&point);
		if (ret>=2)
		{
			make_address(full,bosszone,net,node,point);
			goto full_adr;
		}
// sample like FFFF or FFFF.PPPP
		node=point=0;
		ret=sscanf(tmp,"%d.%d",&node,&point);
		if (ret>0)
		{
			make_address(full,bosszone,bossnet,node,point);
			goto full_adr;
		}
   }
full_adr:
   if (get_nl_stuff || must_listed)
   {
		i=0;
		while (db_get_boss_by_index(i++,bossbuf))
		{
		   get_token(bossbuf,BOSSADDR,str);
	       if (!strcmp(full,str))
		   {
			   get_token(bossbuf,BOSSPHON,help2);
			   get_token(bossbuf,BOSSNAME,help3);
			   info.Format("%s,boss:%s",help2,help3);
			   return 1;
		   }
		}

		zone=net=node=point=0;
	    parse_address(full,&zone,&net,&node,&point);
		if (nl_get_fido_by_number(zone,net,node,point,&number))
		{
			char *p;
			p=strchr(number.flags,'\n');
			if (p) *p=0;
			info.Format("%s,%s,%s",number.user,number.phone,number.flags);
			return 1;
		}

		errorcode=8; // EXP_NO_INFO_IN_NL
		return 0;
   }
   info.Empty();
   return 1;
}

#define CTLESC '\\'
//	=====================================================================
// Returns true if the pattern matches the string.
	int pmatch(LPCSTR pattern,LPCSTR string)
//	=====================================================================
{
register const char *p, *q;
register char c;

	p = pattern;
	q = string;
	for (;;)
	{
		switch (c = *p++)
		{
		case 0:
			goto breakloop;
		case CTLESC:
			if (*q++ != *p++)	return 0;
			break;
		case '?':
			if (*q++ == 0)	return 0;
			break;
		case '*':
			c = *p;
			if (c != CTLESC && c != '?' && c != '*' && c != '[')
			{
				while (*q != c)
				{
					if (*q == 0)	return 0;
					q++;
				}
			}
			do
			{
				if (pmatch(p, q))	return 1;
			} while (*q++ != 0);
			return 0;
		case '[':
		{
			LPCSTR endp;
			int invert, found;
			char chr;

			endp = p;
			if (*endp == '!')	endp++;
			for (;;)
			{
				if (*endp == 0)			goto dft;		/* no matching ] */
				if (*endp == CTLESC)	endp++;
				if (*++endp == ']')		break;
			}
			invert = 0;
			if (*p == '!')
			{
				invert++;
				p++;
			}
			found = 0;
			chr = *q++;
			c = *p++;
			do
			{
				if (c == CTLESC)	c = *p++;
				if (*p == '-' && p[1] != ']')
				{
					p++;
					if (*p == CTLESC)			p++;
					if (chr >= c && chr <= *p)	found = 1;
					p++;
				}
				else if (chr == c)
					found = 1;
			} while ((c = *p++) != ']');
			if (found == invert)	return 0;
			break;
		}
dft:
		default:
			if (*q++ != c)	return 0;
			break;
		}
	}
breakloop:
	if (*q != '\0')	return 0;
	return 1;
}

// ================================================================
// remove extra white spaces
	void ltrim(LPSTR str)
// ================================================================
{
int len;
int i;
	
	if (!str)	return;
	len=strlen(str);
	for (i=0;i<len;i++)   
		if (!ISSPACE(*(str+i))) break;
	strcpy(str,str+i);
}

// ================================================================
// returns formated file size in bytes, Kbytes & Mbytes
	LPSTR fmt_size(LPSTR buf,long val)
// ================================================================
{
int i=1;

	if (val>i*1024) i=i*1024;
	if (val>i*1024) i=i*1024;
	switch (i) 
	{
	case 1:
		strcpy(buf,L("S_287",val));
		break;
	case 1024:
		strcpy(buf,L("S_288",(double)val/i));
		break;
	default:
		strcpy(buf,L("S_289",(double)val/i));
		break;
	}
	trim_all(buf);
	return buf;
}

// ====================================================================
	void load_multimed_cfg(void)
// ====================================================================
{
	gc.sound_enabled=get_cfg(CFG_MM,"PlaySound",1);
	gc.bmps_enabled=get_cfg(CFG_MM,"DisplayBmp",1);
	gc.no_error=get_cfg(CFG_MM,"WarnAbsent",0);
	gc.bmps_size=get_cfg(CFG_MM,"BmpSize",0);
}
	
// ==========================================================
	BOOL get_timezone_inf(int zone,int net,int node,int point,_tzone &tz)
// ==========================================================
{
__int64 z=zone*1000000000000;
__int64 n=net*100000000;
__int64 d=node*10000;
	
	if (Tzutc.Lookup(z+n+d+point,tz))	return TRUE;
	if (Tzutc.Lookup(z+n+d,tz))			return TRUE;
	if (Tzutc.Lookup(z+n,tz))			return TRUE;
	if (Tzutc.Lookup(n+d+point,tz))		return TRUE;
	if (Tzutc.Lookup(n+d,tz))			return TRUE;
	if (Tzutc.Lookup(n,tz))				return TRUE;
	memset(&tz,0,sizeof(_tzone));
	return FALSE;
}

// =================================================
	void load_submenu(LPSTR buf,FILE *fp,CMenu *sm)
// =================================================
{
char	strid[10],capt[100],*ch;
LPSTR	p;
int		lvl,nlvl,cmd;
CMenu	ssm;
	
	p=buf;
	lvl=0;
	while (*p && *p++==' ')	lvl++;	// count submenu level
	fgets(buf,99,fp);
	do
	{
		if (*buf==0 || *buf==';' || *buf=='\n')			goto nxt;
		if (!strnicmp(buf,"@section ",9) || *buf=='$')	return;	// end scanning
		p=buf;
		nlvl=0;
		while (*p && *p++==' ')	nlvl++;	// count submenu level
		if (nlvl<=lvl)	return;
		if (strchr(buf,'|'))
		{
			get_token(buf,0,capt,'|');
			trim_all(capt);
			replace_chars(capt,'\\','\t');
			get_token(buf,1,strid,'|');
			cmd=atoi(strid);
			if (cmd)
				sm->AppendMenu(MF_STRING | MF_ENABLED,MenuIDs[cmd-1],capt);
			else
				sm->AppendMenu(MF_STRING | MF_DISABLED);
			goto nxt;
		}
		if (strstr(buf,"---"))
		{
			sm->AppendMenu(MF_SEPARATOR);
			goto nxt;
		}
		strnzcpy(capt,buf,sizeof(capt));
		trim_all(capt);
		ssm.CreatePopupMenu();
		load_submenu(buf,fp,&ssm);
		sm->AppendMenu(MF_POPUP | MF_STRING | MF_ENABLED,(UINT)ssm.m_hMenu,capt);
		ssm.DestroyMenu();
		ch=buf;
		continue;
nxt:
		ch=fgets(buf,99,fp);
	}while(ch);
}
	
	
	BOOL load_context_menu(LPCSTR mnuid,CMenu *pm)
{
FILE	*fp;
int		cmd,scan=0;
char	sect[30],buf[100],capt[100],*ch;

	fp=fopen(FIPSMENUS,"rt"); 
	if (!fp) ERR_MSG2_RET0("E_CANLOLANF",FIPSMENUS);
	strcpy(sect,"@SECTION ");
	switch (LANG_ID)
	{
	case LANG_GER:
		strcat(sect,"GERMAN");
		break;
	case LANG_RUS:
		strcat(sect,"RUSSIAN");
		break;
	case LANG_FRE:
		strcat(sect,"FRENCH");
		break;
	default:
	case LANG_ENG:
		strcat(sect,"ENGLISH");
		break;
	}
	fgets(buf,99,fp);
	do
	{
		if (*buf==0 || *buf==';' || *buf=='\n')
			goto nxt;
		if (scan==0 && !strnicmp(buf,sect,strlen(sect)))	// language
		{
			scan=1;
			goto nxt;
		}
		if (scan==1 && !strnicmp(buf,mnuid,strlen(mnuid)))	// menu
		{
			scan=2;
			goto nxt;
		}
		if ((scan && *buf=='@') || (scan==2 && *buf=='$'))	break;
		if (scan==2)
		{
			if (strchr(buf,'|'))	// command?
			{
				get_token(buf,0,capt,'|');
				trim_all(capt);
				replace_chars(capt,'\\','\t');
				cmd=get_token_int(buf,1,'|');
				if (cmd)
					pm->AppendMenu(MF_STRING | MF_ENABLED,MenuIDs[cmd-1],capt);
				else
					pm->AppendMenu(MF_STRING | MF_DISABLED);
				goto nxt;
			}
			if (strstr(buf,"---"))
			{
				pm->AppendMenu(MF_SEPARATOR);
				goto nxt;
			}
		}
nxt:	
		ch=fgets(buf,99,fp);
	}while(ch);
	fclose(fp);
	return scan;
}

// =================================================================================================
// converts TAB to spaces and removes LF
	void expand_tabs(CString &str)
// =================================================================================================
{
CString tmp;
int cnt=0;

	for (int pos=0;pos<str.GetLength();pos++)
	{
		switch (str[pos])
		{
		case '\t':
			if (cnt && !(cnt % 8))
			{
				tmp+=' ';
				cnt++;
			}
			while (cnt % 8 || cnt==0)
			{
				tmp+=' ';
				cnt++;
			}
			break;
		case '\n':
			cnt=0;
			break;
		default:
			tmp+=str[pos];
			cnt++;
			break;
		}
	}
	str=tmp;
}

LPCSTR str_str(LPCSTR str1,LPCSTR str2,BOOL useCase)
{
int len1=strlen(str1),len2=strlen(str2);
int len=min(len1,len2);
char b1[2]={0,0},b2[2]={0,0};

	for (int j = 0; j <= len1 - len2; j++)
	{
		int i;
		for (int i = 0; i < len; i++)
		{
			if (useCase) if (str1[j + i] != str2[i]) break;
			if (!useCase)
			{
				b1[0] = toupper(str1[j + i]); b2[0] = toupper(str2[i]);
				if (b1[0] != b2[0]) break;
			}
		}
		if (i == len) return str1 + j;
	}
	return 0;
}

// ==============================================
// returns number of 'm' strings in 's' string
// ==============================================
int count_substr(LPCSTR s,LPCSTR m)
{
LPCSTR p = s;
int n = 0, ln = _tcslen(m);

	while (p=strstr(p, m))
	{
		n++;
		p+=ln;
	}
	return n;
}

BOOL FindStringHelper(CTextBuffer *pBuf,BOOL casesens,BOOL useRx,LPCSTR what,CPoint &found,int &len)
{
CString text;
RxMatchRes match_res;
CPoint pt(0,0);
int pos = -1, nEolns = 0, last_len = -1;

	len = 0;
	RxNode *rx = 0;
	if (useRx)
	{
		rx = RxCompile(what);
		nEolns = count_substr(what, "\\n");
	}
	text = pBuf->GetLineChars(pt.y) + pt.x, pBuf->GetLineLength(pt.y) - pt.x;
	while (pos == -1)
	{
		if (rx)
		{
			if (RxExec(rx, text, text.GetLength(), text, &match_res, casesens) != 0)
			{
				pos = match_res.Open[0];
				len = match_res.Close[0] - match_res.Open[0];
			}
		}
		else
		{
			char *match = (char*)str_str(text, what, casesens);
			if (match != 0) 
			{
				pos = match - text;
				len = strlen(what);
			}
		}
		if (pos == -1)
		{
			if (++pt.y >= pBuf->GetLineCount()) break;
			text += '\n';
			last_len = text.GetLength();
			text += CString(pBuf->GetLineChars(pt.y),	pBuf->GetLineLength(pt.y));
		}
	}
	if (pos >= 0)
	{
		found.y = pt.y - nEolns;
		if (nEolns == 0)
			if (last_len != -1) found.x = pos - last_len;
			else found.x = pos + pt.x;
		else
		{
			const char *pos1 = strstr(what, "\\n");
			ASSERT(pos1 != 0);
			found.x = pBuf->GetLineLength(found.y) - (int)(pos1 - what);
		}
	}
	return pos >= 0;
}

// ==============================================
// checks if now is worktime of station
	BOOL is_work_time(LPCSTR addr)
// ==============================================
{
_fidonumber fido;
time_t tfrom,tto,tnow;
tm		*pt;
int		zone,net,node,point,h,m;
char	t1[20],t2[20];

	parse_address(addr,&zone,&net,&node,&point);
	if (nl_get_fido_by_number(zone,net,node,point,&fido))
	{
		get_call_time(fido.flags,t1,t2);
		if (sscanf(t1,"%d:%d",&h,&m) == 2)
		{
			tfrom=h*60+m;
			if (sscanf(t2,"%d:%d",&h,&m) == 2)
			{
				tto=h*60+m;
				tnow=time(NULL);
				pt=localtime(&tnow);
				tnow=pt->tm_hour*60+pt->tm_min;
				if (tto >= tfrom)
				{
					if (tto >= tnow && tnow >= tfrom) 
						return TRUE;
				}
				else
				{
					if (tto <= tnow || tnow <= tfrom)
						return TRUE;
				}
			}
		}

	}
	return FALSE;
}

// ==============================================
// splits full filename to drive,path,name and extension
	void split_filename(LPCSTR fname,LPSTR drive,LPSTR path,LPSTR name,LPSTR ext)
// ==============================================
{
LPCSTR p,t;
int		i,len=strlen(fname);
BOOL	fe,fn;

	if (*fname==0)	return;
	if (drive != 0)	*drive=0;
	if (path != 0)	*path=0;
	if (name != 0)	*name=0;
	if (ext != 0)		*ext=0;
	fn=fe=FALSE;
	t=fname+len-1;
	for(i=len-1,p=t;i>=0;i--,p--)
	{
		if (*p=='.' && !fe)
		{
			if (ext != 0 && *ext==0 && path != 0 && *path==0)
			{
				strncpy(ext,p+1,t-p);
				ext[t-p]=0;
			}
			t=p-1;
			fe=TRUE;
			continue;
		}
		if (*p=='\\' && !fn)
		{
			if (name != 0 && *name==0)
			{
				strncpy(name,p+1,t-p);
				name[t-p]=0;
			}
			t=p-1;
			fn=TRUE;
			continue;
		}
		if (*p==':' && path != 0 && *path==0)
		{
			strncpy(path,p+1,t-p);
			path[t-p]=0;
			if (p-1>=fname && drive != 0)
			{
				strncpy(drive,fname,p-fname);
				drive[p-fname]=0;
			}
			break;
		}
	}
}

// ==============================================
// returns template string for defined mode and area (may use wildcards)
	void get_template(LPSTR buf,LPSTR area,BOOL part,BOOL quote)
// ==============================================
{
char path[MAX_PATH],defval[1000],mode[3],sect[100],*p;
int	 cnt;

	mode[0]=part ? 'F' : 'H';
	mode[1]=quote ? 'Q' : 'N';
	mode[2]=0;
	*sect=0;
	*buf=0;
	make_path(path,gc.IniPath,TPLFILENAME);
	cnt=GetPrivateProfileString("DEFAULT",mode,"",defval,sizeof(defval),path);
	if (strcspn(area,"\\?*[")!=strlen(area))	// wildcards?
	{
		cnt=GetPrivateProfileString(0,0,"",buf,300,path);
		if (cnt>0)
		{
			p=buf;
			while (p-buf<cnt)
			{
				if (pmatch(area,p))
				{
					strcpy(sect,p);
					break;
				}
				p+=strlen(p)+1;
			}
			if (*sect>0)
				cnt=GetPrivateProfileString(sect,mode,defval,buf,300,path);
			else
				strcpy(buf,defval);
		}
	}
	else
		cnt=GetPrivateProfileString(area,mode,defval,buf,300,path);
}

// ============================================
	void log_to_file(int logid,LPCSTR logname,LPCSTR title,LPCSTR text,BOOL &firsttime)
// ============================================
{
FILE *fp;
char log[300],tmp[300],time[100],date[100];
int  maxsize,shorten;
BOOL enabled;

	_strdate(date);
	_strtime(time);

	get_logoptions(logid,enabled,maxsize,shorten);
	if (!enabled)	return;

	make_path(log,gc.BasePath,logname);
	make_path(tmp,gc.BasePath,"fipstmp.$$$");

	if (!firsttime)	truncate_logfile(log,tmp,maxsize,shorten);
	if (!(fp=fopen(log,"at")))	return;
	if (!firsttime)
	{
		strcpy(log,gStr);
		firsttime=1;
		strcpy(tmp,title);
		strcat(tmp,L("S_508"));	// starting session
		fprintf(fp,"\n         --=====[ %s %s ]=====--\n",tmp,date);
		strcpy(gStr,log);
	}
	fprintf(fp,"%s %s\n",time,text);
	fclose(fp);
}

// ==================================================================================
	void truncate_logfile(char *logfile,char *tmpfile,int maxsize,int shorten)
// ==================================================================================
{
_finddata_t se;
char	buf[300],date[100],time[100];
FILE	*fp,*fp2;
long	hf;
int		counter=0;

	_strdate(date);
	_strtime(time);
	hf=_findfirst(logfile,&se);
	if (hf != -1L)
	{
		if ((int)se.size > maxsize*1024)
		{
			fp=fopen(logfile,"rt");
			fp2=fopen(tmpfile,"wt");
			while ((int)(se.size-counter) > shorten*1024)
			{
				fgets(buf,299,fp);
				counter+=strlen(buf)+1;
			}
			fprintf(fp2,L("S_633",date,time));	// logfile truncated
			while( fgets(buf,299,fp) != NULL )	fprintf(fp2,"%s",buf);
			fclose(fp);
			fclose(fp2);
			_unlink(logfile);
			rename(tmpfile,logfile);
		}
		_findclose(hf);
	}
}

// ==================================================================================
	BOOL IsOem(CFont *pf)
// ==================================================================================
{
LOGFONT lf;

	pf->GetLogFont(&lf);
	return lf.lfCharSet==OEM_CHARSET;
}

// ==================================================================================
	void copy_to_clipboard(LPCSTR txt)
// ==================================================================================
{
HGLOBAL hText,hLoc;
char	*pText;
LCID	*pLoc;
size_t len;
HANDLE hnd;

	if (OpenClipboard(NULL) && EmptyClipboard())
	{
		len=strlen(txt);
		if (hText=GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE,len+1))
		{
			pText= (char*)GlobalLock(hText);
			memcpy(pText,txt,len);
			pText[len]='\0';
			GlobalUnlock(hText);

			if(hLoc=GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE,sizeof(LCID)))
			{
				pLoc = (LCID*)GlobalLock(hLoc);
				*pLoc=GetUserDefaultLCID();
				GlobalUnlock(hLoc);

				hnd=SetClipboardData(CF_TEXT,hText);
				hnd=SetClipboardData(CF_LOCALE,hLoc);
				if (hLoc)	GlobalFree(hLoc);
			}
			if (hText)	GlobalFree(hText);
		}
		CloseClipboard();
	}
}

BOOL IsOem(LPCSTR str)
{
char ch[2];
int ocnt=0,acnt=0;
	
	ch[1]=0;
	for (size_t i=0;i<strlen(str);i++)
	{
		ch[0]=str[i];
		strupr(ch);
		if ((UCHAR)str[i] < 128)	continue;
		if ((UCHAR)str[i] <= 175)	ocnt++;
		if ((UCHAR)str[i] >= 240)	acnt++;
	}
	return ocnt > acnt;
}