// filereq.cpp : implementation file
// IDD_FREQ

#include "stdafx.h"
#include <io.h>
#include "cfido.h"
#include "detmail.h"
#include "sel_freq.h"
#include "cfg_nfll.h"
#include "info_adr.h"
#include "events.h"
#include "cfgexfr.h"
#include "filereq.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

static int grepsearch=0;

void add_dir_info(CStrList &dir);

CStrList nodes;
CStrList edit;

CStrList filelists;

extern CStrList exfrcfg;
extern _gconfig gc;
extern CString StoredFido;
extern CString StoredFile;
extern CString fromfilelist;

extern int  rename_to_short_filenames(const char *path,CStrList &files,CStrList &renamelist);

int		gWeComeFromOtherLines=0;

CString gAdressBeforeOtherLines;
CString gSystemBeforeOtherLines;
CString gFileNameBeforeOtherLines;

static _DlgItemsSize DlgItemsSize[]=
{
	0,			     {0,0,0,0},{0,0,0,0},0,0,0,0,0,
	IDC_STATIC4,	     {0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_STATIC5,	     {0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_ADDRESS,	     {0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_STATIC8,	     {0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_PASSWORD,	     {0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_STATIC6,	     {0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_SYSTEMNAME,	     {0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_STATIC7,	     {0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_FILENAME,	     {0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_SCHEDULE,	     {0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_ADD,	         {0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_DELETE,	         {0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_INFO_ADDRESS,	 {0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_PATTERNSEARCH,	 {0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_INFO_OTHERLINES, {0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDHELP,	     {0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDOK,	             {0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_EXT_TOOL,	     {0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_EXT_TOOL_CFG,    {0,0,0,0},{0,0,0,0},VE,VE,VE,VE,0,
	IDC_FIDO,	         {0,0,0,0},{0,0,0,0},0,0,0,VE,0,
	IDC_REQUEST,	     {0,0,0,0},{0,0,0,0},0,0,0,VE,0,
	IDC_FILELIST,	     {0,0,0,0},{0,0,0,0},0,0,HO|VE,HO|VE,0,
};
static char DlgName[]="IDD_FREQ";

// ===========================================================================
	filereq::filereq(CWnd* pParent ) : CDialog(filereq::IDD, pParent)
// ===========================================================================
{
	//{{AFX_DATA_INIT(filereq)
	//}}AFX_DATA_INIT
}

// ===========================================================================
	void filereq::DoDataExchange(CDataExchange* pDX)
// ===========================================================================
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(filereq)
	DDX_Control(pDX, IDC_FILELIST, m_filelist);
	DDX_Control(pDX, IDC_SYSTEMNAME, m_systemname);
	DDX_Control(pDX, IDC_PASSWORD, m_password);
	DDX_Control(pDX, IDC_FILENAME, m_filename);
	DDX_Control(pDX, IDC_ADDRESS, m_address);
	DDX_Control(pDX, IDC_REQUEST, m_list_request);
	DDX_Control(pDX, IDC_FIDO, m_list_fido);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(filereq, CDialog)
	//{{AFX_MSG_MAP(filereq)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_LBN_SELCHANGE(IDC_FIDO, OnSelchangeFido)
	ON_EN_KILLFOCUS(IDC_ADDRESS, OnKillfocusAddress)
	ON_LBN_DBLCLK(IDC_FILELIST, OnDblclkFilelist)
	ON_BN_CLICKED(IDC_BUTTON1, OnAddNewFileList)
	ON_BN_CLICKED(IDC_BUTTON2, OnDeleteFileList)
	ON_EN_UPDATE(IDC_ADDRESS, OnUpdateAddress)
	ON_BN_CLICKED(IDC_INFO_ADDRESS, OnInfoAddress)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_BN_CLICKED(IDC_SCHEDULE, OnSchedule)
	ON_BN_CLICKED(IDC_EXT_TOOL, OnExternalTool)
	ON_BN_CLICKED(IDC_EXT_TOOL_CFG, OnExternalToolConfig)
	ON_BN_CLICKED(IDC_INFO_OTHERLINES, OnInfoOtherlines)
	ON_BN_CLICKED(IDC_PATTERNSEARCH, OnPatternsearch)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_LBN_SELCHANGE(IDC_FILELIST, OnSelchangeFilelist)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// ===========================================================================
	BOOL filereq::OnInitDialog()
// ===========================================================================
{
int		tabs[]={70,110,140};
CStrList lst;
int		lng[]={
				IDC_ADD,
				IDC_DELETE,
				IDOK,
				IDC_STATIC5,
				IDC_STATIC7,
				IDC_STATIC4,
				IDC_STATIC1,
				IDC_STATIC2,
				IDC_STATIC8,
				IDC_STATIC6,
				IDC_STATIC3,
				IDC_BUTTON1,
				IDC_BUTTON2,
				IDC_INFO_ADDRESS,
				IDC_SCHEDULE,
				IDC_INFO_OTHERLINES,
				IDC_PATTERNSEARCH,
				IDC_EXT_TOOL,
				IDC_EXT_TOOL_CFG,
				IDHELP
				};

	CDialog::OnInitDialog();
    set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));
    StoreOrgItemSizes(DlgName,this,DlgItemsSize,sizeof(DlgItemsSize)/sizeof(_DlgItemsSize));

	EXTENT_LB(IDC_FILELIST,600);
	TABULATE_LB(IDC_FILELIST);

	rename_back_to_long_filenames(gc.UtilPath);

	lst.FillWithDirs(gc.OutboundPath);
	lst.DeleteEmptySubDirs(gc.OutboundPath);

	Rescan();
	m_list_fido.SetFocus();
	m_list_fido.SetCurSel(0);
	OnSelchangeFido();   // Update the first one

	if (StoredFido.GetLength()>0)
	{
		m_address.SetWindowText(StoredFido);

		if (StoredFile.GetLength()>0)
			m_filename.SetWindowText(StoredFile);

		UpdateData(0);
	}
	dispay_systems_with_filelists();
 	return TRUE;
}

// ===========================================================================
	void filereq::OnAdd()
// ===========================================================================
{
CStrList splitted;
FILE	*fp;
CFido   cfid;
CString fido,dir,editline,password,oneline,bossadress;
CString names[100];
char	fname[MAX_PATH],dm[1000];
int     ret;

	m_address.GetWindowText(fido);
    trim_all(fido);

	m_filename.GetWindowText(editline);
	m_password.GetWindowText(password);
    trim_all(password);
	if (password.GetLength()>0 && password[0]!='!')
		password="!"+password;

	if (fido.GetLength()<3 || editline.GetLength()<1)
	{
		ERR_MSG_RET("E_FILLBOTH");
		added=FALSE;
	}

	// Test for correct address	or telephone number
	if (fido[0]!='#')
	{
		ret=parse_address(fido,&ret,&ret,&ret,&ret);
		if (ret!=3 && ret!=4)
		{
			ERR_MSG2_RET("E_NOVALIDADR",fido);
			added=FALSE;
		}
	}
	cfid=fido;
	if (!cfid.GetPhoneAndPwd(dm,dm))
	{
		ERR_MSG2_RET("E_NOVALIDADR",fido);
		added=FALSE;
	}

	dir=cfid.GetAsDir();
	make_path(fname,gc.OutboundPath,dir);
	if (access(fname,0))
	   CreateDirectory(fname,0);

	strcat(fname,"\\" FREQ_LIST);
	if (fp=fopen(fname,"at"))
	{
	   splitted.SplitWords(editline);
	   for (int i=0;i<splitted.GetCount();i++)
	   {
			editline=splitted.GetString(i);
			oneline=editline+" "+password;
			fprintf(fp,"%s\n",oneline);
	   }
	   fclose(fp);
	}
	Rescan();
	m_list_fido.SetCurSel(0);
	m_list_fido.SetCurSel(m_list_fido.FindString(-1,fido));
	OnSelchangeFido();                  // Update the first one
	added=TRUE;
}

// ===========================================================================
	void filereq::OnDelete()
// ===========================================================================
{
CString actdir;
CFido	cf;
char	fname[MAX_PATH];
int		sel_file,sel_node,helpsel;

	 sel_file=m_list_request.GetCurSel();
	 sel_node=m_list_fido.GetCurSel();
	 if (sel_node==LB_ERR)
	    return ;

	 if (sel_file!=LB_ERR)					// Selection was a request entry
	 {
		m_list_fido.GetText(sel_node,actdir);
		cf = PS actdir;	 
		actdir=cf.GetAsDir();
		make_path(fname,gc.OutboundPath,actdir);
		strcat(fname,"\\" FREQ_LIST);
		edit.Remove(sel_file);
		edit.SaveAsEdit(fname);
		helpsel=sel_node;
	 }
	 else									// Selection was a node
	 {
		m_list_fido.GetText(sel_node,actdir);
		cf = PS actdir;	 
		actdir=cf.GetAsDir();
		make_path(fname,gc.OutboundPath,actdir);
		strcat(fname,"\\" FREQ_LIST);
		SafeFileDelete(fname);
		helpsel=0;
	 }
	 Rescan();
	 m_list_fido.SetCurSel(helpsel);
	 OnSelchangeFido();   // Update the first one
}

// ===========================================================================
// if Mailer is not running and are requests then push Rescan Button
	void filereq::OnOK()
// ===========================================================================
{
	if (!gc.mailer.running && m_list_fido.GetCount()>0 &&
		err_out("DY_DORESCAN")==IDYES)
	{
  		show_msg("Rescanning mailer queue...","Сканирование очереди мэйлера...");
		((detmail *)gc.mailer.thisptr)->OnRescan();
	}
    StoreInitDialogSize(DlgName,this);
	CDialog::OnOK();
}

// ===========================================================================
	void filereq::dispay_systems_with_filelists(void)
// ===========================================================================
{
char path[MAX_PATH];

	filelists.RemoveAll();
	make_path(path,gc.UtilPath,"\\*.????list");   // can be .filelist and .newflist
	filelists.FillWithFiles(path);
	filelists.AppendFileInfo(gc.UtilPath);
	add_dir_info(filelists);
	UPDATE_LB(filelists,IDC_FILELIST);
}

// ===========================================================================
	void filereq::OnSelchangeFido()
// ===========================================================================
{
CString store;
CFido	cf;
char	path[MAX_PATH],fname[MAX_PATH];
int		sel;

	sel=m_list_fido.GetCurSel();
	LB_ERR_RET;
	m_list_fido.GetText(sel,path);
	store=path;
	cf=PS path;
	make_path(path,cf.GetAsDir(),FREQ_LIST);
	make_path(fname,gc.OutboundPath,path);
	edit.RemoveAll();
	edit.FillAsEdit(fname);
	UPDATE_LB(edit,IDC_REQUEST);
	m_address.SetWindowText(store);
	OnKillfocusAddress();
}

// ===========================================================================
	void filereq::Rescan()
// ===========================================================================
{
CStrList lst;
CFido	cf;
char	path[MAX_PATH],fname[MAX_PATH],buf[1000];
int		i;

	m_list_fido.ResetContent();
	m_list_request.ResetContent();

	nodes.RemoveAll();
	lst.RemoveAll();
	lst.FillWithDirs(gc.OutboundPath);
	for (i=0;i<lst.GetCount();i++)
	{
		strcpy(path,lst.GetString(i));
		make_path(buf,path,FREQ_LIST);
		make_path(fname,gc.OutboundPath,buf);
		if (!access(fname,0))
		{
		   cf=path;
		   cf.Get(buf);
		   nodes.AddTail(buf);
		}
	}

	nodes.UpdateListBox(&m_list_fido);
	m_list_fido.SetFocus();
	OnSelchangeFido();   // Update the first one
}

// ===============================================================
	void filereq::expand_fido_adress()
// ===============================================================
{
CString fido,addr;
char    bossinfo[MAX_BOSSLEN],buf[30];
int     zone,net,node;

	m_address.GetWindowText(fido);
    trim_all(fido);

	if (is_address(PS fido))
	{
		if (db_get_boss_by_index(0,bossinfo))
		{
			get_token(bossinfo,BOSSADDR,addr);
			parse_address(addr,&zone,&net,&node);

			if (!strchr(fido,'/'))
			{
				sprintf(buf,"%d:%d/%s",zone,net,fido);
				fido=buf;
			}
			else if (!strchr(fido,':'))
			{
				sprintf(buf,"%d:%s",zone,fido);
				fido=buf;
			}
		}
	}
  	m_address.SetWindowText(fido);
}

// ===============================================================
	void filereq::OnDblclkFilelist()
// ===============================================================
{
char buf[1000],*p;
int sel;

	 sel=m_filelist.GetCurSel();
	 LB_ERR_RET;
	 m_filelist.GetText(sel,buf);
	 sscanf(buf,"%s",gc.filelist);
	 p=strchr(buf,'.');
	 if (!p)	return;
	 *p=0;
	 sel_freq dlg;
	 dlg.DoModal();
	 m_filename.SetWindowText(dlg.freq);
	 if (p=strchr(buf,'_'))	*p=':';
	 if (p=strchr(buf,'_'))	*p='/';
	 if (p=strchr(buf,'_'))	*p='.';
	 m_address.SetWindowText(buf);
	 OnKillfocusAddress();
}

// ===============================================================
	void filereq::OnAddNewFileList()
// ===============================================================
{
	cfg_nfll dlg;
	dlg.DoModal();
	dispay_systems_with_filelists();
}

// ===============================================================
	void filereq::OnDeleteFileList()
// ===============================================================
{
char	path[MAX_PATH],tmp[MAX_PATH];
int		sel;

	sel=m_filelist.GetCurSel();
	LB_ERR_RET;
	m_filelist.GetText(sel,path);
	sscanf(path,"%s",tmp);
	make_path(path,gc.UtilPath,tmp);
	if (err_out("DY_REALLYDEL",path)==IDYES)
	{
	  SafeFileDelete(path);
		dispay_systems_with_filelists();
	}
}

// ===============================================================
	void filereq::OnKillfocusAddress()
// ===============================================================
{
CString tmp,inc,full,info;
int		ret,err;
char	*p;

	if (GetFocus()==GetDlgItem(IDC_PATTERNSEARCH) || GetFocus()==GetDlgItem(IDC_INFO_ADDRESS))
		return;

	m_address.GetWindowText(tmp);
  trim_all(tmp);
	if (tmp.IsEmpty())
	{
		gWeComeFromOtherLines=0;
		return;
	}
	m_systemname.SetWindowText(NULL);

	inc=tmp;
	ret=expand_address(tmp,full,info,1,1,1,err);
	if (ret)
	{
		m_address.SetWindowText(full);
		tmp=info;
		p= (char *) strchr(info,',');
		if (p)	p=strchr(p+1,',');
		if (p)	tmp=info.Mid(0,p-info)+"\r\n"+info.Mid(p-info+1);
		m_systemname.SetWindowText(tmp);
	}
	else
	{
		if (gWeComeFromOtherLines)
		{
			m_address.SetWindowText(gAdressBeforeOtherLines);
			m_systemname.SetWindowText(gSystemBeforeOtherLines);
			m_filename.SetWindowText(gFileNameBeforeOtherLines);
		}
		else
			m_systemname.SetWindowText(L("S_78"));	// no additional info

		gWeComeFromOtherLines=0;
	}
}

// ===============================================================
	void filereq::OnUpdateAddress()
// ===============================================================
{
	m_address.GetWindowText(gc.AddressForInfo,sizeof(gc.AddressForInfo)-1);
}

// ===============================================================
	void filereq::OnInfoAddress()
// ===============================================================
{
	info_adr dlg;
	dlg.DoModal();
}

// ========================================================================
    void add_dir_info(CStrList &dir)
// ========================================================================
// TOP: diese Funktion geht von einer List mit fiolgendem Format aus:
//		2_2494_17.filelist
//
{
_fidonumber result;
CStrList	tmpx;
CString		systeminfo;
char		tmp[3000],systmp[3000];
int			zone,net,node,point;
int			i,k,ret;

	tmpx.RemoveAll();
	for(i=0;i<dir.GetCount();i++)
	{

       zone=net=node=point=0;
	   strcpy(tmp,dir.GetString(i));
	   for (k=0;k<(int)strlen(tmp);k++)
	   {
		   if (tmp[k]=='_' || tmp[k]=='.')
		      tmp[k]=' ';
	   }
	   ret=sscanf(tmp,"%d %d %d %d",&zone,&net,&node,&point);
	   if (ret >=3)
	   {
			systeminfo.Empty();
			ret=nl_get_fido_by_number(zone,net,node,point,&result);
			if (ret)
			{
			    systeminfo="  \t";
			    systeminfo=systeminfo+result.user+","+result.phone+","+result.flags;
			}
	   }

	   // Kill eine eventuell angehaengets Newline
	   int lenxx;
	   strcpy(systmp,systeminfo);
	   lenxx=strlen(systmp);
	   if (lenxx>0 && systmp[lenxx-1]=='\n')	systmp[lenxx-1]=0;
	   systeminfo=systmp;
	   tmpx.AddTail(dir.GetString(i)+systeminfo);
	}

	dir.RemoveAll();
	for(i=0;i<tmpx.GetCount();i++)
	   dir.AddTail(tmpx.GetString(i));
}

// ========================================================================
	void filereq::OnSchedule()
// ========================================================================
{
CString str;

	OnAdd();
	if (!added)	return;
	events dlg;
	m_address.GetWindowText(str);
    trim_all(str);
	strcpy(dlg.fido_if_scheduled,str);
	dlg.scheduled=1;
	dlg.DoModal();
}

// ========================================================================
	void filereq::OnExternalToolConfig()
// ========================================================================
{
	cfgexfr tmp;
	tmp.DoModal();
	this->SetFocus();
}

// ========================================================================
	void filereq::OnExternalTool()
// ========================================================================
// TOP: Diese Funktion wird aufgerufen falls der Benutzer auf den external Button
// klickt
{
int		i,sel,t;
CStrList renamelist;
char outfile[MAX_PATH];
char infile[MAX_PATH];
char readyfile[MAX_PATH];
char startedfile[MAX_PATH];
CString tool;
CString help1;
CString help3;
CString line;
char	*p;
char	buf[3000];
char	fidosystem[300];
char	files[1000];
int		zone,net,node,point;
int		startval=0;
FILE	*fp;
CString selected_fido;
CString fdd;
CString fdd1;
CString comment;
CString pf;

	if (exfrcfg.GetCount()<2)
		ERR_MSG_RET("E_NOEXTCY");
	make_path(buf,gc.UtilPath,exfrcfg.GetString(0));
	strcat(buf,gc.UtilPath);
	tool=buf;

	make_path(outfile,gc.UtilPath,"exfrtool.out");
	if (!secure_delete(outfile))
		return;

	make_path(infile,gc.UtilPath,"exfrtool.in");
	if (!secure_delete(infile))
		return;

	make_path(readyfile,gc.UtilPath,"exfrtool.rdy");
	if (!secure_delete(readyfile))
		return;

	make_path(startedfile,gc.UtilPath,"exfrtool.sta");
	if (!secure_delete(startedfile))
		return;

	pf=exfrcfg.GetString(1);	// extern tool is 16-Bit, so filenames must be short
	if (pf=="1" && !rename_to_short_filenames(gc.UtilPath,filelists,renamelist))
		return;

	fp=fopen(outfile,"wt");
	if (!fp)
		ERR_MSG2_RET("E_EWWTFX",outfile);
// Format: FIDO_Address fname Systeminfo
	sel= m_filelist.GetCurSel();
	if (pf!="1")				// 16-Bit System ...
	{
		for (i=0;i<filelists.GetCount();i++)
		{
			zone=net=node=point=0;
			line=filelists.GetString(i);
			sscanf(line,"%d_%d_%d_%d",&zone,&net,&node,&point);
			get_token(line,2,comment);
			get_token(line,0,fdd);
			fprintf(fp,i==sel ? "+" : " ");
			fprintf(fp,"%d:%d/%d.%d %s %s\n",zone,net,node,point,fdd,comment);
		}
	}
	else
	{
		for (i=0;i<renamelist.GetCount();i++)
		{
			zone=net=node=point=0;
			line=filelists.GetString(i);
			sscanf(line,"%d_%d_%d_%d",&zone,&net,&node,&point);
			get_token(line,2,comment);
			fdd=renamelist.GetString(i);
			get_token(fdd,1,fdd1);
			fprintf(fp,i==sel ? "+" : " ");
			fprintf(fp,"%d:%d/%d.%d %s %s\n",zone,net,node,point,fdd1,comment);
		}
	}
	fclose(fp);

	startval=WinExec(PS tool,SW_SHOWNORMAL);
	if (startval <=31)
	{
		switch(startval)
		{
			case 0:
					err_out	("E_WNEXECF","Out of Memory");
					goto startFailed;
					break;
											// The system is out of memory or resources.
			case ERROR_BAD_FORMAT:			// The .EXE file is invalid (non-Win32 .EXE or error in .EXE image).
					err_out	("E_WNEXECF","The .EXE file is invalid");
					goto startFailed;
					break;
			case ERROR_FILE_NOT_FOUND:		// The specified file was not found.
					err_out	("E_WNEXECF","The specified file was not found");
					goto startFailed;
					break;
			case ERROR_PATH_NOT_FOUND:		// The specified path was not found.
					err_out	("E_WNEXECF","The specified path was not found");
					goto startFailed;
					break;
			default:						// Unknown reason.
					err_out	("E_WNEXECF","Unknown reason");
					goto startFailed;
					break;
		}
	}
	for (t=0;t<30;t++)
	{
		Sleep(250);
		if(!access(startedfile,0))
			goto startOK;
	}
	err_out("E_EXSTRFD");

startFailed:
	rename_back_to_long_filenames(gc.UtilPath);
	return;

startOK:
	while (1)
	{
		Sleep(250);
		if(access(readyfile,0)==0)
			break;
	}

	rename_back_to_long_filenames(gc.UtilPath);
	fp=fopen(infile,"rt");
	if (!fp)
		return;

	// Lese Ergebisse:
	// line 1 Fido Address
	// line 2 All Files

	while (fgets(buf,299,fp))
	{
		help3=buf;
        trim_all(help3);
		if (help3.GetLength()<2)
			continue;

		fidosystem[0]=0;
		sscanf(buf,"%s",fidosystem);
		files[0]=0;
		fgets(files,999,fp);
		p=strchr(files,'\n');	if (p)	*p=0;
 		help1=files;
        trim_all(help1);
		m_address.SetWindowText(fidosystem);
		m_filename.SetWindowText(help1);
		OnKillfocusAddress();
		OnAdd();
	}
	fclose(fp);
}

// ========================================================================
	void filereq::OnInfoOtherlines()
// ========================================================================
{
CString help;
CString sys;
char    sysname[5000];

	m_systemname.GetWindowText(sys);
	if (sys.GetLength()<2)
		ERR_MSG_RET("W_SYSNFMBS");

	m_address.GetWindowText(gAdressBeforeOtherLines);
	m_systemname.GetWindowText(gSystemBeforeOtherLines);
	m_filename.GetWindowText(gFileNameBeforeOtherLines);

	sscanf(sys,"%[^,]",sysname);
	help=sysname;
    trim_all(help);
	m_address.SetWindowText(help);
	gWeComeFromOtherLines=1;
	OnKillfocusAddress();
}

// ========================================================================
	void filereq::OnPatternsearch()
// ========================================================================
{
CString lfido;
CString full;
CString info;
int		err;

	m_address.GetWindowText(lfido);
    trim_all(lfido);
	if (expand_address(lfido,full,info,1,0,0,err,1,1))
		m_address.SetWindowText(full);

	m_address.SetFocus();
}

// ============================================
	void filereq::OnSize(UINT nType, int cx, int cy)
// ============================================
{
	CDialog::OnSize(nType, cx, cy);
	resize_wnd(this,DlgItemsSize,sizeof(DlgItemsSize));
}

// ============================================
	void filereq::OnPaint()
// ============================================
{
	CPaintDC dc(this);
	DrawRightBottomKnubble(this,dc);
}

// ============================================
	void filereq::OnSelchangeFilelist()
// ============================================
{
CString sys;
char	buf[300];
int		sel;
int		zone,node,net,point;

	sel=m_filelist.GetCurSel();
	LB_ERR_RET;

	m_filelist.GetText(sel,sys);
	zone=node=net=point=0;
	sscanf(sys,"%d_%d_%d_%d",&zone,&node,&net,&point);
	make_address(buf,zone,node,net,point);
	m_address.SetWindowText(buf);
	OnKillfocusAddress();
}

// ========================================================================
	void filereq::OnHelp()
// ========================================================================
{
	WinHelp(VHELP_FREQ);
}

BOOL filereq::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return TRUE;
}
