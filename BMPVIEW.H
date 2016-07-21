// bmpview.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// bmpview dialog

class bmpview : public CDialog
{
// Construction
public:
	bmpview(CWnd* pParent = NULL);   // standard constructor
	void OnDraw(CDC* pDC);

// Dialog Data
	//{{AFX_DATA(bmpview)
	enum { IDD = IDD_BMP_VIEW };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(bmpview)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(bmpview)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMove(int x, int y);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
