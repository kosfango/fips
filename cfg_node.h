// cfg_node.h : header file
//
#include "SAPrefsSubDlg.h"

/////////////////////////////////////////////////////////////////////////////
// cfg_node dialog

class cfg_node : public CSAPrefsSubDlg
{
// Construction
public:
	cfg_node(CWnd* pParent = NULL);   // standard constructor
	LPCSTR GetName		(void);
	
// Dialog Data
	//{{AFX_DATA(cfg_node)
	enum { IDD = IDD_CFG_NODELIST };
	CListCtrl	m_listctl;
	CEdit	m_edit_control;
	CButton	m_check_pointlist;
	CListBox	m_list;
	CString	m_nodediff_name;
	CString	m_nodelist_name;
	CString	m_nodelist_type;
	CString	m_edit_point_zone;
	CString	m_exclude;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(cfg_node)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(cfg_node)
	virtual BOOL OnInitDialog();
	afx_msg void OnAdd();
	afx_msg void OnDelete();
	afx_msg void OnChange();
	virtual void OnOK();
	afx_msg void OnPrivate();
	afx_msg void OnHelp();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnItemchangedList1(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedAddrbook();
};
