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

File:	ShowOptionsDialog.cpp
Owner:	russf@gipsysoft.com
Purpose:	the options dialog box
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "resource.h"
#include "utils.h"
#include "RegistrySettings.h"
#include "ResString.h"
#include <commdlg.h>

extern void RegisterHotkeys( HWND hwnd );

static const CResString g_szTitle( IDS_APP_TITLE );
static const int knMinimumUpdate  = 1, knMaximumUpdate = 50;
static const int knMinimumGridSize = 1, knMaximumGridSize = 128;

inline int UpdateToPerSecond( int nUpdate )
{
	return 1000 / nUpdate;
}

inline int PerSecondToUpdate( int nPerSecond )
{
	return 1000 / nPerSecond;
}


bool GetDlgInt( HWND hwnd, UINT uID, int &n, UINT uMessageID )
{
	BOOL bValidate;
	n = GetDlgItemInt( hwnd, uID, &bValidate, FALSE );
	bool bRetVal = false;
	if( bValidate )
	{
		if( n >= knMinimumUpdate && n <= knMaximumUpdate )
		{
			bRetVal = true;
		}
	}
	if( !bRetVal )
	{
		CResString str( uMessageID );
		MessageBox( hwnd, str, g_szTitle, MB_OK | MB_ICONHAND );
		SetFocus( GetDlgItem( hwnd, uID ) );
	}
	return bRetVal;
}


static COLORREF g_crCurrentColor = 0;

static void DoChooseColor( HWND hwnd )
{
	COLORREF arrcr[ 16 ] = {0};
	CHOOSECOLOR cc = {0};
	cc.lStructSize = sizeof( CHOOSECOLOR );
	cc.hwndOwner = hwnd;
	cc.hInstance =(HWND) g_hInst;
	cc.rgbResult = g_crCurrentColor;
	cc.Flags = CC_PREVENTFULLOPEN | CC_SOLIDCOLOR | CC_RGBINIT;
	cc.lpCustColors = arrcr;
	if( ChooseColor( &cc ) )
	{
		g_crCurrentColor = cc.rgbResult;
	}
}


BOOL CALLBACK OptionsDialog( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch (message)
	{
	case WM_INITDIALOG:
		{
			g_crCurrentColor = g_reg.m_crGrid;
			(void)::SendMessage( GetDlgItem( hwnd, IDC_UPDATE_SLOW_SPIN ), UDM_SETRANGE, 0, MAKELONG( knMaximumUpdate, knMinimumUpdate ) );
			(void)::SendMessage( GetDlgItem( hwnd, IDC_UPDATE_MEDIUM_SPIN ), UDM_SETRANGE, 0, MAKELONG( knMaximumUpdate, knMinimumUpdate ) );
			(void)::SendMessage( GetDlgItem( hwnd, IDC_UPDATE_FAST_SPIN ), UDM_SETRANGE, 0, MAKELONG( knMaximumUpdate, knMinimumUpdate ) );

			(void)::SendMessage( GetDlgItem( hwnd, IDC_GRID_SIZE_SPIN ), UDM_SETRANGE, 0, MAKELONG( knMaximumGridSize, knMinimumGridSize ) );

			(void)SetDlgItemInt( hwnd, IDC_UPDATE_SLOW, UpdateToPerSecond( g_reg.m_nUpdate1 ), FALSE );
			(void)SetDlgItemInt( hwnd, IDC_UPDATE_MEDIUM, UpdateToPerSecond( g_reg.m_nUpdate2 ), FALSE );
			(void)SetDlgItemInt( hwnd, IDC_UPDATE_FAST, UpdateToPerSecond( g_reg.m_nUpdate3 ), FALSE );
			(void)SetDlgItemInt( hwnd, IDC_GRID_SIZE, g_reg.m_nGridSize, FALSE );
			(void)CheckDlgButton( hwnd, IDC_DISPLAY_MOUSE, g_reg.m_bDisplayMouseWhenFollowing );
			(void)CheckDlgButton( hwnd, IDC_INSTANCE, g_reg.m_bSingleInstance );
			(void)CheckDlgButton( hwnd, IDC_RELATIVE, g_reg.m_bRelativeCoords );
			(void)CheckDlgButton( hwnd, IDC_TRANSPARENT, g_reg.m_bTransparent );
			(void)CheckDlgButton( hwnd, IDC_OSCURSOR, g_reg.m_bUseOSCursor );
			
			if( g_reg.m_bDisplayRGBColourPanel == 0 )
			{
				(void)CheckRadioButton( hwnd, IDC_RGB, IDC_HEXADECIMAL, IDC_HEXADECIMAL );
			}
			else
			{
				(void)CheckRadioButton( hwnd, IDC_RGB, IDC_HEXADECIMAL, IDC_RGB );
			}
			
			SendMessage( GetDlgItem( hwnd, IDC_HOTKEY_COLOUR ), HKM_SETHOTKEY, MAKEWORD( g_reg.m_nHotKeyColourVK, g_reg.m_nHotKeyColourModifiers ), 0 );
			SendMessage( GetDlgItem( hwnd, IDC_HOTKEY_BITMAP ), HKM_SETHOTKEY, MAKEWORD( g_reg.m_nHotKeyBitmapVK, g_reg.m_nHotKeyBitmapModifiers ), 0 );
		}
		return TRUE;


	case WM_HELP:
		if( reinterpret_cast<LPHELPINFO>( lParam )->iCtrlId )
		{
			LPHELPINFO pInfo = reinterpret_cast<LPHELPINFO>( lParam );
			DWORD aIDs[ ] = { pInfo->iCtrlId, pInfo->dwContextId
												, 0, 0
											};
			TRACE( _T("ID %d, HID %d\n"), pInfo->iCtrlId, pInfo->dwContextId );
			::WinHelp( (HWND)pInfo->hItemHandle, HELP_WM_HELP, (DWORD)(LPVOID)aIDs );
			return TRUE;
		}
		break;


	case WM_CONTEXTMENU:
		{
			POINT pt = { LOWORD(lParam), HIWORD(lParam) };
			(void)ScreenToClient( hwnd, &pt );
			HWND	hwndChild = ChildWindowFromPoint( hwnd, pt );
			if( hwndChild && ::GetWindowLong( hwndChild, GWL_ID ) != -1 )
			{
				DWORD aIDs[ ] = { ::GetWindowLong( hwndChild, GWL_ID ) , GetWindowContextHelpId( hwndChild )
									, 0, 0
								 };
				TRACE( _T("ID %d, HID %d\n"), aIDs[ 0 ], aIDs[ 1 ] );
				::WinHelp( hwndChild, HELP_CONTEXTMENU, (DWORD)(LPVOID)aIDs );
			}
		}
		break;


	case WM_COMMAND:
		switch( LOWORD( wParam ) )
		{
		case IDC_COLOUR:
			{
				DoChooseColor( hwnd );
			}
			break;

		case IDOK:
			{
				g_reg.m_crGrid = g_crCurrentColor;
				int nUpdate1, nUpdate2, nUpdate3, nGridSize;
				if( !GetDlgInt( hwnd, IDC_UPDATE_SLOW, nUpdate1, IDS_OPTIONS_UPDATE_FAILED )
						|| !GetDlgInt( hwnd, IDC_UPDATE_MEDIUM, nUpdate2, IDS_OPTIONS_UPDATE_FAILED )
						|| !GetDlgInt( hwnd, IDC_UPDATE_FAST, nUpdate3, IDS_OPTIONS_UPDATE_FAILED )
						|| !GetDlgInt( hwnd, IDC_GRID_SIZE, nGridSize, IDS_OPTIONS_GRIDSIZE_FAILED )
						)
				{
					return 0;
				}
				g_reg.m_nUpdate1 = PerSecondToUpdate( nUpdate1 );
				g_reg.m_nUpdate2 = PerSecondToUpdate( nUpdate2 );
				g_reg.m_nUpdate3 = PerSecondToUpdate( nUpdate3 );
				g_reg.m_bDisplayMouseWhenFollowing = IsDlgButtonChecked( hwnd, IDC_DISPLAY_MOUSE ) ? true : false;
				g_reg.m_bDisplayRGBColourPanel = IsDlgButtonChecked( hwnd, IDC_RGB ) ? true : false;
				g_reg.m_bSingleInstance = IsDlgButtonChecked( hwnd, IDC_INSTANCE ) ? true : false;
				g_reg.m_bRelativeCoords = IsDlgButtonChecked( hwnd, IDC_RELATIVE ) ? true : false;
				g_reg.m_bTransparent = IsDlgButtonChecked( hwnd, IDC_TRANSPARENT ) ? true : false;
				g_reg.m_bUseOSCursor = IsDlgButtonChecked( hwnd, IDC_OSCURSOR ) ? true : false;
				g_reg.m_nGridSize = nGridSize;

				DWORD dwHotkey;
				dwHotkey = SendMessage( GetDlgItem( hwnd, IDC_HOTKEY_COLOUR ), HKM_GETHOTKEY, 0, 0 );
				g_reg.m_nHotKeyColourVK = LOBYTE( dwHotkey );
				g_reg.m_nHotKeyColourModifiers = HIBYTE( dwHotkey );

				dwHotkey = SendMessage( GetDlgItem( hwnd, IDC_HOTKEY_BITMAP ), HKM_GETHOTKEY, 0, 0 );
				g_reg.m_nHotKeyBitmapVK = LOBYTE( dwHotkey );
				g_reg.m_nHotKeyBitmapModifiers = HIBYTE( dwHotkey );
				
				RegisterHotkeys( GetParent( hwnd ) );

			}
			//
			//	Intentionally drop through!!

		case IDCANCEL:
			(void)EndDialog( hwnd, LOWORD(wParam) );
			return TRUE;
		}
	}
	return FALSE;
}

bool ShowOptionsDialog( HWND hwnd )
{
	ASSERT_VALID_HWND( hwnd );
	if( DialogBox( g_hInst, MAKEINTRESOURCE( IDD_OPTIONS ), hwnd, OptionsDialog ) == IDOK )
	{
		return true;
	}

	return false;
}
