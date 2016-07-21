// selevbos.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// selevbos dialog

class selevbos : public CDialog
{
// Construction
public:
	selevbos(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(selevbos)
	enum { IDD = IDD_SELECT_EV_BOSS };
	CListBox	m_list;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(selevbos)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(selevbos)
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
