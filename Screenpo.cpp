// This File contains the stuff	for SCREEN Positioning
#include "stdafx.h"
#include "light.h"
#include "lightdlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
#define GETRECT(ctl,re)	ctl.GetWindowRect(&re); ScreenToClient(&re);

CFont   font_ANSI;
CFont   font_OEM;
CFont   font_subject;
CFont   font_labels;
CFont   font_fields;
//extern CFont   font_editor;

extern CLightApp FipsApp;
extern CLightDlg *gpMain;
extern _gconfig gc;

#define TOP_1 130
#define TOP_2 147 /*151*/
#define TOP_3 164 /*172*/
		
// =============================================================
	void CLightDlg::ForceBitmapPosition() 
// =============================================================
{
int	x=2;
int	y;

	m_animail.MoveWindow(x,0,BARIW,TOPIH,0); x+=BARIW+1;
	m_anitoss.MoveWindow(x,0,BARIW,TOPIH,0); x+=BARIW+1;
	m_anipurg.MoveWindow(x,0,BARIW,TOPIH,0); x+=BARIW+1;
	m_prevarea.MoveWindow(x,0,TOPIW,TOPIH,0); x+=TOPIW+1;
	m_nextarea.MoveWindow(x,0,TOPIW,TOPIH,0);
	
	x=2;
	y=TOPIH+1;
	m_newmail.MoveWindow(x,y,BARIW,BARIH,0); y+=BARIH+1;
	m_quote.MoveWindow(x,y,BARIW,BARIH,0); y+=BARIH+1;
	m_quotea.MoveWindow(x,y,BARIW,BARIH,0);y+=BARIH+1;
	m_filter.MoveWindow(x,y,BARIW,BARIH,0); y+=BARIH+1;
	m_autosave.MoveWindow(x,y,BARIW,BARIH,0); y+=BARIH+1;
	m_find.MoveWindow(x,y,BARIW,BARIH,0); y+=BARIH+1;
	m_freq.MoveWindow(x,y,BARIW,BARIH,0);  y+=BARIH+1;
	m_udef1.MoveWindow(x,y,BARIW,BARIH,0); y+=BARIH+1;
	m_udef2.MoveWindow(x,y,BARIW,BARIH,0); y+=BARIH+1;
	m_udef3.MoveWindow(x,y,BARIW,BARIH,0); y+=BARIH+1;
	m_udef4.MoveWindow(x,y,BARIW,BARIH,0); y+=BARIH+1;
	m_udef5.MoveWindow(x,y,BARIW,BARIH,0); y+=BARIH+1;
	m_exit.MoveWindow(x,y,BARIW,BARIH,0); y+=BARIH+1;
	m_abortscript.MoveWindow(x,y,BARIW,BARIH,0);
}

// =============================================================
	void CLightDlg::OnPaint() 
// =============================================================
{
CSize	 csz;
RECT	 re,re1;
int		 x,y,cxIcon,cyIcon,h,bottompos,hsb;

	if (IsIconic())
	{
		CPaintDC dc(this);
		SendMessage(WM_ICONERASEBKGND,(WPARAM)dc.GetSafeHdc(),0);
// Center icon in client rectangle
		cxIcon = GetSystemMetrics(SM_CXICON);
		cyIcon = GetSystemMetrics(SM_CYICON);
		GetClientRect(&re);
		x = (re.right-re.left - cxIcon + 1) / 2;
		y = (re.bottom-re.top - cyIcon + 1) / 2;
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		if (gc.detscript.disableupdate)	goto endit;
		m_statusbar.GetWindowRect(&re);
		hsb=re.bottom-re.top;		// statusbar height
		GetClientRect(&re);
		bottompos=re.bottom;
		if (gc.scale==100)
		{
			HIDE(m_text_from);
			HIDE(m_text_to);
			HIDE(m_text_subject);
			HIDE(m_from);
			HIDE(m_to);
			HIDE(m_create);
			HIDE(m_receive);
			HIDE(m_subject);
			HIDE(m_from_address);
			HIDE(m_to_address);
			HIDE(m_city);
			HIDE(m_show_attributes);
			HIDE(m_show_hidden);
			HIDE(m_show_deleted);
			gc.split_point=re.bottom-re.top;
		}
		else
		{
			SHOW(m_text_from);
			SHOW(m_text_to);
			SHOW(m_text_subject);
			SHOW(m_from);
			SHOW(m_to);
			SHOW(m_create);
			SHOW(m_receive);
			SHOW(m_subject);
			SHOW(m_from_address);
			SHOW(m_to_address);
			SHOW(m_city);
			SHOW(m_show_attributes);
			SHOW(m_show_hidden);
			SHOW(m_show_deleted);
			gc.split_point=(int)((re.bottom-re.top)*(gc.scale-10)/100.+TOPIH+2);
		}
// scalable bottom margin of subjects window
//		gc.split_point+=TOPIH+2;
// adjust top of static-sized elements
// get height of most tall element

		get_fontsize(&font_labels,GetDlgItem(IDC_TEXT_FROM),csz);
		h=csz.cy;
		get_fontsize(&font_fields,GetDlgItem(IDC_FROM),csz);
		if (h<csz.cy)	h=csz.cy;
		adjust_up(m_text_from,gc.top.text_from,h-3);
		adjust_up(m_text_to,gc.top.text_to,h-3);
		adjust_up(m_text_subject,gc.top.text_subject,h-3);
		adjust_up(m_from,gc.top.from,h);
		adjust_up(m_to,gc.top.to,h);
		adjust_up(m_create,gc.top.create,h);
		adjust_up(m_receive,gc.top.receive,h);
		adjust_up(m_subject,gc.top.subject,h);
		adjust_up(m_from_address,gc.top.from_address,h);
		adjust_up(m_to_address,gc.top.to_address,h);
		adjust_up(m_city,gc.top.city,h);
		adjust_up(m_show_attributes,gc.top.show_attr,h-3);
		adjust_up(m_show_hidden,gc.top.show_hidden,h-3);
		adjust_up(m_show_deleted,gc.top.show_deleted,h-3);
		
// adjust top of variable-sized elements
		
		GETRECT(m_subjlist,re);	// subjects window
		re.top=TOPIH+1;
		re.bottom=gc.split_point-2-(gc.scale==100 ? TOPIH : 0);
		m_subjlist.MoveWindow(&re,0);

		GETRECT(m_attrlist,re);	// flags window
		re.top=TOPIH+1;
		re.bottom=gc.split_point-2-(gc.scale==100 ? TOPIH : 0);
		m_attrlist.MoveWindow(&re,0);

		GETRECT(m_subject,re);
		y=re.bottom;			// top margin for mailtext window
		
		GETRECT(m_statusbar,re);
		int hsb=re.bottom-re.top;	// height of statusbar
// mailtext window
		GETRECT(m_mailview,re);
		re.top=y+2;
		re.bottom=bottompos-hsb;				   
		m_mailview.MoveWindow(&re,0);
// statusbar
		re1.left=0;
		re1.right=re.right;
		re1.top=re.bottom;
		re1.bottom=bottompos;
		m_statusbar.MoveWindow(&re1,0);
		int sw=re.right-re.left;
		int sbparts[] = {sw-230,sw-100,sw-50,sw};
		m_statusbar.SetParts(sizeof(sbparts)/sizeof(int),sbparts);

		if (gc.scaleupdate)	// repaint after scaling
		{
			Invalidate();
			gc.scaleupdate=FALSE;
		}
endit:
		CDialog::OnPaint();
	}
}

// ===================================================================
	void CLightDlg::InitializeFonts(void)
// ===================================================================
{
LOGFONT lf;

// fonts for mailtext listbox
	get_font(&lf,"ANSI");
	font_ANSI.DeleteObject();
	font_ANSI.CreateFontIndirect(&lf);
	if (!gc.ViewOem)	set_mailtext_font(&lf);
	get_font(&lf,"OEM",TRUE);
	font_OEM.DeleteObject();
	font_OEM.CreateFontIndirect(&lf);
	if (gc.ViewOem)		set_mailtext_font(&lf);
// font for subject listbox
	get_font(&lf,"SUBJS");
	font_subject.DeleteObject();
	if (font_subject.CreateFontIndirect(&lf))
	{
		m_subjlist.SetFont(&font_subject);
		m_attrlist.SetFont(&font_subject);
		m_echotag.SetFont(&font_subject);
		m_counter.SetFont(&font_subject);
//		m_topbox.SetFont(&font_subject);
	}

/*/ OEM fixed font
	get_font(&lf,"FLISTS");
	font_filelists.DeleteObject();
	font_filelists.CreateFontIndirect(&lf);*/

// font for mailedit control
	//get_font(&lf,"EDITOR");
	//font_editor.DeleteObject();
	//font_editor.CreateFontIndirect(&lf);

// font for labels in main window
	get_font(&lf,"LABELS");
	font_labels.DeleteObject();
	if (font_labels.CreateFontIndirect(&lf))
	{
		m_text_from.SetFont(&font_labels);
		m_text_to.SetFont(&font_labels);
		m_text_subject.SetFont(&font_labels);
		m_receive.SetFont(&font_labels);
		m_create.SetFont(&font_labels);
	}

// font for edit boxes in main window
	get_font(&lf,"FIELDS");
	font_fields.DeleteObject();
	if (font_fields.CreateFontIndirect(&lf))
	{
		m_from.SetFont(&font_fields);
		m_to.SetFont(&font_fields);
		m_subject.SetFont(&font_fields);
		m_from_address.SetFont(&font_fields);
		m_to_address.SetFont(&font_fields);
		m_city.SetFont(&font_fields);
		m_show_attributes.SetFont(&font_fields);
		m_show_hidden.SetFont(&font_fields);
		m_show_deleted.SetFont(&font_fields);
	}
}

// ===================================================================
	void CLightDlg::AdjustItemSizes(void)
// ===================================================================
{
RECT re;
int  rm,x,w;

	if (gc.detscript.disableupdate || !gpMain)	return;	

	GetWindowRect(&re);
	rm=re.right-re.left-10;	// right margin

	GETRECT(m_subjlist,re);
	re.right=rm;
	m_subjlist.MoveWindow(&re);

	GETRECT(m_mailview,re);
	re.right=rm;
	m_mailview.MoveWindow(&re);

// adjust area info fields
	x=adjust_left(m_animail,1);	// left adjusting for animation buttons
	x=adjust_left(m_anitoss,x+1);
	x=adjust_left(m_anipurg,x+1);
	x=adjust_left(m_prevarea,x+1);
	x=adjust_left(m_nextarea,x+1);
	w=rm-x-1;
	GETRECT(m_counter,re);
	w-=re.right-re.left;	// width of free space
	x=adjust_top_element(m_counter,x+5,0);
	x=adjust_top_element(m_echotag,x,w);

	adjust_right(m_subject,adjust_right(m_city,rm)-4,TRUE);
	adjust_right(m_create,rm);
	rm=adjust_right(m_receive,rm)-4;
	adjust_right(m_from_address,rm);
	rm=adjust_right(m_to_address,rm)-4;

	GETRECT(m_from,re);
	re.right=rm;
	m_from.MoveWindow(&re);

	GETRECT(m_to,re);
	re.right=rm;
	m_to.MoveWindow(&re);

}

// =============================================================
	void CLightDlg::init_top_offsets(void)
// =============================================================
{
   	gc.top.text_from	=TOP_1;
	gc.top.from			=TOP_1;
	gc.top.from_address	=TOP_1;
	gc.top.create		=TOP_1;
	gc.top.show_attr	=TOP_1;
	gc.top.text_to		=TOP_2;
	gc.top.to			=TOP_2;
	gc.top.to_address	=TOP_2;
	gc.top.receive		=TOP_2;
	gc.top.show_hidden	=TOP_2;
	gc.top.text_subject	=TOP_3;
	gc.top.subject		=TOP_3;
	gc.top.city			=TOP_3;
	gc.top.show_deleted	=TOP_3;
}

// =============================================================
	void CLightDlg::OnSize(UINT nType, int cx, int cy) 
// =============================================================
{
	if (gc.detscript.disableupdate)	return;
	CDialog::OnSize(nType, cx, cy);
	AdjustItemSizes();		
	Invalidate();
}

// =============================================================
	void CLightDlg::resizing_on_hide_toolbox(int save) 
// =============================================================
{
static RECT re_small;	
static RECT re_list;	
static RECT re_displist;	
static RECT re_text_from;	
static RECT re_text_to;	
static RECT re_text_subj;	
static RECT re_from;	
static RECT re_to;	
static RECT re_subject;	
static RECT re_from_addr;
static RECT re_to_address;
static RECT re_create;
static RECT re_receive;
static RECT re_city;
static RECT re_show_attributes;
static RECT re_show_hidden;
static RECT re_show_deleted;
//static RECT re_statusbar;

int		x,x1,x2,x3,w1,w2,w3;
RECT	re;
SIZE	sz;

	GetMenu()->CheckMenuItem(ID_SWITCH_LEFTICONS, MF_BYCOMMAND | (gc.HideIcons ? MF_UNCHECKED : MF_CHECKED));
// resizing info fields
	CDC *pDC=GetDC();
	CFont *pFont=pDC->SelectObject(&font_labels);	// font of labels
	GetTextExtentPoint32(pDC->m_hDC,"W",10,&sz);	// width of middle labels
	w1=sz.cx;
	GetTextExtentPoint32(pDC->m_hDC,"O",17,&sz);	// width of left middle fields
	w2=sz.cx;
	pDC->SelectObject(&font_fields);							// font of fields
	GetTextExtentPoint32(pDC->m_hDC,"O",22,&sz);	// width of right middle fields
	w3=sz.cx;
	pDC->SelectObject(pFont);

	if (save)
	{
		x=BARIW+2;
//		adjust_left(m_statusbar,x,TRUE);
//		GETRECT(m_statusbar,re_statusbar);
		adjust_left(m_mailview,x,TRUE);
		GETRECT(m_mailview,re_displist);
		x3=re_displist.right;
		adjust_left(m_show_attributes,x);
		GETRECT(m_show_attributes,re_show_attributes);
		adjust_left(m_show_hidden,x);
		GETRECT(m_show_hidden,re_show_hidden);
		adjust_left(m_show_deleted,x);
		GETRECT(m_show_deleted,re_show_deleted);
		x=adjust_left(m_attrlist,x);
		GETRECT(m_attrlist,re_small);
		adjust_left(m_subjlist,x+1,TRUE);
		GETRECT(m_subjlist,re_list);
		adjust_left(m_text_from,x+1,FALSE,w1);
		GETRECT(m_text_from,re_text_from);
		adjust_left(m_text_to,x+1,FALSE,w1);
		GETRECT(m_text_to,re_text_to);
		x1=adjust_left(m_text_subject,x+1,FALSE,w1);
		GETRECT(m_text_subject,re_text_subj);
		adjust_right(m_create,x3,FALSE,w3);
		GETRECT(m_create,re_create);
		x=adjust_right(m_receive,x3,FALSE,w3);
		GETRECT(m_receive,re_receive);
		adjust_right(m_from_address,x-1,FALSE,w2);
		GETRECT(m_from_address,re_from_addr);
		x2=adjust_right(m_to_address,x-1,FALSE,w2);
		GETRECT(m_to_address,re_to_address);
		adjust_left(m_from,x1+3,FALSE,x2-x1-3);
		GETRECT(m_from,re_from);
		adjust_left(m_to,x1+3,FALSE,x2-x1-3);
		GETRECT(m_to,re_to);
		adjust_left(m_subject,x1+3,FALSE,x2-x1-3);
		GETRECT(m_subject,re_subject);
		adjust_left(m_city,x2-1,FALSE,x3-x2+1);
		GETRECT(m_city,re_city);
		return;
	}

#define LEFT 2
	if (gc.HideIcons)
	{
		m_attrlist.GetWindowRect(&re);
		adjust_left(m_attrlist,LEFT);
		adjust_left(m_subjlist,LEFT+re.right-re.left+1,TRUE);
		adjust_left(m_mailview,LEFT,TRUE);
		adjust_left(m_show_attributes,LEFT);
		adjust_left(m_show_hidden,LEFT);
		x=adjust_left(m_show_deleted,LEFT);
		adjust_left(m_text_from,x+1);
		adjust_left(m_text_to,x+1);
		x=adjust_left(m_text_subject,x+1);
		adjust_left(m_from,x+1);
		adjust_left(m_to,x+1);
		adjust_left(m_subject,x+1);
		Invalidate();
	}
	else
	{
		m_attrlist.MoveWindow		(&re_small);						
		m_subjlist.MoveWindow		(&re_list);					
		m_mailview.MoveWindow		(&re_displist);			
		m_text_from.MoveWindow		(&re_text_from);			
		m_text_to.MoveWindow		(&re_text_to);					
		m_text_subject.MoveWindow	(&re_text_subj);
		m_from.MoveWindow			(&re_from);				
		m_to.MoveWindow				(&re_to);							
		m_subject.MoveWindow		(&re_subject);			
		m_from_address.MoveWindow	(&re_from_addr);			
		m_to_address.MoveWindow		(&re_to_address);			
		m_create.MoveWindow			(&re_create);			
		m_receive.MoveWindow		(&re_receive);
		m_city.MoveWindow			(&re_city);
		m_show_attributes.MoveWindow(&re_show_attributes);
		m_show_hidden.MoveWindow	(&re_show_hidden);
		m_show_deleted.MoveWindow	(&re_show_deleted);
		
		AdjustItemSizes();
		Invalidate();
 	}
}

// =============================================================
// adjust top margin of element to split_point+offset-130
	void CLightDlg::adjust_up(CWnd &wnd,int offset,int height)
// =============================================================
{
RECT	re;
int		h;

	GETRECT(wnd,re); 
	if (!height)
		h=re.bottom-re.top;
	else
		h=height;
	re.top=gc.split_point+offset-130;
	re.bottom=re.top+h;
	wnd.MoveWindow(&re,0);
}

// =============================================================
// adjust right margin of element to offset with fixed or not left margin
	int CLightDlg::adjust_right(CWnd &wnd,int offset,BOOL fix_left,int width)
// =============================================================
{
RECT	re;
int		w;

	GETRECT(wnd,re);
	if (!width)
		w=re.right-re.left;
	else
		w=width;
	re.right=offset;
	if (!fix_left)
		re.left=offset-w;
	wnd.MoveWindow(&re);
	return re.left;
}

// =============================================================
// adjust left margin of element to offset with fixed or not right margin
	int CLightDlg::adjust_left(CWnd &wnd,int offset,BOOL fix_right,int width)
// =============================================================
{
RECT	re;
int		w;

	GETRECT(wnd,re);
	if (!width)
		w=re.right-re.left;
	else
		w=width;
	re.left=offset;
	if (!fix_right)
		re.right=offset+w;
	wnd.MoveWindow(&re,1);
	return re.right;
}

// =============================================================
// adjust height of element to height
	void CLightDlg::adjust_height(CWnd &wnd,int height)
// =============================================================
{
RECT	re;

	GETRECT(wnd,re); 
	re.bottom=re.top+height;
	wnd.MoveWindow(&re,0);
}

// =============================================================
// adjust left and height of top elements
	int CLightDlg::adjust_top_element(CWnd &wnd,int left,int width)
// =============================================================
{
RECT	re;
int		w;

	GETRECT(wnd,re);
	w=re.right-re.left;
	re.left=left;
	if (width>0)
		re.right=re.left+width;
	else
		re.right=re.left+w;
	re.top=0;
	re.bottom=TOPIH;
	wnd.MoveWindow(&re);
	return re.right;
}