	 /***********************************************************/
	 /*****     Sous-Routines : Gestion des Date          *******/
	 /***************  Auteur :    Fr GANNIER   *****************/
	 /***************     Lab. Physio. Anim.    *****************/
	 /***************      Fac. des Sciences    *****************/
	 /***************        37000 Tours        *****************/
	 /***********************************************************/

#include "stdafx.h"
#include "fdate.h"

CString FormatShortDate(SYSTEMTIME st)
{
	CString Temp="";
	CString sShortDate = "MM/dd/yyyy";
	CString sTimeFormat = "HH:mm:ss";	
	{
		CRegKey key;		
		char szValue[1024];
		DWORD pdwCount=1024;
//		HKEY m_hKey;

		if( key.Open(HKEY_CURRENT_USER, "Control Panel\\International", KEY_READ) == ERROR_SUCCESS) // open the key
		{
			if(!key.QueryValue( szValue, "sShortDate", &pdwCount )) // see if there is a value
				sShortDate = szValue; // if there is, set the variable
			if(!key.QueryValue( szValue, "sTimeFormat", &pdwCount )) // see if there is a value
				sTimeFormat = szValue; // if there is, set the variable
			key.Close();
		}
	}

	if (*sShortDate == TEXT('M') || *sShortDate == TEXT('m'))
		Temp.Format("%02d/%02d/%02d  %02d:%02d:%02d ", st.wMonth, st.wDay, st.wYear, st.wHour, st.wMinute, st.wSecond);
	if (*sShortDate == TEXT('D') || *sShortDate == TEXT('d'))
		Temp.Format("%02d/%02d/%02d  %02d:%02d:%02d ", st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute, st.wSecond);
	if (*sShortDate == TEXT('Y') || *sShortDate == TEXT('y'))
		Temp.Format("%02d/%02d/%02d  %02d:%02d:%02d ", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
	return Temp;   
}

/*
typedef struct _SYSTEMTIME {
    WORD wYear;
    WORD wMonth;
    WORD wDayOfWeek;
    WORD wDay;
    WORD wHour;
    WORD wMinute;
    WORD wSecond;
    WORD wMilliseconds;
} SYSTEMTIME;
*/

char *ctime2(const time_t *t)
{
	char *time = ctime(t);
	if (time)
	{
		UINT i = strlen(time);
		time[i-1]=0;
	}
	return time;
}

SYSTEMTIME CreateSystemeTime(time_t t)
{
	SYSTEMTIME st;
	CTime temptime(t);
	temptime.GetAsSystemTime(st);
	return st;
}		
