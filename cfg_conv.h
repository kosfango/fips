// cfg_conv.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// cfg_conv dialog

class cfg_conv : public CDialog
{
// Construction
public:
	cfg_conv(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(cfg_conv)
	enum { IDD = IDD_CHAR_CONVERSION };
	CListBox	m_list;
	CButton	m_enabled;
	CEdit	m_replace;
	CEdit	m_char;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(cfg_conv)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(cfg_conv)
	virtual BOOL OnInitDialog();
	afx_msg void OnAdd();
	afx_msg void OnDelete();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnHelp();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
