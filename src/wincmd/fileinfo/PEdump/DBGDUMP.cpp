//==================================
// PEDUMP - Matt Pietrek 1997
// FILE: DBGDUMP.C
//==================================
#include "stdafx.h"
/*
#include <windows.h>
#include <stdio.h>
#include <time.h>
*/
#include "common.h"
#include "COFFSymbolTable.h"
#include "SymbolTableSupport.h"
#include "extrnvar.h"
#include "dbgdump.h"

CString DumpImageDbgHeader(PIMAGE_SEPARATE_DEBUG_HEADER pImageSepDbgHeader, PIMAGE_OPTIONAL_HEADER32 optionalHeader)
{
    CString str="", strTemp="";
    UINT headerFieldWidth = 30;

    strTemp.Format("  %-*s%04Xh\r\n", headerFieldWidth, "Flags:", pImageSepDbgHeader->Flags);
	str += strTemp;
    strTemp.Format("  %-*s%04Xh %s\r\n", headerFieldWidth, "Machine:", pImageSepDbgHeader->Machine, GetMachineTypeName(pImageSepDbgHeader->Machine, optionalHeader->MajorSubsystemVersion, optionalHeader->MinorSubsystemVersion));
	str += strTemp;
    strTemp.Format("  %-*s%04Xh\r\n", headerFieldWidth, "Characteristics:", pImageSepDbgHeader->Characteristics);
	str += strTemp;
    strTemp.Format("  %-*s%08Xh -> %s", headerFieldWidth, "TimeDateStamp:", pImageSepDbgHeader->TimeDateStamp, ctime((const time_t *)&pImageSepDbgHeader->TimeDateStamp) );
	str += strTemp;
    strTemp.Format("  %-*s%08Xh\r\n", headerFieldWidth, "CheckSum:", pImageSepDbgHeader->CheckSum);
	str += strTemp;
    strTemp.Format("  %-*s%08Xh\r\n", headerFieldWidth, "ImageBase:", pImageSepDbgHeader->ImageBase);
	str += strTemp;
    strTemp.Format("  %-*s%08Xh\r\n", headerFieldWidth, "Size of Image:", pImageSepDbgHeader->SizeOfImage);
	str += strTemp;
    strTemp.Format("  %-*s%04Xh\r\n", headerFieldWidth, "Number of Sections:", pImageSepDbgHeader->NumberOfSections);
	str += strTemp;
    strTemp.Format("  %-*s%04Xh\r\n", headerFieldWidth, "ExportedNamesSize:", pImageSepDbgHeader->ExportedNamesSize);
	str += strTemp;
    strTemp.Format("  %-*s%08Xh\r\n", headerFieldWidth, "DebugDirectorySize:", pImageSepDbgHeader->DebugDirectorySize);
	str += strTemp;
    strTemp.Format("  %-*s%08Xh\r\n", headerFieldWidth, "SectionAlignment:", pImageSepDbgHeader->SectionAlignment);
	str += strTemp;
	return str;
}

CString DumpDbgFile( PIMAGE_SEPARATE_DEBUG_HEADER pImageSepDbgHeader, PIMAGE_OPTIONAL_HEADER32 optionalHeader)
{
    CString str="", strTemp="";
    str += DumpImageDbgHeader(pImageSepDbgHeader, optionalHeader);
    str += "\r\n";
    
    str += DumpSectionTable( (PIMAGE_SECTION_HEADER)(pImageSepDbgHeader+1),
                        pImageSepDbgHeader->NumberOfSections, TRUE);
                    
    str += DumpDebugDirectory(
        MakePtr(PIMAGE_DEBUG_DIRECTORY,
        pImageSepDbgHeader, sizeof(IMAGE_SEPARATE_DEBUG_HEADER) +
        (pImageSepDbgHeader->NumberOfSections * sizeof(IMAGE_SECTION_HEADER))
        + pImageSepDbgHeader->ExportedNamesSize),
        pImageSepDbgHeader->DebugDirectorySize,
        (DWORD)pImageSepDbgHeader);
    
    str += ("\r\n");
    
    if ( g_pCOFFHeader )
	{
        str += DumpCOFFHeader( g_pCOFFHeader );
    
		str += ("\r\n");
#ifdef _DEBUG 
		if (g_pCOFFSymbolTable) AfxMessageBox("COFF Symbol Table not empty", MB_OK|MB_ICONEXCLAMATION);
#endif
		g_pCOFFSymbolTable = new COFFSymbolTable(
			MakePtr( PVOID, g_pCOFFHeader, g_pCOFFHeader->LvaToFirstSymbol),
			g_pCOFFHeader->NumberOfSymbols );


		str += DumpSymbolTable( g_pCOFFSymbolTable );

		delete g_pCOFFSymbolTable; g_pCOFFSymbolTable = NULL;
	}
	return str;
}
