// badxfer.cpp : implementation file
//

#include "stdafx.h"
#include "badxfer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern _gconfig gc;

/////////////////////////////////////////////////////////////////////////////
// badxfer dialog


badxfer::badxfer(CWnd* pParent /*=NULL*/)
	: CDialog(badxfer::IDD, pParent)
{
	//{{AFX_DATA_INIT(badxfer)
	m_BadName = _T("");
	m_RealName = _T("");
	m_Combo = -1;
	m_FileTime = _T("");
	m_FileSize = _T("");
	//}}AFX_DATA_INIT
}


void badxfer::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(badxfer)
	DDX_Control(pDX, IDC_LIST_BAD_XFER, m_Spis);
	DDX_Text(pDX, IDC_EDIT_BADNAME, m_BadName);
	DDX_Text(pDX, IDC_EDIT_REALNAME, m_RealName);
	DDX_CBIndex(pDX, IDC_COMBO1, m_Combo);
	DDX_Text(pDX, IDC_EDIT_FILE_TIME, m_FileTime);
	DDX_Text(pDX, IDC_EDIT_FILESIZE, m_FileSize);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(badxfer, CDialog)
	//{{AFX_MSG_MAP(badxfer)
	ON_LBN_SELCHANGE(IDC_LIST_BAD_XFER, OnSelchangeListBadXfer)
	ON_BN_CLICKED(IDSETONLY, OnSetonly)
	ON_BN_CLICKED(IDDELETE, OnDelete)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// badxfer message handlers

BOOL badxfer::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
char xfer_log[300];
char buffer[1000];
char buffer2[1000];
char bad_real[300];
char bad_temp[300];
long bad_fsize;
long bad_ftime;
int skip_file;
FILE *txfd;

	make_path(xfer_log,gc.BasePath,"BAD-XFER.LOG");
	if ((txfd=fopen(xfer_log,"rt")))
	{
	while (fgets(buffer,1000,txfd)) 
	{
	sscanf(buffer,"%s %s %ld %lo %ld",bad_real,bad_temp,&bad_fsize,&bad_ftime,&skip_file);
	sprintf(buffer2,"%s\t%s\t%ld\t%lo\t%ld",bad_real,bad_temp,bad_fsize,bad_ftime,skip_file);
 	m_Spis.AddString(buffer2);
	};
	fclose(txfd);
	};
	m_Spis.SetCurSel(0);	
	OnSelchangeListBadXfer();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void badxfer::OnSelchangeListBadXfer() 
{
	// TODO: Add your control notification handler code here
int		sel;
CString realname;
CString badname;
CString filesize;
CString filetime;
CString skipfile;
CString all;
	GET_SELID(IDC_LIST_BAD_XFER);
	UpdateData(TRUE);

	m_Spis.GetText(sel,all);
	get_token(all,0,realname);
	get_token(all,1,badname);
	get_token(all,2,filesize);
	get_token(all,3,filetime);
	get_token(all,4,skipfile);

    m_RealName=realname;
    m_BadName=badname;
	m_FileSize=filesize;
    m_FileTime=filetime;
	m_Combo=atoi(skipfile);
	UpdateData(FALSE);

}

void badxfer::SetBadXfer()
{

}

void badxfer::OnSetonly() 
{
	// TODO: Add your control notification handler code here
	
int		sel;
CString all;




	GET_SELID(IDC_LIST_BAD_XFER);
	UpdateData(TRUE);

    all.Format("%s\t%s\t%s\t%s\t%ld",m_RealName,m_BadName,m_FileSize,m_FileTime,m_Combo);
	m_Spis.DeleteString(sel);
	m_Spis.InsertString(sel,all);
	
	UpdateData(FALSE);
	m_Spis.SetFocus();
	xferreplace();
	m_Spis.SetCurSel(sel);	

}


void badxfer::OnDelete() 
{
	// TODO: Add your control notification handler code here
int		sel;
char bad_real[300];
char bad_temp[300];
long bad_fsize;
long bad_ftime;


	GET_SELID(IDC_LIST_BAD_XFER);
	UpdateData(TRUE);
	sscanf(m_FileTime,"%lo",&bad_ftime);
	sscanf(m_FileSize,"%ld",&bad_fsize);
	strcpy(bad_real,m_RealName);
	strcpy(bad_temp,m_BadName);

	xferdel(bad_real,bad_temp,bad_fsize,bad_ftime);
	m_Spis.DeleteString(sel);
	UpdateData(FALSE);
	m_Spis.SetFocus();


	
}


void badxfer::xferdel(char *xfer_real,char *xfer_temp,long xfer_fsize,long xfer_ftime)
{
char new_log[300];
char xfer_log[300];
char linebuf[255];
char bad_real[300];
char bad_temp[300];
long bad_fsize;
long bad_ftime;
FILE *fp, *new_fp;
int left;
char xfer_file[300];
	
	make_path(xfer_log,gc.BasePath,"BAD-XFER.LOG");
	if ((fp = fopen(xfer_log, "rt"))) 
	{
		make_path(new_log,gc.BasePath,"BAD-XFER.$$$");
		if ((new_fp = fopen(new_log, "wt"))) 
		{
			left = false;
			while (fgets(linebuf,255,fp)) 
			{
				sscanf(linebuf,"%s %s %ld %lo",bad_real,bad_temp,&bad_fsize,&bad_ftime);
				if (strcmp(xfer_real,bad_real) || strcmp(xfer_temp,bad_temp) || 
					xfer_fsize != bad_fsize || xfer_ftime != bad_ftime) 
				{
					fputs(linebuf,new_fp);
					left = true;
				}
				else
				{
				make_path(xfer_file,gc.InboundPath,"TEMP");
				make_path(xfer_file,xfer_file,bad_temp);
				DeleteFile(xfer_file);
				}
			}
			fclose(fp);
			fclose(new_fp);
			unlink(xfer_log);
			if (left) 
				rename(new_log,xfer_log);
			else      
				unlink(new_log);
		}
		else
			fclose(fp);
	};



}


void badxfer::xferreplace()
{

char new_log[300];
char xfer_log[300];
FILE *new_fp;
int		sel;
char buffer[1000];

char bad_real[300];
char bad_temp[300];
long bad_fsize;
long bad_ftime;
int skip_file;

	
	
		make_path(new_log,gc.BasePath,"BAD-XFER.$$$");
		make_path(xfer_log,gc.BasePath,"BAD-XFER.LOG");

		if ((new_fp = fopen(new_log, "wt"))) 
		{
			
			for (sel=0;sel<m_Spis.GetCount();sel++) 
			{
			m_Spis.GetText(sel,buffer);
			sscanf(buffer,"%s %s %ld %lo %ld",bad_real,bad_temp,&bad_fsize,&bad_ftime,&skip_file);
			sprintf(buffer,"%s %s %ld %lo %ld\n",bad_real,bad_temp,bad_fsize,bad_ftime,skip_file);
			fputs(buffer,new_fp);
			};
			
			fclose(new_fp);
			unlink(xfer_log);
			rename(new_log,xfer_log);
		}

	


}
