// SButton.cpp : implementation file
//

#include "stdafx.h"
#include "SButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern HINSTANCE hRes;
Type_TrackMouseEvent SButton::TrackMouseEvent=NULL;
/////////////////////////////////////////////////////////////////////////////
// SButton

SButton::SButton() : m_bMouseInside(FALSE),m_hImage(NULL),m_hImageBW(NULL),
					 m_nImWidht(0),m_nImHeight(0),m_bFlat(TRUE),
					 m_nImPos(IMGPOS_LEFT),m_bToggle(FALSE),m_bPushed(FALSE),
					 m_uTypeImage(0),m_nWidthFrame(THIN_FRAME),
					 m_bShowFocus(FALSE),m_bHotText(FALSE)
{
	if( TrackMouseEvent==NULL && LoadLibrary("user32.dll")!=NULL )
		TrackMouseEvent=(Type_TrackMouseEvent)GetProcAddress( GetModuleHandle("user32.DLL"), "TrackMouseEvent" );
}

void SButton::PreSubclassWindow() 
{
	ModifyStyle(0, BS_OWNERDRAW);
	
	CButton::PreSubclassWindow();
}

SButton::~SButton()
{
	SetImages(NULL); // Delete images
}

BEGIN_MESSAGE_MAP(SButton, CButton)
	//{{AFX_MSG_MAP(SButton)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_CAPTURECHANGED()
	//}}AFX_MSG_MAP
	ON_WM_MOUSELEAVE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SButton message handlers

void SButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	CDC	*pDC = CDC::FromHandle( lpDrawItemStruct->hDC );

	RECT		&rect = lpDrawItemStruct->rcItem;
	CString		strBText;
	UINT		nDisabled=DSS_NORMAL; // Equal DSS_DISABLED if button is DISABLED
	CPoint		ptOffsetT;   // left-top point of text
	CPoint		ptOffsetI;   // left-top point of image
	BOOL		bPush=m_bPushed; // флаг означающий рисовать кнопку нажатой, или нет

	// Определим положение текста и изображения
	GetWindowText( strBText );
	if( strBText.IsEmpty() ) {
		ptOffsetI.x = (rect.right-m_nImWidht)/2;
		ptOffsetI.y = (rect.bottom-m_nImHeight)/2;
	} else {
		CSize	szTextSize;
		szTextSize = pDC->GetTextExtent( strBText );
		if( m_hImage ) {
			if( m_nImPos==IMGPOS_LEFT ) {
				ptOffsetI.x = (rect.right-szTextSize.cx-m_nImWidht-3)/2;
				ptOffsetI.y = (rect.bottom-m_nImHeight)/2;
				ptOffsetT.x = (rect.right-szTextSize.cx-m_nImWidht-3)/2+m_nImWidht+3;
				ptOffsetT.y = (rect.bottom-szTextSize.cy)/2;
			} else {
				ptOffsetI.x = (rect.right-m_nImWidht)/2;
				ptOffsetI.y = (rect.bottom-szTextSize.cy-m_nImHeight)/2+1;
				ptOffsetT.x = (rect.right-szTextSize.cx)/2;
				ptOffsetT.y = ptOffsetI.y+m_nImHeight+1;
			}
		} else {
			ptOffsetT.x = (rect.right-szTextSize.cx)/2;
			ptOffsetT.y = (rect.bottom-szTextSize.cy)/2;
		}
	}

	if( lpDrawItemStruct->itemState&ODS_DISABLED )
		nDisabled = DSS_DISABLED;
	
	if( lpDrawItemStruct->itemState&ODS_SELECTED )
		bPush = TRUE;

	if( bPush ) {
		ptOffsetT.x++; ptOffsetT.y++;
		ptOffsetI.x++; ptOffsetI.y++;
	}
	
	// Закрасим фон
	if( m_bToggle && m_bMouseInside==FALSE && bPush ) {
		// Если Toggle button и мышка вне клавиши и нажата, то заполним фон полутоновой кистью
		COLORREF	clrTextColor;
		clrTextColor = pDC->SetTextColor( GetSysColor(COLOR_3DHILIGHT) );
		pDC->FillRect( &rect, CDC::GetHalftoneBrush() );
		pDC->SetTextColor( clrTextColor );
		pDC->SetBkMode( TRANSPARENT );
	} else
		pDC->FillSolidRect( &rect, GetSysColor(COLOR_3DFACE) );
	
	// Выведем текст и изображение
	if( m_bHotText && m_bMouseInside && nDisabled==DSS_NORMAL )
		pDC->SetTextColor( RGB(0,0,255) );
	pDC->DrawState( ptOffsetT, CSize(0,0), strBText, DST_TEXT|nDisabled, FALSE, 0, (HBRUSH)NULL );

	if( m_bMouseInside==FALSE && m_hImageBW && !(m_bToggle && bPush) )
		// Если мышка вне клавиши и задано ЧБ изображение и не является нажатой Toggle button, то рисуем ЧБ изображение
		::DrawState( pDC->m_hDC, NULL, NULL, (LPARAM)m_hImageBW, 0, ptOffsetI.x, ptOffsetI.y, m_nImWidht, m_nImHeight, m_uTypeImage|nDisabled );
	else if( m_hImage )
		::DrawState( pDC->m_hDC, NULL, NULL, (LPARAM)m_hImage, 0, ptOffsetI.x, ptOffsetI.y, m_nImWidht, m_nImHeight, m_uTypeImage|nDisabled );

	// Рисуем фокус
	if( m_bShowFocus && lpDrawItemStruct->itemState&ODS_FOCUS && nDisabled==DSS_NORMAL ) {
		InflateRect(&rect, -3,-3);
		pDC->DrawFocusRect(&rect);
		InflateRect(&rect, 3,3);
	}
	
	// Рисуем 3D-рамку
	if( (m_nWidthFrame!=NO_FRAME) && (!m_bFlat || bPush || (m_bMouseInside && nDisabled==DSS_NORMAL)) ) {
		// Если есть рамка и [не плоская или нажата или (мышка внутри клавиши и не Disabled)], то нужно рисовать рамку
		if( m_nWidthFrame==THIN_FRAME ) 
		{
			if( bPush )
				pDC->Draw3dRect( &rect, GetSysColor(COLOR_3DSHADOW),GetSysColor(COLOR_3DHILIGHT) );
			else
				pDC->Draw3dRect( &rect, GetSysColor(COLOR_3DHILIGHT),GetSysColor(COLOR_3DSHADOW) );
		} else {
			if( bPush )
				pDC->DrawEdge( &rect, EDGE_SUNKEN, BF_RECT|BF_SOFT );
			else
				pDC->DrawEdge( &rect, EDGE_RAISED, BF_RECT|BF_SOFT );
		}
	}
}

void SButton::OnMouseMove(UINT nFlags, CPoint point) 
{
	if( TrackMouseEvent==NULL ) 
	{
		if( m_bMouseInside==FALSE ) {
			m_bMouseInside=TRUE;
			if( GetCapture()!=this ) 
				SetCapture();
			Invalidate( FALSE );
		} else {
			CRect rectClient;
			GetClientRect(&rectClient);
			if( !rectClient.PtInRect(point) && GetCapture()==this ) 
				ReleaseCapture();
		}
	}
	else
	{
		if( m_bMouseInside==FALSE ) {
			TRACKMOUSEEVENT TrackMEvent;
			TrackMEvent.cbSize = sizeof(TrackMEvent);
			TrackMEvent.hwndTrack = m_hWnd;
			TrackMEvent.dwFlags = TME_LEAVE;
			if( TrackMouseEvent(&TrackMEvent) ) {
				m_bMouseInside=TRUE;
				Invalidate( FALSE );
			}
		}
	}
	
	CButton::OnMouseMove(nFlags, point);
}

void SButton::OnMouseLeave()
{
	m_bMouseInside=FALSE;
	Invalidate( FALSE );
}

void SButton::OnCaptureChanged(CWnd *pWnd) 
{
	if( TrackMouseEvent==NULL ) 
	{	
		if( pWnd && pWnd->m_hWnd==m_hWnd )
			return;
		OnMouseLeave();
	}
	CButton::OnCaptureChanged(pWnd);
}

inline DWORD SButton::BWColor( DWORD b, DWORD g, DWORD r )
{
	return (11*b+59*g+30*r)/100;
}

// Преобразует цветной bitmap в чёрно-белый
HBITMAP SButton::BWBitmap( HBITMAP hColorBM, BITMAPINFOHEADER &BMInfo )
{
	int			nWidht,nHeight,nWidhtLine;
	CDC			DisplayDC;
	HBITMAP		hBWBitmap = hColorBM;
	LPBYTE		pBits=NULL;
	SBITMAPINFO	bi;
	
	nWidht  = BMInfo.biWidth;
	nHeight = BMInfo.biHeight;
	nWidhtLine = (nWidht*BMInfo.biBitCount/8+3)&~3;
	
	DisplayDC.CreateDC("DISPLAY",NULL,NULL,NULL);

	pBits = new BYTE[nWidhtLine*nHeight];

	memcpy(&bi,&BMInfo, sizeof(BITMAPINFOHEADER));
	
	if( bi.bmiHeader.biCompression==BI_BITFIELDS )
		bi.bmiHeader.biCompression = 0;

	// Извлечём биты и палитру
	GetDIBits( DisplayDC.GetSafeHdc(),hColorBM,0,nHeight,pBits, 
		LPBITMAPINFO(bi),DIB_RGB_COLORS);
	
	switch( BMInfo.biBitCount ) 
	{
	case 1:
	case 4:
	case 8:
		{
			int nMaxPalSize = 1;
			nMaxPalSize <<= BMInfo.biBitCount;
			// Пройдёмся по палитре и заменим цветные элементы на ЧБ
			for( int i=0; i<nMaxPalSize; i++ ) 
			{
				DWORD dwColor;
				if( (dwColor=bi.bmiColors[i]) ) 
				{					
					dwColor = BWColor(dwColor&0xFF, (dwColor>>8)&0xFF, (dwColor>>16)&0xFF);
					bi.bmiColors[i] = dwColor|(dwColor<<8)|(dwColor<<16);
				}
			}
		}
		break;
	case 16:
		{
			LPWORD pwBits = (LPWORD)pBits;
			nWidhtLine /= 2;
			
			for( int y=0; y<nHeight; y++ )
				for( int x=0; x<nWidht; x++ ) 
				{
					WORD wColor = pwBits[x+y*nWidhtLine];
					
					wColor = (WORD)BWColor(wColor&0x1F, (wColor>>5)&0x1F, (wColor>>10)&0x1F);
					pwBits[x+y*nWidhtLine] = wColor|(wColor<<5)|(wColor<<10);
				}
		}
		break;
	case 24:
	case 32:
		{
			int step = 3;
			if( BMInfo.biBitCount==32 ) step=4;

			for( int y=0; y<nHeight; y++ )
				for( int x=0; x<nWidht*step; x+=step ) {
					BYTE btColor;

					btColor = (BYTE)BWColor(pBits[x+y*nWidhtLine+0],pBits[x+y*nWidhtLine+1], pBits[x+y*nWidhtLine+2]);
					pBits[x+y*nWidhtLine+0] = btColor;
					pBits[x+y*nWidhtLine+1] = btColor;
					pBits[x+y*nWidhtLine+2] = btColor;
				}
		}
		break;
	default:
		delete[] pBits;
		return hBWBitmap;
	}

	hBWBitmap = CreateDIBitmap(DisplayDC.GetSafeHdc(),&BMInfo,CBM_INIT,pBits,
		LPBITMAPINFO(bi),DIB_RGB_COLORS);
	
	delete[] pBits;
	return hBWBitmap;
}

BOOL SButton::SetImages( DWORD dwResourceID, DWORD dwResourceID_BW, BOOL bHotImage, UINT uType )
{
	if( m_uTypeImage==DST_ICON ) 
	{
		if( m_hImage )   
			DestroyIcon( (HICON)m_hImage );
		if( m_hImageBW ) 
			DestroyIcon( (HICON)m_hImageBW );
	} 
	else 
	{
		if( m_hImage )   
			DeleteObject( m_hImage );
		if( m_hImageBW ) 
			DeleteObject( m_hImageBW );
	}

	m_hImage = m_hImageBW = NULL;
	if( dwResourceID==NULL ) return TRUE;

	m_uTypeImage = uType==IMAGE_ICON ? DST_ICON:DST_BITMAP;
	
	m_hImage = LoadImage( hRes, MAKEINTRESOURCE(dwResourceID), uType, 0,0, (uType==IMAGE_ICON) ? LR_DEFAULTCOLOR:LR_LOADMAP3DCOLORS );
	if( m_hImage==NULL ) return FALSE;

	CDC					DisplayDC;
	BITMAPINFOHEADER	bmiHeader;
	
	DisplayDC.CreateDC("DISPLAY",NULL,NULL,NULL);
	memset( &bmiHeader, 0, sizeof(BITMAPINFOHEADER) );
	bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmiHeader.biBitCount = 0; // Retrieve only BITMAPINFOHEADER

	if( uType==IMAGE_ICON ) 
	{
		ICONINFO iconinfo;
		GetIconInfo( (HICON)m_hImage, &iconinfo );
		GetDIBits( DisplayDC.GetSafeHdc(), iconinfo.hbmColor, 0,0, NULL, (LPBITMAPINFO)&bmiHeader, DIB_RGB_COLORS );
		if( bHotImage )
		{
			if( dwResourceID_BW )
				m_hImageBW = LoadImage( hRes, MAKEINTRESOURCE(dwResourceID_BW), uType, 0,0, LR_DEFAULTCOLOR );
			else 
			{
				iconinfo.hbmColor = BWBitmap( iconinfo.hbmColor, bmiHeader );		
				m_hImageBW = CreateIconIndirect(&iconinfo);
			}
		}
		if (iconinfo.hbmMask)
			DeleteObject(iconinfo.hbmMask);
		if (iconinfo.hbmColor)
			DeleteObject(iconinfo.hbmColor);
	}
	
	if( uType==IMAGE_BITMAP ) 
	{
		GetDIBits( DisplayDC.GetSafeHdc(),(HBITMAP)m_hImage,0,0,NULL,(LPBITMAPINFO)&bmiHeader,DIB_RGB_COLORS);

		if( bHotImage )
		{
			if( dwResourceID_BW )
				m_hImageBW = LoadImage( hRes, MAKEINTRESOURCE(dwResourceID_BW), uType, 0,0, LR_LOADMAP3DCOLORS );
			else
				m_hImageBW = BWBitmap( (HBITMAP)m_hImage, bmiHeader );
		}
	}

	if( bHotImage && m_hImageBW==NULL ) return FALSE;

	m_nImWidht  = bmiHeader.biWidth;
	m_nImHeight = bmiHeader.biHeight;

	return TRUE;
}

#define BARIH	26	// top icon height
#define BARIW	37	// top icon width
BOOL SButton::SetAnimation(DWORD dwResourceID)//,void (*pfv)(void))
{
	if (!::IsWindow(m_animation))
	{
		CRect re;
		GetClientRect(&re);// 
		re.right=re.left+BARIW;
		re.bottom=re.top+BARIH;//|WS_VISIBLE
		m_animation=::CreateWindowEx(0,ANIMATE_CLASS,NULL,
			WS_CHILD|ACS_TRANSPARENT|ACS_CENTER,
			re.left,re.top,re.Width(),re.Height(),m_hWnd,NULL,hRes,NULL); 
	}
	return (::SendMessage(m_animation,ACM_OPEN,(WPARAM)hRes,(LPARAM)dwResourceID));
}

void SButton::OnRButtonDown(UINT nFlags, CPoint point) 
{
	ClientToScreen(&point);
	GetParent()->SendMessage(WM_RBUTTONDOWN,nFlags,MAKELONG(point.x,point.y));
	CButton::OnRButtonDown(nFlags, point);
}

void SButton::Play( UINT nFrom, UINT nTo, UINT nRep )
{
	if (::IsWindow(m_animation))
	{
		::ShowWindow(m_animation,SW_SHOW);
		Animate_Play(m_animation,0,-1,-1);
	}
}

void SButton::Stop( void )
{
	if (::IsWindow(m_animation))
	{
		::ShowWindow(m_animation,SW_HIDE);
		Animate_Stop(m_animation);
	}
}

void SButton::Seek( UINT nTo )
{
	if (::IsWindow(m_animation))
		Animate_Seek(m_animation,nTo);
}

void SButton::SetFrameWidth(int nWidth)
{
	m_nWidthFrame = nWidth;
}

void SButton::ShowFocus(BOOL bShow)
{
	m_bShowFocus = bShow;
}

void SButton::SetStyleFlat(BOOL bFlat)
{
	m_bFlat = bFlat;
}

void SButton::SetStyleHotText( BOOL bHot )
{
	m_bHotText = bHot;
}

void SButton::SetImagePosition( int nImPos )
{
	m_nImPos = nImPos;
}

void SButton::SetStyleToggle( BOOL bToggle )
{
	m_bToggle = bToggle;
	m_bPushed = FALSE;
}

BOOL SButton::IsPushed()
{
	return m_bPushed;
}

void SButton::SetPushed( BOOL bPushed )
{
	if( m_bPushed!=bPushed ) {
		m_bPushed = bPushed;
		Invalidate( FALSE );
	}
}

void SButton::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if( m_bToggle )
		m_bPushed = !m_bPushed;
	CButton::OnLButtonDown(nFlags, point);
}
