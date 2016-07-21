// cfgmagic.h : header file
//
#include "SAPrefsSubDlg.h"

/////////////////////////////////////////////////////////////////////////////
// cfgmagic dialog

class cfgmagic : public CSAPrefsSubDlg
{
// Construction
public:
	cfgmagic(CWnd* pParent = NULL);   // standard constructor
	void	OnExport(); 
	LPCSTR  GetName(void);
	
// Dialog Data
	//{{AFX_DATA(cfgmagic)
	enum { IDD = IDD_CFG_MAGIC };
	CEdit	m_path;
	CEdit	m_password;
	CEdit	m_magic;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(cfgmagic)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(cfgmagic)
	virtual BOOL OnInitDialog();
	afx_msg void OnAdd();
	afx_msg void OnBrowse();
	afx_msg void OnDelete();
	afx_msg void OnChange();
	afx_msg void OnHelp();
	virtual void OnOK();
	afx_msg void OnSelchangeList();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
