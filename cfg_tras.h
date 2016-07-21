// cfg_tras.h : header file
//
#include "SAPrefsSubDlg.h"

/////////////////////////////////////////////////////////////////////////////
// cfg_tras dialog

class cfg_tras : public CSAPrefsSubDlg
{
// Construction
public:
	cfg_tras(CWnd* pParent = NULL);   // standard constructor
	LPCSTR GetName(void);

// Dialog Data
	//{{AFX_DATA(cfg_tras)
	enum { IDD = IDD_CFG_TWITS };
	CEdit	m_areas;
	CListBox	m_list;
	CEdit	m_edit;
	CButton	m_to;
	CButton	m_subject;
	CButton	m_from;
	CButton	m_fromaddr;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(cfg_tras)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void make_line(CString &line);

	// Generated message map functions
	//{{AFX_MSG(cfg_tras)
	virtual BOOL OnInitDialog();
	afx_msg void OnAdd();
	afx_msg void OnDelete();
	afx_msg void OnHelp();
	virtual void OnOK();
	afx_msg void OnSelchangeList();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedChange();
};
