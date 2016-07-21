// insmm.cpp : implementation file
// IDD_MM_SELECTION

#include "stdafx.h"
#include <io.h>
#include "insmm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int   radio_wave=0;
int   bmp_wave=0;
int	  WhenToPlay=2;			  // 1=Line , 2=Mail
CStrList mmstuff;
CStrList infolist;
extern CString	MM_Element;
extern _gconfig gc;

#define IDTOCOD(n)	(n==0 ? 2 : (n==1 ? 1 : 4))
static char DlgName []="IDD_MM_SELECTION";

// =====================================================================
	insmm::insmm(CWnd* pParent ) : CDialog(insmm::IDD, pParent)
// =====================================================================
{
	//{{AFX_DATA_INIT(insmm)
	m_mmtype = 0;
	m_mmdisplay = 0;
	//}}AFX_DATA_INIT
}

// =====================================================================
	void insmm::DoDataExchange(CDataExchange* pDX)
// =====================================================================
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(insmm)
	DDX_Control(pDX, IDC_LIST, m_mm_list);
	DDX_Radio(pDX, IDC_BMPTYPE, m_mmtype);
	DDX_Radio(pDX, IDC_PLAYMAIL, m_mmdisplay);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(insmm, CDialog)
	//{{AFX_MSG_MAP(insmm)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_BN_CLICKED(IDC_WAVTYPE, OnRadioWave)
	ON_BN_CLICKED(IDC_BMPTYPE, OnRadioBmps)
	ON_BN_CLICKED(IDTEST, OnTest)
	ON_LBN_DBLCLK(IDC_LIST, OnDblclkList)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// =====================================================================
	BOOL insmm::OnInitDialog()
// =====================================================================
{
int	 tabs[]={75,180};
int  lng[]={
			IDC_WAVTYPE,
			IDOK,
			IDCANCEL,
			IDHELP,
			IDC_STATIC1,
			IDC_STATIC2,
			IDTEST,
			IDC_STATIC4,
			IDC_PLAYMAIL,
			IDC_PLAYLINE,
			IDC_BMPTYPE,
			IDC_PLAYDBLCLK,
			};

	CDialog::OnInitDialog();
  set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));

	MM_Element.Empty();
	infolist.RemoveAll();
	if (gc.disable_insmm_items)
	{
		DISABLEID(IDOK);
		DISABLEID(IDC_PLAYMAIL);
		DISABLEID(IDC_PLAYLINE);
		DISABLEID(IDC_PLAYDBLCLK);
	}

	TABULATE_LB(IDC_LIST);
	mmstuff.RemoveAll();
	UPDATE_LB(mmstuff,IDC_LIST);

	OnRadioBmps();
	UpdateData(0);
	return TRUE;
}

// =====================================================================
	void insmm::OnCancel()
// =====================================================================
{
	infolist.RemoveAll();
	CDialog::OnCancel();
}

// =====================================================================
	void insmm::OnOK()
// =====================================================================
{
int		sel;
CString line;
CString mmfile;
char	buf[300];
char	*p;

	UpdateData(1);
	sel=m_mm_list.GetCurSel();
	if (sel==LB_ERR)
		ERR_MSG_RET("E_PLSAME");

	line=mmstuff.GetString(sel);
	get_token(line,0,mmfile);
	strcpy(buf,mmfile);
	p=strchr(buf,'.');
	if (!p)
		ERR_MSG_RET("E_ILLMML");

	*p=0;
	p=buf;
//	while (*p && *p=='0')	// skip begining zeroes
//		p++;

	MM_Element.Format("%s%s%d%s",MM_PREFIX,m_mmtype==0 ? "B":"W",IDTOCOD(m_mmdisplay),p);
	infolist.RemoveAll();
	CDialog::OnOK();
}

// =====================================================================
	void insmm::OnRadioWave()
// =====================================================================
{
	DisplayNewTypeList();
}

// =====================================================================
	void insmm::OnRadioBmps()
// =====================================================================
{
	DisplayNewTypeList();
}

// =====================================================================
	void insmm::OnTest()
// =====================================================================
{
CString fname;
char	mmdir[300];
int		sel;

	UpdateData(1);
	GET_SELID(IDC_LIST);
	get_token(mmstuff.GetString(sel),0,fname);
	make_path(mmdir,gc.MultimedPath,fname);
	if (m_mmtype==0)
	{
		display_bitmap(mmdir);
		force_bmp_display_update();
	}
	else
		test_sound(mmdir);
}

// =====================================================================
	void insmm::DisplayNewTypeList(void)
// =====================================================================
// Format of .INF-files: 00000001.WAV,name & surname,fido address
// old: 00000001.WAV,deutsch description,englisch description
{
int		i,n,ret;
FILE	*fp;
char	buf[300];
char	tmp[300];
char	name[300];
char	addr[300];
CString	help1;
CString	help2;
CStrList inffiles;
CStrList mmfiles;

	UpdateData(1);
	make_path(tmp,gc.MultimedPath,"*.inf");
	infolist.RemoveAll();
	inffiles.FillWithFiles(tmp);
	for (i=0;i<inffiles.GetCount();i++)
	{
		make_path(tmp,gc.MultimedPath,inffiles.GetString(i));
		fp=fopen(tmp,"rt");
		if (fp)
		{
			while (fgets(buf,299,fp))
			{
				if (strchr(" ;/",buf[0]) || strlen(buf)<4)
					continue;
				infolist.AddTail(buf);
			}
			fclose(fp);
		}
	}

	make_path(tmp,gc.MultimedPath,m_mmtype==0 ? "*.bmp":"*.wav");
	mmstuff.RemoveAll();
	mmfiles.FillWithFiles(tmp);
	for (i=0;i<mmfiles.GetCount();i++)
	{
		help1=mmfiles.GetString(i);
		for (n=0;n<infolist.GetCount();n++)
		{
			help2=infolist.GetString(n);
			if (!strnicmp(help1,help2,help1.GetLength()))
			{
				name[0]=addr[0]=0;
				ret=sscanf(help2,"%[^,]%[, ]%[^,]%[ ,]%[^\n,]",tmp,tmp,name,tmp,addr);
				if (ret!=5)
					break;
				sprintf(buf,"%s\t%s\t%s",help1,name,addr);
				mmstuff.AddTail(buf);
				break;
			}
		}
	}
	UPDATE_LB(mmstuff,IDC_LIST);
}

// =====================================================================
	void insmm::OnDblclkList()
// =====================================================================
{
	OnOK();
}

// =====================================================================
	void handle_mm_stuff(char *mailbuf,int fromwhere)
// =====================================================================
{
static int preflen=strlen(MM_PREFIX);
char	*p,*t;
char	name[300];
char	mmcod[MAX_PATH],mmbase[MAX_PATH];

	if (GetKeyState(VK_CONTROL) & 0x80000000)
		return;

// WAVES
// Format: 	MM_PREFIX|mmtype|whentoplay|name
	p=mailbuf;
handle_wav:
	if ((gc.sound_enabled && !gc.sound_already_done) || fromwhere==3)
	{
find_ag1:
		p=strstr(p,MM_PREFIX);
		if (p)
		{
			p+=preflen;
			switch (*p)
			{
				case 'W':
				case 'w':
					break;
				case 'B':
				case 'b':
					if ((gc.bmps_enabled && !gc.bmps_already_done) || fromwhere==3)
					{
						p-=preflen;
						goto handle_bmp;
					}
					goto find_ag1;
				case '\0':
					return;
				default:
					goto find_ag1;
			}

			p++;
			if (fromwhere!=3)
			{
				switch (*p)	// 1-Play on display , 2-Play on income
				{
					case '1':
						if (fromwhere!=1)
							goto handle_bmp;
						break;
					case '2':
						if (fromwhere!=2)
							goto handle_bmp;
						break;
					default:
						goto handle_bmp;
				}
			}
			p++;
			name[0]=0;
			sscanf(p,"%s",name);
			if (!name[0] || strlen(name)>8)
				goto handle_bmp;

			t=name;
			while (*t)
			{
				if (!isxdigit(*t))
					goto handle_bmp;
				t++;
			}
			strcpy(mmcod,"00000000");
			memcpy(mmcod+(8-strlen(name)),name,strlen(name));
//			strcpy(mmcod,name);
			strcat(mmcod,".WAV");
			make_path(mmbase,gc.MultimedPath,mmcod);
			if (access(mmbase,0))
			{
				if (gc.no_error)
					err_out("W_CNFMMXF",mmbase);
				goto handle_bmp;
			}
			gc.sound_already_done=1;
			test_sound(mmbase);
		}
		else
			return;
	}

// BITMAPS
// Format: 	MM_PREFIX|mmtype|whentoplay|name with ending NULL
handle_bmp:
	if ((gc.bmps_enabled && !gc.bmps_already_done) || fromwhere==3)
	{
find_ag2:
		p=strstr(p,MM_PREFIX);
		if (p)
		{
			p+=preflen;
			switch (*p)
			{
				case 'W':
				case 'w':
					if ((gc.sound_enabled && !gc.sound_already_done) || fromwhere==3)
					{
						p-=preflen;
						goto handle_wav;
					}
					goto find_ag2;
				case 'B':
				case 'b':
					break;
				case '\0':
					return;
				default:
					goto find_ag2;
			}
			p++;
			if (fromwhere!=3)
			{
				switch (*p)	// 1=Play on display , 2=Play on income
				{
					case '1':
						if (fromwhere!=1)
							goto handle_wav;
						break;
					case '2':
						if (fromwhere!=2)
							goto handle_wav;
						break;
					default:
						goto handle_wav;
				}
			}
			p++;
			name[0]=0;
			sscanf(p,"%s",name);
			if (!name[0] || strlen(name)>8)
				goto handle_wav;

			t=name;
			while (*t)
			{
				if (!isxdigit(*t))
					goto handle_wav;
				t++;
			}
			strcpy(mmcod,"00000000");
			memcpy(mmcod+(8-strlen(name)),name,strlen(name));
			strcpy(mmcod,name);
			strcat(mmcod,".BMP");
			make_path(mmbase,gc.MultimedPath,mmcod);
			if (access(mmbase,0))
			{
				if (gc.no_error)
					err_out("W_CNFMMXF",mmbase);
				goto handle_wav;
			}
			gc.bmps_already_done=1;
			display_bitmap(mmbase);
		}
		else
			return;
		
		goto handle_wav;
	}
}

// =====================================================================
	void insmm::OnHelp()
// =====================================================================
{
	WinHelp(VHELP_MM_SELECTION);
}

BOOL insmm::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return TRUE;
}
