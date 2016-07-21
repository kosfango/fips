// txttemp.h : header file
//
#include "SAPrefsSubDlg.h"

/////////////////////////////////////////////////////////////////////////////
// txttemp dialog

class txttemp : public CSAPrefsSubDlg
{
// Construction
public:
	txttemp(CWnd* pParent = NULL);   // standard constructor
	BOOL GetFile(CString &path);
	LPCSTR  GetName(void);
	
// Dialog Data
	//{{AFX_DATA(txttemp)
	enum { IDD = IDD_CFG_TEXTIMP };
	CButton	m_br9;
	CButton	m_br8;
	CButton	m_br7;
	CButton	m_br6;
	CButton	m_br5;
	CButton	m_br4;
	CButton	m_br3;
	CButton	m_br2;
	CButton	m_br0;
	CButton	m_br1;
	CString	m_0;
	CString	m_1;
	CString	m_2;
	CString	m_3;
	CString	m_4;
	CString	m_5;
	CString	m_6;
	CString	m_7;
	CString	m_8;
	CString	m_9;
	int		m_cod1;
	int		m_cod2;
	int		m_cod3;
	int		m_cod4;
	int		m_cod5;
	int		m_cod6;
	int		m_cod7;
	int		m_cod8;
	int		m_cod9;
	int		m_cod0;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(txttemp)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(txttemp)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnHelp();
	afx_msg void OnBr1();
	afx_msg void OnBr2();
	afx_msg void OnBr3();
	afx_msg void OnBr4();
	afx_msg void OnBr5();
	afx_msg void OnBr6();
	afx_msg void OnBr7();
	afx_msg void OnBr8();
	afx_msg void OnBr9();
	afx_msg void OnBr0();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
