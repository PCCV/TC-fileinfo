#if !defined(AFX_WAIT_H__328F5DEC_6E63_433E_A0B3_6CC894EB39DF__INCLUDED_)
#define AFX_WAIT_H__328F5DEC_6E63_433E_A0B3_6CC894EB39DF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Wait.h : header file
//
#include "resource.h"
/////////////////////////////////////////////////////////////////////////////
// CWait dialog

class CWait : public CDialog
{
// Construction
public:
	CWait(CWnd* pParent = NULL);   // standard constructor
	void SetStatus(CString str) { m_str = str; UpdateData(FALSE);}
// Dialog Data
	//{{AFX_DATA(CWait)
	enum { IDD = IDD_Wait };
	CString	m_str;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWait)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWait)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WAIT_H__328F5DEC_6E63_433E_A0B3_6CC894EB39DF__INCLUDED_)
