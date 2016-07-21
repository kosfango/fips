#pragma once


// codesel dialog

class codesel : public CDialog
{
	DECLARE_DYNAMIC(codesel)

public:
	codesel(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	enum { IDD = IDD_CODESEL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(codesel)
	afx_msg void OnOem();
	afx_msg void OnAnsi();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
