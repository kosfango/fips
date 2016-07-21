// tapisel.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// tapisel dialog

class tapisel : public CDialog
{
// Construction
public:
	tapisel(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(tapisel)
	enum { IDD = IDD_SELECT_MODEM };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(tapisel)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(tapisel)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnDblclkList1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
