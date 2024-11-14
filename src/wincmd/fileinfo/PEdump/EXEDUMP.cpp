//==================================
// PEDUMP - Matt Pietrek 1997
// FILE: EXEDUMP.C
//==================================
//  Modifié par FG le 23/03/2003
//
#include "stdafx.h"
#include <stdio.h>
#include "..\peexe\peexe.h"
#include "..\..\common\wait.h"

#include "..\..\..\common\fdate.h"
#pragma hdrstop
#include "common.h"
#include "symboltablesupport.h"
#include "COFFSymbolTable.h"
#include "resdump.h"
#include "extrnvar.h"

#include "..\peexe\decplug.h"
#include "..\unmangle.h"

BOOL test_Name( PCHAR str)
{
	do {
		if ((*str < 33) || (*str > 'z')) 
			return FALSE;
	} while( *(++str));
	return TRUE;
}

CString DumpExeDebugDirectory(PE_EXE &pe)
{
    PIMAGE_DEBUG_DIRECTORY debugDir;
    PIMAGE_SECTION_HEADER header;
    DWORD size;
	
	DWORD va_debug_dir = pe.GetDataDirectoryEntryRVA(IMAGE_DIRECTORY_ENTRY_DEBUG);
	if ( va_debug_dir == 0 )   return "";

	// If we found a .debug section, and the debug directory is at the beginning of this section, it looks like a Borland file
	header = pe.GetSectionHeaderbyName(".debug");
	if ( header && (header->VirtualAddress == va_debug_dir) )
	{
		debugDir = (PIMAGE_DEBUG_DIRECTORY)pe.GetTranslatedPtr(header->PointerToRawData);
		size = pe.GetDataDirectoryEntrySize(IMAGE_DIRECTORY_ENTRY_DEBUG);				
	}
	else	// Look for the debug directory
	{ 
		header = pe.GetEnclosingSectionHeader( va_debug_dir );
		if ( !header ) return "";
		size = pe.GetDataDirectoryEntrySize( IMAGE_DIRECTORY_ENTRY_DEBUG );
		DWORD pIDD = header->PointerToRawData + (va_debug_dir - header->VirtualAddress);
		debugDir = (PIMAGE_DEBUG_DIRECTORY) pe.GetTranslatedPtr( pIDD );
	}
	return DumpDebugDirectory( debugDir, size, pe.GetBase() );
}

//
// Dump the imports table (the .idata section) of a PE file
//
CString DumpImportsSection(PE_EXE &pe)
{
	CString str, strTemp;
	PIMAGE_IMPORT_DESCRIPTOR importDesc;
	PIMAGE_THUNK_DATA32 thunk, thunkIAT=0;
	PIMAGE_THUNK_DATA64 thunk64, thunkIAT64=0;
	PIMAGE_IMPORT_BY_NAME pOrdinalName;
	PIMAGE_NT_HEADERS32 pNTHeader = pe.GetIMAGE_NT_HEADERS32();
	PIMAGE_NT_HEADERS64 pNTHeader64 = pe.GetIMAGE_NT_HEADERS64();

	importDesc = pe.GetImportsDesc();
	
//	DWORD ImportRVA = pe.GetDataDirectoryEntryRVA(IMAGE_DIRECTORY_ENTRY_IMPORT);
	DWORD Importsz = pe.GetImportsSz();

	if ( !importDesc )   return str;		
	if (!pe.IsValidPtr(( DWORD) importDesc))
	{
		str = ("IMPORTS TABLE:\r\n");
#ifdef _DEBUG 
		AfxMessageBox("Error in IMPORT TABLE", MB_OK|MB_ICONEXCLAMATION);
#endif
		str += "IMPORT TABLE not found, due to a possible compressed executable \r\n\r\n";
		return str;
	} 
 	if (!pe.GetReadablePointerFromRVA(importDesc->Name)) return str; // if point to (null)
	str = ("IMPORTS TABLE:\r\n");

// correct bug from Cygwin
	ULONG lastNameAdd = -1;

	while ( 1 )
	{	// See if we've reached an empty IMAGE_IMPORT_DESCRIPTOR
		if ( (importDesc ->TimeDateStamp==0 ) && (importDesc->Name==0) )
			break;
		
// correct bug from Cygwin
		if ( lastNameAdd == importDesc->Name)
		{
			importDesc++; 
			continue;
		} lastNameAdd = importDesc->Name;
// end

		strTemp.Format("   %s\r\n", pe.GetReadablePointerFromRVA(importDesc->Name) );
		str += strTemp;

		strTemp.Format("\tImport Lookup Table RVA:  \t%08Xh	 (Unbound IAT)\r\n", importDesc->Characteristics);
		str += strTemp;

		strTemp.Format("\tTimeDateStamp: \t%08Xh   ", importDesc->TimeDateStamp );
		str += strTemp;
		strTemp.Format( !(importDesc->TimeDateStamp==0 || (importDesc->TimeDateStamp == 0xFFFFFFFF))
				?" -> %s \r\n" 
				: "\r\n", FormatShortDate(CreateSystemeTime(importDesc->TimeDateStamp)));
		str += strTemp;

		strTemp.Format("\tForwarderChain: \t%08Xh\r\n", importDesc->ForwarderChain);
		str += strTemp;

		lastNameAdd = importDesc->Name;
		strTemp.Format("\tDLL Name RVA: \t%08Xh\n", importDesc->Name);
		str += strTemp;

		strTemp.Format("\tImport Address Table RVA: \t%08Xh\n", importDesc->FirstThunk);
		str += strTemp;

		strTemp.Format("\tFirst thunk RVA: \t%08Xh\r\n", importDesc->FirstThunk);
		str += strTemp;
	
		thunk = (PIMAGE_THUNK_DATA32) importDesc->Characteristics;
		thunkIAT = (PIMAGE_THUNK_DATA32) importDesc->FirstThunk;

		if ( thunk == 0 )   // No Characteristics field?
		{	// Yes! Gotta have a non-zero FirstThunk field then.
			thunk = thunkIAT;
			if ( thunk == 0 )   // No FirstThunk field?  Ooops!!!
				return str;
		}
	
		// Adjust the pointer to point where the tables are in the mem mapped file.
		thunk = (PIMAGE_THUNK_DATA32) pe.GetReadablePointerFromRVA((DWORD)thunk);

		if (!thunk ) return str;
		

		thunkIAT = (PIMAGE_THUNK_DATA32) pe.GetReadablePointerFromRVA((DWORD)thunkIAT);
		str += ("\tOrdn  \tName\r\n");
		str += ("\t-----\t-----\r\n");
		
		if (pe.Is64bits()) 
		{
			thunk64 = (PIMAGE_THUNK_DATA64) thunk;
			thunkIAT64 = (PIMAGE_THUNK_DATA64) thunkIAT;
			while ( 1 ) // Loop forever (or until we break out)
			{
				if ( thunk64->u1.AddressOfData == 0 )	break;
				if ( thunk64->u1.Ordinal & IMAGE_ORDINAL_FLAG64 )
				{
					strTemp.Format( "\t%4u \t", IMAGE_ORDINAL(thunk64->u1.Ordinal) );
					str += strTemp;
				}
				else
				{
					pOrdinalName = (PIMAGE_IMPORT_BY_NAME) thunk64->u1.AddressOfData;
					pOrdinalName = (PIMAGE_IMPORT_BY_NAME) pe.GetReadablePointerFromRVA((DWORD)pOrdinalName);
					{	// Undecorate Name   
						if ((pOrdinalName) && test_Name((PCHAR) pOrdinalName->Name))
						{
							PCHAR Textin = (PCHAR) pOrdinalName->Name;
							char Textout[1024];
							if ( fShowUndecorated )
							{
								strTemp.Format("\t%4u \t%s", pOrdinalName->Hint, Undecorate( Textin, Textout, 1024));
							} else 
								strTemp.Format("\t%4u \t%s", pOrdinalName->Hint, Textin);
						} else 
						{
							strTemp.Format("\tNA \t<invalid name>");
#ifdef _DEBUG 
		AfxMessageBox("Invalid name", MB_OK|MB_ICONEXCLAMATION);
#endif
						}
						str += strTemp;
					}
				}
			 // If the user explicitly asked to see the IAT entries, or
			 // if it looks like the image has been bound, append the address
				if ( fShowIATentries || importDesc->TimeDateStamp )
					{	   
						strTemp.Format( " \t\t(Bound to: %08Xh)", thunkIAT64->u1.Function );
						str += strTemp;
					}
				str += ( "\r\n" );
				thunk64++;			// Advance to next thunk
				thunkIAT64++;		 // advance to next thunk
			}
		}
		else
		{
			while ( 1 ) // Loop forever (or until we break out)
			{
				
				if ( thunk->u1.AddressOfData == 0 )	break;
				if ( thunk->u1.Ordinal & IMAGE_ORDINAL_FLAG32 )
				{
					strTemp.Format( "\t%4u \t", IMAGE_ORDINAL(thunk->u1.Ordinal) );
					str += strTemp;
				}
				else
				{
					pOrdinalName = (PIMAGE_IMPORT_BY_NAME) thunk->u1.AddressOfData;
					pOrdinalName = (PIMAGE_IMPORT_BY_NAME) pe.GetReadablePointerFromRVA((DWORD)pOrdinalName);
					if ((DWORD) pOrdinalName <= Importsz)
					{	
#ifdef _DEBUG 
						AfxMessageBox("Ptr out of bound", MB_OK|MB_ICONEXCLAMATION);
#endif /**/
						break;
					}

					if ((pOrdinalName) && test_Name((PCHAR) pOrdinalName->Name))
					{						
						PCHAR Textin = (PCHAR) pOrdinalName->Name;
						char Textout[1024];
						if ( fShowUndecorated )
						{
							strTemp.Format("\t%4u \t%s", pOrdinalName->Hint, Undecorate( Textin, Textout, 1024));
						} else 
							strTemp.Format("\t%4u \t%s", pOrdinalName->Hint, Textin);
					} else 
					{
						strTemp.Format("\tNA \t<invalid name>");
#ifdef _DEBUG 
		AfxMessageBox("Invalid name", MB_OK|MB_ICONEXCLAMATION);
#endif
					}
					str += strTemp;
				}
			 // If the user explicitly asked to see the IAT entries, or
			 // if it looks like the image has been bound, append the address
				if ( fShowIATentries || importDesc->TimeDateStamp )
					{	   
						strTemp.Format( " \t\t(Bound to: %08Xh)", thunkIAT->u1.Function );
						str += strTemp;
					}
				str += ( "\r\n" );
				thunk++;			// Advance to next thunk
				thunkIAT++;		 // advance to next thunk
			}
		}
		importDesc++;   // advance to next IMAGE_IMPORT_DESCRIPTOR
		str += ("\r\n");
	}
	str += ("\r\n");
	return str;
}

//
// Dump the delayed imports table (the .idata section) of a PE file ( FG )
//
CString DumpImportsDelayedSection(PE_EXE &pe)
{
//	PIMAGE_NT_HEADERS pNTHeader = pe.GetIMAGE_NT_HEADERS();
	PIMAGE_THUNK_DATA32 thunk;
	PIMAGE_IMPORT_BY_NAME pOrdinalName;
	
	CString str, strTemp;
	PImgDelayDescr pDImportDesc;
	
	DWORD DDESize = pe.GetDataDirectoryEntrySize( IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT );
	if (0 == DDESize) return str;
	pDImportDesc = pe.GetDelayImportsDesc();
	if ( !pDImportDesc ) return str;
	str = ("DELAYED IMPORT TABLE:\r\n");
	if (!pe.IsValidPtr(( DWORD) pDImportDesc ))
	{
#ifdef _DEBUG 
		AfxMessageBox("Error in DELAYED IMPORT TABLE", MB_OK|MB_ICONEXCLAMATION);
#endif
		str += "DELAYED IMPORT TABLE not found, due to a possible compressed executable \r\n\r\n";
		return str;
	} 
	while ( pDImportDesc->rvaDLLName  )
	{   
//		if (!pe.IsValidPtr(( DWORD) pDImportDesc->rvaDLLName))		break;
		bool bUsingRVA = pDImportDesc->grAttrs & 1;
		DWORD dllNameRVA = (DWORD)pDImportDesc->rvaDLLName;
		PVOID dllNameVA = (PBYTE)0+(DWORD)pDImportDesc->rvaDLLName;
		PSTR pszDLLName = bUsingRVA
			? (PSTR)pe.GetReadablePointerFromRVA(dllNameRVA)
			: (PSTR)pe.GetReadablePointerFromVA(dllNameVA);

		if (!pszDLLName)
		{
			str += "DELAYED IMPORT TABLE not found, due to a possible compressed executable \r\n\r\n";
#ifdef _DEBUG 
	AfxMessageBox("Error in DELAYED IMPORT TABLE", MB_OK|MB_ICONEXCLAMATION);
#endif
			break;
		}
		strTemp.Format("   %s\r\n", pszDLLName );
		str += strTemp;

		strTemp.Format("\tCharacteristics:  \t%08Xh\r\n", pDImportDesc->grAttrs);
		str += strTemp;
		strTemp.Format("\t\tAddress fields treated as %s\r\n", (pDImportDesc->grAttrs & 1)?"RVAs":"VAs");
		str += strTemp;


		strTemp.Format("\tDLL Name %s: \t%08Xh\n", (bUsingRVA?"RVA":"VA"), (bUsingRVA?dllNameRVA: (DWORD) dllNameVA));
		str += strTemp;

		strTemp.Format("\tModule Handle Address %s: \t%08Xh\n", (bUsingRVA?"RVA":"VA"), pDImportDesc->rvaHmod);
		str += strTemp;
		strTemp.Format("\tImport Address Table %s: \t%08Xh\n", (bUsingRVA?"RVA":"VA"), pDImportDesc->rvaIAT);
		str += strTemp;
		strTemp.Format("\tImport Name Table %s: \t%08Xh\n", (bUsingRVA?"RVA":"VA"), pDImportDesc->rvaINT);
		str += strTemp;
		strTemp.Format("\tBound address pointer: \t%08Xh\n", pDImportDesc->rvaBoundIAT);
		str += strTemp;
		strTemp.Format("\tBound address pointer copy: \t%08Xh\n", pDImportDesc->rvaUnloadIAT);
		str += strTemp;
		strTemp.Format("\tTimeDateStamp: \t%08Xh   ", pDImportDesc->dwTimeStamp);
		str += strTemp;
		strTemp.Format( !(pDImportDesc->dwTimeStamp ==0 || (pDImportDesc->dwTimeStamp == 0xFFFFFFFF))
				?" -> %s \r\n" 
				: "\r\n", FormatShortDate(CreateSystemeTime(pDImportDesc->dwTimeStamp)));
		str += strTemp;

// find functions
		thunk = bUsingRVA  
			? (PIMAGE_THUNK_DATA32) pe.GetReadablePointerFromRVA((DWORD)pDImportDesc->rvaINT)
			: (PIMAGE_THUNK_DATA32) pe.GetReadablePointerFromVA((PBYTE)0+(DWORD)pDImportDesc->rvaINT);

//		thunkIAT = bUsingRVA  
//			? (PIMAGE_THUNK_DATA) pe.GetReadablePointerFromRVA((DWORD)pDImportDesc->pIAT)
//			: (PIMAGE_THUNK_DATA) pe.GetReadablePointerFromVA((PBYTE)0+(DWORD)pDImportDesc->pIAT);
		if (!thunk ) return str;
#ifdef _DEBUG 
		if (! pe.IsValidPtr( (DWORD) thunk ))
			AfxMessageBox("Ptr out of bound", MB_OK|MB_ICONEXCLAMATION);
#endif
		str += ("\tOrdn  \tName\r\n");
		str += ("\t-----\t-----\r\n");
	
		while ( 1 ) // Loop forever (or until we break out)
		{
			if ( thunk->u1.AddressOfData == 0 )
				break;

			if ( thunk->u1.Ordinal & IMAGE_ORDINAL_FLAG32 )
			{
				strTemp.Format( "\t%4u \t", IMAGE_ORDINAL(thunk->u1.Ordinal) );
					str += strTemp;
			}
			else
			{
				pOrdinalName = (PIMAGE_IMPORT_BY_NAME) thunk->u1.AddressOfData;
				if ( bUsingRVA ) 
					pOrdinalName = (PIMAGE_IMPORT_BY_NAME) pe.GetReadablePointerFromRVA((DWORD)pOrdinalName);
				else 
					pOrdinalName = (PIMAGE_IMPORT_BY_NAME) pe.GetReadablePointerFromVA((PBYTE)0+(DWORD)pOrdinalName);
				if ((pOrdinalName) && test_Name((PCHAR) pOrdinalName->Name))
				{	// Undecorate Name   
					PCHAR Textin = (PCHAR) pOrdinalName->Name;
					char Textout[1024];
					if ( fShowUndecorated )
					{
						strTemp.Format("\t%4u \t%s", pOrdinalName->Hint, Undecorate( Textin, Textout, 1024));
					} else 
						strTemp.Format("\t%4u \t%s", pOrdinalName->Hint, Textin);
				} else 
				{
					strTemp.Format("\tNA \t<invalid name>");
#ifdef _DEBUG 
		AfxMessageBox("Invalid name", MB_OK|MB_ICONEXCLAMATION);
#endif
				}
				str += strTemp;
			}
			str += ( "\r\n" );
			thunk++;			// Advance to next thunk
		}
		pDImportDesc++;   // advance to next IMAGE_IMPORT_DESCRIPTOR
		str += ("\r\n");
	}
	return str;
}

//
// Dump the exports table (usually the .edata section) of a PE file
//
CString DumpExportsSection(PE_EXE &pe)
{
	CString str, strTemp;

	PIMAGE_EXPORT_DIRECTORY exportDir;
	PSTR	filename;
	DWORD	i;
	PDWORD	functions;
	PWORD	ordinals;
	PDWORD	name;

	DWORD exportsStartRVA = (DWORD) pe.GetDataDirectoryEntryPointer(IMAGE_DIRECTORY_ENTRY_EXPORT);
	DWORD exportsEndRVA = exportsStartRVA + (DWORD) pe.GetDataDirectoryEntrySize( IMAGE_DIRECTORY_ENTRY_EXPORT );

// Get the IMAGE_SECTION_HEADER that contains the exports.  This is usually the .edata section, but doesn't have to be.
	exportDir = pe.GetExportsDesc();
	if ( !exportDir )   return str;

	str = ("EXPORTS TABLE:\r\n");
	if (!pe.IsValidPtr(( DWORD) exportDir ))
	{
#ifdef _DEBUG 
		AfxMessageBox("Error in EXPORT TABLE", MB_OK|MB_ICONEXCLAMATION);
#endif
		str += "EXPORT SECTION not found, due to a possible compressed executable  \r\n\r\n";
		return str;
	}	

	filename = (PSTR)pe.GetReadablePointerFromRVA( exportDir->Name );
	// A changer FG
	if ( ((DWORD) filename > (DWORD) exportDir + pe.GetFileSize()) || ((DWORD) filename < (DWORD) exportDir))
	{
#ifdef _DEBUG 
		AfxMessageBox("Error in EXPORT TABLE", MB_OK|MB_ICONEXCLAMATION);
#endif
		str += "EXPORT TABLE not found, due to a possible compressed executable \r\n\r\n";
		return str;
	}/**/
	
	strTemp.Format("\tName: \t%s\r\n", filename);
	str += strTemp;
	strTemp.Format("\tCharacteristics: \t%08Xh\r\n", exportDir->Characteristics);
	str += strTemp;

	strTemp.Format("\tTimeDateStamp: \t%08Xh   ", exportDir->TimeDateStamp );
	str += strTemp;
	strTemp.Format( !(exportDir->TimeDateStamp==0 || (exportDir->TimeDateStamp == 0xFFFFFFFF))
			?" -> %s \r\n" 
			: "\r\n", FormatShortDate(CreateSystemeTime(exportDir->TimeDateStamp)));
	str += strTemp;

	strTemp.Format("\tVersion:\t%u.%02u\r\n", exportDir->MajorVersion, exportDir->MinorVersion);
	str += strTemp;
	strTemp.Format("\tOrdinal base: \t%08Xh\r\n", exportDir->Base);
	str += strTemp;
	strTemp.Format("\t# of functions: \t%08Xh (%d)\r\n", exportDir->NumberOfFunctions, exportDir->NumberOfFunctions);
	str += strTemp;
	strTemp.Format("\t# of Names: \t%08Xh  (%d)\r\n", exportDir->NumberOfNames, exportDir->NumberOfNames);
	str += strTemp;

	functions = (PDWORD)( pe.GetReadablePointerFromRVA( exportDir->AddressOfFunctions ));
	ordinals = (PWORD)( pe.GetReadablePointerFromRVA( exportDir->AddressOfNameOrdinals ));
	name = (PDWORD) pe.GetReadablePointerFromRVA( exportDir->AddressOfNames );

	str += ("\r\n\tEntry Pt  \tOrdn     Name\r\n");
	DWORD j;
	DWORD entryPointRVA;
	DWORD nbfunc = exportDir->NumberOfFunctions;
	if ( exportDir->NumberOfFunctions > 3000)
		if (AfxMessageBox("More than 3000 exported functions\n May take a long time \n Dump All (OK) or None (Cancel)", MB_OKCANCEL|MB_ICONQUESTION ) == IDCANCEL)
			nbfunc = 0;
	for ( i=0; i < nbfunc; i++ )
	{
		entryPointRVA = functions[i];

		// Skip over gaps in exported function ordinals (the entrypoint is 0 for these functions).
		if ( entryPointRVA == 0 )   continue;			   
									
		strTemp.Format("\t%08Xh     \t%4u", entryPointRVA, i + exportDir->Base );
		str += strTemp;

		// See if this function has an associated name exported for it.
		for ( j=0; j < exportDir->NumberOfNames; j++ )
		{
			if ( ordinals[j] == i )
			{	// Undecorate Name
				PCHAR Textin = (PCHAR) pe.GetReadablePointerFromRVA( name[j] );
				if ( fShowUndecorated && Textin[0] != '\0')
				{
					char Textout[1024];
					strTemp.Format("     %s", Undecorate( Textin, Textout, 1024));
				} else strTemp.Format("     %s", Textin);
				str += strTemp;
			}
		}
		// Is it a forwarder?  If so, the entry point RVA is inside the
		// .edata section, and is an RVA to the DllName.EntryPointName
		if ( (entryPointRVA >= exportsStartRVA) && (entryPointRVA <= exportsEndRVA) )
		{
			strTemp.Format(" (forwarder -> %s)", pe.GetReadablePointerFromRVA( entryPointRVA ));
			str += strTemp;
		}
		
		str += ("\r\n");
	}
	str += ("\r\n");
   return str;
}


CString DumpRuntimeFunctions( PE_EXE &pe )
{
	CString str, strTemp;

//	PIMAGE_RUNTIME_FUNCTION_ENTRY pRTFn = (PIMAGE_RUNTIME_FUNCTION_ENTRY) pe.GetDataDirectoryEntryPointer(IMAGE_DIRECTORY_ENTRY_EXCEPTION);
	PIMAGE_ALPHA64_RUNTIME_FUNCTION_ENTRY pRTFn = (PIMAGE_ALPHA64_RUNTIME_FUNCTION_ENTRY) pe.GetDataDirectoryEntryPointer(IMAGE_DIRECTORY_ENTRY_EXCEPTION);
	if ( !pRTFn )   return str;
	DWORD cEntries = (DWORD) pe.GetDataDirectoryEntrySize(IMAGE_DIRECTORY_ENTRY_EXCEPTION ) / sizeof( IMAGE_RUNTIME_FUNCTION_ENTRY );
	if ( 0 == cEntries )  return str;

//#ifdef _DEBUG 
//	AfxMessageBox("Runtime Function", MB_OK|MB_ICONEXCLAMATION);
//#endif

   str = ( "RUNTIME FUNCTION TABLE (Exception handling)\r\n" );
   str += ( "\tBegin     End       Handler   HndlData  PrologEnd\r\n" );
   str += ( "\t--------  --------  --------  --------  --------\r\n" );

   for ( unsigned i = 0; i < cEntries; i++, pRTFn++ )
   {
		strTemp.Format(   "\t%08Xh  %08Xh  %08Xh  %08Xh  %08Xh",
			pRTFn->BeginAddress, pRTFn->EndAddress, pRTFn->ExceptionHandler,
			pRTFn->HandlerData, pRTFn->PrologEndAddress );
		str += strTemp;
	  if ( g_pCOFFSymbolTable )
	  {
		 PCOFFSymbol pSymbol
			= g_pCOFFSymbolTable->GetNearestSymbolFromRVA(
							  pRTFn->BeginAddress
							  - pe.GetImageBase(),
							  TRUE );
		 if ( pSymbol )
			str += pSymbol->GetName();
		 delete pSymbol;
	  }
	  str += "\r\n";
	}
	str += ("\r\n");
	return str;
}

// The names of the available base relocations
char *SzRelocTypes[] = {"ABSOLUTE","HIGH","LOW","HIGHLOW","HIGHADJ","MIPS_JMPADDR","SECTION","REL32", "unknown", "IA64_IMM64", "DIR64", "HIGH3ADJ" };
#define NBRELOCTYPE 12
//
// Dump the base relocation table of a PE file
//CString DumpBaseRelocationsSection(DWORD base, PIMAGE_NT_HEADERS pNTHeader)
CString DumpBaseRelocationsSection(PE_EXE &pe)
{
	CString str, strTemp;
	PIMAGE_BASE_RELOCATION baseReloc;

	baseReloc = (PIMAGE_BASE_RELOCATION) pe.GetDataDirectoryEntryPointer( IMAGE_DIRECTORY_ENTRY_BASERELOC );
	if ( !baseReloc ) return str;
	str =("BASE RELOCATIONS:\r\n\r\n");

	while ( baseReloc->SizeOfBlock != 0 )
	{
		unsigned i,cEntries;
		PWORD pEntry;
		char *szRelocType;
		WORD relocType;

		// Sanity check to make sure the data looks OK.
		if ( 0 == baseReloc->VirtualAddress )
			break;
		if ( baseReloc->SizeOfBlock < sizeof(*baseReloc) )
			break;
	  
		cEntries = (baseReloc->SizeOfBlock-sizeof(*baseReloc))/sizeof(WORD);
		pEntry = MakePtr( PWORD, baseReloc, sizeof(*baseReloc) );
		
		strTemp.Format("Virtual Address: %08Xh  size: %08Xh\r\n",
				baseReloc->VirtualAddress, baseReloc->SizeOfBlock);
		str += strTemp;
		for ( i=0; i < cEntries; i++ )
		{	// Extract the top 4 bits of the relocation entry.  Turn those 4
			// bits into an appropriate descriptive string (szRelocType)
			relocType = (*pEntry & 0xF000) >> 12;
			szRelocType = relocType < NBRELOCTYPE ? SzRelocTypes[relocType] : "unknown";
			
			strTemp.Format("  %08Xh %s",
					(*pEntry & 0x0FFF) + baseReloc->VirtualAddress,
					szRelocType);
			str += strTemp;

			if ( IMAGE_REL_BASED_HIGHADJ == relocType )
			{
				pEntry++;
				cEntries--;
				strTemp.Format( " (%X)", *pEntry );
				str += strTemp;
			}
			str += ( "\r\n" );
			pEntry++;   // Advance to next relocation entry
		}
		baseReloc = MakePtr( PIMAGE_BASE_RELOCATION, baseReloc, baseReloc->SizeOfBlock);
	}
	str += ("\r\n");
    return str;
}

//
// Dump out the new IMAGE_BOUND_IMPORT_DESCRIPTOR that NT 3.51 added
//
CString DumpBoundImportDescriptors( PE_EXE &pe )
{
	CString str, strTemp;
	// Bound import descriptors RVA
	DWORD bidRVA = pe.GetDataDirectoryEntryRVA(IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT);
	if ( !bidRVA ) return str;

///   FG    !!! check size of section
	DWORD bidsz = pe.GetDataDirectoryEntrySize(IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT);
	if ( !bidsz ) return str;
	
	PIMAGE_BOUND_IMPORT_DESCRIPTOR pibid = (PIMAGE_BOUND_IMPORT_DESCRIPTOR) pe.GetTranslatedPtr(bidRVA);
	if (!pe.IsValidPtr(( DWORD) pibid ))
	{
#ifdef _DEBUG	
		AfxMessageBox("Error in IMPORT TABLE", MB_OK|MB_ICONEXCLAMATION);
#endif
		str = "IMPORT SECTION not found, due to a possible compressed executable  \r\n\r\n";
		return str;
	}

	str = "BOUND IMPORT descriptors:\r\n" ;
	str += "\tModule \tTimeDate\r\n" ;
	str += "\t---------  \t---------\r\n" ;
	
	while ( pibid->TimeDateStamp && (pibid->OffsetModuleName < bidsz))
	{
		unsigned i;
		PIMAGE_BOUND_FORWARDER_REF pibfr;

	  strTemp.Format("  \t%-12s  \t%08Xh  ", pe.GetBase() + bidRVA + pibid->OffsetModuleName, pibid->TimeDateStamp );
	  str += strTemp;
	  strTemp.Format( !(pibid->TimeDateStamp==0 || (pibid->TimeDateStamp == 0xFFFFFFFF))
		 ?" -> %s \r\n" 
		 : "\r\n", FormatShortDate(CreateSystemeTime(pibid->TimeDateStamp)));
	  str += strTemp;

		pibfr = MakePtr(PIMAGE_BOUND_FORWARDER_REF, pibid, sizeof(IMAGE_BOUND_IMPORT_DESCRIPTOR));
		for ( i=0; i < pibid->NumberOfModuleForwarderRefs; i++ )
		{
	     strTemp.Format(" forwarder : %-12s  \t%08Xh  ", pe.GetBase() + bidRVA + pibfr->OffsetModuleName, pibfr->TimeDateStamp );
         str += strTemp;
         strTemp.Format( !(pibfr->TimeDateStamp==0 || (pibfr->TimeDateStamp == 0xFFFFFFFF))
            ?" -> %s \r\n" 
            : "\r\n", FormatShortDate(CreateSystemeTime(pibfr->TimeDateStamp)));
         str += strTemp;

            pibfr++;    // advance to next forwarder ref
               
            // Keep the outer loop pointer up to date too!
            pibid = MakePtr( PIMAGE_BOUND_IMPORT_DESCRIPTOR, pibid,
                             sizeof( IMAGE_BOUND_FORWARDER_REF ) );
        }
        pibid++;    // Advance to next pibid;
    }
	str += ("\r\n");
    return str;
}

const char * SzPacker[] = { "UPX", "Aspack", "Manolo", "PEload", "PEpack", "PE-Prot", "PEshield",
"Petite", "Securom", "Shrinker", "TimeLock/VBox", "VGCrypt", "WWPack32", "PE-Compact", "WINZIP"};
UINT SearchCompressedEXE(PIMAGE_NT_HEADERS32 pNTHeader)
{
	// 1 : UPX	2 : aspack
	PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(pNTHeader);
	for(int i=0; i < pNTHeader->FileHeader.NumberOfSections; i++, section++)
	{
		if (strnicmp((const char *)section->Name , "upx", 3) == 0)
			return 1;
		else if (strnicmp((const char *)section->Name , ".upx", 4) == 0)
			return 1;
		else if (strncmp((const char *)section->Name , ".aspack", 6) == 0)
			return 2;
		else if (strncmp((const char *)section->Name , ".adata", 6) == 0)
			return 2;
		else if (strncmp((const char *)section->Name , ".manolo", 7) == 0)
			return 3;
		else if (strncmp((const char *)section->Name , ".peload", 7) == 0)
			return 4;
		else if (strncmp((const char *)section->Name , "PEPACK!!", 8) == 0)
			return 5;
		else if (strncmp((const char *)section->Name , "PEPROT", 6) == 0)
			return 6;
		else if (strncmp((const char *)section->Name , "ANAKIN98", 8) == 0)
			return 7;
		else if (strncmp((const char *)section->Name , ".petite", 7) == 0)
			return 8;
		else if (strncmp((const char *)section->Name , ".cms*", 5) == 0)
			return 9;
		else if (strncmp((const char *)section->Name , ".load", 5) == 0)
			return 10;
		else if (strncmp((const char *)section->Name , "WeiJunLi", 8) == 0)
			return 11;
		else if (strncmp((const char *)section->Name , ".vgc", 4) == 0)
			return 12;
		else if (strncmp((const char *)section->Name , ".WWP32", 4) == 0)
			return 13;
		else if (strncmp((const char *)section->Name , "pec1", 4) == 0)
			return 14;
		else if (strncmp((const char *)section->Name , "_winzip_", 8) == 0)
			return 15;
	}
	return 0;	
}
//
// top level routine called from PEDUMP.C to dump the components of a PE file
//
extern	PIMAGE_RESOURCE_DIRECTORY_ENTRY pStrResEntries;
extern	PIMAGE_RESOURCE_DIRECTORY_ENTRY pDlgResEntries;
extern	DWORD cStrResEntries;
extern	DWORD cDlgResEntries;

CString DumpExeFile( PE_EXE &pe, CWait &wait )
{
    CString str="", strTp;

    UINT compress = 0;

	if ( !pe.IsValid() )
		return "Not a Portable Executable (PE) EXE\r\n";
    DWORD base = (DWORD) pe.GetdosHeader();
	PIMAGE_NT_HEADERS32 pNTHeader = pe.GetIMAGE_NT_HEADERS32();
	wait.SetStatus("Start dumping...");

	
	strTp.Format("Entry Point RVA: \t%08Xh\r\n", pe.GetOrEntryPointRVA());
	str += strTp;
	str += "Entry Point RAW: \t";
	if (pe.GetOrEntryPoint())
		strTp.Format("%08Xh\r\n\r\n", pe.GetOrEntryPoint());
	else strTp.Format("%s\r\n\r\n", "Invalid or not in CODE section (possible Encrypted or Packed Executable)");
	str += strTp;

// Search compressed EXE
	if (pe.IsCoded())		//Test compressed and decompress
	{
		wait.SetStatus("Unpacking...");
		DecPlugin *pDec = pe.GetDecPtr();
		str += pDec->GetDecStr();
		if (pe.IsAttached()) //pDec->GetNewHeader() )
		{
			str += "Using Decompressed Image\r\n";
			strTp.Format("Size of New Image : %d bytes \r\n", pe.GetFileSize());
			str += strTp;
			strTp.Format("New Entry Point RVA: \t%08Xh\r\n", pe.GetAddressOfEntryPoint());
			str += strTp;
			strTp.Format("New Entry Point RAW: \t%08Xh\r\n", pe.GetEntryPoint());
			str += strTp;

		}
		str += "\r\n";
	} 
	else if (compress = SearchCompressedEXE(pNTHeader))
	{
		str = str + "Possible Packer/Encryptor :\t" + SzPacker[compress-1];
		str += "\r\n";
	}	
	str += DumpHeader((PIMAGE_FILE_HEADER)&pNTHeader->FileHeader, (PIMAGE_OPTIONAL_HEADER32)&pNTHeader->OptionalHeader);
    str += "\r\n";

    str += DumpOptionalHeader((PIMAGE_OPTIONAL_HEADER32)&pNTHeader->OptionalHeader);
    str += "\r\n";

	wait.SetStatus("Dumping Header...");
    str += DumpSectionTable( IMAGE_FIRST_SECTION(pNTHeader), pNTHeader->FileHeader.NumberOfSections, TRUE);
	
	if (fShowSymbolTable) //todo64
	{
		str += DumpExeDebugDirectory(pe);
	    if ( pNTHeader->FileHeader.PointerToSymbolTable == 0 )
		    g_pCOFFHeader = NULL; // Doesn't really exist!
	}

	if ( fShowResources ) //64ok
    {
		wait.SetStatus("Dumping Resources...");
		str += DumpResourceSection(pe);
	}

	wait.SetStatus("Dumping Import..."); //64ok en partie
    str += DumpImportsSection(pe);
    
	wait.SetStatus("Dumping Delayed Import...");
    str += DumpImportsDelayedSection(pe); //todo64 a faire

    if ( pe.GetDataDirectoryEntryRVA(IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT) ) //todo64 a faire
    {
		wait.SetStatus("Dumping BoundImport...");
		str += DumpBoundImportDescriptors(pe);
    }
    
	wait.SetStatus("Dumping Export..."); 
	str += DumpExportsSection( pe ); //todo64 a faire

   //=========================================================================
   // If we have COFF symbols, create a symbol table now
   //=========================================================================
	if ( g_pCOFFHeader )   // Did we see a COFF symbols header while looking
	{                  // through the debug directory?
#ifdef _DEBUG 
		if (g_pCOFFSymbolTable) AfxMessageBox("COFF Symbol Table not empty", MB_OK|MB_ICONEXCLAMATION);
#endif
		g_pCOFFSymbolTable = new COFFSymbolTable(
            (PVOID)(base+ pNTHeader->FileHeader.PointerToSymbolTable),
            pNTHeader->FileHeader.NumberOfSymbols );
   }


	if ( fShowPDATA && !(pe.Is64bits()))  //todo64 a faire
   {
	  wait.SetStatus("Dumping Runtime...");
      str += DumpRuntimeFunctions( pe );
   }

    if ( fShowRelocations ) // pas d'utilité    //todo64 a faire
    {
		wait.SetStatus("Dumping Base relocation...");
        str += DumpBaseRelocationsSection(pe);
    }

//if (!pe.Is64bits()) 
{
   if ( fShowSymbolTable && g_pMiscDebugInfo ) //todo64 a faire
   {
	   	wait.SetStatus("Dumping DebugInfo...");
		str += DumpMiscDebugInfo( g_pMiscDebugInfo );
   }

   if ( fShowSymbolTable && g_pCVHeader ) //todo64 a faire
   {
		if (!pe.IsValidPtr((DWORD) g_pCVHeader))
		{
#ifdef _DEBUG 
			AfxMessageBox("Error in CodeView TABLE", MB_OK|MB_ICONEXCLAMATION);
#endif
			str += "CodeView TABLE not found, due to a possible compressed executable \r\n\r\n";
		}
		else /**/
		{
			wait.SetStatus("Dumping Debug Header...");
			str += DumpCVDebugInfo( g_pCVHeader );
		}
   }

    if ( fShowSymbolTable && g_pCOFFHeader ) //todo64 a faire
    {
		wait.SetStatus("Dumping COFF Header...");
        str += DumpCOFFHeader( g_pCOFFHeader );
    }
    
    if ( fShowLineNumbers && g_pCOFFHeader )  //todo64 a faire
    {
        str += DumpLineNumbers( MakePtr(PIMAGE_LINENUMBER, g_pCOFFHeader,
                            g_pCOFFHeader->LvaToFirstLinenumber),
                            g_pCOFFHeader->NumberOfLinenumbers);
    }

    if ( fShowSymbolTable ) //todo64 a faire
    {
        if ( pNTHeader->FileHeader.NumberOfSymbols 
            && pNTHeader->FileHeader.PointerToSymbolTable
         && g_pCOFFSymbolTable )
        {
			wait.SetStatus("Dumping SymbolTable...");
            str += DumpSymbolTable( g_pCOFFSymbolTable );
        }
    }
    
    if ( fShowRawSectionData )  //todo64 a faire
    {
        str += DumpRawSectionData( (PIMAGE_SECTION_HEADER)(pNTHeader+1),
                            pe.GetdosHeader(),
                            pNTHeader->FileHeader.NumberOfSections);
    }


 } //todo64 a faire	
	
	if ( g_pCOFFSymbolTable )  
	{
		delete g_pCOFFSymbolTable;
		g_pCOFFSymbolTable = NULL;
	}

	g_pCOFFHeader = NULL;
	g_pMiscDebugInfo = NULL;
	g_pCVHeader = NULL;
	g_pMiscDebugInfo = NULL;
	pDlgResEntries = NULL;
	pStrResEntries = NULL;
	cStrResEntries = 0;
	cDlgResEntries = 0;

   return str;
}