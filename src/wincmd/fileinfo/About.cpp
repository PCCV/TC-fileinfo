// About.cpp : implementation file
//

#include "stdafx.h"
#include "About.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__; 
#endif

/////////////////////////////////////////////////////////////////////////////
// CAbout property page

IMPLEMENT_DYNCREATE(CAbout, CResizePage)

CAbout::CAbout() : CResizePage(CAbout::IDD)
{
   //{{AFX_DATA_INIT(CAbout)
    m_edit = _T("");
   //}}AFX_DATA_INIT
	m_mailme = NULL;
	m_followme = NULL;
	m_don = NULL;
	m_support = NULL;
	Init();
}

void CAbout::Init(void)
{
   CResizePage::Init();
	if (!m_mailme) {
		m_mailme = new  CHyperLink;
		m_mailme->SetURL(_T("mailto:fg.tcplugins@free.fr"));
		m_mailme->ModifyLinkStyle(0, CHyperLink::StyleDownClick);  
	}	
	if (!m_followme) {
		m_followme = new  CHyperLink;
		m_followme->SetURL(_T("http://fg.tcplugins.free.fr"));
		m_followme->ModifyLinkStyle(0, CHyperLink::StyleDownClick);  
	}
	if (!m_support) {
		m_support = new  CHyperLink;
		m_support->SetURL(_T("http://www.ghisler.ch/board/viewtopic.php?t=1095&sid=ed1478f6afc3a88e40ba44667cf3614b"));
		m_support->ModifyLinkStyle(0, CHyperLink::StyleDownClick);  
	}
	if (!m_don) {
		CString url = _T("");
		CString business	= _T("fgannier@free.fr");  // your paypal email
		CString description	= _T("Donation%20for%20MMedia/Fileinfo%20TC%20plugins");            // '%20' represents a space. remember HTML!
		CString currency	= _T("EUR");

		url = _T("https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=") + business +
			_T("&item_name=") + description +
			_T("&currency_code=") + currency +
			_T("&cn=") + _T("thanks+for+your+support") + 
			_T("&bn=") + _T("PP%2dDonationsBF");
//        "&lc=" + country +

		m_don = new  CHyperLink;
		m_don->SetURL(url);    
		m_don->ModifyLinkStyle(0, CHyperLink::StyleDownClick);  
	}
}

CAbout::~CAbout()
{
   TRACE0("Fileinfo : Delete CAbout \n");
   CleanUp();
}

void CAbout::OnDestroy() 
{
	TRACE0("CAbout : OnDestroy \n");
	CleanUp();
}

void CAbout::CleanUp()
{
	TRACE0("CAbout : CleanUp \n");
	delete m_mailme; 
	delete m_followme;
	delete m_don;
	delete m_support;
	m_mailme = NULL;
	m_followme = NULL;
	m_don = NULL;
	m_support = NULL;
}


void CAbout::DoDataExchange(CDataExchange* pDX)
{
	CResizePage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAbout)
	if (m_don) {
		DDX_Control(pDX, IDC_followme, *m_followme);
		DDX_Control(pDX, IDC_mailme, *m_mailme);
		DDX_Control(pDX, IDC_support, *m_support);
		DDX_Control(pDX, IDC_Don, *m_don);
	}
	DDX_Control(pDX, IDC_RICHEDIT, m_Redit);
	DDX_Text(pDX, IDC_RICHEDIT, m_edit);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAbout, CResizePage)
   //{{AFX_MSG_MAP(CAbout)
	ON_WM_DESTROY()
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAbout message handlers
void CAbout::Resize(CRect &rectPage)
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
//   m_Redit.SetSel( 0, 0);
}

BOOL CAbout::OnInitDialog() 
{
	Init();
   CResizePage::OnInitDialog();
   CString str;
   str.LoadString( IDS_STRING1 ); // Title
   m_edit = str;

#ifndef _WIN64
	m_edit += "x32 Version\n";
#else
	m_edit += "x64 Version\n";
#endif

#if (_DEBUG)
	time_t now;
	struct tm  when;
	char       buff[80];
	time(&now);
	localtime_s( &when, &now );
	asctime_s( buff, sizeof(buff), &when );
	m_edit += buff;
#endif

   str.LoadString( IDS_STRING2 ); // Thanks
   m_edit += str;
   str.LoadString( IDS_STRING5 ); // history 0
   m_edit += str;
   str.LoadString( IDS_STRING35); // history 2.24
   m_edit += str;
   str.LoadString( IDS_STRING34); // history 2.23
   m_edit += str;
   str.LoadString( IDS_STRING33); // history 2.22
   m_edit += str;
   str.LoadString( IDS_STRING32); // history 2.21
   m_edit += str;
   str.LoadString( IDS_STRING31); // history 2.20
   m_edit += str;
   str.LoadString( IDS_STRING30); // history 2.10
   m_edit += str;
   str.LoadString( IDS_STRING29 ); // history 2.09
   m_edit += str;
   str.LoadString( IDS_STRING28 ); // history 2.08
   m_edit += str;
   str.LoadString( IDS_STRING27 ); // history 2.07
   m_edit += str;
   str.LoadString( IDS_STRING26 ); // history 2.06
   m_edit += str;
   str.LoadString( IDS_STRING25 ); // history 2.05
   m_edit += str;
   str.LoadString( IDS_STRING24 ); // history 2.04b
   m_edit += str;
   str.LoadString( IDS_STRING23 ); // history 2.04a
   m_edit += str;
   str.LoadString( IDS_STRING22 ); // history 2.04
   m_edit += str;
   str.LoadString( IDS_STRING21 ); // history 2.03
   m_edit += str;
   str.LoadString( IDS_STRING20 ); // history 2.02
   m_edit += str;
   str.LoadString( IDS_STRING19 ); // history 2.01
   m_edit += str;
   str.LoadString( IDS_STRING17 ); // history 2.00
   m_edit += str;
   str.LoadString( IDS_STRING4 ); // comments
   m_edit += str;
   str.LoadString( IDS_STRING3 ); // Copyrigth
   m_edit += str;

//   m_Redit.SetEventMask( ENM_KEYEVENTS );
   m_Redit.SetOptions(ECOOP_OR, ECO_SAVESEL);
   UpdateData(FALSE);
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CAbout::PreTranslateMessage(MSG* pMsg) 
{
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
