#if !defined( __listwnd_H )
#define __listwnd_H

#if !defined(AFX_LISTWND_H__6AF1D5C6_87B4_4A87_925D_640F51E862F6__INCLUDED_)
#define AFX_LISTWND_H__6AF1D5C6_87B4_4A87_925D_640F51E862F6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ListWnd.h : header file
//
#include <afxctl.h>
#include "PropertySheetRz.h"

/////////////////////////////////////////////////////////////////////////////
// CListWnd window  

class CListWnd : public CWnd
{
	DECLARE_DYNCREATE( CListWnd )
protected:
	CPropertySheetRz	*m_propSheet;
	INT					m_nHeaderHeight;
	BOOL InitSheet();
// Construction
public:
	CListWnd();
	CListWnd(CWnd *wnd); // for subclassing (routing message)
// Attributes
public:
	virtual CPropertySheetRz	*GetpSheet(void) { return m_propSheet; }
	void SetHeaderHeight(int hh) { m_nHeaderHeight = hh; }
// Operations
public:
	protected:
		void CleanUp();
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListWnd)
	public:
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CListWnd();
	// Generated message map functions
protected:
	//{{AFX_MSG(CListWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LISTWND_H__6AF1D5C6_87B4_4A87_925D_640F51E862F6__INCLUDED_)
#endif 