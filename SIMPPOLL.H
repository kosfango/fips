// simppoll.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// simppoll dialog

class simppoll : public CDialog
{
// Construction
public:
	simppoll(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(simppoll)
	enum { IDD = IDD_MANUAL_POLL };
	CEdit	m_edit;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(simppoll)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(simppoll)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
