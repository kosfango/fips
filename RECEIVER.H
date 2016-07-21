// receiver.h : header file
//

#include "SAPrefsSubDlg.h"
/////////////////////////////////////////////////////////////////////////////
// receiver dialog

class receiver : public CSAPrefsSubDlg
{
// Construction
public:
	receiver(CWnd* pParent = NULL);   // standard constructor
	LPCSTR	GetName(void);
	void	refresh_groups(LPCSTR group);
	void	populate_lists(void);
 

// Dialog Data
	//{{AFX_DATA(receiver)
	enum { IDD = IDD_RECIPIENT_LIST };
	CListBox	m_members;
	CListBox	m_groups;
	CEdit	m_edit_name;
	CEdit	m_edit_group;
	CEdit	m_edit_fido;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(receiver)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(receiver)
	virtual BOOL OnInitDialog();
	afx_msg void OnAddMember();
	afx_msg void OnAddGroup();
	afx_msg void OnDeleteGroup();
	afx_msg void OnDeleteMember();
	afx_msg void OnHelp();
	virtual void OnOK();
	afx_msg void OnLbnSelchangeGroups();
	afx_msg void OnImport();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLbnSelchangeMembers();
	afx_msg void OnChangeMember();
	afx_msg void OnChangeGroup();
};
