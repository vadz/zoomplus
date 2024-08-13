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

File:	ScreenToClientRect.cpp
Owner:	russf@gipsysoft.com
Purpose:	convert a screen rectangle to client coordinates
----------------------------------------------------------------------*/
#include "stdafx.h"

extern bool FASTCALL ScreenToClientRect( HWND hwnd, LPRECT lprc );

bool FASTCALL ScreenToClientRect( HWND hwnd, LPRECT lprc )
{
	ASSERT_VALID_WRITEOBJPTR( lprc );
	if( ::IsWindow( hwnd ) && MapWindowPoints( HWND_DESKTOP, hwnd, reinterpret_cast<LPPOINT>(&lprc->left), 2 ) )
	{
		return true;
	}
	ASSERT( ::IsWindow( hwnd ) );
	TRACE( _T("ScreenToClientRect failed\n" ));
	return false;
}
