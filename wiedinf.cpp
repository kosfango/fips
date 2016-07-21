// presinf.cpp : implementation file
// IDD_REPOST_FOLDER

#include "stdafx.h"
#include <direct.h>
#include <io.h>
#include "lightdlg.h"
#include "wiedinf.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CLightDlg *gpMain;
extern _gconfig gc;

static char DlgName[]="IDD_REPOST_FOLDER";

// =======================================================================
	presinf::presinf(CWnd* pParent ) : CDialog(presinf::IDD, pParent)
// =======================================================================
{
	//{{AFX_DATA_INIT(presinf)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void presinf::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(presinf)
	DDX_Control(pDX, IDC_LIST, m_listctl);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(presinf, CDialog)
	//{{AFX_MSG_MAP(presinf)
	ON_BN_CLICKED(IDC_DELETE_REPOST, OnDeleteRepost)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_BN_CLICKED(IDC_REPOST_NOW, OnRepostNow)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static _DlgItemsSize DlgItemsSize [] =
{
	0,					{0,0,0,0},{0,0,0,0},0,0,0,0,0,
	IDC_REPOST_NOW,	    {0,0,0,0},{0,0,0,0},HO|VE,HO|VE,HO|VE,HO|VE,0,
	IDC_DELETE_REPOST,	{0,0,0,0},{0,0,0,0},HO|VE,HO|VE,HO|VE,HO|VE,0,
	IDOK,				{0,0,0,0},{0,0,0,0},HO|VE,HO|VE,HO|VE,HO|VE,0,
	IDHELP,				{0,0,0,0},{0,0,0,0},VE,VE,HO|VE,HO|VE,0,
	IDC_LIST,			{0,0,0,0},{0,0,0,0},0,0,HO|VE,HO|VE,0,
};

// =======================================================================
	BOOL presinf::OnInitDialog()
// =======================================================================
{
_listcol lc[]={70,"",70,"",70,"",130,"",110,"",110,"",250,"",-1,"file"};
int  lng[]={
			IDOK,
			IDC_REPOST_NOW,
			IDC_DELETE_REPOST,
			IDHELP,
			IDC_STATIC1,
			};

    CDialog::OnInitDialog();
	strcpy(lc[0].hdr,L("S_490"));	// start
	strcpy(lc[1].hdr,L("S_556"));	// interval
	strcpy(lc[2].hdr,L("S_496"));	// remain
	strcpy(lc[3].hdr,L("S_557"));	// area
	strcpy(lc[4].hdr,L("S_558"));	// from
	strcpy(lc[5].hdr,L("S_559"));	// to
	strcpy(lc[6].hdr,L("S_560"));	// subject
    set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));
    StoreOrgItemSizes(DlgName,this,DlgItemsSize,sizeof(DlgItemsSize)/sizeof(_DlgItemsSize));
	MakeColumns(m_listctl,lc,sizeof(lc)/sizeof(_listcol),DlgName,1);
	populate_list();
	return TRUE;
}

// =======================================================================
	void presinf::OnDeleteRepost()
// =======================================================================
{
int		sel;
char	path[MAX_PATH],base[MAX_PATH];

	GET_SELECT(m_listctl);
	make_path(base,gc.MsgBasePath,REPOST_FLD);
	make_path(path,base,m_listctl.GetItemText(sel,7));
	unlink(path);
	populate_list();
}

// =======================================================================
	void presinf::OnRepostNow()
// =======================================================================
{
char	path[MAX_PATH],fpath[MAX_PATH],npath[MAX_PATH];
int		sel,count=11;

	sel=GetSelectedItem(m_listctl); if (sel<0) return;
	make_path(path,gc.MsgBasePath,REPOST_FLD);
	make_path(fpath,path,m_listctl.GetItemText(sel,7));

	do sprintf(npath,"%s%x",fpath,count++);
	while (!access(npath,0));

	rename(fpath,npath);
	check_repost();
	populate_list();
	gpMain->OnUpdateScreenDisplay();
}

// =======================================================================
	void presinf::OnOK()
// =======================================================================
{
	StoreInitDialogSize(DlgName,this);
	SaveColumnsWidth(m_listctl,DlgName,1);
	CDialog::OnOK();
}

// =======================================================================
	void presinf::populate_list()
// =======================================================================
{
_repost rp;
FILE	*fp;
mailheader mh;
CStrList lst;
CString	s1,s2,s3;
char	base[MAX_PATH],path[MAX_PATH],line[1000],buf[500];
double	diff;
UINT	ft;
time_t	now;
tm		*pt;

	lst.RemoveAll();
	make_path(base,gc.MsgBasePath,REPOST_FLD);
	mkdir(base);
	make_path(path,base,"*.");
	lst.FillWithFiles(path);
	_tzset();
	now=time(NULL)-_timezone;
	memset(&mh,0,sizeof(mh));
	m_listctl.DeleteAllItems();
	for (int i=0;i<lst.GetCount();i++)
	{
		strcpy(line,lst.GetString(i));
		ft=0;
		sscanf(line,"%x",&ft);
		diff = (ft-now)/86400.;
 		make_path(path,base,line);
		fp=fopen(path,"rb");
		if (fp)
		{
			memset(&rp,0,sizeof(rp));
			fread(&rp,sizeof(rp),1,fp);
			fread(&mh,sizeof(mh),1,fp);

			s1=mh.fromname;
			s2=mh.toname;
			s3=mh.subject;
			s1.OemToAnsi();
			s2.OemToAnsi();
			s3.OemToAnsi();
			pt=gmtime(&now);
			sprintf(buf,"");
			sprintf(buf,"%02d.%02d.%d\t%d %s\t%.2f %s\t%s\t%s\t%s\t%s\t%x",
				pt->tm_mday,pt->tm_mon+1,pt->tm_year+1900,rp.period,L("S_561"),diff,L("S_561"),
				rp.echotag,s1,s2,s3,ft); // days
			AddRow(m_listctl,buf);
			fclose(fp);
		}
	}
}

// ============================================
	void presinf::OnSize(UINT nType, int cx, int cy)
// ============================================
{
	CDialog::OnSize(nType, cx, cy);
	resize_wnd(this,DlgItemsSize,sizeof(DlgItemsSize));
}

// ============================================
	void presinf::OnPaint()
// ============================================
{
	CPaintDC dc(this);
	DrawRightBottomKnubble(this,dc);
}

// =======================================================================
	void presinf::OnHelp()
// =======================================================================
{
	WinHelp(VHELP_PRESENT_INFO);
}

// =======================================================================
	BOOL presinf::OnHelpInfo(HELPINFO* pHelpInfo) 
// =======================================================================
{
	OnHelp();
	return TRUE;
}
