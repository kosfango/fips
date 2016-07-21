// cfg_ring.h : header file
//
#include "SAPrefsSubDlg.h"

/////////////////////////////////////////////////////////////////////////////
// cfg_ring dialog

class cfg_ring : public CSAPrefsSubDlg
{
// Construction
public:
	cfg_ring(CWnd* pParent = NULL);   // standard constructor
	LPCSTR GetName(void);
	
// Dialog Data
	//{{AFX_DATA(cfg_ring)
	enum { IDD = IDD_CFG_INRINGS };
	CButton	m_disable;
	CString	m_nr_isdn;
	CString	m_nr_m1;
	CString	m_nr_m2;
	CString	m_pause_isdn;
	CString	m_pause_m1;
	CString	m_pause_m2;
	CString	m_from;
	CString	m_to;
	BOOL	m_for1;
	BOOL	m_for2;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(cfg_ring)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(cfg_ring)
	virtual BOOL OnInitDialog();
	afx_msg void OnHelp();
	virtual void OnOK();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
