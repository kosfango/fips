// myeditfr.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// myeditfr window

#ifndef DEFINED_MYEDITFR
 #define DEFINED_MYEDITFR 1

class myeditfr : public CEdit
{
// Construction
public:
	myeditfr();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(myeditfr)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~myeditfr();

	// Generated message map functions
protected:
	//{{AFX_MSG(myeditfr)
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

#endif
