    /***********************************************************/
    /****   Programme :  Plug-in for Total Commander       *****/
    /***************  Auteur :    Fr GANNIER   *****************/
    /***************     Lab. Physio. Anim.    *****************/
    /***************      Fac. des Sciences    *****************/
    /***************        37000 Tours        *****************/
    /***********************************************************/

#include "stdafx.h"

#include "..\common\listplug.h"
#include "FileinfoListWnd.h"
#include "tcoptions.h"

#include "fileinfotext.h"
#include "peexe\peexe.h"
#include "TypeLib.h"
#include "..\..\common\verwin.h"

#include "..\common\TCutils.h"
// #include "vld.h"

#ifndef _WIN32
   #define	 DLL_EXPORT(x)   x __export WINAPI 
#else  
   #define   DLL_EXPORT(x)   __declspec(dllexport) x   WINAPI 
#endif //_WIN32   

#ifdef _DEBUG
   #define new DEBUG_NEW
   #undef THIS_FILE
   static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////
// CListplugApp
BEGIN_MESSAGE_MAP(CListplugApp, CWinApp)
   //{{AFX_MSG_MAP(CListplugApp)
      // NOTE - the ClassWizard will add and remove mapping macros here.
      //    DO NOT EDIT what you see in these blocks of generated code!
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

////////////////////////////////////////////////////////////////////////////
/// Variables globales
HINSTANCE  hInst=0;

BOOL fShowRelocations		= FALSE; // NON !!!
BOOL fShowRawSectionData	= FALSE; // pas necessa
BOOL fShowSymbolTable		= TRUE; // FALSE; // Debug info
BOOL fShowLineNumbers		= TRUE; // FALSE; // for debug info
BOOL fShowIATentries		= FALSE; // Import Address Table ( to add) 
BOOL fShowPDATA				= TRUE; // FALSE;
BOOL fShowResources			= TRUE; // FALSE;

BOOL fShowUndecorated		= TRUE;  // fg
BOOL b_W95Protect			= TRUE;  // fg
BOOL b_disass				= FALSE;

int MaxDepth;


// const char tlexc[] = "cvs,csv,xls";

#define ModuleName "fileinfo.wlx"
char inifilename[MAX_PATH]="fileinfo.ini";
//char inifilename[MAX_PATH]="lsplugin.ini"; 
char iniPath[MAX_PATH]="";

CListplugApp theApp("FILEINFO_WLX");
OPTIONS op={0,0};						// SET Option by default (Voir TCoptions.h)
FontOptions fo={"",0,FALSE,FALSE};		// SET Values by default
bool calledbyQV;

void LoadPlugin(CString inifilename)
{
	int i=0; // Chargement des plugins
	char buff[512], nbstr[3]="0"; 
	(DecPlugin::pluglist).RemoveAll();
	while ( GetPrivateProfileString( "plugins", nbstr, NULL, buff, 511, inifilename))
	{
		char str[512]; // 
		ExpandEnvironmentStrings( buff, str, 510);
#ifdef _WIN64
		strcat(str, "64"); 
#endif
		(DecPlugin::pluglist).AddTail(str);
		if (++i > 9) break;
		nbstr[0] = '0' + i; // incrémenter compteur
	}
}


//PIMAGE_RESOURCE_DIRECTORY GetResDir(PE_EXE &pe);
bool IsMFTres(PE_EXE &pe);

int __stdcall ListLoadNext(HWND ParentWin,HWND ListWin,char* File,int ShowFlags)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString FileToLoad(File);
	CWnd *pFocusedWnd, *pParentWnd = CWnd::FromHandle(ParentWin);
	CFileinfoListWnd *pFIWnd = (CFileinfoListWnd *) CWnd::FromHandle(ListWin);
	ASSERT(pFIWnd);

	CListSheet *sheet = (CListSheet *)pFIWnd->GetpSheet();
//	OPTIONS op = sheet->m_option.GetOptionDefault();
/**********  Test de l'extension ************/
	{   // Test si Fichier !!
		if (strlen(FileToLoad)==0) return LISTPLUGIN_ERROR;
		//  Test si path == ".."	
		PSTR pszEnd = (PSTR) strrchr( FileToLoad, '\\' );
		if ( !strnicmp( ++pszEnd, "..", 2) )
			return LISTPLUGIN_ERROR;
	}
	PE_EXE *pPE = new PE_EXE( File );
// different access on Mapped file
	MEMORY_MAPPED_FILE *pMapFile = (MEMORY_MAPPED_FILE *) pPE;
	OBJ_FILE_TYPE FileType = OBJ_UNKNOWN;
	EXE_FILE *pEXE = (EXE_FILE *) pPE;  // acces à tous les EXEs

	LPTYPELIB lpTypeLib = NULL;
	if (GetPrivateProfileInt( "Options", "ComInt", 1, inifilename))
	{
		FileType = DisplayObjectFile( pMapFile );
		if (pEXE->IsValid())
			CTypeLib::IsTypeLib(FileToLoad, lpTypeLib);
		else if (FileType == OBJ_TL)
			CTypeLib::IsTypeLib(FileToLoad, lpTypeLib); /**/
	}

	bool b_TLib = (lpTypeLib != NULL);
	if ( pEXE->IsValid() || (FileType != OBJ_UNKNOWN) || lpTypeLib) 
	{
		UINT page = pFIWnd->GetpSheet()->GetActiveIndex( );
		pFIWnd->Renew( pPE );
		if (pPE->IsValid())
		{
			sheet->m_dll.Renew(pPE);
			sheet->AddPage( &(sheet->m_dll));
			
			sheet->m_export.Renew(pPE);
			sheet->m_export.SetSortOption(op.sort);
			sheet->AddPage( &(sheet->m_export));
// is OCX file
			if ( lpTypeLib ) // com interface exist
			{
				sheet->m_ocx.Renew(pPE);
				sheet->AddPage( &(sheet->m_ocx));
				lpTypeLib->Release();
			}
			else 
			{
				if(pPE->IsGoodExport())
				{	
					HINSTANCE hDLL;
					if (b_W95Protect)
						hDLL = LoadLibraryEx(pPE->GetName(), NULL, LOAD_LIBRARY_AS_DATAFILE);
					else 
						hDLL = LoadLibraryEx(pPE->GetName(), NULL, DONT_RESOLVE_DLL_REFERENCES); 
					HCRET lpfnDllRegisterServer = (HCRET) GetProcAddress(hDLL, "DllRegisterServer");
					b_TLib = (lpfnDllRegisterServer != NULL);
					//if (lpfnDllRegisterServer )
					if (b_TLib)
						sheet->m_ocx.Renew(pPE);
						sheet->AddPage( &(sheet->m_ocx));
					FreeLibrary( hDLL );
				}
			}/**/
		if (pPE->IsValid())
			sheet->m_dll.SetFillTree(CreateParentTree);	// passer la fonction qui remplit l'arbre
		}
		else 
// is LIB or OBJ or TLB file
		{
			if (lpTypeLib)
			{
				CString strTp;
				CTypeLib::GetTypeLibAsString(lpTypeLib, strTp);
				lpTypeLib->Release();
				sheet->SetPageTitle (1, "Typelib Info");
				sheet->m_fi2.SetText(strTp);
			}
			else
			{
				sheet->SetPageTitle (1, "Library Header");
				sheet->m_fi2.SetFillEdit(CreateText2);			// Dynamic Fill RichEdit
			}
		}

// Manifest window
		bool b_manifest = (bool) IsMFTres(*pPE);
		if (b_manifest) {
			sheet->m_manifest.Renew(pPE);
			sheet->AddPage( &(sheet->m_manifest));
			sheet->SetPageTitle (4 + b_TLib, "Manifest");
			sheet->m_manifest.SetFontPpty( fo );

			int tab0[]={(fo.fontsize<200)?400:fo.fontsize*3, (fo.fontsize<200)?3200:fo.fontsize*25, (fo.fontsize<200)?5500:fo.fontsize*43, (fo.fontsize<200)?6000:fo.fontsize*45};
			sheet->m_fi.SetTab(4, tab0);
//			sheet->m_manifest.SetFillEdit(CreateManifest);
		}
// disassembler window
		if ((b_disass) && (pPE->IsValid()))
		{
			FontOptions fo={"courier",160,FALSE,FALSE};

			sheet->AddPage( &(sheet->m_disass));
			sheet->SetPageTitle (4 + b_TLib + b_manifest, "Disassembly");
			sheet->m_disass.SetFontPpty( fo );

			sheet->m_disass.SetPtr( pPE );
			// Dynamic Fill RichEdit
			sheet->m_disass.SetFillEdit(CreateText3);			
		}
		sheet->AddPage( &(sheet->m_option));
		sheet->AddPage( &(sheet->m_about));

		sheet->m_fi.SetText(CreateText0( FileToLoad));

		sheet->SetActivePage( page );

//**********  Transfert Option  *********************/
//			sheet->m_option.SetOptionDefault(op);
		//			if (op.rememberAP ) sheet->SetActivePage( page );
	
// retirer tous les onglets et remettre les bons
// voir mmedia & mask

//	pMediaWnd->SetFile(FileToLoad, audio);
		return LISTPLUGIN_OK;
	}
	return LISTPLUGIN_ERROR;
}/**/

HWND __stdcall ListLoad(HWND ParentWin,char* File,int ShowFlags)
{
   CWnd *pFocusedWnd, *ParentWnd = CWnd::FromHandle(ParentWin);
   CFileinfoListWnd *prec;
   CString FileToLoad(File);
   UINT page = 0;

/**********  Test de l'extension ************/
	{   // Test si Fichier !!
		if (strlen(FileToLoad)==0) return NULL;
		//  Test si path == ".."	
		PSTR pszEnd = (PSTR) strrchr( FileToLoad, '\\' );
		if ( !strnicmp( ++pszEnd, "..", 2) )
			return NULL;
	}
//  Test file validity
    PE_EXE *pPE = new PE_EXE( File );			// pointer sur PE
// different access on Mapped file
	MEMORY_MAPPED_FILE *pMapFile = (MEMORY_MAPPED_FILE *) pPE;
	OBJ_FILE_TYPE FileType = OBJ_UNKNOWN;
	EXE_FILE *pEXE = (EXE_FILE *) pPE;  // acces à tous les EXEs
	
//************     Pre-Config        ***************/
	GetIniFilePath(hInst, inifilename);

	LPTYPELIB lpTypeLib = NULL;
	if (GetPrivateProfileInt( "Options", "ComInt", 1, inifilename))
	{
		FileType = DisplayObjectFile( pMapFile );
		if (pEXE->IsValid())
			CTypeLib::IsTypeLib(FileToLoad, lpTypeLib);
		else if (FileType == OBJ_TL)
			CTypeLib::IsTypeLib(FileToLoad, lpTypeLib); /**/
		else if ((FileType != OBJ_COFF_OBJ) && (FileType != OBJ_COFF_LIB))
			FileType = OBJ_UNKNOWN;
	}

	if ( pEXE->IsValid() || (FileType != OBJ_UNKNOWN) || lpTypeLib) 
	{
//************     Charge Config        ***************/
		op.rememberAP = GetPrivateProfileInt( "Options", "RememberAP", 1, inifilename);
		if (op.rememberAP)
			page = GetPrivateProfileInt( "Options", "ActivePage", 0, inifilename);
//		op.autosave = GetPrivateProfileInt( "Options", "Autosave", 1, inifilename);
		fShowUndecorated = op.undec = GetPrivateProfileInt( "Options", "Undecorate", 1, inifilename);
		fShowSymbolTable = op.debug = GetPrivateProfileInt( "Options", "ShowDebug", 0, inifilename);
		fShowResources  = op.res = GetPrivateProfileInt( "Options", "Dump_res", 0, inifilename);
		fShowPDATA  = op.pdata = GetPrivateProfileInt( "Options", "Dump_Pdata", 0, inifilename);
		fShowRelocations = op.reloc = GetPrivateProfileInt( "Options", "Dump_Reloc", 0, inifilename);
		MaxDepth = op.MaxDepth = GetPrivateProfileInt( "Options", "MaxDepth", 2, inifilename);
		op.sort = GetPrivateProfileInt( "Options", "Sort", 1, inifilename);
		op.p_ini=inifilename;
		
		b_W95Protect = (GetSystemVersion().ver < WND_NT )?GetPrivateProfileInt( "Options", "W95Protect", 1, inifilename):FALSE;
		b_disass = (GetPrivateProfileInt( "Options", "Disassemble", 0, inifilename) != 0);

		if ( FileType == OBJ_UNKNOWN ) LoadPlugin(inifilename);
//************     Fin Charge Config      ***************/
		if (GetPrivateProfileInt( "Options", "UserFont", 1, inifilename)==1)
			fo = GetFontOptions(ParentWnd, iniPath);

		AFX_MANAGE_STATE(AfxGetStaticModuleState());
		RECT r;
		bool calledbyQV = IsCalledForQV( ParentWnd );
		if ( calledbyQV )
			pFocusedWnd = ParentWnd->GetFocus( );
		else if (op.autosave = GetPrivateProfileInt( "Options", "autosave", 1, inifilename))
		{
			RECT r1;
			if (GetPrivateProfileStruct("Options", "Rect", &r1, sizeof(r1), inifilename))
				ParentWnd->MoveWindow(&r1);
		}
		prec = new CFileinfoListWnd( pPE );
//		prec->SetOptionDefault( op );
		ParentWnd->GetClientRect(&r); 
		if (prec->Create(NULL, NULL, WS_CHILD | WS_VISIBLE | ES_READONLY | ES_NOHIDESEL, r, ParentWnd, (UINT) hInst))
		{
			bool b_TLib = (lpTypeLib != NULL);
// Setup Page
			CListSheet *sheet = (CListSheet *)prec->GetpSheet();
			if ( calledbyQV ) sheet->EnableWindow();
// Add Page
			if (pEXE->IsValid())
			{ 
// is PE file ?
				sheet->SetPageTitle (1, "Image File Header");

				if (pPE->IsValid())
				{
					sheet->AddPage( &(sheet->m_dll));
					sheet->m_export.SetSortOption(op.sort);
					sheet->AddPage( &(sheet->m_export));					
// is OCX file
					if ( lpTypeLib ) // com interface exist
					{
						sheet->AddPage( &(sheet->m_ocx));
						lpTypeLib->Release();
					}
					else 
					{
						if(pPE->IsGoodExport())
						{	
							HINSTANCE hDLL;
							if (b_W95Protect)
								hDLL = LoadLibraryEx(pPE->GetName(), NULL, LOAD_LIBRARY_AS_DATAFILE);
							else 
								hDLL = LoadLibraryEx(pPE->GetName(), NULL, DONT_RESOLVE_DLL_REFERENCES); 
							HCRET lpfnDllRegisterServer = (HCRET) GetProcAddress(hDLL, "DllRegisterServer");
							b_TLib = (lpfnDllRegisterServer != NULL);
							//if (lpfnDllRegisterServer )
							if (b_TLib)
								sheet->AddPage( &(sheet->m_ocx));
							FreeLibrary( hDLL );
						}
					}/**/
				}
				sheet->m_ocx.SetPtr( pPE );
				sheet->m_export.SetPtr( pPE );
				sheet->m_dll.SetPtr( pPE );	
	
				sheet->m_ocx.SetFontPpty( fo );
				if (pPE->IsValid())
					sheet->m_dll.SetFillTree(CreateParentTree);	// passer la fonction qui remplit l'arbre
				sheet->m_fi2.SetFillEdit(CreateText1);			// Dynamic Fill RichEdit
			}
			else 
// is LIB or OBJ or TLB file
			{
				if (lpTypeLib)
				{
					CString strTp;
					CTypeLib::GetTypeLibAsString(lpTypeLib, strTp);
					lpTypeLib->Release();
					sheet->SetPageTitle (1, "Typelib Info");
					sheet->m_fi2.SetText(strTp);
				}
				else
				{
					sheet->SetPageTitle (1, "Library Header");
					sheet->m_fi2.SetFillEdit(CreateText2);			// Dynamic Fill RichEdit
				}
			}
// add common
			sheet->m_fi2.SetPtr( pPE );

// Manifest window
			bool b_manifest = (bool) IsMFTres(*pPE);
			if (b_manifest) {
				sheet->AddPage( &(sheet->m_manifest));
				sheet->m_manifest.SetPtr( pPE );
				sheet->SetPageTitle (4 + b_TLib, "Manifest");
				sheet->m_manifest.SetFontPpty( fo );
	//			int tab0[]={(fo.fontsize<200)?400:fo.fontsize*3, (fo.fontsize<200)?3200:fo.fontsize*25, (fo.fontsize<200)?5500:fo.fontsize*43, (fo.fontsize<200)?6000:fo.fontsize*45};
	//			sheet->m_fi.SetTab(4, tab0);
				sheet->m_manifest.SetFillEdit(CreateManifest);
			}
// disassembler window
			if ((b_disass) && (pPE->IsValid()))
			{
				FontOptions fo={"courier",160,FALSE,FALSE};
				sheet->AddPage( &(sheet->m_disass));
				sheet->SetPageTitle (4 + b_TLib + b_manifest, "Disassembly");
				sheet->m_disass.SetFontPpty( fo );

				sheet->m_disass.SetPtr( pPE );
				// Dynamic Fill RichEdit
				sheet->m_disass.SetFillEdit(CreateText3);			
			}
			sheet->AddPage( &(sheet->m_option));
			sheet->AddPage( &(sheet->m_about));

			sheet->m_fi.SetText(CreateText0( FileToLoad));

			sheet->m_fi.SetFontPpty( fo );
			int tab0[]={(fo.fontsize<200)?400:fo.fontsize*3, (fo.fontsize<200)?3200:fo.fontsize*25, (fo.fontsize<200)?5500:fo.fontsize*43, (fo.fontsize<200)?6000:fo.fontsize*45};
			sheet->m_fi.SetTab(4, tab0);

			sheet->m_fi2.SetFontPpty( fo );
			int tab[]={(fo.fontsize<200)?400:fo.fontsize*3, (fo.fontsize<200)?3200:fo.fontsize*25, (fo.fontsize<200)?5500:fo.fontsize*43, (fo.fontsize<200)?6000:fo.fontsize*45};
			sheet->m_fi2.SetTab(4, tab);
			sheet->Resize(TRUE);

//**********  Transfert Option  *********************/
//			sheet->m_option.SetOptionDefault(op);
			if (op.rememberAP ) sheet->SetActivePage( page );
		} 
		if ( calledbyQV && pFocusedWnd)		//	->IsWindow())
			pFocusedWnd->SetFocus();
	} else { delete pPE; pPE = NULL; return NULL; }
	return prec->GetSafeHwnd();
}

void __stdcall ListGetDetectString(char* DetectString,int maxlen)
{
//	strncpy(DetectString,"MULTIMEDIA | (SIZE>0)",maxlen);
//	strncpy(DetectString,"size>0",maxlen);
}

void __stdcall ListSetDefaultParams(ListDefaultParamStruct* dps)
{
//   dps->size;
//   dps->PluginInterfaceVersionLow;
//   dps->PluginInterfaceVersionHi;
	strncpy(iniPath,dps->DefaultIniName,MAX_PATH-1);
	GetPath(iniPath);
	
	strcat(iniPath, inifilename);
	strncpy(inifilename,iniPath,MAX_PATH-1);
	
	GetPath(iniPath);
	strcat(iniPath, "wincmd.ini");
}

int __stdcall ListSendCommand(HWND ListWin,int Command,int Parameter)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
   CFileinfoListWnd *rec = (CFileinfoListWnd *) CWnd::FromHandlePermanent(ListWin);
   ASSERT(rec);
   switch (Command) {
      case lc_copy:
         ((CResizePage *) rec->GetpSheet()->GetActivePage())->PostMessage(WM_COPY,0,0);
         return LISTPLUGIN_OK;
      case lc_newparams:
/*			switch (Command) 
				case lcp_wraptext	:
//					rec->GetpSheet()->;
				return LISTPLUGIN_OK; /**/
         return LISTPLUGIN_OK;
      case lc_selectall:
         ((CResizePage *) rec->GetpSheet()->GetActivePage())->PostMessage(EM_SETSEL,0,-1);
         return LISTPLUGIN_OK;
   }
   return LISTPLUGIN_ERROR;
}

void __stdcall ListCloseWindow(HWND ListWin)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	char temp[50]; //OPTIONS op;
	CFileinfoListWnd *pWin = ( CFileinfoListWnd *) CFileinfoListWnd::FromHandlePermanent(ListWin);
//	op = ((CListSheet *) pWin->GetpSheet())->m_option.GetOptionDefault();
	if(((CListSheet *)pWin->GetpSheet())->m_option.Update()) {
		WritePrivateProfileString( "Options", "RememberAP", _itoa(op.rememberAP, temp, 10), inifilename);
		WritePrivateProfileString( "Options", "Autosave", _itoa(op.autosave, temp, 10), inifilename);
		WritePrivateProfileString( "Options", "Undecorate", _itoa(op.undec, temp, 10), inifilename);
		WritePrivateProfileString( "Options", "ShowDebug", _itoa(op.debug, temp, 10), inifilename);
		WritePrivateProfileString( "Options", "Dump_res", _itoa(op.res, temp, 10), inifilename);
		WritePrivateProfileString( "Options", "MaxDepth", _itoa(op.MaxDepth, temp, 10), inifilename);
		WritePrivateProfileString( "Options", "Dump_Pdata", _itoa(op.pdata, temp, 10), inifilename);
		WritePrivateProfileString( "Options", "Dump_reloc", _itoa(op.reloc, temp, 10), inifilename);
	}
//	WritePrivateProfileString( "Options", "Sort", _itoa(op.sort, temp, 10), inifilename);
	
	if ( op.rememberAP )
	{
		UINT i = pWin->GetpSheet()->GetActiveIndex( );
		WritePrivateProfileString( "Options", "ActivePage", _itoa(i, temp, 10), inifilename);
	}
	if (op.autosave)   
	{
		CWnd *ParentWnd = pWin->GetParent(); 
		if ( !IsCalledForQV( ParentWnd ) )
		{
			if (!ParentWnd->IsZoomed( ) && !ParentWnd->IsIconic( ))
			{
				RECT r; 
				ParentWnd->GetWindowRect( &r );
				WritePrivateProfileStruct("Options", "Rect", &r, sizeof(r), inifilename);
			}
		}
	}

	pWin->DestroyWindow();
	delete (pWin);
	TRACE0("Destroy ListWin \n");
	DestroyWindow(ListWin);
	TRACE0("FileInfo : ListWin Destroyed \n");
	return;
}

int __stdcall ListPrint(HWND ListWin, char* FileToPrint, char* DefPrinter, int PrintFlags, RECT* Margins)
{
   return 0;
}

/*
int  __stdcall ListSearchText(HWND ListWin,char* SearchString,int SearchParameter)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CFileinfoListWnd *pWin = ( CFileinfoListWnd *) CFileinfoListWnd::FromHandlePermanent(ListWin);
	CListSheet *sheet = (CListSheet *)pWin->GetpSheet();
	sheet->	


/*
	FINDTEXT find;
	int StartPos, Flags;

	if (SearchParameter & lcs_findfirst) {
		//Find first: Start at top visible line
		StartPos=SendMessage(ListWin,EM_LINEINDEX,SendMessage(ListWin,EM_GETFIRSTVISIBLELINE,0,0),0);
		SendMessage(ListWin,EM_SETSEL,StartPos,StartPos);
	} else {
		//Find next: Start at current selection+1
		SendMessage(ListWin,EM_GETSEL,(WPARAM)&StartPos,0);
		StartPos+=1;
	}

	find.chrg.cpMin=StartPos;
	find.chrg.cpMax=SendMessage(ListWin,WM_GETTEXTLENGTH,0,0);
	Flags=0;
	if (SearchParameter & lcs_wholewords)
		Flags |= FR_WHOLEWORD;
	if (SearchParameter & lcs_matchcase)
		Flags |= FR_MATCHCASE;
	if (!(SearchParameter & lcs_backwards))
		Flags |= FR_DOWN;
	find.lpstrText=SearchString;
	int index=SendMessage(ListWin, EM_FINDTEXT, Flags, (LPARAM)&find);
	if (index!=-1) {
	  int indexend=index+strlen(SearchString);
	  SendMessage(ListWin,EM_SETSEL,index,indexend);
	  int line=SendMessage(ListWin,EM_LINEFROMCHAR,index,0)-3;
	  if (line<0)
		  line=0;
      line-=SendMessage(ListWin,EM_GETFIRSTVISIBLELINE,0,0);
      SendMessage(ListWin,EM_LINESCROLL,0,line);
	  return LISTPLUGIN_OK;
	} else {
		SendMessage(ListWin,EM_SETSEL,-1,-1);  // Restart search at the beginning
	}
	return LISTPLUGIN_ERROR;


	return 0; // LISTPLUGIN_OK or LISTPLUGIN_ERROR.


} /**/

/////////////////////////////////////////////////////////////////////////////
// Public C interface ( FG )

BOOL CListplugApp::InitInstance() 
{
   BOOL val = CWinApp::InitInstance();
   hInst = m_hInstance;
   TRACE1("fileinfo : InitInstance : %d\n", m_hInstance);

#ifdef _AFXDLL
//	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

   if (val)
   {
      if (!AfxInitRichEdit( ))
      {   // IDS_RICHED_LOAD_FAIL
         AfxMessageBox("Fail to Load RICHEDIT library", MB_OK|MB_ICONEXCLAMATION);
         return FALSE;
      } /**/
//      AfxEnableControlContainer();  // enable for Activex
   }
   return val;
}

CListplugApp::CListplugApp(LPCTSTR pszAppName) : CWinApp(pszAppName) 
{    
   TRACE0("fileinfo : plug-in initialized\n");
}
