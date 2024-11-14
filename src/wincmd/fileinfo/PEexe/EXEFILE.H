//==========================================
// Modified from Matt Pietrek by F.GANNIER
// Microsoft Systems Journal, Feb 1997
// FILE: EXEFILE.H
//==========================================
#ifndef __EXEFILE_H__
#define __EXEFILE_H__

#ifndef __MEMMAPFL_H__
#include "..\..\common\memorymappedfile.h"
#endif

// MakePtr is a macro that allows you to easily add to values (including
// pointers) together without dealing with C's pointer arithmetic.  It
// essentially treats the last two parameters as DWORDs.  The first
// parameter is used to typecast the result to the appropriate pointer type.
#define MakePtr( cast, ptr, addValue ) (cast)( (DWORD)(ptr) + (DWORD)(addValue))

enum EXE_TYPE { exeType_Invalid, exeType_DOS, exeType_NE, exeType_VXD, exeType_LE,
                exeType_LX, exeType_PE, exeType_DOSEXT, exeType_BW,  
				exeType_W3,exeType_W4, exeType_DL, exeType_MP, exeType_P2, exeType_P3, exeType_PW };

enum errEXE_FILE {  errEXE_FILE_NO_ERROR,
                    errEXE_FILE_FILE_NOT_FOUND,
                    errEXE_FILE_INVALID_FORMAT,
					errPE_EXE_FILE_INVALID_FORMAT
};

class EXE_FILE : public MEMORY_MAPPED_FILE
{
protected:
    errEXE_FILE m_errorType;
private:
    LONG        m_secondaryHeaderOffset;
    EXE_TYPE    m_exeType;
public:
   EXE_FILE( PSTR pszFileName );
   ~EXE_FILE( ){ }
    
    BOOL    IsValid( void ){ return (errMMF_NoError == m_errorType || errPE_EXE_FILE_INVALID_FORMAT == m_errorType); }
    errEXE_FILE GetErrorType( void ){ return m_errorType; }
    DWORD   GetSecondaryHeaderOffset( void ){ return m_secondaryHeaderOffset; }
    EXE_TYPE GetExeType( void ){ return m_exeType; }
    PSTR    GetFileTypeDescription( void );
	PIMAGE_DOS_HEADER GetdosHeader() { return (PIMAGE_DOS_HEADER) GetBase(); }
	PIMAGE_FILE_HEADER GetImageHeader() { return (PIMAGE_FILE_HEADER) GetBase(); }
	
	BOOL Attach(PVOID base, DWORD size);
	void Detach();
};

#endif
