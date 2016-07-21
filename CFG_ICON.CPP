// cfg_icon.cpp : implementation file
// IDD_CFG_ICONS

#include "stdafx.h"
#include "lightdlg.h"
#include "cfg_icon.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CStrList  SoundLst;
extern _gconfig  gc;

static char DlgName[]="IDD_CFG_MMEDIA";

// =========================================================================
	cfg_icon::cfg_icon(CWnd* pParent ) : CSAPrefsSubDlg(cfg_icon::IDD, pParent)
// =========================================================================
{
	//{{AFX_DATA_INIT(cfg_icon)
	m_play_sounds = FALSE;
	m_play_bmps = FALSE;
	m_notfound = FALSE;
	m_wavpath = _T("");
	//}}AFX_DATA_INIT
}

// =========================================================================
	void cfg_icon::DoDataExchange(CDataExchange* pDX)
// =========================================================================
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(cfg_icon)
	DDX_Control(pDX, IDC_LIST, m_list);
	DDX_Check(pDX, IDC_PLAY_SOUNDS, m_play_sounds);
	DDX_Check(pDX, IDC_DISPLAY_BMPS, m_play_bmps);
	DDX_Check(pDX, IDC_DD_NOTFOUND, m_notfound);
	DDX_Text(pDX, IDC_WAVPATH, m_wavpath);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_MMLIST, m_mmlist);
	DDX_Control(pDX, IDC_BMP, m_bmp);
}

BEGIN_MESSAGE_MAP(cfg_icon, CDialog)
	//{{AFX_MSG_MAP(cfg_icon)
	ON_BN_CLICKED(IDDEFAULT, OnDefault)
	ON_BN_CLICKED(IDTESTSOUND, OnTestsound)
	ON_BN_CLICKED(IDSTOPSOUND, OnStopsound)
	ON_BN_CLICKED(IDC_CHANGE, OnChange)
	ON_BN_CLICKED(IDC_WAVEBR, OnWavebr)
	ON_LBN_SELCHANGE(IDC_LIST, OnSelchangeList)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_SHOWBMP, OnBnClickedShowbmp)
	ON_BN_CLICKED(IDC_SHOWWAV, OnBnClickedShowwav)
	ON_LBN_SELCHANGE(IDC_MMLIST, OnLbnSelchangeMmlist)
	ON_BN_CLICKED(IDC_STOP, OnBnClickedStop)
	ON_BN_CLICKED(IDC_PLAY, OnBnClickedPlay)
END_MESSAGE_MAP()

// =====================================================================
LPCSTR cfg_icon::GetName(void)	{return DlgName;}
// =====================================================================

// =========================================================================
	BOOL cfg_icon::OnInitDialog()
// =========================================================================
{
int tabs[]={80,300};
int tabs1[]={50,130};
int lng[]={
			IDC_STATIC1,
			IDC_STATIC2,
			IDC_STATIC3,
			IDC_STATIC4,
			IDC_PLAY_SOUNDS,
			IDC_DISPLAY_BMPS,
			IDC_DD_NOTFOUND,
			IDC_SIZE0,
			IDC_SIZE1,
			IDC_SIZE2,
			IDC_SIZE3,
			IDC_DEFAULT,
			IDTESTSOUND,
			IDSTOPSOUND,
			IDC_CHANGE,
			IDC_SHOWBMP,
			IDC_SHOWWAV,
			IDC_PLAY,
			IDC_STOP
			};

    CDialog::OnInitDialog();
    set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));
	m_play_sounds=get_cfg(CFG_MM,"PlaySound",1);
	m_play_bmps=get_cfg(CFG_MM,"DisplayBmp",1);
	m_notfound=get_cfg(CFG_MM,"WarnAbsent",0);
	CheckRadioButton(IDC_SIZE0,IDC_SIZE3,IDC_SIZE0+get_cfg(CFG_MM,"BmpSize",0));
	TABULATE_LB(IDC_LIST);
	UPDATE_LB(SoundLst,IDC_LIST);
	m_mmlist.SetTabStops((sizeof(tabs1)/sizeof(int)),tabs1);
	((CButton *)GetDlgItem(IDC_SHOWBMP))->SetCheck(TRUE);
	fill_mm_list(FALSE);
	UpdateData(0);
	return TRUE;
}

// =========================================================================
	void cfg_icon::OnOK()
// =========================================================================
{
int	size=GetCheckedRadioButton(IDC_SIZE0,IDC_SIZE3)-IDC_SIZE0;
	
	UpdateData(1);
	set_cfg(CFG_MM,"PlaySound",m_play_sounds);
	set_cfg(CFG_MM,"DisplayBmp",m_play_bmps);
	set_cfg(CFG_MM,"WarnAbsent",m_notfound);
	set_cfg(CFG_MM,"BmpSize",size);
	gc.sound_enabled=m_play_sounds;
	gc.bmps_enabled=m_play_bmps;
	gc.no_error=m_notfound;
	gc.bmps_size=size;
	SoundLst=m_list;
	save_soundlist(SoundLst);
	if(m_bCloseOnOk)
		CDialog::OnOK();
}

// =========================================================================
	void cfg_icon::OnDefault()
// =========================================================================
{
CString str;
CStrList lst;
	
	str.LoadString(IDS_DEFSOUNDS);
	lst.LoadFromDelimString(str,'\n');
	UPDATE_LB(lst,IDC_LIST);
}

void cfg_icon::OnTestsound() 
{
	UpdateData(1);
	test_sound(m_wavpath);
}

void cfg_icon::OnStopsound() 
{
	stop_test_sound();
}

void cfg_icon::OnChange() 
{
CString id,str;
int		sel;
	
	GET_SELID(IDC_LIST);
	UpdateData(1);
	m_list.GetText(sel,id);
	get_token(id,0,str);
	str=str+'\t'+(m_wavpath.IsEmpty() ? "NONE" : m_wavpath);
	m_list.DeleteString(sel);
	m_list.InsertString(sel,str);
	m_list.SetCurSel(sel);
}

void cfg_icon::OnWavebr() 
{
CString	str;
int		ret;
	
	str.LoadString(IDS_SOUNDFILT);
	CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY,str);
	ret=dlg.DoModal();
	restore_base_path();
	if (ret!=IDOK)	return;
	m_wavpath=dlg.GetPathName();
	UpdateData(0);
}

void cfg_icon::OnSelchangeList() 
{
CString id,str;
int		sel;
	
	GET_SELID(IDC_LIST);
	m_list.GetText(sel,id);
	get_token(id,1,str);
	m_wavpath=str;
	UpdateData(0);
}

void cfg_icon::OnBnClickedShowbmp()
{
	fill_mm_list(FALSE);
	GetDlgItem(IDC_PLAY)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STOP)->ShowWindow(SW_HIDE);
}

void cfg_icon::OnBnClickedShowwav()
{
	m_bmp.SetBitmap(NULL);
	fill_mm_list(TRUE);
	GetDlgItem(IDC_PLAY)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_STOP)->ShowWindow(SW_SHOW);
}

void cfg_icon::fill_mm_list(BOOL mode)
{
int		i,n,ret;
FILE	*fp;
char	buf[300],tmp[MAX_PATH],name[300],addr[300];
CString	help1,help2;
CStrList files,infs,desc;

	make_path(tmp,gc.MultimedPath,"*.inf");
	infs.RemoveAll();
	files.FillWithFiles(tmp);
	for (i=0;i<files.GetCount();i++)
	{
		make_path(tmp,gc.MultimedPath,files.GetString(i));
		fp=fopen(tmp,"rt");
		if (fp)
		{
			while (fgets(buf,299,fp))
			{
				if (strchr(" ;/",buf[0]) || strlen(buf)<4)	continue;
				infs.AddTail(buf);
			}
			fclose(fp);
		}
	}

	desc.RemoveAll();
	make_path(tmp,gc.MultimedPath,mode ? "*.wav":"*.bmp");
	files.FillWithFiles(tmp);
	for (i=0;i<files.GetCount();i++)
	{
		help1=files.GetString(i);
		for (n=0;n<infs.GetCount();n++)
		{
			help2=infs.GetString(n);
			if (!strnicmp(help1,help2,help1.GetLength()))
			{
				*name=*addr=0;
				ret=sscanf(help2,"%[^,]%[, ]%[^,]%[ ,]%[^\n,]",tmp,tmp,name,tmp,addr);
				if (ret!=5)
					break;
				sprintf(buf,"%s\t%s\t%s",help1.MakeUpper(),name,addr);
				desc.AddTail(buf);
				break;
			}
		}
	}
	UPDATE_LB(desc,IDC_MMLIST);
}

void cfg_icon::OnLbnSelchangeMmlist()
{
HBITMAP hbmp;
CString str,fname;
char	path[MAX_PATH];
int		sel;

	GET_SELID(IDC_MMLIST);
	m_mmlist.GetText(sel,str);
	get_token(str,0,fname);
	make_path(path,gc.MultimedPath,fname);
	if (((CButton *)GetDlgItem(IDC_SHOWBMP))->GetCheck())
	{
		hbmp=(HBITMAP)LoadImage(NULL,path,IMAGE_BITMAP,110,110,LR_LOADFROMFILE);
		m_bmp.SetBitmap(hbmp);
	}
	else
		test_sound(path);
}

void cfg_icon::OnBnClickedStop()
{
	stop_test_sound();
}

void cfg_icon::OnBnClickedPlay()
{
CString str,fname;
char	path[MAX_PATH];
int		sel;

	GET_SELID(IDC_MMLIST);
	m_mmlist.GetText(sel,str);
	get_token(str,0,fname);
	make_path(path,gc.MultimedPath,fname);
	test_sound(path);
}
