//==========================================
// Modified from Matt Pietrek by F.GANNIER
// Microsoft Systems Journal, Feb 1997
// FILE: MEMORYMPAPPEDFILE.CPP
//==========================================
#include "stdafx.h"

#pragma hdrstop
#include "memorymappedfile.h"

MEMORY_MAPPED_FILE::MEMORY_MAPPED_FILE( PSTR pszFileName )
{
    // Given a filename, the constructor opens a file handle, creates a file
    // mapping, and maps the entire file into memory.
    m_hFile = INVALID_HANDLE_VALUE;
    m_hFileMapping = 0;
    m_pMemoryMappedFileBase = 0;
    m_szFile = 0;
    m_errCode = errMMF_FileOpen;    // Initial error code: not found	 
	m_bAttach = FALSE;

	m_fileName = strdup(pszFileName);
	m_path = strdup(pszFileName);
	::GetPath(m_fileName, m_path, strlen( m_path ));

    // First get a file handle      
    m_hFile = CreateFile(pszFileName, GENERIC_READ, FILE_SHARE_READ, NULL,
                        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, (HANDLE)0);
                    
    if ( m_hFile == INVALID_HANDLE_VALUE )
    {
        m_errCode = errMMF_FileOpen;
        return;
    }
	
	DWORD FileSizeHigh = 0;
	m_szFile = ::GetFileSize( m_hFile, &FileSizeHigh );
	if (FileSizeHigh) 
    {
        CloseHandle(m_hFile);
        m_hFile = INVALID_HANDLE_VALUE;
        m_errCode = errMMF_FileMapping;
        return;
    }

    // Now, create a file mapping   
    m_hFileMapping = CreateFileMapping(m_hFile,NULL, PAGE_READONLY, 0, 0,NULL);
    if ( m_hFileMapping == 0 )
    {
        // Oops.  Something went wrong.  Clean up.
        CloseHandle(m_hFile);
        m_hFile = INVALID_HANDLE_VALUE;
        m_errCode = errMMF_FileMapping;
        return;
    }

    m_pMemoryMappedFileBase = (PCHAR)MapViewOfFile( m_hFileMapping, FILE_MAP_READ, 0, 0, 0);

    if ( m_pMemoryMappedFileBase == 0 )
    {
        // Oops.  Something went wrong.  Clean up.
        CloseHandle(m_hFileMapping);
        m_hFileMapping = 0;
        CloseHandle(m_hFile);
        m_hFile = INVALID_HANDLE_VALUE;
        m_errCode = errMMF_MapView;
        return;
    }

	if (((DWORD64)m_pMemoryMappedFileBase) >> 32)
	{
        UnmapViewOfFile( m_pMemoryMappedFileBase );
		m_pMemoryMappedFileBase = 0;
        CloseHandle(m_hFileMapping);
        m_hFileMapping = 0;
        CloseHandle(m_hFile);
        m_hFile = INVALID_HANDLE_VALUE;
        m_errCode = errMMF_MapView;
		AfxMessageBox("Address of MappedFile > 4GB", MB_OK|MB_ICONERROR);
        return;	
	}
    m_errCode = errMMF_NoError;
}

BOOL MEMORY_MAPPED_FILE::Attach(PVOID base, DWORD size)
{
	if (m_bAttach) return FALSE;
	if (size > 0) m_szFile = size;
	m_pSavedMMFB = m_pMemoryMappedFileBase;
	m_pMemoryMappedFileBase =  base;
	return (m_bAttach = TRUE);
}

void MEMORY_MAPPED_FILE::Detach()
{
	if ( !m_bAttach ) return ;
	m_bAttach = FALSE;
	m_pMemoryMappedFileBase = m_pSavedMMFB;
	DWORD FileSizeHigh = 0;
	m_szFile = ::GetFileSize( m_hFile, &FileSizeHigh );
/*	if (FileSizeHigh) 
    {
        CloseHandle(m_hFile);
        m_hFile = INVALID_HANDLE_VALUE;
        m_errCode = errMMF_FileMapping;
        return;
    }/**/
}

MEMORY_MAPPED_FILE::~MEMORY_MAPPED_FILE(void)
{
    // Clean up everything that was created by the constructor
    if ( m_bAttach ) Detach();
	if ( m_pMemoryMappedFileBase )
        UnmapViewOfFile( m_pMemoryMappedFileBase );

    if ( m_hFileMapping )
        CloseHandle( m_hFileMapping );

    if ( m_hFile != INVALID_HANDLE_VALUE )
        CloseHandle( m_hFile ); 

	free ( m_fileName );
	free ( m_path );
    m_errCode = errMMF_FileOpen;
}
