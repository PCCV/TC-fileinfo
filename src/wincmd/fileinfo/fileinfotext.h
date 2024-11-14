    /***********************************************************/
    /****   Sub-routine :  *****/
    /***************  Auteur :    Fr GANNIER   *****************/
    /***************     Lab. Physio. Anim.    *****************/
    /***************      Fac. des Sciences    *****************/
    /***************        37000 Tours        *****************/
    /***********************************************************/

#if !defined( __FINFO_H )
#define __FINFO_H

#include "..\..\common\fdate.h"
#include "file_ver.h"
#include "..\common\wait.h"

#include "..\common\memorymappedfile.h"

#define ModuleName "fileinfo.wlx"

//CString CreateText0(PVOID ptr);
CString CreateText0(char const *);
CString CreateText1(PVOID ptr, CWait &wait);
CString CreateManifest(PVOID ptr, CWait &wait);

enum OBJ_FILE_TYPE 
{
    OBJ_UNKNOWN = 0,
    OBJ_COFF_OBJ,
    OBJ_COFF_LIB,
    OBJ_OMF_OBJ,
    OBJ_OMF_LIB,
    OBJ_OMF_IMPLIB,
	OBJ_TL
};

CString CreateText2(PVOID ptr, CWait &wait);
CString CreateText3(PVOID ptr, CWait &wait);

OBJ_FILE_TYPE DisplayObjectFile( MEMORY_MAPPED_FILE *pmmf );


void CreateParentTree(PVOID ptr, CTreeCtrl &tree, CWait &wait);
// void CreateOcxList(PVOID ptr, CListCtrl &list, CRichEditCtrl &edit);

#endif