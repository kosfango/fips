#include "stdafx.h" 
#include <sys\stat.h>
#include "..\structs.h" 
#include "supercom.h" 
#include "pr_xfer.h"                    

extern _gconfig gc;

/*---------------------------------------------------------------------------*/

int resdata(char *emsibuf)
{
char *p;
int r1,r2,r3;

	r1=r2=r3=0;
	p=strstr(emsibuf,"{TRAF}");
	if (p) sscanf(p,"{TRAF}{%X %X}{MOH#}{%X}",&r1,&r2,&r3);
	return r1+r2+r3;
}
int senddata(char *sendlist)
{
int s;
int i,index;
char pathname[300];
char *p,*q;
struct stat f;

	index=0;
	s=0;
	if (!sendlist || *sendlist==0) return s;
	do 
	{
		index++;
		p=sendlist;
		for(i=1;(*p != 0) && (i != index);p++)
			if (*p==' ') i++; 
		if (i != index) break;
		if (*p==' ') p++;
		if (*p=='+') p++;
		for(q=p;*q != 0;q++)
			if (*q==' ') break; 
		memset(pathname,0,299);
		memmove(pathname,p,q-p);
		stat(pathname,&f);
		s+= f.st_size;
	} while(1);
	return s;
}

/*---------------------------------------------------------------------------*/
	void show_size (char *buf,long val)
{
int i;

	i=1;
	if (val>i*1024) i=i*1024;
	if (val>i*1024) i=i*1024;
	switch (i) 
	{
	case 1:
		strcpy(buf,L("S_287",val));	// byte
		break;
	case 1024:
		strcpy(buf,L("S_288",(double)val/i));	// kbyte
		break;
	default:
		strcpy(buf,L("S_289",(double)val/i));	// mbyte
		break;
	}
}

	void blog_read(unsigned char c)
{
FILE *pf;

	if (gc.displaylevel==3)
	{
		pf=fopen("mescomr.log","a+b"); 
		fprintf(pf,"%c",c);
		fclose(pf);
	}
	return;
}

	void blog_readstr(unsigned char *p,int len)
{
	for (int i=0;i<len;i++) blog_read(p[i]); 
}

	void blog_write(char c)
{
FILE *pf;

	if (gc.displaylevel==3)
	{
		pf=fopen("mescomw.log","a+b"); 
		fprintf(pf,"%c",c); 
		fclose(pf);
	}
	return;
}

/*==========================================================================*/
//   Передача блока символов в компорт.
/*==========================================================================*/
	void com_putblock(int port,PUCHAR s,USHORT len)
{ 
	for (int i=0;i<len;i++) 
	{
		if (!RS_Carrier(port)) break;              
		com_putbyte(port,s[i]);
	}
	return;
}

/*==========================================================================*/
//   Передача символа в компорт. Задержка 100 милисекунд
/*==========================================================================*/
	void com_putbyte (int port,UCHAR s)
{
	blog_write(s);  
	while (!RS_TXInTime(port,s,SEC0_1)) if (!RS_Carrier(port)) return;
}

/*==========================================================================*/
//   Прием символа из компорта. Задержка 500 милисекунд
/*==========================================================================*/
short com_getbyte (int port, unsigned char St_Pkt,char *p )
{
	while (!RS_RXInTime(port,p,SEC0_5)) 
	if (St_Pkt || !RS_Carrier(port)) return FALSE;
	blog_read(*p);  
	return true;
}
/*==========================================================================*/
//   Процедуры вывода сообщений в лог файл
/*==========================================================================*/
void message (char *fmt,...)
{
        char       buf[255],path[MAX_PATH];
        time_t       tim;
        struct     tm *t;
        FILE       *tmpfile;
        va_list    arg_ptr;

        memset(&buf,0,255);
        tim = time(NULL);
        t = localtime(&tim);
		make_path(path,gc.BasePath,"session.log");
        tmpfile=fopen(path,"a+");

        va_start(arg_ptr,fmt);
        vsprintf(&buf[0], &fmt[0], arg_ptr);
        va_end(arg_ptr);
        
        fprintf(tmpfile,"%02d.%02d.%04d %02d:%02d:%02d ",t->tm_mday, t->tm_mon+1, t->tm_year+1900,t->tm_hour,t->tm_min,t->tm_sec);
        fprintf(tmpfile, "%s\n", buf);
        
        fclose(tmpfile);
}


void clear_status (unsigned char xmit)
{        
	if (xmit)
	{
		gc.mailer.m_transferred=NULL;
		gc.mailer.m_total=NULL;
		gc.mailer.m_proz=NULL;
		gc.mailer.m_cps=NULL;
		gc.mailer.m_timeelapsed=NULL;
		strcpy(gc.mailer.m_filename,"");
	}
	else
	{
		gc.mailer.m_transferred2=NULL;
		gc.mailer.m_total2=NULL;
		gc.mailer.m_proz2=NULL;
		gc.mailer.m_cps2=NULL;
		gc.mailer.m_timeelapsed2=NULL;
		strcpy(gc.mailer.m_filename2,"");
	}
	modem_update();
}/*clear_status()*/

