// cfg_apno.h : header file
//
#include "SAPrefsSubDlg.h"

/////////////////////////////////////////////////////////////////////////////
// cfg_apno dialog

class cfg_apno : public CSAPrefsSubDlg
{
// Construction
public:
	cfg_apno(CWnd* pParent = NULL);   // standard constructor
	LPCSTR GetName		(void);
	
// Dialog Data
	//{{AFX_DATA(cfg_apno)
	enum { IDD = IDD_CFG_PRIVNODES };
	CListBox	m_list;
	CEdit	m_description;
	CEdit	m_fido;
	CEdit	m_name;
	CEdit	m_telephone_number;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(cfg_apno)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(cfg_apno)
	virtual BOOL OnInitDialog();
	afx_msg void OnAdd();
	afx_msg void OnDelete();
	afx_msg void OnHelp();
	virtual void OnOK();
	afx_msg void OnChange();
	afx_msg void OnSelchangeList();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
