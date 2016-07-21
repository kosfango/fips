#include "stdafx.h"

#include "textbuffer.h"

#if defined(_DEBUG) && defined(WIN32)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#if defined(WIN32)
#include "textwnd.h"
#else
#include "ltextwnd.h"
#endif

//  Line allocation gra0arity
#define     CHAR_ALIGN                  16
#define     ALIGN_BUF_SIZE(size)        ((size) / CHAR_ALIGN) * CHAR_ALIGN + CHAR_ALIGN;

#define     UNDO_BUF_SIZE               1024

#ifdef _DEBUG
//#define _ADVANCED_BUGCHECK  1
#endif

/////////////////////////////////////////////////////////////////////////////
// CTextBuffer::SUndoRecord

void CTextBuffer::SUndoRecord::SetText (const char* pszText)
{
  m_pszText = 0L;
  if (pszText != 0L && pszText[0] != '\0')
	{
		int nLength = (int)strlen(pszText);
    if (nLength > 1)
    {
			m_pszText = new char[(nLength + 1) * sizeof (char)];
      strcpy (m_pszText, pszText);
		}
		else m_szText[0] = pszText[0];
	}
}

void CTextBuffer::SUndoRecord::FreeText ()
{
	if (((unsigned int)m_pszText) >> 16 != 0)
		delete m_pszText;
}

/////////////////////////////////////////////////////////////////////////////
// CTextBuffer::CUpdateContext

void CTextBuffer::CInsertContext::RecalcPoint (CPoint & ptPoint)
{
  ASSERT (m_ptEnd.y > m_ptStart.y || (m_ptEnd.y == m_ptStart.y && m_ptEnd.x >= m_ptStart.x));
  if (ptPoint.y < m_ptStart.y)
    return;
  if (ptPoint.y > m_ptStart.y)
	{
		ptPoint.y += (m_ptEnd.y - m_ptStart.y);
    return;
	}
  if (ptPoint.x <= m_ptStart.x)
    return;
  ptPoint.y += (m_ptEnd.y - m_ptStart.y);
  ptPoint.x = m_ptEnd.x + (ptPoint.x - m_ptStart.x);
}

void CTextBuffer::CDeleteContext::RecalcPoint (CPoint & ptPoint)
{
  ASSERT (m_ptEnd.y > m_ptStart.y || (m_ptEnd.y == m_ptStart.y && m_ptEnd.x >= m_ptStart.x));
  if (ptPoint.y < m_ptStart.y)
    return;
  if (ptPoint.y > m_ptEnd.y)
	{
		ptPoint.y -= (m_ptEnd.y - m_ptStart.y);
    return;
	}
  if (ptPoint.y == m_ptEnd.y && ptPoint.x >= m_ptEnd.x)
	{
		ptPoint.y = m_ptStart.y;
    ptPoint.x = m_ptStart.x + (ptPoint.x - m_ptEnd.x);
    return;
	}
  if (ptPoint.y == m_ptStart.y)
  {
		if (ptPoint.x > m_ptStart.x)
			ptPoint.x = m_ptStart.x;
		return;
	}
  ptPoint = m_ptStart;
}

/////////////////////////////////////////////////////////////////////////////
// CTextBuffer

CTextBuffer::CTextBuffer(bool bFidoConv)
{
	m_bFidoConvert = bFidoConv;
	m_bInit = FALSE;
	m_bReadOnly = FALSE;
	m_bModified = FALSE;
	m_nUndoPosition = 0;
	m_ptLastChange.x = m_ptLastChange.y = -1;
}

CTextBuffer::~CTextBuffer ()
{
	ASSERT (!m_bInit);            //  You must call FreeAll() before deleting the object
}

/////////////////////////////////////////////////////////////////////////////
// CTextBuffer message handlers

void CTextBuffer::InsertLine (const char* pszLine, int nLength /*= -1*/ , int nPosition /*= -1*/ )
{
  if (nLength == -1)
	{
		if (pszLine == 0L)
			nLength = 0;
		else nLength = (int)strlen (pszLine);
	}

  SLineInfo li;
  li.m_nLength = nLength;
  li.m_nMax = ALIGN_BUF_SIZE (li.m_nLength);
  ASSERT (li.m_nMax >= li.m_nLength);
  if (li.m_nMax > 0)
    li.m_pcLine = new char[li.m_nMax];
  if (li.m_nLength > 0)
    memcpy (li.m_pcLine, pszLine, sizeof (char) * li.m_nLength);

  if (nPosition == -1)
    m_aLines.resize(m_aLines.size() + 1, li);
  else
    //m_aLines.insert(&m_aLines[nPosition], li); //? SHC
    m_aLines.insert(m_aLines.begin() + nPosition, li);
}

void CTextBuffer::AppendLine (int nLineIndex, const char* pszChars, int nLength /*= -1*/ )
{
  if (nLength == -1)
	{
		if (pszChars == 0L)
			return;
		nLength = (int)strlen (pszChars);
	}

  if (nLength == 0)
		return;

  register SLineInfo & li = m_aLines[nLineIndex];
  int nBufNeeded = li.m_nLength + nLength;
  if (nBufNeeded > li.m_nMax)
	{
		li.m_nMax = ALIGN_BUF_SIZE (nBufNeeded);
		ASSERT (li.m_nMax >= li.m_nLength + nLength);
		char *pcNewBuf = new char[li.m_nMax];
		if (li.m_nLength > 0)
			memcpy (pcNewBuf, li.m_pcLine, sizeof (char) * li.m_nLength);
		delete li.m_pcLine;
		li.m_pcLine = pcNewBuf;
	}
  memcpy (li.m_pcLine + li.m_nLength, pszChars, sizeof (char) * nLength);
  li.m_nLength += nLength;
  ASSERT (li.m_nLength <= li.m_nMax);
}

void CTextBuffer::FreeAll ()
{
  //  Free text
  int nCount = (int)m_aLines.size();
  for (int I = 0; I < nCount; I++)
		if (m_aLines[I].m_nMax > 0)
			delete m_aLines[I].m_pcLine;
  m_aLines.resize(0);

  //  Free undo buffer
	ClearUndo();

  m_bInit = FALSE;
	m_ptLastChange.x = m_ptLastChange.y = -1;
}

bool CTextBuffer::InitNew ()
{
  ASSERT(!m_bInit);
  ASSERT(m_aLines.size() == 0);
  InsertLine("");
  m_bInit = TRUE;
  m_bReadOnly = FALSE;
  m_bModified = FALSE;
  m_nSyncPosition = m_nUndoPosition = 0;
  m_bUndoGroup = m_bUndoBeginGroup = FALSE;
  m_nUndoBufSize = UNDO_BUF_SIZE;
  ASSERT(m_aUndoBuf.size() == 0);
  UpdateWnds (0L, 0L, UPDATE_RESET);
	m_ptLastChange.x = m_ptLastChange.y = -1;
  return TRUE;
}

bool CTextBuffer::GetReadOnly () const
{
  ASSERT (m_bInit);        //  Text buffer not yet initialized.
  //  You must call InitNew() or LoadFromFile() first!

  return m_bReadOnly;
}

void CTextBuffer::SetReadOnly (bool bReadOnly /*= TRUE*/ )
{
  ASSERT (m_bInit);             //  Text buffer not yet initialized.
  //  You must call InitNew() or LoadFromFile() first!

  m_bReadOnly = bReadOnly;
}

const char *crlfs[] =
{
	"\x0d\x0a", //  DOS/Windows style
  "\x0a",     //  UNIX style
  "\x0a"      //  Macintosh style
};

int CTextBuffer::GetLineCount ()
{
  ASSERT (m_bInit);
  //  You must call InitNew() or LoadFromFile() first!

  return (int)m_aLines.size();
}

int CTextBuffer::GetLineLength (int nLine)
{
  ASSERT (m_bInit);             //  Text buffer not yet initialized.
  //  You must call InitNew() or LoadFromFile() first!

  return m_aLines[nLine].m_nLength;
}

char* CTextBuffer::GetLineChars (int nLine)
{
  ASSERT (m_bInit);             //  Text buffer not yet initialized.
  //  You must call InitNew() or LoadFromFile() first!

  return m_aLines[nLine].m_pcLine;
}

unsigned int CTextBuffer::GetLineFlags (int nLine)
{
  ASSERT (m_bInit);             //  Text buffer not yet initialized.
  //  You must call InitNew() or LoadFromFile() first!

  return m_aLines[nLine].m_dwFlags;
}

static int FlagToIndex (unsigned int dwFlag)
{
  int nIndex = 0;
  while ((dwFlag & 1) == 0)
	{
		dwFlag = dwFlag >> 1;
    nIndex++;
    if (nIndex == 32) return -1;
	}
  dwFlag = dwFlag & 0xFFFFFFFE;
  if (dwFlag != 0) return -1;
  return nIndex;

}

int CTextBuffer::FindLineWithFlag (unsigned int dwFlag)
{
  int nSize = (int)m_aLines.size();
  for (int L = 0; L < nSize; L++)
	{
		if ((m_aLines[L].m_dwFlags & dwFlag) != 0)
			return L;
	}
  return -1;
}

int CTextBuffer::GetLineWithFlag (unsigned int dwFlag)
{
  int nFlagIndex =::FlagToIndex (dwFlag);
  if (nFlagIndex < 0)
  {
		ASSERT (FALSE);           //  Invalid flag passed in

    return -1;
	}
  return FindLineWithFlag (dwFlag);
}

void CTextBuffer::SetLineFlag (int nLine, unsigned int dwFlag, bool bSet, bool bRemoveFromPreviousLine /*= TRUE*/ )
{
  ASSERT (m_bInit);             //  Text buffer not yet initialized.
  //  You must call InitNew() or LoadFromFile() first!

  int nFlagIndex =::FlagToIndex (dwFlag);
  if (nFlagIndex < 0)
	{
		ASSERT (FALSE);           //  Invalid flag passed in
    return;
	}

  if (nLine == -1)
  {
		ASSERT (!bSet);
    nLine = FindLineWithFlag (dwFlag);
    if (nLine == -1)
			return;
		bRemoveFromPreviousLine = FALSE;
	}

  unsigned int dwNewFlags = m_aLines[nLine].m_dwFlags;
  if (bSet)
    dwNewFlags = dwNewFlags | dwFlag;
  else
    dwNewFlags = dwNewFlags & ~dwFlag;

  if (m_aLines[nLine].m_dwFlags != dwNewFlags)
  {
		if (bRemoveFromPreviousLine)
    {
			int nPrevLine = FindLineWithFlag (dwFlag);
      if (bSet)
      {
				if (nPrevLine >= 0)
        {
					ASSERT ((m_aLines[nPrevLine].m_dwFlags & dwFlag) != 0);
          m_aLines[nPrevLine].m_dwFlags &= ~dwFlag;
          UpdateWnds (0L, 0L, UPDATE_SINGLELINE | UPDATE_FLAGSONLY, nPrevLine);
        }
      }
      else
      {
				ASSERT (nPrevLine == nLine);
      }
    }
	  m_aLines[nLine].m_dwFlags = dwNewFlags;
		UpdateWnds (0L, 0L, UPDATE_SINGLELINE | UPDATE_FLAGSONLY, nLine);
	}
}

void CTextBuffer::GetText (int nStartLine, int nStartChar, int nEndLine, int nEndChar, string & text, const char* pszCRLF /*= 0L*/ )
{
  ASSERT (m_bInit);             //  Text buffer not yet initialized.
  //  You must call InitNew() or LoadFromFile() first!

  ASSERT (nStartLine >= 0 && nStartLine < (int)m_aLines.size());
  ASSERT (nStartChar >= 0 && nStartChar <= m_aLines[nStartLine].m_nLength);
  ASSERT (nEndLine >= 0 && nEndLine < (int)m_aLines.size());
  ASSERT (nEndChar >= 0 && nEndChar <= m_aLines[nEndLine].m_nLength);
  ASSERT (nStartLine < nEndLine || nStartLine == nEndLine && nStartChar < nEndChar);

  if (pszCRLF == 0L)
    pszCRLF = crlf;
  int nCRLFLength = (int)strlen (pszCRLF);
  ASSERT (nCRLFLength > 0);

  int nBufSize = 0;
  for (int L = nStartLine; L <= nEndLine; L++)
	{
		nBufSize += m_aLines[L].m_nLength;
    nBufSize += nCRLFLength;
	}

	text.resize(nBufSize);
	char *pszBuf = (char*)text.c_str();

  if (nStartLine < nEndLine)
  {
		int nCount = m_aLines[nStartLine].m_nLength - nStartChar;
    if (nCount > 0)
		{
			memcpy (pszBuf, m_aLines[nStartLine].m_pcLine + nStartChar, sizeof (char) * nCount);
      pszBuf += nCount;
		}
    memcpy (pszBuf, pszCRLF, sizeof (char) * nCRLFLength);
    pszBuf += nCRLFLength;
    for (int I = nStartLine + 1; I < nEndLine; I++)
    {
			nCount = m_aLines[I].m_nLength;
      if (nCount > 0)
      {
				memcpy (pszBuf, m_aLines[I].m_pcLine, sizeof (char) * nCount);
        pszBuf += nCount;
      }
      memcpy (pszBuf, pszCRLF, sizeof (char) * nCRLFLength);
      pszBuf += nCRLFLength;
		}
    if (nEndChar > 0)
		{
			memcpy (pszBuf, m_aLines[nEndLine].m_pcLine, sizeof (char) * nEndChar);
			pszBuf += nEndChar;
    }
	}
  else
	{
		int nCount = nEndChar - nStartChar;
    memcpy (pszBuf, m_aLines[nStartLine].m_pcLine + nStartChar, sizeof (char) * nCount);
		pszBuf += nCount;
	}
  pszBuf[0] = 0;
}

void CTextBuffer::AddView (CTextWnd * pView)
{
  m_lpViews.resize(m_lpViews.size() + 1, pView);
}

void CTextBuffer::RemoveView (CTextWnd * pView)
{
	for (int i = 0; i < (int)m_lpViews.size(); i++)
		if (m_lpViews[i] == pView)
		{
			//m_lpViews.erase(&m_lpViews[i]); //? SHC
			m_lpViews.erase(m_lpViews.begin() + i);
			i--;
		}
}

void CTextBuffer::UpdateWnds (CTextWnd * pSource, CUpdateContext * pContext, unsigned int dwUpdateFlags, int nLineIndex /*= -1*/ )
{
	for (int i = 0; i < (int)m_lpViews.size(); i++)
		m_lpViews[i]->UpdateWnd (pSource, pContext, dwUpdateFlags, nLineIndex);
}

bool CTextBuffer::InternalDeleteText (CTextWnd * pSource, int nStartLine, int nStartChar, int nEndLine, int nEndChar)
{
  ASSERT (m_bInit);             //  Text buffer not yet initialized.
  //  You must call InitNew() or LoadFromFile() first!

  ASSERT (nStartLine >= 0 && nStartLine < (int)m_aLines.size());
  ASSERT (nStartChar >= 0 && nStartChar <= m_aLines[nStartLine].m_nLength);
  ASSERT (nEndLine >= 0 && nEndLine < (int)m_aLines.size());
  ASSERT (nEndChar >= 0 && nEndChar <= m_aLines[nEndLine].m_nLength);
  ASSERT (nStartLine < nEndLine || nStartLine == nEndLine && nStartChar < nEndChar);
  if (m_bReadOnly)    return FALSE;

  CDeleteContext context;
  context.m_ptStart.y = nStartLine;
  context.m_ptStart.x = nStartChar;
  context.m_ptEnd.y = nEndLine;
  context.m_ptEnd.x = nEndChar;
  if (nStartLine == nEndLine)
	{
		SLineInfo & li = m_aLines[nStartLine];
    if (nEndChar < li.m_nLength)
    {
			memcpy (li.m_pcLine + nStartChar, li.m_pcLine + nEndChar,
				sizeof (char) * (li.m_nLength - nEndChar));
		}
    li.m_nLength -= (nEndChar - nStartChar);

    UpdateWnds (pSource, &context, UPDATE_SINGLELINE | UPDATE_HORZRANGE, nStartLine);
	}
  else
  {
		int nRestCount = m_aLines[nEndLine].m_nLength - nEndChar;
    char* pszRestChars = 0L;
    if (nRestCount > 0)
    {
			pszRestChars = new char[nRestCount];
			memcpy (pszRestChars, m_aLines[nEndLine].m_pcLine + nEndChar, nRestCount * sizeof (char));
    }
	  int nDelCount = nEndLine - nStartLine;
		for (int L = nStartLine + 1; L <= nEndLine; L++)
			delete m_aLines[L].m_pcLine;
		for (int k = 0; k < nDelCount; k++)
		{
			ASSERT(nStartLine + 1 < (int)m_aLines.size());
			//m_aLines.erase(&m_aLines[nStartLine + 1]); // ? SHC
			m_aLines.erase(m_aLines.begin() + nStartLine + 1);
		}

		//  nEndLine is no more valid
		m_aLines[nStartLine].m_nLength = nStartChar;
    if (nRestCount > 0)
    {
			AppendLine (nStartLine, pszRestChars, nRestCount);
      delete pszRestChars;
    }

    UpdateWnds (pSource, &context, UPDATE_HORZRANGE | UPDATE_VERTRANGE, nStartLine);
	}

  if (!m_bModified)
    SetModified (TRUE);
	// remember current cursor position as last editing position
	m_ptLastChange = context.m_ptStart;
  return TRUE;
}

bool CTextBuffer::InternalInsertText (CTextWnd * pSource, int nLine, int nPos, const char* pszText, int &nEndLine, int &nEndChar)
{
	ASSERT (m_bInit);	// Text buffer not yet initialized. Need InitNew() or LoadFromFile() first!
	ASSERT (nLine >= 0 && nLine < (int)m_aLines.size());
	ASSERT (nPos >= 0 && nPos <= m_aLines[nLine].m_nLength);
	if (m_bReadOnly)	return FALSE;

	CInsertContext context;
	context.m_ptStart.x = nPos;
	context.m_ptStart.y = nLine;

	int nRestCount = m_aLines[nLine].m_nLength - nPos;
	char* pszRestChars = 0L;
	if (nRestCount > 0)
	{
		pszRestChars = new char[nRestCount];
		memcpy (pszRestChars, m_aLines[nLine].m_pcLine + nPos, nRestCount * sizeof (char));
		m_aLines[nLine].m_nLength = nPos;
	}

	int nCurrentLine = nLine;
	bool bNewLines = FALSE;
	int nTextPos;
	for (;;)
	{
		//nTextPos = 0;
		//while (pszText[nTextPos] != 0 && (pszText[nTextPos] != '\r' && pszText[nTextPos] != '\n'))
		//	nTextPos++;
		nTextPos=strcspn(pszText,"\r\n");	// vk replaced

		char break_char = pszText[nTextPos];//NEW!!!

		if (nCurrentLine == nLine)
			AppendLine (nLine, pszText, nTextPos);
		else
		{
			InsertLine (pszText, nTextPos, nCurrentLine);
			bNewLines = TRUE;
		}

		if (pszText[nTextPos] == 0)
		{
			nEndLine = nCurrentLine;
			nEndChar = m_aLines[nCurrentLine].m_nLength;
			AppendLine (nCurrentLine, pszRestChars, nRestCount);
			break;
		}

		nCurrentLine++;
		nTextPos++;
		if ((pszText[nTextPos] == '\n' || pszText[nTextPos] == '\r') && pszText[nTextPos] != break_char)
			nTextPos++;
		//else
		//{
		//	ASSERT (FALSE);       //  Invalid line-end format passed
		//}
		pszText += nTextPos;
	}

  if (pszRestChars != 0L)	delete pszRestChars;

  context.m_ptEnd.x = nEndChar;
  context.m_ptEnd.y = nEndLine;

  if (bNewLines)
    UpdateWnds(pSource, &context, UPDATE_HORZRANGE | UPDATE_VERTRANGE, nLine);
  else
    UpdateWnds(pSource, &context, UPDATE_SINGLELINE | UPDATE_HORZRANGE, nLine);

  if (!m_bModified)
    SetModified (TRUE);
	// remember current cursor position as last editing position
	m_ptLastChange = context.m_ptEnd;

  return TRUE;
}

bool CTextBuffer::CanUndo ()
{
  ASSERT (m_nUndoPosition >= 0 && m_nUndoPosition <= (int)m_aUndoBuf.size());
  return m_nUndoPosition > 0;
}

bool CTextBuffer::CanRedo ()
{
  ASSERT (m_nUndoPosition >= 0 && m_nUndoPosition <= (int)m_aUndoBuf.size());
  return m_nUndoPosition < (int)m_aUndoBuf.size();
}

bool CTextBuffer::Undo (CPoint & ptCursorPos)
{
  ASSERT (CanUndo ());
  ASSERT ((m_aUndoBuf[0].m_dwFlags & UNDO_BEGINGROUP) != 0);
  for (;;)
	{
		m_nUndoPosition--;
    const SUndoRecord & ur = m_aUndoBuf[m_nUndoPosition];
    if (ur.m_dwFlags & UNDO_INSERT)
    {
#ifdef _ADVANCED_BUGCHECK
			//  Try to ensure that we undoing correctly...
      //  Just compare the text as it was before Undo operation
      CString text;
      GetText (ur.m_ptStartPos.y, ur.m_ptStartPos.x, ur.m_ptEndPos.y, ur.m_ptEndPos.x, text);
      ASSERT (lstrcmp (text, ur.GetText ()) == 0);
#endif
			InternalDeleteText (0L, ur.m_ptStartPos.y, ur.m_ptStartPos.x, ur.m_ptEndPos.y, ur.m_ptEndPos.x);
			ptCursorPos = ur.m_ptStartPos;
		}
		else
    {
			int nEndLine, nEndChar;
      InternalInsertText (0L, ur.m_ptStartPos.y, ur.m_ptStartPos.x, ur.GetText (), nEndLine, nEndChar);
#ifdef _ADVANCED_BUGCHECK
			ASSERT (ur.m_ptEndPos.y == nEndLine);
			ASSERT (ur.m_ptEndPos.x == nEndChar);
#endif
			ptCursorPos = ur.m_ptEndPos;
		}
    if (ur.m_dwFlags & UNDO_BEGINGROUP)
			break;
	}
  if (m_bModified && m_nSyncPosition == m_nUndoPosition)
    SetModified (FALSE);
  if (!m_bModified && m_nSyncPosition != m_nUndoPosition)
    SetModified (TRUE);
  return TRUE;
}

bool CTextBuffer::Redo (CPoint & ptCursorPos)
{
  ASSERT (CanRedo ());
  ASSERT ((m_aUndoBuf[0].m_dwFlags & UNDO_BEGINGROUP) != 0);
  ASSERT ((m_aUndoBuf[m_nUndoPosition].m_dwFlags & UNDO_BEGINGROUP) != 0);
  for (;;)
	{
		const SUndoRecord & ur = m_aUndoBuf[m_nUndoPosition];
    if (ur.m_dwFlags & UNDO_INSERT)
		{
			int nEndLine, nEndChar;
      InternalInsertText (0L, ur.m_ptStartPos.y, ur.m_ptStartPos.x, ur.GetText (), nEndLine, nEndChar);
#ifdef _ADVANCED_BUGCHECK
			ASSERT (ur.m_ptEndPos.y == nEndLine);
      ASSERT (ur.m_ptEndPos.x == nEndChar);
#endif
      ptCursorPos = ur.m_ptEndPos;
		}
    else
    {
#ifdef _ADVANCED_BUGCHECK
			CString text;
      GetText (ur.m_ptStartPos.y, ur.m_ptStartPos.x, ur.m_ptEndPos.y, ur.m_ptEndPos.x, text);
      ASSERT (lstrcmp (text, ur.GetText ()) == 0);
#endif
      InternalDeleteText (0L, ur.m_ptStartPos.y, ur.m_ptStartPos.x, ur.m_ptEndPos.y, ur.m_ptEndPos.x);
      ptCursorPos = ur.m_ptStartPos;
		}
    m_nUndoPosition++;
    if (m_nUndoPosition == (int)m_aUndoBuf.size())
			break;
		if ((m_aUndoBuf[m_nUndoPosition].m_dwFlags & UNDO_BEGINGROUP) != 0)
			break;
	}
  if (m_bModified && m_nSyncPosition == m_nUndoPosition)
    SetModified (FALSE);
  if (!m_bModified && m_nSyncPosition != m_nUndoPosition)
    SetModified (TRUE);
  return TRUE;
}

void CTextBuffer::AddUndoRecord (bool bInsert, const CPoint & ptStartPos, const CPoint & ptEndPos, const char* pszText, int n_Action)
{
  //  Forgot to call BeginUndoGroup()?
  ASSERT (m_bUndoGroup);
  ASSERT (m_aUndoBuf.size() == 0 || (m_aUndoBuf[0].m_dwFlags & UNDO_BEGINGROUP) != 0);

  //  Strip unnecessary undo records (edit after undo)
  int nBufSize = (int)m_aUndoBuf.size();
  if (m_nUndoPosition < nBufSize)
  {
		for (int I = m_nUndoPosition; I < nBufSize; I++)
			m_aUndoBuf[I].FreeText ();
		m_aUndoBuf.resize(m_nUndoPosition);
	}

  //  If undo buffer size is close to critical, remove the oldest records
  ASSERT ((int)m_aUndoBuf.size() <= m_nUndoBufSize);
  nBufSize = (int)m_aUndoBuf.size();
  if (nBufSize >= m_nUndoBufSize)
	{
		int nIndex = 0;
    for (;;)
		{
			m_aUndoBuf[nIndex].FreeText ();
      nIndex++;
      if (nIndex == nBufSize || (m_aUndoBuf[nIndex].m_dwFlags & UNDO_BEGINGROUP) != 0)
				break;
		}
		ASSERT(nIndex < (int)m_aUndoBuf.size());
		//for (int k = 0; k < nIndex; k++) m_aUndoBuf.erase(&m_aUndoBuf[0]); // ? SHC
		for (int k = 0; k < nIndex; k++) m_aUndoBuf.erase(m_aUndoBuf.begin());
	}
  ASSERT ((int)m_aUndoBuf.size() < m_nUndoBufSize);

  //  Add new record
  SUndoRecord ur;
  ur.m_dwFlags = bInsert ? UNDO_INSERT : 0;
  if (m_bUndoBeginGroup)
	{
		ur.m_dwFlags |= UNDO_BEGINGROUP;
    m_bUndoBeginGroup = FALSE;
	}
  ur.m_ptStartPos = ptStartPos;
  ur.m_ptEndPos = ptEndPos;
  ur.m_nAction = n_Action;
  ur.SetText (pszText);

  m_aUndoBuf.resize(m_aUndoBuf.size() + 1, ur);
  m_nUndoPosition = (int)m_aUndoBuf.size();

  ASSERT ((int)m_aUndoBuf.size() <= m_nUndoBufSize);
}

bool CTextBuffer::InsertText (CTextWnd * pSource, int nLine, int nPos, const char* pszText,
	int &nEndLine, int &nEndChar, int n_Action)
{
  if (!InternalInsertText(pSource, nLine, nPos, pszText, nEndLine, nEndChar))
    return FALSE;

  bool bGroupFlag = FALSE;
  if (!m_bUndoGroup)
	{
		BeginUndoGroup ();
    bGroupFlag = TRUE;
	}
  AddUndoRecord (TRUE, CPoint (nPos, nLine), CPoint (nEndChar, nEndLine), pszText, n_Action);
  if (bGroupFlag)
    FlushUndoGroup (pSource);
  return TRUE;
}

bool CTextBuffer::DeleteText (CTextWnd * pSource, int nStartLine, int nStartChar,
	int nEndLine, int nEndChar, int n_Action)
{
  string sTextToDelete;
  GetText (nStartLine, nStartChar, nEndLine, nEndChar, sTextToDelete);

  if (!InternalDeleteText (pSource, nStartLine, nStartChar, nEndLine, nEndChar))
    return FALSE;

  bool bGroupFlag = FALSE;
  if (!m_bUndoGroup)
  {
		BeginUndoGroup ();
    bGroupFlag = TRUE;
	}
	AddUndoRecord(FALSE, CPoint(nStartChar, nStartLine), CPoint(nEndChar, nEndLine), sTextToDelete.c_str(), n_Action);
  if (bGroupFlag)
    FlushUndoGroup (pSource);
  return TRUE;
}

void CTextBuffer::SetModified (bool bModified /*= TRUE*/ )
{
  m_bModified = bModified;
}

void CTextBuffer::BeginUndoGroup (bool bMergeWithPrevious /*= FALSE*/ )
{
  ASSERT (!m_bUndoGroup);
  m_bUndoGroup = TRUE;
  m_bUndoBeginGroup = m_nUndoPosition == 0 || !bMergeWithPrevious;
}

void CTextBuffer::FlushUndoGroup (CTextWnd * pSource)
{
  ASSERT (m_bUndoGroup);
  if (pSource != 0L)
	{
		ASSERT (m_nUndoPosition == (int)m_aUndoBuf.size());
		if (m_nUndoPosition > 0) m_bUndoBeginGroup = TRUE;
	}
  m_bUndoGroup = FALSE;
}

CPoint CTextBuffer::GetLastChangePos() const
{
	return m_ptLastChange;
}

void CTextBuffer::ClearUndo()
{ 
	m_nUndoPosition = 0; 
  int nBufSize = (int)m_aUndoBuf.size();
	for (int I = 0; I < nBufSize; I++)
    m_aUndoBuf[I].FreeText ();
	m_aUndoBuf.clear();
}

extern char *UndoDescriptions[];

int CTextBuffer::GetUndoDescription (string & desc, int pos /*= 0L*/ )
{
  ASSERT (CanUndo ());          //  Please call CanUndo() first

  ASSERT ((m_aUndoBuf[0].m_dwFlags & UNDO_BEGINGROUP) != 0);

  int nPosition;
  if (pos == 0)
    {
      //  Start from beginning
      nPosition = m_nUndoPosition;
    }
  else
    {
      nPosition = (int) pos;
      ASSERT (nPosition > 0 && nPosition < m_nUndoPosition);
      ASSERT ((m_aUndoBuf[nPosition].m_dwFlags & UNDO_BEGINGROUP) != 0);
    }

  //  Advance to next undo group
  nPosition--;
  while ((m_aUndoBuf[nPosition].m_dwFlags & UNDO_BEGINGROUP) == 0)
    nPosition--;

  //  Read description
	
	desc = UndoDescriptions[m_aUndoBuf[nPosition].m_nAction];

  //  Now, if we stop at zero position, this will be the last action,
  //  since we return (POSITION) nPosition
  return nPosition;
}

int CTextBuffer::GetRedoDescription (string & desc, int pos /*= 0L*/ )
{
  ASSERT (CanRedo ());          //  Please call CanRedo() before!

  ASSERT ((m_aUndoBuf[0].m_dwFlags & UNDO_BEGINGROUP) != 0);
  ASSERT ((m_aUndoBuf[m_nUndoPosition].m_dwFlags & UNDO_BEGINGROUP) != 0);

  int nPosition;
  if (pos == 0L)
    {
      //  Start from beginning
      nPosition = m_nUndoPosition;
    }
  else
    {
      nPosition = (int) pos;
      ASSERT (nPosition > m_nUndoPosition);
      ASSERT ((m_aUndoBuf[nPosition].m_dwFlags & UNDO_BEGINGROUP) != 0);
    }

  //  Read description
	desc = UndoDescriptions[m_aUndoBuf[nPosition].m_nAction];

  //  Advance to next undo group
  nPosition++;
  while (nPosition < (int)m_aUndoBuf.size() && (m_aUndoBuf[nPosition].m_dwFlags & UNDO_BEGINGROUP) == 0)
    nPosition--;

  if (nPosition >= (int)m_aUndoBuf.size())
    return 0;                //  No more redo actions!

  return nPosition;
}


bool CTextBuffer::SaveToFile(const char *path)
{
	FILE *fp = fopen(path, "wt");
	int i;
	if (!fp) return false;
	for (i = 0; i < (int)m_aLines.size(); i++)
	{
		fwrite(m_aLines[i].m_pcLine, 1, m_aLines[i].m_nLength, fp);
		fwrite("\n", 1, 1, fp);
	}
	fclose(fp);
	return true;
}

bool CTextBuffer::OpenFromFile(const char *path)
{
	FILE *fp = fopen(path, "rt");
	char buf[256];
	string str;
	int len, nLine = 0;
	if (!fp) return false;
	FreeAll();
	//InitNew();
	while (!feof(fp))
	{
		if (!fgets(buf, 255, fp)) break;
		len = (int)strlen(buf) - 1;
		if (len >= 0 && (buf[len] == '\r' || buf[len] == '\n')) 
		{
			buf[len] = 0;
			if (str.length() == 0) InsertLine(buf, -1, nLine++);
			else
			{
				InsertLine((str + buf).c_str(), -1, nLine++);
				str = "";
			}
		}
		else str += buf;
	}
	if (str.length() > 0) InsertLine(str.c_str(), -1, nLine++);
	m_bInit = TRUE;
	ClearUndo();
	fclose(fp);
  UpdateWnds (0L, 0L, UPDATE_RESET);
	return true;
}

void CTextParser::WrapLine(int nLineIndex, int nMaxLineWidth, int *anBreaks, int &nBreaks)
{
	ASSERT(m_pTextWnd);

	int			nLineLength = m_pTextWnd->GetLineLength(nLineIndex);
	int			nTabWidth = m_pTextWnd->GetTabSize();
	int			nLineCharCount = 0;
	int			nCharCount = 0;
	LPCTSTR	szLine = m_pTextWnd->GetLineChars(nLineIndex);
	int			nLastBreakPos = 0;
	int			nLastCharBreakPos = 0;
	BOOL		bWhitespace = FALSE;

	for(int i = 0; i < nLineLength; i++)
	{
		// remember position of whitespace for wrap
		if(bWhitespace)
		{
			nLastBreakPos = i;
			nLastCharBreakPos = nCharCount;
			bWhitespace = FALSE;
		}

		// increment char counter (evtl. expand tab)
		if(szLine[i] == _T('\t'))
		{
			if (m_pTextWnd->GetEqualTabs())
			{
				nLineCharCount += nTabWidth;
				nCharCount += nTabWidth;
			}
			else
			{
				nLineCharCount += (nTabWidth - nCharCount % nTabWidth);
				nCharCount += (nTabWidth - nCharCount % nTabWidth);
			}
		}
		else
		{
			nLineCharCount++;
			nCharCount++;
		}

		// remember whitespace
		if(szLine[i] == _T('\t') || szLine[i] == _T(' '))
			bWhitespace = TRUE;

		// wrap line
		if(nLineCharCount >= nMaxLineWidth)
		{
			if(anBreaks)
			{
				// if no wrap position found, but line is to wide, 
				// wrap at current position
				if((nBreaks && nLastBreakPos == anBreaks[nBreaks - 1]) || (!nBreaks && !nLastBreakPos))
				{
					nLastBreakPos = i;
					nLastCharBreakPos = nCharCount;
				}

				anBreaks[nBreaks++] = nLastBreakPos;
			}
			else
				nBreaks++;

			nLineCharCount = nCharCount - nLastCharBreakPos;
		}
	}
}

inline bool isstylechar(char c) { return (c == '*') || (c == '/') || (c == '_') || (c == '#'); }

unsigned int CFidoParser::ParseLine(unsigned int dwCookie, int nLineIndex, CTextBlock *pBuf, int &nActualItems)
{
	nActualItems = 0;
	int linelen = m_pTextWnd->GetLineLength(nLineIndex), i;
	char *origptr = (char*)malloc(linelen + 1);
	char *ptr = origptr;
	memcpy(ptr, m_pTextWnd->GetLineChars(nLineIndex), linelen);
	ptr[linelen] = 0;
	for (i = 0; i < linelen && i < QUOTE_LENGTH; i++)
		if (ptr[i] == '>' && (i == 0 || !isspace((unsigned char)ptr[i - 1])))
		{
			int n = 0;
			while (i < linelen && ptr[i] == '>') { i++; n++; }
			if (pBuf)
			{
				pBuf[nActualItems].m_nCharPos = 0;
				if (n % 2 == 0) 
					pBuf[nActualItems].m_nColorIndex = COLOR_EDIT_QUOTE1;
				else pBuf[nActualItems].m_nColorIndex = COLOR_EDIT_QUOTE2;
				nActualItems++;
			}
			free(origptr);
			return (n % 2 == 0) ? COLOR_EDIT_QUOTE1 : COLOR_EDIT_QUOTE2;
		}
	

	if (*ptr == 1 || strnicmp(ptr, "AREA:", 5) == 0 || strnicmp(ptr, "SEEN-BY: ", 9) == 0)
	{
		if (pBuf)
		{
			pBuf[nActualItems].m_nCharPos = 0;
			pBuf[nActualItems].m_nColorIndex = COLOR_EDIT_KLUDGE;
			nActualItems++;
		}
		free(origptr);
		return COLOR_EDIT_KLUDGE;
	}
	if (strnicmp(ptr, "--- ", 4) == 0 || strnicmp(ptr, "---\0", 4) == 0)
	{
		if (pBuf)
		{
			pBuf[nActualItems].m_nCharPos = 0;
			pBuf[nActualItems].m_nColorIndex = COLOR_EDIT_TEARLINE;
			nActualItems++;
		}
		free(origptr);
		return COLOR_EDIT_TEARLINE;
	}
	if (strnicmp(ptr, " * Origin: ", 11) == 0)
	{
		if (pBuf)
		{
			pBuf[nActualItems].m_nCharPos = 0;
			pBuf[nActualItems].m_nColorIndex = COLOR_EDIT_ORIGIN;
			nActualItems++;
		}
		free(origptr);
		return COLOR_EDIT_ORIGIN;
	}
	if (strnicmp(ptr, "...", 3) == 0)
	{
		/*
		DWORD nextCookie = (nLineIndex + 1) < GetLineCount() ? GetParseCookie(nLineIndex + 1) : 
			COLOR_EDIT_ORIGIN;
		*/
		int fake;
		DWORD nextCookie = (nLineIndex + 1) < m_pTextWnd->GetLineCount() ? ParseLine(-1, nLineIndex + 1, 0, fake) : 
			COLOR_EDIT_ORIGIN;
		if (((nextCookie == COLOR_EDIT_ORIGIN ||	nextCookie == COLOR_EDIT_TEARLINE)))
		{
			if (pBuf)
			{
				pBuf[nActualItems].m_nCharPos = 0;
				pBuf[nActualItems].m_nColorIndex = COLOR_EDIT_TAGLINE;
				nActualItems++;
			}
			free(origptr);
			return COLOR_EDIT_TAGLINE;
		}
	}

	if (!pBuf) 
	{
		free(origptr);
		return 0;
	}

  unsigned int sclen = 0;
  char* txptr = ptr;

  const char* stylemargins = " -|\\";
  char* punctchars = " !?\"$%&()+,.;<=>@[\\]{|}~\n\r\t";
  char* stylestopchars = "/#*_";
  char prevchar = ' ';
	
	pBuf[nActualItems].m_nCharPos = 0;
	pBuf[nActualItems].m_nColorIndex = COLOR_EDIT_NORMALTEXT;
	nActualItems++;

  while(*ptr) 
  {
    if(isstylechar(*ptr)) 
	  {
      if (strchr(punctchars, prevchar)) 
			{
        int bb = 0, bi = 0, bu = 0, br = 0;
        const char* beginstyle = ptr;
        while(isstylechar(*ptr)) 
				{
          switch(*ptr) 
					{
            case '*': bb++; break;
            case '/': bi++; break;
            case '_': bu++; break;
            case '#': br++; break;
          }
          ptr++;
        }
        if ((bb <= 1) && (bi <= 1) && (br <= 1) && (bu <= 1) && *ptr) 
				{
          const char* beginword = ptr;                 
          char endchar = 0;
          char* end = (char*)ptr;
          do 
					{
            end = strpbrk(++end, punctchars);
          } while ((end) && !isstylechar(*(end-1)));
          if(end)
            endchar = *end;
          else
            end = (char*)ptr + strlen(ptr);
          *end = 0;
          char* endstyle = end-1;                      
          if(isstylechar(*endstyle) && !strchr(stylemargins, *beginword)) 
					{
            char* endword = endstyle;
            int eb = 0, ei = 0, eu = 0, er = 0;
            while(isstylechar(*endword)) 
						{
              switch(*endword) 
							{
                case '*': eb++; break;
                case '/': ei++; break;
                case '_': eu++; break;
                case '#': er++; break;
              }
              endword--;
            }                                          
            if(endword >= beginword && !strchr(stylemargins, *endword)) 
						{
              if((bb == eb) && (bi == ei) && (bu == eu) && (br == er)) 
							{
                char endwordchar = *endword;
                *endword = 0;
                const char* style_stops_present = strpbrk(beginword, stylestopchars);
                *endword = endwordchar;
                if(!style_stops_present) 
								{
                  int colorindex = (bb ? 1 : 0) | (bi ? 2 : 0) | (bu ? 4 : 0) | (br ? 8 : 0);
									pBuf[nActualItems].m_nCharPos = (int)(txptr - origptr);
									pBuf[nActualItems].m_nColorIndex = COLOR_EDIT_NORMALTEXT;
									nActualItems++;
                  sclen += (unsigned int)(beginstyle - txptr) + 1;
									pBuf[nActualItems].m_nCharPos = (int)(beginstyle - origptr);
									pBuf[nActualItems].m_nColorIndex = colorindex + COLOR_EDIT_MARK0;
									nActualItems++;
									sclen += (unsigned int)(endstyle - beginstyle) + 2;
                  txptr = end;
                }
              }
            }
          }
          *end = endchar;
          ptr = end - 1;
        }
      }
    }
    if(*ptr)
      prevchar = *ptr++;
  }

  if (*txptr) 
	{
		pBuf[nActualItems].m_nCharPos = (int)(txptr - origptr);
		pBuf[nActualItems].m_nColorIndex = COLOR_EDIT_NORMALTEXT;
		nActualItems++;
  }
  unsigned int splen = (unsigned int)(strlen(origptr) - sclen);
  if(splen) 
	{
		pBuf[nActualItems].m_nCharPos = m_pTextWnd->GetLineLength(nLineIndex);
		pBuf[nActualItems].m_nColorIndex = COLOR_EDIT_NORMALTEXT;
		nActualItems++;
  }

	free(origptr);
	return 0;
}

char *UndoDescriptions[] =
{
  "",
  "вставку",
  "удаление выделения",
  "вырезание",
  "ввод",
  "стирание последнего символа",
  "отступ",
  "перетаскивание",
  "замену",
  "удаление",
  "автоотступ",
  "вставку файла"
};

void InsertTextToBuffer(const char *text, CTextBuffer *buf)
{
	ASSERT(buf);
	int x = 0, y = 0, line = buf->GetLineCount() - 1;
	int len = buf->GetLineLength(line);
	if (len > 0 || line > 0)
		buf->DeleteText(0, 0, 0, line, len, CE_ACTION_UNKNOWN);
	buf->InsertText(0, 0, 0, text, y, x, CE_ACTION_UNKNOWN); 
}

void GetBufferText(CTextBuffer *buf, CString& text)
{
	ASSERT(buf);
	string str;
	int x = 0, y = 0, line = buf->GetLineCount() - 1;
	int len = buf->GetLineLength(line);
	if (len > 0 || line > 0)
		buf->GetText(0, 0, line, len, str);
	text = str.c_str();		
}

void ReplaceSelection(CTextBuffer *buf, CTextWnd *wnd, const char *newtext)
{
	ASSERT(buf);
	CPoint pt1, pt2;
	int y, x;
	wnd->GetSelection(pt1, pt2);
	if (pt2.y < buf->GetLineCount() && pt2.x < buf->GetLineLength(pt2.y))
		buf->DeleteText(wnd, pt1.y, pt1.x, pt2.y, pt2.x, CE_ACTION_UNKNOWN);
	buf->InsertText(wnd, pt1.y, pt1.x, newtext, y, x, CE_ACTION_UNKNOWN);
}
