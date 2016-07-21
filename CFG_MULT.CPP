// cfg_mult.cpp : implementation file
// IDD_CFG_DESIGN

#include "stdafx.h"
#include "lightdlg.h"
#include "light.h"
#include "floating.h"
#include "cfg_mult.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern HINSTANCE hRes;
extern CLightApp FipsApp;
extern floating	 gFloating;
extern CLightDlg *gpMain;

CImageList *m_pSkin;
HINSTANCE hTmp;
static CStrList	 avalist;
static char DlgName[]="IDD_CFG_DESIGN";

// ====================================================================
	cfg_mult::cfg_mult(CWnd* pParent ) 	: CSAPrefsSubDlg(cfg_mult::IDD, pParent)
// ====================================================================
{
	//{{AFX_DATA_INIT(cfg_mult)
	m_skinpath = _T("");
	//}}AFX_DATA_INIT
}

// ====================================================================
	void cfg_mult::DoDataExchange(CDataExchange* pDX)
// ====================================================================
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(cfg_mult)
	DDX_Control(pDX, IDC_USED, m_used);
	DDX_Control(pDX, IDC_AVAIL, m_avail);
	DDX_Control(pDX, IDC_ICONS, m_skin);
	DDX_Text(pDX, IDC_SKINPATH, m_skinpath);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(cfg_mult, CDialog)
	//{{AFX_MSG_MAP(cfg_mult)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_WM_HELPINFO()
	ON_BN_CLICKED(IDC_SKINBR, OnSkinbr)
	ON_BN_CLICKED(IDC_DOWN, OnDown)
	ON_BN_CLICKED(IDC_UP, OnUp)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_NOTIFY(NM_SETFOCUS, IDC_AVAIL, OnSetfocusAvail)
	ON_NOTIFY(NM_SETFOCUS, IDC_USED, OnSetfocusUsed)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// =====================================================================
LPCSTR cfg_mult::GetName(void)	{return DlgName;}
// =====================================================================

// ====================================================================
	BOOL cfg_mult::OnInitDialog()
// ====================================================================
{
int	lng[]={
			IDC_STATIC1,
			IDC_STATIC2,
			IDC_STATIC3,
			IDC_STATIC4,
			IDC_UP,
			IDC_DOWN,
			};

	CDialog::OnInitDialog();
    set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));
	m_skinpath=get_cfg(CFG_COMMON,"SkinPath","");
	m_skin.SetIconSpacing(48,25);
	m_avail.SetIconSpacing(80,50);
	m_used.SetIconSpacing(80,50);
	m_avail.SetExtendedStyle(LVS_SHAREIMAGELISTS);
	m_used.SetExtendedStyle(LVS_SHAREIMAGELISTS);
	m_pSkin = new CImageList();
	m_pSkin->Create(35,22,ILC_MASK | ILC_COLOR8,12,4);
	m_pIcons = new CImageList();
	m_pIcons->Create(35,22,ILC_MASK | ILC_COLOR8,12,4);
	load_skin(hRes);
	load_avail(hRes);
	load_used(hRes);
	UpdateData(0);
	return TRUE;
}

// ====================================================================
	void cfg_mult::OnOK()
// ====================================================================
{
CString	str,sid;

	UpdateData(1);
	if (hRes) FreeLibrary(hRes);
	if (!m_skinpath.IsEmpty())
	{
		set_cfg(CFG_COMMON,"SkinPath",m_skinpath);
		hRes=LoadLibrary(m_skinpath);
	}
	if (m_skinpath.IsEmpty() || hRes==0)
	{
		del_cfg(CFG_COMMON,"SkinPath");
		hRes=FipsApp.m_hInstance;
	}
	gpMain->set_images();
	gFloating.set_images();
	for (int i=0;i<m_used.GetItemCount();i++)
	{
		if (!str.IsEmpty()) str+=",";
		sid.Format("%d",m_used.GetItemData(i));
		str+=sid;
	}
	set_cfg(CFG_COMMON,"FloatBar",str);
//	ForceRedraw();
	if(m_bCloseOnOk)
		CDialog::OnOK();
}

void cfg_mult::OnSkinbr() 
{
CString	str;
int		ret;
	
	str.LoadString(IDS_SKINFILT);
	CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY,str);
	ret=dlg.DoModal();
	restore_base_path();
	if (ret!=IDOK)	return;
	m_skinpath=dlg.GetPathName();
	hTmp=LoadLibrary(m_skinpath);
	if (hTmp) load_skin(hTmp);
	UpdateData(0);
}

void cfg_mult::OnUp() 
{
POSITION pos;
int		 nItem;
	
	pos=m_used.GetFirstSelectedItemPosition();
	if (pos)
	{
		nItem=m_used.GetNextSelectedItem(pos);
		if (nItem>0)
			swap_fields(nItem,nItem-1);
		m_used.SetFocus();
	}
}

void cfg_mult::OnDown() 
{
POSITION pos;
int		 nItem;
	
	pos = m_used.GetFirstSelectedItemPosition();
	if (pos)
	{
		nItem = m_used.GetNextSelectedItem(pos);
		if (nItem<m_used.GetItemCount()-1)
			swap_fields(nItem,nItem+1);
		m_used.SetFocus();
	}
}

void cfg_mult::OnDelete() 
{
POSITION pos;
LVITEM	lvi;
char	buf[50];
int		nItem;

	pos=m_used.GetFirstSelectedItemPosition();
	if (pos)
	{
		nItem = m_used.GetNextSelectedItem(pos);
		lvi.mask=LVIF_IMAGE | LVIF_TEXT | LVIF_PARAM;
		lvi.pszText=buf;
		lvi.cchTextMax=50;
		lvi.iItem=nItem;
		lvi.iSubItem=0;
		m_used.GetItem(&lvi);
		lvi.iItem=m_avail.GetItemCount();
		if (lvi.lParam!=IDI_EMPTY)
			m_avail.InsertItem(&lvi);
		m_used.DeleteItem(nItem);
		lvi.iItem=nItem==m_used.GetItemCount() ? m_used.GetItemCount()-1 : nItem;
		lvi.mask=LVIF_STATE;
		lvi.state=LVIS_SELECTED | LVIS_FOCUSED;
		lvi.stateMask=-1;
		m_used.SetItem(&lvi);
		m_used.SetFocus();
	}
}

void cfg_mult::OnAdd() 
{
POSITION pos;
LVITEM	lvi;
char	buf[50];
int		nItem;

	pos = m_avail.GetFirstSelectedItemPosition();
	if (pos)
	{
		nItem = m_avail.GetNextSelectedItem(pos);
		lvi.mask=LVIF_IMAGE | LVIF_TEXT | LVIF_PARAM;
		lvi.pszText=buf;
		lvi.cchTextMax=50;
		lvi.iItem=nItem;
		lvi.iSubItem=0;
		m_avail.GetItem(&lvi);
		lvi.iItem=m_used.GetItemCount();
		m_used.InsertItem(&lvi);
		m_used.EnsureVisible(m_used.GetItemCount()-1,FALSE);
		if (lvi.lParam!=IDI_EMPTY)
			m_avail.DeleteItem(nItem);
		lvi.iItem=nItem==m_avail.GetItemCount() ? m_avail.GetItemCount()-1 : nItem;
		lvi.mask=LVIF_STATE;
		lvi.state=LVIS_SELECTED | LVIS_FOCUSED;
		lvi.stateMask=-1;
		m_avail.SetItem(&lvi);
		m_avail.SetFocus();
	}
}

void cfg_mult::OnSetfocusAvail(NMHDR* pNMHDR, LRESULT* pResult) 
{
	DISABLEID(IDC_UP);
	DISABLEID(IDC_DOWN);
	DISABLEID(IDC_DELETE);
	ENABLEID(IDC_ADD);
	*pResult = 0;
}

void cfg_mult::OnSetfocusUsed(NMHDR* pNMHDR, LRESULT* pResult) 
{
	ENABLEID(IDC_UP);
	ENABLEID(IDC_DOWN);
	ENABLEID(IDC_DELETE);
	DISABLEID(IDC_ADD);
	*pResult = 0;
}

cfg_mult::~cfg_mult() 
{
	//if (m_pSkin)	delete m_pSkin;
	//if (m_pIcons)	delete m_pIcons;
}

// ====================================================================
void cfg_mult::OnHelp() 
// ====================================================================
{
	WinHelp(VHELP_CFG_MULTIMEDIA);
}

BOOL cfg_mult::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return TRUE;
}

void cfg_mult::swap_fields(int ind1,int ind2)
{
LVITEM lvi1,lvi2;
char buf1[50],buf2[50];

	lvi1.mask=LVIF_IMAGE | LVIF_TEXT | LVIF_PARAM | LVIF_STATE;
	lvi1.pszText=buf1;
	lvi1.cchTextMax=50;
	lvi1.iItem=ind1;
	lvi1.iSubItem=0;
	m_used.GetItem(&lvi1);
	lvi2.mask=LVIF_IMAGE | LVIF_TEXT | LVIF_PARAM | LVIF_STATE;
	lvi2.pszText=buf2;
	lvi2.cchTextMax=50;
	lvi2.iItem=ind2;
	lvi2.iSubItem=0;
	m_used.GetItem(&lvi2);
	lvi1.iItem=ind2;
	lvi2.iItem=ind1;
	lvi1.state=LVIS_SELECTED | LVIS_FOCUSED;
	lvi1.stateMask=-1;
	m_used.SetItem(&lvi1);
	m_used.SetItem(&lvi2);
	m_used.EnsureVisible(ind2,FALSE);
}

// =========================================================================
void cfg_mult::ForceRedraw(void)
// =========================================================================
{
RECT re;
	
	gFloating.GetClientRect(&re); 
	gFloating.InvalidateRect(&re);
	gFloating.RepositionAllBitmaps();
	gFloating.GetClientRect(&re); 
	gFloating.InvalidateRect(&re);
}

void cfg_mult::load_skin(HINSTANCE hRes)
{
	m_pSkin->SetImageCount(0);
	m_pSkin->Add(LoadIcon(hRes,MAKEINTRESOURCE(IDI_PHOENIX)));
	m_pSkin->Add(LoadIcon(hRes,MAKEINTRESOURCE(IDI_MAILER)));
	m_pSkin->Add(LoadIcon(hRes,MAKEINTRESOURCE(IDI_TOSSER)));
	m_pSkin->Add(LoadIcon(hRes,MAKEINTRESOURCE(IDI_PURGER)));
	m_pSkin->Add(LoadIcon(hRes,MAKEINTRESOURCE(IDI_NEW)));
	m_pSkin->Add(LoadIcon(hRes,MAKEINTRESOURCE(IDI_QUOTE)));
	m_pSkin->Add(LoadIcon(hRes,MAKEINTRESOURCE(IDI_ASAVE)));
	m_pSkin->Add(LoadIcon(hRes,MAKEINTRESOURCE(IDI_FILT)));
	m_pSkin->Add(LoadIcon(hRes,MAKEINTRESOURCE(IDI_FIND)));
	m_pSkin->Add(LoadIcon(hRes,MAKEINTRESOURCE(IDI_FREQ)));
	m_pSkin->Add(LoadIcon(hRes,MAKEINTRESOURCE(IDI_MACRO1)));
	m_pSkin->Add(LoadIcon(hRes,MAKEINTRESOURCE(IDI_EXIT)));
	m_skin.DeleteAllItems();
	m_skin.SetImageList(m_pSkin,LVSIL_NORMAL);
	m_skin.InsertItem(0,"FIPS",0);
	m_skin.InsertItem(1,L("S_436"),1);
	m_skin.InsertItem(2,L("S_437"),2);
	m_skin.InsertItem(3,L("S_438"),3);
	m_skin.InsertItem(4,L("S_147"),4);
	m_skin.InsertItem(5,L("S_370"),5);
	m_skin.InsertItem(6,L("S_98"),6);
	m_skin.InsertItem(7,L("S_251"),7);
	m_skin.InsertItem(8,L("S_252"),8);
	m_skin.InsertItem(9,L("S_379"),9);
	m_skin.InsertItem(10,L("S_435"),10);
	m_skin.InsertItem(11,L("S_236"),11);
}

// =========================================================================
void cfg_mult::load_avail(HINSTANCE hRes)
// =========================================================================
{
int ind;

	m_pIcons->SetImageCount(0);
	ind=m_pIcons->Add(LoadIcon(hRes,MAKEINTRESOURCE(IDI_ASAVE)));
	m_pIcons->Add(LoadIcon(hRes,MAKEINTRESOURCE(IDI_NEW)));
	m_pIcons->Add(LoadIcon(hRes,MAKEINTRESOURCE(IDI_QUOTE)));
	m_pIcons->Add(LoadIcon(hRes,MAKEINTRESOURCE(IDI_QUOTEA)));
	m_pIcons->Add(LoadIcon(hRes,MAKEINTRESOURCE(IDI_FIND)));
	m_pIcons->Add(LoadIcon(hRes,MAKEINTRESOURCE(IDI_FILT)));
	m_pIcons->Add(LoadIcon(hRes,MAKEINTRESOURCE(IDI_FREQ)));
	m_pIcons->Add(LoadIcon(hRes,MAKEINTRESOURCE(IDI_MACRO1)));
	m_pIcons->Add(LoadIcon(hRes,MAKEINTRESOURCE(IDI_MACRO2)));
	m_pIcons->Add(LoadIcon(hRes,MAKEINTRESOURCE(IDI_MACRO3)));
	m_pIcons->Add(LoadIcon(hRes,MAKEINTRESOURCE(IDI_MACRO4)));
	m_pIcons->Add(LoadIcon(hRes,MAKEINTRESOURCE(IDI_MACRO5)));
	m_pIcons->Add(LoadIcon(hRes,MAKEINTRESOURCE(IDI_EXIT)));
	m_pIcons->Add(LoadIcon(hRes,MAKEINTRESOURCE(IDI_EMPTY)));
	m_avail.DeleteAllItems();
	m_used.DeleteAllItems();
	m_avail.SetImageList(m_pIcons,LVSIL_NORMAL);
	m_used.SetImageList(m_pIcons,LVSIL_NORMAL);
	m_avail.SetItemData(m_avail.InsertItem(0,L("S_98"),0),IDI_ASAVE);
	m_avail.SetItemData(m_avail.InsertItem(1,L("S_147"),1),IDI_NEW);
	m_avail.SetItemData(m_avail.InsertItem(2,L("S_370"),2),IDI_QUOTE);
	m_avail.SetItemData(m_avail.InsertItem(3,L("S_371"),3),IDI_QUOTEA);
	m_avail.SetItemData(m_avail.InsertItem(4,L("S_252"),4),IDI_FIND);
	m_avail.SetItemData(m_avail.InsertItem(5,L("S_251"),5),IDI_FILT);
	m_avail.SetItemData(m_avail.InsertItem(6,L("S_379"),6),IDI_FREQ);
	m_avail.SetItemData(m_avail.InsertItem(7,L("S_435"),7),IDI_MACRO1);
	m_avail.SetItemData(m_avail.InsertItem(8,L("S_435"),8),IDI_MACRO2);
	m_avail.SetItemData(m_avail.InsertItem(9,L("S_435"),9),IDI_MACRO3);
	m_avail.SetItemData(m_avail.InsertItem(10,L("S_435"),10),IDI_MACRO4);
	m_avail.SetItemData(m_avail.InsertItem(11,L("S_435"),11),IDI_MACRO5);
	m_avail.SetItemData(m_avail.InsertItem(12,L("S_236"),12),IDI_EXIT);
	m_avail.SetItemData(m_avail.InsertItem(13,L("S_179"),13),IDI_EMPTY);
}

void cfg_mult::load_used(HINSTANCE hRes)
{
LVFINDINFO  lfi;
LVITEM		lvi;
CString		str,sid;
int			i=0;

	str=get_cfg(CFG_COMMON,"FloatBar","");
	if (str.IsEmpty())	return;
	do 
	{
		get_token(str,i++,sid,',');
		lfi.flags=LVFI_PARAM;
		lfi.lParam=atoi(sid);
		lvi.mask=LVIF_STATE;
		lvi.iItem=m_avail.FindItem(&lfi);
		if (lvi.iItem>=0)
		{
			lvi.iSubItem=0;
			lvi.state=LVIS_SELECTED | LVIS_FOCUSED;
			lvi.stateMask=-1;
			m_avail.SetItem(&lvi);
			OnAdd();
		}
	} while(!sid.IsEmpty());
	DISABLEID(IDC_ADD);
}
