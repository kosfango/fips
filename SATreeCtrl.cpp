// SATreeCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "prefs.h"
#include "SATreeCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSATreeCtrl

CSATreeCtrl::CSATreeCtrl()
{
}

CSATreeCtrl::~CSATreeCtrl()
{
}


BEGIN_MESSAGE_MAP(CSATreeCtrl, CTreeCtrl)
	//{{AFX_MSG_MAP(CSATreeCtrl)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSATreeCtrl message handlers

HTREEITEM CSATreeCtrl::GetNextItem(HTREEITEM hItem)
{
   HTREEITEM hti = NULL;
   if (ItemHasChildren(hItem))
          hti = GetChildItem(hItem);

   if (hti == NULL) 
   {
      while ((hti = GetNextSiblingItem(hItem)) == NULL) 
      {
         if ((hItem = GetParentItem(hItem)) == NULL)
            return NULL;
      }
   }

   return hti;
}
