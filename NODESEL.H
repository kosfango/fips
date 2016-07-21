// nodesel.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// nodesel dialog

class nodesel : public CDialog
{
// Construction
public:
	nodesel(CWnd* pParent = NULL);   // standard constructor
	void extract_selected			(void); 
	BOOL HandleRightButton			(MSG* pMsg);

// Dialog Data
	//{{AFX_DATA(nodesel)
	enum { IDD = IDD_SELECT_FROM_NODELIST };
	CListBox	m_list;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(nodesel)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(nodesel)
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkList();
	afx_msg	void OnSize(UINT nType, int cx, int cy); 
	afx_msg void OnPaint(); 
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
