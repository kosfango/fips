// choosas.cpp : implementation file
// IDD_CHOOSE_AUTOSAVE

#include "stdafx.h"
#include "choosas.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern _gconfig gc;

static _DlgItemsSize DlgItemsSize [] =
{
	0,			      {0,0,0,0},{0,0,0,0},0,0,0,0,0,
	IDC_LIST		  ,{0,0,0,0},{0,0,0,0},0,0,HO|VE,HO|VE,0,
	IDC_EDIT1		  ,{0,0,0,0},{0,0,0,0},VE,VE,VE|HO,VE|HO,0,
	IDC_CREATE		  ,{0,0,0,0},{0,0,0,0},VE|HO,VE|HO,VE|HO,VE|HO,0,
	IDCANCEL		  ,{0,0,0,0},{0,0,0,0},VE|HO,VE|HO,VE|HO,VE|HO,0,
	IDOK			  ,{0,0,0,0},{0,0,0,0},VE|HO,VE|HO,VE|HO,VE|HO,0,
};
static char DlgName []="IDD_CHOOSE_AUTOSAVE";

// ===================================================================
	choosas::choosas(CWnd* pParent )  : CDialog(choosas::IDD, pParent)
// ===================================================================
{
	//{{AFX_DATA_INIT(choosas)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

// ===================================================================
	void choosas::DoDataExchange(CDataExchange* pDX)
// ===================================================================
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(choosas)
	DDX_Control(pDX, IDC_EDIT1, m_edit);
	DDX_Control(pDX, IDC_LIST, m_list);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(choosas, CDialog)
	//{{AFX_MSG_MAP(choosas)
	ON_LBN_DBLCLK(IDC_LIST, OnDblclkList)
	ON_BN_CLICKED(IDC_CREATE, OnCreate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// ===================================================================
	BOOL choosas::OnInitDialog()
// ===================================================================
{
areadef ad;
int		i=0;
int		lng[]={
				IDOK,
				IDCANCEL,
				IDC_STATIC1,
				IDC_CREATE
				};
    CDialog::OnInitDialog();
    set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));

	while(db_get_area_by_index(i++,&ad) == DB_OK)
	{
		if (!strcmp(ad.group,ASAVED_GROUP))
			m_list.AddString(ad.echotag);
	}
	if (m_list.GetCount()>0)	m_list.SetCurSel(0);
	gc.AutoSaveDestArea[0]=0;
	return TRUE;
}


// ===================================================================
	void choosas::OnCancel()
// ===================================================================
{
	gc.AutoSaveDestArea[0]=0;
	CDialog::OnCancel();
}

// ===================================================================
	void choosas::OnOK()
// ===================================================================
{
int sel;

	GET_SELID(IDC_LIST);
	m_list.GetText(sel,gc.AutoSaveDestArea);
	CDialog::OnOK();
}

// ===================================================================
	void choosas::OnDblclkList()
// ===================================================================
{
	CDialog::OnOK();
}

// ===================================================================
	void choosas::OnCreate()
// ===================================================================
{
CString str,str1;

	m_edit.GetWindowText(str);
    trim_all(str);
	if (str.GetLength()==0)	return;
	str.Replace(' ','_');
	str1=ASAVED_PREFIX+str;
	m_list.AddString(str1);
	m_list.SetCurSel(m_list.GetCount()-1);
}
