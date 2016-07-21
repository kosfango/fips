// cfgsound.h : header file
//
#include "SAPrefsSubDlg.h"

/////////////////////////////////////////////////////////////////////////////
// cfgsound dialog

class cfgsound : public CSAPrefsSubDlg
{
// Construction
public:
	cfgsound(CWnd* pParent = NULL);   // standard constructor
	LPCSTR GetName(void);
	
// Dialog Data
	//{{AFX_DATA(cfgsound)
	enum { IDD = IDD_CFG_SOUND };
	CEdit	m_path;
	CListBox	m_list;
//	CToolBar	m_toolbar;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(cfgsound)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(cfgsound)
	afx_msg void OnBrowse();
	afx_msg void OnDefault();
	afx_msg void OnOK();
	afx_msg void OnChange();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeList();
	afx_msg void OnHelp();
	afx_msg void OnTestsound();
	afx_msg void OnStopsound();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
