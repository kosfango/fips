#include "stdafx.h"
#include "cfido.h"
#include "structs.h"
#include "resource.h"

extern _gconfig gc;

CFido::CFido() { }
CFido::CFido(LPCSTR str) { *this=str; }
CFido::~CFido(){ }

// ============================================================
	BOOL CFido::operator== (CFido cf)
// ============================================================
{
	return zone==cf.zone && net==cf.net && node==cf.node && point==cf.point;
}

// ============================================================
	void CFido::operator= (LPCSTR name)
// ============================================================
{
int ret;

    isdigit=zone=net=node=point=0;

	if (*name=='#')	   // it's a direct phone number
	{
	   isdigit=1;
	   strcpy(asciibuf,name);
	   return;
	}

	// if number contains '~' than FileSystem has long filenames
	if (strchr(name,'~'))
	{
	   ::MessageBox(NULL,"Detected problem with filename length!","Error",MB_OK);
	   return;
	}

	if (strchr(name,'_'))
	{
		ret=sscanf(name,"%d_%d_%d_%d",&zone,&net,&node,&point);
		ASSERT((ret==3) || (ret==4));
	}
	else
	{
		ret=parse_address(name,&zone,&net,&node,&point);
		ASSERT(ret==3 || ret==4);
	}
// store this adress internally
	make_address(asciibuf,zone,net,node,point);
}

// ============================================================
	LPCSTR CFido::GetAsDir(char *retbuf)
// ============================================================
{
char tmp[500];

	if (isdigit==1)
		return asciibuf;

	ASSERT(zone<=9999 && net<=9999 && node<=9999 && point<=9999);

	if (point==0)
		sprintf(tmp,"%d_%d_%d",zone,net,node);
	else
		sprintf(tmp,"%d_%d_%d_%d",zone,net,node,point);

	if (retbuf)
		strcpy(retbuf,tmp);	 // Fixed by oli!

	strcpy(asciibuf,tmp);
	return asciibuf;
}

// ============================================================
	LPCSTR CFido::Get(char *retbuf)
// ============================================================
{
char tmp[500];

	if (isdigit==1)
	{
		if (retbuf)
		   strcpy(retbuf,asciibuf);
		return asciibuf;
	}

	ASSERT(zone<=9999 && net<=9999 && node<=9999 && point<=9999);
	make_address(tmp,zone,net,node,point);

	if (retbuf)
		strcpy(retbuf,tmp);

	strcpy(asciibuf,tmp);
	return asciibuf;
}


// ============================================================
	void CFido::Get(int *pzone,int *pnet,int *pnode, int *ppoint)
// ============================================================
{
	if (isdigit==1)
	{
		*pzone  = -1;
		*pnet   = -1;
		*pnode  = -1;
		*ppoint = -1;
	}
	else
	{
		ASSERT(zone<=9999 && net<=9999 && node<=9999 && point<=9999);
 		*pzone	=zone;
 		*pnet 	=net;
 		*pnode	=node;
 		*ppoint	=point;
	}
}

// ============================================================
	void CFido::Set(int pzone,int pnet,int pnode, int ppoint)
// ============================================================
{
 	zone=pzone;
 	net =pnet;
 	node=pnode;
 	point=ppoint;
}

// ============================================================
	void CFido::GetSystemName(LPSTR str)
// ============================================================
{
_fidonumber nlquery;

    if (!str)	return;
	if (nl_get_fido_by_number(zone,net,node,point,&nlquery))
		strcpy(str,nlquery.bbsname);
	else
		Get(str);
}

// ============================================================
	int CFido::GetPhoneAndPwd(char *phone,char *passwd,char *aka,char *filefixpw)
// ============================================================
{
_fidonumber nlquery;
CString phon,pass,akax,filepw,entry;
char	buf[MAX_BOSSLEN];
int		i=0;

	if (isdigit==1)
	{
		strcpy(buf,asciibuf);
		strcpy(phone,&buf[1]);
		if (passwd)
			strcpy(passwd,"");
	    return 1;
	}

	while (db_get_boss_by_index(i,buf))
	{
		get_token(buf,BOSSADDR,entry);	  // boss address
		if (entry==asciibuf)
		{
			get_token(buf,BOSSPHON,phon);  // phone
			strcpy(phone,phon);

			get_token(buf,SESSPASW,pass);  // session password
			if (passwd)
				strcpy(passwd,pass);

			if (aka)
			{
				get_token(buf,MAINAKA,akax);	  // main aka
				strcpy(aka,akax);
			}

			if (filefixpw)
			{
				get_token(buf,FFIXPASW,filepw);	  // filefix password
				strcpy(filefixpw,filepw);
			}
			return 1;
		}
		i++;
	}

    if (nl_get_fido_by_number(zone,net,node,point,&nlquery))
	{
		if (passwd)
			strcpy(passwd,"");
		strcpy(phone,nlquery.phone);
		translate_phone_number(phone);
		return 1;
	}
	return 0;
}

// =======================================================
// translate full phone number to work one
	void translate_phone_number(char *phone)
// =======================================================
{
char str[100];
int	 lpref;

// replace country prefix
	strcpy(str,get_cfg(CFG_PREFIX,"CountryPref",""));
	lpref=strlen(str);
	if (*str && !strncmp(phone,str,lpref))
	{
		strcpy(str,get_cfg(CFG_PREFIX,"CountryRepl",""));
		strcat(str,phone+lpref);
		strcpy(phone,str);
	}
// remove city prefix
	strcpy(str,get_cfg(CFG_PREFIX,"CityPref",""));
	lpref=strlen(str);
	if (*str && !strncmp(phone,str,lpref))
	{
		strcpy(str,phone+lpref);
		strcpy(phone,str);
	}
// add international prefix
	strcpy(str,get_cfg(CFG_PREFIX,"InternatPref",""));
	strcat(str,phone);
	strcpy(phone,str);
}

// =======================================================
// expands partial Fido-address to full one
	int CFido::expand_incomplete(char *inc)
// =======================================================
// Available Formats:
// *	2:2494/17
// *	2494/17
// *	17
// *    hornet
// *    #0878192116
{
CString addr;
char    buf[MAX_BOSSLEN];
int		zone,net,node,num=0;

	if (strchr(inc,'#'))
	   return 0;

	strcpy(asciibuf,inc);
	if (db_get_boss_by_index(0,buf)==DB_OK)	// get first boss info
	{
		get_token(buf,BOSSADDR,addr);
		parse_address(addr,&zone,&net,&node);

		if (!strchr(inc,'/'))	// node and point
		{
            sprintf(asciibuf,"%d:%d/%s",zone,net,inc);
			return 1;
		}
		if (!strchr(inc,':'))	//  net,node & point
		{
			sprintf(asciibuf,"%d:%s",zone,inc);
			return 1;
		}
		return 1;
	}
	return 0;
}
