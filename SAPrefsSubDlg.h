/*********************************************************************
   Copyright (C) 2002 Smaller Animals Software, Inc.
   http://www.smalleranimals.com
   smallest@smalleranimals.com
**********************************************************************/
// SAPrefsSubDlg.h: interface for the CSAPrefsSubDlg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SAPREFSSUBDLG_H__26CFAEA8_91FC_11D3_A10C_00500402F30B__INCLUDED_)
#define AFX_SAPREFSSUBDLG_H__26CFAEA8_91FC_11D3_A10C_00500402F30B__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CSAPrefsSubDlg : public CDialog  
{
public:
	DECLARE_DYNCREATE(CSAPrefsSubDlg)
		
	CSAPrefsSubDlg();
	CSAPrefsSubDlg(UINT nID, CWnd *pParent = NULL);
	virtual ~CSAPrefsSubDlg();
	
   UINT GetID()      {return m_id;}

public:
	virtual void OnOK();
	virtual void OnCancel();
	BOOL	m_bCloseOnOk;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSAPrefsSubDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	//}}AFX_VIRTUAL

protected:
	UINT     m_id;

	// Generated message map functions
	//{{AFX_MSG(CSAPrefsSubDlg)
	//}}AFX_MSG
	
	DECLARE_MESSAGE_MAP()

};

#endif // !defined(AFX_SAPREFSSUBDLG_H__26CFAEA8_91FC_11D3_A10C_00500402F30B__INCLUDED_)
