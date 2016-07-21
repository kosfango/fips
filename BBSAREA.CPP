// bbsarea.cpp : implementation file
// IDD_BBS_AREA

#include "stdafx.h"
#include "resource.h"
#include "structs.h"
#include "bbsarea.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static CStrList bbs_areas;
static char DlgName[]="IDD_BBS_AREA";

// =======================================================================
	bbsarea::bbsarea(CWnd* pParent ): CSAPrefsSubDlg(bbsarea::IDD, pParent)
// =======================================================================
{
	//{{AFX_DATA_INIT(bbsarea)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void bbsarea::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(bbsarea)
	DDX_Control(pDX, IDC_LIST, m_list);
	DDX_Control(pDX, IDC_EDIT_PATH_FOR_FILES, m_edit_pathforfiles);
	DDX_Control(pDX, IDC_EDIT_GROUPS, m_edit_groups);
	DDX_Control(pDX, IDC_EDIT_FILESBBS, m_edit_filesbbs);
	DDX_Control(pDX, IDC_EDIT_DESCRIPTION, m_edit_description);
	DDX_Control(pDX, IDC_DD_LEVEL, m_dd_level);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(bbsarea, CDialog)
	//{{AFX_MSG_MAP(bbsarea)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_GET_PATH, OnGetPath)
	ON_BN_CLICKED(IDC_GET_FILELIST, OnGetFilelist)
	ON_BN_CLICKED(IDC_CHANGE, OnChange)
	ON_LBN_SELCHANGE(IDC_LIST, OnSelchangeList)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_IMPORT, OnImportTic)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// =====================================================================
LPCSTR bbsarea::GetName(void)	{return DlgName;}
// =====================================================================

// =======================================================================
	BOOL bbsarea::OnInitDialog()
// =======================================================================
{
CStrList	lst;
CString		help,help1;
int	 tabs[]={150,300,310,360};
int  lng[]={
			IDC_GET_PATH,
			IDC_GET_FILELIST,
			IDC_DELETE,
			IDC_ADD,
			IDC_CHANGE,
			IDC_STATIC2,
			IDC_STATIC1,
			IDC_STATIC4,
			IDC_STATIC5,
			IDC_STATIC6,
			IDC_STATIC3,
			IDC_IMPORT
			};
    CDialog::OnInitDialog();
    set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));
	TABULATE_LB(IDC_LIST);
	EXTENT_LB(IDC_LIST,1000);

	lst.LoadFromFile("bbsgrps.cfg");

	m_dd_level.ResetContent();
	for (int i=0;i<lst.GetCount();i++)
	{
		help=lst.GetString(i);
		get_token(help,0,help1);
		m_dd_level.AddString(help1);
	}

	bbs_areas.LoadFromFile("bbsfare.cfg");
	bbs_areas.Sort(1);

	UPDATE_LB(bbs_areas,IDC_LIST);
	SET_SELID(IDC_LIST,0);
	OnSelchangeList();
	return TRUE;
}

// =======================================================================
	int bbsarea::MakeString(CString &str)
// =======================================================================
{
CString path,desc,level,groups,filesbbs;
int		ret;

	m_edit_pathforfiles.GetWindowText(path);
	m_edit_description.GetWindowText(desc);

	ret=m_dd_level.GetCurSel();
	if (ret==LB_ERR)
	{
		MessageBeep(0);
		return 0;
	}
	m_dd_level.GetLBText(ret,level);
	m_edit_groups.GetWindowText(groups);
	m_edit_filesbbs.GetWindowText(filesbbs);
	if(filesbbs.GetLength()==0)
		filesbbs=path+"\\"+"files.bbs";

    trim_all(path);
    trim_all(desc);
    trim_all(groups);
    trim_all(filesbbs);
	str.Format("%s\t%s\t%s\t%s\t%s",path,desc,level,groups,filesbbs);
	return 1;
}

// =======================================================================
	void bbsarea::DispString(CString line)
// =======================================================================
{
CString path,desc,level,groups,filesbbs,ln;

	get_token(line,0,path);
	get_token(line,1,desc);
	get_token(line,2,level);
	get_token(line,3,groups);
	get_token(line,4,filesbbs);

	m_edit_pathforfiles.SetWindowText(path);
	m_edit_description.SetWindowText(desc);
	m_edit_groups.SetWindowText(groups);
	m_edit_filesbbs.SetWindowText(filesbbs);

	m_dd_level.SetCurSel(0);
	for (int i=0;i<m_dd_level.GetCount();i++)
	{
		m_dd_level.GetLBText(i,ln);
		if (ln==level)
		{
			m_dd_level.SetCurSel(i);
			break;
		}
	}
}

// =======================================================================
	void bbsarea::OnAdd()
// =======================================================================
{
CString str;

	if (!MakeString(str))	return;
	bbs_areas.AddTail(str);
	UPDATE_LB(bbs_areas,IDC_LIST);
}

// =======================================================================
	void bbsarea::OnGetPath()
// =======================================================================
{
CString path;

	if (GetDirectory(path,m_hWnd))
		m_edit_pathforfiles.SetWindowText(path);
}

// =======================================================================
	void bbsarea::OnGetFilelist()
// =======================================================================
{
CString str;
int		ret;

	str.LoadString(IDS_DESCRFILT);
	CFileDialog	dlg(TRUE,"files.bbs","files.bbs",OFN_HIDEREADONLY,str);
	ret=dlg.DoModal();
	restore_base_path();
	if (ret!=IDOK)	return;
	m_edit_filesbbs.SetWindowText(dlg.GetPathName());
}

// =======================================================================
	void bbsarea::OnChange()
// =======================================================================
{
int		sel;
CString str;

	GET_SELID(IDC_LIST);
	if (!MakeString(str))	return;
	bbs_areas.Replace(sel,str);
	UPDATE_LB(bbs_areas,IDC_LIST);
	SET_SELID(IDC_LIST,sel);
}

// =======================================================================
	void bbsarea::OnOK()
// =======================================================================
{
	bbs_areas.SaveToFile("bbsfare.cfg");
	if(m_bCloseOnOk)
		CDialog::OnOK();
}

// =======================================================================
	void bbsarea::OnSelchangeList()
// =======================================================================
{
int		sel;

	GET_SELID(IDC_LIST);
	DispString(bbs_areas.GetString(sel));
}

// =======================================================================
	void bbsarea::OnDelete()
// =======================================================================
{
int sel;

	GET_SELID(IDC_LIST);
	bbs_areas.Remove(sel);
	UPDATE_LB(bbs_areas,IDC_LIST);
	sel--;
	if (sel<0)	sel=0;
	SET_SELID(IDC_LIST,sel);
}

// =======================================================================
	void bbsarea::OnImportTic()
// =======================================================================
{
CStrList ticareas,rpath;
int		t,i,found;
char	tmp[300];
CString actual,mtmp;

		if (err_out("DN_IMPTICFREQ") != IDYES)	return;
		ticareas.LoadFromFile("tics.cfg");
		rpath.LoadFromFile("rpathes.cfg");

		// Get Tic-Areas
		for (t=0;t<ticareas.GetCount();t++)
		{
			actual = ticareas.GetString(t);
			get_token(actual,3,mtmp);
			strcpy(tmp,mtmp);
			addbackslash(tmp);

			found =0;
			for (i=0;i<bbs_areas.GetCount();i++)
			{
				char tmp2[200];
				CString stmp;

				stmp = bbs_areas.GetString(i);
				get_token(stmp,0,stmp);
				strcpy(tmp2,stmp);
				addbackslash(tmp2);

				if (strcmpi(tmp2,tmp) == 0)
				{
					found = 1 ;
					break;
				}
			}

			if (!found && mtmp.GetLength()>0)
			{
				CString path,desc,level,groups,filesbbs,str;

				path.Format("%s",tmp);
				filesbbs=path;
				actual = ticareas.GetString(t);
				get_token(actual,2,desc);

				str.Format("%s\t%s\t10\t%s\t%s",path,desc,groups,filesbbs);

				bbs_areas.AddTail(str);
				UPDATE_LB(bbs_areas,IDC_LIST);
			}
		}

		// Get Request-Pathes
		for (t=0;t<ticareas.GetCount();t++)
		{
			actual = rpath.GetString(t);
			get_token(actual,0,mtmp);
			strcpy(tmp,mtmp);
			addbackslash(tmp);

			found =0;
			for (i=0;i<bbs_areas.GetCount();i++)
			{
				char tmp2[200];
				CString stmp;

				strcpy(tmp2,bbs_areas.GetString(i));
				get_token(tmp2,0,stmp);
				strcpy(tmp2,stmp);
				addbackslash(tmp2);

				if (strcmpi(tmp2,tmp) == 0)
				{
					found = 1 ;
					break;
				}
			}

			if (!found && mtmp.GetLength()>0)
			{
				CString path,desc,level,groups,filesbbs,str;

				path.Format("%s",tmp);
				filesbbs=path;

				actual = ticareas.GetString(t);
				desc.Format("Only defined in REQUEST-PATH Setup");

				str.Format("%s\t%s\t10\t%s\t%s",path,desc,groups,filesbbs);

				bbs_areas.AddTail(str);
				UPDATE_LB(bbs_areas,IDC_LIST);
			}
		}
}

// =======================================================================
	void bbsarea::OnHelp()
// =======================================================================
{
	WinHelp(VHELP_BBS_AREA);
}

BOOL bbsarea::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return TRUE;
}
