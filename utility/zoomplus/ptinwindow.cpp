/*----------------------------------------------------------------------
Copyright (c) 2000 Gipsysoft. All Rights Reserved.
File:	PtInWindow.cpp
Owner:	russf@gipsysoft.com
Purpose:	function to determine if a point is within a window area
----------------------------------------------------------------------*/
#include "stdafx.h"

extern bool FASTCALL PtInWindow( HWND hwnd, const POINT &pt );


bool FASTCALL PtInWindow( HWND hwnd, const POINT &pt )
{
	RECT rc;
	if( ::IsWindow( hwnd ) && GetWindowRect( hwnd, &rc ) && PtInRect( &rc, pt ) )
	{
		return true;
	}
	return false;
}