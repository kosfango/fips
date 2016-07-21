// trepl.h : header file
//
#include "SAPrefsSubDlg.h"

/////////////////////////////////////////////////////////////////////////////
// trepl dialog

class trepl : public CSAPrefsSubDlg
{
// Construction
public:
	trepl(CWnd* pParent = NULL);   // standard constructor
	LPCSTR  GetName(void);
	
// Dialog Data
	//{{AFX_DATA(trepl)
	enum { IDD = IDD_CFG_TEXTREPL };
	CListBox	m_list;
	CEdit	m_string;
	CEdit	m_replacement;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(trepl)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(trepl)
	virtual BOOL OnInitDialog();
	afx_msg void OnAdd();
	afx_msg void OnDelete();
	afx_msg void OnChange();
	afx_msg void OnHelp();
	virtual void OnOK();
	afx_msg void OnSelchangeList();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
