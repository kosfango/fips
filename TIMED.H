// Timed.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTimed dialog

class CTimed : public CDialog
{
// Construction
public:
	CTimed(CWnd* pParent = NULL);   // standard constructor
	char	msgid[20];
	char	but1[100];
	char	but2[100];
	int		defbut;
	int		delay;
	int		retval;

// Dialog Data
	//{{AFX_DATA(CTimed)
	enum { IDD = IDD_MESSAGEBOX_T };
	CStatic	m_message;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTimed)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTimed)
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
