#if !defined(AFX_TRAYICON_H__06AC760C_1133_4E1E_B61D_2F93746CC79A__INCLUDED_)
#define AFX_TRAYICON_H__06AC760C_1133_4E1E_B61D_2F93746CC79A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TrayIcon.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTrayIcon window

class CTrayIcon : public CWnd
{
// Construction
public:
	CTrayIcon(CWnd *pParent,HICON hIcon,char* txtToolTip, UINT uID);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTrayIcon)
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL Create(CWnd* pParent, HICON hIcon, char* txtToolTip, UINT uID);
	virtual ~CTrayIcon();

private:
	CWnd* m_pParent;
	NOTIFYICONDATA iconData;
	int NotificationMsg;	
	// Generated message map functions
protected:
	//{{AFX_MSG(CTrayIcon)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRAYICON_H__06AC760C_1133_4E1E_B61D_2F93746CC79A__INCLUDED_)
