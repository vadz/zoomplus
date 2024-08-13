/*----------------------------------------------------------------------
Copyright (c) 2000 Russ Freeman. All Rights Reserved.
Email: russf@gipsysoft.com
Web site: http://www.gipsysoft.com

This code may be used in compiled form in any way you desire. This
file may be redistributed unmodified by any means PROVIDING it is 
not sold for profit without the authors written consent, and 
providing that this notice and the authors name is included. If 
the source code in this file is used in any commercial application 
then a simple email would be nice.

This file is provided 'as is' with no expressed or implied warranty.
The author accepts no liability if it causes any damage to your
computer.

Expect bugs.

Please use and enjoy. Please let me know of any bugs/mods/improvements 
that you have found/implemented and I will fix/incorporate them into this
file.

File:	ShowLastError.cpp
Owner:	russf@gipsysoft.com
Purpose:	provide a utility function that will display whatever was the last
					error.
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "Utils.h"

void FASTCALL ShowLastError( HWND hwndParent, LPCTSTR pcszCaption )
{
	ASSERT( hwndParent == NULL || IsWindow( hwndParent ) );

	LPCTSTR lpMsgBuf;
	(void)FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &lpMsgBuf, 0, NULL );

	MessageBox( hwndParent, lpMsgBuf, pcszCaption, MB_OK | MB_ICONERROR );

	(void)LocalFree( (LPVOID)lpMsgBuf );
}
