// cfg_time.h : header file
//
#include "SAPrefsSubDlg.h"

/////////////////////////////////////////////////////////////////////////////
// cfg_time dialog

class cfg_time : public CSAPrefsSubDlg
{
// Construction
public:
	cfg_time(CWnd* pParent = NULL);   // standard constructor
	LPCSTR GetName(void);

// Dialog Data
	//{{AFX_DATA(cfg_time)
	enum { IDD = IDD_TIMEOUTS };
	CString	m_loopcounter;
	CString	m_trycounter;
	CString	m_waittime;
	CString m_maximum;
	CString m_seconds;
	CString m_delay;
	BOOL m_enabled;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(cfg_time)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(cfg_time)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnHelp();
	afx_msg void OnBnClickedReset();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
