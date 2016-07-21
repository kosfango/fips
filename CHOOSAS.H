// choosas.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// choosas dialog

class choosas : public CDialog
{
// Construction
public:
	choosas(CWnd* pParent = NULL);   // standard constructor
	int			GetSelectedArea		(void); 

// Dialog Data
	//{{AFX_DATA(choosas)
	enum { IDD = IDD_CHOOSE_AUTOSAVE };
	CEdit	m_edit;
	CListBox	m_list;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(choosas)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(choosas)
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkList();
	afx_msg void OnCreate();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
