// TrayIcon.cpp : implementation file
//
#include "stdafx.h"
#include "TrayIcon.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTrayIcon
CTrayIcon::CTrayIcon(CWnd* pParent, HICON hIcon, char* txtToolTip, UINT uID)
{
	NotificationMsg=RegisterWindowMessage("FPH_MSG");
	Create(pParent,hIcon,txtToolTip,uID);
}

CTrayIcon::~CTrayIcon()
{
	iconData.uFlags =0;
	Shell_NotifyIcon(NIM_DELETE, &iconData);
	DestroyWindow();
}

BEGIN_MESSAGE_MAP(CTrayIcon, CWnd)
	//{{AFX_MSG_MAP(CTrayIcon)
	
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTrayIcon message handlers

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

BOOL CTrayIcon::Create(CWnd* pParent, HICON hIcon, char *txtToolTip,UINT uID)
{
	m_pParent=pParent;
	CWnd::CreateEx(0, AfxRegisterWndClass(0), _T(""), WS_POPUP, 0,0,10,10, NULL, 0);
	iconData.cbSize=sizeof(NOTIFYICONDATA); 
	iconData.hIcon =hIcon;
	iconData.hWnd =this->GetSafeHwnd(); 
	strcpy(iconData.szTip,txtToolTip);
	iconData.uCallbackMessage =NotificationMsg;
	iconData.uFlags =NIF_ICON|NIF_TIP|NIF_MESSAGE;
	iconData.uID=uID; 
	Shell_NotifyIcon(NIM_ADD,&iconData);	
	return TRUE;
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

LRESULT CTrayIcon::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	switch(LOWORD(lParam))
	{
		case WM_RBUTTONUP:
		{
			POINT point;
			CMenu menu;

			menu.CreatePopupMenu();
			load_context_menu("$MENU_TRAY_ICON",&menu);
			GetCursorPos(&point);
			menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,point.x, point.y,m_pParent,NULL);
			break;
		}
		case WM_LBUTTONUP:
			m_pParent->PostMessage(WM_SYSCOMMAND, SC_RESTORE);
	}
	return CWnd::WindowProc(message, wParam, lParam);
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------