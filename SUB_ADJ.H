// sub_adj.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// sub_adj dialog

class sub_adj : public CDialog
{
// Construction
public:
	sub_adj(CWnd* pParent = NULL);   // standard constructor
	void ForceRedraw			();  

// Dialog Data
	//{{AFX_DATA(sub_adj)
	enum { IDD = IDD_ADJUST_SUBJ_LIST };
	CSliderCtrl	m_slider3;
	CSliderCtrl	m_slider2;
	CSliderCtrl	m_slider1;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(sub_adj)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(sub_adj)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnStnClickedStatic2();
	afx_msg void OnStnClickedStatic3();
	afx_msg void OnStnClickedStatic4();
};
