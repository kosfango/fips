// filereq.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// filereq dialog

class filereq : public CDialog
{
// Construction
public:
	filereq(CWnd* pParent = NULL);   // standard constructor
	void Rescan();
	void expand_fido_adress();
	void dispay_systems_with_filelists(); 

// Dialog Data
	//{{AFX_DATA(filereq)
	enum { IDD = IDD_FREQ };
	CListBox	m_filelist;
	CEdit		m_systemname;
	CEdit		m_password;
	CEdit		m_filename;
	CEdit		m_address;
	CListBox	m_list_request;
	CListBox	m_list_fido;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(filereq)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL added;

	// Generated message map functions
	//{{AFX_MSG(filereq)
	afx_msg void OnAdd();
	afx_msg void OnDelete();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeFido();
	afx_msg void OnKillfocusAddress();
	afx_msg void OnDblclkFilelist();
	afx_msg void OnAddNewFileList();
	afx_msg void OnDeleteFileList();
	afx_msg void OnUpdateAddress();
	afx_msg void OnInfoAddress();
	afx_msg void OnHelp();
	afx_msg void OnSchedule();
	afx_msg void OnExternalTool();
	afx_msg void OnExternalToolConfig();
	afx_msg void OnInfoOtherlines();
	afx_msg void OnPatternsearch();
	afx_msg void OnSize	(UINT nType, int cx, int cy); 
	afx_msg void OnPaint(); 
	afx_msg void OnSelchangeFilelist();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
