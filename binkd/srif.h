/*
 *  srif.h -- Create flags or run external programs on mail events
 *
 *  srif.h is a part of binkd project
 *
 *  Copyright (C) 1996-1997  Dima Maloff, 5047/13
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version. See COPYING.
 */

/*
 * $Id: srif.h,v 1.3 1997/10/23 03:37:25 mff Exp mff $
 *
 * $Log: srif.h,v $
 * Revision 1.3  1997/10/23  03:37:25  mff
 * *** empty log message ***
 *
 * Revision 1.2  1997/03/28  06:12:48  mff
 * Changes to support SRIF: + evt_run(), etc.
 */
#ifndef _srif_h
#define _srif_h

#include "ftnq.h"
#include "readcfg.h"
#include "prothlp.h"
#include "protoco2.h"

typedef struct _EVT_FLAG EVT_FLAG;
struct _EVT_FLAG
{
  EVT_FLAG *next;
  char *path;				    /* Set this flag if != NULL */
  char *command;			    /* Run this command if != NULL */
  char *pattern;
  int flag;
  int imm;				    /* Immediate flag */
};

extern EVT_FLAG *evt_flags;

/*
 * Tests if filename matches any of EVT_FLAG's patterns.
 */
int evt_test (char *filename);

/*
 * Runs external programs using S.R.I.F. interface
 * if the name matches one of our "exec"'s
 */
FTNQ *evt_run (FTNQ *q, char *filename0, FTN_ADDR *fa, 
               int nfa, int prot, int listed, char *peer_name, STATE *st);

/*
 * Sets flags for all matched with evt_test events
 */
void evt_set (void);

#endif
