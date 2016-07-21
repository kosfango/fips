// cfg_mode.h : header file
//
#include "SAPrefsSubDlg.h"

/////////////////////////////////////////////////////////////////////////////
// cfg_modem dialog

class cfg_modem : public CSAPrefsSubDlg
{
// Construction
public:
	cfg_modem(CWnd* pParent = NULL);   // standard constructor
	LPCSTR  GetName(void);
	int is_second;
	char  whichmodem[50];
	void  FetchAndSetModemData(void); 
	void  get_sys_modems(void); 

// Dialog Data
	//{{AFX_DATA(cfg_modem)
	enum { IDD = IDD_CFG_HARDWARE };
	CListBox	m_isdnlist;
	CComboBox	m_baud_control;
	CComboBox	m_comport_control;
	CString	m_answer;
	CString	m_dial;
	CString	m_init1;
	CString	m_init2;
	CString	m_init3;
	CString	m_predial1;
	CString	m_predial2;
	CString	m_predial3;
	int		m_databits;
	int		m_parity;
	int		m_stopbits;
	int		m_flowctl;
	BOOL	m_use_second;
	BOOL	m_isdn_enable;
	CString	m_msn;
	CString	m_newline;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(cfg_modem)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void to_mc(_modemcfg *mc);
	void from_mc(_modemcfg *mc);

	// Generated message map functions
	//{{AFX_MSG(cfg_modem)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnHelp();
	afx_msg void OnImport();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnMod1set();
	afx_msg void OnMod2set();
	afx_msg void OnAdd();
	afx_msg void OnDelete();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	BOOL m_useinit1;
	BOOL m_useinit2;
	BOOL m_useinit3;
	BOOL m_usecmd1;
	BOOL m_usecmd2;
	BOOL m_usecmd3;
};
