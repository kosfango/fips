#if !defined(AFX_SATREECTRL_H__1B15B007_9152_11D3_A10C_00500402F30B__INCLUDED_)
#define AFX_SATREECTRL_H__1B15B007_9152_11D3_A10C_00500402F30B__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// SATreeCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSATreeCtrl window

class CSATreeCtrl : public CTreeCtrl
{
// Construction
public:
	CSATreeCtrl();

// Attributes
public:

// Operations
public:
   HTREEITEM GetNextItem(HTREEITEM hItem);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSATreeCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSATreeCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSATreeCtrl)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SATREECTRL_H__1B15B007_9152_11D3_A10C_00500402F30B__INCLUDED_)
