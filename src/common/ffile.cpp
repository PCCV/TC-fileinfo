	 /***********************************************************/
	 /*****     Sous-Routines : Gestion fichiers          *******/
	 /***************  Auteur :    Fr GANNIER   *****************/
	 /***************     Lab. Physio. Anim.    *****************/
	 /***************      Fac. des Sciences    *****************/
	 /***************        37000 Tours        *****************/
	 /***********************************************************/

#include "stdafx.h"
#include <time.h>
#include "ffile.h"

unsigned int CountSubDir(const TCHAR *pszName)
{
	unsigned int cnt = 0;
	while( (pszName = _tcschr( pszName, '\\' ))++) 
	{		
		cnt++;
	}
	return cnt;
}

// const char *GetBaseName(const char * FileName)
const _TCHAR *GetBaseName(const _TCHAR * FileName)
{
    const _TCHAR *pszBaseName = _tcsrchr( FileName, '\\' );
    if ( pszBaseName )  // We found a path, so advance to the base filename
        return ++pszBaseName;
    else
        return FileName;  // No path.  Use the same name for both
}

const _TCHAR *GetExtName(const _TCHAR * FileName)
{
    const _TCHAR *pszBaseName = _tcsrchr( FileName, '.' ); 
    if ( pszBaseName )  // We found a path, so advance to the base filename
        return ++pszBaseName;
    else
        return NULL;  // No path.  Use the same name for both
}

// char *GetPath(const char *in, char *out, int maxchar)
_TCHAR *GetPath(const _TCHAR *in, _TCHAR *out, int maxchar)
{
	_tcsncpy(out, in, maxchar);
    _TCHAR *pszBaseName = _tcsrchr( out, '\\' );
    if ( pszBaseName )  // We found a path, so advance to the base filename
			*(++pszBaseName) = '\0';
	return out;
}

//						=NULL		=_MAX_PATH
//char *GetPath(char *in, char *out, int maxchar)
_TCHAR *GetPath(_TCHAR *in, _TCHAR *out, int maxchar)
{
	if (out)
		_tcsncpy(out, in, maxchar);
	else 
		out = in;
   _TCHAR *pszBaseName = _tcsrchr( out, '\\' );
   if ( pszBaseName )       // We found a path, so advance to the base filename
			*(++pszBaseName) = '\0';
	return out;
}

// renvoie TRUE si le dernier char == '\'
bool IsPath(const TCHAR * FileName)
{
	while(*(FileName++));
	return (*(FileName-2)=='\\');
}

bool FileExist(const _TCHAR* FileName)
{
   struct _stat my_stat;
   return (_tstat(FileName, &my_stat) == 0);
}

void WaitDelay(int delay) // in seconds
{
   time_t tm = time( NULL ) + delay;
   while ( time( NULL ) < tm )
	   ;
}