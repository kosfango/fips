// bbsmain.h : header file
//
#include "SAPrefsSubDlg.h"

/////////////////////////////////////////////////////////////////////////////
// bbsmain dialog

class bbsmain : public CSAPrefsSubDlg
{
// Construction
public:
	bbsmain(CWnd* pParent = NULL);   // standard constructor
	void	FillLevelDropDown(void);
	void	FillLanguageDropDown(void);
	void	SelectLevel(const char *str);
	void	SelectLanguage(const char *str);
	LPCSTR  GetName(void);
	
// Dialog Data
	//{{AFX_DATA(bbsmain)
	enum { IDD = IDD_BBS_MAIN };
	CButton	m_check_mini;
	CEdit	m_edit_sound;
	CComboBox	m_dd_language;
	CComboBox	m_dd_defgroup;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(bbsmain)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(bbsmain)
	virtual BOOL OnInitDialog();
	afx_msg void OnHelp();
	afx_msg void OnSoundChoice();
	afx_msg void OnTestSound();
	virtual void OnOK();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
