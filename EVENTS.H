// events.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// events dialog

class events : public CDialog
{
// Construction
public:
	events(CWnd* pParent = NULL);   // standard constructor
	void enable_has_changed	(void);
	int	 scheduled;
	char fido_if_scheduled[50];

// Dialog Data
	//{{AFX_DATA(events)
	enum { IDD = IDD_EVENTS };
	CButton	m_delete_after_success;
	CListBox	m_list;
	CEdit	m_edit_to;
	CEdit	m_edit_outwait;
	CEdit	m_edit_maxtry;
	CEdit	m_edit_inwait;
	CEdit	m_edit_eventname;
	CEdit	m_edit_from;
	CEdit	m_edit_action_if_ok;
	CEdit	m_edit_action_if_failed;
	CEdit	m_edit_caution;
	CEdit	m_edit_boss3;
	CEdit	m_edit_boss2;
	CEdit	m_edit_boss1;
	CButton	m_event_enabled;
	CButton	m_check_we;
	CButton	m_check_th;
	CButton	m_check_tu;
	CButton	m_check_su;
	CButton	m_check_sa;
	CButton	m_check_mo;
	CButton	m_check_fr;
	//}}AFX_DATA

private:
	void set_wnd_text(CString &str,int id,CWnd &wnd);
	void set_btn_check(CString &str,int id,CButton &btn);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(events)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(events)
	virtual BOOL OnInitDialog();
	afx_msg void OnAddNewEntry();
	afx_msg void OnDeleteEntry();
	afx_msg void OnDisableAllEvents();
	afx_msg void OnEnableAllEvents();
	afx_msg void OnChooseFileActionFailed();
	afx_msg void OnChooseFileActionOk();
	afx_msg void OnSet();
	virtual void OnCancel();
	afx_msg void OnHelp();
	virtual void OnOK();
	afx_msg void OnSelchangeList();
	afx_msg void OnRestartSequence();
	afx_msg void OnStatusreport();
	afx_msg void OnAsdefault();
	afx_msg void OnAbortCurrentEvent();
	afx_msg void OnBrowseBoss();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
