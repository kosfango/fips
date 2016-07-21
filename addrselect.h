#pragma once
#include "afxwin.h"


// addrselect dialog

class addrselect : public CDialog
{
	DECLARE_DYNAMIC(addrselect)

public:
	addrselect(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	enum { IDD = IDD_ADDRSELECT };
	CListBox m_addr;
	CListBox m_grp;
	char	name[100];
	char	address[100];
	char	comment[1000];
	BOOL	selected;
	BOOL	isgroup;
	CButton m_addnldata;

protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// user messages handlers
	afx_msg LRESULT OnStartListSearch(WPARAM,LPARAM);

	// Implementation
protected:
	UINT_PTR m_nTimer;
	UINT	m_Timecount;
	char	m_Buffer[100];
	char	m_pattern[100];
	int		m_nChar;
	void	fill_list(void);
	// Generated message map functions
	//{{AFX_MSG(adrbook)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLbnDblclkAddr();
	afx_msg void OnLbnDblclkGrp();
	afx_msg void OnBnClickedOk();
	afx_msg void OnLbnSelchangeAddr();
	afx_msg void OnLbnSelchangeGrp();
	afx_msg void OnBnClickedAddNL();
	afx_msg void OnBnClickedSearch();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnClose();
	void SetPattern(LPCSTR ptrn);
};
