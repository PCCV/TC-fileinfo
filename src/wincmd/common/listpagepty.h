	#if !defined( __listpty_H )
#define __listpty_H

#if !defined(AFX_LISTPAGEPTY_H__A31543E2_8C2D_482E_8AC4_1F5E119F6160__INCLUDED_)
#define AFX_LISTPAGEPTY_H__A31543E2_8C2D_482E_8AC4_1F5E119F6160__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ListpagePty.h : header file
//
#include "..\common\resource.h"
#include "resource.h"
#include "ResizePage.h"
#include "..\..\wincmd\common\TCutils.h"
#include "wait.h"

typedef CString ( *pfuncRE )( PVOID ptr, CWait &wait );
/////////////////////////////////////////////////////////////////////////////
// CListpagePty dialog

class CListpagePty : public CResizePage
{
   DECLARE_DYNCREATE(CListpagePty)

// Construction
public:
   CListpagePty();
   ~CListpagePty();
   void Resize(CRect &m_rectPage);
   void SetTab(int val, int *p)
   {   
      m_NbTab = val; 
      if (m_pTab) delete m_pTab;
      m_pTab = new int[m_NbTab];
      for (int i=0;i<m_NbTab;i++)
         m_pTab[i] = p[i];
      if (!m_first) 
         UpdateTab();
   }
   void UpdateTab( void );
   void UpdateFont( void );
   void SetFontBold(BOOL fb) { m_fo.fontbold = fb; if (!m_first) UpdateFont(); }
   void SetFontName(_TCHAR *fn) { _tcscpy_s(m_fo.fontname, fn); if (!m_first) UpdateFont(); }
   void SetFontSize(long size) { m_fo.fontsize = size; if (!m_first) UpdateFont(); }
   void SetFontItalic( BOOL fi) { m_fo.fontitalic = fi; if (!m_first) UpdateFont(); }
   void SetFontPpty( FontOptions fo) { m_fo = fo; if (!m_first) UpdateFont();}
   CString GetText() { return m_text; }
   void SetFillEdit(pfuncRE FE) { FillEdit = FE; }
   void SetPtr(PVOID ptr) { m_ptr = ptr; }
	void Renew(PVOID pPE);
	void SetWrap(bool wrap)
	{ 
		m_Redit.SetTargetDevice(NULL, wrap); // 1 = wrap on, 0 = wrap off
		m_Redit.ShowScrollBar( SB_HORZ , wrap);
	}
	void SetText(CString &text);
	void SetCenter(bool b=TRUE);
	void CListpagePty::Renew();

protected:
	void CleanUp();

// Dialog Data
   //{{AFX_DATA(CListpagePty)
   enum { IDD = IDD_fi };
   CRichEditCtrl   m_Redit;  
//   CSimpleHtmlCtrl m_Redit;  
   //}}AFX_DATA
   CString		m_text;
   CRect		m_rectEdit;
//   BOOL			m_exist;
   FontOptions	m_fo;
   int			font;
   int			fontplus;
   int			m_NbTab;
   int			*m_pTab;
   pfuncRE		FillEdit;   
	PVOID		m_ptr;
	bool		m_bwrap;
	bool		m_bcenter;
	
// Overrides
   // ClassWizard generate virtual function overrides
   //{{AFX_VIRTUAL(CListpagePty)
   public:
   virtual BOOL PreTranslateMessage(MSG* pMsg);
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
//   virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
   //}}AFX_VIRTUAL

// Implementation
protected:
   // Generated message map functions
   //{{AFX_MSG(CListpagePty)
	virtual BOOL OnInitDialog();
   	afx_msg void OnDestroy( );
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg BOOL OnMenu(UINT nID);
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LISTPAGEPTY_H__A31543E2_8C2D_482E_8AC4_1F5E119F6160__INCLUDED_)
#endif 