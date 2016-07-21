// outbound.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// outbound dialog

class outbound : public CDialog
{
// Construction
public:
	outbound(CWnd* pParent = NULL);   // standard constructor
	void list_outbound_systems	(void); 
	void get_selected_dir		(CString &actdir);
	void disable_buttons		(void);
	void refresh_list			(void);

// Dialog Data
	//{{AFX_DATA(outbound)
	enum { IDD = IDD_OUTBOUND_VIEW };
	CListBox	m_list;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(outbound)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(outbound)
	afx_msg void OnFileAttach();
	afx_msg void OnMessages();
	afx_msg void OnFileRequest();
	virtual void OnOK();
	afx_msg void OnDeleteThisOutbound();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeList();
	afx_msg void OnHelp();
	afx_msg void OnTicfiles();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
