// #include "stdafx.h"
#include <stdexcept>
#include <delayimp.h>

#ifndef _RAISE
#define _RAISE(x) _invoke_watson(nullptr, nullptr, nullptr, 0, 0)
#endif


/////////////////////////////////////////////////////////////////////////////
// Delay-load notification
//
// The "notify hook" gets called for every call to the delay load helper. This allows a user to hook every call and
// skip the delay load helper entirely.
//
//  dliNotify == { dliStartProcessing |  dliPreLoadLibrary  |  dliPreGetProc |  dliNoteEndProcessing}
//  on this call.

// #pragma comment(lib, "MyDll.lib")
// #pragma comment(lib,"Delayimp")
// #pragma comment(linker, "/DELAYLOAD:MyDll.dll")

// __FUnloadDelayLoadedDLL("MyLib.dll"); 

/*
LONG WINAPI DelayLoadFilter(DWORD code)
{
    switch(code)
    {
        case VcppException(ERROR_SEVERITY_ERROR, ERROR_MOD_NOT_FOUND):
        case VcppException(ERROR_SEVERITY_ERROR, ERROR_PROC_NOT_FOUND):
            return EXCEPTION_EXECUTE_HANDLER;
    }
    return EXCEPTION_CONTINUE_SEARCH;
}

/* code
__try
{
    // the main program: we work with DLL the postponed load.

}
__except(DelayLoadFilter(GetExceptionCode()))
{
    // the processor of exception
    switch(GetExceptionCode())
    {
    case VcppException(ERROR_SEVERITY_ERROR, ERROR_MOD_NOT_FOUND):
        MessageBox(NULL, "DLL not found", "Error", MB_OK);
        break;

    case VcppException(ERROR_SEVERITY_ERROR, ERROR_PROC_NOT_FOUND):
        MessageBox(NULL, "Function in DLL not found", "Error", MB_OK);
        break;
    }
}
*/

/* 
FARPROC WINAPI DliHook(unsigned DliNotify, PDelayLoadInfo pdli) 
{
	if(dliNotify! = dliNotePreGetProcAddress || pdli->.dlp.fImportByName == 0)
        return 0;

    if(to!.strchmp(.pdli->.dlp.szProcName, "SomeFunc"))
        return GetProcAddress(.pdli->.hmodCur, "YetAnotherFunc");

    return 0; }
PfnDliHook __ pfnDliNotifyHook = DliHook;

*/

//  deuxieme exemple
PfnDliHook   __pfnDliFailureHook;

using namespace std;

class dllload_error : public runtime_error 
{
public:
	explicit dllload_error(const string& _S) : runtime_error(_S) {}
	virtual ~dllload_error() {}
protected:
	virtual void _Doraise() const {_RAISE(*this); }
};

/*

FARPROC WINAPI hookDLLLoad(unsigned dliNotify,PDelayLoadInfo  pdli)
{
	char buf[256];
	sprintf(buf,"XXX api not access. notify=%d, lasterr=%d,dllnm=%s",
		dliNotify,pdli->dwLastError,pdli->szDll);
//	cout << buf << endl;
	throw dllload_error(buf);
	return (FARPROC)NULL;
}


// Dans le code
 __pfnDliFailureHook=hookDLLLoad;

// DLLエクスポート関数にアクセスするとき、例外をキャッチする
	try {
		exFunc();
	} catch(dllload_error er) {
		cerr << er.what() << endl;
	}

*/



/*
 __pfnDliFailureHook=hookDLLLoad;

	try {
		exFunc();
	} catch(dllload_error er) {
		cerr << er.what() << endl;
	}
*/
