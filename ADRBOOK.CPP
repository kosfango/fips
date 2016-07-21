// adrbook.cpp : implementation file
// IDD_ADDRBOOK

#include "stdafx.h"
#include "adrbook.h"
#include "cfido.h"
#include "receiver.h"
#include "cfg_gate.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern CStrList Abook;
extern _gconfig gc;

extern int header_changed;
extern int footer_changed;
extern int template_text;

static _DlgItemsSize DlgItemsSize [] =
{
	0,			    {0,0,0,0},{0,0,0,0},0,0,0,0,0,
	IDC_LIST,	    {0,0,0,0},{0,0,0,0},0,0,0,HO|VE,0,
	IDC_STATIC2,	{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_STATIC3,	{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_STATIC4,	{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_STATIC5,	{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_NAME,		{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_FIDO,	    {0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_INFO,		{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_NICKNAME,	{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_CHANGE,	    {0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_ADD,        {0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_DELETE,	    {0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDHELP,			{0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDCANCEL,	    {0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDOK,	        {0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
};
static char DlgName[]="IDD_ADDRBOOK";

// ===========================================================
	adrbook::adrbook(CWnd* pParent /*=NULL*/)
		: CDialog(adrbook::IDD, pParent)
// ===========================================================
{
	//{{AFX_DATA_INIT(adrbook)
	*name=0;
	*address=0;
	//}}AFX_DATA_INIT
}

// ===========================================================
	void adrbook::DoDataExchange(CDataExchange* pDX)
// ===========================================================
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(adrbook)
	DDX_Control(pDX, IDC_NICKNAME, m_nick);
	DDX_Control(pDX, IDC_INFO, m_info);
	DDX_Control(pDX, IDC_NAME, m_name);
	DDX_Control(pDX, IDC_FIDO, m_fido);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_LIST, m_addresses);
}

BEGIN_MESSAGE_MAP(adrbook, CDialog)
	//{{AFX_MSG_MAP(adrbook)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_CHANGE, OnChange)
	ON_BN_CLICKED(ID_SORT_ADRESS, OnSortAdress)
	ON_BN_CLICKED(ID_SORT_COMMENT, OnSortComment)
	ON_BN_CLICKED(ID_SORT_NAME, OnSortName)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_NOTIFY(NM_CLICK, IDC_LIST, OnNMClickAddr)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// ===========================================================
	BOOL adrbook::OnInitDialog()
// ===========================================================
{
CString str;
_listcol lc[]={150,"",80,"",153,"",110,""};
int tabs[]={90,150};
int i;
int lng[]={
			IDOK,
			IDCANCEL,
			IDC_ADD,
			IDC_DELETE,
			IDC_STATIC3,
			IDC_STATIC2,
			IDC_STATIC5,
			IDC_CHANGE,
			IDC_STATIC1,
			IDC_STATIC4,
			IDHELP,
			};

	CDialog::OnInitDialog();
    StoreOrgItemSizes(DlgName,this,DlgItemsSize,sizeof(DlgItemsSize)/sizeof(_DlgItemsSize));
    set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));
	strcpy(lc[0].hdr,L("S_554"));	// name
	strcpy(lc[1].hdr,L("S_538"));	// address
	strcpy(lc[2].hdr,L("S_635"));	// comment
	strcpy(lc[3].hdr,L("S_636"));	// nickname
	MakeColumns(m_addresses,lc,sizeof(lc)/sizeof(_listcol),DlgName,1);
    for (i=0;i<Abook.GetCount();i++)
		AddRow(m_addresses,Abook.GetString(i));
	return TRUE;
}

// ===========================================================
	void adrbook::OnAdd()
// ===========================================================
{
CString str,name,fido,info,nick;

    m_name.GetWindowText(name);
    m_fido.GetWindowText(fido);
    m_info.GetWindowText(info);
    m_nick.GetWindowText(nick);
	str=name+'\t'+fido+'\t'+info+'\t'+nick;
	AddRow(m_addresses,str);
	SelectRow(m_addresses,m_addresses.GetItemCount()-1);
}

// ===========================================================
	void adrbook::OnDelete()
// ===========================================================
{
int sel;

	GET_SELECT(m_addresses);
	m_addresses.DeleteItem(sel);
	SelectRow(m_addresses,sel-1);
}

// ===========================================================
	void adrbook::OnChange()
// ===========================================================
{
int		sel;
CString str,name,fido,info,nick;

	GET_SELECT(m_addresses);
    m_name.GetWindowText(name);
    m_fido.GetWindowText(fido);
    m_info.GetWindowText(info);
    m_nick.GetWindowText(nick);
	str=name+'\t'+fido+'\t'+info+'\t'+nick;
	ReplaceRow(m_addresses,sel,str);
	SelectRow(m_addresses,sel);
}

// ===========================================================
	void adrbook::OnNMClickAddr(NMHDR *pNMHDR, LRESULT *pResult)
// ===========================================================
{
CString str,buf;
int sel;

	GET_SELECT(m_addresses);
	GetRow(m_addresses,sel,str);
	get_token(str,0,buf);
	m_name.SetWindowText(buf);
	get_token(str,1,buf);
	m_fido.SetWindowText(buf);
	get_token(str,2,buf);
	m_info.SetWindowText(buf);
	get_token(str,3,buf);
	m_nick.SetWindowText(buf);
	*pResult = 0;
}

// ===========================================================
	void adrbook::OnOK()
// ===========================================================
{
	UpdateData(1);
	//GET_SELECT(m_addresses);
	//m_groups.GetText(sel,str);
	//get_token(str,0,name);
	//get_token(str,1,fido);
	//get_token(str,2,info);
	//get_token(str,3,nick);
	//gc.adrbook_valid=1;
	//strncpy(gc.adrbook_name,name,99);
	//strncpy(gc.adrbook_fido,fido,99);
	//strncpy(gc.adrbook_comment,info,999);
    StoreInitDialogSize(DlgName,this);
	Abook.SaveToFile("adrbook.cfg");
	SaveColumnsWidth(m_addresses,DlgName,1);
	CDialog::OnOK();
}

//// ===========================================================
//	void adrbook::OnDblclkList1()
//// ===========================================================
//{
//	header_changed=0;
//	footer_changed=0;
//	template_text=0;
//	OnOK();
//}
//
//// ===========================================================
//	void adrbook::OnSelchangeList1()  // Selection Changed ..
//// ===========================================================
//{
//int		sel;
//CString name;
//CString fido;
//CString all;
//CString infozeile;
//CString nickname;
//
//	GET_SELID(IDC_LIST1);
//
//	all=Abook.GetString(sel);
//	get_token(all,0,name);
//	get_token(all,1,fido);
//	get_token(all,2,infozeile);
//	get_token(all,3,nickname);
//
//    m_name.SetWindowText(name);
//    m_fido.SetWindowText(fido);
//	m_info.SetWindowText(infozeile);
//	m_nick.SetWindowText(nickname);
//}

//// ===========================================================
//	void adrbook::OnGroupList()
//// ===========================================================
//{
//	receiver tmp;
//	tmp.DoModal();
//	if (gDblSelGroup.GetLength()>0)
//		CDialog::OnOK();
//}

//// ===========================================================
//	void adrbook::OnInternetgate()
//// ===========================================================
//{
//	cfg_gate tmp;
//	tmp.DoModal();
//	this->SetFocus();
//}

// ============================================
	BOOL adrbook::PreTranslateMessage(MSG* pMsg)
// ============================================
{
	if (pMsg->message==WM_RBUTTONDOWN && HandleRightButton(pMsg))	return TRUE;
	return CDialog::PreTranslateMessage(pMsg);
}

// ============================================
	void adrbook::OnSortAdress()
// ============================================
{
	Abook.Sort(1);
	UPDATE_LB(Abook,IDC_LIST1);
}

// ============================================
	void adrbook::OnSortComment()
// ============================================
{
	Abook.Sort(2);
	UPDATE_LB(Abook,IDC_LIST1);
}

// ============================================
	void adrbook::OnSortName()
// ============================================
{
	Abook.Sort(0);
	UPDATE_LB(Abook,IDC_LIST1);
}

// ============================================
	void adrbook::OnSize(UINT nType, int cx, int cy)
// ============================================
{
	CDialog::OnSize(nType, cx, cy);
	resize_wnd(this,DlgItemsSize,sizeof(DlgItemsSize));
}

// ===============================================
	void adrbook::OnPaint()
// ===============================================
{
	CPaintDC dc(this);
	DrawRightBottomKnubble(this,dc);
}
	
// ===========================================================
	void adrbook::OnHelp() 	// HELP Button
// ===========================================================
{
	WinHelp(VHELP_ADDRBOOK);
}

BOOL adrbook::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return TRUE;
}
