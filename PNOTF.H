// pnotf.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// pnotf dialog

class pnotf : public CDialog
{
// Construction
public:
	pnotf(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(pnotf)
	enum { IDD = IDD_PATTERNNOTFOUND };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(pnotf)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(pnotf)
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
