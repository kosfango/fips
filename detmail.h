// detmail.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// detmail dialog

class detmail : public CDialog
{
// Construction
public:
	detmail(CWnd* pParent = NULL);   // standard constructor
	void	OnUpdateValues			(void); 
	void	OnAddToListBox			(char *text); 
	void	build_flags_string		(LPCSTR path,LPCSTR init,CString &type);
	void	SetLanguage				(void);
	void	SetButtonsState			(void);
	void	update_left_side		(void);
	void	update_right_side		(void);
	void	update_queue			(void);
	void	GraphicalStatusDisplay	(int force=0); 
	void	RefreshTips				(void);
	void	SavePosition			(void);
	void	fill_queuelist			(void);
	afx_msg void OnRescan();
	afx_msg void OnStart();
	afx_msg void OnAbortsession();
	afx_msg void OnShowQueue();
	afx_msg void OnShowEvents();
	afx_msg void OnStartEvents();
	afx_msg void OnStopEvents();
	afx_msg void OnSetupEvents();
	CToolTipCtrl tip;
	int		show_events;

// Dialog Data
	//{{AFX_DATA(detmail)
	enum { IDD = IDD_STAT_MAILER };
	CStatic	m_isdnout;
	CStatic	m_mod2out;
	CStatic	m_mod1out;
	CStatic	m_isdnin;
	CStatic	m_mod2in;
	CStatic	m_mod1in;
	CEdit	m_cps;
	CEdit	m_timeelapsed;
	CEdit	m_transferred;
	CEdit	m_total;
	CEdit	m_transname;
	CEdit	m_cps2;
	CEdit	m_timeelapsed2;
	CEdit	m_transferred2;
	CEdit	m_total2;
	CEdit	m_transname2;
	CProgressCtrl	m_proz2;
	CListBox	m_eventlog;
	CListBox	m_queue;
	CEdit	m_system;
	CButton	m_startstop;
	CEdit	m_status;
	CProgressCtrl	m_proz;
	CListBox	m_list;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(detmail)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(detmail)
	virtual void OnCancel();
	afx_msg void OnClear();
	virtual BOOL OnInitDialog();
	afx_msg void OnSkipItem();
	afx_msg void OnDelete();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDblclkQueue();
	afx_msg void OnSelchangeQueue();
	afx_msg void OnHelp();
	afx_msg void OnChangeStatus();
	afx_msg void OnChangeSystem();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnPaint();
	afx_msg void OnClose();
	afx_msg	void OnSize  (UINT nType, int cx, int cy); 
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnSkipfile();
	afx_msg void OnRemovefile();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
