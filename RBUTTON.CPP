#include "stdafx.h"
#include "light.h"
#include "lightdlg.h"
#include "writmail.h"
#include "adrbook.h"
#include "nodesel.h"
#include "floating.h"

extern CLightApp	FipsApp;
extern CLightDlg	*gpMain;
extern _gconfig		gc;

static char SEP[]="SEPARATOR";

// ============================================
	BOOL CLightDlg::HandleRightButton(MSG *pMsg)
// ============================================
{
CString	str;
RECT	re;
CRect	re1,re2;
int		ind;
CMenu	mnu;

// middle infobar rect
	GetDlgItem(IDC_TEXT_FROM)->GetWindowRect(&re);
	re1.top=re.top;
	re1.left=re.left;
	m_city.GetWindowRect(&re);
	re1.right=re.right;
	re1.bottom=re.bottom;
//	top infobar rect
	m_counter.GetWindowRect(&re);
	re2.top=re.top;
	re2.left=re.left;
	m_echotag.GetWindowRect(&re);
	re2.right=re.right;
	re2.bottom=re.bottom;
	if (pMsg->hwnd==m_subjlist)
	{
		mnu.CreatePopupMenu();
		load_context_menu("$MENU_POP_SUBJLIST",&mnu);
		mnu.TrackPopupMenu(TPM_LEFTALIGN,pMsg->pt.x,pMsg->pt.y,this,NULL);
		return TRUE;
	}
	else if (pMsg->hwnd==m_mailview)
	{
		mnu.CreatePopupMenu();
		load_context_menu("$MENU_POP_MAILTEXT",&mnu);
		mnu.TrackPopupMenu(TPM_LEFTALIGN,pMsg->pt.x,pMsg->pt.y,this,NULL);
		return TRUE;
	}
	else if (re1.PtInRect(pMsg->pt))
	{
		mnu.CreatePopupMenu();
		load_context_menu("$MENU_POP_ADDRESS",&mnu);
		mnu.TrackPopupMenu(TPM_LEFTALIGN,pMsg->pt.x,pMsg->pt.y,this,NULL);
		return TRUE;
	}
	else if (re2.PtInRect(pMsg->pt))
	{
		FipsApp.OnMenuAreas();
		return TRUE;
	}
	else if (pMsg->hwnd==m_udef1 || pMsg->hwnd==m_udef2 || pMsg->hwnd==m_udef3 ||
		pMsg->hwnd==m_udef4 || pMsg->hwnd==m_udef5)
	{
		if(pMsg->hwnd==m_udef1)
			ind=1;
		else if(pMsg->hwnd==m_udef2)
			ind=2;
		else if(pMsg->hwnd==m_udef3)
			ind=3;
		else if(pMsg->hwnd==m_udef4)
			ind=4;
		else
			ind=5;
		str.Format("User%d",ind);
		strcpy(gc.ScriptName,get_cfg(CFG_SCRIPTS,str,""));
		FipsApp.OnSetupScripteditor();
		gc.ScriptName[0]=0;
		return TRUE;
	}
	else if (pMsg->hwnd==m_filter)
	{
		OnFilterSet();
		return TRUE;
	}
	else if (pMsg->hwnd==m_autosave)
	{
		FipsApp.OnAutosaveToArea();
		return TRUE;
	}
	else if (pMsg->hwnd==m_quote)
	{
		OnAnswerAsNetmail();
		return TRUE;
	}
	else if (pMsg->hwnd==m_quotea)
	{
		FipsApp.OnForward();
		return TRUE;
	}
	else if (pMsg->hwnd==m_freq)
	{
		OnFilerequest();
		return TRUE;
	}
	else if (pMsg->hwnd==m_find)
	{
		FipsApp.OnSwitchExtSearch();
		return TRUE;
	}
	return FALSE;
}

// ============================================
	BOOL floating::HandleRightButton(MSG* pMsg)
// ============================================
{
CString	 str;
int		 ind;
	
	if (pMsg->hwnd==m_udef1 || pMsg->hwnd==m_udef2 || pMsg->hwnd==m_udef3 ||
		pMsg->hwnd==m_udef4 || pMsg->hwnd==m_udef5)
	{
		if(pMsg->hwnd==m_udef1)
			ind=1;
		else if(pMsg->hwnd==m_udef2)
			ind=2;
		else if(pMsg->hwnd==m_udef3)
			ind=3;
		else if(pMsg->hwnd==m_udef4)
			ind=4;
		else
			ind=5;
		str.Format("User%d",ind);
		strcpy(gc.ScriptName,get_cfg(CFG_SCRIPTS,str,""));
		FipsApp.OnSetupScripteditor();
		gc.ScriptName[0]=0;
		return TRUE;
	}
	else if (pMsg->hwnd==m_filter)
	{
		gpMain->OnFilterSet();
		return TRUE;
	}
	else if (pMsg->hwnd==m_autosave)
	{
		FipsApp.OnAutosaveToArea();
		return TRUE;
	}
	else if (pMsg->hwnd==m_quote)
	{
		gpMain->OnAnswerAsNetmail();
		return TRUE;
	}
	else if (pMsg->hwnd==m_quotea)
	{
		FipsApp.OnForward();
		return TRUE;
	}
	else if (pMsg->hwnd==m_freq)
	{
		gpMain->OnFilerequest();
		return TRUE;
	}
	else if (pMsg->hwnd==m_find)
	{
		FipsApp.OnSwitchExtSearch();
		return TRUE;
	}
	return FALSE;
}

// ============================================
	BOOL writmail::HandleRightButton(MSG* pMsg)
// ============================================
{
CMenu mnu;

	if (pMsg->hwnd==m_mailtext.m_hWnd)
	{
		mnu.CreatePopupMenu();
		load_context_menu("$MENU_POP_EDITOR",&mnu);
		mnu.TrackPopupMenu(TPM_LEFTALIGN,pMsg->pt.x,pMsg->pt.y,this,NULL);
		return TRUE;
	}
	if (pMsg->hwnd==m_header.m_hWnd)
	{
		OnChangeHeader();
		return TRUE;
	}
	if (pMsg->hwnd==m_footer.m_hWnd)
	{
		OnChangeFooter();
		return TRUE;
	}
	if (pMsg->hwnd==m_to.m_hWnd)
	{
		mnu.CreatePopupMenu();
		load_context_menu("$MENU_POP_PASTETO",&mnu);
		mnu.TrackPopupMenu(TPM_LEFTALIGN,pMsg->pt.x,pMsg->pt.y,this,NULL);
		return TRUE;
	}
	if (pMsg->hwnd==m_toaddr.m_hWnd)
	{
		mnu.CreatePopupMenu();
		load_context_menu("$MENU_POP_PASTETOADDR",&mnu);
		mnu.TrackPopupMenu(TPM_LEFTALIGN,pMsg->pt.x,pMsg->pt.y,this,NULL);
		return TRUE;
	}
	if (pMsg->hwnd==m_subject.m_hWnd)
	{
		mnu.CreatePopupMenu();
		load_context_menu("$MENU_POP_PASTESUBJ",&mnu);
		mnu.TrackPopupMenu(TPM_LEFTALIGN,pMsg->pt.x,pMsg->pt.y,this,NULL);
		return TRUE;
	}
	return FALSE;
}

// ============================================
	BOOL adrbook::HandleRightButton(MSG* pMsg)
// ============================================
{
CMenu mnu;

	if (pMsg->hwnd==::GetDlgItem(m_hWnd,IDC_LIST1))
	{
		mnu.CreatePopupMenu();
		load_context_menu("$MENU_POP_ADDRBOOK",&mnu);
		mnu.TrackPopupMenu(TPM_LEFTALIGN,pMsg->pt.x,pMsg->pt.y,this,NULL);
		return TRUE;
	}
	return FALSE;
}

// ============================================
	BOOL nodesel::HandleRightButton(MSG* pMsg)
// ============================================
{
CMenu mnu;

	if (pMsg->hwnd==m_list)
	{
		mnu.CreatePopupMenu();
		load_context_menu("$MENU_POP_NODESEL",&mnu);
		mnu.TrackPopupMenu(TPM_LEFTALIGN,pMsg->pt.x,pMsg->pt.y,this,NULL);
		return TRUE;
	}
	return FALSE;
}
