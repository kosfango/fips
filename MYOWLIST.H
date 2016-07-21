
#ifndef MyOwlListINCLUDED
	#define MyOwlListINCLUDED

// myowlist.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// myowlist window

class myowlist : public CListBox
{
// Construction
public:
	myowlist();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(myowlist)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~myowlist();

	// Generated message map functions
protected:
	//{{AFX_MSG(myowlist)
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

#endif
