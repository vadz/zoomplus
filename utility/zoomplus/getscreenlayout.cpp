/*----------------------------------------------------------------------
Copyright (c) 2000 Russ Freeman. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
Email: russf@gipsysoft.com
Web site: http://www.gipsysoft.com

This notice must remain intact
This file belongs wholly to Russ Freeman
You may use this file compiled form in your applications.
You may not sell this file in source form.
This source code may not be distributed as part of a library,

This file is provided 'as is' with no expressed or implied warranty.
The author accepts no liability if it causes any damage to your
computer.

Please use and enjoy. Please let me know of any bugs/mods/improvements 
that you have found/implemented and I will fix/incorporate them into this
file.

File:	GetScreenLayout.cpp
Owner:	russf@gipsysoft.com
Purpose:	Get the size of the screen with support for multiple monitors

	Most of this code taken from multimon.h and used as desired.
----------------------------------------------------------------------*/
#include "stdafx.h"

static bool g_bInitialised = false, g_bMultiMonitor = false;
int g_nScreenLeft, g_nScreenTop, g_nScreenWidth, g_nScreenHeight;

#if(WINVER < 0x0500)

	#define SM_XVIRTUALSCREEN       76
	#define SM_YVIRTUALSCREEN       77
	#define SM_CXVIRTUALSCREEN      78
	#define SM_CYVIRTUALSCREEN      79

#endif	//	if(WINVER < 0x0500)

void GetScreenLayout()
{
	if( !g_bInitialised )
	{
		HMODULE hUser32 = GetModuleHandle( _T("USER32") );
    if( hUser32 && GetProcAddress( hUser32,"MonitorFromWindow" ) )
			g_bMultiMonitor = true;
		g_bInitialised = true;
	}

	if( g_bMultiMonitor )
	{
		g_nScreenWidth = GetSystemMetrics( SM_CXVIRTUALSCREEN );
		g_nScreenHeight = GetSystemMetrics( SM_CYVIRTUALSCREEN );
		g_nScreenLeft = GetSystemMetrics( SM_XVIRTUALSCREEN );
		g_nScreenTop = GetSystemMetrics( SM_YVIRTUALSCREEN );		
	}
	else
	{
		g_nScreenWidth = GetSystemMetrics( SM_CXSCREEN );
		g_nScreenHeight = GetSystemMetrics( SM_CYSCREEN );
		g_nScreenLeft = 0;
		g_nScreenTop = 0;
	}

}