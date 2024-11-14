	 /***********************************************************/
	 /*****     Sous-Routines : Gestion fichiers          *******/
	 /***************  Auteur :    Fr GANNIER   *****************/
	 /***************     Lab. Physio. Anim.    *****************/
	 /***************      Fac. des Sciences    *****************/
	 /***************        37000 Tours        *****************/
	 /***********************************************************/


#if !defined( __FFILE_H )
#define __FFILE_H

#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

//const char *GetBaseName(const char * FileName);
const _TCHAR *GetBaseName(const _TCHAR * FileName);
const _TCHAR *GetExtName(const _TCHAR * FileName);


// char *GetPath(char *in, char *out=NULL, int maxchar=_MAX_PATH);
// char *GetPath(const char *in, char *out, int maxchar=_MAX_PATH);
_TCHAR *GetPath(_TCHAR *in, _TCHAR *out=NULL, int maxchar=_MAX_PATH);
_TCHAR *GetPath(const _TCHAR *in, _TCHAR *out, int maxchar=_MAX_PATH);

bool IsPath(const TCHAR * FileName);

bool FileExist(const _TCHAR* FileName);
unsigned int CountSubDir(const TCHAR * FileName);

void WaitDelay(int delay); // in seconds

#endif

