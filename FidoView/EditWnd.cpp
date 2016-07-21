#include "stdafx.h"
#include "editwnd.h"
#include "textbuffer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditWnd

IMPLEMENT_DYNCREATE (CEditWnd, CTextWnd)


CEditWnd::CEditWnd()
{
  AFX_ZERO_INIT_OBJECT (CTextWnd);
  m_mapExpand = new CMap<CString, LPCTSTR, CString, LPCTSTR> (10);
	m_bAutoIndent = TRUE;
	m_bReplaceTabs = FALSE;
}

CEditWnd::~CEditWnd ()
{
  delete m_mapExpand;
}


BEGIN_MESSAGE_MAP (CEditWnd, CTextWnd)
	ON_WM_CREATE ()
	ON_WM_DESTROY ()
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	ON_WM_SYSKEYDOWN()
	ON_MESSAGE(WM_PASTE, OnPaste)
	ON_MESSAGE(WM_CUT, OnCut)
	ON_MESSAGE(WM_UNDO, OnUndo)
	ON_WM_GETDLGCODE()
END_MESSAGE_MAP ()


BOOL CEditWnd::QueryEditable ()
{
  if (m_pTextBuffer == NULL)
    return FALSE;
  return !m_pTextBuffer->GetReadOnly ();
}


BOOL CEditWnd::DeleteCurrentSelection ()
{
  if (IsSelection ())
	{
		CPoint ptSelStart, ptSelEnd;
    GetSelection (ptSelStart, ptSelEnd);

    CPoint ptCursorPos = ptSelStart;
    ASSERT_VALIDTEXTPOS (ptCursorPos);
    SetAnchor (ptCursorPos);
    SetSelection (ptCursorPos, ptCursorPos);
    SetCursorPos (ptCursorPos);
    EnsureVisible (ptCursorPos);

    m_pTextBuffer->DeleteText (this, ptSelStart.y, ptSelStart.x, ptSelEnd.y, ptSelEnd.x, CE_ACTION_DELSEL);
    return TRUE;
	}
  return FALSE;
}

void CEditWnd::InsertText(LPCSTR text, int action)
{
	m_pTextBuffer->BeginUndoGroup();
    
	CPoint ptCursorPos;
  if (IsSelection ())
  {
		CPoint ptSelStart, ptSelEnd;
    GetSelection (ptSelStart, ptSelEnd);
    
    ptCursorPos = ptSelStart;
    
		m_pTextBuffer->DeleteText (this, ptSelStart.y, ptSelStart.x, ptSelEnd.y, ptSelEnd.x, CE_ACTION_PASTE);
	}
  else ptCursorPos = GetCursorPos ();
	ASSERT_VALIDTEXTPOS (ptCursorPos);
      
  int x, y;
  m_pTextBuffer->InsertText (this, ptCursorPos.y, ptCursorPos.x, text, y, x, action);

  ptCursorPos.x = x;
  ptCursorPos.y = y;
  ASSERT_VALIDTEXTPOS (ptCursorPos);
  SetAnchor (ptCursorPos);
  SetSelection (ptCursorPos, ptCursorPos);
  SetCursorPos (ptCursorPos);
  EnsureVisible (ptCursorPos);
  m_pTextBuffer->FlushUndoGroup (this);
}

void CEditWnd::Paste ()
{
  if (!QueryEditable ())
    return;
  if (m_pTextBuffer == NULL)
    return;

  CString text;
  if (GetFromClipboard (text))
		InsertText(text, CE_ACTION_PASTE);
}

void CEditWnd::Cut()
{
  if (!QueryEditable ())
    return;
  if (m_pTextBuffer == NULL)
    return;
  if (!IsSelection ())
    return;

  CPoint ptSelStart, ptSelEnd;
  GetSelection (ptSelStart, ptSelEnd);
  string text;
  GetText (ptSelStart, ptSelEnd, text);
  PutToClipboard (text.c_str());

  CPoint ptCursorPos = ptSelStart;
  ASSERT_VALIDTEXTPOS (ptCursorPos);
  SetAnchor (ptCursorPos);
  SetSelection (ptCursorPos, ptCursorPos);
  SetCursorPos (ptCursorPos);
  EnsureVisible (ptCursorPos);

  m_pTextBuffer->DeleteText (this, ptSelStart.y, ptSelStart.x, ptSelEnd.y, ptSelEnd.x, CE_ACTION_CUT);

}

int CEditWnd::OnCreate (LPCREATESTRUCT lpCreateStruct)
{
  if (CTextWnd::OnCreate (lpCreateStruct) == -1)
    return -1;
  return 0;
}

void CEditWnd::OnDestroy ()
{
  CTextWnd::OnDestroy ();
}

void CEditWnd::UpdateWnd (CTextWnd * pSource, CUpdateContext * pContext, DWORD dwFlags, int nLineIndex /*= -1*/ )
{
  CTextWnd::UpdateWnd (pSource, pContext, dwFlags, nLineIndex);

  if (m_bSelectionPushed && pContext != NULL)
	{
		pContext->RecalcPoint (m_ptSavedSelStart);
		pContext->RecalcPoint (m_ptSavedSelEnd);
		ASSERT_VALIDTEXTPOS (m_ptSavedSelStart);
		ASSERT_VALIDTEXTPOS (m_ptSavedSelEnd);
	}
}

BOOL CEditWnd::ReplaceSelection (LPCTSTR pszNewText)
{
  if (!pszNewText)
    pszNewText = _T ("");

  CPoint ptCursorPos;
  if (IsSelection ())
	{
		CPoint ptSelStart, ptSelEnd;
		GetSelection (ptSelStart, ptSelEnd);
		ptCursorPos = ptSelStart;
		m_pTextBuffer->DeleteText (this, ptSelStart.y, ptSelStart.x, ptSelEnd.y, ptSelEnd.x, CE_ACTION_REPLACE);
	}
  else ptCursorPos = GetCursorPos ();
  ASSERT_VALIDTEXTPOS (ptCursorPos);

  int x, y;
  m_pTextBuffer->InsertText (this, ptCursorPos.y, ptCursorPos.x, pszNewText, y, x, CE_ACTION_REPLACE);
  m_nLastReplaceLen = (int)_tcslen (pszNewText);
  CPoint ptEndOfBlock = CPoint (x, y);
  ASSERT_VALIDTEXTPOS (ptCursorPos);
  ASSERT_VALIDTEXTPOS (ptEndOfBlock);
  SetAnchor (ptEndOfBlock);
  SetSelection (ptCursorPos, ptEndOfBlock);
  SetCursorPos (ptEndOfBlock);
  EnsureVisible (ptEndOfBlock);
  return TRUE;
}

void CEditWnd::OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CTextWnd::OnSysKeyDown(nChar, nRepCnt, nFlags);
	bool isAlt = (GetKeyState(VK_MENU) & 0xF0) != 0;
	bool isShift = (GetKeyState(VK_SHIFT) & 0xF0) != 0;
	bool isCtrl = (GetKeyState(VK_CONTROL) & 0xF0) != 0;
	if (nChar == VK_BACK && isShift && isAlt)
		Redo();
	if ((nChar == VK_BACK && isAlt)) 
		Undo();
}

void CEditWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CTextWnd::OnKeyDown(nChar, nRepCnt, nFlags);
	if (!QueryEditable() || m_pTextBuffer == NULL)
		return;

	bool isShift = (GetKeyState(VK_SHIFT) & 0xF0) != 0;
	bool isCtrl = (GetKeyState(VK_CONTROL) & 0xF0) != 0;

	if (nChar == VK_TAB)
	{
		BOOL bTabify = FALSE;
		CPoint ptSelStart, ptSelEnd;
		if (IsSelection())
		{	
			GetSelection(ptSelStart, ptSelEnd);
			bTabify = ptSelStart.y != ptSelEnd.y;
		}

		if (bTabify)
		{
			m_pTextBuffer->BeginUndoGroup();
	
			int nStartLine = ptSelStart.y;
			int nEndLine = ptSelEnd.y;
			ptSelStart.x = 0;
			if (ptSelEnd.x > 0)
			{
				if (ptSelEnd.y == GetLineCount() - 1)
				{
					ptSelEnd.x = GetLineLength(ptSelEnd.y);
				}
				else
				{
					ptSelEnd.x = 0;
					ptSelEnd.y ++;
				}
			}
			else
				nEndLine --;
			SetSelection(ptSelStart, ptSelEnd);
			SetCursorPos(ptSelEnd);
			EnsureVisible(ptSelEnd);

			//	Shift selection to right
			m_bHorzScrollBarLocked = TRUE;
			//static const TCHAR pszText[] = _T("\t");
			CString pszText;
			if (!m_bReplaceTabs) pszText = "\t";
			else 
			{
				CString tmp(' ', GetTabSize());
				pszText = tmp;
			}

			for (int L = nStartLine; L <= nEndLine; L ++)
			{
				int x, y;
				m_pTextBuffer->InsertText(this, L, 0, pszText, y, x, CE_ACTION_INDENT);
			}
			m_bHorzScrollBarLocked = FALSE;
			RecalcHorzScrollBar();

			m_pTextBuffer->FlushUndoGroup(this);
		}
		else
		{
			m_pTextBuffer->BeginUndoGroup();

			DeleteCurrentSelection();

			CPoint ptCursorPos = GetCursorPos();
			ASSERT_VALIDTEXTPOS(ptCursorPos);

			CString pszText;
			if (!m_bReplaceTabs) pszText = "\t";
			else 
			{
				CString tmp(' ', GetTabSize());
				pszText = tmp;
			}

			int x, y;
			m_pTextBuffer->InsertText(this, ptCursorPos.y, ptCursorPos.x, pszText, y, x, CE_ACTION_TYPING);
			ptCursorPos.x = x;
			ptCursorPos.y = y;
			ASSERT_VALIDTEXTPOS(ptCursorPos);
			SetSelection(ptCursorPos, ptCursorPos);
			SetAnchor(ptCursorPos);
			SetCursorPos(ptCursorPos);
			EnsureVisible(ptCursorPos);

			m_pTextBuffer->FlushUndoGroup(this);
		}
	}

	if (nChar == VK_BACK && !IsSelection())
	{
		CPoint ptCursorPos = GetCursorPos();

		if (ptCursorPos.y > 0 || (ptCursorPos.x > 0 && ptCursorPos.y == 0))
		{
			CPoint ptCurrentCursorPos = ptCursorPos;
			bool	bDeleted = false;
			if(ptCursorPos.x == 0)					
			{
				ptCursorPos.y--;						
				ptCursorPos.x = GetLineLength(ptCursorPos.y);	
				bDeleted = true;	
			}
			else								
			{
				ptCursorPos.x--;	
				bDeleted = true;	
			}
			ASSERT_VALIDTEXTPOS(ptCursorPos);
			SetAnchor(ptCursorPos);
			SetSelection(ptCursorPos, ptCursorPos);
			SetCursorPos(ptCursorPos); 
			EnsureVisible(ptCursorPos);

			if (bDeleted)
				m_pTextBuffer->DeleteText(this, ptCursorPos.y, ptCursorPos.x, 
					ptCurrentCursorPos.y, ptCurrentCursorPos.x, CE_ACTION_BACKSPACE); 
		}
	}

	if (nChar == VK_DELETE && !isShift) 
	{
		CPoint ptSelStart, ptSelEnd;
		GetSelection(ptSelStart, ptSelEnd);
		if (ptSelStart == ptSelEnd)
		{	
			if (ptSelEnd.x == GetLineLength(ptSelEnd.y))
			{
				if (ptSelEnd.y == GetLineCount() - 1)
					return;
				ptSelEnd.y++;
				ptSelEnd.x = 0;

				if (GetLineLength(ptSelStart.y) != 0)
				{
					LPCSTR chars = GetLineChars(ptSelEnd.y);
					for (int j = 0; j < QUOTE_LENGTH && j < GetLineLength(ptSelEnd.y); j++)
						if (chars[j] == '>') ptSelEnd.x = j + 1;
				
					while (ptSelEnd.x < GetLineLength(ptSelEnd.y) && 
						(chars[ptSelEnd.x] == ' ' || chars[ptSelEnd.x] == '\t'))
						ptSelEnd.x++;
				}
			}
			else ptSelEnd.x++;
		}

		CPoint ptCursorPos = ptSelStart;
		ASSERT_VALIDTEXTPOS(ptCursorPos);
		SetAnchor(ptCursorPos);
		SetSelection(ptCursorPos, ptCursorPos);
		SetCursorPos(ptCursorPos);
		EnsureVisible(ptCursorPos);

		m_pTextBuffer->DeleteText(this, ptSelStart.y, ptSelStart.x, ptSelEnd.y, ptSelEnd.x, CE_ACTION_DELETE); 
	}

	if ((nChar == 'X' && isCtrl) || (nChar == VK_DELETE && isShift)) 
		Cut();

	if ((nChar == 'V' && isCtrl) || (nChar == VK_INSERT && isShift)) 
		Paste();
		
	if (nChar == 'Z' && isCtrl && isShift)
		Redo();
		
	if (nChar == 'Z' && isCtrl && !isShift)
		Undo();
}

void CEditWnd::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CTextWnd::OnChar(nChar, nRepCnt, nFlags);
	if (m_bOemCharset)
		CharToOemBuff((char*)&nChar, (char*)&nChar, 1);

	if ((::GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0 ||
			(::GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0)
		return;

	BOOL bTranslated = FALSE;
	if (nChar == VK_RETURN)
	{
		m_pTextBuffer->BeginUndoGroup();

		if (QueryEditable() && m_pTextBuffer != NULL)
		{
			DeleteCurrentSelection();

			CPoint ptCursorPos = GetCursorPos();
			ASSERT_VALIDTEXTPOS(ptCursorPos);
			int x, y;
			CString pszText = "\r\n";

			bool flag;
			if (ptCursorPos.x > 0)
			{
				LPCSTR chars1 = GetLineChars(ptCursorPos.y);
				char *p = 0;
				for (int j = 0; j < QUOTE_LENGTH && j < GetLineLength(ptCursorPos.y); j++)
					if (chars1[j] == '>') p = (char*)chars1 + j;

				flag = p != 0 && ptCursorPos.x < GetLineLength(ptCursorPos.y);
				//Вставим знаки квоты
				if (flag)
				{
					p++;
					while (is_space((unsigned char)*p)) p++;
					pszText += CString(chars1, (int)(p - chars1));
				}
				//Автоотступ
				else if (m_bAutoIndent) 
				{	
					LPCSTR chars = m_pTextBuffer->GetLineChars(ptCursorPos.y);
					int len = m_pTextBuffer->GetLineLength(ptCursorPos.y);
					for (int j = 0; j < len && is_space((unsigned char)chars[j]); j++) pszText += chars[j];
				}
			}
			else flag = false;
			m_pTextBuffer->InsertText(this, ptCursorPos.y, ptCursorPos.x, pszText, y, x, CE_ACTION_TYPING);
			ptCursorPos.x = flag ? 0 : x;
			ptCursorPos.y = y;
			ASSERT_VALIDTEXTPOS(ptCursorPos);
			SetSelection(ptCursorPos, ptCursorPos);
			SetAnchor(ptCursorPos);
			SetCursorPos(ptCursorPos);
			EnsureVisible(ptCursorPos);
		}

		m_pTextBuffer->FlushUndoGroup(this);
		return;
	}

	if (nChar > 31)
	{
		if (QueryEditable() && m_pTextBuffer != NULL)
		{
			m_pTextBuffer->BeginUndoGroup(nChar != _T(' '));

			CPoint ptSelStart, ptSelEnd;
			GetSelection(ptSelStart, ptSelEnd);
			CPoint ptCursorPos;
			if (ptSelStart != ptSelEnd)
			{
				ptCursorPos = ptSelStart;
				DeleteCurrentSelection();
			}
			else
				ptCursorPos = GetCursorPos();

			ASSERT_VALIDTEXTPOS(ptCursorPos);

			char pszText[2];
			pszText[0] = (char) nChar;
			pszText[1] = 0;

			int x, y;
			m_pTextBuffer->InsertText(this, ptCursorPos.y, ptCursorPos.x, pszText, y, x, CE_ACTION_TYPING); // [JRT]
			ptCursorPos.x = x;
			ptCursorPos.y = y;
			ASSERT_VALIDTEXTPOS(ptCursorPos);
			SetSelection(ptCursorPos, ptCursorPos);
			SetAnchor(ptCursorPos);
			SetCursorPos(ptCursorPos);
			EnsureVisible(ptCursorPos);

			m_pTextBuffer->FlushUndoGroup(this);
		}
	}
}

void CEditWnd::Redo()
{
	if (!CanRedo()) return;
	CPoint ptCursorPos;
	if (m_pTextBuffer->Redo(ptCursorPos))
	{
		ASSERT_VALIDTEXTPOS(ptCursorPos);
		SetAnchor(ptCursorPos);
		SetSelection(ptCursorPos, ptCursorPos);
		SetCursorPos(ptCursorPos);
		EnsureVisible(ptCursorPos);
	}
}

void CEditWnd::Undo()
{
	if (!CanUndo()) return;
	CPoint ptCursorPos;
	if (m_pTextBuffer->Undo(ptCursorPos))
	{
		ASSERT_VALIDTEXTPOS(ptCursorPos);
		SetAnchor(ptCursorPos);
		SetSelection(ptCursorPos, ptCursorPos);
		SetCursorPos(ptCursorPos);
		EnsureVisible(ptCursorPos);
	}
}

BOOL CEditWnd::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.lpszClass = "FreeprogEditwnd";
	return CTextWnd::PreCreateWindow(cs);
}

UINT CEditWnd::OnGetDlgCode()
{
	return DLGC_WANTALLKEYS;// | CTextWnd::OnGetDlgCode();
}
