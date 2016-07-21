// myeditfr.cpp : implementation file
//

#include "stdafx.h"
#include "sel_freq.h"
#include "myeditfr.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern sel_freq *sel_reg;

myeditfr::myeditfr(){}

myeditfr::~myeditfr(){}

BEGIN_MESSAGE_MAP(myeditfr, CEdit)
	//{{AFX_MSG_MAP(myeditfr)
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// myeditfr message handlers

void myeditfr::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	switch (nChar)
	{
		case VK_HOME:
			sel_reg->key_top();
			return;
		case VK_END:
			sel_reg->key_bottom();	
			return;
		case VK_UP:
			sel_reg->key_up();	
			return;
		case VK_DOWN:
			sel_reg->key_down();	
			return;
		case VK_PRIOR:
			sel_reg->key_pgup();	
			return;
		case VK_NEXT:
			sel_reg->key_pgdown();	
			return;
	}
	CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}
