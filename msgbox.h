// msgbox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// msgbox dialog

class msgbox : public CDialog
{
// Construction
public:
	msgbox(CWnd* pParent = NULL);   // standard constructor
	CString title;
	int     delay_mult;

// Dialog Data
	//{{AFX_DATA(msgbox)
	enum { IDD = IDD_MESSAGEBOX };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(msgbox)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(msgbox)
	afx_msg void OnTimer(UINT nIDEvent);
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
