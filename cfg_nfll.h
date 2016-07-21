// cfg_nfll.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// cfg_nfll dialog

class cfg_nfll : public CDialog
{
// Construction
public:
	cfg_nfll(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(cfg_nfll)
	enum { IDD = IDD_ADD_FILELIST };
	CStatic	m_path;
	CEdit	m_direct;
	CListBox	m_extract;
	CListBox	m_dir;
	CEdit	m_fido;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(cfg_nfll)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(cfg_nfll)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnExtract();
	virtual void OnCancel();
	afx_msg void OnDblclkList1();
	afx_msg void OnBrowse();
	afx_msg void OnSearch();
	afx_msg void OnHelp();
	afx_msg void OnFlistPath();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
