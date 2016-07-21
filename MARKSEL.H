// marksel.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// marksel dialog

class marksel : public CDialog
{
// Construction
public:
	marksel(CWnd* pParent = NULL);   // standard constructor
	void	get_selection(void);
protected:
	int		m_seltype;	// 0=sel 1=marked 2=both

// Dialog Data
	//{{AFX_DATA(marksel)
	enum { IDD = IDD_CHOOSE_MARK };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(marksel)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(marksel)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg void OnOnSelected();
	afx_msg void OnOnMarked();
	afx_msg void OnOnBoth();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
