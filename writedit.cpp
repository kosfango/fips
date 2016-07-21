// writedit.cpp : implementation file
// handler for mailtext box in editor dialog

#include "stdafx.h"
#include "light.h"
#include "structs.h"
#include "writedit.h"
#include "lightdlg.h"
#include "mystrlst.h"
#include "writmail.h"
#include "globals.h"
#include "cfg_col.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern _gconfig gc;
extern CLightDlg	*gdlg;
extern CStrList trepls;
extern CStrList jokes;
extern writmail	*gEditor;
//my experiments
//extern CRichEditCtrl *editptr;
CRichEditCtrl *gEdit;
//writedit	*gEdit;

CString wphrase;
CString wrepl;
CString OriginalTitle;

void CtrlY_handler(void);
void Enter_handler(void);
void append_to_buffer(char key);
int	 get_phrase_index(void);
int	 get_joke_index(void);

/////////////////////////////////////////////////////////////////////////////
// writedit

writedit::writedit(){}

writedit::~writedit(){}

BEGIN_MESSAGE_MAP(writedit, CRichEditCtrl)
	//{{AFX_MSG_MAP(writedit)
	ON_WM_CHAR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// writedit message handlers

	void writedit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
CString str,joke;
char	repp[10000];
long	start,end,ret;
static	int easteregg=0,TitleWasChanged	=0;

	gc.askforreallycancel=1;
	if (nChar==25 && (nFlags & 0xFF)==21)
	{
	   CtrlY_handler();
	   return;
	}
	if (nChar==13)
	   Enter_handler();

	append_to_buffer((char)nChar);

// handle phrases that should be replaced
	ret=get_phrase_index();
	if (ret!=-1)
	{
  		str=trepls.GetString(ret);
		get_token(str,0,wphrase);
		get_token(str,1,wrepl);
		strcpy(repp,wrepl);
		insert_LFCR(repp);
		GetSel(start,end);
		SetSel(start-wphrase.GetLength()+1,end);
		ReplaceSel(repp);
		return;
 	}

// handle joke phrases
	ret=get_joke_index();
	if (ret!=-1)
	{
  		if (OriginalTitle.GetLength()==0)
			gEditor->GetWindowText(OriginalTitle);

  		str=jokes.GetString(ret);
		get_token(str,1,joke);
		gEditor->SetWindowText(joke);
		TitleWasChanged=1;
		easteregg=1;
		goto proc_char;
 	}
	else
		easteregg=0;

	// was a joke?
	if (!easteregg && TitleWasChanged)
	{
		// restore Orginal Title
		gEditor->SetWindowText(OriginalTitle);
		TitleWasChanged=0;
	}

proc_char:
	CRichEditCtrl::OnChar(nChar, nRepCnt, nFlags);
}

// =======================================================
	void  CtrlY_handler(void)
// =======================================================
// handler for CTRL-Y - delete whole line(s)
{
long sel,sele,x1,x2;
//int sel, sele, x1, x2;
int line;

	gEdit->GetSel(sel,sele);
	line=gEdit->LineFromChar(sel);

	x1=gEdit->LineIndex(line);
	x2=gEdit->LineIndex(line+1);

	gEdit->SetSel(x1,x2);
	gEdit->Clear();
}

// =======================================================
// handler for Enter - insert quote on new line
	void Enter_handler(void)
// =======================================================
{
char buf[100],quote[20],*p;
long s,t,k,m;
//int s, t, k, m;
BOOL b;

	if (!gc.RepeatQuote)	return;
	m=gEdit->GetLine(k=gEdit->LineFromChar(-1),buf,99);
	if (!m)	return;
	k=gEdit->LineIndex(k);
	gEdit->GetSel(s,t);
	p=&buf[t-k];
	b=TRUE;
	while (p<buf+m)
	{
		if (*p++!=' ')
		{
			b=FALSE;
			break;
		}
	}

	if (b)	return;
	p=strchr(buf,'>');
	if (p && s-k>p-buf)
	{
		while (*p && *p=='>')	p++;
		k=p-buf;
		if (*p && ISSPACE(*p) && k<10)
		{
			strncpy(quote,buf,k);
			quote[k]=' ';
			quote[k+1]=0;
			gEdit->ReplaceSel(quote,TRUE);
			gEdit->SetSel(s,s);
			for (s=0;s<=k;s++)
			{
				gEdit->PostMessage(WM_KEYDOWN,VK_RIGHT,1);
				gEdit->PostMessage(WM_KEYUP,VK_RIGHT,1);
			}
		}
	}
}

// ===================================================================
	void append_to_buffer(char key)
// ===================================================================
{
	for (int i=1;i<=19;i++)
	   typebuffer[i-1]=typebuffer[i];

	typebuffer[19]=key;
	typebuffer[20]=0;
}

// ==================================================================
	int get_phrase_index(void)
// ==================================================================
{
CString phrase;
int    	i,n;

	for (i=0;i<trepls.GetCount();i++)
	{
		get_token(trepls.GetString(i),0,phrase);
		n=phrase.GetLength();
		if (!strcmp(phrase,&typebuffer[20-n]))
		{
			memset(&typebuffer[20-n],0,n); // block multichar replacement
			return i;
		}
	}
	return -1;
}

// ==================================================================
	int get_joke_index(void)
// ==================================================================
{
CString line;
CString joke;
int    	i;

	for (i=0;i<jokes.GetCount();i++)
	{
		line=jokes.GetString(i);
		get_token(line,0,joke);
		if (!strcmp(joke,&typebuffer[20-joke.GetLength()]))
			return i;
	}
	return -1;
}

