#include "stdafx.h"
#include "mystrlst.h"
#include "myowlist.h"

#ifndef  CColorListBoxINCLUDED
  #define CColorListBoxINCLUDED

class CColorListBox : public myowlist
{
public:
	int	itemhigh;
	int	type;

// Implementation
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMIS);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDIS);
	virtual int CompareItem(LPCOMPAREITEMSTRUCT lpCIS);
//	virtual void notify_child_key_was_pressed(int nChar);
};


class CColorListBoxSubject : public myowlist
{
public:
	  int            itemhigh;
	  int            type;

// Implementation
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMIS);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDIS);
	virtual int  CompareItem(LPCOMPAREITEMSTRUCT lpCIS);
//	virtual void notify_child_key_was_pressed(int nChar);
};

#endif
