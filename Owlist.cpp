// custlist.cpp : custom listbox
//

#include "stdafx.h"
#include "owlist.h"

extern long		gColors[20];	// global colortable
extern int		SubjPos[3];		// positions in subject list
extern CByteArray AttrListBuffer;
extern CStrList patterns_list;
extern _gconfig gc;
void	out_attributed_line		(CDC *pDC,int left,int top,char *line);
int		get_next_nearest_value	(int);

extern CFont   font_mailtext;
extern CFont   font_mail_under;
extern CFont   font_mail_italic;
extern CFont   font_under_italic;

int  VIPNamesCount=26;
//tw: added anyone ever wrote anything for FIPS (afaik) ...
char *VIPNames[]=
{
"FIPS development",
"Alex Drugov",
"Alex Kuzmin",
"Almaz Sharipov",
"Bodo Priesterath",
"Carsten Martens",
"Christian Koelliker",
"Constantin Roganov",
"Dennis Slagers",
"Denis Korablev",
"Evgeny Karandaev",
"Johann Weinzierl",
"Lidia Tretyakova",
"Lothar Lindinger",
"Martin Roggon",
"Niko Fakitsas",
"Norbert Warnke",
"Oleg Fomin",
"Oliver Weindl",
"Sascha Beutler",
"Sean Rima",
"Sergey Bynkov",
"Stefan Machwirth",
"Thomas Winter",
"Vladimir Kann",
"Vyacheslav Kvatkovskiy",
"Wilhelm Smits"
"Yuri Burawtsow"
};

// =======================================================================================
	void CColorListBox::DrawItem(LPDRAWITEMSTRUCT lpDIS)
// =======================================================================================
{
CBrush	brT(gColors[TEXT_BACKGR_COLOR]),brR(RGB(255,0,0));
CFont	*poldf=0;
RECT	re;
SIZE	size;
char	*line,*p;
int		setback=0,wasorigin=0,offsets[200],ret,plen;

	CDC *pDC = CDC::FromHandle(lpDIS->hDC);
	line = (char *)lpDIS->itemData;       // RGB in item data
	if (!line)	return;

	handle_mm_stuff(line,1);
	if (lpDIS->itemAction & ODA_DRAWENTIRE)
	{
		pDC->SetBkMode(TRANSPARENT);
		if (!type || gc.colored)
		{
			if (gc.colored)
				pDC->SetTextColor(gColors[TEXT_COLOR]);

			if (*line==0x01 || !strncmp(line,"SEEN-BY",7))	// '\001' or "SEEN-BY"
				pDC->SetTextColor(gColors[KLUDGE_COLOR]);
			p=strchr(line,'>');
			if (p && p-line<6)
			{
				int b=true;
				if (gc.altcolored)
				{
					p++;
					while (*p && *p++=='>')
						b=!b;
				}
				pDC->SetTextColor(gColors[b ? ODD_QUOTE_COLOR : EVEN_QUOTE_COLOR]);
			}

			if (!strncmp(line," * Origin:",10))
			{
				pDC->SetTextColor(gColors[ORIGIN_COLOR]);
				wasorigin=1;
			}
		}
		else
			pDC->SetTextColor(RGB(0,0,0));	   // default black

		if (gc.isrot13)	// Handle ROT13 Decoding ...
		{
			p=line;
			while (*p)
			{
				if (*p >= 'a' && *p <= 'z')
					*p='a'+(*p-'a'+13)%26;

				if (*p >= 'A' && *p <= 'Z')
					*p='A'+(*p-'A'+13)%26;
				p++;
			}
		}

		poldf=pDC->SelectObject(&font_mailtext);
		if (!gc.display_attributes || wasorigin)
			pDC->TextOut(lpDIS->rcItem.left+10,lpDIS->rcItem.top+1,line,strlen(line));
		else
			out_attributed_line(pDC,lpDIS->rcItem.left+10,lpDIS->rcItem.top+1,line);
// show found patterns
		if (gc.show_result && gc.display_this_mail_only)
		{
			plen=strlen(gc.search_pattern);
	    ret=find_next_entry(line,!gc.ignorecase,gc.search_pattern,offsets,199,gc.userconv,!gc.ViewOem);
			for (int i=0;i<ret;i++)
			{
				GetTextExtentPoint32(pDC->m_hDC,line,offsets[i],&size);
				memcpy(&re,&lpDIS->rcItem,sizeof(RECT));
				re.left+=size.cx+10;
				GetTextExtentPoint32(pDC->m_hDC,&line[offsets[i]],plen,&size);
				re.right=re.left+size.cx;
				re.bottom=re.top+size.cy;
				pDC->InvertRect(&re);
  		}
		}
	}

	memcpy(&re,&lpDIS->rcItem,sizeof(RECT));
	re.left+=1;
	re.top+=2;
	re.bottom-=2;
	re.right=re.left+7;
	if ((lpDIS->itemState & ODS_SELECTED) && (lpDIS->itemAction & (ODA_SELECT | ODA_DRAWENTIRE)))
	{
		// item has been selected - hilite frame
		if (gc.colored || !type)
			pDC->FillRect(&re, &brR);
		else
			pDC->InvertRect(&re);

	}

	if (!(lpDIS->itemState & ODS_SELECTED) && (lpDIS->itemAction & ODA_SELECT))
	{
		// Item has been de-selected -- remove frame
		if (gc.colored || !type)
			pDC->FillRect(&re, &brT);
		else
			pDC->InvertRect(&re);
	}

	if (poldf)
		pDC->SelectObject(poldf);
}

// =======================================================================================
	void CColorListBox::MeasureItem(LPMEASUREITEMSTRUCT lpMIS)
// =======================================================================================
{
	// all items are of fixed size
	// must use LBS_OWNERDRAWVARIABLE for this to work
	lpMIS->itemHeight = itemhigh;								// CListbox
}

// =======================================================================================
	int CColorListBox::CompareItem(LPCOMPAREITEMSTRUCT lpCIS)
// =======================================================================================
{
   return 1;  // all equal, since we do not need sorting
}

// =======================================================================================
	void out_attributed_line(CDC *pDC,int left,int top,char *line)
// =======================================================================================
{
char	*p,*t;
RECT	re;
SIZE	size;
int		sumoffset=0,under=0,italic=0,bold=0,invert=0,wasdelim=1;
int		forecolor;
size_t	ind;

	ind=strcspn(line,"*_/#");
	if (ind==strlen(line))
	{
		pDC->TextOut(left,top,line,strlen(line));	// nothing found, normal text
		return;
	}
//	p=line+ind;
	forecolor=pDC->GetTextColor();
	p=line;
	while (*p)
	{
		if (strchr(" \r\n",*p))
		{
			wasdelim=1;
		}
		else if (strchr(ATTRS,*p))
		{
			switch (*p)
			{
				case '/':
					 if (italic)
					 {
						italic=0;
						p++;
						continue;
					 }
					 else if (wasdelim)
					 {
						 t=strchr(p+1,'/');
						 if (t>p+1 && (p<line+2 || strncmp(p-2,":/",2)))	// not empty pattern and web-addr
						 {
							 t++;
							 while (*t && strchr(ATTRS,*t))
								 t++;
							 if (strchr(DELIMS,*t))	// is closing delim?
							 {
								 italic=1;
								 p++;
								 continue;
							 }
						 }
					 }
					 break;
				case '_':
					 if (under)
					 {
						under=0;
						p++;
						continue;
					 }
					 else if (wasdelim)
					 {
						 t=strchr(p+1,'_');
						 if (t>p+1)
						 {
							 t++;
							 while (*t && strchr(ATTRS,*t))
								 t++;
							 if (strchr(DELIMS,*t))
							 {
								 under=1;
								 p++;
								 continue;
							 }
						 }
					 }
					 break;
				case '*':
					 if (bold)
					 {
						bold=0;
						p++;
						continue;
					 }
					 else if (wasdelim)
					 {
						 t=strchr(p+1,'*');
						 if (t>p+1)
						 {
							 t++;
							 while (*t && strchr(ATTRS,*t))
								 t++;
							 if (strchr(DELIMS,*t))
							 {
								 bold=1;
								 p++;
								 continue;
							 }
						 }
					 }
					 break;
				case '#':
					 if (invert)
					 {
						invert=0;
						p++;
						continue;
					 }
					 else if (wasdelim)
					 {
						 t=strchr(p+1,'#');
						 if (t>p+1)
						 {
							 t++;
							 while (*t && strchr(ATTRS,*t))
								 t++;
							 if (strchr(DELIMS,*t))
							 {
								 invert=1;
								 p++;
								 continue;
							 }
						 }
					 }
					 break;
			}
		}
		else
			wasdelim=0;
		
		if (italic && under)
			pDC->SelectObject(&font_under_italic);
		else if (italic)
			pDC->SelectObject(&font_mail_italic);
		else if (under)
			pDC->SelectObject(&font_mail_under);
		else
			pDC->SelectObject(&font_mailtext);

		if (bold)
			pDC->SetTextColor(gColors[BOLD_COLOR]);
		else
			pDC->SetTextColor(forecolor);

		pDC->TextOut(left+sumoffset,top,p,1);
		GetTextExtentPoint32(pDC->m_hDC,p,1,&size);
		if (invert)
		{
			re.left=sumoffset+left;
			re.right=re.left+size.cx;
			re.top=top;
			re.bottom=top+size.cy;
			pDC->InvertRect(&re);
		}
		sumoffset+=size.cx;
		p++;
	}
}

// ==========================================================================================
// ======================  OWNERDRAW FOR SUBJECT ============================================

// =======================================================================================
	void CColorListBoxSubject::DrawItem(LPDRAWITEMSTRUCT lpDIS)
// =======================================================================================
{
CString ignore;
CPen    cp;
CPen    *oldpen;
char	*line,*x;
char	buf[1000],buf1[1000],writername[300],ts[300];
int		offsets[20];
int		important=0,offs=0,invert=0;
int		ign,color,i,nn;
int		ih=itemhigh/2;
char	ch;

	CDC* pDC = CDC::FromHandle(lpDIS->hDC);
	line = (char *)lpDIS->itemData;       // RGB in item data

	if (type==ATTR_LIST)
	{
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(gColors[SUBJ_COLOR]);
		ch=AttrListBuffer[lpDIS->itemID];
		invert=ch & 0x80;
		ch &= 0x7F;
		pDC->TextOut(lpDIS->rcItem.left,lpDIS->rcItem.top,&ch,1);
		if (invert)			// DB_USERMARKED
			pDC->InvertRect(&lpDIS->rcItem);
		return;
	}

	if (lpDIS->itemAction & ODA_DRAWENTIRE)
	{
		pDC->SetBkMode(TRANSPARENT);
		for (i=2;i<patterns_list.GetCount();i++)	// skip first two sample patterns
		{
			strcpy(buf,patterns_list.GetString(i));
			get_token(buf,1,buf1);		// pattern
			get_token(buf,2,ignore);	// ignoring register
			if (*ignore && (*ignore=='i' || *ignore=='è'))
				ign=1;
			else
				ign=0;

			if (find_next_entry(line,!ign,buf1,offsets,1,0,TRUE))
			{
				strcpy(buf,patterns_list.GetString(i));
				get_token(buf,3,buf1);	// color
				sscanf(buf1,"H%x",&color);
		   		pDC->SetTextColor(color);
				goto done;
			}
		}

		pDC->SetTextColor(gColors[SUBJ_COLOR]);
done:
		get_token(line,1,ts);
		strcpy(writername,ts);

		#define EDIST 12	// width of nest arrow

		x=ts;
		offs=0;
		while (*x=='#')	// align entry
		{
		   offs+=EDIST;
		   x++;
		}

		nn=get_next_nearest_value(offs);
	    limit_text_len(x,nn-offs-3,pDC);
		pDC->TextOut(lpDIS->rcItem.left+10+offs,lpDIS->rcItem.top,x,strlen(x));

		// second String
		nn=get_next_nearest_value(SubjPos[0]);
		get_token(line,2,ts);
	    limit_text_len(ts,nn-SubjPos[0]-6,pDC);
		pDC->TextOut(lpDIS->rcItem.left+SubjPos[0],lpDIS->rcItem.top,ts,strlen(ts));

		// third String
		nn=get_next_nearest_value(SubjPos[1]);
		get_token(line,3,ts);

		// test for IMPORTANTMAIL event
		x=writername;
		if (x && *x=='#')
			x++;

		if (strstr(ts,IMPORTANT_PREFIX) && strlen(ts)<290 && 
			strnicmp(ts,"RE:",3) && is_important_name(x))
				important=1;

	    limit_text_len(ts,nn-SubjPos[1]-6,pDC);
		pDC->TextOut(lpDIS->rcItem.left+SubjPos[1],lpDIS->rcItem.top,ts,strlen(ts));

		nn=get_next_nearest_value(SubjPos[2]);
		get_token(line,4,ts);
	    limit_text_len(ts,nn-SubjPos[2]-6,pDC);
		pDC->TextOut(lpDIS->rcItem.left+SubjPos[2],lpDIS->rcItem.top,ts,strlen(ts));

		cp.CreatePen(PS_SOLID,1,gColors[NEST_COLOR]);
		oldpen=pDC->SelectObject(&cp);

		if (offs>0)
		{
			// draw nest arrow
			pDC->MoveTo(lpDIS->rcItem.left+offs-EDIST+13,	lpDIS->rcItem.top+ih);
			pDC->LineTo(lpDIS->rcItem.left+offs-3+10,		lpDIS->rcItem.top+ih);
			pDC->MoveTo(lpDIS->rcItem.left+offs-EDIST+13,	lpDIS->rcItem.top+ih);
			pDC->LineTo(lpDIS->rcItem.left+offs-EDIST+13,	lpDIS->rcItem.top);

			// small triangle
			pDC->MoveTo(lpDIS->rcItem.left+offs-EDIST+13,	lpDIS->rcItem.top);
			pDC->LineTo(lpDIS->rcItem.left+offs-EDIST+15,	lpDIS->rcItem.top+ih-1);
			pDC->MoveTo(lpDIS->rcItem.left+offs-EDIST+13,	lpDIS->rcItem.top);
			pDC->LineTo(lpDIS->rcItem.left+offs-EDIST+11,	lpDIS->rcItem.top+ih-1);
		}
		else
		{
			if (gc.structured_display)
			{
				pDC->MoveTo(lpDIS->rcItem.left+3,lpDIS->rcItem.top+ih);
				pDC->LineTo(lpDIS->rcItem.left+4,lpDIS->rcItem.top+ih);
			}
		}

		if (important)
			pDC->InvertRect(&lpDIS->rcItem);

		pDC->SelectObject(oldpen);
		cp.DeleteObject();

	}

	if ((lpDIS->itemState & ODS_SELECTED) && (lpDIS->itemAction & (ODA_SELECT | ODA_DRAWENTIRE)))
	{// item has been selected - hilite frame
		CBrush br(RGB(255,0,0));
		RECT re;
		
		memcpy(&re,&lpDIS->rcItem,sizeof(RECT));
		re.left+=1;
		re.top+=2;
		re.bottom-=2;
		re.right=re.left+7;

		pDC->FillRect(&re,&br); // CDC
	}

	if (!(lpDIS->itemState & ODS_SELECTED) && (lpDIS->itemAction & ODA_SELECT))
	{// Item has been de-selected -- remove frame
		CBrush br(gColors[SUBJ_BACKGR_COLOR]);
		RECT re;
		memcpy(&re,&lpDIS->rcItem,sizeof(RECT));

		re.left+=1;
		re.top+=2;
		re.bottom-=2;
		re.right=re.left+7;
		pDC->FillRect(&re, &br);

		get_token(line,1,ts);

		if (strstr(line,IMPORTANT_PREFIX))
		{
			x=ts;
			if (x && *x=='#')
				x++;

			if (strlen(ts)<290 && !strnicmp(ts,"RE:",3) && is_important_name(x))
				pDC->InvertRect(&re);
		}

		if (gc.structured_display && *ts!='#')
		{
			cp.CreatePen(PS_SOLID,2,gColors[NEST_COLOR]);
			oldpen=pDC->SelectObject(&cp);
			pDC->MoveTo(lpDIS->rcItem.left+3,lpDIS->rcItem.top+ih);
			pDC->LineTo(lpDIS->rcItem.left+4,lpDIS->rcItem.top+ih);
			pDC->SelectObject(oldpen);
			cp.DeleteObject();
		}
	}
}

// =======================================================================================
	void CColorListBoxSubject::MeasureItem(LPMEASUREITEMSTRUCT lpMIS)
// =======================================================================================
// all items use LBS_OWNERDRAWVARIABLE with same itemhigh
{
	lpMIS->itemHeight = itemhigh;
}

// =======================================================================================
	int CColorListBoxSubject::CompareItem(LPCOMPAREITEMSTRUCT lpCIS)
// =======================================================================================
{
   return 1;  // we do not need sorting
}

// =======================================================================================
// returns nearest ceiling in SubjPos items
	int get_next_nearest_value(int val)
// =======================================================================================
{
int ret=9999;

	if (SubjPos[0]>val && SubjPos[0]<ret)
		ret=SubjPos[0];

	if (SubjPos[1]>val && SubjPos[1]<ret)
		ret=SubjPos[1];

	if (SubjPos[2]>val && SubjPos[2]<ret)
		ret=SubjPos[2];

	return ret;
}

// =======================================================================================
// check argument for important names
	BOOL is_important_name(LPCSTR name)
// =======================================================================================
{
	for (int i=0;i<VIPNamesCount;i++)
		if (!strncmp(VIPNames[i],name,strlen(VIPNames[i])))
			return 1;

	return 0;
}
