//==========================================
// Matt Pietrek
// Microsoft Systems Journal, Feb 1997
// FILE: DEPENDENCYLIST.CPP
//==========================================
#include "stdafx.h"

#include <windows.h>
#include <malloc.h>
#pragma hdrstop
#include "dependencylist.h"
#include <delayimp.h>

#include "..\..\..\common\ffile.h"

BOOL test_Name( PCHAR str);

MODULE_DEPENDENCY_LIST::MODULE_DEPENDENCY_LIST(  )
{
	m_errorType = errMDL_GENERAL_FAILURE;
	m_cModules = 0;
	m_cDelayedModules = 0;
	m_pList = 0;
	m_pListDelayed = 0;
	m_pPE = NULL;
}

void MODULE_DEPENDENCY_LIST::LoadPE( PE_EXE *pPE )
{
	m_pPE = pPE;

	// If a path was part of the input filename, save the current directory, then switch to the new directory.
	char szOriginalPath[MAX_PATH];
	if ( *(pPE->GetPath()) != '\0')
	{	
		GetCurrentDirectory(MAX_PATH, szOriginalPath);  // Save original dir
		SetCurrentDirectory( pPE->GetPath() );				 // Switch to app's dir
	}

	m_errorType = AddDirectModule( );
	m_errorType = AddDirectDelayedModule( );
	 
	if ( *(pPE->GetPath()) != '\0')
		SetCurrentDirectory( szOriginalPath );
}

MODULE_DEPENDENCY_LIST::MODULE_DEPENDENCY_LIST( PE_EXE *pPE )
{
	MODULE_DEPENDENCY_LIST(  );
	LoadPE( pPE );
}

void MODULE_DEPENDENCY_LIST::DeletePList(PMODULE_FILE_INFO pList)
{
	 PMODULE_FILE_INFO pTemp;
	 // Delete each MODULE_FILE_INFO structures in the regular linked list
	 pTemp = pList;
	 while ( pTemp )
	 {
		  pTemp = pList->m_pNext;
		  delete pList;
		  pList = pTemp;
	 }
 	m_cModules = 0;
	m_cDelayedModules = 0;

}

MODULE_DEPENDENCY_LIST::~MODULE_DEPENDENCY_LIST( )
{
	DeletePList( m_pList );
	DeletePList( m_pListDelayed );

	m_pList = 0;
	m_pListDelayed = 0;
}

// Given the name of a file, find the MODULE_FILE_INFO structure that
// represents it.  The fFullName parameter specifies whether the full path
// names or just the base file names will be compared.
PMODULE_FILE_INFO MODULE_DEPENDENCY_LIST::LookupModule( PSTR pszFileName, BOOL fFullName )
{
	PMODULE_FILE_INFO p = m_pList;  // Start at the list head
	while ( p ) // While there's still entries in the list...
	{
		PSTR pszCompName = fFullName ? p->m_szFullName : p->m_szBaseName;
		if ( 0 == lstrcmpi( pszFileName, pszCompName ) )
			return p;
		p = p->m_pNext;
	}
	return 0;
}

PSTR MODULE_DEPENDENCY_LIST::GetErrorString( void )
{
	switch ( m_errorType )
	{
		case errMDL_NO_ERROR:		return "No error";
		case errMDL_FILE_NOT_FOUND:	return "File not found";
		case errMDL_NOT_PE_FILE:	return "Not a PE file";
		case errMDL_GENERAL_FAILURE:return "General failure";
		default:					return "<Error ???>";
	}
}

errModuleDependencyList MODULE_DEPENDENCY_LIST::AddDirectModule()
{
	PSTR pszBaseName;

	if (m_pList) 	DeletePList( m_pList );
	m_pList = new MODULE_FILE_INFO( (const PSTR) m_pPE->GetName(), 0, FALSE);
	if ( !m_pList ) return errMDL_GENERAL_FAILURE;	
	PMODULE_FILE_INFO pNew = m_pList;
	PMODULE_FILE_INFO pTemp;
	BOOL  findyet = FALSE;
// correct bug from Cygwin
	ULONG lastNameAdd = -1;

	m_cModules++;
	if ( FALSE == m_pPE->IsValid() )	 // A valid PE file???
		return (errModuleDependencyList)m_pPE->GetErrorType();
	if (0 == m_pPE->GetDataDirectoryEntrySize( IMAGE_DIRECTORY_ENTRY_IMPORT )) 
		return errMDL_NO_ERROR;

	PIMAGE_IMPORT_DESCRIPTOR pImportDir = m_pPE->GetImportsDesc();
	DWORD Importsz = m_pPE->GetImportsSz();
	if ( !pImportDir ) return errMDL_NO_ERROR;
	if ( !m_pPE->IsValidPtr( (DWORD) pImportDir))
	{
		pNew->m_pNext = new MODULE_FILE_INFO( "Error in IMPORT TABLE", 0, FALSE);
		return errMDL_NO_ERROR;
	} 
	while ( pszBaseName = (PSTR) m_pPE->GetReadablePointerFromRVA(pImportDir->Name))
	{
// correct bug from Cygwin
		if ( lastNameAdd == pImportDir->Name)
		{
			pImportDir++;
			continue;
		} lastNameAdd = pImportDir->Name;
// end
		char szPath[MAX_PATH];
		PSTR pszDontCare;
		if ( pszBaseName )
		{
			pTemp = LookupModule( pszBaseName, FALSE );	
			findyet = BOOL (pTemp);
			if ( !findyet && !SearchPath(0, pszBaseName, 0, MAX_PATH, szPath, &pszDontCare))
				pTemp = pNew->m_pNext = new MODULE_FILE_INFO( pszBaseName, pImportDir->Name, FALSE);
			else
			{
				if (!findyet) 
					pTemp = pNew->m_pNext = new MODULE_FILE_INFO( szPath, pImportDir->Name, TRUE );
			}
			CString strTemp="";

//	Add function
			PIMAGE_THUNK_DATA32 thunk, thunkIAT=0;
			PIMAGE_THUNK_DATA64 thunk64, thunkIAT64=0;
			thunk = (PIMAGE_THUNK_DATA32)pImportDir->Characteristics;
			thunkIAT = (PIMAGE_THUNK_DATA32)pImportDir->FirstThunk;

			if ( thunk == 0 )   // No Characteristics field?
			{
				thunk = thunkIAT;
				if ( thunk == 0 ) return errMDL_GENERAL_FAILURE;
			}
			thunk = (PIMAGE_THUNK_DATA32)m_pPE->GetReadablePointerFromRVA((DWORD) thunk);
			if (!thunk ) return errMDL_GENERAL_FAILURE;
			thunkIAT = (PIMAGE_THUNK_DATA32) m_pPE->GetReadablePointerFromRVA((DWORD)thunkIAT);
			if (m_pPE->Is64bits()) 
			{
				thunk64 = (PIMAGE_THUNK_DATA64) thunk;
				thunkIAT64 = (PIMAGE_THUNK_DATA64) thunkIAT;

				while ( 1 ) // Loop forever (or until we break out)
				{
					if ( thunk64->u1.AddressOfData == 0 ) break;
					if ( thunk64->u1.Ordinal & IMAGE_ORDINAL_FLAG64 )
					{
						strTemp.Format( "ordinal %d", IMAGE_ORDINAL(thunk64->u1.Ordinal) );
						pTemp->AddFunc( strTemp );
					}
					else				
					{
						PIMAGE_IMPORT_BY_NAME pOrdinalName = ( PIMAGE_IMPORT_BY_NAME ) thunk64->u1.AddressOfData;
						pOrdinalName = (PIMAGE_IMPORT_BY_NAME) m_pPE->GetReadablePointerFromRVA((DWORD)pOrdinalName);
						if ((pOrdinalName) && test_Name((PCHAR) pOrdinalName->Name))
							pTemp->AddFunc( (PCHAR) pOrdinalName->Name );
						else pTemp->AddFunc( (PCHAR) "<invalid name>" );
					}
					thunk64++;            // Advance to next thunk
					thunkIAT64++;         // advance to next thunk
				} /**/
			} else {
				if (! m_pPE->IsValidPtr( (DWORD) thunk ))
					return errMDL_NO_ERROR; // !!!!!!!!!!!!!!!!!!!!!!!!!!!
				while ( 1 ) // Loop forever (or until we break out)
				{
					if ( thunk->u1.AddressOfData == 0 ) break;
					if ( thunk->u1.Ordinal & IMAGE_ORDINAL_FLAG32 )
					{
						strTemp.Format( "ordinal %d", IMAGE_ORDINAL(thunk->u1.Ordinal) );
						pTemp->AddFunc( strTemp );
					}
					else				
					{
						PIMAGE_IMPORT_BY_NAME pOrdinalName = ( PIMAGE_IMPORT_BY_NAME ) thunk->u1.AddressOfData;
						pOrdinalName = (PIMAGE_IMPORT_BY_NAME) m_pPE->GetReadablePointerFromRVA((DWORD)pOrdinalName);
						if ((DWORD) pOrdinalName <= Importsz)
						{	
#ifdef _DEBUG 
	AfxMessageBox("Ptr out of bound", MB_OK|MB_ICONEXCLAMATION);
#endif /**/
							break;
						}
						
						if ((pOrdinalName) && test_Name((PCHAR) pOrdinalName->Name))
							pTemp->AddFunc( (PCHAR) pOrdinalName->Name );
						else pTemp->AddFunc( (PCHAR) "<invalid name>" );
					}
					thunk++;            // Advance to next thunk
					thunkIAT++;         // advance to next thunk
				}
			}
		} else pNew->m_pNext = new MODULE_FILE_INFO( "unknown or error", 0, FALSE);		
		if ( !pNew ) return errMDL_GENERAL_FAILURE;
		if (!findyet) 
		{
			pNew = pNew->m_pNext;
			m_cModules++;
		}
		pImportDir++;	// Advance to next imported module
		findyet = FALSE;
	} 
	return errMDL_NO_ERROR;
}

errModuleDependencyList MODULE_DEPENDENCY_LIST::AddDirectDelayedModule()
{
	if (m_pListDelayed) 	DeletePList( m_pListDelayed );
	m_pListDelayed = new MODULE_FILE_INFO( (const PSTR) m_pPE->GetName(), 0, FALSE);
	if ( !m_pListDelayed ) return errMDL_GENERAL_FAILURE;
	PMODULE_FILE_INFO pNew = m_pListDelayed;

	m_cDelayedModules++;
	if ( FALSE == m_pPE->IsValid() )	 // A valid PE file???
		return (errModuleDependencyList)m_pPE->GetErrorType();
	DWORD DDESize = m_pPE->GetDataDirectoryEntrySize( IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT );
	if (0 == DDESize) 
		return errMDL_NO_ERROR;
	ImgDelayDescr *pDImportDesc = m_pPE->GetDelayImportsDesc();
	if ( !pDImportDesc ) 
		return errMDL_NO_ERROR;
	if ( !m_pPE->IsValidPtr( (DWORD) pDImportDesc))
	{
		pNew->m_pNext = new MODULE_FILE_INFO( "Error in DELAYED IMPORT TABLE", 0, FALSE);
		return errMDL_NO_ERROR;
	}
	while( pDImportDesc->rvaDLLName )
	{
		char szPath[MAX_PATH];
		PSTR pszDontCare;

		bool bUsingRVA = pDImportDesc->grAttrs & 1;
		PSTR pszDLLName = bUsingRVA
			? (PSTR)m_pPE->GetReadablePointerFromRVA((DWORD)pDImportDesc->rvaDLLName)
			: (PSTR)m_pPE->GetReadablePointerFromVA((PBYTE)0+(DWORD)pDImportDesc->rvaDLLName);
		if (!pszDLLName)
		{
#ifdef _DEBUG 
	AfxMessageBox("Error in DELAYED IMPORT TABLE", MB_OK|MB_ICONEXCLAMATION);
#endif
			pNew->m_pNext = new MODULE_FILE_INFO( "Error in DELAYED IMPORT TABLE", 0, FALSE);
			return errMDL_NO_ERROR;
		}
		if (pszDLLName) 
		{
			if ( !SearchPath(0, pszDLLName, 0, MAX_PATH, szPath, &pszDontCare))
				pNew->m_pNext = new MODULE_FILE_INFO( pszDLLName, 0, FALSE );
			else
			{
				pNew->m_pNext = new MODULE_FILE_INFO( szPath, 0, TRUE );
			}
			CString strTemp="";
// Add functions
			bool bUsingRVA = pDImportDesc->grAttrs & 1;
			PIMAGE_THUNK_DATA32 thunk = bUsingRVA  
				? (PIMAGE_THUNK_DATA32) m_pPE->GetReadablePointerFromRVA((DWORD)pDImportDesc->rvaINT)
				: (PIMAGE_THUNK_DATA32) m_pPE->GetReadablePointerFromVA((PBYTE)0+(DWORD)pDImportDesc->rvaINT);

			if (m_pPE->Is64bits()) {
				PIMAGE_THUNK_DATA64 thunk64 = (PIMAGE_THUNK_DATA64) thunk;
				while ( 1 ) 
				{
					if ( thunk64->u1.AddressOfData == 0 )	break;
					if ( thunk64->u1.Ordinal & IMAGE_ORDINAL_FLAG64 )
					{
						strTemp.Format( "ordinal %d", IMAGE_ORDINAL(thunk64->u1.Ordinal) );
						pNew->m_pNext->AddFunc( strTemp );
					}
					else
					{
						PIMAGE_IMPORT_BY_NAME pOrdinalName = ( PIMAGE_IMPORT_BY_NAME ) thunk64->u1.AddressOfData;
						if ( bUsingRVA ) 
							pOrdinalName = (PIMAGE_IMPORT_BY_NAME) m_pPE->GetReadablePointerFromRVA((DWORD)pOrdinalName);
						else 
							pOrdinalName = (PIMAGE_IMPORT_BY_NAME) m_pPE->GetReadablePointerFromVA((PBYTE)0+(DWORD)pOrdinalName);
						if (pOrdinalName) 
							pNew->m_pNext->AddFunc( (PCHAR) pOrdinalName->Name );
						else pNew->m_pNext->AddFunc( (PCHAR) "<invalid name>" );
					}
					thunk64++;
				}
			} else {
				if (! m_pPE->IsValidPtr( (DWORD) thunk ))
					return errMDL_NO_ERROR; // !!!!!!!!!!!!!!!!!!!!!!!!!!!
				while ( 1 ) 
				{
					if ( thunk->u1.AddressOfData == 0 )	break;
					if ( thunk->u1.Ordinal & IMAGE_ORDINAL_FLAG32 )
					{
						strTemp.Format( "ordinal %d", IMAGE_ORDINAL(thunk->u1.Ordinal) );
						pNew->m_pNext->AddFunc( strTemp );
					}
					else
					{
						PIMAGE_IMPORT_BY_NAME pOrdinalName = ( PIMAGE_IMPORT_BY_NAME ) thunk->u1.AddressOfData;
						if ( bUsingRVA ) 
							pOrdinalName = (PIMAGE_IMPORT_BY_NAME) m_pPE->GetReadablePointerFromRVA((DWORD)pOrdinalName);
						else 
							pOrdinalName = (PIMAGE_IMPORT_BY_NAME) m_pPE->GetReadablePointerFromVA((PBYTE)0+(DWORD)pOrdinalName);
						if (pOrdinalName) 
							pNew->m_pNext->AddFunc( (PCHAR) pOrdinalName->Name );
						else pNew->m_pNext->AddFunc( (PCHAR) "<invalid name>" );
						
					}
					thunk++;            // Advance to next thunk
				}
			}
		} else pNew->m_pNext = new MODULE_FILE_INFO( "unknown or error", 0, FALSE);		
		pNew = pNew->m_pNext;
		if ( !pNew ) return errMDL_GENERAL_FAILURE;
		m_cDelayedModules++;
		pDImportDesc++;
	}
	return errMDL_NO_ERROR;
}


/*
errModuleDependencyList MODULE_DEPENDENCY_LIST::AddModule( PSTR pszFileName )
{
	 PE_EXE peFile( pszFileName );		 // Get easy access to the executable
	 
	 if ( FALSE == peFile.IsValid() )	 // A valid PE file???
		  return (errModuleDependencyList)peFile.GetErrorType();

	 PMODULE_FILE_INFO pNew = new MODULE_FILE_INFO( pszFileName );
	 if ( !pNew )
		  return errMDL_GENERAL_FAILURE;

	 pNew->m_pNext = m_pList;
	 m_pList = pNew;
	 m_cModules++;
	 //
	 // Now see if this module imports any other modules.  If so, we need
	 // to recurse and add them as well.
	 if (0 == peFile.GetDataDirectoryEntrySize( m_type ))
		  return errMDL_NO_ERROR;

	if (m_type == IMAGE_DIRECTORY_ENTRY_IMPORT)
	{	// Make a pointer to the imports table
		PIMAGE_IMPORT_DESCRIPTOR pImportDir;
		pImportDir = (PIMAGE_IMPORT_DESCRIPTOR) peFile.GetDataDirectoryEntryPointer(m_type);
		if ( !pImportDir ) return errMDL_NO_ERROR;

		// While there are still non-null IMAGE_IMPORT_DESCRIPTORs...
		while ( pImportDir->Name )
		{
			// Get a pointer to the imported module's base name
			PSTR pszBaseName;
			pszBaseName = (PSTR)peFile.GetReadablePointerFromRVA(pImportDir->Name);

			if ( !pszBaseName )	break;

			// Check to see if it's already in our list.  Don't add again if so.
			if ( 0 == LookupModule( pszBaseName, FALSE ) )
			{
				// Search path supposedly has the same searching algorithm as
				// the the Win32 loader...
				char szPath[MAX_PATH];
				PSTR pszDontCare;
				if ( SearchPath(0, pszBaseName, 0, MAX_PATH, szPath, &pszDontCare))
					AddModule( szPath );
				else
					pNew->AddNotFoundModule( pszBaseName );
			}
			pImportDir++;	// Advance to next imported module
		}
	}
	else 
	{
		ImgDelayDescr *pe_delay = (ImgDelayDescr *)peFile.GetDataDirectoryEntryPointer(m_type);
		PSTR pszBaseName;
		while (pszBaseName = (PSTR) ((pe_delay->grAttrs & 1)?peFile.GetReadablePointerFromRVA((DWORD) pe_delay->szName):(PSTR) pe_delay->szName))
		{
			if ( ! pszBaseName) break;

			// Check to see if it's already in our list.  Don't add again if so.
			if ( 0 == LookupModule( pszBaseName, FALSE ) )
			{
				// Search path supposedly has the same searching algorithm as
				// the the Win32 loader...
				char szPath[MAX_PATH];
				PSTR pszDontCare;
				if ( SearchPath(0, pszBaseName, 0, MAX_PATH, szPath, &pszDontCare))
					AddModule( szPath );
				else
					pNew->AddNotFoundModule( pszBaseName);
			}
			pe_delay++;
		}
	}	
	 return errMDL_NO_ERROR;
}
*/