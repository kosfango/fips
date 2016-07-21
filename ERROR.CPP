#include "stdafx.h"
#include "resource.h"
#include "lightdlg.h"

#define ERRBUF 300
int get_identifier_line(LPCSTR id,LPSTR buf,LPCSTR sect);

LangID	LANG_ID;
static	char dbuf[300];
CString oldtext;

extern CLightDlg *gpMain;
extern _gconfig gc;

	int err_out (char *id,...)
{
_udp_packet	sp;
short	ret;
va_list arg_ptr;
char    buf[ERRBUF],buf2[ERRBUF],buf3[ERRBUF],id2[50],tmp[100];
UINT    style=MB_OK;		  //default style
char    title[30];

// terminate function if id=="E_WRONGNODEL" and NO nodelist configured
// or installation is running
	if(!strcmp(id,"E_WRONGNODEL"))
	{
		if (gc.nlwarning) return 0;
        CStrList nodel;
		nodel.LoadFromFile("nodelist.cfg");
		if (nodel.GetCount()==0) return 0;
		gc.nlwarning=1;
	}

	strcpy(title,L("S_21",get_versioninfo(tmp,1)));	// message

    va_start(arg_ptr,id);
    strcpy(id2,id);
	ret=get_identifier_line(id2,buf,langstr("GERMAN","ENGLISH","RUSSIAN","FRENCH"));
    if (ret==0)
    {
       va_end (arg_ptr);
       return 0;
    }

// Warning
    if (strncmp("W_",id2,2)==0)
    {
		strcpy(title,L("S_22",get_versioninfo(tmp,1)));	// warning
		style=MB_OK | MB_ICONEXCLAMATION;
    }

// Questions
    if (!strncmp("DY_",id2,3) || !strncmp("DN_",id2,3))
    {
		strcpy(title,L("S_20",get_versioninfo(tmp,1)));	// question
		style=MB_YESNO | MB_ICONQUESTION;
		if (!strncmp("DY_",id2,3))	// default Yes
			style|=MB_DEFBUTTON1;
		else						// default No
			style|=MB_DEFBUTTON2;
    }

// Errors
    if (!strncmp("E_",id2,2) || !strncmp("FA_",id2,3) ||
		!strncmp("FR_",id2,3) || !strncmp("FI_",id2,3))
    {
		strcpy(title,L("S_19",get_versioninfo(tmp,1)));	// error
		if (!strncmp("E_",id2,2))
			style=MB_OK | MB_ICONHAND;
		else if (!strncmp("FA_",id2,3))	// Abort_Retry_Ignore:  Default Abort
			style=MB_ABORTRETRYIGNORE | MB_ICONQUESTION | MB_DEFBUTTON1;
		else if (!strncmp("FR_",id2,3))	// Abort_Retry_Ignore:  Default Retry
			style=MB_ABORTRETRYIGNORE | MB_ICONQUESTION | MB_DEFBUTTON2;
		else if (!strncmp("FI_",id2,3))	// Abort_Retry_Ignore:  Default Ignore
			style=MB_ABORTRETRYIGNORE | MB_ICONQUESTION | MB_DEFBUTTON3;
    }


    vsprintf(buf2,buf,arg_ptr);
	if ((time(NULL)-gc.LastUDPCommand)<5)
	{
		memset(&sp,0,sizeof(_udp_packet));
		sp.command		=UDP_REMOTEERROR;
		sp.subcommand	=style;
		sprintf(buf3,"%s\t%s",title,buf2);
		if (strlen(buf3)>99)	buf3[99]=0;
		strcpy(sp.data,buf3);
		gc.ErrMsgBoxRemoteResponse=-1;
		send_tcp_request(&sp);
		#ifdef DEBUDP
			sprintf(dbuf,"UDP: Sending err_out() to remote\n");
			OutputDebugString(dbuf);
		#endif

		while (gc.ErrMsgBoxRemoteResponse==-1)
		{
			gpMain->GetWindowText(oldtext);
			gpMain->SetWindowText(L("S_466"));	// waiting remote answer
			Sleep(250);
			gpMain->SetWindowText(oldtext);
			Sleep(250);
  		}
		Sleep(10);
		#ifdef DEBUDP
			sprintf(dbuf,"UDP: Got err_out() Answer from remote\n");
			OutputDebugString(dbuf);
		#endif
		ret=gc.ErrMsgBoxRemoteResponse;
		gc.ErrMsgBoxRemoteResponse=-1;
	}
	else
		ret=MessageBox(NULL,buf2,title,style | MB_TASKMODAL);

    va_end(arg_ptr);
    return ret;
}

// ===========================================================
	int get_identifier_line(LPCSTR id,LPSTR buffer,LPCSTR sect)
// ===========================================================
{
FILE  *fp;
char  buf[ERRBUF],base[MAX_PATH],tmp[100],tmp1[100];
char  *p,*h,*k,*nr;
short i=0,count=0;
int   c,validsection=0;

	make_path(base,gc.BasePath,FIPSMESSAGES);
	fp=fopen(base,"rt");
	if (!fp)
	{
		strcpy(tmp,L("S_206",FIPSMESSAGES));
		strcpy(tmp1,L("S_19",get_versioninfo(buf,1)));
		MessageBox(NULL,tmp,tmp1,MB_OK);
		return 0;
	}

	*buf=0;
   while (fgets(buf,ERRBUF-1,fp))
   {
      if (*buf==';')	// comment line
         continue;

	  if (!strnicmp(buf,"@section ",9))
	  {
			sscanf (buf,"%s %s",tmp,tmp1);
			validsection=!strcmp(tmp1,sect) ? 1 : 0;
	  }

	  if (!validsection)
		 continue;

      k=strstr(buf,id);
      if (k && k < &buf[5])
      {
		sscanf(buf,"%s",id);
         p=buf;
         while (*p && i<ERRBUF && count<2)
         {
            if (*p=='"')
            {
               count++;
               if (count==1)
                  nr=p;
            }
            p++;
            i++;
         }
         if (count!=2)
         {
			strcpy(tmp,L("S_200",id));
			strcpy(tmp1,L("S_19",get_versioninfo(tmp1,1)));	// error
			MessageBox (NULL,tmp,tmp1,MB_OK);
            fclose(fp);
            return 0;
         }

         i=0;
         for (h=++nr;*h!='"';h++)
             buffer[i++]=*h;

         if (*++h == '+')
         {

            do
            {
                while (fgetc(fp) != '"')	;

                do
                {
                   buffer[i]=(char)fgetc(fp);

                }  while (buffer[i++] != '"');
                buffer[--i]='\0';
                c=fgetc(fp);

            } while (c=='+');
         }

         buffer[i]='\0';
         fclose(fp);

// Replace char '\' with '\n'
         p=buffer;
         while (*p)
         {
            if (*p=='\\')
                *p='\n';
            p++;
         }
         return 1;
      }
   }

	strcpy(tmp,L("S_16",FIPSMESSAGES,id));
	strcpy(tmp1,L("S_19",get_versioninfo(tmp1,1)));
	MessageBox(NULL,tmp,tmp1,MB_OK);
	fclose(fp);
	return 0;
}
