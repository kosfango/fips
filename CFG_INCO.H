// cfg_inco.h : header file
//
#include "SAPrefsSubDlg.h"

/////////////////////////////////////////////////////////////////////////////
// cfg_inco dialog

class cfg_inco : public CSAPrefsSubDlg
{
// Construction
public:
	cfg_inco(CWnd* pParent = NULL);   // standard constructor
 	int		make_string	(CString &line); 
	void	parse_string(CString &line); 
	LPCSTR  GetName(void);

// Dialog Data
	//{{AFX_DATA(cfg_inco)
	enum { IDD = IDD_CFG_INCOME };
	CButton	m_service_fax;
	CButton	m_disable_all;
	CButton	m_service_mailbox;
	CButton	m_service_emsi;
	CButton	m_secondmodem;
	CListBox	m_list;
	CButton	m_isdnline;
	CButton	m_firstmodem;
	CEdit	m_edit_to;
	CEdit	m_edit_from;
	CEdit	m_edit_split_to;
	CEdit	m_edit_split_from;
	CButton	m_we;
	CButton	m_th;
	CButton	m_tu;
	CButton	m_su;
	CButton	m_sa;
	CButton	m_mo;
	CButton	m_fr;
	CButton	m_splitfor_1st;
	CButton	m_splitfor_2st;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(cfg_inco)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(cfg_inco)
	virtual BOOL OnInitDialog();
	afx_msg void OnAdd();
	afx_msg void OnDelete();
	afx_msg void OnHelp();
	virtual void OnOK();
	afx_msg void OnChange();
	afx_msg void OnSelchangeList1();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
