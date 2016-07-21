// rfilter.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// rfilter dialog

class rfilter : public CDialog
{
// Construction
public:
	rfilter(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(rfilter)
	enum { IDD = IDD_CFG_FILTER };
	CButton	m_pasteto;
	CButton	m_pastesubj;
	CButton	m_pastefrom;
	CEdit	m_edit_showlast;
	CButton	m_check_last;
	CEdit	m_edit_to;
	CEdit	m_edit_subject;
	CEdit	m_edit_from;
	CEdit	m_edit_header;
	CEdit	m_edit_days;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(rfilter)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	void disable_edits(void);

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(rfilter)
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnCInFrom();
	afx_msg void OnCInSubject();
	afx_msg void OnCInTo();
	afx_msg void OnNoReply();
	afx_msg void OnNotOlder();
	afx_msg void OnOutgoing();
	afx_msg void OnUnread();
	afx_msg void OnNofilter();
	afx_msg void OnSinceLastPoll();
	afx_msg void OnHelp();
	afx_msg void OnDefaultFilter();
	afx_msg void OnMarkedonly();
	afx_msg void OnHeaderonly();
	afx_msg void OnForzenOnly();
	afx_msg void OnPasteSubj();
	afx_msg void OnCheck1();
	afx_msg void OnPasteFrom();
	afx_msg void OnPasteTo();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
