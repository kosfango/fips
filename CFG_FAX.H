// cfg_fax.h : header file
//
#include "SAPrefsSubDlg.h"

/////////////////////////////////////////////////////////////////////////////
// cfg_fax dialog

class cfg_fax : public CSAPrefsSubDlg
{
// Construction
public:
	cfg_fax(CWnd* pParent = NULL);   // standard constructor
	LPCSTR  GetName(void);
	
// Dialog Data
	//{{AFX_DATA(cfg_fax)
	enum { IDD = IDD_CFG_FAX };
	CEdit	m_faxviewer_bgfax;
	CEdit	m_faxviewer;
	CEdit	m_basepath;
	CButton	m_dont_delete;
	CButton	m_dont_convert;
	CButton	m_fallback;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(cfg_fax)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(cfg_fax)
	virtual BOOL OnInitDialog();
	afx_msg void OnBrowsePath();
	afx_msg void OnBrowseViewer();
	virtual void OnCancel();
	afx_msg void OnHelp();
	virtual void OnOK();
	afx_msg void OnBrowseViewerBgfax();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
