#include "stdafx.h"
#include "mystrlst.h"
#include "sys\types.h"
#include "sys\stat.h"
#include "structs.h"

// sys/types.h> followed by <sys/stat.h

#include "io.h"
#define MAGIC_FORMAT_ID 0x54635461

//CStrList dateinfos;
//extern char rootpath[MAX_PATH];
extern _gconfig gc;
extern LangID LANG_ID;

struct _btree
{
	struct _btree *left;
	struct _btree *right;
	char *data;
};

void AddItemForSort(struct _btree ** root,char *data,int datalen,int sortindex,int sort_numeric);
void RebuildList(CStrList *me,struct _btree ** root);

// ============================================================
    CStrList::CStrList(){}
// ============================================================

// ============================================================
    CStrList::~CStrList(){}
// ============================================================

// ============================================================
    LPCSTR CStrList::GetString(int i)
// ============================================================
{
	if (i>=m_nCount)
	{
        TRACE("CStrList::GetString ERROR: wanted=%d,has=%d\n",i,m_nCount);
		return("");
	}
	if (i<0)
	{
        TRACE("CStrList::GetString ERROR: Requested INDEX=%d",i);
		return("");
	}
	return((LPCSTR)GetAt(FindIndex(i)));
}

// ============================================================
    int CStrList::GetString(int i,CString &text)
// ============================================================
{
int num;

	num=GetCount();
	if (i>=num)
	{
        TRACE("CStrList::GetString ERROR: wanted=%d,has=%d\n",i,num);
		text.Empty();
	}
	if (i<0)
	{
        TRACE("CStrList::GetString ERROR: Requested INDEX=%d",i);
		text.Empty();
	}
	text=GetAt(FindIndex(i));
	return 1;
}

// ============================================================
    int CStrList::AppendToString(int i,char *apptext,char *appafter)
// ============================================================
{
int		num;
CString help;

	num=GetCount();
	if (i<0)
		return 0;

	while (GetCount()<=i)
		AddTail("");

	help=GetString(i);
	help+=apptext;
	if (appafter)
		help+=appafter;

	Insert(i,help);
	Remove(i);
	return 1;
}

// ============================================================
    int CStrList::SaveToFile(LPCSTR filename,int conv)
// ============================================================
{
int		count;
int		ret;
int		i;
int		magix;
FILE	*fp;
int		old_count				=0;
int		old_defaultindex		=0;
int		old_defaultindexnew		=0;
int		old_reserved3			=0;
int		old_reserved4			=0;
int		old_reserved5			=0;
char	buf[50000];

	install_date=install_checksum=0;

 	fp=fopen(filename,"rb");
	if (fp)
	{
		ret=fread(&magix,				sizeof(int),1,fp);		// Read ist dummy
		ret=fread(&old_count,			sizeof(int),1,fp);		// Read ist dummy
		ret=fread(&old_defaultindex,	sizeof(int),1,fp);		// Read ist dummy
		ret=fread(&old_defaultindexnew,	sizeof(int),1,fp); 		// Read ist dummy
		ret=fread(&install_date,		sizeof(int),1,fp);		// Read ist wichtig
		ret=fread(&install_checksum,	sizeof(int),1,fp);		// Read ist wichtig
		ret=fread(&old_reserved3,		sizeof(int),1,fp);		// Read ist dummy
		ret=fread(&old_reserved4,		sizeof(int),1,fp);		// Read ist dummy
		ret=fread(&old_reserved5,		sizeof(int),1,fp);		// Read ist dummy
		fclose(fp);
	}

	install_date=0;
	install_checksum=0;

	fp=fopen(filename,"wb");
	if (!fp)
		return 0;

	magix = MAGIC_FORMAT_ID;
	ret=fwrite(&magix,sizeof(int),1,fp);			if (ret!=1)	 { 	fclose(fp);	return 0; 	}

	count=GetCount();
	ret=fwrite(&count,sizeof(int),1,fp);			if (ret!=1)	 { 	fclose(fp);	return 0; 	}
	ret=fwrite(&defaultindex,sizeof(int),1,fp);		if (ret!=1)	 { 	fclose(fp);	return 0; 	}
	ret=fwrite(&defaultindexnew,sizeof(int),1,fp);	if (ret!=1)	 { 	fclose(fp);	return 0; 	}

	set_date_check(&install_date,&install_checksum);

	ret=fwrite(&install_date,	 sizeof(int),1,fp); if (ret!=1)	{ fclose(fp); return 0;	}
	ret=fwrite(&install_checksum,sizeof(int),1,fp); if (ret!=1)	{ fclose(fp); return 0;	}
	ret=fwrite(&reserved3,sizeof(int),1,fp);		if (ret!=1)	{ fclose(fp); return 0;	}
	ret=fwrite(&reserved4,sizeof(int),1,fp);		if (ret!=1)	{ fclose(fp); return 0;	}
	ret=fwrite(&reserved5,sizeof(int),1,fp);		if (ret!=1)	{ fclose(fp); return 0;	}

	for (i=0;i<count;i++)
	{
		strcpy(buf,GetAt(FindIndex(i)));
		if (conv>0) OemToChar(buf,buf);
		if (conv<0) CharToOem(buf,buf);
		fprintf(fp,"%s\n",buf);
	}

	fclose(fp);
	HandleAsciiExport(filename);
	return 1;
}

// ============================================================
    int CStrList::LoadFromFile(LPCSTR filename1,int conv)
// ============================================================
// conv=0 - without conversion
//     >0 - OEM to ANSI
//     <0 - ANSI to OEM
{
int  magix,count,ret,i;
FILE *fp;
char *p;
char filename[MAX_PATH],buf[50000];

	RemoveAll();
	HandleNonExistent(filename1);

nach_konvertierung:

	if (strstr(filename1,".cfg"))
		make_path(filename,gc.BasePath,filename1);
	else
		strcpy(filename,filename1);

	fp=fopen(filename,"rb");

	if (!fp)
		return 0;

	ret=fread(&magix,sizeof(int),1,fp);
	if (ret!=1)
	{
		fclose(fp);
		return 0;
	}

	if (magix!=MAGIC_FORMAT_ID)
	{
		fclose(fp);
		if (!convert_to_newer_format(filename))
			ERR_MSG2_RET0("E_CONVERSFAILED",filename);
		goto nach_konvertierung;
	}

	if (fread(&count,sizeof(int),1,fp)!=1) { fclose(fp); return 0;	}
	if (fread(&defaultindex,sizeof(int),1,fp)!=1) { fclose(fp); return 0;	}
	if (fread(&defaultindexnew,sizeof(int),1,fp)!=1) { fclose(fp); return 0;	}
	if (fread(&install_date,sizeof(int),1,fp)!=1) { fclose(fp); return 0; }
	if (fread(&install_checksum,sizeof(int),1,fp)!=1) { fclose(fp); return 0; }
	if (fread(&reserved3,sizeof(int),1,fp)!=1) { fclose(fp); return 0;	}
	if (fread(&reserved4,sizeof(int),1,fp)!=1) { fclose(fp); return 0;	}
	if (fread(&reserved5,sizeof(int),1,fp)!=1) { fclose(fp); return 0;	}

	for (i=0;i<count;i++)
	{
		p=fgets(buf,49999,fp);
		if (!p)
		{
		   fclose(fp);
		   return 0;
		}
 		buf[strlen(buf)-1]=0;
		if (conv>0) OemToChar(buf,buf);
		if (conv<0) CharToOem(buf,buf);
		AddTail(buf);
	}
	fclose(fp);
	HandleAsciiExport(filename,1);
	return 1;
}

// ============================================================
	void CStrList::SaveToDelimText(LPCSTR fname,int conv,char delim)
// ============================================================
// conv=0 - without conversion
//     >0 - OEM to ANSI
//     <0 - ANSI to OEM
{
FILE	*fp;
CString buf;
	
	fp=fopen(fname,"wb");
	if (!fp)
		return;
	
	for (int i=0;i<GetCount();i++)
	{
		buf=GetAt(FindIndex(i));
		buf.Replace('\t',delim);
		if (conv>0) buf.OemToAnsi();
		if (conv<0) buf.AnsiToOem();
		fprintf(fp,"%s\n",buf);
	}
	fclose(fp);
}

// ============================================================
// saves string list to text file
	void CStrList::SaveToDelimText(LPCSTR fname,int conv)
// ============================================================
// conv=0 - without conversion
//     >0 - OEM to ANSI
//     <0 - ANSI to OEM
{
FILE	*fp;
CString buf;
	
	fp=fopen(fname,"wb");
	if (!fp)
		return;
	
	for (int i=0;i<GetCount();i++)
	{
		buf=GetAt(FindIndex(i));
		if (conv>0) buf.OemToAnsi();
		if (conv<0) buf.AnsiToOem();
		fprintf(fp,"%s\r\n",buf);
	}
	fclose(fp);
}

// ============================================================
	void CStrList::SaveToDelimString(CString &str,char delim)
// ============================================================
{
CString buf;
	
	str.Empty();
	for (int i=0;i<GetCount();i++)
	{
		buf=GetAt(FindIndex(i));
		if (i>0)	str+=delim;
		str+=buf;
	}
}

// ============================================================
    int CStrList::LoadFromDelimText(LPCSTR fname,int conv,char delim)
// ============================================================
// conv=0 - without conversion
//     >0 - OEM to ANSI
//     <0 - ANSI to OEM
{
FILE	*fp;
char	buf[1000];
CString str;
	
	RemoveAll();
	fp=fopen(fname,"rt");
	if (fp)
	{
		while (fgets(buf,999,fp))
		{
			if (buf[0]==';' || buf[0]=='\n')	continue;
			str=buf;
			str.Replace(delim,'\t');
			str.Replace('\n','\0');
			if (conv>0) str.OemToAnsi();
			if (conv<0) str.AnsiToOem();
			AddTail(str);
		}
		fclose(fp);
		return 1;
	}
	else
		return 0;
}

// ============================================================
    int CStrList::LoadFromDelimText(LPCSTR fname,int conv)
// ============================================================
// conv=0 - without conversion
//     >0 - OEM to ANSI
//     <0 - ANSI to OEM
{
CString str;
FILE	*fp;
char	buf[1000];
	
	RemoveAll();
	fp=fopen(fname,"rt");
	if (fp)
	{
		while (fgets(buf,999,fp))
		{
			if (buf[0]==';' || buf[0]=='\n')	continue;
			str=buf;
			str.Replace('\r','\0');
			str.Replace('\n','\0');
			if (conv>0) str.OemToAnsi();
			if (conv<0) str.AnsiToOem();
			AddTail(str);
		}
		fclose(fp);
		return 1;
	}
	else
		return 0;
}

// ============================================================
    void CStrList::LoadFromDelimString(LPCSTR str,char delim,BOOL trim)
// ============================================================
{
LPCSTR p,t;
char	buf[1000];
	
	RemoveAll();
	p=t=str;
	while (*p)
	{
		if (*p==delim)
		{
			strncpy(buf,t,p-t);
			buf[p-t]=0;
			if (trim)
				trim_all(buf);
			AddTail(buf);
			t=p+1;
		}
		p++;
	}
	if (p!=t)
	{
		strncpy(buf,t,p-t);
		buf[p-t]=0;
		if (trim)
			trim_all(buf);
		AddTail(buf);
	}
}

// ============================================================
    void CStrList::Insert(int i,LPCSTR p)
// ============================================================
{
    InsertAfter(FindIndex(i),p);
}

// ============================================================
    void CStrList::InsertB(int i,LPCSTR p)
// ============================================================
{
    InsertBefore(FindIndex(i),p);
}

// ============================================================
    void CStrList::Remove(int i)
// ============================================================
{
	RemoveAt(FindIndex(i));
}

// ============================================================
    void  CStrList::UpdateListBox(CListBox *lb)
// ============================================================
{
POSITION pos;
CString	 str;
int		 i,count;

	count=GetCount();
	if (count<1)
	{
		lb->ResetContent();
		return;
	}
	lb->SetRedraw(0);
	lb->ResetContent();
	pos=GetHeadPosition();
	for (i=0;i<count;i++)
	{
		str=GetNext(pos);
		lb->AddString(str);
	}
	lb->SetRedraw(1);
}

// ============================================================
    void  CStrList::UpdateListBox(CListCtrl &lb)
// ============================================================
{
int		i,count;
POSITION pos;
	
	count=GetCount();
	if (count<1)
	{
		lb.DeleteAllItems();
		return;
	}
	lb.SetRedraw(0);
	lb.DeleteAllItems();
	pos=GetHeadPosition();
	for (i=0;i<count;i++)
		AddRow(lb,GetNext(pos));
	lb.SetRedraw(1);
}
	
// ============================================================
// Example: ("d:") or ("c:\winnt35")
    int  CStrList::FillWithDirs(LPCSTR basedir)
// ============================================================
{
WIN32_FIND_DATA finddata;
HANDLE   hfile;
int      count=0;
char     searchdir[500];

	make_path(searchdir,basedir,"*.*");
	RemoveAll();

	hfile = FindFirstFile(searchdir,&finddata);
	if (hfile != INVALID_HANDLE_VALUE)
	{
	    if (finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
		   if (strcmp(finddata.cFileName,"."))
			   if (strcmp(finddata.cFileName,".."))
			   {
				   count++;
				   AddTail(finddata.cFileName);
			   }
		}
		while (FindNextFile(hfile,&finddata))
		{
		    if (finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			   if (strcmp(finddata.cFileName,"."))
				   if (strcmp(finddata.cFileName,".."))
				   {
					   count++;
					   AddTail(finddata.cFileName);
				   }
		}
		FindClose(hfile);
	}
	return count;
}

// ============================================================
// Example: ("d:\\*.exe") or ("c:\\winnt35\\*.wav")
    int  CStrList::FillWithFiles(LPCSTR basedir)
// ============================================================
{
WIN32_FIND_DATA finddata;
HANDLE   hfile;
int      count=0;

	RemoveAll();
	hfile = FindFirstFile(basedir,&finddata);
	if (hfile != INVALID_HANDLE_VALUE)
	{
	    if (!(finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
			strcmp(finddata.cFileName,".") && strcmp(finddata.cFileName,".."))
		{
		   count++;
		   AddTail(finddata.cFileName);
		}
		while (FindNextFile(hfile,&finddata))
		{
		    if (!(finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
				strcmp(finddata.cFileName,".") && strcmp(finddata.cFileName,".."))
			{
			   count++;
			   AddTail(finddata.cFileName);
			}
		}
		FindClose(hfile);
	}
	return count;
}

// ============================================================
    int  CStrList::FillAsEdit(LPCSTR filename)
// ============================================================
{
FILE *fp=fopen(filename,"rt");
char *p;
char buf[1000];

	if (!fp)   return 0;
	RemoveAll();
	while (fgets(buf,999,fp))
	{
		p=strchr(buf,'\n');
		if (p)
			*p=0;
		AddTail(buf);
	}
	fclose(fp);
	return 1;
}

// ============================================================
    int  CStrList::SaveAsEdit(LPCSTR filename)
// ============================================================
{
FILE *fp=fopen(filename,"wt");
int	 cnt=GetCount();

	if (!fp)   return 0;
	for (int i=0;i<cnt;i++)
		fprintf(fp,"%s\n",GetString(i));
	fclose(fp);
	return 1;
}

// ============================================================
// delete all empty subdirs of basedir up to nested level
    void CStrList::DeleteEmptySubDirs(LPCSTR basedir)
// ============================================================
{
CString entry,str;
CStrList xx,yy;
char	buf[300],buf1[300];

  for (int i=0;i<GetCount();i++)
  {
	  entry=GetString(i);
	  make_path(buf,basedir,entry);
	  xx.FillWithDirs(buf);
	  for (int j=0;j<xx.GetCount();j++)
	  {
		  str=xx.GetString(j);
		  sprintf(buf1,"%s\\%s\\*.*",buf,str);
		  yy.FillWithFiles(buf1);
		  if (yy.GetCount()==0)                  // An empty dir
		  {
			  sprintf(buf1,"%s\\%s",buf,str);
			  RemoveDirectory(buf1);
		  }
	  }
	  sprintf(buf,"%s\\%s\\*.*",basedir,entry);
	  xx.FillWithFiles(buf);
	  if (xx.GetCount()==0)                  // An empty dir
	  {
		  sprintf(buf,"%s\\%s",basedir,entry);
		  RemoveDirectory(buf);
	  }
  }
}

// ============================================================
    int  CStrList::SplitWords(LPCSTR text)
// ============================================================
{
LPCSTR p;
char tmp[3000];
int  read;

    ASSERT(text);
 	RemoveAll();
	p=text;

	while (1)
	{
		read=0;
		if (sscanf(p,"%s%n",tmp,&read)!=1)
		   return (GetCount());
		AddTail(tmp);
		p+=read;
	}
}

// ============================================================
    int  CStrList::SplitTabs(char *text,char splitchar)
// ============================================================
// Splits a tabbed string into a CStrList
// Note: Last String has to be terminated with '\0', but not with a '\t'
{
char *plast;
char *p;
char buf[50000];
int  cnt;
char loctext[50000];
char txx[2];

	 ASSERT(text);

	 txx[0]=splitchar;
	 txx[1]=0;
	 strcpy(loctext,text);
	 strcat(loctext,txx);

	 RemoveAll();
	 p=plast=loctext;

     while (*p)
     {
          if (*p==splitchar)
          {
              cnt=p-plast;
			  ASSERT (cnt<500);
              memcpy(buf,plast,cnt);   buf[cnt]=0;
              AddTail(buf);
              plast=p+1;
          }
          p++;
     }

    // Append the last one
	cnt=p-plast;
	memcpy(buf,plast,cnt);	buf[cnt]=0;
	AddTail(buf);
	return (GetCount());
}

// =========================================================================
    void CStrList::BuildAsTabbedString(CStrList &lst,char splitchar)
// =========================================================================
// TOP: Cat's one or more
{
char term[2],buf[50000];
int  count;

	 RemoveAll();
	 strcpy(buf,"");

	 term[0]=splitchar; term[1]=0;
	 count=lst.GetCount();
	 for (int i=0;i<count;i++)
	 {
		strcat(buf,lst.GetString(i));
		if (i!=(count-1))
		    strcat(buf,term);
	 }
	 AddTail(buf);
}

// =========================================================================
	void CStrList::operator=(CListBox &lst)
// =========================================================================
{
CString str;

	RemoveAll();
	for (int i=0;i<lst.GetCount();i++)
	{
		lst.GetText(i,str);
		AddTail(str);
	}
}
	
// =========================================================================
	void CStrList::operator=(CListCtrl &lst)
// =========================================================================
{
CString str;

	RemoveAll();
	for (int i=0;i<lst.GetItemCount();i++)
	{
		str.Empty();
		for (int j=0;j<lst.GetHeaderCtrl()->GetItemCount();j++)
			str+=lst.GetItemText(i,j)+'\t';
		str.Delete(str.GetLength()-1);
		AddTail(str);
	}
}
	
// =========================================================================
    char *CStrList::GetFromTable(int strnbr,int tabnr,char splitchar)
// =========================================================================
{
CStrList yy;
static CString crashhelp;

	crashhelp.Empty();
	if (GetCount()<=0)
		return ((LPSTR)(LPCSTR)crashhelp);

	crashhelp=GetString(strnbr);
	yy.SplitTabs((LPSTR)(LPCSTR)crashhelp);
	crashhelp=yy.GetString(tabnr);
	return ((LPSTR)(LPCSTR)crashhelp);
}

// =========================================================================
    void CStrList::SplitPath(char *tosplit)
// =========================================================================
{
char drive	[100];
char dir	[300];
char fname	[300];
char ext	[100];

	 ASSERT(tosplit);
	 RemoveAll();
	 _splitpath(tosplit,drive,dir,fname,ext);
	 AddTail(drive);
	 AddTail(dir);
	 AddTail(fname);
	 AddTail(ext);
}

// =========================================================================
	int  HandleNonExistent(LPCSTR fname)
// =========================================================================
{
   if (!access(fname,0))	return 1;
   return extract_defaults(fname);
}

// =========================================================================
// extracts default settings from defaults.dat
	BOOL extract_defaults(LPCSTR fname)
// =========================================================================
{
FILE	*fp;
FILE	*fpo;
char	buf[300];
int		size;
int		found=0;

   fp=fopen("defaults.dat","rb");
   if (fp)
   {
	   while (fread(buf,256,1,fp)==1)
	   {
		   if (stricmp(fname,buf)==0)
		   {
				size=*(int *)&buf[20];
				fpo=fopen(fname,"wb");
				if (fpo)
				{
					fwrite(&buf[24],1,size,fpo);
					fclose(fpo);
					found=1;
				}
				else
					found=0;

				break;
		   }
 	   }
	   fclose(fp);
       return found;
   }
   return 0;
}

// =========================================================================
    int CStrList::convert_to_newer_format(char *filename)
// =========================================================================
// TOP: Hier konvertieren wir ein altes Format in das neue ...
{
char buf[50000];
FILE *fp;
char *p;
int  count,ret,i,magix;

	RemoveAll();
 	fp=fopen(filename,"rb");
	if (!fp)
		return 0;

	ret=fread(&count,sizeof(int),1,fp);  			
	if (ret!=1) 
	{	 
		fclose(fp);  
		return 0;	
	}
	if (count==MAGIC_FORMAT_ID)
	{
		fclose(fp);
		return 1;
	}

	ret=fread(&defaultindex,sizeof(int),1,fp);		
	if (ret!=1)	 
	{	
		fclose(fp);	 
		return 0; 
	}
	ret=fread(&defaultindexnew,sizeof(int),1,fp);	
	if (ret!=1)	 
	{ 	
		fclose(fp);	 
		return 0; 
	}

	for (i=0;i<count;i++)
	{
		p=fgets(buf,49999,fp);
		if (!p)
		{
		   fclose(fp);
		   return 0;
		}
 		buf[strlen(buf)-1]=0;
		AddTail(buf);
	}
	fclose(fp);

	install_date=install_checksum=reserved3=reserved4=reserved5=0;

	set_date_check(&install_date,&install_checksum);

	fp=fopen(filename,"wb");
	if (!fp)
		return 0;

	count=GetCount();

	magix = MAGIC_FORMAT_ID;
	ret=fwrite(&magix,sizeof(int),1,fp);			if (ret!=1)	 { 	fclose(fp);	return 0; 	}
	ret=fwrite(&count,sizeof(int),1,fp);			if (ret!=1)	 { 	fclose(fp);	return 0; 	}
	ret=fwrite(&defaultindex,sizeof(int),1,fp);		if (ret!=1)	 { 	fclose(fp);	return 0; 	}
	ret=fwrite(&defaultindexnew,sizeof(int),1,fp);	if (ret!=1)	 { 	fclose(fp);	return 0; 	}
	ret=fwrite(&install_date,		sizeof(int),1,fp);	if (ret!=1)	 { 	fclose(fp);	return 0; 	}
	ret=fwrite(&install_checksum,	sizeof(int),1,fp);	if (ret!=1)	 { 	fclose(fp);	return 0; 	}
	ret=fwrite(&reserved3,sizeof(int),1,fp);	if (ret!=1)	 { 	fclose(fp);	return 0; 	}
	ret=fwrite(&reserved4,sizeof(int),1,fp);	if (ret!=1)	 { 	fclose(fp);	return 0; 	}
	ret=fwrite(&reserved5,sizeof(int),1,fp);	if (ret!=1)	 { 	fclose(fp);	return 0; 	}

	for (i=0;i<count;i++)
		fprintf(fp,"%s\n",(LPCSTR)GetAt(FindIndex(i)));

	fclose(fp);
	return 1;
}

// =========================================================================
    int CStrList::set_date_check(int *pinstall_date,int *pinstall_checksum)
// =========================================================================
{
unsigned int help;

	if (*pinstall_date)
		return 1;

	help=time(NULL);
	*pinstall_date=help;
	*pinstall_checksum=(~help / 0x24254)+0x4263;
	return 1;
}

// =========================================================================
    void CStrList::AddMenu(CString eng,CString ger,CString rus,CString fre,int resID,int flags)
// =========================================================================
{
CString str;

	str.Format("%s\t%s\t%s\t%u\t%u",eng,ger,rus,fre,resID,flags);
	AddTail(str);
}

// =========================================================================
    void CStrList::DoMenu(POINT pt,CWnd *wndptr)
// =========================================================================
{
CMenu	mnu;
CString text,res,flg,line;
int		i,resID=0,flags=0;

	mnu.CreatePopupMenu();
	for (i=0;i<GetCount();i++)
	{
		line=GetString(i);
		get_token(line,LANG_ID,text);
		get_token(line,4,res);
		get_token(line,5,flg);
		sscanf(res,"%u",&resID);
		sscanf(flg,"%u",&flags);

		if (text=="SEPARATOR")
 				mnu.AppendMenu(MF_SEPARATOR);
		else
		{
			if (reserved3==99)
				mnu.AppendMenu(flags|MF_DISABLED|MF_GRAYED,resID,text);
			else
				mnu.AppendMenu(flags,resID,text);
		}
	}
	if (defaultindex==99)
		mnu.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,pt.x-10,pt.y-10,wndptr,NULL);
	else
		mnu.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,pt.x,pt.y,wndptr,NULL);
}

// =========================================================================
    void CStrList::Sort(int sortindex,int sort_numeric)
// =========================================================================
{
_btree *root=0;
CString dd;
int		i,x;

	x=GetCount();
	if (x<1)
		return;
	for (i=0;i<x;i++)
	{
		dd=GetHead();
		AddItemForSort(&root,PS dd,dd.GetLength(),sortindex,sort_numeric);
		RemoveHead();
	}
	RebuildList(this,&root);
}

// =========================================================================
    void RebuildList(CStrList *me,_btree ** root)
// =========================================================================
{
	if ((*root)->left)
		RebuildList(me,&((*root)->left));

	me->AddTail((*root)->data);

	if ((*root)->right)
		RebuildList(me,&((*root)->right));

	if ((*root)->data)
		free((*root)->data);
	if (*root)
		free(*root);
}

// =========================================================================
	void AddItemForSort(_btree ** root,char *data,int datalen,int sortindex,int sort_numeric)
// =========================================================================
{
CString help,help2;
int     val1,val2;

	if (*root==0)
	{
		*root = (_btree *)malloc(sizeof(_btree));
		if (!*root)	return;

		memset(*root,0,sizeof(_btree));

		(*root)->data=(char *)malloc(datalen+1);
		if (!((*root)->data)) return;

		memcpy((*root)->data,data,datalen);
		(*root)->data[datalen]=0;
		return;
	}

	get_token(data,sortindex,help);
	get_token(PS (*root)->data,sortindex,help2);

	if (!sort_numeric)
	{
		if (/*strcmpi(PS help,PS help2)>0)*/help.CompareNoCase(help2)>0)
			AddItemForSort(&((*root)->right),data,datalen,sortindex,sort_numeric);
		else
			AddItemForSort(&((*root)->left),data,datalen,sortindex,sort_numeric);
	}
	else
	{
		val1=atoi(help);
		val2=atoi(help2);

		if (val1>val2)
			AddItemForSort(&((*root)->right),data,datalen,sortindex,sort_numeric);
		else
			AddItemForSort(&((*root)->left),data,datalen,sortindex,sort_numeric);
	}

	return;
}


// ============================================================
    int CStrList::BuildFromMemoryString(char *text,char *split)
// ============================================================
{
int count=0;
char *p;
char *p1;
char buf[10000];

	RemoveAll();

	if (!text)
		goto ende;
	if (strlen(text)<=0)
		goto ende;

	p1=p=text;
	while (p1)
	{
		p1=strstr(p,split);
		if (p1)
		{
			if (p1==p)
			{
				AddTail("");	// Eine Leerzeile ...
				p=p1+strlen(split);
				if (!*p)
					break;
				else
					continue;
			}
			else
			{
				memcpy(buf,p,p1-p);
				buf[p1-p]=0;
				AddTail(buf);
				p=p1+strlen(split);
				if (!*p)
					break;
			}
		}
		else
		{
			AddTail(p);
			break;
		}
	}
ende:
	count=GetCount();
	return (count);
}

// =========================================================================
    int CStrList::EnumRegistryKeys(HKEY whichtree,char *treepath)
// =========================================================================
// TOP: Diese Funktion fuellt die CStrList mit allen vorhandenen
// Subkeys im Ragistry Baum ...
{
HKEY hkey	=0;
int  ret	=0;
int  dwIndex=0;
char buf[300];

	RemoveAll();
	ret=RegOpenKeyEx(
					whichtree,			// handle of open key
					treepath,			// address of name of subkey to open
					0,					// reserved
					KEY_ALL_ACCESS,		// security access mask
					&hkey); 			// address of handle of open key

	if (ret!=ERROR_SUCCESS)
		return 0;

	for (dwIndex=0;;dwIndex++)
	{
		memset(buf,0,sizeof(buf));
		ret=RegEnumKey	(
							hkey,				// handle of key to query
							dwIndex,			// index of subkey to query
							buf,				// address of buffer for subkey name
							sizeof(buf)-1); 	// size of subkey buffer

		if (ret!=ERROR_SUCCESS)
			break;
		AddTail(buf);
	}

	if (hkey)
	{
		RegCloseKey (hkey);   // Close the key handle.
		hkey=0;
	}
	return 1;
}

// =========================================================================
    int CStrList::EnumStringKeyValues(HKEY whichtree,char *treepath)
// =========================================================================
// TOP: Diese Funktion holt alle vorhandenen String Eintraege fuer einen
// Speziellen Key aus der Registry und leght sie im Format Keyname\tParamter ab
{
HKEY			hkey		=0;
int				ret			=0;
int				dwIndex		=0;
unsigned long	datasize	=0;
unsigned long	valuesize	=0;
DWORD			type		=0;
char			ValueName	[500];
char			Data  		[500];
char			buf			[500];

	RemoveAll();
	ret=RegOpenKeyEx	(
						whichtree,			// handle of open key
						treepath,			// address of name of subkey to open
						0,					// reserved
						KEY_ALL_ACCESS,		// security access mask
						&hkey); 			// address of handle of open key

	if (ret!=ERROR_SUCCESS)
		return 0;

	for (dwIndex=0;;dwIndex++)
	{
		datasize	=sizeof(Data)-1;
		valuesize	=sizeof(ValueName)-1;
		type		=0;
		memset(ValueName,0,sizeof(ValueName));
		memset(Data,0,sizeof(Data));
		ret=RegEnumValue(
						hkey,					// handle of key to query
						dwIndex,				// index of value to query
						ValueName,				// address of buffer for value string
						&valuesize,				// address for size of value buffer
						0,						// reserved
						&type,					// address of buffer for type code
						(unsigned char *)Data,	// address of buffer for value data
						&datasize 				// address for size of data buffer
				   );

		if (ret!=ERROR_SUCCESS)
			break;

		if (type==REG_SZ)
		{
			sprintf(buf,"%s\t%s",ValueName,Data);
			AddTail(buf);
		}

	}
	if (hkey)
	{
		RegCloseKey (hkey);   // Close the key handle.
		hkey=0;
	}
	return 1;
}


// ======================================================================
// returns index of line, if ind element in it is equal sample, or -1 else
// found line returns in result
    int CStrList::FindString(LPCSTR sample,CString &result,int ind,int ignorecase)
// ======================================================================
{
CString	line,str;

	result.Empty();
	if (*sample==0)	return -1;

	for (int i=0;i<GetCount();i++)
	{
		line=GetString(i);
		get_token(line,ind,str);
		if ((!stricmp(str,sample) && ignorecase) || (!strcmp(str,sample) && !ignorecase))
		{
			result=line;
			return i;
		}
	}
	return -1;
}

// ======================================================================
    int CStrList::AppendFileInfo(char *basepath)
// ======================================================================
// TOP: Diese Funktion haengt an alle uebergebenen Dateinamen das Datum hinten dran
{
CString help;
CString compath;
int i;
int count;
struct _stat stat;
char buf[300];
char buf1[300];
FILE *fp;
long len;


	count=GetCount();

	for (i=0;i<count;i++)
	{
		help=GetString(i);
		compath=basepath;
		compath+="\\";
		compath+=help;
		_stat(PS compath,&stat);
		buf[0]='\t';
		buf[1]=',';
		unix_time_to_fido(stat.st_mtime,&buf[2]);
		buf[11]=0;
		help+=buf;
		help+=",";


		// Jetzt haengen wir noch die Laenge hinten dran

		fp=fopen(PS compath,"rt");
		if (fp)
		{

			len=_filelength(_fileno(fp));
			sprintf(buf1,"\t%dK",len/1000);
			fclose(fp);
		}
		help+=buf1;
		Insert(i,PS help);
		Remove(i);
	}
	return 1;
}


// ======================================================================
    int CStrList::HandleAsciiExport(LPCSTR pathname,int fromload)
// ======================================================================
// Diese Funktion testet ob für einen bestimmten Namen eine entsprechende .asc Datei existiert
// und exportiert diese dann gegebenfalls
{
CStrList xx;
CString line;
CString line1;
FILE	*fp;
char	name[500];
char	*p;
int		i,n;
int		len;

	strcpy(name,pathname);
	p=strstr(name,".cfg");
	if (!p)
		return 0;

	strcpy(p,".asc");

	if (access(name,0))
		return 0;			// Kein ascii export file vorhanden, also vergiss es ...


	if (fromload)
	{
		fp=fopen(name,"rb");
		if (fp)
		{
			len=_filelength(_fileno(fp));
			fclose(fp);
		}
		else
			return 0;

		if (len>20)
			return 0;
	}

	// Ansonsten exportieren wir da unsere aktuelle config rein ..

	fp=fopen(name,"wt");
	if (fp)
	{
		fprintf(fp,"Count=%d\n",GetCount());
		fprintf(fp,"INT1=%d\n",defaultindex);
		fprintf(fp,"INT2=%d\n",defaultindexnew);
		fprintf(fp,"INT3=%d\n",reserved3);
		fprintf(fp,"INT4=%d\n",reserved4);
		fprintf(fp,"INT5=%d\n",reserved5);

		for (i=0;i<GetCount();i++)
		{
			fprintf(fp,"\n[%d]\n",i+1);
			line=GetString(i);
			xx.SplitTabs(PS line,'\t');
			for (n=0;n<xx.GetCount();n++)
			{
				line1=xx.GetString(n);
				fprintf(fp,"%s|\n",PS line1);
			}
		}
		fclose(fp);
	}
	return 1;
}

// ======================================================================
	void CStrList::Replace(int i, LPCSTR p)
// ======================================================================
{
	Insert(i,p);
	Remove(i);
}
