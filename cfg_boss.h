// cfg_boss.h : header file
//
#include "SAPrefsSubDlg.h"

/////////////////////////////////////////////////////////////////////////////
// cfg_boss dialog

class cfg_boss : public CSAPrefsSubDlg
{
// Construction
public:
	cfg_boss(CWnd* pParent = NULL);   // standard constructor
	void MakeString		(CString &buf); 
	void show_selection	(void);
	LPCSTR GetName		(void);

// Dialog Data
	//{{AFX_DATA(cfg_boss)
	enum { IDD = IDD_CFG_BOSS };
	CButton	m_tcpip;
	CComboBox	m_packertype;
	CButton	m_zmod;
	CButton	m_hydra;
	CString	m_afix_name;
	CString	m_afix_password;
	BOOL	m_afix_plus;
	CString	m_bossname;
	CString	m_echofilename;
	CString	m_ffix_name;
	CString	m_ffix_password;
	BOOL	m_ffix_plus;
	CString	m_fidoaddress;
	CString	m_fl_packedname;
	CString	m_fl_unpackedname;
	CString	m_mypointnumber;
	CString	m_new_packedname;
	CString	m_new_unpackedname;
	CString	m_phonenumber;
	CString	m_sessionpassword;
	CString	m_sysopname;
	CString	m_arc_password;
	CString	m_routing;
	CString	m_afixnetmailname;
	BOOL	m_nopoll;
	BOOL	m_nomail;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(cfg_boss)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(cfg_boss)
	virtual void OnOK();
	afx_msg void OnAdd();
	afx_msg void OnDelete();
	virtual BOOL OnInitDialog();
	afx_msg void OnHelp();
	virtual void OnCancel();
	afx_msg void OnSelchangeList();
	afx_msg void OnChange();
	afx_msg void OnKillFocusPoint();
	afx_msg void OnHydra();
	afx_msg void OnZmodem();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnBinkD();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
