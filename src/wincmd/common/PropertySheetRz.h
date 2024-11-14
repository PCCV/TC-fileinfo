#if !defined( __CPropertySheetRz_H )
#define __CPropertySheetRz_H

#if !defined(AFX_PROPERTYSHEETRZ_H__647178DC_1B9A_4EDF_8EFC_0ECE656333A1__INCLUDED_)
#define AFX_PROPERTYSHEETRZ_H__647178DC_1B9A_4EDF_8EFC_0ECE656333A1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ListSheet.h : header file
//
#include "ResizePage.h"
/////////////////////////////////////////////////////////////////////////////
// CPropertySheetRz

class CPropertySheetRz : public CPropertySheet
{
   DECLARE_DYNAMIC(CPropertySheetRz)

// Var privées
   int m_nHeaderHeight;        //The height of the header control
   int m_oldhh;
   bool m_EditMode;

// Construction
public:
   CPropertySheetRz(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
   void SetHeaderHeight(int hh) { m_nHeaderHeight = hh; }
   void Resize(bool force = FALSE);
// Attributes
public:
   CRect         m_rectPage;
protected:
	void CleanUp();

// Operations
public:

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CPropertySheetRz)
   public:
   virtual BOOL PreTranslateMessage(MSG* pMsg);
   protected:
   virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
   //}}AFX_VIRTUAL

// Implementation
public:
   virtual ~CPropertySheetRz();

   // Generated message map functions
protected:
   //{{AFX_MSG(CPropertySheetRz)
   afx_msg void OnDestroy( );
      afx_msg void OnApplyNow();
      afx_msg LRESULT OnResizePage(WPARAM wParam, LPARAM lParam);
      virtual BOOL OnInitDialog();
      afx_msg BOOL OnPageChanging(NMHDR* pNotifyStruct, LRESULT* pResult);
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTYSHEETRZ_H__647178DC_1B9A_4EDF_8EFC_0ECE656333A1__INCLUDED_)
#endif 
