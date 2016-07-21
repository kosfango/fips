// cfg_fonts.cpp : implementation file
//

#include "stdafx.h"
#include "light.h"
#include "cfg_fonts.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// cfg_fonts

IMPLEMENT_DYNAMIC(cfg_fonts, CFontDialog)

cfg_fonts::cfg_fonts(LPLOGFONT lplfInitial, DWORD dwFlags, CDC* pdcPrinter, CWnd* pParentWnd) : 
	CFontDialog(lplfInitial, dwFlags, pdcPrinter, pParentWnd)
{
}


BEGIN_MESSAGE_MAP(cfg_fonts, CFontDialog)
	//{{AFX_MSG_MAP(cfg_fonts)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

