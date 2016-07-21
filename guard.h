#pragma once


// guard dialog

class guard : public CDialog
{
	DECLARE_DYNAMIC(guard)

public:
	guard(CWnd* pParent = NULL);   // standard constructor
	virtual ~guard();

// Dialog Data
	enum { IDD = IDD_GUARD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	CString m_password;
	BOOL	m_allow;
};
