//==================================
// PEDUMP - Matt Pietrek 1997
// FILE: LIBDUMP.C
//==================================

#include "stdafx.h"
#include "..\..\..\common\fdate.h"
#include "..\unmangle.h"
#ifndef __MEMMAPFL_H__
#include "..\..\common\memorymappedfile.h"
#endif

/*
int fShowRawSectionData=0;
int fShowLineNumbers=0;
int fShowSymbolTable=0;
int fShowRelocations=0;
int fShowUndecorated=1;
*/

// #include "common.h"
//#include "objdump.h"
CString  DumpObjFile( PIMAGE_FILE_HEADER pImageFileHeader, PIMAGE_OPTIONAL_HEADER32 optionalHeader);
// #include "libdump.h"
#include "extrnvar.h"

#define MakePtr( cast, ptr, addValue ) (cast)( (DWORD)(ptr) + (DWORD)(addValue))

PSTR PszLongnames = 0;

DWORD ConvertBigEndian(DWORD bigEndian);


CString DisplayArchiveMemberHeader(
    PIMAGE_ARCHIVE_MEMBER_HEADER pArchHeader,
    DWORD fileOffset )
{
	CString str, strTp;
//strTp.Format
//str += strTp
    strTp.Format("ARCHIVE MEMBER HEADER (%08X):\n", fileOffset);
	str += strTp;
	
    strTp.Format("\tName\t: %.16s", pArchHeader->Name);
	str += strTp;
    if ( pArchHeader->Name[0] == '/' && isdigit(pArchHeader->Name[1]) )
	{
		strTp.Format( "   (%s)", PszLongnames + atoi((char *)pArchHeader->Name+1) );
		str += strTp;
	}
    str += "\r\n";

	char szDateAsLong[64];
	sprintf( szDateAsLong, "%.12s", pArchHeader->Date );
	LONG dateAsLong = atol(szDateAsLong);
	
//    strTp.Format("  Date:     %l ", pArchHeader->Date);
	    strTp.Format("\tDate\t: %ld ", dateAsLong);
	str += strTp;
	strTp.Format( !(dateAsLong==0 || (dateAsLong == 0xFFFFFFFF))
			?" -> %s \r\n" 
			: "\r\n", FormatShortDate(CreateSystemeTime(dateAsLong)));
	str += strTp;

    
	strTp.Format("\tUserID\t: %.6s\n", pArchHeader->UserID);
	str += strTp;
    strTp.Format("\tGroupID\t: %.6s\n", pArchHeader->GroupID);
	str += strTp;
    strTp.Format("\tMode\t: %.8s\n", pArchHeader->Mode);
	str += strTp;
    strTp.Format("\tSize\t: %.10s\n", pArchHeader->Size);
	str += strTp;

	return str;
}

CString DumpFirstLinkerMember(PVOID p, DWORD MaxAddr)
{
	CString str, strTp;
//strTp.Format
//str += strTp;

    DWORD cSymbols = *(PDWORD)p;
    PDWORD pMemberOffsets = MakePtr( PDWORD, p, 4 );
    PSTR pSymbolName;
    unsigned i;

    cSymbols = ConvertBigEndian(cSymbols);
    pSymbolName = MakePtr( PSTR, pMemberOffsets, 4 * cSymbols );
    
    str += ("FIRST LINKER MEMBER\n");
    strTp.Format( "\tSymbols\t: %08X\n\n", cSymbols );
	str += strTp;
    str += ( "\tMbrOffs \tName\n\t-------------------- \t--------------------\n" );
        
    for ( i = 0; i < cSymbols; i++ )
    {
        DWORD offset = ConvertBigEndian( *pMemberOffsets );
		{	// Undecorate Name   
			if ((DWORD) pSymbolName < MaxAddr) { 	// test si l'adresse est correct
				PCHAR Textin = (PCHAR) pSymbolName;
				char Textout[1024];
				if ( fShowUndecorated )
				{
					if (strncmp("__imp_", Textin, 6))
						strTp.Format("\t%08X  \t%s\r\n", offset, Undecorate( Textin, Textout, 1024));
					else strTp ="";
				} else 
					strTp.Format("\t%08X  \t%s\r\n", offset, Textin);
				str += strTp;
			}
		}
        
//      strTp.Format("  %08X  %s\n", offset, pSymbolName);
//      str += strTp;
        pMemberOffsets++;
        pSymbolName += strlen(pSymbolName) + 1;
    }

	return str;
}

CString DumpSecondLinkerMember(PVOID p, DWORD MaxAddr)
{
	CString str, strTp;
//strTp.Format
//str += strTp;

    DWORD cArchiveMembers = *(PDWORD)p;
    PDWORD pMemberOffsets = MakePtr( PDWORD, p, 4 );
    DWORD cSymbols;
    PSTR pSymbolName;
    PWORD pIndices;
    unsigned i;

    cArchiveMembers = cArchiveMembers;

    // The number of symbols is in the DWORD right past the end of the
    // member offset array.
    cSymbols = pMemberOffsets[cArchiveMembers];

    pIndices = MakePtr( PWORD, p, 4 + cArchiveMembers * sizeof(DWORD) + 4 );

    pSymbolName = MakePtr( PSTR, pIndices, cSymbols * sizeof(WORD) );
    
    str += ("SECOND LINKER MEMBER:\n");
    
    strTp.Format( "\tArchive Members\t: %08X\n", cArchiveMembers );
    strTp.Format( "\tSymbols\t: %08X\n", cSymbols );
	str += strTp;
    str += ( "\tMbrOffs\tName\n\t-------------------- \t--------------------\n" );

    for ( i = 0; i < cSymbols; i++ )
    {
		{	// Undecorate Name   
			
			if ((DWORD) pSymbolName < MaxAddr) {	// test si l'adresse est correct
				PCHAR Textin = (PCHAR) pSymbolName;
				char Textout[1024];
				if ( fShowUndecorated )
				{
					if (strncmp("__imp_", Textin, 6))
						strTp.Format("\t%08X  \t%s\r\n", pMemberOffsets[pIndices[i] - 1], Undecorate( Textin, Textout, 1024));
					else strTp ="";
				} else 
					strTp.Format("\t%08X  \t%s\r\n", pMemberOffsets[pIndices[i] - 1], Textin);
				str += strTp;
			}
		}
//		strTp.Format("\t%08X  %s\n", pMemberOffsets[pIndices[i] - 1], pSymbolName);
//		str += strTp;
        pSymbolName += strlen(pSymbolName) + 1;
    }

	return str;
}

CString DumpLongnamesMember(PVOID p, DWORD len)
{
	CString str, strTp;
//strTp.Format
//str += strTp;
    PSTR pszName = (PSTR)p;
    DWORD offset = 0;

    PszLongnames = (PSTR)p;     // Save off pointer for use when dumping
                                // out OBJ member names

    str += ("Longnames:\n");
    
    // The longnames member is a series of null-terminated string.  Print
    // out the offset of each string (in decimal), followed by the string.
    while ( offset < len )
    {
        unsigned cbString = lstrlen( pszName )+1;

        strTp.Format("\t%05u\t: %s\n", offset, pszName);
		str += strTp;
        offset += cbString;
        pszName += cbString;
    }

	return str;
}

CString DumpLibFile( LPVOID ptr )
{
	MEMORY_MAPPED_FILE *libFile = ( MEMORY_MAPPED_FILE *) ptr;
	//PIMAGE_FILE_HEADER
	LPVOID lpFileBase =	(LPVOID) libFile->GetBase();

	CString str;
//strTp.Format
//str +=
    PIMAGE_ARCHIVE_MEMBER_HEADER pArchHeader;
    BOOL fSawFirstLinkerMember = FALSE;
    BOOL fSawSecondLinkerMember = FALSE;
    BOOL fBreak = FALSE;

    if ( strncmp((char *)lpFileBase,IMAGE_ARCHIVE_START,
                            		IMAGE_ARCHIVE_START_SIZE ) )
    {
        return ("Not a valid .LIB file - signature not found\n");
    }
    
    pArchHeader = MakePtr(PIMAGE_ARCHIVE_MEMBER_HEADER, lpFileBase, IMAGE_ARCHIVE_START_SIZE);
	if (libFile->IsValidPtr((DWORD) pArchHeader)) {
		while ( pArchHeader )
		{
			DWORD thisMemberSize;
			str += DisplayArchiveMemberHeader( pArchHeader, (PBYTE)pArchHeader - (PBYTE) lpFileBase );
			str += ("\n");

			if ( !strncmp( 	(char *)pArchHeader->Name,
        					IMAGE_ARCHIVE_LINKER_MEMBER, 16) )
			{
				if ( !fSawFirstLinkerMember )
				{
				str += DumpFirstLinkerMember( (PVOID)(pArchHeader + 1), libFile->GetBase()+libFile->GetFileSize() );
					str += ("\n");
					fSawFirstLinkerMember = TRUE;
				}
				else if ( !fSawSecondLinkerMember )
				{
					str += DumpSecondLinkerMember( (PVOID)(pArchHeader + 1), libFile->GetBase()+libFile->GetFileSize() );
					str += ("\n");
					fSawSecondLinkerMember = TRUE;
				}
			}
			else if( !strncmp(	(char *)pArchHeader->Name,
        						IMAGE_ARCHIVE_LONGNAMES_MEMBER, 16) )
			{
				str += DumpLongnamesMember( (PVOID)(pArchHeader + 1), atoi((char *)pArchHeader->Size) );
				str += ("\n");
			}
			else    // It's an OBJ file
			{
				str += DumpObjFile( (PIMAGE_FILE_HEADER)(pArchHeader + 1), (PIMAGE_OPTIONAL_HEADER32)(pArchHeader + 2));
			}

			// Calculate how big this member is (it's originally stored as as ASCII string.
			thisMemberSize = atoi((char *)pArchHeader->Size)+ IMAGE_SIZEOF_ARCHIVE_MEMBER_HDR;
			thisMemberSize = (thisMemberSize+1) & ~1;   // Round up
			// Get a pointer to the next archive member
			pArchHeader = MakePtr(PIMAGE_ARCHIVE_MEMBER_HEADER, pArchHeader, thisMemberSize);

			// test si le pointer exist!!
			if (libFile->IsValidPtr((DWORD) pArchHeader))
				break;
	//			return (str + "\nSize of archive member exceed file size !\n");

			// Bail out if we don't see the EndHeader signature in the next record
			if (strncmp( (char *)pArchHeader->EndHeader, IMAGE_ARCHIVE_END, 2))
				break;
		}
	}
	return str;
}

// Routine to convert from big endian to little endian
DWORD ConvertBigEndian(DWORD bigEndian)
{
	DWORD temp = 0;

	// printf( "bigEndian: %08X\n", bigEndian );

	temp |= bigEndian >> 24;
	temp |= ((bigEndian & 0x00FF0000) >> 8);
	temp |= ((bigEndian & 0x0000FF00) << 8);
	temp |= ((bigEndian & 0x000000FF) << 24);

	return temp;
}

