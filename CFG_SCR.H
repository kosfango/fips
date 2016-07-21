// cfg_scr.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// cfg_scr dialog

class cfg_scr : public CDialog
{
// Construction
public:
	cfg_scr(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(cfg_scr)
	enum { IDD = IDD_SCRIPT_EDITOR };
	CListBox	m_commands;
	CEdit	m_udef1;
	CEdit	m_udef2;
	CEdit	m_udef3;
	CEdit	m_udef4;
	CEdit	m_udef5;
	CEdit	m_udef6;
	CEdit	m_udef7;
	CEdit	m_udef8;
	CEdit	m_udef9;
	CEdit	m_udef10;
	CEdit	m_udef11;
	CEdit	m_udef12;
	CEdit	m_udef13;
	CEdit	m_udef15;
	CEdit	m_udef16;
	CEdit	m_edit;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(cfg_scr)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void select_script(CWnd &ctl);
	void load_script(CString &scr);
	// Generated message map functions
	//{{AFX_MSG(cfg_scr)
	virtual BOOL OnInitDialog();
	afx_msg void OnLoad();
	afx_msg void OnSave();
	virtual void OnCancel();
	afx_msg void OnUdefFind1();
	afx_msg void OnUdefFind2();
	afx_msg void OnUdefFind3();
	afx_msg void OnUdefFind4();
	afx_msg void OnUdefFind5();
	afx_msg void OnUdefFind6();
	afx_msg void OnUdefFind7();
	afx_msg void OnUdefFind8();
	afx_msg void OnUdefFind9();
	afx_msg void OnUdefFind10();
	afx_msg void OnUdefFind11();
	afx_msg void OnUdefFind12();
	afx_msg void OnUdefFind13();
	afx_msg void OnUdefFind15();
	afx_msg void OnUdefFind16();
	afx_msg void OnDblclkList1();
	afx_msg void OnHelp();
	virtual void OnOK();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
