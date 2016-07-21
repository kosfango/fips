//
// ########## charging info stuff #################
//

#include "stdafx.h"			// loadlibrary
#include "structs.h"

extern CStrList costbaselist;
extern CStrList costuserlist;

static char curphone[300];
static char currency[30];
static char chargzone[300];

#define BASEZONE 0
#define BASEPERU 1
#define BASEWDAY 2
#define BASEWEND 3
#define BASECURR 4
#define USERPHON 0
#define USERZONE 1

//=====================================================
// returns currency
	char *get_currency(void)
//=====================================================
{
	return currency;
}

//=====================================================
// returns charging zone
	char *get_zone(void)
//=====================================================
{
	return chargzone;
}

//=====================================================
// returns time in seconds since last call and reset counter
	long chargingtimediff(void)
//=====================================================
{
time_t oldtime;
long	t;

	t=time(NULL)-oldtime;
	time(&oldtime);
	return t;
}

// ===========================================================
// chargestring like "UNIT CHARGESTRING", returns secs per unit and charging sum
	int current_period(char *chargestring,float *charge)
// ===========================================================
{
CString tmp;
char curtime[30];
char from[10],to[10];
int	 fh,fm,th,tm,sec,count;

	_strtime(curtime);
	curtime[5]=0; // strip seconds

	count = count_tokens(chargestring);
	if (count<2)	return 0;

	get_token(chargestring,0,tmp,' ');
	sscanf(tmp,"%f",charge);	// charge unit

	for (int t=1;t<count;t++)
	{
		get_token(chargestring,t,tmp,' ');
		sscanf(tmp,"%d:%d-%d:%d=%d",&fh,&fm,&th,&tm,&sec);
		sprintf(from,"%02d:%02d",fh,fm);
		sprintf(to  ,"%02d:%02d",th,tm);

		if (strcmp(curtime,from)>=0 && strcmp(curtime,to)<0)
			return sec;
	}
	return 0;
}

// ===========================================================
// searches the time-period by the specified zone, returns string "UNIT CHARGESTRING"
	void get_period_by_zone(const char *zone,CString &period)
// ===========================================================
{
CString str,tmp;
struct tm *loctime;
time_t tim;

	period.Empty();
	for (int t=0;t<costbaselist.GetCount();t++)
	{
		str=costbaselist.GetString(t);
		get_token(str,BASEZONE,tmp);
		if (!tmp.CompareNoCase(zone))
		{
			time(&tim);                /* Get time as long integer. */
			loctime = localtime(&tim); /* Convert to local time. */

			if (loctime->tm_wday == 0 || loctime->tm_wday == 6)
				get_token(str,BASEWEND,period);   // weekend
			else
				get_token(str,BASEWDAY,period);   // workdays

			get_token(str,BASECURR,tmp);	// currency
			strcpy(currency,tmp);
			get_token(str,BASEPERU,tmp);	// charging per unit
			period = tmp+" "+period;
			return;
		}
	}
}

// ===========================================================
// searches for valid charging-period, if not found, the default-string is returned
	void get_current_zone(char *timeperiods)
// ===========================================================
{
CString str;
CString tmp;
size_t  matchlen=0;

	*timeperiods=0;
	for (int t=0;t<costuserlist.GetCount();t++)
	{
		str=costuserlist.GetString(t);
		get_token(str,USERPHON,tmp);
		if (!timeperiods[0] && (!stricmp(tmp,"default") || !stricmp(tmp,"по умолчанию")))
		{
			get_token(str,USERZONE,tmp);
			strcpy(chargzone,tmp);
			get_period_by_zone(chargzone,tmp);
			strcpy(timeperiods,tmp); // defaultvalue found
		}
		else if (!strncmp(curphone,tmp,strlen(tmp)) && strlen(tmp)>matchlen)
// search maximum similarity to curphone in costuser
		{
			matchlen = strlen(tmp);
			get_token(str,USERZONE,tmp);
			strcpy(chargzone,tmp);
			get_period_by_zone(chargzone,tmp);
			strcpy(timeperiods,tmp);
		}
	}
}

// ===========================================================
// returns charged money since last call and updates charging intervall
	float get_charging_sum(int *charging)
// ===========================================================
{
int		t;
float	charged=0;
float	sum=0;
char	periods[1024];

	t = chargingtimediff(); // get seconds since last call
	if (!t)	return 0;
	*charging-=t;		// reduce charging intervall on elapsed secs
	get_current_zone(periods);
	t=current_period(periods,&sum);
	if (!t)	return 0;	// timeintervall is missing
	while (*charging<0) // while charging intervall <0, we have spent money
	{
		*charging+=t;
		charged+=sum;
	}
	return charged;
}

// ===========================================================
// inits charging information and resets timer, should be called after a successful connect !
	void init_charging_for_one_call(char *phone)
// ===========================================================
{
char timeperiods[1024],tmp[300],*p,*t;

	p=tmp;
	for (t=phone;*t;t++)	if (isdigit(*t))	*p++=*t;	// remove non-digit chars
	*p=0;
	strcpy(curphone,tmp); // store phonenumber
	*currency=0;
	strcpy(chargzone,L("S_54"));
	get_current_zone(timeperiods); // init magics
	chargingtimediff(); // start charging timer
}
