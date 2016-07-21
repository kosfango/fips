// myedit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// myedit window

class myedit : public CEdit
{
// Construction
public:
	myedit();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(myedit)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~myedit();

	// Generated message map functions
protected:
	//{{AFX_MSG(myedit)
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
