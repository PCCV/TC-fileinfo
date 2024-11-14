	 /***********************************************************/
	 /****   Programme :  Plug-in for Total Commander       *****/
	 /***************  Auteur :    Fr GANNIER   *****************/
	 /***************     Lab. Physio. Anim.    *****************/
	 /***************      Fac. des Sciences    *****************/
	 /***************        37000 Tours        *****************/
	 /***********************************************************/

#if !defined( __TCUTILS_H )
#define __TCUTILS_H

#include <atlbase.h>				// Registry access

#include "..\..\common\ffile.h"

struct FontOptions{
	_TCHAR fontname[_MAX_PATH];
	int fontsize; // height, 200 twips == 10 points
	BOOL fontbold; 
	BOOL fontitalic;
}; // FontOptions, *pFontOptions;
//	FontOptions fo={"",0,FALSE,FALSE};

// #define info(st)  ((MessageBox(NULL, st,"Info",MB_ICONINFORMATION | MB_OKCANCEL)==IDOK)? TRUE : FALSE )

bool IsCalledForQV(CWnd *wnd1);
bool IsGoodExt(const _TCHAR *p, CString str);
//BOOL GetIniFilePath(CString file, _TCHAR *inifile);
BOOL GetIniFilePath(HINSTANCE, _TCHAR *inifile);
FontOptions GetFontOptions(CWnd *ParentWnd, CString TCiniPath);
CString GetTCini(CString TCPath = _T(""));
void ConvertFont2String(CWnd *ParentWnd, FontOptions &fo, _TCHAR *st);

#endif