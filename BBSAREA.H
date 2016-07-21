// bbsarea.h : header file
//
#include "SAPrefsSubDlg.h"

/////////////////////////////////////////////////////////////////////////////
// bbsarea dialog

class bbsarea : public CSAPrefsSubDlg
{
// Construction
public:
	bbsarea(CWnd* pParent = NULL);   // standard constructor
	int		MakeString(CString &buf);
	void	DispString(CString line);
	LPCSTR  GetName(void);

// Dialog Data
	//{{AFX_DATA(bbsarea)
	enum { IDD = IDD_BBS_AREA };
	CListBox	m_list;
	CEdit	m_edit_pathforfiles;
	CEdit	m_edit_groups;
	CEdit	m_edit_filesbbs;
	CEdit	m_edit_description;
	CComboBox	m_dd_level;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(bbsarea)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(bbsarea)
	virtual BOOL OnInitDialog();
	afx_msg void OnAdd();
	afx_msg void OnGetPath();
	afx_msg void OnGetFilelist();
	afx_msg void OnChange();
	afx_msg void OnHelp();
	virtual void OnOK();
	afx_msg void OnSelchangeList();
	afx_msg void OnDelete();
	afx_msg void OnImportTic();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
