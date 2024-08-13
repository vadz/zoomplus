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
Purpose:	registry settings for the application
----------------------------------------------------------------------*/

#include "stdafx.h"
#include "RegistrySettings.h"
#include "Utils.h"

static LPCTSTR g_pcszAppName = _T("Software\\GipsySoft.com\\ZoomPlus\\1");


struct REGENTRY
{
  LPTSTR  pcszName;
  DWORD dwLength;
  void *ptr;
  DWORD dwType;
	bool bLoaded;
};

#define REG_LIST_START( name )	REGENTRY name[] = {
#define REG_LIST_BINARY( name, var )	{ _T(name), sizeof( var ), &var, REG_BINARY, false },
#define REG_LIST_DWORD( name, var )	{ _T(name), sizeof( var ), &var, REG_DWORD, false },
#define REG_LIST_BOOL( name, var )	{ _T(name), sizeof( var ), &var, REG_BINARY, false },
#define REG_LIST_SZ( name, var )	{ _T(name), sizeof( var ), var, REG_SZ, false },
#define REG_LIST_END()	};

bool WriteRegList( HKEY hkey, REGENTRY *pre, DWORD dwCount )
{
	for( UINT n = 0; n < dwCount; n++ )
	{
		REGENTRY &re = pre[ n ];
		if( RegSetValueEx( hkey, re.pcszName, NULL, re.dwType, (LPBYTE)re.ptr, re.dwLength ) != ERROR_SUCCESS )
		{
			return false;
		}
	}
	return true;
}


bool ReadRegList( HKEY hkey, REGENTRY *pre, DWORD dwCount )
{
	for( UINT n = 0; n < dwCount; n++ )
	{
		REGENTRY &re = pre[ n ];
		if( RegQueryValueEx( hkey, re.pcszName, NULL, NULL, (LPBYTE)re.ptr, &re.dwLength ) == ERROR_SUCCESS )
		{
			re.bLoaded = true;
		}
	}
	return true;
}



CRegistrySettings g_reg;

REG_LIST_START( g_values )
	REG_LIST_BOOL( "Transparent", g_reg.m_bTransparent )
	REG_LIST_SZ( "InitialDirectory", g_reg.m_szInitialDirectory )			//	0	-	must not change position due to the fact that it is indexed
	REG_LIST_BINARY( "WindowPos", g_reg.m_rcWindow )									//	1	-	must not change position due to the fact that it is indexed
	REG_LIST_BOOL( "AlwaysOnTop", g_reg.m_bAlwaysOntop )
	REG_LIST_BINARY( "HelpFileTime", g_reg.m_ftHelp )
	REG_LIST_DWORD( "Update", g_reg.m_Update )
	REG_LIST_DWORD( "ZoomFactor", g_reg.m_nZoomFactor )
	REG_LIST_BINARY( "ZoomPoint", g_reg.m_ptZoom )
	REG_LIST_DWORD( "Emailed", g_reg.m_bEmailed )
	REG_LIST_BOOL( "SaveZoomedImage", g_reg.m_bSaveZoomedImage )
	REG_LIST_DWORD( "Update1", g_reg.m_nUpdate1 )
	REG_LIST_DWORD( "Update2", g_reg.m_nUpdate2 )
	REG_LIST_DWORD( "Update3", g_reg.m_nUpdate3 )
	REG_LIST_BOOL( "MouseTracking", g_reg.m_bMouseTracking )
	REG_LIST_BOOL( "DisplayToolbar", g_reg.m_bDisplayToolbar )	
	REG_LIST_BOOL( "Visited", g_reg.m_bVisited )	
	REG_LIST_BOOL( "DisplayStatusbar", g_reg.m_bDisplayStatusbar )	
	REG_LIST_BOOL( "DisplayMouseWhenFollowing", g_reg.m_bDisplayMouseWhenFollowing )
	REG_LIST_BOOL( "DisplayRGBColourPanel", g_reg.m_bDisplayRGBColourPanel )
	REG_LIST_BOOL( "SingleInstance", g_reg.m_bSingleInstance )
	REG_LIST_BOOL( "ShowGrid", g_reg.m_bShowGrid )
	REG_LIST_DWORD( "GridColor", g_reg.m_crGrid )
	REG_LIST_DWORD( "GridSize", g_reg.m_nGridSize )
	REG_LIST_BOOL( "RelativeCoords", g_reg.m_bRelativeCoords )
	REG_LIST_DWORD( "nTransparency", g_reg.m_nTransparency )
	REG_LIST_DWORD( "nSlowScrollRate", g_reg.m_nSlowScrollRate )
	REG_LIST_DWORD( "nFastScrollRate", g_reg.m_nFastScrollRate )
	REG_LIST_DWORD( "nHotKeyColourVK", g_reg.m_nHotKeyColourVK )
	REG_LIST_DWORD( "nHotKeyBitmapVK", g_reg.m_nHotKeyBitmapVK )
	REG_LIST_DWORD( "nHotKeyColourModifiers", g_reg.m_nHotKeyColourModifiers )
	REG_LIST_DWORD( "nHotKeyBitmapModifiers", g_reg.m_nHotKeyBitmapModifiers )
	REG_LIST_BOOL( "bUseOSCursor", g_reg.m_bUseOSCursor )	
REG_LIST_END()


CRegistrySettings::CRegistrySettings()
	: m_bAlwaysOntop( false )
	, m_Update( knNoUpdate )
	, m_nZoomFactor( 3 )
	, m_hkeyCurrent( NULL )
	, m_bEmailed( false )
	, m_bSaveZoomedImage( false )
	, m_nUpdate1( 1000 )
	, m_nUpdate2( 100 )
	, m_nUpdate3( 50 )
	, m_bMouseTracking( false )
	, m_bDisplayToolbar( true )
	, m_bVisited( false )
	, m_bDisplayStatusbar( true )
	, m_bDisplayMouseWhenFollowing( true )
	, m_bDisplayRGBColourPanel( true )
	, m_bSingleInstance( false )
	, m_bShowGrid( false )
	, m_crGrid( RGB( 0, 0, 0 ) )
	, m_nGridSize( 1 )
	, m_bRelativeCoords( false )
	, m_bTransparent( false )
	, m_nTransparency( 255 * 70 / 100 )
	, m_nSlowScrollRate( 1 )
	, m_nFastScrollRate( 16 )
	, m_bUseOSCursor( true )
	, m_nHotKeyColourVK( 'C' )
	, m_nHotKeyColourModifiers( HOTKEYF_CONTROL | HOTKEYF_SHIFT )
	, m_nHotKeyBitmapVK( 'B' )
	, m_nHotKeyBitmapModifiers( HOTKEYF_CONTROL | HOTKEYF_SHIFT )
{
	*m_szInitialDirectory = _T('\000');

	m_ftHelp.dwLowDateTime = m_ftHelp.dwHighDateTime = 0;
	m_ptZoom.x = m_ptZoom.y = 0;
	m_rcWindow.left = m_rcWindow.top = m_rcWindow.right = m_rcWindow.bottom = 0;

	if( RegOpenKey( HKEY_CURRENT_USER, g_pcszAppName, &m_hkeyCurrent ) == ERROR_SUCCESS )
	{
		VERIFY( ReadRegList( m_hkeyCurrent, g_values, countof( g_values ) ) );
	}
	if( !g_values[ 0 ].bLoaded )
	{
		(void)GetSpecialFolderPath( m_szInitialDirectory, CSIDL_PERSONAL );
	}
}


CRegistrySettings::~CRegistrySettings()
{
	Save();

	VERIFY( RegCloseKey( m_hkeyCurrent ) == ERROR_SUCCESS );

	m_hkeyCurrent = NULL;
}


void CRegistrySettings::Save()
//
//	Write out our registry settings.
{
	if( !m_hkeyCurrent )
	{
		if( RegCreateKey( HKEY_CURRENT_USER, g_pcszAppName, &m_hkeyCurrent ) != ERROR_SUCCESS )
		{
			return;
		}
	}

	ASSERT( m_hkeyCurrent );

	VERIFY( WriteRegList( m_hkeyCurrent, g_values, countof( g_values ) ) );
}

bool CRegistrySettings::GotWindowPos() const
//
//	return true if teh window position is valid, false otherwise
{
	return g_values[ 1 ].bLoaded;
}
