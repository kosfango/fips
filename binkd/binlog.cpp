/*--------------------------------------------------------------------*/
/*                        System include files                        */
/*--------------------------------------------------------------------*/

#include "stdafx.h"
#include <time.h>
#include <stdio.h>
#include "Config.h"
/*--------------------------------------------------------------------*/
/*                        Local include files                         */
/*--------------------------------------------------------------------*/

#include "Config.h"
#include "binlog.h"
#include "sys.h"
#include "readcfg.h"
#include "prothlp.h"
#include "protoco2.h"
#include "assert.h"
#include "sem.h"

/*--------------------------------------------------------------------*/
/*                         Global definitions                         */
/*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*/
/*                          Global variables                          */
/*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*/
/*                           Local variables                          */
/*--------------------------------------------------------------------*/

static MUTEXSEM LSem;		/* =0 removed */

/*--------------------------------------------------------------------*/
/*    void BinLogInit(Void)                                           */
/*                                                                    */
/*    Initialise BinLog semaphore.                                    */
/*--------------------------------------------------------------------*/

void BinLogInit(void) {
	InitSem(&LSem);
}

/*--------------------------------------------------------------------*/
/*    void BinLogDeInit(Void)                                         */
/*                                                                    */
/*    Deinitialise BinLog semaphore.                                  */
/*--------------------------------------------------------------------*/

void BinLogDeInit(void) {
	CleanSem(&LSem);
}

/*--------------------------------------------------------------------*/
/*    void TLogStat (char*, STATE*)                                   */
/*                                                                    */
/*    Add record to T-Mail style binary log.                          */
/*--------------------------------------------------------------------*/

void TLogStat (char *status, STATE *state)
{
	struct {
		u16    fZone;
		u16    fNet;
		u16    fNode;
		u16    fPoint;
		u32    fSTime;
		u32    fLTime;
		u32    fBReceive;
		u32    fBSent;
		u8     fFReceive;
		u8     fFSent;
		u16    fStatus;
	} TS;

	FILE *fl;

	if (binlogpath[0]) {
		TS.fStatus = 0;

		if (state->to) {
			TS.fZone = state->to->fa.z;
			TS.fNet = state->to->fa.net;
			TS.fNode = state->to->fa.node;
			TS.fPoint = state->to->fa.p;
			TS.fStatus = 1;
		} else if (state->fa) {
			TS.fZone = state->fa->z;
			TS.fNet = state->fa->net;
			TS.fNode = state->fa->node;
			TS.fPoint = state->fa->p;
			TS.fStatus = 2;
		} else {
			TS.fZone = 0;
			TS.fNet = 0;
			TS.fNode = 0;
			TS.fPoint = 0;
			TS.fStatus = 0;
		}
		TS.fBReceive = state->bytes_rcvd;
		TS.fBSent = state->bytes_sent;
		TS.fFReceive = state->files_rcvd;
		TS.fFSent = state->files_sent;
		TS.fSTime = state->start_time + tzoff;
		TS.fLTime = time(NULL) - state->start_time;
		if (STRICMP(status, "OK") != 0) {
			TS.fStatus |= 3;
		}
		if ((fl = fopen(binlogpath,"ab")) != NULL) {
			fwrite(&TS,sizeof(TS),1,fl);
			fclose(fl);
		} else {
			Log(1,"unable to open binary log file `%s'",binlogpath);
		}
	}

}

/*--------------------------------------------------------------------*/
/*    void FDLogStat (STATE*)                                         */
/*                                                                    */
/*    Add record to FrontDoor-style binary log.                       */
/*--------------------------------------------------------------------*/

void FDLogStat (STATE *state)
{
	struct
	{
		u16    Zone;
		u16    Net;
		u16    Node;
		u16    Point;
		char   Domain[16];
		u32    TimeStart;
		u32    TimeEnd;
		char   StationName[32];
		char   StationLoc[40];
		u32    Received;
		u32    Sent;
		u32    Cost;
	} std;

	FILE *fp;

	if (!state->fa || ((state->to && !fdouthist[0]) || (!state->to && !fdinhist[0])))
            return; /* nothing to do */

	std.TimeStart = state->start_time + tzoff;

	time ((time_t*) &std.TimeEnd);
	std.TimeEnd += tzoff;

	std.Zone = state->fa->z;
	std.Net = state->fa->net;
	std.Node = state->fa->node;
	std.Point = state->fa->p;
	strnzcpy (std.Domain, state->fa->domain, sizeof (std.Domain));
	strnzcpy (std.StationName, state->sysname, sizeof (std.StationName));
	strnzcpy (std.StationLoc, state->location, sizeof (std.StationLoc));
	std.Received = state->bytes_rcvd;
	std.Sent = state->bytes_sent;
	std.Cost = 0; /* Let it be free :) */


	if (state->to) fp = fopen( fdouthist, "ab+" );
		  else fp = fopen( fdinhist , "ab+" );

	if( fp != NULL )
	{
		fwrite ( &std, (size_t) sizeof(std), (size_t) 1, fp);
		fclose( fp );
	}
	else Log (1, "failed to write to %s", (state->to ? fdouthist : fdinhist));

}

/*--------------------------------------------------------------------*/
/*    void BinLogStat (char*, STATE*)                                 */
/*                                                                    */
/*    Add record to binary logs.                                      */
/*--------------------------------------------------------------------*/

void BinLogStat (char *status, STATE *state)
{
  LockSem (&LSem);
  TLogStat (status, state);
  FDLogStat (state);
  ReleaseSem (&LSem);
}

