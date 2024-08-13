/*----------------------------------------------------------------------
Copyright (c) 2000 Gipsysoft. All Rights Reserved.
File:	strrepc.cpp
Owner:	leea@gipsysoft.com
Web site: http://www.gipsysoft.com
Purpose:	replace all occurances of a character in a string with a different
					character.
----------------------------------------------------------------------*/
#include "stdafx.h"

extern void FASTCALL strrepc( TCHAR *pszString, TCHAR cSearch, TCHAR cReplace );

void FASTCALL strrepc( TCHAR *pszString, TCHAR cSearch, TCHAR cReplace )
{
	while( *pszString )
	{
		if( *pszString == cSearch )
		{
			*pszString = cReplace;
		}
		pszString++;
	}
}
