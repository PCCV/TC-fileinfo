// ListExport.cpp : implementation file
//

#include "stdafx.h"
#include "ListExport.h"
#include "ListDlg.h"
#include <imagehlp.h>
#include <afxole.h> // for clipboard
#include <afxadv.h> // shared file

#include "..\common\wait.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define FontSize 13
extern BOOL b_W95Protect;

/////////////////////////////////////////////////////////////////////////////
// CListExport property page

IMPLEMENT_DYNCREATE(CListExport, CResizePage)

CListExport::CListExport() : CResizePage(CListExport::IDD)
{
	//{{AFX_DATA_INIT(CListExport)
	m_undecorate = TRUE;
	m_bsort = FALSE;
	m_nbfunc = 0;
	//}}AFX_DATA_INIT
	m_pe = NULL;
	font = NULL;
	m_NbIF = 0;
	m_bsort = FALSE;
}

extern char inifilename[MAX_PATH];
CListExport::~CListExport()
{
	CleanUp();
}

void CListExport::OnDestroy() 
{
	TRACE0("CListExport : OnDestroy \n");
	CleanUp();
}

void CListExport::Renew(PE_EXE *pPE)
{
	m_pe=pPE;
	if (m_listmodule.m_hWnd) {
		m_listmodule.ResetContent();
		if (m_pe->IsValid())
			Load();
	}
}

void CListExport::CleanUp()
{
	TRACE0("CListExport : CleanUp \n");
	if (font)	delete font;
	font = NULL;
	if (IsWindow(m_hWnd))   
	{
		UpdateData(TRUE);	
		char temp[50]; 
		WritePrivateProfileString( "Options", "Sort", _itoa(m_bsort, temp, 10), inifilename);
	}
}

void CListExport::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CListExport)
	DDX_Control(pDX, IDC_LIST1, m_listmodule);
	DDX_Control(pDX, IDC_LIST2, m_list);
	DDX_Check(pDX, IDC_undecorate, m_undecorate);
	DDX_Check(pDX, IDC_CHECK1, m_bsort);
	DDX_Text(pDX, IDC_nbfunc, m_nbfunc);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CListExport, CPropertyPage)
	//{{AFX_MSG_MAP(CListExport)
	ON_LBN_SELCHANGE(IDC_LIST1, OnSelchangeModules)
	ON_BN_CLICKED(IDC_undecorate, Onundecorate)
	ON_BN_CLICKED(IDC_BUTTON1, OnTestImport)
	ON_LBN_SELCHANGE(IDC_LIST2, OnSelchangeFunc)
	ON_BN_CLICKED(IDC_CHECK1, OnSort)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListExport message handlers
void CListExport::Resize(CRect &rectPage)
{
   CResizePage::Resize(rectPage);

   m_list.GetWindowRect(&m_rectList);
   ScreenToClient(&m_rectList);
   m_rectList.right -= m_sizeRelChange.cx; 
   m_rectList.bottom -= m_sizeRelChange.cy;
   m_list.MoveWindow(&m_rectList);

   m_listmodule.GetWindowRect(&m_rectModule);
   ScreenToClient(&m_rectModule);
   m_rectModule.right -= m_sizeRelChange.cx;   
   m_listmodule.MoveWindow(&m_rectModule);
}

void CListExport::Load()
{
		CString str;
		CWait wait(this);
		wait.SetStatus("Listing Modules...");
		
		m_pe->IsCoded();		//Test compressed and decompress
		str.Format("%s  ( Exported functions )", m_pe->GetBaseName() );
		m_listmodule.AddString( str );
		MODULE_DEPENDENCY_LIST *pDep = m_pe->GetDepends();
		if (pDep)
		{
			PMODULE_FILE_INFO pModInfo = pDep->GetNextModule( (PMODULE_FILE_INFO) 0 );
			if (pModInfo )
				while ( pModInfo = pDep->GetNextModule( pModInfo ) )
				{
					m_listmodule.AddString( pModInfo->GetBaseName() );
					m_NbIF++;
				}
			pModInfo = pDep->GetNextDelayedModule( (PMODULE_FILE_INFO) 0 );
			if (pModInfo )
				while ( pModInfo = pDep->GetNextDelayedModule( pModInfo ) )
				{
					str.Format("%s  ( Delayed Import )", pModInfo->GetBaseName() );
					m_listmodule.AddString( str );
				}
		}
		wait.SetStatus("Listing Functions...");
		AddFunction(0);
		m_listmodule.SetCurSel( 0 );
}

BOOL CListExport::OnInitDialog() 
{
	CResizePage::OnInitDialog();
	if (m_bsort) OnToggleListStyle();
	if (m_pe->IsValid())
	{
		Load();
	}
	m_list.SetHorizontalExtent( m_Hsize * 10 );
	font = new(CFont);
	font->CreateFont( -FontSize, 0, 0, 0, FW_THIN, 0, 0, 0, ANSI_CHARSET, OUT_DEVICE_PRECIS, CLIP_CHARACTER_PRECIS, PROOF_QUALITY, FF_MODERN, "Tahoma" ); // modern Courrier New
	m_list.SetFont( font );
	
    return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void CListExport::OnToggleListStyle() 
{
	CRect rc;
	CFont *fnt = m_list.GetFont();
	m_list.GetWindowRect(&rc);
	ScreenToClient(&rc);
	m_ListStyle = m_list.GetStyle() ^ LBS_SORT | WS_VSCROLL | WS_HSCROLL;
	m_list.DestroyWindow();
	m_list.Create( m_ListStyle, rc, this, IDC_LIST2 );
	m_list.ModifyStyleEx( 0, WS_EX_CLIENTEDGE, SWP_DRAWFRAME );
	m_list.SetFont( fnt );
	UpdateData(FALSE);
}

#define SIZEBUFFER 1024
extern PSTR Undecorate(PSTR Textin, PSTR Textout, int len);
void CListExport::AddFunction(int sel)
{
	m_list.ResetContent( );
	CString strTemp="";
	int i;
	m_Hsize =0;
	if (sel)
	{
// Import Section
		MODULE_DEPENDENCY_LIST *pDep = m_pe->GetDepends();
		PMODULE_FILE_INFO pModInfo;
		m_listmodule.GetText( sel, strTemp );
		m_nbfunc = 0;
		if (pDep)
		{
			if ( sel <= m_NbIF )
			{
				pModInfo = pDep->GetNextModule( (PMODULE_FILE_INFO) 0 );
				if (!pModInfo) return;
				while(strTemp != pModInfo->GetBaseName())
				{
					pModInfo = pDep->GetNextModule( pModInfo );
					if (!pModInfo) return;
				}
				CStringList *pFlist = pModInfo->GetFList();
				POSITION pos = pFlist->GetHeadPosition();
				if (!pos) return;
				CString func; 
				do {
					func = pFlist->GetNext(pos);
					if (strncmp("ordinal ", func, 8)==0)
						strTemp = func;
					else 
					{
						if ( m_undecorate ) // Undecorate Name
						{
				 			char Textout[SIZEBUFFER];
							strTemp.Format("%s", Undecorate((char *) (LPCSTR) func, Textout, SIZEBUFFER));
						}
						else strTemp.Format("%s", func);
					}
					if (m_Hsize < strTemp.GetLength()) m_Hsize = strTemp.GetLength();
					m_list.AddString( strTemp );
					m_nbfunc ++;
				} while( pos );
			}
			else 
			{
				pModInfo = pDep->GetNextDelayedModule( (PMODULE_FILE_INFO) 0 );
				if (!pModInfo) return;
				int nb = strTemp.GetLength();
				while(strncmp(strTemp, pModInfo->GetBaseName(), nb-20))
				{
					pModInfo = pDep->GetNextDelayedModule( pModInfo );
					if (!pModInfo) return;
				}

				CStringList *pFlist = pModInfo->GetFList();
				POSITION pos = pFlist->GetHeadPosition();
				if (!pos) return;
				CString func;
				do {
					func = pFlist->GetNext(pos);
					if (strncmp("ordinal ", func, 8)==0)
						strTemp = func;
					else 
					{
						if ( m_undecorate ) // Undecorate Name
						{
				 			char Textout[SIZEBUFFER];
							strTemp.Format("%s", Undecorate((char *) (LPCSTR) func, Textout, SIZEBUFFER));
						}
						else strTemp.Format("%s", func);
					}
					if (m_Hsize < strTemp.GetLength()) m_Hsize = strTemp.GetLength();
					m_list.AddString( strTemp );
					m_nbfunc ++;
				} while( pos );
			}
		}
	}
	else
	{
// Export Section
		PIMAGE_EXPORT_DIRECTORY exportDir = m_pe->GetExportsDesc();
		m_nbfunc = 0;
		if ( exportDir  && m_pe->IsValidPtr(( DWORD) exportDir )) 
		{
			PSTR	filename = (PSTR)m_pe->GetReadablePointerFromRVA( exportDir->Name );
			if (!(((DWORD) filename > (DWORD) exportDir + m_pe->GetFileSize()) || ((DWORD) filename < (DWORD) exportDir)))
			{
				PDWORD functions = ( PDWORD ) m_pe->GetReadablePointerFromRVA( exportDir->AddressOfFunctions );
				PWORD ordinals = (PWORD) m_pe->GetReadablePointerFromRVA( exportDir->AddressOfNameOrdinals );
				PDWORD name = ( PDWORD ) m_pe->GetReadablePointerFromRVA( exportDir->AddressOfNames);			
				m_nbfunc =exportDir->NumberOfNames;
				for(i=0; i < (int) exportDir->NumberOfFunctions; i++)
				{
					BOOL found = FALSE;
					DWORD entryPointRVA = functions[i];
					if ( entryPointRVA == 0 ) continue; // Skip over gaps in exported function
					for ( int j=0; j < (int) exportDir->NumberOfNames; j++ )
						if ( ordinals[j] == i )
						{
							found = TRUE;
							PSTR Name = (PSTR) m_pe->GetReadablePointerFromRVA( name[j] );
							if ( m_undecorate ) 	// Undecorate Name
							{
								char Textout[SIZEBUFFER];
								m_list.AddString( Undecorate(Name, Textout, SIZEBUFFER));								
							}
							else m_list.AddString( Name );
							int size = strlen(Name);
							if (m_Hsize < size) m_Hsize = size;
						}
					if (!found) 
					{
						strTemp.Format( "ordinal %d", i );
						m_list.AddString( strTemp );
					}

				}
			}
		}
	}
	UpdateData(FALSE);
}

void CListExport::OnSelchangeModules() 
{
	CWait wait(this);
	wait.SetStatus("Listing Functions...");
	int sel = m_listmodule.GetCurSel( );
	if (sel == -1) return;
	AddFunction(sel);
	m_list.SetHorizontalExtent( m_Hsize *10 );
	m_listmodule.SetFocus();
}

void CListExport::Onundecorate() 
{
	// UpdateData(TRUE);
	m_undecorate = !m_undecorate;
	int sel = m_list.GetCurSel( );
	AddFunction(m_listmodule.GetCurSel( ));
	m_list.SetHorizontalExtent( m_Hsize * 10 );
	m_list.SetCurSel( sel );
	UpdateData(TRUE);
}

void CListExport::OnTestImport() 
{
	int sel = m_listmodule.GetCurSel( );
	if (sel)
	{
		MODULE_DEPENDENCY_LIST *pDep = m_pe->GetDepends();
		if (pDep)
		{
			PMODULE_FILE_INFO pModInfo;
			CString Module; int decal = 0; //BOOL bdelay=FALSE;

			m_listmodule.GetText( sel, Module );
			if ( sel <= m_NbIF ) // import
				pModInfo = pDep->GetNextModule( (PMODULE_FILE_INFO) 0 );
			else 
			{
				decal = 20;
				pModInfo = pDep->GetNextDelayedModule( (PMODULE_FILE_INFO) 0 );
			}

			if (!pModInfo) return;
			while(strncmp(Module, pModInfo->GetBaseName(), Module.GetLength()-decal))
			{
				if (decal) pModInfo = pDep->GetNextDelayedModule( pModInfo );
				else pModInfo = pDep->GetNextModule( pModInfo );
				if (!pModInfo) return;
			}
			if (decal) Module.SetAt( Module.GetLength()-decal, '\0');
			CStringList *pFlist = pModInfo->GetFList();
			POSITION pos = pFlist->GetHeadPosition();

			int error=0;
			char szPath[MAX_PATH];
			char szOriginalPath[MAX_PATH];
			GetCurrentDirectory(MAX_PATH, szOriginalPath);  // Save original dir
			SetCurrentDirectory( m_pe->GetPath() );				 // Switch to app's dir

			PSTR pszDontCare;
			if ((pos) && SearchPath(0, Module, 0, MAX_PATH, szPath, &pszDontCare))
			{
				HINSTANCE h;
				if (b_W95Protect)
					h = LoadLibraryEx(szPath, NULL, LOAD_LIBRARY_AS_DATAFILE);
				else 
					h = LoadLibraryEx(szPath, NULL, DONT_RESOLVE_DLL_REFERENCES);  				
				if (h)
				{
					CStringList StrL;
					CString func;
					do {
						func = pFlist->GetNext(pos);
						if (strncmp("ordinal ", func, 8)==0)
						{
							if (!GetProcAddress( h, (const char *) MAKELONG(atoi((LPCTSTR) func + 8),0)))
								error ++;
						}
						else if (!GetProcAddress( h, func ))
						{
							error ++;
							StrL.AddTail(func);
						}
					} while( pos );
					if (!error) AfxMessageBox("Module and all imported functions loaded");
					else {
						ListDlg ldlg(&StrL);
						ldlg.DoModal();
					}
					FreeLibrary( h );		
				} else AfxMessageBox("Cannot load Module ");
			} else AfxMessageBox("Cannot find Module ");
			SetCurrentDirectory( szOriginalPath );
		}/**/
	}
}

void CListExport::OnSelchangeFunc() 
{
	int sel = m_list.GetCurSel( );
	if (sel == -1) return;
	COleDataSource* pData = new COleDataSource;
  
	CSharedFile sf(GMEM_MOVEABLE|GMEM_DDESHARE|GMEM_ZEROINIT);
	CString	func;
	m_list.GetText( sel, func);
	sf.Write(func, func.GetLength()); // you can write to the clipboard as you would to any cfile

	HGLOBAL  hmem = sf.Detach();
	if (!hmem) return;
	
	pData->CacheGlobalData( CF_TEXT, hmem );
	pData->SetClipboard();

}

void CListExport::OnSort() 
{
	// UpdateData(TRUE);
	m_bsort = !m_bsort;
	OnToggleListStyle();
	int sel = m_listmodule.GetCurSel( );
	if (sel == -1) return;
	AddFunction(sel);
	m_list.SetHorizontalExtent( m_Hsize * 10 );
	UpdateData(TRUE);
}

BOOL CListExport::PreTranslateMessage(MSG* pMsg) 
{
   if (pMsg->message == WM_KEYDOWN ) 
   {
		if (pMsg->wParam == 85)
		{
			Onundecorate();
			return 0;
		}
		if (pMsg->wParam == 83)
		{
			OnSort();
			return 0;
		}
		if (pMsg->wParam == VK_F3)
		{
			int sel = m_listmodule.GetCurSel( );
			if (sel)
			{
				MODULE_DEPENDENCY_LIST *pDep = m_pe->GetDepends();
				if (pDep)
				{
					PMODULE_FILE_INFO pModInfo;
					CString Module; int decal = 0; //BOOL bdelay=FALSE;

					m_listmodule.GetText( sel, Module );
					if ( sel <= m_NbIF ) // import
						pModInfo = pDep->GetNextModule( (PMODULE_FILE_INFO) 0 );
					else 
					{
						decal = 20;
						pModInfo = pDep->GetNextDelayedModule( (PMODULE_FILE_INFO) 0 );
					}

					if (!pModInfo) return 0;
					while(strncmp(Module, pModInfo->GetBaseName(), Module.GetLength()-decal))
					{
						if (decal) pModInfo = pDep->GetNextDelayedModule( pModInfo );
						else pModInfo = pDep->GetNextModule( pModInfo );
						if (!pModInfo) return 0;
					}
					if (pModInfo->IsModuleFound())
					{
						SHELLEXECUTEINFO sei  = { sizeof(sei ) };
						sei.fMask = SEE_MASK_DOENVSUBST;
						sei.nShow = SW_SHOWNORMAL;
				//		sei.lpVerb = argv[1];
						sei.lpFile = "%commander_exe%";
						CString com = "/S=L \" " + (CString) pModInfo->GetFullName() + "\"";
						sei.lpParameters = com;
						if(ShellExecuteEx(&sei) == FALSE)
						{
							CString mess;
							mess.Format("ShellExecuteEx Error \narg : %s",(const char *)com);
							AfxMessageBox(mess,MB_ICONEXCLAMATION);
						}
					}
				}
			}
			return 0; 
		}
	}
	return CResizePage::PreTranslateMessage(pMsg);
}