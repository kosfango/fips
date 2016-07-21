// inouthis.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// inouthis dialog

class inouthis : public CDialog
{
// Construction
public:
	inouthis(CWnd* pParent = NULL);   // standard constructor
	void	display_this_id		(int id);

// Dialog Data
	//{{AFX_DATA(inouthis)
	enum { IDD = IDD_INOUT_HIST };
	CListBox	m_list;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(inouthis)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(inouthis)
	virtual BOOL OnInitDialog();
	afx_msg void OnIncome();
	afx_msg void OnOutgoing();
	virtual void OnOK();
	afx_msg	void OnSize  (UINT nType, int cx, int cy); 
	afx_msg void OnPaint (); 
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
