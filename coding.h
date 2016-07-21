#if !defined(AFX_CODING_H__C87752B6_5045_4D76_AF3D_CC04FC980395__INCLUDED_)
#define AFX_CODING_H__C87752B6_5045_4D76_AF3D_CC04FC980395__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// coding.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// coding dialog

class coding : public CDialog
{
// Construction
public:
	coding(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(coding)
	enum { IDD = IDD_CODING };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(coding)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(coding)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CODING_H__C87752B6_5045_4D76_AF3D_CC04FC980395__INCLUDED_)
