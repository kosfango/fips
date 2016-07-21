// cha_xfer.cpp : implementation file
// IDD_XFERFLS_EDIT

#include "stdafx.h"
#include "cha_xfer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern char gXferFile[300];
static char DlgName[]="IDD_XFERFLS_EDIT";

cha_xfer::cha_xfer(CWnd *pParent /*=NULL*/)
	: CDialog(cha_xfer::IDD, pParent)
{
	//{{AFX_DATA_INIT(cha_xfer)
	m_edit = _T("");
	//}}AFX_DATA_INIT
}

void cha_xfer::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(cha_xfer)
	DDX_Text(pDX, IDC_EDIT, m_edit);
	DDV_MaxChars(pDX, m_edit, 500);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(cha_xfer, CDialog)
	//{{AFX_MSG_MAP(cha_xfer)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// cha_xfer message handlers

	BOOL cha_xfer::OnInitDialog()
{
char buf[400];
FILE *fp;
char *p;
int  lng[]={
			IDCANCEL,
			IDOK,
			IDHELP
			};

	CDialog::OnInitDialog();
    set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));
	fp=fopen(gXferFile,"rt");
	if (fp)
	{
		while (fgets(buf,399,fp))
		{
			p=strchr(buf,'\n');	if (p) *p=0;
			p=strchr(buf,'\r');	if (p) *p=0;
			strcat(buf,"\r\n");
			m_edit+=buf;
		}
		fclose(fp);
    }
	UpdateData(0);
	return TRUE;
}

	void cha_xfer::OnCancel()
{
	CDialog::OnCancel();
}

	void cha_xfer::OnOK()
{
int		i;
CString line;
CStrList files;
FILE	*fp;
char	*p;
int		count=0;

	UpdateData(1);

	files.SplitTabs(PS m_edit,'\n');

	fp=fopen(gXferFile,"wt");
	if (fp)
	{
		for (i=0;i<files.GetCount();i++)
		{
		   line=files.GetString(i);
           trim_all(line);
		   p= (char *) strchr(line,'\r');
		   if (p)
		      *p=0;
		   if (strlen(line)>1)
		   {
		      fprintf(fp,"%s\n",line);
			  count++;
		   }
		}
		fclose(fp);
	}
	else
	    err_out("E_COXFWF",gXferFile);

	if (!count)
	{
	   Sleep(100);
	   unlink(gXferFile);
	}
	CDialog::OnOK();
}

	void cha_xfer::OnHelp()
{
	WinHelp(VHELP_OUTBOUND_VIEW);
}

BOOL cha_xfer::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return TRUE;
}
