// ================================
//    structs.h
// ================================
// This file contains common used structures
//

#ifndef structs_h
#define structs_h

#include "myowlist.h"
#include "mystrlst.h"
#include "editwnd.h"

#define MAINWINID			"IDD_MAIN_WINDOW"	// name of main window identifier
#define IMPORTANT_PREFIX	"IMPORTANT MAIL"
#define BBSMAIL             "$BBSMAILS$"
// files
#define UUMULFILE			"fpsmulti.uue"
#define INIFILENAME			"fips.ini"
#define COSTFILENAME		"costs.blg"
#define RECIPTTEMPLATE		"urcptext.txt"
#define OTHERPLACES			"addlook.txt"
#define FIPSDIALOGS			"FipsDlg.def"
#define FIPSMESSAGES		"FipsMsg.def"
#define FIPSSTRINGS			"FipsStr.def"
#define FIPSMENUS			"FipsMnu.def"
#define TPLFILENAME			"FipsTpl.def"
#define FIPSBBS				"FipsBbs.def"
#define FREQ_LIST			"files.req"
#define XFER_LIST			"xfer.fls"

#define AREABACKUP			"areaswwd.bck"
#define AREADBF	 			"areas.wwd"
#define AREADBF1	 		"areas1.wwd"
#define AREADBF2	 		"areas2.wwd"
#define AREADBF3	 		"areas3.wwd"
#define AREADBF4	 		"areas4.wwd"
#define AREADBFT	 		"areast.wwd"	// pretossing state
#define AREADBFP	 		"areasp.wwd"	// prepurging state
#define AREADBFS	 		"areass.wwd"	// afterstart state
#define EXT_HEADER	 		"hdr"
#define EXT_MESSAGE 		"mes"

#define TMP_HDR_FILE		"~hdr.tmp"
#define TMP_MSG_FILE		"~mes.tmp"
#define PRINT_FILE			"fipsmail.prn"
#define LastSucessfulCall	"lastcall.idx"
#define AUTOEXEC_FPS		"autoexec.fps"
//#define AUTOEXIT_FPS		"autoexit.fps"

#define nlidxname 			"$NDL_IDX.IDX"
#define nludxname 			"$NDL_UDX.UDX"
#define nlidxtmpname		"$IDXTMP$.DAT"
#define nludxtmpname		"$UDXTMP$.DAT"
#define IDX_NAME			"$NL_IDX$.DAT"
#define ABOOK_IDX			(UCHAR)0xFE	// index for address book
#define PRIVATE_IDX			(UCHAR)0xFF	// index for private nodes

// limits
#define MAX_MSG				32768	// maximum size of storing mail text
//#define MAX_EDIT			1000000	// maximum size of composing mail text
#define HDR_LEN				1000	// max length of kludges header (for quick in-buffer)
#define MAX_AREAS			1000    // maximum number of areas
#define MAX_SELMAILS		32768
#define MAX_PACKERS			20		// maximum number of packers
#define MAX_EVENTLOG		60		// maximum number of events
#define MAX_BOSSLEN			1000	// maximum len of boss config string
#define MAX_PACKLEN			2000	// maximum len of packer config string
#define MAX_NODELISTS		253		// maximum number of nodelists
#define MAX_AREA_FNAME		128		// maximum len of a base filename
#define MAX_NL_ENTRIES		128000	// maximum nodelist entries
#define MAX_ICONS			30		// maximum number of icons for floating toolbar
#define MAX_REQ_MAIL		64000
#define MAX_AKAS_EMSI		100
#define MAX_UPLINKS			256
#define MAX_RDBUF			300		// size of file line read buffer
//#define MAX_CONVERTS		500		// maximum converts of special chars (for german)

#define ISDNPORT			250		// BEI AENDERUNG KOPIE IN BIND.H NICHT VERGERSSEN
#define ALT_TEL_DELIM		"["		// delimiter for phone rotation
#define WHITECHARS			" \t\r\n"
#define ASAVED_GROUP		"ASAVED"
//test group
#define DEFAULT_GROUP		"FIDO"
#define ASAVED_PREFIX		ASAVED_GROUP":  "
#define ASAVED_NETMAIL		ASAVED_GROUP":  MY_NETMAILS"
#define ASAVED_ECHO			ASAVED_GROUP":  MY_ECHOMAILS"
#define REPOST_FLD			"REPOST"
#define MM_PREFIX			"FmM"
#define PS  (LPSTR)(LPCSTR)

// macroses for mail sorting
#define SORT_SENDER_INC		2
#define SORT_SENDER_DEC		3
#define SORT_RECIPIENT_INC	4
#define SORT_RECIPIENT_DEC	5
#define SORT_SUBJECT_INC	8
#define SORT_SUBJECT_DEC	9
#define SORT_CREATION_INC	16
#define SORT_CREATION_DEC	17
#define SORT_RECEPTION_INC	32
#define SORT_RECEPTION_DEC	33
#define SORT_STATUS_INC		64
#define SORT_STATUS_DEC		65

#define SORT_CREATE			1
#define SORT_RECIEVE		2
#define SORT_FROM			4
#define SORT_TO				8
#define SORT_SUBJECT		16
#define SORT_STATUS			32
#define SORT_ORDER			64

// macroses for Extended Search
#define EXS_TEXTNEEDED		0x80000000
#define EXS_FROM			0x00000001
#define EXS_TO				0x00000002
#define EXS_SUBJECT			0x00000004
#define EXS_MAILTEXT		(0x00000008	| EXS_TEXTNEEDED)
#define EXS_RECEIVE			0x00000010
#define EXS_CREATION		0x00000020
#define EXS_CONTAINS		0x00000080
#define EXS_BEGINS			0x00000100
#define EXS_YOUNGER			0x00000200
#define EXS_NOT				0x00000400
#define EXS_CASESENS		0x00000800
#define EXS_LASTCALL		0x00001000
#define EXS_SCANNED			0x00002000
#define EXS_NEW_OWN_MAIL	0x00004000
#define EXS_ALREADY_READ	0x00008000
#define EXS_NEVER_DELETE	0x00010000
#define EXS_FROZEN			0x00020000
#define EXS_MAILSTATUS		0x00040000
#define EXS_USERMARKED		0x00080000
#define EXS_MARKEDREAD		0x00100000
#define EXS_ADDRESS			(EXS_FROM    | EXS_TO)
#define EXS_HEADER			(EXS_ADDRESS | EXS_SUBJECT)
#define EXS_ALL				(EXS_HEADER  | EXS_MAILTEXT)

// macroses for events handling
#define EV_FAILED		-1
#define EV_TODO			0
#define EV_OK			1
#define EV_SLEEP		2
#define EV_OFF			3

// macroses for dialog ajusting
#define HO				1	// horizontal resizing
#define VE				2	// vertical resizing
#define HV				3	// horizontal & vertical

// macroses for events tabbed string
#define EVNT_event		0
#define IDTOlastcheck	2
#define EVNT_enabled	3
#define EVNT_from		4
#define EVNT_to			5
#define EVNT_inwait		6
#define EVNT_outwait	7
#define EVNT_maxtry		8
#define EVNT_boss1		9
#define EVNT_boss2		10
#define EVNT_boss3		11
#define EVNT_if_ok		12
#define EVNT_if_failed	13
#define EVNT_caution	14
#define EVNT_mo			15
#define EVNT_tu			16
#define EVNT_we			17
#define EVNT_th			18
#define EVNT_fr			19
#define EVNT_sa			20
#define EVNT_su			21
#define IDTOlasttouch	22
#define IDTOstatus		23
#define IDTOacttry		24
#define IDTOdialindex	25
#define IDTOlaststatus	26
#define EVNT_delete 27	// delete after success

// macroses for Incomming Call Setup  ...
#define IDINCCm_edit_from			  0
#define IDINCCm_edit_to				  1
#define IDINCCm_firstmodem			  2
#define IDINCCm_secondmodem			  3
#define IDINCCm_isdnline			  4
#define IDINCCm_service_mailbox		  5
#define IDINCCm_service_emsi		  6
#define IDINCCm_mo					  7
#define IDINCCm_tu					  8
#define IDINCCm_we					  9
#define IDINCCm_th					  10
#define IDINCCm_fr					  11
#define IDINCCm_sa					  12
#define IDINCCm_su					  13
#define IDINCCm_service_fax		      14
//#define IDINCCm_service_voice		  15
#define IDINCCm_split_enable		  16
#define IDINCCm_split_edit_from		  17
#define IDINCCm_split_edit_to		  18

#define RETVAL_BASE        			  1000000
//#define RETVAL_UNDEFINED        	  1000001
//#define RETVAL_FILENOTFOUND		  1000002
//#define RETVAL_PATH_NOTFOUND		  1000003
//#define RETVAL_WINERROR         	  1000004
//#define RETVAL_PARAMERROR       	  1000005
//#define RETVAL_SYSTEMFAILED     	  1000006
//#define RETVAL_CHANGEDIRFAILED  	  1000007
//#define RETVAL_WAITFORSINGLEOBJFAILED 1000008
//#define RETVAL_GETEXITCODE		  1000008

// Definitions for fields in boss strings
#define BOSSSYST	0
#define BOSSADDR	1
#define BOSSPHON	2
#define BOSSNAME	3
#define MAINAKA		4
#define SESSPASW	5
#define ARCHIVER	6
#define ECHOFILE	7
#define FLISTARC	8
#define FLISTUNA	9
#define NFLSTARC	10
#define NFLSTUNA	11
#define AREAFIX		12
#define AFIXPASW	13
#define AFIXPLUS	14
#define FILEFIX		15
#define FFIXPASW	16
#define FFIXPLUS	17
#define ARCMPASW	18
#define ROUTING		19
#define NOPOLL		20
#define NOTSEND		21
#define NOHYDRA		22
#define NOZMODEM	23
#define TCPIP		24

#define NL_VERSION	0

#define SUBJ_LIST	1
#define ATTR_LIST	2

#define MAIL_INDEX	5           // order of MailIndex in subject Listbox

#define	TEXT_BACKGR_COLOR	0
#define TEXT_COLOR			1
#define ODD_QUOTE_COLOR		2
#define KLUDGE_COLOR		3
#define ORIGIN_COLOR	    4
#define EVEN_QUOTE_COLOR	5
#define	SUBJ_BACKGR_COLOR	6
#define	SUBJ_COLOR			7
#define	NEST_COLOR			8
#define	WRITMAILSIZE		9
#define	BOLD_COLOR			10
#define	SUBJ_LINE_DISTANCE	17
#define	TEXT_LINE_DISTANCE	19

#define MODEM_FAIL			0
#define MODEM_OK			1
#define MODEM_TIMEOUT		2
#define MODEM_ABORTED		3
#define MODEM_CANNOT_INIT	4
#define MODEM_EMSI_FAILURE  6
#define MODEM_ERROR			9
#define MODEM_NOTFOUND		10

#define LOG_MAILER			0
#define LOG_TOSSER			1
#define LOG_PURGER			2
#define LOG_EVENTS			3
#define LOG_NL				4

#define CFG_UTILITIES		0
#define CFG_OUTBOUND		1
#define CFG_NODELIST		2
#define CFG_MESSAGEBASE		3
#define CFG_INBOUND			4
#define CFG_BASEDIR			5
#define CFG_PRENAME			6
#define CFG_ANAME			7
#define CFG_SYSTEMNAME		8
#define CFG_LOCATION		9
#define CFG_TELEPHONE		10
#define CFG_MULTIMEDIA		11
#define CFG_TICBASIC		12
#define CFG_FAXINBOUND		13

#define MATCHEQUAL 			1
#define MATCHCONTAINS		2
#define MATCHCONTAINSWORDS	3

#define NO_ENTRY_SELECTED	-1 // 99999	value for non-selected headers,footers & origins (random)

#define AUTOCREATEAD_ASAVE  4

// user messages
#define UDM_EDITEND			(WM_USER+1)	// edit session is ended
#define UDM_DOSEARCH		(WM_USER+2)	// start search in list in address selection

//#ifndef HANDLE
// #define HANDLE void *
//#endif

/*--------------------------------------------------------------------------*/
/* FIDO Message attributes (attr) (informational)                           */
/*--------------------------------------------------------------------------*/
#define MSGPRIVATE 0x0001  /* private message,          0000 0000 0000 0001 */
#define MSGCRASH   0x0002  /* accept for forwarding +   0000 0000 0000 0010 */
#define MSGREAD    0x0004  /* read by addressee         0000 0000 0000 0100 */
#define MSGSENT    0x0008  /* sent OK (remote)          0000 0000 0000 1000 */
#define MSGFILE    0x0010  /* file attached to msg  +   0000 0000 0001 0000 */
#define MSGFWD     0x0020  /* being forwarded           0000 0000 0010 0000 */
#define MSGORPHAN  0x0040  /* unknown dest node         0000 0000 0100 0000 */
#define MSGKILL    0x0080  /* kill after mailing    +   0000 0000 1000 0000 */
#define MSGLOCAL   0x0100  /* FidoNet vs. local         0000 0001 0000 0000 */
#define MSGXX1     0x0200  /*                           0000 0010 0000 0000 */
#define MSGXX2     0x0400  /* STRIPPED by FidoNet<tm>   0000 0100 0000 0000 */
#define MSGFRQ     0x0800  /* file request              0000 1000 0000 0000 */
#define MSGRRQ     0x1000  /* receipt requested     +   0001 0000 0000 0000 */
#define MSGCPT     0x2000  /* is a return receipt       0010 0000 0000 0000 */
#define MSGARQ     0x4000  /* audit trail requested     0100 0000 0000 0000 */
#define MSGURQ     0x8000  /* update request            1000 0000 0000 0000 */

#define	UDP_MODEM_LISTBOX		1
#define	UDP_UPDATE_LEFT			2
#define	UDP_UPDATE_RIGHT		3
#define	UDP_CLEAR_QUEUE			4
#define	UDP_ADD_QUEUE			5
#define	UDP_QUEUE_SEL			6
#define	UDP_RESCAN				10
#define	UDP_ABORTSESSION		11
#define	UDP_START				12
#define	UDP_REMOTEERROR			13
#define	UDP_REMOTEANSWER		14
#define	UDP_EXITFIPS			15
#define	UDP_REMSELECTIONCHANGE  16
#define	UDP_DEVICESTATUS        17

//vk: added

#define GETRECT(ctl,re)		ctl.GetWindowRect(&re); ScreenToClient(&re);
#define GETRECTID(id,re)	GetDlgItem(id)->GetWindowRect(&re); ScreenToClient(&re);
#define	ERR_MSG_RET(msg)	{ err_out(msg);	return; }
#define	ERR_MSG_RET0(msg)	{ err_out(msg);	return 0; }
#define	ERR_MSG_RET1(msg)	{ err_out(msg);	return 1; }
#define	ERR_MSG2_RET(m1,m2)	 { err_out(m1,m2);	return; }
#define	ERR_MSG2_RET0(m1,m2) { err_out(m1,m2);	return 0; }
#define	ERR_MSG2_RET1(m1,m2) { err_out(m1,m2);	return 1; }
#define LB_ERR_RET			if (sel==LB_ERR)	return;
#define	GET_SELID(id)		sel=((CListBox *)GetDlgItem(id))->GetCurSel(); if (sel==LB_ERR) return;
#define	GET_SEL(lb)			sel=lb.GetCurSel(); if (sel==LB_ERR) return;
#define SET_SELID(id,sel)	((CListBox *)GetDlgItem(id))->SetCurSel(sel);
#define SET_SEL(lb,sel)		lb.SetCurSel(sel);
#define GET_SELECT(lv)		sel=GetSelectedItem(lv); if (sel<0) return;
#define UPDATE_LB(lst,id)	lst.UpdateListBox((CListBox *)GetDlgItem(id));
#define	EXTENT_LB(id,num)	((CListBox *)GetDlgItem(id))->SetHorizontalExtent(num);
#define TABULATE_LB(id)		((CListBox *)GetDlgItem(id))->SetTabStops((sizeof(tabs)/sizeof(int)),tabs);
#define TAB_LIST(lb,ar)		lb.SetTabStops((sizeof(ar)/sizeof(int)),ar);
#define DISABLEID(id)		GetDlgItem(id)->EnableWindow(0);
#define ENABLEID(id)		GetDlgItem(id)->EnableWindow(1);
#define DISABLE(ctl)		ctl.EnableWindow(0);
#define ENABLE(ctl)			ctl.EnableWindow(1);
#define HIDEID(id)			GetDlgItem(id)->ShowWindow(SW_HIDE);
#define SHOWID(id)			GetDlgItem(id)->ShowWindow(SW_SHOWNORMAL);
#define HIDE(ctl)			ctl.ShowWindow(SW_HIDE);
#define SHOW(ctl)			ctl.ShowWindow(SW_SHOWNORMAL);
#define BTOS(i)				(i ? "1" : "0")
#define STOB(s)				(strcmp(s,"1") ? 0 : 1)
#define SET_MARK(s)			(s ? "+" : "")
#define GET_MARK(s)			(s[0]=='+' ? 1 : 0)
#define ISSPACE(c)			(c==' '||c=='\r'||c=='\n'||c=='\t')
#define ALLOC(buf,size)		buf=(char*)malloc(size);if (buf==0)	ERR_MSG_RET("E_MEM_OUT");
#define ALLOC0(buf,size)	buf=(char*)malloc(size);if (buf==0)	ERR_MSG_RET0("E_MEM_OUT");

#define CC_ENG			"CarbonCopy from area "
#define	CC_RUS			"Копия из области "
#define DELIMS			" .,;:!?~<>[](){}'\"\r\n"
#define ATTRS			"_/*#"
#define AREACHARS		"._*/-!$&:"

enum LangID 
{
	LANG_ENG,
	LANG_GER,
	LANG_RUS,
	LANG_FRE
};

struct _tzone {		// struct for time zone information
	char city[50];
	int winter;
	int summer;
};

struct	_listcol	// init ListCtrl columns struct
{
	int	 width;
	char hdr[100];
};

struct	_flistcol	// init FListCtrl columns struct
{
	int	 width;
	char hdr[100];
	int	 align;
};

struct	_packer		// packer info
{
	char name[20];
	char pack_cmd[MAX_PATH];
	char upck_cmd[MAX_PATH];
	char sign[10];	// -1 if position is ignored
};

struct _fu_rec	// structure of index of usernames
{
	char name[40];
	USHORT  zone,net,node,point;
	int		filepos;
	UCHAR	nl_idx;
};

struct _fn_rec	// structure of index of fido-numbers
{
	USHORT  zone,net,node,point;
	int		filepos;
	UCHAR	nl_idx;
};

struct _nl_idx	// structure of nodelist-index
{
	char	filename[300];
	int		pointzone;
	int		nltype;
};

struct _nl_rec_temp // temporary structure for sorting in extensive search
{
	int nlidx;
	int filepos;
	int recnumber;
};

#define TOPIH	26	// top icon height
#define TOPIW	26	// top icon width
#define BARIH	26	// top icon height
#define BARIW	37	// top icon width

// INI sections
#define CFG_COMMON	0
#define CFG_EDIT	1
#define CFG_TEXTIMP	2
#define CFG_COLORS	3
#define CFG_PURGER	4
#define CFG_MM		5
#define CFG_PRINT	6
#define CFG_BITMAP	7
#define	CFG_ESEARCH 8
#define CFG_TIMEOUT 9
#define CFG_SCRIPTS 10
#define CFG_TOSSER	11
#define CFG_DIALOGS	12
#define CFG_BOSSES	13
#define CFG_SEARCH	14
#define CFG_PACKERS	15
#define CFG_PREFIX	16
#define CFG_LOGS	17
#define CFG_SOUNDS	18
#define CFG_INCOMIN	19
#define CFG_FONTS	20
#define CFG_HWARE	21
#define CFG_MAILED	22

// Structures

struct _magicid
{
  long affe1;
  long affe2;
  long count1;
  long count2;
};

struct _DlgItemsSize
{
	int		id;
	RECT	orgrect;
	RECT	tomove;
	int		top;
	int		left;
	int		right;
	int		bottom;
	int     mustmove;
};

struct _costinfo
{
	long	time;
	float	cost;
	int		type;
	char	fido[20];
	int		dummy1;
	int		dummy2;
};

struct _udp_packet
{
	long	command;          // Welches Kommando
	long	subcommand;       // Welches Unterkommando
	char	data[100];        // Die zu versendenden Daten
};

struct evstruct
{
	int  enabled;
	char event[100];
	char boss1[30];
	char boss2[30];
	char boss3[30];
	char from[30];
	char to[30];
	char condition[100];
	char if_ok[300];
	char if_failed[300];
	int	 check_mo;
	int	 check_tu;
	int	 check_we;
	int	 check_th;
	int	 check_fr;
	int	 check_sa;
	int	 check_su;
	int	 del_after;
	int	 maxtry;
	int	 acttry;
	int	 inwait;
	int	 outwait;
	int  dialindex;
	long lasttouch;
	int  lasttouchstatus;
	int  status;
};

struct _rfilter
{
	int  Inverse;
	int  OnNoFilter;
	int  OnCInFrom;
	int  OnCInSubject;
	int  OnCInTo;
	int  OnNoReply;
	int  OnNotOlder;
	int  OnOutgoing;
	int  OnUnread;
	int  OnSinceLastPoll;
	char to		[40];
	char subject[40];
	char from	[40];
	char days	[40];
	int  OnMarkedOnly;
	int  OnFrozenOnly;
	int  OnHeaderOnly;
	char header	[40];
	int  ShowLastOnly;
	char lastonly[40];
};

struct _tmpfile
{
	char filename[30];
	int	 timestamp;
	int dummy1;
	int dummy2;
	int dummy3;
	int dummy4;
};

// used in tosslib for inbound and outbound history
struct _sessiondata
{
	time_t  starttime;
	time_t  endtime;
	float cost;
	char  system		[300];
	char  user			[300];
};

// structure for Nodelist info
struct _fidonumber
{
	char user[40];
	char bbsname[40];
	char location[40];
	char phone[40];
	char flags[80];
	USHORT zone;
	USHORT net;
	USHORT node;
	USHORT point;
};

struct _fidouser
{
	USHORT zone;
	USHORT net;
	USHORT node;
	USHORT point;
	char user[36];
};

/*typedef struct _mainconf
{
	long structlen;				// length of structure
	char ownpath	[MAX_PATH]; // basic path
	char msgbase	[MAX_PATH];	// messagebase path
	char util		[MAX_PATH];	// utilities path
	char nodelist	[MAX_PATH];	// nodelist stuff path
	char inbound	[MAX_PATH];	// inbound path
	char outbound	[MAX_PATH];	// outbound path
	long duperecord; 			// Wieviele dupes sollen aufgehoben werden
} *pmainconf;

#ifndef IN_DATABASE
	extern struct _mainconf mainconf;
#endif*/

struct _modem_inits
{
	char name[40];
	char detect[40];
	char response[40];
	char ini[3][40];
	char dial[20];
	char answer[20];
};

struct _modemcfg 				       // MODEMCFG Struct
{
	int  m_comport;
	int  m_baud;
	int  m_data;
	int  m_stop;
	char m_parity;
	int  m_flow;
	char m_answer[100];
	char m_dial[100];
	BOOL m_useinit1;
	char m_init1[100];
	BOOL m_useinit2;
	char m_init2[100];
	BOOL m_useinit3;
	char m_init3[100];
	BOOL m_usecmd1;
	char m_predial1[100];
	BOOL m_usecmd2;
	char m_predial2[100];
	BOOL m_usecmd3;
	char m_predial3[100];
};

struct _repost
{
	char	echotag[100];
	UINT	period;
	time_t	starttime;
	int		reserved1;
	int		reserved2;
};

typedef struct areadef
{
	long index;                     // Index for simple identify
	long structlen;					// length of structure
	long status;					// diverse Status Information
	char description[129];			// area description
	char echotag[101];          	// echotag
	char filename[MAX_AREA_FNAME];  // Basis name without path and extension
	long lastread;					// line index of last read mail
	HANDLE hheader;					// File Pointer to header file
	HANDLE hmessage;				// File Pointer to message file
	char aka[21];					// AKA
	char uplink[31];				// Uplink
	char group[11];				    // Group
	int  membership;                // Base Group of this Area
	int  purge_age_create;          // autopurge setting
	int  purge_age_recipt;          // autopurge setting
	int  purge_max_num;             // autopurge setting
	int  purge_flag_create;         // autopurge setting
	int  purge_flag_age_recipt;     // autopurge setting
	int  purge_flag_nrmails;       	// autopurge setting
	int  number_of_mails;			// total number of mails
	int  number_of_read;			// number of read mails
	long localmail;                 // Never export this area
	int  additional_days;           // Add days for non read mails before purge
	long area_creation_date;        // creation date for area
	long udefindex;                 // NEW: index in userdef list, -1 - if not in list
	long last_mailindex;			// NEW: last viewed mail
	long delflag;                   // deleted flag
	long header_new;                // header index for new mail
	long header_quote;				// header index for answers
	long footer_new;				// footer index for new mail
	long footer_quote;				// footer index for answers
	long origin_new;				// origin index for new mail
	long origin_quote;				// origin index for answers
	long dummy12;                   //
	long dummy13;                   //
	long dummy14;                   //
	long dummy15;                   //
	long affe1;                     //  OLD dummy16
	long affe2;                     //	OLD dummy17
	long count1;                    //	OLD dummy18
	long count2;                    //	OLD dummy19
} *pareadef;

typedef struct mailheader
{
	char subject [72];	// Subject line
	char datetime[20];	// Date
	char toname  [36];	// Recipient Name
	char fromname[36];	// Sender Name
	long structlen;			// length of structure
	long status;				// diverse Status Information
	ULONG mailid;       // wird vom tosser geholt und gesetzt.
	time_t recipttime;		// date when Mail was tossed
	long offset;				// Byte Offset for text in file
	long text_length;		// length of mailtext
	ULONG index;				// Index of this Header
	USHORT filler;			// some stuff from the packet header
	USHORT attrib;
	USHORT cost;
	USHORT srczone;
	USHORT srcnet;
	USHORT srcnode;
	USHORT srcpoint;
	USHORT dstzone;
	USHORT dstnet;
	USHORT dstnode;
	USHORT dstpoint;
	ULONG replyid;      // Reply ID (Falls vorhanden), wird vom Tosser gesetzt
	long nestlevel;     // Temporary Variable with Nesting Level in struct display
	long unixtime;      // DateTime im UNIX Format
	long zone_net;      // Upper 16 Bit contains Zone, lower 16 Bit contains net
	long node;					// Contains node (for Mail with clear Flag DB_MAIL_ROUTED)
	LPSTR mail_text;		// Pointer to Mailtext, May be NIL
} *pmailheader;


struct _watchmail
{
	ULONG index;
	ULONG mailid;
	int	dispoff;
	int isCrossfind;
	struct mailheader mh;
};

struct _top
{
	int text_from;
	int text_to;
	int text_subject;
	int from;
	int to;
	int subject;
	int create;
	int receive;
	int from_address;
	int to_address;
	int city;
	int show_attr;
	int show_hidden;
	int show_deleted;
};

struct _purger					// struct of Purger state
{
	int  running;					// is this Thread running
	int	 area_index;			// index of purged area, or -1 for all areas
	int  text_only;		    // Update Text only
	int  mails_total;			// Mails Total to scan
	int  mails_removed;		// number of removed mails
	int  bytes_total;			// total scanned bytes
	int  bytes_removed;		// number of removed bytes
	int  area_counter;		// Area that is actually purged
	int  number_of_areas;	// number of purged areas
	BOOL breakit;					// user break
};

struct _script				// structure for holding script engine status
{
	int  running;			// is this Thread running
	int  abort;			    // abort a running script
	int  disableupdate;		// Screen Update disabled ?
	char scriptname[100];   // The name of the script to run
};

struct _nlcomp				// structure for holding nodlist compiler status
{
	int  running;			// thread is running
	int  checking;			// checking is running
};

struct _tosser				// structure for holding tosser status
{
	void *thisptr;          // Handle of Dialog Window
	int  running;			// is this Thread running
							// special:      0 not running
							//               1 FIDO Tosser Running
							//               2 NEWS Tosser Running
	char m_status[50];		// actual Status
	int  m_packets;			// Number of packets
	int  m_mails;			// Total tossed mails
	int  m_bytes;			// Bytes of tossed mails
	int  m_twits;			// number of twits
	int  m_carbons;			// number of carbon copies
	int  m_netmails;		// number of netmails
	int  m_echomails;		// number of echomails
};

struct _detincome			// Struktur enthaelt den Status des MAILER's
{
	void *thisptr;          // Handle of Dailog Window
	int  running;			// is this Thread running
	HANDLE EventToKillWaitCommEvent;    // Dieser Event wird signalled bei FIPS Ende ..
	HANDLE EventWaitCommEvent1;			// Dieser Event	wird signalled bei einem COMM Event auf dem first modem port.
	HANDLE EventWaitCommEvent2;			// Dieser Event	wird signalled bei einem COMM Event auf dem second modem port.
	HANDLE EventWaitCommEventISDN;		// Dieser Event	wird signalled bei einem ISDN Event.
	int		MailerNeedsModem;           // Der Mailer haette gern die Communication Devices ..
	int		OllisTurn;					// Olli hat einen COM Port ...
	int		WaitingForMin1Device;		// Wir warten auf mind. ein Device ...
	int		InModemEmsiSession;		    // Wir befinden uns in einer outgoing_emsi_session

};

struct _mailer				// Struktur enthaelt den Status des MAILER's
{
	void *thisptr;          // Handle of Dialog Window
	int  running;			// is this Thread running
	int  keyboard_break;	// User signalled a break
    int  skip_only;         // Skip or Abort
	int  m_timeelapsed;		// elapsed outgoing time
	int  m_timeelapsed2;	// elapsed incoming time
	char m_status[150];		// Mailer's status such as STARTING, DIALING, CONNECTED
	char m_system[150];		// The system connected to
	char m_sysop[150];		// The sysop connected to
	char m_location[150];	// The location connected to
	char m_filename[150];	// outgoing File
	char m_filename2[150];	// incoming File
	int  m_cps;				// current outgoing CPS
	int  m_cps2;			// current incoming CPS
	int  m_transferred;		// Bytes transferred out
	int  m_transferred2;	// Bytes transferred in
	int  m_total;			// total bytes to transfer out
	int  m_total2;			// total bytes to transfer in
	int  m_proz;            // procent outgoing status [0-100%]
	int  m_proz2;           // procent incoming status [0-100%]
	char m_charge[150];     // cost info
	char protocol[50];		// protocol info
	int	 skip_file;			// skip-action: =0 - none, =-1 - skip, =-2 - refuse
};

//struct _detwndstatus
//{
//	int  mail_valid;		// Window Position des MAILER Windows gueltig ?
//	int  toss_valid;		// Window Position des TOSSER Windows gueltig ?
//	int  purg_valid;		// Window Position des PURGER Windows gueltig ?
//	int  search_valid;		// Window Position des SEARCH Windows gueltig ?
//    RECT mail_wnd;
//    RECT toss_wnd;
//    RECT purg_wnd;
//    RECT search_wnd;
//};
//
struct _gconfig					// contains all stuff we probably want to save to disk
{
	int  scale;						// current scale for main dialog
	int  split_point;			// zero point for screen splitting
	BOOL nlwarning;				// old-NL warning only once while session
	BOOL scaleupdate;		// repainting after change of scale
//  int  newmail_mode;	// new/quote mail modes
//	int  quote_mode;		// quote mode
//  int  change_mode;		// change mode
//  int  forward_mode;	// forward mode
	int	 mode;						// mail editing mode
	BOOL block_status;		// blocking output to statusbar
// display configs
	int  immediate_update;		// immediate update
	int  hide_leading_blank;	// Hide leading blank lines
	int  golded_compatible;		// Golded compatible cursor keys
	int  hide_deleted;  		// Don't display DELETED MAILS
	int  show_tags;				// Show stuff begining with ASCII 0x1
	int  show_seenby;			// Show SEEN_BY Lines
	int  first_non_read;		// show the first non_read or the last-read
	int  autoscroll;			// autoscroll subject list
	int  colored;				// colored display
	int  altcolored;			// alternate-colored quotes
	int  structured_display;	// Structured Subject Display enabled ?
	int  display_attributes;	// Attribute extensions enabled
	int	 clear_tearline;		// Clear tearline
	char tearline[100];			// tearline
	int  display_this_mail_only; // Display only until mail changed
	BOOL NoTooltips;			// no tooltips
	BOOL MinToTray;				// minimize to tray
	int  show_result;			// Display actual search pattern
	int  next_find;				// force closing search dialog
	char search_pattern[100];
	BOOL inheader;
	BOOL ignorecase;
	BOOL inmessage;
	BOOL searchflag;
	BOOL userconv;						// use convertion in search
	BOOL disablectl;					// temporary disabled control key (for switched hotkeys)
// commandline flags
	BOOL init;								// initialization
	BOOL nosplash;						// skip splash screen
    int  displaylevel;			// Level of error display
	BOOL UseRecycled;					// use recycling bin
	BOOL ReadOnly;						// work with read-only mailbase
	char autoscript[MAX_PATH];// autostart script
	
	int  selected_area;				// handle of area selected in the dialog, or -1 if none
  int  new_header_index;		// new Header Index
  int  new_footer_index;		// new Footer Index
  int  HideIcons;						// Hide Icons
	int  quickexit;						// quick exit
	int  exitincomingemsi;		// quick exit from EMSI-session
	int  ineventaction;				// Event in action ?
	int  eventsenabled; 			// Events enabled ?
	int  restartevents;				// Muessen wir den EvenWorkerThread zuruecksetzten ?
	int  InEventDialogBox;		// Sind wir grad in der Setup DialogBox ??
	int  NotBremseOn;					// NotBremse Ein ?
	int  NotBremseActive;			// NotBremse _gerade_ jetzt activ ?
	int  ExitEventThread;			// Immediate exit eventworkerthread ?
	int  actual_event_index;	// Der aktuell bearbeitete EventIndex
	int  no_event_update;			// Do not update this Event because it is deleted
	int  abort_current_event;	// User pressed abort button in events ...
	int  ExitFips;						// exit signal from script
	int  SetOnFipsExit;				// Wird beim Fips Verlassen gesetzt damit der SearchWorker Thread ...
	int  SearchThreadRunning;	// Der SearchThread rennt grad
	_top top;									// screen top position of main dialog elements
	char changed[300];			// Der zu editiernde String
	char filelist[100];			// filelist for filerequest dialog
	char AddressForInfo[100];	// address for search
	_tosser tosser;						// struct of TOSSER stuff
	_purger purger;						// struct of PURGER stuff
	_mailer mailer;						// struct of MAILER stuff
	_script detscript;				// struct of running Script stuff
	_nlcomp detnl;            // struct of Nodelist Compiler stuff
	//_detwndstatus detwndstatus; // Die Positionen der 'kleinen' Fenster
	int    areasetup_active;  // AreaSetup dialog is open
	int    writemail_active;	// Sind wir bereits in der Writmail ??
	int    no_dblclick;				// block Double CLick handling in subjects list
	int    isrot13;           // ROT13 Display Mode
	int    DisplayUserdefArea;// Anzeige als USERDEF Area
	int    WatchDisplayMails; // Mails checken die dargestellt werden ...
	int	   sound_enabled;			// play Sound
	int	   bmps_enabled;			// show Bitmaps
	int	   bmps_size;					// Bitmaps size
	int	   sound_already_done;// Wurde fuer die aktuelle Mail breits ein Snd gespielt ?
	int	   bmps_already_done;	// Wurde fuer die aktuelle Mail breits das BMP gepaintet ?
	int	   no_error;					// ...
	int	   disable_insmm_items; // Einfuegen etc ausschalten ...
	int	   save_posnow;			// Position noch nicht abspeichern
	int	   last_scale;			// Der letzte, nicht 100% Scale
	int	   RequestFromMenu;     // Adress Info sofort ausfuehren ?
	int	   DisableRemote;       // Disable Remote Aktion
	int    LastUDPCommand;      // unix_time des letzten UDP Commands ..
	int    ErrMsgBoxRemoteResponse;      // Response auf remote error Box ..
	int    tcpipLoadError;		// Lade Fehler  ...
	char   UUfilename[MAX_PATH];// uu-encoded filename
	int	   NumberOfSections;	// number of uu-encoded sections
	int	   MaxUULines;			// max uu-lines in section
	int	   EditorWidth;			// width of editor window in chars
	int    isLongUUstuf;        // multi-section binary data (larger than 15K)
	char   *LongUUBasePointer;  // pointer to uu-buffer
	char   *UUBasePointertmp;	// work pointer to uu-buffer
	int    LongUUstufAddlines;  // Number of lines in additional buffer
	int    LongUUstufNrBytes;   // Number of Bytes additional buffer
	int	  isMultipleSelektionMode;      // ....
	int	  AreaSelectionMode;			// lock changes while in area selection
	int	  SuppressModeSwitch;			// kein 100% 0% switch ...
	int	  UpdateOnCancel;
	int	  LocalBBSLogin;				// Ein lokaler Login erwuenscht ...
	char  AutoSaveDestArea[100];		// destination area for Autosave
	int   ManualAccept;					// modem config for manual acception
	int   WeComeFromEventScript;        // RESCAN von einem Event aus
	int   LastBlinkState;				// Der letzte Stand der ROLLEN Blinktaste
	int   ShouldMarkAsAnsweredOnSave;	// ...
	BOOL	asel_as_list;				// show area select dialog as listbox
// config settings
	int		NoBlinkScrollLock;
	BOOL	RepeatQuote;				// repeat quote on breaked lines
	BOOL	MultiSpace;					// multiselect in navigation by SPACE
	BOOL	NdlPrefer;					// prefer nodelist in search location
	BOOL	DefaultConv;				// default convertion of national chars
	BOOL	ViewOem;					// display mailtext in OEM-coding
	int		HiddenMode;					// show or hide kludges
	int		SortCriteria;
	BOOL	SortOrder;

	char	IniPath[MAX_PATH];		// path to fips.ini
	char	BasePath[MAX_PATH];
	char	MsgBasePath[MAX_PATH];
	char	OutboundPath[MAX_PATH];
	char	InboundPath[MAX_PATH];
	char	MultimedPath[MAX_PATH];
	char	NodelistPath[MAX_PATH];
	char	UtilPath[MAX_PATH];
	char	FirstName[100];
	char	SecondName[100];
	char	ScriptName[MAX_PATH];		// name of a script, that will be edited
	BOOL    askforreallycancel;
};

struct _ustat			    // Holds current user interaction status like current sel area etc.
{
	long		cur_area_handle;		// Current selected area
	long		dest_area_handle;		// Destination Area Handle for newmail
	long		ar2han;		        	// Database handle to actual area
	areadef		act_area;	    // Definition Strukture fuer die aktuelle Area
	mailheader	act_mailh;	// Mailheader der aktuellen Mail
	int			already_one_valid_mail; // There was already one mail displayed: need for disp update!

	_fidonumber *findresult;	// Pointer auf Result nach einer Nodelist Search
	int			findcount;				// Anzahl der gefundenen Eintraege
	_fidonumber selected;     // Der vom User Selektierte FIDO NODE
	int			lastfindoffset;      	//      Offset of last find
	int         first_non_read_mail;    // Die erste noch nicht gelesene Mail in einer Area
	int         last_already_read_mail; // Die letzte bereits gelesene Mail in einer Area
};

struct _curmail
{
	char phone[300];			// The PHONE Number
	char basedir[300];			// The basedir
	char passwd[30];			// The Session Password
	char myaka[30];             // my main aka for this poll
};


// Error return codes

// ERRORRETURNCODE    !!Do not change this comment line	!!
#define DB_OK						1	// All OK
#define DB_AREAFILE_NOT_FOUND		10	// Cannot find area description database file
#define DB_OUT_OF_MEMORY			11	// Out of Memory
#define DB_INVALID_AREA_SIZE		12	// Invalid size of area description database
#define DB_WRONG_AREADDEF_VERSION	13	// Sizeinfo in structure does'nt match
#define DB_NO_SUCH_INDEX			14	// An Area with this index does not exist
#define DB_WRITE_AREA_FAILED		15	// Writing to the area description database failed
#define DB_AREA_INDEX_NOT_FOUND		16	// This areaindex cannot be found
#define DB_AREA_CANT_CREATE		    17	// Cannot create empty area database
#define DB_FILE_OPEN_ERROR			18	// cannot open area specific database file
#define DB_WRONG_MAILHEADER_SIZE	19	// Mailheader structure len is not compatible
#define DB_MESSAGE_WRITE_FAILED		20	// Write to message file failed
#define DB_HEADER_WRITE_FAILED		21	// Write to header file failed
#define DB_HEADERFILE_EMPTY			22	// There are no valid headers in the headerfile
#define DB_CANNOT_CREATE_TEMP		23	// Cannot create temp file
#define DB_WRITE_ERROR_TEMP_FILE	24	// Write error on temp file
#define DB_CANNOT_DELETE_HEADER		25	// Cannot delete old Header  database
#define DB_CANNOT_DELETE_MESSAGE	26	// Cannot delete old Message database
#define DB_CANNOT_RENAME_HEADER		27	// Cannot rename old Header  database
#define DB_CANNOT_RENAME_MESSAGE	28	// Cannot rename old Message database
#define DB_SEEK_FOR_HEADER_FAILED	29	// SEEK for a header index failed
#define DB_HEADER_READ_FAILED		30	// Mailheader read failed after SEEK
#define DB_INDEX_HEADER_NEXIST		31	// This mailheader is marked as invalid
#define DB_SEEK_FOR_MESSAGE_FAILED	32	// SEEK for message failed
#define DB_MESSAGE_READ_FAILED		33	// Message READ failed
#define DB_SEEK_FOR_DELETE_FAILED	34	// SEEK failed for header deletion
#define DB_HEADERR_ON_DELETE		35	// Read header failed on deletion
#define DB_WRITE_ERROR_ON_HDR       36	// Write error header (deletion)
#define DB_AREA_NAME_NF				37	// Area Name not found
#define DB_CREATE_NAME				38	// Cannot find a name for new area
#define DB_AREA_ALREADY_LOCKED		39	// This Area is already LOCKED
#define DB_NO_SUCH_BOSS		        40	// Specified boss cannot be found
#define TOSS_OK						1000     // All OK
#define TOSS_NO_VALID_PACKET        1010     // No valid packet
#define TOSS_INVALID_HEADER         1011     // Invalid header
#define TOSS_FILE_NOT_FOUND         1012     // File not found
#define TOSS_NO_ARCHIVE             1013	 // No archive
#define TOSS_UNKNOWN_PACKER         1014	 // Unknown Packer
// ERRORRETURNEND    !!Do not change this comment line !!

#define DB_DELETED					0x0001   // This structure is marked for deletion
#define DB_MARKED_FOR_PURGE			0x0002   // Area marked for purging
#define DB_AREA_LOCKED		        0x0004   // This area is actually locked
#define DB_MAIL_READ		        0x0008   // Mail already read
#define DB_AREA_MODIFIED            0x0010   // Area must be SCANNED
#define DB_MAIL_CREATED			    0x0020   // A new created MAIL
#define DB_MAIL_SCANNED			    0x0040   // The SCANNER has already seen this mail
#define DB_NEVER_DELETE		        0x0080   // Never delete this MAIL
#define DB_MAIL_ROUTED		        0x0100   // send to Boss, otherwise - to Point direct
//#define DB_DUPE_MAIL                0x0200   // Mail is a DUPE
#define DB_FROZEN_MAIL              0x0400   // frozen Mail
#define DB_USERMARKED				0x0800   // user marked Mail
#define DB_ANSWERED					0x1000   // this Mail have been answered
#define DB_CONVERTED			    0x2000   // Message File of Area was converted

#define MODE_NEW					1
#define MODE_QUOTE					2
#define MODE_CHANGE					3
#define MODE_FORWARD				4
#define MODE_CROSSPOST				5

// combined flags
#define DB_NOTSCAN	(DB_MAIL_SCANNED | DB_FROZEN_MAIL | DB_DELETED) // not scan this mail

typedef struct
 {
	 USHORT    filler;
	 USHORT    srcnode;
	 USHORT    dstnode;
	 USHORT    srcnet;
	 USHORT    dstnet;
	 USHORT    attrib;
	 USHORT    cost;
 } _msghdr;


typedef struct  {     /* FSC-0045 */
    USHORT
        onode,
        dnode,
        opoint,
        dpoint;
    char
        zeros[8];
    USHORT
        subver,       /* 2 */
        version,      /* 2 */
        onet,
        dnet;
    char
        product,
        rev_lev,
        password[8];
    USHORT
        ozone,
        dzone;
    char
        odomain[8],
        ddomain[8];
    long
        specific;
} NEWPKTHDR;

typedef struct {               /* close to stoneage =58 bytes */
	USHORT
        orig_node,             /* originating node */
        dest_node,             /* destination node */
        year,                  /* 1989 - nnnnn */
        month,
        day,
        hour,
        minute,
        second,
        rate,                  /* unused */
        ver,                   /* 2 */
        orig_net,              /* originating net */
        dest_net;              /* destination net */
	 char
        product,
        rev_lev,               /* revision level */
        password[8];
	 USHORT
        qm_orig_zone,
        qm_dest_zone;
	 char
		domain[8];
	 USHORT
        orig_zone,             /* originating zone */
        dest_zone,             /* destination zone */
        orig_point,            /* originating point */
        dest_point;            /* destination point */
     long
        pr_data;
} OLDPKTHDR;

typedef struct {               /* FSC-0039 */
	USHORT
        orig_node,             /* originating node */
        dest_node,             /* destination node */
        year,                  /* 1989 - nnnnn */
        month,
        day,
        hour,
        minute,
        second,
        rate,                  /* unused */
        ver,                   /* 2 */
        orig_net,              /* originating net */
        dest_net;              /* destination net */
	 char
        product,
        rev_lev,               /* revision level */
        password[8];
	 USHORT
        qm_orig_zone,
        qm_dest_zone;
	 char
        filler[2];
     USHORT
        capword2;
     char
        product2,
        rev_lev2;
     USHORT
        capword,
        orig_zone,             /* originating zone */
        dest_zone,             /* destination zone */
        orig_point,            /* originating point */
        dest_point;            /* destination point */
     long
        pr_data;
} MEDPKTHDR;

typedef struct 
{               /* FSC-0048 */
	USHORT
        orig_node,             /* originating node */
        dest_node,             /* destination node */
        year,                  /* 1989 - nnnnn */
        month,
        day,
        hour,
        minute,
        second,
        rate,                  /* unused */
        ver,                   /* 2 */
        orig_net,              /* originating net */
        dest_net;              /* destination net */
	 char
        product,
        rev_lev,               /* revision level */
        password[8];
	 USHORT
        qm_orig_zone,
        qm_dest_zone,
        aux_net;
     USHORT
        capword2;
     char
        product2,
        rev_lev2;
     USHORT
        capword,
        orig_zone,             /* originating zone */
        dest_zone,             /* destination zone */
        orig_point,            /* originating point */
        dest_point;            /* destination point */
     long
        pr_data;
} MEDPKTHDR2;

 struct _emsi_dat
{
	char bbs[800];
	char sysop[800];
	char phone[800];
	char site[800];
	int  zone,net,node,point;
};

 struct _emsi_dat_host
{
	char bbs[800];
	char pwd[160];
	char mailer[800];
	char sysop[800];
	char adr_list[800];
	char phone[800];
	char site[800];
	int  zone,net,node,point;
};

// Prototypes

//  AREA description functions
int  db_open_area_descriptions	(int create_if_non_existent);
void db_close_area_descriptions	(void);
int  db_get_area_by_index		(long ind,pareadef pa);
int  db_get_area_by_name		(LPCSTR areatag,pareadef pa=NULL);
int  db_append_new_area			(pareadef pa,long *hnd,LPCSTR uplink=NULL,BOOL from_cfg=0);
int  db_change_area_properties	(pareadef pa);
int	 db_lock_area				(int ind);
int  db_unlock_area				(int ind);
int  db_get_count_of_areas		(int *count);
int  db_set_area_state			(long hnd,int flag);
int  db_get_area_state			(long hnd);
void db_set_area_to_read		(long ind);
int  db_get_uplink_for_area		(long hnd,LPSTR boss, LPSTR point);
int  db_create_default_areas	(void);

// Header only functions
int  db_getfirsthdr				(long hnd,pmailheader pmh);
int  db_getnexthdr				(long hnd,pmailheader pmh);
int  db_gethdr_by_index 		(long hnd,long ind,pmailheader pmh);
int  db_sethdr_by_index 		(long hnd,long ind,pmailheader pmh,int dont_update_time=0);
int  db_getallheaders			(long hnd,mailheader **ppmh,int *count,BOOL tzc=FALSE);

// Message DB open and close functions
int  db_get_area_handle			(LPCSTR echotag,long *pha,int create_if_new,LPCSTR uplink=NULL);
void db_close_all_areas			(void);

// Message access functions
int  db_append_new_message	(long handle,pmailheader pheader);
int  db_purge_area					(long handle);
int  db_delete_message      (long handle,pmailheader pheader);
int  db_get_mailtext				(long handle,pmailheader pheader,LPSTR text_storage,long maxlen);

// Directory functions
int  db_get_area_counters		(int index,int *gesamt,int *already_read);
int  db_refresh_areas_info		(void);
int	 db_refresh_area_info		(long ind);
void db_inc_read_counter		(long ind,int num=1);
int  db_open_simple				(long ind);
int  db_find_area_description	(LPCSTR areatag,LPSTR areadesc);

// Boss list functions
void db_open_boss_descriptions	(int force=0);
int	 db_get_boss_by_index		(long index, LPSTR erg);
int	 db_setnew_mailtext			(long hnd,long index, pmailheader pheader);
void db_set_area_to_carboncopy	(long hnd);
void db_init_create_date		(void);

// utility functions
void create_path				(LPCSTR path);
int  should_save_this_message 	(mailheader *mhdr,areadef *paloc,int total,int already_removed);
int  flush_areabase_to_disk		(void);
int  get_error_string			(int errornumber,char	 *buf);
int  create_new_areaname		(LPSTR areaname);

// Threaded actions
void purge_one_or_more_areas	(int from,int to);
UINT mailslot_worker_thread		(LPVOID dummy);

int	 autosave_mail				(void);
void tossing_proc				(void);
int  expand_template			(LPSTR format,LPSTR expanded,mailheader *mph);
int  outgoing_emsi_session		(LPCSTR phone,LPCSTR pwd,LPSTR dir,LPCSTR mainaka,int nopoll,int nohydra,int nozmod);

long parse_time					(LPCSTR time,BOOL lang=FALSE);
//void focus_was_set_to_edit		(void);
int  scan_database				(void);
void make_emsi_dat				(LPSTR pak,LPCSTR pwd,LPCSTR mainaka,int ISDN,int nohydra,int nozmod);
void db_build_akas				(LPSTR buf,BOOL extra=TRUE);
LPSTR build_fido_time			(LPSTR buf);

//  Nodelist stuff
int	 nl_compile					(void);
int	 nl_get_fido_by_name		(LPSTR name,int *count,struct _fidonumber result[]);
int  nl_get_fido_by_number		(int zone,int net,int node,int point,struct _fidonumber *result);
int	 nl_get_fido_by_partname	(LPSTR namesearch,int *count,struct _fidonumber result[]);
int	 nl_get_fido_by_partname_ext(LPSTR namesearch,int *count,struct _fidonumber result[]);
int	 comparefidonumber			(const void  *,const void *);
int	 comparefidouser			(const void *,const  void *);
void killunder					(LPSTR string);
int	 is_thrue_that				(LPSTR text,int match,int casesens,LPSTR list);
int  find						(LPSTR text,int casesens,LPSTR search);
void check_scale				(int id);
int  err_out					(LPSTR, ...);
void load_editor_cfg			(void);
int	 system_sound				(LPSTR soundid,int forcesync=0);
void remove_LFCR				(LPSTR text);
void insert_LFCR				(LPSTR text);
void expand_tabs				(CString &str);
int  get_selected_lines			(LPSTR buf,int minlines,BOOL addLF=TRUE);
int  split_long_lines			(LPSTR tmp);
void translate_phone_number		(LPSTR phone);
int  find_next_entry			(LPCSTR txt,int casesens,LPCSTR patrn,LPINT offs,int max,int userconv=0,BOOL charset=0);
UINT ScriptWorkerThread			(LPVOID p);
UINT SearchEngineWorkerThread	(LPVOID p);
UINT NlCompMainThread			(LPVOID p);
int  is_address					(LPSTR addr);
int  is_full_address			(LPSTR addr);
int  confirm_aborting			(void);
int  HandleNonExistent			(LPCSTR fname);
BOOL extract_defaults			(LPCSTR fname);
int  unix_time_to_fido		    (time_t unixtime,LPSTR result);
void kill_tabs					(LPSTR text);
int  check_for_nodediffs		(void);
int  nl_compile					(void);
int  play_sound_file			(LPSTR fname);
int  prepare_one_entry_for_dialing	(int sel,struct _curmail *cm);
void highlight_current_selected	(void);
void remove_successful_call		(int x);
int  check_inbound				(void);
CString &trim_all				(CString &buf);
LPSTR trim_all					(LPSTR str);
int   is_exe_available			(LPCSTR name);
void  prepare_tmp_dir			(void);
int	  expand_address			(LPCSTR inc,CString &full,CString &info,int usersel,int get_nl_stuff,int must_listed,int &errorcode,int patternsearch=0,int grepsearch=0);
int   systemcall				(LPSTR command,int &retval);
int   system_command			(LPCSTR cmd,LPSTR title,LPCSTR startupdir,int showwindow,int Timeout,int *retval,int suppress_error=0);
void  restore_base_path			(void);
void  writelog					(int idd, LPCSTR text);
void  init_empty_patterns		(void);
void  set_mailmark				(int ind,char s);
void  remove_all_filters		(void);
void  mark_all_mails_as_read	(void);
void  mark_all_mails_as_deleted	(void);
int   SortAreaCallback			(const void *a1,const void *a2);
int	  SortMailHeaderForDupeCheck(const void *a1,const void *a2);
void save_mails_selection		(void);
void restore_mails_selection	(void);
void save_current_mail_index	(int which=0);
void restore_current_mail_index	(int which=0);
void expand_external_cmd		(LPSTR templ,LPSTR flist,LPCSTR outfile,LPSTR outcmd);
void load_language				(void);
void set_language				(void);
void set_dlg_language			(CDialog *cw,LPCSTR id,int *ids,int count);
void create_dirs_always			(void );
int	 delayed_purge_on_exit		(void);
void ScriptUudecode				(void);
void set_tabbed_string			(CString &string,int index,LPCSTR text);
int  build_routed_netmail		(LPCSTR from_name,LPCSTR from_addr,LPCSTR to_name,LPCSTR to_addr,
								 LPCSTR subject,LPCSTR mailtext,int crash,int recret,int only_local);
int		event_pump				(int index);
int		touch_actual_time_to_eventstring(int index);
int		replace_phone_number	(CString &number);
int		event_add_listbox		(LPCSTR text);
void	modem_add_listbox		(LPCSTR text);
int		diff_to_lasttouch_to_big(evstruct &evs);
void	restart_events			(void);
void	load_modem_cfg			(int num,_modemcfg *pm=0);
void	save_modem_cfg			(int num,_modemcfg *pm=0);
int		test_Notbremse			(void);
int		reset_Notbremse			(void);
void	show_events_status		(void);
int		is_excluded_area		(LPCSTR line,LPCSTR area);
void	OnAbortCurrentEventWork	(void);
int		show_first_selected		(void);
int		show_next_selected		(void);
int		show_msg_t				(LPCSTR msgid,int defbut,int delay=20,LPCSTR text1="OK",LPCSTR text2="CANCEL");
int		set_file_time			(LPSTR fname,int day,int mon,int year,int hour,int min, int sec);
int		db_set_all_headers		(long hndle,mailheader *pheader,int count);
int		check_dupes_for_area	(int ind);
void	initialize_jokes_list	(void);
void	nl_add_listbox			(LPSTR text);
int		get_token_int			(LPCSTR str,int n,char delim='\t');
int		count_tokens			(LPCSTR src,LPCSTR delim=" \t\r\n");
//int		countwords     			(LPCSTR text,LPCSTR delim);
LPSTR	extractstr	     		(int num,LPSTR text,LPSTR delim,LPSTR result, int maxlen);
LPSTR	find_nst_delimiter 		(int num,LPSTR text,LPSTR delim);
int		get_devices_listen_for_call		(int &devices,int &services);
int		match_number_of_rings	(int port);
void	add_to_nl_stringlist	(LPSTR fname);
void	kill_nl_stringlist		(void);
int		nodediff_merge			(void);
int		incoming_emsi_session	(int port,int services);
void	test_watchstuff			(int index,struct mailheader *mh);
int		prepare_result_format	(CString &lc,struct mailheader *mh,struct areadef *adef);
void	check_udef_existence	(void);
int		get_next_area			(int direction);
void	get_first_value			(LPCSTR src,LPSTR ua);
void	test_sound				(LPCSTR soundname);
void	stop_test_sound			(void);
void	handle_mm_stuff			(LPSTR text,int fromwhere);
void	load_multimed_cfg		(void);
int		display_bitmap			(LPCSTR str);
void	hide_mm_bitmap			(void);
void	free_dib_stuff			(void);
BOOL	SetBitmapToDisplay		(LPCTSTR lpszPathName);
int		ReadDIBFile				(CFile& file);
int		InitDIBData				(void);
void	force_bmp_display_update(void);
void	change_mm_action		(LPSTR src);	// deactivate multimedia
int		interpret_format		(CString &,CString &,CString &,CString &,CString &,CString &,CString &,CString &);
int		handle_ingate_translation		(CString &,CString &,CString &,CString &,CString &,CString &);
int		secure_delete			(LPSTR path);
int		rename_back_to_long_filenames	(LPSTR path);
int		run_bbs					(char);
int		get_first_aka			(int *,int *,int *,int *);
int		init_tcpip_stuff		(void);
int		send_tcp_request		(_udp_packet *);
int		free_tcpip_stuff		(void);
int		send_datagram			(LPSTR data);
int		get_UDP_request			(_udp_packet *receivepacket);
int		dynamic_load_tcpip		(void);
int		dynamic_unload_tcpip	(void);
int		handle_tcpip_load_error	(void);
int		net_debug				(LPSTR tmp);
int		make_tic_file			(int zone,int net,int node,int point,int myzone,int mynet,int mynode,int mypoint,LPCSTR  _filename,LPCSTR  _pwd,LPCSTR description,LPCSTR  _areatag,int deletefile,LPCSTR replacefile,LPCSTR magic);
ULONG	crc32_of_file			(LPSTR fname);
void	uudecode_from_file		(LPSTR fname=UUMULFILE);
int		ModifyCommDCB			(LPDCB xx);
void	addbackslash			(LPSTR path);
void	tic_inbound				(void);
int		set_comport_options		(UCHAR Com,_modemcfg *modemcfg,int baudrate=0);
int		callacc_initialize_modem(_modemcfg *mc);
int		init_modem				(UCHAR Com,_modemcfg *mc);	//LPSTR init1,LPSTR init2,LPSTR init3,BOOL logit=FALSE);
BOOL	is_important_name		(LPCSTR name);
int		db_get_boss_flags		(LPCSTR addr,int *nopoll,int *nohydra,int *nozmod,int *tcpip);
void	set_usermark			(int ind,int on);
void	disable_multimedia		(BOOL save);
void	handle_usermark_bit		(int command);
void	unmark_all_mails		(void);

// arcmail stuff in arcmail.cpp
int		get_arcmail_name				(LPCSTR path,LPSTR arcmail);
int		get_boss_packer					(int bossindex,LPSTR packcommand);
int		get_bossindex					(LPSTR fido);
int		pack_file						(LPSTR zipname,LPSTR files,LPSTR packer);
int		extract_file					(LPCSTR name,LPSTR destpath);
int		handle_function_key				(MSG *pMsg);
int		append_to_cost_file				(_costinfo *cost);
void	init_charging_for_one_call		(LPSTR phone);
float	get_charging_sum				(int *charging);
long	chargingtimediff				(void);
int		build_unix_date					(LPSTR strdate,ULONG *unixt);
void	handle_save_of_cached_display_index	(int mailindex);
int		faxreceive							(int Com);
int		get_equal_size						(LPCSTR str1,LPCSTR str2);
void	set_fips_priority					(int flag);
void	write_lastcall_idx					(void);
//int		modify_origin_aka_for_area			(int areahandle,CString &origin);
int		HandleTestForIncomingISDNCall		(void);
int		ISDNCallAcceptionOnOff				(int on);
void	modem_update				(void);
int		broesel_msns				(void);
int		run_bbs						(UCHAR Com);
int		existfile					(LPSTR fname); // check of file existence
void	mailer_stat					(LPCSTR text);
void	db_flush_area				(void);
int		handle_fax_doubleclick		(CString line);
void	zm_errcode					(int zresult,char* dummy);
void	write_bbs					(int Com, LPSTR buf, int len,int show_console);
void	bbs_gotoxy					(int Com, int x, int y);
void	display_file				(int Com, LPSTR fname);
char	get_a_char					(int Com);
int		checkgroup					(LPSTR ga, LPSTR gu);
int		check_farea					(CString &area);
void	bbsuser_struct2string		(LPSTR tmp);
void	black_return				(int Com,int i);
BOOL	HandlerFunc					(DWORD cc);
void	write_bbs_setup				(void);
int		check_nodelists				(void);
int		handle_fax_with_bgfax		(CString line);
void	db_mark_all_areas_as_read	(void);
void 	StoreOrgItemSizes			(LPSTR name,CWnd *wx,_DlgItemsSize *sf, int count);
void 	DynamicAdjustItemSizes		(CWnd *wx,_DlgItemsSize *sf,int count, int newx, int newy);
void	StoreInitDialogSize			(LPSTR name,CWnd *wptr);
void	DrawRightBottomKnubble		(CWnd *wptr,CPaintDC &dc);
void	db_sort_area				(int first_handle,int last_handle);
void	handle_tel_nr_rotation		(LPSTR phone);
int		LineSplitterDetectedRing	(int waitres);
void	SetRollLock					(BOOL bState);
int		IsThereAInterestingEvent	(void);
void	handle_recipt_template		(CString &mailtext);
void	mark_as_answered			(int action);
int		get_uplink_address			(LPCSTR addr,LPSTR uplink);
void	check_import				(LPSTR  path);
int		check_repost				(void);
void	tosser_add_listbox			(LPCSTR text);
//int     import_boss_config        (LPSTR filename);

//vk: added functions
void	show_msg		(LPCSTR msg, int delay=400);
void	show_msg		(LPCSTR eng,LPCSTR rus,int delay=400);
void	show_msg		(LPCSTR eng,LPCSTR ger,LPCSTR rus,int delay=400);
int		get_other_cfg	(int index, int defval=0);
CString	get_cfg			(int id_sect,LPCSTR key,LPCSTR defval);
CString get_lim_cfg		(int id_sect,LPCSTR key,LPCSTR defval,int lim);
int		get_cfg			(int id_sect,LPCSTR key,int defval);
void	set_cfg			(int id_sect,LPCSTR key,LPCSTR val,BOOL ifnew=FALSE);
void	set_cfg			(int id_sect,LPCSTR key,int val,BOOL ifnew=FALSE);
void	del_cfg			(int id_sect);
void	del_cfg			(int id_sect,LPCSTR key);
LPSTR	L				(LPCSTR id,...);
char	*langstr		(LPCSTR eng,LPCSTR rus);
char	*langstr		(LPCSTR ger,LPCSTR eng,LPCSTR rus);
char	*langstr		(LPCSTR ger,LPCSTR eng,LPCSTR rus,LPCSTR fre);
void	set_first_token	(CString &line,LPCSTR text);
void	resize_wnd		(CWnd *wnd,_DlgItemsSize *items,UINT size);
void	make_address	(LPSTR buf,int zone,int net,int node=0,int point=0);
void	make_address	(CString &,int zone,int net,int node=0,int point=0);
int		parse_address	(LPCSTR buf,int *pz,int *pne,int *pno=NULL,int *pp=NULL);
int		parse_address	(LPCSTR buf,USHORT *pzone,USHORT *pnet,USHORT *pnode=NULL,USHORT *ppoint=NULL);
void	make_fidodir	(LPSTR buf,int zone,int net,int node,int point=0);
int		split_fidodir	(LPCSTR buf,int *pz,int *pne,int *pno,int *pp=NULL);
void	get_filename	(LPCSTR in,LPSTR out);
LPSTR	get_script_desc	(int i);
CString get_sect_name	(int id_sect);
BOOL	GetDirectory	(CString &path, HWND hwnd);

int		GetSelectedItem			(CListCtrl &list);
void	MakeColumns				(CListCtrl &list,_listcol *listcol,int count,LPCSTR key=NULL,int ind=0);
void	RestoreColumnsWidth		(CListCtrl &list,LPCSTR key,int ind);
void	SaveColumnsWidth		(CListCtrl &list,LPCSTR key,int ind);
void	RefreshColumns			(CListCtrl &list,LPSTR *hdrs);
void	AddRow					(CListCtrl &list,LPCSTR line);
void	GetRow					(CListCtrl &list,int ind,CString &line);
void	SelectRow				(CListCtrl &list,int ind);
void	DeselectRow				(CListCtrl &list,int ind);
void	ReplaceRow				(CListCtrl &list,int ind,LPCSTR line);

int		select_font				(CFont *font,LPCSTR fname);
void	get_font				(LOGFONT *plf,LPCSTR fname,BOOL bOEM=FALSE);
void	save_font				(LOGFONT *plf,LPCSTR fname);
void	SaveToDelimText			(CListCtrl &list,LPCSTR fname,int conv=0,char delim=',');
int		LoadFromDelimText		(CListCtrl &list,LPCSTR fname,int conv=0,char delim=',');
void	get_call_time			(LPCSTR str,LPSTR time1,LPSTR time2);
void	decode_time				(const char chtime,LPSTR sztime);
void	parse_name				(LPCSTR name,CString &first,CString &second);
BOOL	IsValidDir				(LPCSTR path);
//void	make_user_conversion	(CString &text);
void	get_colors				(void);
BOOL	get_token				(LPCSTR src,int n,CString &dst,char delim='\t');
BOOL	get_token				(LPCSTR src,int n,LPSTR dst,char delim='\t');
int		floor					(int num,int div);
char	*langtime				(LPCSTR src,LPSTR dst);
char	*langdate				(time_t tim,LPSTR dst,BOOL fullmonth=FALSE,BOOL withtime=FALSE);
void	rtrim					(LPSTR str);
CString&get_tearline			(CString &str,BOOL Oem=FALSE);
void	get_fullname			(CString &str,BOOL inv=FALSE);
void	get_mailtext			(LPSTR buf,int ind,BOOL kludge);
char	*make_path				(LPSTR filepath,LPCSTR path,LPCSTR file="");
void	set_queue_mark			(int ind,LPCSTR txt);
int		get_header				(areadef *pad,BOOL fornew,CString &header,BOOL Oem=FALSE);
int		get_footer				(areadef *pad,BOOL fornew,CString &footer,BOOL Oem=FALSE);
int		get_origin				(areadef *pad,BOOL fornew,CString &origin,BOOL Oem=FALSE);
void	load_origins			(CStrList &lst);
void	load_headers			(CStrList &lst);
void	load_footers			(CStrList &lst);
void	SafeFileDelete			(LPCSTR fpath,BOOL filesonly=TRUE);
void	get_exepath				(LPSTR path);
//void	load_pathes				(LPCSTR path);
void	get_folder_path			(LPCSTR ipath,LPCSTR fpath,LPCSTR key,LPSTR gcstr);
void	set_transparent_bmp	(CStatic *ctl,int id);
void	get_path						(LPCSTR src,LPSTR dst);
void	update_charging			(void);
void	load_lbx_from_str		(myowlist &lbx,LPCSTR lst);

void	load_bosslist				(CStrList &lst);
void	save_bosslist				(CStrList &lst);
void	get_logoptions			(int id,BOOL &enabled,int &maxsize,int &shorten);
void	get_packlist				(BOOL def=FALSE);
void	add_packer					(int ind,LPCSTR str);
void	get_soundlist				(CStrList &lst,BOOL def=FALSE);
void	save_soundlist			(CStrList &lst);
void	get_hiddenkludges		(CStrList &lst,BOOL def=FALSE);
void	save_hiddenkludges	(CStrList &lst);
CString get_extrakludges	(BOOL addLF=FALSE);

BOOL	is_valid_address		(LPCSTR str);
void	get_fontsize				(CFont *fnt,CWnd *ctl,CSize &chs);
LPSTR	get_info						(CStatic &ctl);
LPSTR	get_deliverytime		(LPCSTR str);
CString&russian_conversion(CString &str,BOOL oem=FALSE);
LPSTR	russian_conversion	(LPSTR str,BOOL oem=FALSE);
void	make_inistr					(CStrList &lst,LPSTR buf);
char	get_mail_marker			(long status);
void	parse_textimp_str		(LPCSTR key,CString &file,int &cod);
void	split_string				(LPCSTR str,CStrList &lst);
void	collect_string			(CString &str,CListBox &lst);
void	set_attributes			(int attr,BOOL all=FALSE);
void	handle_mailer_ctls	(BOOL state);
void	set_mailtext_font		(LOGFONT *plf);
CString& replace_tabs			(CString &str);
LPSTR	replace_tabs				(LPSTR str);
void	filter_mailtext			(LPCSTR src,LPSTR dst);
int		count_lines					(LPCSTR src);
int		limit_text_len			(LPSTR text,int maxpt,CDC* pDC);
//LPSTR	get_shortname				(LPSTR dst,LPCSTR src);
LPSTR	get_versioninfo			(LPSTR str,int ind);
void	replace_chars			(LPSTR str,char s,char d);
void	backup_wwd				(LPCSTR nold,LPCSTR nnew,BOOL useroot=FALSE);
int		parse_import			(LPCSTR fname);
int		pmatch					(LPCSTR pattern,LPCSTR string);
int		binkd					(CString line);
void	ltrim					(LPSTR str);
LPSTR	fmt_size				(LPSTR buf,long val);
BOOL	get_timezone_inf		(int zone,int net,int node,int point,_tzone &tz);
void	load_submenu			(LPSTR buf,FILE *fp,CMenu *sm);
BOOL	load_context_menu		(LPCSTR mnuid,CMenu *pm);
BOOL	FindStringHelper		(CTextBuffer *pBuf,BOOL casesens,BOOL rx,LPCSTR what,CPoint &found,int &len);
BOOL	is_work_time			(LPCSTR addr);
void	split_filename			(LPCSTR fname,LPSTR drive,LPSTR path,LPSTR name,LPSTR ext);
void	get_template			(LPSTR buf,LPSTR area,BOOL part,BOOL quote);
void	truncate_logfile		(char *logfile,char *tmpfile,int maxsize,int shorten);
void	log_to_file				(int logid,LPCSTR logname,LPCSTR title,LPCSTR text,BOOL &firsttime);
BOOL	IsOem					(CFont *pf);
BOOL	IsOem					(LPCSTR str);
void	copy_to_clipboard		(LPCSTR txt);

#endif
