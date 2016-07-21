#ifndef _readcfg_h
#define _readcfg_h

#include "Config.h"
#include "iphdr.h"
#include "ftnaddr.h"
#include "ftndom.h"
#include "ftnnode.h"

#define MAXINCLUDELEVEL 8
#define MAXCFGLINE 1024

#define MAXSYSTEMNAME 120
#define MAXSYSOPNAME 120
#define MAXLOCATIONNAME 120
#define MAXNODEINFO 120

extern int nAddr;
extern FTN_ADDR *pAddr;
extern int iport;
extern int oport;
extern int oblksize;
extern int nettimeout;
extern int rescan_delay;
extern int call_delay;
extern int max_servers;
extern int max_clients;
extern char sysname[MAXSYSTEMNAME + 1];
extern char sysop[MAXSYSOPNAME + 1];
extern char location[MAXLOCATIONNAME + 1];
extern char nodeinfo[MAXNODEINFO + 1];
extern char inbound[MAXPATHLEN + 1];
extern char inbound_nonsecure[MAXPATHLEN + 1];
extern char temp_inbound[MAXPATHLEN + 1];
extern int kill_dup_partial_files;
extern int kill_old_partial_files;
extern int kill_old_bsy;
extern int minfree;
extern int minfree_nonsecure;
extern int tries;
extern int hold;
extern int hold_skipped;
extern int backresolv;
extern int send_if_pwd;
extern int debugcfg;
extern char logpath[MAXPATHLEN + 1];
extern char binlogpath[MAXPATHLEN + 1];
extern char fdinhist[MAXPATHLEN + 1];
extern char fdouthist[MAXPATHLEN + 1];
extern char pid_file[MAXPATHLEN + 1];
extern int loglevel;
extern int conlog;
extern int printq;
extern int percents;
extern int tzoff;
extern char root_domain[MAXHOSTNAMELEN + 1];
extern int prescan;
#ifdef MAILBOX
/* FileBoxes dir */
extern char tfilebox[MAXPATHLEN + 1];
/* BrakeBoxes dir */
extern char bfilebox[MAXPATHLEN + 1];
extern int deleteablebox;
#endif
extern char proxy[MAXHOSTNAMELEN + 40];
extern char socks[MAXHOSTNAMELEN + 40];
extern struct conflist_type 
  { char *path;
    struct conflist_type *next;
    unsigned long mtime;
  } *config_list;

struct KEYWORD
{
  const char *key;
  void (*callback) (KEYWORD *key, char *s);
  void *var;
  int option1;
  int option2;
};

/*
 * Parses and reads the path as a config
 */
void readcfg (void);

int  get_host_and_port (int n, char *host, unsigned short *port, char *src, FTN_ADDR *fa);
void read_domain_info (KEYWORD *key, char *s);
void read_node_info (KEYWORD *key, char *s);
void read_aka_list (KEYWORD *key, char *s);

#endif
