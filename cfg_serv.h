#if !defined(AFX_CFG_TEMPL_H__63106C3D_458C_4EFE_9DD1_AA4585733B00__INCLUDED_)
#define AFX_CFG_TEMPL_H__63106C3D_458C_4EFE_9DD1_AA4585733B00__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// cfg_serv.h : header file
//
#include "SAPrefsDialog.h"

/////////////////////////////////////////////////////////////////////////////
// cfg_serv dialog

class cfg_serv : public CSAPrefsSubDlg
{
// Construction
public:
	cfg_serv(CWnd* pParent = NULL);   // standard constructor
	LPCSTR GetName		(void);
	
// Dialog Data
	//{{AFX_DATA(cfg_serv)
	enum { IDD = IDD_CFG_SERVICES };
	BOOL	m_autorescan;
	BOOL	m_autostart;
	BOOL	m_delseenby;
	BOOL	m_ignorerr;
	BOOL	m_noinfomails;
	BOOL	m_nonewarea;
	BOOL	m_notasaved;
	BOOL	m_packoutbound;
	BOOL	m_prochidden;
	BOOL	m_purgescanned;
	BOOL	m_startupevents;
	BOOL	m_akaroute;
	BOOL	m_noproctic;
	BOOL	m_delimp;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(cfg_serv)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(cfg_serv)
	virtual BOOL OnInitDialog();
	afx_msg void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CFG_TEMPL_H__63106C3D_458C_4EFE_9DD1_AA4585733B00__INCLUDED_)
