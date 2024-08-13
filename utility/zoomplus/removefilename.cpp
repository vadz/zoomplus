/*----------------------------------------------------------------------
Copyright (c) 2000 Gipsysoft. All Rights Reserved.
File:	RemoveFilename.cpp
Owner:	russf@gipsysoft.com
Purpose:	Remove the filename from the path passed.
					Could have used splitpath and makepath but they are just too
					heavy for such a small task.
----------------------------------------------------------------------*/
#include "stdafx.h"

extern void FASTCALL RemoveFilename( const TCHAR * pcszFilePath, TCHAR * pszBuffer );

void FASTCALL RemoveFilename( const TCHAR * pcszFilePath, TCHAR * pszBuffer )
{
	LPTSTR pszSlash = pszBuffer;
	while( ( *pszBuffer = *pcszFilePath ) != '\000' )
	{
		if( *pszBuffer == '\\' || *pszBuffer == '/' )
		{
			pszSlash = pszBuffer + 1;
		}

		pszBuffer++;
		pcszFilePath++;
	}
	*pszSlash = '\000';
}