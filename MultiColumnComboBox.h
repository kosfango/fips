/************************************
  REVISION LOG ENTRY
  Revision By: Mihai Filimon
  Revised on 9/16/98 2:20:55 PM
  Comments: MultiColumnComboBox.h : header file
 ************************************/

#if !defined(AFX_MULTICOLUMNCOMBOBOX_H__1385ADA1_4D4F_11D2_8693_0040055C08D9__INCLUDED_)
#define AFX_MULTICOLUMNCOMBOBOX_H__1385ADA1_4D4F_11D2_8693_0040055C08D9__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CMultiColumnComboBox window

#include <afxtempl.h>

class CMultiColumnComboBox : public CWnd
{
// Construction
public:
	static UINT m_nSelChange;
	class CWindowProcs
	{
	public:
		CWindowProcs(){};
	protected:
		// Store infos about one window, such as window procedure
		struct SWindowProcedure
		{
			CWnd* m_pWnd;
			WNDPROC m_oldWndProcWnd;
			SWindowProcedure(CWnd* pWnd, WNDPROC wndProc)
			{
				if (m_pWnd = pWnd)
					m_oldWndProcWnd = (WNDPROC)::SetWindowLong(m_pWnd->m_hWnd, GWL_WNDPROC, (long)wndProc);
			};
			virtual ~SWindowProcedure()
			{
				if (m_pWnd)
					::SetWindowLong(m_pWnd->m_hWnd, GWL_WNDPROC, (long)m_oldWndProcWnd);
			}
		};
		// Maps with window procedure of all windows controls registered
		CMap<HWND,HWND, SWindowProcedure*, SWindowProcedure*> m_mapWindowProcedureEdit;
		CMap<HWND,HWND, SWindowProcedure*, SWindowProcedure*> m_mapWindowProcedureListCtrl;
		CMap<HWND,HWND, SWindowProcedure*, SWindowProcedure*> m_mapWindowProcedureParent;
	public:
			// Add, Get, Remove Edit elements
			void AddEdit(CWnd* pEdit, WNDPROC wndProc)
			{
				m_mapWindowProcedureEdit[pEdit->m_hWnd] = new SWindowProcedure(pEdit, wndProc);
			};
			SWindowProcedure* GetEditStructure(HWND hWnd)
			{
				SWindowProcedure* pEditSW = NULL;
				m_mapWindowProcedureEdit.Lookup(hWnd, pEditSW);
				return pEditSW;
			};
			WNDPROC GetOldEditProcedure(HWND hWnd)
			{
				return GetEditStructure(hWnd)->m_oldWndProcWnd;
			}
			void RemoveEdit(CWnd* pEdit)
			{
				if (SWindowProcedure* pSW = GetEditStructure(pEdit->m_hWnd))
				{
					delete pSW;
					m_mapWindowProcedureEdit.RemoveKey(pEdit->m_hWnd);
				}
			};
			// Add, Get, Remove ListControl elements
			void AddListCtrl(CWnd* pListCtrl, WNDPROC wndProc)
			{
				m_mapWindowProcedureListCtrl[pListCtrl->m_hWnd] = new SWindowProcedure(pListCtrl, wndProc);
			};
			SWindowProcedure* GetListCtrlStructure(HWND hWnd)
			{
				SWindowProcedure* pListCtrlSW = NULL;
				m_mapWindowProcedureListCtrl.Lookup(hWnd, pListCtrlSW);
				return pListCtrlSW;
			};
			WNDPROC GetOldListCtrlProcedure(HWND hWnd)
			{
				return GetListCtrlStructure(hWnd)->m_oldWndProcWnd;
			}
			void RemoveListCtrl(CWnd* pListCtrl)
			{
				if (SWindowProcedure* pSW = GetListCtrlStructure(pListCtrl->m_hWnd))
				{
					delete pSW;
					m_mapWindowProcedureListCtrl.RemoveKey(pListCtrl->m_hWnd);
				}
			};
			// Add, Get, Remove Parent elements
			void AddParent(CWnd* pParentWnd, WNDPROC wndProc)
			{
				ASSERT (pParentWnd != NULL);
				if (!GetParentStructure(pParentWnd->m_hWnd))
					m_mapWindowProcedureParent[pParentWnd->m_hWnd] = new SWindowProcedure( pParentWnd, wndProc );
			};
			SWindowProcedure* GetParentStructure(HWND hWnd)
			{
				SWindowProcedure* pParentSW = NULL;
				m_mapWindowProcedureParent.Lookup(hWnd, pParentSW);
				return pParentSW;
			};
			WNDPROC GetOldParentProcedure(HWND hWnd)
			{
				return GetParentStructure(hWnd)->m_oldWndProcWnd;
			}
			void RemoveParent(CWnd* pParent)
			{
				if (SWindowProcedure* pSW = GetParentStructure(pParent->m_hWnd))
				{
					delete pSW;
					m_mapWindowProcedureParent.RemoveKey(pParent->m_hWnd);
				}
			};
	};

	static CWindowProcs m_wndProcs;

	CMultiColumnComboBox(int nColumnKey = 0);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMultiColumnComboBox)
	protected:
	virtual void PreSubclassWindow();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual int SetMultipleHeight(int nMHeight = 5);
	virtual double SetRateWidth(double dWidthList = 0);
	virtual void SetCurrentItem(int nIndex);
	virtual int GetCurrentItem();
	void ForwardMessage(UINT nMsg, WPARAM wParam, LPARAM lParam);
	virtual BOOL IsDropedDown();
	virtual void DropDown(BOOL bDown = TRUE);
	virtual CListCtrl* GetListCtrl() { return m_pListCtrl; }
	virtual CEdit* GetEdit() { return m_pEdit; }

	static BOOL RegClassMultiColumnComboBox();
	virtual ~CMultiColumnComboBox();
	virtual void CaptureListCtrl();

	// Statical functions
	static LRESULT CALLBACK ListCtrlWindowProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK EditWindowProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK ParentWindowProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam);

	// Generated message map functions
protected:
	BOOL m_bFirstShow;
	int m_nMultipleHeight;
	double m_dWidthList;
	virtual void SelectCurrentItem();
	int m_nColumnKey;
	virtual void Search(LPCTSTR lpszFindItem);
	BOOL OnInit();
	static CMultiColumnComboBox* m_pActiveMCBox;
	virtual void DrawButton(CDC* pDC, CRect r, BOOL bDown = FALSE);
	virtual void Resize();
	CListCtrl* m_pListCtrl;
	CEdit* m_pEdit;
	static CFont m_font;
	static CBrush m_brBkGnd;
	//{{AFX_MSG(CMultiColumnComboBox)
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
//	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
//	afx_msg void OnPaint();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	BOOL m_bCaptured;
	void SetButton();
	void ReleaseButton();
	CRect m_rectBtn;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MULTICOLUMNCOMBOBOX_H__1385ADA1_4D4F_11D2_8693_0040055C08D9__INCLUDED_)
