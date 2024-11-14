// ListpagePty.cpp : implementation file
//

#include "stdafx.h"
#include "ListpagePty.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CListpagePty property page

IMPLEMENT_DYNCREATE(CListpagePty, CResizePage)

CListpagePty::CListpagePty() : CResizePage(CListpagePty::IDD)
{
   //{{AFX_DATA_INIT(CListpagePty)
   //}}AFX_DATA_INIT
   m_text = _T("");
   FillEdit = NULL;
//   m_exist = FALSE;
   m_fo.fontsize = 0;
   *m_fo.fontname = '\0';
   m_fo.fontbold = FALSE;
   m_fo.fontitalic = FALSE;
   fontplus = 0;
   m_bwrap = TRUE;
   m_NbTab = 1;
   m_pTab = new int[m_NbTab];
   m_pTab[0]   = 2000;
   m_bcenter = FALSE;
}

CListpagePty::~CListpagePty()
{
	TRACE0("ListPagePty : Delete RichEditPage \n");
	CleanUp();
}

void CListpagePty::Renew(PVOID pPE)
{
	m_text = _T("");
	m_ptr = pPE;
	if ( m_Redit.m_hWnd) {
	   if (FillEdit) 
	   {
			CWait wait(this);
			wait.SetStatus("Dumping...");
			//m_text = ((pfuncRE) FillEdit) ((LPCTSTR) m_file, wait);
			m_text = ((pfuncRE) FillEdit) ( m_ptr, wait);
	   }
		m_Redit.SetWindowText( m_text );
		UpdateTab( );
		m_Redit.SetOptions(ECOOP_OR, ECO_SAVESEL);
	}
}

void CListpagePty::OnDestroy() 
{
	TRACE0("CListpagePty : OnDestroy \n");
	// CleanUp();
}

void CListpagePty::CleanUp()
{
	TRACE0("CListpagePty : CleanUp \n");
	if (m_pTab) {
		delete [] m_pTab;
		m_pTab = NULL;
		m_NbTab = 0;
	}
}

void CListpagePty::DoDataExchange(CDataExchange* pDX)
{
   CResizePage::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CListpagePty)
   DDX_Control(pDX, IDC_RicheditFI, m_Redit);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CListpagePty, CResizePage)
   //{{AFX_MSG_MAP(CListpagePty)
	ON_WM_DESTROY()
	ON_WM_CREATE()

	ON_WM_CONTEXTMENU()
	ON_COMMAND_EX_RANGE(ID_COPY, ID_SELECTALL, OnMenu)
//	ON_COMMAND_EX(ID_COPY, OnMenu)
//	ON_COMMAND_EX(ID_COPY_S, OnMenu)
//	ON_COMMAND_EX(ID_SELECTALL, OnMenu)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListpagePty message handlers

// http://www.tenouk.com/visualcplusmfc/visualcplusmfc7atwo.html
BOOL CListpagePty::OnMenu(UINT nID) 
{
	switch(nID) {
		case ID_COPY: m_Redit.Copy(); break;
		case ID_COPY_S: m_Redit.SetSel( 0, -1 ); m_Redit.Copy(); m_Redit.SetSel( 0, 0 ); break;
		case ID_SELECTALL: m_Redit.SetSel( 0, -1 ); break;
	}
	return TRUE;
}

void CListpagePty::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	if (*pWnd == m_Redit) 
	{
		CMenu menu;
		CMenu* pContextMenu; 
		VERIFY(menu.LoadMenu(IDR_MENU2));
		pContextMenu = menu.GetSubMenu(0);
		ASSERT(pContextMenu != NULL);
		if (point.x < 0) GetCursorPos(&point);
		pContextMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_LEFTBUTTON, point.x, point.y, this);
	}
}

/*
int CListpagePty::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CResizePage::OnCreate(lpCreateStruct) == -1)
		return -1;
//	::SendMessage(m_Redit.m_hWnd, EM_SETEVENTMASK, (WPARAM)0, (LPARAM)ENM_MOUSEEVENTS);
	return 0;
}/**/

void CListpagePty::Resize(CRect &rectPage)
{
	CResizePage::Resize(rectPage);
   	if (m_sizeRelChange.cx + m_sizeRelChange.cy)
	{
		m_Redit.GetWindowRect(&m_rectEdit);
		ScreenToClient(&m_rectEdit);
		m_rectEdit.right -= m_sizeRelChange.cx; 
		m_rectEdit.bottom -= m_sizeRelChange.cy;
		m_Redit.MoveWindow(&m_rectEdit);
	}
}

BOOL CListpagePty::OnInitDialog()
{
   CResizePage::OnInitDialog();   
   UpdateFont();   
   if (FillEdit) 
   {
		CWait wait(this);
		wait.SetStatus("Dumping...");
		//m_text = ((pfuncRE) FillEdit) ((LPCTSTR) m_file, wait);
		m_text = ((pfuncRE) FillEdit) ( m_ptr, wait);
   }
   m_Redit.SetWindowText( m_text );
   UpdateTab( );
   m_Redit.SetOptions(ECOOP_OR, ECO_SAVESEL);

	//   m_Redit.SetEventMask( ENM_KEYEVENTS );
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CListpagePty::PreTranslateMessage(MSG* pMsg) 
{
//	if ((0x8000 & GetKeyState(VK_CONTROL))) // if CTRL
   if (pMsg->message == WM_KEYDOWN ) 
   {

		if (pMsg->wParam == 'W')
			{
				m_bwrap=!m_bwrap;
				SetWrap(m_bwrap);
				return 0;
			}
	}
// EN_MSGFILTER 
   if (pMsg->message == EM_SETSEL)
   {
      m_Redit.SetSel( 0, -1 );
      return 1;
   }
   if (pMsg->message == WM_COPY)
   {
      m_Redit.Copy();
      return 1;
   }
   return CResizePage::PreTranslateMessage(pMsg);
}

/*
BOOL CListpagePty::PreCreateWindow(CREATESTRUCT& cs) 
{
   // TODO: Add your specialized code here and/or call the base class
   if(!CResizePage::PreCreateWindow(cs))
      return FALSE;
   //////////////////////////////////////////////////////////////////////////////////////////////
   // Use RichEdit 2.0 !!!
   // To use RichEdit 1.0 just comment next line
   cs.lpszClass=RICHEDIT_CLASSA;
   //////////////////////////////////////////////////////////////////////////////////////////////
   return TRUE;
}
*/

void CListpagePty::UpdateFont( void )
{
    CHARFORMAT cf; BOOL modif = FALSE;
    m_Redit.GetDefaultCharFormat( cf );
	if (m_fo.fontsize )
	{
	  cf.dwMask = CFM_SIZE;
	  cf.yHeight = m_fo.fontsize;
	  modif = TRUE;
	} 
	else font = cf.yHeight; /**/
	if (fontplus)
		cf.yHeight += fontplus;

   if (*m_fo.fontname)
   {
      cf.dwMask = cf.dwMask | CFM_FACE;
      _tcscpy(cf.szFaceName,  m_fo.fontname);
      modif = TRUE;
   }

   if (m_fo.fontbold)
   {
      cf.dwMask |= CFM_BOLD;
      cf.dwEffects = CFE_BOLD;
      modif = TRUE;
   }
   
   if (m_fo.fontitalic)
   {
      cf.dwMask |= CFM_ITALIC;
      cf.dwEffects = CFE_ITALIC;
      modif = TRUE;
   }

   if (modif)
      m_Redit.SetDefaultCharFormat( cf );
}

void CListpagePty::UpdateTab( void )
{
   PARAFORMAT2 pf;
    pf.cbSize=sizeof(PARAFORMAT2);

   m_Redit.SetSel(0,-1);
   pf.dwMask=PFM_SPACEBEFORE;
   pf.dySpaceBefore=60;
   pf.dwMask |= PFM_TABSTOPS;
   pf.cTabCount = m_NbTab; 
   for (int i=0;i<m_NbTab;i++)
      pf.rgxTabs[i] = m_pTab[i];

	if (m_bcenter)
	{
		pf.dwMask = PFM_ALIGNMENT;
		pf.wAlignment = PFA_CENTER;
	}
   m_Redit.SetParaFormat( pf );
   m_Redit.SetSel(0,0);
}

void CListpagePty::SetText(CString &text) 
{ 
	m_text = text;
	if (!m_first) 
	{
		if (m_Redit) {
			m_Redit.SetWindowText( m_text );
			UpdateTab( );
		}
	}
}

void CListpagePty::SetCenter(bool b)
{
	m_bcenter = b;
}