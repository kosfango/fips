// cfgrpath.h : header file
//
#include "SAPrefsSubDlg.h"

/////////////////////////////////////////////////////////////////////////////
// cfgrpath dialog

class cfgrpath : public CSAPrefsSubDlg
{
// Construction
public:
	cfgrpath(CWnd* pParent = NULL);   // standard constructor
	LPCSTR GetName(void);

// Dialog Data
	//{{AFX_DATA(cfgrpath)
	enum { IDD = IDD_CFG_REQPATH };
	CEdit	m_edit_path;
	CEdit	m_edit_password;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(cfgrpath)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(cfgrpath)
	virtual BOOL OnInitDialog();
	afx_msg void OnAdd();
	afx_msg void OnBrowse();
	afx_msg void OnDelete();
	afx_msg void OnChange();
	afx_msg void OnHelp();
	virtual void OnOK();
	afx_msg void OnSelchangeList(); 
	afx_msg void OnImport();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
