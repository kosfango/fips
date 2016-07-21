// hatch.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// hatch dialog

class hatch : public CDialog
{
// Construction
public:
	hatch(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(hatch)
	enum { IDD = IDD_HATCHDLG };
	CEdit	m_edit_magic;
	CButton	m_deleteafter;
	CListBox	m_list;
	CEdit	m_filename;
	CEdit	m_description;
	CEdit	m_replace;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(hatch)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(hatch)
	virtual BOOL OnInitDialog();
	afx_msg void OnBrowse();
	virtual void OnCancel();
	afx_msg void OnHelp();
	virtual void OnOK();
	afx_msg void OnImportDiz();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};
