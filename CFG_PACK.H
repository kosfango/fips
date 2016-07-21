// cfg_pack.h : header file
//

#include "SAPrefsSubDlg.h"
/////////////////////////////////////////////////////////////////////////////
// cfg_pack dialog

class cfg_pack : public CSAPrefsSubDlg
{
// Construction
public:
	cfg_pack(CWnd* pParent = NULL);   // standard constructor
	void	make_string(void); 
	LPCSTR  GetName(void);
// Dialog Data
	//{{AFX_DATA(cfg_pack)
	enum { IDD = IDD_CFG_PACKER };
	CButton	m_builtin;
	CListBox	m_list;
	CString	m_unpack;
	CString	m_pack;
	CString	m_name;
	CString	m_p1;
	CString	m_p10;
	CString	m_p2;
	CString	m_p3;
	CString	m_p4;
	CString	m_p5;
	CString	m_p6;
	CString	m_p7;
	CString	m_p8;
	CString	m_p9;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(cfg_pack)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(cfg_pack)
	afx_msg void OnAdd();
	virtual void OnOK();
	afx_msg void OnChange();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeList();
	afx_msg void OnDelete();
	afx_msg void OnDefault();
	afx_msg void OnHelp();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
