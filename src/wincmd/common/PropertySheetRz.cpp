// ListSheet.cpp : implementation file
//

#include "stdafx.h"
#include "PropertySheetRz.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CListSheet

LRESULT CALLBACK GetMessageHookProc( int nCode, WPARAM wParam, LPARAM lParam);
HHOOK g_hGetMsgHook = NULL;
int HookCount = 0;
/**/
IMPLEMENT_DYNAMIC(CPropertySheetRz, CPropertySheet)

CPropertySheetRz::CPropertySheetRz(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
   :CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	m_nHeaderHeight = 0;
	m_oldhh = 0;
	EnableStackedTabs( FALSE );
}

CPropertySheetRz::~CPropertySheetRz()
{
//	TRACE0("Delete CPropertySheetRz \n");
	CleanUp();
}

void CPropertySheetRz::OnDestroy( )
{
	TRACE0("CPropertySheetRz : OnDestroy \n");
	CleanUp();
}

void CPropertySheetRz::CleanUp()
{
	TRACE0("CPropertySheetRz : CleanUp \n");
	if (!HookCount)
	{
		VERIFY (::UnhookWindowsHookEx (g_hGetMsgHook));
		g_hGetMsgHook = NULL;
	} else HookCount--;
}

BOOL CPropertySheetRz::OnInitDialog() 
{
   BOOL bResult = CPropertySheet::OnInitDialog();
//   Resize();
	// Disable the OK button	
    GetDlgItem(IDOK)->DestroyWindow();
//	GetDlgItem(IDCANCEL)->DestroyWindow();
//	GetDlgItem(IDHELP)->DestroyWindow();
//	GetDlgItem(ID_APPLY_NOW)->DestroyWindow();

 // Install the WH_GETMESSAGE hook function.
   if (!g_hGetMsgHook)
	{
		g_hGetMsgHook = ::SetWindowsHookEx(
			WH_GETMESSAGE,
			GetMessageHookProc,
			AfxGetInstanceHandle(),
			GetCurrentThreadId());
		ASSERT (g_hGetMsgHook);
	} else HookCount++;

   return bResult;
}

#define WM_RESIZEPAGE (WM_USER + 111)
BEGIN_MESSAGE_MAP(CPropertySheetRz, CPropertySheet)
   //{{AFX_MSG_MAP(CPropertySheetRz)
		ON_WM_DESTROY()
      ON_COMMAND(ID_APPLY_NOW, OnApplyNow)
      ON_MESSAGE(WM_RESIZEPAGE, OnResizePage)
      ON_NOTIFY_REFLECT_EX(PSN_SETACTIVE, OnPageChanging)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertySheetRz message handlers


LRESULT CPropertySheetRz::OnResizePage(WPARAM, LPARAM)
{   // Resize the page using m_rectPage, which was set in Resize().
   CResizePage* pPage = (CResizePage*) GetActivePage();
   ASSERT(pPage);   
   pPage->Resize(m_rectPage);
   return 0;
}

void CPropertySheetRz::OnApplyNow()
{   // The sheet resizes the page whenever the apply button is clicked.
   // So we need to resize it to what we want.
   PostMessage(WM_RESIZEPAGE);
}


BOOL CPropertySheetRz::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
   NMHDR* pNMHDR = (LPNMHDR) lParam;
   // The sheet resizes the page whenever it is activated so we need to resize it to what we want
   if (pNMHDR->code == TCN_SELCHANGE)
      // user-defined message needs to be posted, not sent, because
      // page must be resized after TCN_SELCHANGE has been processed.
      PostMessage(WM_RESIZEPAGE);

   return CPropertySheet::OnNotify(wParam, lParam, pResult);
}

/////////////////////////////////////
// A Voir   
// http://www.codeguru.com/propertysheet/resizeinview.shtml
void CPropertySheetRz::Resize(bool force)
{   // Find parent 
   CWnd* pParent;
   CRect rectParent;
   pParent = GetParent();
   pParent->GetClientRect(&rectParent);
   
   if ((rectParent.right==0) || (rectParent.bottom==0)) { return; } // exit if size is zero
   // Resize the sheet, First find relative change
   CSize sizeRelChange;
   CRect rectWindow;

   GetWindowRect(&rectWindow);
   ScreenToClient(&rectWindow);
   sizeRelChange.cx = rectWindow.Width() - rectParent.Width();
   sizeRelChange.cy = rectWindow.Height() - rectParent.Height();

   if (sizeRelChange.cy || sizeRelChange.cx || force)
   {
      // Decalage Header
      rectWindow.top += m_nHeaderHeight;
      rectWindow.right -= sizeRelChange.cx;
      rectWindow.bottom -= sizeRelChange.cy;
      // Then resize the sheet
      MoveWindow(&rectWindow);

      // Resize the CTabCtrl
      CTabCtrl* pTab = GetTabControl();
      ASSERT(pTab);
      pTab->GetWindowRect(&rectWindow);
      ScreenToClient(&rectWindow);

      rectWindow.right -= sizeRelChange.cx;
      rectWindow.bottom -= (sizeRelChange.cy + m_nHeaderHeight);
      pTab->MoveWindow(&rectWindow);

      // Resize the active page
      CResizePage* pPage = (CResizePage*) GetActivePage();
      ASSERT(pPage);

      pPage->GetWindowRect(&m_rectPage);
      ScreenToClient(&m_rectPage);

      m_rectPage.right -= sizeRelChange.cx;
      m_rectPage.bottom -= (sizeRelChange.cy + m_nHeaderHeight);
      pPage->Resize(m_rectPage);
   }
   m_oldhh = m_nHeaderHeight;
}


BOOL CPropertySheetRz::PreTranslateMessage(MSG* pMsg) 
{
	if (!((CResizePage*) GetActivePage())->IsEditable())
		if (pMsg->message == WM_KEYDOWN ) 
		{
			if ((0x8000 & GetKeyState(VK_CONTROL))) // if CTRL
			{
//				if (pMsg->wParam == VK_TAB)
//					return CPropertySheet::PreTranslateMessage(pMsg);
				if ((pMsg->wParam >= '0') && (pMsg->wParam <= '9'))
					SetActivePage( pMsg->wParam - '1' );
			}
			if (pMsg->wParam == VK_DOWN
				  || pMsg->wParam == VK_UP 
				  || pMsg->wParam == VK_LEFT 
				  || pMsg->wParam == VK_RIGHT 
				  || pMsg->wParam == VK_RETURN
				  || pMsg->wParam == VK_TAB)
//                    return GetParent()->PostMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
                    GetParent()->PostMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
			if (pMsg->wParam == VK_ESCAPE
				  || pMsg->wParam == VK_DELETE
				  || pMsg->wParam == VK_SPACE)
               return GetParent()->PostMessage(pMsg->message, pMsg->wParam, pMsg->lParam);

//         if ((pMsg->wParam >= '*') && (pMsg->wParam <= '/'))
//					return GetParent()->PostMessage(pMsg->message, pMsg->wParam, pMsg->lParam);

         if ((pMsg->wParam >= '0') && (pMsg->wParam <= 'Z'))
					return GetParent()->PostMessage(pMsg->message, pMsg->wParam, pMsg->lParam);

			if ((pMsg->wParam >= 0x60) && (pMsg->wParam <= 0x6F)) // + & -
				return GetParent()->PostMessage(pMsg->message, pMsg->wParam, pMsg->lParam);

/*			if ((0x8000 & GetKeyState(VK_CONTROL))) // if CTRL
			{
				if (pMsg->wParam == 'N')
						return GetParent()->PostMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
				if (pMsg->wParam == 'P')
						return GetParent()->PostMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
			}/**/
		}
	return CPropertySheet::PreTranslateMessage(pMsg);
}


BOOL CPropertySheetRz::OnPageChanging(NMHDR* /*pNotifyStruct*/, LRESULT* /*pResult*/)
{
   PostMessage(WM_RESIZEPAGE);   
   return FALSE;   // continue routing
}

static LRESULT CALLBACK GetMessageHookProc( int nCode, WPARAM wParam, LPARAM lParam)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
    // If this is a keystrokes message, translate it in controls'PreTranslateMessage().
    LPMSG lpMsg = (LPMSG) lParam;
	if( (nCode >= 0)
		&& PM_REMOVE == wParam 
		&& ((lpMsg->message >= WM_KEYFIRST && lpMsg->message <= WM_KEYLAST)
		|| lpMsg->message == EM_SETSEL
		|| lpMsg->message == WM_COPY))
			if ( AfxGetApp()->PreTranslateMessage((LPMSG)lParam) )
			{
				 lpMsg->message = WM_NULL;
				 lpMsg->lParam = 0L;
				 lpMsg->wParam = 0;
			}
	return ::CallNextHookEx(g_hGetMsgHook, nCode, wParam, lParam);
}