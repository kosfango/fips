
#include "stdafx.h"
#include <sys/stat.h>
#include "readcfg.h"
#include "srif.h"
#include "iptools.h"
#include "readflo.h"

static char *path;
static int line;

char siport[MAXSERVNAME + 1] = "";
char soport[MAXSERVNAME + 1] = "";
int havedefnode=0;
int iport = 0;
int oport = 0;
int call_delay = 60;
int rescan_delay = 60;
int nettimeout = DEF_TIMEOUT;
int oblksize = DEF_BLKSIZE;
int max_servers = 100;
int max_clients = 100;
int kill_dup_partial_files = 0;
int kill_old_partial_files = 0;
int kill_old_bsy = 0;
int percents = 0;
int minfree = -1;
int minfree_nonsecure = -1;
int debugcfg = 0;
int printq = 0;
int backresolv = 0;
char sysname[MAXSYSTEMNAME + 1] = "";
char sysop[MAXSYSOPNAME + 1] = "";
char location[MAXLOCATIONNAME + 1] = "";
char nodeinfo[MAXNODEINFO + 1] = "";
char inbound[MAXPATHLEN + 1] = ".";
char inbound_nonsecure[MAXPATHLEN + 1] = "";
char temp_inbound[MAXPATHLEN + 1] = "";
#ifdef MAILBOX
/* FileBoxes dir */
char tfilebox[MAXPATHLEN + 1] = "";
/* BrakeBoxes dir */
char bfilebox[MAXPATHLEN + 1] = "";
int  deleteablebox = 0;
#endif
char logpath[MAXPATHLEN + 1] = "";
char binlogpath[MAXPATHLEN + 1] = "";
char fdinhist[MAXPATHLEN + 1] = "";
char fdouthist[MAXPATHLEN + 1] = "";
char pid_file[MAXPATHLEN + 1] = "";
char proxy[MAXHOSTNAMELEN + 40] = "";
char socks[MAXHOSTNAMELEN + 40] = "";
int loglevel = 4;
int conlog = 0;
int send_if_pwd = 0;
int tzoff = 0;
char root_domain[MAXHOSTNAMELEN + 1] = "fidonet.net.";
int prescan = 0;
int connect_timeout = 0;
struct conflist_type *config_list = NULL;

#if defined (HAVE_VSYSLOG) && defined (HAVE_FACILITYNAMES)

int syslog_facility = -1;

#endif

int tries = 0;
int hold = 0;
int hold_skipped = 60 * 60;

int nAddr = 0;
FTN_ADDR *pAddr = 0;

extern FTN_DOMAIN *pDomains;
extern int no_MD5;
int id_boss=0;



static void read_int (KEYWORD *, char *);
static void read_string (KEYWORD *, char *);
static void read_bool (KEYWORD *, char *);
static void read_flag_exec_info (KEYWORD *, char *);
static void read_rfrule (KEYWORD *, char *);
static void read_skipmask (KEYWORD *key, char *s);
void skipmask_add(char *mask);

#if defined (HAVE_VSYSLOG) && defined (HAVE_FACILITYNAMES)

static void read_syslog_facility (KEYWORD *, char *);

#endif

#define DONT_CHECK 0x7fffffff

KEYWORD keywords[] =
{
  {"log", read_string, logpath, 'f', 0},
  {"loglevel", read_int, &loglevel, 0, DONT_CHECK},
  {"conlog", read_int, &conlog, 0, DONT_CHECK},
  {"binlog", read_string, binlogpath, 'f', 0},
  {"fdinhist", read_string, fdinhist, 'f', 0},
  {"fdouthist", read_string, fdouthist, 'f', 0},
  {"tzoff", read_int, &tzoff, DONT_CHECK, DONT_CHECK},
  {"domain", read_domain_info, NULL, 0, 0},
  {"address", read_aka_list, NULL, 0, 0},
  {"sysname", read_string, sysname, 0, MAXSYSTEMNAME},
  {"sysop", read_string, sysop, 0, MAXSYSOPNAME},
  {"location", read_string, location, 0, MAXLOCATIONNAME},
  {"nodeinfo", read_string, nodeinfo, 0, MAXNODEINFO},
  {"iport", read_string, siport, 0, MAXSERVNAME},
  {"oport", read_string, soport, 0, MAXSERVNAME},
  {"rescan-delay", read_int, &rescan_delay, 1, DONT_CHECK},
  {"call-delay", read_int, &call_delay, 1, DONT_CHECK},
  {"timeout", read_int, &nettimeout, 1, DONT_CHECK},
  {"oblksize", read_int, &oblksize, MIN_BLKSIZE, MAX_BLKSIZE},
  {"maxservers", read_int, &max_servers, 0, DONT_CHECK},
  {"maxclients", read_int, &max_clients, 0, DONT_CHECK},
  {"inbound", read_string, inbound, 'd', 0},
  {"inbound-nonsecure", read_string, inbound_nonsecure, 'd', 0},
  {"temp-inbound", read_string, temp_inbound, 'd', 0},
  {"node", read_node_info, NULL, 0, 0},
  {"defnode", read_node_info, NULL, 1, 0},
  {"kill-dup-partial-files", read_bool, &kill_dup_partial_files, 0, 0},
  {"kill-old-partial-files", read_int, &kill_old_partial_files, 1, DONT_CHECK},
  {"kill-old-bsy", read_int, &kill_old_bsy, 1, DONT_CHECK},
  {"percents", read_bool, &percents, 0, 0},
  {"minfree", read_int, &minfree, 0, DONT_CHECK},
  {"minfree-nonsecure", read_int, &minfree_nonsecure, 0, DONT_CHECK},
  {"flag", read_flag_exec_info, NULL, 'f', 0},
  {"exec", read_flag_exec_info, NULL, 'e', 0},
  {"debugcfg", read_bool, &debugcfg, 0, 0},
  {"printq", read_bool, &printq, 0, 0},
  {"try", read_int, &tries, 0, 0xffff},
  {"hold", read_int, &hold, 0, DONT_CHECK},
  {"hold-skipped", read_int, &hold_skipped, 0, DONT_CHECK},
  {"backresolv", read_bool, &backresolv, 0, 0},
  {"pid-file", read_string, pid_file, 'f', 0},
  {"proxy", read_string, proxy, 0, MAXHOSTNAMELEN + 40},
  {"socks", read_string, socks, 0, MAXHOSTNAMELEN + 40},
  {"ftrans", read_rfrule, NULL, 0, 0},
  {"send-if-pwd", read_bool, &send_if_pwd, 0, 0},
  {"root-domain", read_string, root_domain, 0, MAXHOSTNAMELEN},
  {"prescan", read_bool, &prescan, 0, 0},
  {"connect-timeout", read_int, &connect_timeout, 0, DONT_CHECK},
  {"skipmask", read_skipmask, NULL, 0, 0},
  {NULL, NULL, NULL, 0, 0}
};

#define TEST(var) if (!*var) Log (0, "%s: "#var" should be defined", path)
 

/*
 * Parses and reads _path as config.file
 */
void readcfg (void)
{
  //md5 �����������
  no_MD5=get_cfg_bink(id_boss,"No_MD5",0);

  //������ ���������
  strcpy(nodeinfo,get_cfg_bink(id_boss,"nodeinfo","115200,TCP,BINKP"));
  call_delay=get_cfg_bink(id_boss,"call-delay",5);
  rescan_delay=get_cfg_bink(id_boss,"rescan-delay",120); 
  max_clients=get_cfg_bink(id_boss,"maxclients",3); 
  tries=get_cfg_bink(id_boss,"try",10); 
  hold=get_cfg_bink(id_boss,"hold",600); 
  loglevel=get_cfg_bink(id_boss,"loglevel",7); 
  conlog=get_cfg_bink(id_boss,"conlog",4); 
  percents=get_cfg_bink(id_boss,"percents",1); 
  printq=get_cfg_bink(id_boss,"printq",1); 
  kill_dup_partial_files=get_cfg_bink(id_boss,"kill-dup-partial-files",0); 
  kill_old_partial_files=get_cfg_bink(id_boss,"kill-old-partial-files",86400); 
  kill_old_bsy=get_cfg_bink(id_boss,"kill-old-bsy",43200); 
  send_if_pwd=get_cfg_bink(id_boss,"send-if-pwd",0); 
  minfree_nonsecure=get_cfg_bink(id_boss,"minfree-nonsecure",2048); 
  minfree=get_cfg_bink(id_boss,"minfree",1024); 
  tzoff=get_cfg_bink(id_boss,"tzoff",10800); 
  
  backresolv=get_cfg_bink(id_boss,"backresolv",1); 
  strcpy(logpath,get_cfg_bink(id_boss,"log","binkd.log")); 
  
  //��������� ������ �������
  strcpy(proxy,get_cfg_bink(id_boss,"proxy","")); 
  strcpy(socks,get_cfg_bink(id_boss,"socks","")); 
  
  //��������� ����� �����
  strcpy(siport,get_cfg_bink(id_boss,"iport","")); 
  strcpy(soport,get_cfg_bink(id_boss,"oport","")); 
  iport = find_port (siport);
  oport = find_port (soport);
  
	    

  TEST (sysname);
  TEST (sysop);
  TEST (location);
  TEST (nodeinfo);

}


/*
 *  METHODS TO PROCESS KEYWORDS' ARGUMETS
 */


void read_aka_list (KEYWORD *key, char *s)
{
  int i;
  char *w;

  for (i = 1; (w = getword (s, i + 1)) != 0; ++i)
  {
    pAddr = (struct _FTN_ADDR *) xrealloc (pAddr, sizeof (FTN_ADDR) * (nAddr + 1));
    if (!parse_ftnaddress (w, pAddr + nAddr))
    {
      Log (0, "%s: %i: %s: the address cannot be parsed", path, line, w);
    }
    if (!is4D (pAddr + nAddr))
    {
      Log (0, "%s: %i: %s: must be at least a 4D address", path, line, w);
    }
    if (!pAddr[nAddr].domain[0])
    {
      if (!pDomains)
	Log (0, "%s: %i: at least one domain must be defined first", path, line);
      strcpy (pAddr[nAddr].domain, get_def_domain ()->name);
    }
    ++nAddr;
    free (w);
  }
}

void read_domain_info (KEYWORD *key, char *s)
{
  char *w1 = getword (s, 2);
  char *w2 = getword (s, 3);
  char *w3 = getword (s, 4);
  FTN_DOMAIN *new_domain;

  if (!w1 || !w2 || !w3)
    Log (0, "%s: %i: domain: not enough args", path, line);

  if (get_domain_info (w1) == 0)
  {
    new_domain = (struct _FTN_DOMAIN *) xalloc (sizeof (FTN_DOMAIN));
    strnzcpy (new_domain->name, w1, sizeof (new_domain->name));
    if (!STRICMP (w2, "alias-for"))
    {
      FTN_DOMAIN *tmp_domain;

      if ((tmp_domain = get_domain_info (w3)) == 0)
	Log (0, "%s: %i: %s: undefined domain", path, line, w3);
      else
	new_domain->alias4 = tmp_domain;
      free (w2);
    }
    else
    {
      char *s;
      int z;

      if ((z = atoi (w3)) <= 0)
	Log (0, "%s: %i: invalid zone", path, line);

      new_domain->z = (int *) xalloc (sizeof (int) * 2);
      new_domain->z[0] = z;
      new_domain->z[1] = 0;
      new_domain->alias4 = 0;

      for (s = w2 + strlen (w2) - 1; (*s == '/' || *s == '\\') && s >= w2; --s)
	*s = 0;
      if ((s = max (strrchr (w2, '\\'), strrchr (w2, '/'))) == 0)
      {
	new_domain->dir = w2;
	new_domain->path = (char*) xstrdup (".");
      }
      else
      {
	new_domain->dir = (char*) xstrdup (s + 1);
	for (; *s == '/' || *s == '\\'; --s)
	  *s = 0;
	new_domain->path = w2;
      }
      if (strchr (new_domain->dir, '.'))
	Log (0, "%s: %i: there should be no extension for "
	     "the base outbound name", path, line);
    }
    new_domain->next = pDomains;
    pDomains = new_domain;
  }
  else
  {
    Log (0, "%s: %i: %s: duplicate domain", path, line, w1);
  }
  free (w1);
  free (w3);
}

static void check_dir_path (char *s)
{
  if (s)
  {
    char *w = s + strlen (s) - 1;

    while (w >= s && (*w == '/' || *w == '\\'))
      *(w--) = 0;
  }
}

void read_node_info (KEYWORD *key, char *s)
{
#define ARGNUM 6
  char *w[ARGNUM], *tmp;
  int i, j, NR_flag = NR_USE_OLD, ND_flag = ND_USE_OLD;
  int MD_flag = 0, crypt_flag = CRYPT_USE_OLD, restrictIP = 0; 
  FTN_ADDR fa;

  memset (w, 0, sizeof (w));
  i = 0;			       /* index in w[] */
  j = 2;			       /* number of word in the source string */
  
  /*if(key->option1) 
  {
 	  w[i++]= (char*) xstrdup("0:0/0.0@defnode");
	  havedefnode=1;
  } */

  while (1)
  {
    if ((tmp = getword (s, j++)) == NULL)
      break;

    if (tmp[0] == '-')
    {
      if (tmp[1] != '\0')
      {
        if (STRICMP (tmp, "-md") == 0)
          MD_flag = 1;
        else if (STRICMP (tmp, "-nomd") == 0)
          MD_flag = (-1);
        else if (STRICMP (tmp, "-nr") == 0)
	  NR_flag = NR_ON;
	else if (STRICMP (tmp, "-nd") == 0)
	{
	  NR_flag = NR_ON;
	  ND_flag = ND_ON;
	}
	else if (STRICMP (tmp, "-ip") == 0)
	  restrictIP = 1;
	else if (STRICMP (tmp, "-crypt") == 0)
	  crypt_flag = CRYPT_ON;
	else
	  Log (0, "%s: %i: %s: unknown option for `node' keyword", path, line, tmp);
      }
      else
      {
	/* Process "-": skip w[i]. Let it be filled with default NULL */
	++i;
      }
    }
    else if (i >= ARGNUM)
      Log (0, "%s: %i: too many argumets for `node' keyword", path, line);
    else
      w[i++] = tmp;
  }

  if (!parse_ftnaddress (w[0], &fa))
    Log (0, "%s: %i: %s: the address cannot be parsed", path, line, w[0]);
  else
    exp_ftnaddress (&fa);

  if (w[2] && w[2][0] == 0)
    Log (0, "%s: %i: empty password", path, line);
  if (w[3] && w[3][0] != '-' && !isflvr (w[3][0]))
    Log (0, "%s: %i: %s: incorrect flavour", path, line, w[3]);
  check_dir_path (w[4]);
  check_dir_path (w[5]);

  if (!add_node (&fa, w[1], w[2], (char)(w[3] ? w[3][0] : '-'), w[4], w[5],
		 NR_flag, ND_flag, crypt_flag, MD_flag, restrictIP))
    Log (0, "%s: add_node() failed", w[0]);

  for (i = 0; i < ARGNUM; ++i)
    if (w[i])
      free (w[i]);

#undef ARGNUM
}

/*
 *  Gets hostname/portnumber for ``n''-th host in ``src'' string (1 ... N)
 *    <src> = <host> [ "," <src> ]
 *    <host> = "*"
 *    <host> = <hostname> [ ":" <service> ]
 *
 *  "*" will expand in corresponding domain name for ``fn''
 *                        (2:5047/13 --> "f13.n5047.z2.fidonet.net.")
 *
 *  ``Host'' should contain at least MAXHOSTNAMELEN bytes.
 *
 *  Returns 0 on error, -1 on EOF, 1 otherwise
 */
int get_host_and_port (int n, char *host, unsigned short *port, char *src, FTN_ADDR *fa)
{
  int rc = 0;
  char *s = getwordx2 (src, n, 0, ",;", "");

  if (s)
  {
    char *t = strchr (s, ':');

    if (t)
      *t = 0;

    if (!strcmp (s, "*"))
      ftnaddress_to_domain (host, fa);
    else
      strnzcpy (host, s, MAXHOSTNAMELEN);

    if (!t)
    {
      *port = oport;
      rc = 1;
    }
    else if ((*port = find_port (t + 1)) != 0)
      rc = 1;

    free (s);
  }
  else
    rc = -1;
  return rc;
}

/*
 * Read a string (key->option1 == 0)
 * or a directory name (key->option1 == 'd')
 * or a file name (key->option1 == 'f')
 */
static void read_string (KEYWORD *key, char *s)
{
  struct stat sb;
  char *target = (char *) (key->var);
  char *w;

  if ((w = getword (s, 2)) == NULL)
    Log (0, "%s: %i: missing an argument for `%s'", path, line, key->key);

  if (getword (s, 3) != NULL)
    Log (0, "%s: %i: extra arguments for `%s'", path, line, key->key);

  strnzcpy (target, w, key->option1 == 0 ? key->option2 : MAXPATHLEN);
  free (w);

  if (key->option1 != 0)
  {
    w = target + strlen (target) - 1;
    while (w >= target && (*w == '/' || *w == '\\'))
    {
      if (key->option1 == 'f')
      {
	Log (0, "%s: %i: unexpected `%c' at the end of filename",
	     path, line, *w);
      }
      *(w--) = 0;
    }
    if (key->option1 == 'd' && (stat (target, &sb) == -1 ||
				!(sb.st_mode & S_IFDIR)))
    {
      Log (0, "%s: %i: %s: incorrect directory", path, line, target);
    }
  }
}

static void read_int (KEYWORD *key, char *s)
{
  int *target = (int *) (key->var);
  char *w;

  if ((w = getword (s, 2)) == NULL)
    Log (0, "%s: %i: missing an argument for `%s'", path, line, key->key);

  if (getword (s, 3) != NULL)
    Log (0, "%s: %i: extra arguments for `%s'", path, line, key->key);

  *target = atoi (w);
  free (w);

  if ((key->option1 != DONT_CHECK && *target < key->option1) ||
      (key->option2 != DONT_CHECK && *target > key->option2))
    Log (0, "%s: %i: %i: incorrect value", path, line, *target);
}

#if defined (HAVE_VSYSLOG) && defined (HAVE_FACILITYNAMES)
static void read_syslog_facility (KEYWORD *key, char *s)
{
  int *target = (int *) (key->var);
  char *w;

  if ((w = getword (s, 2)) != 0 && getword (s, 3) == 0)
  {
    int i;

    for (i = 0; facilitynames[i].c_name; ++i)
      if (!strcmp (facilitynames[i].c_name, w))
	break;

    if (facilitynames[i].c_name == 0)
      Log (0, "%s: %i: %s: incorrect facility name", path, line, w);
    *target = facilitynames[i].c_val;
    free (w);
  }
  else
    Log (0, "%s: %i: the syntax is incorrect", path, line);
}
#endif

static void read_rfrule (KEYWORD *key, char *s)
{
  char *w1, *w2;

  if ((w1 = getword (s, 2)) != 0 &&
      (w2 = getword (s, 3)) != 0 &&
      getword (s, 4) == 0)
  {
    rf_rule_add (w1, w2);
  }
  else
    Log (0, "%s: %i: the syntax is incorrect", path, line);
}

static void read_skipmask (KEYWORD *key, char *s)
{
  char *w;
  int i;

  for (i=2; (w = getword (s, i)) != NULL; i++)
    skipmask_add (w);
  if (i == 2)
    Log (0, "%s: %i: the syntax is incorrect", path, line);
}

static void read_bool (KEYWORD *key, char *s)
{
  if (getword (s, 2) == 0)
  {
    *(int *) (key->var) = 1;
  }
  else
    Log (0, "%s: %i: the syntax is incorrect", path, line);
}

static void read_flag_exec_info (KEYWORD *key, char *s)
{
  EVT_FLAG *tmp;
  char *path, *w;
  int i;
  static EVT_FLAG *last = 0;

  if ((path = getword (s, 2)) == 0)
    Log (0, "%s: %i: the syntax is incorrect", path, line);
  for (i = 2; (w = getword (s, i + 1)) != 0; ++i)
  {
    tmp = (struct _EVT_FLAG *) xalloc (sizeof (EVT_FLAG));
    memset (tmp, 0, sizeof (EVT_FLAG));
    if (key->option1 == 'f' && *path == '!')
    {
      tmp->imm = 1;		       /* Immediate flag */
      tmp->path = path + 1;
    }
    else
    {
      tmp->imm = 0;
      tmp->path = path;
    }
    if (key->option1 == 'e')
    {
      if(*path == '!')
      {
        tmp->imm = 1;		       /* Immediate flag */
        tmp->command = path + 1;
      }
      else
      tmp->command = path;
      tmp->path = 0;
    }
    tmp->pattern = w;
    strlower (tmp->pattern);
    tmp->flag = 0;

    tmp->next = 0;
    if (last == 0)
      evt_flags = tmp;
    else
      last->next = tmp;
    last = tmp;
  }
}

