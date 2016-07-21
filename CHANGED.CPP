// changed.cpp : implementation file
// IDD_CHANGE_DEST

#include "stdafx.h"
#include "cfido.h"
#include "changed.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern _gconfig gc;
static int DontEnd=0;
static char DlgName[]="IDD_CHANGE_DEST";

/////////////////////////////////////////////////////////////////////////////
// changed dialog

CString	fido;
CString	type;
CString	boss;
CString	telnr;
CString	pasword;
CString	aka;

changed::changed(CWnd* pParent /*=NULL*/)
	: CDialog(changed::IDD, pParent)
{
	//{{AFX_DATA_INIT(changed)
	m_password = _T("");
	m_phone = _T("");
	m_system = _T("");
	//}}AFX_DATA_INIT
}


void changed::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(changed)
	DDX_Control(pDX, IDC_CDPHONE, m_phonecontrol);
	DDX_Text(pDX, IDC_CDPASSWORD, m_password);
	DDV_MaxChars(pDX, m_password, 8);
	DDX_Text(pDX, IDC_CDPHONE, m_phone);
	DDV_MaxChars(pDX, m_phone, 100);
	DDX_Text(pDX, IDC_CDSYSTEM, m_system);
	DDV_MaxChars(pDX, m_system, 100);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(changed, CDialog)
	//{{AFX_MSG_MAP(changed)
	ON_BN_CLICKED(ID_OTHERLINES, OnOtherlines)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// changed message handlers

// ==========================================================
	BOOL changed::OnInitDialog()
// ==========================================================
{
int  lng[]={
			IDC_STATIC2,
			IDC_STATIC3,
			ID_OTHERLINES,
			IDC_STATIC1,
			IDOK,
			IDCANCEL
			};

	CDialog::OnInitDialog();
    set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));

	get_token(gc.changed,1,fido);
	get_token(gc.changed,2,type);
	get_token(gc.changed,3,boss);
	get_token(gc.changed,4,telnr);
	get_token(gc.changed,5,pasword);
	get_token(gc.changed,6,aka);

	m_password=pasword;
	m_phone="#"+telnr;
	m_system=fido+" "+boss;
	UpdateData(0);

	return TRUE;
}

// ==========================================================
	void changed::OnOK()
// ==========================================================
{
CFido xx,yy;
CString all;
char help[300],phone[300],passwd[300];

	UpdateData(1);

	if (m_phone[0]!='#')    // User has entered a fido address
	{
		xx.expand_incomplete(PS m_phone);
		strcpy(help,xx.asciibuf);
		yy=help;
		if (yy.GetPhoneAndPwd(phone,passwd))
		{
			m_phone.Format("#%s",phone);
			UpdateData(0);
		}

		else
		{
		   m_phonecontrol.SetFocus();
		   m_phonecontrol.SetSel(0,-1,0);
		   return;
		}
	}

	sscanf(m_system,"%s",help);
	m_system=help;

	all="\t"+m_system+"\t"+type+"\t"+boss+"\t";
	if (m_phone.GetLength()>0 && m_phone[0]=='#')
	{
	   strcpy(help,m_phone);
	   m_phone=&help[1];
	}
	
	all+=m_phone+"\t"+m_password+"\t"+aka+"\t";
	strcpy(gc.changed,all);

	if (DontEnd)
	   return;

	CDialog::OnOK();
}

// ==========================================================
	void changed::OnOtherlines()
// ==========================================================
{
_fidonumber nlquery;
CFido	fido;
CString addr,info,full;
int		zone,net,node,point=0;
int		err;

	get_token(gc.changed,1,addr);
	if (parse_address(addr,&zone,&net,&node,&point)<3)
		return;

    if (!nl_get_fido_by_number(zone,net,node,point,&nlquery))
		ERR_MSG_RET("W_NOFSFIM");

	if (expand_address(nlquery.user,full,info,1,1,0,err,0,0))
	{
		m_system=full;
		m_phone=full;
		fido=full;

		if (!nl_get_fido_by_number(fido.zone,fido.net,fido.node,fido.point,&nlquery))
			ERR_MSG_RET("W_NOFSFIM");

		boss.Format("%s",nlquery.bbsname);
		UpdateData(0);
		DontEnd=1;
		OnOK();
		DontEnd=0;
	}
}
