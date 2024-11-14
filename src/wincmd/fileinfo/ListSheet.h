#if !defined( __listsheet_H )
#define __listsheet_H

#if !defined(AFX_LISTSHEET_H__F0510B14_F615_46D5_BF14_A5E8433B7BD4__INCLUDED_)
#define AFX_LISTSHEET_H__F0510B14_F615_46D5_BF14_A5E8433B7BD4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ListSheet.h : header file
//
#include "..\common\PropertySheetRz.h"
#include "..\common\ListPagePty.h"
#include "ListExport.h"
#include "PageTree.h"
#include "ListOCX.h"
#include "Option.h"
#include "About.h"
/////////////////////////////////////////////////////////////////////////////
// CListSheet

class CListSheet : public CPropertySheetRz
{
	DECLARE_DYNAMIC(CListSheet)
// Var privées
// Construction
public:
	CListSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
// Attributes
public:
	CListpagePty	m_fi;
	CListpagePty	m_fi2;
	CPageTree		m_dll;
	CListExport		m_export;
	CListOcx		m_ocx;
	CListpagePty	m_disass;
    COption			m_option;
	CListpagePty	m_manifest;
	CAbout			m_about;

protected:
	void CleanUp();

// Operations
public:
	void Renew(PE_EXE *pPE);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListSheet)
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CListSheet();
	afx_msg void OnDestroy( );
	BOOL SetPageTitle (int nPage, LPTSTR pszText);
	// Generated message map functions
protected:
	//{{AFX_MSG(CListSheet)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LISTSHEET_H__F0510B14_F615_46D5_BF14_A5E8433B7BD4__INCLUDED_)
#endif 