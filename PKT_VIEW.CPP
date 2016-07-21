// pkt_view.cpp : implementation file
// IDD_OUTBOUND_MAIL_VIEW

#include "stdafx.h"
#include <io.h>
#include "cfido.h"
#include "lightdlg.h"
#include "pkt_view.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

static CStrList pkt_list;
static CStrList mails_in_pkt_list;
extern CString SelectedPktOutdir;
static CString CopyOfSelectedPktOutdir;

CString		pktpath;
CStrList	LastPktContents;
int			analyse_pkt(char *fname,CStrList &out);

static _DlgItemsSize DlgItemsSize [] =
{
	0,						   {0,0,0,0},{0,0,0,0},0,0,0,0,0,
	IDC_LIST_CONTENTS		  ,{0,0,0,0},{0,0,0,0},0,0,HO,HO,0,
	IDC_EDIT				  ,{0,0,0,0},{0,0,0,0},0,0,HO|VE,HO|VE,0,
	IDHELP			  ,{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_DELETE_THIS_PACKET	  ,{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_DELETE				  ,{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDCANCEL				  ,{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,

};
static char DlgName[]="IDD_OUTBOUND_MAIL_VIEW";

// =============================================================
	pkt_view::pkt_view(CWnd* pParent ): CDialog(pkt_view::IDD, pParent)
// =============================================================
{
	//{{AFX_DATA_INIT(pkt_view)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

// =============================================================
	void pkt_view::DoDataExchange(CDataExchange* pDX)
// =============================================================
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(pkt_view)
	DDX_Control(pDX, IDC_EDIT, m_edit);
	DDX_Control(pDX, IDC_LIST_PKT, m_list_pkt);
	DDX_Control(pDX, IDC_LIST_CONTENTS, m_list_contents);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(pkt_view, CDialog)
	//{{AFX_MSG_MAP(pkt_view)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_LBN_SELCHANGE(IDC_LIST_PKT, OnSelchangeListPkt)
	ON_LBN_SELCHANGE(IDC_LIST_CONTENTS, OnSelchangeListContents)
	ON_BN_CLICKED(IDC_DELETE_THIS_PACKET, OnDeleteThisPacket)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// =============================================================
	BOOL pkt_view::OnInitDialog()
// =============================================================
{
char arcmail[MAX_PATH];
int  tabs[]={70,160};
int  lng[]={
			IDCANCEL,
			IDHELP,
			IDC_DELETE,
			IDC_STATIC1,
			IDC_STATIC2,
			IDC_STATIC6,
			IDC_DELETE_THIS_PACKET
			};

	CDialog::OnInitDialog();
    set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));
    StoreOrgItemSizes(DlgName,this,DlgItemsSize,sizeof(DlgItemsSize)/(sizeof(_DlgItemsSize)));

	TABULATE_LB(IDC_LIST_CONTENTS);
	EXTENT_LB(IDC_LIST_CONTENTS,1000);

	if (get_arcmail_name(SelectedPktOutdir,arcmail) &&
		extract_file(arcmail,PS SelectedPktOutdir)==1)
			unlink(arcmail);

	SetForegroundWindow();
	LastPktContents.RemoveAll();
	CopyOfSelectedPktOutdir=SelectedPktOutdir;
	DoTheWholeDisplayStuff();
	return TRUE;
}

// =============================================================
	void pkt_view::OnDelete()
// =============================================================
{
CString line,str;
int		ret,sel,mailstart,mailtextstart,maillength,filelen;
char    *p=0;
FILE    *fp;

	GET_SELID(IDC_LIST_CONTENTS);

	if (err_out("DN_DYRWDTM")!=IDYES)
	   return;

	if (mails_in_pkt_list.GetCount()==1)
	{
		unlink(pktpath);
		DoTheWholeDisplayStuff();
		return;
	}

	line=LastPktContents.GetString(sel);
	get_token(line,5,str); 	// 4:MAILANFANG
	sscanf(str,"%d",&mailstart);
	get_token(line,4,str);	// 5:MAILTEXT
	sscanf(str,"%d",&mailtextstart);
	get_token(line,6,str);  	// 6:LAENGE Der Ganzen Mail
	sscanf(str,"%d",&maillength);

	fp=fopen(pktpath,"rb");
	if (fp)
	{
		filelen=_filelength(_fileno(fp));
		p=(char *)malloc(filelen+100);
		if (p)
		{
			ret=fread(p,filelen,1,fp);
			if (ret!=1)
			{
			    err_out("E_EORPF",pktpath);
				goto endit;
			}
		}
		else
		{
		    err_out("E_MEM_OUT");
			goto endit;
		}
		fclose(fp);
	}

	unlink(pktpath);
	if (access(pktpath,0)==0)
	{
		err_out("E_CANMODOPF",pktpath);
		goto endit;
	}

	fp=fopen(pktpath,"wb");
	if (!fp)
	{
		err_out("E_WOWTPF",pktpath);
		goto endit;
	}
	fwrite(p,mailstart,1,fp);
	fwrite(p+(mailstart+maillength),filelen-(mailstart+maillength),1,fp);

	fclose(fp);

endit:
	if (p)
	    free(p);

	OnSelchangeListPkt();
}

// =============================================================
	void pkt_view::OnCancel()
// =============================================================
{
	LastPktContents.RemoveAll();
	Repacking();
    StoreInitDialogSize(DlgName,this);
	CDialog::OnCancel();
}

// =============================================================
	void pkt_view::OnSelchangeListPkt()
// =============================================================
{
int		i;
int		sel;
CString line;
CString newall;
CString str;

	mails_in_pkt_list.RemoveAll();

	GET_SELID(IDC_LIST_PKT);

	line=pkt_list.GetString(sel);
	LastPktContents.RemoveAll();
	pktpath=CopyOfSelectedPktOutdir+"\\"+line;
	if (!analyse_pkt(PS pktpath,LastPktContents))
		ERR_MSG2_RET("E_ANOPKT",pktpath);
	// Die LastPktContents Liste hat jetzt folgendes Format:
    // toname /t fromanem /t subject /t datetime /t offset auf mailtext /t offset auf mailanfang /t laenge der ganzen mail
	// Also pfrimeln wir das um in das Format welches wir brauchen

	for (i=0;i<LastPktContents.GetCount();i++)
	{
	    line=LastPktContents.GetString(i);
		get_token(line,1,str);  
		newall=str+"\t";	// 0:TO
		get_token(line,0,str);  
		newall+=str+"\t";	// 1:FROM
		get_token(line,2,str);  
		newall+=str+"\t";	// 2:SUBJECT
		get_token(line,3,str);  
		newall+=str+"\t";	// 3:DATETIME
		get_token(line,5,str);  
		newall+=str+"\t";	// 4:MAILHEADER
		get_token(line,4,str);  
		newall+=str+"\t";	// 5:MAILTEXT
		get_token(line,6,str);  
		newall+=str+"\t";	// 6:LENGTH of mail
		newall.OemToAnsi();
		mails_in_pkt_list.AddTail(PS newall);
	}

	UPDATE_LB(mails_in_pkt_list,IDC_LIST_CONTENTS);
	m_list_contents.SetCurSel(0);
	OnSelchangeListContents();
}

// =============================================================
	void pkt_view::OnSelchangeListContents()
// =============================================================
// So, und hierher kommen wir wenn der Benutzer eine Mail in einem PKT angeklickt hat.
// Also holen wir uns den selktierten String raus. ...
{
CString line;
CString str;
FILE	*fp;
int		sel;
char	src[MAX_MSG],dst[MAX_MSG];
int		startpos,length;
char	*p1,*p2;

	GET_SELID(IDC_LIST_CONTENTS);
	line=mails_in_pkt_list.GetString(sel);
	get_token(line,5,str);
	sscanf(str,"%d",&startpos);
	get_token(line,6,str);
	sscanf(str,"%d",&length);

	memset(src,0,sizeof(src));
	memset(dst,0,sizeof(dst));

	fp=fopen(pktpath,"rb");
	if (fp)
	{
		fseek(fp,startpos,SEEK_SET);
		fread(src,length,1,fp);
		fclose(fp);
		src[length]=0;
	}
	else
		ERR_MSG2_RET("E_FTOPKT",pktpath);

	p1=src;
	p2=dst;

	while (*p1)
	{
		 if (*p1==1)
		 {
			 *p2++=' ';
			 p1++;
			 continue;
		 }
		 if (*p1==13)
		 {
			 *p2++='\r';
			 *p2++='\n';
			 p1++;
		 }
		 else
		 {
			 *p2=*p1;
			 p1++;
			 p2++;
		 }
	}
	*p2=0;
	OemToChar(dst,dst);
	m_edit.SetWindowText(dst);
}

// =============================================================
	void pkt_view::DoTheWholeDisplayStuff(void)
// =============================================================
{
CStrList lst;

	m_list_pkt.ResetContent();
	m_list_contents.ResetContent();
	m_edit.SetWindowText(NULL);

	pkt_list.RemoveAll();
	mails_in_pkt_list.RemoveAll();

	SelectedPktOutdir=CopyOfSelectedPktOutdir;

	SelectedPktOutdir +="\\*.pkt";
	pkt_list.FillWithFiles(SelectedPktOutdir);

	UPDATE_LB(pkt_list,IDC_LIST_PKT);
	m_list_pkt.SetCurSel(0);
	OnSelchangeListPkt();
	m_list_contents.SetFocus();
}

// =========================================================
	void pkt_view::OnDeleteThisPacket()
// =========================================================
// deleting entire PKT
{
int sel;

	GET_SELID(IDC_LIST_PKT);
	if (err_out("DN_DYRWDTPKT")!=IDYES)
	   return;

   	SafeFileDelete(pktpath);
	DoTheWholeDisplayStuff();
}

// =========================================================
	int pkt_view::Repacking()
// =========================================================
// repacking outbound after viewing pkts
{
CString addr;
CString entry;
CString cspack;
char	buf[MAX_BOSSLEN];
char	dirname[MAX_PATH],zipname[MAX_PATH],files2pack[MAX_PATH];
char	*p;
int		i;

	if(!get_cfg(CFG_COMMON,"PackOutbound",1) ||
		SelectedPktOutdir.Find("\\NETMAIL\\")>=0)	// if need not pack outbound or it is netmail
		return 0;

	buf[0]=zipname[0]=files2pack[0]=0;
	get_path(SelectedPktOutdir,dirname);
	get_arcmail_name(dirname,zipname);
	get_path(zipname,files2pack);
	strcat(files2pack,"\\*.pkt");
// get address in dir format
	strcpy(buf,dirname);
	p=strrchr(buf,'\\');
	if (!p)
		return 0;
	p++;
	CFido fid;
	fid=p;
	addr=fid.Get(0);

	i=0;
	while (db_get_boss_by_index(i,buf))
	{
		get_token(buf,BOSSADDR,entry);
		if (entry==addr)
		{
			get_token(buf,ARCHIVER,cspack);
			pack_file(zipname,files2pack,PS cspack);
			return 1;
		}
		i++;
	}
	return 0;
}

// ============================================
	void pkt_view::OnSize(UINT nType, int cx, int cy)
// ============================================
{
	CDialog::OnSize(nType, cx, cy);
	resize_wnd(this,DlgItemsSize,sizeof(DlgItemsSize));
}

// ===============================================
	void pkt_view::OnPaint()
// ===============================================
{
	CPaintDC dc(this);
	DrawRightBottomKnubble(this,dc);
}

// =============================================================
	void pkt_view::OnHelp()
// =============================================================
{
	WinHelp(VHELP_OUTBOUND_MAIL_VIEW);
}

BOOL pkt_view::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return TRUE;
}
