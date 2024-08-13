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

File:	ZoomPlus.cpp
Owner:	russf@gipsysoft.com
Purpose:	Main...
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "resource.h"
#include "ResString.h"
#include "utils.h"
#include "MainWindow.h"
#include "RegistrySettings.h"

// Global Variables:
HINSTANCE g_hInst;
HCURSOR g_hcHand;
static CMainWindow g_MainWindow;
static LPCTSTR g_pcszClassName = _T("ZoomPlusMainWnd");

static const CResString g_szTitle( IDS_APP_TITLE );


// Foward declarations of functions included in this code module:
bool InitInstance(HINSTANCE, int);

LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);


int APIENTRY WinMain( HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int nCmdShow )
{
	g_hInst = hInstance;
	g_hcHand = ::LoadCursor( NULL, MAKEINTRESOURCE(/*IDC_HAND*/32649) );
	if( ! g_hcHand )
	{
		g_hcHand = ::LoadCursor( g_hInst, MAKEINTRESOURCE( IDC_POINTER ) );
	}

	INITCOMMONCONTROLSEX sex = { sizeof( sex ), ICC_COOL_CLASSES | ICC_BAR_CLASSES };
	if( !InitCommonControlsEx( &sex ) )
	{
		ShowLastError( NULL, g_szTitle );
		return 0;
	}

	MSG msg;
	HACCEL hAccelTable;

	// Perform application initialization:
	if( !InitInstance ( hInstance, nCmdShow ) )
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators( hInstance, MAKEINTRESOURCE( ID_ACCEL ) );
	ASSERT( hAccelTable );

	// Main message loop:
	while( GetMessage(&msg, NULL, 0, 0 ) ) 
	{
		if( !TranslateAccelerator(msg.hwnd, hAccelTable, &msg ) ) 
		{
			TranslateMessage(&msg);
			(void)DispatchMessage(&msg);
		}
	}

	return msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage is only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass( HINSTANCE hInstance )
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof( WNDCLASSEX );

	wcex.style			= CS_DBLCLKS | CS_BYTEALIGNCLIENT;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_ZOOMPLUS);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= 0;
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= g_pcszClassName;
	wcex.hIconSm		= WinHelper::LoadIcon16x16( wcex.hInstance, IDI_ZOOMPLUS );
	ASSERT( wcex.hIconSm );

	return RegisterClassEx(&wcex);
}

inline bool SwitchToOtherInstance()
{
	HWND hwnd = FindWindow( g_pcszClassName, NULL );
	if( hwnd )
	{
		hwnd = GetLastActivePopup( hwnd );
		(void)BringWindowToTop( hwnd );
		(void)SetForegroundWindow( hwnd );
		(void)SetActiveWindow( hwnd );
		if( IsIconic( hwnd ) )
		{
			ShowWindow( hwnd, SW_RESTORE );
		}
	}
	return hwnd ? true : false;
}


bool InitInstance( HINSTANCE hInstance, int nCmdShow )
{
	int nLeft, nTop, nWidth, nHeight;

	if( g_reg.GotWindowPos() )
	{
		nLeft = g_reg.m_rcWindow.left;
		nTop = g_reg.m_rcWindow.top;
		nWidth = g_reg.m_rcWindow.right - g_reg.m_rcWindow.left;
		nHeight = g_reg.m_rcWindow.bottom - g_reg.m_rcWindow.top;
	}
	else
	{
		nLeft = nTop = CW_USEDEFAULT;
		nWidth = 450;
		nHeight = 250; 
	}

	if( g_reg.m_bSingleInstance == FALSE || !SwitchToOtherInstance() )
	{
		DWORD dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
		HWND hwnd = CreateWindowEx( WS_EX_APPWINDOW, (LPCTSTR)MyRegisterClass( hInstance ), g_szTitle, dwStyle, nLeft, nTop, nWidth, nHeight, NULL, NULL, hInstance, NULL);
		if( hwnd )
		{
			(void)ShowWindow( hwnd, nCmdShow );

			g_MainWindow.AdjustWindowPositionForScreenLayout();
			return true;
		}

		ShowLastError( NULL, g_szTitle );
	}
	return false;
}



LRESULT CALLBACK WndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message )
	{
	case WM_GETMINMAXINFO:	//	Thsi just happens to be the first message we get so...
		g_MainWindow.Attach( hwnd );
		break;
	default:
		;	//	Intentionally do nothing
	}
	return g_MainWindow.WndProc( message, wParam, lParam );
}
