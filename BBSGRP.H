// bbsgrp.h : header file
//
#include "SAPrefsSubDlg.h"

/////////////////////////////////////////////////////////////////////////////
// bbsgrp dialog

class bbsgrp : public CSAPrefsSubDlg
{
// Construction
public:
	bbsgrp(CWnd* pParent = NULL);   // standard constructor
	int		MakeString(CString &erg); 
	LPCSTR  GetName(void);
	
// Dialog Data
	//{{AFX_DATA(bbsgrp)
	enum { IDD = IDD_BBS_LEVELS };
	CListBox	m_list;
	CEdit	m_edit_time;
	CEdit	m_edit_comment;
	CEdit	m_edit_level;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(bbsgrp)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(bbsgrp)
	afx_msg void OnAdd();
	afx_msg void OnDelete();
	virtual BOOL OnInitDialog();
	afx_msg void OnChange();
	afx_msg void OnHelp();
	virtual void OnOK();
	afx_msg void OnSelchangeList1();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
