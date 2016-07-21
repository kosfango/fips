// costanal.cpp : implementation file
// IDD_COSTANAL

#include "stdafx.h"
#include "time.h"
#include "costanal.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern _gconfig gc;
static CStrList clist;
static CStrList loctmp;

static _DlgItemsSize DlgItemsSize [] =
{
	0,			    {0,0,0,0},{0,0,0,0},0,0,0,0,0,
	IDC_LIST1,		{0,0,0,0},{0,0,0,0},0,0,VE|HO,VE|HO,0,
	IDHELP,			{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_CLEAR_INFO,	{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_WRITE_FILE,	{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDCANCEL,		{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
};
static char DlgName[]="IDD_COSTANAL";

// ====================================================================
	costanal::costanal(CWnd* pParent ) : CDialog(costanal::IDD, pParent)
// ====================================================================
{
	//{{AFX_DATA_INIT(costanal)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

// ====================================================================
	void costanal::DoDataExchange(CDataExchange* pDX)
// ====================================================================
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(costanal)
	DDX_Control(pDX, IDC_LIST1, m_list);
	DDX_Control(pDX, IDC_EDIT_STARTDATE, m_edit_startdate);
	DDX_Control(pDX, IDC_EDIT_ENDDATE, m_edit_enddate);
	DDX_Control(pDX, IDC_SUPRESS_EMPTY, m_check_supress);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(costanal, CDialog)
	//{{AFX_MSG_MAP(costanal)
	ON_BN_CLICKED(IDC_DEFAULT, OnDefault)
	ON_BN_CLICKED(IDC_DISPLAY, OnDisplay)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_BN_CLICKED(IDC_DISPLAY_ALL, OnDisplayAll)
	ON_BN_CLICKED(IDC_CLEAR_INFO, OnClearInfo)
	ON_BN_CLICKED(IDC_DISPLAY_TODAY, OnDisplayToday)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_WRITE_FILE, OnWriteFile)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// ====================================================================
	BOOL costanal::OnInitDialog()
// ====================================================================
{
CStrList lst;

int  lng[]={
			IDCANCEL,
			IDHELP,
			IDC_DISPLAY,
			IDC_DEFAULT,
			IDC_STATIC1,
			IDC_STATIC2,
			IDC_STATIC573,
			IDC_STATIC88,
			IDC_DISPLAY_ALL,
			IDC_CLEAR_INFO,
			IDC_SUPRESS_EMPTY,
			IDC_DISPLAY_TODAY,
			IDC_WRITE_FILE
			};
    CDialog::OnInitDialog();
	loctmp.RemoveAll();
    set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));
    StoreOrgItemSizes(DlgName,this,DlgItemsSize,sizeof(DlgItemsSize)/sizeof(_DlgItemsSize));

	lst.LoadFromFile("datedef.cfg");
	if (lst.GetCount()>0)
	{
		m_edit_startdate.SetWindowText(lst.GetString(0));
		m_edit_enddate.SetWindowText(lst.GetString(1));
	}
	return TRUE;
}

// ====================================================================
	void costanal::OnDisplay()
// ====================================================================
{
#define MAX_FIDO_SYSTEMS 1000

FILE	*fp;
CString basedir;
CString start;
CString ende;
CString help;
CString help1;
struct  _costinfo cost;
struct	tm *tt;
int		last_day	=-1;
int		last_mon	=-1;
int		last_year	=-1;
int		last_day_mode=0;
int		i,x;
int		count;
int		foundi;
int		itemcount=0;
unsigned long start_unix	=0;
unsigned long end_unix	=0;
LV_ITEM item;				// CListCtrl
LV_COLUMN col;				// CListCtrl
float	array_in[MAX_FIDO_SYSTEMS];
float	array_sum[MAX_FIDO_SYSTEMS];
float	sum_all=0.0f;

	BeginWaitCursor();
	loctmp.RemoveAll();
	// Wir loeschen zur Sicherheit die alten Felder ...
	memset(array_in,0,sizeof(array_in));
	memset(array_sum,0,sizeof(array_sum));

	// Als allererstes holen wir uns die Datumsangaben ...
	m_edit_startdate.GetWindowText(start);
	m_edit_enddate.GetWindowText(ende);

    trim_all(start);
    trim_all(ende);

	if (start.GetLength()>0  &&  ende.GetLength()<2)
	{
		m_edit_enddate.SetWindowText(start);
		ende=start;
	}

	if (start.GetLength()>0)
		build_unix_date(PS start,&start_unix);
	else
		start_unix=1;

	if (start.GetLength()>0)
		build_unix_date(PS ende,&end_unix);
	else
		end_unix=0xFFFFFFFF;

	if (!start_unix || !end_unix || (start_unix>end_unix))
		ERR_MSG_RET("E_WRDAAGBE");
	// Falls der Starttag == dem Endtag ist addieren wir einen Tag
	if (start_unix==end_unix)
		end_unix+=(24*3600);

	// So, hier haben wir das Start und das EndeDatum ...
	// Jetzt oeffnen wir die LOGDATEI und lesen den ganzen Quak ein ...

	basedir.Format("%s\\%s",gc.BasePath,COSTFILENAME);

	// Wir muessen eine eventuell vorhandene alte Loeschen ...
	m_list.DeleteAllItems();
	for (int n=clist.GetCount();n>=0;n--)		// CListCtrl
		m_list.DeleteColumn(n);
	clist.RemoveAll();

	// Das 'EckFeld'
	memset(&col,0,sizeof(col));
	col.mask=LVCF_TEXT | LVCF_WIDTH ;
	col.cx  =60;
	loctmp.AppendToString(0,L("S_514"),"\t");
	col.pszText=L("S_514");
	m_list.InsertColumn(0,&col);

	fp=fopen(basedir,"rb");
	if (fp)
	{
		// Hier lesen wir die ganze Datei rein ...
		while (fread((void *)&cost,sizeof (struct _costinfo),1,fp))
		{
			if ((cost.time >= (long)start_unix) && (cost.time <= (long)end_unix))
				test_and_append(&cost);
		}
		// Jetzt haben wir eine Liste aller vorhandenen FIDO Systeme ...
		// in clist  ...
		// Deshalb koennen wir jetzt die Ueberschriftenzeile reinhaengen ...
		for (i=0;i<clist.GetCount();i++)
		{
			// Zuerst wird das ITEM an sich reingehaengt ...
			memset(&col,0,sizeof(col));
			col.mask=LVCF_TEXT | LVCF_WIDTH ;       // see below
			col.cx  =80;
			help1.Format("%s",PS clist.GetString(i));
			col.pszText=PS help1;
			loctmp.AppendToString(0,PS help1,"\t");
			m_list.InsertColumn(i+1,&col);
		}

		// Jetzt lesen wir das File nochmal durch und holen die Gebuehren Info raus ..
		rewind(fp);			// wir gehen wieder an den Anfang ...
		while (fread((void *)&cost,sizeof (struct _costinfo),1,fp))
		{
			if (!((cost.time >= (long)start_unix) && (cost.time <= (long)end_unix)))
				continue;

			// Passst die ZeitInfo
			tt=localtime((time_t *)&cost.time);
			if (!tt)
			   continue;

			if (tt->tm_year > 99)
				tt->tm_year -= 100;

			// Fuer die erste Zeile muessen wir die default's so setzten dass
			// es immer paast
			if (last_day==-1)
			{
				last_day	= tt->tm_mday;
				last_mon	= tt->tm_mon;
				last_year	= tt->tm_year;
			}

			// Ist es derselbe Tag wie der zuletzt gelesene	?
			if ((last_year==tt->tm_year) && (last_mon ==tt->tm_mon ) && (last_day ==tt->tm_mday))
			{
				   foundi=find_this_fido_system(cost.fido);
				   array_in[foundi]+=cost.cost;	  array_sum[foundi]+=cost.cost;  sum_all+=cost.cost;
			}
			else  // Ein anderer Tag als der zuletzt gelesene, daher haengen wir die bisherige Statistik an ...
			{
write_one_st_line:
				count=clist.GetCount();
				ASSERT(count <MAX_FIDO_SYSTEMS);

				// Zuerst wird das ITEM an sich reingehaengt ...
				memset(&item,0,sizeof(item));
				item.mask=LVCF_TEXT ;			// see below
				item.iItem=itemcount;
				m_list.InsertItem(&item);

				memset(&item,0,sizeof(item));
				item.mask=LVIF_TEXT ;			// see below
				help.Format("%02d/%02d/%02d",last_day,last_mon+1,last_year);
				item.pszText=PS help;
				item.iItem=itemcount;
				item.iSubItem=0;
				m_list.SetItem(&item);
				loctmp.AppendToString(itemcount+1,PS help,"\t");

				for (i=0;i<count;i++)
				{
					memset(&item,0,sizeof(item));
					item.mask=LVIF_TEXT;
					help1.Format("%5.2f",array_in[i]);
					if (array_in[i]==0.0 && m_check_supress.GetCheck())
						help1.Empty();

					item.pszText=PS help1;
					item.iItem=itemcount;
					item.iSubItem=i+1;
					m_list.SetItem(&item);
					loctmp.AppendToString(itemcount+1,PS help1,"\t");
				}
				itemcount++;

				if (last_day_mode)
					goto alles_end;

				memset(&array_in,0,sizeof(array_in));
				foundi=find_this_fido_system(cost.fido);
				array_in[foundi]+=cost.cost; 
				array_sum[foundi]+=cost.cost;	
				sum_all+=cost.cost;

			}
			last_day= tt->tm_mday;
			last_mon= tt->tm_mon;
			last_year= tt->tm_year;
		}
		fclose(fp);
		last_day_mode=1;
  		goto write_one_st_line;
	}
	else
		ERR_MSG_RET("W_NOCLAV");

alles_end:

	for (x=0;x<2;x++)
	{
		memset(&item,0,sizeof(item));
		item.mask=LVCF_TEXT ;
		item.iItem=itemcount;
		m_list.InsertItem(&item);

		memset(&item,0,sizeof(item));
		item.mask=LVIF_TEXT ;
		if (x==0)
			item.pszText="  ";
		else
			item.pszText=L("S_418");	// sum:
		item.iItem=itemcount;
		item.iSubItem=0;
		m_list.SetItem(&item);
		loctmp.AppendToString(itemcount+2,item.pszText,"\t");

		for (i=0;i<count;i++)
		{
			memset(&item,0,sizeof(item));
			item.mask=LVIF_TEXT;
			if (x==0)
				help1="=====";
			else
				help1.Format("%5.2f",array_sum[i]);
			item.pszText=PS help1;
			item.iItem=itemcount;
			item.iSubItem=i+1;
			m_list.SetItem(&item);
			loctmp.AppendToString(itemcount+2,item.pszText,"\t");
		}
		itemcount++;
	}

	for (x=0;x<2;x++)
	{
		memset(&item,0,sizeof(item));
		item.mask=LVCF_TEXT ;
		item.iItem=itemcount;
		m_list.InsertItem(&item);
		memset(&item,0,sizeof(item));
		item.mask=LVIF_TEXT ;
		item.pszText=L("S_619");	// Total:
		if (x==0)
			item.pszText="  ";
		item.iItem=itemcount;
		item.iSubItem=0;
		m_list.SetItem(&item);
		loctmp.AppendToString(itemcount+3,item.pszText,"\t");

		memset(&item,0,sizeof(item));
		item.mask=LVIF_TEXT;
		help1.Format("%5.2f",sum_all);
		if (x==0)
			help1="   ";
		item.pszText=PS help1;
		item.iItem=itemcount;
		item.iSubItem=1;
		m_list.SetItem(&item);
		loctmp.AppendToString(itemcount+3,item.pszText,"\t");
		itemcount++;
	}

	EndWaitCursor();
}

// ====================================================================
	void costanal::OnDefault()
// ====================================================================
{
CString start;
CString end;
CStrList lst;

	m_edit_startdate.GetWindowText(start);
	m_edit_enddate.GetWindowText(end);

	lst.RemoveAll();
	lst.AddTail(start);
	lst.AddTail(end);
	lst.SaveToFile("datedef.cfg");

	show_msg("Saved","Saved","Записано");
}


// ====================================================================
	void costanal::OnCancel()
// ====================================================================
{
	loctmp.RemoveAll();
	clist.RemoveAll();
    StoreInitDialogSize (DlgName,this);
	CDialog::OnCancel();
}

// ====================================================================
	int build_unix_date(char *datedef,unsigned long *unixt)
// ====================================================================
// Diese Funktion generiert aus einem String mit dem Fromat TT.MM.JJJJ
// eine UnixTime und gibt den in *unixt zureuck ...
{
int	d=0;
int	m=0;
int	y=0;
int ret;
struct tm stime;

	ret=sscanf(datedef,"%d.%d.%d",&d,&m,&y);
	if (ret!=3 || d<0 || d>31 || m<0 || m>12 || y<1800 || y>2030)
		return 0;

	memset(&stime,0,sizeof(stime));
	stime.tm_mday=d;
	stime.tm_mon=m-1;
	stime.tm_year=y-1900;
	*unixt=mktime(&stime);
	return 1;
}


//	==============================================
	void costanal::test_and_append(struct _costinfo* ci)
//	==============================================
// 	Diese Funktion traegt eine enthaltene FidoAdresse ein falls sie noch nicht in der
//  Liste existiert ...
{
CString str;

	for (int i=0;i<clist.GetCount();i++)
	{
		str=clist.GetString(i);
		if (!strcmp(str,ci->fido))
			return;
	}
	clist.AddTail(ci->fido);
}

//	==============================================
	int	costanal::find_this_fido_system(char *tofind)
//	==============================================
{
CString str;

	for (int i=0;i<clist.GetCount();i++)
	{
		str=clist.GetString(i);
		if (!strcmp(str,tofind))
			return i;
	}
	return 0;
}


//	==============================================
	void costanal::OnDisplayAll()
//	==============================================
{
	m_edit_startdate.SetWindowText("01.01.1980");
	m_edit_enddate.SetWindowText("30.12.2020");
	OnDisplay();
}

//	==============================================
	void costanal::OnClearInfo()
//	==============================================
{
char path[MAX_PATH];

	if (err_out("DN_RCCIFO")!=IDYES)
		return;

	make_path(path,gc.BasePath,COSTFILENAME);
	unlink(path);
	show_msg("Cleared","Очищено");
}

//	==============================================
	void costanal::OnDisplayToday()
//	==============================================
{
char buf[100];
int  m,d,y;

 	_strdate(buf);
 	sscanf(buf,"%d/%d/%d",&m,&d,&y);
 	sprintf(buf,"%02d.%02d.%d",d,m,y+2000);

	m_edit_startdate.SetWindowText(buf);
	m_edit_enddate.SetWindowText(buf);
	OnDisplay();
}

// ============================================
	void costanal::OnSize(UINT nType, int cx, int cy)
// ============================================
{
	CDialog::OnSize(nType, cx, cy);
	resize_wnd(this,DlgItemsSize,sizeof(DlgItemsSize));
}

// ===============================================
	void costanal::OnPaint()
// ===============================================
{
	CPaintDC dc(this);
	DrawRightBottomKnubble(this,dc);
}

// ===============================================
	void costanal::OnWriteFile()
// ===============================================
{
CStrList	lst;
CString		line;
CString		str;
CString		path;
int			i,n,ret;
FILE		*fp;

	str.LoadString(IDS_TEXTFILT);
	CFileDialog dlg(FALSE,"txt",NULL,OFN_HIDEREADONLY,str);
	ret=dlg.DoModal();
	restore_base_path();
	if (ret!=IDOK)	return;
	
	fp=fopen(dlg.GetPathName(),"wt");
	if (!fp)
		return;

	line=loctmp.GetString(0);
	lst.SplitTabs(PS line);

	for (i=0;i<lst.GetCount();i++)
	{
		str=lst.GetString(i);
        trim_all(str);
		str.AnsiToOem();
		fprintf(fp,"%-15s",str);
	}
	fprintf(fp,"\n");

	for (n=1;n<loctmp.GetCount();n++)
	{
		line=loctmp.GetString(n);
		lst.SplitTabs(PS line);
		for (i=0;i<lst.GetCount();i++)
		{
			str=lst.GetString(i);
            trim_all(str);
			str.AnsiToOem();
			fprintf(fp,"%-15s",str);
		}
		fprintf(fp,"\n");
	}
	fclose(fp);
}

// ====================================================================
	void costanal::OnHelp()
// ====================================================================
{
	WinHelp(VHELP_COSTANAL);
}

BOOL costanal::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return TRUE;
}
