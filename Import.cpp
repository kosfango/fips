//
//		ASCII-Import stuff
//
//		BOSS-Setup-Import stuff
//

#include "stdafx.h"			// loadlibrary
#include <ASSERT.h>
#include <direct.h>
#include <io.h>
#include "structs.h"
#include "mystrlst.h"

//extern CStrList rpathes;
extern CStrList Akas;
extern CStrList Bosses;
extern _gconfig gc;
struct _aline
{
	char system[300];
	char fido[300];
	char tel[300];
	char sysop[300];
	char aka[300];
	char session[300];
	char arcmail[300];
	char packer[300];
	char arealist[300];
	char ap[300];
	char au[300];
	char np[300];
	char nu[300];
	char areafix[300];
	char apwd[300];
	int  aplus;
	char filefix[300];
	char fpwd[300];
	int  fplus;
	char route [300];
	char noroute[300];
	int	 pollpacket;
	int  nohydra;
	int  nozmodem;
	int  queue;
} aline;

//=========================================================================
// imports the mail fname
	int parse_import(LPCSTR fname)
//=========================================================================
{
CStrList msg;
CString str;
FILE	*fp;
char	line[300],tmp[300],tmp2[300],rest[300],area[300],from[300],to[300];
char	fromadr[300],toadr[300],subj[300],buf[300],origin[300],path[MAX_PATH];
char	*p;
int		szone,snet,snode,spoint,dzone,dnet,dnode,dpoint;
int		scanned=0,intext=0,attach=0,crash=0,privatemail=0,keyflag=0;

	szone=snet=snode=spoint=0;
	dzone=dnet=dnode=dpoint=0;
	*subj=0;
	fp=fopen(fname,"rt");	if (!fp) return 0;
	msg.RemoveAll();
	tosser_add_listbox(L("S_359",fname));	// proccessing file
	//db_get_area_by_index(,&ad);
	get_origin(&(gustat.act_area),TRUE,str,TRUE);
	strcpy(origin,str);

	while (fgets(line,300,fp))
	{
		line[299]=0;
		trim_all(line);
		*tmp=0;
		get_token(line,0,tmp,' ');

		if (!intext)
		{
			p = line;
			while (*p && *p != ' ')	p++;
			while (*p && *p == ' ')	p++;

			strcpy(rest,p);
			trim_all(rest);
			russian_conversion(rest,TRUE);

			if (!stricmp(tmp,"Area"))		{ strcpy(area,rest); strupr(area); }
			if (!stricmp(tmp,"From"))		strcpy(from,rest);
			if (!stricmp(tmp,"FromAddr"))
			{
				strcpy(fromadr,rest);
				parse_address(fromadr,&szone,&snet,&snode,&spoint);
			}

			if (!stricmp(tmp,"To"))				strcpy(to,rest);
			if (!stricmp(tmp,"ToAddr"))
			{
				strcpy(toadr,rest);
				parse_address(toadr,&dzone,&dnet,&dnode,&dpoint);
			}

			if (!stricmp(tmp,"Subject"))		strcpy(subj,rest);
			if (!stricmp(tmp,"SendDirect"))		crash=1;
			if (!stricmp(tmp,"!SendDirect"))	crash=0;
			if (!stricmp(tmp,"PrivateMail"))	privatemail=1;
			if (!stricmp(tmp,"!PrivateMail"))	privatemail=0;
			if (!stricmp(tmp,"Scanned"))		scanned=1;
			if (!stricmp(tmp,"!Scanned"))		scanned=0;
			if (!stricmp(tmp,"ClearMsgText"))	msg.RemoveAll();

			if (!stricmp(tmp,"Attach"))
			{
				GetShortPathName(rest,subj,sizeof(subj));
				if (!access(subj,0))
				{
					attach=1;
					strupr(subj);
					tosser_add_listbox(L("S_246",subj));	// attached file
				}
				else
				{
					attach=0;
					tosser_add_listbox(L("S_119",subj));	// file missed
					strcpy(subj,"ERROR: Cannot find source file");
				}
				subj[75]=0;
			}

			if (!stricmp(tmp,"RequestFile"))
			{
				int zone,net,node,point;
				char fname[300],dummy[300];
				CString basedir;
				FILE *fp;

				zone=net=node=point=0;

				basedir.Format("%s\\",gc.OutboundPath);

				if (*rest != '#')
				{
					if (sscanf(rest,"%d:%d/%d.%d %s",&zone,&net,&node,&point,fname)<4)
					{
						if (sscanf(rest,"%d:%d/%d %s",&zone,&net,&node,fname)<3)
						{
							sscanf(rest,"%s",dummy);
							tosser_add_listbox(L("S_282",dummy));	// invalid address
							goto exit_proc;
						}
					}
					make_fidodir(dummy,zone,net,node,point);
				}
				else // direct phone number with preffix '#'
					sscanf(rest,"%s %s",dummy,fname);

				basedir=basedir+dummy; // get the outbound-dir
				mkdir(basedir);
				make_path(path,basedir,FREQ_LIST);
				strupr(fname);
				fp = fopen(path,"at");
				if (fp)
				{
					fprintf(fp,"%s\n",fname);
					fclose(fp);
					if (*dummy != '#')
						tosser_add_listbox(L("S_380",fname,zone,net,node,point));	// freq
					else
						tosser_add_listbox(L("S_15",fname,dummy));	// freq
				}
				else
					tosser_add_listbox(L("S_112",path));	// freq error
exit_proc:
				strcpy(buf,"");
			}

			if (!stricmp(tmp,"ORIGIN"))
			{
				strcpy(origin,rest);
				origin[70]=0;
			}

			if (!stricmp(tmp,"@BEGINTEXT"))			intext=1;

			if (!stricmp(tmp,"@IMPORTFILE"))
			{
				FILE *fp;
				char line[1024],*p;

				strcpy(tmp2,rest);
				fp = fopen(tmp2,"rt");
				if (fp)
				{
					while (fgets(line,1000,fp))
					{
						p=strchr(line,'\r'); if(p) *p=0;
						p=strchr(line,'\n'); if(p) *p=0;
						russian_conversion(line,TRUE);
						msg.AddTail(line);
					}
					fclose(fp);
				}
				else
				{
					tosser_add_listbox(L("S_121",tmp2));	// error import file
					sprintf(buf,"Cannot import file* %s *",tmp2);
					msg.AddTail("");
					msg.AddTail("!! //Error// !!");
					msg.AddTail("");
					msg.AddTail(buf);
				}
			}

			if (!stricmp(tmp,"WRITEMAIL"))
			{
				mailheader mh;
				areadef ad;
				char	txt[MAX_MSG];
				int		len=0;
				long	hand;

				memset(&ad,0,sizeof(areadef));
				memset(&mh,0,sizeof(mailheader));
				mh.mailid = time(NULL);

				if (db_get_area_handle(area,&hand,0)!=DB_OK)
				{
					fclose(fp);
					tosser_add_listbox(L("S_122",area));	// error: unknown area
					return 0;
				}

// write mail into msgbase
				tosser_add_listbox(L("S_145",to));	// mail created
				mh.status|=DB_MAIL_SCANNED;
				if (!scanned)
				{
					tosser_add_listbox(L("S_307"));	// redirected to boss
					mh.status &= ~DB_MAIL_SCANNED;
					mh.status |= DB_MAIL_CREATED;
				}

				db_get_area_by_index(hand,&ad);
				memset(txt,0,sizeof(txt));
// netmail kludges
				if (hand==0)
				{
					sprintf(txt,"\01INTL %d:%d/%d %d:%d/%d\r",dzone,dnet,dnode,szone,snet,snode);
					if (spoint)
					{
						sprintf(tmp,"\01FMPT %d\r",spoint);
						strcat(txt,tmp);
					}
					if (dpoint)
					{
						sprintf(tmp,"\01TOPT %d\r",dpoint);
						strcat(txt,tmp);
					}
				}
// common kludges
				sprintf(tmp,"\01MSGID: %d:%d/%d.%d %X\r\01PID: %s\r",
					szone,snet,snode,spoint,mh.mailid,get_versioninfo(tmp2,0));
				strcat(txt,tmp);
// mailtext
				for (int t=0;t<msg.GetCount();t++)
				{
					strcpy(line,msg.GetString(t));
					strcat(line,"\r");
					if (len < MAX_MSG-5000)
					{
						len+=strlen(line);
						strcat(txt,line);
					}
				}

// tearline and origin
				strcat(txt,get_tearline(str,TRUE));
				str.Format("\r * Origin: %s (%s)\r",origin,ad.aka);
				strcat(txt,str);

				mh.mail_text=txt;
				mh.text_length=strlen(txt)+1;
				mh.structlen=sizeof(mailheader);
				mh.mailid = time(NULL);
				mh.attrib=0;

				if (attach)	mh.attrib |= MSGFILE;
				if (crash)	mh.attrib |= MSGCRASH;
				if (hand==0 || privatemail)	mh.attrib |= MSGPRIVATE;

				unix_time_to_fido(mh.mailid,mh.datetime);
				mh.unixtime=parse_time(mh.datetime);
				strcpy(mh.toname,to);
				strcpy(mh.fromname,from);
				strcpy(mh.subject,subj);
				mh.srczone=mh.srcnet=mh.srcnode=mh.srcpoint=0;
				mh.dstzone=mh.dstnet=mh.dstnode=mh.dstpoint=0;
				parse_address(fromadr,&mh.srczone,&mh.srcnet,&mh.srcnode,&mh.srcpoint);
				parse_address(toadr,&mh.dstzone,&mh.dstnet,&mh.dstnode,&mh.dstpoint);

				db_append_new_message(hand,&mh);
				db_set_area_state(hand,1);

				scanned=intext=attach=crash=privatemail=0;
			}
		}
		else	// we are in the intext-section
		{
			if (!stricmp(tmp,"@ENDTEXT"))
				intext=0;
			else
				msg.AddTail(russian_conversion(line,TRUE));
		}
	}
	fclose(fp);
	return 1;
}

//=========================================================================
// searches in path for mails to import with extension .IMP
	void check_import(char *path)
//=========================================================================
{
_finddata_t se;
char	tmp[MAX_PATH];
long	hfile;

	make_path(tmp,path,"*.IMP");
	if ((hfile=_findfirst(tmp,&se)) != -1L)
	{
		do
		{
			make_path(tmp,path,se.name);
			if (parse_import(tmp))
			{
				unlink(tmp);
				tosser_add_listbox(L("S_442",tmp));
			}
		}
		while (_findnext(hfile,&se) == 0);
		_findclose(hfile);
	}
}
