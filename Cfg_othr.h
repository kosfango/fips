// cfg_othr.h : header file
//
#include "SAPrefsSubDlg.h"

/////////////////////////////////////////////////////////////////////////////
// cfg_oth dialog

class cfg_oth : public CSAPrefsSubDlg
{
// Construction
public:
	cfg_oth(CWnd* pParent = NULL);   // standard constructor
	LPCSTR  GetName(void);
	
// Dialog Data
	//{{AFX_DATA(cfg_oth)
	enum { IDD = IDD_CFG_OTHERS };
	BOOL m_convert_latin;
	BOOL m_hidecmd;
	BOOL m_no_reminder;
	BOOL m_notooltips;
	BOOL m_blink_scroll;
	BOOL m_mintotray;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(cfg_oth)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(cfg_oth)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnHelp();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
