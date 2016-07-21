// cha_tmpl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// cha_tmpl dialog

class cha_tmpl : public CDialog
{
// Construction
public:
	cha_tmpl(int t,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(cha_tmpl)
	enum { IDD = IDD_CHANGE_TEMPLATE };
	CListBox	m_list;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(cha_tmpl)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
private:

	int	type;	// type of template

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(cha_tmpl)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnDblclkList();
	afx_msg void OnHelp();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
