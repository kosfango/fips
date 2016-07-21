// =====================================================================================
// This file contains all global variables that are used in the F.I.P.S. project
// Copyright Johann Weinzierl 1995
// =====================================================================================
#ifndef globals_h
#define globals_h

extern _detincome gincome;					// Holds all the global stuff for INCOME HAndling
extern _ustat gustat;								// Holds all the stuff for user-interaction    
extern _watchmail watchmail;				// Die Mail nach der gekuckt werden soll ...
extern _magicid magicid;						// MAGIC ID Struktur

extern HANDLE mutex_tosspurg;       // Mutex to excutise TOSSER <-> PURGER
extern _curmail curmail;						// Infos's on current mail session
extern _rfilter grfilter;       		// Globales Reader Filter
extern ULONG *notbremse;     				// Notbremse array
extern long	 last_modemsession_time;// Unixtime der letzten Anwahl ...
extern int 	 notbremscounter;				// Wieviele Eintraeg sind guetig ..

extern char  typebuffer[25];				// for jokes searches

extern int jumpstack[100];	        // Jump back stack
extern int jumpcount;								// jump back stackcounter
 
extern long	gColors[20];						// global colortable
extern char	gMailbuf[2*MAX_MSG];		// current Mailtext (for expansion with '\n')
extern int 	gLastSucessfulCallTime;	// Letzte erfolgreiche MAIL Session
extern int 	gNewAreaFromExtSearch;
#endif
