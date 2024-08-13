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

File:	MarqueeWindow.cpp
Owner:	russf@gipsysoft.com
Purpose:	Scrolling marquee, marching ants.
					Code taken form the September 1998 article of WDJ
					Thanks Tom Brown.
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "MarqueeWindow.h"
#include "resource.h"

static ATOM g_Class = NULL;
static const int knLineWidth = 1;
static const int knLineHeight = 1;

CMarqueeWindow::CMarqueeWindow()
	: m_iLoopCnt( 0 )
	, m_hbrHorz( NULL )
	, m_hbrVert( NULL )
{
}


CMarqueeWindow::~CMarqueeWindow()
{
	if( m_hbrHorz )
		VAPI( DeleteObject( m_hbrHorz ) );
	if( m_hbrVert )
		VAPI( DeleteObject( m_hbrVert ) );

	if( g_Class )
	{
		VAPI( UnregisterClass( (LPCTSTR)g_Class, g_hInst ) );
		g_Class = NULL;
	}
}


void CMarqueeWindow::SetRect(RECT rect)
{
	m_iUpperLeftX = rect.left;
	m_iUpperLeftY = rect.top;
	m_iLowerRightX = rect.right;
	m_iLowerRightY = rect.bottom;

}


void CMarqueeWindow::ShiftBrushOrigin()
{
    // shift origin based on coordinates (see text)
    m_iLeftShift = m_iUpperLeftX % 2;
    m_iRightShift = m_iLowerRightX % 2;

    m_iTopShift = m_iUpperLeftY % 2;
    m_iBottomShift = m_iLowerRightY % 2;

#ifndef _WIN32           
    // compensate for un-explained 16 bit behaviour (see text)
    m_iTopShift += 2;
    m_iBottomShift += 2;
#endif
}


void CMarqueeWindow::Display( HWND hwndParent )
{
	if( g_Class == NULL )
	{
		WNDCLASSEX wcex = {0};

		wcex.cbSize = sizeof( WNDCLASSEX );
		wcex.style				= 0;
		wcex.hInstance		= g_hInst;
		wcex.lpszClassName	= _T("rjf_Marquee");

		g_Class = CWindow::RegisterClassEx( &wcex );
	}

	if( !m_hbrHorz )
	{
		HBITMAP hbmp = ::LoadBitmap( g_hInst, MAKEINTRESOURCE( IDB_HORIZONTAL_BITMAP ) );
		m_hbrHorz = CreatePatternBrush( hbmp );
		VAPI( DeleteObject( hbmp ) );
	}

	if( !m_hbrVert )
	{
		HBITMAP hbmp = ::LoadBitmap( g_hInst, MAKEINTRESOURCE( IDB_VERTICAL_BITMAP ) );
		m_hbrVert = CreatePatternBrush( hbmp );
		VAPI( DeleteObject( hbmp ) );
	}

	if( g_Class )
	{
		if( Create( g_Class, WS_POPUP, hwndParent ) )
		{
			SetTimer( m_hwnd, 1, 100, NULL );
		}
	}
}


void CMarqueeWindow::Destroy()
{
	KillTimer( m_hwnd, 1 );
	CWindow::Destroy();
}


void CMarqueeWindow::Move( POINT pt, int cx, int cy )
{
	VAPI( ::SetWindowPos( m_hwnd, NULL, pt.x, pt.y, cx, cy, SWP_NOACTIVATE | SWP_NOZORDER | SWP_SHOWWINDOW ) );
}


LRESULT CMarqueeWindow::WndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message )
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			(void)::BeginPaint( m_hwnd, &ps );
			HDC hdc = ps.hdc;

			int iHorzIndent = 0;
			int iVertIndent = 0;

			HBRUSH hOldBrush = NULL;

			// knLineWidth/2 -> start half a line width from edge
    
			// Then, to end half a line width from edge, 
			// means we need to cut the length of the line by a full 
			// line width
			// (the amount of the indent on both ends) 
			// thus -> indent * 2

			iHorzIndent = knLineWidth / 2;
			iVertIndent = knLineHeight / 2;

			// Top
			UnrealizeObject(m_hbrHorz);
			(void)SetBrushOrgEx(hdc,m_iLoopCnt,m_iTopShift,NULL);
			hOldBrush = (HBRUSH)SelectObject(hdc, m_hbrHorz);
        
			PatBlt(hdc, m_iUpperLeftX+iHorzIndent, m_iUpperLeftY,
				m_iLowerRightX-m_iUpperLeftX-(iHorzIndent*2), 
				knLineHeight, PATCOPY);
        
			// Bottom
			UnrealizeObject(m_hbrHorz);
			(void)SetBrushOrgEx(hdc,8-m_iLoopCnt,m_iBottomShift,NULL);
			SelectObject(hdc, m_hbrHorz);
        
			PatBlt(hdc, m_iUpperLeftX+iHorzIndent, 
				m_iLowerRightY-knLineHeight,  
				m_iLowerRightX-m_iUpperLeftX-(iHorzIndent*2), 
				knLineHeight, PATCOPY);
        
			// Left
			UnrealizeObject(m_hbrVert);
			(void)SetBrushOrgEx(hdc,m_iLeftShift,8-m_iLoopCnt,NULL);
			SelectObject(hdc, m_hbrVert);
        
			PatBlt(hdc, m_iUpperLeftX,  m_iUpperLeftY+iVertIndent, 
				knLineWidth,  
				(m_iLowerRightY-m_iUpperLeftY)-(iVertIndent*2), 
				PATCOPY);
        
			// Right
			UnrealizeObject(m_hbrVert);
			(void)SetBrushOrgEx(hdc,m_iRightShift,m_iLoopCnt,NULL);
			SelectObject(hdc, m_hbrVert);
        
			PatBlt(hdc, m_iLowerRightX-knLineWidth,  
				m_iUpperLeftY+iVertIndent, knLineWidth,  
				(m_iLowerRightY-m_iUpperLeftY)-(iVertIndent*2), 
				PATCOPY);

			// clean up 
			SelectObject(hdc, hOldBrush);

			(void)::EndPaint( m_hwnd, &ps );
		}
		break;


	case WM_SIZE:
		if( LOWORD(lParam) && HIWORD(lParam) )
		{
			WinHelper::CRect rc;
			GetClientRect( m_hwnd, rc );
			SetRect( rc );
			WinHelper::CRect rcInner( rc );
			rcInner.Inflate( -knLineWidth, -knLineWidth );
			HRGN rgn = CreateRectRgnIndirect( rc );
			HRGN rgnInner = CreateRectRgnIndirect( rcInner );
			CombineRgn( rgn, rgnInner, rgn, RGN_XOR );
			VAPI( ::DeleteObject( rgnInner ) );
			SetWindowRgn( m_hwnd, rgn, TRUE );
		}
		break;


	case WM_TIMER:
		InvalidateRect( m_hwnd, NULL, NULL );

		// inc the loop count, x2 to have a good speed march
		m_iLoopCnt+=2;
  
		if (m_iLoopCnt > 7)
			m_iLoopCnt = 0;
		break;


	default:
		return DefWindowProc( hwnd, message, wParam, lParam );
  }
  return 0;
}