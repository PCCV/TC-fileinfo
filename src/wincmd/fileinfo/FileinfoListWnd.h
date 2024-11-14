#if !defined(AFX_FILEINFOLISTWND_H__529E2F84_CB8B_4E18_BEF7_35D132972730__INCLUDED_)
#define AFX_FILEINFOLISTWND_H__529E2F84_CB8B_4E18_BEF7_35D132972730__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FileinfoListWnd.h : header file
//
#include "..\common\ListWnd.h"
#include "ListSheet.h"

#include "peexe\peexe.h"

/////////////////////////////////////////////////////////////////////////////
// CFileinfoListWnd window

class CFileinfoListWnd : public CListWnd
{
// Construction
	CListSheet *m_ppropSheet;
	PE_EXE		*m_pPE;	
//	OPTIONS		m_op;

protected:
	void CleanUp();

public:
	CFileinfoListWnd(PE_EXE *pPE);
//	CFileinfoListWnd(CWnd *wnd);
	BOOL IsCoded();
	void Save();
	PE_EXE *GetPtr() { return m_pPE; }
	void Renew(PE_EXE *pPE);

//	void SetOptionDefault(OPTIONS op) { m_op = op; }
//	OPTIONS *GetOptionDefault() { return &m_op; }

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFileinfoListWnd)
	protected:
//	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CFileinfoListWnd();
	// Generated message map functions
protected:
	//{{AFX_MSG(CFileinfoListWnd)
		// NOTE - the ClassWizard will add and remove member functions here.
		afx_msg void OnDestroy( );
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILEINFOLISTWND_H__529E2F84_CB8B_4E18_BEF7_35D132972730__INCLUDED_)
