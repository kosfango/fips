// This File contains the whole stuff for Mail Conversion
#include "stdafx.h"
#include "lightdlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

int is_same_senders		(LPCSTR line1,LPCSTR line2);
int is_already_quoted	(LPCSTR line,LPSTR head);
int is_extra_spaces		(LPCSTR line);

CStrList divided;

// =========================================================================
    void CLightDlg::make_string_list(LPSTR text,CStrList &lst)
// =========================================================================
{
char *p,*plast,*pend;
char tmp[MAX_MSG];
int  len;

	lst.RemoveAll();
	p=plast=text;
	pend=p+strlen(text);

	p=strstr(plast,"\r\n");
	while (p && p<pend)
	{
		memcpy(tmp,plast,p-plast);
		tmp[p-plast]=0;
		if (split_long_lines(tmp))
			lst.AddTail(&divided);
		else
			lst.AddTail(tmp);
		plast=p;
		p+=2 ;
		if (p<=pend)
			plast=p;
		p=strstr(plast,"\r\n");
 	}

	*tmp=0;
	len=pend-plast;
	if (len)
	{
		memcpy(tmp,plast,len);
		tmp[len]=0;
		p=strstr(tmp,"\r\n");
		if (p)
		   *p=0;
		if (split_long_lines(tmp))
			lst.AddTail(&divided);
		else
			lst.AddTail(tmp);
	}
}

// =========================================================================
	int split_long_lines(LPSTR tmp)
// =========================================================================
// Funktion splittet lange Zeilen (>80 Chars) in mehrere einzelne
{
char buf[300];
int  x,xlast;

   if (strlen(tmp)<=80)
       return 0;			// Kein Spit notwendig

   divided.RemoveAll();

anfang:
   if (strlen(tmp)<=80)
   {
	   if (strlen(tmp)>0)
		   divided.AddTail(tmp);
       return 1;			// Kein weiterer Spit mehr notwendig
   }

   xlast=0;
   for (x=0;x<82;x++)
   {
       if ((tmp[x]==' ') ||	(tmp[x]=='\0'))
		   xlast=x;
   }

   if (xlast <50)
       xlast=75;

   memcpy(buf,tmp,xlast);
   buf[xlast]=0;
   divided.AddTail(buf);

   tmp+=xlast;
   if (*tmp==' ')
      tmp++;
   goto anfang;
}

// =================================================================================================
// returns string from initials from latin and cyrillic charsets
	const char get_first_alphnum(LPCSTR str)
// =================================================================================================
{
LPCSTR  p;
UCHAR	c;

	if (p=str)
		while (c=*p++)
			if (isalnum(c) || (c>0x7F && c<0xB0) || c>0xDF) return c;
	return 0;
}

// =================================================================================================
// converts source text into quoted text
	void make_quoted_text(LPCSTR src,CString &dst,char *sender)
// =================================================================================================
{
CStrList qu;
CString	str,str1,str2,name;
LPCSTR s,t;
char	line[MAX_MSG],line2[MAX_MSG],buf[100],*p,c;
char	dm0[100],dm1[100],dm2[100],dm3[100],dm4[100];
int		i,len,breaked,brk,num;

	len=strlen(src);
// make quote header
	*dm0=*dm1=*dm2=0;
	num=sscanf(sender,"%s %s %s %s %s",dm0,dm1,dm2,dm3,dm4);
	if (num>0 && (c=get_first_alphnum(dm0)))	name=c;
	if (num>1 && (c=get_first_alphnum(dm1)))	name+=c;
	if (num>2 && (c=get_first_alphnum(dm2)))	name+=c;
	if (num>3 && (c=get_first_alphnum(dm3)))	name+=c;
	if (num>4 && (c=get_first_alphnum(dm4)))	name+=c;
	name+='>';
	name.OemToAnsi();
// read lines into string list, skipping doubled empty lines
	t=src;
	i=0;
	while (s=strchr(t,'\n'))	// till newline
	{
	  s++;
		if (*t=='\r' || *t=='\n')
			i++;
		else
			i=0;
		if (i < 2)
		{
			strncpy(line,t,s-t);
			line[s-t]=0;
			change_mm_action(line);
			qu.AddTail(line);
		}
		t=s;
	}
// add rest of mailtext (not ending with '\n')
	if (*t)
	{
		if (*t=='\r' || *t=='\n')
			i++;
		else
			i=0;
		if (i < 2)
		{
			strncpy(line,t,MAX_MSG-1);
			line[MAX_MSG-1]=0;
			change_mm_action(line);
			qu.AddTail(line);
		}
	}

	do
	{
		breaked=0;
		for (i=0;i<qu.GetCount();i++)
		{
			str=qu.GetString(i);
			if (str.GetLength()>75)	// need to split line?
			{
				breaked=1;
				strcpy(line,str);
				for (brk=73;brk>24;brk--)
					if (line[brk-1]==' ')	break;

				if (brk < 25)	brk=65;
// split line
				str1=line+brk;	// rest of line after break point
				p=strchr(line,'>');
				if (p && (p-line)<5)
				{
					while (*(++p)=='>');
					if (*p==' ')	p++;
					strncpy(buf,line,p-line);	// copy quote header
					buf[p-line]=0;
					str1=buf+str1;		// add quote header to rest of line
 				}

				line[brk]=0;
				str=line;
				str+="\r\n";

				qu.Insert(i,str1);
				qu.Replace(i,str);

				str=qu.GetString(i+1);
				if (str.GetLength() < 40 && qu.GetCount() > i+2)
				{
					 str1=qu.GetString(i+2);
					 if (str1.GetLength() > 2 && is_same_senders(str,str1))
					 {
							p= (char *) strchr(str1,'>');
							if (p && p-(PS str1) <5)
								while (*p && *p++=='>');
							else
								p=PS str1;

						  str2=p;
						  strcpy(line2,str);
						  p=strchr(line2,'\r');   
						  if (p)	*p=0;
						  p=strchr(line2,'\n');   
						  if (p)	*p=0;
						  p=line2+strlen(line2);
						  while (p > line2 && (*(p-1)==' ' || *(p-1)=='\t'))	*(--p)=0;
						  str=line2;
						  str+=" "+str2;
						  qu.Replace(i+1,str);
						  qu.Remove(i+2);
					}
				}
   		}
		}
	} while (breaked);

// add quote sign and form resulting text
	dst.Empty();
	for (i=0;i<qu.GetCount();i++)
	{
		str=qu.GetString(i);
		brk=str.Find('>');
		if (brk<0 || brk>4)
		{
			if(str.GetLength()>2) 
			  dst+=name+" "+str;
			else
				dst+=str; 
		}
		else
		{
			str.Insert(brk,">");
			str.TrimLeft();
			dst+=str; 
		}
	}
}

// =========================================================================
// returns true for lines with same sender or not quoted and having no extra
// spaces in second line
	int is_same_senders(LPCSTR line1,LPCSTR line2)
// =========================================================================
{
char  head1[10],head2[10];
int  q1,q2;

	q1=is_already_quoted(line1, head1);
	q2=is_already_quoted(line2,head2);
	if (!q1 && !q2)	return !is_extra_spaces(line2);
	if (q1 && q2 && !strcmp(head1,head2))	return !is_extra_spaces(line2);
	return 0;
}

// =========================================================================
// returns quote header and true for quoted lines
	int is_already_quoted(LPCSTR line,LPSTR head)
// =========================================================================
{
LPCSTR p;

	p=strchr(line,'>');
	if (!p || p-line > 4)	return 0;	// not quoted line
	while(*(++p)=='>');
	strncpy(head,line,p-line);
	head[p-line]=0;
	return 1;
}

// =========================================================================
// returns true if there are more than 4 spaces before text (may be quoted)
	int is_extra_spaces(LPCSTR line)
// =========================================================================
{
LPCSTR p;
int  count;

	p=strchr(line,'>');
	if (p && p-line < 5)
		while (*(++p)=='>');
	else
		p=line;

	count=0;
	while (*p==' ' || *p=='\t')
	{
		count++;
		p++;
	}
	return count>4;
}
