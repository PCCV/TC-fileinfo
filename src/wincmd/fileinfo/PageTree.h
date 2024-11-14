#if !defined(AFX_PAGETREE_H__6E3A41C3_5866_48C5_8E85_2183C801182A__INCLUDED_)
#define AFX_PAGETREE_H__6E3A41C3_5866_48C5_8E85_2183C801182A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PageTree.h : header file
//
#include "resource.h"
#include "..\common\ResizePage.h"
#include "..\common\wait.h"

typedef void ( * pfunc )(PVOID ptr, CTreeCtrl &tree, CWait &wait );
/////////////////////////////////////////////////////////////////////////////
// CPageTree dialog

class CPageTree : public CResizePage
{
   DECLARE_DYNCREATE(CPageTree)

// Construction
public:
   CPageTree();
   ~CPageTree();
   void Resize(CRect &m_rectPage);
   CTreeCtrl   *GetTree() { return &m_tree; }
   void SetPtr(PVOID ptr) { m_ptr = ptr; }
   void SetFillTree(pfunc FT) { FillTree = FT; }
   void Renew(PVOID pPE);

// Dialog Data
   //{{AFX_DATA(CPageTree)
   enum { IDD = IDD_PageDll };
   CTreeCtrl   m_tree;
   //}}AFX_DATA
   CRect   m_rectTree;
   PVOID		m_ptr;
   pfunc FillTree;			//   void (* FillTree )(const char *FileToLoad, CTreeCtrl &tree );

protected:
	void CleanUp();

// Overrides
   // ClassWizard generate virtual function overrides
   //{{AFX_VIRTUAL(CPageTree)
   protected:
   virtual BOOL PreTranslateMessage(MSG* pMsg);
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:
   CImageList  *pImageList;
   // Generated message map functions
   //{{AFX_MSG(CPageTree)
	   virtual BOOL OnInitDialog();
	   	afx_msg void OnDestroy( );
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGETREE_H__6E3A41C3_5866_48C5_8E85_2183C801182A__INCLUDED_)
