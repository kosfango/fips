// cfg_tics.h : header file
//
//#include "SAPrefsSubDlg.h"

/////////////////////////////////////////////////////////////////////////////
// cfg_tics dialog

class cfg_tics : public CDialog
{
// Construction
public:
	cfg_tics(CWnd* pParent = NULL);   // standard constructor
	LPCSTR GetName(void);
	void fill_uplink_list();

// Dialog Data
	//{{AFX_DATA(cfg_tics)
	enum { IDD = IDD_CFG_TICS };
	CComboBox	m_uplink;
	CButton	m_dontdelete;
	CListBox	m_list;
	CEdit	m_path;
	CEdit	m_echo;
	CEdit	m_description;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(cfg_tics)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(cfg_tics)
	virtual BOOL OnInitDialog();
	afx_msg void OnAdd();
	afx_msg void OnBrowse();
	afx_msg void OnDelete();
	afx_msg void OnChange();
	afx_msg void OnHelp();
	afx_msg void OnSelchangeList();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
