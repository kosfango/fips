// ticedit.cpp : implementation file
// "IDD_TIC_EDITOR"

#include "stdafx.h"
#include <io.h>
#include "outbound.h"
#include "cfido.h"
#include "filereq.h"
#include "ticedit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern	CString gEditTICPath;
CString gLastDisplayedTicFile="";

CStrList available_ticfiles;
int		caseexit=0;
static char DlgName[]="IDD_TIC_EDITOR";

// =============================================================
	ticedit::ticedit(CWnd* pParent ) : CDialog(ticedit::IDD, pParent)
// =============================================================
{
	//{{AFX_DATA_INIT(ticedit)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

// =============================================================
	void ticedit::DoDataExchange(CDataExchange* pDX)
// =============================================================
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ticedit)
	DDX_Control(pDX, IDC_LIST, m_list);
	DDX_Control(pDX, IDC_EDIT, m_edit);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(ticedit, CDialog)
	//{{AFX_MSG_MAP(ticedit)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_LBN_SELCHANGE(IDC_LIST, OnSelchangeList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// =============================================================
	BOOL ticedit::OnInitDialog()
// =============================================================
{
int  lng[]={
			IDC_DELETE,
			IDCANCEL,
			};

    CDialog::OnInitDialog();
    set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));
	caseexit=0;
	m_edit.SetModify(0);
	fill_tic_box();
	m_list.SetCurSel(0);
	OnSelchangeList();
	return TRUE;
}

// =============================================================
	void ticedit::OnDelete()
// =============================================================
{
char path[MAX_PATH];
int	 sel;

	GET_SELID(IDC_LIST);
	make_path(path,gEditTICPath,available_ticfiles.GetString(sel));
	if (err_out("DN_DRYWDTTF",path)==IDNO)	return;
	unlink(path);
	m_edit.SetWindowText(NULL);
	fill_tic_box();
	m_list.SetCurSel(0);
	OnSelchangeList();
}

// =============================================================
	void ticedit::fill_tic_box()
// =============================================================
{
char path[MAX_PATH];

	make_path(path,gEditTICPath,"*.tic");
	available_ticfiles.FillWithFiles(path);
	UPDATE_LB(available_ticfiles,IDC_LIST);
}

// =============================================================
	void ticedit::OnCancel()
// =============================================================
{
	caseexit=1;
	OnSelchangeList();
	gEditTICPath.Empty();
	gLastDisplayedTicFile.Empty();
	available_ticfiles.RemoveAll();
	CDialog::OnCancel();
}

// =============================================================
	void ticedit::OnSelchangeList()
// =============================================================
{
FILE	*fp;
int		sel,i;
char	*p;
CString description,str,line;
CStrList lines;
char	buf[10000],path[MAX_PATH];

	if (available_ticfiles.GetCount()==0)
		return;

	if (m_edit.GetModify())
	{
		ASSERT(strlen(gLastDisplayedTicFile)>2);
		if (err_out("DY_SSCTTF",gLastDisplayedTicFile)==IDYES)
		{
			m_edit.GetWindowText(description);
			description.AnsiToOem();
			lines.SplitTabs(PS description,'\n');
			fp=fopen(gLastDisplayedTicFile,"wt");
			if (fp)
			{
				for (i=0;i<lines.GetCount();i++)
				{
				   line=lines.GetString(i);
                   trim_all(line);
				   p= (char *) strchr(line,'\r');
				   if (p)
					  *p=0;
				   if (strlen(line)>1)
					  fprintf(fp,"%s\n",line);
				}
				fclose(fp);
			}
			else
				err_out("E_FILERROR",gLastDisplayedTicFile);
		}
	}

	if (caseexit)
		return;

	sel=((CListBox *)GetDlgItem(IDC_LIST))->GetCurSel();
	ASSERT(sel!=LB_ERR);
	make_path(path,gEditTICPath,available_ticfiles.GetString(sel));
	gLastDisplayedTicFile=path;
	fp=fopen(path,"rt");
	str.Empty();
	if (fp)
	{
		while (fgets(buf,399,fp))
		{
			p=strchr(buf,'\n');
			if (p)	*p=0;
			p=strchr(buf,'\r');
			if (p)	*p=0;
			strcat(buf,"\r\n");
			str+=buf;
		}
		fclose(fp);
    }
	else
		err_out("E_FILERROR",path);
	
	str.OemToAnsi();
	m_edit.SetWindowText(str);
}





