// cfg_mult.h : header file
//
#include "SAPrefsSubDlg.h"

/////////////////////////////////////////////////////////////////////////////
// cfg_mult dialog

class cfg_mult : public CSAPrefsSubDlg
{
// Construction
public:
	cfg_mult(CWnd* pParent = NULL);   // standard constructor
	virtual ~cfg_mult();
	LPCSTR  GetName(void);
	
// Dialog Data
	//{{AFX_DATA(cfg_mult)
	enum { IDD = IDD_CFG_DESIGN };
	CListCtrl	m_used;
	CListCtrl	m_avail;
	CListCtrl	m_skin;
	CString	m_skinpath;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(cfg_mult)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	CImageList *m_pSkin;
	CImageList *m_pIcons;
	void load_skin(HINSTANCE hi);
	void load_avail(HINSTANCE hi); 
	void load_used(HINSTANCE hi); 
	void ForceRedraw(void); 
	void swap_fields(int ind1,int ind2);
		
	// Generated message map functions
	//{{AFX_MSG(cfg_mult)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnHelp();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnSkinbr();
	afx_msg void OnDown();
	afx_msg void OnUp();
	afx_msg void OnDelete();
	afx_msg void OnAdd();
	afx_msg void OnSetfocusAvail(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSetfocusUsed(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
