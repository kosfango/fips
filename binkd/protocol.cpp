
#include "stdafx.h"

#include <sys/stat.h>
#include "iphdr.h"
#include "iptools.h"
#include "bsy.h"
#include "inbound.h"
#include "srif.h"
#include "readflo.h"
#include "protocol.h"
#include "binlog.h"
#include "md5b.h"
#include "crypt.h"
#include "sem.h"
#include "..\globals.h" 
#include "..\SUP\pr_xfer.h" 

extern MUTEXSEM hostsem;

extern int no_MD5;
extern _gconfig gc;

int n_servers = 0;

static char *scommand[] = {"NUL", "ADR", "PWD", "FILE", "OK", "EOB",
                           "GOT", "ERR", "BSY", "GET", "SKIP"};

static struct skiprule
  {
    struct skiprule *next;
    char *mask;
  } *skipmask = NULL;

int abort_bink(void);

void deletesendfile(char *namefile)
{ char buffer[500];
//if (state->out.f) return;
strcpy(buffer,curmail.basedir);
strcat(buffer,"\\");
strcat(buffer,namefile);
deletefile(buffer);
//NetMail
strcpy(buffer,curmail.basedir);
strcat(buffer,"\\NETMAIL\\");
strcat(buffer,namefile);
deletefile(buffer);


}
void resived_status(STATE *state)
{
long elapsed,bytes,cps;
char tempbuf[200];
char tempbuf2[200];
int dif,t,m,s;

//Данные сообщения а не файла
if (state->imsg) return;
//Проверка на не открытый файл
if (state->in.f<=0) return;

		elapsed = time(NULL) - state->in.start;
		bytes = ftell(state->in.f)+state->oleft;
		show_size(tempbuf2,state->in.size);  
		sprintf(tempbuf,"%s %s",state->in.netname,tempbuf2);
		gc.mailer.m_transferred2=bytes;
		gc.mailer.m_total2=state->in.size;
		dif=state->in.size-bytes;
		if (gc.mailer.m_total2>0) 
			gc.mailer.m_proz2=(gc.mailer.m_transferred2*100)/gc.mailer.m_total2;
		else 
			gc.mailer.m_proz2=0;
		cps = elapsed ? bytes / elapsed:0;
		dif= cps ? dif/cps:0;
		t=(int)(dif/3600);
		dif=dif-(t*3600);
		m=(int)(dif/60);
		s = dif-(m*60);
		sprintf(tempbuf2," (%02d:%02d:%02d) ",t,m,s);
		strcat(tempbuf,tempbuf2);
		gc.mailer.m_cps2=cps;
		gc.mailer.m_timeelapsed2=elapsed;
		strcpy(gc.mailer.m_filename2,tempbuf);

		update_charging();
		modem_update();

return;
}

void send_status(STATE *state)
{
long elapsed,bytes,cps;
char tempbuf[200];
char tempbuf2[200];
int dif,t,m,s;

//Данные сообщения а не файла
//if (state->imsg) return;
//Проверка на не открытый файл
if (state->out.f<=0) return;


		elapsed = time(NULL) - state->out.start;
		bytes = ftell(state->out.f)+state->oleft;
		show_size(tempbuf2,state->out.size);
		sprintf(tempbuf,"%s %s",state->out.netname,tempbuf2);
		gc.mailer.m_transferred=bytes;
		gc.mailer.m_total=state->out.size;
		dif=state->out.size-bytes;
		if (gc.mailer.m_total>0) 
			gc.mailer.m_proz=(gc.mailer.m_transferred*100)/gc.mailer.m_total;
		else 
			gc.mailer.m_proz=0;
		cps = elapsed ? bytes / elapsed:0;
		dif= cps ? dif/cps:0;
		t=(int)(dif/3600);
		dif=dif-(t*3600);
		m=(int)(dif/60);
		s = dif-(m*60);
		sprintf(tempbuf2," (%02d:%02d:%02d) ",t,m,s);
		strcat(tempbuf,tempbuf2);
		gc.mailer.m_cps=cps;
		gc.mailer.m_timeelapsed=elapsed;
		strcpy(gc.mailer.m_filename,tempbuf);

		
		update_charging();
	        modem_update();
}
 /*
 * Fills <<state>> with initial values, allocates buffers, etc.
 */
static int init_protocol (STATE *state, SOCKET socket, FTN_NODE *to)
{

  memset (state, 0, sizeof (STATE));

  state->major = 1;
  state->minor = 0;
  state->msgs_in_batch = 0;
  state->off_req_sent = 0;
  state->waiting_for_GOT = 0;
  state->send_eof = 0;
  state->inbound = inbound_nonsecure;
  state->io_error = 0;
  state->ibuf = (char*) xalloc (MAX_BLKSIZE + BLK_HDR_SIZE + 1);
  state->isize = -1;
  state->obuf = (char*) xalloc (MAX_BLKSIZE + BLK_HDR_SIZE + 1);
  state->optr = 0;
  state->oleft = 0;
  state->bytes_sent = state->bytes_rcvd = 0;
  state->files_sent = state->files_rcvd = 0;
  state->to = to;
  state->NR_flag = (to && to->NR_flag == NR_ON) ? WANT_NR : NO_NR;
  state->ND_flag = (to && to->ND_flag == ND_ON) ? WE_ND : NO_ND;
  state->MD_flag = 0;
  state->MD_challenge = NULL;
  state->crypt_flag = (!to || to->crypt_flag == CRYPT_ON) ? WE_CRYPT : NO_CRYPT;
  strcpy (state->expected_pwd, "-");
  state->skip_all_flag = state->r_skipped_flag = 0;
  state->maxflvr = 'h';
  state->listed_flag = 0;
  state->fa = NULL;
  state->nfa = state->nallfa = 0;
  setsockopts (state->s = socket);
  TF_ZERO (&state->in);
  TF_ZERO (&state->out);
  TF_ZERO (&state->flo);
  TF_ZERO (&state->in_complete);
  state->ND_addr.z = -1;
  state->start_time = time (NULL);
  Log (6, "binkp init done, socket # is %i", state->s);
  return 1;
}

/*
 * Clears protocol buffers and queues, closes files, etc.
 */
static int deinit_protocol (STATE *state)
{
  int i;

  if (state->in.f)
  {
    fclose (state->in.f);
    if (pmatch ("????????.[Pp][Kk][Tt]", state->in.netname))
    {
      Log (2, "%s: partial .pkt", state->in.netname);
      inb_reject (state->in.netname, state->in.size, state->in.time,
		  state->fa, state->nallfa, state->inbound);
    }
  }
  if (state->out.f)
    fclose (state->out.f);
  if (state->in_complete.f)
    fclose (state->in_complete.f);
  if (state->flo.f)
    fclose (state->flo.f);
  if (state->killlist)
    free_killlist (&state->killlist, &state->n_killlist);
  if (state->rcvdlist)
    free_rcvdlist (&state->rcvdlist, &state->n_rcvdlist);
  if (state->ibuf)
    free (state->ibuf);
  if (state->obuf)
    free (state->obuf);
  if (state->msgs)
    free (state->msgs);
  if (state->sent_fls)
    free (state->sent_fls);
  if (state->q)
    q_free (state->q);
  for (i = 0; i < state->nfa; ++i)
    bsy_remove (state->fa + i, F_BSY);
  if (state->fa)
    free (state->fa);
  if (state->MD_challenge)
	free (state->MD_challenge);
  Log (6, "binkp deinit done...");

  
  return 0;
}
/* Process rcvdlist */
FTNQ *process_rcvdlist (STATE *state, FTNQ *q)
{
  int i;

  Log (6, "processing rcvd list");
  for (i = 0; i < state->n_rcvdlist; ++i)
  {
    q = evt_run (q, state->rcvdlist[i].name, state->fa, state->nfa,
		 state->state == P_SECURE, state->listed_flag,
		 state->peer_name, NULL);
  }
  return q;
}

/* Fills s[0] and s[1] with binkp frame header using value of u */
static void mkhdr (unsigned char *s, unsigned u)
{
  s[0] = (unsigned char) (u >> 8);
  s[1] = (unsigned char) u;
}

/*
 * Puts a message to the output msg. queue. These msgs will be send
 * right after the current data block.
 */
static void msg_send2 (STATE *state, t_msg m, char *s1, char *s2)
{
  if (!s1)
    s1 = "";
  if (!s2)
    s2 = "";

  state->msgs = (struct _BMSG *) xrealloc (state->msgs, sizeof (BMSG) * (state->n_msgs + 1));
  state->msgs[state->n_msgs].t = m;
  /* We will check for sz correctness (sz <= 0x7fff) later, when sending
   * messages from the queue. */
  state->msgs[state->n_msgs].sz = strlen (s1) + strlen (s2) + 1;
  state->msgs[state->n_msgs].s =
    (char*) xalloc (state->msgs[state->n_msgs].sz + 3);

  mkhdr ((unsigned char *) state->msgs[state->n_msgs].s,
	 (unsigned) (state->msgs[state->n_msgs].sz | 0x8000));
  state->msgs[state->n_msgs].s[2] = m;

  strcpy (state->msgs[state->n_msgs].s + 3, s1);
  strcat (state->msgs[state->n_msgs].s + 3, s2);
  state->msgs[state->n_msgs].sz += 2;
  if (state->crypt_flag == YES_CRYPT)
    encrypt_buf(state->msgs[state->n_msgs].s, state->msgs[state->n_msgs].sz,
            state->keys_out);

  ++state->n_msgs;
  ++state->msgs_in_batch;

  Log (5, "send message %s %s%s", scommand[m], s1, s2);
}

/*
 * Sends a message using format string
 */
static void msg_sendf (STATE *state, t_msg m, char *s,...)
{
  char msg_text[max (MAXPATHLEN, MAX_NETNAME) + 80];
  va_list ap;

  va_start (ap, s);
#ifdef HAVE_SNPRINTF
  vsnprintf (msg_text, sizeof (msg_text), s, ap);
#else
  vsprintf (msg_text, s, ap);
#endif
  va_end (ap);
  msg_send2 (state, m, msg_text, 0);
}

static void current_file_was_sent (STATE *state)
{
  fclose (state->out.f);
  state->sent_fls = (struct _TFILE *) xrealloc (state->sent_fls,
			      ++(state->n_sent_fls) * sizeof (TFILE));
  memcpy (state->sent_fls + state->n_sent_fls - 1,
	  &state->out,
	  sizeof (TFILE));
  TF_ZERO (&state->out);
  if (state->ND_flag == YES_ND)
  {
    state->waiting_for_GOT = 1;
    Log(4, "Waiting for M_GOT");
  }
clear_status(true);
}

/*
 * Sends next msg from the msg queue or next data block
 */
static int send_block (STATE *state)
{
  int i, n;
  send_status(state);
 //Проверка на выход
	  if (abort_bink()) return 0;

  /* Have something to send in buffers */
  if (state->optr && state->oleft)
  {
    Log (7, "sending %li byte(s)", (long) (state->oleft));
    n = send_socet (state->s, state->optr, state->oleft, 0);
    Log (7, "send() done, rc=%i", n);
    if (n == state->oleft)
    {
      state->optr = 0;
      state->oleft = 0;
      Log (7, "data sent");
    }
    else if (n == SOCKET_ERROR)
    {
      if (TCPERRNO != TCPERR_WOULDBLOCK && TCPERRNO != TCPERR_AGAIN)
      {
	state->io_error = 1;
	Log (1, "send: %s", TCPERR ());
	if (state->to)
	  bad_try (&state->to->fa, TCPERR ());
	return 0;
      }
      Log (7, "data transfer would block");
    }
    else
    {
      state->optr += n;
      state->oleft -= n;
      Log (7, "partially sent, %li byte(s) left", state->oleft);
    }
  }
  else
  {
    /* There is no data partially sent */

    if (state->msgs)
    {
      /* There are unsent msgs */
      state->optr = state->obuf;
      state->oleft = 0;
      for (i = 0; i < state->n_msgs; ++i)
      {
	if (state->msgs[i].s)
	{
	  /* Check for possible internal error */
	  if (state->msgs[i].sz - 2 > MAX_BLKSIZE)
	  {
	    Log (1, "size of msg we want to send is too big (%i)",
		 state->msgs[i].sz - 2);
	    return 0;
	  }

	  /* Is there some space for the new msg? */
	  if (state->oleft + state->msgs[i].sz > MAX_BLKSIZE)
	    break;

	  Log (7, "put next msg to obuf, %li", (long) state->msgs[i].sz);
	  memcpy (state->optr, state->msgs[i].s, state->msgs[i].sz);
	  state->oleft += state->msgs[i].sz;
	  state->optr += state->msgs[i].sz;
	  free (state->msgs[i].s);
	  state->msgs[i].s = 0;
	}
      }

      /* Optr should be non-NULL if there are some data to send */
      if (state->oleft == 0)
	state->optr = 0;
      else
	state->optr = state->obuf;

      /* If the message queue is empty, free it */
      if (i >= state->n_msgs)
      {
	free (state->msgs);
	state->msgs = 0;
	state->n_msgs = 0;
      }
      return 1;
    }
    else if ((state->out.f && !state->off_req_sent && !state->waiting_for_GOT) ||
	     state->send_eof)
    {
      /* There is a file in transfer and we don't wait for an answer for * *
       * "FILE ... -1" */
      unsigned sz;

      if (state->out.f)
      {
	sz = min ((size_t) oblksize, state->out.size - ftell (state->out.f));
	if (percents && state->out.size > 0)
	{
	  printf ("%-20.20s %3.0f%%\r", state->out.netname,
		  100.0 * ftell (state->out.f) / (float) state->out.size);
	  fflush (stdout);
	}
      }
      else
      {
	state->send_eof = 0;
	sz = 0;
      }
      Log (7, "next block to send: %u byte(s)", sz);
      mkhdr ((unsigned char *)state->obuf, sz);
      if (sz != 0)
      {
	Log (7, "freading %u byte(s)", sz);
	if ((n = fread (state->obuf + BLK_HDR_SIZE, 1, sz, state->out.f)) < (int) sz)
	{
	  Log (1, "error reading %s: expected %u, read %i",
	       state->out.path, sz, n);
	  return 0;
	}
	  
      }
      else if (state->out.f)
	/* The current file have been sent */
	current_file_was_sent (state);
      state->optr = state->obuf;
      state->oleft = sz + BLK_HDR_SIZE;
      if (state->crypt_flag == YES_CRYPT)
        encrypt_buf(state->optr, state->oleft, state->keys_out);
   }
  }
  return 1;
}

/*
 * Extends parse_args() by sending error message to the remote
 */
static int parse_msg_args (int ac, char **av, char *s, char *ID, STATE *state)
{
  int rc = parse_args (ac, av, s, ID);

  if (!rc)
    msg_send2 (state, M_ERR, ID, ": cannot parse args");
  return rc;
}

/*
 * Truncates the file if action == 't'
 * Removes the file if action == 'd'
 * Removes on successful session if action == 's'
 * Otherwise lefts the file unchanged
 */
static int perform_action (STATE *state, char *path, char action)
{
  if (action == 'd')
  {
    deletefile (path);
  }
  else if (action == 't')
  {
    trunc (path);
  }
  else if (action == 's')
  {
    add_to_killlist (&state->killlist, &state->n_killlist, path, 's');
  }
  return 1;
}

/*
 * Marks the file in flopath as sent. (Empty .?lo will be removed)
 * If file == 0 just tryes to unlink flopath.
 * If flopath == 0 performs action on file.
 */
static int remove_from_spool (STATE *state, char *flopath,
			       char *file, char action)
{
  char buf[MAXPATHLEN + 1], *w = 0;
  FILE *flo = 0;
  size_t offset = 0, curr_offset;
  int i;
  int seek_flag = 0;		       /* Seek _state->flo.f_ to */

  /* _offset_ after processing */
  int empty_flo_flag = 1;

  if (file)
    Log (5, "removing from spool: %s", file);
  else if (flopath)
    Log (5, "removing flo: %s", flopath);
  else
    Log (1, "internal error in remove_from_spool!");

  if (flopath && *flopath)	       /* A file attached via .?lo */
  {
    if (state->flo.f && !strcmp (state->flo.path, flopath))
    {
      flo = state->flo.f;
      offset = ftell (flo);
      fseek (flo, 0, SEEK_SET);
      seek_flag = 1;
    }
    else
    {
      if ((flo = fopen (flopath, "r+b")) == 0)
      {
	Log (5, "remove_from_spool: %s: %s", flopath, strerror (errno));
	return 0;
      }
    }

    while (!feof (flo))
    {
      curr_offset = ftell (flo);
      if (!fgets (buf, MAXPATHLEN, flo))
	break;
      for (i = strlen (buf) - 1; i > 0 && isspace (buf[i]); --i)
	buf[i] = 0;

      if (file && (!strcmp (file, buf) ||
		 ((*buf == '^' || *buf == '#') && !strcmp (file, buf + 1))))
      {
	clearerr (flo);
	if (fseek (flo, curr_offset, SEEK_SET) == EOF)
	  Log (1, "remove_from_spool: fseek(%s): %s", flopath,
	       strerror (errno));
	else if (putc ('~', flo) == EOF)
	  Log (1, "remove_from_spool: fputc(%s): %s", flopath,
	       strerror (errno));
	fflush (flo);
	/* The line was marked, now skip it */
	fgets (buf, MAXPATHLEN, flo);
	/* We've found the file in flo, so try to translate it's name before
	 * the action */
	if (w == 0 && (w = trans_flo_line (file)) != 0)
	{
	  Log (5, "%s mapped to %s", file, w);
	}
      }
      else if (*buf && *buf != '~')
      {
	empty_flo_flag = 0;
      }
    }
    if (seek_flag)
    {
      fseek (flo, offset, SEEK_SET);
      fflush (flo);
    }
    else
    {
      fclose (flo);
      if (empty_flo_flag)
	deletefile (flopath);
    }
  }
  if (w)
  {
    perform_action (state, w, action);
    free (w);
  }
  else if (file)
    perform_action (state, file, action);
  return 1;
}

/*
 * Removes n-th element from the sent files queue,
 * free()'s the queue if it's empty
 */
static void remove_from_sent_files_queue (STATE *state, int n)
{
  state->sent_fls[n].netname[0] = 0;

  for (n = 0; n < state->n_sent_fls; ++n)
    if (state->sent_fls[n].netname[0])
      return;

  if (n >= state->n_sent_fls)
  {
    free (state->sent_fls);
    state->sent_fls = 0;
    state->n_sent_fls = 0;
  }
}

static void do_prescan(STATE *state)
{
  char s[64];
  unsigned long netsize, filessize;
  int savend;

  if (OK_SEND_FILES (state) && prescan)
  {
    state->q = q_scan_addrs (0, state->fa, state->nfa);
    /* hack to avoid warning: this scan can be before receive "OPT ND" */
    savend = state->ND_flag;
    state->ND_flag = YES_ND;
    q_get_sizes (state->q, &netsize, &filessize);
    if (state->q) q_free (state->q);
    state->q = NULL;
    state->ND_flag = savend;
    sprintf(s, "%lu %lu", netsize, filessize);
    msg_send2 (state, M_NUL, "TRF ", s);
  }
}

/*
 * These functions down to recv_block() handle binkp msgs
 *
 * They should return 0 on fatal error, otherwise 1
 */

/*
 * Parses if needed and logs down the M_NUL message data
 */
static int NUL (STATE *state, char *buf, int sz)
{
  char *s, *a, *b;
  char sbuf[255];

  Log (3, "%s", s = strquote (buf, SQ_CNTRL));
  if (!memcmp (s, "VER ", 4) &&
      (a = strstr (s, PRTCLNAME "/")) != 0 &&
      (b = strstr (a, ".")) != 0)
  {
    state->major = atoi (a + 6);
    state->minor = atoi (b + 1);
    Log (6, "remote uses " PRTCLNAME " v.%i.%i", state->major, state->minor);
  }
  else if (!memcmp (s, "OPT ", 4))
  {
    char *w;
    int i;

    for (i = 1; (w = getwordx (s + 4, i, 0)) != 0; ++i)
    {
      if (!strcmp (w, "NR"))
      {
	state->NR_flag = WANT_NR;      /* They want NR mode */
        Log(2, "Remote requests NR mode");
      }
      if (!strcmp (w, "ND"))
      {
	state->ND_flag |= THEY_ND;     /* They want ND mode */
        Log(2, "Remote requests ND mode");
      }

      if (!strcmp (w, "CRYPT"))
      {
	state->crypt_flag |= THEY_CRYPT;  /* They want crypt mode */
        Log(2, "Remote requests CRYPT mode");
      }

      if (!strncmp(w, "CRAM-", 5) && !no_MD5 &&
          state->to && (state->to->MD_flag>=0))
      {
        Log(2, "Remote requests MD mode");
        if(state->MD_challenge) free(state->MD_challenge);
        state->MD_challenge=MD_getChallenge(w, NULL);
      }
      free (w);
    }
  }
  else if (!memcmp (s, "SYS ", 4))
    strnzcpy (state->sysname, s + 4, sizeof (state->sysname));
  else if (!memcmp (s, "ZYZ ", 4))
    strnzcpy (state->sysop, s + 4, sizeof (state->sysop));
  else if (!memcmp (s, "LOC ", 4))
    strnzcpy (state->location, s + 4, sizeof (state->location));
  if (!memcmp (s, "SYS ", 4)) {sprintf(sbuf,"| %s",s);modem_add_listbox(sbuf);}
  if (!memcmp (s, "ZYZ ", 4)) {sprintf(sbuf,"| %s",s);modem_add_listbox(sbuf);}
  if (!memcmp (s, "LOC ", 4)) {sprintf(sbuf,"| %s",s);modem_add_listbox(sbuf);}
  if (!memcmp (s, "VER ", 4)) {sprintf(sbuf,"| %s",s);modem_add_listbox(sbuf);}
  if (!memcmp (s, "NDL ", 4)) {sprintf(sbuf,"| %s",s);modem_add_listbox(sbuf);}
  if (!memcmp (s, "OPM ", 4)) {sprintf(sbuf,"| %s",s);modem_add_listbox(sbuf);}
  if (!memcmp (s, "TIME ", 5)) {sprintf(sbuf,"| %s",s);modem_add_listbox(sbuf);}
  if (!memcmp (s, "TRF ", 5)) {sprintf(sbuf,"| %s",s);modem_add_listbox(sbuf);}
  if (!memcmp (s, "CRYPT", 5)) {sprintf(sbuf,"| %s",s);modem_add_listbox(sbuf);}
  if (!memcmp (s, "CRAM-", 5)) {sprintf(sbuf,"| %s",s);modem_add_listbox(sbuf);}
  free (s);
  
  strcpy(gc.mailer.m_system,state->sysname);
  sprintf(gc.mailer.m_sysop,"%s",state->sysop);
  sprintf(gc.mailer.m_location,"%s",state->location);
  modem_update();
  	
  return 1;
}

/*
 * Handles M_ERR msg from the remote
 */
static int RError (STATE *state, char *buf, int sz)
{
  char *s;

  Log (1, "rerror: %s", s = strquote (buf, SQ_CNTRL));
  if (state->to)
    bad_try (&state->to->fa, s);
  free (s);
  return 0;
}

static int BSY (STATE *state, char *buf, int sz)
{
  char *s;

  Log (1, "got M_BSY: %s", s = strquote (buf, SQ_CNTRL));
  if (state->to)
    bad_try (&state->to->fa, s);
  free (s);
  return 0;
}

static int ADR (STATE *state, char *s, int sz)
{
  int i, j, main_AKA_ok = 0;
  char *w;
  FTN_ADDR fa;
  FTN_NODE *n;
  char szFTNAddr[FTN_ADDR_SZ + 1];
  char sbuf[255];

  s[sz] = 0;
  sprintf(sbuf,"| ADR %s",s);modem_add_listbox(sbuf);
  
  for (i = 1; (w = getwordx (s, i, 0)) != 0; ++i)
  {
    if (!parse_ftnaddress (w, &fa) || !is5D (&fa))
    {
      char *q = strquote (s, SQ_CNTRL);

      msg_send2 (state, M_ERR, "Bad address", 0);
      Log (1, "remote passed bad address: `%s'", q);
      free (w);
      free (q);
      return 0;
    }

    free (w);
    ftnaddress_to_str (szFTNAddr, &fa);

    if (state->to == 0 && (n = get_node_info (&fa)) != 0 && n->restrictIP)
    { int i, ipok = 0, rc;
      struct hostent *hp;
      struct in_addr defaddr;
      char **cp;
      char host[MAXHOSTNAMELEN + 1];       /* current host/port */
      unsigned short port;
      struct sockaddr_in sin;

      i=sizeof(struct sockaddr_in);
      if (getpeername (state->s, (struct sockaddr *) &sin, &i) == -1)
      { Log (1, "Can't getpeername(): %s", TCPERR());
        ipok = 2;
      }

      for (i = 1; ipok == 0 && (rc = get_host_and_port
		  (i, host, &port, n->hosts, &n->fa)) != -1; ++i)
      {
	if (rc == 0)
	{
	  Log (1, "%s: %i: error parsing host list", n->hosts, i);
	  continue;
	}
	if (!isdigit (host[0]) ||
	    (defaddr.s_addr = inet_addr (host)) == INADDR_NONE)
	{
	  /* If not a raw ip address, try nameserver */
	  Log (5, "resolving `%s'...", host);
	  LockSem(&hostsem);
	  if ((hp = gethostbyname (host)) == NULL)
	  {
	    Log (1, "%s: unknown host", host);
	    ReleaseSem(&hostsem);
	    continue;
	  }
	  for (cp = hp->h_addr_list; cp && *cp; cp++)
	    if (((struct in_addr *) * cp)->s_addr == sin.sin_addr.s_addr)
	    {
	      ipok = 1;
	      break;
	    }
	  ReleaseSem(&hostsem);
	}
	else
	{
	  if (defaddr.s_addr == sin.sin_addr.s_addr)
	    ipok = 1;
	}
      }
      if (ipok != 1)
      {
		Log (1, "addr: %s (not from allowed remote address)", szFTNAddr);
        return 0;
      }
    }

    if (bsy_add (&fa, F_BSY))
    {
      Log (2, "addr: %s", szFTNAddr);
      state->fa = (struct _FTN_ADDR *) xrealloc (state->fa, sizeof (FTN_ADDR) * ++state->nallfa);
      ++state->nfa;
      for (j = state->nallfa - 1; j >= state->nfa; j--)
	memcpy (state->fa + j, state->fa + j - 1, sizeof (FTN_ADDR));
      memcpy (state->fa + state->nfa - 1, &fa, sizeof (FTN_ADDR));
      if (state->to &&
	  !ftnaddress_cmp (state->fa + state->nfa - 1, &state->to->fa))
      {
	main_AKA_ok = 1;
      }
    }
    else
    {
      Log (2, "addr: %s (n/a or busy)", szFTNAddr);
      state->fa = (struct _FTN_ADDR *) xrealloc (state->fa, sizeof (FTN_ADDR) * ++state->nallfa);
      memcpy (state->fa + state->nallfa - 1, &fa, sizeof (FTN_ADDR));
    }

    if (state->expected_pwd[0] && (n = get_node_info (&fa)) != 0)
    {
      state->listed_flag = 1;
      if (!strcmp (state->expected_pwd, "-"))
      {
	memcpy (state->expected_pwd, n->pwd, sizeof (state->expected_pwd));
        state->MD_flag=n->MD_flag;
      }
      else if (strcmp (state->expected_pwd, n->pwd))
      {
	Log (1, "inconsistent pwd settings for this node");
	state->expected_pwd[0] = 0;
      }
    }
  }
  if (state->nfa == 0)
  {
    Log (1, "no AKAs in common domains or all AKAs are busy");
    msg_send2 (state, M_BSY, "No AKAs in common domains or all AKAs are busy", 0);
    return 0;
  }
  if (state->to != 0 && main_AKA_ok == 0)
  {
    ftnaddress_to_str (szFTNAddr, &state->to->fa);
    Log (1, "called %s, but remote has no such AKA", szFTNAddr);
    if (state->to)
      bad_try (&state->to->fa, "Remote has no needed AKA");
    return 0;
  }
  if (state->to)
  {
    do_prescan (state);
    if(state->MD_challenge)
    {
      char *tp=MD_buildDigest(state->to->pwd, (unsigned char *) state->MD_challenge);
      if(!tp) 
      {
        Log(2, "Unable to build MD5 digest");
        bad_try (&state->to->fa, "Unable to build MD5 digest");
        return 0;
      }
      msg_send2 (state, M_PWD, tp, 0);
      state->MD_flag=1;
      free(tp);
    }
    else if ((state->to->MD_flag == 1) && !no_MD5) /* We do not want to talk without MD5 */
    {
      Log(2, "CRAM-MD5 is not supported by remote");
      bad_try (&state->to->fa, "CRAM-MD5 is not supported by remote");
      return 0;
    }
    else
    msg_send2 (state, M_PWD, state->to->pwd, 0);
  }
  return 1;
}

static char *select_inbound (FTN_ADDR *fa, int secure_flag)
{
  FTN_NODE *node = get_node_info (fa);

  return ((node && node->ibox) ? node->ibox :
	  (secure_flag == P_SECURE ? inbound : inbound_nonsecure));
}

static void complete_login (STATE *state)
{
  state->NR_flag =
  (state->NR_flag == WANT_NR && state->major * 100 + state->minor >= 101) ?
  WE_NR :
  NO_NR;
  if (state->ND_flag!=YES_ND) state->ND_flag=NO_ND;
  state->inbound = select_inbound (state->fa, state->state);
  if (OK_SEND_FILES (state))
    state->q = q_scan_addrs (0, state->fa, state->nfa);
  state->msgs_in_batch = 0;	       /* Forget about login msgs */
  if (state->state == P_SECURE)
    Log (2, "pwd protected session (%s)",
         (state->MD_flag == 1) ? "MD5" : "plain text");
  if (state->ND_flag == YES_ND)
    Log (3, "session in ND mode");
  else if (state->NR_flag == WE_NR)
    Log (3, "we are in NR mode");
  if (state->state != P_SECURE)
    state->crypt_flag = NO_CRYPT;
  else if (state->crypt_flag == (WE_CRYPT|THEY_CRYPT) && !state->MD_flag)
  { state->crypt_flag = NO_CRYPT;
    Log (3, "Crypt allowed only with MD5 authorization");
  }
  else if (state->crypt_flag == (WE_CRYPT|THEY_CRYPT))
  { char *p;
    state->crypt_flag = YES_CRYPT;
    Log (3, "session in CRYPT mode");
    if (state->to)
    { init_keys(state->keys_out, state->to->pwd);
      init_keys(state->keys_in,  "-");
      for (p=state->to->pwd; *p; p++)
        update_keys(state->keys_in, (int)*p);
    } else
    { init_keys(state->keys_in, state->expected_pwd);
      init_keys(state->keys_out,  "-");
      for (p=state->expected_pwd; *p; p++)
        update_keys(state->keys_out, (int)*p);
    }
  }
  if (state->crypt_flag!=YES_CRYPT) state->crypt_flag=NO_CRYPT;
}

static int PWD (STATE *state, char *pwd, int sz)
{
  int bad_pwd=STRNICMP(pwd, "CRAM-", 5);
  int no_password=!strcmp (state->expected_pwd, "-");
  if ((no_password)&&(bad_pwd))
  {
    do_prescan (state);
    state->state = P_NONSECURE;
    msg_send2 (state, M_OK, "non-secure", 0);
    if (strcmp (pwd, "-"))
      Log (1, "unexpected password from the remote: `%s'", pwd);
  }
  else 
  {
    if((state->MD_flag == 1) || ((!bad_pwd) && (state->MD_challenge)))
    {
      char *sp;
      if((bad_pwd)&&(state->MD_flag))
      {
        msg_send2(state, M_ERR, "You must support MD5", 0);
        Log(1, "Caller not supported MD5");
        return 0;
      }
      if((sp=MD_buildDigest(state->expected_pwd, (unsigned char *) state->MD_challenge))!=NULL)
      {
        if((bad_pwd=STRICMP(sp, pwd))==0) state->MD_flag=1;
	free(sp);
        sp=NULL;
      }
      else {
        Log(2, "Unable to build Digest");
        bad_pwd=1;
      }
    }
    else bad_pwd=(state->expected_pwd[0] == 0 || strcmp (state->expected_pwd, pwd));

    if ((bad_pwd)&&(!no_password)) /* I don't check password if we do not need one */
    {
      msg_send2 (state, M_ERR, "Bad password", 0);
      Log (1, "`%s': incorrect password", pwd);
      return 0;
    }
    else
    {
      if(no_password) 
      {
        state->state = P_NONSECURE;
        do_prescan (state);
        msg_send2 (state, M_OK, "non-secure", 0);
        if(bad_pwd) 
          Log (1, "unexpected password digest from the remote");
      }
      else
      {
	state->state = P_SECURE;
        do_prescan (state);
	msg_send2 (state, M_OK, "secure", 0);
      }
    }
  }
  complete_login (state);
  return 1;
}

static int OK (STATE *state, char *buf, int sz)
{
  state->state = !strcmp (state->to->pwd, "-") ? P_NONSECURE : P_SECURE;
  complete_login (state);
  return 1;
}

void skipmask_add(char *mask)
{
  struct skiprule *ps;

  if (skipmask == NULL)
  {
    skipmask = (struct skiprule *) xalloc(sizeof(*skipmask));
    ps = skipmask;
  }
  else
  {
    for (ps = skipmask; ps->next; ps = ps->next);
    ps->next = (struct skiprule *) xalloc(sizeof(*ps));
    ps = ps->next;
  }
  ps->next = NULL;
  ps->mask = (char*) xstrdup(mask);
}

static int skip_test(char *netname, char *mask)
{
  struct skiprule *ps;

  for (ps = skipmask; ps; ps = ps->next)
    if (pmatch(ps->mask, netname))
    {
      strcpy(mask, ps->mask);
      return 1;
    }
  return 0;
}

/*
 * Handles M_FILE msg from the remote
 * M_FILE args: "%s %li %li %li", filename, size, time, offset
 */
static int start_file_recv (STATE *state, char *args, int sz)
{
  const int argc = 4;
  char *argv[4];
  size_t offset;
  char tempbuf2[255];

  if (parse_msg_args (argc, argv, args, "M_FILE", state))
  {
    /* They request us for offset (M_FILE "name size time -1") */
    int off_req = 0;

    if (state->in.f &&		       /* Already receiving smthing */
	strcmp (argv[0], state->in.netname))	/* ...a file with another *
						 * name! */
    {
      fclose (state->in.f);
      Log (1, "receiving of %s interrupted", state->in.netname);
      TF_ZERO (&state->in);
    }
    if ((state->ND_flag == YES_ND) && state->in_complete.f)
    { /* rename complete received file to its true form */
      char realname[MAXPATHLEN + 1];
      char szAddr[FTN_ADDR_SZ + 1];

      if (fclose(state->in_complete.f))
        Log (1, "Cannot fclose(%s): %s!",
             state->in_complete.netname, strerror (errno));
      if (inb_done (state->in_complete.netname, state->in_complete.size,
	            state->in_complete.time, state->fa, state->nallfa,
		    state->inbound, realname) == 0)
        {	resived_status(state);return 0;} /* error, drop session */
      if (*realname)
      {
        /* Set flags */
        if(evt_test (realname))
          state->q = evt_run (state->q, realname, state->fa, state->nfa,
                 state->state == P_SECURE, state->listed_flag,
                 state->peer_name, state);
        /* We will run external programs using this list */
        add_to_rcvdlist (&state->rcvdlist, &state->n_rcvdlist, realname);
      }
      ftnaddress_to_str (szAddr, state->fa);
      state->bytes_rcvd += state->in_complete.size;
      ++state->files_rcvd;
      Log (2, "rcvd: %s (%li, %.2f CPS, %s)", state->in_complete.netname,
	   (long) state->in_complete.size,
	   (double) (state->in_complete.size) /
	   (time (0) == state->in_complete.start ?
	                1 : (time (0) - state->in_complete.start)), szAddr);
      TF_ZERO (&state->in_complete);
    }
    if (state->in.f == 0)
    {
      state->in.start = time (0);
      strnzcpy (state->in.netname, argv[0], MAX_NETNAME);
      state->in.size = atol (argv[1]);
      state->in.time = atol (argv[2]);
    }
    offset = (size_t) atol (argv[3]);
    if (!strcmp (argv[3], "-1"))
    {
      off_req = 1;
      Log (6, "got offset request for %s", state->in.netname);
      if (state->NR_flag != THEY_NR)
      {
	state->NR_flag = THEY_NR;
	if (!state->ND_flag)
	  Log (3, "remote is in NR mode");
      }
    }

    if (state->in.f == 0)
    {
      char realname[MAXPATHLEN + 1];

      if (skip_test (state->in.netname, realname))
      {
	Log (1, "skipping %s (destructive, %li byte(s), skipmask %s)",
	     state->in.netname, state->in.size, realname);
	msg_sendf (state, M_GOT, "%s %li %li",
		   state->in.netname,
		   (long) state->in.size,
		   (long) state->in.time);
        resived_status(state);
	return 1;
      }
      if (inb_test (state->in.netname, state->in.size,
		    state->in.time, state->inbound, realname))
      {
	Log (2, "already have %s (%s, %li byte(s))",
	     state->in.netname, realname, state->in.size);
	msg_sendf (state, M_GOT, "%s %li %li",
		   state->in.netname,
		   (long) state->in.size,
		   (long) state->in.time);
	resived_status(state);
	return 1;
      }
      else if (!state->skip_all_flag)
      {
	if ((state->in.f = inb_fopen (state->in.netname, state->in.size,
				      state->in.time, state->fa, state->nallfa,
				      state->inbound)) == 0)
	{
	  state->skip_all_flag = 1;
	}
      }

		if (state->skip_all_flag)
		{
			Log (2, "skipping %s (non-destructive)", state->in.netname);
			msg_sendf (state, M_SKIP, "%s %li %li",
			state->in.netname,
			(long) state->in.size,
			(long) state->in.time);
			if (state->in.f)
				fclose (state->in.f);
			TF_ZERO (&state->in);
			resived_status(state);
			return 1;
		}
    }

    if (off_req || offset != (size_t) ftell (state->in.f))
    {
		Log(2,"have %li byte(s) of %s",(long) ftell (state->in.f), state->in.netname);
		msg_sendf (state, M_GET, "%s %li %li %li", state->in.netname,
			(long) state->in.size, (long) state->in.time,
			(long) ftell (state->in.f));
		++state->GET_FILE_balance;
		fclose (state->in.f);
		TF_ZERO (&state->in);
		resived_status(state);
		return 1;
    }
    else if (offset != 0 || state->NR_flag >= WE_NR)
    {
      --state->GET_FILE_balance;
    }

    Log(3, "receiving %s (%li byte(s), off %li)",
	 state->in.netname, (long) (state->in.size), (long) offset);

      show_size(tempbuf2,state->in.size);
      modem_add_listbox(L("S_595",state->in.netname,tempbuf2));

    if (fseek (state->in.f, offset, SEEK_SET) == -1)
    {
      Log (1, "fseek: %s", strerror (errno));
     	resived_status(state);
     return 0;
    }
    else
      {	resived_status(state);return 1;}
  }
  else
    {	resived_status(state);return 0;}
}

static int ND_set_status(char *status, FTN_ADDR *fa, STATE *state)
{
  char buf[MAXPATHLEN+1];
  FILE *f;
  int  rc;

  if (state->NR_flag < WE_NR)
    return 1; /* ignoring status if no NR mode */ 
  if (fa->z==-1)
  { Log(8, "ND_set_status: unknown address for '%s'", status);
    return 0;
  }
  Log(4, "Set link status with %u:%u/%u.%u to '%s'",
      fa->z, fa->net, fa->node, fa->p, status ? status : "");
  ftnaddress_to_filename (buf, fa);
  if (*buf=='\0') return 0;
  strnzcat(buf, ".stc", sizeof(buf));
  if (!status || !*status)
    return (unlink(buf) && errno != ENOENT) ? 0 : 1;
  else
  {
    f=fopen(buf, "w");
    if (f==NULL)
    { Log(1, "Can't write to %s: %s", buf, strerror(errno));
      return 0;
    }
    rc=1;
    if (fputs(status, f)==EOF)
      rc=0;
    if (fclose(f)==EOF)
      rc=0;
    return rc;
  }
}

/*
 * M_GET message from the remote: Resend file from offset
 * M_GET args: "%s %li %li %li", filename, size, time, offset
 */
static int GET (STATE *state, char *args, int sz)
{
  const int argc = 4;
  char *argv[4];
  int i, rc = 0;
  size_t offset;

  if (parse_msg_args (argc, argv, args, "M_GET", state))
  {
    /* Check if the file was already sent */
    for (i = 0; i < state->n_sent_fls; ++i)
    {
      if (!tfile_cmp (state->sent_fls + i, argv[0], atol (argv[1]), atol (argv[2])))
      {
	if (state->out.f)
	{
	  fclose (state->out.f);
	  TF_ZERO (&state->out);
	}
	memcpy (&state->out, state->sent_fls + i, sizeof (TFILE));
	state->sent_fls[i].netname[0] = 0;
	if ((state->out.f = fopen (state->out.path, "rb")) == 0)
	{
	  Log (1, "GET: %s: %s", state->out.path, strerror (errno));
	  TF_ZERO (&state->out);
	}
	break;
      }
    }

    if ((state->out.f || state->off_req_sent) &&
         !tfile_cmp (&state->out, argv[0], atol (argv[1]), atol (argv[2])))
    {
      if (!state->out.f)
      { /* response for status */
	rc = 1;
	/* to satisfy remote GET_FILE_ballance */
	msg_sendf (state, M_FILE, "%s %li %li %li", state->out.netname,
	   (long) state->out.size, (long) state->out.time, atol(argv[3]));
	
	if (atol(argv[3])==(long)state->out.size && state->ND_flag == YES_ND)
	{
	  state->send_eof = 1;
	  state->waiting_for_GOT = 1;
	  Log(4, "Waiting for M_GOT");
	  state->off_req_sent = 0;
	  return rc;
	}
	else
	  /* request from offset 0 - file already renamed */
	  ND_set_status("", &state->out_addr, state);
	TF_ZERO(&state->out);
      }
      else if (fseek (state->out.f, offset = atol (argv[3]), SEEK_SET) == -1)
      {
	Log (1, "GET: error seeking %s to %li: %s",
	     argv[0], offset, strerror (errno));
	rc = 0;
      }
      else
      {
	Log (2, "sending %s from %li", argv[0], offset);
	msg_sendf (state, M_FILE, "%s %li %li %li", state->out.netname,
		   (long) state->out.size, (long) state->out.time, offset);
	rc = 1;
      }
    }
    else
      Log (1, "unexpected M_GET for %s", argv[0]);
    ND_set_status("", &state->ND_addr, state);
    state->ND_addr.z=-1;
    if (state->ND_flag == YES_ND)
    {
      state->waiting_for_GOT = 0;
      Log(9, "Don't waiting for M_GOT");
    }
    state->off_req_sent = 0;

    return rc;
  }
  else
    return 0;
}

/*
 * M_SKIP: Remote asks us to skip a file. Only a file currently in
 * transfer will be skipped!
 *
 * M_SKIP args: "%s %li %li", filename, size, time
 */
static int SKIP (STATE *state, char *args, int sz)
{
  const int argc = 3;
  char *argv[3];
  int n;

  if (parse_msg_args (argc, argv, args, "M_SKIP", state))
  {
    for (n = 0; n < state->n_sent_fls; ++n)
    {
      if (!tfile_cmp (state->sent_fls + n, argv[0], atol (argv[1]), atol (argv[2])))
      {
	state->r_skipped_flag = 1;
	Log (2, "%s skipped by remote", state->sent_fls[n].netname);
	remove_from_sent_files_queue (state, n);
      }
    }
    if (state->out.f && !tfile_cmp (&state->out, argv[0],
				    atol (argv[1]), atol (argv[2])))
    {
      state->r_skipped_flag = 1;
      fclose (state->out.f);
      Log (2, "%s skipped by remote", state->out.netname);
      TF_ZERO (&state->out);
    }
    ND_set_status("", &state->ND_addr, state);
    state->ND_addr.z=-1;
    if (state->ND_flag == YES_ND || state->NR_flag >= WE_NR)
    {
      state->waiting_for_GOT = state->off_req_sent = 0;
      Log(9, "Don't waiting for M_GOT");
    }
    return 1;
  }
  else
    return 0;
}

/*
 * M_GOT args: "%s %li %li", filename, size, time
 */
static int GOT (STATE *state, char *args, int sz)
{
  const int argc = 3;
  char *argv[3];
  int n, rc=1;
  char *status = NULL;

  
  if (state->ND_flag == YES_ND)
    status = strdup(args);
  else
    ND_set_status("", &state->ND_addr, state);
  if (parse_msg_args (argc, argv, args, "M_GOT", state))
  {
    if (!tfile_cmp (&state->out, argv[0], atol (argv[1]), atol (argv[2])))
    {
      Log (2, "remote already has %s", state->out.netname);
      if (state->out.f)
        fclose (state->out.f);
      deletesendfile(argv[0]);
      memcpy(&state->ND_addr, &state->out_addr, sizeof(state->out_addr));
      if (state->ND_flag == YES_ND)
        Log (7, "Set ND_addr to %u:%u/%u.%u",
             state->ND_addr.z, state->ND_addr.net, state->ND_addr.node, state->ND_addr.p);
      if (status)
      {
	if (state->off_req_sent)
	  rc = ND_set_status("", &state->ND_addr, state);
	else
	  rc = ND_set_status(status, &state->ND_addr, state);
	state->waiting_for_GOT = state->off_req_sent = 0;
	Log(9, "Don't waiting for M_GOT");
      }
      else if (state->NR_flag >= WE_NR)
	state->off_req_sent = 0;
      remove_from_spool (state, state->out.flo,
			 state->out.path, state->out.action);
      TF_ZERO (&state->out);   deletesendfile(argv[0]);
   
    }
    else
    {
      for (n = 0; n < state->n_sent_fls; ++n)
      {
	if (!tfile_cmp (state->sent_fls + n, argv[0], atol (argv[1]), atol (argv[2])))
	{
	  char szAddr[FTN_ADDR_SZ + 1];

	  ftnaddress_to_str (szAddr, state->fa);
	  state->bytes_sent += state->sent_fls[n].size;
	  ++state->files_sent;
          memcpy(&state->ND_addr, &state->out_addr, sizeof(state->out_addr));
          if (state->ND_flag == YES_ND)
             Log (7, "Set ND_addr to %u:%u/%u.%u",
                  state->ND_addr.z, state->ND_addr.net, state->ND_addr.node, state->ND_addr.p);
	  Log (2, "sent: %s (%li, %.2f CPS, %s)", state->sent_fls[n].path,
	       state->sent_fls[n].size,
	       (double) (state->sent_fls[n].size) /
	       (time (0) == state->sent_fls[n].start ?
		1 : (time (0) - state->sent_fls[n].start)), szAddr);
	
	  if (status)
	  {
	    if (state->off_req_sent)
	      rc = ND_set_status("", &state->ND_addr, state);
	    else
	      rc = ND_set_status(status, &state->ND_addr, state);
	    state->waiting_for_GOT = state->off_req_sent = 0;
	    Log(9, "Don't waiting for M_GOT");
	  }
	  else if (state->NR_flag >= WE_NR)
	    state->off_req_sent = 0;
	  remove_from_spool (state, state->sent_fls[n].flo,
			state->sent_fls[n].path, state->sent_fls[n].action);
	  remove_from_sent_files_queue (state, n);
	  break;		       /* we have ACK for _ONE_ file */
	}
      }
    }
       deletesendfile(argv[0]);
       return rc;
  }
  else
    {   deletesendfile(argv[0]);
   return 0;
	}
}

static int EOB (STATE *state, char *buf, int sz)
{
  state->remote_EOB = 1;
  if (state->in.f)
  {
    fclose (state->in.f);
    Log (1, "receiving of %s interrupted", state->in.netname);
    TF_ZERO (&state->in);
  }
  if ((state->ND_flag == YES_ND) && state->in_complete.f)
  { /* rename complete received file to its true form */
    char realname[MAXPATHLEN + 1];
    char szAddr[FTN_ADDR_SZ + 1];

    fclose (state->in_complete.f);
    if (inb_done (state->in_complete.netname, state->in_complete.size,
                  state->in_complete.time, state->fa, state->nallfa,
	          state->inbound, realname) == 0)
      return 0;
    if (*realname)
    {
      /* Set flags */
      if(evt_test (realname))
        state->q = evt_run (state->q, realname, state->fa, state->nfa,
               state->state == P_SECURE, state->listed_flag,
               state->peer_name, state);
      /* We will run external programs using this list */
      add_to_rcvdlist (&state->rcvdlist, &state->n_rcvdlist, realname);
    }
    ftnaddress_to_str (szAddr, state->fa);
    state->bytes_rcvd += state->in_complete.size;
    ++state->files_rcvd;
    Log (2, "rcvd: %s (%li, %.2f CPS, %s)", state->in_complete.netname,
         (long) state->in_complete.size,
         (double) (state->in_complete.size) /
         (time (0) == state->in_complete.start ?
                      1 : (time (0) - state->in_complete.start)), szAddr);
    TF_ZERO (&state->in_complete);
  }
  return 1;
}

typedef int command (STATE *state, char *buf, int sz);
command *commands[] =
{
  NUL, ADR, PWD, start_file_recv, OK, EOB, GOT, RError, BSY, GET, SKIP
};

/* Recvs next block, processes msgs or writes down the data from the remote */
static int recv_block (STATE *state)
{
  int no;
 //Проверка на выход
  if (abort_bink()) return 0;
 
	  int sz = state->isize == -1 ? BLK_HDR_SIZE : state->isize;

  if (sz == 0)
    no = 0;
  else if ((no = recv_socet (state->s, state->ibuf + state->iread,
		       sz - state->iread, 0)) == SOCKET_ERROR)
  {
    if (TCPERRNO == TCPERR_WOULDBLOCK || TCPERRNO == TCPERR_AGAIN)
    {
      return 1;
    }
    else
    {
      state->io_error = 1;
      Log (1, "recv: %s", TCPERR ());
      if (state->to)
	bad_try (&state->to->fa, TCPERR ());
      return 0;
    }
  }
  if (state->crypt_flag == YES_CRYPT)
    decrypt_buf(state->ibuf + state->iread, no, state->keys_in);
  state->iread += no;
  /* assert (state->iread <= sz); */
  if (state->iread == sz)
  {
    if (state->isize == -1)	       /* reading block header */
    {
      state->imsg = state->ibuf[0] >> 7;
      state->isize = ((((unsigned char *) state->ibuf)[0] & ~0x80) << 8) +
	((unsigned char *) state->ibuf)[1];
      Log (7, "recvd hdr: %i (%s)", state->isize, state->imsg ? "msg" : "data");
      if (state->isize == 0)
	goto DoNotEvenTryToRecvZeroLengthBlock;
    }
    else
    {
  DoNotEvenTryToRecvZeroLengthBlock:
      Log (7, "got block: %i (%s)", state->isize, state->imsg ? "msg" : "data");
      resived_status(state);
      if (state->imsg)
      {
	int rc = 1;

	++state->msgs_in_batch;

	if (state->isize == 0)
	  Log (1, "zero length command from remote (must be at least 1)");
	else if ((unsigned) (state->ibuf[0]) > M_MAX)
	  Log (1, "unknown msg type from remote: %u", state->ibuf[0]);
	else
	{
	  state->ibuf[state->isize] = 0;
          Log (5, "rcvd msg %s %s", scommand[(unsigned char)(state->ibuf[0])], state->ibuf+1);
	  rc = commands[(unsigned) (state->ibuf[0])]
	    (state, state->ibuf + 1, state->isize - 1);
	}

	if (rc == 0)
	  return 0;
      }
      else if (state->in.f)
      {
	if (state->isize != 0 &&
	    fwrite (state->ibuf, state->isize, 1, state->in.f) < 1)
	{
	  Log (1, "write error: %s", strerror(errno));
	  return 0;
	}
	
// Проверка на разрушающий скип
	  if (gc.mailer.skip_file==-1)
    {
			msg_sendf (state, M_GOT, "%s %li %li",state->in.netname,(long) state->in.size,(long) state->in.time); 
			modem_add_listbox(L("S_621",state->in.netname));	// Denying file %s
			gc.mailer.skip_file=0;
    }

 // Проверка на не разрушающий скип
	  if (gc.mailer.skip_file==1)
    {
			msg_sendf (state, M_SKIP, "%s %li %li",state->in.netname,(long) state->in.size,(long) state->in.time); 
			modem_add_listbox(L("S_622",state->in.netname));	// Skipping file %s
			gc.mailer.skip_file=0;
    }

	if (percents && state->in.size > 0)
	{
	  printf ("%-20.20s %3.0f%%\r", state->in.netname,
		  100.0 * ftell (state->in.f) / (float) state->in.size);
	  fflush (stdout);
	}
	if ((size_t) ftell (state->in.f) == state->in.size)
	{
	  char szAddr[FTN_ADDR_SZ + 1];
	  char realname[MAXPATHLEN + 1];

	  if (state->ND_flag == YES_ND)
	  {
	    Log (5, "File %s complete received, waiting for renaming",
	         state->in.netname);
	    memcpy(&state->in_complete, &state->in, sizeof(state->in_complete));
	  }
	  else
	  {
	    fclose (state->in.f);
	    if (inb_done (state->in.netname, state->in.size,
		          state->in.time, state->fa, state->nallfa,
		          state->inbound, realname) == 0)
              return 0;
	    if (*realname)
	    {
	      /* Set flags */
              if (evt_test (realname))
                state->q = evt_run (state->q, realname, state->fa, state->nfa,
                       state->state == P_SECURE, state->listed_flag,
                       state->peer_name, state);
	      /* We will run external programs using this list */
	      add_to_rcvdlist (&state->rcvdlist, &state->n_rcvdlist, realname);
	    }
	    ftnaddress_to_str (szAddr, state->fa);
	    state->bytes_rcvd += state->in.size;
	    ++state->files_rcvd;
	    Log (2, "rcvd: %s (%li, %.2f CPS, %s)", state->in.netname,
	         (long) state->in.size,
	         (double) (state->in.size) /
	         (time (0) == state->in.start ?
		  1 : (time (0) - state->in.start)), szAddr);
        
	  }
	  msg_sendf (state, M_GOT, "%s %li %li",
		     state->in.netname,
		     (long) state->in.size,
		     (long) state->in.time);
	  TF_ZERO (&state->in);
	}
	else if ((size_t) ftell (state->in.f) > state->in.size)
	{
	  Log (1, "rcvd %li extra bytes!", (long) ftell (state->in.f) - state->in.size);
	  resived_status(state);
	}
      }
      else if (state->isize > 0)
      {
	Log (7, "ignoring data block (%li byte(s))", (long) state->isize);
      }
      state->isize = -1;
    }
    state->iread = 0;
  }
  if (no == 0 && sz > 0)
  {
    state->io_error = 1;
    Log (1, "recv: connection closed by foreign host");
    return 0;
  }
  else
    return 1;
}


static int start_file_transfer (STATE *state, FTNQ *file)
{
  struct stat sb;
  FILE *f;
  int action = -1;
  char tempbuf2[255];

  if (state->out.f)
    fclose (state->out.f);
  TF_ZERO (&state->out);	       /* No file in transfer */

  if (state->flo.f == 0)	       /* There is no open .?lo */
  {
    state->maxflvr = MAXFLVR (state->maxflvr, file->flvr);
    /* Try to open the suggested file */
    if (file->type == 's')
    {
      sb.st_size  = file->size;
      sb.st_ctime = sb.st_mtime = file->time;
      f = NULL;
    }
    else
    {
      if ((f = fopen (file->path, (file->type == 'l') ? "r+b" : "rb")) == 0 ||
	  fstat (fileno (f), &sb) == -1)
      {
        Log (1, "%s: cannot open: %s", file->path, strerror (errno));
        return 0;
      }
      /* We've opened a .?lo */
      if (file->type == 'l')
      {
        state->flo.action = file->action;
        strcpy (state->flo.path, file->path);
        state->flo.f = f;
      }
    }
    memcpy(&state->out_addr, &file->fa, sizeof(state->out_addr));
    if (state->ND_flag != YES_ND)
      memcpy(&state->ND_addr, &file->fa, sizeof(state->out_addr));
    Log (8, "cur remote addr is %u:%u/%u.%u",
         file->fa.z, file->fa.net, file->fa.node, file->fa.p);
  }
  if (state->flo.f != 0)
  {
    strcpy (state->out.flo, state->flo.path);
    while (1)
    {
      char *w;
       //Проверка на выход
	  if (abort_bink()) return 0;

      if (!read_flo_line (state->out.path, &action, state->flo.f))
      {
	fclose (state->flo.f);
	state->flo.f = 0;
	/* .?lo closed, remove_from_spool() will now unlink it */
	remove_from_spool (state, state->flo.path, 0, 0);
	TF_ZERO (&state->flo);
	return 0;
      }

      if ((w = trans_flo_line (state->out.path)) != 0)
	Log (5, "%s mapped to %s", state->out.path, w);

      if ((f = fopen (w ? w : state->out.path, "rb")) == 0 ||
	  fstat (fileno (f), &sb) == -1 ||
	  (sb.st_mode & S_IFDIR) != 0)
      {
	Log (1, "start_file_transfer: %s: %s",
	     w ? w : state->out.path, strerror (errno));
        if (f) fclose(f);
	if (w) free (w);
	remove_from_spool (state, state->out.flo,
			   state->out.path, state->out.action);
      }
      else
      {
	if (w) free (w);
	break;
      }
    }
  }

  if (action == -1)
  {
    strcpy (state->out.path, file->path);
    state->out.flo[0] = 0;
    state->out.action = file->action;
    state->out.type = file->type;
  }
  else
  {
    state->out.action = action;
    state->out.type = 0;
  }
  state->out.f = f;
  state->out.size = sb.st_size;
  state->out.time = sb.st_mtime;
  state->waiting_for_GOT = 0;
  Log(9, "Dont waiting for M_GOT");
  state->out.start = time (0);
  netname (state->out.netname, &state->out);

  show_size(tempbuf2,state->out.size);
  modem_add_listbox(L("S_129",state->out.netname,tempbuf2));	// >> File: %s %s

  Log (3, "sending %s as %s (%li)",
       state->out.path, state->out.netname, (long) state->out.size);

  if (state->NR_flag >= WE_NR)
  {
    msg_sendf (state, M_FILE, "%s %li %li -1",
	       state->out.netname, (long) state->out.size,
	       (long) state->out.time);
    state->off_req_sent = 1;
  }
  else
    msg_sendf (state, M_FILE, "%s %li %li 0",
	       state->out.netname, (long) state->out.size,
	       (long) state->out.time);

  return 1;
}

void log_end_of_session (char *status, STATE *state)
{
  char szFTNAddr[FTN_ADDR_SZ + 1];

  BinLogStat (status, state);

  if (state->to)
    ftnaddress_to_str (szFTNAddr, &state->to->fa);
  else if (state->fa)
    ftnaddress_to_str (szFTNAddr, state->fa);
  else
    strcpy (szFTNAddr, "?");

  Log (2, "done (%s%s, %s, S/R: %i/%i (%lu/%lu bytes))",
       state->to ? "to " : (state->fa ? "from " : ""), szFTNAddr,
       status,
       state->files_sent, state->files_rcvd,
       state->bytes_sent, state->bytes_rcvd);
}

static void banner (STATE *state)
{
  char szFTNAddr[FTN_ADDR_SZ + 1];
  char *szAkas;
  int i, tz;
  char szLocalTime[60];
  time_t t, gt;
  struct tm *tm;
  char *dayweek[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
  char *month[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                   "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

  if ((!no_MD5) && (!state->to) &&
      ((state->MD_challenge=MD_getChallenge(NULL, state))!=NULL))
  {  /* Answering side MUST send CRAM message as a very first M_NUL */
    char s[MD5_DIGEST_LEN*2+15]; /* max. length of opt string */
    strcpy(s, "OPT ");
    MD_toString(s+4, state->MD_challenge[0], (unsigned char *) state->MD_challenge+1);
    state->MD_flag=1;
    msg_send2 (state, M_NUL, s, "");
  }
  else
    state->MD_flag=0;

  msg_send2 (state, M_NUL, "SYS ", sysname);
  msg_send2 (state, M_NUL, "ZYZ ", sysop);
  msg_send2 (state, M_NUL, "LOC ", location);
  msg_send2 (state, M_NUL, "NDL ", nodeinfo);

  tzset();
  time (&t);
  tm = gmtime (&t);
  tm->tm_isdst = 0;
  gt = mktime(tm);
  tm = localtime (&t);
  tm->tm_isdst = 0;
  tz = (int)(((long)mktime(tm)-(long)gt)/60);
  tm = localtime (&t);

  sprintf (szLocalTime, "%s, %2d %s %d %02d:%02d:%02d %c%02d%02d",
           dayweek[tm->tm_wday], tm->tm_mday, month[tm->tm_mon],
           tm->tm_year+1900, tm->tm_hour, tm->tm_min, tm->tm_sec,
           (tz>=0) ? '+' : '-', abs(tz)/60, abs(tz)%60);

  msg_send2 (state, M_NUL, "TIME ", szLocalTime);

  msg_sendf (state, M_NUL,
    "VER " MYNAME "/" MYVER "%s " PRTCLNAME "/" PRTCLVER, get_os_string ());

  szAkas = (char*) xalloc (nAddr * (FTN_ADDR_SZ + 1));
  *szAkas = 0;
  for (i = 0; i < nAddr; ++i)
  {
    ftnaddress_to_str (szFTNAddr, pAddr + i);
    strcat (szAkas, " ");
    strcat (szAkas, szFTNAddr);
  }
  msg_send2 (state, M_ADR, szAkas, 0);

  if (state->NR_flag == WANT_NR ||
      (state->crypt_flag & WE_CRYPT) ||
      (state->ND_flag & WE_ND))
    msg_sendf (state, M_NUL, "OPT%s%s%s",
               state->NR_flag == WANT_NR ? " NR" : "",
               (state->ND_flag & WE_ND) ? " ND" : "",
               (state->crypt_flag & WE_CRYPT) ? " CRYPT" : "");
  free (szAkas);
}

void protocol (SOCKET socket, FTN_NODE *to)
{
  extern int n_servers;
  STATE state;
  struct timeval tv;
  fd_set r, w;
  int no;
  struct sockaddr_in peer_name;
  int peer_name_len = sizeof (peer_name);
  char host[MAXHOSTNAMELEN + 1];
  char sbuf[255];

	sprintf(sbuf,L("S_623"));	// == Start BinkP session
	modem_add_listbox(sbuf);

  if (!init_protocol (&state, socket, to))
    return;

  if((to)&&(to->current_addr)) peer_name.sin_addr.s_addr=to->current_addr;
  else
  if (getpeername (socket, (struct sockaddr *) & peer_name, &peer_name_len) == -1)
    Log (1, "getpeername: %s", TCPERR ());

  LockSem(&hostsem);
  get_hostname(&peer_name, host, sizeof(host));
  state.peer_name = host;
 
  modem_add_listbox(L("S_624",state.peer_name,inet_ntoa(peer_name.sin_addr)));	// session with %s (%s)

  Log (2, "session with %s (%s)",
       state.peer_name,
       inet_ntoa (peer_name.sin_addr));
  ReleaseSem(&hostsem);

  if (getsockname (socket, (struct sockaddr *) & peer_name, &peer_name_len) == -1)
    Log (1, "getpeername: %s", TCPERR ());
  else 
    state.our_ip=peer_name.sin_addr.s_addr;

  banner (&state);
    
  if (n_servers > max_servers && !to)
  {
    Log (1, "too many servers");
    msg_send2 (&state, M_BSY, "Too many servers", 0);
  }
  else
  {
    while (1)
    {
      /* If the queue is not empty and there is no file in tranafer */
      if (!state.local_EOB && state.q && state.out.f == 0 &&
          !state.waiting_for_GOT && !state.off_req_sent)
      {
	FTNQ *q;
    //Проверка на выход
	if (abort_bink()) break;

	while (1)
	{			       /* Next .pkt, .flo or a file */
    //Проверка на выход
	if (abort_bink()) break;

	  q = 0;
	  if (state.flo.f ||
	      (q = select_next_file (state.q, state.fa, state.nfa)) != 0)
	  {
	    if (q && (q->type=='s') && (state.NR_flag < WE_NR))
	    { /* TODO: wait for send queue and switch to NR mode */
	      Log(1, "WARNING: status present and no NR mode!");
	      continue;
	    }
	    if (start_file_transfer (&state, q))
	      break;
	  }
	  else
	  {
	    q_free (state.q);
	    state.q = 0;
	    break;
	  }
	}
      }

      /* No more files to send in this batch, so send EOB */
      if (!state.q && !state.local_EOB && state.state != P_NULL && state.sent_fls == 0)
      {
	state.local_EOB = 1;
	msg_send2 (&state, M_EOB, 0, 0);
      }

      FD_ZERO (&r);
      FD_ZERO (&w);
      FD_SET (socket, &r);
      if (state.msgs ||
          (state.out.f && !state.off_req_sent && !state.waiting_for_GOT) ||
          state.oleft || state.send_eof)
	FD_SET (socket, &w);

      if (state.remote_EOB && state.sent_fls == 0 && state.local_EOB &&
	  state.GET_FILE_balance == 0 && state.in.f == 0 && state.out.f == 0)
      {
	/* End of the current batch */
	if (state.rcvdlist)
	{
	  state.q = process_rcvdlist (&state, state.q);
	  q_to_killlist (&state.killlist, &state.n_killlist, state.q);
	  free_rcvdlist (&state.rcvdlist, &state.n_rcvdlist);
	}
	Log (6, "there were %i msgs in this batch", state.msgs_in_batch);
	if (state.msgs_in_batch <= 2 || (state.major * 100 + state.minor <= 100))
	{
          ND_set_status("", &state.ND_addr, &state);
          state.ND_addr.z=-1;
	  break;
	}
	else
	{
	  /* Start the next batch */
	  state.msgs_in_batch = 0;
	  state.remote_EOB = state.local_EOB = 0;
	  if (OK_SEND_FILES (&state))
	    state.q = q_scan_boxes (state.q, state.fa, state.nfa);
	  continue;
	}
      }

      tv.tv_sec = nettimeout;	       /* Set up timeout for select() */
      tv.tv_usec = 0;
      Log (8, "tv.tv_sec=%li, tv.tv_usec=%li", (long) tv.tv_sec, (long) tv.tv_usec);
      no = select (socket + 1, &r, &w, 0, &tv);
      Log (8, "selected %i (r=%i, w=%i)", no, FD_ISSET (socket, &r), FD_ISSET (socket, &w));
      bsy_touch ();		       /* touch *.bsy's */
      if (no == 0)
      {
	state.io_error = 1;
	Log (1, "timeout!");
	if (to)
	  bad_try (&to->fa, "Timeout!");
	break;
      }
      else if (no < 0)
      {
	state.io_error = 1;
	Log (1, "select: %s (args: %i %i)", TCPERR (), socket, tv.tv_sec);
	if (to)
	  bad_try (&to->fa, TCPERR ());
	break;
      }
      if (FD_ISSET (socket, &r))       /* Have something to read */
      {
	if (!recv_block (&state))
	  break;
      }
      if (FD_ISSET (socket, &w))       /* Clear to send */
      {
	if (!send_block (&state))
	  break;
      }
    }
  }

  /* Still have something to send */
  //Проверка на выход

  while (!state.io_error &&
	 (state.msgs || (state.optr && state.oleft)) && send_block (&state))
	 if (gc.mailer.keyboard_break==1) break;

  if (state.local_EOB && state.remote_EOB && state.sent_fls == 0 &&
      state.GET_FILE_balance == 0 && state.in.f == 0 && state.out.f == 0)
  {
    /* Successful session */
    log_end_of_session ("OK", &state);
    process_killlist (state.killlist, state.n_killlist, 's');
    if (to)
      good_try (&to->fa, "CONNECT/BND");
  }
  else
  {
    /* Unsuccessful session */
    log_end_of_session ("failed", &state);
    process_killlist (state.killlist, state.n_killlist, 0);
    if (to)
    {
      /* We called and there were still files in transfer -- restore poll */
      if (tolower (state.maxflvr) != 'h')
      {
	Log (4, "restoring poll with `%c' flavour", state.maxflvr);
	create_poll (&state.to->fa, state.maxflvr);
      }
    }
  }

  if (to && state.r_skipped_flag && hold_skipped > 0)
  {
    Log (2, "holding skipped mail for %li sec", (long) hold_skipped);
    hold_node (&to->fa, time (0) + hold_skipped);
  }

  Log (4, "session closed, quitting...");
  deinit_protocol (&state);
  modem_add_listbox(L("S_625"));	// == End BinkP session
  
}
