#if !defined( __listoption_H )
#define __listoption_H

#if !defined(AFX_OPTION_H__50E7EDB9_12C2_401F_961D_364849E8CB0F__INCLUDED_)
#define AFX_OPTION_H__50E7EDB9_12C2_401F_961D_364849E8CB0F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// About.h : header file
//
#include "resource.h"
#include "..\common\ResizePage.h"
#include "tcoptions.h"
#include "..\common\HyperLink.h"

/////////////////////////////////////////////////////////////////////////////
// COption dialog

class COption : public CResizePage
{
   DECLARE_DYNCREATE(COption)

// Construction
public:
   COption();
   ~COption();
	void Resize(CRect &rectPage);
	BOOL UpdateData(BOOL bSaveAndValidate = TRUE);
	BOOL Update();
// Dialog Data
   //{{AFX_DATA(COption)
	enum { IDD = IDD_Option };
	//}}AFX_DATA
// Overrides
   // ClassWizard generate virtual function overrides
   //{{AFX_VIRTUAL(COption)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:
   // Generated message map functions
   //{{AFX_MSG(COption)
	virtual BOOL OnInitDialog();
   	afx_msg void OnBnClickedEdit();

   	virtual BOOL OnKillActive( );
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
public:
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTION_H__50E7EDB9_12C2_401F_961D_364849E8CB0F__INCLUDED_)
#endif