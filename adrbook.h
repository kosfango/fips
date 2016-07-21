// adrbook.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// adrbook dialog

#include "afxcmn.h"
class adrbook : public CDialog
{
// Construction
public:
	adrbook(CWnd* pParent = NULL);   // standard constructor
	BOOL HandleRightButton		(MSG* pMsg); 

// Dialog Data
	//{{AFX_DATA(adrbook)
	enum { IDD = IDD_ADDRBOOK };
	CEdit	m_nick;
	CEdit	m_info;
	CEdit	m_name;
	CEdit	m_fido;
	CListCtrl m_addresses;
	char	name[100];
	char	address[100];
	//}}AFX_DATA


// Overrides
public:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(adrbook)
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(adrbook)
	virtual BOOL OnInitDialog();
	afx_msg void OnAdd();
	afx_msg void OnDelete();
	virtual void OnOK();
	afx_msg void OnHelp();
	afx_msg void OnChange();
	afx_msg void OnInternetgate();
	afx_msg void OnSortAdress();
	afx_msg void OnSortComment();
	afx_msg void OnSortName();
	afx_msg void OnNMClickAddr(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint(); 
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
