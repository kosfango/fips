// impwhat.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// impwhat dialog

class impwhat : public CDialog
{
// Construction
public:
	impwhat(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(impwhat)
	enum { IDD = IDD_CHOOSE_IMPORT };
//	CEdit	m_edit;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(impwhat)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(impwhat)
	virtual BOOL OnInitDialog();
	afx_msg void OnOemText();
	afx_msg void OnAnsiText();
	afx_msg void OnUuencode();
	virtual void OnCancel();
	afx_msg void OnMultim();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
