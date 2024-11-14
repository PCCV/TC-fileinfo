//==================================
// PEDUMP - Matt Pietrek 1997
// FILE: COMMON.C
//==================================
#include "stdafx.h"

#include "..\..\..\common\fdate.h"
#include "common.h"
#include "symboltablesupport.h"
#include "extrnvar.h"

/***************************
/
/	Voir WINNT;H pour les formats
/
/***************************/

PIMAGE_DEBUG_MISC g_pMiscDebugInfo = NULL;
PDWORD g_pCVHeader = NULL;
PIMAGE_COFF_SYMBOLS_HEADER g_pCOFFHeader = NULL;
COFFSymbolTable * g_pCOFFSymbolTable = NULL;

/*----------------------------------------------------------------------------*/
//
// Header related stuff
//
/*----------------------------------------------------------------------------*/

typedef struct
{
    WORD    flag;
    PSTR    name;
} WORD_FLAG_DESCRIPTIONS;

typedef struct
{
    DWORD   flag;
    PSTR    name;
} DWORD_FLAG_DESCRIPTIONS;

///////////////////////////////////////////////////////////////
//  A voir !!!!!!!!!!!!!!!!!!!
// http://giscenter.icc.ru:8082/scripts/WWWBinV.dll/ShowR?coff.rfi
//

// Bitfield values and names for the IMAGE_FILE_HEADER flags
WORD_FLAG_DESCRIPTIONS ImageFileHeaderCharacteristics[] = 
{
{ IMAGE_FILE_RELOCS_STRIPPED, "Relocation info stripped from file" },
{ IMAGE_FILE_EXECUTABLE_IMAGE, "File is executable  (i.e. no unresolved external references)" },
{ IMAGE_FILE_LINE_NUMS_STRIPPED, "COFF line numbers have been removed" },
{ IMAGE_FILE_LOCAL_SYMS_STRIPPED, "COFF symbol table entries for local symbols have been removed" },
{ IMAGE_FILE_AGGRESIVE_WS_TRIM, "Agressively trim working set" },
{ IMAGE_FILE_LARGE_ADDRESS_AWARE, "LARGE_ADDRESS_AWARE (can handle >2Gb Addresses)" },
{ IMAGE_FILE_BYTES_REVERSED_LO, "Little endian: LSB precedes MSB in memory" },
{ IMAGE_FILE_32BIT_MACHINE, "Machine based on 32-bit-word architecture" },
{ IMAGE_FILE_DEBUG_STRIPPED, "Debugging info stripped from file in .DBG file" },
{ IMAGE_FILE_REMOVABLE_RUN_FROM_SWAP, "If Image is on removable media, copy and run from the swap file" },
{ IMAGE_FILE_NET_RUN_FROM_SWAP, "If Image is on Net, copy and run from the swap file" },
{ IMAGE_FILE_SYSTEM, "The image file is a system file, not a user program" },
{ IMAGE_FILE_DLL, "File is a DLL" },
{ IMAGE_FILE_UP_SYSTEM_ONLY, "File should only be run on a UP machine" },
{ IMAGE_FILE_BYTES_REVERSED_HI, "Big endian: MSB precedes LSB in memory" }
// { IMAGE_FILE_MINIMAL_OBJECT, "MINIMAL_OBJECT" }, // Removed in NT 3.5
// { IMAGE_FILE_UPDATE_OBJECT, "UPDATE_OBJECT" },   // Removed in NT 3.5
// { IMAGE_FILE_16BIT_MACHINE, "16BIT_MACHINE" },   // Removed in NT 3.5
// { IMAGE_FILE_PATCH, "PATCH" },
};

#define NUMBER_IMAGE_HEADER_FLAGS \
    (sizeof(ImageFileHeaderCharacteristics) / sizeof(WORD_FLAG_DESCRIPTIONS))

//
// Dump the IMAGE_FILE_HEADER for a PE file or an OBJ
// common to 32 and 64 bits PE
CString DumpHeader(PIMAGE_FILE_HEADER pImageFileHeader, PIMAGE_OPTIONAL_HEADER32 optionalHeader)
{
    CString str="", strTemp="";
    UINT headerFieldWidth = 30;
    UINT i;
	str += ("FILE CHARACTERISTICS : \r\n");
    for ( i=0; i < NUMBER_IMAGE_HEADER_FLAGS; i++ )
    {
        if ( pImageFileHeader->Characteristics & ImageFileHeaderCharacteristics[i].flag )
		{
            strTemp.Format( "\t\t%s\r\n", ImageFileHeaderCharacteristics[i].name );
			str += strTemp;
		}
    }

    str += ("FILE HEADER :\r\n\r\n");

	WORD MajorSubsystemVersion = 0;
	WORD MinorSubsystemVersion = 0;
	if (optionalHeader != nullptr)
	{
		MajorSubsystemVersion = optionalHeader->MajorSubsystemVersion;
		MinorSubsystemVersion = optionalHeader->MinorSubsystemVersion;
	}

    strTemp.Format("\t%-*s\t%04Xh (%s)\r\n", headerFieldWidth, "Machine:", 
                pImageFileHeader->Machine,
                GetMachineTypeName(pImageFileHeader->Machine, MajorSubsystemVersion, MinorSubsystemVersion) );
	str += strTemp;
    strTemp.Format("\t%-*s\t%04Xh\r\n", headerFieldWidth, "Number of Sections:",
                pImageFileHeader->NumberOfSections);
	str += strTemp;

    strTemp.Format("\t%-*s\t%08Xh ", headerFieldWidth, "Time Date Stamp:", pImageFileHeader->TimeDateStamp);
	str += strTemp;
	strTemp.Format( !(pImageFileHeader->TimeDateStamp==0 || (pImageFileHeader->TimeDateStamp == 0xFFFFFFFF))
			?" -> %s \r\n" 
			: "\r\n", FormatShortDate(CreateSystemeTime(pImageFileHeader->TimeDateStamp)));
	str += strTemp;

    strTemp.Format("\t%-*s\t%08Xh\r\n", headerFieldWidth, "Symbols Pointer:",
                pImageFileHeader->PointerToSymbolTable);
	str += strTemp;
    strTemp.Format("\t%-*s\t%08Xh\r\n", headerFieldWidth, "Number Of Symbols:",
                pImageFileHeader->NumberOfSymbols);
	str += strTemp;
    strTemp.Format("\t%-*s\t%04Xh\r\n", headerFieldWidth, "Size Of Optional Header:",
                pImageFileHeader->SizeOfOptionalHeader);
	str += strTemp;
    strTemp.Format("\t%-*s\t%04Xh\r\n", headerFieldWidth, "Flags:",
                pImageFileHeader->Characteristics);
	str += strTemp;
	return str;
}

// #ifndef	IMAGE_DLLCHARACTERISTICS_WDM_DRIVER
#define IMAGE_DLLCHARACTERISTICS_NO_BIND 0x0800 // Do not bind image
#define IMAGE_DLLCHARACTERISTICS_WDM_DRIVER  0x2000     // Driver uses WDM model
#define IMAGE_DLLCHARACTERISTICS_TERMINAL_SERVER_AWARE 0x8000 //Image is Terminal Server aware
// #endif

// Marked as obsolete in MSDN CD 9
// Bitfield values and names for the DllCharacteritics flags
WORD_FLAG_DESCRIPTIONS DllCharacteristics[] = 
{
	{ 1, "Reserved" },
	{ 2, "Reserved" },
	{ 4, "Reserved" },
	{ 8, "Reserved" },
	{ IMAGE_DLLCHARACTERISTICS_NO_BIND, "Do not bind image" },
	{ IMAGE_DLLCHARACTERISTICS_WDM_DRIVER, "Driver uses a WDM model" },
	{ IMAGE_DLLCHARACTERISTICS_TERMINAL_SERVER_AWARE, "Image is Terminal Server aware"},
};
#define NUMBER_DLL_CHARACTERISTICS \
    (sizeof(DllCharacteristics) / sizeof(WORD_FLAG_DESCRIPTIONS))

#if 0
// Marked as obsolete in MSDN CD 9
// Bitfield values and names for the LoaderFlags flags
DWORD_FLAG_DESCRIPTIONS LoaderFlags[] = 
{
{ IMAGE_LOADER_FLAGS_BREAK_ON_LOAD, "BREAK_ON_LOAD" },
{ IMAGE_LOADER_FLAGS_DEBUG_ON_LOAD, "DEBUG_ON_LOAD" }
};
#define NUMBER_LOADER_FLAGS \
    (sizeof(LoaderFlags) / sizeof(DWORD_FLAG_DESCRIPTIONS))
#endif

// Names of the data directory elements that are defined
char *ImageDirectoryNames[] = {
    "Export Directory", "Import Directory", "Resource Directory", "Exception table", "Security table", "Base Relocation table",
    "Debug Directory", 
	"Architecture Specific Data", "Global Pointer", "TLS Directory", "Load config table",
    "Bound Import table", "Import Address Table",  // These two entries added for NT 3.51
	"Delay import descriptor", "COM descriptor" };		// This entry added in NT 5

#define NUMBER_IMAGE_DIRECTORY_ENTRYS \
    (sizeof(ImageDirectoryNames)/sizeof(char *))


#define IMAGE_SUBSYSTEM_EFI_APPLICATION 10
#define IMAGE_SUBSYSTEM_EFI_BOOT_SERVICE_DRIVER 11
#define IMAGE_SUBSYSTEM_EFI_RUNTIME_DRIVER 12
#define IMAGE_SUBSYSTEM_EFI_ROM 13
#define IMAGE_SUBSYSTEM_XBOX 14
#define IMAGE_SUBSYSTEM_WINDOWS_BOOT_APPLICATION 16
//
// Dump the IMAGE_OPTIONAL_HEADER from a PE file
// 64 bits ready
CString DumpOptionalHeader(PIMAGE_OPTIONAL_HEADER32 optionalHeader)
{
    CString str="", strTemp="";
    UINT width = 30;
    char *s;
    UINT i;
    
    str += ("OPTIONAL HEADER :\r\n");

    strTemp.Format("\t%-*s\t%04Xh ", width, "Magic", optionalHeader->Magic);
	str += strTemp;
	switch(optionalHeader->Magic)
	{
		case 0x10b: strTemp = "( PE32 : normal 32-bit ) \r\n"; break;
		case 0x20b: strTemp = "( PE32+ : 64-bit ready ) \r\n"; break;
		case 0x107: strTemp = "( ROM image ) \r\n"; break;
		default :  strTemp = "( unknown ) \r\n";
	}
	str += strTemp;

    strTemp.Format("\t%-*s\t%u.%02u\r\n", width, "Linker version",
        optionalHeader->MajorLinkerVersion,
        optionalHeader->MinorLinkerVersion);
	str += strTemp;
    strTemp.Format("\t%-*s\t%08Xh\r\n", width, "Size of code", optionalHeader->SizeOfCode);
	str += strTemp;
    strTemp.Format("\t%-*s\t%08Xh\r\n", width, "Size of initialized data",
        optionalHeader->SizeOfInitializedData);
	str += strTemp;
    strTemp.Format("\t%-*s\t%08Xh\r\n", width, "Size of uninitialized data",
        optionalHeader->SizeOfUninitializedData);
	str += strTemp;
    strTemp.Format("\t%-*s\t%08Xh\r\n", width, "Address of Entry Point (RVA) ",
        optionalHeader->AddressOfEntryPoint);
	str += strTemp;
	
    strTemp.Format("\t%-*s\t%08Xh\r\n", width, "Base of code", optionalHeader->BaseOfCode);
	str += strTemp;

	if (optionalHeader->Magic != 0x10b) // 64bits
	{
	    strTemp.Format("\t%-*s\t%08X%08Xh\r\n", width, "Image base", optionalHeader->ImageBase ,optionalHeader->BaseOfData);
		str += strTemp;
	} else 
	{
	    strTemp.Format("\t%-*s\t%08Xh\r\n", width, "Base of data", optionalHeader->BaseOfData);
		str += strTemp;
		strTemp.Format("\t%-*s\t%08Xh\r\n", width, "Image base", optionalHeader->ImageBase);
		str += strTemp;
	}

	strTemp.Format("\t%-*s\t%08Xh\r\n", width, "Section Alignment",
        optionalHeader->SectionAlignment);
	str += strTemp;
    strTemp.Format("\t%-*s\t%08Xh\r\n", width, "File Alignment", optionalHeader->FileAlignment);
	str += strTemp;
    strTemp.Format("\t%-*s\t%u.%02u\r\n", width, "Required OS version",
        optionalHeader->MajorOperatingSystemVersion,
        optionalHeader->MinorOperatingSystemVersion);
	str += strTemp;
    strTemp.Format("\t%-*s\t%u.%02u\r\n", width, "Image version",
        optionalHeader->MajorImageVersion,
        optionalHeader->MinorImageVersion);
	str += strTemp;
    strTemp.Format("\t%-*s\t%u.%02u\r\n", width, "Subsystem version",
        optionalHeader->MajorSubsystemVersion,
        optionalHeader->MinorSubsystemVersion);
	str += strTemp;
    strTemp.Format("\t%-*s\t%X\r\n", width, "Reserved1", optionalHeader->Win32VersionValue); //"Win32 Version"
	str += strTemp;
    strTemp.Format("\t%-*s\t%08Xh ( %lu bytes)\r\n", width, "Size of image", optionalHeader->SizeOfImage, optionalHeader->SizeOfImage);
	str += strTemp;
    strTemp.Format("\t%-*s\t%08Xh\r\n", width, "Size of headers", optionalHeader->SizeOfHeaders);
	str += strTemp;
    strTemp.Format("\t%-*s\t%08Xh\r\n", width, "Checksum", optionalHeader->CheckSum);
	str += strTemp;
    switch( optionalHeader->Subsystem )
    {
        case IMAGE_SUBSYSTEM_NATIVE: s = "Native : Image doesn't require a subsystem"; break;
        case IMAGE_SUBSYSTEM_WINDOWS_GUI: s = "Image runs in the Windows GUI subsystem"; break;
        case IMAGE_SUBSYSTEM_WINDOWS_CUI: s = "Image runs in the Windows character subsystem"; break;
        case IMAGE_SUBSYSTEM_OS2_CUI: s = "Image runs in the OS/2 character subsystem"; break;
        case IMAGE_SUBSYSTEM_POSIX_CUI: s = "Image runs in the Posix character subsystem"; break;
		case IMAGE_SUBSYSTEM_NATIVE_WINDOWS: s = "Image is a native Win9x driver"; break;
		case IMAGE_SUBSYSTEM_WINDOWS_CE_GUI: s = "Image runs in the Windows CE subsystem"; break;
		case IMAGE_SUBSYSTEM_EFI_APPLICATION:s = "Image is an EFI application"; break;
		case IMAGE_SUBSYSTEM_EFI_BOOT_SERVICE_DRIVER:s = "Image is an EFI driver that provides boot services"; break;
		case IMAGE_SUBSYSTEM_EFI_RUNTIME_DRIVER:s = "Image is an EFI driver that provides runtime services"; break;
		case IMAGE_SUBSYSTEM_EFI_ROM : s = "Image is an EFI ROM"; break;
		case IMAGE_SUBSYSTEM_XBOX: s = "Image run in a Xbox System"; break;
		case IMAGE_SUBSYSTEM_WINDOWS_BOOT_APPLICATION : s = "Image run as a Windows boot application"; break;
        default: s = "unknown";
    }
    strTemp.Format("\t%-*s\t%04Xh (%s)\r\n", width, "Subsystem", optionalHeader->Subsystem, s);
	str += strTemp;

// Marked as obsolete in MSDN CD 9
    strTemp.Format("\t%-*s\t%04Xh\r\n", width, "DLL Characteristics", optionalHeader->DllCharacteristics);
	str += strTemp;
    for ( i=0; i < NUMBER_DLL_CHARACTERISTICS; i++ )
    {
        if ( optionalHeader->DllCharacteristics &  DllCharacteristics[i].flag )
		{
            strTemp.Format( "\t%-*s\t%s", width, " ", DllCharacteristics[i].name );
			str += strTemp;
		}
    }
    if ( optionalHeader->DllCharacteristics )
        str += ("\r\n");

	if (optionalHeader->Magic == 0x10b)
	{

		strTemp.Format("\t%-*s\t%08Xh\r\n", width, "Size of Stack Reserve", optionalHeader->SizeOfStackReserve);
		str += strTemp;
		strTemp.Format("\t%-*s\t%08Xh\r\n", width, "Size of Stack Commit", optionalHeader->SizeOfStackCommit);
		str += strTemp;
		strTemp.Format("\t%-*s\t%08Xh\r\n", width, "Size of Heap Reserve", optionalHeader->SizeOfHeapReserve);
		str += strTemp;
		strTemp.Format("\t%-*s\t%08Xh\r\n", width, "Size of Heap Commit", optionalHeader->SizeOfHeapCommit);
		str += strTemp;

	// Marked as obsolete in MSDN CD 9
		strTemp.Format("\t%-*s\t%08Xh (obsolete)\r\n", width, "loader flags", optionalHeader->LoaderFlags);
		str += strTemp;
	#if 0
		for ( i=0; i < NUMBER_LOADER_FLAGS; i++ )
		{
			if ( optionalHeader->LoaderFlags &  LoaderFlags[i].flag )
			{
				strTemp.Format( "\t%s", LoaderFlags[i].name );
				str += strTemp;
			}
		}
		if ( optionalHeader->LoaderFlags )  str += ("\r\n");
	#endif
												// Size !!!
		strTemp.Format("\t%-*s\t%08Xh\r\n", width, "Number of Data Directory", optionalHeader->NumberOfRvaAndSizes); //"RVAs & sizes"
		str += strTemp;

		str += ("\r\nDATA DIRECTORY (Virtual Address and Size)\r\n");
		for ( i=0; i < optionalHeader->NumberOfRvaAndSizes; i++)
		{
			strTemp.Format( "\t%-12s  \trva: ",
				(i >= NUMBER_IMAGE_DIRECTORY_ENTRYS)
					? "unused" : ImageDirectoryNames[i]);
			str += strTemp;
			if (optionalHeader->DataDirectory[i].VirtualAddress != 0)
			{
				strTemp.Format( "%08Xh  \tsize: %08Xh\r\n",
					optionalHeader->DataDirectory[i].VirtualAddress,
					optionalHeader->DataDirectory[i].Size);
			} else strTemp = "\r\n";
			str += strTemp;
		}
	}
	else  //64bits
	{
		PIMAGE_OPTIONAL_HEADER64 OpHdr64 = (PIMAGE_OPTIONAL_HEADER64) optionalHeader;
		strTemp.Format("\t%-*s\t%016Xh\r\n", width, "Size of Stack Reserve", OpHdr64->SizeOfStackReserve);
		str += strTemp;
		strTemp.Format("\t%-*s\t%016Xh\r\n", width, "Size of Stack Commit", OpHdr64->SizeOfStackCommit);
		str += strTemp;
		strTemp.Format("\t%-*s\t%016Xh\r\n", width, "Size of Heap Reserve", OpHdr64->SizeOfHeapReserve);
		str += strTemp;
		strTemp.Format("\t%-*s\t%016Xh\r\n", width, "Size of Heap Commit", OpHdr64->SizeOfHeapCommit);
		str += strTemp;

	// Marked as obsolete in MSDN CD 9
		strTemp.Format("\t%-*s\t%08Xh (obsolete)\r\n", width, "loader flags", OpHdr64->LoaderFlags);
		str += strTemp;
	#if 0
		for ( i=0; i < NUMBER_LOADER_FLAGS; i++ )
		{
			if ( OpHdr64->LoaderFlags &  LoaderFlags[i].flag )
			{
				strTemp.Format( "\t%s", LoaderFlags[i].name );
				str += strTemp;
			}
		}
		if ( OpHdr64->LoaderFlags )  str += ("\r\n");
	#endif

												// Size !!!
		strTemp.Format("\t%-*s\t%08Xh\r\n", width, "Number of Data Directory", OpHdr64->NumberOfRvaAndSizes); //"RVAs & sizes"
		str += strTemp;

		str += ("\r\nDATA DIRECTORY (Virtual Address and Size)\r\n");
		for ( i=0; i < OpHdr64->NumberOfRvaAndSizes; i++)
		{
			strTemp.Format( "\t%-12s  \trva: ",
				(i >= NUMBER_IMAGE_DIRECTORY_ENTRYS)
					? "unused" : ImageDirectoryNames[i]);
			str += strTemp;
			if (OpHdr64->DataDirectory[i].VirtualAddress != 0)
			{
				strTemp.Format( "%08Xh  \tsize: %08Xh\r\n",
					OpHdr64->DataDirectory[i].VirtualAddress,
					OpHdr64->DataDirectory[i].Size);
			} else strTemp = "\r\n";
			str += strTemp;
		}
	}

	return str;
}

#define IMAGE_FILE_MACHINE_UNKNOWN           0
#define IMAGE_FILE_MACHINE_I386              0x014c  // Intel 386.
#define	IMAGE_FILE_MACHINE_I860				 0x14d

#define IMAGE_FILE_MACHINE_R3000             0x0162  // MIPS little-endian, 0x160 big-endian
#define IMAGE_FILE_MACHINE_R4000             0x0166  // MIPS little-endian
#define IMAGE_FILE_MACHINE_R10000            0x0168  // MIPS little-endian
#define IMAGE_FILE_MACHINE_WCEMIPSV2         0x0169  // MIPS little-endian WCE v2
#define IMAGE_FILE_MACHINE_ALPHA             0x0184  // Alpha_AXP
#define IMAGE_FILE_MACHINE_SH3               0x01a2  // SH3 little-endian
#define IMAGE_FILE_MACHINE_SH3DSP            0x01a3
#define IMAGE_FILE_MACHINE_SH3E              0x01a4  // SH3E little-endian
#define IMAGE_FILE_MACHINE_SH4               0x01a6  // SH4 little-endian
#define IMAGE_FILE_MACHINE_SH5               0x01a8  // SH5
#define IMAGE_FILE_MACHINE_ARM               0x01c0  // ARM Little-Endian
#define IMAGE_FILE_MACHINE_THUMB             0x01c2
#define IMAGE_FILE_MACHINE_AM33              0x01d3
#define IMAGE_FILE_MACHINE_POWERPC           0x01F0  // IBM PowerPC Little-Endian
#define IMAGE_FILE_MACHINE_POWERPCFP         0x01f1

#define IMAGE_FILE_MACHINE_IA64              0x0200  // Intel 64
#define IMAGE_FILE_MACHINE_MIPS16            0x0266  // MIPS

#define IMAGE_FILE_MACHINE_M68K              0x0268
#define IMAGE_FILE_MACHINE_ALPHA64           0x0284  // ALPHA64
#define IMAGE_FILE_MACHINE_AXP64             IMAGE_FILE_MACHINE_ALPHA64

#define IMAGE_FILE_MACHINE_MIPSFPU           0x0366  // MIPS
#define IMAGE_FILE_MACHINE_MIPSFPU16         0x0466  // MIPS

#define IMAGE_FILE_MACHINE_TRICORE           0x0520  // Infineon
#define IMAGE_FILE_MACHINE_EBC               0x0EBC  // EFI Byte Code
#define IMAGE_FILE_MACHINE_AMD64_ARM64EC     0x8664  // AMD64 (K8) or ARM64EC depending on Subsystem 
#define IMAGE_FILE_MACHINE_M32R              0x9041  // M32R little-endian

#define IMAGE_FILE_MACHINE_ARM64             0xAA64  // ARM64

#define IMAGE_FILE_MACHINE_CEE               0xC0EE
#define IMAGE_FILE_MACHINE_CEF               0x0CEF

PSTR GetMachineTypeName( WORD wMachineType, WORD MajorSubsystemVersion,WORD MinorSubsystemVersion)
{
    switch( wMachineType )
    {
        case IMAGE_FILE_MACHINE_I386	:return "i386 or later, and compatible";
        case IMAGE_FILE_MACHINE_I860	:return "i860";
		case IMAGE_FILE_MACHINE_WCEMIPSV2:
		case IMAGE_FILE_MACHINE_MIPS16	:
		case IMAGE_FILE_MACHINE_MIPSFPU	:
		case IMAGE_FILE_MACHINE_MIPSFPU16:return "MIPS type processor";
        case IMAGE_FILE_MACHINE_R3000	:return "R3000 = MIPS little-endian";
		case 0x160						:return "R3000 = MIPS big endian";
        case IMAGE_FILE_MACHINE_R4000	:return "R4000 = MIPS little-endian";
		case IMAGE_FILE_MACHINE_R10000	:return "R10000 = MIPS little-endian";
        case IMAGE_FILE_MACHINE_ALPHA	:return "Alpha AXP";
        case IMAGE_FILE_MACHINE_POWERPCFP:
		case IMAGE_FILE_MACHINE_POWERPC :return "IBM PowerPC Little-Endian";
//		case IMAGE_FILE_MACHINE_AXP64   :
		case IMAGE_FILE_MACHINE_IA64    :return "Intel IA64";
		case IMAGE_FILE_MACHINE_ALPHA64 :return "Alpha AXP 64-bit";
		case IMAGE_FILE_MACHINE_M68K	:return "Motorola 68000 series";
		case IMAGE_FILE_MACHINE_THUMB   :
		case IMAGE_FILE_MACHINE_ARM		:return "ARM or compatible";
		case IMAGE_FILE_MACHINE_SH3DSP  :
		case IMAGE_FILE_MACHINE_SH3E    :
		case IMAGE_FILE_MACHINE_SH3     :return "Hitachi SH3 type processor";
//		case IMAGE_FILE_MACHINE_SH3		:return "Intel PXA250 or compatible";
		case IMAGE_FILE_MACHINE_SH4     :return "Hitachi SH4 type processor";
		case IMAGE_FILE_MACHINE_SH5     :return "Hitachi SH5 type processor";

		case IMAGE_FILE_MACHINE_AM33    :return "AM33";
		case IMAGE_FILE_MACHINE_TRICORE :return "Infineon";// Infineon
		case IMAGE_FILE_MACHINE_EBC     :return "EFI Byte Code";
		case IMAGE_FILE_MACHINE_AMD64_ARM64EC:
		{
			if (
				(MajorSubsystemVersion == 5) && (MinorSubsystemVersion == 2)
				|| (MajorSubsystemVersion == 6) && (MinorSubsystemVersion == 2)
				)
				return "ARM64EC";
			return "AMD64 (K8)";
		}

		case IMAGE_FILE_MACHINE_M32R    :return "M32R little-endian";
		case IMAGE_FILE_MACHINE_ARM64	:return "ARM64";
		case IMAGE_FILE_MACHINE_CEE     :return "CEE";
		case IMAGE_FILE_MACHINE_CEF     :return "CEF"; // ?

        default:    					return "unknown";
    }
}


/*----------------------------------------------------------------------------*/
//
// Section related stuff
//
/*----------------------------------------------------------------------------*/

//
// These aren't defined in the NT 4.0 WINNT.H, so we'll define them here
//
#ifndef IMAGE_SCN_TYPE_DSECT
#define IMAGE_SCN_TYPE_DSECT                 0x00000001  // Reserved.
#endif
#ifndef IMAGE_SCN_TYPE_NOLOAD
#define IMAGE_SCN_TYPE_NOLOAD                0x00000002  // Reserved.
#endif
#ifndef IMAGE_SCN_TYPE_GROUP
#define IMAGE_SCN_TYPE_GROUP                 0x00000004  // Reserved.
#endif
#ifndef IMAGE_SCN_TYPE_COPY
#define IMAGE_SCN_TYPE_COPY                  0x00000010  // Reserved.
#endif
#ifndef IMAGE_SCN_TYPE_OVER
#define IMAGE_SCN_TYPE_OVER                  0x00000400  // Reserved.
#endif
#ifndef IMAGE_SCN_MEM_PROTECTED
#define IMAGE_SCN_MEM_PROTECTED              0x00004000
#endif
#ifndef IMAGE_SCN_MEM_SYSHEAP
#define IMAGE_SCN_MEM_SYSHEAP                0x00010000
#endif

// Bitfield values and names for the IMAGE_SECTION_HEADER flags
DWORD_FLAG_DESCRIPTIONS SectionCharacteristics[] = 
{

{ IMAGE_SCN_TYPE_DSECT, "DSECT" },
{ IMAGE_SCN_TYPE_NOLOAD, "NOLOAD" },
{ IMAGE_SCN_TYPE_GROUP, "GROUP" },
{ IMAGE_SCN_TYPE_NO_PAD, "NO_PAD" },
{ IMAGE_SCN_TYPE_COPY, "COPY" },
{ IMAGE_SCN_CNT_CODE, "CODE" },
{ IMAGE_SCN_CNT_INITIALIZED_DATA, "INITIALIZED_DATA" },
{ IMAGE_SCN_CNT_UNINITIALIZED_DATA, "UNINITIALIZED_DATA" },
{ IMAGE_SCN_LNK_OTHER, "OTHER" },
{ IMAGE_SCN_LNK_INFO, "INFO" },
{ IMAGE_SCN_TYPE_OVER, "OVER" },
{ IMAGE_SCN_LNK_REMOVE, "REMOVE" },
{ IMAGE_SCN_LNK_COMDAT, "COMDAT" },
{ IMAGE_SCN_MEM_PROTECTED, "PROTECTED" },
#define IMAGE_SCN_NO_DEFER_SPEC_EXC          0x00004000  // Reset speculative exceptions handling bits in the TLB entries for this section.
#define IMAGE_SCN_GPREL                      0x00008000  // Section content can be accessed relative to GP
{ IMAGE_SCN_MEM_FARDATA, "FARDATA" },
{ IMAGE_SCN_MEM_SYSHEAP, "SYSHEAP" },
{ IMAGE_SCN_MEM_PURGEABLE, "PURGEABLE" },
{ IMAGE_SCN_MEM_LOCKED, "LOCKED" },
{ IMAGE_SCN_MEM_PRELOAD, "PRELOAD" },
{ IMAGE_SCN_LNK_NRELOC_OVFL, "NRELOC_OVFL" },
{ IMAGE_SCN_MEM_DISCARDABLE, "DISCARDABLE" },
{ IMAGE_SCN_MEM_NOT_CACHED, "NOT_CACHED" },
{ IMAGE_SCN_MEM_NOT_PAGED, "NOT_PAGED" },
{ IMAGE_SCN_MEM_SHARED, "SHARED" },
{ IMAGE_SCN_MEM_EXECUTE, "EXECUTE" },
{ IMAGE_SCN_MEM_READ, "READ" },
{ IMAGE_SCN_MEM_WRITE, "WRITE" },
};

#define NUMBER_SECTION_CHARACTERISTICS \
    (sizeof(SectionCharacteristics) / sizeof(DWORD_FLAG_DESCRIPTIONS))

//
// Dump the section table from a PE file or an OBJ
//
CString DumpSectionTable(PIMAGE_SECTION_HEADER section,
                      unsigned cSections,
                      BOOL IsEXE)
{
    CString str="", strTemp="";
    unsigned i, j;
	PSTR pszAlign;

    str += ("SECTION TABLE\r\n");
    
    for ( i=1; i <= cSections; i++, section++ )
    {
/*        strTemp.Format( "%02X %-8.8s  \r\n\t%s: \t%08Xh  VirtAddr:      \t%08Xh\r\n",
                i, section->Name,
                IsEXE ? "VirtSize" : "PhysAddr",
                section->Misc.VirtualSize, section->VirtualAddress);
*/
        strTemp.Format( "%02X %-8.8s  \r\n\tVirtAddr:\t%08Xh  %s: \t%08Xh\r\n",
                i, section->Name,
				section->VirtualAddress,
				IsEXE ? "VirtSize" : "PhysAddr",
                section->Misc.VirtualSize);

		str += strTemp;
        strTemp.Format( "\traw data offs:   \t%08Xh  raw data size: \t%08Xh\r\n",
                section->PointerToRawData, section->SizeOfRawData );
		str += strTemp;
        strTemp.Format( "\trelocation offs: \t%08Xh  relocations:   \t%08Xh\r\n",
                section->PointerToRelocations, section->NumberOfRelocations );
		str += strTemp;
        strTemp.Format( "\tline # offs:     \t%08Xh  line #'s:      \t%08Xh\r\n",
                section->PointerToLinenumbers, section->NumberOfLinenumbers );
		str += strTemp;
        strTemp.Format( "\tcharacteristics: \t%08Xh\r\n", section->Characteristics);
		str += strTemp;

        str += ("\t");
        for ( j=0; j < NUMBER_SECTION_CHARACTERISTICS; j++ )
        {
            if ( section->Characteristics &  SectionCharacteristics[j].flag )
			{
				strTemp.Format( "%s  ", SectionCharacteristics[j].name );
				str += strTemp;
			}
        }
		
		switch( section->Characteristics & IMAGE_SCN_ALIGN_64BYTES )
		{
			case IMAGE_SCN_ALIGN_1BYTES: pszAlign = "ALIGN_1BYTES"; break;
			case IMAGE_SCN_ALIGN_2BYTES: pszAlign = "ALIGN_2BYTES"; break;
			case IMAGE_SCN_ALIGN_4BYTES: pszAlign = "ALIGN_4BYTES"; break;
			case IMAGE_SCN_ALIGN_8BYTES: pszAlign = "ALIGN_8BYTES"; break;
			case IMAGE_SCN_ALIGN_16BYTES: pszAlign = "ALIGN_16BYTES"; break;
			case IMAGE_SCN_ALIGN_32BYTES: pszAlign = "ALIGN_32BYTES"; break;
			case IMAGE_SCN_ALIGN_64BYTES: pszAlign = "ALIGN_64BYTES"; break;
			default: pszAlign = "ALIGN_DEFAULT(16)"; break;
		}
		strTemp.Format( "%s  ", pszAlign );
		str += strTemp;
        str += ("\r\n");
    }
	str += ("\r\n");
	return str;
}

//
// Given a section name, look it up in the section table and return a
// pointer to the start of its raw data area.
//
LPVOID GetSectionPtr(PSTR name, PIMAGE_NT_HEADERS32 pNTHeader, DWORD imageBase)
{
    PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(pNTHeader);
    unsigned i;
    
    for ( i=0; i < pNTHeader->FileHeader.NumberOfSections; i++, section++ )
    {
        if (strncmp((char *)section->Name, name, IMAGE_SIZEOF_SHORT_NAME) == 0)
            return (LPVOID)(section->PointerToRawData + imageBase);
    }
    
    return 0;
}

LPVOID GetPtrFromRVA( DWORD rva, PIMAGE_NT_HEADERS32 pNTHeader, DWORD imageBase )
{
	PIMAGE_SECTION_HEADER pSectionHdr;
	INT delta;
		
	pSectionHdr = GetEnclosingSectionHeader( rva, pNTHeader );
	if ( !pSectionHdr )
		return 0;

	delta = (INT)(pSectionHdr->VirtualAddress-pSectionHdr->PointerToRawData);
	return (PVOID) ( imageBase + rva - delta );
}

//
// Given a section name, look it up in the section table and return a
// pointer to its IMAGE_SECTION_HEADER
PIMAGE_SECTION_HEADER GetSectionHeader(PSTR name, PIMAGE_NT_HEADERS32 pNTHeader)
{
    PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(pNTHeader);
    unsigned i;
    for ( i=0; i < pNTHeader->FileHeader.NumberOfSections; i++, section++ )
    {
        if ( 0 == strncmp((char *)section->Name,name,IMAGE_SIZEOF_SHORT_NAME) )
            return section;
    }
    return 0;
}

//
// Given an RVA, look up the section header that encloses it and return a
// pointer to its IMAGE_SECTION_HEADER
//
PIMAGE_SECTION_HEADER GetEnclosingSectionHeader(DWORD rva,
                                                PIMAGE_NT_HEADERS32 pNTHeader)
{
    PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(pNTHeader);
    unsigned i;
    
    for ( i=0; i < pNTHeader->FileHeader.NumberOfSections; i++, section++ )
    {
        // Is the RVA within this section?
        if ( (rva >= section->VirtualAddress) && 
             (rva < (section->VirtualAddress + section->Misc.VirtualSize)))
            return section;
    }
    
    return 0;
}


char *SzDebugFormats[] = {
"UNKNOWN/BORLAND","COFF","CODEVIEW","FPO","MISC","EXCEPTION","FIXUP",
"OMAP_TO_SRC", "OMAP_FROM_SRC", "BORLAND", "RESERVED"};
#define NBDEBUGTYPE 11
//
// Dump the debug directory array
//
CString DumpDebugDirectory(PIMAGE_DEBUG_DIRECTORY debugDir, DWORD size, DWORD base)
{
	PIMAGE_COFF_SYMBOLS_HEADER PCOFFDebugInfo = 0;
    CString str="", Temp="";
    DWORD cDebugFormats = size / sizeof(IMAGE_DEBUG_DIRECTORY);
    PSTR szDebugFormat;
    unsigned i;
    
    if ( cDebugFormats == 0 ) return str;
    
    str += "DEBUG Format in File\r\n";
    for ( i=0; i < cDebugFormats; i++ )
    {
        szDebugFormat = (debugDir->Type < NBDEBUGTYPE )
                        ? SzDebugFormats[debugDir->Type] : "???";

		Temp.Format("\tType\t%s\r\n", szDebugFormat);
		str +=Temp;
		Temp.Format("\tSize\t%08Xh\r\n", debugDir->SizeOfData);
		str +=Temp;
		Temp.Format("\tAddress\t%08Xh\r\n", debugDir->AddressOfRawData);
		str +=Temp;
		Temp.Format("\tFile Pointer\t%08Xh\r\n", debugDir->PointerToRawData);
		str +=Temp;
		Temp.Format("\tCharacteristics\t%08Xh\r\n", debugDir->Characteristics);
		str +=Temp;
		Temp.Format("\tTimeDate\t%08Xh ", debugDir->TimeDateStamp);
		str +=Temp;
		Temp.Format( !(debugDir->TimeDateStamp==0 || (debugDir->TimeDateStamp == 0xFFFFFFFF))
				?" -> %s \r\n" 
				: "\r\n", FormatShortDate(CreateSystemeTime(debugDir->TimeDateStamp)));
		str += Temp;
		Temp.Format("\tVersion\t%u.%02u\r\n", debugDir->MajorVersion, debugDir->MinorVersion);
		str +=Temp;

		switch( debugDir->Type )
		{
        	case IMAGE_DEBUG_TYPE_COFF:
	            g_pCOFFHeader =
                (PIMAGE_COFF_SYMBOLS_HEADER)(base+ debugDir->PointerToRawData);
				break;

			case IMAGE_DEBUG_TYPE_MISC:
				g_pMiscDebugInfo =
				(PIMAGE_DEBUG_MISC)(base + debugDir->PointerToRawData);
				break;

			case IMAGE_DEBUG_TYPE_CODEVIEW:
				g_pCVHeader = (PDWORD)(base + debugDir->PointerToRawData);
				break;
		}

        debugDir++;
    }
	str += "\r\n";
	return str;
}

/*----------------------------------------------------------------------------*/
//
// Other assorted stuff
//
/*----------------------------------------------------------------------------*/

//
// Do a hexadecimal dump of the raw data for all the sections.  You
// could just dump one section by adjusting the PIMAGE_SECTION_HEADER
// and cSections parameters
//
CString DumpRawSectionData(PIMAGE_SECTION_HEADER section,
                        PVOID base,
                        unsigned cSections)
{
    CString str="", strTemp="";
    unsigned i;
    char name[IMAGE_SIZEOF_SHORT_NAME + 1];

    str += ("SECTION HEX Dumps\r\n");
    
    for ( i=1; i <= cSections; i++, section++ )
    {
        // Make a copy of the section name so that we can ensure that
        // it's null-terminated
        memcpy(name, section->Name, IMAGE_SIZEOF_SHORT_NAME);
        name[IMAGE_SIZEOF_SHORT_NAME] = 0;

        // Don't dump sections that don't exist in the file!
        if ( section->PointerToRawData == 0 )
            continue;
        
        strTemp.Format( "section %02Xh (%s)  size: %08Xh  file offs: %08Xh\r\n",
                i, name, section->SizeOfRawData, section->PointerToRawData);
		str += strTemp;

        HexDump( MakePtr(PBYTE, base, section->PointerToRawData),
                 section->SizeOfRawData );
        str += ("\r\n");
    }
	return str;
}

// Number of hex values displayed per line
#define HEX_DUMP_WIDTH 16

//
// Dump a region of memory in a hexadecimal format
//
void HexDump(PBYTE ptr, DWORD length)
{
    char buffer[256];
    PSTR buffPtr, buffPtr2;
    unsigned cOutput, i;
    DWORD bytesToGo=length;

    while ( bytesToGo  )
    {
        cOutput = bytesToGo >= HEX_DUMP_WIDTH ? HEX_DUMP_WIDTH : bytesToGo;

        buffPtr = buffer;
        buffPtr += sprintf(buffPtr, "%08Xh:  ", length-bytesToGo );
        buffPtr2 = buffPtr + (HEX_DUMP_WIDTH * 3) + 1;
        
        for ( i=0; i < HEX_DUMP_WIDTH; i++ )
        {
            BYTE value = *(ptr+i);

            if ( i >= cOutput )
            {
                // On last line.  Pad with spaces
                *buffPtr++ = ' ';
                *buffPtr++ = ' ';
                *buffPtr++ = ' ';
            }
            else
            {
                if ( value < 0x10 )
                {
                    *buffPtr++ = '0';
                    itoa( value, buffPtr++, 16);
                }
                else
                {
                    itoa( value, buffPtr, 16);
                    buffPtr+=2;
                }
 
                *buffPtr++ = ' ';
                *buffPtr2++ = isprint(value) ? value : '.';
            }
            
            // Put an extra space between the 1st and 2nd half of the bytes
            // on each line.
            if ( i == (HEX_DUMP_WIDTH/2)-1 )
                *buffPtr++ = ' ';
        }

        *buffPtr2 = 0;  // Null terminate it.
        puts(buffer);   // Can't use printf(), since there may be a '%'
                        // in the string.
        bytesToGo -= cOutput;
        ptr += HEX_DUMP_WIDTH;
    }
}
