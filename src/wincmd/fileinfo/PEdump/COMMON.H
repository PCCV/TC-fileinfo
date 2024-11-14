//==================================
// PEDUMP - Matt Pietrek 1997
// FILE: COMMON.H
//==================================
#include <afx.h>
#include "..\exetype.h"

// #include "..\..\..\commun32\RTFFormat.h"
#define rtfReturn "\r\n"

// MakePtr is a macro that allows you to easily add to values (including
// pointers) together without dealing with C's pointer arithmetic.  It
// essentially treats the last two parameters as DWORDs.  The first
// parameter is used to typecast the result to the appropriate pointer type.
#define MakePtr( cast, ptr, addValue ) (cast)( (DWORD)(ptr) + (DWORD)(addValue))

CString DumpHeader(PIMAGE_FILE_HEADER pImageFileHeader, PIMAGE_OPTIONAL_HEADER32 optionalHeader);
CString DumpOptionalHeader(PIMAGE_OPTIONAL_HEADER32 pImageOptionalHeader);
CString DumpSectionTable(PIMAGE_SECTION_HEADER section,
                      unsigned cSections,
                      BOOL IsEXE);
LPVOID GetSectionPtr(PSTR name, PIMAGE_NT_HEADERS32 pNTHeader, DWORD imageBase);
LPVOID GetPtrFromRVA( DWORD rva, PIMAGE_NT_HEADERS32 pNTHeader, DWORD imageBase );
PIMAGE_SECTION_HEADER GetSectionHeader(PSTR name, PIMAGE_NT_HEADERS32 pNTHeader);
PIMAGE_SECTION_HEADER GetEnclosingSectionHeader(DWORD rva,
                                                PIMAGE_NT_HEADERS32 pNTHeader);
CString DumpRawSectionData(PIMAGE_SECTION_HEADER section,
                        PVOID base,
                        unsigned cSections);
CString DumpDebugDirectory(PIMAGE_DEBUG_DIRECTORY debugDir, DWORD size, DWORD base);
CString DumpCOFFHeader(PIMAGE_COFF_SYMBOLS_HEADER pDbgInfo);
void HexDump(PBYTE ptr, DWORD length);

PSTR GetMachineTypeName( WORD wMachineType, WORD MajorSubsystemVersion, WORD MinorSubsystemVersion);

#define GetImgDirEntryRVA( pNTHdr, IDE ) \
   (pNTHdr->OptionalHeader.DataDirectory[IDE].VirtualAddress)

#define GetImgDirEntrySize( pNTHdr, IDE ) \
   (pNTHdr->OptionalHeader.DataDirectory[IDE].Size)

