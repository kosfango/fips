#if !defined(AFX_CEDITWnd_H__8F3F8B63_6F66_11D2_8C34_0080ADB86836__INCLUDED_)
#define AFX_CEDITWnd_H__8F3F8B63_6F66_11D2_8C34_0080ADB86836__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "textwnd.h"
/////////////////////////////////////////////////////////////////////////////
//  Forward class declarations

class CEditWnd : public CTextWnd
{
	DECLARE_DYNCREATE (CEditWnd)
public:
	int m_nLastReplaceLen;
	CMap<CString, LPCTSTR, CString, LPCTSTR> *m_mapExpand;
private:
	BOOL m_bSelectionPushed;
	CPoint m_ptSavedSelStart, m_ptSavedSelEnd;

	BOOL m_bAutoIndent;
	BOOL m_bReplaceTabs;

	BOOL DeleteCurrentSelection ();
public:
	void Paste();
	void Cut();
	void Redo();
	void Undo();
	bool CanUndo() { return m_pTextBuffer->CanUndo() != 0; }
	bool CanRedo() { return m_pTextBuffer->CanRedo() != 0; }
	void InsertText(LPCSTR text, int action);
	void SetAutoIndent(BOOL ai) { m_bAutoIndent = ai; }
	void SetReplaceTabs(BOOL rt) { m_bReplaceTabs = rt; }
		
	CEditWnd ();
	virtual ~CEditWnd ();


	virtual BOOL QueryEditable ();
	virtual void UpdateWnd (CTextWnd * pSource, CUpdateContext * pContext, DWORD dwFlags, int nLineIndex = -1);

	BOOL ReplaceSelection (LPCTSTR pszNewText);

protected :
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChar (UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg int OnCreate (LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy ();
	afx_msg LRESULT OnPaste(WPARAM, LPARAM) { Paste(); return 0; }
	afx_msg LRESULT OnCut(WPARAM, LPARAM) { Cut(); return 0; }
	afx_msg LRESULT OnUndo(WPARAM, LPARAM) { Undo(); return 0; }
	afx_msg void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	DECLARE_MESSAGE_MAP ()
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
public:
	afx_msg UINT OnGetDlgCode();
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CEDITWnd_H__8F3F8B63_6F66_11D2_8C34_0080ADB86836__INCLUDED_)
