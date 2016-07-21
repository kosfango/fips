// AniButton.cpp : implementation file
//

#include "stdafx.h"
#include "AniButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//REMEMBER TO SET BUTTON TEXT VERTICAL POSITION AS 'BOTTOM' OR ELSE IT WILL GET HIDDEN BEHIND THE AVI!!

/////////////////////////////////////////////////////////////////////////////
// CAniButton

CAniButton::CAniButton()
{
  m_bRedrawFlag=TRUE;
}

CAniButton::~CAniButton()
{
}


BEGIN_MESSAGE_MAP(CAniButton, CButton)
	//{{AFX_MSG_MAP(CAniButton)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAniButton message handlers






void CAniButton::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CButton::OnLButtonDown(nFlags, point);
  if (m_bRedrawFlag)
      m_ctlAnimate.RedrawWindow();
}

void CAniButton::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CButton::OnLButtonUp(nFlags, point);
     if (m_bRedrawFlag)
       m_ctlAnimate.RedrawWindow();

}

void CAniButton::OnMouseMove(UINT nFlags, CPoint point) 
{
 	CButton::OnMouseMove(nFlags, point);
   if (nFlags!=0)
     if (m_bRedrawFlag)
        m_ctlAnimate.RedrawWindow();
 
 }

void CAniButton::Play()
{
  m_bRedrawFlag=FALSE;
  m_ctlAnimate.Play(0,-1,-1);

}

void CAniButton::Close()
{
  m_bRedrawFlag=FALSE;
  m_ctlAnimate.Close();

}

void CAniButton::Stop()
{
  m_ctlAnimate.Stop();
}

BOOL CAniButton::Open( LPCTSTR lpszFileName )
{
  TRACE("Open lpszfilename called\n");
  if (!::IsWindow(m_ctlAnimate))
  {
    CRect arect;
    GetClientRect(&arect);
    arect.DeflateRect(8,arect.Height()/5.0);
    arect.OffsetRect(0,-arect.Height()/5.0);
    m_ctlAnimate.Create(WS_CHILD | WS_VISIBLE | ACS_TRANSPARENT,arect,this,5000);//IDC_ANIMATE1);

  }
  m_bRedrawFlag=TRUE;
  return m_ctlAnimate.Open(lpszFileName);
}

BOOL CAniButton::Open( UINT nID )
{
  TRACE("Open id called\n");
  if (!::IsWindow(m_ctlAnimate))
  {
    CRect arect;
    GetClientRect(&arect);
    arect.DeflateRect(8,arect.Height()/5.0);
    arect.OffsetRect(0,-arect.Height()/5.0);
    m_ctlAnimate.Create(WS_CHILD | WS_VISIBLE | ACS_TRANSPARENT |ACS_CENTER ,arect,this,5000);//IDC_ANIMATE1);
  }
  //if you change the Create call to include ACS_AUTOPLAY, then do not set m_bRedrawFlag to TRUE
  m_bRedrawFlag=TRUE;





  return m_ctlAnimate.Open(nID);
}




