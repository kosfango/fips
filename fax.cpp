#include "stdafx.h"
#include <io.h>
#include "lightdlg.h"
#include "detmail.h"
#include "supercom.h"
#include "faxinc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

CString gFaxInboundDir;

int		ComFax=-1;

extern  detmail gMailer;
extern	CLightDlg *gpMain;
extern _gconfig gc;

CStrList gReceivedFaxList;	// 0= remote id
extern  CStrList faxcfg;

int convert_fax(LPCSTR filename,CString &created);

/*--------------------------------------------------------------------------*/
/* Local routines                                                           */
/*--------------------------------------------------------------------------*/

static int		GetFaxFile				(int);
static int		ReadG3StreamData		(FILE *, int);
static void		get_faxline				(char *, int, unsigned int);
void			init_swaptable			(void);
static void		init_modem_response		(void);
static void		get_modem_result_code	(void);
static void		parse_text_response		(char *);
static int		faxmodem_receive_page	(int);
static void		update_bytes_transferred(int);

// FIPS Interface Functions ...
static int		do_ready				(char *text);
static int		elapse_time				(void);
static int		dexists					(char *filename);
static char		status_line				[300];
static int		CLEAR_INBOUND			(void);
static int		is_carrier				(void);
static void		time_release			(void);
static int		SENDBYTE				(unsigned char c);
static int		CHAR_AVAIL				(void);
static unsigned short MODEM_IN			(void);
static unsigned short TIMED_READ		(int timeout);
static int		SENDCHARS				(char *text,int size,int dcdcheck);
static void		set_status_line			(char *text);
static void		set_filename			(char *text);
static int		generate_fax_info_mail	(void);

/*--------------------------------------------------------------------------*/
/* Private data                                                             */
/*--------------------------------------------------------------------------*/

static int				gEnd_of_document;
unsigned char			swaptable[256];
static int				swaptableinit = FALSE;
static struct			faxmodem_response response;
static unsigned long	faxsize = 0L;
static int				isswaptableinit=0;
/*--------------------------------------------------------------------------*/
/* FAX RECEIVE Routines                                                     */
/*--------------------------------------------------------------------------*/

/* receive fax files into basefilename */

// ============================================================================
	int faxreceive (int Com)
// ============================================================================
{
	char buf[300];
	char tmp[300];
	char tmp1[300];
	int result;
    int page;
	time_t ltime;
	struct tm *today;


	ComFax=Com;
	gFaxInboundDir=faxcfg.GetString(3);

    time( &ltime );
	today = localtime( &ltime );
    strftime(tmp,299,"%d_%b_%y\\%H_%M_%S",today);
	sprintf(tmp1,"%s",gFaxInboundDir);
	addbackslash(tmp1);
	strcat(tmp1,tmp);
	gFaxInboundDir=tmp1;
	gFaxInboundDir+="\\";
	create_path(gFaxInboundDir);

	if (!isswaptableinit)
		init_swaptable ();

	do_ready ("Receiving incomming FAX:");
	set_status_line("Receiving FAX");

	(void) init_modem_response ();
	gEnd_of_document = FALSE;
	response.fcon = TRUE;				/* we already connected */

	result = 0;
	for (page = 0;!gEnd_of_document; page++)
	{
		result = GetFaxFile (page);
		switch ((int) result)
		{
			case PAGE_GOOD:
							sprintf(status_line,"FAX: Page received successfully");	modem_add_listbox(status_line);
							continue;

			case PAGE_HANGUP:
							sprintf(status_line,"FAX: Received total %d pages", page);	modem_add_listbox(status_line);
							gReceivedFaxList.defaultindex=page;
							result = 1;
							gEnd_of_document = TRUE;
							sprintf(buf,"FAX: Received %d pages",page); set_status_line(buf);
							generate_fax_info_mail();
							break;

			default:
							sprintf(status_line,"FAX: Error during transmission <%d>",result);	modem_add_listbox(status_line);
							result = page;
							gEnd_of_document = TRUE;
							break;
		}
	}

	// Reset Display ..
	set_filename("");
	update_bytes_transferred (0);
	return result;
}

/* This executes the +FDR receive page command, and looks for
 * the proper CONNECT response, or, if the document is finished,
 * looks for the FHNG/FHS code.
 *
 * returns:
 *  PAGE_GOOD                no error conditions occured during reception
 *  PAGE_HANGUP              normal end of transmission
 *  PAGE_ERROR               something's wrong
 */

// ============================================================================
	static int  GetFaxFile (int page)
// ============================================================================
{
	char buf[256], j[100];
	int result;
	FILE *fp = NULL;
	int opage = page;
	CString		help;

	sprintf(status_line,"FAX: Start receiving page ...");	modem_add_listbox(status_line);

	// Name fuer die Datei ...
	do
	{
			sprintf (buf, "%sPAGE%04x.FAX",gFaxInboundDir, opage++);
	}
	while (dexists (buf) && (opage < 0xFFFF));

	if (opage == 0xFFFF)
	{
		sprintf(status_line,"FAX: ERROR: Couldn't create output file");	modem_add_listbox(status_line);
		return (PAGE_ERROR);
	}

	if ((result = faxmodem_receive_page (page)) == 0)
	{
		/* filename to create for this page of document */
		if ((fp = fopen (buf,"wb")) == NULL)
		{
			sprintf(status_line,"FAX: ERROR Couldn't create output file <%s>", buf); modem_add_listbox(status_line);
			return (PAGE_ERROR);
		}

		if (!page)
		{
			sprintf(status_line,"FAX: Connected with %s", response.remote_id);	modem_add_listbox(status_line);
			gReceivedFaxList.RemoveAll();
			gReceivedFaxList.AddTail(response.remote_id);
		}

		help.Format("%s\t%d",buf,page);
		gReceivedFaxList.AddTail(PS help);

		set_filename(buf);

		(void) sprintf (j, "Receiving page %02x as file <%s>",page,buf);

		// if (un_attended && fullscreen)
		// {
			// clear_filetransfer ();
			// sb_move (filewin, 1, 2);
			// sb_puts (filewin, j);
			// (void) sprintf (j, " vr%d br%d wd%d ln%d df%d ec%d bf%d st%d",
			// 	response.T30.vr, response.T30.br, response.T30.wd,
			// 	response.T30.ln, response.T30.df, response.T30.ec,
			// 	response.T30.bf, response.T30.st);
			// sb_move (filewin, 2, 40);
			// sb_puts (filewin, j);

			sprintf(status_line,j);	modem_add_listbox(status_line);
		//	elapse_time ();
		// }
		// else
		// {
			// set_xy (j);
			// set_xy (NULL);
			// locate_x += 2;
		// }
		result = ReadG3StreamData (fp, page);
	}

	if (fp)
	{
		fclose (fp);
		if (faxsize <= 256L)
			unlink (buf);
		else
		{
			sprintf(status_line," FAX File received <%s> (%lub)", buf, faxsize);	modem_add_listbox(status_line);
		}
	}

	return (result);
}

/* Reads a data stream from the faxmodem, unstuffing DLE characters.
 * Returns the +FET value (2 if no more pages) or 4 if hangup.
 */

// ============================================================================
	static int  ReadG3StreamData (FILE * fp, int page)
// ============================================================================
{
	register short c;
	unsigned char *secbuf, *p;
    long ltimer = 0L;						/* MB 94-01-01 */
    int pseudo_carrier;						/* MB 94-01-01 */

	// sprintf(status_line,">FAX [ReadG3StreamData]");	modem_add_listbox(status_line);

	// happy_compiler = page;						/* Make compiler happy      */
	response.post_page_response_code = -1;		/* reset page codes         */
	response.post_page_message_code = -1;

	CLEAR_INBOUND ();						/* flush echoes or return codes */

	if ((secbuf = (unsigned char *) calloc (1, 1024)) == NULL)
		goto fax_error;

	p = secbuf;

	update_bytes_transferred (faxsize);

	pseudo_carrier = !is_carrier();			/* test if modem sets DCD */
	if (pseudo_carrier)
	{
		sprintf(status_line,"FAX: Modem doesn't set DCD");	modem_add_listbox(status_line);
	}

	// sprintf(status_line,"FAX: DC2  [ReadG3StreamData]");	modem_add_listbox(status_line);
	/* Send DC2 to start phase C data stream */

	SENDBYTE ((unsigned char) DC2);

	while (1)								/* data only when carrier high */
	{
		if (!CHAR_AVAIL ())			/* if nothing ready,*/
		{
			if (pseudo_carrier)		/* MB 94-01-01 */
			{						/* process timeout if modem does not   */
									/* set DCD, this is only a kludge, but */
									/* it could prevent an endless loop    */
				if (!ltimer)
					ltimer=GetTickCount();
				else
					if ((GetTickCount()-ltimer)>15000)
						goto fax_error;	/* Houston, we lost the downlink   */
			}
			time_release ();
			continue;				/* process timeouts */
		}
		else
			ltimer = 0L;			/* reset no char waiting timer */

		c = MODEM_IN () & 0xff;		/* get a character  */

		if (c == DLE)				/* DLE handling     */
		{
			long ltimer2 = 0L;

			while (!CHAR_AVAIL ())
			{
				if (!ltimer2)
					ltimer2 = GetTickCount();
				else
					if ((GetTickCount()-ltimer2)>5000)
				{
					faxsize = 0L;
					goto fax_error;		/* give up */
				}
			}

			c = TIMED_READ (0);

			if (c == ETX)		/* end of stream */
				goto end_page;

			/* DLE DLE gives DLE. We don't know what to do if it
			   isn't ETX (above) or DLE. So we'll just always treat
			   DLE (not ETX) as (not ETX).

			   Fall out of here into storage. */
		}

		*p++ = swaptable[(unsigned char) c];
		faxsize++;

		if (!(faxsize % 1024))
		{
			update_bytes_transferred (faxsize);
			if (fwrite (secbuf, 1, 1024, fp) != 1024)
			{
				goto fax_error;	/* hoppala */
			}
			p = secbuf;
			time_release ();
		}
	}

end_page:

	if (faxsize % 1024)
	{
		if (fwrite (secbuf, 1, (size_t) (faxsize % 1024), fp) != (size_t) (faxsize % 1024))
			goto fax_error;		/* hoppala */
		update_bytes_transferred (faxsize);
	}

	free (secbuf);

	sprintf(status_line,"FAX: Waiting for +FET/+FHNG");	modem_add_listbox(status_line);

	c = 0;
	while (response.post_page_message_code == -1)	/* wait for +FET */
	{
		(void) get_modem_result_code ();
		c++;
		if ((!response.post_page_response_code) || (c > 5) || (response.error))
			return (PAGE_ERROR);
		if (response.hangup_code != -1)
			return (PAGE_HANGUP);
	}
	return (PAGE_GOOD);

fax_error:

	if (secbuf != NULL)
		free (secbuf);

	sprintf(status_line,"!FAX Error receiving page");	modem_add_listbox(status_line);
	(void) get_modem_result_code ();
	return (PAGE_ERROR);
}

/*--------------------------------------------------------------------------*/
/* Class 2 Faxmodem Protocol Functions                                      */
/*                                                                          */
/* Taken from EIA Standards Proposal No. 2388: Proposed New Standard        */
/* "Asynchronous Facsimile DCE Control Standard" (if approved,              */
/* to be published as EIA/TIA-592)                                          */
/*--------------------------------------------------------------------------*/

/* reads a line of characters, terminated by a newline */

// ============================================================================
	static void  get_faxline (char *p, int nbytes, unsigned int wtime)
// ============================================================================
{
	short c;					/* current modem character   */
	int count = 1;				/* character count (+null)   */
	long t;
	char *resp;

	t = GetTickCount();

	resp = p;

	while ((count < nbytes)		/* until we have n bytes,    */
		&& (((GetTickCount()-t)<(wtime*10))))		/* or out of time            */
	{
		if (!CHAR_AVAIL ())		/* if nothing ready yet,     */
		{
			time_release ();
			continue;			/* just process timeouts     */
		}
		c = MODEM_IN () & 0xff;	/* get a character           */
		if (c == '\n')
			continue;
		if (c == '\r')
			if (count > 1)
				break;			/* get out                   */
			else
				continue;		/* otherwise just keep going */
		*p++ = (char) c;		/* store the character       */
		++count;				/* increment the counter     */
	}

	*p = '\0';					/* terminate the new string  */
//    modem_add_listbox(resp);

//	if ((count > 1) && strnicmp (resp, "AT", 2))
//	{
//		sprintf(status_line,"FAX: Response [%s]", resp);	modem_add_listbox(status_line);
//	}
}

// ============================================================================
	void	init_swaptable (void)
// ============================================================================
{
	int i, j;

	for (i = 0; i < 256; i++)
	{
		/* swap the low order 4 bits with the high order */

		j = (((i & 0x01) << 7) |
			((i & 0x02) << 5) |
			((i & 0x04) << 3) |
			((i & 0x08) << 1) |
			((i & 0x10) >> 1) |
			((i & 0x20) >> 3) |
			((i & 0x40) >> 5) |
			((i & 0x80) >> 7));
		swaptable[i] = (unsigned char) j;
	}
	isswaptableinit = TRUE;
}

/****************************************************************
 * Initialize a faxmodem_response struct
 */

// ============================================================================
	static void  init_modem_response (void)
// ============================================================================
{
	response.remote_id[0]				= '\0';
	response.fcon						= FALSE;
	response.connect					= FALSE;
	response.ok							= FALSE;
	response.error						= FALSE;
	response.hangup_code				= -1;
	response.post_page_response_code	= -1;
	response.post_page_message_code		= -1;
	response.T30.ec = response.T30.bf	= 0;
}

/* This function parses numeric responses from the faxmodem.
 * It fills in any relevant slots of the faxmodem_response structure.
 */

// ============================================================================
	static void  get_modem_result_code (void)
// ============================================================================
{
	char buf[256];
	long t;

	// sprintf(status_line,">FAX [get_modem_result_code]");	modem_add_listbox(status_line);
	t = GetTickCount();

	while ((GetTickCount()-t)<4000)
	{
		buf[0] = '\0';
		(void) get_faxline (buf, 255, 100);
		if (buf[0])
		{
			(void) parse_text_response (buf);
			return;
		}
	}
	return;
}


// ============================================================================
	static void  parse_text_response (char *str)
// ============================================================================
{
	/* Look for +FCON, +FDCS, +FDIS, +FHNG, +FHS, +FPTS, +FK, +FTSI */

	if (!strnicmp ("+FCO", str, 4))
	{
		response.fcon = TRUE;
		modem_add_listbox ("FAX: REMOTE: +FCO");
		return;
	}

	if (!strnicmp (str, "OK", 2))
	{
		response.ok = TRUE;
		return;
	}

	if (!strnicmp (str, "CONNECT", 7))
	{
		response.connect = TRUE;
		return;
	}

	if (!strnicmp (str, "NO CARRIER", 10) || !strnicmp (str, "ERROR", 5))
	{
		response.error = TRUE;
		response.hangup_code = 0;
		return;
	}

	if (!strnicmp (str, "+FDCS", 5))
	{
		sscanf (str + 6, "%d,%d,%d,%d,%d,%d,%d,%d",
			&response.T30.vr, &response.T30.br, &response.T30.wd,
			&response.T30.ln, &response.T30.df, &response.T30.ec,
			&response.T30.bf, &response.T30.st);
		modem_add_listbox ("FAX: REMOTE: +FDCS");
		return;
	}

	if (!strnicmp (str, "+FHNG", 5))
	{
		sscanf (str + 6, "%d", &response.hangup_code);
		modem_add_listbox ("FAX: REMOTE: +FHNG");
		return;
	}

	if (!strnicmp (str, "+FPTS", 5))
	{
		sscanf (str + 6, "%d", &response.post_page_response_code);
		modem_add_listbox ("FAX: REMOTE: +FPTS");
		return;
	}

	if (!strnicmp (str, "+FTSI", 5))
	{
		(void) strcpy (response.remote_id, str + 6);
		modem_add_listbox ("FAX: REMOTE: +FTSI");
		return;
	}

	if (!strnicmp (str, "+FET", 4))
	{
		sscanf (str + 5, "%d", &response.post_page_message_code);
		modem_add_listbox ("FAX: REMOTE: +FET");
		return;
	}


	if (!strnicmp (str, "+FHS", 4))	/* Class 2.0 */
	{
		sscanf (str + 5, "%d", &response.hangup_code);
		modem_add_listbox ("FAX: REMOTE: +FHS");
		return;
	}

	if (!strnicmp (str, "+FCS", 4))	/* Class 2.0 */
	{
		sscanf (str + 5, "%d,%d,%d,%d,%d,%d,%d,%d",
			&response.T30.vr, &response.T30.br, &response.T30.wd,
			&response.T30.ln, &response.T30.df, &response.T30.ec,
			&response.T30.bf, &response.T30.st);
		modem_add_listbox ("FAX: REMOTE: +FCS");
		return;
	}

	if (!strnicmp (str, "+FPS", 4))	/* Class 2.0 */
	{
		sscanf (str + 5, "%d", &response.post_page_response_code);
		modem_add_listbox ("FAX: REMOTE: +FPS");
		return;
	}

	if (!strnicmp (str, "+FTI", 4))	/* Class 2.0 */
	{
		(void) strcpy (response.remote_id, str + 5);
		modem_add_listbox ("FAX: REMOTE: +FTI");
		return;
	}

}

/****************************************************************
 * Action Commands
 */

/* Receive a page
 * after receiving OK,
 * send +FDR
 * This is somewhat ugly, because the FDR command can return
 * a couple of possible results;
 * If the connection is valid, it returns something like
 *  +FCFR
 *  +FDCS: <params>
 *  CONNECT
 *
 * If, on the other hand, the other machine has hung up, it returns
 * +FHNG: <code>  or
 * +FHS: <code>
 *
 * and if the connection was never made at all, it returns ERROR (actually numeric
 * code 4)
 *
 * faxmodem_receive_page returns values:
 * PAGE_GOOD     page reception OK, data coming
 * PAGE_HANGUP   normal hangup
 * PAGE_ERROR    page error
 */

// ============================================================================
	static int  faxmodem_receive_page (int page)
// ============================================================================
{
	long t;
	char buf[100];

	faxsize = 0L;
	response.connect = response.ok = FALSE;

  /* We wait until a string "OK" is seen
   * or a "+FHNG"
   * or a "ERROR" or "NO CARRIER"
   * or until 10 seconds for a response.
   */

	t = GetTickCount();

	sprintf(status_line,"FAX: Waiting for OK ...");	modem_add_listbox(status_line);

	while (((GetTickCount()-t)<10000) && (!response.ok))
	{
		(void) get_faxline (buf, 100, 100);

		sprintf(status_line,"FAX: Response %s", buf);	modem_add_listbox(status_line);
		(void) parse_text_response (buf);

		if (response.hangup_code != -1)
			return (PAGE_HANGUP);

		if (response.error)
			return (PAGE_ERROR);
	}

	if (!response.ok)
		return (PAGE_ERROR);

	SENDCHARS ("AT+FDR\r", 7, 1);

	sprintf(status_line,"FAX: AT+FDR");	modem_add_listbox(status_line);

	/* We wait until either a string "CONNECT" is seen
    * or a "+FHNG"
    * or until 10 seconds for a response.
    */

	t = GetTickCount();

	sprintf(status_line,"FAX: Waiting for CONNECT");	modem_add_listbox(status_line);

	while ( (GetTickCount()-t) < 10000 )
	{
		(void) get_faxline (buf, 100, 100);

		sprintf(status_line,"FAX: Response %s", buf);	modem_add_listbox(status_line);
		(void) parse_text_response (buf);

		if (response.connect == TRUE)
			return (PAGE_GOOD);

		if (response.hangup_code != -1)
			return (PAGE_HANGUP);

		if (response.error)
			return (PAGE_ERROR);
	}

	return (PAGE_ERROR);
}

// +++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++
//		FIPS INTERFACE FUNCTIONS
// +++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++

// ========================================
	int do_ready (char *text)
// ========================================
{
	modem_add_listbox(text);
	return 1;
}

// ============================================================================
	int	elapse_time (void)
// ============================================================================
{

	return 1;
}

// ============================================================================
	static int dexists	(char *filename)
// ============================================================================
// check existance of file
{
	return access(filename,0)==0 ? 1 : 0;
}

// ============================================================================
	static void  update_bytes_transferred (int btrans)
// ============================================================================
{
	gc.mailer.m_transferred=btrans;
	gMailer.OnUpdateValues();
}

// ============================================================================
	static void  set_status_line (char *text)
// ============================================================================
{
	strcpy(gc.mailer.m_status,text);
	gMailer.OnUpdateValues();
}

// ============================================================================
	static void  set_filename (char *text)
// ============================================================================
{
	strcpy(gc.mailer.m_filename,text);
	gMailer.OnUpdateValues();
}

// ============================================================================
	static int	CLEAR_INBOUND (void)		/* flush echoes or return codes */
// ============================================================================
{
	ComBufClear(ComFax,DIR_INC);
	ComBufClear(ComFax,DIR_OUT);
	return 1;
}

// ============================================================================
	static int is_carrier	(void )
// ============================================================================
{
int ret;

	ret=RS_Carrier(ComFax);
	return ret;
}

// ============================================================================
	static void time_release (void)
// ============================================================================
{
	Sleep(2);
	return;
}

// ============================================================================
	static int SENDBYTE	(unsigned char c)
// ============================================================================
{
unsigned short result;

	RS_TXPInTime(ComFax,(char *)&c,1,SEC_1,&result);
	return 1;
}


// ============================================================================
	static int CHAR_AVAIL (void)
// ============================================================================
{
int ret;

	ret=ComBufCount(ComFax,DIR_INC);
	if (ret>0)
		return 1;
	else
		return 0;
}

// ============================================================================
	static unsigned short MODEM_IN (void)
// ============================================================================
{
	char c;

	RS_RXInTime(ComFax,&c,SEC0_5);
	return ((unsigned short) c);
}

// ============================================================================
	static unsigned short TIMED_READ (int timeout)
// ============================================================================
{
	char c;

	RS_RXInTime(ComFax,&c,SEC0_5);
	return ((unsigned short) c);
}


// ============================================================================
	static int	SENDCHARS (char *text,int size,int dcdcheck)
// ============================================================================
{
unsigned short result;
int ret;

	// Happy compiler ...
	ret=dcdcheck;
	RS_TXPInTime(ComFax,text,size,SEC_5,&result);
	return 1;
}


// ============================================================================
static int	generate_fax_info_mail (void)
// ============================================================================
{
CString fullname;
CString subject;
CString mailtext;
CString help1;
CString file;
CString sender;
CString help2;
CString fehldeu;
CString fehleng;
CString fehlrus;
CString created;

int		ret,page;

	db_open_simple(0);
	get_fullname(fullname);
	sender=gReceivedFaxList.GetString(0);
    trim_all(sender);
	subject=L("S_240");
	mailtext=L("S_28",sender,gReceivedFaxList.defaultindex);
	for (int n=1;n < gReceivedFaxList.GetCount();n++)
	{
		help1=gReceivedFaxList.GetString(n);
		get_token(help1,0,file);
		page=get_token_int(help1,1);
		mailtext+=L("S_401",page+1,file);
	}
	// Hier führen wir die 'faxin1.bat' aus falls vorhanden
	if(access("faxin1.bat",0)==0)
	{
		char buf1[300],buf[300],*p;

		strcpy(buf,file);
		p=strrchr(buf,'\\');
		if (p)
		{
			*p=0;
			sprintf(buf1,"faxin1.bat %s",buf);
			system(buf1);
		}
	}

	ret=convert_fax(file,created);
	switch (ret)
	{
	case 0:
		mailtext+=langstr(fehldeu,fehleng,fehlrus);
		mailtext+="\r\n";
		modem_add_listbox(L("S_202"));	// error conversion
		break;
	case 1:
		mailtext+=L("S_33",created);	// fax created
		modem_add_listbox(L("S_241"));	// successfully converted
		break;
	case 2:
		mailtext+=L("S_290");	// conversion off
		modem_add_listbox(L("S_239"));	// conversion disabled
		break;
	}

	mailtext+="\n\r\n\r";
	sender=L("S_242");	// FAX-manager
	build_routed_netmail(sender,"",fullname,"",subject,mailtext,0,0,1);

	if(access("faxin2.bat",0)==0)
	{
		char buf1[300],buf[300],*p;

		strcpy(buf,file);
		p=strrchr(buf,'\\');
		if (p)
		{
			*p=0;
			sprintf(buf1,"faxin2.bat %s",buf);
			system(buf1);
		}
	}

	// force Update for Area Read Statistic
	db_refresh_area_info(0);
	if (gustat.cur_area_handle==0)
		gpMain->OnUpdateScreenDisplay();
	
	return 1;
}

// ============================================================================
// converts FAX into .AWD Format.
	int convert_fax	(LPCSTR filename,CString &created)
// ============================================================================
// returns:
// 0 Error
// 1 OK
// 2 Not enabled
{
CString		alcall,help3,todel;
CStrList	xx;
char		file[1000],*p;
int			i,retval=0;

	created.Empty();
	if (faxcfg.reserved4)	return 2;
	strcpy(file,filename);
	p=strrchr(file,'\\');
	if (p)
	{
		p++;
		*p=0;
	}

	alcall=alcall+"faxconv.exe -a"+file+"pages.awd "+file;
	system(alcall);
	created="*";
	created+=file;
	created+="pages.awd*";

	if (!faxcfg.reserved3)
	{
		addbackslash(file);
		file[strlen(file)-1]=0;
		help3.Format("%s\\*.fax",file);
		xx.FillWithFiles(help3);
		for (i=0;i<xx.GetCount();i++) {	todel=xx.GetString(i);	unlink(todel); }
		help3.Format("%s\\*.bmp",file);
		xx.FillWithFiles(help3);
		for (i=0;i<xx.GetCount();i++) {	todel=xx.GetString(i);	unlink(todel); }
	}
	return 1;
}


// ============================================================================
	int handle_fax_doubleclick(CString line)
// ============================================================================
{
char	buf[300],*p,*pa;
int     ret;
CString file,str,err;

	line.MakeLower();
	memset(buf,0,sizeof(buf));
	p= (char *) strstr(line,"pages.awd");
	if (!p)	return 0;
	pa=p;
	while (*pa != ' ')	pa--;
	pa++;
	if (*pa=='*')	pa++;

	p+=9;
	memcpy(buf,pa,p-pa);
	str=faxcfg.GetString(0);
	if (str.GetLength()==0)	str="faxview.exe ";
	str+=buf;
	ret=WinExec(PS str,SW_SHOWNORMAL);
	if (ret<=31)
	{
		switch (ret)
		{
			case	0:
				err="Out of resources";
				break;
			case	ERROR_FILE_NOT_FOUND:
				err="File not found";
				break;
			case	ERROR_PATH_NOT_FOUND:
				err="Path not found";
				break;
		}
		err_out("E_EXFAXV",buf,err);
		return 0;
	}
	return 1;
}

// ============================================================================
	int handle_fax_with_bgfax	(CString line)
// ============================================================================
{
char	buf[300],*p,*pa,*px;
int     ret,pos;
CString file,str,err,thelp;

	line.MakeLower();
	memset(buf,0,sizeof(buf));
	p= (char *) strstr(line,".fax");
	if (!p)	return 0;
	pa=p;
	while (*pa != ' ')	pa--;
	pa++;
	if (*pa=='*')	pa++;
	p+=9;
	memcpy(buf,pa,p-pa);
	str=faxcfg.GetString(4);
	if (str.GetLength()==0)	ERR_MSG_RET0("E_NODFAXVCONV");
	px=strchr(buf,'*');
	if (px)	*px=0;
	pos=str.Find("%1");
	if (pos==-1)	ERR_MSG_RET0("E_PARAM1FA");
	char shortbuf[300];
	memset(shortbuf,0,sizeof(shortbuf));
	GetShortPathName(buf,shortbuf,299);
	px= (char *) strstr(str,"%1");
	thelp=str.Left(pos);
	thelp+=shortbuf;
	thelp+=" ";
	px+=2;
	thelp+=px;
	str=thelp;
	ret=WinExec(str,SW_SHOWNORMAL);
	if (ret<=31)
	{
		switch (ret)
		{
			case	0:
				err="Out of resources";
				break;
			case	ERROR_FILE_NOT_FOUND:
				err="File not found";
				break;
			case	ERROR_PATH_NOT_FOUND:
				err="Path not found";
				break;
		}
		err_out("E_EXFAXV",buf,err);
		return 0;
	}
	return 1;
}
