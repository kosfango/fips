// sel_freq.cpp : implementation file
// IDD_FILELIST_REQUEST

#include "stdafx.h"
#include "sel_freq.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

sel_freq *sel_reg;
//extern CFont   font_filelists;
extern CFont   font_ANSI;
extern CFont   font_OEM;
extern _gconfig gc;

UINT lastdisplayed=0;

UINT	linecount=0;	  // count of lines in file
static UINT gLastDisplayedLine=0;
										  
#define MAX_LINE_COUNT 500000	// CScrollbar

static _DlgItemsSize DlgItemsSize [] =
{
	0,			    {0,0,0,0},{0,0,0,0},0,0,0,0,0,
	IDC_SCROLLBAR1	,{0,0,0,0},{0,0,0,0},HO,HO,HO|VE,HO|VE,0,
	IDC_EDIT1		,{0,0,0,0},{0,0,0,0},0,0,HO|VE,HO|VE,0,
	IDC_ADD			,{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDHELP			,{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_EDIT2		,{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_TOP			,{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_BOTTOM		,{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_PGUP		,{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_PGDOWN		,{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDCANCEL		,{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDOK			,{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_ANSI		,{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_OEM			,{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
};
static char DlgName[]="IDD_FILELIST_REQUEST";

// =======================================================================
	sel_freq::sel_freq(CWnd* pParent ) : CDialog(sel_freq::IDD, pParent)
// =======================================================================
{
	//{{AFX_DATA_INIT(sel_freq)
	*freq=0;
	lineoffs=0;
	//}}AFX_DATA_INIT
}

sel_freq::~sel_freq(){
	if (lineoffs)	free(lineoffs);
	if (fpseek)		fclose(fpseek);
}

// =======================================================================
	void sel_freq::DoDataExchange(CDataExchange* pDX)
// =======================================================================
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(sel_freq)
	DDX_Control(pDX, IDC_OEM, m_oem);
	DDX_Control(pDX, IDC_GOTO, m_gotoedit);
	DDX_Control(pDX, IDC_EDIT3, m_search);
	DDX_Control(pDX, IDC_EDIT2, m_list);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(sel_freq, CDialog)
	//{{AFX_MSG_MAP(sel_freq)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_CASEINSENS, OnCaseinsens)
	ON_BN_CLICKED(IDC_CASESENS, OnCasesens)
	ON_BN_CLICKED(IDC_BUTTON1, OnGoto)
	ON_BN_CLICKED(IDC_TOP, OnTop)
	ON_BN_CLICKED(IDC_PGDOWN, OnPgdown)
	ON_BN_CLICKED(IDC_PGUP, OnPgup)
	ON_BN_CLICKED(IDC_BOTTOM, OnBottom)
	ON_BN_CLICKED(IDC_ANSI, OnAnsi)
	ON_BN_CLICKED(IDC_OEM, OnOem)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_WM_VSCROLL()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// =======================================================================
	BOOL sel_freq::OnInitDialog()
// =======================================================================
{
char	fname[MAX_PATH];
UCHAR	c=0;
RECT	re;
CSize	chs;
int		lng[]={
				IDC_CASEINSENS,
				IDC_CASESENS,
				IDC_ADD,
				IDC_STATIC1,
				IDC_BUTTON1,
				IDC_TOP,
				IDC_PGUP,
				IDC_PGDOWN,
				IDC_BOTTOM,
				IDHELP,
				IDCANCEL
				};

	CDialog::OnInitDialog();
	BeginWaitCursor();
	set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));
	StoreOrgItemSizes(DlgName,this,DlgItemsSize,sizeof(DlgItemsSize)/sizeof(_DlgItemsSize));

	sel_reg=this;
	m_scroll.SubclassDlgItem(IDC_SCROLLBAR1,this);
	m_edit.SubclassDlgItem(IDC_EDIT1,this);
	lineoffs=(UINT*)malloc(MAX_LINE_COUNT*sizeof(int));
	if (!lineoffs)
	{
		EndWaitCursor();
		ERR_MSG_RET1("E_MEM_OUT");
	}

	*freq=0;
	m_edit.SetFont(&font_OEM);
	m_oem.SetCheck(TRUE);
// fill array of line offsets in current filelist
	make_path(fname,gc.UtilPath,gc.filelist);
	fpseek=fopen(fname,"rb");
	if (!fpseek)
	{
		EndWaitCursor();
		ERR_MSG2_RET1("E_COFLFTZ",fname);
	}
	lineoffs[0]=0;
	linecount=1;
	c=fgetc(fpseek);
	while (!feof(fpseek))
	{
		if (c=='\n')
		{
			if (linecount > MAX_LINE_COUNT-5)
			{
				fclose(fpseek);
				EndWaitCursor();
				ERR_MSG_RET1("E_FILETOOBIG");
			}
			lineoffs[linecount++]=ftell(fpseek);
		}
		c=fgetc(fpseek);
	}
	rewind(fpseek);
	m_edit.FmtLines(1);
	m_edit.GetClientRect(&re);
	get_fontsize(&font_OEM,&m_edit,chs);
	displines=(re.bottom-re.top)/chs.cy;
	if (linecount<=displines)
		norm=1;
	else
		norm=(linecount-displines)/32768.;
	m_scroll.SetScrollRange(0,32767,0);
	DisplayFromLine(0);
	EndWaitCursor();
	return TRUE;
}

// =======================================================================
	void sel_freq::OnOK()
// =======================================================================
{
	m_list.GetWindowText(freq,999);
	freq[999]=0;
	trim_all(freq);
	StoreInitDialogSize(DlgName,this);
	CDialog::OnOK();
}

// =======================================================================
	void sel_freq::OnCancel()
// =======================================================================
{
	StoreInitDialogSize(DlgName,this);
	CDialog::OnCancel();
}

// ============================================
	void sel_freq::OnSize(UINT nType, int cx, int cy)
// ============================================
{
RECT re;
CSize chs;

	CDialog::OnSize(nType, cx, cy);
	resize_wnd(this,DlgItemsSize,sizeof(DlgItemsSize));
	if (m_edit)
	{
		m_edit.GetClientRect(&re);
		get_fontsize(&font_OEM,&m_edit,chs);
		displines=(re.bottom-re.top)/chs.cy;
		if (gLastDisplayedLine)	DisplayFromLine(gLastDisplayedLine);
	}
}

// ============================================
	void sel_freq::OnPaint()
// ============================================
{
	CPaintDC dc(this);
	DrawRightBottomKnubble(this,dc);
}

// =======================================================================
	void sel_freq::OnAdd()
// =======================================================================
{
CString str,tmp;
int		ns,ne;

	m_edit.GetSel(ns,ne);
	if (ns==ne)	return;
	m_edit.GetWindowText(tmp);
	str=tmp.Mid(ns,ne-ns);
	trim_all(str);
	m_list.GetWindowText(tmp);
	trim_all(tmp);
	if (tmp.GetLength())	tmp+=' ';
	str.OemToAnsi();
	tmp+=str;
	m_list.SetWindowText(tmp);
}

// =======================================================================
	void sel_freq::OnCaseinsens()
// =======================================================================
{
	find_in_flist(0);
}

// =======================================================================
	void sel_freq::OnCasesens()
// =======================================================================
{
	find_in_flist(1);
}

// =======================================================================
	void  sel_freq::find_in_flist(int caseses)
// =======================================================================
{
CString pattern;
char	buf[400];
int		offsets[100],start,end,b1,b2,d,plen,cnt=0,found=0;

	m_search.GetWindowText(pattern);
	if (pattern.GetLength()==0)	return;
	m_edit.GetSel(start,end);
	if (end<0)	end=0;

	int LineInEdit=m_edit.LineFromChar(end);	// end of previous match
	int shouldbegin=LineInEdit+(start==end ? 0 : 1);	// start search from next line
	int beginsearch=lineoffs[lastdisplayed+shouldbegin];	// offset to next line

	BeginWaitCursor();
	fseek(fpseek,beginsearch,SEEK_SET);

	plen=pattern.GetLength();
	while (fgets(buf,199,fpseek))
	{
		found=find_next_entry(buf,caseses,pattern,offsets,1,0,!m_oem.GetCheck());
		if (found)	break;
		cnt++;
	}
	EndWaitCursor();

	if (found > 0)
	{
		d=lastdisplayed+LineInEdit+cnt+(start==end ? 0 : 1);
		b1=lineoffs[d];
		if (d >= 3)	d-=3;	// display from 3 lines above match
		DisplayFromLine(d);
		b2=	lineoffs[d];
		d=offsets[0]+b1-b2+3;
		m_edit.SetSel(d,d+plen,0);
	}
	else
		show_msg(L("S_347"));	// not found
}

// =======================================================================
// fills Edit field from fileoffset
	void sel_freq::DisplayFromLine(UINT startline)
// =======================================================================
{
UCHAR c;
char  buf[20000],*p;
UINT  offset,start,cnt=0;

	if (startline < 0)
		start=0;
	else if (startline > linecount-displines)
		start=linecount-displines;
	else
		start=startline;
	gLastDisplayedLine=lastdisplayed=start;
	sprintf(buf,"%d",start+1);
	m_gotoedit.SetWindowText(buf);
	*buf=0;
	offset=lineoffs[start];
	fseek(fpseek,offset,SEEK_SET);
	p=buf;
	c=fgetc(fpseek);
	while (!feof(fpseek))
	{
		if (c=='\n')
		{
			cnt++;
			if (cnt > displines)	break;
			*p++='\r';
			*p++='\n';
		}
		else
			*p++=c;
		c=fgetc(fpseek);
	}

	*p=0;
	if (!m_oem.GetCheck())	CharToOem(buf,buf);
	m_edit.SetWindowText(buf);
	m_scroll.SetScrollPos(int(start/norm));
}

// =======================================================================
	void sel_freq::OnGoto()
// =======================================================================
{
CString str;
UINT    n;

	m_gotoedit.GetWindowText(str);
	sscanf(str,"%d",&n);
	DisplayFromLine(n);
}

// =======================================================================
	void sel_freq::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
// =======================================================================
{
	switch (nSBCode)
	{
		case SB_THUMBPOSITION:
		case SB_THUMBTRACK:
//			if (lastdisplayed==int(nPos*norm))	return;
			DisplayFromLine(int(nPos*norm));
			break;
		case SB_LINEUP:
			key_up();
			break;
		case SB_LINEDOWN:
			key_down();
			break;
		case SB_PAGEUP:
			key_pgup();
			break;
		case SB_PAGEDOWN:
			key_pgdown();
	}
	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

void sel_freq::OnTop()		{	key_top();	 	}
void sel_freq::OnPgdown() { key_pgdown(); }
void sel_freq::OnPgup()		{ key_pgup();		}
void sel_freq::OnBottom()	{ key_bottom();	}

// =======================================================================
	void sel_freq::key_up()
// =======================================================================
{
	lastdisplayed--;
	if ((int)lastdisplayed < 0)	lastdisplayed=0;
	DisplayFromLine(lastdisplayed);
}

// =======================================================================
	void sel_freq::key_down()
// =======================================================================
{
	lastdisplayed++;
	if (lastdisplayed >= linecount-displines)	lastdisplayed =linecount-displines;
	DisplayFromLine(lastdisplayed);
}

// =======================================================================
	void sel_freq::key_pgdown()
// =======================================================================
{
	lastdisplayed+=displines;
	if (lastdisplayed >= linecount-displines)	lastdisplayed=linecount-displines;
	DisplayFromLine(lastdisplayed);
}
// =======================================================================
	void sel_freq::key_pgup()
// =======================================================================
{
	lastdisplayed-=displines;
	if ((int)lastdisplayed < 0)	lastdisplayed=0;
	DisplayFromLine(lastdisplayed);
}
// =======================================================================
	void sel_freq::key_bottom()
// =======================================================================
{
	lastdisplayed=linecount-displines;
	if ((int)lastdisplayed < 0)	lastdisplayed=0;
	DisplayFromLine(lastdisplayed);
}

// =======================================================================
	void sel_freq::key_top()
// =======================================================================
{
	lastdisplayed=0;
	DisplayFromLine(lastdisplayed);
}

// =======================================================================
	void sel_freq::OnHelp()
// =======================================================================
{
	WinHelp(VHELP_FREQ);
}

BOOL sel_freq::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return TRUE;
}

void sel_freq::OnAnsi() 
{
	DisplayFromLine(lastdisplayed);
}

void sel_freq::OnOem() 
{
	DisplayFromLine(lastdisplayed);
}
