/*----------------------------------------------------------------------
Copyright (c) 2000 Gipsysoft. All Rights Reserved.
File:	CopyBitmapToClipboard.cpp
Owner:	russf@gipsysoft.com
Purpose:	Copy a bitmap to the clipboard
----------------------------------------------------------------------*/
#include "stdafx.h"
#ifndef _INC_TCHAR
	#include <TCHAR.h>
#endif	//	_INC_TCHAR

extern bool CopyBitmapToClipboard( HWND hwnd, HBITMAP hbm );

bool  CopyBitmapToClipboard( HWND hwnd, HBITMAP hbm )
//
//	Simple little helper function that wraps up all the calls required
//	to get a bitmap onto the clipboard. It also does all of the error
//	handling that is ignored and outputs a beep/trace message if things go wrong.
{
	bool bRetVal = false;
	if( OpenClipboard( hwnd )
			&& EmptyClipboard()
			&& SetClipboardData( CF_BITMAP, hbm )
			&& CloseClipboard() )
	{
		bRetVal = true;
	}
	else
	{
		TRACE( _T("CopyBitmapToClipboard FAILED with error code %d\n"), ::GetLastError() );
		(void)MessageBeep(0);
	}
	return bRetVal;
}