// pkt_view.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// pkt_view dialog

class pkt_view : public CDialog
{
// Construction
public:
	pkt_view(CWnd* pParent = NULL);   // standard constructor
	void DoTheWholeDisplayStuff			(void); 
	int  Repacking						(void);

// Dialog Data
	//{{AFX_DATA(pkt_view)
	enum { IDD = IDD_OUTBOUND_MAIL_VIEW };
	CEdit	m_edit;
	CListBox	m_list_pkt;
	CListBox	m_list_contents;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(pkt_view)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(pkt_view)
	virtual BOOL OnInitDialog();
	afx_msg void OnDelete();
	afx_msg void OnHelp();
	virtual void OnCancel();
	afx_msg void OnSelchangeListPkt();
	afx_msg void OnSelchangeListContents();
	afx_msg void OnDeleteThisPacket();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint(); 
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
