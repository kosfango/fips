#if !defined(AFX_SBUTTON_H__68A9D681_35A3_11D5_9650_E1BABEF7180B__INCLUDED_)
#define AFX_SBUTTON_H__68A9D681_35A3_11D5_9650_E1BABEF7180B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SButton.h : header file
//

typedef BOOL (FAR WINAPI *Type_TrackMouseEvent)(LPTRACKMOUSEEVENT);

#define ON_WM_MOUSELEAVE() \
	{ WM_MOUSELEAVE, 0, 0, 0, AfxSig_vv, \
		(AFX_PMSG)(AFX_PMSGW)(void (AFX_MSG_CALL CWnd::*)())&OnMouseLeave },

/////////////////////////////////////////////////////////////////////////////
// SButton window

class SButton : public CButton
{
	struct SBITMAPINFO {
		BITMAPINFOHEADER    bmiHeader;
		DWORD				bmiColors[256];			
		
		operator BITMAPINFO() { return *this; }
		operator LPBITMAPINFO() { return (LPBITMAPINFO)this; }
		operator LPBITMAPINFOHEADER() { return (LPBITMAPINFOHEADER)this; }	
	};
	
	HBITMAP BWBitmap( HBITMAP hColorBM, BITMAPINFOHEADER &BMInfo );

public:
	SButton();
	/*virtual*/ ~SButton();

// Attributes
	BOOL IsPushed(); // Проверка нажата ли клавиша
	void SetPushed( BOOL bPushed=true ); // Сделать клавишу нажатой/отжатой
	void SetImagePosition( int nImPos ); // SBTN_LEFT or SBTN_TOP
	
	void SetStyleFlat( BOOL bFlat=true );
	void SetStyleHotText( BOOL bHot=true );
	void SetStyleToggle( BOOL bToggle=true ); // Клавиша с 2 состояниями( нажата/отжата )

	BOOL SetImages(DWORD dwID,DWORD dwID_BW=NULL,BOOL bHotImage=true,UINT uType=IMAGE_ICON/*or IMAGE_BITMAP*/ );
	BOOL SetAnimation(DWORD dwID);//,void (*)(void)=NULL);
	void Play( UINT nFrom, UINT nTo, UINT nRep );
	void Seek( UINT nTo );
	void Stop( void );
	void SetFrameWidth(int nWidth);
	void ShowFocus(BOOL bShow=true);

	inline DWORD BWColor( DWORD b, DWORD g, DWORD r );
		
	enum { NO_FRAME, THIN_FRAME, THICK_FRAME, IMGPOS_LEFT, IMGPOS_TOP };
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SButton)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(SButton)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	//}}AFX_MSG
	afx_msg void OnMouseLeave();

	DECLARE_MESSAGE_MAP()

private:

	BOOL	m_bPushed;      // Для Toggle button означает нажата ли клавиша
	int     m_nWidthFrame;	// Толщина 3D-рамки
// Styles
	BOOL	m_bFlat;
	BOOL	m_bToggle;
	BOOL	m_bShowFocus;
	BOOL	m_bHotText;
// Image data
	HANDLE	m_hImage;		// Цветное изображение
	HANDLE	m_hImageBW;		// ЧБ изображение
	UINT	m_uTypeImage;   // Тип изображения(Icon or Bitmap)
	int		m_nImPos;		// Позиция изображения: над текстом или слева
	int		m_nImHeight;
	int		m_nImWidht;
//	void	(*m_pfv)(void);
//	CAnimateCtrl	m_animation;	// анимация
	HWND	m_animation;

	BOOL	m_bMouseInside; // мышка внутри клавиши

	static Type_TrackMouseEvent TrackMouseEvent;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SBUTTON_H__68A9D681_35A3_11D5_9650_E1BABEF7180B__INCLUDED_)
