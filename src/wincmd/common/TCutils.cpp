	 /***********************************************************/
	 /****   Programme :  Plug-in for Total Commander       *****/
	 /***************  Auteur :    Fr GANNIER   *****************/
	 /***************     Lab. Physio. Anim.    *****************/
	 /***************      Fac. des Sciences    *****************/
	 /***************        37000 Tours        *****************/
	 /***********************************************************/

#include "stdafx.h"
#include "TCutils.h"

bool IsCalledForQV(CWnd *wnd1)
{
	CWnd *wnd2 = wnd1->GetParent();
	return (wnd2?TRUE:FALSE);
}

bool IsGoodExt(const _TCHAR *p, CString str)
{
	CString ext="";
	int n=0;
	while(AfxExtractSubString(ext, str, n++, _T(',')))
	{
		if (!p || _tcsicmp(p,ext)==0)  return 1;
	}
	return 0;
}
						// = ""
CString GetTCini(CString TCPath )
{
	CRegKey rk;
	TCHAR PathtoTCini[300]; DWORD nb=300;
	LONG status;
	if ((status = rk.Open(HKEY_CURRENT_USER, _T("Software\\Ghisler\\Total Commander"))) == ERROR_SUCCESS)
		status = rk.QueryStringValue(PathtoTCini, _T("IniFileName"), &nb);
	if (status != ERROR_SUCCESS)
		if ((status = rk.Create(HKEY_LOCAL_MACHINE, _T("Software\\Ghisler\\Total Commander"))) == ERROR_SUCCESS)
			status = rk.QueryStringValue(PathtoTCini, _T("IniFileName"), &nb);
	if (status != ERROR_SUCCESS) 
		return TCPath;

   _TCHAR *pPathtoTCini = PathtoTCini;
   if (PathtoTCini[0]=='.')   
   {
      _TCHAR temp2[300]; nb = 300;
      if (rk.QueryStringValue(temp2, _T("InstallDir"), &nb) == ERROR_SUCCESS)
      {
         _tcscat(temp2, ++pPathtoTCini);
         _tcscpy(PathtoTCini, temp2);
         if (FileExist(PathtoTCini))
				TCPath = PathtoTCini;
      }
   }
	rk.Close();
   return TCPath;
}

/*
BOOL GetIniFilePath(CString file, _TCHAR *inifile)
{
	_TCHAR temp[200];
//	GetCurrentDirectory(200, temp);
	if (GetModuleFileName(GetModuleHandle(file), temp, 200))
	{
//		int ln = _tcslen(temp);
		_TCHAR *p = _tcsrchr(temp, '.'); p++;
		_tcscpy(p,_T("ini"));
		if (FileExist( temp ))
		{
			_tcscpy(inifile, temp);
			return TRUE;
		}
	}
	return FALSE;
}/**/

BOOL GetIniFilePath(HINSTANCE  hInst, _TCHAR *inifile)
{
	_TCHAR temp[200];
//	GetCurrentDirectory(200, temp);
	if (GetModuleFileName(hInst, temp, 200))
	{
//		int ln = _tcslen(temp);
		_TCHAR *p = _tcsrchr(temp, '.'); p++;
		_tcscpy(p,_T("ini"));
		if (FileExist( temp ))
		{
			_tcscpy(inifile, temp);
			return TRUE;
		}
	}
	return FALSE;
}

void ConvertFont2String(CWnd *ParentWnd, FontOptions &fo, _TCHAR *st)
{
	CString text;
	_TCHAR temp[50]; 
	if (_tcslen(fo.fontname) < 1)
		return;
	CDC *pDC = ParentWnd->GetDC( );
	int size = -MulDiv(fo.fontsize/20, pDC->GetDeviceCaps(LOGPIXELSX), 72);
	text = (CString) fo.fontname + _T(",")+ _itot(size, temp, 10);
	if (fo.fontbold)
		text += _T(",b");
	if (fo.fontitalic)
		text += _T(",i");
	_tcsncpy(st, text, 1024);
}

FontOptions GetFontOptions(CWnd *ParentWnd, CString TCiniPath)
{
	FontOptions fo={_T(""),0,FALSE,FALSE};
	if (!FileExist(TCiniPath))
		TCiniPath = GetTCini(TCiniPath); // FindTCDir
   if (!TCiniPath.IsEmpty())  // Read Font Preference
   {
		_TCHAR temp[300]; CString fontname;
        if (GetPrivateProfileString( _T("Lister"), _T("Font1"), _T(""), temp, 300, TCiniPath))
        {
           CString strRes(temp), size, type;
           AfxExtractSubString(fontname, strRes, 0, _T(','));
		   _tcscpy(fo.fontname, fontname);
           AfxExtractSubString(size, strRes, 1, _T(','));
           fo.fontsize = _tstoi(( LPCTSTR )size);  // fontsize en Device Units
           CDC *pDC = ParentWnd->GetDC( );
           fo.fontsize = - MulDiv( fo.fontsize, 72, pDC->GetDeviceCaps(LOGPIXELSX));  // fontsize en points
		   fo.fontsize *= 20;
           ParentWnd->ReleaseDC( pDC );
           AfxExtractSubString(size, strRes, 2, _T(','));
           AfxExtractSubString(type, strRes, 3, _T(','));
           if (size=='b') fo.fontbold = TRUE;
           if (size=='i') fo.fontitalic = TRUE;
           if (type=='b') fo.fontbold = TRUE;
           if (type=='i') fo.fontitalic = TRUE;
        } 
	}
	return fo;
}