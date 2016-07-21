// again.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// again dialog

class again : public CDialog
{
// Construction
public:
	again(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(again)
	enum { IDD = IDD_REPOST };
	CEdit	m_edit_days;
	int		period;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(again)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(again)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnDefault();
	afx_msg void On1day();
	afx_msg void On3days();
	afx_msg void OnWeek();
	afx_msg void OnMonth();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
