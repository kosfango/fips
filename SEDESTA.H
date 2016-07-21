// sedesta.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// sedesta dialog

class sedesta : public CDialog
{
// Construction
public:
	sedesta(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(sedesta)
	enum { IDD = IDD_SEARCH_AREA_SELECTION };
	CListBox	m_list;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(sedesta)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(sedesta)
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnUse();
	afx_msg void OnDblclkList1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
