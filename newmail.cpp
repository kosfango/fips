// newmail.cpp : implementation file
// IDD_CFG_TEMPLATE

#include "stdafx.h"
#include "io.h"
#include "resource.h"
#include "structs.h"
#include "newmail.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern _gconfig	gc;
extern CStrList HeaderList;
extern CStrList FooterList;
extern CStrList OriginList;
extern pareadef gAreadef;

static char DlgName[]="IDD_CFG_TEMPLATE";

// ==================================================================
	newmail::newmail(CWnd* pParent /*=NULL*/)
		: CSAPrefsSubDlg(newmail::IDD, pParent)
// ==================================================================
{
	//{{AFX_DATA_INIT(newmail)
	m_originpath = _T("");
	//}}AFX_DATA_INIT
}

// ==================================================================
	void newmail::DoDataExchange(CDataExchange* pDX)
// ==================================================================
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(newmail)
	DDX_Control(pDX, IDC_RANDOM_NEW, m_random_new);
	DDX_Control(pDX, IDC_RANDOM_QUOTE, m_random_quote);
	DDX_Control(pDX, IDC_EDIT, m_edit);
	DDX_Text(pDX, IDC_ORIGINPATH, m_originpath);
	DDV_MaxChars(pDX, m_originpath, 255);
	DDX_Control(pDX, IDC_AREAS, m_areas);
	DDX_Control(pDX, IDC_TMPL, m_tmpl);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(newmail, CDialog)
	//{{AFX_MSG_MAP(newmail)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_CHANGE, OnChange)
	ON_BN_CLICKED(IDC_SET_NEW, OnSetNew)
	ON_BN_CLICKED(IDC_SET_QUOTE, OnSetQuote)
	ON_BN_CLICKED(IDC_RANDOM_QUOTE, OnRandomQuote)
	ON_BN_CLICKED(IDC_RANDOM_NEW, OnRandomNew)
	ON_BN_CLICKED(IDC_ORIGINFILE, OnOriginfile)
	ON_EN_KILLFOCUS(IDC_ORIGINPATH, OnKillfocusOriginpath)
	ON_BN_CLICKED(IDC_CHK_HEADER, OnChkHeader)
	ON_BN_CLICKED(IDC_CHK_FOOTER, OnChkFooter)
	ON_BN_CLICKED(IDC_CHK_ORIGIN, OnChkOrigin)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
	ON_NOTIFY(NM_CLICK, IDC_AREAS, OnNMClickAreas)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_TMPL, OnLvnItemchangedTmpl)
END_MESSAGE_MAP()

// =====================================================================
LPCSTR newmail::GetName(void)	{return DlgName;}
// =====================================================================

// ==================================================================
	BOOL newmail::OnInitDialog()
// ==================================================================
{
//areadef	ad;
pareadef pad;
CString str,aka;
char buf[1000];
_listcol lc[]={300,"",0,"HN",0,"HQ",0,"FN",0,"FQ",0,"ON",0,"OQ"};
_listcol lct[]={30,"¹",1000,""};
int	 i,maxaka,cnt;
int  lng[]={
			IDC_CHANGE,
			IDC_DELETE,
			IDC_ADD,
			IDC_SET_NEW,
			IDC_SET_QUOTE,
			IDC_RANDOM_NEW,
			IDC_RANDOM_QUOTE,
			IDC_STATIC2,
			IDC_CHK_HEADER,
			IDC_CHK_FOOTER,
			IDC_CHK_ORIGIN,
			};

	CDialog::OnInitDialog();
	set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));
	strcpy(lc[0].hdr,L("S_92"));	// area
	strcpy(lct[1].hdr,L("S_634"));	// template
	MakeColumns(m_areas,lc,sizeof(lc)/sizeof(_listcol),DlgName,1);
	MakeColumns(m_tmpl,lct,sizeof(lct)/sizeof(_listcol),DlgName,2);
// determine max length of AKA for calculating origin length
	maxaka=i=0;
	while (db_get_boss_by_index(i++,buf))
	{
		get_token(buf,MAINAKA,aka);
		if (aka.GetLength() > maxaka)	maxaka=aka.GetLength();
	}
	m_originpath=get_cfg(CFG_EDIT,"OriginFile","");
	if (access(m_originpath,0))	m_originpath.Empty();
// populate area list
	m_areas.DeleteAllItems();
	db_get_count_of_areas(&cnt);
	for(i=0;i<cnt;i++)
	{
		pad=&gAreadef[i];
		if (strcmp(pad->echotag,BBSMAIL) && pad->localmail==0 && strcmp(pad->group,ASAVED_GROUP))
		{
			sprintf(buf,"%s\t%d\t%d\t%d\t%d\t%d\t%d",pad->echotag,pad->header_new,pad->header_quote,
				pad->footer_new,pad->footer_quote,pad->origin_new,pad->origin_quote);
			AddRow(m_areas,buf);
		}
	}
	load_headers(TempHeader);
	load_footers(TempFooter);
	load_origins(TempOrigin);
	CheckRadioButton(IDC_CHK_HEADER,IDC_CHK_ORIGIN,IDC_CHK_HEADER);
	OnChkHeader();
	UpdateData(0);
	return TRUE;
}

// ==================================================================
	void newmail::OnAdd()
// ==================================================================
{
char buf[2000],buf1[2000];

	m_edit.GetWindowText(buf,1999);
	buf[1999]=0;
	remove_LFCR(buf);
	if (*buf==0)	return;
	russian_conversion(buf);
	switch (GetCheckedRadioButton(IDC_CHK_HEADER,IDC_CHK_ORIGIN))
	{
	default:
	case IDC_CHK_HEADER:
		TempHeader.AddTail(buf);
		sprintf(buf1,"%d\t%s",TempHeader.GetCount(),buf);
		AddRow(m_tmpl,buf1);
		SelectRow(m_tmpl,TempHeader.GetCount()-1);
		break;
	case IDC_CHK_FOOTER:
		TempFooter.AddTail(buf);
		sprintf(buf1,"%d\t%s",TempFooter.GetCount(),buf);
		AddRow(m_tmpl,buf1);
		SelectRow(m_tmpl,TempFooter.GetCount()-1);
		break;
	case IDC_CHK_ORIGIN:
		TempOrigin.AddTail(buf);
		sprintf(buf1,"%d\t%s",TempOrigin.GetCount(),buf);
		AddRow(m_tmpl,buf1);
		SelectRow(m_tmpl,TempOrigin.GetCount()-1);
		break;
	}
}

// ==================================================================
	void newmail::OnChange()
// ==================================================================
{
char buf[2000],buf1[2000];
int  sel;

	GET_SELECT(m_tmpl);
	m_edit.GetWindowText(buf,1999);
	buf[1999]=0;
	remove_LFCR(buf);
	if (*buf==0)	return;
	russian_conversion(buf);
	switch (GetCheckedRadioButton(IDC_CHK_HEADER,IDC_CHK_ORIGIN))
	{
	default:
	case IDC_CHK_HEADER:
		TempHeader.Replace(sel,buf);
		sprintf(buf1,"%d\t%s",TempHeader.GetCount(),buf);
		ReplaceRow(m_tmpl,sel,buf1);
		SelectRow(m_tmpl,TempHeader.GetCount()-1);
		break;
	case IDC_CHK_FOOTER:
		TempFooter.Replace(sel,buf);
		sprintf(buf1,"%d\t%s",TempFooter.GetCount(),buf);
		ReplaceRow(m_tmpl,sel,buf1);
		SelectRow(m_tmpl,TempFooter.GetCount()-1);
		break;
	case IDC_CHK_ORIGIN:
		TempOrigin.Replace(sel,buf);
		sprintf(buf1,"%d\t%s",TempOrigin.GetCount(),buf);
		ReplaceRow(m_tmpl,sel,buf1);
		SelectRow(m_tmpl,TempOrigin.GetCount()-1);
		break;
	}
	SelectRow(m_tmpl,sel);
}

// ==================================================================
	void newmail::OnDelete()
// ==================================================================
{
int sel;

	GET_SELECT(m_tmpl);
	switch (GetCheckedRadioButton(IDC_CHK_HEADER,IDC_CHK_ORIGIN))
	{
	default:
	case IDC_CHK_HEADER:
		TempHeader.Remove(sel);
		break;
	case IDC_CHK_FOOTER:
		TempFooter.Remove(sel);
		break;
	case IDC_CHK_ORIGIN:
		TempOrigin.Remove(sel);
		break;
	}
	m_tmpl.DeleteItem(sel);
}

// ====================================================
	void newmail::OnSetNew()
// ====================================================
{
int  sel;

	GET_SELECT(m_tmpl);
	SetNew(sel);
}

// ==================================================================
	void newmail::OnSetQuote()
// ==================================================================
{
int  sel;

	GET_SELECT(m_tmpl);
	SetQuote(sel);
}

// ====================================================
	void newmail::OnRandomNew()
// ====================================================
{
	SetNew(NO_ENTRY_SELECTED);
}

// ====================================================
	void newmail::OnRandomQuote()
// ====================================================
{
	SetQuote(NO_ENTRY_SELECTED);
}

// ====================================================
	void newmail::OnLvnItemchangedTmpl(NMHDR *pNMHDR, LRESULT *pResult)
// ====================================================
{
LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
char buf[2000];
int sel;

	GET_SELECT(m_tmpl);
	m_tmpl.GetItemText(sel,1,buf,1999);
	buf[1999]=0;
	insert_LFCR(buf);
	m_edit.SetWindowText(buf);
	*pResult = 0;
}


// ====================================================
	void newmail::OnOriginfile() 
// ====================================================
{
CString str;
int		ret;

	str.LoadString(IDS_TEXTFILT);
	CFileDialog	dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY,str);
	ret=dlg.DoModal();
	restore_base_path();
	if (ret != IDOK)	return;
	m_originpath=dlg.GetPathName();
	UpdateData(0);
	m_tmpl.SetFocus();
	OnKillfocusOriginpath();
}

// ==================================================================
	void newmail::OnKillfocusOriginpath() 
// ==================================================================
{
	UpdateData(1);
	if (access(m_originpath,0))
	{
		m_originpath.Empty();
		UpdateData(0);
		TempOrigin.LoadFromDelimText("origins.def",0);
	}
	else
		TempOrigin.LoadFromDelimText(m_originpath,0);
	CheckRadioButton(IDC_CHK_HEADER,IDC_CHK_ORIGIN,IDC_CHK_ORIGIN);
	OnChkOrigin();
}

// ==================================================================
	void newmail::OnOK()
// ==================================================================
{
areadef ad;
CString str,str1;
int cnt;

	UpdateData(1);
	if (gc.tosser.running || gc.purger.running)	ERR_MSG_RET("E_PMNCWTR");
	if (TempHeader.GetCount()<1 || TempOrigin.GetCount()<1 || TempFooter.GetCount()<1)	 
		ERR_MSG_RET("E_ATLEASTONE");
	
	cnt=TempHeader.GetCount();
	if (TempHeader.defaultindex >= cnt)	TempHeader.defaultindex=0;
	if (TempHeader.defaultindexnew >= cnt)TempHeader.defaultindexnew=0;
	TempHeader.SaveAsEdit("headers.def");
	set_cfg(CFG_EDIT,"HeaderQ",TempHeader.defaultindex);
	set_cfg(CFG_EDIT,"HeaderN",TempHeader.defaultindexnew);

	cnt=TempFooter.GetCount();
	if (TempFooter.defaultindex>=cnt)	TempFooter.defaultindex=0;
	if (TempFooter.defaultindexnew>=cnt)TempFooter.defaultindexnew=0;
	TempFooter.SaveAsEdit("footers.def");
	set_cfg(CFG_EDIT,"FooterQ",TempFooter.defaultindex);
	set_cfg(CFG_EDIT,"FooterN",TempFooter.defaultindexnew);
	
	set_cfg(CFG_EDIT,"OriginFile",m_originpath);
	if (!m_originpath.IsEmpty() && !access(m_originpath,0))
		TempOrigin.SaveAsEdit(m_originpath);
	else
		TempOrigin.SaveAsEdit("origins.def");
	
	cnt=TempOrigin.GetCount();
	if (TempOrigin.defaultindex>=cnt)	TempOrigin.defaultindex=NO_ENTRY_SELECTED;
	if (TempOrigin.defaultindexnew>=cnt)TempOrigin.defaultindexnew=NO_ENTRY_SELECTED;
	set_cfg(CFG_EDIT,"OriginQ",TempOrigin.defaultindex);
	set_cfg(CFG_EDIT,"OriginN",TempOrigin.defaultindexnew);

	load_headers(HeaderList);
	load_footers(FooterList);
	load_origins(OriginList);
	for(int i=0;i<m_areas.GetItemCount();i++)
	{
		GetRow(m_areas,i,str);
		get_token(str,0,str1);
		db_get_area_by_name(str1,&ad);
		gAreadef[ad.index].header_new=get_token_int(str,1);
		gAreadef[ad.index].header_quote=get_token_int(str,2);
		gAreadef[ad.index].footer_new=get_token_int(str,3);
		gAreadef[ad.index].footer_quote=get_token_int(str,4);
		gAreadef[ad.index].origin_new=get_token_int(str,5);
		gAreadef[ad.index].origin_quote=get_token_int(str,6);
	}
	flush_areabase_to_disk();
	SaveColumnsWidth(m_areas,DlgName,1);
	SaveColumnsWidth(m_tmpl,DlgName,2);
	if(m_bCloseOnOk)	CDialog::OnOK();
}

// ==================================================================
	void newmail::OnHelp()
// ==================================================================
{
	WinHelp(VHELP_MAIL_TEMPLATE);
}

// ==================================================================
	BOOL newmail::OnHelpInfo(HELPINFO* pHelpInfo) 
// ==================================================================
{
	OnHelp();
	return TRUE;
}

// ==================================================================
	void newmail::OnChkHeader() 
// ==================================================================
{
	ClearSelection();
	ENABLEID(IDC_SET_NEW);
	ENABLEID(IDC_SET_QUOTE);
	SetLists(TempHeader,TempHeader.defaultindexnew,TempHeader.defaultindex,FALSE);
}

// ==================================================================
	void newmail::OnChkFooter() 
// ==================================================================
{
	ClearSelection();
	ENABLEID(IDC_SET_NEW);
	ENABLEID(IDC_SET_QUOTE);
	SetLists(TempFooter,TempFooter.defaultindexnew,TempFooter.defaultindex,FALSE);
}

// ==================================================================
	void newmail::OnChkOrigin() 
// ==================================================================
{
	ClearSelection();
	DISABLEID(IDC_SET_NEW);
	DISABLEID(IDC_SET_QUOTE);
	SetLists(TempOrigin,TempOrigin.defaultindexnew,TempOrigin.defaultindex,TRUE);
}

// ==================================================================
	void newmail::SetLists(CStrList &lst,int indn,int indq,BOOL orig) 
// ==================================================================
{
char buf[2000];
NMHDR tmp;
LRESULT tmp1;

	m_tmpl.DeleteAllItems();
	for (int i=0;i < lst.GetCount();i++)
	{
		sprintf(buf,"%d\t%s",i+1,lst.GetString(i));
		AddRow(m_tmpl,buf);
	}
	GetDlgItem(IDC_RANDOM_NEW)->EnableWindow(orig);
	GetDlgItem(IDC_RANDOM_QUOTE)->EnableWindow(orig);
	m_random_new.SetCheck(orig);
	m_random_quote.SetCheck(orig);
	if (orig)
	{
		GetDlgItem(IDC_SET_NEW)->SetWindowText(L("S_602"));
		GetDlgItem(IDC_SET_QUOTE)->SetWindowText(L("S_601"));
		m_edit.SetWindowText("");
	}
	else
	{
		sprintf(buf,"%s %d",L("S_602"),indn+1);
		GetDlgItem(IDC_SET_NEW)->SetWindowText(buf);
		sprintf(buf,"%s %d",L("S_601"),indq+1);
		GetDlgItem(IDC_SET_QUOTE)->SetWindowText(buf);
		SelectRow(m_tmpl,indn);
		OnLvnItemchangedTmpl(&tmp,&tmp1);
	}
}

// ==================================================================
	void newmail::OnNMClickAreas(NMHDR *pNMHDR, LRESULT *pResult)
// ==================================================================
{
char buf[20];
CString	str;
int	sel,nsel;
NMHDR tmp;
LRESULT tmp1;

	sel=GetSelectedItem(m_tmpl);
	if (sel >= 0)	DeselectRow(m_tmpl,sel);
	GET_SELECT(m_areas);
	nsel=m_areas.GetSelectedCount();
	if (nsel==1)	GetRow(m_areas,sel,str);
	switch (GetCheckedRadioButton(IDC_CHK_HEADER,IDC_CHK_ORIGIN))
	{
	default:
	case IDC_CHK_HEADER:
		m_random_new.SetCheck(FALSE);
		m_random_quote.SetCheck(FALSE);
		sprintf(buf,nsel==1 ? "%s %d" : "%s ?",L("S_602"),get_token_int(str,1)+1);
		GetDlgItem(IDC_SET_NEW)->SetWindowText(buf);
		sprintf(buf,nsel==1 ? "%s %d" : "%s ?",L("S_601"),get_token_int(str,2)+1);
		GetDlgItem(IDC_SET_QUOTE)->SetWindowText(buf);
		SelectRow(m_tmpl,get_token_int(str,1));
		OnLvnItemchangedTmpl(&tmp,&tmp1);
		break;
	case IDC_CHK_FOOTER:
		m_random_new.SetCheck(FALSE);
		m_random_quote.SetCheck(FALSE);
		sprintf(buf,nsel==1 ? "%s %d" : "%s ?",L("S_602"),get_token_int(str,3)+1);
		GetDlgItem(IDC_SET_NEW)->SetWindowText(buf);
		sprintf(buf,nsel==1 ? "%s %d" : "%s ?",L("S_601"),get_token_int(str,4)+1);
		GetDlgItem(IDC_SET_QUOTE)->SetWindowText(buf);
		SelectRow(m_tmpl,get_token_int(str,3));
		OnLvnItemchangedTmpl(&tmp,&tmp1);
		break;
	case IDC_CHK_ORIGIN:
		m_random_new.SetCheck(nsel==1 ? get_token_int(str,5)==NO_ENTRY_SELECTED : FALSE);
		m_random_quote.SetCheck(nsel==1 ? get_token_int(str,6)==NO_ENTRY_SELECTED : FALSE);
		sprintf(buf,nsel==1 ? (get_token_int(str,5)==NO_ENTRY_SELECTED ? "%s" : "%s %d")
			: "%s ?",L("S_602"),get_token_int(str,5)+1);
		GetDlgItem(IDC_SET_NEW)->SetWindowText(buf);
		sprintf(buf,nsel==1 ? (get_token_int(str,6)==NO_ENTRY_SELECTED ? "%s" : "%s %d")
			: "%s ?",L("S_601"),get_token_int(str,6)+1);
		GetDlgItem(IDC_SET_QUOTE)->SetWindowText(buf);
		SelectRow(m_tmpl,get_token_int(str,5));
		OnLvnItemchangedTmpl(&tmp,&tmp1);
		break;
	}
	ENABLEID(IDC_SET_NEW);
	ENABLEID(IDC_SET_QUOTE);
	*pResult = 0;
}

// ==================================================================
// clear selections in areas list
	void newmail::ClearSelection(void)
// ==================================================================
{
int	 ni=-1;

	for (UINT i=0;i < m_areas.GetSelectedCount();i++)
	{
		ni = m_areas.GetNextItem(ni,LVNI_SELECTED);
		m_areas.SetItemState(ni,0,0x000F);
	}
}

// ==================================================================
// update selections in areas list
	void newmail::UpdateSelection(int ind,int val)
// ==================================================================
{
char buf[10];
int	 ni=-1;

	for (UINT i=0;i < m_areas.GetSelectedCount();i++)
	{
		ni = m_areas.GetNextItem(ni,LVNI_SELECTED);
		m_areas.SetItemText(ni,ind,itoa(val,buf,10)); 
	}
}

// ==================================================================
// set new value for news
	void newmail::SetNew(int val)
// ==================================================================
{
char buf[20];
int nsel;

	nsel=m_areas.GetSelectedCount();
	switch (GetCheckedRadioButton(IDC_CHK_HEADER,IDC_CHK_ORIGIN))
	{
	default:
	case IDC_CHK_HEADER:
		if (nsel > 0)
			UpdateSelection(1,val);
		else
			TempHeader.defaultindexnew=val;
		break;
	case IDC_CHK_FOOTER:
		if (nsel > 0)
			UpdateSelection(3,val);
		else
			TempFooter.defaultindexnew=val;
		break;
	case IDC_CHK_ORIGIN:
		if (nsel > 0)
			UpdateSelection(5,val);
		else
			TempOrigin.defaultindexnew=NO_ENTRY_SELECTED;
		break;
	}
	sprintf(buf,val==NO_ENTRY_SELECTED ? "%s" : "%s %d",L("S_602"),val+1);	// for new
	m_random_new.SetCheck(val==NO_ENTRY_SELECTED);
	GetDlgItem(IDC_SET_NEW)->SetWindowText(buf);
}

// ==================================================================
// set new value for quotas
	void newmail::SetQuote(int val)
// ==================================================================
{
char buf[20];
int nsel;

	nsel=m_areas.GetSelectedCount();
	switch (GetCheckedRadioButton(IDC_CHK_HEADER,IDC_CHK_ORIGIN))
	{
	default:
	case IDC_CHK_HEADER:
		if (nsel > 0)
			UpdateSelection(2,val);
		else
			TempHeader.defaultindex=val;
		break;
	case IDC_CHK_FOOTER:
		if (nsel > 0)
			UpdateSelection(4,val);
		else
			TempFooter.defaultindex=val;
		break;
	case IDC_CHK_ORIGIN:
		if (nsel > 0)
			UpdateSelection(6,val);
		else
			TempOrigin.defaultindex=val;
		break;
	}
	sprintf(buf,val==NO_ENTRY_SELECTED ? "%s" : "%s %d",L("S_601"),val+1);	// for quote
	m_random_quote.SetCheck(val==NO_ENTRY_SELECTED);
	GetDlgItem(IDC_SET_QUOTE)->SetWindowText(buf);
}