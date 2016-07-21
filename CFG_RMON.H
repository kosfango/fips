// cfg_rmon.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// cfg_rmon dialog

class cfg_rmon : public CDialog
{
// Construction
public:
	cfg_rmon(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(cfg_rmon)
	enum { IDD = IDD_CFG_REMOTE };
	CButton	m_check;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(cfg_rmon)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(cfg_rmon)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnHelp();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
