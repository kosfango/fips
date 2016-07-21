// valarea.cpp : implementation file
// IDD_VALUE_AREAFIX

#include "stdafx.h"
#include "lightdlg.h"
#include "valarea.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CStrList linkedareas;
extern CLightDlg *gpMain;
static char DlgName[]="IDD_VALUE_AREAFIX";

BOOL isAreaName(LPSTR text);

static _DlgItemsSize DlgItemsSize[] =
{
	0,			{0,0,0,0},{0,0,0,0},0,0,0,0,0,
	IDC_EDIT	,{0,0,0,0},{0,0,0,0},VE,VE,HO|VE,HO|VE,0,
	IDHELP		,{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDCANCEL	,{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDOK		,{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_STATIC2 ,{0,0,0,0},{0,0,0,0},0,0,HO|VE,HO|VE,0,
	IDC_STATIC3 ,{0,0,0,0},{0,0,0,0},VE,VE,HO|VE,HO|VE,0,
	IDC_LIST	,{0,0,0,0},{0,0,0,0},0,0,HO|VE,HO|VE,0,
};

// =============================================================
	valarea::valarea(CWnd* pParent ) : CDialog(valarea::IDD, pParent)
// =============================================================
{
	//{{AFX_DATA_INIT(valarea)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

// =============================================================
	void valarea::DoDataExchange(CDataExchange* pDX)
// =============================================================
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(valarea)
	DDX_Control(pDX, IDC_LIST, m_list);
	DDX_Control(pDX, IDC_UPLINK, m_uplink);
	DDX_Control(pDX, IDC_EDIT, m_bigedit);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(valarea, CDialog)
	//{{AFX_MSG_MAP(valarea)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_LBN_DBLCLK(IDC_LIST, OnDblclkList)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_HELPINFO()
	ON_CBN_SELENDOK(IDC_UPLINK, OnSelendokUplink)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// =============================================================
	BOOL valarea::OnInitDialog()
// =============================================================
{
int tabs[]={120};
int lng[]={
			IDOK,
			IDHELP,
			IDCANCEL,
			IDC_STATIC1,
			IDC_STATIC2,
			IDC_STATIC3,
			};

  CDialog::OnInitDialog();
  set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));
  StoreOrgItemSizes(DlgName,this,DlgItemsSize,sizeof(DlgItemsSize)/sizeof(_DlgItemsSize));
	TABULATE_LB(IDC_LIST);
	ExtractSelectedEchoes();
	FillDropDownList();
	FillLinkedAreasList();
	return TRUE;
}

// =============================================================
	void valarea::OnCancel()
// =============================================================
{
	linkedareas.RemoveAll();
  StoreInitDialogSize(DlgName,this);
	CDialog::OnCancel();
}

// =============================================================
	void valarea::OnOK()
// =============================================================
{
	if (!build_afix_netmail())	return;
	linkedareas.RemoveAll();
  StoreInitDialogSize(DlgName,this);
	CDialog::OnOK();
}

// =============================================================
	void valarea::OnDblclkList()
// =============================================================
{
CString area,str,str1;
int		sel;

	 GET_SELID(IDC_LIST);
	 get_token(linkedareas.GetString(sel),0,area);
	 if (!area.GetLength())	return;
	 m_bigedit.GetWindowText(str);
	 str1.Format("-%s\r\n%s",area,str);
	 m_bigedit.SetWindowText(str1);
}

// =============================================================
	void valarea::OnSelendokUplink() 
// =============================================================
{
	FillLinkedAreasList();
}

// =============================================================
// fills listbox with linked echoes
	void valarea::FillLinkedAreasList()
// =============================================================
{
CString addr;
areadef ad;
char	line[300];
int		count=0;

	linkedareas.RemoveAll();
	m_uplink.GetWindowText(addr);
	db_get_count_of_areas(&count);
	for (int i=2;i<count;i++)
	{
		memset(&ad,0,sizeof(ad));
		db_get_area_by_index(i,&ad);
// not CarbonCopy and Autosaved and other uplink
		if (ad.localmail || !strcmp(ad.group,ASAVED_GROUP) || addr.Compare(ad.uplink))	continue;
		strcpy(line,ad.echotag);
		strcat(line,"\t");
		strcat(line,ad.description);
		linkedareas.AddTail(line);
	}
	UPDATE_LB(linkedareas,IDC_LIST);
}

// =============================================================
	BOOL valarea::PreTranslateMessage(MSG* pMsg) 
// =============================================================
{
	if (pMsg->message==WM_RBUTTONDOWN)
	{
		linkedareas.Sort(0);
		UPDATE_LB(linkedareas,IDC_LIST);
		return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}

// =============================================================
	void valarea::FillDropDownList()
// =============================================================
{
CString	addr,aka;
char	bossinfo[MAX_BOSSLEN];
int		i=0,ind=0;

	make_address(aka,gustat.act_mailh.srczone,gustat.act_mailh.srcnet,
				gustat.act_mailh.srcnode,0);
	m_uplink.ResetContent();
	while (db_get_boss_by_index(i++,bossinfo))
	{
		get_token(bossinfo,BOSSADDR,addr);
		if (addr.GetLength()>0)
		{
			m_uplink.AddString(addr);
			if (aka==addr)	ind=i-1;
		}
	}

	if (m_uplink.GetCount()==0)	return;
	m_uplink.SetCurSel(ind);
}

// =============================================================
// builds arealist from selected lines
	void valarea::ExtractSelectedEchoes()
// =============================================================
{
char	buf[MAX_MSG],mail[MAX_MSG],str[100];
char	*p;
int		i,sel;

	*mail=0;
	for (i=0;i<gpMain->m_mailview.GetSelCount();i++)
	{
		if ((sel=gpMain->m_mailview.GetSel(i))==LB_ERR)	return;
		if (sel==0)			continue;
		strcpy(buf,(LPSTR)gpMain->m_mailview.GetItemDataPtr(i));
		if (strlen(buf)<=1)	continue;

		strupr(buf);
		p=buf;
		while (*p)
		{
			if (isalnum(*p) || strchr(AREACHARS,*p))
			{
				*str=0;
				sscanf(p,"%s",&str);
				if (isAreaName(str))
				{
					strcat(mail,str);
					strcat(mail,"\r\n");
					break;
				}
			}
			p++;
		}
	}
	m_bigedit.SetWindowText(mail);
	m_bigedit.SetSel(-1,0,0);
}

// ============================================
	void valarea::OnSize(UINT nType, int cx, int cy)
// ============================================
{
	CDialog::OnSize(nType, cx, cy);
	resize_wnd(this,DlgItemsSize,sizeof(DlgItemsSize));
}

// ============================================
	void valarea::OnPaint()
// ============================================
{
	CPaintDC dc(this);
	DrawRightBottomKnubble(this,dc);
}

// ===========================================================================
	int valarea::build_afix_netmail()
// ===========================================================================
{
mailheader mh;
CStrList	lst;
CString		INTL,FMPT,MSGID,POINT,PID,mailout,bosscfg,order,addplus,str;
char			buf[MAX_BOSSLEN],tmp[100];
int				i,sel,ret;

	m_bigedit.GetWindowText(order);
	sel=m_uplink.GetCurSel();	
	if (order.GetLength()<2 || sel==LB_ERR)  
		return 0;

	BeginWaitCursor();
	memset(&mh,0,sizeof(mailheader));

	ret=db_get_boss_by_index(sel,buf);
	ASSERT(ret==DB_OK);
	bosscfg=buf;

	get_token(bosscfg,AFIXPASW,str); 
	strcpy(mh.subject,str);
	get_token(bosscfg,AFIXPLUS,addplus);
	get_token(bosscfg,MAINAKA,POINT);
	build_fido_time(buf);	
	strcpy(mh.datetime,buf);
	get_token(bosscfg,AREAFIX,str); 
	strcpy(mh.toname,str);
	get_fullname(str);
	str.AnsiToOem();
	strcpy(mh.fromname,str);
	mh.structlen=sizeof(mailheader);
	mh.status=DB_MAIL_CREATED;
	mh.recipttime=mh.mailid=time(NULL);
	get_token(bosscfg,MAINAKA,str);
	sscanf(str,"%hu:%hu/%hu.%hu",&mh.srczone,&mh.srcnet,&mh.srcnode,&mh.srcpoint);
	get_token(bosscfg,BOSSADDR,str);
	sscanf(str,"%hu:%hu/%hu.%hu",&mh.dstzone,&mh.dstnet,&mh.dstnode,&mh.dstpoint);

	INTL.Format	("\001INTL %d:%d/%d %d:%d/%d\r",
				mh.dstzone,mh.dstnet,mh.dstnode,mh.srczone,mh.srcnet,mh.srcnode);
	FMPT.Format	("\001FMPT %d\r",mh.srcpoint);
	MSGID.Format("\001MSGID: %s %x\r",POINT,mh.mailid);
    PID.Format	("\001PID: %s\r",get_versioninfo(tmp,0));

	// RemoveAll Mailtext
	mailout=INTL+FMPT+MSGID+PID+"\r\r";
	lst.BuildFromMemoryString(PS order);
	for (i=0;i<lst.GetCount();i++)
	{
		str=lst.GetString(i);
        trim_all(str);
		if (str[0]!='-' && str[0]!='%' && addplus=="1")
			mailout+="+";

		mailout+=str+"\r";
	}
	mailout+="\r\r"+get_tearline(str,TRUE)+"\r";
	expand_tabs(mailout);
	mh.text_length=mailout.GetLength()+1;
	mh.mail_text=PS mailout;

	if (db_append_new_message(0,&mh) != DB_OK)
	{
	   EndWaitCursor();
	   ERR_MSG_RET0("E_APPMFAILED");
	}

	if (db_set_area_state(0,1) != DB_OK)
	{
		EndWaitCursor();
		ERR_MSG_RET0("E_MODFLAGSET");
	}
	EndWaitCursor();
	err_out("M_ECHOSEND");

	if (gustat.cur_area_handle==0)
		gpMain->OnUpdateScreenDisplay();

	return 1;
}

// =============================================================
	void valarea::OnHelp()
// =============================================================
{
	WinHelp(VHELP_VALUE_AREAFIX);
}

// =============================================================
	BOOL valarea::OnHelpInfo(HELPINFO* pHelpInfo) 
// =============================================================
{
	OnHelp();
	return TRUE;
}

// ===============================================================================
	BOOL isAreaName(char *text)
// ===============================================================================
{
char *p;

	if (*text==0)	return FALSE;
	p=text;
	while (*p)
	{
		if (!isalnum(*p) && !strchr(AREACHARS,*p))
			return FALSE;
		p++;
	}
	return TRUE;
}
