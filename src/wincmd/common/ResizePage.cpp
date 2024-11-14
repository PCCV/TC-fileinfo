// ResizePage.cpp : implementation file
//

#include "stdafx.h"
#include "ResizePage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CResizePage property page

IMPLEMENT_DYNCREATE(CResizePage, CPropertyPage)

CResizePage::CResizePage(UINT nIDTemplate): CPropertyPage(nIDTemplate)
{
    m_first = TRUE;
    m_editmode = FALSE;
	m_forceredraw = FALSE;
   //{{AFX_DATA_INIT(CResizePage)
      // NOTE: the ClassWizard will add member initialization here
   //}}AFX_DATA_INIT
//   Construct(nIDTemplate);
}

CResizePage::~CResizePage()
{
//   TRACE0("Delete ResizePage \n");
//	CleanUp();
}

void CResizePage::OnDestroy( )
{
//	TRACE0("CResizePage : OnDestroy \n");
//	CleanUp();
	m_first = TRUE;
}

void CResizePage::CleanUp()
{
	TRACE0("CResizePage : CleanUp \n");
}

void CResizePage::DoDataExchange(CDataExchange* pDX)
{
   CPropertyPage::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CResizePage)
      // NOTE: the ClassWizard will add DDX and DDV calls here
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CResizePage, CPropertyPage)
   //{{AFX_MSG_MAP(CResizePage)
   ON_WM_SHOWWINDOW()
	ON_WM_DESTROY()
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResizePage message handlers


BOOL CResizePage::OnInitDialog() 
{
   CPropertyPage::OnInitDialog();
   GetWindowRect(&m_oldRect);
   GetParent()->ScreenToClient(&m_oldRect);
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void CResizePage::Resize(CRect &rectPage)
{ 
   m_sizeRelChange.cx = m_oldRect.right - rectPage.right;
   m_sizeRelChange.cy = m_oldRect.bottom - rectPage.bottom;
   MoveWindow(&rectPage);
   m_oldRect = rectPage; 
}


void CResizePage::OnShowWindow(BOOL bShow, UINT nStatus) 
{
   CPropertyPage::OnShowWindow(bShow, nStatus);
   
   if (m_first)
   {
      CRect TempRect;
      GetWindowRect(&TempRect);
      GetParent()->ScreenToClient(&TempRect);
      m_oldRect.top += TempRect.top; 
      m_oldRect.left += TempRect.left; 
      m_oldRect.bottom += TempRect.top; 
      m_oldRect.right += TempRect.left; 
      m_first=FALSE;
   }
}