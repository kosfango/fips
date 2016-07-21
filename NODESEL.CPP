// nodesel.cpp : implementation file
// IDD_SELECT_FROM_NODELIST

#include "stdafx.h"
#include "nodesel.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

static CStrList  displist;

static _DlgItemsSize DlgItemsSize [] =
{
	0,			 {0,0,0,0},{0,0,0,0},0,0,0,0,0,
	IDC_LIST	 ,{0,0,0,0},{0,0,0,0},0,0,HO|VE,HO|VE,0,
	IDC_STATIC1	 ,{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDCANCEL	 ,{0,0,0,0},{0,0,0,0},HO|VE,HO|VE,HO|VE,HO|VE,0,
	IDOK		 ,{0,0,0,0},{0,0,0,0},HO|VE,HO|VE,HO|VE,HO|VE,0,
};
static char DlgName []="IDD_SELECT_FROM_NODELIST";

nodesel::nodesel(CWnd* pParent ) : CDialog(nodesel::IDD, pParent)
{
	//{{AFX_DATA_INIT(nodesel)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

// =====================================================================
	void nodesel::DoDataExchange(CDataExchange* pDX)
// =====================================================================
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(nodesel)
	DDX_Control(pDX, IDC_LIST, m_list);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(nodesel, CDialog)
	//{{AFX_MSG_MAP(nodesel)
	ON_LBN_DBLCLK(IDC_LIST, OnDblclkList)
	ON_WM_SIZE()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// =====================================================================
	BOOL nodesel::OnInitDialog()
// =====================================================================
{
_fidonumber *result;
_fidonumber number;
CString flags,bbsname,location,phone;
char	buf[300];
char	*p;
int		ret;
int tabs[]={85,140,250,350,430};
int lng[]={
			IDOK,
			IDCANCEL,
			IDC_STATIC1
			};

    CDialog::OnInitDialog();
    set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));
    StoreOrgItemSizes(DlgName,this,DlgItemsSize,sizeof(DlgItemsSize)/sizeof(_DlgItemsSize));

	TABULATE_LB(IDC_LIST);
	EXTENT_LB(IDC_LIST,1000);

	result=gustat.findresult;
	BeginWaitCursor();
	displist.RemoveAll();
	for (int i=0;i<gustat.findcount;i++)
	{
	   ret=nl_get_fido_by_number(result->zone,result->net,result->node,result->point,&number);
	   if (ret)
	   {
		   p=strchr(number.flags,'\n');		if (p) *p=0; flags.Format	("%s",number.flags);
		   p=strchr(number.bbsname,'\n');	if (p) *p=0; bbsname.Format	("%s",number.bbsname);
		   p=strchr(number.location,'\n');	if (p) *p=0; location.Format("%s",number.location);
		   p=strchr(number.phone,'\n');		if (p) *p=0; phone.Format	("%s",number.phone);

	   }
	   sprintf(buf,"%s\t%d:%d/%d.%d\t%s\t%s\t%s\t%s",
		   result->user,result->zone,result->net,result->node,result->point,flags,bbsname,location,phone);
	   displist.AddTail(buf);
	   result++;
	}
	UPDATE_LB(displist,IDC_LIST);
	EndWaitCursor();
	return TRUE;
}

// =====================================================================
	void nodesel::OnOK()
// =====================================================================
{
	if (m_list.GetCurSel() != LB_ERR)	extract_selected();
	displist.RemoveAll();
    StoreInitDialogSize(DlgName,this);
	CDialog::OnOK();
}

// =====================================================================
	void nodesel::OnCancel()
// =====================================================================
{
	gustat.selected.zone=0xFFFF;
	CDialog::OnCancel();
}

// =====================================================================
	void nodesel::OnDblclkList()
// =====================================================================
{
	OnOK();
}

// =====================================================================
	void nodesel::extract_selected()
// =====================================================================
{
CString str,name,rest;
int sel;

	 sel=m_list.GetCurSel();
	 if (sel==LB_ERR)	OnCancel();
	 m_list.GetText(sel,str);

	 get_token(str,0,name);
	 get_token(str,1,rest);

	 strcpy(gustat.selected.user,name);
	 sscanf(rest,"%hu:%hu/%hu.%hu",
		 &gustat.selected.zone,&gustat.selected.net,&gustat.selected.node,&gustat.selected.point);
}

// ============================================
	BOOL nodesel::PreTranslateMessage(MSG* pMsg)
// ============================================
{
	if (pMsg->message==WM_RBUTTONDOWN && HandleRightButton(pMsg))	return TRUE;
	return CDialog::PreTranslateMessage(pMsg);
}

// ============================================
	void nodesel::OnSize(UINT nType, int cx, int cy)
// ============================================
{
	CDialog::OnSize(nType, cx, cy);
	resize_wnd(this,DlgItemsSize,sizeof(DlgItemsSize));
}

// ============================================
	void nodesel::OnPaint()
// ============================================
{
	CPaintDC dc(this);
	DrawRightBottomKnubble(this,dc);
}

// ============================================
	BOOL nodesel::OnCommand(WPARAM wParam, LPARAM lParam) 
// ============================================
{
int	 sort=-1;

	if (lParam==0)
	{
		switch (wParam)
		{
			case IDC_NAME:
				sort=0;
				break;
			case IDC_ADDRESS:
				sort=1;
				break;
			case IDC_FLAGS:
				sort=2;
				break;
			case IDC_BBSNAME:
				sort=3;
				break;
			case IDC_TOWN:
				sort=4;
				break;
			case IDC_PHONE:
				sort=5;
				break;
		}
		if (sort>=0)
		{
			BeginWaitCursor();
			displist.Sort(sort);
			UPDATE_LB(displist,IDC_LIST);
			EndWaitCursor();
			return TRUE;
		}
	}
	return CDialog::OnCommand(wParam, lParam);
}
