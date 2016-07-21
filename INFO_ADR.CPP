// info_adr.cpp : implementation file
// IDD_INFO_ADDRESS

#include "stdafx.h"
#include "info_adr.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

int incomeexpand=0;	   // Ist es der AutoSearch on income ?
int is_grepsearch=0;   // Ist es eine GrepSuche ?

extern _gconfig gc;
static char DlgName[]="IDD_INFO_ADDRESS";

// ==========================================================================================
	info_adr::info_adr(CWnd *pParent) 	: CDialog(info_adr::IDD, pParent)
// ==========================================================================================
{
	//{{AFX_DATA_INIT(info_adr)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

// ==========================================================================================
	void info_adr::DoDataExchange(CDataExchange* pDX)
// ==========================================================================================
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(info_adr)
	DDX_Control(pDX, IDC_EDIT7, m_time);
	DDX_Control(pDX, IDC_EDIT6, m_info);
	DDX_Control(pDX, IDC_EDIT5, m_name);
	DDX_Control(pDX, IDC_EDIT1, m_fido);
	DDX_Control(pDX, IDC_EDIT_BBS, m_bbs);
	DDX_Control(pDX, IDC_EDIT_LOCATION, m_location);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(info_adr, CDialog)
	//{{AFX_MSG_MAP(info_adr)
	ON_BN_CLICKED(IDOK, OnSearch)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_BN_CLICKED(IDEXTENDED, OnExtendedSearch)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// ================================================================================
	BOOL info_adr::OnInitDialog()
// ================================================================================
{
int  lng[]={
			IDHELP,
			IDOK,
			IDCANCEL,
			IDC_STATIC1,
			IDC_STATIC2,
			IDC_STATIC3,
			IDC_STATIC5,
			IDEXTENDED,
			IDC_STATIC6
			};

	CDialog::OnInitDialog();
  set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));

	m_fido.SetWindowText(gc.AddressForInfo);

	if (gc.RequestFromMenu)
	{
		incomeexpand=0;
		OnSearch();
		return TRUE;
	}

	if (strlen(gc.AddressForInfo) >0)
	{
		incomeexpand=1;
		OnSearch();
	}
	return TRUE;
}

// ================================================================================
	void info_adr::OnSearch()
// ================================================================================
{
_fidonumber number;
CString lfido,full,info;
int		ret,errorcode,zone,net,node,point=0;
char	time1[5],time2[5];
char	*p;

	m_fido.GetWindowText(lfido);
  trim_all(lfido);
  if (!incomeexpand)
	{
	  ret=expand_address(lfido,full,info,1,1,0,errorcode,1,is_grepsearch);
	  if (ret)
		  m_fido.SetWindowText(full);
	  else
	  {
		  parse_address(full,&zone,&net,&node,&point);
		  point=0;
		  make_address(lfido,zone,net,node,point);
		  ret=expand_address(lfido,full,info,1,1,0,errorcode,1);
		  if (ret)
		  {
				SetWindowText(L("S_80"));
				m_fido.SetWindowText(full);
		  }
		  else
		  {
			  m_info.SetWindowText(NULL);
			  m_name.SetWindowText(NULL);
			  m_bbs.SetWindowText(NULL);
			  m_location.SetWindowText(NULL);
			  m_time.SetWindowText(NULL);
			  m_fido.SetSel(0,-1);
		      return;
		  }
	  }
	}

	incomeexpand=0;
	if (full.IsEmpty())	return;

	parse_address(full,&zone,&net,&node,&point);
	if (nl_get_fido_by_number(zone,net,node,point,&number))
	{
		p=strchr(number.flags,'\n');
		if (p) 
			*p=0;
		m_name.SetWindowText(number.user);
		if (!strcmp(number.flags,L("S_81")))	// addressbook
		{
			m_bbs.SetWindowText(number.flags);
			m_time.SetWindowText(L("S_344"));		// not defined
		}
		else
		{
			info.Format("%s (%s)",number.bbsname,number.phone);
			m_bbs.SetWindowText(info);
			m_location.SetWindowText(number.location);
			info.Format("%s",number.flags);
			m_info.SetWindowText(info);
			get_call_time(number.flags,time1,time2);
			if (*time1 && *time2)
				info.Format("%s-%s",time1,time2);
			else
				info=L("S_344");	// not defined
			m_time.SetWindowText(info);
		}
	}
}

// ================================================================================
	void info_adr::OnExtendedSearch()
// ================================================================================
{
	is_grepsearch=1;
	OnSearch();
	is_grepsearch=0;
}

// ================================================================================
	void info_adr::OnHelp()
// ================================================================================
{
	WinHelp(VHELP_INFO_ADDRESS);
}
	
BOOL info_adr::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return TRUE;
}
