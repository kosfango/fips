#if !defined(AFX_CFG_FONTS_H__63FA3350_9736_411C_9076_E4E35E65A3E6__INCLUDED_)
#define AFX_CFG_FONTS_H__63FA3350_9736_411C_9076_E4E35E65A3E6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// cfg_fonts.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// cfg_fonts dialog

class cfg_fonts : public CFontDialog
{
	DECLARE_DYNAMIC(cfg_fonts)

public:
	cfg_fonts(LPLOGFONT lplfInitial = NULL,
		DWORD dwFlags = CF_EFFECTS | CF_SCREENFONTS,
		CDC* pdcPrinter = NULL,
		CWnd* pParentWnd = NULL);
#ifndef _AFX_NO_RICHEDIT_SUPPORT
	cfg_fonts(const CHARFORMAT& charformat,
		DWORD dwFlags = CF_SCREENFONTS,
		CDC* pdcPrinter = NULL,
		CWnd* pParentWnd = NULL);
#endif

protected:
	//{{AFX_MSG(cfg_fonts)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CFG_FONTS_H__63FA3350_9736_411C_9076_E4E35E65A3E6__INCLUDED_)
