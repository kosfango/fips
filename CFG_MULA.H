// cfg_mula.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// cfg_mula dialog

class cfg_mula : public CDialog
{
// Construction
public:
	cfg_mula(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(cfg_mula)
	enum { IDD = IDD_CFG_AREA_MULTIPLE };
	CButton	m_u_uplink;
	CButton	m_u_enable_nr_of_mails;
	CButton	m_u_edit_nr_of_mails;
	CButton	m_u_edit_age_of_reciption;
	CButton	m_u_edit_age_of_creation;
	CButton	m_u_edit_additional_days;
	CButton	m_u_aka;
	CButton	m_u_age_of_reciption;
	CButton	m_u_age_of_creation;
	CComboBox	m_uplink;
	CButton	m_nrmails;
	CEdit	m_aka;
	CButton	m_age_recipt;
	CButton	m_age_create;
	CEdit	m_edit_maxnr;
	CEdit	m_edit_age_recipt;
	CEdit	m_edit_age_create;
	CEdit	m_edit_additional_days;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(cfg_mula)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(cfg_mula)
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnUAgeOfCreation();
	afx_msg void OnUAgeOfReaciption();
	afx_msg void OnUAka();
	afx_msg void OnUEditAdditionalDays();
	afx_msg void OnUEditAgeOfCreation();
	afx_msg void OnUEditAgeOfReciption();
	afx_msg void OnUEditNrOfMails();
	afx_msg void OnUEnableNrOfMails();
	afx_msg void OnUUplink();
	virtual BOOL OnInitDialog();
	afx_msg void OnHelp();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
