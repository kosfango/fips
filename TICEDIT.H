// ticedit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// ticedit dialog

class ticedit : public CDialog
{
// Construction
public:
	ticedit(CWnd* pParent = NULL);   // standard constructor
	void fill_tic_box(); 

// Dialog Data
	//{{AFX_DATA(ticedit)
	enum { IDD = IDD_TIC_EDITOR };
	CListBox	m_list;
	CEdit	m_edit;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ticedit)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(ticedit)
	afx_msg void OnDelete();
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg void OnSelchangeList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
