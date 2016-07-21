// lightdlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLightDlg dialog

#include "owlist.h"
#include "sbutton.h"
#include "TrayIcon.h"
#include "afxwin.h"

class CLightDlg : public CDialog
{
// Construction
public:
	CLightDlg(CWnd* pParent = NULL);	// standard constructor
	void ForceBitmapPosition		(void);
	void InitializeFonts			(void);
	void AdjustItemSizes			(void);
	void make_string_list			(char *text,CStrList &lst);
	int  begin_area_selection		(BOOL skip_dlg,BOOL as_list=FALSE);
	int  finish_area_selection		(void);
	int  show_subjects				(int area);
	void show_mail					(_ustat *gu);
	void free_all_stuff				(void);
	void add_marker					(void);
	void OnDupeCheckAll				(void);
	BOOL HandleRightButton			(MSG *pMsg);
	void FindOriginalMail			(void);
	void MarkAllAreasAsRead			(void);
	void OnIncomingModem1			(void);
	void OnIncomingModem2			(void);
	void SetLanguage				(void);
	void select_area_by_index		(int ind);
	void select_area_by_name		(LPCSTR str);
	int	 exec_script_key			(LPCSTR pref,int num);
	int  execute_script				(LPCSTR name);
	int  handle_autoexec			(void);
	void select_first_mail_in_area	(void);
	void force_current_mail_update	(void);
	void MailSlotAlternateWrite		(LPSTR buf);
	void do_about					(void);
	void OnInfoAdress				(void);
	void OnExit						(void);
	int  handle_startup_script		(LPSTR name);
	void SetNewMenu					(void); // 0==ENG,1==GER,2==RUS,3==FRE
	int  switch_left_icons			(void);
	void resizing_on_hide_toolbox	(int save);
	void MarkAllMailsAsDeleted		(void);
	void MailSort					(int from_script_file=0);
	void BuildUserWish				(CString &result,BOOL useselection); 
	void AppendMailTofile			(LPCSTR path); 
	void adjust_up					(CWnd &wnd,int offset,int height=0);
	int  adjust_right				(CWnd &wnd,int offset,BOOL fix_left=FALSE,int width=0);
	int  adjust_left				(CWnd &wnd,int offset,BOOL fix_right=FALSE,int width=0);
	void adjust_height				(CWnd &wnd,int height);
	int  adjust_top_element			(CWnd &wnd,int left,int width);
	void get_areainfo				(areadef &,CString &,int current=0);
	void show_mail_by_index			(int ind,int mode);
	void select_prevarea			(void);
	void select_nextarea			(void);
	int  execute_a_single_line		(LPSTR line,int *iResult,LPSTR pResult,CStrList *pscname,int *SelLine);
	int  eval						(CStrList *pscname,int *line,LPSTR str,LPSTR Name);
	int  dowhile					(CStrList *pscname,int *SelLine);
	int  ifelse						(CStrList *pscname,int *SelLine);
	int  dogoto						(CStrList *pscname,int *SelLine);
	void OnSetEng					(void);
	void OnSetGer					(void);
	void OnSetRus					(void);
	void OnSetFre					(void);
	void RefreshTips				(void);
	void RefreshInfoTips			(void);
	void RefreshSubjects			(void);
	void RefreshAreatag				(void);
// user messages handlers
	afx_msg LRESULT OnRefreshAfterEdit(WPARAM,LPARAM);
// standard handlers
	afx_msg void OnAsave();
	afx_msg void OnFind();
	afx_msg void OnNew();
	afx_msg void OnQuote();
	afx_msg void OnQuoteA();
	afx_msg void OnFreq();
	afx_msg void OnMacro1();
	afx_msg void OnMacro2();
	afx_msg void OnMacro3();
	afx_msg void OnMacro4();
	afx_msg void OnMacro5();
	afx_msg void OnSelchangeList();
	afx_msg void OnDblclkList();
	afx_msg void OnChangeCurrentMail();
	afx_msg void OnFilter();
	afx_msg void OnFilterSet();
	afx_msg void MarkAllMailsAsRead();
	afx_msg void OnAbortscript();
	afx_msg void OnAnswerAsNetmail();
	afx_msg void OnAlt10();
	afx_msg void OnHideicons();
	afx_msg void OnToggleStruct();
	afx_msg void OnDupeCheck();
	afx_msg void OnArea();
	afx_msg void OnUpdateScreenDisplay();
	afx_msg void OnPrevious();
	afx_msg void OnNext();
	afx_msg void OnFilerequest();
	afx_msg void OnDeleteSelected();
	afx_msg void OnNeverDelete();
	afx_msg void OnConfig();

	CColorListBoxSubject	m_subjlist;	// subclassed control for subject list
	CColorListBoxSubject	m_attrlist;	// subclassed control for mail attributes list
	CColorListBox			m_mailview;
	CStatusBarCtrl			m_statusbar;
	CMenu					m_menu;
//	CMultiColumnComboBox m_topbox;
//	CString				 m_topboxstring;

// Dialog Data
	//{{AFX_DATA(CLightDlg)
	enum { IDD = IDD_MAIN_WINDOW };
	SButton	m_text_subject;
	SButton	m_text_to;
	SButton	m_text_from;
	SButton m_animail;
	SButton	m_anitoss;
	SButton	m_anipurg;
	SButton	m_show_attributes;
	SButton	m_show_hidden;
	SButton	m_show_deleted;
	SButton m_autosave;
	SButton m_prevarea;
	SButton m_nextarea;
	SButton m_find;
	SButton m_newmail;
	SButton m_quote;
	SButton m_quotea;
	SButton m_udef1;
	SButton m_udef2;
	SButton m_udef3;
	SButton m_udef4;
	SButton m_udef5;
	SButton m_freq;
	SButton m_exit;
	SButton m_abortscript;
	SButton	m_filter;
	//SButton	m_coding;
	CStatic	m_echotag;
	CStatic	m_counter;
	CStatic	m_from;
	CStatic	m_subject;
	CStatic	m_to;
	CStatic	m_city;
	CStatic	m_create;
	CStatic	m_from_address;
	CStatic	m_receive;
	CStatic	m_to_address;
	CToolTipCtrl	tip;
	//}}AFX_DATA

	void startup_init(void);
	void init_top_offsets(void);
	void select_area(BOOL skipdlg,BOOL as_list);
	void create_button(SButton &ctl,int id,int resid);
	void create_switch(SButton &ctl,int id);
	void set_images(void);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLightDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

protected:
	HICON m_hIcon;
	CTrayIcon *pTray;

// Implementation
protected:
	
	// Generated message map functions
	//{{AFX_MSG(CLightDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSize	(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	virtual void OnOK();
	afx_msg void OnSetfocusSmall();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnFindNext();
	afx_msg void OnClose();
	afx_msg void OnUpdateToAddress();
	virtual void OnCancel();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnNextAvailSer();
	afx_msg void OnDblclkMailtext();
	afx_msg void OnCopyFromToClipboard();
	afx_msg void OnCopyToToClipboard();
	afx_msg void OnCopySubjectToClipboard();
	afx_msg void OnInfoSender();
	afx_msg void OnInfoRecipient();
	afx_msg void OnLastScale();
	afx_msg void OnDeliveryTime();
	afx_msg void OnToggleUsermark();
	afx_msg void OnShowAttributes();
	afx_msg void OnShowHidden();
	afx_msg void OnShowDeleted();
	afx_msg void OnCtrlReturn();
	afx_msg void OnSwitchAttr();
	afx_msg void OnSwitchKludge();
	afx_msg void OnSwitchDel();
	afx_msg void OnEchotag();
	afx_msg void OnCounter();
	afx_msg void OnDescription();
	afx_msg void OnAnimail();
	afx_msg void OnAnipurge();
	afx_msg void OnAnitoss();
	afx_msg void OnSpacePressed();
	afx_msg void OnPlusPressed();
	afx_msg void OnMinusPressed();
	afx_msg void OnMultiplyPressed();
	afx_msg void OnDividePressed();
	afx_msg void OnBadXfer();
	afx_msg void OnRestore();
	afx_msg void OnSwitchCoding();
	afx_msg void OnSaveOrigin();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnTextFrom();
	afx_msg void OnTextTo();
	afx_msg void OnTextSubject();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLbnSelchangeAttrlist();
};
