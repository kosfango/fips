// newmail.h : header file
//
#include "SAPrefsSubDlg.h"
#include "afxwin.h"

/////////////////////////////////////////////////////////////////////////////
// newmail dialog

class newmail : public CSAPrefsSubDlg
{
// Construction
public:
	newmail(CWnd* pParent = NULL);   // standard constructor
	LPCSTR  GetName(void);
	
// Dialog Data
	//{{AFX_DATA(newmail)
	enum { IDD = IDD_CFG_TEMPLATE };
	CButton		m_random_new;
	CButton		m_random_quote;
	CEdit		m_edit;
	CString		m_originpath;
	CListCtrl	m_areas;
	CListCtrl	m_tmpl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(newmail)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:
	CStrList TempHeader;
	CStrList TempFooter;
	CStrList TempOrigin;
	void SetLists(CStrList &lst,int indn,int indq,BOOL orig);
	void ClearSelection(void);
	void UpdateSelection(int ind,int val);
	void SetNew(int val);
	void SetQuote(int val);
	
// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(newmail)
	afx_msg void OnAdd();
	afx_msg void OnDelete();
	afx_msg void OnChange();
	afx_msg void OnSetNew();
	afx_msg void OnSetQuote();
	afx_msg void OnHelp();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnRandomNew();
	afx_msg void OnRandomQuote();
	afx_msg void OnOriginfile();
	afx_msg void OnKillfocusOriginpath();
	afx_msg void OnChkHeader();
	afx_msg void OnChkFooter();
	afx_msg void OnChkOrigin();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnNMClickAreas(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangedTmpl(NMHDR *pNMHDR, LRESULT *pResult);
};
