// simppoll.cpp : implementation file
// IDD_MANUAL_POLL

#include "stdafx.h"
#include <io.h>
#include "cfg_boss.h"
#include "cfido.h"
#include "detmail.h"
#include <direct.h>
#include "simppoll.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern _gconfig gc;
extern detmail  gMailer;
static char DlgName[]="IDD_MANUAL_POLL";

// ======================================================
	simppoll::simppoll(CWnd* pParent ) : CDialog(simppoll::IDD, pParent)
// ======================================================
{
	//{{AFX_DATA_INIT(simppoll)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

// ======================================================
	void simppoll::DoDataExchange(CDataExchange* pDX)
// ======================================================
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(simppoll)
	DDX_Control(pDX, IDC_EDIT1, m_edit);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(simppoll, CDialog)
	//{{AFX_MSG_MAP(simppoll)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// ======================================================
	BOOL simppoll::OnInitDialog()
// ======================================================
{
int  lng[]={
			IDOK,
			IDCANCEL,
			IDC_STATIC1,
			IDC_STATIC2,
			};
    CDialog::OnInitDialog();
    set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));

	return TRUE;
}

// ======================================================
	void simppoll::OnOK()
// ======================================================
{
CString str,full,info;
char	tmp[MAX_PATH],path[MAX_PATH];
char	eofchr[]={0,0};
time_t	oldsecs;
int		errorcode,zone,net,node,point;

OLDPKTHDR   po;   // groesse jeweils 58 bytes
NEWPKTHDR   *pn;
FILE		*fp;

	m_edit.GetWindowText(str);
    trim_all(str);

	int ret=expand_address(str,full,info,1,1,0,errorcode);
	if (!ret)	
		ERR_MSG_RET("W_NOFSFIM");

	CFido xx;
	xx=PS full;

	make_path(tmp,gc.OutboundPath,xx.GetAsDir(0));

	if (!access(tmp,0))
		ERR_MSG2_RET("M_PSAE",full);

	time(&oldsecs);
	str.Format("%08X",oldsecs);

	_mkdir(tmp);
	make_path(path,tmp,str);

	// now fill pkt-structure

	get_first_aka(&zone,&net,&node,&point);
	pn = (NEWPKTHDR *)&po;
	memset(pn,0,sizeof(NEWPKTHDR));

	pn->product	= 0;
	pn->rev_lev	= 0;
	pn->subver	= 2;
	pn->version	= 2;
	pn->ozone 	= zone;	  // main aka
	pn->onet 	= net;
	pn->onode 	= node;
	pn->opoint 	= point;
  	pn->dzone 	= xx.zone; // remote aka
	pn->dnet 	= xx.net;
	pn->dnode 	= xx.node;
	pn->dpoint 	= xx.point;

	fp=fopen(path,"wb");
	if (!fp)	
		ERR_MSG2_RET("E_CANTCREATEDUMMY",path);

	fwrite(pn,sizeof(NEWPKTHDR),1,fp);
	fwrite(eofchr,2,1,fp);
	fclose(fp);

	gMailer.ShowWindow(SW_SHOWNORMAL);
	gMailer.GraphicalStatusDisplay(1);

	CDialog::OnOK();
}
