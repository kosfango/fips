// cfg_aaka.h : header file
//
#include "SAPrefsSubDlg.h"

/////////////////////////////////////////////////////////////////////////////
// cfg_aaka dialog

class cfg_aaka : public CSAPrefsSubDlg
{
// Construction
public:
	cfg_aaka(CWnd* pParent = NULL);   // standard constructor
	LPCSTR GetName(void);
	
// Dialog Data
	//{{AFX_DATA(cfg_aaka)
	enum { IDD = IDD_CFG_PERSONAL };
	CListBox	m_aliases;
	CListBox	m_akas;
	CEdit	m_edit1;
	CEdit	m_edit2;
	CString	m_prename;
	CString	m_location;
	CString	m_sysname;
	CString	m_phone;
	CString	m_aftername;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(cfg_aaka)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(cfg_aaka)
	afx_msg void OnAddAka();
	afx_msg void OnDeleteAka();
	afx_msg void OnAddAlias();
	afx_msg void OnDeleteAlias();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnHelp();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnSelchangeAka();
	afx_msg void OnSelchangeAlias();
	afx_msg void OnChangeAka();
	afx_msg void OnChangeAlias();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	CString m_password;
	afx_msg void OnBnClickedSetpas();
};
