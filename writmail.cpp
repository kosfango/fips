// writmail.cpp : implementation file
// IDD_WRITMAIL640

#include "stdafx.h"
#include "writmail.h"
#include "cha_tmpl.h"
#include "lightdlg.h"
#include "addrselect.h"
#include "info_adr.h"
#include "txttemp.h"
#include "impwhat.h"
#include "searepl.h"
#include "cuuendec.h"
#include "insmm.h"
#include <io.h>
#include "inafound.h"
#include <afxdisp.h>

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern CFont   	font_mailtext;
extern CStrList gatecfg;
extern CStrList *pmystr;
extern CStrList Abook;
extern CStrList Aliases;
extern CStrList OriginList;
extern CStrList HeaderList;
extern CStrList FooterList;
extern CStrList Akas;
extern CStrList converts;
extern CStrList Agroups;
extern CListBox *m_globedit;
extern _gconfig gc;
extern LangID LANG_ID;

extern CLightDlg *gpMain;
extern CString	fwdstring;
extern CString	gUseThisURL;

_ustat		ust;		// holds the stuff for user-interaction in WRITMAIL   
CStrList	EmailList;
CString		Org_ToName;
CString		Org_ToAddr;
CString		Org_FromAddr;
CString		MM_Element;

int UpHeight;			// up distance to editbox
int BottomHeight;		// down distance from editbox
int forcefocus;

ULONG reply_id;			// Reply id from REPLY kludge
ULONG msg_id;			// Msgid from MSGID kludge
char  msg_addr[100];	// Address from MSGID kludge
char  reply_addr[100];	// Reply address from REPLY kludge
int	  through_gate;		// gate macros was processed?
int	  extra_aka;
int	  changed_fields;	// bit-oriented flag of changed fields

int	header_changed;		// 
int	footer_changed;		// 
int	template_text;		// 
int	forceclose;			// block possible nodelist search when dialog is closing

writmail	*gEditor;
//my experiments
CEditWnd	*gEdit;
//CRichEditCtrl  	*gEdit;
char		*Mailbuf;

void find_emails		(LPSTR text,CStrList &lst);
void remove_kludges		(CString &str);
int	 extract_quotes		(LPCSTR p,CString &text);
void convert_specchars	(CString &str);
void make_quoted_text	(LPCSTR src,CString &dest,LPSTR sender);
BOOL build_address_list	(LPCSTR name,LPCSTR addr,CStrList &lst);

/////////////////////////////////////////////////////////////////////////////
// writmail dialog

static int new_header_index=-1;
static char DlgName[]="IDD_WRITMAIL640";

mailheader	tmh;			// temporary MailHeader struct
static int  cur_header=0;	// current header index
static int  cur_footer=0;	// current footer index
CString     rcp_first;		// interactive recipient name
CString     rcp_second;		// interactive recipient surname
CString     Cur_ToAddr;		// interactive to-address
CString     Cur_Subject;	// current subject

// ================================================================
writmail::writmail(CWnd* pParent /*=NULL*/)
	: CDialog(writmail::IDD, pParent)
// ================================================================
{
	//{{AFX_DATA_INIT(writmail)
	//}}AFX_DATA_INIT
}

writmail::~writmail(){}

// ================================================================
	void writmail::DoDataExchange(CDataExchange* pDX)
// ================================================================
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(writmail)
	DDX_Control(pDX, IDC_ALIASES, m_aliases);
	DDX_Control(pDX, IDC_PRIVATE, m_private);
	DDX_Control(pDX, IDC_CHAR_CONVERSION, m_convert);
	DDX_Control(pDX, IDC_RANGE, m_range);
	DDX_Control(pDX, IDC_FIDODROP, m_fidodrop);
	DDX_Control(pDX, IDC_SEND_DIRECT, m_direct);
	DDX_Control(pDX, IDC_REQUEST_RECIPT, m_recipt);
	DDX_Control(pDX, IDC_KILL_AFTER_SEND, m_killafter);
	DDX_Control(pDX, IDC_ATTACH, m_attach);
	DDX_Control(pDX, IDC_ATTACH_CHK, m_attached);
	DDX_Control(pDX, IDC_FREEZEMAIL, m_freezemail);
	DDX_Control(pDX, IDC_AREATAG, m_area);
	DDX_Control(pDX, IDC_SUBJECT, m_subject);
	DDX_Control(pDX, IDC_TO_FIDO, m_toaddr);
	DDX_Control(pDX, IDC_TO, m_to);
	DDX_Control(pDX, IDC_TMP, m_tmp);
	DDX_Control(pDX, IDC_ORIGIN, m_origin);
	DDX_Control(pDX, IDC_HEADER, m_header);
	DDX_Control(pDX, IDC_FOOTER, m_footer);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(writmail, CDialog)
	//{{AFX_MSG_MAP(writmail)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_WM_CLOSE()
	ON_WM_HELPINFO()
	ON_BN_CLICKED(IDC_ATTACH, OnAttach)
	ON_BN_CLICKED(IDC_ATTACH_CHK, OnAttachChk)
	ON_BN_CLICKED(IDC_ADDRESS, OnAddress)
	ON_BN_CLICKED(IDC_INFO, OnInfoAdress)
	ON_BN_CLICKED(IDC_IMPORT, OnImport)
	ON_BN_CLICKED(IDC_00, On00)
	ON_BN_CLICKED(IDC_11, On11)
	ON_BN_CLICKED(IDC_22, On22)
	ON_BN_CLICKED(IDC_33, On33)
	ON_BN_CLICKED(IDC_44, On44)
	ON_BN_CLICKED(IDC_55, On55)
	ON_BN_CLICKED(IDC_66, On66)
	ON_BN_CLICKED(IDC_77, On77)
	ON_BN_CLICKED(IDC_88, On88)
	ON_BN_CLICKED(IDC_99, On99)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_BN_CLICKED(IDC_SEND_DIRECT, OnSendDirect)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_COMMAND(IDC_REPLACEDIALOG, OnReplacedialog)
	ON_COMMAND(IDC_ROT13, OnRot13)
	ON_COMMAND(IDC_CLIP_COPY, OnClipCopy)
	ON_COMMAND(IDC_CLIP_CUT, OnClipCut)
	ON_COMMAND(IDC_CLIP_PASTE, OnClipPaste)
	ON_COMMAND(ID_TO_PASTE, OnToPaste)
	ON_COMMAND(ID_SUBJ_PASTE, OnSubjectPaste)
	ON_COMMAND(ID_ADDR_PASTE, OnFidoPaste)
	ON_COMMAND(IDC_FINDINET, OnFindinet)
	ON_COMMAND(IDC_USERECIPIENT, OnUseRecipient)
	ON_COMMAND(ID_OEMTEXT, OnOemTextInsert)
	ON_COMMAND(ID_ANSITEXT, OnAnsiTextInsert)
	ON_COMMAND(IDC_BINARY_INSERT, OnBinaryInsert)
	ON_COMMAND(IDC_MULTIMEDIA_INSERT, OnMultimediaInsert)
	ON_EN_UPDATE(IDC_TO_FIDO, OnUpdateToAddr)
	ON_EN_UPDATE(IDC_TO, OnUpdateTo)
	ON_EN_UPDATE(IDC_SUBJECT, OnUpdateSubject)
	ON_EN_KILLFOCUS(IDC_TO, OnKillfocusTo)
	ON_EN_KILLFOCUS(IDC_TO_FIDO, OnKillfocusToFido)
	ON_EN_KILLFOCUS(IDC_SUBJECT, OnKillfocusSubject)
	ON_EN_SETFOCUS(IDC_TEXT, OnSetfocusText)
	ON_EN_SETFOCUS(IDC_TO, OnSetfocusTo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// ================================================================
	BOOL writmail::OnInitDialog()
// ================================================================
{
CStrList alst;
CString aka,str,str1,tmp,echoes,s_second,mail,mailq;
areadef ad;
RECT	re,re1;
char	chbuf[2*MAX_MSG],buf[MAX_BOSSLEN];//,buf1[50];
int		ret,step,h,i;
int		lng[]={
				IDC_SAVE,
				IDC_ATTACH,
				IDCANCEL,
				IDC_STATIC3,
				IDC_STATIC2,
				IDC_STATIC4,
				IDC_STATIC1,
				IDC_ATTACH_CHK,
				IDC_REQUEST_RECIPT,
				IDC_SEND_DIRECT,
				IDC_KILL_AFTER_SEND,
				IDC_INFO,
				IDC_IMPORT,
				IDC_STATIC7,
				IDC_CHAR_CONVERSION,
				IDC_FREEZEMAIL,
				IDC_PRIVATE,
				IDHELP,
				IDC_STATIC5,
				IDC_ADDRESS
				};

	CDialog::OnInitDialog();
	gEditor=this;
	set_dlg_language(this,DlgName,lng,sizeof(lng)/sizeof(int));
// hide unnecessary elements
	if (LANG_ID==LANG_GER)	// german language
	{
		SHOW(m_convert);
		m_convert.SetCheck(get_cfg(CFG_COMMON,"CharConversion",0));
	}
	if (ust.dest_area_handle==0)	// netmail
	{
		//SHOW(m_attached);
		//SHOW(m_toaddr);
		//ENABLE(m_attach);
		if (!get_cfg(CFG_EDIT,"NetmailOrigin",0))	HIDE(m_origin);
	}
	else	// arcmail
	{
		HIDE(m_attached);
		HIDE(m_direct);
		HIDE(m_recipt);
		HIDE(m_killafter);
		HIDE(m_toaddr);
		DISABLE(m_attach);
	}
	if (gc.mode==MODE_CHANGE)
	{
		HIDE(m_origin);
		HIDE(m_header);
		HIDE(m_footer);
		DISABLE(m_attach);
	}
// set fonts and adjust sizes
	m_origin.GetWindowRect(&re);
	h=re.bottom-re.top;
	m_header.SetFont(&font_mailtext);
	m_footer.SetFont(&font_mailtext);
	m_origin.SetFont(&font_mailtext);
// textedit init
	m_buffer.InitNew();
	m_mailtext.SubclassDlgItem(IDC_TEXT,this);
	m_mailtext.SetParser(&m_parser);
	m_mailtext.AttachToBuffer(&m_buffer);
	m_mailtext.ResetView();
	ApplyEditOptions();
	gEdit=&m_mailtext;
// adjust sizes
	m_origin.GetWindowRect(&re);
	h=re.bottom-re.top-h;	// delta height
	AdjustDlgWidth();
// set limits
	m_to.SetLimitText(35);
	m_aliases.LimitText(35);
	m_subject.SetLimitText(71);
// init tooltips
	InitTooltips();

	header_changed=footer_changed=template_text=changed_fields=0;
	forceclose=forcefocus=reply_id=extra_aka=0;
	through_gate=0;
	//Org_FromAddr.Empty();
	*chbuf=0;
	
//	if (!IsOem(&font_mailtext))	CharToOem(gMailbuf,gMailbuf);	// set code conversion !!! see

// init work buffer and fill it with mailtext
	ALLOC0(Mailbuf,2*MAX_MSG);
	strcpy(Mailbuf,gMailbuf);
	memcpy(&ust,&gustat,sizeof(gustat));
	mailheader &mh=gustat.act_mailh;
// backup original requisites
	make_address(Org_FromAddr,mh.srczone,mh.srcnet,mh.srcnode,mh.srcpoint);
	make_address(Org_ToAddr,mh.dstzone,mh.dstnet,mh.dstnode,mh.dstpoint);
	Org_ToName=mh.toname;
	memcpy(&tmh,&mh,sizeof(mailheader));
// clear fields for new mail
	if (gc.mode==MODE_NEW)
	{
		tmh.fromname[0]=0;
		tmh.toname[0]=0;
		tmh.dstzone =0;
		tmh.dstnet	=0;
		tmh.dstnode =0;
		tmh.dstpoint=0;
		tmh.subject[0]=0;
	}
	//*buf=*buf1=0;
	//sscanf(tmh.toname,"%s %s",buf,buf1);
	//rcp_first=buf;
	//rcp_second=buf1;
	make_address(Cur_ToAddr,tmh.dstzone,tmh.dstnet,tmh.dstnode,tmh.dstpoint);
	Cur_Subject=tmh.subject;
	Cur_Subject.OemToAnsi();

	set_to_box(&mh);
	if (gc.mode==MODE_CHANGE)	ust.dest_area_handle=ust.cur_area_handle;
	fill_aka_combo(ust.dest_area_handle);

// adjust size of addressbook button in arcmail
	if (ust.dest_area_handle > 0)
	{
		GETRECT(m_fidodrop,re);
		GETRECTID(IDC_ADDRESS,re1);
		re1.left=re.left;
		re1.right=re.right;
		GetDlgItem(IDC_ADDRESS)->MoveWindow(&re1);
	}

// LocalMail & BBS-mail will never be send, ignore in Netmail
	*point=*buf=0;
	if (db_get_uplink_for_area(ust.dest_area_handle,buf,point) != DB_OK && 
		strncmp(ust.act_area.echotag,BBSMAIL,strlen(BBSMAIL)) && ust.dest_area_handle>1)
			ERR_MSG_RET0("E_NOVALIDBOSS");

	mh=ust.act_mailh;
	set_aliases();
	parse_kludge("\001MSGID: ",msg_addr,&msg_id);
	parse_kludge("\001REPLY: ",reply_addr,&reply_id);
	db_get_area_by_index(ust.dest_area_handle,&ad);
	if (gc.mode==MODE_CHANGE)
	{
		SetWindowText(L("S_562"));	// editing mail
		m_attached.SetCheck	(mh.attrib & MSGFILE);
		m_direct.SetCheck	(mh.attrib & MSGCRASH);
		m_recipt.SetCheck	(mh.attrib & MSGRRQ);
		m_killafter.SetCheck(mh.attrib & MSGKILL);
		m_private.SetCheck	(mh.attrib & MSGPRIVATE);
		m_freezemail.SetCheck(mh.status & DB_FROZEN_MAIL);

/*		str=mh.toname;
		str.OemToAnsi();
		m_to.SetWindowText(str);*/

		str=mh.subject;
		str.OemToAnsi();
		m_subject.SetWindowText(str);
		make_address(str,mh.dstzone,mh.dstnet,mh.dstnode,mh.dstpoint);
		m_toaddr.SetWindowText(str);
		m_area.SetWindowText(ad.echotag);
		mailq=gMailbuf;
		remove_kludges(mailq);
		InsertTextToBuffer(mailq,&m_buffer);

		if (db_get_mailtext(ust.dest_area_handle,&ust.act_mailh,chbuf,HDR_LEN) != DB_OK)
			ERR_MSG2_RET0("DBGETMAILTEXT",ust.dest_area_handle);

		forcefocus=1;
		goto exitfunc;
	}

	if (ust.dest_area_handle==0 && gc.mode != MODE_NEW)
	{
		make_address(buf,mh.srczone,mh.srcnet,mh.srcnode,mh.srcpoint);
		m_toaddr.SetWindowText(buf);
	}
	
// Origin
	m_origin.ResetContent();
	for (i=0;i<OriginList.GetCount();i++)
	{
		str=OriginList.GetString(i);
		if (IsOem(&font_mailtext))	str.AnsiToOem();
	    m_origin.AddString(str);
	}
	m_origin.SetCurSel(get_origin(&ad,gc.mode,str));

// Header and Footer
	if (gc.mode==MODE_NEW)
		m_freezemail.SetCheck(get_cfg(CFG_COMMON,"DefaultFreeze",0));
	else
		forcefocus=1;
	cur_header=get_header(&ad,gc.mode==MODE_NEW,str);
	cur_footer=get_footer(&ad,gc.mode==MODE_NEW,str);
	proc_text_macro();

	if (gc.mode != MODE_NEW)
	{
		if (gc.mode==MODE_FORWARD)
		{
// empty TO in netmail, 'All' in echomail
/*			if (ust.dest_area_handle)
				m_to.SetWindowText("All");
			else
				m_to.Clear();*/
			str=mh.subject;
		}
		else
		{
/*			str=mh.fromname;
			str.OemToAnsi();
			m_to.SetWindowText(str);*/
			if (!get_cfg(CFG_COMMON,"DontGenREs",0) && 
					strnicmp("re:",mh.subject,3) && strnicmp("re^",mh.subject,3))
				str.Format("RE: %s",mh.subject);	// no RE's
			else
				str=mh.subject;
		}
		str.OemToAnsi();
		m_subject.SetWindowText(str);
	}
	
// get the areatag from the destination area
	ret=db_get_area_by_index(ust.dest_area_handle,&ad);
	if (ret != DB_OK)	ERR_MSG2_RET0("E_DBGETAREABYINDEX",ret);

	m_area.SetWindowText(ad.echotag);
	if (gc.mode != MODE_NEW)
	{
		mail=get_selected_lines(chbuf,2) ? chbuf : gMailbuf;	// if no selection, quote all Mail
		mail.TrimRight();
//	disable or hide taglines
		remove_kludges(mail);
		if (gc.mode==MODE_FORWARD)	// forward mode
		{
			LPSTR p=mail.GetBuffer(2*MAX_MSG);
			change_mm_action(p);
			mail.ReleaseBuffer();
			mailq=fwdstring+mail+
				"\r\n\r\n======================<End Forward>============================\r\n";
		}
		else	// normal mode
		{
			make_quoted_text(mail,mailq,mh.fromname);
			forcefocus=1;
		}
		mailq.Replace("\n--- ","\n+++ ");
		mailq.Replace("\n---\r","\n+++\r");
		mailq.Replace("\n * Origin","\n + Origin");
		mailq.Replace("\nSEEN-BY: ","\nSEEN+BY: ");
		if (mailq.Right(2) != "\r\n")	mailq+="\r\n";
		InsertTextToBuffer(mailq,&m_buffer);
	}

// in FORWARD_MAIL_MODE clear m_toaddr so search will be done
	if (gc.mode==MODE_FORWARD)	m_toaddr.SetWindowText(NULL);

exitfunc:
	GETRECT(m_mailtext,re);
	UpHeight=re.top;
	BottomHeight=re.bottom;
	GetWindowRect(&re);
	BottomHeight=re.bottom-re.top-BottomHeight+h;
	ResizeDialog();
	SystemParametersInfo(SPI_GETWORKAREA,0,&re,0);
	step=(re.bottom-re.top-UpHeight-BottomHeight)/12;
	m_range.SetRange(0,12,TRUE);
	m_range.SetPos(gColors[WRITMAILSIZE]/step);
	SetTimer(2345,300,NULL);
	CenterWindow();
	return TRUE;
}

// ====================================================
	void writmail::OnOK()
// ====================================================
{
	NextDlgCtrl();
}

// ================================================================
	void writmail::OnSave() 
// ================================================================
{
mailheader  mh,*pmh;
CStrList	adrlst;
CString		mailout,str,str1,orgtxt,contb,conte;
CString		top,middle,bottom;
int			subjpos=0,longrepl=0,i=0,found=0,len,tlen;
char		mto[35],madr[200],tmp[200],bossinfo[MAX_BOSSLEN];

	BeginWaitCursor();
	m_to.GetWindowText(mto,34);
	m_toaddr.GetWindowText(madr,199);
	if (!build_address_list(mto,madr,adrlst))	return;
	Header.Empty();Footer.Empty();Msgtext.Empty();Fmpt.Empty();Topt.Empty();
	Pid.Empty();Msgid.Empty();Intl.Empty();Reply.Empty();Exkl.Empty();
	Toname.Empty();Toaddr.Empty();Origin.Empty();Tearline.Empty();
// loop on mailing list
	for (int ia=0;ia<adrlst.GetCount();ia++)
	{
		str=adrlst.GetString(ia);
		get_token(str,0,Toname);
		m_to.SetWindowText(Toname);
		get_token(str,1,Toaddr);
		m_toaddr.SetWindowText(Toaddr);
		pmh=&ust.act_mailh;
		longrepl=0;
		*mto=*madr=*tmp=0;
// check Areafix and Allfix as recipient in echomail
		if (ust.dest_area_handle > 0)
		{
			while (db_get_boss_by_index(i++,bossinfo))
			{
				get_token(bossinfo,AREAFIX,str);
				get_token(bossinfo,FILEFIX,str1);
				if ((!Toname.CompareNoCase(str) || !Toname.CompareNoCase(str1)) && 
					err_out("DN_TMMBAFC") != IDYES)	// areafix or allfix in echomail !
				{
					EndWaitCursor();
					return;
				}
			}
		}

		memset(&mh,0,sizeof(mh));
		mh.structlen=sizeof(mh);
// timestamps
		build_fido_time(tmp);
		mh.recipttime=time(NULL);
		strnzcpy(mh.datetime,tmp,20);
		CharToOem(mh.datetime,mh.datetime);
// set sender address
		if (!strcmp(ust.act_area.group,ASAVED_GROUP) && gc.mode==MODE_CHANGE)
			strcpy(point,Org_FromAddr);
		else
			m_fidodrop.GetLBText(m_fidodrop.GetCurSel(),point);
		if (parse_address(point,&mh.srczone,&mh.srcnet,&mh.srcnode,&mh.srcpoint)<3 
			&& strncmp(ust.act_area.echotag,BBSMAIL,strlen(BBSMAIL)))
		{
			EndWaitCursor();
			ERR_MSG2_RET("E_INVALIDPOINT",point);
		}
// get header and footer
		if (gc.mode != MODE_CHANGE)
		{
			m_header.GetWindowText(Header);
			m_footer.GetWindowText(Footer);
		}
// get mailtext
		GetBufferText(&m_buffer, Msgtext);
// convert umlauts
		if (m_convert.GetCheck())
		{
			convert_specchars(Header);
			convert_specchars(Msgtext);
			convert_specchars(Footer);
		}
// build technical lines
		build_kludges(ust.dest_area_handle,&mh);
		build_origin(ust.dest_area_handle,point);
		build_tearline();
		mh.mailid = Msgtime;
// subject
		m_subject.GetWindowText(str);
		if (m_convert.GetCheck() && !m_attached.GetCheck())	convert_specchars(str);
		strnzcpy(mh.subject,prepare_text(str),72);
// to-name
		m_to.GetWindowText(str);
		if (m_convert.GetCheck())	convert_specchars(str);
		strnzcpy(mh.toname,prepare_text(str),36);
// from-name
		m_aliases.GetWindowText(str);
		if (m_convert.GetCheck())	convert_specchars(str);
		strnzcpy(mh.fromname,prepare_text(str),36);
// set flags
		if (m_private.GetCheck())	mh.attrib |= MSGPRIVATE;
		mh.status = DB_MAIL_CREATED | DB_MAIL_READ;
		mh.status &= ~DB_MAIL_SCANNED;	// clear SCANNED
		if (m_freezemail.GetCheck())
			mh.status |= DB_FROZEN_MAIL;
		else
			mh.status &= ~DB_FROZEN_MAIL;
// set routing
		if (!set_routing(&mh))	{ EndWaitCursor(); return; }
// prepare text parts
		top=Intl+Fmpt+Topt+Msgid+"\r\n"+Reply+Exkl+Pid+"\r\n";
		//prepare_text(top);
		bottom="\r\n\r\n"+Footer+"\r\n"+Tearline+"\r\n"+Origin;
		//prepare_text(bottom);
// finishing mailtext
		//if (gc.mode==MODE_CHANGE)
		//	mailout=top+Msgtext;
		//else
		//{
		//	//top=mailout;
		//	bottom="\r\n\r\n"+Footer+"\r\n"+Tearline+"\r\n"+Origin;
//		mailout+=Header+"\r\n"+Msgtext+"\r\n\r\n"+Footer+"\r\n"+Tearline+"\r\n"+Origin;
		//}
		//prepare_text(mailout);
		//mh.text_length=mailout.GetLength()+1;
		//mh.mail_text=PS mailout;	// Split it !!!
		contb=L("S_604");	// continuation
		conte=L("S_36");	// to be continued
		if (gc.mode==MODE_CHANGE)
		{
			if (reply_id)	mh.replyid=reply_id;
			if (db_setnew_mailtext(ust.dest_area_handle,pmh->index,&mh) != DB_OK)
			{
				err_out("E_NEWMAILTF");
				EndWaitCursor();
				return;
			}
		}
		else
		{
			len=MAX_MSG-top.GetLength()-bottom.GetLength()-contb.GetLength()-conte.GetLength();	// max text length
			ASSERT(len>0);
// split message on max MAX_MSG blocks
			LPCSTR pb=Msgtext,pe;
			int	part=1,nparts=Msgtext.GetLength()/len+1;
			CString subj=mh.subject;
			subj.Delete(65,72);
			do
			{
				tlen=strlen(pb);
				if (tlen > len)
				{
					pe=pb+len;
					while (pe >= pb && *pe != '\r')	pe--;// search last CR before split
					if (*pe != '\r') pe=pb+len;
				}
				else
					pe=pb+tlen;
				mailout=top;
				if (part > 1)	mailout+=contb;
				mailout+=Msgtext.Mid(pb-Msgtext,pe-pb);
				if (pe+1-Msgtext < Msgtext.GetLength())	mailout+=conte;
				mailout+=bottom;
				prepare_text(mailout);
				mh.mail_text=PS mailout;
				mh.text_length=mailout.GetLength()+1;
				if (nparts>1)	sprintf(mh.subject,"%s (%02d)",subj,part);
				if (db_append_new_message(ust.dest_area_handle,&mh) != DB_OK)
				{
					err_out("E_NEWMAILF");
					EndWaitCursor();
					return;
				}
				part++;
				pb=pe+1;
				if (*pb == '\n') pb++;
			} while(pb-Msgtext < Msgtext.GetLength());
/*		if (gc.isLongUUstuf)	// modify Subject if we have multipart code
		{
			subjpos=strlen(mh.subject);
			if (subjpos>60)
			{
				mh.subject[60]=0;
				subjpos=60;
			}
			// first Mail
			sprintf(&mh.subject[subjpos]," [01/%02d]",gc.NumberOfSections);
		}
		if (db_append_new_message(ust.dest_area_handle,&mh)!=DB_OK)
		{
			err_out("E_NEWMAILF");
			EndWaitCursor();
			return;
		}

		db_refresh_area_info(ust.dest_area_handle);
		mark_as_answered(1);

		int		bytesdone,len;
		char	*curpos;
		char	stobuf[MAX_MSG+1000];	// buffer for one section
		if (gc.isLongUUstuf)
		{
			char headbuf[5000];
			char *x;
			long xmailid;
			int  xlen,ipid;
			
//			BeginWaitCursor();
			curpos=gc.LongUUBasePointer;
			bytesdone=0;
			len=gc.MaxUULines*62;
// find MAILID place in header
			x=strstr(mh.mail_text,"PID:");
			x=strchr(x,'\r');
			xlen=x-mh.mail_text;
			memcpy(headbuf,mh.mail_text,xlen);
			headbuf[xlen]=0;
// look for second SPACE after MSGID
			x=strstr(headbuf,"MSGID:");
			sscanf(x,"%*s %*s %n%x",&ipid,&xmailid);
			
			for (int j=2;j<=gc.NumberOfSections;j++)
			{
				memset(stobuf,0,sizeof(stobuf));
				if (j==gc.NumberOfSections)
					len=gc.LongUUstufNrBytes-bytesdone;	// last section
				ASSERT(len<(MAX_MSG+1000));
				memcpy(stobuf,curpos,len);
				bytesdone+=len;
				curpos+=len;

				CString footbuf;
				char dm[100];
				// write new ID (without ending 0)
				xmailid++;
				sprintf(dm,"%x",xmailid);
//				memcpy(pid,dm,strlen(dm));
				memcpy(x+ipid,dm,strlen(dm));

				mailout.Format("\r\r\rsection %d of %d of file %s   -=< FIPS-uuencoder >=-\r\r",
					j,gc.NumberOfSections,gc.UUfilename);
				mailout=mailout+stobuf+"\r\r\r";
				footbuf=Footer+"\r"+Tearline+"\r"+Origin;
				expand_tabs(footbuf);
				mailout+=footbuf;
				mailout.AnsiToOem();
				mailout=headbuf+mailout;
				mh.mail_text=PS mailout;
				mh.text_length=mailout.GetLength()+1;
				ASSERT(mh.text_length<MAX_MSG);
				mh.mailid=xmailid;

				ASSERT((int)strlen(mh.subject)>=subjpos);
			   	sprintf(&mh.subject[subjpos]," [%02d/%02d]",j,gc.NumberOfSections);

				if (db_append_new_message(ust.dest_area_handle,&mh)!=DB_OK)
				{
					err_out("E_NEWAPPUUF");
					EndWaitCursor();
					return;
				}
			}
//			EndWaitCursor();
		}*/
		}

// mark area for scanning if it's not AUTOSAVE
	areadef ad;
	memset(&ad,0,sizeof(ad));
//	db_open_simple(ust.dest_area_handle);
	db_get_area_by_index(ust.dest_area_handle,&ad);
//!(ad.membership & AUTOCREATEAD_ASAVE)
	if (strcmp(ad.group,ASAVED_GROUP) &&
		db_set_area_state(ust.dest_area_handle,1) != DB_OK)
	{
		err_out("E_MODFLAGSET");
		EndWaitCursor();
		return;
	}

	memcpy(&ust.act_mailh,&tmh,sizeof(mailheader));// copy old Mailheader

//	if (multimode)	goto nextmultiple;
	}	// next address

//endmultiple:
	KillTimer(2345);
	show_msg(L("S_606"));	// saved
	gc.writemail_active=0;
	if (gc.LongUUBasePointer)	free(gc.LongUUBasePointer);	// long UU-buffer exist
	gc.LongUUBasePointer=0;	
	gc.isLongUUstuf=0;
	EndWaitCursor();
	gpMain->SendMessage(UDM_EDITEND);
	CDialog::DestroyWindow();	// required for nonmodal dialog
}

// ====================================================
// returns 0 if number of selected lines < minlines, otherwise - 1
// selected lines returns in buf
	int get_selected_lines(char *buf,int need,BOOL addLF)
// ====================================================
{
LPSTR p;
int		cnt=gpMain->m_mailview.GetSelCount();

	if (cnt < need)	return 0;
	CArray<int,int> aSel;
	aSel.SetSize(cnt);
	gpMain->m_mailview.GetSelItems(cnt,aSel.GetData()); 
	for (int n=0;n<cnt;n++)
  {
		p=(LPSTR)m_globedit->GetItemDataPtr(aSel[n]);
		if (p < 0)	return 0;
		strcat(buf,p);
		if (n < cnt-1)
		{
			strcat(buf,"\r");
			if (addLF)	strcat(buf,"\n");
		}
	}
	return 1;
}

// ====================================================
	void writmail::OnAttachChk()
// ====================================================
{
	m_direct.SetCheck(m_attached.GetCheck());
}

// =========================================================
// calls address selection
	void writmail::OnAddress()
// =========================================================
{
CString str;
int x,y;

	m_to.GetWindowText(str);
	addrselect dlg;
	dlg.SetPattern(str);
	//m_to.GetWindowText(dlg.name,99);
	m_toaddr.GetWindowText(dlg.address,99);
	dlg.DoModal();

	if (!dlg.selected) return;
	if (dlg.isgroup)
	{
	   str=str+"::"+dlg.name;
	   m_to.SetWindowText(str);
	   m_toaddr.SetWindowText(NULL);
	   m_subject.SetWindowText(NULL);
	   m_mailtext.SetWindowText(NULL);
	}
	else
	{
		m_to.SetWindowText(dlg.name);
		m_toaddr.SetWindowText(dlg.address);
		proc_macros(dlg.comment);
		set_aka_combo();
		m_to.GetWindowText(str);
		if (*dlg.name)	m_subject.SetFocus();
		proc_text_macro();
		if (template_text)
		{
			y = m_buffer.GetLineCount();
			x = m_buffer.GetLineLength(y - 1);
			m_mailtext.SetCursorPos(CPoint(x - 1, y - 1));
			m_mailtext.SetFocus();
		}
	}
}

// =========================================================
	LPSTR build_fido_time(LPSTR buf)
// =========================================================
{
char tmp[100];
int  m,d,y;
char names[12][4]={"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};

 	_strdate(buf);
 	sscanf(buf,"%d/%d/%d",&m,&d,&y);

 	_strtime(tmp);
 	sprintf(buf,"%02d %s %02d  %s",d,names[m-1],y,tmp);
	return buf;
}

// ================================================================
	void writmail::OnCancel()
// ================================================================
{
	if (err_out("DY_REACANCL") != IDYES)	return;

	KillTimer(2345);
	memcpy(&ust.act_mailh,&tmh,sizeof(mailheader));
	gc.writemail_active=0;
	if (gc.LongUUBasePointer) 	
	{ 
		free(gc.LongUUBasePointer); 
		gc.LongUUBasePointer=0;	
	}
	gc.isLongUUstuf=0;
	gc.ShouldMarkAsAnsweredOnSave=0;
	CDialog::DestroyWindow();
}

// ================================================================
	void writmail::OnAttach()
// ================================================================
{
CString	str;
char	buf[MAX_PATH];
int		ret;

	str.LoadString(IDS_ALLFILES);
	CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY,str);
	ret=dlg.DoModal();
	restore_base_path();
	if (ret!=IDOK)	return;
	GetShortPathName(dlg.GetPathName(),buf,MAX_PATH);
	m_attached.ShowWindow(SW_SHOWNORMAL);
	m_subject.SetWindowText(buf);
	m_attached.SetCheck(1);
}

// ================================================================
// sets droplist to best fitted aka
	void writmail::set_aka_combo(LPCSTR fido)
// ================================================================
{
CString addr;
char	str[50];
int		len,i,bestind,bestlen;

	if (*fido)
		addr=str;
	else
		m_toaddr.GetWindowText(addr);
	if (get_uplink_address(addr,str))
		addr=str;

	i=0;
	bestind=0;
	bestlen=0;
	while (m_fidodrop.GetLBText(i++,str)!=LB_ERR)
	{
		len=get_equal_size(str,addr);
		if (len>bestlen)
		{
			bestlen=len;
			bestind=i-1;
		}
	}
	m_fidodrop.SetCurSel(bestind);
}

// ================================================================
	void writmail::OnUpdateToAddr()
// ================================================================
{
	if (ust.dest_area_handle==0 && gc.mode != MODE_CHANGE)	set_aka_combo();
	m_toaddr.GetWindowText(gc.AddressForInfo,sizeof(gc.AddressForInfo)-1);
	proc_text_macro();
}

// ================================================================
	void writmail::OnInfoAdress()
// ================================================================
{
	info_adr dlg;
	dlg.DoModal();
	m_mailtext.SetFocus();
}

// ================================================================
	void writmail::OnImport()
// ================================================================
{
	impwhat dlg;
	dlg.DoModal();
	m_mailtext.SetFocus();
}

void writmail::On00()  {  handle_import(0); }
void writmail::On11()  {  handle_import(1); }
void writmail::On22()  {  handle_import(2); }
void writmail::On33()  {  handle_import(3); }
void writmail::On44()  {  handle_import(4); }
void writmail::On55()  {  handle_import(5); }
void writmail::On66()  {  handle_import(6); }
void writmail::On77()  {  handle_import(7); }
void writmail::On88()  {  handle_import(8); }
void writmail::On99()  {  handle_import(9); }

// ================================================================
	void writmail::handle_import(int index)
// ================================================================
{
CString key,path;
FILE	*fp;
char	buf[MAX_MSG],line[5000];
char	*p;
int		cod;

	key.Format("Alt%d",index);
	parse_textimp_str(key,path,cod);
	if (path.GetLength()==0)	return;

	if (fp=fopen(path,"rt"))
	{
		memset(buf,0,MAX_MSG);
		while (fgets(line,4999,fp))
		{
			p=strchr(line,'\r');  
			if (p)  
				*p=0;
			p=strchr(line,'\n');  
			if (p)  
				*p=0;
			strcat(line,"\r\n");
			strcat(buf,line);
			/*
			if (strlen(buf)+msglen>MAX_MSG-1000)
				break;
			*/
		}
		fclose(fp);
		if (strlen(buf))
		{
			if (!cod)	OemToChar(buf,buf);
			ReplaceSelection(&m_buffer, &m_mailtext, buf);
		}
	}
	else
		err_out("E_CANOPIM",path);
}

// =======================================================
// resizes editor dialog and repositions its elements
	void writmail::ResizeDialog (void)
// =======================================================
{
RECT re;
int up,bt,h;
int	off=gColors[WRITMAILSIZE];	// height of dialog

	GetWindowRect(&re);
	re.bottom=re.top+UpHeight+off+BottomHeight;
	MoveWindow(&re,1);
	h=AdjustUp(IDC_FOOTER,UpHeight+off);
	h=AdjustUp(IDC_ORIGIN,h)+6;
	AdjustUp(IDHELP,h);
	AdjustUp(IDC_INFO,h);
	AdjustUp(IDC_ATTACH,h);
	AdjustUp(IDC_IMPORT,h);
	AdjustUp(IDCANCEL,h);
	AdjustUp(IDC_SAVE,h);

	if (gc.mode==MODE_CHANGE)	// expand edit box in change-mail mode
	{
		GETRECTID(IDC_SUBJECT,re);
		up=re.bottom+5;
		GETRECTID(IDC_SAVE,re);
		bt=re.top-6;
		GETRECT(m_mailtext,re);
		re.top=up;
		re.bottom=bt;
	}
	else
	{
		GETRECT(m_mailtext,re);
		re.bottom=re.top+off;
	}
	m_mailtext.MoveWindow(&re);
}

// =======================================================
	void writmail::OnTimer(UINT nIDEvent)
// =======================================================
{
int size;
RECT re;
    
	SystemParametersInfo(SPI_GETWORKAREA,0,&re,0);
	size=m_range.GetPos()*(re.bottom-re.top-UpHeight-BottomHeight)/12;
	if (size!=gColors[WRITMAILSIZE])// adjust the height?
	{
	    gColors[WRITMAILSIZE]=size;
		set_cfg(CFG_COLORS,"WriteEditSize",size);
		ResizeDialog();
	}
	CDialog::OnTimer(nIDEvent);
}

// =======================================================
	int writmail::AdjustUp(int id,int top)
// =======================================================
// TOP: Verschiebt ein DialogElement um die angegebene Position nach unten oder oben
{
RECT re;
int  h;

	GETRECTID(id,re);
	h=re.bottom-re.top;
	re.top=top;
	re.bottom=top+h;
	GetDlgItem(id)->MoveWindow(&re,1);
	return re.bottom;
}

// ====================================================
	int expand_template(char *tmpl,char *expd,mailheader *pmh)
// ====================================================
// '%A': - current area
// '%d': - current date
// '%D': - quoted date (dd mmm yy)
// '%f': - quoted from-address
// '%F': - quoted to-address
// '%I': - insert text file, format %I#, where # - digit from text import setup
// '%G': - quoted subject
// '%n': - current to-name (interactive)
// '%N': - current to-surname (interactive)
// '%r': - quoted to-name
// '%R': - quoted to-surname
// '%s': - quoted from-name
// '%S': - quoted from-surname
// '%t': - current time
// '%T': - quoted time (hh:mm)
// '%Z': - current to-address (interactive)
{
CString str,s_first,s_second,r_first,r_second;
FILE	*fp;
char	buf[60000],line[5000],buf1[100],buf2[100];
char	*p,*ep,*t=0;
BOOL	atr=FALSE;
CTime	now=CTime::GetCurrentTime();
CTime	dat;
int		cod;

//	setlocale(LC_ALL,"...");
//English_United States.1252
//Russian_Russia.1251
//German_Germany.1252
	p=tmpl;
	ep=expd;
// FROM
	str=pmh->fromname;
	str.OemToAnsi();
	parse_name(str,s_first,s_second);
// TO
	str=pmh->toname;
	str.OemToAnsi();
	parse_name(str,r_first,r_second);

	 while (*p)
	 {
		 if (*p=='%')
		 {
			switch (*(p+1))
			{
				case 'A':	// original area for copy
					if (gustat.act_area.localmail)
					{
						db_get_mailtext(gustat.cur_area_handle,pmh,buf,1000);
						CharToOem(L("S_130",""),buf2);
						p=strstr(buf,buf2);
						if (p)
						{
							sscanf(p+strlen(buf2),"%s",buf1);
							str=buf1;
						}
					}
					else		// current area
						str=gustat.act_area.echotag;
					break;
				case 'd':	// current date (localized)
					build_fido_time(buf1);
					buf1[9]=0;
					str=langtime(buf1,buf2);
					break;
				case 'D':	// quoted date (localized)
					strnzcpy(buf1,pmh->datetime,10);
					str=langtime(buf1,buf2);
					break;
				case 'f':	// quoted from-address
					make_address(str,pmh->srczone,pmh->srcnet,pmh->srcnode,pmh->srcpoint);
					break;
				case 'F':	// quoted to-address
					make_address(str,pmh->dstzone,pmh->dstnet,pmh->dstnode,pmh->dstpoint);
					break;
				case 'I':	// insert text file, format %I#, where # - digit from text import setup
					if (isdigit(*(p+2)))
					{
						strcpy(line,"Alt");
						strncat(line,p+2,1);
						line[4]=0;
						parse_textimp_str(line,str,cod);
						if (str.GetLength())
						{
							fp=fopen(str,"rt");
							if (fp)
							{
								memset(buf,0,59999);
								while (fgets(line,4999,fp))
								{
									t=strchr(line,'\r');  if (t)  *t=0;
									t=strchr(line,'\n');  if (t)  *t=0;
									strcat(line,"\r\n");
									strcat(buf,line);
									if (strlen(buf)>58000)	break;
								}

								fclose(fp);
								str=buf;
								if (str.GetLength()>1)	// remove ending LF/CR
									str=str.Left(str.GetLength()-2);
								if (!cod)
									str.OemToAnsi();
							}
							else
							{
								err_out("E_CANOPIM",str);
								str.Empty();
							}
							p++;
						}
					}
					break;
				case 'G':	// quoted subject
//				case 'L':
					str=Cur_Subject;
					break;
				case 'n':	// current to-name (changed with field)
					str=rcp_first;
					break;
				case 'N':	// current to-surname (changed with field)
					str=rcp_second;
					break;
				case 'r':	// quoted to-name
					str=r_first;
					break;
				case 'R':	// quoted to-surname
					str=r_second;
					break;
				case 's':	// quoted from-name
					str=s_first;
					break;
				case 'S':	// quoted from-surname
					str=s_second;
					break;
				case 't':	// current time
					str=now.Format("%H:%M");
					break;
				case 'T':	// quoted time (hh:mm)
					str=pmh->datetime;
					str=str.Mid(11,5);
					break;
				case 'Z':	// current to-address (changed with field)
					str=Cur_ToAddr;
					break;
				default:
					str="%";
					p--;
			}
			if (strlen(str))
			{
				strcpy(ep,str);
				ep+=strlen(str);
				p+=2;
			}
			else	// expansion is empty
			{
				t=ep-1;
				while (t>=expd && strchr(ATTRS,*t))
					*t--=0;	// skip attributes
				ep=t+1;
				p+=2;
				while (*p && strchr(ATTRS,*p))
					p++;
			}
		 }
		 else
			 *ep++=*p++;	// Copy Byte
	 }
	 *ep=0;
	 return 1;
}

// ================================================================
	void writmail::OnChangeHeader(void)
// ================================================================
{
	cha_tmpl tmp(1);
	tmp.DoModal();
	cur_header=gc.new_header_index;
	proc_text_macro();
}

// ================================================================
	void writmail::OnChangeFooter(void)
// ================================================================
{
	cha_tmpl tmp(2);
	tmp.DoModal();
	cur_footer=gc.new_footer_index;
	proc_text_macro();
}

// ====================================================
	void writmail::proc_text_macro(void)
// ====================================================
{
char    buf[1000],buf1[1000],buf2[1000];

	*buf1=*buf2=0;
	m_to.GetWindowText(buf,999);
	buf[999]=0;
	sscanf(buf,"%s %s",buf1,buf2);
	rcp_first=buf1;
	rcp_second=buf2;
	m_toaddr.GetWindowText(Cur_ToAddr);
	if (!through_gate && !header_changed)
	{
		strnzcpy(buf,HeaderList.GetString(cur_header),sizeof(buf));
		insert_LFCR(buf);
		expand_template(buf,buf1,&tmh);
		if (IsOem(&font_mailtext))	CharToOem(buf1,buf1);
		m_header.SetWindowText(buf1);
	}
	if (!footer_changed)
	{
		strnzcpy(buf,FooterList.GetString(cur_footer),sizeof(buf));
		insert_LFCR(buf);
		expand_template(buf,buf1,&tmh);
		if (IsOem(&font_mailtext))	CharToOem(buf1,buf1);
		m_footer.SetWindowText(buf1);
	}
}

// =======================================================================
	void writmail::OnSetfocusTo() 
// =======================================================================
{
	if (forcefocus)	
	{
		forcefocus=0;
		m_mailtext.SetFocus();
	}
}

// =======================================================
	void writmail::OnKillfocusTo()
// =======================================================
{
CWnd		*pWnd=GetFocus();
CString		toname,tofido,str,info;

// quick exit
	if (forceclose || 
		pWnd==GetDlgItem(IDCANCEL) || pWnd==GetDlgItem(IDC_IMPORT) || 
		pWnd==GetDlgItem(IDC_TO_FIDO) || pWnd==GetDlgItem(IDC_ADDRESS) ||
		(pWnd==&m_mailtext && gc.mode != MODE_NEW))
			return;
// change checkboxes and return focus
	if (pWnd==&m_freezemail || pWnd==&m_direct || 
		pWnd==&m_private || pWnd==&m_recipt ||
		pWnd==&m_killafter || pWnd==&m_attached || pWnd==&m_convert)
	{	// proceed checkboxes
		((CButton*)pWnd)->SetCheck(!((CButton*)pWnd)->GetCheck());
		m_to.SetFocus();
		return;
	}
/*/ force focus to text without checking TO
	if (forcefocus)
	{
		forcefocus=0;
		m_mailtext.SetFocus();
		return;
	}*/

	m_to.GetWindowText(toname);
	m_toaddr.GetWindowText(tofido);
    trim_all(toname);
	trim_all(tofido);
	if (!toname.IsEmpty() && !tofido.IsEmpty())	return;
	find_recipient(toname,tofido);
	proc_text_macro(); // Update Header und Footer stuff

	if (template_text)
	{
		int y = m_buffer.GetLineCount();
		int x = m_buffer.GetLineLength(y - 1);
		m_mailtext.SetCursorPos(CPoint(x - 1, y - 1));
	}
//	if (gc.change_mode || !gc.newmail_mode)
//		m_mailtext.SetFocus();
//	else
		m_subject.SetFocus();
}

// ====================================================
	void writmail::OnKillfocusToFido()
// ====================================================
{
CWnd		*pWnd=GetFocus();
CString	toname,tofido;

// quick exit
	if (forceclose || 
		pWnd==GetDlgItem(IDCANCEL) || pWnd==GetDlgItem(IDC_IMPORT) || 
		pWnd==GetDlgItem(IDC_TO) || pWnd==GetDlgItem(IDC_ADDRESS) ||
		(pWnd==&m_mailtext && gc.mode==MODE_CHANGE))	return;
// change checkboxes and return focus
	if (pWnd==&m_freezemail || pWnd==&m_direct || 
		pWnd==&m_private || pWnd==&m_recipt ||
		pWnd==&m_killafter || pWnd==&m_attached || pWnd==&m_convert)
	{
		((CButton*)pWnd)->SetCheck(!((CButton*)pWnd)->GetCheck());
		m_toaddr.SetFocus();
		return;
	}
/*/ force focus to text without checking TO
	if (forcefocus)
	{
		forcefocus=0;
		m_mailtext.SetFocus();
		return;
	}*/

	m_to.GetWindowText(toname);
	m_toaddr.GetWindowText(tofido);
    trim_all(toname);
	trim_all(tofido);
	if (!toname.IsEmpty() && !tofido.IsEmpty())	return;
	find_recipient(toname,tofido);
	proc_text_macro();
}

// ====================================================
	void writmail::find_recipient(LPCSTR name,LPCSTR addr)
// ====================================================
{
mailheader *pmh;
_fidonumber number;
CString full,info,str;
int		zone,net,node,point,ret;

	if (ust.dest_area_handle)	return;	// only for netmail
	pmh=&ust.act_mailh;
	sscanf(name,"%s",pmh->toname);
	ret=parse_address(addr,&zone,&net,&node,&point);
	if (strlen(addr))
		str=addr;
	else
		str=name;
	if (!search_in_abook(str))
	{
		if (expand_address(str,full,info,1,0,1,ret,1))
		{
			m_toaddr.SetWindowText(full);
			set_aka_combo();
			parse_address(full,&zone,&net,&node,&point);
			if (nl_get_fido_by_number(zone,net,node,point,&number))
			{
				strcpy(pmh->toname,number.user);
				m_to.SetWindowText(number.user);
				strcpy(pmh->toname,number.user);
				sscanf(full,"%hu:%hu/%hu.%hu",
					&pmh->dstzone,&pmh->dstnet,&pmh->dstnode,&pmh->dstpoint);
			}
		}
	}
}

// ====================================================
	void writmail::OnKillfocusSubject()
// ====================================================
{
WIN32_FIND_DATA finddata;
HANDLE  hfile;
CString filename,str;
char 	drive[300],dir[300],fname[300],ext[300];

	if (m_attached.GetCheck())	// check existance of fileattach
	{
		m_subject.GetWindowText(str);
		hfile = FindFirstFile(str,&finddata);
		if (hfile != INVALID_HANDLE_VALUE)
		{
			while (!strcmp(finddata.cFileName,".") || !strcmp(finddata.cFileName,".."))
			{
				if (!FindNextFile(hfile,&finddata))
				{
					CloseHandle(hfile);
					goto nothing;
				}
			}
			FindClose(hfile);
			_splitpath(str,drive,dir,fname,ext);
			filename=drive;
			filename+=dir;
			filename+=finddata.cFileName;
			filename.MakeUpper();
			m_subject.SetWindowText(filename);

		}
		else
			show_msg(L("S_5"));	// no file
	}

nothing:
	proc_text_macro();
}

// ====================================================
	void writmail::OnUpdateTo()
// ====================================================
{
	proc_text_macro();
}

// ====================================================
	void writmail::OnUpdateSubject()
// ====================================================
{
	proc_text_macro();
}

// ============================================
// returns TRUE for valid address list, lst contains pairs name-address
	BOOL build_address_list(LPCSTR name,LPCSTR addr,CStrList &lst)
// ============================================
{
CString tmp,tmp1,line;

	lst.RemoveAll();
	if (strncmp(name,"::",2))	// single address
	{
		if (*name)
		{
			tmp=name;
			trim_all(tmp);
			tmp=tmp+"\t"+addr;
			trim_all(tmp);
			lst.AddTail(tmp);
			return 1;
		}
		err_out("E_NOT_FCOR",name);
	}
	else						// group address
	{
		for (int i=0;i<Agroups.GetCount();i++)
		{
			line=Agroups.GetString(i);
			get_token(line,0,tmp);
			if (!tmp.CompareNoCase(name+2))
			{
				for (int k=1;k<=get_token_int(line,1);k++)
				{
					get_token(line,k*2,tmp);
					get_token(line,k*2+1,tmp1);
					trim_all(tmp);
					trim_all(tmp1);
					tmp=tmp+"\t"+tmp1;
					lst.AddTail(tmp);
				}
				return 1;
			}
		}
		err_out("E_NOSGF",name+2);
	}
	return 0;
}

// ============================================
	void writmail::OnReplacedialog()
// ============================================
{
	searepl tmp;
	tmp.DoModal();
}

// ============================================
	void writmail::OnSendDirect()
// ============================================
{
	if (!m_direct.GetCheck() && m_attached.GetCheck())// Fileattach
			err_out("W_ASSING");
}

// ============================================
	void writmail::OnRot13()
// ============================================
{
string str;
CPoint start,stop;

	m_mailtext.GetSelection(start,stop);

	if (start == stop) return;
	m_buffer.GetText(start.y, start.x, stop.y, stop.x, str);
	for (int j = 0; j < (int)str.length(); j++)
	{
		if ((str[j] >= 'a') && (str[j] <= 'z'))
			str[j]='a'+(((str[j]-'a')+13)%26);

		if ((str[j] >= 'A') && (str[j] <= 'Z'))
			str[j]='A'+(((str[j]-'A')+13)%26);

	}
	int y, x;
	m_buffer.DeleteText(0, start.y, start.x, stop.y, stop.x, CE_ACTION_UNKNOWN);
	m_buffer.InsertText(0, start.y, start.x, str.c_str(), y, x, CE_ACTION_UNKNOWN); 
	m_mailtext.SetSelection(start,stop);
}

// ============================================
	void writmail::OnMultimediaInsert()
// ============================================
{
	MM_Element.Empty();
	insmm dlg;
	dlg.DoModal();
	if (MM_Element.IsEmpty())	return;
	ReplaceSelection(&m_buffer, &m_mailtext, MM_Element);
	m_mailtext.SetFocus();
}

// ====================================================================
	void writmail::OnOemTextInsert()
// ====================================================================
{
	TextInsert(TRUE);
}

// ====================================================================
	void writmail::OnAnsiTextInsert()
// ====================================================================
{
	TextInsert(FALSE);
}

// ====================================================================
	void writmail::TextInsert(BOOL bOem)
// ====================================================================
{
CString	str;
FILE	*fp;
char	line[1000],path[MAX_PATH];
int		ret;

	str.LoadString(IDS_TEXTFILT);
	CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY,str);
	ret=dlg.DoModal();
	restore_base_path();
	if (ret != IDOK)	return;
	strcpy(path,dlg.GetPathName());
	fp=fopen(path,"rt");
	if (fp)
	{
		str.Empty();
		while (fgets(line,999,fp))
		{
			*(line+strcspn(line,"\r\n"))=0;
			str=str+line+"\r\n";
		}
		fclose(fp);
		//IsOem(str);	// test
		if (bOem && !IsOem(&font_mailtext))	str.OemToAnsi();
		if (!bOem && IsOem(&font_mailtext))	str.AnsiToOem();
		ReplaceSelection(&m_buffer,&m_mailtext,str);
	}
	else
		err_out("E_CANOPIM",path);
	m_mailtext.SetFocus();
}

// ====================================================================
	void writmail::OnBinaryInsert()
// ====================================================================
{
CString str;
char buf[MAX_MSG],aline[1000];
int  ret,cnt;
BOOL isfirst=TRUE;

	m_tmp.Clear();
	CUUENC uue(m_hWnd);
	uue.Reset();
	memset(buf,0,sizeof(buf));

	gc.LongUUstufAddlines=gc.LongUUstufNrBytes=0;
	cnt=0;
	BeginWaitCursor();
	while (1)
	{
		if (isfirst)
			isfirst=FALSE;	// first line - title, not countered
		else
			cnt++;
		ret=uue.EncodeLine(aline,1000);
		ASSERT (strlen(aline)<1000);
		switch (ret)
		{
			case STAT_END:		// User has canceled
				EndWaitCursor();
				return;
			case STAT_ERROR:	// File Open error
				EndWaitCursor();
				ERR_MSG_RET("E_FILEOPEN");
			case STAT_LONG:		// File too long
				EndWaitCursor();
				ERR_MSG_RET("M_FILETOOLONG");
			case 999:
				goto exitloop;
		}
		str+=aline;
		str+="\r\n";
/*		m_tmp.SetSel(0,-1);
		if (!gc.isLongUUstuf || cnt<=gc.MaxUULines)	// one-section uu-code
		{
			strcat(buf,aline); 
			strcat(buf,"\r\n");
		}
		else	// multi-section uu-code
		{
			strcat(aline,"\r");
			len=strlen(aline);
			gc.LongUUstufNrBytes+=len;
			strcat(gc.UUBasePointertmp,aline);
			gc.UUBasePointertmp+=len-1;
			gc.LongUUstufAddlines++;
		}*/
		memset(aline,0,sizeof(aline));
	}
	
exitloop:
	// copy first section to clipboard
	m_tmp.SetWindowText(str);
	m_tmp.SetSel(0,-1);
	m_tmp.Copy();
	m_tmp.Clear();
	EndWaitCursor();
	err_out(gc.isLongUUstuf ? "M_DATABIGNIC" : "M_DATANIC");
	m_mailtext.SetFocus();
}

// ============================================
	void writmail::OnSetfocusText()
// ============================================
{
char to[100],to_fido[100];

	if (gc.mode != MODE_NEW || !gatecfg.reserved3)	return;	// InGate?
	m_to.GetWindowText(to,99);
	to[99]=0;
	m_toaddr.GetWindowText(to_fido,99);
	to_fido[99]=0;
	if (strchr(to,'@') || strchr(to_fido,'@'))	HandleInGateMacros();
}

// ============================================
	void writmail::HandleInGateMacros()
// ============================================
{
CString to,to_fido,subject,aka,mailtext,inaddr,copyof_aka,copyof_mailtext,tmp;
int		sel,count;

	through_gate=1;
	m_to.GetWindowText		(to);
	m_toaddr.GetWindowText	(to_fido);
	m_subject.GetWindowText	(subject);
	GetBufferText(&m_buffer, mailtext);
	sel=m_fidodrop.GetCurSel();	
	m_fidodrop.GetLBText(sel,aka);
	inaddr.Empty();
	if (strchr(to,'@'))			inaddr=to;
	if (strchr(to_fido,'@'))	inaddr=to_fido;

	copyof_aka=aka;
	copyof_mailtext.Empty();

	if (handle_ingate_translation(to,to_fido,subject,aka,mailtext,inaddr))
	{
		m_to.SetWindowText(to);
		m_toaddr.SetWindowText(to_fido);
		m_subject.SetWindowText(subject);

		if (mailtext != copyof_mailtext)
			m_header.SetWindowText(copyof_mailtext+mailtext);

		if (aka != copyof_aka)
		{
			if (!extra_aka)
			{
				extra_aka=1;
				m_fidodrop.AddString(aka);
			}
			count=m_fidodrop.GetCount()-1;
			if (count<0)
				count=0;
			m_fidodrop.SetCurSel(count);
		}
	}
	m_mailtext.SetFocus();
}

// ============================================
	BOOL writmail::PreTranslateMessage(MSG *pMsg)
// ============================================
{
int x, y, line, len;

	if (tip)	tip.RelayEvent(pMsg);
	if (pMsg->message==WM_KEYDOWN && (GetKeyState(VK_CONTROL) & 0x80000000))
	{
		switch (pMsg->lParam & 0xFF0000)
		{
			case  0x1c0000:	// Ctrl+Enter:
				OnSave();
				return TRUE;
			case  0x040000:	//	Ctrl+#
				InsertAttributes("#");
				return TRUE;
			case  0x090000:	//	Ctrl+*
				InsertAttributes("*");
				return TRUE;
			case  0x0C0000:	//	Ctrl+_
				InsertAttributes("_");
				return TRUE;
			case  0x350000:	//	Ctrl+/
				InsertAttributes("/");
				return TRUE;
			case 0x1e0000:	//	Ctrl+A
	  			x = 0, y = 0, line = m_buffer.GetLineCount() - 1;
				len = m_buffer.GetLineLength(line);
				m_mailtext.SetSelection(CPoint(0, 0), CPoint(len - 1, line));
				return TRUE;
			default:
				return CDialog::PreTranslateMessage(pMsg);
		}
	}
	if (pMsg->message==WM_RBUTTONDOWN)
	{
		if (pMsg->hwnd==m_freezemail.m_hWnd)
		{
			set_cfg(CFG_COMMON,"DefaultFreeze",m_freezemail.GetCheck());
			show_msg(L("S_608"),500);	// scanning mode saved
			return TRUE;
		}
		else if (pMsg->hwnd==m_convert.m_hWnd)
		{
			set_cfg(CFG_COMMON,"CharConversion",m_convert.GetCheck());
			show_msg(L("S_609"),500);	// convertion mode saved
			return TRUE;
		}

		if (HandleRightButton(pMsg))	return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}

// ============================================
	void writmail::OnClipCopy()
// ============================================
{
	m_mailtext.Copy();
}

// ============================================
	void writmail::OnClipCut()
// ============================================
{
	m_mailtext.Cut();
}

// ============================================
	void writmail::OnClipPaste()
// ============================================
{
	m_mailtext.Paste();
}

// ============================================
	void writmail::OnToPaste()
// ============================================
{
	m_to.SetSel(0,-1);
	m_to.Paste();
}

// ============================================
	void writmail::OnSubjectPaste()
// ============================================
{
	m_subject.SetSel(0,-1);
	m_subject.Paste();
}

// ============================================
	void writmail::OnFidoPaste()
// ============================================
{
	m_toaddr.SetSel(0,-1);
	m_toaddr.Paste();
}

// ============================================
	void writmail::OnFindinet()
// ============================================
{
	if (!Mailbuf)	return;

	if (ust.dest_area_handle!=0)
		ERR_MSG_RET("E_IGWCFN");

	find_emails(Mailbuf,EmailList);
	if (EmailList.GetCount()==0)
		ERR_MSG_RET("M_NOINETEADF");

	inafound dlg;
	dlg.DoModal();

	if (gUseThisURL.GetLength()>0)
	{
		m_to.SetWindowText(gUseThisURL);
		HandleInGateMacros();
	}
	EmailList.RemoveAll();
}

// ============================================
	void writmail::OnDestroy()
// ============================================
{
	if (Mailbuf)
	{ 
		free(Mailbuf); 
		Mailbuf=0;
	}
	m_buffer.FreeAll();
	CDialog::OnDestroy();
}

// ============================================
	void writmail::OnUseRecipient()
// ============================================
{
	m_to.SetWindowText(Org_ToName);
	m_toaddr.SetWindowText(Org_ToAddr);
	proc_text_macro();
}

// ============================================
	void writmail::AdjustDlgWidth(void)
// ============================================
{
RECT	re,wre;
CSize	chw;
int		dlgw,wndw;

	GetWindowRect(&re);
	dlgw=re.right-re.left;	// width of editor dialog
	get_fontsize(&font_mailtext,&m_mailtext,chw);
	ResizeElement(IDC_TEXT,chw.cx*(gc.EditorWidth+1),TRUE);
	m_mailtext.GetWindowRect(&wre);
	wndw=wre.right-wre.left;	// actual width of edit field
	ResizeElement(IDC_FOOTER,wndw,FALSE);
	ResizeElement(IDC_HEADER,wndw,FALSE);
	ResizeElement(IDC_ORIGIN,wndw,FALSE);
	if (dlgw<wndw+12)	// adjust Dialogbox width
	{
		re.left=4;
		re.right=re.left+wndw+12;
		MoveWindow(&re);
	}
}

// ============================================
// resize client area of element to newwidth
	void writmail::ResizeElement(int id,int newwidth,BOOL client)
// ============================================
{
RECT re,cre;
int  framewidth=0;

	GetDlgItem(id)->GetWindowRect(&re);
	if (client)
	{
		GetDlgItem(id)->GetClientRect(&cre);
		framewidth=(re.right-re.left)-(cre.right-cre.left);
	}
	re.right=re.left+newwidth+framewidth;
	ScreenToClient(&re);
	GetDlgItem(id)->MoveWindow(&re);
}

// ============================================
// handler for macros in adressbook templates
	void  writmail::proc_macros(LPCSTR tmpl)
// ============================================
// Format:   [SUBJECT] or [S]="bla bla"
// Format:   [TEXT] or [T]="bla bla"
// Format:   [HEADER] or [H]="bla bla"
// Format:   [FOOTER] or [F]="bla bla"
{
CString text;
LPCSTR p,x;

	x=tmpl;
	if (*x==0)	return;

	p=strstr(x,"[SUBJECT]=");
	if (!p)	p=strstr(x,"[S]=");
	if (p && extract_quotes(p,text))	m_subject.SetWindowText(text);
	p=strstr(x,"[TEXT]=");
	if (!p)	p=strstr(x,"[T]=");
	if (p && extract_quotes(p,text))
	{
		m_mailtext.SetWindowText(text);
		template_text=1;
	}
	p=strstr(x,"[HEADER]=");
	if (!p)	p=strstr(x,"[H]=");
	if (p && extract_quotes(p,text))
	{
		m_header.SetWindowText(text);
		header_changed=1;
	}
	p=strstr(x,"[FOOTER]=");
	if (!p)	p=strstr(x,"[F]=");
	if (p && extract_quotes(p,text))
	{
		m_footer.SetWindowText(text);
		footer_changed=1;
	}
}

// ============================================
// extract first quoted string
	int extract_quotes(LPCSTR p, CString &text)
// ============================================
{
char buf[1000],*t,*t1;

	text.Empty();
	t= (char *) strchr(p,'"');
	if (!t)
		return FALSE;
	t++;

	t1=strchr(t,'"');
	if (!t1 || t1-t>999)
		return FALSE;

	memcpy(buf,t,t1-t);
	buf[t1-t]=0;
	text=buf;
	return TRUE;
}

// ============================================
// look for Nickname or Name in addressbook and save result in gc
	int  writmail::search_in_abook(LPCSTR pattern)
// ============================================
{
CString line,name,fido,comment,nickname;

	for (int i=0;i<Abook.GetCount();i++)
	{
		line=Abook.GetString(i);
		get_token(line,0,name);
		get_token(line,3,nickname);

		if (!stricmp(pattern,nickname) || !stricmp(pattern,name))
		{
			get_token(line,1,fido);
			get_token(line,2,comment);
			m_to.SetWindowText(name);
			m_toaddr.SetWindowText(fido);
			proc_macros(comment);
			return 1;
		}
	}
	return 0;
}
	
// ============================================
// look for address in addressbook and save result in gc
	int  writmail::search_in_abook(int zone,int net,int node,int point)
// ============================================
{
CString line,name,fido,comment,nickname,str;

	for (int i=0;i<Abook.GetCount();i++)
	{
		line=Abook.GetString(i);
		get_token(line,1,fido);
		make_address(str,zone,net,node,point);
		if (!stricmp(str,fido))
		{
			get_token(line,0,name);
			get_token(line,2,comment);
			m_to.SetWindowText(name);
			m_toaddr.SetWindowText(fido);
			proc_macros(comment);
			return 1;
		}
	}
	return 0;
}
	
// ====================================================
	void writmail::InsertAttributes(LPCSTR attr)
// ====================================================
{
/*
CString	msg;
long	b,e,len;
const char	*p,*t;

	m_mailtext.GetWindowText(msg);
	m_mailtext.GetSel(b,e);
	t=PS msg;
	p=t+b-1;
	while (p>=t && !strchr(DELIMS,*p))
		p--;
	b=p-t+1;
	m_mailtext.SetSel(b,b);
	ReplaceSelection(&m_buffer, &m_mailtext, attr);
	p=t+e;
	len=msg.GetLength();
	while (p<t+len && !strchr(DELIMS,*p))
		p++;
	e=p-t+1;
	m_mailtext.SetSel(e,e);
	ReplaceSelection(&m_buffer, &m_mailtext, attr);
	m_mailtext.SetSel(e+1,e+1);
*/
}

// ====================================================
	void writmail::OnClose() 
// ====================================================
{
	forceclose=1;	
	CDialog::OnClose();
}
	
// ====================================================
	void writmail::OnHelp()
// ====================================================
{
	WinHelp(VHELP_WRITMAIL);
}

BOOL writmail::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return TRUE;
}

// =======================================================================
// Search e-mail addresses in text in format xxxx@yyyyy.zzz
    void find_emails(char *text, CStrList &lst)
// =======================================================================
{
char	*p,*t;
char	str[100];
BOOL	wasat=FALSE,wasdot=FALSE,start=FALSE;

	p=text;
	t=0;
	while (*p)
	{
		if(__iscsym(*p) || *p=='-')
		{
			if(!t)	t=p;
		}
		else if(*p=='@')
		{
			if(wasat)
			{
				t=0;	// second @
				wasat=FALSE;
				wasdot=FALSE;
			}
			else if(t && p>t)
				wasat=TRUE;
		}
		else if(*p=='.' && wasat)	// dot after @
			wasdot=TRUE;
		else if(*p!='.')
		{
			if(t && wasat && wasdot)
			{
				strncpy(str,t,p-t);
				str[p-t]=0;
				lst.AddTail(str);
			}
			t=0;
			wasat=FALSE;
			wasdot=FALSE;
		}
		p++;
	}
	if(t && wasat && wasdot)
	{
		strncpy(str,t,p-t);
		str[p-t]=0;
		lst.AddTail(str);
	}
}

// =======================================================================
	void remove_kludges(CString &mail)
// =======================================================================
{
int i,j;

	if (!gc.show_tags)
	{
		i=mail.Find('\001',0);
		while (i>=0)
		{
			j=mail.Find("\r\n",i);
			if (j<0)
				j=mail.GetLength()-2;
			mail.Delete(i,j-i+2);
			i=mail.Find('\001',i);
		}
	}
	else
		mail.Replace('\001','@');
}

// =================================================================
	void convert_specchars(CString &str)
// =================================================================
{
CString buf;
char s[300],d[300];
int  num;


	num=converts.GetCount();
	if (num==0)	return;
	for (int i=0;i<num;i++)
	{
		converts.GetString(i,buf);
		get_token(buf,0,s);
		s[1]=0;
		get_token(buf,1,d);
		str.Replace(s,d);
	}
}

// =================================================================
	void writmail::build_kludges(long hnd,mailheader *pmh)
// =================================================================
{
char tmp[100];
ULONG  msg_id;

	Intl.Empty(); Fmpt.Empty(); Topt.Empty();
	if (hnd==0)
	{
		m_toaddr.GetWindowText(tmp,20);
		tmp[20]=0;
		if (parse_address(tmp,&pmh->dstzone,&pmh->dstnet,&pmh->dstnode,&pmh->dstpoint)<3)
		{
			m_toaddr.SetFocus();
			EndWaitCursor();
			ERR_MSG2_RET("E_INVALIDDEST",tmp);
		}

// INTL
		Intl.Format("\001INTL %d:%d/%d %d:%d/%d\r\n",
			pmh->dstzone,pmh->dstnet,pmh->dstnode,pmh->srczone,pmh->srcnet,pmh->srcnode);
// FMPT
		if (pmh->srcpoint)	Fmpt.Format("\001FMPT %d\r\n",pmh->srcpoint);
// TOPT
		if (pmh->dstpoint)	Topt.Format("\001TOPT %d\r\n",pmh->dstpoint);
	}

// MSGID
	Msgtime=time(NULL);
	Msgid.Format("\001MSGID: %s %x",point,Msgtime);

// PID
	Pid.Format("\001PID: %s",get_versioninfo(tmp,0));

// REPLY
	if (gc.mode == MODE_CHANGE)
	{
		if (strlen(reply_addr)>0)
			Reply.Format("\001REPLY: %s %x\r\n",reply_addr,reply_id);
		pmh->replyid=reply_id;
	}
	else
	{
		msg_id=0;
		if (gc.mode != MODE_NEW)
		{
			if (strlen(msg_addr)==0)
				make_address(msg_addr,pmh->srczone,pmh->srcnet,pmh->srcnode,pmh->srcpoint);

			Reply.Format("\001REPLY: %s %x\r\n",msg_addr,msg_id);
		}
		pmh->replyid=msg_id;
	}
// extra kludges
	Exkl=get_extrakludges(TRUE);
}

// ============================================
// extract data from kludge
	BOOL writmail::parse_kludge(LPCSTR kludge,LPSTR addr,ULONG *id)
// ============================================
{
char txt[HDR_LEN],*p;
int  ret;

	*addr=0; *id=0;
	ret=db_get_mailtext(ust.cur_area_handle,&ust.act_mailh,txt,HDR_LEN);
	if (ret != DB_OK)	ERR_MSG2_RET0("E_DBGETMAILTEXT",ret);
	p=strstr(txt,kludge);
	if (!p)	return FALSE;
	sscanf(p+strlen(kludge),"%s %x",addr,id);
	return TRUE;
}

// =================================================================
// build origin line
	void writmail::build_origin(long hnd,LPCSTR addr)
// =================================================================
{
CString str;
int ret;

	Origin.Empty();
	if (gc.mode==MODE_CHANGE)	return;	// change mode
	if (hnd==0 && !get_cfg(CFG_EDIT,"NetmailOrigin",0))	// origin in netmail
	{
		Origin="\r\n";
		return;
	}
	ret=m_origin.GetCurSel();
	if (ret!=LB_ERR)
		m_origin.GetLBText(ret,str);
	else
		m_origin.GetWindowText(str);

	Origin.Format(" * Origin: %65");
	Origin=" * Origin: ";
	Origin+=str.Left(65-strlen(addr));	// max length=79-Len(" * ORIGIN: ")-Len(" ()")
	Origin+=" (";
	Origin+=point;
	Origin+=")";
}

// =================================================================
// build tearline
	void writmail::build_tearline(void)
// =================================================================
{
	Tearline.Empty();
	if (gc.mode==MODE_CHANGE)	return;
	get_tearline(Tearline,FALSE);		
}

// =================================================================
// build route
	BOOL writmail::set_routing(mailheader *pmh)
// =================================================================
{
_fidonumber fn;
CString		str,str1;
USHORT		zone=0,net=0,node=0;
int			err=0;

	if (ust.dest_area_handle==0)	// netmail
	{
		pmh->status &= ~DB_MAIL_ROUTED;
		if (m_attached.GetCheck())	pmh->attrib |= MSGFILE;
		if (m_recipt.GetCheck())	pmh->attrib |= MSGRRQ;
		if (m_killafter.GetCheck())	pmh->attrib |= MSGKILL;
		if (m_direct.GetCheck())
		{
			pmh->attrib |= MSGCRASH;
			if (pmh->dstpoint != 0 && err_out("DY_CMSDTB") == IDYES)	// direct to boss?
			{
				if (!nl_get_fido_by_number(pmh->dstzone,pmh->dstnet,pmh->dstnode,0,&fn))
				{
					err_out("E_CANGNOB",pmh->dstzone,pmh->dstnet,pmh->dstnode);
					return 0;
				}
				if (!expand_address(fn.user,str,str1,1,1,1,err))
				{
					err_out("E_WTTGAFB");
					return 0;
				}
				pmh->status |= DB_MAIL_ROUTED;
				parse_address(str,&zone,&net,&node);
				pmh->zone_net=zone<<16 | net;
				pmh->node=node;
			}
		}
	}
	return 1;
}

// =================================================================
// prepare text for inserting into mail
	CString &writmail::prepare_text(CString &str)
// =================================================================
{
	russian_conversion(str);
	expand_tabs(str);
	str.AnsiToOem();
	return str;
}

// =================================================================
// set recipient field
	void writmail::set_to_box(mailheader *pmh)
// =================================================================
{
CString str;

	if (gc.mode==MODE_NEW || gc.mode==MODE_FORWARD)
		strcpy(pmh->toname,ust.dest_area_handle==0 ? "" : "All");
	gpMain->m_city.GetWindowText(str);
	if (strchr(str,'@'))
		m_to.SetWindowText(str);
	else
	{
		if (gc.mode==MODE_FORWARD || gc.mode==MODE_QUOTE)
			str=pmh->fromname;
		else
			str=pmh->toname;
		str.OemToAnsi();
		m_to.SetWindowText(str);
	}
}

// =================================================================
// tooltips initialisation
	void writmail::InitTooltips(void)
// =================================================================
{
	tip.Create(this);
	tip.SetMaxTipWidth(0xFFFFFF);
	tip.AddTool(GetDlgItem(IDC_ADDRESS),L("S_534"));
	tip.AddTool(&m_attached,L("S_535"));
	tip.AddTool(&m_aliases,L("S_536"));
	tip.AddTool(&m_fidodrop,L("S_537"));
	tip.AddTool(&m_toaddr,L("S_538"));
	tip.AddTool(&m_range,L("S_539"));
	tip.AddTool(&m_recipt,L("S_540"));
	tip.AddTool(&m_direct,L("S_541"));
	tip.AddTool(&m_killafter,L("S_542"));
	tip.AddTool(&m_private,L("S_543"));
	tip.AddTool(&m_convert,L("S_544"));
	tip.AddTool(&m_freezemail,L("S_545"));
	tip.AddTool(&m_header,L("S_546"));
	tip.AddTool(&m_footer,L("S_547"));
	tip.AddTool(&m_origin,L("S_548"));
	tip.AddTool(&m_attach,L("S_549"));
	tip.AddTool(GetDlgItem(IDC_IMPORT),L("S_550"));
	tip.AddTool(GetDlgItem(IDC_INFO),L("S_551"));
	tip.AddTool(GetDlgItem(IDCANCEL),L("S_552"));
	tip.AddTool(GetDlgItem(IDC_SAVE),L("S_553"));
	tip.AddTool(&m_to,L("S_554"));
	tip.AddTool(&m_subject,L("S_555"));
	tip.Activate(!gc.NoTooltips);
}

// =================================================================
	void writmail::fill_aka_combo(int ah)
// =================================================================
{
areadef ad;
char		buf[1000],aka[30];
int			i=0,j=0;

	if (ah==0)	// netmail
	{
// add all main aka's
		while (db_get_boss_by_index(i++,buf))
		{
			get_token(buf,MAINAKA,aka);
			m_fidodrop.AddString(aka);
			if (Org_FromAddr==aka)	j=m_fidodrop.GetCount()-1;
		}
// add secondary aka's
		for (i=0;i<Akas.GetCount();i++)
		{
			strcpy(aka,Akas.GetString(i));
			m_fidodrop.AddString(aka);
			if (Org_FromAddr==aka)	j=m_fidodrop.GetCount()-1;
		}
		if (gc.mode==MODE_CHANGE)
			m_fidodrop.SetCurSel(j);
		else
			set_aka_combo();
	}
	else	// arcmail
	{
		db_get_area_by_index(ah,&ad);
		m_fidodrop.AddString(ad.aka);
		m_fidodrop.SetCurSel(0);
	}
}

// =================================================================
	void writmail::set_aliases(void)
// =================================================================
{
areadef	ad;
CStrList alst;
CString str,tmp,echoes;
int			i,j,k;

	get_fullname(str);
	m_aliases.AddString(str);	// add generic name
	j=0;
	db_get_area_by_index(ust.dest_area_handle,&ad);
// fill aliases list		
	for (i=0;i<Aliases.GetCount();i++)
	{
		str=Aliases.GetString(i);
		trim_all(str);
		alst.RemoveAll();
		k=str.Find('(');
// search alias for current area
		if (k >= 0)
		{
			echoes=str.Mid(k+1);
//			str.Delete(k,str.GetLength()-k);
			str=str.Left(k);
			str.TrimRight();
			k=echoes.Find(')');
			if (k>0)	echoes=echoes.Left(k);
			alst.LoadFromDelimString(echoes,',',TRUE);
			if (j==0 && alst.GetCount()>0 && alst.FindString(ad.echotag,tmp) >= 0)
				j=i+1;	// find corresponding alias
		}
		m_aliases.AddString(str);
	}
// set selection to sender in change mode
	if (gc.mode==MODE_CHANGE)
	{
		str=ust.act_mailh.fromname;
		str.OemToAnsi();
		j=m_aliases.FindStringExact(-1,str);
		if (j == LB_ERR)	j=0;
	}
	m_aliases.SetCurSel(j);
}

// ==============================================
	void writmail::ApplyEditOptions(void)
// ==============================================
{
LOGFONT lf;

	m_mailtext.SetScreenChars(get_cfg(CFG_EDIT, "RightMargin", 0));
	m_mailtext.SetAutoIndent(get_cfg(CFG_EDIT, "AutoIndent", 1));
	m_mailtext.SetOemCharset(IsOem(&font_mailtext));//get_cfg(CFG_EDIT, "OemCharset", 1));
	m_mailtext.SetWordWrapping(get_cfg(CFG_EDIT, "WordWrap", 1));
	m_mailtext.SetTabSize(get_cfg(CFG_EDIT, "TabSize", 4));
	m_mailtext.SetEqualTabs(get_cfg(CFG_EDIT, "EqualTabs", 0));
	m_mailtext.SetReplaceTabs(get_cfg(CFG_EDIT, "ReplaceTabs", 0));
	m_mailtext.SetViewTabs(get_cfg(CFG_EDIT, "ViewTabs", 0));
	m_mailtext.SetColorMark(get_cfg(CFG_EDIT, "ColorMark", 0));
	font_mailtext.GetLogFont(&lf);
	m_mailtext.SetFont(lf);
	//m_mailtext.GetFont(lf);
	//strcpy(lf.lfFaceName, get_cfg(CFG_MAILED, "FontName", "Courier New"));
	//HDC hDC = ::GetDC(m_mailtext.m_hWnd);
	//lf.lfHeight = -MulDiv(get_cfg(CFG_MAILED, "FontSize", 10), GetDeviceCaps(hDC, LOGPIXELSY), 72);
	//::ReleaseDC(m_mailtext.m_hWnd, hDC);
	//m_mailtext.SetFont(lf);
}
