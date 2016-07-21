// al_exist.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// al_exist dialog

class al_exist : public CDialog
{
// Construction
public:
	al_exist(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(al_exist)
	enum { IDD = IDD_CHOOSE_SAVE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(al_exist)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(al_exist)
	virtual void OnCancel();
	afx_msg void OnAppend();
	afx_msg void OnOver();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
