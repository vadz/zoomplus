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

File:	WinHelp.cpp
Owner:	russf@gipsysoft.com
Purpose:	WinHelp helper function
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "utils.h"

static LPCTSTR g_pcszHelpFileName = _T("ZoomPlus.hlp>NormWin");

void FASTCALL WinHelp( HWND hWndMain, UINT uCommand, DWORD dwData )
{
	TCHAR szFilename[ MAX_PATH + MAX_PATH ];
	(void)GetModulePath( g_hInst, szFilename, countof( szFilename ) );
	(void)lstrcat( szFilename, g_pcszHelpFileName );
	(void)::WinHelp( hWndMain, szFilename, uCommand, dwData );
}
