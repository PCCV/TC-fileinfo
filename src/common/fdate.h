	 /***********************************************************/
	 /*****     Sous-Routines : Gestion des Date          *******/
	 /***************  Auteur :    Fr GANNIER   *****************/
	 /***************     Lab. Physio. Anim.    *****************/
	 /***************      Fac. des Sciences    *****************/
	 /***************        37000 Tours        *****************/
	 /***********************************************************/

#if !defined( __FDATE_H )
#define __FDATE_H

#include <afx.h>
#include <atlbase.h>
#include <time.h>

CString FormatShortDate(SYSTEMTIME st);
char *ctime2(const time_t &t);
SYSTEMTIME CreateSystemeTime(time_t t);

#endif