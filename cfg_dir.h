// cfg_dir.h : header file
//

#include "SAPrefsSubDlg.h"
/////////////////////////////////////////////////////////////////////////////
// cfg_dir dialog

class cfg_dir : public CSAPrefsSubDlg
{
// Construction
public:
	cfg_dir(CWnd* pParent = NULL);   // standard constructor
	LPCSTR GetName(void);
	
// Dialog Data
	//{{AFX_DATA(cfg_dir)
	enum { IDD = IDD_CFG_DIRS };
	CString	m_basedir;
	CString	m_inbound;
	CString	m_messagebase;
	CString	m_nodelist;
	CString	m_outbound;
	CString	m_utilities;
	CString	m_multimedia;
	CString	m_ticbasic;
	CString	m_prtprog;
	BOOL	m_useansi;
	BOOL	m_useroot;
	CString	m_browser;
	BOOL	m_hidden;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(cfg_dir)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(cfg_dir)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnHelp();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnPath1();
	afx_msg void OnPath2();
	afx_msg void OnPath3();
	afx_msg void OnPath4();
	afx_msg void OnPath5();
	afx_msg void OnPath6();
	afx_msg void OnPath7();
	afx_msg void OnPath8();
	afx_msg void OnChangeBasedir();
	afx_msg void OnPath9();
	afx_msg void OnPath10();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
