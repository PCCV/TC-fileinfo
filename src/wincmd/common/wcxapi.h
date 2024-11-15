/*****************************************************************************
**  FILE   :   wcxapi.h
**  PURPOSE:   Interface functions for WinCom
**   Author:   Konstantin Polyakov
*****************************************************************************/

#ifndef _WCXAPI_H_
#define _WCXAPI_H_ 

#include "windows.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef WCX_PLUGIN_EXPORTS
   #define WCX_API __declspec(dllexport)
   #define STDCALL __stdcall
#else
   #define WCX_API
   #define STDCALL
#endif

// OpenArchive should perform all necessary operations 
// when an archive is to be opened
WCX_API HANDLE STDCALL OpenArchive ( tOpenArchiveData *ArchiveData);

// WinCmd calls ReadHeader to find out what files are in the archive
WCX_API   int STDCALL ReadHeader (HANDLE hArcData, tHeaderData *HeaderData);

// ProcessFile should unpack the specified file 
// or test the integrity of the archive
WCX_API int STDCALL ProcessFile ( HANDLE hArcData, int Operation, char *DestPath, char *DestName);

// CloseArchive should perform all necessary operations 
// when an archive is about to be closed.
WCX_API int STDCALL CloseArchive (HANDLE hArcData);

// This function allows you to notify user 
// about changing a volume when packing files
WCX_API void STDCALL SetChangeVolProc ( HANDLE hArcData, tChangeVolProc pChangeVolProc1);

// This function allows you to notify user about 
// the progress when you un/pack files
WCX_API void STDCALL SetProcessDataProc ( HANDLE hArcData, tProcessDataProc pProcessDataProc);

// GetPackerCaps tells WinCmd what features your packer plugin supports
WCX_API int STDCALL GetPackerCaps ();

// PackFiles specifies what should happen when a user creates, 
// or adds files to the archive.
WCX_API int STDCALL PackFiles ( char *PackedFile, char *SubPath, char *SrcPath, char *AddList, int Flags);

// ConfigurePacker gets called when the user clicks the Configure button 
// from within "Pack files..." dialog box in WinCmd
WCX_API void STDCALL ConfigurePacker ( HWND Parent, HINSTANCE DllInstance);

#ifdef __cplusplus
}
#endif

#endif
