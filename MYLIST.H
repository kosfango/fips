// mylist.h : header file
//

#ifndef CMyList_DEFINED
	#define CMyList_DEFINED 1
	/////////////////////////////////////////////////////////////////////////////
	// CMyList window

	class CMyList : public CListBox
	{
	// Construction
	public:
		CMyList();

	// Attributes
	public:

	// Operations
	public:

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CMyList)
		//}}AFX_VIRTUAL

	// Implementation
	public:
		virtual ~CMyList();

		// Generated message map functions
	protected:
		//{{AFX_MSG(CMyList)
		afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
		//}}AFX_MSG

		DECLARE_MESSAGE_MAP()
	};

#endif