#if !defined(textwnd_h)
#define textwnd_h

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CTextBuffer;
class CUpdateContext;

#include "TextBuffer.h"

#define QUOTE_LENGTH 15

////////////////////////////////////////////////////////////////////////////
// CTextWnd class declaration

enum
{
	COLOR_EDIT_WHITESPACE,
	COLOR_EDIT_NORMALTEXT,
	COLOR_EDIT_SELBKGND,
	COLOR_EDIT_SELTEXT,
	COLOR_EDIT_QUOTE1,
	COLOR_EDIT_QUOTE2,
	COLOR_EDIT_TEARLINE,
	COLOR_EDIT_TAGLINE,
	COLOR_EDIT_ORIGIN,
	COLOR_EDIT_KLUDGE,
	////////////////   ruib
	COLOR_EDIT_MARK0,
	COLOR_EDIT_MARK1,
	COLOR_EDIT_MARK2,
	COLOR_EDIT_MARK3,
	COLOR_EDIT_MARK4,
	COLOR_EDIT_MARK5,
	COLOR_EDIT_MARK6,
	COLOR_EDIT_MARK7,
	COLOR_EDIT_MARK8,
	COLOR_EDIT_MARK9,
	COLOR_EDIT_MARK10,
	COLOR_EDIT_MARK11,
	COLOR_EDIT_MARK12,
	COLOR_EDIT_MARK13,
	COLOR_EDIT_MARK14,
	COLOR_EDIT_MARK15,
	NUM_EDIT_COLORS
};

class CTextWnd : public CWnd
{
	DECLARE_DYNCREATE (CTextWnd)
private:
	BOOL m_bCursorHidden;

	//  Битмэп для отриосвки
	CBitmap *m_pCacheBitmap;

	//  Высота стрки и число символов в строке
	int m_nLineHeight, m_nCharWidth, m_nMaxCharWidth;
	void CalcLineCharDim();

	int m_nTabSize; // Размер табуляции
	BOOL m_bViewTabs; // Видны ли ' ' и '\t'
	BOOL m_bEqualTabs; // Равные  табуляции
	BOOL m_bColorMark;

	//  Число строк/символов полностью заполняющих клиентскую область окна
	int m_nScreenLines, m_nScreenChars;

	CArray<int, int> *m_panSubLines;//Сублайны
	CArray<int, int> *m_panScreenChars;

	int m_nMaxLineLength;//Длина самой длиной строки
	int m_nIdealCharPos;

	BOOL m_bFocused;
	CPoint m_ptAnchor;
	LOGFONT m_lfBaseFont;
	CFont *m_apFonts[4];

	//  Парсинг
	DWORD *m_pdwParseCookies;//Кукисы
	int m_nParseArraySize;//Число кукисов
	DWORD GetParseCookie (int nLineIndex);

	//  Длины строк
	int m_nActualLengthArraySize;
	int *m_pnActualLineLength;

	BOOL m_bDragSelection, m_bWordSelection, m_bLineSelection;

	CPoint m_ptDrawSelStart, m_ptDrawSelEnd;
	CPoint m_ptCursorPos, m_ptCursorLast;
	CPoint m_ptSelStart, m_ptSelEnd;
	void PrepareSelBounds();

	void ExpandChars (LPCTSTR pszChars, int nOffset, int nCount, CString& line);

	int ApproxActualOffset (int nLineIndex, int nOffset);
	void AdjustTextPoint (CPoint & point);
	void DrawLineHelperImpl (CDC * pdc, CPoint & ptOrigin, 
		const CRect& rcClip, LPCTSTR pszChars, int nOffset, int nCount);
	BOOL IsInsideSelBlock (CPoint ptTextPos);
public:
	virtual void ResetView();
	virtual int GetLineCount();
	BOOL HighlightText(const CPoint& ptStartPos, int nLength, BOOL bReverse);
	int GetIdealCharPos() { return m_nIdealCharPos; }
	void MoveCtrlHome(BOOL bSelect);
	void MoveCtrlEnd(BOOL bSelect);
	void MovePgUp(BOOL bSelect);
	void MovePgDn(BOOL bSelect);
	virtual void GetText (const CPoint & ptStart, const CPoint & ptEnd, string & text);
	CTextParser *SetParser(CTextParser *pParser);
	void SetColorMark(BOOL cm);
	BOOL IsColorMark() { return m_bColorMark; }

protected:
	CTextParser *m_pParser;
	BOOL m_bOemCharset;
	CPoint WordToRight (CPoint pt);
	CPoint WordToLeft (CPoint pt);

	CTextBuffer *m_pTextBuffer;
	BOOL m_bVertScrollBarLocked, m_bHorzScrollBarLocked;
	CPoint m_ptDraggedTextBegin, m_ptDraggedTextEnd;
	void UpdateCaret();
	void SetAnchor (const CPoint & ptNewAnchor);
	bool IsValidTextPos (const CPoint &point);
	bool IsValidTextPosX (const CPoint &point);
	bool IsValidTextPosY (const CPoint &point);

	BOOL m_bShowInactiveSelection;

	BOOL m_bWordWrap;

	CPoint ClientToText (const CPoint & point);
	CPoint TextToClient (const CPoint & point);
	void InvalidateLines (int nLine1, int nLine2, BOOL bInvalidateMargin = FALSE);
	int CalculateActualOffset (int nLineIndex, int nCharIndex);

	//  Обработчики клавы
	void MoveLeft(BOOL bSelect);
	void MoveRight(BOOL bSelect);
	void MoveWordLeft(BOOL bSelect);
	void MoveWordRight(BOOL bSelect);
	void MoveUp(BOOL bSelect);
	void MoveDown(BOOL bSelect);
	void MoveHome(BOOL bSelect);
	void MoveEnd(BOOL bSelect);


	BOOL IsSelection();
	BOOL IsInsideSelection (const CPoint & ptTextPos);

	int m_nTopLine, m_nOffset;
	int m_nTopSubLine;
	BOOL m_bSmoothScroll;

	int GetLineHeight();
	int GetSubLines(int nLineIndex);
	int CharPosToPoint(int nLineIndex, int nCharPos, CPoint &charPoint);
	int CursorPointToCharPos(int nLineIndex, const CPoint &curPoint);
	void SubLineCursorPosToTextPos(const CPoint &subLinePos, CPoint &textPos);
	int SubLineEndToCharPos(int nLineIndex, int nSubLineOffset);
	int SubLineHomeToCharPos(int nLineIndex, int nSubLineOffset);
	int GetCharWidth();
	int GetMaxCharWidth();
	int GetMaxLineLength();
	CFont *GetFont (BOOL bItalic = FALSE, BOOL bBold = FALSE);

	void RecalcVertScrollBar (BOOL bPositionOnly = FALSE);
	void RecalcHorzScrollBar (BOOL bPositionOnly = FALSE);

	//  Прокрутка
	void ScrollToOffset(int nNewOffset, BOOL bNoSmoothScroll = FALSE, BOOL bTrackScrollBar = TRUE);
	void ScrollToLine(int nNewTopLine, BOOL bNoSmoothScroll = FALSE, BOOL bTrackScrollBar = TRUE);
	void ScrollToSubLine(int nNewTopSubLine, BOOL bNoSmoothScroll = FALSE, BOOL bTrackScrollBar = TRUE);
	virtual int GetSubLineCount();
	virtual int GetSubLineIndex(int nLineIndex);
	virtual void GetLineBySubLine(int nSubLineIndex, int &nLine, int &nSubLine);

	virtual int GetLineActualLength (int nLineIndex);

	//  Буфер обмена
	virtual BOOL TextInClipboard();
	virtual BOOL PutToClipboard (LPCTSTR pszText);
	virtual BOOL GetFromClipboard (CString & text);

	virtual void GetLineColors (int nLineIndex, COLORREF & crBkgnd,
		COLORREF & crText, BOOL & bDrawWhitespace);
	virtual BOOL GetItalic (int nColorIndex);
	virtual BOOL GetBold (int nColorIndex);

	void DrawLineHelper (CDC * pdc, CPoint & ptOrigin, const CRect & rcClip, int nColorIndex,
		LPCTSTR pszChars, int nOffset, int nCount, CPoint ptTextPos);
	virtual void DrawSingleLine (CDC * pdc, const CRect & rect, int nLineIndex);

	virtual void InvalidateLineCache(int nLineIndex1, int nLineIndex2);

	virtual void DrawScreenLine(CDC *pdc, CPoint &ptOrigin, const CRect &rcClip,
		CTextBlock *pBuf, int nBlocks, int &nActualItem,
		COLORREF crText, COLORREF crBkgnd, BOOL bDrawWhitespace,
		LPCTSTR pszChars, int nOffset, int nCount, CPoint ptTextPos);
	
	COLORREF GetColor(int index);

public:
	virtual LPCTSTR GetLineChars (int nLineIndex);
	virtual DWORD GetLineFlags (int nLineIndex);
	void GoToLine(int nLine, bool bRelative);
	void GetSelection(CPoint & ptStart, CPoint & ptEnd);
	void WrapLineCached(int nLineIndex, int nMaxLineWidth, int *anBreaks, int &nBreaks);
	int GetScreenChars();
	int GetScreenChars(int nLine);
	virtual int GetLineLength (int nLineIndex);
	void Copy();

public:
	void SelectAll();
	BOOL GetViewTabs();
	void SetViewTabs (BOOL bViewTabs);
	int GetTabSize();
	void SetTabSize (int nTabSize);
	void GetFont(LOGFONT& lf);
	void SetFont(const LOGFONT& lf);

	BOOL GetWordWrapping() const;
  void SetWordWrapping(BOOL bWordWrap);
	int GetScreenLines();
	void SetOemCharset(BOOL oc) { m_bOemCharset = oc; ResetView(); }
	//BOOL IsOemCharset() { return m_bOemCharset; }

public:
	void AttachToBuffer(CTextBuffer * pBuf = NULL);
  void DetachFromBuffer();

	virtual void UpdateWnd (CTextWnd * pSource, CUpdateContext * pContext, DWORD dwFlags, int nLineIndex = -1);

	CPoint GetCursorPos();
	void SetCursorPos (const CPoint & ptCursorPos);
	void ShowCursor();
	void HideCursor();

	void EnsureVisible (CPoint pt);
	void SetScreenChars(int val);

public:
	virtual BOOL PreCreateWindow (CREATESTRUCT & cs);
	virtual BOOL PreTranslateMessage (MSG * pMsg);
protected :

public :
	CTextWnd();
	virtual ~CTextWnd();

protected :

public:
#ifdef _DEBUG
	void AssertValidTextPos (const CPoint & pt);
#endif

	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd (CDC * pDC);
	afx_msg void OnSize (UINT nType, int cx, int cy);
	afx_msg void OnVScroll (UINT nSBCode, UINT nPos, CScrollBar * pScrollBar);
	afx_msg BOOL OnSetCursor (CWnd * pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonDown (UINT nFlags, CPoint point);
	afx_msg void OnSetFocus (CWnd * pOldWnd);
	afx_msg void OnHScroll (UINT nSBCode, UINT nPos, CScrollBar * pScrollBar);
	afx_msg void OnLButtonUp (UINT nFlags, CPoint point);
	afx_msg void OnMouseMove (UINT nFlags, CPoint point);
	afx_msg void OnKillFocus (CWnd * pNewWnd);
	afx_msg void OnLButtonDblClk (UINT nFlags, CPoint point);
	afx_msg void OnEditSelectAll();
	afx_msg void OnUpdateEditSelectAll (CCmdUI * pCmdUI);
	afx_msg void OnRButtonDown (UINT nFlags, CPoint point);
	afx_msg void OnSysColorChange();
	afx_msg int OnCreate (LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnCopy(WPARAM, LPARAM) { Copy(); return 0; }

	DECLARE_MESSAGE_MAP()
public:
	void ScrollUp();
	void ScrollDown();
	void ScrollPageUp();
	void ScrollPageDown();
	void ScrollLeft();
	void ScrollRight();
	void SetSelection (const CPoint & ptStart, const CPoint & ptEnd);
	BOOL GetEqualTabs() const;
  void SetEqualTabs(BOOL bWordWrap);
};

inline COLORREF CTextWnd::GetColor(int index)
{
	extern COLORREF TextWndColors[];
	if (m_bColorMark)	return TextWndColors[index];
	else return (index >= COLOR_EDIT_MARK0 && index <= COLOR_EDIT_MARK15) ? 
		TextWndColors[COLOR_EDIT_NORMALTEXT] : TextWndColors[index];
}

#ifdef _DEBUG
#define ASSERT_VALIDTEXTPOS(pt)     AssertValidTextPos(pt);
#else
#define ASSERT_VALIDTEXTPOS(pt)
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif
