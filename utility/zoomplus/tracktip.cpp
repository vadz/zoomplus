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

File:	TrackTip.cpp
Owner:	russf@gipsysoft.com
Purpose:	Tracking tooltip, until I can ensure the OS version works I
					think I'll do my own special one...
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "resource.h"
#include "Utils.h"
#include "TrackTip.h"

static ATOM g_Class = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTrackTip::CTrackTip()
{

}

CTrackTip::~CTrackTip()
{
	if( g_Class )
	{
		VERIFY( UnregisterClass( (LPCTSTR)g_Class, g_hInst ) );
		g_Class = NULL;
	}
}


void CTrackTip::Show( HWND hwndParent )
//
//	Show our tooltip, register our class if it has not been registered
{
	if( g_Class == NULL )
	{
		WNDCLASSEX wcex = { 0 };
		wcex.cbSize = sizeof( WNDCLASSEX );

		wcex.style			= CS_DBLCLKS | CS_BYTEALIGNCLIENT;
		wcex.hInstance		= g_hInst;
		wcex.lpszClassName	= _T("rjf_TrackingToolTip");

		g_Class = RegisterClassEx( &wcex );
	}

	if( g_Class )
	{
		VERIFY( Create( g_Class, WS_BORDER | WS_POPUP, hwndParent ) );
	}
}


void CTrackTip::Set( HWND hwndParent, POINT ptBottomLeft, LPCTSTR pcszText )
{
	if( m_hwnd == NULL )
	{
		Show( hwndParent );
	}

	if( m_hwnd )
	{
		ASSERT( lstrlen( pcszText ) < countof( m_szTextBuffer ) );
		lstrcpy( m_szTextBuffer, pcszText );

		//
		//	Calculate the size of the new text
		HDC hdc = GetDC( m_hwnd );
		SIZE size;
		HGDIOBJ hOldFont = ::SelectObject( hdc, GetStockObject( DEFAULT_GUI_FONT ) );
		::GetTextExtentPoint32( hdc, m_szTextBuffer, lstrlen( m_szTextBuffer ), &size );
		::SelectObject( hdc, hOldFont );
		VERIFY( ::ReleaseDC( m_hwnd, hdc ) );

		//
		//	Add on a pixel for the border on each side plus 2 on left and right
		size.cx += 8 + 16;
		size.cy += 2;
		if( size.cy < 20 ) size.cy = 20;
		const int x = ptBottomLeft.x;
		const int y = ptBottomLeft.y - size.cy;
		SetWindowPos( m_hwnd, NULL, x, y, size.cx, size.cy, SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW );
		InvalidateRect( m_hwnd, NULL, NULL );
	}
}


LRESULT CTrackTip::WndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message )
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			(void)::BeginPaint( m_hwnd, &ps );
			WinHelper::CRect rc;
			::GetClientRect( hwnd, rc );
			::FillRect( ps.hdc, rc, GetSysColorBrush( COLOR_INFOBK ) );
			HICON hIcon = WinHelper::LoadIcon16x16( g_hInst, IDI_ZOOMPLUS );
			::DrawIconEx( ps.hdc, 1,1,hIcon, 16,16, 0, NULL, DI_NORMAL );

			::SetBkMode( ps.hdc, TRANSPARENT );
			::SetTextColor( ps.hdc, GetSysColor( COLOR_INFOTEXT ) );

			HGDIOBJ hOldFont = ::SelectObject( ps.hdc, GetStockObject( DEFAULT_GUI_FONT ) );
			rc.left += 16;
			::DrawText( ps.hdc, m_szTextBuffer, -1, rc, DT_SINGLELINE | DT_CENTER | DT_VCENTER );

			::SelectObject( ps.hdc, hOldFont );
			
			(void)::EndPaint( m_hwnd, &ps );
		}
		break;

	default:
		return DefWindowProc( hwnd, message, wParam, lParam );
  }
  return 0;
}