#if !defined(AFX_KLUDGES_H__7DCC4D33_16E8_424D_BA8A_2DEAA5F4E39F__INCLUDED_)
#define AFX_KLUDGES_H__7DCC4D33_16E8_424D_BA8A_2DEAA5F4E39F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// kludges.h : header file
//
#include "SAPrefsSubDlg.h"

/////////////////////////////////////////////////////////////////////////////
// kludges dialog

class kludges : public CSAPrefsSubDlg
{
// Construction
public:
	kludges(CWnd* pParent = NULL);   // standard constructor
	LPCSTR  GetName(void);
	
// Dialog Data
	//{{AFX_DATA(kludges)
	enum { IDD = IDD_CFG_KLUDGES };
	CListBox	m_hidkludges;
	CListBox	m_addkludges;
	CString	m_mykludge;
	CString	m_hiskludge;
	int		m_hidemode;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(kludges)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(kludges)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnAdd1();
	afx_msg void OnDelete1();
	afx_msg void OnDelete2();
	afx_msg void OnAdd2();
	afx_msg void OnHelp();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnSelchangeList1();
	afx_msg void OnSelchangeList2();
	afx_msg void OnChange1();
	afx_msg void OnChange2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KLUDGES_H__7DCC4D33_16E8_424D_BA8A_2DEAA5F4E39F__INCLUDED_)
