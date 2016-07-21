// exsearch.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// exsearch dialog

class exsearch : public CDialog
{
// Construction
public:
	exsearch(CWnd* pParent = NULL);   // standard constructor
	int		FormLine						(CString &all); 
	void	ParseLine						(CString &all); 
	void	FillComboboxes					(void); 
	void	FillAreasList					(void); 	
	void	GetSelectedArea					(void); 	
	int		MailsNumberOfSelectedArea		(void); 	
	int		UpdateProgressControl			(int actual,int gesamt); 	
	int		DisplayThisMail					(int areaindex,int index,int mailid);
	void	DisplayFirstResult				(void); 
	void	DisplayNextResult				(void); 
	void	OnLoadQuery2					(LPCSTR path);
	void	OnLoadResults2					(LPCSTR path);
	void	OnSaveResults2					(LPCSTR path);
	void	SetLanguage						(void);
	void	handle_sda_file					(LPCSTR path);
	void	SavePosition					(void); 
	afx_msg void OnAdd();
	afx_msg void OnDelete();
	afx_msg void OnLoadQuery  ();
	afx_msg void OnLoadResults();
	afx_msg void OnSaveQuery();
	afx_msg void OnSaveResults();
	afx_msg void OnSearch();
	afx_msg void OnHelp();
	afx_msg void OnStopSearch();
//	afx_msg void OnSelchangeQuery();
	afx_msg void OnOk2();
	afx_msg void OnClearAll();
	afx_msg void OnSelAll();
	afx_msg void OnClearSearch();
	afx_msg void OnClearQueryList();
	afx_msg void OnDispNextQResult();

// Dialog Data
	//{{AFX_DATA(exsearch)
	enum { IDD = IDD_EXT_SEARCH };
	CButton	m_procdeleted;
	CListCtrl	m_listres;
	CListCtrl	m_listarea;
	CListCtrl	m_listqry;
	CButton	m_markasread;
	CButton	m_markallfound;
	CStatic	m_status;
	CButton	m_ored;
	CListBox	m_arealist;
	CButton	m_case_sensitive;
	CStatic	m_nr_found;
	CEdit	m_edit_max;
	CListBox	m_results;
	CListBox	m_query;
	CProgressCtrl	m_progress;
	CButton	m_not;
	CEdit	m_edit;
	CComboBox	m_dd_field;
	CComboBox	m_dd_contains;
	CEdit	m_edit_copy_to_area;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(exsearch)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(exsearch)
	virtual BOOL OnInitDialog();
	afx_msg void OnDisplay();
	afx_msg void OnChange();
	afx_msg void OnSelchangeDdField();
	afx_msg void OnBrowseAreas();
	afx_msg void OnClose();
	afx_msg	void OnSize  (UINT nType, int cx, int cy); 
	afx_msg void OnPaint (); 
	afx_msg void OnMove(int x, int y);
	afx_msg void OnClickList2(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnItemchangedList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
