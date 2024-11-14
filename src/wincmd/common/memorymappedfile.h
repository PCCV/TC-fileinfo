//==========================================
// Modified from Matt Pietrek by F.GANNIER
// Microsoft Systems Journal, Feb 1997
// FILE: MEMORYMAPPEDFILE.H
//==========================================
#ifndef __MEMMAPFL_H__
#define __MEMMAPFL_H__
#include <windows.h>
#include "..\..\common\ffile.h"

enum errMMF {   errMMF_NoError, errMMF_FileOpen,
                errMMF_FileMapping, errMMF_MapView };

class MEMORY_MAPPED_FILE
{
protected:
    HANDLE      m_hFileMapping; // Handle of memory mapped file

private:
    HANDLE      m_hFile;
    PVOID       m_pMemoryMappedFileBase, m_pSavedMMFB;
    DWORD		m_szFile;
	errMMF      m_errCode;  
	char		*m_fileName;
	char		*m_path;
	BOOL		m_bAttach;

public:
    MEMORY_MAPPED_FILE( PSTR pszFileName );
    ~MEMORY_MAPPED_FILE(void);
    
//    PVOID   GetBase( void ){ return m_pMemoryMappedFileBase; }
	DWORD   GetBase( void ){ return (DWORD) m_pMemoryMappedFileBase; }
    DWORD   GetFileSize( void ){ return m_szFile; }
    BOOL    IsValid( void ) { return errMMF_NoError == m_errCode; } 
    errMMF	GetErrorType(){ return m_errCode; }
	const char	*GetBaseName() { return ::GetBaseName(m_fileName); }
	const char	*GetName() { return m_fileName; }
	const char	*GetPath() { return m_path; }
	BOOL		IsValidPtr( DWORD ptr ) { return (ptr - GetBase() < GetFileSize()); }
	DWORD 		GetTranslatedPtr( DWORD ptr ) { return GetBase() + ptr; }
	
	BOOL IsAttached() { return m_bAttach; }	
	BOOL Attach(PVOID base, DWORD size);
	void Detach();
};

typedef MEMORY_MAPPED_FILE *pMEMORY_MAPPED_FILE;

#endif
