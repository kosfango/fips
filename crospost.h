// crospost.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// crospost dialog

class crospost : public CDialog
{
// Construction
public:
	crospost(CWnd* pParent = NULL);   // standard constructor
	void BuildCrossPostHeader(long areaind); 
	void BuildCrossPostTail(long areaind);
private:
	CString CrossPostHeader;
	CString CrossPostTail;
	int		handles[1000];
	int		entry[1000];
	int		hcount;

// Dialog Data
	//{{AFX_DATA(crospost)
	enum { IDD = IDD_CROSSPOST };
	CListBox	m_list;
	int			m_crosstyle;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(crospost)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(crospost)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnHelp();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
