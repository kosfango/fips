// cfg_carb.h : header file
//
#include "SAPrefsSubDlg.h"

/////////////////////////////////////////////////////////////////////////////
// cfg_carb dialog

class cfg_carb : public CSAPrefsSubDlg
{
// Construction
public:
	cfg_carb(CWnd* pParent = NULL);   // standard constructor
	LPCSTR GetName(void);

// Dialog Data
	//{{AFX_DATA(cfg_carb)
	enum { IDD = IDD_CFG_CARBONS };
	CListCtrl m_listctl;
	CEdit	m_do_not_copy;
	CEdit	m_destarea;
	CEdit	m_string;
	BOOL	m_cc_echo;
	BOOL	m_cc_netmail;
	BOOL	m_ign_case;
	BOOL	m_nsave_orig;
	BOOL	m_globalasave;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(cfg_carb)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(cfg_carb)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnAdd();
	afx_msg void OnDelete();
	afx_msg void OnHelp();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnItemchangedList1(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
