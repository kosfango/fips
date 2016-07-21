// logopti.h : header file
//
#include "SAPrefsSubDlg.h"

/////////////////////////////////////////////////////////////////////////////
// logopti dialog

class logopti : public CSAPrefsSubDlg
{
// Construction
public:
	logopti(CWnd* pParent = NULL);   // standard constructor
	LPCSTR GetName(void);

// Dialog Data
	//{{AFX_DATA(logopti)
	enum { IDD = IDD_CFG_LOGFILES };
	CEdit	m_nodelist_shortto;
	CEdit	m_nodelist_maxsize;
	CButton	m_nodelist;
	CEdit	m_events_shortto;
	CEdit	m_tosser_shortto;
	CEdit	m_purger_shortto;
	CEdit	m_mailer_shortto;
	CEdit	m_tosser_maxsize;
	CEdit	m_events_maxsize;
	CEdit	m_purger_maxsize;
	CEdit	m_mailer_maxsize;
	CButton	m_events;
	CButton	m_tosser;
	CButton	m_purger;
	CButton	m_mailer;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(logopti)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(logopti)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnHelp();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
