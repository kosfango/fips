// ecolst.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// ecolst dialog

class ecolst : public CDialog
{
// Construction
public:
	ecolst(CWnd* pParent = NULL);   // standard constructor
	void display_boss_configs();
	void display_the_area_stuff_for_actual_boss		();
	void build_available_echotags					();
	int  do_we_have_this_area_subscribed			(char *);
	void build_diff_list							(void); 
	int  build_afix_netmail							(int mode);
	void handle_uplink_remove_for_this_area			(char *area);
	
	
// Dialog Data
	//{{AFX_DATA(ecolst)
	enum { IDD = IDD_ECHO_FILELIST };
	CListBox	m_arealist;
	CListBox	m_bosslist;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ecolst)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(ecolst)
	virtual BOOL OnInitDialog();
	afx_msg void OnSend();
	afx_msg void OnRequestEchoList();
//	virtual void OnOK();
	afx_msg void OnSelchangeBoss();
	afx_msg void OnDblclkarealist();
	afx_msg void OnSendComplete();
	afx_msg void OnBosscfg();
	afx_msg void OnAreacfg();
	afx_msg void OnManualOrder();
	afx_msg void OnHelp();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
