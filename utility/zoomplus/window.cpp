/*----------------------------------------------------------------------
Copyright (c) 2000 Russ Freeman. All Rights Reserved.
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

File:	Window.cpp
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "Window.h"


LRESULT CALLBACK BaseWndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	CWindow *pWnd = reinterpret_cast<CWindow*>( GetWindowLong( hwnd, GWL_USERDATA ) );
	if( pWnd )
	{
		return pWnd->WndProc( hwnd, message, wParam, lParam );
	}
	else
	{
		switch( message )
		{
		case WM_CREATE:
			if( pWnd == NULL )
			{
				LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>( lParam );
				SetWindowLong( hwnd, GWL_USERDATA, reinterpret_cast<long>( lpcs->lpCreateParams ) );
			}
			break;

		default:
			return DefWindowProc( hwnd, message, wParam, lParam );
		}
	}
	return 0;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWindow::CWindow()
	: m_hwnd( NULL )
{

}

CWindow::~CWindow()
{
	if( m_hwnd )
	{
		Destroy();
	}
}


bool CWindow::Create( ATOM atomClass, DWORD dwStyle, HWND hwndParent )
{
	m_hwnd = CreateWindowEx( 0, (LPCTSTR)atomClass, NULL, dwStyle, 0, 0, 0, 0, hwndParent, NULL, g_hInst, this );
	VAPI( m_hwnd );
	return m_hwnd ? true : false;
}


bool CWindow::CreateEx( ATOM atomClass, DWORD dwStyleEx, DWORD dwStyle, HWND hwndParent )
{
	m_hwnd = CreateWindowEx( dwStyleEx, (LPCTSTR)atomClass, NULL, dwStyle, 0, 0, 0, 0, hwndParent, NULL, g_hInst, this );
	VAPI( m_hwnd );
	return m_hwnd ? true : false;
}


void CWindow::Destroy()
{
	if( m_hwnd )
	{
		ASSERT_VALID_HWND( m_hwnd );
		VAPI( ::DestroyWindow( m_hwnd ) );
		m_hwnd = NULL;
	}
}


ATOM CWindow::RegisterClassEx( WNDCLASSEX *pwndclassex )
{
	pwndclassex->lpfnWndProc	= (WNDPROC)BaseWndProc;
	return ::RegisterClassEx( pwndclassex );
}