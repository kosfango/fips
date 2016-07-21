#if !defined(AFX_TEXTBUFFER_H__1918D4FD_7B1F_4BC5_AF71_CBAB75C51187__INCLUDED_)
#define AFX_TEXTBUFFER_H__1918D4FD_7B1F_4BC5_AF71_CBAB75C51187__INCLUDED_

#define UNDO_DESCRIP_BUF        32

//  CTextWnd::UpdateWnd() flags
enum
{
  UPDATE_HORZRANGE	= 0x0001, //  update horz scrollbar
  UPDATE_VERTRANGE	= 0x0002, //  update vert scrollbar
  UPDATE_SINGLELINE	= 0x0100, //  single line has changed
  UPDATE_FLAGSONLY	= 0x0200, //  only line-flags were changed
  UPDATE_RESET		= 0x1000  //  document was reloaded, update all!
};

const char crlf[] = "\r\n";
extern const char *crlfs[];


class CTextWnd;

#if defined(UNIX)
class CPoint
{
public:
	CPoint() {x = 0; y = 0;};
	CPoint(int xpos, int ypos) {x = xpos; y = ypos;};
	int x;
	int y;
};
//typedef wxTextWnd CTextWnd;
#endif

/////////////////////////////////////////////////////////////////////////////
// wxUpdateContext class

class CUpdateContext
{
public :
	virtual void RecalcPoint (CPoint & ptPoint) = 0;
};


/////////////////////////////////////////////////////////////////////////////
// CTextBuffer command target

enum
{
  CE_ACTION_UNKNOWN = 0,
  CE_ACTION_PASTE = 1,
  CE_ACTION_DELSEL = 2,
  CE_ACTION_CUT = 3,
  CE_ACTION_TYPING = 4,
  CE_ACTION_BACKSPACE = 5,
  CE_ACTION_INDENT = 6,
  CE_ACTION_DRAGDROP = 7,
  CE_ACTION_REPLACE = 8,
  CE_ACTION_DELETE = 9,
  CE_ACTION_AUTOINDENT = 10,
  CE_ACTION_INSFILE = 11
};

struct CTextBlock
{
	int m_nCharPos;
	int m_nColorIndex;
};


class CTextParser
{
protected:
	friend class CTextWnd;
	CTextWnd *m_pTextWnd;
public:
	CTextParser(): m_pTextWnd(0){}
	virtual void WrapLine(int nLineIndex, int nMaxLineWidth, int *anBreaks, int &nBreaks);
	virtual unsigned int ParseLine(unsigned int dwCookie, int nLineIndex, CTextBlock *pBlock, int &nActualItems) = 0;
};

class CFidoParser: public CTextParser
{
public:
	virtual unsigned int ParseLine(unsigned int dwCookie, int nLineIndex, CTextBlock *pBlock, int &nActualItems);
};

class CTextBuffer
{
private:
	bool m_bInit;
	bool m_bFidoConvert;
	bool m_bReadOnly;
	bool m_bModified;
	int m_nUndoBufSize;
	int m_nCRLFMode;
	int FindLineWithFlag (unsigned int dwFlag);

protected :
#pragma pack(push, 1)
	//  Строка буфера
	struct SLineInfo
	{
		char *m_pcLine;
		int m_nLength, m_nMax;
		unsigned int m_dwFlags;

		SLineInfo ()
		{
		  memset (this, 0, sizeof (SLineInfo));
		};
	};

	enum
	{
	  UNDO_INSERT = 0x0001,
	  UNDO_BEGINGROUP = 0x0100
	};

	struct SUndoRecord
	{
		unsigned int m_dwFlags;
		int m_nAction;
		CPoint m_ptStartPos, m_ptEndPos;  // Границы блока текста
	private :
		union
		{
			char *m_pszText;     //  Если текст длиннее 1 символа
			char m_szText[2];    //  Для текста длиною в один символ
	  };
	public :
		SUndoRecord ()
		{
		  memset (this, 0, sizeof (SUndoRecord));
		};

		void SetText (const char* pszText);
		void FreeText ();

		const char* GetText () const
		{
		  if (((unsigned int) m_pszText) >> 16 != 0)
		    return m_pszText;
		  return m_szText;
		};
	};

#pragma pack(pop)

	class CInsertContext : public CUpdateContext
	{
	public :
		CPoint m_ptStart, m_ptEnd;
		virtual void RecalcPoint (CPoint & ptPoint);
	};

	class CDeleteContext : public CUpdateContext
	{
	public :
		CPoint m_ptStart, m_ptEnd;
		virtual void RecalcPoint (CPoint & ptPoint);
	};

	//  Lines of text
	vector<SLineInfo> m_aLines;

	//  Undo
	vector<SUndoRecord> m_aUndoBuf;
	int m_nUndoPosition;
	int m_nSyncPosition;
	bool m_bUndoGroup, m_bUndoBeginGroup;

	CPoint m_ptLastChange;

	vector<CTextWnd *> m_lpViews;

	void AppendLine (int nLineIndex, const char* pszChars, int nLength = -1);
	void InsertLine (const char* pszLine, int nLength = -1, int nPosition = -1);

	bool InternalInsertText (CTextWnd * pSource, int nLine, int nPos, const char* pszText, int &nEndLine, int &nEndChar);
	bool InternalDeleteText (CTextWnd * pSource, int nStartLine, int nStartPos, int nEndLine, int nEndPos);

	void AddUndoRecord (bool bInsert, const CPoint & ptStartPos, const CPoint & ptEndPos,
		const char* pszText, int n_Action);

public :
	CTextBuffer(bool bFidoConv = true);
	virtual ~CTextBuffer();

	bool InitNew();
	void FreeAll();
	
	bool SaveToFile(const char *path);
	bool OpenFromFile(const char *path);

	virtual void SetModified(bool bModified = TRUE);
	bool IsModified() const { return m_bModified; }

	void AddView(CTextWnd * pView);
	void RemoveView(CTextWnd * pView);

	int GetUndoDescription (string & desc, int pos = 0);
  int GetRedoDescription (string& desc, int pos = 0);

	int GetLineCount();
	int GetLineLength(int nLine);
	char* GetLineChars(int nLine);
	unsigned int GetLineFlags(int nLine);
	int GetLineWithFlag(unsigned int dwFlag);
	void SetLineFlag(int nLine, unsigned int dwFlag, bool bSet, bool bRemoveFromPreviousLine = TRUE);
	void GetText(int nStartLine, int nStartChar, int nEndLine, int nEndChar, string & text, const char* pszCRLF = NULL);

	bool GetReadOnly() const;
	void SetReadOnly(bool bReadOnly = TRUE);
	int GetCRLFMode() { return m_nCRLFMode; }
	void GetCRLFMode(int mode) { m_nCRLFMode = mode; }

	bool InsertText(CTextWnd * pSource, int nLine, int nPos, const char* pszText, int &nEndLine, int &nEndChar, int n_Action);
	bool DeleteText(CTextWnd * pSource, int nStartLine, int nStartPos, int nEndLine, int nEndPos, int n_Action);

	bool CanUndo();
	bool CanRedo();
	bool Undo(CPoint & ptCursorPos);
	bool Redo(CPoint & ptCursorPos);

	void BeginUndoGroup(bool bMergeWithPrevious = FALSE);
	void FlushUndoGroup(CTextWnd * pSource);
	
	bool IsFidoConvert() { return m_bFidoConvert; }
	void SetFidoConvert(bool fc) { m_bFidoConvert = fc; }

	CPoint GetLastChangePos() const;

	void UpdateWnds(CTextWnd * pSource, CUpdateContext * pContext,
		unsigned int dwUpdateFlags, int nLineIndex = -1);

	void ClearUndo();
	bool IsOk() { return m_bInit; }
protected :
};

void InsertTextToBuffer(const char *text, CTextBuffer *buf);
void GetBufferText(CTextBuffer *buf, CString& text);
void ReplaceSelection(CTextBuffer *buf, CTextWnd *wnd, const char *newtext);
#endif
