#if !defined( __listabout_H )
#define __listabout_H

#if !defined(AFX_ABOUT_H__50E7EDB9_12C2_401F_961D_364849E8CB0F__INCLUDED_)
#define AFX_ABOUT_H__50E7EDB9_12C2_401F_961D_364849E8CB0F__INCLUDED_

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
// CAbout dialog

class CAbout : public CResizePage
{
   DECLARE_DYNCREATE(CAbout)

// Construction
public:
   CAbout();
   ~CAbout();
   void Resize(CRect &rectPage);
   void Init(void);
// Dialog Data
   //{{AFX_DATA(CAbout)
   enum { IDD = IDD_About };
   CRichEditCtrl   m_Redit;
   CRect         m_rectEdit;
   CString   m_edit;
	CHyperLink *m_mailme;
	CHyperLink *m_followme;
	CHyperLink *m_don;
	CHyperLink *m_support;

   //}}AFX_DATA

// Overrides
   // ClassWizard generate virtual function overrides
   //{{AFX_VIRTUAL(CAbout)
   protected:
   virtual BOOL PreTranslateMessage(MSG* pMsg);
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL
protected:
	void CleanUp();

// Implementation
protected:
   // Generated message map functions
   //{{AFX_MSG(CAbout)
   virtual BOOL OnInitDialog();
	afx_msg void OnDestroy( );
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ABOUT_H__50E7EDB9_12C2_401F_961D_364849E8CB0F__INCLUDED_)
#endif 
