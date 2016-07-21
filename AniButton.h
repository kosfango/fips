#if !defined(AFX_ANIBUTTON_H__6B8886C4_D6D9_11D3_8118_0050DA6BF740__INCLUDED_)
#define AFX_ANIBUTTON_H__6B8886C4_D6D9_11D3_8118_0050DA6BF740__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AniButton.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAniButton window

class CAniButton : public CButton
{
// Construction
public:
	CAniButton();

  CAnimateCtrl m_ctlAnimate;

  UINT m_nId;

  BOOL Open( LPCTSTR lpszFileName );

  BOOL Open( UINT nID );

// Attributes
public:

private:
  	BOOL m_bRedrawFlag;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAniButton)
	public:
	//}}AFX_VIRTUAL

// Implementation
public:
	void Stop();
	void Close();
	void Play();
	virtual ~CAniButton();

	// Generated message map functions
protected:
	//{{AFX_MSG(CAniButton)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ANIBUTTON_H__6B8886C4_D6D9_11D3_8118_0050DA6BF740__INCLUDED_)
