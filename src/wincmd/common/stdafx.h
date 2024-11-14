// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__D42E664F_E7D2_4FA6_B468_0A3442858649__INCLUDED_)
#define AFX_STDAFX_H__D42E664F_E7D2_4FA6_B468_0A3442858649__INCLUDED_
#define WINVER 0x0600		// vista 600

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// Insert your headers here
#define WIN32_LEAN_AND_MEAN      // Exclude rarely-used stuff from Windows headers

#include <afx.h>

#define VC_EXTRALEAN      // Exclude rarely-used stuff from Windows headers

#ifndef WINVER
#define WINVER 0x0501
#endif

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>      // MFC support for Internet Explorer 4 Common Controls
#include <afxdlgs.h>
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>         // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT



//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__D42E664F_E7D2_4FA6_B468_0A3442858649__INCLUDED_)