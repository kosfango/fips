#include "stdafx.h"
#include "textwnd.h"
#include "textbuffer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define TAB_CHARACTER (m_bOemCharset ? '\xFD' : '\xBB')
#define SPACE_CHARACTER (m_bOemCharset ? '\xFA' : '\x95')

#define SMOOTH_SCROLL_FACTOR 6

////////////////////////////////////////////////////////////////////////////
// CTextWnd

IMPLEMENT_DYNCREATE (CTextWnd, CWnd)

BEGIN_MESSAGE_MAP (CTextWnd, CWnd)
	ON_WM_KEYDOWN()
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_VSCROLL()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_SETFOCUS()
	ON_WM_HSCROLL()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_KILLFOCUS()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_SYSCOLORCHANGE()
	ON_WM_CREATE()
	ON_MESSAGE(WM_COPY, OnCopy)
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()

CTextWnd::CTextWnd()
{
  AFX_ZERO_INIT_OBJECT (CWnd);

  m_nScreenLines = -1;
  
  m_nLeftMargin = 0;
  m_nRightMargin = 0;
  m_bColorMark = false;
	
	m_nScreenChars = -1;
  m_bWordWrap = FALSE;
	m_bViewTabs = FALSE;
  m_nTabSize = 4;
	m_bEqualTabs = TRUE;
	
	m_panSubLines = new CArray<int, int>();
	ASSERT(m_panSubLines);
	m_panSubLines->SetSize(0, 4096);

	m_panScreenChars = new CArray<int, int>();
	ASSERT(m_panScreenChars);
	m_panScreenChars->SetSize(0, 4096);

  memset(&m_lfBaseFont, 0, sizeof(m_lfBaseFont));
  m_lfBaseFont.lfWeight = FW_NORMAL;
  m_lfBaseFont.lfItalic = FALSE;
	if (m_bOemCharset)
		m_lfBaseFont.lfCharSet = OEM_CHARSET;
	else m_lfBaseFont.lfCharSet = ANSI_CHARSET;
  m_lfBaseFont.lfOutPrecision = OUT_DEFAULT_PRECIS;
  m_lfBaseFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
  m_lfBaseFont.lfQuality = DEFAULT_QUALITY;
  m_lfBaseFont.lfPitchAndFamily = DEFAULT_PITCH;
  strcpy(m_lfBaseFont.lfFaceName, "Courier New");
  HDC hDC = ::GetDC(m_hWnd);
  m_lfBaseFont.lfHeight = -MulDiv(10, GetDeviceCaps(hDC, LOGPIXELSY), 72);
  ::ReleaseDC(m_hWnd, hDC);

  ResetView();
}

CTextWnd::~CTextWnd()
{
  ASSERT (m_pCacheBitmap == NULL);
  ASSERT (m_pTextBuffer == NULL);   //  Must be correctly detached

  if (m_pdwParseCookies != NULL)
    delete m_pdwParseCookies;
  if (m_pnActualLineLength != NULL)
    delete m_pnActualLineLength;

	if(m_panSubLines)
		delete m_panSubLines;

	if (m_panScreenChars)
		delete m_panScreenChars;
}

BOOL CTextWnd::PreCreateWindow(CREATESTRUCT & cs)
{
  CWnd *pParentWnd = CWnd::FromHandlePermanent (cs.hwndParent);
	if (m_bWordWrap && m_nScreenChars == -1) 
		cs.style|= WS_VSCROLL;
	else cs.style |= (WS_HSCROLL | WS_VSCROLL);
	
  cs.lpszClass = AfxRegisterWndClass(CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW);

  return CWnd::PreCreateWindow (cs);
}


/////////////////////////////////////////////////////////////////////////////
// CTextWnd drawing

void CTextWnd::GetSelection (CPoint & ptStart, CPoint & ptEnd)
{
  PrepareSelBounds();
  ptStart = m_ptDrawSelStart;
  ptEnd = m_ptDrawSelEnd;
}

int CTextWnd::GetLineActualLength (int nLineIndex)
{
  int nLineCount = GetLineCount();
  ASSERT (nLineCount > 0);
  ASSERT (nLineIndex >= 0 && nLineIndex < nLineCount);
  if (m_pnActualLineLength == NULL)
	{
		m_pnActualLineLength = new int[nLineCount];
		memset (m_pnActualLineLength, 0xff, sizeof (int) * nLineCount);
		m_nActualLengthArraySize = nLineCount;
	}

  if (m_pnActualLineLength[nLineIndex] >= 0)
    return m_pnActualLineLength[nLineIndex];

  //  Actual line length is not determined yet, let's calculate a little
  int nActualLength = 0;
  int nLength = GetLineLength (nLineIndex);
  if (nLength > 0)
	{
		LPCTSTR pszLine = GetLineChars (nLineIndex);
		LPTSTR pszChars = (LPTSTR) _alloca (sizeof (TCHAR) * (nLength + 1));
		memcpy (pszChars, pszLine, sizeof (TCHAR) * nLength);
		pszChars[nLength] = 0;
		LPTSTR pszCurrent = pszChars;

		int nTabSize = GetTabSize();
		for (;;)
		{
			LPTSTR psz = _tcschr (pszCurrent, _T('\t'));
			if (psz == NULL)
			{
				nActualLength += (int)(pszChars + nLength - pszCurrent);
				break;
			}

			nActualLength += (int)(psz - pszCurrent);
			if (GetEqualTabs()) nActualLength += nTabSize;
			else nActualLength += (int)(nTabSize - nActualLength % nTabSize);
			pszCurrent = psz + 1;
		}
	}

  m_pnActualLineLength[nLineIndex] = nActualLength;
  return nActualLength;
}

void CTextWnd::ScrollToOffset(int nNewOffset, BOOL bNoSmoothScroll /*= FALSE*/ , BOOL bTrackScrollBar /*= TRUE*/)
{
	if (m_bWordWrap && m_nScreenChars == -1)
		return;

  if (m_nOffset != nNewOffset)
	{
		int nScroll = m_nOffset - nNewOffset;
		m_nOffset = nNewOffset;
		CRect rcScroll;
		GetClientRect(&rcScroll);
		ScrollWindow(nScroll, 0, &rcScroll, &rcScroll);
		UpdateWindow();
		if (bTrackScrollBar)
		RecalcHorzScrollBar(TRUE);
	}
}

void CTextWnd::ScrollToSubLine(int nNewTopSubLine, 
	BOOL bNoSmoothScroll /*= FALSE*/, BOOL bTrackScrollBar /*= TRUE*/)
{
	if (m_nTopSubLine != nNewTopSubLine)
	{
		if (bNoSmoothScroll || ! m_bSmoothScroll)
		{
			int nScrollLines = m_nTopSubLine - nNewTopSubLine;
			m_nTopSubLine = nNewTopSubLine;
			ScrollWindow(0, nScrollLines * GetLineHeight());
			UpdateWindow();
			if (bTrackScrollBar)
				RecalcVertScrollBar(TRUE);
		}
		else
		{
			//	Do smooth scrolling
			int nLineHeight = GetLineHeight();
			if (m_nTopSubLine > nNewTopSubLine)
			{
				int nIncrement = (m_nTopSubLine - nNewTopSubLine) / SMOOTH_SCROLL_FACTOR + 1;
				while (m_nTopSubLine != nNewTopSubLine)
				{
					int nTopSubLine = m_nTopSubLine - nIncrement;
					if (nTopSubLine < nNewTopSubLine)
						nTopSubLine = nNewTopSubLine;
					int nScrollLines = nTopSubLine - m_nTopSubLine;
					m_nTopSubLine = nTopSubLine;
					ScrollWindow(0, - nLineHeight * nScrollLines);
					UpdateWindow();
					if (bTrackScrollBar)
						RecalcVertScrollBar(TRUE);
				}
			}
			else
			{
				int nIncrement = (nNewTopSubLine - m_nTopSubLine) / SMOOTH_SCROLL_FACTOR + 1;
				while (m_nTopSubLine != nNewTopSubLine)
				{
					int nTopSubLine = m_nTopSubLine + nIncrement;
					if (nTopSubLine > nNewTopSubLine)
						nTopSubLine = nNewTopSubLine;
					int nScrollLines = nTopSubLine - m_nTopSubLine;
					m_nTopSubLine = nTopSubLine;
					ScrollWindow(0, - nLineHeight * nScrollLines);
					UpdateWindow();
					if (bTrackScrollBar)
						RecalcVertScrollBar(TRUE);
				}
			}
		}
		int nDummy;
		GetLineBySubLine(m_nTopSubLine, m_nTopLine, nDummy);
		InvalidateRect(NULL);	// repaint whole window
	}
}

void CTextWnd::ScrollToLine (int nNewTopLine, BOOL bNoSmoothScroll /*= FALSE*/ , BOOL bTrackScrollBar /*= TRUE*/)
{
	if(m_nTopLine != nNewTopLine)
		ScrollToSubLine(GetSubLineIndex(nNewTopLine), bNoSmoothScroll, bTrackScrollBar);
}

void CTextWnd::ExpandChars (LPCTSTR pszChars, int nOffset, int nCount, CString & line)
{
  if (nCount <= 0)
	{
		line = _T ("");
		return;
	}

  int nTabSize = GetTabSize();

  int nActualOffset = 0;
  for (int I = 0; I < nOffset; I++)
	{
		if (pszChars[I] == _T ('\t'))
			//nActualOffset += (nTabSize - nActualOffset % nTabSize);
			if (GetEqualTabs()) nActualOffset += nTabSize;
			else nActualOffset += (nTabSize - nActualOffset % nTabSize);
		else nActualOffset++;
	}

  pszChars += nOffset;
  int nLength = nCount;

  int nTabCount = 0;
  for (I = 0; I < nLength; I++)
	{
		if (pszChars[I] == _T ('\t'))
		nTabCount++;
	}

  LPTSTR pszBuf = line.GetBuffer (nLength + nTabCount * (nTabSize - 1) + 1);

  int nCurPos = 0;
  if (nTabCount > 0 || m_bViewTabs)
	{
		for (I = 0; I < nLength; I++)
		{
			if (pszChars[I] == _T ('\t'))
			{
				//int nSpaces = nTabSize - (nActualOffset + nCurPos) % nTabSize;
				int nSpaces;
				if (GetEqualTabs()) nSpaces = nTabSize;
				else nSpaces = nTabSize - (nActualOffset + nCurPos) % nTabSize;
				if (m_bViewTabs)
				{
					pszBuf[nCurPos++] = TAB_CHARACTER;
					nSpaces--;
				}
				while (nSpaces > 0)
				{
					pszBuf[nCurPos++] = _T (' ');
					nSpaces--;
				}
			}
			else
			{
				if (pszChars[I] == _T (' ') && m_bViewTabs)
					pszBuf[nCurPos] = SPACE_CHARACTER;
				else
					pszBuf[nCurPos] = pszChars[I];
				nCurPos++;
			}
		}
	}
  else
	{
		memcpy (pszBuf, pszChars, sizeof (TCHAR) * nLength);
		nCurPos = nLength;
	}
  pszBuf[nCurPos] = 0;
  line.ReleaseBuffer();
}

void CTextWnd::DrawLineHelperImpl (CDC * pdc, CPoint & ptOrigin, const CRect & rcClip,
	LPCTSTR pszChars, int nOffset, int nCount)
{
  ASSERT (nCount >= 0);
  if (nCount > 0)
	{
		CString line;
    ExpandChars(pszChars, nOffset, nCount, line);
    int nWidth = rcClip.right - ptOrigin.x;
    if (nWidth > 0)
		{
/*
			int nCharWidth = GetCharWidth();
      int nCount = line.GetLength();
      int nCountFit = nWidth / nCharWidth + 1;
      if (nCount > nCountFit)
				nCount = nCountFit;
			VERIFY (pdc->ExtTextOut(ptOrigin.x, ptOrigin.y, ETO_CLIPPED, &rcClip, line, nCount, NULL));
		}
		ptOrigin.x += GetCharWidth() * line.GetLength();
*/
	///////////////////////////////////////////////////////////////////////////////////
      int nCount = line.GetLength();
      int nCountFit = 0, cur_len = 0;
      while (cur_len < nWidth && nCountFit < line.GetLength())
      {
				nCountFit++;
				cur_len = TextWidth(pdc, line, nCountFit);
      }
      if (nCount > nCountFit)
				nCount = nCountFit;
			VERIFY (pdc->ExtTextOut(ptOrigin.x, ptOrigin.y, ETO_CLIPPED, &rcClip, line, nCount, NULL));
		}
		ptOrigin.x += TextWidth(pdc, line);
	///////////////////////////////////////////////////////////////////////////////////
	}
}

void CTextWnd::DrawLineHelper (CDC * pdc, CPoint & ptOrigin, const CRect & rcClip, int nColorIndex,
	LPCTSTR pszChars, int nOffset, int nCount, CPoint ptTextPos)
{
	if (nCount > 0)
  {
		if (m_bFocused || m_bShowInactiveSelection)
		{
			int nSelBegin = 0, nSelEnd = 0;
      if (m_ptDrawSelStart.y > ptTextPos.y)
				nSelBegin = nCount;
			else if (m_ptDrawSelStart.y == ptTextPos.y)
      {
				nSelBegin = m_ptDrawSelStart.x - ptTextPos.x;
        if (nSelBegin < 0)
					nSelBegin = 0;
        if (nSelBegin > nCount)
					nSelBegin = nCount;
			}
      if (m_ptDrawSelEnd.y > ptTextPos.y)
				nSelEnd = nCount;
			else if (m_ptDrawSelEnd.y == ptTextPos.y)
			{
				nSelEnd = m_ptDrawSelEnd.x - ptTextPos.x;
        if (nSelEnd < 0)
					nSelEnd = 0;
				if (nSelEnd > nCount)
					nSelEnd = nCount;
			}
	    ASSERT (nSelBegin >= 0 && nSelBegin <= nCount);
			ASSERT (nSelEnd >= 0 && nSelEnd <= nCount);
      ASSERT (nSelBegin <= nSelEnd);

      //  Draw part of the text before selection
      if (nSelBegin > 0)
				DrawLineHelperImpl (pdc, ptOrigin, rcClip, pszChars, nOffset, nSelBegin);
      if (nSelBegin < nSelEnd)
			{
				COLORREF crOldBk = pdc->SetBkColor(GetColor(COLOR_EDIT_SELBKGND));
        COLORREF crOldText = pdc->SetTextColor (GetColor(COLOR_EDIT_SELTEXT));
        DrawLineHelperImpl (pdc, ptOrigin, rcClip, pszChars, nOffset + nSelBegin, nSelEnd - nSelBegin);
        pdc->SetBkColor (crOldBk);
        pdc->SetTextColor (crOldText);
      }
      if (nSelEnd < nCount)
				DrawLineHelperImpl (pdc, ptOrigin, rcClip, pszChars, nOffset + nSelEnd, nCount - nSelEnd);
		}
		else
			DrawLineHelperImpl (pdc, ptOrigin, rcClip, pszChars, nOffset, nCount);
	}
}

void CTextWnd::GetLineColors (int nLineIndex, COLORREF & crBkgnd,
	COLORREF & crText, BOOL & bDrawWhitespace)
{
  DWORD dwLineFlags = GetLineFlags (nLineIndex);
  bDrawWhitespace = TRUE;
  crText = RGB (255, 255, 255);
  crBkgnd = CLR_NONE;
  crText = CLR_NONE;
  bDrawWhitespace = FALSE;
}

DWORD CTextWnd::GetParseCookie (int nLineIndex)
{
  int nLineCount = GetLineCount();
  if (m_pdwParseCookies == NULL)
	{
		m_nParseArraySize = nLineCount;
    m_pdwParseCookies = new DWORD[nLineCount];
    memset (m_pdwParseCookies, 0xff, nLineCount * sizeof (DWORD));
	}

  if (nLineIndex < 0)
    return 0;
  if (m_pdwParseCookies[nLineIndex] != (DWORD) - 1)
    return m_pdwParseCookies[nLineIndex];

  int L = nLineIndex;
  while (L >= 0 && m_pdwParseCookies[L] == (DWORD) - 1)
    L--;
  L++;

  int nBlocks;
  while (L <= nLineIndex)
  {
		DWORD dwCookie = 0;
    if (L > 0)
			dwCookie = m_pdwParseCookies[L - 1];
		ASSERT (dwCookie != (DWORD) - 1);
    m_pdwParseCookies[L] = m_pParser ? m_pParser->ParseLine(dwCookie, L, NULL, nBlocks) : 0;
    ASSERT (m_pdwParseCookies[L] != (DWORD) - 1);
    L++;
	}

  return m_pdwParseCookies[nLineIndex];
}

void CTextWnd::WrapLineCached(int nLineIndex, int nMaxLineWidth, 
	int *anBreaks, int &nBreaks)
{
	// if word wrap is not active, there is not any break in the line
	if(!m_bWordWrap)
	{
		nBreaks = 0;
		return;
	}

	// word wrap is active
	if(nLineIndex < m_panSubLines->GetSize() && !anBreaks && (*m_panSubLines)[nLineIndex] > -1)
		// return cached data
		nBreaks = (*m_panSubLines)[nLineIndex] - 1;
	else
	{
		// recompute line wrap
		nBreaks = 0;
		if (m_pParser) m_pParser->WrapLine(nLineIndex, nMaxLineWidth, anBreaks, nBreaks);

		// cache data
		ASSERT(nBreaks > -1);
		m_panSubLines->SetAtGrow(nLineIndex, nBreaks + 1);

		// RecalcVertScrollBar();
	}
}


void CTextWnd::InvalidateLineCache(int nLineIndex1, int nLineIndex2 /*= -1*/)
{
	// invalidate cached sub line count

	if(nLineIndex2 == -1 && nLineIndex1 < m_panSubLines->GetSize())
		for(int i = nLineIndex1; i < m_panSubLines->GetSize(); i++)
			(*m_panSubLines)[i] = -1;
	else
	{
		if(nLineIndex1 > nLineIndex2)
		{
			int	nStorage = nLineIndex1;
			nLineIndex1 = nLineIndex2;
			nLineIndex2 = nStorage;
		}

		if(nLineIndex1 >= m_panSubLines->GetSize())
			return;

		if(nLineIndex2 >= m_panSubLines->GetSize())
			nLineIndex2 = (int)m_panSubLines->GetUpperBound();

		for(int i = nLineIndex1; i <= nLineIndex2; i++)
			if(i >= 0 && i < m_panSubLines->GetSize())
				(*m_panSubLines)[i] = -1;
	}
}


void CTextWnd::DrawScreenLine(CDC *pdc, CPoint &ptOrigin, const CRect &rcClip,
	CTextBlock *pBuf, int nBlocks, int &nActualItem, 
	COLORREF crText, COLORREF crBkgnd, BOOL bDrawWhitespace,
	LPCTSTR pszChars, int nOffset, int nCount, CPoint ptTextPos)
{
	CPoint	originalOrigin = ptOrigin;
	CRect		frect = rcClip;
	int			nLength = GetLineLength(ptTextPos.y);

	frect.top = ptOrigin.y;
	frect.bottom = frect.top + GetLineHeight();

	ASSERT(nActualItem < nBlocks);

	if (crText == CLR_NONE)
	{
		pdc->SetTextColor(GetColor(pBuf[nActualItem].m_nColorIndex));
	}
	pdc->SelectObject(
		GetFont(GetItalic(pBuf[nActualItem].m_nColorIndex), 
		GetBold(pBuf[nActualItem].m_nColorIndex),
		GetUnderline(pBuf[nActualItem].m_nColorIndex)));

	if(nBlocks > 0 && nActualItem < nBlocks - 1 && 
		pBuf[nActualItem + 1].m_nCharPos >= nOffset && 
		pBuf[nActualItem + 1].m_nCharPos <= nOffset + nCount)
	{
		ASSERT(pBuf[nActualItem].m_nCharPos >= 0 && pBuf[nActualItem].m_nCharPos <= nLength);

		for (int I = nActualItem; I < nBlocks - 1 && pBuf[I + 1].m_nCharPos <= nOffset + nCount; I ++)
		{
			ASSERT(pBuf[I].m_nCharPos >= 0 && pBuf[I].m_nCharPos <= nLength);
			if (crText == CLR_NONE)
			{
				if (!IsColorMark() && pBuf[I].m_nColorIndex >= COLOR_EDIT_MARK0 && 
					pBuf[I].m_nColorIndex <= COLOR_EDIT_MARK15 && 
					((pBuf[I].m_nColorIndex - COLOR_EDIT_MARK0) & 8) != 0)
				{
					pdc->SetTextColor(GetColor(COLOR_EDIT_WHITESPACE));
					pdc->SetBkColor(GetColor(COLOR_EDIT_NORMALTEXT));
				}
				else
				{
					pdc->SetTextColor(GetColor(pBuf[I].m_nColorIndex));
					pdc->SetBkColor(GetColor(COLOR_EDIT_WHITESPACE));
				}
			}

			pdc->SelectObject(GetFont(GetItalic(pBuf[I].m_nColorIndex), GetBold(pBuf[I].m_nColorIndex),
				GetUnderline(pBuf[I].m_nColorIndex)));

			int nOffsetToUse = (nOffset > pBuf[I].m_nCharPos)? nOffset : pBuf[I].m_nCharPos;
			DrawLineHelper(pdc, ptOrigin, rcClip, pBuf[I].m_nColorIndex, pszChars,
					(nOffset > pBuf[I].m_nCharPos)? nOffset : pBuf[I].m_nCharPos, 
					pBuf[I + 1].m_nCharPos - nOffsetToUse,
					CPoint(nOffsetToUse, ptTextPos.y));
		}

		nActualItem = I;

		ASSERT(pBuf[nActualItem].m_nCharPos >= 0 && pBuf[nActualItem].m_nCharPos <= nLength);
		if (crText == CLR_NONE)
			pdc->SetTextColor(GetColor(pBuf[nActualItem].m_nColorIndex));

		pdc->SelectObject(GetFont(GetItalic(pBuf[nActualItem].m_nColorIndex),
							GetBold(pBuf[nActualItem].m_nColorIndex),
							GetUnderline(pBuf[nActualItem].m_nColorIndex)));
		DrawLineHelper(pdc, ptOrigin, rcClip, pBuf[nActualItem].m_nColorIndex, pszChars,
							pBuf[nActualItem].m_nCharPos, nOffset + nCount - pBuf[nActualItem].m_nCharPos,
							CPoint(pBuf[nActualItem].m_nCharPos, ptTextPos.y));
	}
	else
	{
		DrawLineHelper(
			pdc, ptOrigin, rcClip, pBuf[nActualItem].m_nColorIndex, 
			pszChars, nOffset, nCount, ptTextPos);
	}

	// Draw space on the right of the text
	if (ptOrigin.x > frect.left)
		frect.left = ptOrigin.x;
	if (frect.right > frect.left)
	{
		if ((m_bFocused || m_bShowInactiveSelection) 
			&& IsInsideSelBlock(CPoint(GetLineLength(ptTextPos.y), ptTextPos.y)) 
			&& (nOffset + nCount) == GetLineLength(ptTextPos.y))
		{
			pdc->FillSolidRect(frect.left, frect.top, GetCharWidth(), frect.Height(),
				GetColor(COLOR_EDIT_SELBKGND));
			frect.left += GetCharWidth();
		}
		if (frect.right > frect.left)
			pdc->FillSolidRect(frect, bDrawWhitespace ? crBkgnd : GetColor(COLOR_EDIT_WHITESPACE));
	}

	// set origin to beginning of next screen line
	ptOrigin.x = originalOrigin.x;
	ptOrigin.y+= GetLineHeight();
}

void CTextWnd::DrawSingleLine (CDC * pdc, const CRect & rc, int nLineIndex)
{
  ASSERT (nLineIndex >= -1 && nLineIndex < GetLineCount());

  if (nLineIndex == -1)
	{
      //  Draw line beyond the text
		pdc->FillSolidRect (rc, GetColor(COLOR_EDIT_WHITESPACE));
    return;
	}

  //  Acquire the background color for the current line
  BOOL bDrawWhitespace = FALSE;
  COLORREF crBkgnd, crText;
  GetLineColors (nLineIndex, crBkgnd, crText, bDrawWhitespace);
  if (crBkgnd == CLR_NONE)
    crBkgnd = GetColor(COLOR_EDIT_WHITESPACE);

  int nLength = GetLineLength (nLineIndex);
  if (nLength == 0)
  {
      //  Draw the empty line
		CRect rect = rc;
    if ((m_bFocused || m_bShowInactiveSelection) && IsInsideSelBlock (CPoint (0, nLineIndex)))
    {
			pdc->FillSolidRect (rect.left, rect.top, GetCharWidth(), rect.Height(), GetColor (COLOR_EDIT_SELBKGND));
			rect.left += GetCharWidth();
		}
    pdc->FillSolidRect (rect, bDrawWhitespace ? crBkgnd : GetColor (COLOR_EDIT_WHITESPACE));
    return;
	}

  //  Parse the line
  LPCTSTR pszChars = GetLineChars (nLineIndex);
  DWORD dwCookie = GetParseCookie (nLineIndex - 1);
  CTextBlock *pBuf = (CTextBlock *) _alloca (sizeof (CTextBlock) * nLength * 3);
  int nBlocks = 0;

	// insert at least one CTextBlock of normal color at the beginning
	pBuf[0].m_nCharPos = 0;
	pBuf[0].m_nColorIndex = COLOR_EDIT_NORMALTEXT;
	nBlocks++;

  m_pdwParseCookies[nLineIndex] = m_pParser ? m_pParser->ParseLine(dwCookie, nLineIndex, pBuf, nBlocks) : 0;
  ASSERT (m_pdwParseCookies[nLineIndex] != (DWORD) - 1);


	int nActualItem = 0;

	// Wrap the line
	int *anBreaks = (int*)_alloca(sizeof(int) * nLength);
	int	nBreaks = 0;

	WrapLineCached(nLineIndex, GetScreenChars(nLineIndex), anBreaks, nBreaks);


  //  Draw the line text
  CPoint origin (rc.left - m_nOffset, rc.top);
  pdc->SetBkColor (crBkgnd);
  if (crText != CLR_NONE)
    pdc->SetTextColor (crText);
	BOOL bColorSet = FALSE;

	// BEGIN SW
	if(nBreaks > 0)
	{
		// Draw all the screen lines of the wrapped line
		ASSERT(anBreaks[0] < nLength);
		
		// draw start of line to first break
		DrawScreenLine(
			pdc, origin, rc,
			pBuf, nBlocks, nActualItem,
			crText, crBkgnd, bDrawWhitespace,
			pszChars, 0, anBreaks[0], CPoint(0, nLineIndex));
		
		// draw from first break to last break
		for(int i = 0; i < nBreaks - 1; i++)
		{
			ASSERT(anBreaks[i] >= 0 && anBreaks[i] < nLength);
			DrawScreenLine(pdc, origin, rc, pBuf, nBlocks, nActualItem,
				crText, crBkgnd, bDrawWhitespace, pszChars, anBreaks[i], 
				anBreaks[i + 1] - anBreaks[i], CPoint(anBreaks[i], 
				nLineIndex));
		}
		
		// draw from last break till end of line
		DrawScreenLine(pdc, origin, rc, pBuf, nBlocks, nActualItem,
			crText, crBkgnd, bDrawWhitespace, pszChars, anBreaks[i], 
			nLength - anBreaks[i], CPoint(anBreaks[i], nLineIndex));
	}
	else DrawScreenLine(pdc, origin, rc, pBuf, nBlocks, nActualItem,
		crText, crBkgnd, bDrawWhitespace, pszChars, 0, nLength, 
		CPoint(0, nLineIndex));

	//	Draw whitespaces to the left of the text
}

DWORD CTextWnd::GetLineFlags (int nLineIndex)
{
  if (m_pTextBuffer == NULL)
    return 0;
  return m_pTextBuffer->GetLineFlags (nLineIndex);
}

BOOL CTextWnd::IsInsideSelBlock (CPoint ptTextPos)
{
  ASSERT_VALIDTEXTPOS (ptTextPos);
  if (ptTextPos.y < m_ptDrawSelStart.y)
    return FALSE;
  if (ptTextPos.y > m_ptDrawSelEnd.y)
    return FALSE;
  if (ptTextPos.y < m_ptDrawSelEnd.y && ptTextPos.y > m_ptDrawSelStart.y)
    return TRUE;
  if (m_ptDrawSelStart.y < m_ptDrawSelEnd.y)
	{
		if (ptTextPos.y == m_ptDrawSelEnd.y)
			return ptTextPos.x < m_ptDrawSelEnd.x;
		ASSERT (ptTextPos.y == m_ptDrawSelStart.y);
    return ptTextPos.x >= m_ptDrawSelStart.x;
	}
  ASSERT (m_ptDrawSelStart.y == m_ptDrawSelEnd.y);
  return ptTextPos.x >= m_ptDrawSelStart.x && ptTextPos.x < m_ptDrawSelEnd.x;
}

BOOL CTextWnd::IsInsideSelection (const CPoint & ptTextPos)
{
  PrepareSelBounds();
  return IsInsideSelBlock (ptTextPos);
}

void CTextWnd::PrepareSelBounds()
{
  if (m_ptSelStart.y < m_ptSelEnd.y ||
		(m_ptSelStart.y == m_ptSelEnd.y && m_ptSelStart.x < m_ptSelEnd.x))
  {
		m_ptDrawSelStart = m_ptSelStart;
    m_ptDrawSelEnd = m_ptSelEnd;
	}
  else
  {
		m_ptDrawSelStart = m_ptSelEnd;
    m_ptDrawSelEnd = m_ptSelStart;
	}
}

void CTextWnd::OnPaint()
{
	PAINTSTRUCT ps;
	CDC *pdc = BeginPaint(&ps);
  CRect rcClient;
  GetClientRect (rcClient);
  rcClient.left += m_nLeftMargin;

  int nLineCount = GetLineCount();
  int nLineHeight = GetLineHeight();
  PrepareSelBounds();

  CDC cacheDC;
  VERIFY (cacheDC.CreateCompatibleDC (pdc));
  if (m_pCacheBitmap == NULL)
	{
		m_pCacheBitmap = new CBitmap;
  	VERIFY(m_pCacheBitmap->CreateCompatibleBitmap(pdc, rcClient.Width(), rcClient.Height()));
	}
  CBitmap *pOldBitmap = cacheDC.SelectObject (m_pCacheBitmap);

  CRect rcLine;
  rcLine = rcClient;
  rcLine.bottom = rcLine.top + nLineHeight;
  CRect rcCacheLine (0, 0, rcLine.Width(), nLineHeight);

	// initialize rects
	int	nSubLineOffset = GetSubLineIndex(m_nTopLine) - m_nTopSubLine;
	if(nSubLineOffset < 0)
		rcCacheLine.OffsetRect(0, nSubLineOffset * nLineHeight);

	int		nBreaks;
	int		nMaxLineChars = GetScreenChars();


  int nCurrentLine = m_nTopLine;
  while (rcLine.top < rcClient.bottom)
	{
	
		nBreaks = 0;
		if(nCurrentLine < nLineCount /*&& GetLineLength(nCurrentLine) > nMaxLineChars*/)
			WrapLineCached(nCurrentLine, nMaxLineChars, NULL, nBreaks);

		rcLine.bottom = rcLine.top + (nBreaks + 1) * nLineHeight;
		rcCacheLine.bottom = rcCacheLine.top + rcLine.Height();

		if(rcCacheLine.top < 0)
		rcLine.bottom+= rcCacheLine.top;
	
    if (nCurrentLine < nLineCount)
			DrawSingleLine (&cacheDC, rcCacheLine, nCurrentLine);
		else DrawSingleLine (&cacheDC, rcCacheLine, -1);

		VERIFY (pdc->BitBlt (rcLine.left, rcLine.top, rcLine.Width(), rcLine.Height(), &cacheDC, 0, 0, SRCCOPY));

    nCurrentLine++;
	
  	rcLine.top = rcLine.bottom;
	  rcCacheLine.top = 0;
	}
	if (m_nLeftMargin > 0)
	{
		COLORREF btn_color = GetSysColor(COLOR_BTNFACE);
		CRect marg_rect(0, rcClient.top, m_nLeftMargin, rcClient.bottom);
		CBrush marg_brush(btn_color);
		CPen marg_pen(PS_SOLID, 1, btn_color);
		pdc->FillRect(&marg_rect, &marg_brush);
		pdc->SelectObject(marg_pen);
		pdc->DrawEdge(&marg_rect, EDGE_ETCHED, BF_RIGHT);
	}
	if (m_nRightMargin > 0)
	{
		int r = GetCharWidth() * m_nRightMargin + m_nLeftMargin;
		CPen marg_pen(PS_SOLID, 1, RGB(128, 128, 128));
		pdc->SelectObject(marg_pen);
		pdc->MoveTo(r, rcClient.top);
		pdc->LineTo(r, rcClient.bottom);
	}

  cacheDC.SelectObject (pOldBitmap);
  cacheDC.DeleteDC();
	EndPaint(&ps);
}

void CTextWnd::ResetView()
{
  m_nTopLine = 0;
  m_nTopSubLine = 0;
  m_nOffset = 0;
  m_nLineHeight = -1;
  m_nCharWidth = -1;
  m_nMaxCharWidth = -1;
  m_nMaxLineLength = -1;
  m_nIdealCharPos = -1;
  m_ptAnchor.x = 0;
  m_ptAnchor.y = 0;
  for (int I = 0; I < 8; I++)
  {
		if (m_apFonts[I] != NULL)
    {
			m_apFonts[I]->DeleteObject();
			delete m_apFonts[I];
			m_apFonts[I] = NULL;
		}
	}
  if (m_pdwParseCookies != NULL)
	{
		delete m_pdwParseCookies;
		m_pdwParseCookies = NULL;
	}
  if (m_pnActualLineLength != NULL)
	{
		delete m_pnActualLineLength;
    m_pnActualLineLength = NULL;
	}
  m_nParseArraySize = 0;
  m_nActualLengthArraySize = 0;
  m_ptCursorPos.x = 0;
  m_ptCursorPos.y = 0;
  m_ptSelStart = m_ptSelEnd = m_ptCursorPos;
  m_bDragSelection = FALSE;
  m_bVertScrollBarLocked = FALSE;
  m_bHorzScrollBarLocked = FALSE;
  if (::IsWindow (m_hWnd))
    UpdateCaret();
  m_bShowInactiveSelection = TRUE; 
  m_panScreenChars->SetSize(0, 4096);
}

void CTextWnd::UpdateCaret()
{
  ASSERT_VALIDTEXTPOS (m_ptCursorPos);
  ////////////////////////////////////////////////////////////////////////////////
  int actual_offset = CalculateActualOffset (m_ptCursorPos.y, m_ptCursorPos.x);
  CDC *pdc = GetDC();
  CFont *pOldFont = pdc->SelectObject(GetFont(FALSE, FALSE, FALSE));
  CSize sz = TextWidth(pdc, GetLineChars(m_ptCursorPos.y), m_ptCursorPos.x);
  pdc->SelectObject(pOldFont);
  ReleaseDC(pdc);
  ////////////////////////////////////////////////////////////////////////////////
  if (m_bFocused && !m_bCursorHidden &&	sz.cx >= m_nOffset)
	{
		CreateSolidCaret(2, GetLineHeight());
    SetCaretPos(TextToClient(m_ptCursorPos));
    ShowCaret();
	}
  else HideCaret();
}

int CTextWnd::GetTabSize()
{
  ASSERT (m_nTabSize >= 0 && m_nTabSize <= 64);
	return m_nTabSize;
}

void CTextWnd::SetTabSize (int nTabSize)
{
  ASSERT (nTabSize >= 0 && nTabSize <= 64);
  if (m_nTabSize != nTabSize)
	{
		m_nTabSize = nTabSize;
    if (m_pnActualLineLength != NULL)
		{
			delete m_pnActualLineLength;
      m_pnActualLineLength = NULL;
		}
    m_nActualLengthArraySize = 0;
    m_nMaxLineLength = -1;
    RecalcHorzScrollBar();
    Invalidate();
    UpdateCaret();
	}
}

int CALLBACK GFEnumFontFamProc(ENUMLOGFONT *lpelf, NEWTEXTMETRIC *lpntm, DWORD FontType, LPARAM lParam)
{
	((LOGFONT*)lParam)->lfPitchAndFamily = lpelf->elfLogFont.lfPitchAndFamily;
	return 0;
}

CFont *CTextWnd::GetFont (BOOL bItalic /*= FALSE*/ , BOOL bBold /*= FALSE*/, BOOL bUnderline)
{
  int nIndex = 0;
  if (bBold)
    nIndex |= 1;
  if (bItalic)
    nIndex |= 2;
	if (bUnderline)
		nIndex |= 4;

  if (m_apFonts[nIndex] == NULL)
  {
		m_apFonts[nIndex] = new CFont;
		CClientDC dc (GetDesktopWindow());
    if (!m_lfBaseFont.lfHeight)
    {
      m_lfBaseFont.lfHeight = -MulDiv (11, dc.GetDeviceCaps (LOGPIXELSY), 72);
		}
    m_lfBaseFont.lfWeight = bBold ? FW_BOLD : FW_NORMAL;
    m_lfBaseFont.lfItalic = (BYTE)bItalic;
    m_lfBaseFont.lfUnderline = (BYTE)bUnderline;

		if (m_bOemCharset)
			m_lfBaseFont.lfCharSet = OEM_CHARSET;
		else m_lfBaseFont.lfCharSet = DEFAULT_CHARSET;

    EnumFontFamilies(dc.m_hDC, m_lfBaseFont.lfFaceName, (FONTENUMPROC)GFEnumFontFamProc, (LPARAM)&m_lfBaseFont);
    if (!m_apFonts[nIndex]->CreateFontIndirect (&m_lfBaseFont))
    {
			delete m_apFonts[nIndex];
      m_apFonts[nIndex] = NULL;
      return CWnd::GetFont();
		}
	}
  return m_apFonts[nIndex];
}

void CTextWnd::CalcLineCharDim()
{
  CDC *pdc = GetDC();
  TEXTMETRIC tm;
  CFont *pOldFont = pdc->SelectObject(GetFont(FALSE, FALSE, FALSE));
  pdc->GetTextMetrics(&tm);
  m_nLineHeight = pdc->GetTextExtent("X").cy;//tm.tmHeight;
  m_nCharWidth = tm.tmAveCharWidth;
  m_nMaxCharWidth = tm.tmMaxCharWidth;
  /*
  CSize szCharExt = pdc->GetTextExtent("X");
  m_nLineHeight = szCharExt.cy;
  if (m_nLineHeight < 1)
    m_nLineHeight = 1;
  m_nCharWidth = szCharExt.cx;
  */
  pdc->SelectObject(pOldFont);
  ReleaseDC (pdc);
}

int CTextWnd::GetLineHeight()
{
  if (m_nLineHeight == -1)
    CalcLineCharDim();
  return m_nLineHeight;
}

int CTextWnd::GetSubLines(int nLineIndex)
{
	// get number of wrapped lines, this line contains of
	int	nBreaks = 0;
	WrapLineCached(nLineIndex, GetScreenChars(nLineIndex), NULL, nBreaks);

	return nBreaks + 1;
}

int CTextWnd::CharPosToPoint(int nLineIndex, int nCharPos, CPoint &charPoint)
{
	// if we do not wrap lines, y is allways 0 and x is equl to nCharPos
	if(!m_bWordWrap)
	{
		charPoint.x = nCharPos;
		charPoint.y = 0;
	}

	// line is wrapped
	int *anBreaks = (int*)_alloca(sizeof(int) * GetLineLength(nLineIndex));
	int	nBreaks = 0;

	WrapLineCached(nLineIndex, GetScreenChars(nLineIndex), anBreaks, nBreaks);

	for(int i = nBreaks - 1; i >= 0 && nCharPos < anBreaks[i]; i--);

	charPoint.x = (i >= 0)? nCharPos - anBreaks[i] : nCharPos;
	charPoint.y = i + 1;

	return (i >= 0)? anBreaks[i] : 0;
}

int CTextWnd::CursorPointToCharPos(int nLineIndex, const CPoint &curPoint)
{
	// calculate char pos out of point
	int			nLength = GetLineLength(nLineIndex);
	int			nScreenChars = GetScreenChars(nLineIndex);
	LPCTSTR	szLine = GetLineChars(nLineIndex);

	// wrap line
	int *anBreaks = (int*)_alloca(sizeof(int) * nLength);
	int	nBreaks = 0;

	WrapLineCached(nLineIndex, nScreenChars, anBreaks, nBreaks);

	// find char pos that matches cursor position
	int nXPos = 0;
	int nYPos = 0;
	int	nCurPos = 0;
	int nTabSize = GetTabSize();

	for(int nIndex = 0; nIndex < nLength; nIndex++)
	{
		if(nBreaks && nIndex == anBreaks[nYPos])
		{
			nXPos = 0;
			nYPos++;
		}

		if (szLine[nIndex] == _T('\t'))
		{
			if (GetEqualTabs()) 
			{
				nXPos += nTabSize;
				nCurPos += nTabSize;
			}
			else 
			{
				nXPos+= (nTabSize - nCurPos % nTabSize);
				nCurPos+= (nTabSize - nCurPos % nTabSize);
			}
		}
		else
		{
			nXPos++;
			nCurPos++;
		}

		if(nXPos > curPoint.x && nYPos == curPoint.y)
			break;
		else if(nYPos > curPoint.y)
		{
			nIndex--;
			break;
		}
	}

	return nIndex;	
}

void CTextWnd::SubLineCursorPosToTextPos(const CPoint &subLineCurPos, CPoint &textPos)
{
	// Get line breaks
	int	nSubLineOffset, nLine;

	GetLineBySubLine(subLineCurPos.y, nLine, nSubLineOffset);

	// compute cursor-position
	textPos.x = CursorPointToCharPos(nLine, CPoint(subLineCurPos.x, nSubLineOffset));
	textPos.y = nLine;
}

int CTextWnd::SubLineEndToCharPos(int nLineIndex, int nSubLineOffset)
{
	int		nLength = GetLineLength(nLineIndex);

	// if word wrapping is disabled, the end is equal to the length of the line -1
	if(!m_bWordWrap /*|| nLength <= GetScreenChars()*/)
		return nLength;

	// wrap line
	int *anBreaks = (int*)_alloca(sizeof(int) * nLength);
	int	nBreaks = 0;

	WrapLineCached(nLineIndex, GetScreenChars(nLineIndex), anBreaks, nBreaks);

	// if there is no break inside the line or the given subline is the last
	// one in this line...
	if(!nBreaks || nSubLineOffset == nBreaks)
		return nLength;

	// compute character position for end of subline
	ASSERT(nSubLineOffset >= 0 && nSubLineOffset <= nBreaks);
	
	return anBreaks[nSubLineOffset] - 1;
}

int CTextWnd::SubLineHomeToCharPos(int nLineIndex, int nSubLineOffset)
{
	int		nLength = GetLineLength(nLineIndex);

	// if word wrapping is disabled, the start is 0
	if(!m_bWordWrap || nSubLineOffset == 0)
		return 0;

	// wrap line
	int *anBreaks = (int*)_alloca(sizeof(int) * nLength);
	int	nBreaks = 0;

	WrapLineCached(nLineIndex, GetScreenChars(nLineIndex), anBreaks, nBreaks);

	// if there is no break inside the line...
	if(!nBreaks)
		return 0;

	// compute character position for end of subline
	ASSERT(nSubLineOffset > 0 && nSubLineOffset <= nBreaks);
	
	return anBreaks[nSubLineOffset - 1];
}

int CTextWnd::GetCharWidth()
{
  if (m_nCharWidth == -1)
    CalcLineCharDim();
  return m_nCharWidth;
}

int CTextWnd::GetMaxCharWidth()
{
  if (m_nMaxCharWidth == -1)
    CalcLineCharDim();
  return m_nMaxCharWidth;
}

int CTextWnd::GetMaxLineLength()
{
  if (m_nMaxLineLength == -1)
	{
		m_nMaxLineLength = 0;
    int nLineCount = GetLineCount();
    for (int I = 0; I < nLineCount; I++)
		{
			int nActualLength = GetLineActualLength (I);
      if (m_nMaxLineLength < nActualLength)
				m_nMaxLineLength = nActualLength;
		}
	}
  return m_nMaxLineLength;
}

void CTextWnd::GoToLine (int nLine, bool bRelative)
{
  int nLines = m_pTextBuffer->GetLineCount() - 1;
  CPoint ptCursorPos = GetCursorPos();
  if (bRelative) nLine += ptCursorPos.y;
  if (nLine) nLine--;
  if (nLine > nLines) nLine = nLines;
  if (nLine >= 0)
	{
		int nChars = m_pTextBuffer->GetLineLength (nLine);
    if (nChars) nChars--;
		if (ptCursorPos.x > nChars) ptCursorPos.x = nChars;
		if (ptCursorPos.x >= 0)
    {
			ptCursorPos.y = nLine;
      ASSERT_VALIDTEXTPOS (ptCursorPos);
      SetAnchor (ptCursorPos);
      SetSelection (ptCursorPos, ptCursorPos);
      SetCursorPos (ptCursorPos);
      EnsureVisible (ptCursorPos);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CTextWnd message handlers

int CTextWnd::GetLineCount()
{
  if (m_pTextBuffer == NULL)
    return 1;                   //  Single empty line

  int nLineCount = m_pTextBuffer->GetLineCount();
  ASSERT (nLineCount > 0);
  return nLineCount;
}

int CTextWnd::GetSubLineCount()
{
	if(!m_bWordWrap)
		return GetLineCount();

	// calculate number of sub lines
	int	nLineCount = GetLineCount();
	int nSubLineCount = 0;

	for(int i = 0; i < nLineCount; i++)
		nSubLineCount += GetSubLines(i);

	return nSubLineCount;
}

int CTextWnd::GetSubLineIndex(int nLineIndex)
{
	// if we do not wrap words, subline index of this line is equal to its index
	if(!m_bWordWrap)
		return nLineIndex;

	// calculate subline index of the line
	int	nSubLineCount = 0;

	if(nLineIndex >= GetLineCount())
		nLineIndex = GetLineCount() - 1;

	for(int i = 0; i < nLineIndex; i++)
		nSubLineCount+= GetSubLines(i);

	return nSubLineCount;
}

void CTextWnd::GetLineBySubLine(int nSubLineIndex, int &nLine, int &nSubLine)
{
	ASSERT(nSubLineIndex < GetSubLineCount());

	// if we do not wrap words, nLine is equal to nSubLineIndex and nSubLine is allways 0
	if(!m_bWordWrap)
	{
		nLine = nSubLineIndex;
		nSubLine = 0;
	}

	// compute result
	int	nSubLineCount = 0;
	int	nLineCount = GetLineCount();

	for(int i = 0; i < nLineCount; i++)
	{
		nSubLineCount+= GetSubLines(i);
		if(!(nSubLineCount <= nSubLineIndex))
			break;
	}

	ASSERT(i < nLineCount);
	nLine = i;
	nSubLine = nSubLineIndex - (nSubLineCount - GetSubLines(i));
}
//END SW

int CTextWnd::GetLineLength (int nLineIndex)
{
  if (m_pTextBuffer == NULL)
    return 0;
  return m_pTextBuffer->GetLineLength (nLineIndex);
}

LPCTSTR CTextWnd::GetLineChars (int nLineIndex)
{
  if (m_pTextBuffer == NULL)
    return NULL;
  return m_pTextBuffer->GetLineChars (nLineIndex);
}

void CTextWnd::AttachToBuffer (CTextBuffer * pBuf)
{
  if (m_pTextBuffer != NULL)
    m_pTextBuffer->RemoveView (this);
  m_pTextBuffer = pBuf;
  if (m_pTextBuffer != NULL)
    m_pTextBuffer->AddView (this);
  ResetView();

  //  Init scrollbars
  CScrollBar *pVertScrollBarCtrl = GetScrollBarCtrl (SB_VERT);
  if (pVertScrollBarCtrl != NULL) pVertScrollBarCtrl->EnableScrollBar (GetScreenLines() >= GetLineCount()?
		ESB_DISABLE_BOTH : ESB_ENABLE_BOTH);
  CScrollBar *pHorzScrollBarCtrl = GetScrollBarCtrl (SB_HORZ);
  if (pHorzScrollBarCtrl != NULL) pHorzScrollBarCtrl->EnableScrollBar (GetScreenChars() >= GetMaxLineLength()?
		ESB_DISABLE_BOTH : ESB_ENABLE_BOTH);

  //  Update scrollbars
  RecalcVertScrollBar();
  RecalcHorzScrollBar();
}

void CTextWnd::DetachFromBuffer()
{
  if (m_pTextBuffer != NULL)
	{
		m_pTextBuffer->RemoveView (this);
		m_pTextBuffer = NULL;
    ResetView();
	}
}

int CTextWnd::GetScreenLines()
{
  if (m_nScreenLines == -1)
	{
		CRect rect;
		GetClientRect (&rect);
		//m_nScreenLines = rect.Height() / GetLineHeight();
		return rect.Height() / GetLineHeight();
	}
  return m_nScreenLines;
}

int CTextWnd::GetScreenChars(int nLine)
{
  if (m_nScreenChars == -1)
	{
		if (m_lfBaseFont.lfPitchAndFamily & FIXED_PITCH)
			return GetScreenChars();
		
		if (m_panScreenChars && nLine < m_panScreenChars->GetSize() && m_panScreenChars->GetAt(nLine) >= 0)
			return m_panScreenChars->GetAt(nLine);
		else
		{
			CRect rect;
			GetClientRect (&rect);
			rect.left = m_nLeftMargin;
			LPCSTR pszChars = GetLineChars(nLine);
			int n = 0;
			CDC *pdc = GetDC();
			CFont *pOldFont = pdc->SelectObject(GetFont(FALSE, FALSE, FALSE));
    
			CString eline;
			ExpandChars(pszChars, 0, GetLineLength(nLine), eline);
			while (n < eline.GetLength() && TextWidth(pdc, eline, n) < rect.Width()) n++;
    
			pdc->SelectObject(pOldFont);
			ReleaseDC(pdc);
			m_panScreenChars->SetAtGrow(nLine, n + 1);
			return n + 1;
		}
		
	}
  return m_nScreenChars;
}

int CTextWnd::GetScreenChars()
{
  if (m_nScreenChars == -1)
	{
		CRect rect;
    GetClientRect(&rect);
    //m_nScreenChars = rect.Width() / GetCharWidth();
		return (rect.Width() - m_nLeftMargin) / GetCharWidth();
	}
  return m_nScreenChars;
}

void CTextWnd::OnDestroy()
{
  GetFont(FALSE, FALSE, FALSE)->GetLogFont(&m_lfBaseFont);
  DetachFromBuffer();

  CWnd::OnDestroy();

  for (int I = 0; I < 8; I++)
  {
		if (m_apFonts[I] != NULL)
    {
			m_apFonts[I]->DeleteObject();
      delete m_apFonts[I];
      m_apFonts[I] = NULL;
    }
	}
  if (m_pCacheBitmap != NULL)
  {
		delete m_pCacheBitmap;
    m_pCacheBitmap = NULL;
	}
}

BOOL CTextWnd::OnEraseBkgnd (CDC * pdc)
{
  return TRUE;
}

void CTextWnd::OnSize(UINT nType, int cx, int cy)
{
  CWnd::OnSize (nType, cx, cy);

	// get char position of top left visible character with old cached word wrap
	CPoint	topPos;
	SubLineCursorPosToTextPos(CPoint(0, m_nTopSubLine), topPos);

  if (m_pCacheBitmap != NULL)
	{
		m_pCacheBitmap->DeleteObject();
		delete m_pCacheBitmap;
		m_pCacheBitmap = NULL;
	}

	// we have to recompute the line wrapping
	InvalidateLineCache(0, -1);

	// compute new top sub line
	CPoint	topSubLine;
	CharPosToPoint(topPos.y, topPos.x, topSubLine);
	m_nTopSubLine = topPos.y + topSubLine.y;

	// set caret to right position
	UpdateCaret();

  RecalcVertScrollBar();
  RecalcHorzScrollBar();

	if (m_panScreenChars) 
		m_panScreenChars->SetSize(0, 4096);
		
	Invalidate();
	UpdateWindow();
}

void CTextWnd::RecalcVertScrollBar (BOOL bPositionOnly /*= FALSE*/)
{
  SCROLLINFO si;
  si.cbSize = sizeof (si);
  if (bPositionOnly)
	{
		si.fMask = SIF_POS;
		si.nPos = m_nTopSubLine;
	}
  else
	{
		if(GetScreenLines() >= GetSubLineCount() && m_nTopSubLine > 0)
		{
			m_nTopLine = 0;
      Invalidate();
      UpdateCaret();
		}
		si.fMask = SIF_DISABLENOSCROLL | SIF_PAGE | SIF_POS | SIF_RANGE;
		si.nMin = 0;
	
		si.nMax = GetSubLineCount() - 1;
    si.nPage = GetScreenLines();
		si.nPos = m_nTopSubLine;
	}
  VERIFY (SetScrollInfo (SB_VERT, &si));
}

void CTextWnd::OnVScroll (UINT nSBCode, UINT nPos, CScrollBar * pScrollBar)
{
  CWnd::OnVScroll (nSBCode, nPos, pScrollBar);

  //  Note we cannot use nPos because of its 16-bit nature
  SCROLLINFO si;
  si.cbSize = sizeof (si);
  si.fMask = SIF_ALL;
  VERIFY (GetScrollInfo (SB_VERT, &si));


	int nPageLines = GetScreenLines();
	int nSubLineCount = GetSubLineCount();

	int nNewTopSubLine;
	BOOL bDisableSmooth = TRUE;
	switch (nSBCode)
	{
	case SB_TOP:
		nNewTopSubLine = 0;
		bDisableSmooth = FALSE;
		break;
	case SB_BOTTOM:
		nNewTopSubLine = nSubLineCount - nPageLines + 1;
		bDisableSmooth = FALSE;
		break;
	case SB_LINEUP:
		nNewTopSubLine = m_nTopSubLine - 1;
		break;
	case SB_LINEDOWN:
		nNewTopSubLine = m_nTopSubLine + 1;
		break;
	case SB_PAGEUP:
		nNewTopSubLine = m_nTopSubLine - si.nPage + 1;
		bDisableSmooth = FALSE;
		break;
	case SB_PAGEDOWN:
		nNewTopSubLine = m_nTopSubLine + si.nPage - 1;
		bDisableSmooth = FALSE;
		break;
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		nNewTopSubLine = si.nTrackPos;
		break;
	default:
		return;
	}

	if (nNewTopSubLine < 0)
		nNewTopSubLine = 0;
	if (nNewTopSubLine >= nSubLineCount)
		nNewTopSubLine = nSubLineCount - 1;
	ScrollToSubLine(nNewTopSubLine, bDisableSmooth);
}


void CTextWnd::RecalcHorzScrollBar (BOOL bPositionOnly /*= FALSE*/)
{
  //  Again, we cannot use nPos because it's 16-bit
  SCROLLINFO si;
  si.cbSize = sizeof (si);
  if (bPositionOnly)
	{
		si.fMask = SIF_POS;
    si.nPos = m_nOffset;
	}
  else
  {
		if (GetScreenChars() >= GetMaxLineLength() && m_nOffset > 0)
    {
			m_nOffset = 0;
      Invalidate();
      UpdateCaret();
		}
    si.fMask = SIF_DISABLENOSCROLL | SIF_PAGE | SIF_POS | SIF_RANGE;
    si.nMin = 0;
    si.nMax = (GetMaxLineLength() - 1) * GetMaxCharWidth();
    si.nPage = GetScreenChars() * GetMaxCharWidth();
    si.nPos = m_nOffset;
	}
  VERIFY (SetScrollInfo (SB_HORZ, &si));
}

void CTextWnd::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar * pScrollBar)
{
  CWnd::OnHScroll(nSBCode, nPos, pScrollBar);

  SCROLLINFO si;
  si.cbSize = sizeof (si);
  si.fMask = SIF_ALL;
  VERIFY (GetScrollInfo (SB_HORZ, &si));

  int nPageChars = GetScreenChars();
  int nMaxLineLength = GetMaxLineLength();

  int nNewOffset;
  switch (nSBCode)
	{
    case SB_LEFT:
      nNewOffset = 0;
      break;
    case SB_BOTTOM:
      nNewOffset = (nMaxLineLength - nPageChars) * GetMaxCharWidth() + 1;
      break;
    case SB_LINEUP:
      nNewOffset = m_nOffset - GetCharWidth();
      break;
    case SB_LINEDOWN:
      nNewOffset = m_nOffset + GetCharWidth();
      break;
    case SB_PAGEUP:
      nNewOffset = m_nOffset - si.nPage + 1;
      break;
    case SB_PAGEDOWN:
      nNewOffset = m_nOffset + si.nPage - 1;
      break;
    case SB_THUMBPOSITION:
    case SB_THUMBTRACK:
      nNewOffset = si.nTrackPos;
      break;
    default:
      return;
	}

  /*
  if (nNewOffset >= nMaxLineLength)
    nNewOffset = nMaxLineLength - 1;
  */
  if (nNewOffset < 0)
    nNewOffset = 0;
  ScrollToOffset(nNewOffset, TRUE);
  UpdateCaret();
}

BOOL CTextWnd::OnSetCursor (CWnd * pWnd, UINT nHitTest, UINT message)
{
  if (nHitTest == HTCLIENT)
	{
		CPoint pt;
		::GetCursorPos (&pt);
    ScreenToClient (&pt);
    CPoint ptText = ClientToText (pt);
    PrepareSelBounds();
    ::SetCursor (::LoadCursor (NULL, MAKEINTRESOURCE (IDC_IBEAM)));
    return TRUE;
	}
  return CWnd::OnSetCursor (pWnd, nHitTest, message);
}

CPoint CTextWnd::ClientToText (const CPoint & point)
{
	int nSubLineCount = GetSubLineCount();
	int nLineCount = GetLineCount();

	CPoint pt;
	pt.y = m_nTopSubLine + point.y / GetLineHeight();
	if (pt.y >= nSubLineCount)
		pt.y = nSubLineCount - 1;
	if (pt.y < 0)
		pt.y = 0;

	int nLine;
	int nSubLineOffset;

	GetLineBySubLine(pt.y, nLine, nSubLineOffset);
	pt.y = nLine;

	//int nOffsetChar = m_nOffset;
	int nOffsetChar = 0;
	
	///////////////////////////////////////////////////////////////////////////////////
  CDC *pdc = GetDC();
	CFont *pOldFont = pdc->SelectObject(GetFont(FALSE, FALSE, FALSE));
	///////////////////////////////////////////////////////////////////////////////////

	LPCTSTR pszLine = NULL;
	int	nLength = 0;
	int *anBreaks = NULL;
	int	nBreaks = 0;
	int cur_len;
	if (pt.y >= 0 && pt.y < nLineCount)
	{
		nLength = GetLineLength(pt.y);
		anBreaks = (int*)_alloca(sizeof(int) * nLength);
		pszLine = GetLineChars(pt.y);
		WrapLineCached(pt.y, GetScreenChars(pt.y), anBreaks, nBreaks);

		/*
		if (nBreaks > 0)
			nOffsetChar = anBreaks[nBreaks - 1];
		else
		*/
		if(nSubLineOffset > 0)
			nOffsetChar = anBreaks[nSubLineOffset - 1];
		else
		{
			nOffsetChar = 0;
			cur_len = 0;
			while (cur_len < m_nOffset && nOffsetChar < nLength)
			{
				nOffsetChar++;
				cur_len = TextWidth(pdc, pszLine, nOffsetChar);
			}
		}
		if(nBreaks > nSubLineOffset)
			nLength = anBreaks[nSubLineOffset] - 1;
	}

	//int nPos = nOffsetChar + point.x / GetCharWidth();
	///////////////////////////////////////////////////////////////////////////////////
  cur_len = 0; int nPos = 0;
  while (cur_len < (point.x - m_nLeftMargin) && nOffsetChar + nPos < nLength)
  {
		nPos++;		
		CString eline;
		ASSERT(nOffsetChar >= 0);
		ExpandChars(pszLine, nOffsetChar, nPos, eline);
		cur_len = TextWidth(pdc, eline);
  }
  pdc->SelectObject(pOldFont);
	ReleaseDC(pdc);
	nPos += nOffsetChar;
	///////////////////////////////////////////////////////////////////////////////////
	
	if (nPos < 0)
		nPos = 0;

	int nIndex = 0, nCurPos = 0, n = 0, i = 0;
	int nTabSize = GetTabSize();

	while (nIndex < nLength)
	{
		if(nBreaks && nIndex == anBreaks[i])
		{
			n = nIndex;
			i++;
		}

		/****
		if (pszLine[nIndex] == _T('\t'))
		{
			if (GetEqualTabs()) 
			{
				n += nTabSize;
				nCurPos += nTabSize;
			}
			else 
			{
				n+= (nTabSize - nCurPos % nTabSize);
				nCurPos += (nTabSize - nCurPos % nTabSize);
			}
		}
		else
		****/
		{
			n++;
			nCurPos ++;
		}

		if (n > nPos && i == nSubLineOffset)
			break;

		nIndex ++;
	}

	ASSERT(nIndex >= 0 && nIndex <= nLength);
	pt.x = nIndex;
	return pt;

}

#ifdef _DEBUG
void CTextWnd::AssertValidTextPos (const CPoint & point)
{
  if (GetLineCount() > 0)
	{
		ASSERT (m_nTopLine >= 0 && m_nOffset >= 0);
    ASSERT (point.y >= 0 && point.y < GetLineCount());
    ASSERT (point.x >= 0 && point.x <= GetLineLength (point.y));
	}
}
#endif

bool CTextWnd::IsValidTextPos (const CPoint &point)
{
  return GetLineCount() > 0 && m_nTopLine >= 0 && m_nOffset >= 0 &&
    point.y >= 0 && point.y < GetLineCount() && point.x >= 0 && point.x <= GetLineLength (point.y);
}

bool CTextWnd::IsValidTextPosX (const CPoint &point)
{
  return GetLineCount() > 0 && m_nTopLine >= 0 && m_nOffset >= 0 &&
    point.y >= 0 && point.y < GetLineCount() && point.x >= 0 && point.x <= GetLineLength (point.y);
}

bool CTextWnd::IsValidTextPosY (const CPoint &point)
{
  return GetLineCount() > 0 && m_nTopLine >= 0 && m_nOffset >= 0 &&
    point.y >= 0 && point.y < GetLineCount();
}

CPoint CTextWnd::TextToClient (const CPoint & point)
{
  ASSERT_VALIDTEXTPOS (point);
  LPCTSTR pszLine = GetLineChars (point.y);

  CPoint pt;

	CPoint	charPoint;
	int			nSubLineStart = CharPosToPoint(point.y, point.x, charPoint);
	charPoint.y+= GetSubLineIndex(point.y);

	// compute y-position
	pt.y = (charPoint.y - m_nTopSubLine) * GetLineHeight();

	// if pt.x is null, we know the result
	if(charPoint.x == 0)
	{
		pt.x = m_nLeftMargin;
		return pt;
	}

	// we have to calculate x-position
	int	nPreOffset = 0;
	pt.x = 0;

  //int nTabSize = GetTabSize();
  for (int nIndex = 0; nIndex < point.x; nIndex++)
	{
		if(nIndex == nSubLineStart)
			nPreOffset = pt.x;
		/*
		if (pszLine[nIndex] == _T ('\t'))
			if (GetEqualTabs()) pt.x += nTabSize;
			else pt.x += (nTabSize - pt.x % nTabSize);
		else
		*/
			pt.x++;
	}

	pt.x -= nPreOffset;
	//pt.x = (pt.x - m_nOffsetChar) * GetCharWidth();
	
	///////////////////////////////////////////////////////////////////////////////////
	CString eline;
	ExpandChars(pszLine + nPreOffset, 0, pt.x, eline);
  CDC *pdc = GetDC();
  CFont *pOldFont = pdc->SelectObject(GetFont(FALSE, FALSE, FALSE));
	//pt.x = pdc->GetTextExtent(pszLine + nPreOffset, pt.x).cx - m_nOffsetChar;
	pt.x = TextWidth(pdc, eline) - m_nOffset + m_nLeftMargin;
  pdc->SelectObject(pOldFont);
  ReleaseDC(pdc);
	///////////////////////////////////////////////////////////////////////////////////

  return pt;
}

void CTextWnd::InvalidateLines (int nLine1, int nLine2, BOOL bInvalidateMargin /*= FALSE*/)
{
  bInvalidateMargin = TRUE;
  if (nLine2 == -1)
	{
		CRect rcInvalid;
    GetClientRect (&rcInvalid);
  	rcInvalid.top = (GetSubLineIndex(nLine1) - m_nTopSubLine) * GetLineHeight();
    InvalidateRect (&rcInvalid, FALSE);
	}
  else
  {
		if (nLine2 < nLine1)
    {
			int nTemp = nLine1;
      nLine1 = nLine2;
      nLine2 = nTemp;
		}
    CRect rcInvalid;
    GetClientRect (&rcInvalid);
	
		rcInvalid.top = (GetSubLineIndex(nLine1) - m_nTopSubLine) * GetLineHeight();
		rcInvalid.bottom = (GetSubLineIndex(nLine2) - m_nTopSubLine + GetSubLines(nLine2)) * GetLineHeight();
    InvalidateRect (&rcInvalid, FALSE);
	}
}

void CTextWnd::SetSelection (const CPoint & ptStart, const CPoint & ptEnd)
{
  ASSERT_VALIDTEXTPOS (ptStart);
  ASSERT_VALIDTEXTPOS (ptEnd);
  if (m_ptSelStart == ptStart)
  {
		if (m_ptSelEnd != ptEnd)
			InvalidateLines (ptEnd.y, m_ptSelEnd.y);
	}
  else
  {
		InvalidateLines (ptStart.y, ptEnd.y);
    InvalidateLines (m_ptSelStart.y, m_ptSelEnd.y);
	}
  m_ptSelStart = ptStart;
  m_ptSelEnd = ptEnd;
}

void CTextWnd::AdjustTextPoint(CPoint & point)
{
  //point.x += GetCharWidth() / 2;
	///////////////////////////////////////////////////////////////////////////////////
	/*
	CDC *pdc = GetDC();
	CPoint pt = ClientToText(point);
	CFont *pOldFont = pdc->SelectObject(GetFont());
  point.x += pdc->GetTextExtent(GetLineChars(pt.y) + pt.x, 1).cx / 2;
  pdc->SelectObject (pOldFont);
  ReleaseDC (pdc);
  */
	///////////////////////////////////////////////////////////////////////////////////
}

void CTextWnd::OnSetFocus(CWnd * pOldWnd)
{
  CWnd::OnSetFocus (pOldWnd);

	int id = GetWindowLong(m_hWnd, GWL_ID); 
	NMHDR nmhdr = { m_hWnd, id, NM_SETFOCUS };
	GetParent()->SendMessage(WM_NOTIFY, id, (LPARAM)&nmhdr);
  m_bFocused = TRUE;
  if (m_ptSelStart != m_ptSelEnd)
    InvalidateLines (m_ptSelStart.y, m_ptSelEnd.y);
  UpdateCaret();
}

char *strnstr(char *str1, char *str2, int n)
{
	int len = (int)strlen(str2);
	for (int j = 0; j < n - len; j++)
	{
		int i;
		for (i = 0; i < len; i++)
			if (str2[i] != str1[j + i]) break;
		if (i == len) return str1 + i;
	}
	return 0;
}

char *memrchr(char *str, char v, int len)
{
	char *res = 0;
	for (int j = 0; j < len; j++)
		if (str[j] == v) res = str + j;
	return res;
}

int CTextWnd::CalculateActualOffset (int nLineIndex, int nCharIndex)
{
  int nLength = GetLineLength(nLineIndex);
  ASSERT (nCharIndex >= 0 && nCharIndex <= nLength);
  LPCTSTR pszChars = GetLineChars (nLineIndex);
  int nOffset = 0;
  int nTabSize = GetTabSize();

	int			*anBreaks = (int*)_alloca(sizeof(int) * nLength);
	int			nBreaks = 0;

	WrapLineCached(nLineIndex, GetScreenChars(nLineIndex), anBreaks, nBreaks);

	int	nPreOffset = 0;
	int	nPreBreak = 0;

	if(nBreaks)
	{
		for (int J = nBreaks - 1; J >= 0 && nCharIndex < anBreaks[J]; J--);
		nPreBreak = anBreaks[J];
	}

  for (int I = 0; I < nCharIndex; I++)
	{
	
		if(nPreBreak == I && nBreaks)
			nPreOffset = nOffset;
	
		if (pszChars[I] == _T ('\t'))
			if (GetEqualTabs()) nOffset += nTabSize;
			else nOffset += (nTabSize - nOffset % nTabSize);
		else nOffset++;
	}

	if(nPreBreak == I && nBreaks) return 0;
	else return nOffset - nPreOffset;
}

int CTextWnd::ApproxActualOffset (int nLineIndex, int nOffset)
{
  if (nOffset == 0)
    return 0;

  int nLength = GetLineLength (nLineIndex);
  LPCTSTR pszChars = GetLineChars (nLineIndex);
  int nCurrentOffset = 0;
  int nTabSize = GetTabSize();
  for (int I = 0; I < nLength; I++)
  {
		if (pszChars[I] == _T ('\t'))
			nCurrentOffset += (nTabSize - nCurrentOffset % nTabSize);
		else nCurrentOffset++;
		if (nCurrentOffset >= nOffset)
    {
			if (nOffset <= nCurrentOffset - nTabSize / 2)
				return I;
			return I + 1;
		}
	}
  return nLength;
}

void CTextWnd::EnsureVisible(CPoint pt)
{
	int	nSubLineCount = GetSubLineCount();
	int	nNewTopSubLine = m_nTopSubLine;
	CPoint subLinePos;

	CharPosToPoint(pt.y, pt.x, subLinePos);
	subLinePos.y += GetSubLineIndex(pt.y);

	if(subLinePos.y >= nNewTopSubLine + GetScreenLines())
		nNewTopSubLine = subLinePos.y - GetScreenLines() + 1;
	if(subLinePos.y < nNewTopSubLine)
		nNewTopSubLine = subLinePos.y;

	if (nNewTopSubLine < 0)
		nNewTopSubLine = 0;
	if (nNewTopSubLine >= nSubLineCount)
		nNewTopSubLine = nSubLineCount - 1;
	
	if (nNewTopSubLine != m_nTopSubLine)
	{
		ScrollToSubLine(nNewTopSubLine);
		UpdateCaret();
	}
	// we do not need horizontally scrolling, if we wrap the words
	if(m_bWordWrap)
		return;
/*
  int nActualPos = CalculateActualOffset(pt.y, pt.x);
  int nNewOffset = m_nOffset;
  
  if (nActualPos > nNewOffset + GetScreenChars())
		nNewOffset = nActualPos - GetScreenChars();
  if (nActualPos < nNewOffset)
		nNewOffset = nActualPos;

  if (nNewOffset >= GetMaxLineLength())
    nNewOffset = GetMaxLineLength() - 1;
  if (nNewOffset < 0)
    nNewOffset = 0;

  if (m_nOffset != nNewOffset)
	{
		ScrollToChar(nNewOffset);
    UpdateCaret();
	}
*/
  ///////////////////////////////////
  int nNewOffset = m_nOffset;

  CDC *pdc = GetDC();
  CFont *pOldFont = pdc->SelectObject(GetFont(FALSE, FALSE, FALSE));
  
  CPoint subline_pos;
  LPCSTR pszLine = GetLineChars(pt.y);
  int n = CharPosToPoint(pt.y, pt.x, subline_pos);
  int sz_prev = TextWidth(pdc, pszLine + n, pt.x - n);
  int sz = TextWidth(pdc, pszLine + pt.x, 1);
  
  pdc->SelectObject(pOldFont);
  ReleaseDC(pdc);
  
  CRect rect;
  GetClientRect(&rect);
  if (sz_prev + sz > m_nOffset + rect.Width())
		nNewOffset = sz_prev + sz - rect.Width();
	if (sz_prev < m_nOffset)
		nNewOffset = sz_prev;
  if (nNewOffset < 0)
    nNewOffset = 0;
  if (m_nOffset != nNewOffset)
	{
		ScrollToOffset(nNewOffset);
    UpdateCaret();
	}
  ///////////////////////////////////
}

void CTextWnd::OnKillFocus (CWnd * pNewWnd)
{
  CWnd::OnKillFocus (pNewWnd);

  m_bFocused = FALSE;
  UpdateCaret();
  if (m_ptSelStart != m_ptSelEnd)
    InvalidateLines (m_ptSelStart.y, m_ptSelEnd.y);
  if (m_bDragSelection)
		m_bDragSelection = FALSE;
}

void CTextWnd::OnSysColorChange()
{
  CWnd::OnSysColorChange();
  Invalidate();
}

void CTextWnd::GetText (const CPoint & ptStart, const CPoint & ptEnd, string & text)
{
  if (m_pTextBuffer != NULL)
    m_pTextBuffer->GetText (ptStart.y, ptStart.x, ptEnd.y, ptEnd.x, text);
  else
    text = _T ("");
}

void CTextWnd::UpdateWnd (CTextWnd * pSource, CUpdateContext * pContext,
	DWORD dwFlags, int nLineIndex /*= -1*/)
{
  if (dwFlags & UPDATE_RESET)
	{
		ResetView();
    RecalcVertScrollBar();
    RecalcHorzScrollBar();
    return;
	}

  int nLineCount = GetLineCount();
  ASSERT (nLineCount > 0);
  ASSERT (nLineIndex >= -1 && nLineIndex < nLineCount);
  if ((dwFlags & UPDATE_SINGLELINE) != 0)
  {
		ASSERT (nLineIndex != -1);
		if (nLineIndex < m_panScreenChars->GetSize())
			m_panScreenChars->SetAt(nLineIndex, -1);
    //  All text below this line should be reparsed
    if (m_pdwParseCookies != NULL)
		{
			ASSERT (m_nParseArraySize == nLineCount);
      memset (m_pdwParseCookies + nLineIndex, 0xff, sizeof (DWORD) * (m_nParseArraySize - nLineIndex));
		}
    //  This line'th actual length must be recalculated
    if (m_pnActualLineLength != NULL)
    {
			ASSERT (m_nActualLengthArraySize == nLineCount);
      m_pnActualLineLength[nLineIndex] = -1;
		
			InvalidateLineCache(nLineIndex, nLineIndex);
		
		}
    //  Repaint the lines
    InvalidateLines (nLineIndex, -1, TRUE);
	}
  else
  {
		if (nLineIndex == -1)
			nLineIndex = 0;         //  Refresh all text
		//  All text below this line should be reparsed

		m_panScreenChars->SetSize(0, 4096);
    if (m_pdwParseCookies != NULL)
    {
			if (m_nParseArraySize != nLineCount)
      {
				//  Reallocate cookies array
        DWORD *pdwNewArray = new DWORD[nLineCount];
        if (nLineIndex > 0)
           memcpy (pdwNewArray, m_pdwParseCookies, sizeof (DWORD) * nLineIndex);
        delete m_pdwParseCookies;
        m_nParseArraySize = nLineCount;
        m_pdwParseCookies = pdwNewArray;
      }
      memset (m_pdwParseCookies + nLineIndex, 0xff, sizeof (DWORD) * (m_nParseArraySize - nLineIndex));
    }
    //  Recalculate actual length for all lines below this
    if (m_pnActualLineLength != NULL)
    {
      if (m_nActualLengthArraySize != nLineCount)
      {
				//  Reallocate actual length array
        int *pnNewArray = new int[nLineCount];
        if (nLineIndex > 0)
					memcpy (pnNewArray, m_pnActualLineLength, sizeof (int) * nLineIndex);
        delete m_pnActualLineLength;
        m_nActualLengthArraySize = nLineCount;
        m_pnActualLineLength = pnNewArray;
      }
      memset (m_pnActualLineLength + nLineIndex, 0xff, sizeof (DWORD) * (m_nActualLengthArraySize - nLineIndex));
    }
	
		InvalidateLineCache(nLineIndex, -1);
	
      //  Repaint the lines
    InvalidateLines (nLineIndex, -1, TRUE);
	}

  //  All those points must be recalculated and validated
  if (pContext != NULL)
	{
		pContext->RecalcPoint (m_ptCursorPos);
    pContext->RecalcPoint (m_ptSelStart);
    pContext->RecalcPoint (m_ptSelEnd);
    pContext->RecalcPoint (m_ptAnchor);
    ASSERT_VALIDTEXTPOS (m_ptCursorPos);
    ASSERT_VALIDTEXTPOS (m_ptSelStart);
    ASSERT_VALIDTEXTPOS (m_ptSelEnd);
    ASSERT_VALIDTEXTPOS (m_ptAnchor);
    CPoint ptTopLine (0, m_nTopLine);
    pContext->RecalcPoint (ptTopLine);
    ASSERT_VALIDTEXTPOS (ptTopLine);
    m_nTopLine = ptTopLine.y;
    UpdateCaret();
	}

  //  Recalculate vertical scrollbar, if needed
  if ((dwFlags & UPDATE_VERTRANGE) != 0)
  {
		if (!m_bVertScrollBarLocked)
			RecalcVertScrollBar();
	}

  //  Recalculate horizontal scrollbar, if needed
  if ((dwFlags & UPDATE_HORZRANGE) != 0)
  {
		m_nMaxLineLength = -1;
    if (!m_bHorzScrollBarLocked)
			RecalcHorzScrollBar();
	}
}

int CTextWnd::OnCreate (LPCREATESTRUCT lpCreateStruct)
{
  if (CWnd::OnCreate (lpCreateStruct) == -1)
    return -1;

  return 0;
}

void CTextWnd::SetAnchor (const CPoint & ptNewAnchor)
{
  ASSERT_VALIDTEXTPOS (ptNewAnchor);
  m_ptAnchor = ptNewAnchor;
}

BOOL CTextWnd::PreTranslateMessage(MSG * pMsg)
{
  return CWnd::PreTranslateMessage (pMsg);
}

CPoint CTextWnd::GetCursorPos()
{
  return m_ptCursorPos;
}

void CTextWnd::SetCursorPos(const CPoint & ptCursorPos)
{
  ASSERT_VALIDTEXTPOS (ptCursorPos);
  m_ptCursorPos = ptCursorPos;
  m_nIdealCharPos = CalculateActualOffset (m_ptCursorPos.y, m_ptCursorPos.x);
  UpdateCaret();
}

void CTextWnd::GetFont (LOGFONT & lf)
{
  lf = m_lfBaseFont;
}

void CTextWnd::SetFont (const LOGFONT & lf)
{
  m_lfBaseFont = lf;
  m_nScreenLines = -1;
  m_nScreenChars = -1;
  m_nCharWidth = -1;
  m_nMaxCharWidth = -1;
  m_nLineHeight = -1;
  if (m_pCacheBitmap != NULL)
	{
		m_pCacheBitmap->DeleteObject();
    delete m_pCacheBitmap;
    m_pCacheBitmap = NULL;
	}
  for (int I = 0; I < 8; I++)
  {
		if (m_apFonts[I] != NULL)
    {
			m_apFonts[I]->DeleteObject();
      delete m_apFonts[I];
      m_apFonts[I] = NULL;
		}
	}
  if (::IsWindow(m_hWnd))
  {
		RecalcVertScrollBar();
    RecalcHorzScrollBar();
    UpdateCaret();
    Invalidate();
	}
}

void CTextWnd::ShowCursor()
{
  m_bCursorHidden = FALSE;
  UpdateCaret();
}

void CTextWnd::HideCursor()
{
  m_bCursorHidden = TRUE;
  UpdateCaret();
}

BOOL CTextWnd::GetViewTabs()
{
  return m_bViewTabs;
}

void CTextWnd::SetViewTabs (BOOL bViewTabs)
{
  if (bViewTabs != m_bViewTabs)
  {
		m_bViewTabs = bViewTabs;
    if (::IsWindow (m_hWnd))
			Invalidate();
	}
}

BOOL CTextWnd::GetEqualTabs() const
{
	return m_bEqualTabs;
}
 
void CTextWnd::SetEqualTabs(BOOL bEqualTabs)
{
  if (bEqualTabs != m_bEqualTabs)
	{
		m_bEqualTabs = bEqualTabs;
		if (::IsWindow (m_hWnd))
			Invalidate();
	}
}

BOOL CTextWnd::GetWordWrapping() const
{
	return m_bWordWrap;
}

void CTextWnd::SetScreenChars(int val)
{
	if (m_nScreenChars != val)
	{
		m_nScreenChars = val;

		if(IsWindow(m_hWnd))
			InvalidateLines(0, -1, TRUE);
	}
}

void CTextWnd::SetWordWrapping(BOOL bWordWrap)
{
	if (m_bWordWrap != bWordWrap)
	{
		m_bWordWrap = bWordWrap;

		if(IsWindow(m_hWnd))
			InvalidateLines(0, -1, TRUE);
	}
	if(IsWindow(m_hWnd))
		ShowScrollBar(SB_HORZ, !m_bWordWrap);
}

void CTextWnd::OnLButtonDown (UINT nFlags, CPoint point)
{
  CWnd::OnLButtonDown (nFlags, point);

  
  BOOL bShift = GetKeyState (VK_SHIFT) & 0x8000;
  BOOL bControl = GetKeyState (VK_CONTROL) & 0x8000;

	CPoint ptText = ClientToText (point);
	PrepareSelBounds();
	AdjustTextPoint (point);
	m_ptCursorPos = ClientToText (point);
	if (!bShift)
		m_ptAnchor = m_ptCursorPos;

	CPoint ptStart, ptEnd;
	if (bControl)
	{
		if (m_ptCursorPos.y < m_ptAnchor.y ||
    	m_ptCursorPos.y == m_ptAnchor.y && m_ptCursorPos.x < m_ptAnchor.x)
		{
			ptStart = WordToLeft (m_ptCursorPos);
			ptEnd = WordToRight (m_ptAnchor);
		}
		else
		{
			ptStart = WordToLeft (m_ptAnchor);
			ptEnd = WordToRight (m_ptCursorPos);
		}
	}
  else
  {
		ptStart = m_ptAnchor;
		ptEnd = m_ptCursorPos;
	}

	m_ptCursorPos = ptEnd;
	UpdateCaret();
	EnsureVisible (m_ptCursorPos);
	SetSelection (ptStart, ptEnd);

  m_bWordSelection = bControl;
  m_bLineSelection = FALSE;
  m_bDragSelection = TRUE;
  ASSERT_VALIDTEXTPOS (m_ptCursorPos);
	SetFocus();

	// we must set the ideal character position here!
	m_nIdealCharPos = CalculateActualOffset(m_ptCursorPos.y, m_ptCursorPos.x);

}

void CTextWnd::OnMouseMove (UINT nFlags, CPoint point)
{
  CWnd::OnMouseMove (nFlags, point);

  if (m_bDragSelection)
	{
		AdjustTextPoint (point);
		CPoint ptNewCursorPos = ClientToText (point);

		CPoint ptStart, ptEnd;
		if (m_bLineSelection)
    {
			//  Moving to normal selection mode
      ::SetCursor (::LoadCursor (NULL, MAKEINTRESOURCE (IDC_IBEAM)));
      m_bLineSelection = m_bWordSelection = FALSE;
		}

		if (m_bWordSelection)
    {
			if (ptNewCursorPos.y < m_ptAnchor.y ||
				ptNewCursorPos.y == m_ptAnchor.y && ptNewCursorPos.x < m_ptAnchor.x)
			{
		    ptStart = WordToLeft (ptNewCursorPos);
			  ptEnd = WordToRight (m_ptAnchor);
			}
			else
      {
		    ptStart = WordToLeft (m_ptAnchor);
			  ptEnd = WordToRight (ptNewCursorPos);
			}
		}
		else
		{
			ptStart = m_ptAnchor;
			ptEnd = ptNewCursorPos;
		}

    m_ptCursorPos = ptEnd;
    UpdateCaret();
    SetSelection (ptStart, ptEnd);
	}

  ASSERT_VALIDTEXTPOS (m_ptCursorPos);
}

void CTextWnd::OnLButtonUp (UINT nFlags, CPoint point)
{
  CWnd::OnLButtonUp (nFlags, point);

  if (m_bDragSelection)
	{
		AdjustTextPoint (point);
    CPoint ptNewCursorPos = ClientToText (point);

    CPoint ptStart, ptEnd;
    if (m_bLineSelection)
    {
			CPoint ptEnd;
      if (ptNewCursorPos.y < m_ptAnchor.y ||
				ptNewCursorPos.y == m_ptAnchor.y && ptNewCursorPos.x < m_ptAnchor.x)
      {
			
				CPoint	pos;
				ptEnd = m_ptAnchor;
				CharPosToPoint(ptEnd.y, ptEnd.x, pos);
				if(GetSubLineIndex(ptEnd.y) + pos.y == GetSubLineCount() - 1)
					ptEnd = SubLineEndToCharPos(ptEnd.y, pos.y);
				else
				{
					int	nLine, nSubLine;
					GetLineBySubLine(GetSubLineIndex(ptEnd.y) + pos.y + 1, nLine, nSubLine);
					ptEnd.y = nLine;
					ptEnd.x = SubLineHomeToCharPos(nLine, nSubLine);
				}
				CharPosToPoint(ptNewCursorPos.y, ptNewCursorPos.x, pos);
				ptNewCursorPos.x = SubLineHomeToCharPos(ptNewCursorPos.y, pos.y);
					m_ptCursorPos = ptNewCursorPos;
      }
      else
      {
				ptEnd = m_ptAnchor;
				CPoint	pos;
				CharPosToPoint(ptEnd.y, ptEnd.x, pos);
				ptEnd.x = SubLineHomeToCharPos(ptEnd.y, pos.y);

				m_ptCursorPos = ptNewCursorPos;
				CharPosToPoint(ptNewCursorPos.y, ptNewCursorPos.x, pos);
				if(GetSubLineIndex(ptNewCursorPos.y) + pos.y == GetSubLineCount() - 1)
					ptNewCursorPos.x = SubLineEndToCharPos(ptNewCursorPos.y, pos.y);
				else
				{
					int	nLine, nSubLine;
					GetLineBySubLine(GetSubLineIndex(ptNewCursorPos.y) + pos.y + 1, nLine, nSubLine);
					ptNewCursorPos.y = nLine;
					ptNewCursorPos.x = SubLineHomeToCharPos(nLine, nSubLine);
				}
				m_ptCursorPos = ptNewCursorPos;
      }
      EnsureVisible(m_ptCursorPos);
      UpdateCaret();
      SetSelection (ptNewCursorPos, ptEnd);
    }
    else
		{		
      if (m_bWordSelection)
      {
				if (ptNewCursorPos.y < m_ptAnchor.y ||
        ptNewCursorPos.y == m_ptAnchor.y && ptNewCursorPos.x < m_ptAnchor.x)
        {
					ptStart = WordToLeft (ptNewCursorPos);
          ptEnd = WordToRight (m_ptAnchor);
        }
        else
        {
					ptStart = WordToLeft (m_ptAnchor);
          ptEnd = WordToRight (ptNewCursorPos);
        }
      }
      else
      {
				ptStart = m_ptAnchor;
        ptEnd = m_ptCursorPos;
      }
			m_ptCursorPos = ptEnd;
      EnsureVisible(m_ptCursorPos);
      UpdateCaret();
      SetSelection (ptStart, ptEnd);
    }
	  m_bDragSelection = FALSE;
	}

  ASSERT_VALIDTEXTPOS (m_ptCursorPos);
}

void CTextWnd::OnLButtonDblClk (UINT nFlags, CPoint point)
{
  CWnd::OnLButtonDblClk (nFlags, point);

  if (!m_bDragSelection)
	{
		AdjustTextPoint (point);

    m_ptCursorPos = ClientToText (point);
    m_ptAnchor = m_ptCursorPos;

    CPoint ptStart, ptEnd;
    if (m_ptCursorPos.y < m_ptAnchor.y ||
			m_ptCursorPos.y == m_ptAnchor.y && m_ptCursorPos.x < m_ptAnchor.x)
		{
			ptStart = WordToLeft (m_ptCursorPos);
      ptEnd = WordToRight (m_ptAnchor);
		}
    else
    {
			ptStart = WordToLeft (m_ptAnchor);
      ptEnd = WordToRight (m_ptCursorPos);
    }

    m_ptCursorPos = ptEnd;
    UpdateCaret();
    EnsureVisible (m_ptCursorPos);
    SetSelection (ptStart, ptEnd);

    m_bWordSelection = TRUE;
    m_bLineSelection = FALSE;
    m_bDragSelection = TRUE;
	}
}

void ShowErr()
{
	LPVOID lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(),
    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
    (LPTSTR) &lpMsgBuf, 0, NULL);
	// Display the string.
	::MessageBox(NULL, (char*)lpMsgBuf, "GetLastError", 
		MB_OK|MB_ICONINFORMATION);
	// Free the buffer.
	LocalFree(lpMsgBuf);
}

void CTextWnd::OnRButtonDown (UINT nFlags, CPoint point)
{
	CWnd::OnRButtonDown (nFlags, point);
}

BOOL CTextWnd::PutToClipboard (LPCTSTR pszText)
{
  if (pszText == NULL || _tcslen (pszText) == 0)
    return FALSE;

  CWaitCursor wc;
  BOOL bOK = FALSE;
  if (OpenClipboard())
	{
		EmptyClipboard();

    HGLOBAL hData = GlobalAlloc (GMEM_MOVEABLE | GMEM_DDESHARE, _tcslen(pszText) + 1);
    if (hData != NULL)
    {
			LPTSTR pszData = (LPTSTR)::GlobalLock (hData);
      _tcscpy (pszData, (LPTSTR) pszText);
      GlobalUnlock (hData);
      bOK = SetClipboardData (m_bOemCharset ? CF_OEMTEXT : CF_TEXT, hData) != NULL;
		}

		/*
		int size = MultiByteToWideChar(m_bOemCharset ? CP_OEMCP : CP_ACP, 0, pszText, -1, 0, 0);
    HGLOBAL hData = GlobalAlloc (GMEM_MOVEABLE | GMEM_DDESHARE, size * 2);
    if (hData != NULL)
    {
			LPWSTR pszData = (LPWSTR)::GlobalLock (hData);
			MultiByteToWideChar(m_bOemCharset ? CP_OEMCP : CP_ACP, 0, pszText, -1, pszData, size);
      bOK = SetClipboardData (CF_UNICODETEXT, hData) != NULL;

			hData = GlobalAlloc (GMEM_MOVEABLE | GMEM_DDESHARE, size);
			strcpy((LPSTR)::GlobalLock (hData), pszText);
      bOK &= SetClipboardData (m_bOemCharset ? CF_OEMTEXT : CF_TEXT, hData) != NULL;
    }
    */
    CloseClipboard();
	}
  return bOK;
}

BOOL CTextWnd::GetFromClipboard (CString & text)
{
  BOOL bSuccess = FALSE;
  if (OpenClipboard())
  {
		/*
		if (IsClipboardFormatAvailable(CF_UNICODETEXT))
		{
			HGLOBAL hData = GetClipboardData(CF_UNICODETEXT);
			wchar_t *pszData = (wchar_t *)GlobalLock (hData);
			if (pszData != NULL)
			{
				int len = (int)wcslen(pszData);
				char *buf = (char*)malloc(len + 1);
				if (buf)
				{
					WideCharToMultiByte(m_bOemCharset ? CP_OEMCP : CP_ACP, 0, pszData, len, 
						buf, len, 0, 0);
					buf[len] = 0;
				}
				text = buf;
				free(buf);
				GlobalUnlock (hData);
				bSuccess = TRUE;
			}
		}
		else
		*/
		{
			HGLOBAL hData = GetClipboardData(m_bOemCharset ? CF_OEMTEXT : CF_TEXT);
			if (hData != NULL)
			{
				LPSTR pszData = (LPSTR)GlobalLock (hData);
				if (pszData != NULL)
				{
					text = pszData;
					GlobalUnlock (hData);
					bSuccess = TRUE;
				}
			}
		}
		
		/*
		HGLOBAL hData = 0;
		if (m_bOemCharset)
		{
			bool canoem = IsClipboardFormatAvailable(CF_OEMTEXT);
			if (canoem)
			{
				hData = GetClipboardData(CF_OEMTEXT);
				if (hData != NULL)
				{
					LPSTR pszData = (LPTSTR)GlobalLock (hData);
					if (pszData != NULL)
					{
						text = pszData;
						GlobalUnlock (hData);
						bSuccess = TRUE;
					}
				}
			}
			else
			{
				hData = GetClipboardData(CF_TEXT);
				if (hData != NULL)
				{
					LPTSTR pszData = (LPTSTR)GlobalLock (hData);
					if (pszData != NULL)
					{
						text = pszData;
						text.AnsiToOem();
						GlobalUnlock (hData);
						bSuccess = TRUE;
					}
				}
			}
		}
		else
		{
		}
		*/
		
		CloseClipboard();
	}
  return bSuccess;
}

CPoint CTextWnd::WordToRight (CPoint pt)
{
  ASSERT_VALIDTEXTPOS (pt);
  int nLength = GetLineLength (pt.y);
  LPCTSTR pszChars = GetLineChars (pt.y);
  while (pt.x < nLength)
	{
		if (is_space((unsigned char)pszChars[pt.x]))
			break;
    pt.x++;
	}
  ASSERT_VALIDTEXTPOS (pt);
  return pt;
}

CPoint CTextWnd::WordToLeft (CPoint pt)
{
  ASSERT_VALIDTEXTPOS (pt);
  LPCTSTR pszChars = GetLineChars (pt.y);
  while (pt.x > 0)
  {
		if (is_space((unsigned char)pszChars[pt.x - 1]))
			break;
		pt.x--;
	}
  ASSERT_VALIDTEXTPOS (pt);
  return pt;
}

BOOL CTextWnd::TextInClipboard()
{
  return IsClipboardFormatAvailable (CF_TEXT);
}

BOOL CTextWnd::IsSelection()
{
  return m_ptSelStart != m_ptSelEnd;
}

void CTextWnd::MoveLeft (BOOL bSelect)
{
  PrepareSelBounds();
  if (m_ptDrawSelStart != m_ptDrawSelEnd && !bSelect)
		m_ptCursorPos = m_ptDrawSelStart;
  else
	{
		if (m_ptCursorPos.x == 0)
    {
			if (m_ptCursorPos.y > 0)
      {
				m_ptCursorPos.y--;
        m_ptCursorPos.x = GetLineLength (m_ptCursorPos.y);
      }
		}
		else
    {
			m_ptCursorPos.x--;
      /*
      if (m_pTextBuffer->IsMBSTrail (m_ptCursorPos.y, m_ptCursorPos.x) &&
				// here... if its a MBSTrail, then should move one character more....
        m_ptCursorPos.x > 0)
      
      m_ptCursorPos.x--;
      */
    }
	}
  m_nIdealCharPos = CalculateActualOffset (m_ptCursorPos.y, m_ptCursorPos.x);
  EnsureVisible (m_ptCursorPos);
  UpdateCaret();
  if (!bSelect)
    m_ptAnchor = m_ptCursorPos;
  SetSelection (m_ptAnchor, m_ptCursorPos);
}

void CTextWnd::MoveRight (BOOL bSelect)
{
  PrepareSelBounds();
  if (m_ptDrawSelStart != m_ptDrawSelEnd && !bSelect)
		m_ptCursorPos = m_ptDrawSelEnd;
  else
	{
		if (m_ptCursorPos.x == GetLineLength (m_ptCursorPos.y))
    {
			if (m_ptCursorPos.y < GetLineCount() - 1)
      {
				m_ptCursorPos.y++;
        m_ptCursorPos.x = 0;
      }
		}
    else
    {
			m_ptCursorPos.x++;
      /*
      if (m_pTextBuffer->IsMBSTrail (m_ptCursorPos.y, m_ptCursorPos.x) &&
				// here... if its a MBSTrail, then should move one character more....
        m_ptCursorPos.x < GetLineLength (m_ptCursorPos.y))
				m_ptCursorPos.x++;
			*/
		}
	}
  m_nIdealCharPos = CalculateActualOffset (m_ptCursorPos.y, m_ptCursorPos.x);
  EnsureVisible (m_ptCursorPos);
  UpdateCaret();
  if (!bSelect)
    m_ptAnchor = m_ptCursorPos;
  SetSelection (m_ptAnchor, m_ptCursorPos);
}

void CTextWnd::MoveWordLeft (BOOL bSelect)
{
  PrepareSelBounds();
  if (m_ptDrawSelStart != m_ptDrawSelEnd && !bSelect)
	{
		MoveLeft (bSelect);
    return;
	}

  if (m_ptCursorPos.x == 0)
  {
		if (m_ptCursorPos.y == 0)
			return;
		m_ptCursorPos.y--;
		m_ptCursorPos.x = GetLineLength (m_ptCursorPos.y);
	}

  LPCTSTR pszChars = GetLineChars (m_ptCursorPos.y);
  int nPos = m_ptCursorPos.x;
  while (nPos > 0 && is_space((unsigned char)pszChars[nPos - 1]))
    nPos--;

  if (nPos > 0)
  {
		nPos--;
    if (!is_space((unsigned char)pszChars[nPos]))
			while (nPos > 0 && !is_space((unsigned char)pszChars[nPos - 1]))
				nPos--;
    else while (nPos > 0 && is_space((unsigned char)pszChars[nPos - 1])
			&& !is_space ((unsigned char)pszChars[nPos - 1]))
			nPos--;
	}

  m_ptCursorPos.x = nPos;
  m_nIdealCharPos = CalculateActualOffset (m_ptCursorPos.y, m_ptCursorPos.x);
  EnsureVisible (m_ptCursorPos);
  UpdateCaret();
  if (!bSelect)
    m_ptAnchor = m_ptCursorPos;
  SetSelection (m_ptAnchor, m_ptCursorPos);
}

void CTextWnd::MoveWordRight(BOOL bSelect)
{
  PrepareSelBounds();
  if (m_ptDrawSelStart != m_ptDrawSelEnd && !bSelect)
  {
		MoveRight (bSelect);
    return;
	}

  if (m_ptCursorPos.x == GetLineLength (m_ptCursorPos.y))
	{
		if (m_ptCursorPos.y == GetLineCount() - 1)
			return;
		m_ptCursorPos.y++;
    m_ptCursorPos.x = 0;
	}

  int nLength = GetLineLength (m_ptCursorPos.y);
  if (m_ptCursorPos.x == nLength)
    {
      MoveRight (bSelect);
      return;
    }

  LPCTSTR pszChars = GetLineChars (m_ptCursorPos.y);
  int nPos = m_ptCursorPos.x;
  if (!is_space((unsigned char)pszChars[nPos]))
    {
      while (nPos < nLength && !is_space((unsigned char)pszChars[nPos]))
        nPos++;
    }
  else
    {
      while (nPos < nLength && is_space((unsigned char)pszChars[nPos])
            && !is_space ((unsigned char)pszChars[nPos]))
        nPos++;
    }

  while (nPos < nLength && is_space((unsigned char)pszChars[nPos]))
    nPos++;

  m_ptCursorPos.x = nPos;
  m_nIdealCharPos = CalculateActualOffset (m_ptCursorPos.y, m_ptCursorPos.x);
  EnsureVisible (m_ptCursorPos);
  UpdateCaret();
  if (!bSelect)
    m_ptAnchor = m_ptCursorPos;
  SetSelection (m_ptAnchor, m_ptCursorPos);
}

void CTextWnd::MoveUp(BOOL bSelect)
{
  PrepareSelBounds();
  if (m_ptDrawSelStart != m_ptDrawSelEnd && !bSelect)
    m_ptCursorPos = m_ptDrawSelStart;


	CPoint	subLinePos;
	CharPosToPoint(m_ptCursorPos.y, m_ptCursorPos.x, subLinePos);

	int nSubLine = GetSubLineIndex(m_ptCursorPos.y) + subLinePos.y;

	if(nSubLine > 0)
	{
		if (m_nIdealCharPos == -1)
			m_nIdealCharPos = CalculateActualOffset (m_ptCursorPos.y, m_ptCursorPos.x);
	
		SubLineCursorPosToTextPos(CPoint(m_nIdealCharPos, nSubLine - 1), m_ptCursorPos);
		if (m_ptCursorPos.x > GetLineLength (m_ptCursorPos.y))
			m_ptCursorPos.x = GetLineLength (m_ptCursorPos.y);
	}
  EnsureVisible(m_ptCursorPos);
  UpdateCaret();
  if (!bSelect)
    m_ptAnchor = m_ptCursorPos;
  SetSelection(m_ptAnchor, m_ptCursorPos);
}

void CTextWnd::MoveDown(BOOL bSelect)
{
  PrepareSelBounds();
  if (m_ptDrawSelStart != m_ptDrawSelEnd && !bSelect)
    m_ptCursorPos = m_ptDrawSelEnd;

	CPoint subLinePos;
	CharPosToPoint(m_ptCursorPos.y, m_ptCursorPos.x, subLinePos);

	int	nSubLine = GetSubLineIndex(m_ptCursorPos.y) + subLinePos.y;

	if (nSubLine < GetSubLineCount() - 1)
	{
		if (m_nIdealCharPos == -1)
			m_nIdealCharPos = CalculateActualOffset(m_ptCursorPos.y, m_ptCursorPos.x);
	
		SubLineCursorPosToTextPos(CPoint(m_nIdealCharPos, nSubLine + 1), m_ptCursorPos);
		if (m_ptCursorPos.x > GetLineLength (m_ptCursorPos.y))
			m_ptCursorPos.x = GetLineLength (m_ptCursorPos.y);
	}
  EnsureVisible (m_ptCursorPos);
  UpdateCaret();
  if (!bSelect)
    m_ptAnchor = m_ptCursorPos;
  SetSelection(m_ptAnchor, m_ptCursorPos);
}

void CTextWnd::MoveHome(BOOL bSelect)
{
  int nLength = GetLineLength (m_ptCursorPos.y);
  LPCTSTR pszChars = GetLineChars (m_ptCursorPos.y);

	CPoint pos;
	CharPosToPoint(m_ptCursorPos.y, m_ptCursorPos.x, pos);
	int nHomePos = SubLineHomeToCharPos(m_ptCursorPos.y, pos.y);
	int nOriginalHomePos = nHomePos;
  while (nHomePos < nLength && is_space((unsigned char)pszChars[nHomePos]))
    nHomePos++;
  if (nHomePos == nLength || m_ptCursorPos.x == nHomePos)
	
		m_ptCursorPos.x = nOriginalHomePos;
  else
    m_ptCursorPos.x = nHomePos;
  m_nIdealCharPos = CalculateActualOffset (m_ptCursorPos.y, m_ptCursorPos.x);
  EnsureVisible (m_ptCursorPos);
  UpdateCaret();
  if (!bSelect)
    m_ptAnchor = m_ptCursorPos;
  SetSelection (m_ptAnchor, m_ptCursorPos);
}

void CTextWnd::MoveEnd (BOOL bSelect)
{
	CPoint	pos;
	CharPosToPoint(m_ptCursorPos.y, m_ptCursorPos.x, pos);
	m_ptCursorPos.x = SubLineEndToCharPos(m_ptCursorPos.y, pos.y);
  m_nIdealCharPos = CalculateActualOffset(m_ptCursorPos.y, m_ptCursorPos.x);
  EnsureVisible(m_ptCursorPos);
  UpdateCaret();
  if (!bSelect)
    m_ptAnchor = m_ptCursorPos;
  SetSelection(m_ptAnchor, m_ptCursorPos);
}

void CTextWnd::MovePgUp (BOOL bSelect)
{
	// scrolling windows
	int nNewTopSubLine = m_nTopSubLine - GetScreenLines() + 1;
	if (nNewTopSubLine < 0)
		nNewTopSubLine = 0;
	if (m_nTopSubLine != nNewTopSubLine)
		ScrollToSubLine(nNewTopSubLine);

	// setting cursor
	CPoint	subLinePos;
	CharPosToPoint(m_ptCursorPos.y, m_ptCursorPos.x, subLinePos);

	int	nSubLine = GetSubLineIndex(m_ptCursorPos.y) + subLinePos.y - GetScreenLines() + 1;

	if(nSubLine < 0)
		nSubLine = 0;

	SubLineCursorPosToTextPos(
		CPoint(m_nIdealCharPos, nSubLine), m_ptCursorPos);

  m_nIdealCharPos = CalculateActualOffset (m_ptCursorPos.y, m_ptCursorPos.x);
  EnsureVisible (m_ptCursorPos);    //todo: no vertical scroll

  UpdateCaret();
  if (!bSelect)
    m_ptAnchor = m_ptCursorPos;
  SetSelection (m_ptAnchor, m_ptCursorPos);
}

void CTextWnd::MovePgDn (BOOL bSelect)
{

	// scrolling windows
	int nNewTopSubLine = m_nTopSubLine + GetScreenLines() - 1;
	int nSubLineCount = GetSubLineCount();

	if (nNewTopSubLine > nSubLineCount)
		nNewTopSubLine = nSubLineCount - 1;
	if (m_nTopSubLine != nNewTopSubLine)
		ScrollToSubLine(nNewTopSubLine);

	// setting cursor
	CPoint	subLinePos;
	CharPosToPoint(m_ptCursorPos.y, m_ptCursorPos.x, subLinePos);

	int nSubLine = GetSubLineIndex(m_ptCursorPos.y) + subLinePos.y + GetScreenLines() - 1;

	if(nSubLine > nSubLineCount - 1)
		nSubLine = nSubLineCount - 1;

	SubLineCursorPosToTextPos(
		CPoint(m_nIdealCharPos, nSubLine), m_ptCursorPos);

  m_nIdealCharPos = CalculateActualOffset (m_ptCursorPos.y, m_ptCursorPos.x);
  EnsureVisible (m_ptCursorPos);    //todo: no vertical scroll

  UpdateCaret();
  if (!bSelect)
    m_ptAnchor = m_ptCursorPos;
  SetSelection (m_ptAnchor, m_ptCursorPos);
}

void CTextWnd::MoveCtrlHome (BOOL bSelect)
{
  m_ptCursorPos.x = 0;
  m_ptCursorPos.y = 0;
  m_nIdealCharPos = CalculateActualOffset (m_ptCursorPos.y, m_ptCursorPos.x);
  EnsureVisible (m_ptCursorPos);
  UpdateCaret();
  if (!bSelect)
    m_ptAnchor = m_ptCursorPos;
  SetSelection (m_ptAnchor, m_ptCursorPos);
}

void CTextWnd::MoveCtrlEnd (BOOL bSelect)
{
  m_ptCursorPos.y = GetLineCount() - 1;
  m_ptCursorPos.x = GetLineLength (m_ptCursorPos.y);
  m_nIdealCharPos = CalculateActualOffset (m_ptCursorPos.y, m_ptCursorPos.x);
  EnsureVisible (m_ptCursorPos);
  UpdateCaret();
  if (!bSelect)
    m_ptAnchor = m_ptCursorPos;
  SetSelection (m_ptAnchor, m_ptCursorPos);
}

void CTextWnd::Copy()
{
  if (m_ptSelStart == m_ptSelEnd)
    return;

  PrepareSelBounds();
  string text;
  GetText (m_ptDrawSelStart, m_ptDrawSelEnd, text);
  PutToClipboard (text.c_str());
}

void CTextWnd::SelectAll()
{
  int nLineCount = GetLineCount();
  m_ptCursorPos.x = GetLineLength (nLineCount - 1);
  m_ptCursorPos.y = nLineCount - 1;
  SetSelection (CPoint (0, 0), m_ptCursorPos);
  UpdateCaret();
}

void CTextWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (m_pTextBuffer == NULL)
		return;

	BOOL isShift = GetKeyState(VK_SHIFT) & 0xF0;
	BOOL isCtrl = GetKeyState(VK_CONTROL) & 0xF0;
	if (nChar == VK_LEFT) 
		if (isCtrl)
			MoveWordLeft(isShift);
		else MoveLeft(isShift);
	if (nChar == VK_RIGHT) 
		if (isCtrl)
			MoveWordRight(isShift);
		else MoveRight(isShift);
	if (nChar == VK_UP) MoveUp(isShift);
	if (nChar == VK_DOWN) MoveDown(isShift);
	if (nChar == VK_HOME) 
		if (!isCtrl)
			MoveHome(isShift);
		else MoveCtrlHome(isShift);
	if (nChar == VK_END) 
		if (!isCtrl)
			MoveEnd(isShift);
		else
			MoveCtrlEnd(isShift);
	if (nChar == VK_NEXT) MovePgDn(isShift);
	if (nChar == VK_PRIOR) MovePgUp(isShift);
	
	if (nChar == 'A' && isCtrl)
		SelectAll();

	if ((nChar == 'C' && isCtrl) || (nChar == VK_INSERT && isCtrl)) 
		Copy();
}

BOOL CTextWnd::HighlightText(const CPoint& ptStartPos, 
	int nLength, BOOL bReverse)
{
  ASSERT_VALIDTEXTPOS (ptStartPos);
  CPoint ptEndPos = ptStartPos;
  int nCount = GetLineLength (ptEndPos.y) - ptEndPos.x;
  if (nLength <= nCount) ptEndPos.x += nLength;
  else
	{
		while (nLength > nCount)
    {
			nLength -= nCount + 1;
      nCount = GetLineLength (++ptEndPos.y);
    }
    ptEndPos.x = nLength;
	}
  ASSERT_VALIDTEXTPOS (m_ptCursorPos);  //  Probably 'nLength' is bigger than expected...

  m_ptCursorPos = bReverse ? ptStartPos : ptEndPos;
  m_ptAnchor = m_ptCursorPos;
  SetSelection (ptStartPos, ptEndPos);
  UpdateCaret();
  EnsureVisible(m_ptCursorPos);
  return TRUE;
}

void CTextWnd::ScrollPageUp()
{
	int n = GetScreenLines();
	if (m_nTopSubLine > n)
		ScrollToSubLine(m_nTopSubLine - n, FALSE, TRUE);
	else ScrollToSubLine(0, FALSE, TRUE);
}

void CTextWnd::ScrollUp()
{
	/*
  if (m_nTopLine > 0)
    ScrollToLine (m_nTopLine - 1);
	*/
  if (m_nTopSubLine > 0)
		ScrollToSubLine(m_nTopSubLine - 1, FALSE, TRUE);
}

void CTextWnd::ScrollDown()
{
  /*
	if (m_nTopLine < GetLineCount() - 1)
    ScrollToLine(m_nTopLine + 1);
	*/
	if (m_nTopSubLine < GetSubLineCount() - 1)
		ScrollToSubLine(m_nTopSubLine + 1, FALSE, TRUE);
}

void CTextWnd::ScrollPageDown()
{
	int n = GetScreenLines();
	if (m_nTopSubLine + n < GetSubLineCount() - 1)
		ScrollToSubLine(m_nTopSubLine + n, FALSE, TRUE);
}

void CTextWnd::ScrollLeft()
{
	if (m_nOffset > 0)
	{
		ScrollToOffset(m_nOffset - 1);
		UpdateCaret();
	}
}

void CTextWnd::ScrollRight()
{
	if (m_nOffset < GetMaxLineLength() - 1)
	{
		ScrollToOffset(m_nOffset + 1);
		UpdateCaret();
	}
}

CTextParser *CTextWnd::SetParser(CTextParser *pParser)
{
	CTextParser *pOldParser = m_pParser;
	m_pParser = pParser;
	if (pParser)
		pParser->m_pTextWnd = this;
	return pOldParser;
}

void CTextWnd::SetColorMark(bool cm)
{
	if (m_bColorMark != cm)
	{
		m_bColorMark = cm;
		if (::IsWindow(m_hWnd))
		{
			Invalidate();
			UpdateWindow();
		}
	}
}

BOOL CTextWnd::GetUnderline(int nColorIndex) 
{ 
	if (!m_bColorMark && nColorIndex >= COLOR_EDIT_MARK0 && nColorIndex <= COLOR_EDIT_MARK15)
		return ((nColorIndex - COLOR_EDIT_MARK0) & 4) != 0;
	return FALSE;
}

BOOL CTextWnd::GetItalic(int nColorIndex) 
{ 
	if (!m_bColorMark && nColorIndex >= COLOR_EDIT_MARK0 && nColorIndex <= COLOR_EDIT_MARK15)
		return ((nColorIndex - COLOR_EDIT_MARK0) & 2) != 0;
	return FALSE;
}

BOOL CTextWnd::GetBold(int nColorIndex) 
{ 
	if (!m_bColorMark && nColorIndex >= COLOR_EDIT_MARK0 && nColorIndex <= COLOR_EDIT_MARK15)
		return ((nColorIndex - COLOR_EDIT_MARK0) & 1) != 0;
	return FALSE;
}

BOOL CTextWnd::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// -> HE
	int nPageLines = GetScreenLines();
	int nSubLineCount = GetSubLineCount();

	int nNewTopSubLine = m_nTopSubLine - zDelta / 40;

	if (nNewTopSubLine < 0)
		nNewTopSubLine = 0;
	if (nNewTopSubLine >= nSubLineCount)
		nNewTopSubLine = nSubLineCount - 1;

	ScrollToSubLine(nNewTopSubLine, TRUE);
	// <- HE
/* Old
  int nLineCount = GetLineCount ();

  int nNewTopLine = m_nTopLine - zDelta / 40;

  if (nNewTopLine < 0)
    nNewTopLine = 0;
  if (nNewTopLine >= nLineCount)
    nNewTopLine = nLineCount - 1;

  if (m_nTopLine != nNewTopLine)
    {
      int nScrollLines = m_nTopLine - nNewTopLine;
      m_nTopLine = nNewTopLine;
      ScrollWindow (0, nScrollLines * GetLineHeight ());
      UpdateWindow ();
    }

  RecalcVertScrollBar (TRUE);
*/

	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}
