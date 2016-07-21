// coding.cpp : implementation file
//

#include "stdafx.h"
#include "light.h"
#include "coding.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// coding dialog


coding::coding(CWnd* pParent /*=NULL*/)
	: CDialog(coding::IDD, pParent)
{
	//{{AFX_DATA_INIT(coding)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void coding::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(coding)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(coding, CDialog)
	//{{AFX_MSG_MAP(coding)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// coding message handlers
