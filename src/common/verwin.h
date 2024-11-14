#if !defined (__VERWIN_H)
#define __VERWIN_H


// version
#define WND_SERVER2008R2 13
#define WND_SERVER2008 12
#define WND_SEVEN 11
#define WND_VISTA 10
#define WND_SERVER2003 9
#define WND_XP   8
#define WND_2K   7
#define WND_NT   6
#define WND_ME   5
// #define WND_98SE 4
#define WND_98   3
#define WND_95   2
#define WND_32S  1

// type 
#define WND_WKS3 1
#define WND_WKS4 2
#define WND_S    3
#define WND_4EE	 4
#define WND_4S    5
#define WND_HE	 6
#define WND_PE	7
#define WND_AS	8
#define WND_DE	9
#define WND_EE	10
#define WND_WE	11
#define WND_SE	12
#define WND_DS	13

#define WND_OSR2 102
#define WND_OSR1 101
/**/
	// #define VER_NT_SERVER 0x80000000
#ifndef VER_NT_WORKSTATION 
	#define VER_NT_WORKSTATION 0x1
	#define VER_NT_DOMAIN_CONTROLLER 0x2
	#define VER_NT_SERVER 0x3
	#define VER_WORKSTATION_NT 0x40000000
	 // Microsoft Small Business Server
	#define VER_SUITE_SMALLBUSINESS 0x1    
	 // Win2k Adv Server or .Net Enterprise Server
	#define VER_SUITE_ENTERPRISE 0x2       
	 // Terminal Services is installed.  
	#define VER_SUITE_TERMINAL 0x10        
		// Win2k Datacenter
	#define VER_SUITE_DATACENTER 0x80
	 // Terminal server in remote admin mode
	#define VER_SUITE_SINGLEUSERTS 0x100
	#define VER_SUITE_PERSONAL 0x200
	 // Microsoft .Net webserver installed
	#define VER_SUITE_BLADE 0x400     
#endif
/**/

typedef struct _VERSION{
	BYTE ver;
	BYTE type;
	DWORD major;
	DWORD minor;
	WORD sp;
	DWORD build;
} *PVERSION, VERSION;


VERSION GetSystemVersion();
CString GetSystemVersionName(VERSION &ver);


#endif