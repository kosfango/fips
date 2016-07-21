// cfg_advn.h : header file
//
#include "SAPrefsSubDlg.h"

/////////////////////////////////////////////////////////////////////////////
// cfg_advn dialog

class cfg_advn : public CSAPrefsSubDlg
{
// Construction
public:
	cfg_advn(CWnd* pParent = NULL);   // standard constructor
	LPCSTR GetName		(void);
	
// Dialog Data
	//{{AFX_DATA(cfg_advn)
	enum { IDD = IDD_CFG_PHONES };
	CListBox	m_list;
	CEdit	m_edit_replace;
	CEdit	m_edit_number;
	CString	m_countrypf;
	CString	m_citypf;
	CString	m_officepf;
	CString	m_translate;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(cfg_advn)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(cfg_advn)
	virtual BOOL OnInitDialog();
	afx_msg void OnAdd();
	afx_msg void OnDelete();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
