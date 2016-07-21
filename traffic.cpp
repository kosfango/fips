// traffic.cpp : implementation file
// IDD_TRAFFIC

#include "stdafx.h"
#include <io.h>
#include "lightdlg.h"
#include "traffic.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define MAX_AREA_NUM 5000

CStrList all_areas;
extern CLightDlg 	*gpMain;

static _DlgItemsSize DlgItemsSize [] =
{
	0,			{0,0,0,0},{0,0,0,0},0,0,0,0,0,
	IDC_GENERIC1,{0,0,0,0},{0,0,0,0},0,0,VE|HO,VE|HO,0,
	IDC_BUTTON1,{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDHELP,		{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDOK,		{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
};
static char DlgName[]="IDD_TRAFFIC";

// ========================================================
	traffic::traffic(CWnd* pParent ) : CDialog(traffic::IDD, pParent)
// ========================================================
{
	//{{AFX_DATA_INIT(traffic)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void traffic::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(traffic)
	DDX_Control(pDX, IDC_GENERIC1, m_traffic);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(traffic, CDialog)
	//{{AFX_MSG_MAP(traffic)
	ON_BN_CLICKED(IDC_BUTTON1, OnClearStatistics)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// ========================================================
	void traffic::OnOK()
// ========================================================
{
    StoreInitDialogSize (DlgName,this);
	CDialog::OnOK();
}

// ========================================================
	BOOL traffic::OnInitDialog()
// ========================================================
{
int  lng[]={
			IDOK,
			IDC_STATIC1,
			IDC_BUTTON1,
			IDHELP
			};

	CDialog::OnInitDialog();
	gpMain->BeginWaitCursor();
    set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));
    StoreOrgItemSizes(DlgName,this,DlgItemsSize,sizeof(DlgItemsSize)/sizeof(_DlgItemsSize));
	all_areas.RemoveAll();
	display_the_stuff();
	create_statistic();
	m_traffic.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	all_areas.RemoveAll();
	gpMain->EndWaitCursor();
	return TRUE;
}

//	==============================================
	int traffic::display_the_stuff()
//	==============================================
{
LV_COLUMN col;				// CListCtrl
int		spal=0;

	memset(&col,0,sizeof(col));
	col.mask=LVCF_TEXT | LVCF_WIDTH ;       // see below
	col.cx  =60;
	col.pszText=L("S_514");
	m_traffic.InsertColumn(spal,&col);
	return 1;
}

//	==============================================
	int traffic::create_statistic()
//	==============================================
{
LV_COLUMN col;
LV_ITEM item;
FILE	*fp;
char	line[3000],area[100],io[5];
int		array_in[MAX_AREA_NUM],array_out[MAX_AREA_NUM];
int		last_day=-1,last_mon=-1,last_year=-1,last_day_mode=0,itemcount=0;
int		count,i,ret,number;
time_t	time=0;
tm		*tt;
CString help,help1,xx;

	fp=fopen("traffic.log","rt");
	if (fp  && _filelength(fileno(fp))>1)
	{
		while (fgets(line,2999,fp))
		{
			 if (line[0]==';')   continue;	// comment line
			 sscanf(line,"%s %x %s %d",io,&time,area,&number);
 			 insert_area(area);
		}
		fclose(fp);
	}
	else
		ERR_MSG_RET1("W_NOTRAFDATA");

   count=all_areas.GetCount();
   ASSERT(count <MAX_AREA_NUM);

   for (i=0;i<count;i++)
   {
		memset(&col,0,sizeof(col));
		col.mask=LVCF_TEXT | LVCF_WIDTH ;
		col.cx  =80;
		help1.Format("%s",all_areas.GetString(i));
		col.pszText=PS help1;
		m_traffic.InsertColumn(i+1,&col);
   }

	memset(&array_in,0,sizeof(array_in)); 
	memset(&array_out,0,sizeof(array_out));
	fp=fopen("traffic.log","rt");
	ASSERT (fp);

	while (fgets(line,2999,fp))
	{
		if (line[0]==';')  continue;	// comment
		ret=sscanf(line,"%s %x %s %d",io,&time,area,&number);
		if (ret!=4)	continue;

		tt=localtime(&time);
		if (!tt)	continue;

		if (tt->tm_year > 99)
			tt->tm_year -= 100;

		if (last_day==-1)
		{
			last_day = tt->tm_mday;
			last_mon = tt->tm_mon;
			last_year= tt->tm_year;
		}

		if (last_year==tt->tm_year && last_mon==tt->tm_mon && last_day==tt->tm_mday)
		{
			ASSERT(*io=='I' || *io=='E');
			if (io[0]=='I')
			   array_in[find_area(area)]+=number;
			else
			   array_out[find_area(area)]+=number;
		}
		else
		{
write_one_st_line:
			count=all_areas.GetCount();
		    ASSERT(count <MAX_AREA_NUM);

			memset(&item,0,sizeof(item));
			item.mask=LVCF_TEXT ;
			item.iItem=itemcount;
			m_traffic.InsertItem(&item);

			memset(&item,0,sizeof(item));
			item.mask=LVIF_TEXT ;
            help.Format("%02d/%02d/%02d",last_day,last_mon+1,last_year);
			item.pszText=PS help;
			item.iItem=itemcount;
			item.iSubItem=0;
			m_traffic.SetItem(&item);

			for (i=0;i<count;i++)
			{
				memset(&item,0,sizeof(item));
				item.mask=LVIF_TEXT;

				if (array_out[i]>0)
					help1.Format("%d ( %d )",array_in[i],array_out[i]);
				else
					help1.Format("%d",array_in[i]);
				item.pszText=PS help1;
				item.iItem=itemcount;
				item.iSubItem=i+1;
				m_traffic.SetItem(&item);
			}
			itemcount++;
			memset(&array_in,0,sizeof(array_in)); 
			memset(&array_out,0,sizeof(array_out));
			ASSERT(*io=='I' || *io=='E');
			if (io[0]=='I')
			   array_in[find_area(area)]+=number;
			else
			   array_out[find_area(area)]+=number;
			if (last_day_mode)
				return 1;
		}
		last_day	= tt->tm_mday;
		last_mon	= tt->tm_mon;
		last_year	= tt->tm_year;
	}
	fclose(fp);
	last_day_mode=1;
  	goto write_one_st_line;
}

//	==============================================
	void traffic::insert_area(LPCSTR area)
//	==============================================
{
int i,count;
int vgl;

	count=all_areas.GetCount();
	if (!count)
	{
	   all_areas.AddTail(area);
	   return;
	}

	for (i=0;i<count;i++)
	{
		vgl=strcmp(area,all_areas.GetString(i));
		if (!vgl)
		   return;

		if (vgl<1)
		{
			all_areas.InsertBefore(all_areas.FindIndex(i),area);
			return;
		}
	}
    all_areas.AddTail(area);
}

//	==============================================
	int traffic::find_area(LPCSTR area)
//	==============================================
{
int count;

	count=all_areas.GetCount();
	for (int i=0;i<count;i++)
	{
		if (!strcmp(area,all_areas.GetString(i)))	return i;
	}
    return 0;
}

//	==============================================
	void traffic::OnClearStatistics()
//	==============================================
{
	if (err_out("DN_CLEARSTAT") == IDYES)
	{
		unlink("traffic.log");
		OnOK();
	}
}

// ============================================
	void traffic::OnSize(UINT nType, int cx, int cy)
// ============================================
{
	CDialog::OnSize(nType, cx, cy);
	resize_wnd(this,DlgItemsSize,sizeof(DlgItemsSize));
}

// ===============================================
	void traffic::OnPaint()
// ===============================================
{
	CPaintDC dc(this);
	DrawRightBottomKnubble(this,dc);
}

//	==============================================
	void traffic::OnHelp()
//	==============================================
{
	WinHelp(VHELP_TRAFFIC);
}

BOOL traffic::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return TRUE;
}
