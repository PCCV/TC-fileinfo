#include "stdafx.h"
#include "winnt.h"
#include "winbase.h"
#include "verwin.h"

/*   --- Changer dans Winbase.h --- 

typedef struct _OSVERSIONINFOEXA {
    DWORD dwOSVersionInfoSize;
    DWORD dwMajorVersion;
    DWORD dwMinorVersion;
    DWORD dwBuildNumber;
    DWORD dwPlatformId;
    CHAR   szCSDVersion[ 128 ];     // Maintenance string for PSS usage
    WORD wServicePackMajor;
    WORD wServicePackMinor;
	WORD wSuiteMask;
	WORD wProductType;
    WORD wReserved;
} OSVERSIONINFOEXA, *POSVERSIONINFOEXA, *LPOSVERSIONINFOEXA;
*/

VERSION GetSystemVersion()
{
#define BUFSIZE 256
	VERSION ver={0,0,0,0};
   OSVERSIONINFOEX osvi;
   BOOL bOsVersionInfoEx;

   // Try calling GetVersionEx using the OSVERSIONINFOEX structure.
   // If that fails, try using the OSVERSIONINFO structure.
   ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
   osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

   if( !(bOsVersionInfoEx = GetVersionEx ((OSVERSIONINFO *) &osvi)) )
   {
      osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
      if (! GetVersionEx ( (OSVERSIONINFO *) &osvi) ) 
         return ver;
   }
	
   ver.major = osvi.dwMajorVersion;
   ver.minor = osvi.dwMinorVersion;
   ver.sp = osvi.wServicePackMajor;
   ver.build = osvi.dwBuildNumber;
//   ver.major = osvi.dwMajorVersion;
//   ver.minor = osvi.dwMinorVersion;

	switch (osvi.dwPlatformId)
   {// Test for the Windows NT product family.
      case VER_PLATFORM_WIN32_NT:
		 if ( osvi.dwMajorVersion == 6)
		 {
			 if ( osvi.dwMinorVersion == 0 )
				if ( osvi.wProductType == VER_NT_WORKSTATION )
					ver.ver = WND_VISTA;
				else ver.ver = WND_SERVER2008;
			if ( osvi.dwMinorVersion == 1 ) 
				if ( osvi.wProductType == VER_NT_WORKSTATION )
					ver.ver = WND_SEVEN;
				else ver.ver = WND_SERVER2008R2;
		 }
         if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2 )
			ver.ver = WND_SERVER2003;

         if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1 )
            ver.ver = WND_XP;

         if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0 )
            ver.ver = WND_2K;

         if ( osvi.dwMajorVersion <= 4 )
			 ver.ver = WND_NT;

         // Test for specific product on Windows NT 4.0 SP6 and later.
         if( bOsVersionInfoEx )
         {
            // Test for the workstation type.
			if ( osvi.wProductType == VER_NT_WORKSTATION )
            {
               if( osvi.dwMajorVersion == 4 )
				   ver.type = WND_WKS4;
               else if( osvi.wSuiteMask & VER_SUITE_PERSONAL )
					ver.type = WND_HE;
               else
				   ver.type = WND_PE;
            }
// Test for the server type.
            else if ( osvi.wProductType == VER_NT_SERVER || osvi.wProductType == VER_NT_DOMAIN_CONTROLLER )
            {
               if( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2 )
               {
                  if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
					  ver.type = WND_DE;
                  else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
					  ver.type = WND_EE;
                  else if ( osvi.wSuiteMask == VER_SUITE_BLADE )
                     ver.type = WND_WE;
                  else
					  ver.type = WND_SE;
				}
				else if( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0 )
				{
					if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
					  ver.type = WND_DS;
					else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
					  ver.type = WND_AS;
					else
					  ver.type = WND_S;
				}
				else  // Windows NT 4.0 
				{
					if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
						ver.type = WND_4EE;
					else
						ver.type = WND_4S;
				}
			}
		 }
// Test for specific product on Windows NT 4.0 SP5 and earlier
		else  
		{
			HKEY hKey;
			char szProductType[BUFSIZE];
			DWORD dwBufLen=BUFSIZE;
			LONG lRet;
			lRet = RegOpenKeyEx( HKEY_LOCAL_MACHINE,
			   "SYSTEM\\CurrentControlSet\\Control\\ProductOptions",
			   0, KEY_QUERY_VALUE, &hKey );
			if( lRet != ERROR_SUCCESS )
			   return ver;

			lRet = RegQueryValueEx( hKey, "ProductType", NULL, NULL,
			   (LPBYTE) szProductType, &dwBufLen);
			if( (lRet != ERROR_SUCCESS) || (dwBufLen > BUFSIZE) )
			   return ver;

			RegCloseKey( hKey );

			if ( lstrcmpi( "WINNT", szProductType) == 0 )
				if (osvi.dwMajorVersion == 4)
					ver.type = WND_WKS4;
				else ver.type = WND_WKS3;
			if ( lstrcmpi( "LANMANNT", szProductType) == 0 )
				if (osvi.dwMajorVersion == 4)
					ver.type = WND_4S;
				else ver.type = WND_S;
			if ( lstrcmpi( "SERVERNT", szProductType) == 0 )
					ver.type = WND_AS;
		}
// Display service pack (if any) and build number.
        if( osvi.dwMajorVersion == 4 && 
             lstrcmpi( osvi.szCSDVersion, "Service Pack 6" ) == 0 )
        {
            HKEY hKey;
            LONG lRet;

            // Test for SP6 versus SP6a.
            lRet = RegOpenKeyEx( HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Hotfix\\Q246009", 0, KEY_QUERY_VALUE, &hKey );
            if( lRet == ERROR_SUCCESS )
			{
				ver.sp = 7; // sp6a
				ver.build = osvi.dwBuildNumber & 0xFFFF;
			}
            else // Windows NT 4.0 prior to SP6a
				ver.build = osvi.dwBuildNumber & 0xFFFF;

            RegCloseKey( hKey );
         }
         else // Windows NT 3.51 and earlier or Windows 2000 and later
			ver.build = osvi.dwBuildNumber & 0xFFFF;
         break;

      // Test for the Windows 95 product family.
      case VER_PLATFORM_WIN32_WINDOWS:
         if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 0)
         {
			ver.ver = WND_95;
            if ( osvi.szCSDVersion[1] == 'C' || osvi.szCSDVersion[1] == 'B' )
			      ver.type = WND_OSR2;
         } 

         if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 10)
         {
			ver.ver = WND_98;
             if ( osvi.szCSDVersion[1] == 'A' ) 
				 ver.type = WND_OSR1;
         } 

         if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 90)
			 ver.type = WND_ME;
         break;

      case VER_PLATFORM_WIN32s: ver.ver = WND_32S;
         break;
   }
   return ver; 
}

typedef void (WINAPI *PGNSI)(LPSYSTEM_INFO);
CString GetSystemVersionName(VERSION &ver)
{
	SYSTEM_INFO si; PGNSI pGNSI;
	CString str;
	// Use GetProcAddress to avoid load issues on Windows 2000
	pGNSI = (PGNSI) GetProcAddress(GetModuleHandle("kernel32.dll"),
		"GetNativeSystemInfo");
	if(NULL != pGNSI)
	pGNSI(&si);
	switch (ver.ver)
	{
		case 0 : break;
		case WND_SERVER2008R2 : str = ("Microsoft Windows Server 2008 Revision 2 family "); break;
		case WND_SERVER2008 : str = ("Microsoft Windows Server 2008 family "); break;
		case WND_SEVEN : str = ("Microsoft Windows Seven "); break;

		case WND_VISTA: str = ("Microsoft Windows Vista "); break;
		case WND_SERVER2003: str = ("Microsoft Windows Server 2003 family "); break;
		case WND_XP: str = ("Microsoft Windows XP ");break;
		case WND_2K: str = ("Microsoft Windows 2000 ");break;
		case WND_NT: str = "Microsoft Windows NT ";break;
		case WND_95: str = ("Microsoft Windows 95 ");break;
		case WND_98: str = ("Microsoft Windows 98 ");break;
		case WND_ME: str =("Microsoft Windows Millennium Edition\n");break;
		case WND_32S: str =("Microsoft Win32s\n");break;
	}
	if ( si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_IA64 )
		str += "Itanium-based Systems "; 
	else if ( si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64 )
		str += "x64 "; 
	switch (ver.type)
	{
		case WND_WKS4: str += ( "Workstation 4.0 " ); break;
		case WND_HE:  str += ( "Home Edition " );break;
		case WND_PE: str += ( "Professional Edition " );break;
		case WND_DE: str += ( "Datacenter Edition " );break;
		case WND_EE: str += ( "Enterprise Edition " );break;
		case WND_WE: str += ( "Web Edition " );break;
		case WND_SE: str += ( "Standard Edition " );break;
		case WND_DS: str += ( "Datacenter Server " );break;
		case WND_AS: str += ( "Advanced Server" );break;
		case WND_S:  str += ( "Server " );break;
		case WND_4EE: str += ( "Server 4.0, Enterprise Edition " );break;
		case WND_4S: str += ( "Server 4.0 " );break;
		case WND_OSR2: str += ("OSR2 " );break;
		case WND_OSR1: str += ("SE " );break;
		case WND_WKS3: str += ( "Workstation " ); break;		

	}
CString strtp;
	if ( ver.ver == WND_NT)
	{
		if (ver.sp == 7)
			strtp.Format( "Service Pack 6a (Build %d)\n", ver.build );
		else strtp.Format( "Service Pack %d (Build %d)\n", ver.sp, ver.build );
	}
	else {
		if (ver.sp > 0)
			strtp.Format( "Version %d.%d (build %d) Service pack %d \n", ver.major, ver.minor, ver.build, ver.sp);	   
		else 
			strtp.Format( "Version %d.%d (build %d)", ver.major, ver.minor, ver.build);	   

	}
	str += strtp;
   return str; 
}