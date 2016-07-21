// inafound.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// inafound dialog

class inafound : public CDialog
{
// Construction
public:
	inafound(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(inafound)
	enum { IDD = IDD_INADR_FOUND };
	CEdit	m_edit;
	CListBox	m_list;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(inafound)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(inafound)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg void OnUseThisAdress();
	afx_msg void OnSelchangeList1();
	afx_msg void OnDblclkList1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
