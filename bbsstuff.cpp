//#################################################
// BBS related stuff
//#################################################

#include "stdafx.h"
#include <direct.h>
#include "supercom.h"
#include "lightdlg.h"
#include <io.h>
#include "mmsystem.h"
#include "gcomm.h"

// global prototypes for console window

HANDLE	consoleStdin;
HANDLE	consoleStdout;
char	consoletitle[300];

extern _gcomm gcomm;
extern _gconfig gc;
extern CLightDlg *gpMain;

static int useansi;
static int lastkey=0;
static int sysopchat=0;
static int exitatonce=0;
static int passwordmode=0;
static int lastmin;
static long lasttime;
static int timeoutwarning=0;
static TANSIDATA ANSIData;
static char language[300];

static int init_write_bbs=0;

static CString curfarea;
static CStrList farea;
static CStrList taglist;

static char yeschar[300];
static char nochar[300];
static char continuechar[300];
static char abortchar[300];
static char stopchar[300];
static char markchar[300];
static char delchar[300];
static CString sysopname;

extern _sessiondata sessiondata; // used in tosslib

static struct _currentuser
{
	char fullname[300];
	char firstname[300];
	char lastname[300];
	char street[300];
	char town[300];
	char tel[300];
	char modem[300];
	int	 sec;
	int lps;
	int ansi;
	char groups[300];
	unsigned long pwdcrc32;
	char language[300];
	int tleft;
	char lastlogin[100];
} curuser;

struct _bbsinput			// used for get_input !
{
	int ispassword;
	int isname;
	int isuppercase;
	int onlydigits;
	int onlyuppercase;
	int nodupes;
} bbsinput;

#define BBSMAIL		"$BBSMAILS$"
#define FILLCHAR    '.'
#define BACKSPACE   "\08"
#define MACROCHAR   '~'
#define FROMSERIAL	1
#define FROMSYSOP	2
#define desclen		49
#define SET_SCHEME1	set_color(Com,BGBLACK);set_color(Com,FGWHITE);set_color(Com,HIGH);
#define SET_SCHEME2	set_color(Com,BGBLUE);set_color(Com,FGYELLOW);set_color(Com,HIGH);

// prototypes //
// ########## //

extern int	COMMAPI	Info				(BYTE Com, BYTE Action, DWORD Data, PFDATA FileData);

// ANSI functions //
// ############## //


/* Foreground color 30..37 */
/* 30=Black, 31=Red, 32=Green, 33=Yellow, */
/* 34=Blue, 35=Magenta, 36=Cyan, 37=White */

#define FGBLACK		30
#define FGRED		31
#define FGGREEN		32
#define FGYELLOW	33
#define FGBLUE		34
#define FGMAGENTA	35
#define FGCYAN		36
#define FGWHITE		37

/* Background color 40..47 */
/* 40=Black, 41=Red, 42=Green, 43=Yellow, */
/* 44=Blue, 45=Magenta, 46=Cyan, 47=White */

#define BGBLACK		40
#define BGRED		41
#define BGGREEN		42
#define BGYELLOW	43
#define BGBLUE		44
#define BGMAGENTA	45
#define BGCYAN		46
#define BGWHITE		47

/* 0=No special attribute, 1=High intensity, 2=Low intensity, */

#define HIGH		1
#define LOW			0 // war 2
#define BLINK       5

//-------------------------------
	void ANSI_Clear(BOOLEAN all)
//-------------------------------
{
COORD coord;
CONSOLE_SCREEN_BUFFER_INFO info;
unsigned long w;

	if (all == TRUE) // clearscreen
	{
		coord.X = 0;
		coord.Y = 0;
		FillConsoleOutputAttribute(consoleStdout,FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,80*25,coord,&w);
		FillConsoleOutputCharacter(consoleStdout,' ',80*25,coord,&w);
		coord.X = 0;
		coord.Y = 0;
		SetConsoleCursorPosition(consoleStdout,coord);
	}
	else // clear EOL
	{
		GetConsoleScreenBufferInfo(consoleStdout,&info);
		coord.X=info.dwCursorPosition.X;
		coord.Y=info.dwCursorPosition.Y;
		FillConsoleOutputCharacter(consoleStdout,' ',80-coord.X,coord,&w);
	}
}

//--------------------------------------
	void ANSI_CheckXY(PSCRPOS ScrPos)
//--------------------------------------
{
	if (ScrPos->Y>25) ScrPos->Y=25;
	if (ScrPos->X>80) ScrPos->X=80;
	if (ScrPos->Y<1)  ScrPos->Y=1;
	if (ScrPos->X<1)  ScrPos->X=1;
}

//--------------------------------------
	void ANSI_GotoXY(PSCRPOS ScrPos)
//--------------------------------------
{
COORD coord;

	coord.X = ScrPos->X-1;
	coord.Y = ScrPos->Y-1;
	SetConsoleCursorPosition(consoleStdout,coord);
}

//--------------------------------------
	void ANSI_GetXY(PSCRPOS ScrPos)
//--------------------------------------
{
CONSOLE_SCREEN_BUFFER_INFO info;
	GetConsoleScreenBufferInfo(consoleStdout,&info);
	ScrPos->X=info.dwCursorPosition.X+1;
	ScrPos->Y=info.dwCursorPosition.Y+1;
}

//-----------------------------------------------------
	void ANSI_UserFunction()//BYTE Com, LPSTRING ANSIMsg)
//-----------------------------------------------------
{
	// unknown ansi-msg detected, skipping
	return;
}

//--------------------------------------
	void ANSI_SetVideo(WORD VideoAction)
//--------------------------------------
{
WORD colors;
CONSOLE_SCREEN_BUFFER_INFO info;

	GetConsoleScreenBufferInfo(consoleStdout,&info);

// FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE
	colors=info.wAttributes;
// clear foreground colors
	if (VideoAction>=30 && VideoAction <= 37)
		colors = colors & ~(FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
// clear background colors
	if (VideoAction>=40 && VideoAction <= 47)
		colors = colors & ~(BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED);

	switch (VideoAction)
	{
 /* Attributes 0..8 */
 /* 0=No special attribute, 1=High intensity, 2=Low intensity, */
 /* 3=Italic, 4=Underline, 5=Blinking, 6=Rapid blinking,       */
 /* 7=Reverse video, 8=Invisible (no display)                  */
	case 0:
		colors = colors & ~(FOREGROUND_INTENSITY | BACKGROUND_INTENSITY | FOREGROUND_RED |
			FOREGROUND_BLUE | FOREGROUND_GREEN | BACKGROUND_RED | BACKGROUND_BLUE | 
			BACKGROUND_GREEN);
		colors|=FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED;
		break;
	case 1:
		colors = colors | FOREGROUND_INTENSITY;
		break;
	case 2:
		colors = colors & ~FOREGROUND_INTENSITY;
		break;

 /* Foreground color 30..37 */
 /* 30=Black, 31=Red, 32=Green, 33=Yellow, */
 /* 34=Blue, 35=Magenta, 36=Cyan, 37=White */
	case 30:
			colors = colors & ~(FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN);
			break;
	case 31:
			colors|=FOREGROUND_RED;
			break;
	case 32:
			colors|=FOREGROUND_GREEN;
			break;
	case 33:
			colors|=FOREGROUND_GREEN|FOREGROUND_RED;
			break;
	case 34:
			colors|=FOREGROUND_BLUE;
			break;
	case 35:
			colors|=FOREGROUND_RED|FOREGROUND_BLUE;
			break;
	case 36:
			colors|=FOREGROUND_GREEN|FOREGROUND_BLUE;
			break;
	case 37:
			colors|=FOREGROUND_GREEN|FOREGROUND_BLUE|FOREGROUND_RED;
			break;

/* Background color 40..47 */
/* 40=Black, 41=Red, 42=Green, 43=Yellow, */
/* 44=Blue, 45=Magenta, 46=Cyan, 47=White */
	case 40:
			colors = colors & ~BACKGROUND_RED;
			colors = colors & ~BACKGROUND_BLUE;
			colors = colors & ~BACKGROUND_GREEN;
			break;
	case 41:
			colors|=BACKGROUND_RED;
			break;
	case 42:
			colors|=BACKGROUND_GREEN;
			break;
	case 43:
			colors|=BACKGROUND_GREEN|BACKGROUND_RED;
			break;
	case 44:
			colors|=BACKGROUND_BLUE;
			break;
	case 45:
			colors|=BACKGROUND_RED|BACKGROUND_BLUE;
			break;
	case 46:
			colors|=BACKGROUND_GREEN|BACKGROUND_BLUE; // war read
			break;
	case 47:
			colors|=BACKGROUND_GREEN|BACKGROUND_BLUE|BACKGROUND_RED;
			break;
	}
	if (useansi)
		SetConsoleTextAttribute(consoleStdout,colors);
}

//----------------------------------------------------------------------------------------
	LONGINT COMMAPI ANSIProc(BYTE Com, TANSIACTION Action, WORD wParam, POINTER pParam)
//----------------------------------------------------------------------------------------
{
	switch (Action)
	{
		case cfANSIGETXY:    ANSI_GetXY((PSCRPOS)pParam);				break;
		case cfANSIGOTOXY:   ANSI_GotoXY((PSCRPOS)pParam);				break;
		case cfANSICHECKXY:  ANSI_CheckXY((PSCRPOS)pParam);				break;
		case cfANSICLEARSCR: ANSI_Clear(TRUE);							break;
		case cfANSICLEAREOL: ANSI_Clear(FALSE);							break;
		case cfANSISETVIDEO: ANSI_SetVideo(wParam);						break;
		case cfANSIUSER:     ANSI_UserFunction();//Com, (LPSTRING)pParam);	break;
	}
	return 0;
}

// bss related functions //
// ##################### //

//==========================================================
	void update_console(void)
//==========================================================
{
char	tmp[100];

	if (strlen(curuser.groups))
		sprintf(tmp,L("S_295"),curuser.sec,curuser.groups,(int)(curuser.tleft/60));
	else
		sprintf(tmp,L("S_296"),curuser.sec,(int)(curuser.tleft/60));
	
	CharToOem(tmp,tmp);
	sprintf(consoletitle,"%s, %s, %s",curuser.fullname,curuser.town,tmp);
	SetConsoleTitle(consoletitle);
}

//==========================================================
	void add_missing_files(char *fbbs, char * path)
//==========================================================
{
CString temp;
char tmp[4096];
FILE * fp;
CStrList files;
struct _finddata_t se;
long hfile;

	files.RemoveAll();

	fp = fopen(fbbs,"rt");
	if (fp)
	{
		while (fgets(tmp,4000,fp)!=0)
		{
			if (*tmp && *tmp!=' ' && *tmp!='+')
			{
				get_token(tmp,1,temp,' ');
				temp.MakeUpper();
				files.AddTail(temp);
			}
		}
		fclose(fp);
	}

	files.Sort(0);

	// now search the directory and find files which are not in the list
	make_path(tmp,path,"*.*");
	hfile = _findfirst(tmp,&se);
	if (hfile != -1L)
	{
		do 
		{
			if (!(se.attrib & _A_SUBDIR) && 
				strcmpi("FILES.BBS",se.name) && strcmpi("FILES.BAK",se.name))
			{
				int t;
				int found=0;

				for (t=0; t < files.GetCount();t++)
				{
					if (strcmpi(files.GetString(t),se.name)==0)
					{
						found=1;
						break;
					}
				}

				if (!found) // append file to files.bbs
				{
					FILE * fp;

					fp = fopen(fbbs,"at");
					if (fp)
					{
						fprintf(fp,"%-13s Autoadded file\n",se.name);
						fclose(fp);

						files.AddTail(se.name);

						files.Sort(0);
					}
				}
			}
		} while (_findnext(hfile, &se)== 0);
	}
}

//==========================================================
	void write_bbs_setup(void)
//==========================================================
{
CStrList tmp;

	// userfile
 	// 0  name
	// 1  pwd		CRC32 !
	// 2  seclevel
	// 3  groups
	// 4  street
	// 5  town
	// 6  tel
	// 7  modem/fax
	// 8  lines per screen  (int)
	// 9  ansi-color 0 or 1 (int)
	// 10 language

	//tmp.RemoveAll();
	//tmp.AddTail("Oliver Weindl\t\t100\tABC\tSudetenstr.3\t84056 Rottenburg\t08781-92115\t08781-92116\t24\t1\tGerman");

	//tmp.SaveToFile("BBSUSER.CFG");

	tmp.LoadFromFile("BBSUSER.CFG");

	if (tmp.GetCount()<1)
		tmp.SaveToFile("BBSUSER.CFG");

	// bbs defaults for new users

	// 0 seclevel
	// 1 groups

	//tmp.RemoveAll();                     is now string 4 in bbsmain.cfg !!
	//tmp.AddTail("10\tA");
	//tmp.SaveToFile("BBSDFUSR.CFG");

	tmp.LoadFromFile("BBSGRPS.CFG");
	if (tmp.GetCount()<1)
	{
		tmp.AddTail("10\t20");
		tmp.SaveToFile("BBSgrps.CFG");
	}


	tmp.LoadFromFile("BBSFARE.CFG");
	if (tmp.GetCount()<1)
		tmp.SaveToFile("BBSFARE.CFG");

	tmp.LoadFromFile("BBSMAIN.CFG");
	if (tmp.GetCount()<1)
	{
		tmp.AddTail("");
		tmp.AddTail("");
		tmp.AddTail("English");
		tmp.AddTail("BEEP");
		tmp.AddTail("10\t20");
		tmp.SaveToFile("BBSMAIN.CFG");
	}
}

//==========================================================
	int existfile(char *fname)
//==========================================================
// searches the specified file and returns 1 if the file exists, 0 if the file does not exist
//
{
struct _finddata_t se;
long hfile;

	hfile = _findfirst(fname,&se);
	if (hfile != -1L)
	{
		_findclose(hfile);
		return 1;
	}
	return 0;
}

//==========================================================
	int set_color(int Com,int whatcolor, int intens=-1)
//==========================================================
// for this function use the defines at the start of the ansi-action !
//
{
char tmp[300];

	if (useansi)
	{
		ANSI_SetVideo((WORD)whatcolor); // set the console-color

		if (intens != -1)
		{
			ANSI_SetVideo((WORD)intens);

			sprintf(tmp,"\x1B[%d;%dm",intens,whatcolor);
			write_bbs(Com,tmp,strlen(tmp),0); // don't display on console

			return 1;
		}
		sprintf(tmp,"\x1B[%dm",whatcolor);
		write_bbs(Com,tmp,strlen(tmp),0); // don't display on console
	}
	return 1;
}

//==========================================================
	int kill_tail(char * line)
//==========================================================
// kill 0x0A and 0x0D at the end of a string
//
{
	while (strlen(line)>0)
	{
		if (line[strlen(line)-1] != 13 && line[strlen(line)-1] != 10)
			break;

		line[strlen(line)-1] =0;
	}
	return 1;
}

//==========================================================
	int kill_spaces_hinten(char * line)
//==========================================================
// kill 0x0A and 0x0D at the end of a string
//
{
	while (strlen(line)>0)
	{
		if (line[strlen(line)-1] != ' ')
			break;

		line[strlen(line)-1] =0;
	}
	return 1;
}


//==========================================================
	int kill_tabbs(char * line)
//==========================================================
// kill tabbs in a string and replace them with a space
//
{
char * p;

	p = line;
	while (*p)
	{
		if (*p == '\t')
			*p = ' ';

		p++;
	}
	return 1;
}

//==========================================================
// searches for MACRO in BBSLANG.DEF and returns found string in retval
	int	get_string(char *macro, char *retval)
//==========================================================
{
FILE	*fp;
char	tmppath[MAX_PATH];
int		found=0;
char	tmp[300];
char	line[1024];

	strcpy(tmp,language); // get language
	strcpy(retval,"^[FB[UNKNOWN MACRO]");
	strcpy(line,"[");
	strcat(line,tmp);
	strcat(line,"]");
	strcpy(tmp,line);

	make_path(tmppath,gc.BasePath,FIPSBBS);
	fp = fopen(tmppath,"rt");
	if (!fp)
		return 0;

	// now search until start of section

	found=0;
	while (fgets(line,999,fp))
	{
		char *p;

		kill_tail(line);
		kill_tabbs(line);

		p = line;
		while (*p && *p == ' ')
			p++;

		if (strcmpi(p,tmp) == 0)
		{
			found = 1;
			break;
		}
	}

	if (!found)
		goto endandexit;// error-message "section not found"

	// now search for macro

	found=0;
	while (fgets(line,999,fp))
	{
		char * p, *c;
		char ident[300];

		kill_tail(line);
		kill_tabbs(line);

		p = line;
		while (*p && *p == ' ')
			p++;

		strcpy(ident,p);
		c = ident;

		while (*c && *c != ' ') // first space
			c++;
		
		*c=0;
		if (strcmpi(ident,macro) == 0 && *p)
		{
			found = 1;
			p = line;
			while (*p && *p == ' ') // first non space
				p++;

			while (*p && *p != ' ') // first space
				p++;

			while (*p && *p == ' ') // first non space of retval
				p++;

			strcpy(retval,p);
			break;
		}
	}

endandexit:

	fclose(fp);
	return 1;
}

//==========================================================
	BOOL HandlerFunc(DWORD cc)
//==========================================================
{
	if (cc==CTRL_C_EVENT || cc==CTRL_BREAK_EVENT || cc==CTRL_CLOSE_EVENT || 
		cc==CTRL_LOGOFF_EVENT || cc==CTRL_SHUTDOWN_EVENT)
	{
		exitatonce=1;
		return TRUE;
	}
	return FALSE;
}

//==========================================================
	int init_bbs_stuff(void)
//==========================================================
{
COORD coord;
BOOL success;

	write_bbs_setup(); // schreibt temporaere bbs-config, nacher rauswerfen !!!!!!!!!!!!
	AllocConsole();

	success = SetConsoleCtrlHandler((PHANDLER_ROUTINE) HandlerFunc,TRUE);

	coord.X = 1;
	coord.Y = 1;

	SetConsoleCursorPosition(consoleStdout,coord);

	consoleStdin =	GetStdHandle(STD_INPUT_HANDLE);
	consoleStdout = GetStdHandle(STD_OUTPUT_HANDLE);

	if (consoleStdin==consoleStdout)
		return 0;

	return 1;
}

//==========================================================
	void get_standard_chars()
//==========================================================
{
	get_string("YESCHAR",yeschar);
	get_string("NOCHAR",nochar);
	get_string("CONTINUE",continuechar);
	get_string("ABORT",abortchar);
}

//==========================================================
	void bbs_gotoxy(int Com, int x, int y)
//==========================================================
{
COORD coord;
char tmp[512];

	// the console-stuff
	coord.X = x-1;
	coord.Y = y-1;

	SetConsoleCursorPosition(consoleStdout,coord);

	// the ansi-stuff
	sprintf(tmp,"\x1B[%d;%dH",y,x);
	write_bbs(Com,tmp,strlen(tmp),0);
}
//==========================================================
	void clear_screen(int Com,BOOLEAN full)
//==========================================================
{
char tmp[300];

	ANSI_Clear(full);

	if (full == TRUE)
		strcpy(tmp,"\x1b[2J");
	else
		strcpy(tmp,"\x1b[K");

	write_bbs(Com,tmp,strlen(tmp),0);
}

//==========================================================
	int ask_yes_no(int Com)
//==========================================================
{
char tmp[300];
int retval;
char p;

	retval = 1;
	get_standard_chars();

	tmp[0] = ' ';
	tmp[1] = '(';
	tmp[2] = yeschar[0];
	tmp[3] = '/';
	tmp[4] = nochar[0];
	tmp[5] = ')';
	tmp[6] = 0;

	write_bbs(Com,tmp,strlen(tmp),1);

	p=0;
	do
	{
		p = get_a_char(Com);
		if (p!=0)
		{
			  if (strchr(yeschar,p) != NULL)
				  break;
			  if (strchr(nochar,p) != NULL)
				  break;
		}

	if (exitatonce)
		return 0;

	} while (1);

	if (strchr(nochar,p) != NULL)
		return 0;

	return 1;
}


//==========================================================
	int release_bbs_stuff()
//==========================================================
{
	SetConsoleCtrlHandler((PHANDLER_ROUTINE) HandlerFunc,FALSE);
	FreeConsole();
	return 1;
}


//==========================================================
	char get_a_char(int Com)
//==========================================================
{
char tmp[1024];
int retval;
int ch;
time_t akttime;
unsigned long t;
unsigned char *p;
DWORD * dummy;
WORD * dumword;
INPUT_RECORD buf;

	lastkey=0;
	time(&akttime);
	if (lasttime==0)
		lasttime=akttime;

	if (akttime!=lasttime)
	{
		curuser.tleft-=(akttime-lasttime);
		lasttime=akttime;

		if ((curuser.tleft%60) == 0)
			update_console();

		if (!timeoutwarning && curuser.tleft < 120) // timeout-warning
		{
			get_string("TIMEOUTWARN",tmp);
			black_return(Com,1);
			write_bbs(Com,tmp,strlen(tmp),1);
			Sleep(3000);
			timeoutwarning=1;
		}

		if (curuser.tleft < 0)					// no usertime left
		{
			get_string("TIMEOUT",tmp);
			black_return(Com,1);
			write_bbs(Com,tmp,strlen(tmp),1);
			Sleep(3000);
			exitatonce=1;
			return 0;
		}

		if (Com != LOCALCOM) // check for CARRIER
		{
			if (Com != ISDNPORT)
			{
				if ( RS_Carrier(Com) == 0)
				{
					exitatonce = 1;
					modem_add_listbox(L("S_456"));
					return 0;
				}
			}
			else
			{
				if (!gcomm.ConnectedOnIsdn)
				{
					exitatonce = 1;
					modem_add_listbox(L("S_457"));
					return 0;
				}
			}
		}
	}

	retval = WaitForSingleObject(consoleStdin,5);

	if (retval != WAIT_TIMEOUT) // Sysop-Key pessed
	{
		PeekConsoleInput(consoleStdin,&buf,1,&t);
		if (t>0)
		{
			int		*keystate;
			char	*keystatecp =(char*)&buf.Event.KeyEvent.bKeyDown;
			keystatecp+=2;

			keystate = (int*)keystatecp;

			if (buf.EventType!=KEY_EVENT || *keystate == FALSE || buf.Event.KeyEvent.uChar.AsciiChar==0)
				ReadConsoleInput(consoleStdin,&buf,1,&t);
			else
			{
				char ch;

				//ReadFile(consoleStdin, &ch, 1, &t, NULL);
				ReadConsoleInput(consoleStdin,&buf,1,&t);

				if (*keystate == 1)
				{
					p=(unsigned char *)&(buf.Event.KeyEvent.uChar.AsciiChar);
					p+=2;
					ch=*p;
					lastkey=FROMSYSOP;

					p = (unsigned char*)&(buf.Event.KeyEvent.dwControlKeyState);
					p+=2;
					dummy=(DWORD *)p;

					if (*dummy & (LEFT_ALT_PRESSED | RIGHT_ALT_PRESSED))
					{
						if (ch == 'c') // sysopchat
						{
							sysopchat=1;
							return 0;
						}

						if (ch == 'h') // hangup
						{
							exitatonce=1;
							return 0;
						}
					}

					p = (UCHAR*)&(buf.Event.KeyEvent.wVirtualScanCode);
					p+=2;
					dumword=(WORD*)p;

					if (*dumword == 72)
					{
						curuser.tleft+=60;
						update_console();
					}

					if (*dumword == 80)
					{
						if (curuser.tleft > 0)
							curuser.tleft-=60;

						if (curuser.tleft < 0)
							curuser.tleft=0;
						update_console();

					}
					return ch;
				}
				else
					return 0;
			}
		}
	}

	// check char ready on comport !
	if (Com != LOCALCOM && RS_RXInTime(Com,(char *)&ch,0)) // war SEC0_005
	{
		lastkey=FROMSERIAL;
		return(ch);
	}
	return 0;
}

//=======================================================================================
	int get_input(int Com, char *input, int maxlen, struct _bbsinput *bbs)
//=======================================================================================
// Get a inputline from console or from the user
//
{
int	t;
int lastwasesc=1; // first char alway uppercase if isname
char ch;
char backspace;
char buf[1024];
char test[300];
char fillchar;

	backspace = 8;
	fillchar=' ';
	black_return(Com,0);
	write_bbs(Com," ",1,1);
	SET_SCHEME2;

	if (strlen(input)>0)
		write_bbs(Com,input,strlen(input),1);

	for (t=strlen(input);t<(maxlen);t++)
	{
		DWORD dwVersion;

		write_bbs(Com,&fillchar,1,1);
		//Sleep(1);			 // getversion

		dwVersion = GetVersion();

		if (dwVersion >= 0x80000000)  // nur unter Win95
			Sleep(1);
	}

	buf[0]=8;
	buf[1]=0;

	for (t=0;t<(int)(maxlen-strlen(input));t++)
		write_bbs(Com,buf,1,1);

	ch = get_a_char(Com);

	while (ch != 13)
	{
		if (exitatonce)
			return 0;

		if (ch != 0)
		{
checkagain:
			test[0]=ch;
			test[1]=0;

			switch(ch)
			{
				case 8:	// backspace
					if (strlen(input) >0)
					{
						input[strlen(input)-1]=0;

						buf[0]=8;
						buf[1]=0;
						write_bbs(Com,buf,1,1);
						buf[0]=fillchar;
						buf[1]=0;
						write_bbs(Com,buf,1,1);
						buf[0]=8;
						buf[1]=0;
						write_bbs(Com,buf,1,1);
					}
					break;
				case 0x1b: // ansi-sequence
					if (Com != LOCALCOM && RS_RXInTime(Com,&ch,SEC0_1) &&
						ch == '[' && RS_RXInTime(Com,&ch,SEC0_1))
					{
						if (ch == 'D') // cursor left -> backspace
						{
							ch = 8;
							goto checkagain;
						}
						if (ch == 'C') // cursor right -> space
						{
							ch = ' ';
							goto checkagain;
						}
					}
					break;
				default: // any other char
					if (bbs->onlyuppercase)
						test[0] = toupper(test[0]);

					if (bbs->nodupes && strchr(input,test[0])!=0)
						break;

					if ((int)strlen(input) < maxlen)
					{
						if (lastwasesc && bbs->isname)
							test[0] = toupper(test[0]);

						if (test[0] == ' ')
							lastwasesc=1;
						else
							lastwasesc=0;


						if (bbs->onlydigits && !isdigit(test[0]) && 
							test[0] != '-' && test[0] != '/' && 
							test[0] != '+' && test[0] != '\\')
								break;

						strcat(input,test);

						if (!bbs->ispassword)
							write_bbs(Com,test,1,1);
						else
							write_bbs(Com,"*",1,1);
					}
					break;
			}
		}
		else
			Sleep(50);

		ch = get_a_char(Com);
	}
	return 1;
}

//==========================================================
	void write_bbs(int Com, char * buf, int len,int show_console)
//==========================================================
// show_console == 0		only shown to remote terminal
// show_console == 1		shown to remote terminal and console-window
// show_console == 2		only shown to console-window
{
unsigned long k;
unsigned short retx;
int		t,oldt;
int		printit;
char	temp[1024];
static int macrostate=0;

	if (!init_write_bbs)
	{
		macrostate=0;
		init_write_bbs=1;
	}

	if (len < 2) // wenn nur ein zeichen, brauchen wir die riesenschleife nicht.
	{
		if (buf[0] != MACROCHAR)
		{
			if (!macrostate)
			{
				if (show_console==1 || show_console == 2)
					WriteConsole(consoleStdout,buf,1,&k,0);

				if (Com!=LOCALCOM && show_console != 2)
						RS_TXInTime(Com,buf[0],SEC_5);
			}
			else //we have to write a MACRO
			{
				int found=0;
				switch (buf[0])
				{
					case '#':
						sprintf(temp,"%d",(int)(curuser.tleft/60));
						found=1;
						break;
					case 'N':
						strcpy(temp,curuser.fullname);
						found=1;
						break;
					case 'S':
						strcpy(temp,curuser.street);
						found=1;
						break;
					case 'T':
						strcpy(temp,curuser.town);
						found=1;
						break;
					case 'P':
						strcpy(temp,curuser.tel);
						found=1;
						break;
					case 'F':
						strcpy(temp,curuser.modem);
						found=1;
						break;
					case 'L':
						strcpy(temp,curuser.language);
						found=1;
				}

				if (found)
				{
					if (show_console==1 || show_console == 2)
							WriteConsole(consoleStdout,temp,strlen(temp),&k,0);

					if (Com!=LOCALCOM && show_console != 2)
							RS_TXPInTime(Com,temp,strlen(temp),SEC_5,&retx);
				}
				else
				{
					char tmp[100];

					tmp[0] = MACROCHAR;
					// print macrochar
					if (show_console==1 || show_console == 2)
							WriteConsole(consoleStdout,tmp,1,&k,0);

					if (Com!=LOCALCOM)
					{
						if (show_console != 2)
							RS_TXInTime(Com,tmp[0],SEC_5);
					}


					// and now the current char

					if (show_console==1 || show_console == 2)
						WriteConsole(consoleStdout,buf,1,&k,0);

					if (Com!=LOCALCOM && show_console != 2)
							RS_TXInTime(Com,buf[0],SEC_5);
				}
			}

			macrostate=0;
			return;
		}
		else
		{
			macrostate =1;
			return;
		}

	}

	macrostate=0; // we display a string ...

	for (t=0;t<len;t++)
	{
		printit=1;

nextescape:

		if (Com != ISDNPORT && Com != LOCALCOM)
		{
			if (!RS_Carrier(Com))
			{
				exitatonce=1;
				return;
			}
		}
		else
			if (Com != LOCALCOM && !gcomm.ConnectedOnIsdn)
			{
				exitatonce=1;
				return;
			}

		oldt=t;

		if (t<(len-1))
		{
			if (buf[t] == '^' && buf[t+1] == '[')
			{

				t++;t++;
				if (t<len)
				{
					char c,d;

					c=toupper(buf[t]);
					t++;
					if (t<len)
					{
						d=toupper(buf[t]);
						switch (c)
						{
							case 'C':
								switch (d)
								{
									case 'S':
										ANSI_Clear(TRUE);
										break;
								}
							case 'F':
								switch (d)
								{
									case '0':
										set_color(Com,FGBLACK,LOW);
										break;
									case '1':
										set_color(Com,FGBLUE,LOW);
										break;
									case '2':
										set_color(Com,FGGREEN,LOW);
										break;
									case '3':
										set_color(Com,FGCYAN,LOW);
										break;
									case '4':
										set_color(Com,FGRED,LOW);
										break;
									case '5':
										set_color(Com,FGMAGENTA,LOW);
										break;
									case '6':
										set_color(Com,FGYELLOW,LOW);
										break;
									case '7':
										set_color(Com,FGWHITE,LOW);
										break;
									case '8':
										set_color(Com,FGBLACK,HIGH);
										break;
									case '9':
										set_color(Com,FGBLUE,HIGH);
										break;
									case 'A':
										set_color(Com,FGGREEN,HIGH);
										break;
									case 'B':
										set_color(Com,FGCYAN,HIGH);
										break;
									case 'C':
										set_color(Com,FGRED,HIGH);
										break;
									case 'D':
										set_color(Com,FGMAGENTA,HIGH);
										break;
									case 'E':
										set_color(Com,FGYELLOW,HIGH);
										break;
									case 'F':
										set_color(Com,FGWHITE,HIGH);
										break;
								}
								break; // switch F
							case 'B':
								switch (d)
								{
									case '0':
										set_color(Com,BGBLACK);
										break;
									case '1':
										set_color(Com,BGBLUE);
										break;
									case '2':
										set_color(Com,BGGREEN);
										break;
									case '3':
										set_color(Com,BGCYAN);
										break;
									case '4':
										set_color(Com,BGRED);
										break;
									case '5':
										set_color(Com,BGMAGENTA);
										break;
									case '6':
										set_color(Com,BGYELLOW);
										break;
									case '7':
										set_color(Com,BGWHITE);
										break;
								}
								break; // Switch B
						}
					}
					printit=0;
				}
			}

			if (buf[t] == '^' && buf[t+1] == 'M')	// ^M is a return
			{										// ################
			 char tmp[10];
			 unsigned short result;

			 t++; t++;

			 strcpy(tmp,"\x0a\x0d");

				if (show_console==1 || show_console == 2)
					WriteConsole(consoleStdout,tmp,2,&k,0);

				if (Com!=LOCALCOM)
				{
					if (show_console != 2)
						RS_TXPInTime(Com,tmp,2,SEC_5,&result);
				}
			}

			if (buf[t] == '^' && buf[t+1] == '$')	// ^$ is current file-area
			{										// ################
				CString help;
				unsigned short result;
				char tmp[300];

				t+=2;
				get_token(curfarea,1,help);
				help.AnsiToOem();
				strcpy(tmp,help);

				if (show_console==1 || show_console == 2)
					WriteConsole(consoleStdout,tmp,strlen(tmp),&k,0);

				if (Com!=LOCALCOM && show_console != 2)
					RS_TXPInTime(Com,tmp,strlen(tmp),SEC_5,&result);
			}

			if (buf[t] == '^' && buf[t+1] == '#')	// ^# time
			{										// ################
				char help[100];
				unsigned short result;

				t+=2;
				sprintf(help,"%d",(int)(curuser.tleft/60));

				if (show_console==1 || show_console == 2)
					WriteConsole(consoleStdout,help,strlen(help),&k,0);

				if (Com!=LOCALCOM && show_console != 2)
					RS_TXPInTime(Com,help,strlen(help),SEC_5,&result);
			}

			if (buf[t] == '^' && buf[t+1] == 'R')	// ^R Wait for a RETURN
			{										// ################
				do {Sleep(100);} while (get_a_char(Com) ==0);
			}

			if (buf[t] == '^' && buf[t+1] == 'D')	// ^D is a delay of 1 second
			{										// #########################
				t++; t++;
				Sleep(1000);
			}

			if (buf[t] == '^' && buf[t+1] == '@')	// display a file
			{										// ################
				char *p;

				t++;
				t++;

				if (t < len)
				{
					strcpy(temp,buf);

					p = &temp[t];
					while (t<len && temp[t] != '@')
						t++;

					if (temp[t] == '@')
					{
						char tmp[300];

						temp[t]=0;
						if (p != &temp[t])
						{
							strcpy(tmp,p);
							display_file(Com,tmp);
							printit=0;
						}
					}
				}
			}
		}

		if (printit && buf[t] && buf[t] != '^')
		{
			unsigned short result;

			if (show_console==1 || show_console == 2)
				WriteConsole(consoleStdout,&buf[t],1,&k,0);

			if (Com!=LOCALCOM && show_console != 2)
				RS_TXPInTime(Com,&buf[t],1,SEC_5,&result);
		}
		else
			if (oldt != t)
				goto nextescape;
	}
}

//==========================================================
	void black_return(int Com,int i)
//==========================================================
// writes a return (white on black) to the console
{
int t;

	SET_SCHEME1;
	if (!i)
		return;

	for (t=0;t<i;t++)
		write_bbs(Com,"\n\r",2,1);
}

//==========================================================
	void display_file(int Com,char *fname)
//==========================================================
// displays a file given in filename
// the file must be located in the /fips/bbspics directory
// extension _MUST_ not be given !
//
{
char	buf[1024];
char	tmp[1024];
FILE	*fp;
CString help;

	/*
    CStrList bbsmaincfg;

	bbsmaincfg.LoadFromFile("BBSMAIN.CFG");

	strcpy(buf,PS bbsmaincfg.GetString(1));
	addbackslash(buf);
	strcat(buf,fname);
	*/

	get_string("FILEDIRECTORY",buf);
	addbackslash(buf);
	strcat(buf,fname);
	strcat(buf,useansi ? ".ANS" : ".ASC");

	if (!existfile(buf) && useansi) // gibts ein ascii-file
	{
		get_string("FILEDIRECTORY",buf);
		addbackslash(buf);
		strcat(buf,fname);
		strcat(buf,".ASC");

		if (!existfile(buf))
		{

			sprintf(tmp,"\r\n>> Cannot find file %s <<\n",buf);
			write_bbs(Com,tmp,strlen(tmp),1);
			return;
		}

	}

	if (!existfile(buf) && !useansi) // gibts doch ein ansi-file ?
	{

		get_string("FILEDIRECTORY",buf);
		addbackslash(buf);
		strcat(buf,fname);
		strcat(buf,".ANS");

		if (!existfile(buf))
		{
			sprintf(tmp,"\r\n>> Cannot find file %s <<\n",buf);
			write_bbs(Com,tmp,strlen(tmp),1);
			return ;
		}
	}


	fp = fopen(buf,"rb");
	if (!fp)
		return ;

	// now display file

	RS_ANSIInit(Com, &ANSIData);

	buf[0]=0;

	RS_ANSICheck(Com,buf,1); // init ansi-state-machine

	while (fread(buf,1,1,fp) == 1)
	{
		if (Com != ISDNPORT && Com != LOCALCOM)
		{
			if (!RS_Carrier(Com))
			{
				exitatonce=1;
				goto displaydone;
			}
		}
		else
			if (Com != LOCALCOM && !gcomm.ConnectedOnIsdn)
			{
				exitatonce=1;
				goto displaydone;
			}

		if (buf[0]==13 || buf[0]==10)
			write_bbs(Com,buf,1,1);
		else
		{

			if (RS_ANSICheck(Com,buf,0)==TRUE)
			{

				if (buf[0]!=0)
					write_bbs(Com,buf,1,1);
			}
			else
			{
				if (useansi)
					write_bbs(Com,buf,1,0);
			}
		}
	}
displaydone:

	fclose(fp);
}

//==========================================================
	int bbs_language_list(CStrList &lst)
//==========================================================
{
char	path[MAX_PATH],line[1024],tmp[50];
FILE	*fp;

	lst.RemoveAll();
	make_path(path,gc.BasePath,FIPSBBS);
	fp = fopen(path,"rt");
	if (!fp)	return 0;
	while (fgets(line,999,fp))
	{
		if (!strnicmp(line,"@section ",9))
		{
			sscanf(line,"%*s %s",tmp);
			lst.AddTail(tmp);
		}
	}
	fclose(fp);
	return 1;
}


//==========================================================
int change_pwd(int Com,char * pwd1, char * pwd2)
//==========================================================
{
char buf[1024];

	strcpy(pwd1,"");
	strcpy(pwd2,"");

// pwd
	SET_SCHEME1;
	write_bbs(Com,"\r\n\n",3,1);
	get_string("ASKPWD",buf);
	write_bbs(Com,buf,strlen(buf),1);
	SET_SCHEME2;

	bbsinput.ispassword=1;
	bbsinput.onlyuppercase=1;
	get_input(Com,pwd1,16,&bbsinput);
	bbsinput.onlyuppercase=0;
	bbsinput.ispassword=0;

	if (exitatonce)	return 0;

// pwd again
	black_return(Com,1);
	write_bbs(Com,"\r\n",2,1);
	get_string("ASKPWDAGAIN",buf);
	write_bbs(Com,buf,strlen(buf),1);
	SET_SCHEME2;

	bbsinput.ispassword=1;
	bbsinput.onlyuppercase=1;
	get_input(Com,pwd2,16,&bbsinput);
	bbsinput.onlyuppercase=0;
	bbsinput.ispassword=0;
	return 1;
}

//============================================
int change_language(int Com)
//============================================
// lets user change language
{
CStrList lang;
char buf[1000];
int t,ok=0;

	if (!bbs_language_list(lang))
	{
		sprintf(buf,"\n\n\r>>> Language File not found, disconnecting...\n\n\r");
		write_bbs(Com,buf,strlen(buf),1);
		Sleep(2000);
		return 0;
	}
	write_bbs(Com,"\n\r",2,1);

	get_string("ASKLANGUAGE",buf);
	strcat(buf,"\r\n\n");
	write_bbs(Com,buf,strlen(buf),1);

	for (t=0;t<lang.GetCount();t++)
	{

		sprintf(buf,"^[F2%d^[FF %s\n\r",t+1,lang.GetString(t));
		write_bbs(Com,buf,strlen(buf),1);
	}

	write_bbs(Com,"\n\r",2,1);
	ok =0;
	do
	{
		black_return(Com,1);
		// clear line
		for (t=0;t<70;t++)
			write_bbs(Com,"\x08",1,1);
		for (t=0;t<70;t++)
			write_bbs(Com," ",1,1);
		for (t=0;t<70;t++)
			write_bbs(Com,"\x08",1,1);

		get_string("ASKLANGUAGE",buf);
		write_bbs(Com,buf,strlen(buf),1);

		SET_SCHEME2;
		strcpy(buf,"");
		bbsinput.onlydigits=1;
		get_input(Com,buf,2,&bbsinput);
		bbsinput.onlydigits=0;

		if (exitatonce)
			return 0;

		t = atoi(buf);
		if (t > 0 && t <= lang.GetCount())
			ok = 1;

	} while (!ok);

	strcpy(language,lang.GetString(t-1));
	strcpy(curuser.language,lang.GetString(t-1));
	return 1;
}

//==========================================================
void change_mailing_adress(int Com)
//==========================================================
{
char buf[1024];

	black_return(Com,2);
	get_string("ASKSTREET",buf);
	write_bbs(Com,buf,strlen(buf),1);
	SET_SCHEME2;

	bbsinput.isname=1;
	get_input(Com,curuser.street,40,&bbsinput);
	bbsinput.isname=0;

// town
	SET_SCHEME1;
	write_bbs(Com,"\r\n",2,1);
	get_string("ASKTOWN",buf);
	write_bbs(Com,buf,strlen(buf),1);
	SET_SCHEME2;
	bbsinput.isname=1;
	get_input(Com,curuser.town,40,&bbsinput);
	bbsinput.isname=0;
}

//==========================================================
	void change_telephone(int Com)
//==========================================================
{
char buf[1024];

// tel
	SET_SCHEME1;
	write_bbs(Com,"\r\n\n",3,1);
	get_string("ASKTEL",buf);
	write_bbs(Com,buf,strlen(buf),1);
	SET_SCHEME2;

	bbsinput.onlydigits=1;
	get_input(Com,curuser.tel,20,&bbsinput);
	bbsinput.onlydigits=0;

	if (exitatonce)
		return ;

// modem
	SET_SCHEME1;
	write_bbs(Com,"\r\n",2,1);
	get_string("ASKMODEM",buf);
	write_bbs(Com,buf,strlen(buf),1);
	SET_SCHEME2;

	bbsinput.onlydigits=1;
	get_input(Com,curuser.modem,20,&bbsinput);
	bbsinput.onlydigits=0;
}

//==========================================================
	void change_ansi(int Com)
//==========================================================
{
	char buf[1024];

	SET_SCHEME1;
	write_bbs(Com,"\r\n\n",3,1);
	get_string("ASKANSI",buf);
	write_bbs(Com,buf,strlen(buf),1);

	if (ask_yes_no(Com) == 1)
		curuser.ansi = 1;
	else
		curuser.ansi =0;

	useansi=curuser.ansi;
}

//==========================================================
	void change_lps(int Com)
//==========================================================
{
char buf[1024];

	SET_SCHEME1;
	write_bbs(Com,"\r\n\n",3,1);
	get_string("ASKLPS",buf);
	write_bbs(Com,buf,strlen(buf),1);
	SET_SCHEME2;

	sprintf(buf,"%d",curuser.lps);
	bbsinput.onlydigits=1;
	get_input(Com,buf,2,&bbsinput);
	bbsinput.onlydigits=0;
	curuser.lps=atoi(buf);
}

//==========================================================
	int make_new_user(int Com)
//==========================================================
{
CStrList lang,bbsuser;
CString help;
char buf[4096],pwd1[200],pwd2[200];

	display_file(Com,"NEWUSER");

// language
	lang.RemoveAll();

	if (!change_language(Com))
		return 0;

// ansi
	change_ansi(Com);

	if (exitatonce)
		return 0;

// street
	change_mailing_adress(Com);

	if (exitatonce)
		return 0;

// phone
	change_telephone(Com);

	if (exitatonce)
		return 0;

	curuser.lps = 24;
	change_lps(Com);

	if (exitatonce)
		return 0;

pwdagain:

	if (!change_pwd(Com,pwd1,pwd2))
		return 0;

	if (strcmp(pwd1,pwd2) !=0)
	{
		SET_SCHEME1;
		write_bbs(Com,"\r\n",2,1);
		get_string("PWDMISMATCH",buf);
		write_bbs(Com,buf,strlen(buf),1);
		write_bbs(Com,"\r\n",2,1);

		if (exitatonce)
			return 0;

		goto pwdagain;
	}

	if (strlen(pwd1) < 1)
	{
		SET_SCHEME1;
		write_bbs(Com,"\r\n",2,1);
		get_string("ASKEMPTYPWD",buf);
		write_bbs(Com,buf,strlen(buf),1);

		if (!ask_yes_no(Com))
			goto pwdagain;

		if (exitatonce)
			return 0;
	}

	curuser.pwdcrc32=0;
	RS_GetCRC_CCITT32 (pwd1,strlen(pwd1),&curuser.pwdcrc32); // save pwd
	/*
	static struct _currentuser
		{
			char fullname[300];
			char firstname[300];
			char lastname[300];
			char street[300];
			char town[300];
			char tel[300];
			char modem[300];
			int	 sec;
			int lps;
			int ansi;
			char groups[300];
			int pwdcrc32;
			char language[300];
		} curuser;

 	// 0  name
	// 1  pwd		CRC32 !
	// 2  seclevel
	// 3  groups
	// 4  street
	// 5  town
	// 6  tel
	// 7  modem/fax
	// 8  lines per screen  (int)
	// 9  ansi-color 0 or 1 (int)
	// 10 language
*/

// lang is used to read bbs-defaults
	lang.LoadFromFile("BBSMAIN.CFG");

// set user-defaults
	curuser.sec=get_token_int(lang.GetString(4),0);
	get_token(lang.GetString(0),1,help);
	strcpy(curuser.groups,help);
	bbsuser_struct2string(buf);
	bbsuser.LoadFromFile("BBSUSER.CFG");
	bbsuser.AddTail(buf);
	bbsuser.SaveToFile("BBSUSER.CFG");

	return 1;
}

//==========================================================
	int bbs_startup(int Com)
//==========================================================
// main bbs function
//
{
ULONG	pwdcrc32;
CStrList bbsuser;
char	buf[1024],username[300],name[1000],pwd[300],tmp[300];
int		t,found,wrongpwdcount=0;

	memset(&bbsinput,0,sizeof(bbsinput));
	memset(username,0,sizeof(username));
	memset(pwd,0,sizeof(pwd));

	bbsuser.LoadFromFile("BBSMAIN.CFG"); // bbsuser is misused here !
	strcpy(language,bbsuser.GetString(2));
	strupr(language);
	display_file(Com,"logo");

	SET_SCHEME1;
	if (exitatonce)
		return 0;

	sprintf(buf,"\n\n\r%s BBS",get_versioninfo(tmp,0));
	write_bbs(Com,buf,strlen(buf),1);

	set_color(Com,BGBLACK);
	set_color(Com,FGCYAN);
	set_color(Com,HIGH);

	sprintf(buf,"\n\r(c) 1995-2003 by Deutch & Russian teams");
	write_bbs(Com,buf,strlen(buf),1);

	set_color(Com,BGBLACK);
	set_color(Com,FGBLUE);
	set_color(Com,HIGH);

	sprintf(buf,L("S_29"));
	CharToOem(buf,buf);
	write_bbs(Com,buf,strlen(buf),1);

startagain:

	memset(&curuser,0,sizeof(curuser));
	curuser.tleft=600;

	if (exitatonce)
		return 0;

	sprintf(buf,"\n\n\r");
	write_bbs(Com,buf,strlen(buf),1);
	SET_SCHEME1;
	get_string("LOGIN",buf);
	write_bbs(Com,buf,strlen(buf),1);
	SET_SCHEME2;

	strcpy(buf,"");
	bbsinput.isname=1;
	get_input(Com,username,30,&bbsinput);
	bbsinput.isname=0;

	if (exitatonce)
		return 0;

	extractstr(1,username," ",curuser.firstname,250);
	extractstr(2,username," ",curuser.lastname,250);
	sprintf(curuser.fullname,"%s %s",curuser.firstname,curuser.lastname);
	SET_SCHEME1;

	bbsuser.LoadFromFile("BBSUSER.CFG");
	found=0;

	for (t=0;t<bbsuser.GetCount();t++)
	{
		get_token(bbsuser.GetString(t),0,name);
		if (stricmp(name, curuser.fullname) == 0)
		{
 			// 0  name
			// 1  pwd		CRC32 !
			// 2  seclevel
			// 3  groups
			// 4  street
			// 5  town
			// 6  tel
			// 7  modem/fax
			// 8  lines per screen
			// 9  ansi-color 0 or 1 (int)

			curuser.sec=get_token_int(bbsuser.GetString(t),2);
			curuser.lps=get_token_int(bbsuser.GetString(t),8);
			curuser.ansi=get_token_int(bbsuser.GetString(t),9);
			get_token(bbsuser.GetString(t),10,name);

			sscanf(name,"%x",&curuser.pwdcrc32);
			useansi=curuser.ansi;

			get_token(bbsuser.GetString(t),4,name);
			strcpy(curuser.street,name);
			get_token(bbsuser.GetString(t),3,name);
			strcpy(curuser.groups,name);
			get_token(bbsuser.GetString(t),5,name);
			strcpy(curuser.town,name);
			get_token(bbsuser.GetString(t),6,name);
			strcpy(curuser.tel,name);
			get_token(bbsuser.GetString(t),7,name);
			strcpy(curuser.modem,name);
			get_token(bbsuser.GetString(t),1,name);
			strcpy(curuser.language,name);
			strcpy(language,curuser.language);
			curuser.tleft=get_token_int(bbsuser.GetString(t),11);
			get_token(bbsuser.GetString(t),12,name);
			strcpy(curuser.lastlogin,name);
			
			_strdate(buf); // get current date

			if (strcmp(curuser.lastlogin,buf)!=0)
			{
                CStrList grps;
				int i;

				grps.LoadFromFile("bbsgrps.cfg");
				grps.Sort(0);

				for (int t=0;t<grps.GetCount();t++)
				{
					i=get_token_int(grps.GetString(t),0);
					if (i <= curuser.sec)
					{
						i=get_token_int(grps.GetString(t),1);
						curuser.tleft=i*60; // minutes to seconds
					}
				}
			}

			found=1;
			if (curuser.pwdcrc32 !=0)
			{
				SET_SCHEME1;
				write_bbs(Com,"\n\r",3,1);
				get_string("PASSWORD",buf);
				write_bbs(Com,buf,strlen(buf),1);
				strcpy(buf,"");
				SET_SCHEME2;

				strcpy(pwd,"");
				bbsinput.ispassword=1;
				bbsinput.onlyuppercase=1;
				get_input(Com,pwd,16,&bbsinput);
				bbsinput.ispassword=0;
				bbsinput.onlyuppercase=0;

				if (exitatonce)
					return 0;

				// get crc32 of pwd
				pwdcrc32=0;
  				RS_GetCRC_CCITT32 (pwd,strlen(pwd),&pwdcrc32);

				// found the user in database, now check pwd
				if (curuser.pwdcrc32 == pwdcrc32)
					return 1;	// correct pwd
				else
				{	// wrong pwd
					set_color(Com,BGBLACK);set_color(Com,FGRED);set_color(Com,HIGH);
					write_bbs(Com,"\n\r\n",3,1);
					get_string("WRONGPWD",buf);
					write_bbs(Com,buf,strlen(buf),1);
					write_bbs(Com,"\n\r\n",3,1);
					wrongpwdcount++;
					modem_add_listbox(L("S_17",curuser.fullname));
					goto startagain;
				}
			}
		}
	}

	if (!found)
	{
		char tmp[1024];
        CStrList grps;
		int i;

		write_bbs(Com,"\n\r\n",3,1);

		get_string("UNKNOWN",buf);
		sprintf(tmp,buf,username);
		write_bbs(Com,tmp,strlen(tmp),1);

		if (!ask_yes_no(Com))
			goto startagain;

		make_new_user(Com);
		grps.LoadFromFile("bbsgrps.cfg");
		grps.Sort(0);

		for (t=0;t<grps.GetCount();t++) // now find online.time for new user
		{
			i=get_token_int(grps.GetString(t),0);
			if (i <= curuser.sec)
				curuser.tleft=get_token_int(grps.GetString(t),1)*60;// minutes to seconds
		}
	}
	return 1;
}

//==========================================================
	int checkgroup(char *ga, char * gu)
//==========================================================
// first parameter is the groups which have access
// second parameter are the groups user belongs
// if all chars in gu are found in ga retval is 1
//
{
	if (strchr(ga,'*')!=0 || strlen(ga)==0)
		return 1;	// all groups have access or no groups defined

	for (int t=0;t<(int)strlen(ga);t++)
		if (!strchr(gu,ga[t]))
			return 0;

	return 1;
}

//==========================================================
	int check_farea(CString &area)
//==========================================================
// checks user access level to this file-area
//
{
CString help;
int		sec=0;

	sec=get_token_int(area,2);
	get_token(area,3,help);

	if (sec>curuser.sec || !checkgroup(PS help,curuser.groups))
		return 0;

	return 1;
}

//==========================================================
	void change_file_area(int Com)
//==========================================================
// lets user change the current filearea
{
char	buf[1024];
int		count=0;
int		t;
CString help;

	black_return(Com,2);
	if (curfarea.GetAt(0) == '@') // fuer den user stehen keine areas zur verfuegung !
		return;

	clear_screen(Com,TRUE);
	get_string("AREASAVAIL",buf);
	write_bbs(Com,buf,strlen(buf),1);
	black_return(Com,2);

	farea.LoadFromFile("bbsfare.cfg");

	count = 2;

	for (t=0;t<farea.GetCount();t++)
	{
		help=farea.GetString(t);
		if (check_farea(help))
		{
			get_token(farea.GetString(t),1,help);
			help.AnsiToOem();
			sprintf(buf,"^[FE%4d ^[FB%s\n\r",t+1,help);
			write_bbs(Com,buf,strlen(buf),1);
			count++;

			if (count>curuser.lps)
			{

				int i;

				get_string("CONTQUEST",buf);

				write_bbs(Com,buf,strlen(buf),1);

				if (!ask_yes_no(Com))
				{
					black_return(Com,2);
					break;
				}

				SET_SCHEME1;
				write_bbs(Com,"\r",1,1);
				for (i=0;i<70;i++)
					write_bbs(Com," ",1,1);
				write_bbs(Com,"\r",1,1);
			}
		}
	}

	black_return(Com,1);
	get_string("SELECTAREA",buf);
	strcat(buf," ");
	write_bbs(Com,buf,strlen(buf),1);

	strcpy(buf,"1");

	SET_SCHEME2;

	strcpy(buf,"");
	bbsinput.onlydigits=1;
	get_input(Com,buf,3,&bbsinput);
	bbsinput.onlydigits=0;

	if (exitatonce)
		return ;

	if (strlen(buf)>0)
	{
		t = atoi(buf);

		if ((t-1) > farea.GetCount()) // invalid selection
			return;
		help=farea.GetString(t-1);
		if (t!=0 && check_farea(help))
			curfarea=farea.GetString(t-1);
	}
}


//==========================================================
	void write_wrapped(int Com,char *b, int * count)
//==========================================================
// writes a description-lines, wrappes the line if it is too long
// only used in list_files_bbs
{
char * p;
char tmp[8192];
char tmp2[8192];
int t;

	strcpy(tmp,b);
	p = tmp;
	while (1)
	{
		if (strlen(tmp) <= desclen)
		{
			write_bbs(Com,tmp,strlen(tmp),1);
			tmp[0]=0;
		}
		else
		{
			strcpy(tmp2,tmp);
			p=tmp2+desclen-1;

			while (p>tmp2 && *p != ' ')
				p--;

			if (p > tmp2)
			{

				*p=0;
				write_bbs(Com,tmp2,strlen(tmp2),1);
				black_return(Com,1);

				for (t=0;t<30;t++)
					write_bbs(Com," ",1,1);

				if (*count >=0)
					*count = *count +1;
				p++;
				strcpy(tmp,p);
			}
			else // line contains no space !
			{
				p=tmp2+desclen;
				*p=0;
				write_bbs(Com,tmp2,strlen(tmp2),1);
				black_return(Com,1);
				for (t=0;t<30;t++)
					write_bbs(Com," ",1,1);
				if (*count >=0)
					*count = *count +1;
				p++;
				strcpy(tmp,p);
			}


		}

		if (strlen(tmp) < 1)
			break;
	}
}

//==========================================================
	void get_file_sizedate(char * path,char * fname,char * size,char * date)
//==========================================================
{
char tmp[300];
struct _finddata_t se;
float t;
long hfile;

	strcpy(tmp,path);
	strcat(tmp,fname);

	strcpy(size,"");
	strcpy(date,"^[F4>^[FCOFFLINE^[F4<");

	if (!existfile(tmp))
		return;

	hfile = _findfirst(tmp,&se);

	if (hfile != -1L)
	{
		CTime z;
		strcpy(date,"        ");
		z = se.time_write; // strftime
		strcpy(date,PS z.Format("%d.%b %y"));

		t = (float)se.size;

		if (t>= 1022976.0)
		{
			t = t/(float)1048576.0;// display in Megabytes
			sprintf(tmp,"%4.1f^[F3M",t);
		}
		else
		{
			if (t>= 999.0)
			{
				t = t/1024; // display kilobytes
				if (t<1)
					t=(float)1;

				sprintf(tmp,"%4d^[F3k",(int)t);
			}
			else
				sprintf(tmp,"%4.0f^[F3b",t); //bytes
		}

		sprintf(size,"%-8s",tmp);
		_findclose(hfile);
	}
}

//==========================================================
	int select_tagged(int Com,char *markarray,char markfile[50][50],int * count, CStrList & taglist)
//==========================================================
{
char what;
char ask[300];

askagain:
//	black_return(Com,1);
	get_string("LISTFILES",ask);
	write_bbs(Com,ask,strlen(ask),1);

	do
	{
		if (exitatonce)
			return 0;

		what = get_a_char(Com);
		if (!what)
			Sleep(100);

		if (what)
		{
			if (strchr(yeschar,what) || strchr("\r\n",what))
				break;

			if (strchr(continuechar,what))
			{
				(*count)=-1;
				break;
			}

			if (strchr(markchar,what))
			{

				char tmp[300];
				int t;

				if (exitatonce)
					return 0;

				get_string("ASKMARK",tmp);
				write_bbs(Com,"\r",1,1);
				for (t=0;t<75;t++)
					write_bbs(Com," ",1,1);
				write_bbs(Com,"\r",1,1);
				write_bbs(Com,tmp,strlen(tmp),1);
				strcpy(tmp,"");
				
				SET_SCHEME2;
				bbsinput.onlyuppercase=1;
				bbsinput.nodupes=1;
				get_input(Com,tmp,30,&bbsinput);
				bbsinput.onlyuppercase=0;
				bbsinput.nodupes=0;

				if (exitatonce)
					return 0;

				SET_SCHEME1;
				for (t=0;t<(int)strlen(tmp);t++)
				{
					int i;

					for (i=0;i<50;i++)
					{
						if (markarray[i] == tmp[t])
						{
							// add file to taglist
							if (strlen((markfile)[i]) >0)
							taglist.AddTail((markfile)[i]);
							break;
						}
					}
				}

				write_bbs(Com,"\r",1,1);
				for (t=0;t<78;t++)
					write_bbs(Com," ",1,1);
				write_bbs(Com,"\r",1,1);

				goto askagain;
			}
			if (strchr(nochar,what))
				return 0;
		}

	} while(1);

	return 1;
}

//==========================================================
	void list_files_bbs(int Com,char * fbbs, CStrList &taglist)
//==========================================================
{
char	markarray[50];
char	markfile[50][50];
char	buf[8192];
char	marker[10];
char	path[300];
char	ask[300];
FILE	*fp;
int		count=0;
int		mcount=0;
int		foundone=0;

	strcpy(markarray,"ABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890!\"§$%&/()=+*#");

	memset(marker,0,sizeof(marker));
	memset(markfile,0,sizeof(markfile));

	if (!existfile(fbbs))
	{
		black_return(Com,2);
		sprintf(buf,langstr("^[FE%s^[FB not found, please contact SysOp !",
			"^[FE%s^[FB не найден. Пожалуйста, сообщите SysOp-у !"),fbbs);
		CharToOem(buf,buf);
		write_bbs(Com,buf,strlen(buf),1);
		Sleep(2000);
		return;
	}

	strcpy(path,fbbs);

	fp = fopen(fbbs,"rt");	// open FILES.BBS
	if (!fp)
	{
		black_return(Com,2);
		sprintf(buf,langstr("^[FBCannot access ^[FE%s^[FB, please contact SysOp !",
			"^[FBНет доступа к ^[FE%s^[FB. Пожалуйста, сообщите SysOp-у !"),fbbs);
		CharToOem(buf,buf);
		write_bbs(Com,buf,strlen(buf),1);
		Sleep(2000);
		return;
	}

	if (strlen(path) > 0)
	while (path[strlen(path)-1] != '\\')
	{
		path[strlen(path)-1]=0;
		if (strlen(path) < 1)
			break;
	}

	get_string("LISTFILES",ask);
	get_string("YESCHAR",yeschar);
	get_string("NOCHAR",nochar);
	get_string("MARKCHAR",markchar);
	get_string("CONTINUE",continuechar);

	clear_screen(Com,TRUE);
	black_return(Com,1);

	while (fgets(buf,8100,fp))
	{
		char *p;

		kill_tail(buf);
		if (exitatonce)
			goto closedisplay;

		if (strlen(buf)>0)
		{
			char tmp[8000];

			p = buf;
			while (*p && (*p==' ' || *p=='\t'))	// skipping begining spaces and tabs
				p++;

			if (*p != '+')
			{	// line with filename
				char *b;

				if (buf[0] != ' ') // this is a file
				{
					char sizestr[100];
					char datestr[100];

					b=p;
					while (*b && *b != ' ')
						b++;
					
					*b=0;
					b++;
					while (*b && *b == ' ')
						b++;

					marker[0]=markarray[mcount];
					marker[1]=0;
					strcpy(markfile[mcount],p); // store filename
					mcount++;
					get_file_sizedate(path,p,sizestr,datestr);
					foundone=1;

					sprintf(tmp,"^[FD%s ^[FB%-12s^[FE%-5s ^[FA%s ",marker,p,sizestr,datestr);
					write_bbs(Com,tmp,strlen(tmp),1);
					SET_SCHEME1;
					write_wrapped(Com,b,&count);
				}
				else	// this is a commentline
				{
					set_color(Com,BGBLACK);
					set_color(Com,FGMAGENTA);
					set_color(Com,HIGH);
					write_bbs(Com,buf,strlen(buf),1);
				}
			}
			else	// line continuation
			{
				int t;

				SET_SCHEME1;
				p++;
				for (t=0;t<30;t++)
					write_bbs(Com," ",1,1);

				write_wrapped(Com,p,&count);
			}
		}

		black_return(Com,1);

		if (count >=0)
			count++;

		if (mcount >= curuser.lps || mcount >=50)
			mcount =0;

		if (count >= curuser.lps-2)
		{
			int t,ret;

			ret=select_tagged(Com,markarray,markfile,&count,taglist);
			foundone=0;

			if (!ret)
				goto closedisplay;

			black_return(Com,0);
			write_bbs(Com,"\r",1,1);

			for (t=0;t<75;t++)
				write_bbs(Com," ",1,1);
			write_bbs(Com,"\r",1,1);

			if (count >=0)
				count=0;
		}
	}

closedisplay:
	fclose(fp);

	if (foundone && ! exitatonce)
		select_tagged(Com,markarray,markfile,&count,taglist);

/*	get_string("ENDLIST",buf);
	black_return(Com,1);
	write_bbs(Com,buf,strlen(buf),1);
	do 
	{
		Sleep(100);
		if (exitatonce)		break;
	} while (get_a_char(Com) == 0);*/
}

//==========================================================
	int handle_menu_line(int Com,char * line,CStrList & commands, char * commandlist)
//==========================================================
{
char * p,*e;
char sec[300];
int seclvl;
char group[300];

	kill_tabbs(line);
	kill_tail(line);

	p = line;

	while (*p && *p==' ')
		p++;

	memset(sec,0,sizeof(sec));
	memset(group,0,sizeof(group));
	seclvl=0;

	extractstr(2,line," ",sec,299);
	extractstr(3,line," ",group,299);

	seclvl=atoi(sec);

	if (seclvl <= curuser.sec && checkgroup(group,curuser.groups))
	{
		if (*p) // this is a valid line
		{
			char tmp[1024];
			char tmp2[1024];
			char tmp3[1024];
			char tmp4[1024];
			char tmp5[1024];
			char tmp6[1024];
			char tmp7[1024];
			char tmp8[1024];

			extractstr(1,line," ",tmp,999);

			if (strcmpi("DISPLAYFILE",tmp)==0)
			{
				extractstr(4,line," ",tmp2,999);
				display_file(Com,tmp2);
			}

			if (strcmpi("ADDCOMMAND",tmp)==0)
			{
				char test[1024];

				memset(tmp2,0,sizeof(tmp2));
				memset(tmp3,0,sizeof(tmp3));
				memset(tmp4,0,sizeof(tmp4));
				memset(tmp5,0,sizeof(tmp5));
				memset(tmp6,0,sizeof(tmp6));
				memset(tmp7,0,sizeof(tmp7));
				memset(tmp8,0,sizeof(tmp8));

				extractstr(4,line," ",tmp2,999);
				extractstr(5,line," ",tmp3,999);
				extractstr(6,line," ",tmp4,999);
				extractstr(7,line," ",tmp5,999);
				extractstr(8,line," ",tmp6,999);
				extractstr(9,line," ",tmp7,999);
				extractstr(10,line," ",tmp8,999);

				sprintf(test,"%s\t%s\t%s\t%s\t%s\t%s\t%s",tmp2,tmp3,tmp4,tmp5,tmp6,tmp7,tmp8);

				commands.AddTail(test);
				strcat(commandlist,tmp2);
			}

			if (strcmpi("OUTPUT",tmp)==0)
			{
				e = strstr(line,tmp);

				while (*e && *e != '"')
					e++;

				SET_SCHEME1;

				if (*e)
				{
					char *o;
					e++;
					o = e;

					while (*o && *o != '"')
						o++;
					
					*o=0;
					strcpy(tmp,e);
					write_bbs(Com,tmp,strlen(tmp),1);
				}
				black_return(Com,1);
			}
		}
	}
	return 1;
}

//==========================================================
	void redraw_fullscreen_chatter(int Com,int fsline,int fuline,int sline)
//==========================================================
{
char tmp[300],tmp2[300];

	clear_screen(Com,TRUE);
	bbs_gotoxy(Com,1,fsline);
	sprintf(tmp,"^[FE^[B1 %-78s",sysopname);
	write_bbs(Com,tmp,strlen(tmp),1);
	bbs_gotoxy(Com,1,fuline);
	sprintf(tmp,"^[FF^[B2 %-78s",curuser.fullname);
	write_bbs(Com,tmp,strlen(tmp),1);
	get_string("CHATINFO",tmp2);
	bbs_gotoxy(Com,1,sline);
	sprintf(tmp,"^[FF^[B1 %-78s",tmp2);
	write_bbs(Com,tmp,strlen(tmp),1);
}

//==========================================================
	void sysop_chat(int Com,int ask)
//==========================================================
{
int ende=0;
int t;
char ch;
int lastuser=0;
char tmp[1024];
int fsline,fuline,sline;
CStrList  user;
CStrList  sysop;
char callwave[400];

char cuser[300], csysop[300];

int lsx,lsy,lux,luy;
time_t starttime,akttime,tmptime=0;
int count=0;

	get_fullname(sysopname);
	sysopchat=0;
	if (!ask)
		goto direktchat;

	user.LoadFromFile("bbsmain.cfg");

	strcpy(callwave,user.GetString(3));
	user.RemoveAll();

	if (strlen(callwave)>0)
		if (!existfile(callwave))
			strcpy(callwave,"BEEP");

	// ask user

	get_string("CHATASK",tmp);
	black_return(Com,2);
	write_bbs(Com,tmp,strlen(tmp),1);

	memset(tmp,0,sizeof(tmp));
	memset(&bbsinput,0,sizeof(struct _bbsinput));
	get_input(Com,tmp,30,&bbsinput);

	if (strlen(tmp)<1)
		return;

	get_string("CHATWAIT",tmp);
	black_return(Com,2);
	write_bbs(Com,tmp,strlen(tmp),1);
	black_return(Com,1);

	time(&starttime);
	ende=0;

	strcpy(tmp,"^[FF<SysOpInfo> Press ^[FAC ^[FFto chat, ^[FAA ^[FFto abort !");
	write_bbs(Com,tmp,strlen(tmp),2); // 2 means, do not display to user

	do {
		time(&akttime);

		if (tmptime != akttime)
		{
			count--;
			tmptime=akttime;
		}

		if (count < 1)
		{
			if (strcmpi(callwave,"BEEP") ==0)
			{
				MessageBeep(0xffffffff);
				Sleep(50);
				MessageBeep(0xffffffff);
			}
			else
				PlaySound(callwave,0,SND_ASYNC);

			count = 5;
		}

		if ((akttime-starttime) > 30)
			ende = 2;

		ch = get_a_char(Com);
		if (ch && lastkey == FROMSYSOP)
		{
			ch = _toupper(ch);

			if (ch == 'A')
				ende = 2;

			if (ch == 'C')
				ende = 1;
		}

		if (exitatonce)
			return ;

	} while (ende==0);


	if (ende==2)
	{
		black_return(Com,2);
		get_string("CHATABORT",tmp);
		write_bbs(Com,tmp,strlen(tmp),1);
		Sleep(1500);
		sysopchat=0;
		return;
	}

direktchat:

	ch=0;
	ende=0;

	if (!useansi)
	{
		black_return(Com,1);
		get_string("CHATSTART",tmp);
		write_bbs(Com,tmp,strlen(tmp),1);
		black_return(Com,2);

		do
		{
			ch = get_a_char(Com);

			if (exitatonce)
				return ;

			if (ch!= 0 && ch != 27)
			{
				if (lastuser != lastkey)
				{
					switch (lastkey)
					{
						case FROMSERIAL:	
							strcpy(tmp,"^[FB");
							strcat(tmp,curuser.fullname);
							strcat(tmp,": ");
							black_return(Com,1);
							write_bbs(Com,tmp,strlen(tmp),1);
							break;
						case FROMSYSOP:		
							strcpy(tmp,"^[FE");
							strcat(tmp,sysopname);
							strcat(tmp,": ");
							black_return(Com,1);
							write_bbs(Com,tmp,strlen(tmp),1);
							break;
					}

					lastuser = lastkey;
				}

				tmp[0]=ch;
				tmp[1]=0;
				write_bbs(Com,tmp,1,1);
			}

			if (ch == 27) // end chat with ESC
				ende = 1;

			if (ch == 13)  // nexttime write username again
			{
				black_return(Com,1);
				lastuser=0;
			}

		} while (!ende);

		sysopchat=0;
		return;
	}
	else // fullscreen-chatter
	{
		sysop.RemoveAll();
		user.RemoveAll();

		fsline=1;
		fuline=10;
		sline=20;
		lux=1;
		lsx=1;
		luy=fuline+1;
		lsy=fsline+1;

		redraw_fullscreen_chatter(Com,fsline,fuline,sline);
		black_return(Com,0);
		do
		{
			ch = get_a_char(Com);

			if (exitatonce)
				return ;

			if (ch!= 0 && ch != 27 && ch != 8 && ch != 10)
			{
				if (lastuser != lastkey)
				{
					switch (lastkey)
					{
						case FROMSERIAL:	
							bbs_gotoxy(Com,lux,luy);
							set_color(Com,HIGH);
							set_color(Com,FGCYAN);
							break;
						case FROMSYSOP:		
							bbs_gotoxy(Com,lsx,lsy);
							set_color(Com,FGYELLOW);
							set_color(Com,HIGH);
							break;
					}
					lastuser = lastkey;
				}

				tmp[0]=ch;
				tmp[1]=0;

				if (ch!=13)
				{
					switch (lastkey)
					{
						case FROMSERIAL:	
							strcat(cuser,tmp);
							write_bbs(Com,tmp,1,1);
							lux++;
							break;
						case FROMSYSOP:		
							strcat(csysop,tmp);
							write_bbs(Com,tmp,1,1);
							lsx++;
							break;
					}
				}
				else
				{
					lastuser=0;
					switch (lastkey)
					{
						case FROMSERIAL:	
							lux=79;
							break;
						case FROMSYSOP:		
							lsx=79;
							break;
					}
				}

				if (lsx > 78) // handle sysop-window
				{
					set_color(Com,HIGH);
					set_color(Com,FGYELLOW);

					sysop.AddTail(csysop);

					if (sysop.GetCount() > (fuline-fsline-2))
					{
						sysop.RemoveHead();
						set_color(Com,FGYELLOW);
						set_color(Com,HIGH);

						for (t=0;t < (fuline-fsline-1);t++)
						{
							bbs_gotoxy(Com,1,t+2);
							clear_screen(Com,FALSE);
							write_bbs(Com,"\r",1,1);
							write_bbs(Com,PS sysop.GetString(t),strlen(PS sysop.GetString(t)),1);
						}
						write_bbs(Com,"\r",1,1);
					}
					else
					{
						lsy++;
						lsx=1;
						bbs_gotoxy(Com,lsx,lsy);
					}

					strcpy(csysop,"");
					lsx=1;
					lastuser=0;
				}

				if (lux > 78) // handle user-window
				{
					set_color(Com,HIGH);
					set_color(Com,FGCYAN);

					user.AddTail(cuser);

					if (user.GetCount() > (sline-fuline-2))
					{
						if	(user.GetCount()>0)
							user.RemoveHead();

						set_color(Com,HIGH);
						set_color(Com,FGCYAN);

						for (t=0;t < (sline-fuline-1);t++)
						{
							bbs_gotoxy(Com,1,t+1+fuline);
							clear_screen(Com,FALSE);
							write_bbs(Com,"\r",1,1);
							write_bbs(Com,PS user.GetString(t),strlen(PS user.GetString(t)),1);
						}

						write_bbs(Com,"\r",1,1);
					}
					else
					{
						luy++;
						lux=1;
						bbs_gotoxy(Com,lux,luy);
					}

					strcpy(cuser,"");
					lux=1;
					lastuser=0;
				}
			}

			if (ch==8)
			{
				strcpy(tmp,"\x8 \x8");
				switch (lastkey)
				{
					case FROMSERIAL:
						if (strlen(cuser)>0)
						{
							cuser[strlen(cuser)-1]=0;
							bbs_gotoxy(Com,lux,luy);
							write_bbs(Com,tmp,strlen(tmp),1);
							lux--;
						}
						break;
					case FROMSYSOP:
						if (strlen(csysop)>0)
						{
							csysop[strlen(csysop)-1]=0;
							bbs_gotoxy(Com,lsx,lsy);
							write_bbs(Com,tmp,strlen(tmp),1);
							lsx++;
						}
						break;
				}
			}

			if (ch==27)
				ende = 1;

		} while (!ende);
	}

	sysopchat=0;
}

//==========================================================
	void edit_marked_files(int Com,CStrList & taglist)
//==========================================================
{
char tmp[1024];
int t;

	black_return(Com,2);
	if (taglist.GetCount() < 1)
	{
		get_string("NOFLSTAGGED",tmp);
		write_bbs(Com,tmp,strlen(tmp),1);
		Sleep(1500);
		return;
	}

editagain:

	if (exitatonce)
		return ;

	black_return(Com,0);
	clear_screen(Com,TRUE);

	get_string("MARKEDFILES",tmp);
	get_string("ABORT",abortchar);
	get_string("DELCHAR",delchar);

	write_bbs(Com,tmp,strlen(tmp),1);
	black_return(Com,1);

	for (t=0;t<taglist.GetCount();t++)
	{
		sprintf(tmp,"^[FF%02d ^[FB%s\r\n",t+1,taglist.GetString(t));
		write_bbs(Com,tmp,strlen(tmp),1);
	}

	black_return(Com,1);
	get_string("EDITMARKED",tmp);
	write_bbs(Com,tmp,strlen(tmp),1);

	t=0;
	while (!t)
	{
		char ch;
		ch = get_a_char(Com);

		if (exitatonce)
			return ;

		if (ch)
		{
			if (strchr(abortchar,ch) !=0)
				t=1;

			if (strchr(delchar,ch) !=0)
			{
				int which;

				black_return(Com,1);
				get_string("DELMARKED",tmp);
				write_bbs(Com,tmp,strlen(tmp),1);
				strcpy(tmp,"");
				SET_SCHEME2;

				bbsinput.onlydigits=1;
				get_input(Com,tmp,30,&bbsinput);
				bbsinput.onlydigits=0;
				which = atoi(tmp);

				if (which)
				{
					int i;
                    CStrList temp;

					which--; // from number to index

					temp.RemoveAll();

					for (i=0;i<taglist.GetCount();i++)
						if (i != which)
							temp.AddTail(taglist.GetString(i));

					taglist.RemoveAll();

					for (i=0;i<temp.GetCount();i++)
						taglist.AddTail(temp.GetString(i));
				}
				goto editagain;
			}
		}
	}
}

//==========================================================
	void search_in_farea(char * path, char * fname,CStrList &found)
//==========================================================
{
struct _finddata_t se;
long hfile;
char tmp[300];

	hfile = _findfirst(fname,&se);

	if (hfile != -1L)
	{
		do {
			if (!(se.attrib & _A_SUBDIR))
			{
				if (strcmpi("FILES.BBS",se.name) && strcmpi("FILES.BAK",se.name))
				{
					strcpy(tmp,path);
					addbackslash(tmp);
					_toupper(tmp);
					strcat(tmp,se.name);
					found.AddTail(tmp);
				}
			}
		} while (_findnext(hfile, &se)== 0);

		_findclose(hfile);
	}
}


//==========================================================
	void clear_dir(const char *path)
//==========================================================
{
struct _finddata_t se;
long hfile;
char tmp[1024];

	strcpy(tmp,path);
	addbackslash(tmp);
	strcat(tmp,"*.*");
	hfile = _findfirst(tmp,&se);
	if (hfile != -1L)             // packet-archiv found
		do {

			if (!(se.attrib & _A_SUBDIR))
			{
				char temp[300];

				strcpy(temp,path);
				addbackslash(temp);
				strcat(temp,se.name);
				unlink(temp);
			}
		} while (_findnext(hfile,&se) == 0);
}

//==========================================================
	void enter_desc(int Com,CStrList &desc,char * path,char * fname)
//==========================================================
{
int count=0,t;
char line[300];
char tmp[300];
FILE * fp;

askagain:
	count=0;
	desc.RemoveAll();

	do {
		count++;
		sprintf(line,"%-2d: ",count);
		write_bbs(Com,line,strlen(line),1);
		strcpy(line,"");
		memset(&bbsinput,0,sizeof(struct _bbsinput));
		strcpy(line,"");
		get_input(Com,line,40,&bbsinput);

		if (exitatonce)
			return;

		if (strlen(line)>0)
			desc.AddTail(line);

		black_return(Com,1);
	} while (strlen(line)>0);

	if (desc.GetCount()<1)
	{
		get_string("ULDSCNEED",line);
		write_bbs(Com,line,strlen(line),1);
		black_return(Com,1);
		goto askagain;
	}

	// now we have a description, append it to the files.bbs in the given path

	strcpy(line,path);
	addbackslash(line);
	strcat(line,"FILES.BBS");

	fp = fopen(line,"at");
	if (!fp)
	{
		sprintf(tmp,"BBS ERROR writing to %s",line);
		modem_add_listbox(tmp);

		sprintf(tmp,"^[FEError writing to ^[FB%s",line);
		write_bbs(Com,tmp,strlen(tmp),1);

		Sleep(4000);
		return;
	}

	strcpy(tmp,fname);
	strupr(tmp);

	for (t=0;t<desc.GetCount();t++)
	{
		if (t==0)
			fprintf(fp,"%-12s %s\n",tmp,desc.GetString(t));
		else
			fprintf(fp," +%s\n",desc.GetString(t));
	}

	fclose(fp);
}


//==========================================================
	void upload_file(int Com,char * path)
//==========================================================
{
CStrList files;
CStrList desc;
CString help;
char	tmppath[300];
char	tmp[1024],dummy[300],sbuf[300];
CString	basepath;
char	*p;
char	ch;
int		zresult;
int		t;

	if (Com == LOCALCOM)
	{

		black_return(Com,2);
		strcpy(tmp,langstr("^[FBNo Upload available when running in local mode !",
			"^[FBВ локальном режиме Upload невозможен !"));
		CharToOem(tmp,tmp);
		write_bbs(Com,tmp,strlen(tmp),1);

		Sleep(3000);
		return;
	}

	zmodem_compatibility_proc(000000);

	strcpy(tmppath,gc.BasePath);
	strcat(tmppath,"\\$BBSIN$");
	mkdir(tmppath);
	clear_dir(tmppath);

	if (strcmpi(path,"@CURAREA@")!=0)
		if (path[0] != '@')
			basepath=path;
		else
			return; // no area available !
	else
	{
		get_token(curfarea,0,help);
		basepath=help;
	}

	strcpy(tmp,basepath);
	p = tmp+strlen(tmp);

	if (p>tmp)
	{
		p--;
		if (*p=='\\')
			*p=0;
	}
	basepath=tmp;

	if (basepath.GetLength()<1)
	{
		black_return(Com,2);
		sprintf(tmp,langstr("\n\rNo upload-path defined, please contact SysOp !",
			"\n\rНе задана папка для Upload. Пожалуйста, сообщите SysOp-у !"));
		CharToOem(tmp,tmp);
		write_bbs(Com,tmp,strlen(tmp),1);
		Sleep(2000);
		return;
	}

	mkdir(basepath);
	black_return(Com,0);
	clear_screen(Com,TRUE);
	get_string("ULPROTO",tmp);
	write_bbs(Com,tmp,strlen(tmp),1);

	do {
		ch = get_a_char(Com);

		if (exitatonce)
			return;

	} while (ch ==0);

	mailer_stat("Receiving ZModem");
	modem_update();
	modem_add_listbox("Receiving ZModem");
	strcpy(dummy,"");
	set_burst_mode(1);

	RS_RXFile_ZMODEM(Com,dummy,&zresult,(TINFOPROC)Info,PS tmppath);

	set_burst_mode(0);	   	// evtl weg ?!?
	zm_errcode(zresult,(char*)dummy);

	sprintf(sbuf,"ZModem receive status %s",dummy);
	modem_add_listbox(sbuf);

	Sleep(1500);

	ComBufClear(Com,DIR_OUT);
	ComBufClear(Com,DIR_INC);

	// now check for received files !
	strcpy(dummy,tmppath);
	addbackslash(dummy);
	strcat(dummy,"*.*");

	files.FillWithFiles(dummy);

	if (files.GetCount()<1)
	{
		get_string("ULNOFILES",tmp);
		black_return(Com,2);
		write_bbs(Com,tmp,strlen(tmp),1);
		Sleep(2000);
		return;
	}

	for (t=0;t<files.GetCount();t++)
	{
		strcpy(dummy, tmppath);
		addbackslash(dummy);

		black_return(Com,2);
		get_string("ULDESC",sbuf);
		sprintf(tmp,sbuf,files.GetString(t));

		write_bbs(Com,tmp,strlen(tmp),1);
		black_return(Com,1);

		strcpy(tmp,basepath);
		addbackslash(tmp);

		desc.RemoveAll();

		enter_desc(Com,desc,tmp,PS files.GetString(t));

		// now copy the file to the destination-dir !

		strcat(dummy,files.GetString(t)); // source-file

		strcpy(tmp,basepath);
		addbackslash(tmp);
		strcat(tmp,files.GetString(t)); // destination file

		CopyFile(dummy,tmp,FALSE); // overwrite if exists

		_unlink(dummy);

		if (exitatonce)
			return;
	}
}

//==========================================================
	void download_files(int Com,CStrList &taglist)
//==========================================================
{
char tmp[16384],sendlist[8192];
int t,count,i;
int ende=0;
int zresult=0;
int whatprotocol=0;
char sbuf[1024],dummy[1024],path[300];
CStrList found;
CString help;

	count=0;
	zmodem_compatibility_proc(000000);

	do{ // ask for protocol

		get_string("PROTOCOL",tmp);

		black_return(Com,0);

		clear_screen(Com,TRUE);
		write_bbs(Com,tmp,strlen(tmp),1);
		black_return(Com,2);

		strcpy(tmp,"^[FA1)^[FB   ^[FEZModem (^[FD1k^[FE)");
		write_bbs(Com,tmp,strlen(tmp),1);
		black_return(Com,1);

		strcpy(tmp,"^[FA2)^[FB   ^[FEZedZap (^[FDZModem 8k^[FE)");
		write_bbs(Com,tmp,strlen(tmp),1);
		black_return(Com,2);

		strcpy(tmp,"     ^[FE==>");
		write_bbs(Com,tmp,strlen(tmp),1);

		memset(&bbsinput,0,sizeof(struct _bbsinput));
		strcpy(tmp,"");

		bbsinput.onlydigits=1;
		get_input(Com,tmp,2,&bbsinput);
		bbsinput.onlydigits=0;

		whatprotocol = atoi(tmp);

		if (exitatonce)
			return;

	} while (whatprotocol<1 || whatprotocol > 2);

	black_return(Com,2);
	sprintf(sbuf,langstr("++ ERROR unknown protocol %d","++ Неизвестный протокол %d"),whatprotocol);

	if (whatprotocol==1)
		strcpy(sbuf,langstr("++ User selected ZModem 1k","++ Пользователь выбрал ZModem 1k"));
	if (whatprotocol==2)
		strcpy(sbuf,langstr("++ User selected ZedZap 8k","++ Пользователь выбрал ZedZap 8k"));

	modem_add_listbox(sbuf);

	get_string("DLINFO",tmp);
	write_bbs(Com,tmp,strlen(tmp),1);
	black_return(Com,2);

	if (taglist.GetCount()>0)
	{
		char test[300];
		char temp[300];

		get_string("DLFILES",tmp);

		for (t=0;t<taglist.GetCount();t++)
		{
			count++;

			sprintf(test,tmp,count);
			strcat(test," ^[B1^[FF");

			sprintf(temp,"%-12s",taglist.GetString(t));
			strcat(test,temp);

			write_bbs(Com,test,strlen(test),1);
			black_return(Com,1);
		}
	}

	get_string("DLFILES",tmp);

	do {
		char fname[300];
		char test[300];

		count++;

		sprintf(test,tmp,count);
		write_bbs(Com,test,strlen(test),1);
		strcpy(fname,"");

		bbsinput.onlyuppercase=1;
		get_input(Com,fname,12,&bbsinput);
		bbsinput.onlyuppercase=0;
		black_return(Com,1);

		kill_tail(fname);

		if (strlen(fname)>0)
		{
			_toupper(fname);
			taglist.AddTail(fname);
		}
		else 
			ende=1;


	} while (!ende);

	// now search the selected files in the file-areas and ask user

	found.RemoveAll();
	memset(sendlist,0,sizeof(sendlist));

	for (i=0;i<farea.GetCount();i++)
	{
		help=farea.GetString(i);
		if (check_farea(help)) // is user allowed to download from this area
		{
			// "d:\\test\\area1\tTest-Area Nummer 1\t5\tA\td:\\test\\area1\\files.bbs");
			get_token(farea.GetString(i),0,help);
			strcpy(path,help);
			addbackslash(path);

			for (t=0;t<taglist.GetCount();t++)
			{
				make_path(tmp,help,taglist.GetString(t));
				search_in_farea(path,tmp,found);
			}
		}
	}

	get_string("YESCHAR",yeschar);
	get_string("NOCHAR",nochar);
	black_return(Com,2);

	if (found.GetCount()<1)
	{
		// no files found
		get_string("DLNOFOUND",dummy);
		write_bbs(Com,dummy,strlen(dummy),1);
		Sleep(3000);
		black_return(Com,1);
		return;
	}

	for (i=0;i<found.GetCount();i++)
	{
		char temp[300];
		char *p;
		char size[300];
		char datestr[300];
		char path[300];

		get_string("DLARCHIVE",sbuf);
		strcpy(temp,found.GetString(i));
		strcpy(path,temp);

		p = path+strlen(path);
		while (p > path && *p != '\\')
			p--;
		if (*p == '\\')
		{
			p++;
			*p=0;
		}

		p = temp+strlen(temp);
		while (p > temp && *p != '\\')
			p--;

		p++;

		get_file_sizedate(path,p,size,datestr);
		sprintf(tmp,sbuf,p,size,datestr);
		write_bbs(Com,tmp,strlen(tmp),1);

		if (ask_yes_no(Com))
		{
			if (strlen(sendlist)>0)
				strcat(sendlist," ");

			strcat(sendlist,found.GetString(i));
			write_bbs(Com," ^[FE",4,1);
			write_bbs(Com,&yeschar[0],1,1);
			sprintf(sbuf,langstr("++ Download %s","++ Прием %s"),p);
			modem_add_listbox(sbuf);
		}
		else
		{
			write_bbs(Com," ^[FF",4,1);
			write_bbs(Com,&nochar[0],1,1);
		}

		black_return(Com,1);
	}

	black_return(Com,2);
	get_string("DLSTART",tmp);
	write_bbs(Com,tmp,strlen(tmp),1);

	do 
	{
		char ch;

		ch = get_a_char(Com);

		if (exitatonce)
			return ;

		if (ch != 0)
		{
			if (ch == '\r')
				break;

			if (ch == 27)
				return;
		}

	} while (1);

	if (Com == LOCALCOM)
	{
		char buf[300];

		strcpy(buf,langstr("^[FADownload not available in local mode !",
			"^[FAВ локальном режиме Download невозможен !"));
		CharToOem(buf,buf);
		black_return(Com,2);
		write_bbs(Com,buf,strlen(buf),1);
		Sleep(2000);
		return;
	}

	set_burst_mode(1);		// start of zmodem-sendfiles

	ComBufClear(Com,DIR_OUT);
	ComBufClear(Com,DIR_INC);

	strcpy(dummy,"^[FF\n<SysOpInfo> Starting ZModem transfer...\n^[FB<SysOpInfo> ZModem Infos are visible in the mailer-window !\n");
	write_bbs(Com,dummy,strlen(dummy),2);
	mailer_stat(langstr("Sending ZModem","Передача ZModem"));
	modem_update();
	modem_add_listbox(langstr("Sending ZModem","Передача ZModem"));

	if (whatprotocol==1)
		RS_TXFile_ZMODEM(Com,sendlist,&zresult,(TINFOPROC)Info,1); // zmodem 1k
	if (whatprotocol==2)
		RS_TXFile_ZMODEM(Com,sendlist,&zresult,(TINFOPROC)Info,0); // zedzap

	zm_errcode(zresult,(char*)dummy);

	sprintf(sbuf,langstr("++ Result: [%s]","++ Результат: [%s]"),dummy);
	modem_add_listbox(sbuf);
	set_burst_mode(0);

	sprintf(sbuf,"^[FF<SysOpInfo> Zmodem send status ^[FE%s",dummy);
	write_bbs(Com,sbuf,strlen(sbuf),2);

	ComBufClear(Com,DIR_OUT);
	ComBufClear(Com,DIR_INC);

	if (strcmp(dummy,"OK")==0) // delete taglist if transfer is ok...
		taglist.RemoveAll();
}

//==========================================================
	int display_as_file(int Com,CStrList &file,char *path,int *mcount,int *count,CStrList &taglist)
//==========================================================
{
int  t;
static char markarray[50];
static char markfile[50][50];
static int first=1;
char marker[10];
char * p;
char * d;
char tmp[1024];
char sizestr[300];
char datestr[300];

	if (first)
	{
		memset(marker,0,sizeof(marker));
		memset(markfile,0,sizeof(markfile));
		strcpy((char*)markarray,"ABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890!\"§$%&/()=+*#");
		first=0;
	}

	if (*mcount == -2)
	{
		select_tagged(Com,markarray,markfile,count,taglist);
		return 1;
	}

	get_string("YESCHAR",yeschar);
	get_string("NOCHAR",nochar);
	get_string("MARKCHAR",markchar);
	get_string("CONTINUE",continuechar);

	for (t=0;t<file.GetCount();t++)
	{
		if (t==0)
		{
			char temp[1024];

			strcpy(tmp,file.GetString(t));
			p = tmp;

			while (*p && *p != ' ')
				p++;

			d = p;
			while (*d && *d == ' ')
				d++;

			*p=0;
			marker[0]=markarray[*mcount];
			marker[1]=0;
			strcpy(markfile[*mcount],tmp);

			(*mcount)++;

			get_file_sizedate(path,tmp,sizestr,datestr);

			sprintf(temp,"^[FD%s ^[FB%-12s^[FE%-5s ^[FA%s ",marker,tmp,sizestr,datestr);
			write_bbs(Com,temp,strlen(temp),1);
			set_color(Com,FGWHITE);
			set_color(Com,HIGH);
			write_wrapped(Com,d,count);
		}
		else
		{
			int k;

			strcpy(tmp,file.GetString(t));
			p = tmp;
			if (strchr(tmp,'+')!=0) // nur wenn ein plus drin ist, ansonsten nur anzeigen...
			{
				while (*p && *p != '+')
					p++;
				p++;

				for (k=0;k<30;k++)
					write_bbs(Com," ",1,1);
			}
			write_wrapped(Com,p,count);
		}

		black_return(Com,1);

		if (*count >=0)
			(*count)++;

		if (*count >= curuser.lps)
		{
			int ret= select_tagged(Com,markarray,markfile,count,taglist);

			black_return(Com,0);
			write_bbs(Com,"\r",1,1);

			for (t=0;t<75;t++);
				write_bbs(Com," ",1,1);
			write_bbs(Com,"\r",1,1);

			*count=0;

			if (!ret)
				return 0;
		}
	}
	return 1;
}

//==========================================================
	void search_for_files(int Com,CStrList &taglist)
//==========================================================
{
char tmp[1024];
char pattern[100];
char fname[300];
char path[300];
int mcount=0;
int count=0;
int t;
FILE * fp;
CString help;
CStrList lfile;

	get_string("SEARCHFILE",tmp);
	black_return(Com,2);
	write_bbs(Com,tmp,strlen(tmp),1);
	strcpy(pattern,"");
	bbsinput.onlyuppercase=1;
	get_input(Com,pattern,25,&bbsinput);
	bbsinput.onlyuppercase=0;
	sprintf(tmp,"User searched for \"%s\"",pattern);
	modem_add_listbox(tmp);
	memset(tmp,0,sizeof(tmp));

	black_return(Com,0);
	clear_screen(Com,TRUE);

	if (strlen(pattern) < 1)
		return;

	if (exitatonce)
		return;

	farea.LoadFromFile("bbsfare.cfg");

	for (t=0;t<farea.GetCount();t++)
	{

		if (exitatonce)
			return;

		help=farea.GetString(t);
		if (check_farea(help))
		{
			get_token(farea.GetString(t),1,help);
			help.AnsiToOem();
			write_bbs(Com,PS help,help.GetLength(),1);
			black_return(Com,1);
			count++;

			get_token(farea.GetString(t),0,help);
			strcpy(fname,help);
			strcpy(path,fname);
			addbackslash(path);
			addbackslash(fname);
			strcat(fname,"FILES.BBS");

			if (existfile(fname))
			{
				fp = fopen(fname,"rt");
				if (fp)
				{
					lfile.RemoveAll();

					while (fgets(tmp,1023,fp))
					{
						kill_tail(tmp);

						if (tmp[0] == ' ' && lfile.GetCount()>0) // description-line
						{
							char * p;

							p = tmp;

							lfile.AddTail(p);
						}


						if (tmp[0] != ' ' && strlen(tmp)>0) // filename-line
						{

							if (lfile.GetCount() >0) // check wether oldfile matches the searchpattern
							{
								int i;
								char tmp[1024];

								for (i=0;i<lfile.GetCount();i++)
								{
									strcpy(tmp,PS lfile.GetString(i));
									strupr(tmp);

									if (strstr(tmp,pattern)!=0)
									{

									 if (!display_as_file(Com,lfile,path,&mcount,&count,taglist))
										 goto endofsearch;

									 if (mcount > curuser.lps)
										 mcount=0;

									 break;
									}


								}
							}

							lfile.RemoveAll(); // prepare for new file

							lfile.AddTail(tmp);
						}

					}

					fclose(fp);
				}
			}

		}

	}

endofsearch:

	lfile.RemoveAll();

	mcount=-2;


	get_string("NOMOREFOUND",tmp);
	write_bbs(Com,tmp,strlen(tmp),1);
	black_return(Com,1);

	display_as_file(Com,lfile,path,&mcount,&count,taglist); // last question


	return;

}


//==========================================================
void enter_mail(int Com, char * name)
//==========================================================
{
char	subject[300];
CString dest;
char	tmp[1024];
char	thismail[MAX_MSG];
int		found=0,t;
long	hand;
int		ret;
mailheader header;
CStrList users;

	get_string("YESCHAR",yeschar);
	get_string("NOCHAR",nochar);
	get_string("CONTINUE",continuechar);

	dest=name;

	if (strcmpi(dest,"SYSOP")==0)
			get_fullname(dest);

	memset(&bbsinput,0,sizeof(_bbsinput));
	memset(thismail,0,sizeof(thismail));
	black_return(Com,2);
	get_string("MTOWHO",tmp);
	write_bbs(Com,tmp,strlen(tmp),1);

	bbsinput.isname=1;
	get_input(Com,PS dest,35,&bbsinput);
	bbsinput.isname=0;

	black_return(Com,1);

	if (strlen(dest)<1 || exitatonce!=0)
		return;

	users.LoadFromFile("BBSUSER.CFG");
	found=0;

	for (t=0;t<users.GetCount();t++)
	{
		CString help;
		get_token(users.GetString(t),0,help);

		if (help.CompareNoCase(dest)==0)
		{
			dest=help;
			found=1;
			break;
		}
	}

	if (!found)
	{
		get_string("MUSRNF",tmp);
		black_return(Com,2);
		write_bbs(Com,tmp,strlen(tmp),1);
		Sleep(3000);
		return;
	}

	black_return(Com,1);

	get_string("MSUBJECT",tmp);
	strcpy(subject,"");
	write_bbs(Com,tmp,strlen(tmp),1);
	get_input(Com,subject,60,&bbsinput);

	black_return(Com,2);

	get_string("MINFO",tmp);
	write_bbs(Com,tmp,strlen(tmp),1);
	black_return(Com,1);

	set_color(Com,FGCYAN,HIGH);
	for (t=0;t<79;t++)
		write_bbs(Com,"-",1,1);
	black_return(Com,1);


	t=0;

	while (1)
	{
		char sbuf[300];
		t++;

		if (exitatonce)
			return;

		sprintf(sbuf,"^[FE%-3d^[FA:",t);
		write_bbs(Com,sbuf,strlen(sbuf),1);

		strcpy(tmp,"");
		get_input(Com,tmp,74,&bbsinput);
		black_return(Com,1);

		if (strlen(tmp)<1)
		{
			get_string("MQUES",sbuf);
			write_bbs(Com,sbuf,strlen(sbuf),1);

			while (1)
			{
				char ch;

				ch = get_a_char(Com);
				if (ch!=0)
				{
					if (strchr(yeschar,ch)!=0)
						goto savethismail;

					if (strchr(nochar,ch)!=0)
						return;

					if (strchr(continuechar,ch)!=0 || strchr("\r\n",ch)!=0)
					{
						black_return(Com,0);
						write_bbs(Com,"\r",1,1);

						for (int t=0;t<78;t++)
							write_bbs(Com," ",1,1);

						write_bbs(Com,"\r",1,1);
						strcat(thismail,"\r");
						break;
					}
				}
			}
		}
		else
		{		// append line to mailtext
			strcat(thismail,tmp);
			strcat(thismail,"\r");
		}
	}

savethismail:
	char sbuf[300];

	sprintf(sbuf,langstr("++ wrote mail to %s","++ создано письмо %s"),dest);
	modem_add_listbox(sbuf);
	black_return(Com,2);
	strcpy(sbuf,BBSMAIL);

	ret=db_get_area_handle(sbuf,&hand,1,"0:0/0.0");                             // ADDED

	if (ret != DB_OK)
	{
		sprintf(sbuf,langstr("ERROR writing to messagebase\n\r","Ошибка сохранения письма\r\n"));
		modem_add_listbox(sbuf);
		CharToOem(sbuf,sbuf);
		write_bbs(Com,sbuf,strlen(sbuf),1);
		return;
	}

	memset(&header,0,sizeof(struct mailheader));

	header.mail_text=thismail;
	header.text_length=strlen(thismail)+1; // mit 0
	header.structlen=sizeof(struct mailheader);
	header.status=DB_MAIL_SCANNED;
	header.mailid = time(NULL);

	unix_time_to_fido(header.mailid,header.datetime);

	header.unixtime=parse_time(header.datetime);

	strcpy(header.toname,dest);
	strcpy(header.fromname,curuser.fullname);
	strcpy(header.subject,subject);

	ret=db_append_new_message       (hand,&header);

	db_set_area_to_carboncopy(hand); // make local-area

	// now check wether this mail should be a carbon-copy for the sysop...

	CString check1;
	CString check2;
	
	get_fullname(check1);
	get_fullname(check2,TRUE);

	if (!strcmpi(check1,header.toname) || !strcmpi(check2,header.toname) || t==1)
	{
		char tmpmsg[MAX_MSG];

		strcpy(tmpmsg,langstr(CC_ENG,CC_RUS));
		strcat(tmpmsg,BBSMAIL"\r");
		strcat(tmpmsg,thismail);
		header.mail_text=tmpmsg;
		header.text_length=strlen(tmpmsg)+1; // mit 0

		ret=db_get_area_handle("LOCALMAIL",&hand,0,"0:0/0.0");
		if (ret == DB_OK)
		{
			ret=db_append_new_message(hand,&header);
			sprintf(sbuf,langstr("++ CC to LOCALMAIL","++ Копия в LOCALMAIL"));
			modem_add_listbox(sbuf);
		}
	}
}

//==========================================================
	void display_mail_wrapped(int Com, char ** p)
//==========================================================
{
int		chars=0;
char	*z;
char	*c;

	z=*p;

	while (**p != 0 && chars < 79)
	{

		if (**p=='\r')
			break;

		(*p)++;

		if (**p=='\r')
			break;

	}

	if (chars >= 79)
	{

		while (**p != ' ' && **p != ',' && **p != '.' && *p > z)
			p--;

		if (z==*p)
			*p = z+79;

	}

	**p=0;

	set_color(Com,FGWHITE);

	if (*z==1)
		set_color(Com,FGBLACK,HIGH);

	if (strncmp(z," * Origin",9)==0)
		set_color(Com,FGMAGENTA,HIGH);

	if (strncmp(z,"---",3)==0)
		set_color(Com,FGMAGENTA);


	c=z;
	while ((*c) != 0 && *c != '>')
		c++;

	if ((c-z) < 5 && *c !=0)
		set_color(Com,FGYELLOW,HIGH);

	write_bbs(Com,z,strlen(z),1);
	(*p)++;

}

//==========================================================
void display_mail(int Com, struct mailheader header,long hand)
//==========================================================
{

	char tmp[1024],tmp2[1024],sbuf[300];
	int count,t;
	char ch;
	char *p;

	count=0;

	black_return(Com,0);
	clear_screen(Com,TRUE);

	get_string("MMAILFROM",tmp);
	sprintf(tmp2,tmp,header.fromname,header.datetime);

	write_bbs(Com,tmp2,strlen(tmp2),1);
	black_return(Com,1);

	get_string("MMAILTO",tmp);
	sprintf(tmp2,tmp,header.toname);

	write_bbs(Com,tmp2,strlen(tmp2),1);
	black_return(Com,1);

	get_string("MMAILSUBJ",tmp);
	sprintf(tmp2,tmp,header.subject);

	write_bbs(Com,tmp2,strlen(tmp2),1);
	black_return(Com,1);

	set_color(Com,FGCYAN,HIGH);
	for (t=0;t<79;t++)
		write_bbs(Com,"-",1,1);

	black_return(Com,1);
	count+=4;
	p = header.mail_text;
	while (1)
	{
		if (exitatonce)
			return;

		count++;
		if (count >= curuser.lps)
		{
			get_string("MCONTINUE",sbuf);
			write_bbs(Com,sbuf,strlen(sbuf),1);
			count=0;
			while (1)
			{
				ch = get_a_char(Com);

				if (exitatonce)
					return;

				if (ch != 0)
					break;
			}

			black_return(Com,0);
			write_bbs(Com,"\r",1,1);
			clear_screen(Com,FALSE);

		}

		display_mail_wrapped(Com,&p);
		black_return(Com,1);
		if (*p == 0)
			break;
	}

	set_color(Com,FGGREEN);
//	set_color(Com,FGCYAN,HIGH);
	for (t=0;t<79;t++)
		write_bbs(Com,"-",1,1);
	get_string("MDELETE",sbuf);
	write_bbs(Com,sbuf,strlen(sbuf),1);

	if (ask_yes_no(Com))
	{
		header.status |= DB_DELETED;
		if (db_sethdr_by_index (hand,header.index,&header,1) != DB_OK)
		{
			strcpy(sbuf,"BBS Error deleting mail");
			modem_add_listbox(sbuf);

			strcpy(sbuf,"^[FE\r\n\nError deleting mail");
			write_bbs(Com,sbuf,strlen(sbuf),1);
			Sleep(2000);
		}
	}

	black_return(Com,1);
}

//==========================================================
void check_for_mail(int Com)
//==========================================================
{

int ret;
struct mailheader header;
long hand;
int foundone=0;
char tmp[MAX_MSG];
int k;

	ret=db_get_area_handle(BBSMAIL,&hand,0);
	if (ret == DB_OK)
	{


		k = db_getfirsthdr(hand,&header);

		while (k==DB_OK)
		{

			if (strcmpi(header.toname,curuser.fullname)==0 &&
				!(header.status & DB_DELETED))
			{

				if (!foundone)
				{
					black_return(Com,1);
					get_string("MNEWMAIL",tmp);
					write_bbs(Com,tmp,strlen(tmp),1);

					if (!ask_yes_no(Com))
						return;

					foundone=1;
				}

				// now display the mails

				ret=db_get_mailtext(hand,&header,tmp,sizeof(tmp));

				header.mail_text=tmp;

				if (ret == DB_OK)
					display_mail(Com,header,hand);
				else
				{
					sprintf(tmp,"\n\n\r^[FEError retrieving mail from messagebase !\n");
					write_bbs(Com,tmp,strlen(tmp),1);
					Sleep(2000);
					black_return(Com,2);
				}

			}

			if (exitatonce)
				return;

			k = db_getnexthdr(hand,&header);
		}



	}
}

//==========================================================
	int display_menu(int Com,char * fname, CStrList & commands, char * commandlist)
//==========================================================
{
FILE *fp;
char line[1024];
char tmp[300];
char *p;

	commands.RemoveAll();
	memset(&bbsinput,0,sizeof(struct _bbsinput)); // default-edit settings

	fp = fopen(fname,"rt");
	if (!fp)
		return 0;

	while (fgets(line,999,fp))
		handle_menu_line(Com,line,commands,commandlist);

	fclose(fp);

	// now handle the globals menu

	// get globals.mnu name
	strcpy(tmp,fname);
	p = tmp+strlen(tmp);
	while (p>tmp && *p != '\\')
		p--;
	p++;
	*p=0;
	strcat(tmp,"GLOBAL.MNU");

	fp = fopen(tmp,"rt");
	if (fp)
	{
		while (fgets(line,999,fp))
			handle_menu_line(Com,line,commands,commandlist);

		fclose(fp);
	}

	get_string("PROMPT",line);
	black_return(Com,1);
	write_bbs(Com,line,strlen(line),1);
	update_console();

	return 1;
}

//==========================================================
	int goto_main_bbs(int Com)
//==========================================================
{
CStrList commands;
CStrList taglist;
CString help;
char	buf[1024];
char	currentmenu[1024];
char	commandlist[1024];
int		exitbbs=0;
int		found;

	SET_SCHEME1;
	write_bbs(Com,"\r\n\r\n",4,1);
	get_string("MENUDIRECTORY",currentmenu);
	addbackslash(currentmenu);
	strcat(currentmenu,"MAIN.MNU"); // first menu in system

menuagain:

	exitbbs=0;
	strcpy(commandlist,"");
	display_menu(Com,currentmenu, commands, commandlist);

	while (!exitbbs)
	{
		char ch;
		Sleep(25);
		do {
			ch = get_a_char(Com);
			if (sysopchat)
			{
				sysop_chat(Com,0);
				clear_screen(Com,TRUE);
				goto menuagain;
			}

			if (exitatonce)
				return 1;

		} while (sysopchat);

		if (ch >= 'a' && ch <= 'z')
			ch = _toupper(ch);

		found = -1;

		if (ch)
		{
			if (strchr(commandlist,ch) !=0) // gueltiges commando
			{
				int t;

				for (t=0;t<commands.GetCount();t++)
				{
					get_token(commands.GetString(t),0,help);
					if (strchr(help,ch))
					{
						found = t;
						help = commands.GetString(t);
						break;
					}
				}
			}

			if (found != -1)
			{
				CString help2;

				get_token(help,1,help2);
				if (!strcmpi("GOTOMENU",help2))		// GOTOMENU
				{											// ########
					char oldmenu[300];

					strcpy(oldmenu,currentmenu);
					get_token(help,2,help2);
					get_string("MENUDIRECTORY",currentmenu);

					addbackslash(currentmenu);
					strcat(currentmenu,help2);
					strcat(currentmenu,".MNU");

					if (existfile(currentmenu))
					{
						strcpy(commandlist,"");
						display_menu(Com,currentmenu, commands, commandlist);
					}
					else
					{

						sprintf(buf,"\n\n\r^[FC>> %s not found !^[FF\n\r",currentmenu);
						strcpy(currentmenu,oldmenu);
						write_bbs(Com,buf,strlen(buf),1);

						Sleep(2500);

						strcpy(commandlist,"");
						display_menu(Com,currentmenu, commands, commandlist);

					}
				}


				if (!strcmpi("EXITBBS",help2))		// EXITBBS
				{										// ########
					get_string("LOGOFF",buf);
					SET_SCHEME1;
					display_file(Com,"LOGOFF");

//					if (Com != LOCALCOM)
						Sleep(2000);

					exitbbs = 1;
					return 1; // exit bbs
				}

				if (!strcmpi("CHANGELANGUAGE",help2))	// Change Language
				{											// ###############
					char tmp[300];
					char *p;

					change_language(Com);
					p = currentmenu+strlen(currentmenu); // get new menu for new language
					while (p > currentmenu && *p != '\\')
						p--;

					p++;

					get_string("MENUDIRECTORY",tmp);
					addbackslash(tmp);
					strcat(tmp,p);
					strcpy(currentmenu,tmp);

					if (!existfile(currentmenu)) // if a menu with the same name does not exist
					{
						get_string("MENUDIRECTORY",currentmenu);
						addbackslash(currentmenu);
						strcat(currentmenu,"MAIN.MNU"); // go back to main-menu
					}

					strcpy(commandlist,"");
					display_menu(Com,currentmenu, commands, commandlist);
				}

				if (!strcmpi("CHANGEADRESS",help2))	// Change Post-Adress
				{											// ##################
					change_mailing_adress(Com);
					strcpy(commandlist,"");
					display_menu(Com,currentmenu, commands, commandlist);
				}

				if (!strcmpi("CHANGETELEPHONE",help2))	// Change Telephone
				{											// ###############
					change_telephone(Com);
					strcpy(commandlist,"");
					display_menu(Com,currentmenu, commands, commandlist);
				}

				if (!strcmpi("CHANGEANSI",help2))		// Change ANSI
				{
					change_ansi(Com);
					strcpy(commandlist,"");
					display_menu(Com,currentmenu, commands, commandlist);
				}

				if (!strcmpi("CHANGELPS",help2))		// Change LPS
				{
					change_lps(Com);
					strcpy(commandlist,"");
					display_menu(Com,currentmenu, commands, commandlist);
				}

				if (!strcmpi("CHANGEFILEAREA",help2))		// Change File-area
				{
					change_file_area(Com);
					strcpy(commandlist,"");
					display_menu(Com,currentmenu, commands, commandlist);
				}

				if (!strcmpi("CHANGEPWD",help2))	// Change Password
				{
					char pwd1[300],pwd2[300];

					change_pwd(Com,pwd1,pwd2);
					if (strcmpi(pwd1,pwd2) ==0)
					{
						black_return(Com,2);
						curuser.pwdcrc32=0;
						RS_GetCRC_CCITT32(pwd1,strlen(pwd1),&curuser.pwdcrc32); // save pwd
						get_string("CHNGPWDSUCC",buf);
						write_bbs(Com,buf,strlen(buf),1);
						Sleep(2000);
					}
					else
					{
						black_return(Com,2);
						get_string("CHNGPWDFAIL",buf); // pwd failed
						write_bbs(Com,buf,strlen(buf),1);
						Sleep(2000);
					}

					strcpy(commandlist,"");
					display_menu(Com,currentmenu, commands, commandlist);
				}

				if (!strcmpi("DISPLAYAREA",help2))		// list files.bbs
				{
					CString help1;
					CString help3;

					get_token(curfarea,4,help1);
					get_token(help,2,help3);

					if (!strcmpi(help3,"AUTO")) // check whether new files should be added
					{
						char path[300];

						strcpy(path,help1);

						while (strlen(path)>0)
						{
							if (path[strlen(path)-1] != '\\')
								path[strlen(path)-1] =0;
							else
								break;
						}

						if (strlen(path)>0)
							add_missing_files(PS help1, path);
					}

					list_files_bbs(Com,PS help1, taglist);

					strcpy(commandlist,"");
					display_menu(Com,currentmenu, commands, commandlist);
				}


				if (!strcmpi("SEARCHFILE",help2))		// searches for a file
				{
					search_for_files(Com,taglist);
					strcpy(commandlist,"");
					display_menu(Com,currentmenu, commands, commandlist);
				}

				if (!strcmpi("DOWNLOAD",help2))		// download files
				{
					download_files(Com,taglist);
					strcpy(commandlist,"");
					display_menu(Com,currentmenu, commands, commandlist);
				}

				if (!strcmpi("CHECKFORMAIL",help2))	// checks users mailbox
				{
					check_for_mail(Com);
					strcpy(commandlist,"");
					display_menu(Com,currentmenu, commands, commandlist);
				}

				if (!strcmpi("UPLOAD",help2))		// upload a file
				{
					char tmp[1024];

					get_token(help,2,help2);
					strcpy(tmp,help2);
					upload_file(Com,tmp);
					strcpy(commandlist,"");
					display_menu(Com,currentmenu, commands, commandlist);
				}

				if (!strcmpi("ENTERMAIL",help2))		// write a mail
				{
					char tmp[1024];
					CString help3,help4,help5;
					long ahandle=999;
					int rethans;

					help2.Empty();
					help3.Empty();
					help4.Empty();
					help5.Empty();

					get_token(help,2,help2);
					get_token(help,3,help3);
					get_token(help,4,help4);
					get_token(help,5,help5);

					sprintf(tmp,"%s %s %s %s",help2,help3,help4,help5);
					kill_spaces_hinten(tmp);
					enter_mail(Com,tmp);

					rethans=db_get_area_handle(BBSMAIL,&ahandle,0,0);
					if (rethans==DB_OK)
					{
						db_refresh_area_info(ahandle);		// Area Statistik updaten
						if (gustat.cur_area_handle==ahandle)
							gpMain->OnUpdateScreenDisplay();		// Screen update erzwingen
					}
					strcpy(commandlist,"");
					display_menu(Com,currentmenu, commands, commandlist);
				}

				if (!strcmpi("EDITMARKED",help2))		// edit marked files
				{
					edit_marked_files(Com, taglist);
					strcpy(commandlist,"");
					display_menu(Com,currentmenu, commands, commandlist);
				}

				if (!strcmpi("YELLSYSOP",help2))		// chat with sysop
				{
					sysop_chat(Com,1);
					strcpy(commandlist,"");
					display_menu(Com,currentmenu, commands, commandlist);
				}

				if (!strcmpi("DISPLAYFILEWAIT",help2))		// displayfile with wait at end
				{
					get_token(help,2,help2);
					display_file(Com,PS help2);
					do {

						Sleep(100);

					} while (get_a_char(Com) ==0 );

					strcpy(commandlist,"");
					display_menu(Com,currentmenu, commands, commandlist);
					help2.Empty();
				}

				if (!strcmpi("DISPLAYFILE",help2))			// displayfile without wait !
				{
					get_token(help,2,help2);
					display_file(Com,PS help2);
					strcpy(commandlist,"");
					display_menu(Com,currentmenu, commands, commandlist);
					help2.Empty();
				}
			}
		}
	}
	return 1;
}

//==========================================================
	void bbsuser_struct2string(char *tmp)
//==========================================================
{
	_strdate(curuser.lastlogin); // get todays date for last login
	sprintf(tmp,"%s\t%s\t%d\t%s\t%s\t%s\t%s\t%s\t%d\t%d\t%08X\t%d\t%s",
		curuser.fullname,curuser.language,curuser.sec,curuser.groups,curuser.street,
		curuser.town,curuser.tel,curuser.modem,curuser.lps,curuser.ansi,curuser.pwdcrc32,
		curuser.tleft,curuser.lastlogin);
}

//==========================================================
	int save_current_user(void)
//==========================================================
// searches the user in curuser structure in bbsuser strnglist
// and writes the bbsuser.cfg back to disk
//
{
CStrList bbsuser;
CStrList newbbsuser;
CString help;
char tmp[8192];

	bbsuser.LoadFromFile("bbsuser.cfg");
	if (bbsuser.GetCount() < 1)
		return 0;

	while (1)
	{
		get_token(bbsuser.GetString(0),0,help);

		if (!strcmpi(help,curuser.fullname))
		{

			bbsuser_struct2string(tmp);
			newbbsuser.AddTail(tmp);
			bbsuser.RemoveHead();
		}
		else
		{
			newbbsuser.AddTail(bbsuser.GetString(0));
			bbsuser.RemoveHead();
		}

		if (bbsuser.GetCount() < 1)
			break;
	}
	newbbsuser.SaveToFile("bbsuser.cfg");
	return 1;
}

//==========================================================
	int run_bbs(unsigned char Com)
//==========================================================
{


	char tmp[300];
	int  port;

	port = (int)Com;

	if (!init_bbs_stuff())
	{
		modem_add_listbox("Cannot allocate console window for BBS !");
		release_bbs_stuff();
		return 0;
	}

	write_bbs_setup();
	useansi = 1;
	exitatonce=0;
	lastmin=0;
	timeoutwarning=0;
	lasttime=0;
	init_write_bbs=0;

	ANSIData.ANSIProc=(PANSIPROC)ANSIProc;

	strcpy(consoletitle,"BBS Startup");
	SetConsoleTitle(consoletitle);

	taglist.RemoveAll();
	memset(&curuser,0,sizeof(struct _currentuser));
	curuser.tleft=300; // 5 mins logon-time

	Sleep(1000);

	if (bbs_startup(port))
	{
		farea.LoadFromFile("bbsfare.cfg");
		if (farea.GetCount() < 1)
			curfarea="@\t[Not available]\t0\t*";
		else
		{
			int t=0;

			do {

				curfarea=farea.GetString(t);
				t++;

			} while (check_farea(curfarea) == 0 && t < farea.GetCount());

			if (t>farea.GetCount())
				curfarea="@\t[Not available]\t0\t*";
		}

		update_console();

		sprintf(tmp,
			langstr("++ user %s (Sec %d, Grp. %s)","++ пользователь %s (уров. %d, гр. %s)"),
			curuser.fullname,curuser.sec,curuser.groups);
		modem_add_listbox(tmp);
		sprintf(tmp,langstr("++ city %s (Tel. %s)","++ город %s (тел. %s)"),
			curuser.town,curuser.tel);
		modem_add_listbox(tmp);

	    sprintf(sessiondata.system,"BBS-LOGIN %s (%s)",curuser.fullname,curuser.tel);
		strcpy(sessiondata.user,curuser.town);
		check_for_mail(Com);
		goto_main_bbs(port);
		save_current_user();
	}
	release_bbs_stuff();
	return 1;
}
