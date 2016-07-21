//
// Dieses File enthaelt die Subclassing Info fuer die eigene Listbox
//
// mylist.cpp : implementation file
//
//

#include "stdafx.h"
#include "mylist.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

void handle_scroll(int offset);
extern CListBox *m_globedit;
extern _gconfig gc;

/////////////////////////////////////////////////////////////////////////////
// CMyList

CMyList::CMyList(){}

CMyList::~CMyList(){}

BEGIN_MESSAGE_MAP(CMyList, CListBox)
	//{{AFX_MSG_MAP(CMyList)
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyList message handlers
void CMyList::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	switch (nChar)
	{
		case VK_UP:
			if (gc.golded_compatible)	handle_scroll(-1);
			return;
		case VK_DOWN:
			if (gc.golded_compatible)	handle_scroll(1);
			return;
		case VK_LEFT:
			if (gc.golded_compatible)
			{
				nChar=VK_UP;
				break;
			}
			handle_scroll(-1);
			return;
		case VK_RIGHT:
			if (gc.golded_compatible)
			{
				nChar=VK_DOWN;
				break;
			}
			handle_scroll(1);
			return;
	}
	CListBox::OnKeyDown(nChar, nRepCnt, nFlags);
}

// ===============================================
	void handle_scroll(int offset)
// ===============================================
{
int ret,max;
    
	ret=m_globedit->GetTopIndex();
	max=m_globedit->GetCount();

	ret=ret+offset;
	if (ret<0)
	   ret=0;
	else if (ret>=max)
	   ret=max-1;

	m_globedit->SetTopIndex(ret);
}
