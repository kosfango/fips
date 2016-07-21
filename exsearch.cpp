// exsearch.cpp : implementation file
// IDD_EXT_SEARCH

#include "stdafx.h"
#include "lightdlg.h"
#include "csobj.h"
#include "resultfm.h"
#include "sedesta.h"
#include "exsearch.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
static char		DlgName[]="IDD_EXT_SEARCH";

extern CString gSearchDestArea;
extern _gconfig gc;

DWORD		xxdummy=0;
CWinThread	*search_thread;

extern CLightDlg *gpMain;
extern exsearch	 gSearch;
extern CStrList	 resultdisp;
static CString	 lochelp;
static CString	 locall;
static CString	 locall2;
CStrList	AreasForQuery;
CSobj		csarray[100];
int			cscount=0;
int			stop_search_thread=0;

static _DlgItemsSize DlgItemsSize [] =
{
	0,						{0,0,0,0},{0,0,0,0},0,0,0,0,0,
	IDC_STOP_SEARCH		    ,{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_CLEAR_SEARCH		,{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_DISPLAY				,{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_LOAD_RESULTS		,{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_SAVE_RESULTS		,{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_STATIC45			,{0,0,0,0},{0,0,0,0},VE,VE,VE|HO,VE|HO,0,
	IDC_ORED				,{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_STATIC_123			,{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_EDIT1				,{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_MARKALLFOUND		,{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_PROCDELETED			,{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_MARKALLFOUNDASREAD	,{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_STATIC639			,{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_EDIT_COPYTOAREA		,{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_BROWSE_AREAS		,{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_QUERY				,{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_LIST				,{0,0,0,0},{0,0,0,0},VE,VE,VE|HO,VE|HO,0,
	IDC_DD_FIELD			,{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_STATIC352			,{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_CASE_SESITIVE		,{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_DD_CONTAINS			,{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_NOT					,{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_EDIT				,{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_CLEAR_QUERY_LIST	,{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_DELETE				,{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_ADD					,{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_CHANGE				,{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_LOAD_QUERY			,{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_SAVE_QUERY			,{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDOK					,{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_SEL_ALL				,{0,0,0,0},{0,0,0,0},VE|HO,VE|HO,VE|HO,VE|HO,0,
	IDC_CLEAR_ALL			,{0,0,0,0},{0,0,0,0},VE|HO,VE|HO,VE|HO,VE|HO,0,
	IDC_HIDE				,{0,0,0,0},{0,0,0,0},VE|HO,VE|HO,VE|HO,VE|HO,0,
	IDHELP					,{0,0,0,0},{0,0,0,0},VE|HO,VE|HO,VE|HO,VE|HO,0,
	IDC_LIST2				,{0,0,0,0},{0,0,0,0},0,0,VE|HO,VE|HO,0,
	IDC_STATIC876			,{0,0,0,0},{0,0,0,0},0,0,VE|HO,VE|HO,0,
	IDC_LIST1				,{0,0,0,0},{0,0,0,0},HO,HO,VE|HO,VE|HO,0,
};
static char *qhdrs[]={"Field","Поле","N","Н","Position","Положение","C","Р","Pattern","Образец"};
static char	*ahdrs[]={"Areas to search in","Области поиска","",""};
static char	*rhdrs[]={"Area","Область","From","От кого","Subject","Тема","","","",""};

// =====================================================================
	exsearch::exsearch(CWnd* pParent ) 	: CDialog(exsearch::IDD, pParent)
// =====================================================================
{
	//{{AFX_DATA_INIT(exsearch)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

// =====================================================================
	void exsearch::DoDataExchange(CDataExchange* pDX)
// =====================================================================
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(exsearch)
	DDX_Control(pDX, IDC_PROCDELETED, m_procdeleted);
	DDX_Control(pDX, IDC_LIST2, m_listres);
	DDX_Control(pDX, IDC_LIST1, m_listarea);
	DDX_Control(pDX, IDC_LIST, m_listqry);
	DDX_Control(pDX, IDC_MARKALLFOUNDASREAD, m_markasread);
	DDX_Control(pDX, IDC_MARKALLFOUND, m_markallfound);
	DDX_Control(pDX, IDC_STATUS, m_status);
	DDX_Control(pDX, IDC_ORED, m_ored);
	DDX_Control(pDX, IDC_CASE_SESITIVE, m_case_sensitive);
	DDX_Control(pDX, IDC_STATIC_NR_FOUND, m_nr_found);
	DDX_Control(pDX, IDC_EDIT1, m_edit_max);
	DDX_Control(pDX, IDC_PROGRESS1, m_progress);
	DDX_Control(pDX, IDC_NOT, m_not);
	DDX_Control(pDX, IDC_EDIT, m_edit);
	DDX_Control(pDX, IDC_DD_FIELD, m_dd_field);
	DDX_Control(pDX, IDC_DD_CONTAINS, m_dd_contains);
	DDX_Control(pDX, IDC_EDIT_COPYTOAREA, m_edit_copy_to_area);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(exsearch, CDialog)
	//{{AFX_MSG_MAP(exsearch)
	ON_BN_CLICKED(IDC_DISPLAY, OnDisplay)
	ON_BN_CLICKED(IDC_CHANGE, OnChange)
	ON_CBN_SELCHANGE(IDC_DD_FIELD, OnSelchangeDdField)
	ON_BN_CLICKED(IDC_BROWSE_AREAS, OnBrowseAreas)
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_MOVE()
	ON_NOTIFY(NM_CLICK, IDC_LIST2, OnClickList2)
	ON_WM_SHOWWINDOW()
	ON_WM_HELPINFO()
	ON_BN_CLICKED(IDC_CLEAR_QUERY_LIST, OnClearQueryList)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_LOAD_QUERY, OnLoadQuery)
	ON_BN_CLICKED(IDC_LOAD_RESULTS, OnLoadResults)
	ON_BN_CLICKED(IDC_SAVE_QUERY, OnSaveQuery)
	ON_BN_CLICKED(IDC_SAVE_RESULTS, OnSaveResults)
	ON_BN_CLICKED(IDOK, OnSearch)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_BN_CLICKED(IDC_STOP_SEARCH, OnStopSearch)
	ON_BN_CLICKED(IDC_HIDE, OnOk2)
	ON_BN_CLICKED(IDC_CLEAR_ALL, OnClearAll)
	ON_BN_CLICKED(IDC_SEL_ALL, OnSelAll)
	ON_BN_CLICKED(IDC_CLEAR_SEARCH, OnClearSearch)
	ON_BN_CLICKED(IDC_DISP_NEXT_Q_RESULT, OnDispNextQResult)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST, OnItemchangedList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// =====================================================================
	BOOL exsearch::OnInitDialog()
// =====================================================================
{
_listcol lc[]={100,"",25,"",160,"",25,"",210,""};
_listcol la[]={250,"",-1,""};
_listcol lr[]={130,"",140,"",240,"",-1,"",-1,""};

	CDialog::OnInitDialog();

	MakeColumns(m_listqry,lc,sizeof(lc)/sizeof(_listcol),DlgName,1);
	MakeColumns(m_listarea,la,sizeof(la)/sizeof(_listcol),DlgName,2);
	MakeColumns(m_listres,lr,sizeof(lr)/sizeof(_listcol),DlgName,3);

	OnClearSearch();
	FillComboboxes();
	FillAreasList();
	m_edit.SetFocus();
	return TRUE;
}

// =====================================================================
	void exsearch::OnAdd()
// =====================================================================
{
CString line;

	 if (!FormLine(line))
		ERR_MSG_RET("E_IINVA");
	 AddRow(m_listqry,line);
	 m_edit.SetWindowText(NULL);
}

// ===========================================
	void exsearch::OnChange()
// ===========================================
{
CString	line;
int		sel;
	
	if (!FormLine(line))	ERR_MSG_RET("E_IINVA");
	GET_SELECT(m_listqry);
	ReplaceRow(m_listqry,sel,line);
}

// =====================================================================
	void exsearch::OnDelete()
// =====================================================================
{
int sel;

	GET_SELECT(m_listqry);
	m_listqry.DeleteItem(sel);
}

// =====================================================================
	void exsearch::OnSearch()
// =====================================================================
{
areadef adef;
CString line,help;
int		i;

	if (gc.SearchThreadRunning)
		ERR_MSG_RET("E_SCRSWT");

	FILE *fpx=fopen(LastSucessfulCall,"rb");
	if (fpx)
	{
	  	fread(&last_modemsession_time,1,sizeof(int),fpx);
		fclose(fpx);
	}

	cscount=m_listqry.GetItemCount();
	if (cscount==0)
	{
		if (!FormLine(line))	ERR_MSG_RET("E_IINVA");
		AddRow(m_listqry,line);
		cscount=1;
	}

	if (cscount>99)	ERR_MSG_RET("E_TOMUSC");
	m_edit_max.GetWindowText(help);
	i=0;
	sscanf(help,"%d",&i);
	if (i<=0)	m_edit_max.SetWindowText("500");
	GetSelectedArea();
	if (AreasForQuery.GetCount()==0)	ERR_MSG_RET("E_ATLOAMBSFS");
	m_edit_copy_to_area.GetWindowText(help);
    trim_all(help);
	if (help.GetLength()>1)
	{
		int ret;
		long hand;
		ret=db_get_area_handle(help,&hand,0);
		if (ret==DB_OK)
		{
			db_get_area_by_index(hand,&adef);
			if (adef.localmail==0)	ERR_MSG_RET("E_SCTTAINA");
		}
	}

	for (i=0;i<cscount;i++)
	{
		GetRow(m_listqry,i,line);
		csarray[i].BuildSearchStuff(line);
	}
	m_edit_max.GetWindowText(help);
	set_cfg(CFG_ESEARCH,"MaxResults",atoi(help));
	set_cfg(CFG_ESEARCH,"AndFlag",m_ored.GetCheck());
	set_cfg(CFG_ESEARCH,"MarkFound",m_markallfound.GetCheck());
	set_cfg(CFG_ESEARCH,"MarkRead",m_markasread.GetCheck());
	set_cfg(CFG_ESEARCH,"ProcDeleted",m_procdeleted.GetCheck());
	
	search_thread=AfxBeginThread(SearchEngineWorkerThread,&xxdummy);
	if (!search_thread)	ERR_MSG_RET("E_COSEWTF");
}

// =====================================================================
	void exsearch::OnSaveQuery()
// =====================================================================
{
CString str,path;
char	buf[300],*p;
int		ret;

	str.LoadString(IDS_QUERYFILT);
	CFileDialog dlg(FALSE,"qry",NULL,OFN_HIDEREADONLY,str);
	ret=dlg.DoModal();
	restore_base_path();
	if (ret!=IDOK)	return;
	path=dlg.GetPathName();
	SaveToDelimText(m_listqry,path,0,'\t');

	strcpy(buf,path);
	p=strrchr(buf,'.');
	if (p)
	{
		strcpy(p,".sda");          // Search destination directory
		unlink(buf);
		m_edit_copy_to_area.GetWindowText(str);
        trim_all(str);
		if (str.GetLength()>1)
		{
            CStrList sdafile;
			sdafile.RemoveAll();
			sdafile.AddTail(str);
			sdafile.SaveToFile(buf);
		}
	}
}

// =====================================================================
	void exsearch::OnLoadQuery()
// =====================================================================
{
CString str,path;
int		ret;

	str.LoadString(IDS_QUERYFILT);
	CFileDialog dlg(TRUE,"qry",NULL,OFN_HIDEREADONLY,str);
	ret=dlg.DoModal();
	restore_base_path();
	if (ret!=IDOK)	return;

	path=dlg.GetPathName();
	OnClearSearch();
	LoadFromDelimText(m_listqry,path);
	handle_sda_file(path);
	OnItemchangedList(0,0);
}

// =====================================================================
	void exsearch::OnLoadQuery2(LPCSTR path)
// =====================================================================
{
	OnClearSearch();
	LoadFromDelimText(m_listqry,path);
	handle_sda_file(path);
	OnItemchangedList(0,0);
}

// =====================================================================
	void exsearch::handle_sda_file(LPCSTR path)
// =====================================================================
{
CString help;
char	buf[400],*p;

	strcpy(buf,path);
	p=strrchr(buf,'.');
	if (p)
	{
        CStrList sdafile;
		strcpy(p,".sda");          // Search destination directory
		sdafile.LoadFromFile(buf);
		if (sdafile.GetCount()>0)
		{
			help=sdafile.GetString(0);
			m_edit_copy_to_area.SetWindowText(help);
		}
	}
}

// =====================================================================
	void exsearch::OnStopSearch()
// =====================================================================
{
	stop_search_thread=1;
}

// =====================================================================
// Split list line
	void exsearch::ParseLine(CString &line)
// =====================================================================
{
CString help,ln;
int		i;

	get_token(line,0,help);
	for (i=0;i<m_dd_field.GetCount();i++)
	{
		m_dd_field.GetLBText(i,ln);
		if (ln==help)
		{
			m_dd_field.SetCurSel(i);
			break;
		}
	}
	get_token(line,1,help);
	m_not.SetCheck(help=="-");

	ln.Empty();
	get_token(line,2,help);
	for (i=0;i<m_dd_contains.GetCount();i++)
	{
		m_dd_contains.GetLBText(i,ln);
		if (ln==help)
		{
			m_dd_contains.SetCurSel(i);
			break;
		}
	}

	get_token(line,3,help);
	m_case_sensitive.SetCheck(help=="+");
	get_token(line,4,help);
	m_edit.SetWindowText(help);
}

// =====================================================================
// Build list line
	int exsearch::FormLine(CString &line)
// =====================================================================
{
CString field,searchtext,contains,not,casesens;
int		ret,retfield;

	retfield=m_dd_field.GetCurSel();
	if (retfield==LB_ERR)
		return 0;
	m_dd_field.GetLBText(retfield,field);

	if (retfield!=9)
	{
		ret=m_dd_contains.GetCurSel();
		if (ret==LB_ERR)
			return 0;
		m_dd_contains.GetLBText(ret,contains);
	}

	if (retfield<9)
	{
		m_edit.GetWindowText(searchtext);
		if (searchtext.GetLength()==0)
			return 0;
	}

	if (searchtext==L("S_26"))
		ERR_MSG_RET0("E_FDAYSNIC");

	not=m_not.GetCheck() ? "-" : "";
	casesens=m_case_sensitive.GetCheck() ? "+" : "";

	line.Format("%s\t%s\t%s\t%s\t%s",field,not,contains,casesens,searchtext);
	return 1;
}

// =====================================================================
	void exsearch::FillComboboxes(void)
// =====================================================================
{
	m_dd_field.ResetContent();
	m_dd_field.AddString(L("S_83"));
	m_dd_field.AddString(L("S_270"));
	m_dd_field.AddString(L("S_82"));
	m_dd_field.AddString(L("S_264"));
	m_dd_field.AddString(L("S_424"));
	m_dd_field.AddString(L("S_416"));
	m_dd_field.AddString(L("S_312"));
	m_dd_field.AddString(L("S_375"));
	m_dd_field.AddString(L("S_148"));
	m_dd_field.AddString(L("S_292"));
	m_dd_field.AddString(L("S_311"));
	m_dd_field.SetCurSel(0);
	m_dd_contains.ResetContent();
	m_dd_contains.AddString(L("S_140"));
	m_dd_contains.AddString(L("S_102"));
	m_dd_contains.SetCurSel(0);
	m_not.SetCheck(0);
	m_case_sensitive.SetCheck(0);
	m_edit.SetWindowText(NULL);
}

// =====================================================================
	void exsearch::FillAreasList(void)
// =====================================================================
{
areadef ad;
CString	line;
int		i;

	m_listarea.DeleteAllItems();
	for (i=0;;i++)
	{
		if (db_get_area_by_index(i,&ad)!=DB_OK)
			return;
		line.Format("%s\t%d",ad.echotag,ad.index);
		AddRow(m_listarea,line);
	}
}

// =====================================================================
	void exsearch::GetSelectedArea(void)
// =====================================================================
{
CString line;
POSITION pos;

	AreasForQuery.RemoveAll();
	pos=m_listarea.GetFirstSelectedItemPosition();
	while (pos)
	{
		GetRow(m_listarea,m_listarea.GetNextSelectedItem(pos),line);
		AreasForQuery.AddTail(line);
	}
}

// =====================================================================
	void exsearch::OnClearAll()
// =====================================================================
{
	for (int i=0;i<m_listarea.GetItemCount();i++)
		DeselectRow(m_listarea,i);
}

// =====================================================================
	void exsearch::OnSelAll()
// =====================================================================
{
	for (int i=0;i<m_listarea.GetItemCount();i++)
		SelectRow(m_listarea,i);
}

// =====================================================================
	int exsearch::MailsNumberOfSelectedArea(void)
// =====================================================================
{
CString line,buf;
DWORD	dummy;
areadef ad;
int		i,n,ret,areanr,NrOfMails=0,onearea=0;

	n=AreasForQuery.GetCount();
	for (i=0;i<n;i++)
	{
		line=AreasForQuery.GetString(i);
		get_token(line,1,buf);
		areanr=0;
		sscanf(buf,"%d",&areanr);
		db_open_simple(areanr);
		ret=db_get_area_by_index(areanr,&ad);
		ASSERT(ret==DB_OK);
		ASSERT(ad.hheader!=INVALID_HANDLE_VALUE);
		onearea=GetFileSize(ad.hheader,&dummy)/sizeof(mailheader);
		NrOfMails+=onearea;
	}
	return NrOfMails;
}

// =====================================================================
	int exsearch::UpdateProgressControl(int actual,int total)
// =====================================================================
{
static int last_proc=-1000;
int		proc,loc_total;

	loc_total=total;
	if (!loc_total)
		loc_total=100000;

	proc=actual*100/loc_total;

	if (proc<(last_proc+5) && proc!=100 && proc!=0)
		return 0;

	last_proc=proc;
	m_progress.SetRange(0,100);
	m_progress.SetPos(proc);
	return 1;
}

// ==================================================
	UINT SearchEngineWorkerThread(LPVOID p)
// ==================================================
{
areadef adef;
mailheader *mh,*mht;
char	destarea[300],mailtextbuf[MAX_MSG+1000];
int		areaindex,i,h,s,ret,retx,mustwrite;
int		eintragen=0,mailtext_gotten=0,mails_already_scanned=0;
int		SumNrOfMails=0,SumNrOfFoundMails=0,hdrcount=0,mustupdatestat=0;
long	last_updated=0;
long	hand=-1;
int		maxres=get_cfg(CFG_ESEARCH,"MaxResults",500);
int		andflag=get_cfg(CFG_ESEARCH,"AndFlag",0);
int		markfnd=get_cfg(CFG_ESEARCH,"MarkFound",0);
int		markrd=get_cfg(CFG_ESEARCH,"MarkRead",0);
int		procdel=get_cfg(CFG_ESEARCH,"ProcDeleted",0);

#ifdef DEBSEARCH
	char ddbuf[300];
#endif

	gc.SearchThreadRunning=1;
	(gSearch.GetDlgItem(IDOK))->EnableWindow(0);
	(gSearch.GetDlgItem(IDC_STOP_SEARCH))->EnableWindow(1);

	stop_search_thread=0;
	SumNrOfMails=gSearch.MailsNumberOfSelectedArea();
	#ifdef DEBSEARCH
		sprintf(ddbuf,"SearchEngine:\tSearching in %d mails\n",SumNrOfMails);
		OutputDebugString(ddbuf);
	#endif

	gSearch.UpdateProgressControl(0,SumNrOfMails);
	gSearch.m_status.SetWindowText(L("S_399"));
	gSearch.m_nr_found.SetWindowText(L("S_255",SumNrOfFoundMails));

	gSearch.m_edit_copy_to_area.GetWindowText(lochelp);
    trim_all(lochelp);
	strcpy(destarea,lochelp);

	for (i=0;i<AreasForQuery.GetCount();i++)
	{
		mustupdatestat=0;
		if (gc.SetOnFipsExit || stop_search_thread) 
			goto endthread;
		lochelp=AreasForQuery.GetString(i);
		areaindex=get_token_int(lochelp,1);
		ret=db_get_area_by_index(areaindex,&adef);
		if (ret!=DB_OK)
		{
			#ifdef DEBSEARCH
				sprintf(ddbuf,"SearchEngine:\tdb_get_area_by_index %d failed with %d\n",areaindex,ret);
				OutputDebugString(ddbuf);
			#endif
			continue;
		}
		mh=0;
		ret=db_getallheaders (areaindex,&mh,&hdrcount);
		if (ret!=DB_OK)
		{
			#ifdef DEBSEARCH
				sprintf(ddbuf,"SearchEngine:\tdb_getallheaders %d failed with %d\n",areaindex,ret);
				OutputDebugString(ddbuf);
			#endif
			continue;
		}
		mht=mh;
		for (h=0;h<hdrcount;h++)
		{
			if (procdel || !(mht->status & DB_DELETED))
			{
				if (gc.SetOnFipsExit || stop_search_thread) 
					goto endthread;
				gSearch.UpdateProgressControl(mails_already_scanned,SumNrOfMails);
				for (s=0;s<cscount;s++)
				{
					// Is this search object needed in Mailtext?
					if (csarray[s].mailtext_needed && !mailtext_gotten)
					{
						mailtextbuf[0]=0;
						mailtext_gotten=1;
						if (db_get_mailtext(areaindex,mht,mailtextbuf,MAX_MSG)!=DB_OK)
							goto nextheader;
					}
					if (csarray[s].WasSearchSuccessfull(mht,mailtextbuf))
					{
						if (!andflag)
							goto eintragen;// normal OR'ed query
						else if ((s+1)>=cscount)
							goto eintragen;
					}
					else if (andflag)
						goto nextheader;
				}
				goto nextheader;

eintragen:
 				SumNrOfFoundMails++;
				if (markfnd || markrd)
				{
					mustwrite=0;
					if (markfnd && !(mht->status & DB_USERMARKED))
					{
						mht->status |= DB_USERMARKED;
						mustwrite=1;
					}

					if (markrd && !(mht->status & DB_MAIL_READ))
					{
						mht->status |= DB_MAIL_READ;
						mustwrite=1;
						mustupdatestat=1;
					}

					if (mustwrite)
					{
						retx=db_sethdr_by_index(areaindex,mht->index,mht);
						if (retx!=DB_OK)
						{
							err_out("E_DBSETHDRBYINDEX",ret);
							goto endthread;
						}
					}
				}

				locall.Empty();
				prepare_result_format(locall,mht,&adef);
				locall2.Format("%s\t%d\t%d",locall,mht->index,mht->mailid);
				locall2.OemToAnsi();
				AddRow(gSearch.m_listres,locall2);

				if (strlen(destarea)>1)
				{
					int	ret;
					if (!mailtext_gotten)
					{
						ret=db_get_mailtext(areaindex,mht,mailtextbuf,MAX_MSG);
						if (ret!=DB_OK)
						{
							#ifdef DEBSEARCH
								sprintf(ddbuf,"SearchEngine:\tdb_getmailtext %d failed with %d\n",areaindex,ret);
								OutputDebugString(ddbuf);
							#endif
							goto nextheader;
						}

					}
					if (hand==-1)
					{

						gNewAreaFromExtSearch=1;
						ret=db_get_area_handle(destarea,&hand,1);
						gNewAreaFromExtSearch=0;
						if (ret==DB_OK)	db_set_area_to_carboncopy(hand);
					}
					else
						ret=DB_OK;
					if (ret!=DB_OK)
					{
						char xbuf[100];
						strcpy(xbuf,destarea);
						err_out("E_DBGETAHANDLE",xbuf);
					}
					else
					{
						char tmpmsg[MAX_MSG+3000];
						db_set_area_to_carboncopy(hand);
						strcpy(tmpmsg,langstr(CC_ENG,CC_RUS));
						strcat(tmpmsg,adef.echotag);
						strcat(tmpmsg,"\r");
						mht->text_length+=strlen(tmpmsg);                                                                      // ADDED
						strcat(tmpmsg,mailtextbuf);
						strcpy(mailtextbuf,tmpmsg);
						mht->mail_text=mailtextbuf;
						mht->status &= ~DB_MAIL_READ;
						ret=db_append_new_message(hand,mht);
						if (ret!=DB_OK)
							err_out("E_APPMFAILED");
					}

				}  // End of CarbonCopy

				gSearch.m_nr_found.SetWindowText(L("S_255",SumNrOfFoundMails));

				if (SumNrOfFoundMails>=	maxres)
				{
					#ifdef DEBSEARCH
						sprintf(ddbuf,"SearchEngine:\tMaximum reached, aborted\n");
						OutputDebugString(ddbuf);
					#endif
					gSearch.m_status.SetWindowText(L("S_297"));
					goto endthread;
				}

			}
nextheader:
			mails_already_scanned++;
			mht++;
			mailtext_gotten=0;
		}
		free(mh);

		if (mustupdatestat)
			db_refresh_area_info(areaindex);
	}

	gSearch.m_status.SetWindowText(L("S_137"));
	gSearch.UpdateProgressControl(100,100);

endthread:
	if (hand!=-1)
		db_refresh_area_info(hand);

	gSearch.m_status.SetWindowText(L("S_271"));
	system_sound("SEARCH_END");
	(gSearch.GetDlgItem(IDOK))->EnableWindow(1);
	(gSearch.GetDlgItem(IDC_STOP_SEARCH))->EnableWindow(0);
	gc.SearchThreadRunning=0;
	AfxEndThread(155);
	return 1;
}

// ==================================================
	void exsearch::OnClearSearch()
// ==================================================
{
	m_listres.DeleteAllItems();
	UpdateProgressControl(0,100);
	m_edit_max.SetWindowText(get_cfg(CFG_ESEARCH,"MaxResults","500"));
	m_ored.SetCheck(get_cfg(CFG_ESEARCH,"AndFlag",0));
	m_markallfound.SetCheck(get_cfg(CFG_ESEARCH,"MarkFound",0));
	m_markasread.SetCheck(get_cfg(CFG_ESEARCH,"MarkRead",0));
	m_procdeleted.SetCheck(get_cfg(CFG_ESEARCH,"ProcDeleted",0));
	m_status.SetWindowText(L("S_271"));
	m_nr_found.SetWindowText(L("S_258"));
}

// ==================================================
	void exsearch::OnClearQueryList()
// ==================================================
{
	m_listqry.DeleteAllItems();
	m_edit_copy_to_area.SetWindowText(NULL);
}

// ==================================================
	void exsearch::OnClickList2(NMHDR* pNMHDR, LRESULT* pResult) 
// ==================================================
{
CString	line,area;
int		sel,ret,index=0,mailid=0;
long	areaindex=0;

	GET_SELECT(m_listres);
	GetRow(m_listres,sel,line);
	area=m_listres.GetItemText(sel,0);
	index=atoi(m_listres.GetItemText(sel,3));
	mailid=atoi(m_listres.GetItemText(sel,4));
    ret=db_get_area_handle(area,&areaindex,0,NULL);
	if (ret!=DB_OK)
		ERR_MSG2_RET("E_CANFAX",area);

	DisplayThisMail(areaindex,index,mailid);
}
	
// ==================================================
	int	exsearch::DisplayThisMail(int areaindex,int index,int mailid)
// ==================================================
// TOP: Diese funktion stellt die entsprechende Mail dar und beruecksichtigt
//      auch einen eventuellen Areawechsel
{
	gustat.cur_area_handle=areaindex;
	gpMain->begin_area_selection(TRUE);

	gc.WatchDisplayMails=1;
	watchmail.index=index;
	watchmail.mailid=mailid;
	watchmail.dispoff=-1;
	watchmail.isCrossfind=0;
	gpMain->show_subjects(gustat.cur_area_handle);
	gc.WatchDisplayMails=0;

	if (watchmail.dispoff==-1)
	{
		show_msg("Mail not found","Письмо не найдено",500);
		return 0;
	}
	gpMain->m_subjlist.SetSel(-1,0);
	gpMain->m_subjlist.SetSel(watchmail.dispoff,1);

	int topindex=watchmail.dispoff-3;
	if (topindex<0)
		topindex=0;

	gpMain->m_subjlist.SetTopIndex(topindex);
	gpMain->OnSelchangeList();
	gpMain->m_subjlist.SetFocus();
	return 1;
}

// ==================================================
	int	prepare_result_format(CString &lc,struct mailheader *mh,struct areadef *adef)
// ==================================================
{
CString all;
char	fm[300],sult[300],*p;

	strcpy(fm,resultdisp.GetString(0));
	p=fm;

	while (*p)
	{
		switch (*p)
		{
			case 'A':
				all+=adef->echotag;
				break;
			case 'S':
				all+=mh->fromname;
				break;
			case 'E':
				all+=mh->toname;
				break;
			case 'T':
				all+='\t';
				break;
			case ' ':
				all+=' ';
				break;
			case 'I':
				all+=mh->subject;
				break;
			case 'D':
				unix_time_to_fido(mh->recipttime,sult);
				all+=sult;
				break;
			case 'K':
				unix_time_to_fido(mh->mailid,sult);
				all+=sult;
				break;
			case 'X':
				make_address(sult,mh->srczone,mh->srcnet,mh->srcnode,mh->srcpoint);
				all+=sult;
				break;
			case 'Y':
				make_address(sult,mh->dstzone,mh->dstnet,mh->dstnode,mh->dstpoint);
				all+=sult;
				break;
			default:
				ASSERT(0);
				break;
		}
		p++;
	}
	lc+=all;
	return 1;
}

// ==================================================
	void exsearch::OnDisplay()
// ==================================================
{
	resultfm dlg;
	dlg.DoModal();
}

// =====================================================================
	void exsearch::OnSaveResults()
// =====================================================================
{
CString	str;
int		ret;

	if (m_listres.GetItemCount()<=0)	return;
	str.LoadString(IDS_QRESFILT);
	CFileDialog dlg(FALSE,"qre",NULL,OFN_HIDEREADONLY,str);
	ret=dlg.DoModal();
	restore_base_path();
	if (ret!=IDOK)	return;
	SaveToDelimText(m_listres,dlg.GetPathName(),-1,'\t');
}

// =====================================================================
	void exsearch::OnSaveResults2(LPCSTR path)
// =====================================================================
{
	if (m_listres.GetItemCount()>0)
		SaveToDelimText(m_listres,path,-1,'\t');
}

// =====================================================================
	void exsearch::OnLoadResults()
// =====================================================================
{
CString	str;
int		ret;

	str.LoadString(IDS_QRESFILT);
	CFileDialog dlg(TRUE,"qre",NULL,OFN_HIDEREADONLY,str);
	ret=dlg.DoModal();
	restore_base_path();
	if (ret!=IDOK)	return;
	LoadFromDelimText(m_listres,dlg.GetPathName(),1,'\t');
}

// =====================================================================
	void exsearch::OnLoadResults2(LPCSTR path)
// =====================================================================
{
	LoadFromDelimText(m_listres,path,1,'\t');
}

// =====================================================================
	void exsearch::DisplayFirstResult(void)
// =====================================================================
{
	if (m_listres.GetItemCount()<=0)
		return;

	SelectRow(m_listres,0);
	OnClickList2(0,0);
}

// =====================================================================
	void exsearch::DisplayNextResult(void)
// =====================================================================
{
int count;
int sel;

	count=m_listres.GetItemCount();
	if (count<=0)
		return;

	sel=GetSelectedItem(m_listres);
	if (sel<0)
	{
		DisplayFirstResult();
		return;
	}
	sel++;
	if (sel>=count)
		return;

	SelectRow(m_listres,sel);
	OnClickList2(0,0);
}

// =====================================================================
	void exsearch::OnDispNextQResult()
// =====================================================================
{
	DisplayNextResult();
}

// ===========================================
	void exsearch::SetLanguage(void)
// ===========================================
{
int  lng[]={
			IDC_NOT,
			IDC_ADD,
			IDC_DELETE,
			IDC_CLEAR_QUERY_LIST,
			IDOK,
			IDC_LOAD_QUERY,
			IDC_SAVE_QUERY,
			IDC_SEL_ALL,
			IDC_CLEAR_ALL,
			IDC_HIDE,
			IDHELP,
			IDC_STOP_SEARCH,
			IDC_CLEAR_SEARCH,
			IDC_DISPLAY,
			IDC_LOAD_RESULTS,
			IDC_SAVE_RESULTS,
			IDC_ORED,
			IDC_STATIC45,
			IDC_STATIC_NR_FOUND,
			IDC_STATIC_123,
			IDC_STATIC876,
			IDC_CASE_SESITIVE,
			IDC_CHANGE,
			IDC_STATIC639,
			IDC_STATIC352,
			IDC_MARKALLFOUND,
			IDC_MARKALLFOUNDASREAD,
			IDC_PROCDELETED
			};

    set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));
    StoreOrgItemSizes(DlgName,this,DlgItemsSize,sizeof(DlgItemsSize)/sizeof(_DlgItemsSize));
	OnClearSearch();
	FillComboboxes();
	RefreshColumns(m_listqry,qhdrs);
	RefreshColumns(m_listarea,ahdrs);
	RefreshColumns(m_listres,rhdrs);
}

// ===========================================
// Handler for fields combobox
	void exsearch::OnSelchangeDdField()
// ===========================================
{
CString field;
int		sel;

	sel=m_dd_field.GetCurSel();
	LB_ERR_RET;

	m_dd_field.GetLBText(sel,field);
	if(field==L("S_292"))
	{
		m_dd_contains.ResetContent();
		m_dd_contains.EnableWindow(0);
		m_not.SetCheck(0);
		m_not.EnableWindow(0);
		m_edit.SetWindowText(NULL);
		m_edit.EnableWindow(0);
		m_case_sensitive.SetCheck(0);
		m_case_sensitive.EnableWindow(0);
	}
	else if(field==L("S_375") || field==L("S_148"))
	{
		m_dd_contains.ResetContent();
		m_dd_contains.AddString(L("S_473"));
		m_dd_contains.SetCurSel(0);
		m_dd_contains.EnableWindow(1);
		m_not.EnableWindow(1);
		m_edit.SetWindowText(L("S_26"));
		m_edit.EnableWindow(1);
		m_case_sensitive.EnableWindow(0);
	}
	else if(field==L("S_311"))
	{
		m_dd_contains.ResetContent();
		m_dd_contains.AddString(L("S_395"));
		m_dd_contains.AddString(L("S_325"));
		m_dd_contains.AddString(L("S_85"));
		m_dd_contains.AddString(L("S_324"));
		m_dd_contains.AddString(L("S_267"));
		m_dd_contains.AddString(L("S_458"));
		m_dd_contains.AddString(L("S_313"));
		m_dd_contains.SetCurSel(0);
		m_dd_contains.EnableWindow(1);
		m_not.EnableWindow(1);
		m_edit.SetWindowText(NULL);
		m_edit.EnableWindow(0);
		m_case_sensitive.SetCheck(0);
		m_case_sensitive.EnableWindow(0);
	}
	else
	{
		m_dd_contains.EnableWindow(1);
		m_dd_contains.ResetContent();
		m_dd_contains.AddString(L("CONTAINS","СОДЕРЖИТ"));
		m_dd_contains.AddString(L("BEGINS","НАЧИНАЕТСЯ С"));
		m_dd_contains.SetCurSel(0);
		m_dd_contains.EnableWindow(1);
		m_not.EnableWindow(1);
		m_edit.EnableWindow(1);
		m_case_sensitive.EnableWindow(1);
	}
}

// ===========================================
	void exsearch::OnBrowseAreas()
// ===========================================
{
	sedesta dlg;
	dlg.DoModal();

	if (gSearchDestArea.GetLength()>1)
		m_edit_copy_to_area.SetWindowText(gSearchDestArea);
}

// ===========================================
	BOOL exsearch::PreTranslateMessage(MSG* pMsg)
// ===========================================
{
	if (handle_function_key(pMsg))	return 1;
	return CDialog::PreTranslateMessage(pMsg);
}

// ===========================================
	void exsearch::OnClose()
// ===========================================
{
	OnOk2();
}

// =====================================================================
	void exsearch::OnOk2() 		 // HIDE Button
// =====================================================================
{
	SaveColumnsWidth(m_listqry,DlgName,1);
	SaveColumnsWidth(m_listarea,DlgName,2);
	SaveColumnsWidth(m_listres,DlgName,3);
	ShowWindow(SW_HIDE);
	CDialog::OnCancel();
}

// ============================================
	void exsearch::OnSize(UINT nType,int cx,int cy)
// ============================================
{
	CDialog::OnSize(nType, cx, cy);
	resize_wnd(this,DlgItemsSize,sizeof(DlgItemsSize));
}

// ============================================
	void exsearch::OnPaint()
// ============================================
{
	CPaintDC dc(this);
	DrawRightBottomKnubble(this,dc);
}

// ===========================================
	void exsearch::SavePosition(void)
// ===========================================
{
    StoreInitDialogSize(DlgName,this);
}

// ===========================================
	void exsearch::OnMove(int x,int y)
// ===========================================
{
	CDialog::OnMove(x, y);
}

// ===========================================
	void exsearch::OnItemchangedList(NMHDR* pNMHDR, LRESULT* pResult) 
// ===========================================
{
int		sel;
CString line;
	
	GET_SELECT(m_listqry);
	GetRow(m_listqry,sel,line);
	ParseLine(line);
	OnSelchangeDdField();
	ParseLine(line);
	*pResult = 0;
}

// =====================================================================
	void exsearch::OnShowWindow(BOOL bShow, UINT nStatus) 
// =====================================================================
{
	CDialog::OnShowWindow(bShow, nStatus);
	(gpMain->GetMenu())->CheckMenuItem(ID_SWITCH_EXTSEARCH, 
		MF_BYCOMMAND | (bShow ? MF_CHECKED : MF_UNCHECKED));
	if (!bShow)	gpMain->m_subjlist.SetFocus();
}

// =====================================================================
	void exsearch::OnHelp()
// =====================================================================
{
	WinHelp(VHELP_EXT_SEARCH);
}

BOOL exsearch::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return TRUE;
}
