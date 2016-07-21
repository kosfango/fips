// myedit.cpp : implementation file
//

#include "stdafx.h"
#include "myedit.h"
#include "structs.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

myedit::myedit(){}
myedit::~myedit(){}

BEGIN_MESSAGE_MAP(myedit, CEdit)
	//{{AFX_MSG_MAP(myedit)
	ON_WM_KEYDOWN()
	ON_WM_SETFOCUS()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// myedit message handlers

// =================================================================
	void myedit::OnSetFocus(CWnd* pOldWnd) 
// =================================================================
{
	CEdit::OnSetFocus(pOldWnd);
	
	//focus_was_set_to_edit();	
}

// =================================================================
	void myedit::OnRButtonDown(UINT nFlags, CPoint point) 
// =================================================================
{
	CEdit::OnRButtonDown(nFlags, point);
}
