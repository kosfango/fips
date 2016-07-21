// quickset.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// quickset dialog

class quickset : public CDialog
{
// Construction
public:
	quickset(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(quickset)
	enum { IDD = IDD_QUICK_SETUP };
	CComboBox	m_language;
	CEdit	m_bossname;
	CEdit	m_sessionpsw;
	CEdit	m_bossphone;
	CComboBox	m_packertype;
	CEdit	m_point;
	CEdit	m_init1;
	CEdit	m_bossnode;
	CEdit	m_echofilename;
	CEdit	m_dialcommand;
	CComboBox	m_comport;
	CEdit	m_phone;
	CEdit	m_systname;
	CEdit	m_name;
	CEdit	m_surname;
	CEdit	m_location;
	CEdit	m_bosssyst;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(quickset)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(quickset)
	virtual BOOL OnInitDialog();
	afx_msg void OnHelp();
	virtual void OnOK();
	afx_msg void OnKillfocusPoint();
	afx_msg void OnKillfocusNode();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnSelendokLang();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
