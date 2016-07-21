// bbsuser.h : header file
//
#include "SAPrefsSubDlg.h"

/////////////////////////////////////////////////////////////////////////////
// bbsuser dialog

class bbsuser : public CSAPrefsSubDlg
{
// Construction
public:
	bbsuser(CWnd* pParent = NULL);   // standard constructor
	int		MakeString(CString &line);
	int		DispString(CString line);
	int		FillDropDowns(void);
	LPCSTR	GetName(void);

// Dialog Data
	//{{AFX_DATA(bbsuser)
	enum { IDD = IDD_BBS_USERS };
	CEdit	m_edit_remain;
	CListBox	m_list;
	CEdit	m_edit_town;
	CEdit	m_edit_tel;
	CEdit	m_edit_street;
	CEdit	m_edit_password;
	CEdit	m_edit_name;
	CEdit	m_edit_modemfax;
	CEdit	m_edit_lines;
	CEdit	m_edit_groups;
	CComboBox	m_dd_level;
	CComboBox	m_dd_language;
	CButton	m_check_ansi;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(bbsuser)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(bbsuser)
	virtual BOOL OnInitDialog();
	afx_msg void OnHelp();
	afx_msg void OnAdd();
	afx_msg void OnDelete();
	afx_msg void OnChange();
	virtual void OnOK();
	afx_msg void OnSelchangeList();
	afx_msg void OnChangeEditPassword();
	afx_msg void OnChangeEditRemain();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
