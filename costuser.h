// costuser.h : header file
//
#include "SAPrefsSubDlg.h"

/////////////////////////////////////////////////////////////////////////////
// costuser dialog

class costuser : public CSAPrefsSubDlg
{
// Construction
public:
	costuser(CWnd* pParent = NULL);   // standard constructor
	LPCSTR GetName			(void);
	void FillBaseCosts	(void); 
	void String2DispB		(LPCSTR str);
	void String2DispU		(LPCSTR str);
	void Disp2StringB		(CString &str);
	void Disp2StringU		(CString &str);

// Dialog Data
	//{{AFX_DATA(costuser)
	enum { IDD = IDD_CFG_COST };
	CButton	m_enabled;
	CListBox	m_listb;
	CListBox	m_listu;
	CComboBox	m_basecosts;
	CString	m_phone;
	CString	m_delay;
	CString	m_cost;
	CString	m_name;
	CString	m_workdays;
	CString	m_unit;
	CString	m_weekend;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(costuser)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(costuser)
	virtual BOOL OnInitDialog();
	afx_msg void OnAddB();
	afx_msg void OnDeleteB();
	afx_msg void OnChangeB();
	afx_msg void OnAddU();
	afx_msg void OnDeleteU();
	afx_msg void OnChangeU();
	virtual void OnOK();
	afx_msg void OnSelchangeListU();
	afx_msg void OnSelchangeListB();
	afx_msg void OnHelp();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
