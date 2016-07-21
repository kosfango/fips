#if !defined(AFX_BADXFER_H__F9C4FAAD_A978_48C9_B435_E129111CD7D0__INCLUDED_)
#define AFX_BADXFER_H__F9C4FAAD_A978_48C9_B435_E129111CD7D0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// badxfer.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// badxfer dialog

class badxfer : public CDialog
{
// Construction
public:
	void xferreplace();
	void xferdel(char *xfer_real,char *xfer_temp,long xfer_fsize,long xfer_ftime);
	void SetBadXfer();
	badxfer(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(badxfer)
	enum { IDD = IDD_CFG_BADXFER };
	CListBox	m_Spis;
	CString	m_BadName;
	CString	m_RealName;
	int		m_Combo;
	CString	m_FileTime;
	CString	m_FileSize;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(badxfer)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(badxfer)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeListBadXfer();
	afx_msg void OnSetonly();
	afx_msg void OnDelete();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BADXFER_H__F9C4FAAD_A978_48C9_B435_E129111CD7D0__INCLUDED_)
