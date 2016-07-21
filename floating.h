// floating.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// floating dialog
#include "sbutton.h"

class floating : public CDialog
{
// Construction
public:
	floating(CWnd* pParent = NULL);   // standard constructor
	void	SavePosition		(void);
	void	RepositionAllBitmaps(void);
	BOOL	HandleRightButton	(MSG* pMsg);
	void	RefreshTips			(void);
	void	create_button		(SButton &ctl,int id,int resid);
	void	set_images			(void);
// Dialog Data
	//{{AFX_DATA(floating)
	enum { IDD = IDD_FLOATING_BAR };
	SButton m_filter;
	SButton m_autosave;
	SButton m_find;
	SButton m_newmail;
	SButton m_quote;
	SButton m_quotea;
	SButton m_udef1;
	SButton m_udef2;
	SButton m_udef3;
	SButton m_udef4;
	SButton m_udef5;
	SButton m_freq;
	SButton m_exit;
	SButton	m_abortscript;
	CToolTipCtrl tip;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(floating)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(floating)
	afx_msg void OnAsave();
	afx_msg void OnFind();
	afx_msg void OnNew();
	afx_msg void OnQuote();
	afx_msg void OnQuoteA();
	afx_msg void OnFreq();
	afx_msg void OnMacro1();
	afx_msg void OnMacro2();
	afx_msg void OnMacro3();
	afx_msg void OnMacro4();
	afx_msg void OnMacro5();
	afx_msg void OnExit();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnClose();
	afx_msg void OnFilter();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
