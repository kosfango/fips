// cfg_gate.h : header file
//

#include "SAPrefsSubDlg.h"
/////////////////////////////////////////////////////////////////////////////
// cfg_gate dialog

class cfg_gate : public CSAPrefsSubDlg
{
// Construction
public:
	cfg_gate(CWnd* pParent = NULL);   // standard constructor
	LPCSTR	GetName(void);

// Dialog Data
	//{{AFX_DATA(cfg_gate)
	enum { IDD = IDD_CFG_GATE };
	CButton	m_enable;
	CEdit	m_edit_aka;
	CEdit	m_edit_to_fido;
	CEdit	m_edit_to;
	CEdit	m_edit_subject;
	CEdit	m_edit_mailcont;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(cfg_gate)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(cfg_gate)
	virtual BOOL OnInitDialog();
	afx_msg void OnHelp();
	virtual void OnOK();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
