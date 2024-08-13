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

File:	RegistrySettings.cpp
Owner:	russf@gipsysoft.com
Purpose:	Hotkey handling
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "RegistrySettings.h"

static bool g_bRegistered = false;


static int CorrectHotkeyModifiers( int nModifiers )
//
//	Why-oh-why-oh-why can't the hotkey control and the hotkey registration
//	use the same modifiers...gah, it's like split personalities hell bent
//	on making more work for everyone
{
	//	We want...
	//	#define MOD_ALT         0x0001
	//	#define MOD_CONTROL     0x0002
	//	#define MOD_SHIFT       0x0004

	//	We get...
	//	#define HOTKEYF_SHIFT           0x01
	//	#define HOTKEYF_CONTROL         0x02
	//	#define HOTKEYF_ALT             0x04
	int nNew = 0;
	if( nModifiers & HOTKEYF_SHIFT )
	{
		nNew |= MOD_SHIFT;
	}
	if( nModifiers & HOTKEYF_CONTROL )
	{
		nNew |= MOD_CONTROL;
	}
	if( nModifiers & HOTKEYF_ALT )
	{
		nNew |= MOD_ALT;
	}
	return nNew;
}



void UnRegisterHotkeys( HWND hwnd )
{
	if( g_bRegistered )
	{
		UnregisterHotKey( hwnd, 1 );
		UnregisterHotKey( hwnd, 2 );
	}
	
}

void RegisterHotkeys( HWND hwnd )
{
	UnRegisterHotkeys( hwnd );

	if(
		RegisterHotKey( hwnd, 1, CorrectHotkeyModifiers( g_reg.m_nHotKeyBitmapModifiers ), g_reg.m_nHotKeyBitmapVK )
		&& RegisterHotKey( hwnd, 2, CorrectHotkeyModifiers( g_reg.m_nHotKeyColourModifiers ), g_reg.m_nHotKeyColourVK )
		)
	{
		g_bRegistered = true;
	}
}