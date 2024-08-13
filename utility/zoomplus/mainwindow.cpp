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

File:	MainWindow.cpp
Owner:	russf@gipsysoft.com
Purpose:	The guts of ZoomPlus
----------------------------------------------------------------------*/
#include "stdafx.h"
#include <ZMouse.h>
#include <commdlg.h>
#include "resource.h"
#include "MainWindow.h"
#include "RegistrySettings.h"
#include "ResString.h"
#include "Utils.h"
#include "ZoomPlus.hm"
#include "ownd/ownd.h"

class CUnicodeToAnsi
{
public:
	CUnicodeToAnsi( LPCWSTR pcszText, UINT uLength )
	{
		m_pszText = new char [ uLength * sizeof( TCHAR ) + 1 ];
		WideCharToMultiByte( CP_UTF8, 0, pcszText, uLength, m_pszText, uLength * sizeof( TCHAR ), NULL, NULL );
		m_pszText[ uLength ] = '\000';
	}

	~CUnicodeToAnsi()
	{
		delete[] m_pszText;
	}

	LPCWSTR AsUnicode() const { return (LPCWSTR)m_pszText; }

	operator const LPCSTR () const { return m_pszText; }

private:
	LPSTR m_pszText;
};




extern bool CopyBitmapToClipboard( HWND hwnd, HBITMAP hbm );

extern void RegisterHotkeys( HWND hwnd );
extern void UnRegisterHotkeys( HWND hwnd );

extern HCURSOR g_hcHand;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define ID_STATUSBAR	3

static UINT nMsgMouseWheel =
   (((::GetVersion() & 0x80000000) && LOBYTE(LOWORD(::GetVersion()) == 4)) ||
	 (!(::GetVersion() & 0x80000000) && LOBYTE(LOWORD(::GetVersion()) == 3)))
	 ? ::RegisterWindowMessage(MSH_MOUSEWHEEL) : 0;

enum { knMinZoomFactor = 1, knMaxZoomFactor = 32 };
enum { knToolbarHeight = 21};
enum { knMinUpdate = 1, knMaxUpdate = CRegistrySettings::knUpdateFast };
enum { knToolbarBitmapButtons = 6 };
enum { knStatusBarPartsCount = 5 };

enum { knUpdateTimerEvent = 1							//	Timer used when just using auto-update
			, knMouseUpdateTimerEvent = 2				//	Timer used when follow mouse is used
			};

extern void GetScreenLayout();
extern int g_nScreenLeft, g_nScreenTop, g_nScreenWidth, g_nScreenHeight;

static const UINT uOriginWindowUpdateMessage = ::RegisterWindowMessage( OWND_WINDOW_UPDATE );
static const UINT uOriginWindowEndMessage = ::RegisterWindowMessage( OWND_WINDOW_END );
static HMODULE g_hInstOwnd = NULL;

extern void About( HWND hwndParent );

static const CResString g_szTitle( IDS_APP_TITLE );
static const CResString g_szZoomLevelFormat( IDS_ZOOM_LEVEL_FORMAT );
static const CResString g_szAutoUpdateFormat( IDS_AUTO_UPDATE_FORMAT );
static const CResString g_szDefault( IDS_DEFAULT_STATUSBAR_TEXT );
static const CResString g_szWidestRGB( IDS_WIDEST_RGB );
static const CResString g_szWidestCOORDS( IDS_WIDEST_COORDS );
static const CResString g_szRGBFormat( IDS_RGB_FORMAT );
static const CResString g_szHEXFormat( IDS_HEX_FORMAT );
static const CResString g_szCoordsFormat( IDS_COORDS_FORMAT );
static const CResString g_szGipsysoft( IDS_GIPSYSOFT );
static const CResString g_szDragSelectionTip( IDS_DRAG_SELECTION_TIP );

extern bool ShowOptionsDialog( HWND hwnd );

static LPCTSTR g_pcszHelpFileNameBare = _T("ZoomPlus.hlp");

static LPCTSTR g_pcszUpdateDescriptions[] = 
{
	_T("None"), 
	_T("Slow"),
	_T("Medium"), 
	_T("Fast"),
};

CMainWindow::CMainWindow()
	: m_hwnd( NULL )
	, m_cxZoomRatio( 0 )
	, m_cyZoomRatio( 0 )
	, m_bTracking( false )
	, m_hPalette( CreatePhysicalPalette() )
	, m_bRebarCreated( false )
	, m_nToolbarHeight( 0 )
	, m_hwndToolbar( NULL )
	, m_hwndZoomSlider( NULL )
	, m_hwndUpdateSlider( NULL )
	, m_hrgnMain( NULL )
	, m_bNoDraw( false )
	, m_bRightDrag( false )
	, m_bDisplayingSelection( false )
	, m_hMouseCursor( NULL )
{
	m_ptTracking = g_reg.m_ptZoom;
	GetScreenLayout();
}


CMainWindow::~CMainWindow()
{
	if( m_hrgnMain )
		VERIFY( DeleteObject( m_hrgnMain ) );		
	if( m_hPalette )
		VERIFY( DeleteObject( m_hPalette ) );
}


void CMainWindow::GetClientRect( RECT &rc ) const
//
//	Get the client rectangle, actually gets the size of teh area
//	that we can draw on.
{
	(void)::GetClientRect( m_hwnd, &rc );

	if( g_reg.m_bDisplayToolbar )
		rc.top += m_nToolbarHeight;

	if( g_reg.m_bDisplayStatusbar )
		rc.bottom -= m_nStatusBarHeight;
}


void CMainWindow::UpdateNow( ) const
//
//	Force an update of the client area of teh window.
{
	HDC hdc = GetDC( m_hwnd );
	Draw( hdc );
	ReleaseDC( m_hwnd, hdc );
}


LRESULT CMainWindow::WndProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message )
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			(void)BeginPaint( m_hwnd, &ps );
			Draw( ps.hdc );
			(void)EndPaint( m_hwnd, &ps );
		}
		break;


	case WM_ERASEBKGND:
		break;


	case WM_DRAWITEM:
		if ((int)wParam == ID_STATUSBAR)
		{
			LPDRAWITEMSTRUCT lpDis = (LPDRAWITEMSTRUCT)lParam;
			if( lpDis->itemID == 2 )
			{
				HBRUSH hbrush = CreateSolidBrush( m_crColorAtMousePoint );
				FillRect( lpDis->hDC, &lpDis->rcItem, hbrush );
				DeleteObject( hbrush );
			}
			else if( lpDis->itemID == 4 )
			{
				HFONT hFont = reinterpret_cast<HFONT>( ::SendMessage( lpDis->hwndItem, WM_GETFONT, 0, 0 ) );
				LOGFONT lf;
				GetObject( hFont, sizeof( lf ), &lf );
				lf.lfUnderline = TRUE;
				HGDIOBJ hfontNew = CreateFontIndirect( &lf );
				hfontNew = SelectObject( lpDis->hDC, hfontNew );
				SetTextColor( lpDis->hDC, RGB( 0, 0, 255 ) );
				SetBkMode( lpDis->hDC, TRANSPARENT );
				DrawText( lpDis->hDC, g_szGipsysoft, g_szGipsysoft.GetLength(), &lpDis->rcItem, DT_CENTER );

				DeleteObject( SelectObject( lpDis->hDC, hfontNew ) );
			}
		}
		break;


	case WM_SETCURSOR:
		{
			WinHelper::CPoint pt;
			GetCursorPos( pt );
			if( PtInWindow( m_hwndStatusBar, pt ) )
			{
				WinHelper::CPoint ptStatus( pt );
				(void)ScreenToClient( m_hwndStatusBar, &ptStatus );
				WinHelper::CRect rc;
				if( ::SendMessage( m_hwndStatusBar, SB_GETRECT, 4, reinterpret_cast<LPARAM>( &rc ) ) )
				{
					if( rc.PtInRect( ptStatus ) )
					{
						SetCursor( g_hcHand );
						return TRUE;
					}
				}
			}
		}
		return DefWindowProc( m_hwnd, message, wParam, lParam );				
		

	case WM_MOUSEMOVE:
		if( m_bTracking )
		{
			//
			//	Undo the tracking rectangle
			//	Reposition it due to our new mouse position
			//	and redraw the tracking rectangle
			DrawTrackingRect();
			VERIFY( GetCursorPos( &m_ptTracking ) );
			CalcTrackingRect();
			DrawTrackingRect();
			UpdateNow();
		}
		else if( m_bRightDrag )
		{
			WinHelper::CRect rcClient;
			GetClientRect( rcClient );
			POINTS pt = MAKEPOINTS( lParam );
			m_ptRightDragEnd.x = min( rcClient.right, max( pt.x, rcClient.left ) );
			m_ptRightDragEnd.y = min( rcClient.bottom, max( pt.y, rcClient.top ) );

			UpdateSelection();
		}
		break;


	case WM_SIZE:
		if( wParam != SIZE_MINIMIZED )
		{
			if( LOWORD(lParam) && HIWORD(lParam) )
			{
				RecalcLayout();
				UpdateNow();
				UpdateStatusbarPanels();
			}
		}
		break;


	case WM_TIMER:

		//
		//	If we are minimised then do nothing
		if( IsIconic( m_hwnd ) )
			break;

		if( wParam == knUpdateTimerEvent )
		{
			if( !m_bDisplayingSelection && !m_bRightDrag )
			{
				(void)InvalidateRgn( m_hwnd, m_hrgnMain, FALSE );
			}
		}
		else if( wParam == knMouseUpdateTimerEvent )
		{
			POINT pt;
			VERIFY( GetCursorPos( &pt ) );
			if( g_reg.m_bMouseTracking && !m_bDisplayingSelection && !m_bRightDrag )
			{
				if( pt.x != m_ptTracking.x || pt.y != m_ptTracking.y )
				{
					m_ptTracking = pt;
					UpdateNow();
				}
			}

			TCHAR szPosition[ 64 ];		//	Enough for large coordinates
			if( g_reg.m_bRelativeCoords )
			{
				POINT pt2 = pt;
				HWND hwndPoint = WindowFromPoint( pt2 );
				if( hwndPoint )
					ScreenToClient( hwndPoint, &pt2 );
				wsprintf( szPosition, g_szCoordsFormat, pt2.x, pt2.y );
			}
			else
			{
				wsprintf( szPosition, g_szCoordsFormat, pt.x, pt.y );
			}
			VERIFY( ::SendMessage( m_hwndStatusBar, SB_SETTEXT, 3, reinterpret_cast<LPARAM>(szPosition) ) );

			HDC hdcScreen = GetDC( NULL ); 
			COLORREF cr = GetPixel( hdcScreen, pt.x, pt.y );
			ReleaseDC( NULL, hdcScreen );
			if( m_crColorAtMousePoint != cr )
			{
				m_crColorAtMousePoint = cr;
				TCHAR szRGB[ 20 ];
				if( g_reg.m_bDisplayRGBColourPanel )
				{
					wsprintf( szRGB, g_szRGBFormat, GetRValue( m_crColorAtMousePoint ), GetGValue( m_crColorAtMousePoint ), GetBValue( m_crColorAtMousePoint ) );
				}
				else
				{
					wsprintf( szRGB, g_szHEXFormat, GetRValue( m_crColorAtMousePoint ), GetGValue( m_crColorAtMousePoint ), GetBValue( m_crColorAtMousePoint ) );
				}
				VERIFY( ::SendMessage( m_hwndStatusBar, SB_SETTEXT, 1, reinterpret_cast<LPARAM>( szRGB ) ) );
				VERIFY( ::SendMessage( m_hwndStatusBar, SB_SETTEXT, SBT_OWNERDRAW | 2, reinterpret_cast<LPARAM>( _T("") ) ) );
				VERIFY( ::SendMessage( m_hwndStatusBar, SB_SETTEXT, SBT_OWNERDRAW | 4, reinterpret_cast<LPARAM>( _T("") ) ) );
			}
		}
		break;


	case WM_COMMAND:
		if( !OnCommand( LOWORD( wParam ) ) )
		{
			return DefWindowProc( m_hwnd, message, wParam, lParam );
		}
		break;


	case WM_CONTEXTMENU:
		{
			POINT pt = { LOWORD(lParam), HIWORD(lParam) };
			if( !OnContextMenu( pt ) )
				return DefWindowProc( m_hwnd, message, wParam, lParam );				
		}
		break;


	case WM_MBUTTONDOWN:
		if( m_bDisplayingSelection || m_bRightDrag )
		{
			StopRightTracking();
		}
		StopTracking();

		g_hInstOwnd = LoadLibrary( _T( OWND_LIBNAME ) );
		if( g_hInstOwnd )
		{
			StartPanProc pStartPanning = (StartPanProc)GetProcAddress( g_hInstOwnd, START_PANNING_PROC );
			if( pStartPanning )
			{
				POINT pt = { LOWORD(lParam), HIWORD(lParam) };
				pStartPanning( m_hwnd, TRUE, TRUE, pt );
			}
		}
		break;


	case WM_LBUTTONDOWN:
		if( m_bDisplayingSelection || m_bRightDrag )
		{
			StopRightTracking();
		}
		else
		{
			SetCapture( m_hwnd );
			TCHAR szBuffer[ 256 ];
			(void)lstrcpy( szBuffer, CResString( IDS_DRAG_ZOOM_INSTRUCTIONS ) );
			VERIFY( ::SendMessage( m_hwndStatusBar, SB_SETTEXT, SBT_NOBORDERS, reinterpret_cast<LPARAM>(szBuffer) ) );

			m_bTracking = true;
			VERIFY( GetCursorPos( &m_ptTracking ) );
			CalcTrackingRect();
			DrawTrackingRect();
			SetCursor( ::LoadCursor( g_hInst, MAKEINTRESOURCE( IDC_ZOOMCURSOR ) ) );

			//
			//	We need to adjust the position of the tip window a little otherwise it will appear
			//	in the top portion of the drag rectangle - which looks ugly. We also adjust it by a fudge
			//	of 3 to ensure it 'looks' just right.
			WinHelper::CPoint pt( m_ptTracking );
			pt.y -= ( m_cyHalfZoomRatio + 3 );
			m_tracktip.Set( m_hwnd, pt, g_szDragSelectionTip );


		}
		break;


	case WM_KILLFOCUS:
		//
		//	If one of our child windows gets the focus then we simply switch
		//	the focus back to us. This prevents the slider controls from getting a
		//	focus rectangle drawn around them, and if the do get the focus then none
		//	of our keys such as zoom in or out work any more! bummer eh?
		if( wParam && IsChild( reinterpret_cast<HWND>(wParam) ) )
		{
			SetFocus( m_hwnd );
		}
		break;


	case WM_LBUTTONUP:
		if( m_bTracking )
		{
			StopTracking();
			UpdateNow();
		}
		break;


	case WM_RBUTTONDOWN:
		StopTracking();
		m_ptRightDragStart.x = LOWORD(lParam);
		m_ptRightDragStart.y = HIWORD(lParam);
		m_ptRightDragEnd = m_ptRightDragStart;

		m_Marquee.Display( m_hwnd );

		m_bRightDrag = TRUE;
		SetCapture( m_hwnd );
		break;


	case WM_RBUTTONUP:
		if( m_bRightDrag )
		{
			m_bRightDrag = false;
			ReleaseCapture();
			WinHelper::CRect rc;
			GetSelectionRect( rc );

			//
			//	If there is no width or no height then we should have no ability to save the bitmap
			//	or copy the selection
			if( !rc.Width() || !rc.Height() )
			{
				m_tracktip.Destroy();
				m_Marquee.Destroy();
				m_bDisplayingSelection = false;
			}
			else
			{
				m_bDisplayingSelection = true;
			}
			POINT pt = { LOWORD(lParam), HIWORD(lParam) };
			VERIFY( ClientToScreen( m_hwnd, &pt ) );
			OnContextMenu( pt );

			if( m_bDisplayingSelection )
			{
				m_tracktip.Destroy();
				m_bDisplayingSelection = false;
				UpdateNow();
			}
		}
		break;


	case WM_NOTIFY:
		{
			LPNMHDR pnmh = (LPNMHDR) lParam;
			if( pnmh->code == TTN_NEEDTEXT )
			{
				//
				//	Need to remove the status bar help text if there is any.
				CResString str( pnmh->idFrom );
				LPTSTR pszCR = str;
				while( *pszCR && *pszCR != _T('\n') ) pszCR++;
				if( *pszCR )
					*pszCR = _T('\000');
				LPTOOLTIPTEXT lpToolTipText = (LPTOOLTIPTEXT)lParam; 
				(void)lstrcpyn( lpToolTipText->szText, str, countof( lpToolTipText->szText ) );
			}
			else if( pnmh->code == NM_CLICK && pnmh->hwndFrom == m_hwndStatusBar )
			{
				//
				//	It's our URL in the status bar. We just execute the link, simple enough.
				LPNMMOUSE lpMouse = reinterpret_cast<LPNMMOUSE>( lParam );
				if( lpMouse->dwItemSpec == 4 )
				{
					static CResString g_szVisitMe( IDS_VISIT_ME_URL );
					(void)ShellExecute( NULL, _T("open"), g_szVisitMe, NULL, NULL, SW_SHOWNORMAL );					
				}
			}
		}
		break;


	case WM_MENUSELECT:
		{
			TCHAR szBuffer[ 256 ];
			(void)lstrcpy( szBuffer, g_szDefault );

			if( lParam != NULL && HIWORD(wParam) != 0xFFFF && LOWORD(wParam) )
			{

				//
				//	Need to skip past the tool tip text if there is any and find our status bar help text
				CResString str( LOWORD(wParam) );
				LPTSTR pszCR = str;
				if( pszCR )
					while( *pszCR && *pszCR != _T('\n') ) pszCR++;
				if( pszCR && *pszCR )
				{
					(void)lstrcpy( szBuffer, pszCR + 1 );
				}
			}
			VERIFY( ::SendMessage( m_hwndStatusBar, SB_SETTEXT, SBT_NOBORDERS, reinterpret_cast<LPARAM>(szBuffer) ) );

		}
		break;

	case WM_SYSKEYDOWN:
		StopRightTracking();

	case WM_CANCELMODE:
		StopTracking();
		break;


	case WM_KEYDOWN:
		OnKeyDown( wParam );
		break;


	case WM_SETTINGCHANGE:
		AdjustWindowPositionForScreenLayout();

		RecalcLayout();
		UpdateNow();
		break;


	case WM_DESTROY:
		{
			//
			//	Save our window position.
			WINDOWPLACEMENT wpl;
			wpl.length = sizeof( wpl );
			VERIFY( GetWindowPlacement( m_hwnd, &wpl ) );
			g_reg.m_rcWindow = wpl.rcNormalPosition;
			g_reg.m_ptZoom = m_ptTracking;
			//
			//	Quit in the usual way...
			PostQuitMessage( 0 );
			WinHelp( m_hwnd, HELP_QUIT, 0 );
			UnRegisterHotkeys( m_hwnd );
		}
		break;


	case WM_MOUSEWHEEL:
		{
			StopTracking();
			StopRightTracking();

			UINT nFlags = LOWORD(wParam);
			short nDelta = HIWORD(wParam);
			if( nFlags & MK_SHIFT )
			{
				if( nDelta < 0 )
				{
					g_reg.m_nTransparency = max( g_reg.m_nTransparency - 30, 10 );
				}
				else
				{
					g_reg.m_nTransparency = min( g_reg.m_nTransparency + 30, 255 );
				}
				SetWindowTransparent( m_hwnd, g_reg.m_bTransparent, g_reg.m_nTransparency );
			}
			else if( nFlags & MK_CONTROL )
			{
				if( nDelta < 0 )
				{
					SetZoom( g_reg.m_nZoomFactor - 1 );
				}
				else
				{
					SetZoom( g_reg.m_nZoomFactor + 1 );
				}
			}
		}
		break;

	case WM_SYSCOMMAND:

		if( (wParam & 0xFFF0) == IDM_ABOUTBOX )
		{
			About( m_hwnd );
		}
		else if( (wParam & 0xFFF0) == IDM_OPTIONS )
		{
			if( ShowOptionsDialog( m_hwnd ) )
			{
				g_reg.Save();
				SetWindowTransparent( m_hwnd, g_reg.m_bTransparent, g_reg.m_nTransparency );
				UpdateNow();
			}
		}
		else
		{
			return DefWindowProc( m_hwnd, message, wParam, lParam );
		}
		break;

	case WM_HELP:
		WinHelp( m_hwnd, HELP_CONTEXT, HID_MAIN_HELP );
		break;


  case WM_HSCROLL:
		{
			if( reinterpret_cast<HWND>(lParam) == m_hwndZoomSlider )
			{
				int nNewZoom = g_reg.m_nZoomFactor;
				switch( LOWORD(wParam) )
				{
				case SB_LINEDOWN:
					nNewZoom++;
					break;

				case SB_LINEUP:
					nNewZoom--;
					break;

				case SB_PAGEUP:
					nNewZoom -= 2;
					break;

				case SB_PAGEDOWN:
					nNewZoom += 2;
					break;

				case SB_THUMBPOSITION:
				case SB_THUMBTRACK:
					nNewZoom = HIWORD(wParam);
					break;
				default:
					;	//	Intentionally do nothing
				}
				SetZoom( nNewZoom );
			}
			else if( reinterpret_cast<HWND>(lParam) == m_hwndUpdateSlider )
			{
				int nNewUpdate = g_reg.m_Update;
				switch( LOWORD(wParam) )
				{
				case SB_LINEDOWN:
					nNewUpdate++;
					break;

				case SB_LINEUP:
					nNewUpdate--;
					break;

				case SB_PAGEUP:
					nNewUpdate -= 2;
					break;

				case SB_PAGEDOWN:
					nNewUpdate += 2;
					break;

				case SB_THUMBPOSITION:
				case SB_THUMBTRACK:
					nNewUpdate = HIWORD(wParam);
					break;
				default:
					;	//	Intentionally do nothing
				}
				SetUpdate( nNewUpdate );
			}
		}
		break;


	case WM_SETFOCUS:
		UpdateNow();
		break;


	case WM_CREATE:
		if( !OnCreate() )
		{
			return -1;
		}
		break;

	case WM_HOTKEY:
		OnHotkey( wParam );
		break;

	default:
		if( message == nMsgMouseWheel )
		{
			return HandleMouseWheelRegisteredMessage( m_hwnd, wParam, lParam );
		}
		else if( message == uOriginWindowUpdateMessage )
		{
			const WinHelper::CSize *psizeDistance = reinterpret_cast<const WinHelper::CSize *>( lParam );
			MoveZoomBy( psizeDistance->cx, psizeDistance->cy );
			return 1;
		}
		else if( message == uOriginWindowEndMessage )
		{
			VAPI( FreeLibrary( g_hInstOwnd ) );
		}

		return DefWindowProc( m_hwnd, message, wParam, lParam );
  }
  return 0;
}


void CMainWindow::OnHotkey( int nID )
{
	switch( nID )
	{
	case 1:
		TRACE( _T("Copy bitmap to clipboard\n") );
		CopyBitmapToClipboard( m_hwnd, GetAsBitmap( true, false ) );
		break;

	case 2:
		{
			TCHAR szRGB[ 20 ];
			if( g_reg.m_bDisplayRGBColourPanel )
			{
				wsprintf( szRGB, _T("%d,%d,%d"), GetRValue( m_crColorAtMousePoint ), GetGValue( m_crColorAtMousePoint ), GetBValue( m_crColorAtMousePoint ) );
			}
			else
			{
				wsprintf( szRGB, _T("#%02x%02x%02x"), GetRValue( m_crColorAtMousePoint ), GetGValue( m_crColorAtMousePoint ), GetBValue( m_crColorAtMousePoint ) );
			}

			HGLOBAL hGlob = GlobalAlloc( GMEM_MOVEABLE, _tcslen( szRGB ) + 1 );
			LPSTR p = (LPSTR)GlobalLock( hGlob );
			#ifdef _UNICODE
				CUnicodeToAnsi ansi( szRGB, _tcslen( szRGB ) );
				strcpy( p, ansi );
			#else
				strcpy( p, szRGB );
			#endif

			(void)GlobalUnlock( hGlob );
			(void)OpenClipboard( m_hwnd );
			(void)EmptyClipboard();
			(void)SetClipboardData( CF_TEXT, hGlob );
			(void)CloseClipboard();
		}
		break;

	default:
		TRACE( _T("Unkown hotkey pressed\n") );
	}
}


void CMainWindow::Attach( HWND hwnd )
//
//	Attach a window handle to our simple window.
{
	ASSERT_VALID_HWND( hwnd );
	m_hwnd = hwnd;
}


bool CMainWindow::OnCommand( UINT uID )
//
//	Excercise all command messages sent to our window.
{
	switch( uID )
	{
	case IDM_EXIT:
		DestroyWindow( m_hwnd );
		return true;

	case ID_COPY_TO_CLIPBOARD:
		(void)CopyBitmapToClipboard( m_hwnd, GetAsBitmap( true, false ) );
		break;

	case ID_COPY_TO_CLIPBOARD_SCREEN:
		(void)CopyBitmapToClipboard( m_hwnd, GetAsBitmap( false, false ) );
		break;

	case ID_SAVE_AS:
		SaveBitmapToFile( m_bDisplayingSelection );
		break;

	case ID_GRID:
		g_reg.m_bShowGrid = !g_reg.m_bShowGrid;
		UpdateGridStatus();
		break;

	case ID_ALWAYSONTOP:
		g_reg.m_bAlwaysOntop = !g_reg.m_bAlwaysOntop;
		UpdateOnTopStatus();
		break;

	case ID_FOLLOW_MOUSE:
		g_reg.m_bMouseTracking = !g_reg.m_bMouseTracking;
		UpdateFollowMouse();
		break;

	case ID_ZOOMIN:
		SetZoom( g_reg.m_nZoomFactor + 1 );
		break;

	case ID_ZOOMOUT:
		SetZoom( g_reg.m_nZoomFactor - 1 );
		break;

	case ID_ABOUT:
		About( m_hwnd );
		break;

	case ID_UPDATE_PLUS:
		SetUpdate( static_cast<int>(g_reg.m_Update) + 1 );
		break;

	case ID_UPDATE_MINUS:
		SetUpdate( static_cast<int>(g_reg.m_Update) - 1 );
		break;

	case ID_2X:
		SetZoom( 2 );
		break;

	case ID_4X:
		SetZoom( 4 );
		break;

	case ID_TOGGLE_TOOLBAR:
		g_reg.m_bDisplayToolbar = !g_reg.m_bDisplayToolbar;
		UpdateToolbar();
		break;

	case ID_TOGGLE_STATUSBAR:
		g_reg.m_bDisplayStatusbar = !g_reg.m_bDisplayStatusbar;
		UpdateStatusbar();
		break;

	case ID_OPTIONS:
		if( ShowOptionsDialog( m_hwnd ) )
		{
			g_reg.Save();
			SetWindowTransparent( m_hwnd, g_reg.m_bTransparent, g_reg.m_nTransparency );
			UpdateNow();
		}
		break;		

	case ID_HELP:
		WinHelp( m_hwnd, HELP_CONTEXT, HID_MAIN_HELP );
		break;

	case ID_COPYSCREENSELECTION:
		(void)CopyBitmapToClipboard( m_hwnd, GetAsBitmap( false, true ) );
		break;

	case ID_COPYSCREENSELECTION_ZOOMED:
		(void)CopyBitmapToClipboard( m_hwnd, GetAsBitmap( true, true ) );
		break;

	case ID_SAVESELECTION:
		SaveBitmapToFile( true );
		break;

	case ID_CONTEXT_MENU:
		{
			WinHelper::CRect rc;
			GetClientRect( rc );
			POINT pt = rc.TopLeft();
			(void)ClientToScreen( m_hwnd, &pt );
			OnContextMenu( pt );
		}
		break;

	case ID_CANCEL:
		StopRightTracking();
		break;

	case ID_HIGHLIGHT:
		{
			int nCount = 4;
			CalcTrackingRect();
			while( nCount-- )
			{
				DrawTrackingRect();
				Sleep( 100 );
				DrawTrackingRect();
				Sleep( 100 );
			}
		}
		break;

	case ID_ZOOM_TO:
		ZoomToSelection();
		break;

	case ID_OPTION_SCREEN_COORDINATES:
		g_reg.m_bRelativeCoords = false;
		InvalidateRect( m_hwndStatusBar, NULL, NULL );
		break;

	case ID_OPTION_WINDOW_COORDINATES:
		g_reg.m_bRelativeCoords = true;
		InvalidateRect( m_hwndStatusBar, NULL, NULL );
		break;

	case ID_OPTION_RGB:
		g_reg.m_bDisplayRGBColourPanel = true;
		InvalidateRect( m_hwndStatusBar, NULL, NULL );
		break;

	case ID_OPTION_HEX:
		g_reg.m_bDisplayRGBColourPanel = false;
		InvalidateRect( m_hwndStatusBar, NULL, NULL );
		break;

	case ID_TOGGLE_TRANSPARENT:
		g_reg.m_bTransparent = !g_reg.m_bTransparent;
		SetWindowTransparent( m_hwnd, g_reg.m_bTransparent, g_reg.m_nTransparency );
		break;

	default:
		;//	Intentionally do nothing
	}
	return false;
}


void CMainWindow::DrawTrackingRect()
{
	HDC hdc = GetDC(NULL);
	DrawHighlightRect( hdc, m_rcTracking );
	ReleaseDC( NULL, hdc );

}


void CMainWindow::CalcTrackingRect()
{
	const int x = BOUND( m_ptTracking.x, g_nScreenLeft + m_cxHalfZoomRatio, m_nScreenMaxX );
	const int y = BOUND( m_ptTracking.y, g_nScreenTop + m_cyHalfZoomRatio, m_nScreenMaxY );

	m_rcTracking.left   = x - m_cxHalfZoomRatio - 1;
	m_rcTracking.top    = y - m_cyHalfZoomRatio - 1;
	m_rcTracking.right  = m_rcTracking.left + m_cxZoomRatio + 1;
	m_rcTracking.bottom = m_rcTracking.top + m_cyZoomRatio + 1;
}


void CMainWindow::StopTracking()
//
//	If we are tracking then stop.
{
	if( m_bTracking )
	{
		m_bTracking = false;
		ReleaseCapture();
		DrawTrackingRect();

		TCHAR szBuffer[ 256 ];
		(void)lstrcpy( szBuffer, g_szDefault );
		VERIFY( ::SendMessage( m_hwndStatusBar, SB_SETTEXT, SBT_NOBORDERS, reinterpret_cast<LPARAM>(szBuffer) ) );

		m_tracktip.Destroy();
	}

	if( m_bRightDrag )
	{
		StopRightTracking();
	}
}


void CMainWindow::OnKeyDown( int nVirtKey )
{
	const bool bCtrl = WinHelper::IsControlPressed();
	int nMoveDelta = g_reg.m_nSlowScrollRate;
	if( bCtrl )
	{
		nMoveDelta = g_reg.m_nFastScrollRate;
	}

	switch( nVirtKey )
	{
	case VK_ESCAPE:
		StopTracking();
		StopRightTracking();
		break;

	case VK_UP:			MoveZoomBy( 0, -nMoveDelta );	break;
	case VK_DOWN:		MoveZoomBy( 0, nMoveDelta );	break;
	case VK_LEFT:		MoveZoomBy( -nMoveDelta, 0 );	break;
	case VK_RIGHT:	MoveZoomBy( nMoveDelta, 0 );	break;
	case VK_PRIOR:
		{
			if( bCtrl )
				MoveZoomBy( -(m_cxZoomRatio - 1), 0 );
			else
				MoveZoomBy( 0, -(m_cxZoomRatio - 1) );
		}
		break;
	case VK_NEXT:
		{
			if( bCtrl )
				MoveZoomBy( m_cxZoomRatio - 1, 0 );
			else
				MoveZoomBy( 0, m_cxZoomRatio - 1 );
		}
		break;
	case VK_HOME:		break;
	case VK_END:		break;

	default:
		;	//	Intentionally do nothing
	}
}


void CMainWindow::Draw( HDC hdcReal ) const
{
	if( m_bNoDraw )
		return;

	WinHelper::CRect rcClient;
	GetClientRect( rcClient );

	const WinHelper::CSize size( g_reg.m_nZoomFactor * m_cxZoomRatio, g_reg.m_nZoomFactor * m_cyZoomRatio );
	HDC hdc = CreateCompatibleDC( hdcReal );
	HBITMAP hBmpMem = CreateCompatibleBitmap( hdcReal, size.cx, size.cy );
	HGDIOBJ hBmpMemOld = SelectObject( hdc, hBmpMem );

	HPALETTE hpalOld = NULL, hpalMemOld = NULL;
	if( m_hPalette )
	{
		hpalMemOld = SelectPalette(hdc, m_hPalette, FALSE);
		(void)RealizePalette(hdc);
		hpalOld = SelectPalette( hdcReal, m_hPalette, FALSE);
		RealizePalette( hdcReal );
	}

	const int x = BOUND(m_ptTracking.x, g_nScreenLeft + m_cxHalfZoomRatio, m_nScreenMaxX);
	const int y = BOUND(m_ptTracking.y, g_nScreenTop + m_cyHalfZoomRatio, m_nScreenMaxY);

	int nDrawTop;
	if( g_reg.m_bDisplayToolbar )
		nDrawTop = m_nToolbarHeight;
	else
		nDrawTop = 0;

	SetStretchBltMode( hdc, COLORONCOLOR );
	HDC hdcScreen = GetDC( NULL );
	StretchBlt( hdc, 0, 0, size.cx, size.cy, hdcScreen, x - m_cxHalfZoomRatio, y - m_cyHalfZoomRatio, m_cxZoomRatio, m_cyZoomRatio, SRCCOPY );
	ReleaseDC(NULL, hdcScreen);

	if( g_reg.m_bShowGrid && ( g_reg.m_nZoomFactor >= 2 || g_reg.m_nGridSize > 1 ) )
	{
		HGDIOBJ hOld = SelectObject( hdc, CreatePen( PS_SOLID, 1, g_reg.m_crGrid ) );
		for( UINT iX = g_reg.m_nGridSize; iX < (UINT)size.cx; iX += g_reg.m_nGridSize )
		{
			MoveToEx( hdc, iX * g_reg.m_nZoomFactor, 0, NULL );
			LineTo( hdc, iX * g_reg.m_nZoomFactor, size.cy );
		}

		for( UINT iY = g_reg.m_nGridSize; iY < (UINT)size.cy; iY+=g_reg.m_nGridSize )
		{
			MoveToEx( hdc, 0, iY * g_reg.m_nZoomFactor, NULL );
			LineTo( hdc, size.cx, iY * g_reg.m_nZoomFactor );
		}
		VAPI( ::DeleteObject( SelectObject( hdc, hOld ) ) );
	}


	if( g_reg.m_bMouseTracking && g_reg.m_bDisplayMouseWhenFollowing )
	{
		const int nCursorX = g_reg.m_nZoomFactor * ( m_cxHalfZoomRatio - ( x - m_ptTracking.x ) ) + g_reg.m_nZoomFactor / 2;
		const int nCursorY = g_reg.m_nZoomFactor * ( m_cyHalfZoomRatio - ( y - m_ptTracking.y ) ) + g_reg.m_nZoomFactor / 2;

		CURSORINFO ci = { sizeof( ci ) };
		if( g_reg.m_bUseOSCursor && ::GetCursorInfo( &ci ) )
		{
			DrawIcon( hdc, nCursorX, nCursorY, (HICON)ci.hCursor );
		}
		else
		{
			DrawIcon( hdc, nCursorX, nCursorY, (HICON)m_hMouseCursor );
		}
	}

	SetStretchBltMode(hdc, COLORONCOLOR);
	BitBlt(hdcReal, 0, nDrawTop, rcClient.Width(), rcClient.Height(), hdc, 0, 0, SRCCOPY);

	if( hpalOld )
	{
		(void)SelectPalette( hdc, hpalMemOld, FALSE );
		(void)SelectPalette( hdcReal, hpalOld, FALSE );
	}

	SelectObject( hdc, hBmpMemOld );
	DeleteObject( hBmpMem );
	DeleteDC( hdc );
}


void CMainWindow::RecalcLayout()
//
//	Recaclulate all of our 'almost' contants.
{
	WinHelper::CRect rcClient;
	GetClientRect( rcClient );
	if( rcClient.Width() < 0 || rcClient.Height() < 0 )
	{
		m_bNoDraw = true;
		return;
	}
	m_bNoDraw = false;
	(void)SetWindowPos( m_hwndRebar, NULL, 0, 0, rcClient.Width(), m_nToolbarHeight, SWP_NOZORDER | SWP_NOMOVE );
	(void)SetWindowPos( m_hwndStatusBar, NULL, 0, rcClient.bottom, rcClient.Width(), m_nStatusBarHeight, SWP_NOZORDER );

	m_cxZoomRatio = (rcClient.Width() / g_reg.m_nZoomFactor) + 1;
	m_cyZoomRatio = (rcClient.Height() / g_reg.m_nZoomFactor) + 1;
	m_cyHalfZoomRatio = m_cyZoomRatio / 2;
	m_cxHalfZoomRatio = m_cxZoomRatio / 2;

	m_nScreenMaxX = g_nScreenWidth - 1 - m_cxHalfZoomRatio;
	m_nScreenMaxY = g_nScreenHeight - 1 - m_cyHalfZoomRatio;

	if( m_hrgnMain )
		VERIFY( DeleteObject( m_hrgnMain ) );
	m_hrgnMain = CreateRectRgn( rcClient.left, rcClient.top, rcClient.right, rcClient.bottom);

	RECT rc;
	//	We go from 1 because the first item is the help text and it doesn't have a tooltip.
	for( int n = 1; n < knStatusBarPartsCount; n++ )
	{
		if( ::SendMessage( m_hwndStatusBar, SB_GETRECT, n, reinterpret_cast<LPARAM>( &rc ) ) )
		{
			m_Tooltip.UpdateToolRect( m_hwndStatusBar, n, rc );
		}
	}
}


void CMainWindow::SetZoom( int nNewZoom )
{
	nNewZoom = min( nNewZoom, knMaxZoomFactor );
	nNewZoom = max( nNewZoom, knMinZoomFactor );
	if( g_reg.m_nZoomFactor != nNewZoom )
	{
		g_reg.m_nZoomFactor = nNewZoom;
		RecalcLayout();
		UpdateNow();

		(void)SendMessage( m_hwndZoomSlider, TBM_SETPOS, TRUE, g_reg.m_nZoomFactor );
		TCHAR szBuffer[ 128 ];
		wsprintf( szBuffer, g_szZoomLevelFormat, g_reg.m_nZoomFactor );
		m_Tooltip.UpdateTipText( m_hwndZoomSlider, szBuffer );
	}
}


void CMainWindow::MoveZoomBy( int cx, int cy )
//
//	Move the view in the direction specified.
{
	bool bChanged = false;
	if( cx )
	{
		const int nNewX = BOUND(m_ptTracking.x + cx, g_nScreenLeft + m_cxHalfZoomRatio, m_nScreenMaxX);
		if( nNewX != m_ptTracking.x )
		{
			m_ptTracking.x = nNewX;
			bChanged = true;
		}
	}

	if( cy )
	{
		const int nNewY = BOUND(m_ptTracking.y + cy, g_nScreenTop + m_cyHalfZoomRatio, m_nScreenMaxY);
		if( nNewY != m_ptTracking.y )
		{
			m_ptTracking.y = nNewY;
			bChanged = true;
		}
	}

	if( bChanged )
	{
		RecalcLayout();
		UpdateNow();
	}
}


HBITMAP CMainWindow::GetAsBitmap( bool bZoomed, bool bSelection )
{
	HBITMAP hbm = NULL;
	UNREF( bSelection );
	int nGrabLeft = BOUND(m_ptTracking.x, g_nScreenLeft + m_cxHalfZoomRatio, m_nScreenMaxX) - m_cxHalfZoomRatio;
	int nGrabTop = BOUND(m_ptTracking.y, g_nScreenTop + m_cyHalfZoomRatio, m_nScreenMaxY) - m_cyHalfZoomRatio;
	int nWidth = m_cxZoomRatio;
	int nHeight = m_cyZoomRatio;
	int nGrabWidth = m_cxZoomRatio;
	int nGrabHeight = m_cyZoomRatio;

	if( bSelection )
	{
		WinHelper::CRect rc, rcClient;
		GetClientRect( rcClient );
		GetSelectionRect( rc );
		nGrabLeft += ( ( rc.left - rcClient.left ) / g_reg.m_nZoomFactor );
		nGrabTop += ( ( rc.top - rcClient.top ) / g_reg.m_nZoomFactor );
		nGrabWidth = rc.Width() / g_reg.m_nZoomFactor;
		nGrabHeight = rc.Height() / g_reg.m_nZoomFactor;

		if( bZoomed )
		{
			nWidth = rc.Width();
			nHeight = rc.Height();
		}
		else
		{
			nWidth = rc.Width() / g_reg.m_nZoomFactor;
			nHeight = rc.Height() / g_reg.m_nZoomFactor;
		}
	}
	else
	{
		if( bZoomed )
		{
			WinHelper::CRect rcTemp;
			GetClientRect( rcTemp );
			nWidth = rcTemp.Width();
			nHeight = rcTemp.Height();
		}

	}

	HDC hdcSrc = GetDC( NULL );
	if( hdcSrc )
	{
		hbm = CreateCompatibleBitmap( hdcSrc, nWidth, nHeight );
		if( hbm )
		{
			HDC hdcDst = CreateCompatibleDC( hdcSrc );
			if( hdcDst )
			{
				HGDIOBJ hbmOld = SelectObject( hdcDst, hbm );
				SetStretchBltMode( hdcDst, COLORONCOLOR );
				StretchBlt( hdcDst, 0, 0, nWidth, nHeight, hdcSrc, nGrabLeft, nGrabTop, nGrabWidth, nGrabHeight, SRCCOPY );
				
				(void)SelectObject( hdcDst, hbmOld );
				VERIFY( DeleteDC(hdcDst) );
			}
			else
			{
				VERIFY( DeleteObject( hbm ) );
			}
		}

		(void)ReleaseDC( NULL, hdcSrc );
	}
	return hbm;
}


bool CMainWindow::OnCreate()
{
	RegisterHotkeys( m_hwnd );

	m_hMouseCursor = ::LoadCursor( g_hInst, MAKEINTRESOURCE( IDC_ARROW_ZOOMPLUS ) );
	SetWindowTransparent( m_hwnd, g_reg.m_bTransparent, g_reg.m_nTransparency );
	enum { knSliderWidthMultiplier = 3, knSliderGap = 2 };

	(void)m_Tooltip.Create( g_hInst, m_hwnd );

	//
	//	Add the rebar
	DWORD dwMajor, dwMinor;
	if( SUCCEEDED( GetComCtlVersion( &dwMajor, &dwMinor ) ) && dwMajor > 4 || ( dwMajor == 4 && dwMinor >= 71 ) )
	{
		m_hwndRebar = CreateWindowEx( 0L, REBARCLASSNAME, NULL,
			 WS_VISIBLE | WS_BORDER | WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | CCS_NODIVIDER | CCS_TOP,
					0, 0, 0, 0,
			 m_hwnd,
			 (HMENU)1,
			 g_hInst,
			 NULL );
		if( m_hwndRebar )
			m_bRebarCreated = true;
	}

	//
	// Create the toolbar
	static const TBBUTTON tbButton[] =
	{
		{ 0,	ID_ALWAYSONTOP,	TBSTATE_ENABLED, TBSTYLE_CHECK, 0, 0, 0 },
		{ 0,	0,	TBSTATE_ENABLED, TBSTYLE_SEP, 0, 0, 0 },
		{ 1,	ID_SAVE_AS,	TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0 },
		{ 0,	0,	TBSTATE_ENABLED, TBSTYLE_SEP, 0, 0, 0 },
		{ 2,	ID_COPY_TO_CLIPBOARD,	TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0 },
		{ 3,	ID_COPY_TO_CLIPBOARD_SCREEN,	TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0 },
		{ 0,	0,	TBSTATE_ENABLED, TBSTYLE_SEP, 0, 0, 0 },
		{ 5,	ID_FOLLOW_MOUSE,	TBSTATE_ENABLED, TBSTYLE_CHECK, 0, 0, 0 },
		{ 0,	0,	TBSTATE_ENABLED, TBSTYLE_SEP, 0, 0, 0 },
		{ 6,	ID_GRID,	TBSTATE_ENABLED, TBSTYLE_CHECK, 0, 0, 0 },
		{ 0,	0,	TBSTATE_ENABLED, TBSTYLE_SEP, 0, 0, 0 },
		{ 7,	ID_OPTIONS,	TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0 },
		{ 0,	0,	TBSTATE_ENABLED, TBSTYLE_SEP, 0, 0, 0 },
	};

	HWND hwndToolbarParent = m_hwndRebar;
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | TBSTYLE_FLAT | TBSTYLE_TOOLTIPS | CCS_NOPARENTALIGN | CCS_NORESIZE | CCS_NODIVIDER;
	if(	!m_bRebarCreated )
	{
		hwndToolbarParent = m_hwnd;
		dwStyle = WS_BORDER | WS_CHILD | WS_VISIBLE | TBSTYLE_TOOLTIPS | CCS_NODIVIDER | CCS_TOP;
	}
  m_hwndToolbar = CreateToolbarEx( hwndToolbarParent,
                                dwStyle, 
                                ID_TOOLBAR, 
                                knToolbarBitmapButtons,
                                g_hInst,
                                IDR_MAIN_TOOLBAR,
                                tbButton,
                                0,//countof(tbButton),		//	Set zero buttons so we can set the tooltip control below
                                16,
                                15,
                                64,
                                15,
                                sizeof(TBBUTTON));

	ASSERT( m_hwndToolbar );
	if( !m_bRebarCreated )
		m_hwndRebar = m_hwndToolbar;

	(void)SendMessage( m_hwndToolbar, TB_SETTOOLTIPS, (WPARAM)m_Tooltip.GetSafeHwnd(), 0 );
	VERIFY( SendMessage( m_hwndToolbar, TB_ADDBUTTONS, countof(tbButton), (LPARAM)tbButton ) );
	VERIFY( SendMessage( m_hwndToolbar, TB_SETINDENT, 4, 0 ) );

	if( m_bRebarCreated )
	{
		WinHelper::CRect m_rcTemp;
		GetWindowRect( m_hwndToolbar,m_rcTemp );
		REBARBANDINFO  rbbi = { 0 };
		rbbi.cbSize       = sizeof(REBARBANDINFO);
		rbbi.fMask        = RBBIM_SIZE | 
												RBBIM_STYLE | 
												RBBIM_CHILD |
												RBBIM_CHILDSIZE |
												0;
		rbbi.cxMinChild   = 0;
		rbbi.cyMinChild   = knToolbarHeight;
		rbbi.hwndChild    = m_hwndToolbar;
		rbbi.cx           = 100;
		rbbi.fStyle       = RBBS_CHILDEDGE |
												RBS_FIXEDORDER |
												0;
		VERIFY( ::SendMessage( m_hwndRebar, RB_INSERTBAND, (WPARAM)-1, (LPARAM)(LPREBARBANDINFO)&rbbi ) );
	}

	WinHelper::CRect rcToolbar;
	GetWindowRect( m_hwndRebar, rcToolbar );
	m_nToolbarHeight = rcToolbar.Height() - 2;


	//
	//	Create the zoom slider
	WinHelper::CRect rcSlider;
	VERIFY( SendMessage( m_hwndToolbar, TB_GETRECT, ID_OPTIONS, (LPARAM)&rcSlider ) );
	(void)SetRect( &rcSlider, rcSlider.right + knSliderGap, rcSlider.top, rcSlider.right + knSliderGap + rcSlider.Width() * knSliderWidthMultiplier, rcSlider.top + rcSlider.Height() );
	m_hwndZoomSlider = CreateWindow( TRACKBAR_CLASS,
																NULL,
																WS_CHILD | WS_VISIBLE | WS_TABSTOP | TBS_HORZ | TBS_NOTICKS | WS_GROUP | TBS_FIXEDLENGTH,
																rcSlider.left, rcSlider.top, 
																rcSlider.Width(), rcSlider.Height(), 
																m_hwnd, 
																NULL,
																g_hInst,
																NULL);
	(void)SendMessage( m_hwndZoomSlider, TBM_SETRANGE, TRUE, MAKELONG( knMinZoomFactor, knMaxZoomFactor ) );
	(void)SendMessage( m_hwndZoomSlider, TBM_SETPOS, TRUE, g_reg.m_nZoomFactor );

	//
	//	Add the zoom slider to the toolbar
	VERIFY( SetParent( m_hwndZoomSlider, m_hwndToolbar ) );


	//
	//	Create the zoom slider
	(void)SetRect( &rcSlider, rcSlider.right + knSliderGap, rcSlider.top, rcSlider.right + knSliderGap + rcSlider.Width() / 2, rcSlider.top + rcSlider.Height() );
	m_hwndUpdateSlider = CreateWindow( TRACKBAR_CLASS,
																NULL,
																WS_CHILD | WS_VISIBLE | WS_TABSTOP | TBS_HORZ | TBS_NOTICKS | WS_GROUP | TBS_FIXEDLENGTH,
																rcSlider.left, rcSlider.top, 
																rcSlider.Width(), rcSlider.Height(), 
																m_hwnd, 
																NULL,
																g_hInst,
																NULL);
	(void)SendMessage( m_hwndUpdateSlider, TBM_SETRANGE, TRUE, MAKELONG( knMinUpdate, knMaxUpdate ) );
	(void)SendMessage( m_hwndUpdateSlider, TBM_SETPOS, TRUE, g_reg.m_Update );


	//
	//	Add the update slider to the toolbar
	VERIFY( SetParent( m_hwndUpdateSlider, m_hwndToolbar ) );

	//
	//	Adjust the slider heights, this prevents the sliders from having their points chopped off
	//	in when there is no rebar control.
	if( !m_bRebarCreated )
	{
		int nLength = SendMessage( m_hwndZoomSlider, TBM_GETTHUMBLENGTH, 0, 0 );
		(void)SendMessage( m_hwndZoomSlider, TBM_SETTHUMBLENGTH, nLength - 2, 0 );
		(void)SendMessage( m_hwndUpdateSlider, TBM_SETTHUMBLENGTH, nLength - 2, 0 );
	}
	//
	//	Status bar now.
	m_hwndStatusBar = CreateStatusWindow( WS_BORDER | WS_VISIBLE | WS_CHILD | CCS_BOTTOM | SBARS_SIZEGRIP, g_szDefault, m_hwnd, ID_STATUSBAR );
	VERIFY( ::SendMessage( m_hwndStatusBar, SB_SETTEXT, SBT_NOBORDERS, reinterpret_cast<LPARAM>( (LPCTSTR)g_szDefault ) ) );
	ASSERT( m_hwndStatusBar );
	(void)::SendMessage( m_hwndStatusBar, WM_SETFONT, reinterpret_cast<WPARAM>( GetStockObject( DEFAULT_GUI_FONT ) ), 0 );

	WinHelper::CRect rcStatus;
	GetWindowRect( m_hwndStatusBar, rcStatus );
	m_nStatusBarHeight = rcStatus.Height();

	UpdateStatusbarPanels();

	AddTooltip();

	//
	//	Any further startup processing
	FILETIME ftHelp;
	if( GetFileTime( g_pcszHelpFileNameBare, ftHelp ) && CompareFileTime( &ftHelp, &g_reg.m_ftHelp ) > 0 )
	{
		g_reg.m_ftHelp = ftHelp;
		WinHelp( m_hwnd, HELP_CONTEXT, HID_MAIN_HELP );
	}

	SetUpdate( g_reg.m_Update, true );

	SetTimer( m_hwnd, knMouseUpdateTimerEvent, 100, NULL );

	UpdateOnTopStatus();
	UpdateGridStatus();
	UpdateToolbar();
	UpdateStatusbar();


	//
	//	Modify the system menu, adding the About... item to it.
	HMENU hmenu = GetSystemMenu( m_hwnd, FALSE );
	if( hmenu )
	{
		CResString szOptions( IDS_OPTIONS );
		VERIFY( AppendMenu( hmenu, MF_SEPARATOR, 0, NULL ) );
		VERIFY( AppendMenu( hmenu, MF_STRING, IDM_OPTIONS, szOptions ) );
		
		CResString szAbout( IDS_ABOUT_ZOOM_PLUS );
		VERIFY( AppendMenu( hmenu, MF_SEPARATOR, 0, NULL ) );
		VERIFY( AppendMenu( hmenu, MF_STRING, IDM_ABOUTBOX, szAbout ) );

	}

	return true;
}


void CMainWindow::UpdateOnTopStatus()
//
//	Either adds or removes the on top status of teh window dependent
//	on the setting from the registry
{
	DWORD dwFlags = TBSTATE_ENABLED;
	HWND hwnd = HWND_NOTOPMOST;
	if( g_reg.m_bAlwaysOntop )
	{
		hwnd = HWND_TOPMOST;
		dwFlags |= TBSTATE_CHECKED;
	}
	VERIFY( SetWindowPos( m_hwnd, hwnd, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE ) );
	VERIFY( ::SendMessage( m_hwndToolbar, TB_SETSTATE, ID_ALWAYSONTOP, (LPARAM) MAKELONG( dwFlags, 0 ) ) );
}


void CMainWindow::UpdateGridStatus()
{
	DWORD dwFlags = TBSTATE_ENABLED;
	HWND hwnd = HWND_NOTOPMOST;
	if( g_reg.m_bShowGrid )
	{
		hwnd = HWND_TOPMOST;
		dwFlags |= TBSTATE_CHECKED;
	}
	VERIFY( ::SendMessage( m_hwndToolbar, TB_SETSTATE, ID_GRID, (LPARAM) MAKELONG( dwFlags, 0 ) ) );
	UpdateNow();
}


void CMainWindow::UpdateFollowMouse()
//
//	Either switches on or oof the follow mouse status.
{
	DWORD dwFlags = 0;
	dwFlags = TBSTATE_ENABLED;

	if( g_reg.m_bMouseTracking )
	{
		dwFlags |= TBSTATE_CHECKED;
	}
	VERIFY( ::SendMessage( m_hwndToolbar, TB_SETSTATE, ID_FOLLOW_MOUSE, (LPARAM) MAKELONG( dwFlags, 0 ) ) );
	//
	//	We do an update here to force removal or display of the mouse pointer on the display. If we did not do this
	//	then we would end up with the display out of sync with the toolbar buttons.
	UpdateNow();
}


void CMainWindow::AddTooltip()
//
//	Adds the default tooltips to the tooltip control.
{
	TCHAR szBuffer[ 128 ];
	wsprintf( szBuffer, g_szZoomLevelFormat, g_reg.m_nZoomFactor);
	(void)m_Tooltip.AddTool( m_hwndZoomSlider, szBuffer );
	m_Tooltip.Activate();

	ASSERT( g_reg.m_Update - 1 < countof( g_pcszUpdateDescriptions ) );
	wsprintf( szBuffer, g_szAutoUpdateFormat, g_pcszUpdateDescriptions[ g_reg.m_Update - 1 ]);
	(void)m_Tooltip.AddTool( m_hwndUpdateSlider, szBuffer );

	RECT rc;
	::SendMessage( m_hwndStatusBar, SB_GETRECT, 1, reinterpret_cast<LPARAM>( &rc ) );
	m_Tooltip.AddTool( m_hwndStatusBar, rc, CResString( IDS_STATUSBAR_TIP_COLOR_RGB ), 1 );

	::SendMessage( m_hwndStatusBar, SB_GETRECT, 2, reinterpret_cast<LPARAM>( &rc ) );
	m_Tooltip.AddTool( m_hwndStatusBar, rc, CResString( IDS_STATUSBAR_TIP_COLOR ), 2 );

	::SendMessage( m_hwndStatusBar, SB_GETRECT, 3, reinterpret_cast<LPARAM>( &rc ) );
	m_Tooltip.AddTool( m_hwndStatusBar, rc, CResString( IDS_STATUSBAR_TIP_COORDINATES ), 3 );

	::SendMessage( m_hwndStatusBar, SB_GETRECT, 4, reinterpret_cast<LPARAM>( &rc ) );
	m_Tooltip.AddTool( m_hwndStatusBar, rc, CResString( IDS_STATUSBAR_TIP_URL ), 4 );
}


void CMainWindow::SetUpdate( int nUpdate, bool bForce )
//
//	Set the update frequency.
{
	nUpdate = min( nUpdate, knMaxUpdate );
	nUpdate = max( nUpdate, knMinUpdate );
	if( nUpdate != g_reg.m_Update || bForce )
	{
		ASSERT( g_reg.m_Update - 1 < countof( g_pcszUpdateDescriptions ) );
		TCHAR szBuffer[ 128 ];
		wsprintf( szBuffer, g_szAutoUpdateFormat, g_pcszUpdateDescriptions[ nUpdate - 1 ]);
		m_Tooltip.UpdateTipText( m_hwndUpdateSlider, szBuffer );
		if( nUpdate == CRegistrySettings::knNoUpdate && g_reg.m_Update != CRegistrySettings::knNoUpdate )
		{
			KillTimer( m_hwnd, knUpdateTimerEvent );
		}

		g_reg.m_Update = (CRegistrySettings::Update)nUpdate;
		(void)SendMessage( m_hwndUpdateSlider, TBM_SETPOS, TRUE, g_reg.m_Update );

		switch( g_reg.m_Update )
		{
		case CRegistrySettings::knUpdateSlow:		SetTimer( m_hwnd, knUpdateTimerEvent, g_reg.m_nUpdate1, NULL );	break;
		case CRegistrySettings::knUpdateMedium:	SetTimer( m_hwnd, knUpdateTimerEvent, g_reg.m_nUpdate2, NULL );	break;
		case CRegistrySettings::knUpdateFast:		SetTimer( m_hwnd, knUpdateTimerEvent, g_reg.m_nUpdate3, NULL );	break;
		case CRegistrySettings::knNoUpdate:
		case CRegistrySettings::knInvalidUpdate:
				//	Intentionally drop through
		default:
			;	//	Intentionally do nothing
		}
		UpdateFollowMouse();
	}
}


int CMainWindow::MessageBox( UINT uIDS, UINT uFlags )
{
	CResString sz( uIDS );
	CResString szCaption( IDS_APP_TITLE );
	return ::MessageBox( m_hwnd, sz, szCaption, uFlags );
}


static bool g_bZoomedImage = false;

UINT CALLBACK OFNHookProcOldStyle( HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam )
//
{
	switch( uiMsg )
	{
	case WM_INITDIALOG:
		(void)::SendDlgItemMessage( hdlg, IDC_SCREEN_IMAGE, BM_SETCHECK, g_bZoomedImage, 0 );
		break;

	case WM_COMMAND:
		switch( LOWORD(wParam) )
		{
		case IDC_SCREEN_IMAGE:
			if( HIWORD(wParam) == BN_CLICKED )
			{
				if( ::SendDlgItemMessage( hdlg, IDC_SCREEN_IMAGE, BM_GETCHECK, 0, 0 ) == BST_CHECKED )
				{
					g_bZoomedImage = true;
				}
				else
				{
					g_bZoomedImage = false;
				}
			}
		}
		break;

	case WM_HELP:
		if( reinterpret_cast<LPHELPINFO>( lParam )->iCtrlId )
		{
			DWORD aIds[ ] = { IDC_SCREEN_IMAGE, HID_SAVE_ZOOMED_IMAGE
												, 0, 0
											};
			::WinHelp( GetDlgItem( hdlg, IDC_SCREEN_IMAGE ), HELP_WM_HELP, (DWORD)(LPVOID)aIds );
			return TRUE;
		}
		break;

	case WM_CONTEXTMENU:
		{
			POINT pt = { LOWORD(lParam), HIWORD(lParam) };
			HWND hwndChild = WindowFromPoint( pt );
			if( hwndChild == GetDlgItem( hdlg, IDC_SCREEN_IMAGE ) )
			{
				DWORD aIds[ ] = { IDC_SCREEN_IMAGE, HID_SAVE_ZOOMED_IMAGE
													, 0, 0
												};
				::WinHelp( hwndChild, HELP_CONTEXTMENU, (DWORD)(LPVOID)aIds );
				return TRUE;
			}
			break;
		}
	}
	return 0;
}


void CMainWindow::SaveBitmapToFile(bool bSelection)
{
	TCHAR szBuffer[ MAX_PATH + MAX_PATH ] = _T("");
	OPENFILENAME ofn = { 0 };
	ofn.hwndOwner = m_hwnd;
	ofn.lStructSize = sizeof( ofn );
	ofn.lpstrInitialDir = g_reg.m_szInitialDirectory;
	CResString pszFilter( IDS_SAVE_AS_FILE_MASK );
	strrepc( pszFilter, _T('|'), _T('\000') );
	
	ofn.lpstrFilter = pszFilter;
	ofn.lpstrFile = szBuffer;
	ofn.nMaxFile = countof( szBuffer );
	ofn.hInstance = g_hInst;
	ofn.Flags = OFN_ENABLESIZING | OFN_NOREADONLYRETURN | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY | OFN_EXPLORER | OFN_ENABLETEMPLATE | OFN_ENABLEHOOK;
	ofn.lpTemplateName = MAKEINTRESOURCE( IDD_SAVE_BITMAP_TEMPLATE );
	ofn.lpfnHook = OFNHookProcOldStyle;
	ofn.lpstrDefExt = _T("bmp");

	g_bZoomedImage = g_reg.m_bSaveZoomedImage ? true : false;
	if( GetSaveFileName( &ofn ) )
	{
		g_reg.m_bSaveZoomedImage = g_bZoomedImage;
		HBITMAP hbm = GetAsBitmap( g_bZoomedImage, bSelection );
		RemoveFilename( ofn.lpstrFile, g_reg.m_szInitialDirectory );

		HANDLE hDib = DDBToDIB( hbm, BI_RGB, m_hPalette );
		if( hDib )
		{
			if( !WriteDIB( ofn.lpstrFile, hDib ) )
			{
				(void)MessageBox( IDS_FAILED_TO_WRITE_DIB, MB_OK | MB_ICONHAND );
			}
			VERIFY( GlobalFree( hDib ) == NULL );
		}
		else
		{
			(void)MessageBox( IDS_FAILED_TO_ACQUIRE_DIB, MB_OK | MB_ICONHAND );
		}
		VERIFY( DeleteObject( hbm ) );
	}

}


bool CMainWindow::GetHelpIDFromPoint( const POINT pt, DWORD &dwHelpID ) const
//
//	Given an arbitrary point on the main display find the help ID
//	associated with it.
{
	dwHelpID = 0;
	POINT ptClient = pt;
	(void)ScreenToClient( m_hwnd, &ptClient );

	//
	//	Start big an work down from there, ending up at the toolbar buttons etc.
	if( m_hrgnMain && PtInRegion( m_hrgnMain, ptClient.x, ptClient.y ) )
	{
		dwHelpID = HID_MAINSCREEN;
	}
	else if( PtInWindow( m_hwndUpdateSlider, pt ) )
	{
		dwHelpID = HID_UPDATE;
	}
	else if( PtInWindow( m_hwndZoomSlider, pt ) )
	{
		dwHelpID = HID_ZOOM;
	}
	else if( PtInWindow( m_hwndStatusBar, pt ) )
	{
		dwHelpID = HID_STATUSBAR;
		POINT ptStatus = pt;
		(void)ScreenToClient( m_hwndStatusBar, &ptStatus );
		for( int n = 0; n < knStatusBarPartsCount; n++ )
		{
			RECT rc;
			if( ::SendMessage( m_hwndStatusBar, SB_GETRECT, n, reinterpret_cast<LPARAM>( &rc ) ) )
			{
				if( PtInRect( &rc, ptStatus ) )
				{
					dwHelpID = HID_STATUSBAR + n;
				}
			}
		}
	}
	else
	{
		//
		//	Determine if the point is on a toolbar button, if not
		//	then check if point is in the toolbar and if not check if
		//	the point is in the rebar, if nothing then..well, nothing.
		POINT ptToolbar = pt;
		(void)ScreenToClient( m_hwndToolbar, &ptToolbar );
		int nIndex = ::SendMessage( m_hwndToolbar, TB_HITTEST, 0, (LPARAM)&ptToolbar );
		if( nIndex >= 0 )
		{
			TBBUTTON bt;
			if( ::SendMessage( m_hwndToolbar, TB_GETBUTTON, nIndex, (LPARAM)&bt )) 
			{
				dwHelpID = bt.idCommand;
			}
		}

		if( !dwHelpID )
		{
			if( PtInWindow( m_hwndToolbar, pt ) || PtInWindow( m_hwndRebar, pt ) )
			{
				dwHelpID = HID_TOOLBAR;
			}
		}
	}

	if( dwHelpID == 0 )
		return false;

	return true;
}


bool CMainWindow::OnContextMenu( const POINT &pt )
//
//	Context menu handler.
{
	DWORD aIds[ ] = { 1, 0
								, 0, 0
							 };

	if( GetHelpIDFromPoint( pt, aIds[1] ) )
	{
		HMENU hMenu = LoadMenu( g_hInst, MAKEINTRESOURCE( IDR_CONTEXT_MENU ) );
		HMENU hMenuPopup;
		if( aIds[ 1 ] == HID_STATUSBAR_RGB )
		{
			hMenuPopup = GetSubMenu( hMenu, 2 );
			(void)CheckMenuItem( hMenuPopup, ID_TOGGLE_STATUSBAR, MF_BYCOMMAND | ( g_reg.m_bDisplayStatusbar ? MF_CHECKED : MF_UNCHECKED ) );
			(void)CheckMenuRadioItem( hMenuPopup, ID_OPTION_RGB, ID_OPTION_HEX, g_reg.m_bDisplayRGBColourPanel ? ID_OPTION_RGB : ID_OPTION_HEX, MF_BYCOMMAND );
		}
		else if( aIds[ 1 ] == HID_STATUSBAR_COORDINATES )
		{
			hMenuPopup = GetSubMenu( hMenu, 3 );
			(void)CheckMenuItem( hMenuPopup, ID_TOGGLE_STATUSBAR, MF_BYCOMMAND | ( g_reg.m_bDisplayStatusbar ? MF_CHECKED : MF_UNCHECKED ) );
			(void)CheckMenuRadioItem( hMenuPopup, ID_OPTION_SCREEN_COORDINATES, ID_OPTION_WINDOW_COORDINATES , g_reg.m_bRelativeCoords ? ID_OPTION_WINDOW_COORDINATES : ID_OPTION_SCREEN_COORDINATES, MF_BYCOMMAND );
		}
		else if( m_bDisplayingSelection )
		{
			hMenuPopup = GetSubMenu( hMenu, 1 );
			(void)EnableMenuItem( hMenuPopup, ID_ZOOM_TO, MF_BYCOMMAND | ( g_reg.m_nZoomFactor == knMaxZoomFactor ? MF_GRAYED : MF_ENABLED ) );
		}
		else
		{
			hMenuPopup = GetSubMenu( hMenu, 0 );
			(void)CheckMenuItem( hMenuPopup, ID_ALWAYSONTOP, MF_BYCOMMAND | ( g_reg.m_bAlwaysOntop ? MF_CHECKED : MF_UNCHECKED ) );
			(void)CheckMenuItem( hMenuPopup, ID_FOLLOW_MOUSE, MF_BYCOMMAND | ( g_reg.m_bMouseTracking ? MF_CHECKED : MF_UNCHECKED ) );
			(void)CheckMenuItem( hMenuPopup, ID_TOGGLE_TOOLBAR, MF_BYCOMMAND | ( g_reg.m_bDisplayToolbar ? MF_CHECKED : MF_UNCHECKED ) );
			(void)CheckMenuItem( hMenuPopup, ID_TOGGLE_TRANSPARENT, MF_BYCOMMAND | ( g_reg.m_bTransparent? MF_CHECKED : MF_UNCHECKED ) );

			(void)CheckMenuItem( hMenuPopup, ID_TOGGLE_STATUSBAR, MF_BYCOMMAND | ( g_reg.m_bDisplayStatusbar ? MF_CHECKED : MF_UNCHECKED ) );
			(void)CheckMenuItem( hMenuPopup, ID_2X, MF_BYCOMMAND | ( g_reg.m_nZoomFactor == 2 ? MF_CHECKED : MF_UNCHECKED ) );
			(void)CheckMenuItem( hMenuPopup, ID_4X, MF_BYCOMMAND | ( g_reg.m_nZoomFactor == 4 ? MF_CHECKED : MF_UNCHECKED ) );
		}

		UINT nCommand = TrackPopupMenu( hMenuPopup, TPM_RETURNCMD | TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, 0, m_hwnd, NULL );
		VERIFY( DestroyMenu( hMenu ) );

		if( nCommand == ID_WHATS_THIS )
		{
			POINT ptClient = pt;
			(void)ScreenToClient( m_hwnd, &ptClient );
			//
			//	I create a small window at the point at which the What's This will appear, I give it the ID of 1 because that
			//	is what WinHelp will look for. This makes it really easy to provide context help for pretty much any point
			//	on the display. The GetHelpIDFromPoint function will fill in the help ID for us the rest is history.
			HWND hwndChild = CreateWindow( _T("STATIC"), NULL, WS_CHILD, ptClient.x, ptClient.y, 10, 10, m_hwnd, (HMENU)1, g_hInst, 0 );
			::WinHelp( hwndChild, HELP_WM_HELP, (DWORD)(LPVOID)aIds );
			VERIFY( ::DestroyWindow( hwndChild ) );
		}
		else
		{
			StopRightTracking();
			OnCommand( nCommand );
		}
		return true;
	}
	StopRightTracking();

	return false;
}


void CMainWindow::UpdateToolbar()
//
//	either hide or show the toolbar and force a recalculate of the display,
{
	ShowWindow( m_hwndRebar, g_reg.m_bDisplayToolbar ? SW_SHOW : SW_HIDE );
	RecalcLayout();
	UpdateNow();
}


void CMainWindow::UpdateStatusbar()
//
//	either hide or show the statusbar and force a recalculate of the display,
{
	ShowWindow( m_hwndStatusBar, g_reg.m_bDisplayStatusbar ? SW_SHOW : SW_HIDE );
	RecalcLayout();
	UpdateNow();
}


void CMainWindow::UpdateStatusbarPanels()
//
//	Recalculate the status bar panels
{
	//
	//	We want the first panel to be flat, and to be stretchy, the second panel will have the
	//	RGB value in it, the third panel is a little colour panel, and then last panel is the
	//	mouse coordinates.
	//	We know the text of mouse coordinates, and we know the width of the colour panel, we
	//	know the text of the RGB panel, lastly we know the text we will display by default in the
	//	stretchy panel.
	HDC hdc = GetDC( m_hwnd );
	HGDIOBJ hOldFont = SelectObject( hdc, GetStockObject( DEFAULT_GUI_FONT ) );

	const int nCXBorders = 6;	//	Thsi number taken from MFC source code, 3 pixels for each pane

	SIZE size;
	VERIFY( GetTextExtentPoint32( hdc, g_szDefault, g_szDefault.GetLength(), &size ) );
	const int nStretchyMinLength = size.cx + nCXBorders;
	VERIFY( GetTextExtentPoint32( hdc, g_szWidestRGB, g_szWidestRGB.GetLength(), &size ) );
	const int nRGBMinLength = size.cx + nCXBorders;
	VERIFY( GetTextExtentPoint32( hdc, g_szWidestCOORDS, g_szWidestCOORDS.GetLength(), &size ) );
	const int nCoordsMinLength = size.cx + nCXBorders;
	const int nColourPanelMinLength = 25 + nCXBorders;
	VERIFY( GetTextExtentPoint32( hdc, g_szGipsysoft, g_szGipsysoft.GetLength(), &size ) );
	const int nGipsySoftLength = size.cx + nCXBorders * 2;
	

	SelectObject( hdc, hOldFont );
	ReleaseDC( m_hwnd, hdc );

	int arrParts[ knStatusBarPartsCount ] = { nStretchyMinLength, nRGBMinLength, nColourPanelMinLength, nCoordsMinLength, nGipsySoftLength };
	WinHelper::CRect rcClient;
	GetClientRect( rcClient );
	if( rcClient.Width() - ( m_nStatusBarHeight + 3 - nCXBorders ) > nStretchyMinLength + nRGBMinLength +  nColourPanelMinLength + nCoordsMinLength + nGipsySoftLength )
	{
		arrParts[ 0 ] = (rcClient.Width() - ( m_nStatusBarHeight + 3 - nCXBorders ) ) - ( nRGBMinLength +  nColourPanelMinLength + nCoordsMinLength + nGipsySoftLength );
	}
	for( UINT n = 1; n < countof( arrParts ); n++ )
	{
		arrParts[ n ] += arrParts[ n - 1 ];
	}
  VERIFY( ::SendMessage(m_hwndStatusBar, SB_SETPARTS, (WPARAM) countof( arrParts ), (LPARAM) arrParts ) );
}


void CMainWindow::GetSelectionRect( WinHelper::CRect &rc ) const
{
	WinHelper::CRect rcClient;
	GetClientRect( rcClient );

	//
	//	Initialise the rectangle
	rc.left = m_ptRightDragStart.x;
	rc.top = m_ptRightDragStart.y;
	rc.right = m_ptRightDragEnd.x;
	rc.bottom = m_ptRightDragEnd.y;
	NormalizeRect( rc );
	(void)OffsetRect( &rc, -rcClient.left, -rcClient.top );

	rc.bottom = min( rc.bottom, ( rc.bottom / g_reg.m_nZoomFactor + 1) * g_reg.m_nZoomFactor );
	rc.right = min( rc.right, ( rc.right / g_reg.m_nZoomFactor + 1) * g_reg.m_nZoomFactor );

	//
	//	Easy way to ensure that the selection rectamgle falls on a zoomed pixel boundary.
	rc.top = rc.top / g_reg.m_nZoomFactor;
	rc.left = rc.left / g_reg.m_nZoomFactor;
	rc.right = rc.right / g_reg.m_nZoomFactor;
	rc.bottom = rc.bottom / g_reg.m_nZoomFactor;

	//
	//	Now undo but round our selection rectangle, this makes sure the rectangle falls on a pixel
	rc.top *= g_reg.m_nZoomFactor;
	rc.left *= g_reg.m_nZoomFactor;
	rc.right *= g_reg.m_nZoomFactor;
	rc.bottom *= g_reg.m_nZoomFactor;

	(void)OffsetRect( &rc, rcClient.left, rcClient.top );
}


void CMainWindow::UpdateSelection()
{
	WinHelper::CRect rc;
	GetSelectionRect( rc );

	//
	//	Only show if we already have or there is something to show, prevents tip appearing
	//	when the user simply right clicks and does not drag.
	if( m_tracktip.IsSet() || ( rc.Width() || rc.Height() ) )
	{
		//
		//	Add the text to the status bar, this displays the dimensions of the selection rectangle
		CResString szFormat( IDS_SELECT_RECTANGLE_DIMENSIONS );
		TCHAR szSelectionSize[ 128 ];

		wsprintf( szSelectionSize, szFormat, rc.Width() / g_reg.m_nZoomFactor, rc.Height() / g_reg.m_nZoomFactor );
		POINT ptTrackTip = m_ptRightDragStart;
		ptTrackTip.x = ptTrackTip.x / g_reg.m_nZoomFactor * g_reg.m_nZoomFactor - 5;
		ptTrackTip.y = ptTrackTip.y / g_reg.m_nZoomFactor * g_reg.m_nZoomFactor - 5;

		::ClientToScreen( m_hwnd, &ptTrackTip );
		m_tracktip.Set( m_hwnd, ptTrackTip, szSelectionSize );
	}

	WinHelper::CRect rcSelection( rc );
	MapWindowPoints( m_hwnd, HWND_DESKTOP, reinterpret_cast<LPPOINT>(&rcSelection.left), 2 );
	m_Marquee.Move( rcSelection.TopLeft(), rcSelection.Width(), rcSelection.Height() );
}


void CMainWindow::StopRightTracking()
//
//	Cease right tracking. Does the same code as in StopTracking() but also does an update.
{
	if( m_bDisplayingSelection || m_bRightDrag )
	{
		m_tracktip.Destroy();
		m_bDisplayingSelection = false;
		if( m_bRightDrag )
		{
			m_bRightDrag = false;
			ReleaseCapture();
		}
		m_Marquee.Destroy();
		UpdateNow();
	}
}


void CMainWindow::DrawHighlightRect(HDC hdc, RECT &rc)
{
	const int nWidth = rc.right-rc.left;
	const int nHeight = rc.bottom-rc.top;
	VERIFY( PatBlt( hdc, rc.left,  rc.bottom,  1, -(nHeight - 1),   DSTINVERT ) );		//	Left
	VERIFY( PatBlt( hdc, rc.left,  rc.top,     nWidth, 1,  DSTINVERT ) );						//	Top
	VERIFY( PatBlt( hdc, rc.right, rc.top,     1,   nHeight + 1,   DSTINVERT ) );		//	Right
	VERIFY( PatBlt( hdc, rc.right, rc.bottom,  -(nWidth), 1, DSTINVERT ) );					//	Bottom
}


void CMainWindow::ZoomToSelection()
//
//	Zoom our display to the selection.
{
	WinHelper::CRect rcSelection;
	GetSelectionRect( rcSelection );

	WinHelper::CRect rcClient;
	GetClientRect( rcClient );
	int nXRatio = min( rcClient.Width() / rcSelection.Width(), rcClient.Height() / rcSelection.Height() );

	//
	//	This gives us the current screen rect that we are viewing.
	CalcTrackingRect();

	POINT ptOffset;
	ptOffset.x = ( rcSelection.left - rcClient.left ) / g_reg.m_nZoomFactor;
	ptOffset.y = ( rcSelection.top - rcClient.top ) / g_reg.m_nZoomFactor;

	m_rcTracking.left += ptOffset.x;
	m_rcTracking.top += ptOffset.y;
	m_rcTracking.right = m_rcTracking.left + rcSelection.Width() / g_reg.m_nZoomFactor;
	m_rcTracking.bottom = m_rcTracking.top + rcSelection.Height() / g_reg.m_nZoomFactor;

	m_ptTracking.x = m_rcTracking.left + m_rcTracking.Width() / 2;
	m_ptTracking.y = m_rcTracking.top + m_rcTracking.Height() / 2;

	SetZoom( nXRatio * g_reg.m_nZoomFactor );
	UpdateNow();
}


void CMainWindow::AdjustWindowPositionForScreenLayout()
{
	//
	//	Need this to set some nasty global variables used for the current screen layout.
	//	We then use the information to adjust the window rectangle - just in case the previous
	//	rectangle was on a monitor (or off screen) that no longer exists.
	GetScreenLayout();

	WinHelper::CRect rcWindow;
	GetWindowRect( m_hwnd, rcWindow );
	bool bAdjusted = false;
	int nScreenRight = g_nScreenLeft + g_nScreenWidth;
	int nScreenBottom = g_nScreenTop + g_nScreenHeight;

	if( rcWindow.left < g_nScreenLeft )
	{
		rcWindow.Offset( g_nScreenLeft - rcWindow.left, 0 );
		bAdjusted = true;
	}

	if( rcWindow.right > nScreenRight )
	{
		rcWindow.Offset( nScreenRight - rcWindow.right, 0 );
		bAdjusted = true;
	}

	if( rcWindow.top < g_nScreenTop )
	{
		rcWindow.Offset( 0, g_nScreenTop - rcWindow.top);
		bAdjusted = true;
	}

	if( rcWindow.bottom > nScreenBottom )
	{
		rcWindow.Offset( 0, nScreenBottom - rcWindow.bottom );
		bAdjusted = true;
	}

	if( bAdjusted )
	{
		(void)SetWindowPos( m_hwnd, NULL, rcWindow.left, rcWindow.top, rcWindow.Width(), rcWindow.Height(), SWP_NOZORDER );
	}
}