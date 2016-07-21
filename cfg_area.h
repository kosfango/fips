// cfg_area.h : header file
//
//#include "SAPrefsSubDlg.h"
#include "FListCtrl.h"
#include "afxwin.h"

/////////////////////////////////////////////////////////////////////////////
// cfg_area dialog

class cfg_area : public CDialog
{
// Construction
public:
	cfg_area(CWnd* pParent = NULL);   // standard constructor
	LPCSTR  GetName(void);
	
// Dialog Data
	//{{AFX_DATA(cfg_area)
	enum { IDD = IDD_CFG_AREA };
	CComboBox	m_uplink;
	CFListCtrl	m_listctl;
	CString	m_aka;
	CString	m_areatag;
	CString	m_description;
	CString	m_group;
	CString	m_adddays;
	CString	m_number;
	CString	m_create;
	CString	m_recipt;
	CString	m_maxmails;
	BOOL	m_ccarea;
	CStatic m_newrec;
	CStatic m_modrec;
	CStatic m_delrec;
	CStatic m_totrec;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(cfg_area)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	int	 delrec;
	int	 newrec;
	int	 modrec;
	int	 totrec;
	void MakeColumns(_flistcol *fl,int count);
	void GetFields(HFROW sel,int flag);
	void SetFields(void);
	void CountRecs(void);

	// Generated message map functions
	//{{AFX_MSG(cfg_area)
	afx_msg void OnAdd();
	afx_msg void OnChange();
	afx_msg void OnDelete();
//	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnDefault();
	afx_msg void OnLookForDescription();
//	virtual void OnCancel();
	afx_msg void OnSetGroup();
//	afx_msg void OnHelp();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnRenum();
	afx_msg void OnSelendokUplink();
	afx_msg void OnListEvent(NMHDR *pNMHDR,LRESULT *pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedHelp();
};
