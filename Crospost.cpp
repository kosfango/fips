// crospost.cpp : implementation file
// IDD_CROSSPOST

#include "stdafx.h"
#include "crospost.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

static char DlgName[]="IDD_CROSSPOST";

void remove_kludges(char *text);

// ================================================================
	crospost::crospost(CWnd* pParent ) : CDialog(crospost::IDD, pParent)
// ================================================================
{
	//{{AFX_DATA_INIT(crospost)
		// NOTE: the ClassWizard will add member initialization here
	m_crosstyle=1;
	//}}AFX_DATA_INIT
}

// ================================================================
	void crospost::DoDataExchange(CDataExchange* pDX)
// ================================================================
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(crospost)
	DDX_Control(pDX, IDC_LIST, m_list);
	DDX_Radio(pDX, IDC_UNMODIFIED, m_crosstyle);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(crospost, CDialog)
	//{{AFX_MSG_MAP(crospost)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// ================================================================
	BOOL crospost::OnInitDialog()
// ================================================================
{
areadef ad;
int	 i;
char buf[300];
int  lng[]={
			IDC_UNMODIFIED,
			IDC_FIPSSTYLE,
			IDC_GEDSTYLE,
			IDC_STATIC1,
			IDC_STATIC2,
			IDC_STATIC3,
			IDC_STATIC4,
			IDCANCEL,
			IDOK,
			IDHELP
			};

	CDialog::OnInitDialog();
    set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));
	m_list.SetTabStops(120);
	EXTENT_LB(IDC_LIST,400);

	CrossPostHeader.Empty();
	CrossPostTail.Empty();
	m_crosstyle=get_cfg(CFG_EDIT,"CrossStyle",1);

	i=2;
	while (db_get_area_by_index(i,&ad)==DB_OK)
	{
		if (!ad.localmail && strcmp(ad.group,ASAVED_GROUP))
		{
			sprintf(buf,"%s\t%s\t%d",ad.echotag,ad.description,i);
			m_list.AddString(buf);
		}
		i++;
	}
	UpdateData(0);
	m_list.SetFocus();
	return TRUE;
}

// ================================================================
	void crospost::OnCancel()
// ================================================================
{
	CDialog::OnCancel();
}

// ================================================================
	void crospost::OnOK()
// ================================================================
{
areadef ad;
mailheader mh;
CString str,str1,exkl,tline;
char	msgbuf[MAX_MSG+1000],newmail[MAX_MSG+1000],buf[100],boss[50],point[50];
int		i;
int		count=0;

    hcount=m_list.GetSelItems(999,entry);
	if (hcount < 1)	ERR_MSG_RET("W_MSATLODA");
	if (err_out("DN_RESYWTCP") != IDYES)	return;

	UpdateData(1);
// get handles of selected areas
	for (i=0;i<hcount;i++)
	{
		m_list.GetText(entry[i],str);
		get_token(str,2,str1);
		sscanf(str1,"%d",&handles[i]);
	}
	db_get_mailtext(gustat.ar2han,&gustat.act_mailh,msgbuf,MAX_MSG);
	remove_kludges(msgbuf);
// generate extra kludges
	exkl=get_extrakludges(FALSE);
	exkl.AnsiToOem();
	get_tearline(tline,TRUE);

	for (i=0;i<hcount;i++)
	{
		BuildCrossPostHeader(gustat.ar2han);
		BuildCrossPostTail(i);
// build crosspost mailtext
		db_get_uplink_for_area(handles[i],boss,point);
		str.Format("\001MSGID: %s %x\r%s\001PID: %s\r",point,time(NULL),exkl,get_versioninfo(buf,0));
		str+=CrossPostHeader+msgbuf+CrossPostTail+tline;
		strcpy(newmail,str);
		db_get_area_by_index(handles[i],&ad);
		get_origin(&ad,TRUE,str,TRUE);
		sprintf(buf,"\r * Origin: %s (%s)\r",str,point);
		strcat(newmail,buf);
// build mailheader as a modified copy of current mailheader (!)
		mh=gustat.act_mailh;
		sscanf(point,"%hu:%hu/%hu.%hu",&mh.srczone,&mh.srcnet,&mh.srcnode,&mh.srcpoint);
		get_fullname(str);	// set my initials as sender
		CharToOem(str,mh.fromname);
		strcpy(mh.toname,"All");
		mh.recipttime=mh.unixtime=time(NULL);
		unix_time_to_fido(mh.unixtime,buf);
		strcpy(mh.datetime,buf);
		mh.mail_text=newmail;
		mh.text_length=strlen(newmail)+1;
		mh.status=DB_MAIL_CREATED;		// set New Mail
		mh.structlen=sizeof(mailheader);

// verify areahandle exist
		//db_open_simple(handles[i]);
		if (db_append_new_message(handles[i],&mh)!=DB_OK)	err_out("E_NEWMAILF");
		if (db_set_area_state(handles[i],1)!=DB_OK)			err_out("E_MODFLAGSET");
		count++;
	}
	set_cfg(CFG_EDIT,"CrossStyle",m_crosstyle);
	show_msg(L("S_316",count));
	CDialog::OnOK();
}

// ================================================================
	void crospost::BuildCrossPostHeader(long areaind)
// ================================================================
{
CString name,aka,recp;
areadef ad;
int		ret;

	CrossPostHeader.Empty();
	if (m_crosstyle!=1)	return;
	ret=db_get_area_by_index(areaind,&ad);
	ASSERT(ret);
	get_fullname(name);
	make_address(aka,gustat.act_mailh.srczone,gustat.act_mailh.srcnet,
					gustat.act_mailh.srcnode,gustat.act_mailh.srcpoint);
	recp=gustat.act_mailh.fromname;
	recp.OemToAnsi();
	CrossPostHeader=
		"====================<Crosspost Summary>==========================\r";
	CrossPostHeader+=
		"| Crossposted by "+name+" ("+ad.aka+")\r"+
		"| Origin Area: "+ad.echotag+" ("+ad.description+")\r"+
		"| Original mail was from "+recp+" ("+aka+")\r"+
		"====================< Begin Crosspost >==========================\r";
	CrossPostHeader.AnsiToOem();
}

// ================================================================
	void crospost::BuildCrossPostTail(long areaind)
// ================================================================
{
CString	str;
CString area;

	CrossPostTail.Empty();
	if (m_crosstyle==0)	return;
	if (m_crosstyle==1)
		CrossPostTail="======================< End Crosspost >==========================\r";
	else
	{
		CrossPostTail=CrossPostTail+"| Originally in "+gustat.act_area.echotag+'\r';
		for (int i=0;i<hcount;i++)
		{
			m_list.GetText(entry[i],str);
			get_token(str,0,area);
			CrossPostTail+="| Crossposted in "+area+'\r';
		}
	}
}

// ================================================================
// removes kludges, tearline and origin
	void remove_kludges(char *text)//,int areaind)
// ================================================================
{
char *p,*t,*s;

	t=p=text;
	while (*p)
	{
		if (*p=='\001')	
		{
			do p++; while(*p && *p!='\r');	// skip kludge line
			if (*p=='\r')	p++;
			continue;
		}
		if (!strncmp(p,"---",3) && (!*(p+3) || *(p+3)==' ' || *(p+3)=='\r'))	break;
		s=p;
		while(*p && *p!='\r')	p++;
		if (*p=='\r')	p++;
		strncpy(t,s,p-s);
		t+=p-s;
	}
	*t=0;
}

// ================================================================
	void crospost::OnHelp()
// ================================================================
{
	WinHelp(VHELP_CROSSPOST);
}

BOOL crospost::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return TRUE;
}
