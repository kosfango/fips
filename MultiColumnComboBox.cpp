/************************************
  REVISION LOG ENTRY
  Revision By: Mihai Filimon
  Revised on 9/16/98 2:20:27 PM
  Comments: MultiColumnComboBox.cpp : implementation file
 ************************************/

#include "stdafx.h"
#include "MultiColumnComboBox.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CFont font_fields;

#define OBM_COMBO 32738
#define defaultRGBBkGnd RGB(192,192,192)
#define wndClassName _T("MultiColumnComboBox")
#define IDLISTCTRL	0x3E8
#define IDEDIT		0x3E9
#define defaultEditStyle WS_CHILD | WS_VISIBLE | ES_NOHIDESEL | ES_READONLY// | 0x200 | ES_AUTOHSCROLL
#define defaultListCtrlStyle WS_CHILD | WS_BORDER | WS_CLIPSIBLINGS | WS_OVERLAPPED | LVS_REPORT | LVS_SINGLESEL // | LVS_SHOWSELALWAYS | LVS_NOCOLUMNHEADER
#define defaultSizeDY 25 //21
#define defaultSizeDX 14
#define defaultDropDownKey VK_F4
#define defaultSelChange _T("MCCBN_SELCHANGE")

/////////////////////////////////////////////////////////////////////////////
// CMultiColumnComboBox

CBrush CMultiColumnComboBox::m_brBkGnd(defaultRGBBkGnd);
//CFont CMultiColumnComboBox::m_font;

//static const LOGFONT logFontPages =
//{
//*LONG lfHeight*/8,
//*LONG lfWidth*/0,
//*LONG lfEscapement*/0,
//*LONG lfOrientation*/0,
//*LONG lfWeight*/FW_BOLD,//FW_NORMAL,
//*BYTE lfItalic*/FALSE,
//*BYTE lfUnderline*/FALSE,
//*BYTE lfStrikeOut*/FALSE,
//*BYTE lfCharSet*/ANSI_CHARSET,
//*BYTE lfOutPrecision*/0,
//*BYTE lfClipPrecision*/0,
//*BYTE lfQuality*/DEFAULT_QUALITY,
//*BYTE lfPitchAndFamily*/DEFAULT_PITCH,
//*CHAR lfFaceName[LF_FACESIZE]*/_T("MS Sans Serif")
//};

// default constuctor
CMultiColumnComboBox::CMultiColumnComboBox(int nColumnKey)
{
	RegClassMultiColumnComboBox();
	m_pListCtrl = NULL;
	m_pEdit = NULL;
	m_bCaptured = FALSE;
	m_nColumnKey = nColumnKey;
	SetRateWidth(0.0);
	SetMultipleHeight();
	m_bFirstShow = TRUE; 
}

// virtual destructor
CMultiColumnComboBox::~CMultiColumnComboBox()
{
}

BEGIN_MESSAGE_MAP(CMultiColumnComboBox, CWnd)
	//{{AFX_MSG_MAP(CMultiColumnComboBox)
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
//	ON_WM_RBUTTONDOWN()
	ON_WM_WINDOWPOSCHANGED()
//	ON_WM_PAINT()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SETFOCUS()
	ON_WM_CREATE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMultiColumnComboBox message handlers
UINT CMultiColumnComboBox::m_nSelChange = NULL;

// Register this window class
BOOL CMultiColumnComboBox::RegClassMultiColumnComboBox()
{
	WNDCLASS wndClass;
		wndClass.style = CS_DBLCLKS;
		wndClass.lpfnWndProc = ::DefWindowProc;
		wndClass.cbClsExtra = NULL;
		wndClass.cbWndExtra = NULL;
		wndClass.hInstance = AfxGetInstanceHandle();
		wndClass.hIcon = NULL;
		wndClass.hCursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
		wndClass.hbrBackground = (HBRUSH)m_brBkGnd;
		wndClass.lpszMenuName = NULL;
		wndClass.lpszClassName = wndClassName;
	BOOL bResult = AfxRegisterClass(&wndClass);
	if (bResult)
	{
		if (!m_nSelChange)
			m_nSelChange = RegisterWindowMessage(defaultSelChange);
/*		if (!m_font.GetSafeHandle())
		{
			//At the first call set the new font
			m_font.CreateFontIndirect(&logFontPages);
		}*/
	}
	return bResult;
}

CMultiColumnComboBox* CMultiColumnComboBox::m_pActiveMCBox = NULL;
CMultiColumnComboBox::CWindowProcs CMultiColumnComboBox::m_wndProcs;

// ListControl window procedure
LRESULT CALLBACK CMultiColumnComboBox::ListCtrlWindowProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	CMultiColumnComboBox* pOwner=CMultiColumnComboBox::m_pActiveMCBox;
	if (pOwner)
		pOwner->ForwardMessage(nMsg, wParam, lParam);
	WNDPROC wndProc = CMultiColumnComboBox::m_wndProcs.GetOldListCtrlProcedure(hWnd);
	ASSERT (wndProc);
	return CallWindowProc( wndProc, hWnd, nMsg, wParam, lParam );
}

// Parent window procedure.
LRESULT CALLBACK CMultiColumnComboBox::ParentWindowProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	CMultiColumnComboBox* pOwner=CMultiColumnComboBox::m_pActiveMCBox;
	if (pOwner)
	{
		switch (nMsg)
		{
			case WM_COMMAND:
			case WM_CAPTURECHANGED:
			case WM_SYSCOMMAND:
			case WM_LBUTTONDOWN:
			case WM_NCLBUTTONDOWN:
			{
				BOOL bDropped = pOwner->IsDropedDown();
				pOwner->DropDown(FALSE);
				if (nMsg == WM_COMMAND)
					if (LOWORD(wParam) == IDOK)
					{
						pOwner->SelectCurrentItem();
						return FALSE;
					}
					else if (LOWORD(wParam) == IDCANCEL && bDropped)
						return FALSE;
				break;
			}
		}
	}
	WNDPROC wndProc = CMultiColumnComboBox::m_wndProcs.GetOldParentProcedure(hWnd);
	ASSERT (wndProc);
	return CallWindowProc( wndProc, hWnd, nMsg, wParam, lParam );
}

// Edit window procedure
LRESULT CALLBACK CMultiColumnComboBox::EditWindowProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	CMultiColumnComboBox* pOwner=CMultiColumnComboBox::m_pActiveMCBox;
//	if (pOwner) 
//	{
		switch (nMsg)
		{
			case WM_SETFOCUS:
				{
					CMultiColumnComboBox::m_pActiveMCBox = (CMultiColumnComboBox*)CWnd::FromHandle(::GetParent(hWnd));
					break;
				}
			case WM_KILLFOCUS:
				{
					if (pOwner)
					{
						pOwner->DropDown(FALSE);
						CMultiColumnComboBox::m_pActiveMCBox = NULL;
					}
					break;
				}
			case WM_KEYDOWN:
			case WM_SYSKEYDOWN:
				{
					if (pOwner)
						switch ((int) wParam)
						{
							case VK_UP:
							case VK_DOWN:
								{
									if (GetAsyncKeyState(VK_MENU) >= 0)
									{
										pOwner->SetCurrentItem(pOwner->GetCurrentItem() + ((int) wParam == VK_UP ? -1 : +1));
										pOwner->SelectCurrentItem();
										break;
									}
								}
							case defaultDropDownKey:
								{
									pOwner->DropDown(!pOwner->IsDropedDown());
									pOwner->CaptureListCtrl();
									break;
								}
						}
					break;
				}
			case WM_RBUTTONUP:
				{
					DWORD st=ListView_GetExtendedListViewStyle(pOwner->m_pListCtrl->m_hWnd);
					st ^= LVS_EX_CHECKBOXES;
					ListView_SetExtendedListViewStyle(pOwner->m_pListCtrl->m_hWnd, st);
					if (!pOwner->IsDropedDown())	pOwner->DropDown();
					return TRUE;
				}
		}
//	}
	WNDPROC wndProc = CMultiColumnComboBox::m_wndProcs.GetOldParentProcedure(hWnd);
	ASSERT (wndProc);
	return CallWindowProc( wndProc, hWnd, nMsg, wParam, lParam );
}

// Init the control
BOOL CMultiColumnComboBox::OnInit()
{
	ASSERT (/*m_pEdit == NULL &&*/ m_pListCtrl == NULL);//Do not call twice
//	ASSERT (m_font.GetSafeHandle());
//	SetFont(&m_font);
//	ModifyStyle(WS_OVERLAPPED ,0);// WS_TABSTOP);
//	m_pEdit		= new CEdit();
	m_pListCtrl = new CListCtrl();
//	if (m_pEdit->Create(defaultEditStyle, CRect(0,0,0,0), this, IDEDIT ))
//	{
//		ModifyStyleEx(0, WS_EX_STATICEDGE);
//		m_pEdit->SetFont(&m_font);
		if (m_pListCtrl->Create(defaultListCtrlStyle , CRect(0,0,0,0), GetDesktopWindow(), IDLISTCTRL))
		{
			//Set the reference to this object in user data dword
			::SetWindowLong(m_pListCtrl->m_hWnd, GWL_USERDATA, (long)this);
//			::SetWindowLong(m_pListCtrl->m_hWnd, GWL_STYLE, GetWindowLong(m_pListCtrl->m_hWnd, GWL_STYLE) | WS_CLIPSIBLINGS | WS_OVERLAPPED);
			m_pListCtrl->ModifyStyleEx(0, WS_EX_TOOLWINDOW | WS_EX_LEFTSCROLLBAR);
			m_pListCtrl->SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
			m_pListCtrl->SetWindowPos(&CWnd::wndTopMost,0,0,0,0, SWP_NOSIZE | SWP_NOMOVE);
//			ListView_SetExtendedListViewStyle(m_pListCtrl->m_hWnd, LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
			Resize();
//			m_wndProcs.AddEdit(GetEdit(), EditWindowProc);
			m_wndProcs.AddListCtrl(GetListCtrl(), ListCtrlWindowProc);
			m_wndProcs.AddParent(GetParent(), ParentWindowProc);
			return TRUE;
		}
//	}
	return FALSE;
}

// Call to subclass window
void CMultiColumnComboBox::PreSubclassWindow() 
{
	CWnd::PreSubclassWindow();

	// If this is dynamical created then do not call OnInit
	if (AfxGetThreadState()->m_pWndInit == 0)
		OnInit();
}

// Remove all dependent datas.
void CMultiColumnComboBox::OnDestroy() 
{
	CWnd::OnDestroy();

//	m_wndProcs.RemoveEdit(GetEdit());
	m_wndProcs.RemoveListCtrl(GetListCtrl());
	m_wndProcs.RemoveParent(GetParent());
	
	if (CListCtrl* pList = GetListCtrl())	delete pList;
//	if (CEdit* pEdit = GetEdit())			delete pEdit;

	m_pListCtrl = NULL;
//	m_pEdit = NULL;
}

// Draw down button
void CMultiColumnComboBox::DrawButton(CDC * pDC, CRect r, BOOL bDown)
{
	CPen penWhite(PS_SOLID,1,RGB(255,255,255));
	CPen penBlack(PS_SOLID,1,RGB(0,0,0));
	pDC->FrameRect(r,&CBrush(RGB(128,128,128)));
	if (!bDown)
	{
		pDC->SelectObject(&penWhite);
		 pDC->MoveTo(r.left, r.bottom - 2);
		 pDC->LineTo(r.left, r.top);
		 pDC->LineTo(r.right - 1, r.top);
	}
	CBitmap bitmapOEM;
	if (bitmapOEM.LoadOEMBitmap(OBM_COMBO))
	{
		CDC dcMem;
		if (dcMem.CreateCompatibleDC(pDC))
		{
			BITMAP b; bitmapOEM.GetBitmap(&b);
			int leftC = (r.Width() - b.bmWidth) / 2;
			int topC = (r.Height() - b.bmHeight) / 2;
			if (bDown)
			{
				leftC++;
				topC++;
			}
			CBitmap* pOldBitmap = dcMem.SelectObject(&bitmapOEM);
			pDC->BitBlt(r.left + leftC, r.top + topC, r.Width(), r.Height(), &dcMem, 0,0, SRCCOPY);
			pOldBitmap = dcMem.SelectObject(pOldBitmap);
		}
	}
}

/*/ On Draw function
void CMultiColumnComboBox::OnPaint() 
{
	CPaintDC dc(this);
	GetClientRect(m_rectBtn);
	m_rectBtn.right=m_rectBtn.left+GetSystemMetrics(SM_CXHSCROLL);
	DrawButton(&dc, m_rectBtn);
}*/

// Call to remove the edit and list controls
void CMultiColumnComboBox::Resize()
{
	ASSERT (GetListCtrl());
	CRect r;
	GetWindowRect(r);
	SetWindowPos(0, 0, 0, r.Width(), defaultSizeDY, SWP_NOMOVE | SWP_NOZORDER);
// Set the height and width of edit control
	GetClientRect(r);
/*	r.InflateRect(-1,-2); r.bottom++;
	r.left += GetSystemMetrics(SM_CXHSCROLL)+3;*/
	r.right-=3;
	r.left += GetSystemMetrics(SM_CXHSCROLL)+3;
	r.bottom = r.top+GetSystemMetrics(SM_CYHSCROLL);
//	GetEdit()->MoveWindow(&r);
// Set the height and width of list control
	GetWindowRect(r);
	int dy = r.Height();
	r.top = r.bottom;
	r.left++;r.right--;
	r.bottom += m_nMultipleHeight * dy;
	r.right += int(m_dWidthList * r.Width());
	GetListCtrl()->MoveWindow(&r);
}

void CMultiColumnComboBox::DropDown(BOOL bDown)
{
	if (IsWindowVisible())
	{
		if (IsDropedDown() != bDown)
		{
			Resize();
			GetListCtrl()->ShowWindow(bDown ? SW_SHOW : SW_HIDE);
//			if (bDown)
//				GetEdit()->SetFocus();
//			else
			if (!bDown)
				ReleaseCapture();
		}
	}
}

// If the list control is dropped downd, return TRUE
BOOL CMultiColumnComboBox::IsDropedDown()
{
	return GetListCtrl()->IsWindowVisible();
}

void CMultiColumnComboBox::OnWindowPosChanged(WINDOWPOS FAR* lpwndpos) 
{
	CWnd::OnWindowPosChanged(lpwndpos);
	Resize();
}

/*void CMultiColumnComboBox::OnRButtonDown(UINT nFlags, CPoint point) 
{
	AfxMessageBox("OK");
	CWnd::OnRButtonDown(nFlags, point);
}*/

void CMultiColumnComboBox::OnLButtonDown(UINT nFlags, CPoint point) 
{
//	if (m_rectBtn.PtInRect(point))
//	{
//		SetButton();
		DropDown(!IsDropedDown());
//	}
	
	CWnd::OnLButtonDown(nFlags, point);
}

void CMultiColumnComboBox::OnLButtonUp(UINT nFlags, CPoint point) 
{
	ReleaseButton();
	CaptureListCtrl();
	CWnd::OnLButtonUp(nFlags, point);
}

void CMultiColumnComboBox::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (m_bCaptured)
	{
		CPoint p; ::GetCursorPos(&p);
		ScreenToClient(&p);
		if (!m_rectBtn.PtInRect(p))
			ReleaseButton();
	}
	
	CWnd::OnMouseMove(nFlags, point);
}

// Call to release the capture and image of button. After SetButton()
void CMultiColumnComboBox::ReleaseButton()
{
	if (m_bCaptured)
	{
		ReleaseCapture();
		CDC* pDC = GetDC();
			DrawButton(pDC, m_rectBtn);
		ReleaseDC(pDC);
		m_bCaptured = FALSE;
		GetListCtrl()->SetCapture();
	}
}

void CMultiColumnComboBox::SetButton()
{
	if (!m_bCaptured)
	{
		SetCapture();
		CDC* pDC = GetDC();
			DrawButton(pDC, m_rectBtn, TRUE);
		ReleaseDC(pDC);
		m_bCaptured = TRUE;
	}
}

// This function is called by ListCtrlWindowProc
void CMultiColumnComboBox::ForwardMessage(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	ASSERT (GetListCtrl());
	switch (nMsg)
	{
		case WM_MOUSEMOVE:
		case WM_NCMOUSEMOVE:
		{
			::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
			GetListCtrl()->SetCapture();
			break;
		}
		case WM_LBUTTONDOWN:
		{
//			CPoint point(LOWORD(lParam), HIWORD(lParam));
//			CRect rectClient; GetListCtrl()->GetClientRect(rectClient);
			CRect rectWindow; GetListCtrl()->GetWindowRect(rectWindow);
			CPoint pointScreen(LOWORD(lParam), HIWORD(lParam));//point);
			GetListCtrl()->ClientToScreen(&pointScreen);
			pointScreen.x &= 0xFFFF; pointScreen.y &= 0xFFFF;
			LPARAM lPoint = MAKELPARAM(pointScreen.x, pointScreen.y);
			UINT ht = GetListCtrl()->SendMessage(WM_NCHITTEST,0,lPoint);
			if (ht == HTCLIENT)
			{
				int nIndex = GetListCtrl()->HitTest(CPoint(LOWORD(lParam), HIWORD(lParam)));
				if (GetCurrentItem() != nIndex)
					SetCurrentItem(nIndex);
			}
			else if (rectWindow.PtInRect(pointScreen))// && nMsg == WM_LBUTTONDOWN)
			{
				ReleaseCapture();
				GetListCtrl()->SendMessage(WM_NCLBUTTONDOWN, ht, lPoint);
				break;
			}
//			if (nMsg == WM_LBUTTONDOWN)
//			{
				DropDown(FALSE);
				if (rectWindow.PtInRect(pointScreen))
					SelectCurrentItem();
//			}
			break;
		}
	}
}

// When control have focus then edit will take the focus
void CMultiColumnComboBox::OnSetFocus(CWnd* pOldWnd) 
{
	CWnd::OnSetFocus(pOldWnd);
	
//	GetEdit()->SetFocus();

}

// When something is happen in edit control, notify the list control
BOOL CMultiColumnComboBox::OnCommand(WPARAM wParam, LPARAM lParam) 
{
/*	if (LOWORD(wParam) == IDEDIT)
		if (HIWORD(wParam) == EN_CHANGE)
		{
			ASSERT( GetEdit() && GetEdit()->GetDlgCtrlID() == IDEDIT);
			CString text,t ;
			GetEdit()->GetWindowText(t);
			Search(t);
		}*/
	
	return CWnd::OnCommand(wParam, lParam);
}

// Look for the lpszFindItem
void CMultiColumnComboBox::Search(LPCTSTR lpszFindItem)
{
	if (CListCtrl* pListCtrl = GetListCtrl())
	{
		LV_FINDINFO fInfo;
		 fInfo.flags = LVFI_PARTIAL | LVFI_STRING;
		 fInfo.psz = lpszFindItem;
		int nItem = pListCtrl->FindItem(&fInfo);
		SetCurrentItem(nItem);
		pListCtrl->EnsureVisible(nItem, FALSE); 
	}
}

// Select the current item of list. Called if user click the mouse, or press ENTER
void CMultiColumnComboBox::SelectCurrentItem()
{
	int nIndex = GetCurrentItem();
	CWnd* pParent = GetParent();
	/*GetEdit()->*/SetWindowText(GetListCtrl()->GetItemText(nIndex, m_nColumnKey));
	//Notify the parent that one item was changed
	if (nIndex >= 0 && pParent)
		pParent->SendMessage(m_nSelChange, (WPARAM)GetDlgCtrlID(), (LPARAM)m_hWnd);
}

// Get current item from list control
int CMultiColumnComboBox::GetCurrentItem()
{
	return GetListCtrl()->GetNextItem(-1, LVNI_SELECTED);
}

// Set current item from list control to nIndex
void CMultiColumnComboBox::SetCurrentItem(int nIndex)
{
	GetListCtrl()->SetItemState(nIndex, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
}

// Call OnInit if control is created dynamically
int CMultiColumnComboBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	OnInit();
	
	return 0;
}

// This function will changes the width of inside listcontrol
double CMultiColumnComboBox::SetRateWidth(double dWidthList)
{
	double dResult = m_dWidthList;
	m_dWidthList = fabs(dWidthList);
	return dResult;
}

int CMultiColumnComboBox::SetMultipleHeight(int nMHeight)
{
	int nResult = m_nMultipleHeight;
	m_nMultipleHeight = abs(nMHeight);
	return nResult;
}

LRESULT CMultiColumnComboBox::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
/*	if (GetEdit())
		switch ( message )
		{
			case WM_SETTEXT:
			case WM_GETTEXT:
			{
				GetEdit()->SendMessage( message, wParam, lParam );
				break;
			}
		}*/
	
	return CWnd::WindowProc(message, wParam, lParam);
}

// resize combo if needs
void CMultiColumnComboBox::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	
	Resize();
	
}

// Capture listcontrol to know when the drop will be up
void CMultiColumnComboBox::CaptureListCtrl()
{
	if (IsDropedDown())
		GetListCtrl()->SetCapture();
}
