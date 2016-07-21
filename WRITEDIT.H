// writedit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// writedit window

class writedit : public CRichEditCtrl
{
// Construction
public:
	writedit();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(writedit)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~writedit();

	// Generated message map functions
protected:
	//{{AFX_MSG(writedit)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
