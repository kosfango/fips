// ##############################################################################
//
//			stuff for packing or unpacking ARCMAIL (packed PKT's)
//
// ##############################################################################

#include "stdafx.h"
#include <ASSERT.h>
#include "structs.h"
#include <io.h>
#include "direct.h"
#include <ctype.h>
#include <stdio.h>
#include <time.h>
#include "supercom.h"
#include "mystrlst.h"
#include "resource.h"

typedef int	(_stdcall *PACKPROC)(char *,char *,char *,char *,char *);

extern _packer	Packers[];
extern HINSTANCE hUni,hRar,hCab,hAce;
int	get_packer_cmd(LPCSTR name,LPSTR cmd);
PACKPROC PackPrc;

//====================================================================
// returns valid arcmail-name in specified path or new name if none was found
	int get_arcmail_name(LPCSTR path,LPSTR arcmail)
//====================================================================
// returns 1 if an already existing file is returned, 0 if a new arcmail-name is returned
{
_finddata_t se;
ULONG		crc=0xFFFFFFFF;
time_t		tim;
long		hf;
char		tmp[MAX_PATH];
char		*pkt[]={"*.SU*","*.MO*","*.TU*","*.WE*","*.TH*","*.FR*","*.SA*"};

	strcpy(tmp,path);
	RS_GetCRC_CCITT32(tmp,(WORD)strlen(tmp),&crc);
	strcpy(arcmail,"");
	mkdir(path);

	for (int t=0;t<7;t++)
	{
		make_path(tmp,path,pkt[t]);
		if ((hf=_findfirst(tmp,&se)) != -1L)	// packet-archiv found
		{
			do
			{
				if (!(se.attrib & _A_SUBDIR))
				{
					make_path(arcmail,path,se.name);
					_findclose(hf);
					return 1;
				}
			} while (!_findnext(hf,&se));
			_findclose(hf);
		}
	}
	time(&tim);
	strcpy(tmp,ctime(&tim));
	tmp[2]=0;		// extracts day from daystring
	sprintf(arcmail,"%s\\%08X.%s0",path,crc,tmp);
	return 0;
}

//====================================================================
// returns the cmd for defined packername
	int get_packer_cmd(LPCSTR name,LPSTR cmd)
//====================================================================
{
int i=0;

	*cmd=0;
	while (Packers[i].name[0])
	{
		if (!strcmpi(Packers[i].name,name))
		{
			strcpy(cmd,Packers[i].pack_cmd);
			return 1;
		}
		i++;
	}
	return 0; // no packer found
}

// =========================================================================
// returns the cmd for defined boss
	int get_boss_packer(int ind,LPSTR cmd)
// =========================================================================
{
CString name;
char	str[MAX_BOSSLEN];

	if (ind<0)
		return 0;

	db_get_boss_by_index(ind,str);
	get_token(str,ARCHIVER,name);
	return get_packer_cmd(name,cmd);
}

//====================================================================================
// returns index to boss according to fido-number given in char*
// returns -1 if no boss is found
	int get_bossindex(char *fido)
//====================================================================================
{
CStrList boss;
CString buf;
int ret=-1;

	load_bosslist(boss);
	for (int t=0;t<boss.GetCount();t++)
	{
		get_token(boss.GetString(t),BOSSADDR,buf);
		if (!strncmp(buf,fido,strlen(buf)))
		{
			ret=t;
			break;
		}
	}
	return ret;
}

//====================================================================================
//  packs all files in files_lst to the archive zipname, full paths must be given
	int pack_file(LPSTR zipname,LPSTR files_lst,LPSTR packername)
//====================================================================================
//  returns 0 if any error occured, 1 if successful
{
_finddata_t se;
CString pcks,str;
long hf;
char outcmd[MAX_PATH],cmd[MAX_PATH],path[MAX_PATH];
int  retval=0,ret=0,mode;
BOOL built;

	built=get_cfg(CFG_TOSSER,"EnableBuiltinArc",0) && hUni;
	ret=get_packer_cmd(packername,cmd);
	if (!built && !ret)
		return 0;

	if ((hf=_findfirst(files_lst,&se)) != -1L)
	{
		_findclose(hf);
		if (built && (!stricmp(packername,"pkzip") || !stricmp(packername,"zip")))
		{
			PackPrc=(PACKPROC)GetProcAddress(hUni,"Pack");
			if (!PackPrc)
			{
				str.Format(IDS_SYSERROR,GetLastError(),"Pack");
				AfxMessageBox(str);
				goto extpack;
			}
			ret=PackPrc(zipname,NULL,"atZip",NULL,files_lst);
			if (ret)
				SafeFileDelete(files_lst);
		}
		else
		{
extpack:
			expand_external_cmd(cmd,files_lst,zipname,outcmd);
			get_path(zipname,path);
			mode=get_cfg(CFG_COMMON,"HideCmdWin",0) ? SW_HIDE : SW_SHOWMINNOACTIVE;
			ret=system_command(outcmd,L("S_352"),path,mode,300000,&retval);
		}
	 	if (!ret || retval)	return 0;
	}
	return 1;
}
