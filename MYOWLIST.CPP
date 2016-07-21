// myowlist.cpp : implementation file
//

#include "stdafx.h"
#include "lightdlg.h"
#include "myowlist.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// myowlist

extern CLightDlg *gpMain;
extern _gconfig gc;
extern void handle_scroll(int offset);

BEGIN_MESSAGE_MAP(myowlist, CListBox)
	//{{AFX_MSG_MAP(myowlist)
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// myowlist message handlers

myowlist::myowlist(){}
myowlist::~myowlist(){}

void myowlist::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
RECT	re;

	switch (nChar)
	{
		case VK_PRIOR:
			if (GetDlgCtrlID()==IDC_SUBJLIST && 
				!((GetKeyState(VK_CONTROL) | GetKeyState(VK_SHIFT)) & 0x80000000))
			{
				gpMain->m_mailview.GetWindowRect(&re);
				handle_scroll(-(re.bottom-re.top)/gpMain->m_mailview.GetItemHeight(0));
				return;
			}
			break;
		case VK_NEXT:
			if (GetDlgCtrlID()==IDC_SUBJLIST &&
				!((GetKeyState(VK_CONTROL) | GetKeyState(VK_SHIFT)) & 0x80000000))
			{
				gpMain->m_mailview.GetWindowRect(&re);
				handle_scroll((re.bottom-re.top)/gpMain->m_mailview.GetItemHeight(0));
				return;
			}
			break;
		case VK_HOME:
			if (GetDlgCtrlID()==IDC_SUBJLIST &&
				!((GetKeyState(VK_CONTROL) | GetKeyState(VK_SHIFT)) & 0x80000000))
			{
				gpMain->m_mailview.SetTopIndex(0);
				return;
			}
			break;
		case VK_END:
			if (GetDlgCtrlID()==IDC_SUBJLIST &&
				!((GetKeyState(VK_CONTROL) | GetKeyState(VK_SHIFT)) & 0x80000000))
			{
				gpMain->m_mailview.SetTopIndex(gpMain->m_mailview.GetCount()-1);
				return;
			}
			break;
		case VK_UP:
			if (gc.golded_compatible)
			{
				handle_scroll(-1);
				return;
			}
			else
			{
				nChar=VK_LEFT;
				break;
			}
		case VK_LEFT:
			if (gc.golded_compatible)
			{
			   nChar=VK_UP;
			   break;
			}
			else
			{
			   handle_scroll(-1);
			   return;
			}
		case VK_DOWN:
			if (gc.golded_compatible)
			{
				handle_scroll(1);
				return;
			}
			else
			{
				if (GetDlgCtrlID()==IDC_SUBJLIST &&
					(gpMain->m_subjlist.GetCurSel()+1)>=gpMain->m_subjlist.GetCount())
				{
					if (GetKeyState(VK_CONTROL) & 0x80000000 ||
						!get_cfg(CFG_COMMON,"LastAreaSelection",1))
							return;
					
					gpMain->OnArea();
					return;
				}
				nChar=VK_RIGHT;
				break;
			}
		case VK_RIGHT:
			if (gc.golded_compatible)
			{
				if (GetDlgCtrlID()==IDC_SUBJLIST &&
					(gpMain->m_subjlist.GetCurSel()+1)>=gpMain->m_subjlist.GetCount())	// last mail
				{
					if (GetKeyState(VK_CONTROL) & 0x80000000 ||	!get_cfg(CFG_COMMON,"LastAreaSelection",1))
						return;
					
					gpMain->OnArea();
					return;
				}
				nChar=VK_DOWN;
				break;
			}
			else
			{
				handle_scroll(1);
				return;
			}
	}
	CListBox::OnKeyDown(nChar, nRepCnt, nFlags);
}
