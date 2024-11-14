// ListWnd.cpp : implementation file
//

#include "stdafx.h"
#include "ListWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CListWnd

IMPLEMENT_DYNAMIC(CListWnd, CWnd)

CListWnd::CListWnd():m_nHeaderHeight(0)
{
   m_propSheet = NULL;
}

CListWnd::CListWnd(CWnd *wnd):m_nHeaderHeight(0)
{
/*
   m_hWnd = 0;
   if (!this->SubclassWindow(wnd->m_hWnd)) return;

   if (m_propSheet->m_psh.hwndParent != NULL) return;
   DWORD style = WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE;
   DWORD exStyle = WS_EX_RIGHTSCROLLBAR;
   BOOL result = m_propSheet->Create(this, style, exStyle);
*/
}

CListWnd::~CListWnd()
{
   TRACE0("Delete ListWnd \n");
	CleanUp();
}

void CListWnd::CleanUp()
{
	TRACE0("CListWnd : CleanUp \n");
}

BEGIN_MESSAGE_MAP(CListWnd, CWnd)
   //{{AFX_MSG_MAP(CListWnd)
   ON_WM_CREATE()
   ON_WM_WINDOWPOSCHANGED()
   ON_WM_SETFOCUS()
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListWnd message handlers

BOOL CListWnd::InitSheet() 
{
	if (m_propSheet->m_psh.hwndParent != NULL) return -1;
	DWORD style = WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE;
	if (GetActiveWindow() != GetParent())	style |= WS_DISABLED;
	DWORD exStyle = WS_EX_RIGHTSCROLLBAR;
	bool val = ( m_propSheet->Create(this, style, exStyle) != 0);
	m_propSheet->ModifyStyleEx (0, WS_EX_CONTROLPARENT);
	m_propSheet->ModifyStyle( 0, WS_TABSTOP );
	return val;
}

int CListWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
   if (!m_propSheet) return -1;
   if (!m_hWnd)
      if (CWnd::OnCreate(lpCreateStruct) == -1)
      return -1;
   InitSheet();
   return 0;
}

void CListWnd::OnWindowPosChanged(WINDOWPOS FAR* lpwndpos) 
{
   CWnd::OnWindowPosChanged(lpwndpos);
   if (m_propSheet) m_propSheet->Resize();
}   

void CListWnd::OnSetFocus(CWnd* pOldWnd) 
{
	m_propSheet->EnableWindow();
	CWnd::OnSetFocus(pOldWnd);
	m_propSheet->SetFocus();
}
