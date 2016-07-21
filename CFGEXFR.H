// cfgexfr.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// cfgexfr dialog

class cfgexfr : public CDialog
{
// Construction
public:
	cfgexfr(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(cfgexfr)
	enum { IDD = IDD_CFG_EXTFREQ };
	CButton	m_check_16bit;
	CEdit	m_edit_path;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(cfgexfr)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(cfgexfr)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
