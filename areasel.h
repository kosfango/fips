// areasel.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAreasel dialog

class CAreasel : public CDialog
{
// Construction
public:
	CAreasel(CWnd* pParent = NULL);   // standard constructor
	void	switch_lists(int userdef); 
	int		GetSelected();
		
// Dialog Data
	//{{AFX_DATA(CAreasel)
	enum { IDD = IDD_AREASELECT };
	CListCtrl	m_listudef;
	CListCtrl	m_listarea;
	CButton	m_check_udefdisplay;
	CEdit	m_edit_comment;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAreasel)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void next_unread(CListCtrl &lst);

	// Generated message map functions
	//{{AFX_MSG(CAreasel)
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnAreaTraffic();
	afx_msg void OnHelp();
	afx_msg void OnAddComment();
	afx_msg void OnDeleteComment();
	afx_msg void OnDisplayUdef();
	afx_msg void OnDown();
	afx_msg void OnUp();
	afx_msg void OnRest();
	afx_msg void OnSaveUdef();
	afx_msg void OnSize	(UINT nType, int cx, int cy); 
	afx_msg void OnPaint(); 
	afx_msg void OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkListu(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickListu(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg BOOL OnNcActivate( BOOL bActive );
	afx_msg void OnClose();
	afx_msg void OnRclickList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclickListu(NMHDR* pNMHDR, LRESULT* pResult);
//	afx_msg void OnSetfocusList(NMHDR* pNMHDR, LRESULT* pResult);
//	afx_msg void OnSetfocusListu(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
