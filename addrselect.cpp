// addrselect.cpp : implementation file
// IDD_ADDRSELECT

#include "stdafx.h"
#include "LIGHT.h"
#include "addrselect.h"

extern CRITICAL_SECTION NodeListAcces;
extern CStrList Abook;
extern CStrList Agroups;
extern _gconfig gc;

static char DlgName[]="IDD_ADDRSELECT";

IMPLEMENT_DYNAMIC(addrselect, CDialog)
addrselect::addrselect(CWnd* pParent /*=NULL*/) : CDialog(addrselect::IDD, pParent){
	*name=0;
	*address=0;
	*comment=0;
	isgroup=FALSE;
	selected=FALSE;
	m_Timecount=0;
	*m_pattern=0;
	m_nChar=0;
	memset(m_Buffer,0,sizeof(m_Buffer));
}

void addrselect::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ADDR, m_addr);
	DDX_Control(pDX, IDC_GRP, m_grp);
	DDX_Control(pDX, IDC_ADDNLDATA, m_addnldata);
}

BEGIN_MESSAGE_MAP(addrselect, CDialog)
	ON_LBN_DBLCLK(IDC_ADDR, OnLbnDblclkAddr)
	ON_LBN_DBLCLK(IDC_GRP, OnLbnDblclkGrp)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_LBN_SELCHANGE(IDC_ADDR, OnLbnSelchangeAddr)
	ON_LBN_SELCHANGE(IDC_GRP, OnLbnSelchangeGrp)
	ON_BN_CLICKED(IDC_ADDNLDATA, OnBnClickedAddNL)
	ON_MESSAGE(UDM_DOSEARCH,OnStartListSearch)
	ON_BN_CLICKED(IDC_SEARCH, OnBnClickedSearch)
	ON_WM_TIMER()
	ON_WM_CLOSE()
END_MESSAGE_MAP()

// addrselect message handlers
// ===========================================================
	BOOL addrselect::OnInitDialog()
// ===========================================================
{
int lng[]={
		IDC_STATIC2,
		IDC_STATIC3,
		IDHELP,
		IDOK,
		IDCANCEL,
		IDC_ADDNLDATA,
		IDC_SEARCH,
		IDC_FILTLIST,
		IDC_STATIC1
		};

	CDialog::OnInitDialog();
	set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));
	((CButton*)GetDlgItem(IDC_ADDNLDATA))->SetCheck(get_cfg(CFG_COMMON,"AddNLData",0));
	((CButton*)GetDlgItem(IDC_SEARCH))->SetCheck(get_cfg(CFG_COMMON,"AddrSearch",0));
	m_addr.SetTabStops(110);
	m_grp.SetTabStops(100);
	for (int i=0;i<Agroups.GetCount();i++)
		m_grp.AddString(Agroups.GetString(i));
	fill_list();
	m_nTimer = SetTimer(777,10,NULL);
	return TRUE;
}

// ===========================================================
	void addrselect::OnLbnDblclkAddr()
// ===========================================================
{
CString str;
int sel;

	GET_SEL(m_addr);
	m_addr.GetText(sel,str);
	get_token(str,0,name);
	get_token(str,1,address);
	get_token(str,2,comment);
	isgroup=FALSE;
	selected=TRUE;
	CDialog::OnOK();
}

// ===========================================================
	void addrselect::OnLbnDblclkGrp()
// ===========================================================
{
CString str;
int sel;

	GET_SEL(m_grp);
	m_grp.GetText(sel,str);
	get_token(str,0,name);
	get_token(str,1,address);
	get_token(str,2,comment);
	isgroup=TRUE;
	selected=TRUE;
	CDialog::OnOK();
}

void addrselect::OnBnClickedOk()
{
	if (m_addr.GetCurSel() >= 0)	OnLbnDblclkAddr();
	if (m_grp.GetCurSel() >= 0)		OnLbnDblclkGrp();
}

void addrselect::OnLbnSelchangeAddr()
{
	m_grp.SetCurSel(-1);
}

void addrselect::OnLbnSelchangeGrp()
{
	m_addr.SetCurSel(-1);
}

void addrselect::OnBnClickedAddNL()
{
	fill_list();
}

// ============================================
	BOOL addrselect::PreTranslateMessage(MSG *pMsg)
// ============================================
{
	if (pMsg->message==WM_KEYDOWN)
	{
		if (pMsg->wParam>='A' && pMsg->wParam<='Z')
		{
			m_Buffer[m_nChar++]=pMsg->wParam;
			m_Timecount=0;
		}
		else if (pMsg->wParam=='\b')
		{
			memset(m_Buffer,0,sizeof(m_Buffer));
			m_nChar=0;
			m_Buffer[m_nChar]='\b';
			m_Timecount=100;
		}
		else
			m_Timecount=100;
		return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}

// ============================================
	void addrselect::OnTimer(UINT nIDEvent)
// ============================================
{
	if (++m_Timecount >= 50)
	{
		if (*m_Buffer > 0)
		{
			if (*m_Buffer=='\b')
				*m_pattern=0;	// backspace as pattern
			else
				memcpy(m_pattern,m_Buffer,m_nChar+1);
			GetDlgItem(IDC_PATTERN)->SetWindowText(m_pattern);
			m_nChar=0;
			memset(m_Buffer,0,sizeof(m_Buffer));
			SendMessage(UDM_DOSEARCH);
		}
		m_Timecount=0;
	}
	CDialog::OnTimer(nIDEvent);
}

// ============================================
	void addrselect::OnClose()
// ============================================
{
	KillTimer(m_nTimer);
	CDialog::OnClose();
}

// =================================================================
	LRESULT addrselect::OnStartListSearch(WPARAM w,LPARAM l)
// =================================================================
{
int sel;

	if (((CButton*)GetDlgItem(IDC_SEARCH))->GetCheck())
	{
		sel=m_addr.FindString(-1,m_pattern);
		if (sel != LB_ERR)	m_addr.SetCurSel(sel);
	}
	else
		fill_list();
	return 0;
}

// =================================================================
	void addrselect::fill_list(void)
// =================================================================
{
FILE *fp;
char fname[MAX_PATH],ptrn[100],str[1000];
_fu_rec	ru;

	strcpy(ptrn,"*");
	strcat(ptrn,m_pattern);
	strcat(ptrn,"*");
	m_addr.ResetContent();
	BeginWaitCursor();
// add addressbook data
	for (int i=0;i<Abook.GetCount();i++)
	{
		strcpy(str,Abook.GetString(i));
		if (pmatch(ptrn,strupr(str)))	m_addr.AddString(Abook.GetString(i));
	}
// add nodelist data
	if (((CButton*)GetDlgItem(IDC_ADDNLDATA))->GetCheck())
	{
		EnterCriticalSection(&NodeListAcces);
		make_path(fname,gc.NodelistPath,nludxname);
		fp=fopen(fname,"rb");
		if (!fp)
		{
			LeaveCriticalSection(&NodeListAcces);
			EndWaitCursor();
			return;
		}
		while (!feof(fp))
		{
			fread(&ru,sizeof(_fu_rec),1,fp);
			strcpy(str,ru.name);
			if (pmatch(ptrn,strupr(str)))
			{
				sprintf(str,"%s\t%d:%d/%d.%d",ru.name,ru.zone,ru.net,ru.node,ru.point);
				m_addr.AddString(str);
			}
		}
		fclose(fp);
		LeaveCriticalSection(&NodeListAcces);
	}
	EndWaitCursor();
}

void addrselect::OnBnClickedSearch()
{
	fill_list();
}

void addrselect::SetPattern(LPCSTR ptrn)
{
	strcpy(m_pattern,ptrn);
}
