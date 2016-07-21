// cfg_icon.h : header file
//
#include "SAPrefsSubDlg.h"
#include "afxwin.h"

/////////////////////////////////////////////////////////////////////////////
// cfg_icon dialog

class cfg_icon : public CSAPrefsSubDlg
{
// Construction
public:
	cfg_icon(CWnd* pParent = NULL);   // standard constructor
	LPCSTR  GetName(void);

// Dialog Data
	//{{AFX_DATA(cfg_icon)
	enum { IDD = IDD_CFG_MMEDIA };
	CListBox	m_list;
	BOOL	m_play_sounds;
	BOOL	m_play_bmps;
	BOOL	m_notfound;
	CString	m_wavpath;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(cfg_icon)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void fill_mm_list(BOOL mode);

	// Generated message map functions
	//{{AFX_MSG(cfg_icon)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnDefault();
	afx_msg void OnTestsound();
	afx_msg void OnStopsound();
	afx_msg void OnChange();
	afx_msg void OnWavebr();
	afx_msg void OnSelchangeList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedShowbmp();
	CListBox m_mmlist;
	afx_msg void OnBnClickedShowwav();
	CStatic m_bmp;
	afx_msg void OnLbnSelchangeMmlist();
	afx_msg void OnBnClickedStop();
	afx_msg void OnBnClickedPlay();
};
