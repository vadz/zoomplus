/*----------------------------------------------------------------------
Copyright (c) 2000 Gipsysoft. All Rights Reserved.
File:	GetModulePath.cpp
Owner:	russf@gipsysoft.com
Purpose:	Get the full path to a module.
----------------------------------------------------------------------*/
#include "stdafx.h"
#ifndef _INC_TCHAR
	#include <TCHAR.H>
#endif	//	_INC_TCHAR

extern DWORD FASTCALL GetModulePath( HINSTANCE hInst, LPTSTR pszBuffer, DWORD dwSize );

DWORD  FASTCALL GetModulePath( HINSTANCE hInst, LPTSTR pszBuffer, DWORD dwSize )
//
//	Return the size of the path in bytes.
{
	DWORD dwLength = GetModuleFileName( hInst, pszBuffer, dwSize );
	if( dwLength )
	{
		while( dwLength && pszBuffer[ dwLength ] != _T('\\') )
		{
			dwLength--;
		}

		if( dwLength )
			pszBuffer[ dwLength + 1 ] = _T('\000');
	}
	return dwLength;
}
