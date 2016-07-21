// valarea.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// valarea dialog

class valarea : public CDialog
{
// Construction
public:
	valarea(CWnd* pParent = NULL);   // standard constructor
	void FillLinkedAreasList	(); 
	void FillDropDownList		(); 
	void ExtractSelectedEchoes	(); 
	int	 build_afix_netmail		();

// Dialog Data
	//{{AFX_DATA(valarea)
	enum { IDD = IDD_VALUE_AREAFIX };
	CListBox	m_list;
	CComboBox	m_uplink;
	CEdit	m_bigedit;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(valarea)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(valarea)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg void OnHelp();
	virtual void OnOK();
	afx_msg void OnDblclkList();
	afx_msg	void OnSize  (UINT nType, int cx, int cy); 
	afx_msg void OnPaint (); 
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnSelendokUplink();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
