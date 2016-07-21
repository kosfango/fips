// receiver.cpp : implementation file
// IDD_RECIPIENT_LIST

#include "stdafx.h"
#include "receiver.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern CStrList Agroups;

static char DlgName[]="IDD_RECIPIENT_LIST";

// strings Format:
// GROUPNAME /t Number_of_Members /tJohann Weinzierl/t2:2494/17.1/tOliver Weindl/t2:2494/17.0/t
//

// ===========================================================================
	receiver::receiver(CWnd* pParent ) : CSAPrefsSubDlg(receiver::IDD, pParent)
// ===========================================================================
{
	//{{AFX_DATA_INIT(receiver)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

// ===========================================================================
	void receiver::DoDataExchange(CDataExchange* pDX)
// ===========================================================================
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(receiver)
	DDX_Control(pDX, IDC_MEMBERS, m_members);
	DDX_Control(pDX, IDC_GROUPS, m_groups);
	DDX_Control(pDX, IDC_EDIT_NAME, m_edit_name);
	DDX_Control(pDX, IDC_EDIT_GROUP, m_edit_group);
	DDX_Control(pDX, IDC_EDIT_FIDO, m_edit_fido);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(receiver, CDialog)
	//{{AFX_MSG_MAP(receiver)
	ON_BN_CLICKED(IDC_ADD2, OnAddMember)
	ON_BN_CLICKED(IDC_ADD1, OnAddGroup)
	ON_BN_CLICKED(IDC_DELETE1, OnDeleteGroup)
	ON_BN_CLICKED(IDC_DELETE2, OnDeleteMember)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_LBN_SELCHANGE(IDC_GROUPS, OnLbnSelchangeGroups)
	ON_BN_CLICKED(IDC_IMPORT, OnImport)
	//}}AFX_MSG_MAP
	ON_LBN_SELCHANGE(IDC_MEMBERS, OnLbnSelchangeMembers)
	ON_BN_CLICKED(IDC_CHANGE2, OnChangeMember)
	ON_BN_CLICKED(IDC_CHANGE1, OnChangeGroup)
END_MESSAGE_MAP()

// =====================================================================
LPCSTR receiver::GetName(void)	{return DlgName;}
// =====================================================================

// ===========================================================================
	BOOL receiver::OnInitDialog()
// ===========================================================================
{
CString str;
int tabs[]={60,170};
int tabs2[]={100};
int  lng[]={
			IDC_ADD1,
			IDC_ADD2,
			IDC_CHANGE1,
			IDC_CHANGE2,
			IDC_DELETE1,
			IDC_DELETE2,
			IDC_STATIC1,
			IDC_STATIC2,
			IDC_STATIC4,
			IDC_STATIC3,
			IDC_STATIC5,
			IDC_IMPORT,
			};

	CDialog::OnInitDialog();
    set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));
	TAB_LIST(m_groups,tabs2);
	TAB_LIST(m_members,tabs);
	for (int i=0;i<Agroups.GetCount();i++)
	{
		str=Agroups.GetString(i);
		str.Trim();
		m_groups.AddString(str);
	}
	SET_SEL(m_groups,0);
	OnLbnSelchangeGroups();
	return TRUE;
}

// ===========================================================================
	void receiver::OnOK()
// ===========================================================================
{
	Agroups.SaveToFile("receiver.cfg");
	if(m_bCloseOnOk)
		CDialog::OnOK();
}

// ===========================================================================
	void receiver::OnLbnSelchangeGroups()
// ===========================================================================
{
CString str,buf,name,fido;
int		sel;

	GET_SEL(m_groups);
	m_groups.GetText(sel,str);
	get_token(str,0,buf);
	m_edit_group.SetWindowText(buf);
	m_members.ResetContent();
	for (int i=1;i<=get_token_int(str,1);i++)
	{
		get_token(str,i*2,name);
		get_token(str,i*2+1,fido);
		buf=name+"\t"+fido;
		m_members.AddString(buf);
	}
	m_edit_name.SetWindowText("");
	m_edit_fido.SetWindowText("");
}

// ===========================================================================
	void receiver::OnLbnSelchangeMembers()
// ===========================================================================
{
CString str,buf;
int		sel;

	GET_SEL(m_members);
	m_members.GetText(sel,str);
	get_token(str,0,buf);
	m_edit_name.SetWindowText(buf);
	get_token(str,1,buf);
	m_edit_fido.SetWindowText(buf);
}

// ===========================================================================
	void receiver::OnAddMember()
// ===========================================================================
{
CString name,addr,str,buf;
int		sel,zone,node,net,point;

	GET_SEL(m_groups);
	m_edit_name.GetWindowText(name);
	if (name.IsEmpty())	return;
	m_edit_fido.GetWindowText(addr);
	if (parse_address(addr,&zone,&net,&node,&point) < 3)	ERR_MSG_RET("E_INVALDFIDOA");
	str=name+"\t"+addr;
	m_members.AddString(str);
	m_groups.GetText(sel,str);
	get_token(str,0,buf);
	refresh_groups(buf);
	SET_SEL(m_members,m_members.GetCount()-1);
}

// ===========================================================================
	void receiver::OnDeleteMember()
// ===========================================================================
{
CString str,buf;
int sel;

	GET_SEL(m_groups);
	m_groups.GetText(sel,str);
	get_token(str,0,buf);
	GET_SEL(m_members);
	m_members.DeleteString(sel);
	refresh_groups(buf);
	SET_SEL(m_members,m_members.GetCount()-1);
	OnLbnSelchangeMembers();
}

// ===========================================================================
	void receiver::OnChangeMember()
// ===========================================================================
{
CString str,buf,name,addr;
int		sel,zone,node,net,point;

	GET_SEL(m_groups);
	m_groups.GetText(sel,str);
	get_token(str,0,buf);
	GET_SEL(m_members);
	m_edit_name.GetWindowText(name);
	if (name.IsEmpty())	return;
	m_edit_fido.GetWindowText(addr);
	if (parse_address(addr,&zone,&net,&node,&point) < 3)	ERR_MSG_RET("E_INVALDFIDOA");
	str=name+"\t"+addr;
	m_members.DeleteString(sel);
	m_members.InsertString(sel,str);
	refresh_groups(buf);
	SET_SEL(m_members,sel);
	OnLbnSelchangeMembers();
}

// ===========================================================================
	void receiver::OnAddGroup()
// ===========================================================================
{
CString str;

	m_edit_group.GetWindowText(str);
	if (str.IsEmpty())	return;
	str+="\t0";
	m_groups.AddString(str);
	SET_SEL(m_groups,m_groups.GetCount()-1);
	OnLbnSelchangeGroups();
}

// ===========================================================================
	void receiver::OnDeleteGroup()
// ===========================================================================
{
int sel;

	GET_SEL(m_groups);
	m_groups.DeleteString(sel);
	SET_SEL(m_groups,m_groups.GetCount()-1);
	OnLbnSelchangeGroups();
}

// ===========================================================================
	void receiver::OnChangeGroup()
// ===========================================================================
{
CString str;
int sel;

	GET_SEL(m_groups);
	m_edit_group.GetWindowText(str);
	if (str.IsEmpty())	return;
	refresh_groups(str);
	SET_SEL(m_groups,sel);
	OnLbnSelchangeGroups();
}

// ===========================================================================
	void receiver::refresh_groups(LPCSTR group)
// ===========================================================================
{
CString str,buf;
int sel;

	GET_SEL(m_groups);
	str.Format("%s\t%d",group,m_members.GetCount());
	for (int i=0;i<m_members.GetCount();i++)
	{
		m_members.GetText(i,buf);
		str+="\t"+buf;
	}
	m_groups.DeleteString(sel);
	m_groups.InsertString(sel,str);
	SET_SEL(m_groups,sel);
}

// ===========================================================================
	void receiver::populate_lists()
// ===========================================================================
{
CString help,group;
int i;

	m_groups.ResetContent();
	m_members.ResetContent();
	for (i=0;i<Agroups.GetCount();i++)
	{
		help=Agroups.GetString(i);
		get_token(help,0,group);
		m_groups.AddString(group);
	}
}


// ===========================================================================
// ASCII file import in format:   xxxxxxxxxx ?:???/?.?
	void receiver::OnImport()
// ===========================================================================
{
FILE *fp;
CString path,str;
char buf[300],fido[20];
int  zone,net,node,point,ret;
char *p;

	str.LoadString(IDS_TEXTFILT);
	CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY,str);

	ret=dlg.DoModal();
	restore_base_path();
	if (ret!=IDOK)	return;
	BeginWaitCursor();
	path=dlg.GetPathName();
	fp=fopen(path,"rt");
	if (fp)
	{
		while (fgets(buf,199,fp))
		{
		   if (*buf==';')		   // comment
		      continue;

		   replace_tabs(buf);	   // remove TAB's
		   p=strchr(buf,':');
		   if (!p)
		      continue;
		   while (*p!=' ')
		         p--;
		   p++;
		   if (parse_address(p,&zone,&net,&node,&point)<3)
			  continue;

		   sscanf(p,"%s",fido);
		   *p=0;

		   m_edit_fido.SetWindowText(fido);
		   m_edit_name.SetWindowText(trim_all(buf));
		   OnAddMember();

		}
		fclose(fp);
	}
	else
		ERR_MSG2_RET("E_CANOPIM",path);

	EndWaitCursor();
}

// ===========================================================================
	void receiver::OnHelp()
// ===========================================================================
{
	WinHelp(VHELP_RECIPIENT_LIST);
}

//// ===========================================================================
//	void receiver::OnDblclkList()
//// ===========================================================================
//{
//	m_groups.GetText(m_groups.GetCurSel(),gDblSelGroup);
//	Agroups.SaveToFile("receiver.cfg");
//	CDialog::OnOK();
//}
