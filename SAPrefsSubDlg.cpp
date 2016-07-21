/*********************************************************************
   SAPrefsSubDlg
   http://www.smalleranimals.com
   smallest@smalleranimals.com
**********************************************************************/

// SAPrefsSubDlg.cpp: implementation of the CSAPrefsSubDlg class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SAPrefsSubDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CSAPrefsSubDlg, CDialog)

CSAPrefsSubDlg::CSAPrefsSubDlg()
{
   ASSERT(0);
   // don't use this constructor!
}

//////////////////////////////////////////////////////////////////////

CSAPrefsSubDlg::CSAPrefsSubDlg(UINT nID, CWnd *pParent /*=NULL*/)
: CDialog(nID)
{
   m_id = nID;
}

//////////////////////////////////////////////////////////////////////

CSAPrefsSubDlg::~CSAPrefsSubDlg()
{
}

//////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CSAPrefsSubDlg, CDialog)
	//{{AFX_MSG_MAP(CSAPrefsSubDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////

void CSAPrefsSubDlg::OnOK()
{
   EndDialog(IDOK);
}

//////////////////////////////////////////////////////////////////////

void CSAPrefsSubDlg::OnCancel()
{
   EndDialog(IDCANCEL);
}

//////////////////////////////////////////////////////////////////////

BOOL CSAPrefsSubDlg::PreTranslateMessage(MSG* pMsg) 
{
	// Don't let CDialog process the Escape key.
	if ((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_ESCAPE))
	{
		return TRUE;
	}
	
	// Don't let CDialog process the Return key, if a multi-line edit has focus
	if ((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_RETURN))
	{
		// Special case: if control with focus is an edit control with
		// ES_WANTRETURN style, let it handle the Return key.

		TCHAR szClass[10];
		CWnd* pWndFocus = GetFocus();
		if (((pWndFocus = GetFocus()) != NULL) &&
			IsChild(pWndFocus) &&
			(pWndFocus->GetStyle() & ES_WANTRETURN) &&
			GetClassName(pWndFocus->m_hWnd, szClass, 10) &&
			(lstrcmpi(szClass, _T("EDIT")) == 0))
		{
			pWndFocus->SendMessage(WM_CHAR, pMsg->wParam, pMsg->lParam);
			return TRUE;
		}

		return FALSE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

//////////////////////////////////////////////////////////////////////
