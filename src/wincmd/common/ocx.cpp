//	if (op.flxocx)
//		op.flxocx = TestOCX(pParentWnd, "msflxgrd.ocx");



bool isDllRegistered(const char* dllName)
{
  bool found = FALSE;
  HKEY clsidKey;
 
  const char* clsidPath = "SOFTWARE\\Classes\\CLSID";
  const char* clsidInsideKeyPath = "\\InprocServer32";
  const int clsidPathSize = strlen(clsidPath);
  const int clsidInsideKeyPathSize = strlen(clsidInsideKeyPath);
 
  if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, clsidPath, 0, KEY_READ, &clsidKey) == ERROR_SUCCESS)
  {
    int keyNameSize = 255;
    char keyName[255];
    int keyIndex = 0;
    while(RegEnumKey(clsidKey, keyIndex, keyName, keyNameSize) == ERROR_SUCCESS && !found)
    {
      //build the full path => SOFTWARE\Classes\CLSID\{1234-5678-1234-5678}\InprocServer32
      const int keySize = strlen(keyName);
      const int fullPathSize = clsidPathSize+clsidInsideKeyPathSize+keySize+1/* the slash */;
      char* fullPath = (char*)malloc(sizeof(char)*(fullPathSize+1));
      strncpy(fullPath, clsidPath, clsidPathSize);
      strncpy(fullPath+clsidPathSize+1, keyName, keySize);
      strncpy(fullPath+clsidPathSize+1+keySize, clsidInsideKeyPath, clsidInsideKeyPathSize);
      fullPath[clsidPathSize] = '\\';
      fullPath[fullPathSize] = '\0';
 
      //open the path and get the value
      HKEY fullPathKey;
      if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, fullPath, 0, KEY_READ, &fullPathKey) == ERROR_SUCCESS)
      {
        unsigned long size = 255;
        char keyValue[255];
        if(RegQueryValueEx(fullPathKey, 0, 0, 0, (BYTE*)keyValue, &size)  == ERROR_SUCCESS)
        {
          char* vcfCheck = strstr(_strlwr(keyValue), dllName);
          if (vcfCheck != NULL) found = TRUE;
        }
        RegCloseKey(fullPathKey);
      }
 
      free(fullPath);
      ++keyIndex;
    }
    RegCloseKey(clsidKey);
  }
 
  return found;
}


bool TestOCX(CWnd *pWnd, char ocxName[])
{
	if (isDllRegistered(ocxName))
		return true;
	HINSTANCE hDLL = LoadLibrary(ocxName);
	CString str;
	if(NULL == hDLL)
	{
		str.Format("\"%s\" not found, Please copy then in system directory or disabled \"Frame\" tab in options", ocxName);
		pWnd->MessageBox(str,"Info",MB_ICONINFORMATION | MB_OKCANCEL);
		return FALSE;
	}
	typedef HRESULT (CALLBACK *HCRET)(void);
	HCRET lpfnDllRegisterServer
		= (HCRET)GetProcAddress(hDLL, "DllRegisterServer");
	if(NULL == lpfnDllRegisterServer)
	{
		str.Format("\"%s\" not loaded, Frame Header not shown", ocxName);
		pWnd->MessageBox(str,"Info",MB_ICONINFORMATION | MB_OKCANCEL);
		return FALSE;
	}

	if(FAILED((*lpfnDllRegisterServer)()))
	{
		str.Format("DLLRegisterServer() Failed; \"%s\" not registered ", ocxName);
		pWnd->MessageBox(str,"Info",MB_ICONINFORMATION | MB_OKCANCEL);
		return FALSE;
	}
	return TRUE;
}
