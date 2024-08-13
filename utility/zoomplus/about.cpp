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

File:	About.cpp
Owner:	russf@gipsysoft.com
Purpose:	About box handler
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "resource.h"
#include "utils.h"
#include "RegistrySettings.h"
#include "ResString.h"

static const COLORREF m_crUnvisited = RGB( 0, 0, 255 );       // blue
static const COLORREF m_crVisited   = RGB( 128, 0, 128 );     // purple
static HFONT g_hFontStatic = NULL;
static CResString g_szMailMe( IDS_MAIL_ME_URL );
static CResString g_szVisitMe( IDS_VISIT_ME_URL );

extern HCURSOR g_hcHand;

static WNDPROC wndprocOldStatic = NULL;
static LRESULT CALLBACK StaticWindowProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg)
	{
	case WM_SETCURSOR:
		SetCursor( g_hcHand );
		return TRUE;
	}
	return CallWindowProc( wndprocOldStatic, hwnd, uMsg, wParam, lParam );
}


BOOL CALLBACK About( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message )
	{
	case WM_INITDIALOG:
		{
			wndprocOldStatic = (WNDPROC)SetWindowLong( GetDlgItem( hwnd, IDC_MAIL_ME_STATIC ), GWL_WNDPROC, reinterpret_cast<long>( StaticWindowProc ) );
			(WNDPROC)SetWindowLong( GetDlgItem( hwnd, IDC_VISIT_ME_STATIC ), GWL_WNDPROC, reinterpret_cast<long>( StaticWindowProc ) );
			HFONT hFont = reinterpret_cast<HFONT>( ::SendMessage( hwnd, WM_GETFONT, 0, 0 ) );
			LOGFONT lf;
			GetObject( hFont, sizeof( lf ), &lf );
			lf.lfUnderline = TRUE;
			g_hFontStatic = CreateFontIndirect( &lf );
		}
		return TRUE;


	case WM_DESTROY:
		if( g_hFontStatic )
		{
			VERIFY( DeleteObject( g_hFontStatic ) );
			g_hFontStatic = NULL;
		}
		break;


	case WM_COMMAND:
		switch( LOWORD( wParam ) )
		{
		case IDOK:
		case IDCANCEL:
			EndDialog( hwnd, LOWORD(wParam) );
			return TRUE;

		case IDC_VISIT_ME_STATIC:
			if( HIWORD( wParam ) == BN_CLICKED )
			{
				HINSTANCE h = ShellExecute( NULL, _T("open"), g_szVisitMe, NULL, NULL, SW_SHOWNORMAL );
				if( (UINT)h > 32 )
				{
					g_reg.m_bVisited = TRUE;
					InvalidateRect( reinterpret_cast<HWND>(lParam), NULL, FALSE );            // repaint to show visited color
				}
				else
				{
					MessageBeep( 0 );
					TRACE( _T("*** WARNING: StaticLink: unable to execute file %s\n"), g_szVisitMe );
				}
			}
			break;

		case IDC_MAIL_ME_STATIC:
			if( HIWORD( wParam ) == BN_CLICKED )
			{
				HINSTANCE h = ShellExecute( NULL, _T("open"), g_szMailMe, NULL, NULL, SW_SHOWNORMAL );
				if( (UINT)h > 32 )
				{
					g_reg.m_bEmailed = TRUE;
					InvalidateRect( reinterpret_cast<HWND>(lParam), NULL, FALSE );            // repaint to show visited color
				}
				else
				{
					MessageBeep( 0 );
					TRACE( _T("*** WARNING: StaticLink: unable to execute file %s\n"), g_szMailMe );
				}
			}
			break;
		}
		break;

	case WM_CTLCOLORSTATIC:
		{
			HDC hdc = reinterpret_cast<HDC>(wParam);
			HWND hwndStatic = reinterpret_cast<HWND>(lParam);
			if( hwndStatic == GetDlgItem( hwnd, IDC_MAIL_ME_STATIC ) )
			{
        SelectObject( hdc, g_hFontStatic );
        SetTextColor( hdc, g_reg.m_bEmailed ? m_crVisited : m_crUnvisited );
        SetBkMode( hdc, TRANSPARENT );
        return reinterpret_cast<BOOL>( ::GetStockObject( HOLLOW_BRUSH ) );
			}
			else if( hwndStatic == GetDlgItem( hwnd, IDC_VISIT_ME_STATIC ) )
			{
        SelectObject( hdc, g_hFontStatic );
        SetTextColor( hdc, g_reg.m_bVisited ? m_crVisited : m_crUnvisited );
        SetBkMode( hdc, TRANSPARENT );
        return reinterpret_cast<BOOL>( ::GetStockObject( HOLLOW_BRUSH ) );
			}
		}
		break;
	}
	return FALSE;
}


void About( HWND hwndParent )
{
	ASSERT_VALID_HWND( hwndParent );
	DialogBox( g_hInst, MAKEINTRESOURCE( IDD_ABOUTBOX ), hwndParent, About );
}
