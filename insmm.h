// insmm.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// insmm dialog

class insmm : public CDialog
{
// Construction
public:
	insmm(CWnd* pParent = NULL);   // standard constructor
	void DisplayNewTypeList	(void); 	  

// Dialog Data
	//{{AFX_DATA(insmm)
	enum { IDD = IDD_MM_SELECTION };
	CListBox	m_mm_list;
	int		m_mmtype;
	int		m_mmdisplay;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(insmm)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(insmm)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg void OnHelp();
	virtual void OnOK();
	afx_msg void OnRadioWave();
	afx_msg void OnRadioBmps();
	afx_msg void OnTest();
	afx_msg void OnDblclkList();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
