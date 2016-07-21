// info_adr.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// info_adr dialog

class info_adr : public CDialog
{
// Construction
public:
	info_adr(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(info_adr)
	enum { IDD = IDD_INFO_ADDRESS };
	CEdit	m_time;
	CEdit	m_info;
	CEdit	m_name;
	CEdit	m_fido;
	CEdit	m_bbs;
	CEdit	m_location;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(info_adr)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(info_adr)
	afx_msg void OnSearch();
	virtual BOOL OnInitDialog();
	afx_msg void OnHelp();
	afx_msg void OnExtendedSearch();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
