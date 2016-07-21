// logdisp.cpp : implementation file
// IDD_LOGFILE_DISPLAY

#include "stdafx.h"
#include <io.h>
#include "logdisp.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

CString loctext;
extern CRITICAL_SECTION GlobalLogFileAccess;
static char DlgName[]="IDD_LOGFILE_DISPLAY";

static _DlgItemsSize DlgItemsSize [] =
{
	0,							{0,0,0,0},{0,0,0,0},0,0,0,0,0,
	IDC_EDIT1				   ,{0,0,0,0},{0,0,0,0},0,0,VE|HO,VE|HO,0,
	IDHELP	   ,{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDOK					   ,{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,

};

// =================================================================
	logdisp::logdisp(CWnd* pParent)	: CDialog(logdisp::IDD, pParent)
// =================================================================
{
	//{{AFX_DATA_INIT(logdisp)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

// =================================================================
	void logdisp::DoDataExchange(CDataExchange* pDX)
// =================================================================
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(logdisp)
	DDX_Control(pDX, IDC_EDIT2, m_type);
	DDX_Control(pDX, IDC_EDIT1, m_edit);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(logdisp, CDialog)
	//{{AFX_MSG_MAP(logdisp)
	ON_BN_CLICKED(IDC_MAILER_LOGFILE, OnMailerLogfile)
	ON_BN_CLICKED(IDC_PURGER_LOGFILE, OnPurgerLogfile)
	ON_BN_CLICKED(IDC_TOSSER_LOGFILE, OnTosserLogfile)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_BN_CLICKED(IDC_EVENTS_LOGFILE, OnEventsLogfile)
	ON_BN_CLICKED(IDC_NODELIST_LOGFILE, OnNodelistLogfile)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// =================================================================
	void logdisp::OnMailerLogfile()
// =================================================================
{
	m_type.SetWindowText(L("S_308"));	// MAILER LOGFILE
	DisplayFile("fipsmail.log");
}

// =================================================================
	void logdisp::OnPurgerLogfile()
// =================================================================
{
	m_type.SetWindowText(L("S_369"));	// PURGER LOGFILE
	DisplayFile("fipspurg.log");
}

// =================================================================
	void logdisp::OnTosserLogfile()
// =================================================================
{
	m_type.SetWindowText(L("S_427"));	// TOSSER LOGFILE
	DisplayFile("fipstoss.log");
}

// =================================================================
	void logdisp::OnEventsLogfile()
// =================================================================
{
	m_type.SetWindowText(L("S_227"));	// EVENTS LOGFILE
	DisplayFile("fipsevnt.log");
}

// =================================================================
	void logdisp::OnNodelistLogfile()
// =================================================================
{
	m_type.SetWindowText(L("S_340"));	// NODELIST LOGFILE
	DisplayFile("fipsnodl.log");
}

// =================================================================
	void logdisp::OnOK()
// =================================================================
{
    StoreInitDialogSize(DlgName,this);
	CDialog::OnOK();
}

// =================================================================
	BOOL logdisp::OnInitDialog()
// =================================================================
{
int  lng[]={
			IDC_MAILER_LOGFILE,
			IDC_TOSSER_LOGFILE,
			IDC_PURGER_LOGFILE,
			IDC_EVENTS_LOGFILE,
			IDC_NODELIST_LOGFILE,
			IDHELP,
			IDOK
			};
	
	CDialog::OnInitDialog();
    set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));
    StoreOrgItemSizes(DlgName,this,DlgItemsSize,sizeof(DlgItemsSize)/sizeof(_DlgItemsSize));
	m_type.SetWindowText(L("S_354"));	// select
	return TRUE;
}

// =================================================================
	void logdisp::DisplayFile(char *filename)
// =================================================================
{
FILE *fp;
char buf[10000],*x;
int  dateigroesse,counter=0;

	 BeginWaitCursor();
	 // First clear
	 m_edit.SetWindowText(NULL);

	 EnterCriticalSection(&GlobalLogFileAccess);

nochmal:
	 fp=fopen(filename,"rt");
	 if (!fp)
	 {
		 Sleep(10);
		 counter++;
		 if (counter<20)
		   	goto nochmal;
		 else
		 {
			 err_out("E_COLFD",filename);
			 EndWaitCursor();
			 goto endit;
		 }
	 }
     dateigroesse=_filelength(_fileno(fp));
	 if (dateigroesse >31500)
	 {
		fseek(fp,dateigroesse-31000,SEEK_CUR);
		fgets(buf,9999,fp);
	 }

	 loctext.Empty();
	 while (fgets(buf,9999,fp))
	 {
		 x=strchr(buf,'\n');
		 if (x)
		 {
		    *x='\r';
			x++;
		    *x='\n';
			x++;
			*x=0;
			loctext+=buf;
		 }
 	 }
	 fclose(fp);
	 m_edit.SetWindowText(loctext);
	 m_edit.SetSel(loctext.GetLength(),loctext.GetLength());
	 EndWaitCursor();

endit:
	 LeaveCriticalSection(&GlobalLogFileAccess);
}

// ============================================
	void logdisp::OnSize(UINT nType, int cx, int cy)
// ============================================
{
	CDialog::OnSize(nType, cx, cy);
	resize_wnd(this,DlgItemsSize,sizeof(DlgItemsSize));
}

// ===============================================
	void logdisp::OnPaint()
// ===============================================
{
	CPaintDC dc(this);
	DrawRightBottomKnubble(this,dc);
}

// =================================================================
	void logdisp::OnHelp()
// =================================================================
{
	WinHelp(VHELP_LOGFILE_DISPLAY);
}

BOOL logdisp::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return TRUE;
}
