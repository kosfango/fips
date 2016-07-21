// areasel.cpp : implementation file
// IDD_AREASELECT

#include "stdafx.h"
#include "lightdlg.h"
#include "traffic.h"
#include "areasel.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern CLightDlg	*gpMain;
extern CStrList		UserdefAreaList;
extern _gconfig gc;
CStrList AreaShortList;

static _DlgItemsSize DlgItemsSize [] =
{
	0,			     {0,0,0,0},{0,0,0,0},0,0,0,0,0,
	IDC_LIST,				{0,0,0,0},{0,0,0,0},0,0,HO|VE,HO|VE,0,
	IDC_LISTU,				{0,0,0,0},{0,0,0,0},0,0,HO|VE,HO|VE,0,
	IDC_DISPLAY_UDEF,		{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_STATIC45,			{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_EDIT_COMMENT,		{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_ADD_COMMENT,		{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_DELETE_COMMENT,		{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_UP,					{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_DOWN,				{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_REST,				{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_SAVE_UDEF,			{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDHELP,					{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_BUTTON1,			{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDCANCEL,				{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDOK,					{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
};
static char DlgName[]="IDD_AREASELECT";

/////////////////////////////////////////////////////////////////////////////
// CAreasel dialog

CAreasel::CAreasel(CWnd* pParent /*=NULL*/)
	: CDialog(CAreasel::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAreasel)
	//}}AFX_DATA_INIT
}

void CAreasel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAreasel)
	DDX_Control(pDX, IDC_LISTU, m_listudef);
	DDX_Control(pDX, IDC_LIST, m_listarea);
	DDX_Control(pDX, IDC_DISPLAY_UDEF, m_check_udefdisplay);
	DDX_Control(pDX, IDC_EDIT_COMMENT, m_edit_comment);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAreasel, CDialog)
	//{{AFX_MSG_MAP(CAreasel)
	ON_BN_CLICKED(IDC_BUTTON1, OnAreaTraffic)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_BN_CLICKED(IDC_ADD_COMMENT, OnAddComment)
	ON_BN_CLICKED(IDC_DELETE_COMMENT, OnDeleteComment)
	ON_BN_CLICKED(IDC_DISPLAY_UDEF, OnDisplayUdef)
	ON_BN_CLICKED(IDC_DOWN, OnDown)
	ON_BN_CLICKED(IDC_UP, OnUp)
	ON_BN_CLICKED(IDC_REST, OnRest)
	ON_BN_CLICKED(IDC_SAVE_UDEF, OnSaveUdef)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_NOTIFY(NM_DBLCLK, IDC_LIST, OnDblclkList)
	ON_NOTIFY(NM_DBLCLK, IDC_LISTU, OnDblclkListu)
	ON_NOTIFY(NM_CLICK, IDC_LIST, OnClickList)
	ON_NOTIFY(NM_CLICK, IDC_LISTU, OnClickListu)
	ON_WM_HELPINFO()
	ON_WM_NCACTIVATE()
	ON_NOTIFY(NM_RCLICK, IDC_LIST, OnRclickList)
	ON_NOTIFY(NM_RCLICK, IDC_LISTU, OnRclickListu)
//	ON_NOTIFY(NM_SETFOCUS, IDC_LIST, OnSetfocusList)
//	ON_NOTIFY(NM_SETFOCUS, IDC_LISTU, OnSetfocusListu)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAreasel message handlers

// ========================================================
	BOOL CAreasel::OnInitDialog()
// ========================================================
{
RECT	re;
int		i,l,t,r,b,cnt;
long	style;
areadef ad;
char	buf[500];
int		notread;
_listcol lc[]={	250,"",100,"",100,"",250,""};
int		lng[]={
				IDC_BUTTON1,
				IDC_DISPLAY_UDEF,
				IDC_UP,
				IDC_DOWN,
				IDC_STATIC45,
				IDC_ADD_COMMENT,
				IDC_DELETE_COMMENT,
				IDC_REST,
				IDC_SAVE_UDEF,
				IDHELP,
				IDCANCEL,
				IDOK
				};

	CDialog::OnInitDialog();
	set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));
	strcpy(lc[0].hdr,L("S_92"));	// area
	strcpy(lc[1].hdr,L("S_431"));	// total
	strcpy(lc[2].hdr,L("S_452"));	// unread
	strcpy(lc[3].hdr,L("S_93"));	// description
	gc.selected_area=-1;
	gc.AreaSelectionMode=1;
	if (gc.asel_as_list)	// quick select
	{
		StoreOrgItemSizes(DlgName,this,DlgItemsSize,sizeof(DlgItemsSize)/sizeof(_DlgItemsSize));
		style=GetWindowLong(m_hWnd,GWL_STYLE) & ~(WS_CAPTION | WS_THICKFRAME | WS_SYSMENU);
		SetWindowLong(m_hWnd,GWL_STYLE,style);
		(gpMain->m_nextarea).GetWindowRect(&re);
		l=re.right;
		t=re.bottom;
		gpMain->GetWindowRect(&re);
		r=re.right-GetSystemMetrics(SM_CXVSCROLL)-GetSystemMetrics(SM_CXSIZEFRAME)-6;
		m_listarea.GetWindowRect(&re);
		b=re.bottom-re.top;
		GetWindowRect(&re);
		re.left=l;
		re.right=r;
		re.top=t;
		re.bottom=t+b;
		MoveWindow(&re);
// resize lists
		m_listarea.GetWindowRect(&re);
		ScreenToClient(&re);
		re.bottom=re.top+b-3;
		m_listarea.MoveWindow(&re);
		m_listudef.GetWindowRect(&re);
		ScreenToClient(&re);
		re.bottom=re.top+b-2;
		m_listudef.MoveWindow(&re);
		lc[3].width=re.right-re.left-480;
// hide unused elements
		cnt=sizeof(DlgItemsSize)/sizeof(_DlgItemsSize);
		for (i=3;i<cnt;i++)
			GetDlgItem(DlgItemsSize[i].id)->ShowWindow(SW_HIDE);
	}
	else
		StoreOrgItemSizes(DlgName,this,DlgItemsSize,sizeof(DlgItemsSize)/sizeof(_DlgItemsSize));
	
	MakeColumns(m_listarea,lc,sizeof(lc)/sizeof(_listcol),DlgName,1);
	MakeColumns(m_listudef,lc,sizeof(lc)/sizeof(_listcol),DlgName,2);
	AreaShortList.RemoveAll();
	m_listarea.DeleteAllItems();
	i=0;
	while (db_get_area_by_index(i++,&ad) == DB_OK)
	{
		notread=ad.number_of_mails-ad.number_of_read;
		if (notread<0)	notread=0;

		if (!strcmp(ad.echotag,BBSMAIL))
			sprintf(buf,"%s\t%d\t---\t%s",ad.echotag,ad.number_of_mails,L("S_101"));	// BBS local area
		else
			sprintf(buf,"%s\t%d\t%d\t%s",ad.echotag,ad.number_of_mails,notread,ad.description);
		
		AddRow(m_listarea,buf);
		AreaShortList.AddTail(buf);
	}
	SelectRow(m_listarea,gustat.cur_area_handle);
	gc.DisplayUserdefArea=get_cfg(CFG_COMMON,"ShortAreaList",0);
	switch_lists(gc.DisplayUserdefArea);
	return TRUE;
}

// ========================================================
	void CAreasel::OnCancel()
// ========================================================
{
	UserdefAreaList.LoadFromFile("udefdef.cfg");
	if (gc.UpdateOnCancel==1)	gc.UpdateOnCancel=2;
	if (!gc.asel_as_list)		StoreInitDialogSize(DlgName,this);
	gc.AreaSelectionMode=0;
	CDialog::OnCancel();
}

// ========================================================
	void CAreasel::OnClickList(NMHDR* pNMHDR, LRESULT* pResult) 
// ========================================================
{
CRect re;
POINT pt;

    if (gc.asel_as_list)
    {
        if (gc.DisplayUserdefArea)
            m_listudef.GetWindowRect(&re);
        else
            m_listarea.GetWindowRect(&re);
        GetCursorPos(&pt);
        if (re.PtInRect(pt))
            OnOK();
        else
            OnCancel();
        *pResult = 1;
    }
    else
        *pResult = 0;
}

// ========================================================
	void CAreasel::OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult) 
// ========================================================
{
	OnOK();	
	*pResult = 1;
}

// ========================================================
	void CAreasel::OnClickListu(NMHDR* pNMHDR, LRESULT* pResult) 
// ========================================================
{
CRect re;
POINT pt;
int   sel;

    if (gc.asel_as_list)
    {
        if (gc.DisplayUserdefArea)
            m_listudef.GetWindowRect(&re);
        else
            m_listarea.GetWindowRect(&re);
        GetCursorPos(&pt);
        *pResult = 1;
        if (re.PtInRect(pt))
        {
            sel=GetSelected();
            if (sel < 0)	return;
            SelectRow(m_listarea,sel);
            OnOK();
        }
        else
            OnCancel();
    }
    else
        *pResult = 0;
}
	
// ========================================================
	void CAreasel::OnDblclkListu(NMHDR* pNMHDR, LRESULT* pResult) 
// ========================================================
{
int	sel;

	*pResult = 1;
	sel=GetSelected();
	if (sel<0)
		return;
	SelectRow(m_listarea,sel);
	OnOK();	
}

// ========================================================
	BOOL CAreasel::OnNcActivate(BOOL bActive) 
// ========================================================
{
	if (!bActive && gc.asel_as_list)	PostMessage(WM_CLOSE);
	return CWnd::OnNcActivate(bActive);
}

// ========================================================
	void CAreasel::OnRclickList(NMHDR* pNMHDR, LRESULT* pResult) 
// ========================================================
{
	OnCancel();
//	*pResult = 0;
}

// ========================================================
	void CAreasel::OnRclickListu(NMHDR* pNMHDR, LRESULT* pResult) 
// ========================================================
{
	OnCancel();
//	*pResult = 0;
}

// ========================================================
	void CAreasel::OnOK()
// ========================================================
{
int sel;

	if (gc.DisplayUserdefArea)
		sel=GetSelected();
	else
		sel=GetSelectedItem(m_listarea); 

	if (sel<0)	return;
	gc.selected_area=sel;

	if (gc.mode==MODE_QUOTE)
		gustat.dest_area_handle=sel;
	else
		gustat.cur_area_handle=sel;

	set_cfg(CFG_COMMON,"ShortAreaList",gc.DisplayUserdefArea);
	gc.UpdateOnCancel=0;
	if (!gc.asel_as_list)	StoreInitDialogSize(DlgName,this);
	gc.AreaSelectionMode=0;
	SaveColumnsWidth(m_listarea,DlgName,1);
	SaveColumnsWidth(m_listudef,DlgName,2);
	CDialog::OnOK();
}

// ========================================================
	void CAreasel::OnAreaTraffic()
// ========================================================
{
	traffic dlg;
	dlg.DoModal();
}

// ========================================================
	void CAreasel::OnAddComment()
// ========================================================
{
CString str;
int		sel;

	sel=GetSelectedItem(m_listudef);
	if (sel<0)	sel=0;
	m_edit_comment.GetWindowText(str);
	str=";"+str;
	UserdefAreaList.Insert(sel,str);
	UserdefAreaList.UpdateListBox(m_listudef);
	SelectRow(m_listudef,sel+1);
}

// ========================================================
	void CAreasel::OnDeleteComment()
// ========================================================
{
int	sel;

	GET_SELECT(m_listudef);
	UserdefAreaList.Remove(sel);
	UserdefAreaList.UpdateListBox(m_listudef);
	SelectRow(m_listudef,sel);
}

// ========================================================
	void CAreasel::OnDisplayUdef()
// ========================================================
{
	gc.DisplayUserdefArea=m_check_udefdisplay.GetCheck();
	switch_lists(gc.DisplayUserdefArea);
}

// ========================================================
	void CAreasel::OnDown()
// ========================================================
{
CString str;
int		sel;

	sel=GetSelectedItem(m_listudef);
	if (sel<0 || sel>=(UserdefAreaList.GetCount()-1))	return;
	str=UserdefAreaList.GetString(sel);
	UserdefAreaList.Insert(sel+1,str);
	UserdefAreaList.Remove(sel);
	UserdefAreaList.UpdateListBox(m_listudef);
	SelectRow(m_listudef,sel+1);
	m_listudef.EnsureVisible(sel+1,0);
}

// ========================================================
	void CAreasel::OnUp()
// ========================================================
{
CString str;
int		sel;

	sel=GetSelectedItem(m_listudef);
	if (sel<=0)	return;
	str=UserdefAreaList.GetString(sel);
	UserdefAreaList.InsertB(sel-1,str);
	UserdefAreaList.Remove(sel+1);
	UserdefAreaList.UpdateListBox(m_listudef);
	SelectRow(m_listudef,sel-1);
	m_listudef.EnsureVisible(sel-1,0);
}

// ========================================================
	void CAreasel::OnRest()
// ========================================================
{
CString str;
int		appcount=0;
char	usrarea[300],orgarea[300];

	for (int i=0;i<m_listarea.GetItemCount();i++)
	{
		m_listarea.GetItemText(i,0,orgarea,300);
		for (int n=0;n<UserdefAreaList.GetCount();n++)
		{
			str=UserdefAreaList.GetString(n);
			if (*str==';')	continue;
			*usrarea=0;
			get_first_value(str,usrarea);
			if (!strcmp(usrarea,orgarea))
				goto cont;
		}

		if (!appcount)
			UserdefAreaList.AddTail(L("S_25"));

		appcount++;
		GetRow(m_listarea,i,str);
		UserdefAreaList.AddTail(str);
cont:;
	}
	if (appcount>0)
	{
		err_out("M_ADDANLA",appcount);
		UserdefAreaList.UpdateListBox(m_listudef);
		SelectRow(m_listudef,UserdefAreaList.GetCount()-(appcount+1));
		m_listudef.EnsureVisible(UserdefAreaList.GetCount()-(appcount+1),0);
	}
}

// ========================================================
	void CAreasel::OnSaveUdef()
// ========================================================
{
	UserdefAreaList.SaveToFile("udefdef.cfg");
	show_msg(L("S_606"));	// saved
}

// ============================================
	void CAreasel::OnSize(UINT nType, int cx, int cy)
// ============================================
{
	CDialog::OnSize(nType, cx, cy);
	resize_wnd(this,DlgItemsSize,sizeof(DlgItemsSize));
}

// ============================================
	void CAreasel::OnPaint()
// ============================================
{
	CPaintDC dc(this);
	DrawRightBottomKnubble(this,dc);
}

// ========================================================
	int CAreasel::GetSelected()
// ========================================================
{
CString str;
int		sel;
char	area[100];
	
	sel=GetSelectedItem(m_listudef);
	if (sel < 0)		return -1;
	str=UserdefAreaList.GetString(sel);
	if (*str == ';')	return -1;
	get_first_value(str,area);
	return db_get_area_by_name(area);
}

// ========================================================
	BOOL CAreasel::PreTranslateMessage(MSG* pMsg) 
// ========================================================
{
	if (pMsg->message==WM_KEYDOWN && pMsg->wParam==VK_TAB && gc.asel_as_list)	return 1;
	if (pMsg->message==WM_KEYDOWN && pMsg->wParam==VK_SPACE)
	{
		if (gc.DisplayUserdefArea)
			next_unread(m_listudef);
		else
			next_unread(m_listarea);
		return 1;
	}
	return CDialog::PreTranslateMessage(pMsg);
}

// ========================================================
	void CAreasel::OnHelp()
// ========================================================
{
	WinHelp(VHELP_AREASELECT);
}

// ========================================================
	BOOL CAreasel::OnHelpInfo(HELPINFO* pHelpInfo) 
// ========================================================
{
	OnHelp();
	return TRUE;
}

// ========================================================
	void CAreasel::switch_lists(int userdef)
// ========================================================
{
CStrList	list;
CString		str;
char		uarea[300],udef[300],orgarea[300];
int			i,n,mustsave=0;

	if (userdef)
	{
		check_udef_existence();
 		ENABLEID(IDC_SAVE_UDEF);
 		ENABLEID(IDC_REST);
 		ENABLEID(IDC_STATIC45);
 		ENABLEID(IDC_EDIT_COMMENT);
 		ENABLEID(IDC_ADD_COMMENT);
 		ENABLEID(IDC_DELETE_COMMENT);
 		ENABLEID(IDC_UP);
 		ENABLEID(IDC_DOWN);
		m_listarea.ShowWindow(SW_HIDE);
		m_listudef.ShowWindow(SW_SHOWNORMAL);
		m_check_udefdisplay.SetCheck(1);

		list.RemoveAll();
		for (i=0;i<UserdefAreaList.GetCount();i++)
		{
			str=UserdefAreaList.GetString(i);
			if (*str==';')
			{
				list.AddTail(str);
				continue;
			}
			get_first_value(str,uarea);

			for (n=0;n<AreaShortList.GetCount();n++)
			{
				str=AreaShortList.GetString(n);
				get_first_value(str,orgarea);
				if (!strcmp(orgarea,uarea))
				{
					list.AddTail(str);
					goto cont;
				}
			}
			mustsave=1;
cont:;
		}

		UserdefAreaList.RemoveAll();
		for (i=0;i<list.GetCount();i++)
		{
			str=list.GetString(i);
			UserdefAreaList.AddTail(str);
		}
		if (mustsave)
			UserdefAreaList.SaveToFile("udefdef.cfg");
		
		UserdefAreaList.UpdateListBox(m_listudef);
		m_listarea.GetItemText(gustat.cur_area_handle,0,orgarea,300);
		for (i=0;i<UserdefAreaList.GetCount();i++)
		{
			get_first_value(UserdefAreaList.GetString(i),udef);
			if (!strcmp(orgarea,udef))
			{
				SelectRow(m_listudef,i);
				break;
			}
		}
		m_listudef.SetFocus();
	}
	else
	{
 		DISABLEID(IDC_SAVE_UDEF);
 		DISABLEID(IDC_REST);
 		DISABLEID(IDC_STATIC45);
 		DISABLEID(IDC_EDIT_COMMENT);
 		DISABLEID(IDC_ADD_COMMENT);
 		DISABLEID(IDC_DELETE_COMMENT);
 		DISABLEID(IDC_UP);
		DISABLEID(IDC_DOWN);
		m_check_udefdisplay.SetCheck(0);
		m_listarea.ShowWindow(SW_SHOWNORMAL);
		m_listudef.ShowWindow(SW_HIDE);
		m_listarea.SetFocus();
	}
}

void CAreasel::next_unread(CListCtrl &lst)
{
CString str;
int sel;

	GET_SELECT(lst);
	for (int i=sel+1;i<lst.GetItemCount();i++)
	{
		GetRow(lst,i,str);
		if (get_token_int(str,2)>0)
		{
			SelectRow(lst,i);
			/*if (gc.asel_as_list)	*/OnOK();
			return;
		}
	}
}