#if !defined( __listplug_H )
#define __listplug_H

// listplug.h : main header file for the LISTPLUG DLL
//

#if !defined(AFX_LISTPLUG_H__4BC0694E_D8EE_4571_AC29_34F211A668EB__INCLUDED_)
#define AFX_LISTPLUG_H__4BC0694E_D8EE_4571_AC29_34F211A668EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
   #error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"      // main symbols

/**************   Commande transmise par TC  *******************/

/* Contents of file listplug.h */

#define lc_copy		1
#define lc_newparams	2
#define lc_selectall	3
#define lc_setpercent	4

#define lcp_wraptext	1
#define lcp_fittowindow 2
#define lcp_ansi		4
#define lcp_ascii		8
#define lcp_variable	12
#define lcp_forceshow	16
#define lcp_fitlargeronly 32
#define lcp_center	64

#define lcs_findfirst	1
#define lcs_matchcase	2
#define lcs_wholewords	4
#define lcs_backwards	8

#define itm_percent	0xFFFE
#define itm_fontstyle	0xFFFD
#define itm_wrap		0xFFFC
#define itm_fit		0xFFFB
#define itm_next		0xFFFA
#define itm_center	0xFFF9

#define LISTPLUGIN_OK	0

#define LISTPLUGIN_ERROR	1

typedef struct {
	int size;
	DWORD PluginInterfaceVersionLow;
	DWORD PluginInterfaceVersionHi;
	char DefaultIniName[MAX_PATH];
} ListDefaultParamStruct;

HWND __stdcall ListLoad(HWND ParentWin,char* FileToLoad,int ShowFlags);
HWND __stdcall ListLoadW(HWND ParentWin,WCHAR* FileToLoad,int ShowFlags);
int __stdcall ListLoadNext(HWND ParentWin,HWND PluginWin,char* FileToLoad,int ShowFlags);
int __stdcall ListLoadNextW(HWND ParentWin,HWND PluginWin,WCHAR* FileToLoad,int ShowFlags);
void __stdcall ListCloseWindow(HWND ListWin);
void __stdcall ListGetDetectString(char* DetectString,int maxlen);
int __stdcall ListSearchText(HWND ListWin,char* SearchString,int SearchParameter);
int __stdcall ListSearchTextW(HWND ListWin,WCHAR* SearchString,int SearchParameter);

int __stdcall ListSearchDialog(HWND ListWin,int FindNext);
int __stdcall ListSendCommand(HWND ListWin,int Command,int Parameter);
int __stdcall ListPrint(HWND ListWin,char* FileToPrint,char* DefPrinter,int PrintFlags,RECT* Margins);
int __stdcall ListPrintW(HWND ListWin,WCHAR* FileToPrint,WCHAR* DefPrinter,int PrintFlags,RECT* Margins);
int __stdcall ListNotificationReceived(HWND ListWin,int Message,WPARAM wParam,LPARAM lParam);
void __stdcall ListSetDefaultParams(ListDefaultParamStruct* dps);
HBITMAP __stdcall ListGetPreviewBitmap(char* FileToLoad,int width,int height,char* contentbuf,int contentbuflen);
HBITMAP __stdcall ListGetPreviewBitmapW(WCHAR* FileToLoad,int width,int height, char* contentbuf,int contentbuflen);
/////////////////////////////////////////////////////////////////////////////
// CListplugApp
// See listplug.cpp for the implementation of this class
//

class CListplugApp : public CWinApp
{
public:
   CListplugApp(LPCTSTR pszAppName);
   virtual BOOL InitInstance();

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CListplugApp)
   //}}AFX_VIRTUAL

   //{{AFX_MSG(CListplugApp)
      // NOTE - the ClassWizard will add and remove member functions here.
      //    DO NOT EDIT what you see in these blocks of generated code !
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LISTPLUG_H__4BC0694E_D8EE_4571_AC29_34F211A668EB__INCLUDED_)

#endif