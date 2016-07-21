// writmail.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// writmail dialog

#include "textbuffer.h"
#include "editwnd.h"

class writmail : public CDialog
{
// Construction
public:
	writmail(CWnd* pParent = NULL);   // standard constructor
	virtual ~writmail();
	void ResizeDialog 				(void);
	int	 AdjustUp					(int id,int top); 
	void HandleInGateMacros			(void); 
	BOOL HandleRightButton			(MSG *pMsg); 
	void AdjustDlgWidth				(void);
	void ResizeElement				(int id,int newwidth,BOOL client);
	void InsertAttributes			(LPCSTR attr);
	void OnChangeHeader				(void);
	void OnChangeFooter				(void);
	void ApplyEditOptions			(void);
	void TextInsert					(BOOL bOem);
	afx_msg void OnOemTextInsert	(void);
	afx_msg void OnAnsiTextInsert	(void);
	afx_msg void OnBinaryInsert		(void);
	afx_msg void OnMultimediaInsert	(void);

	char		point[30];
	CToolTipCtrl tip;

// Dialog Data
	//{{AFX_DATA(writmail)
	enum { IDD = IDD_WRITMAIL640 };
	CComboBox	m_aliases;
	CButton	m_private;
	CButton	m_convert;
	CSliderCtrl	m_range;
	CComboBox	m_fidodrop;
	CButton	m_direct;
	CButton	m_recipt;
	CButton	m_killafter;
	CButton	m_attach;
	CButton	m_attached;
	CButton	m_freezemail;
	CEdit	m_area;
	CEdit	m_subject;
	CEdit	m_toaddr;
	CEdit	m_to;
	CEdit	m_tmp;
	//my experiments
	CEditWnd m_mailtext;
	//CRichEditCtrl m_mailtext;
	CComboBox	m_origin;
	CEdit	m_header;
	CEdit	m_footer;
	//}}AFX_DATA
	CTextBuffer m_buffer;
	CFidoParser m_parser;
private:
	CString Intl;
	CString Fmpt;
	CString Topt;
	CString Msgid;
	CString Pid;
	CString Reply;
	CString Exkl;
	CString Toname;
	CString Toaddr;
	CString Header;
	CString Footer;
	CString Origin;
	CString Tearline;
	CString Msgtext;
	time_t	Msgtime;
	void build_kludges		(long hnd,mailheader *pmh);
	void build_origin		(long hnd,LPCSTR addr);
	BOOL parse_kludge		(LPCSTR kludge,LPSTR addr,ULONG *id);
	void build_tearline		(void);
	BOOL set_routing		(mailheader *pmh);
	void set_to_box			(mailheader *pmh);
	void proc_macros		(LPCSTR tmpl);
	int  search_in_abook	(LPCSTR pattern);
	int  search_in_abook	(int zone,int net,int node,int point);
	void find_recipient		(LPCSTR name,LPCSTR addr);
	void handle_import		(int ind);
	void proc_text_macro	(void);
	//int  set_next_address	(LPSTR empf);
	CString& prepare_text	(CString &str);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(writmail)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(writmail)
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnAttach();
	virtual BOOL OnInitDialog();
	afx_msg void OnAttachChk();
	afx_msg void OnAddress();
	afx_msg void OnUpdateToAddr();
	afx_msg void OnInfoAdress();
	afx_msg void OnImport();
	afx_msg void On00();
	afx_msg void On11();
	afx_msg void On22();
	afx_msg void On33();
	afx_msg void On44();
	afx_msg void On55();
	afx_msg void On66();
	afx_msg void On77();
	afx_msg void On88();
	afx_msg void On99();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnKillfocusTo();
	afx_msg void OnKillfocusToFido();
	afx_msg void OnKillfocusSubject();
	afx_msg void OnUpdateTo();
	afx_msg void OnUpdateSubject();
	afx_msg void OnHelp();
	afx_msg void OnReplacedialog();
	afx_msg void OnSendDirect();
	afx_msg void OnRot13();
	afx_msg void OnSetfocusText();
	afx_msg void OnClipCopy();
	afx_msg void OnClipCut();
	afx_msg void OnClipPaste();
	afx_msg void OnToPaste();
	afx_msg void OnSubjectPaste();
	afx_msg void OnFidoPaste();
	afx_msg void OnFindinet();
	afx_msg void OnDestroy();
	afx_msg void OnUseRecipient();
	afx_msg void OnClose();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnSetfocusTo();
	afx_msg void OnSave();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	// tooltips initialisation
	void InitTooltips(void);
	void fill_aka_combo(int areahandle);
	void set_aka_combo(LPCSTR addr="");
	void set_aliases(void);
};
