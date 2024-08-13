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

File:	MainWindow.h
Owner:	russf@gipsysoft.com
Purpose:	main window class header
----------------------------------------------------------------------*/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#ifndef TOOLTIP_H
	#include "ToolTip.h"
#endif	//	TOOLTIP_H

#ifndef MARQUEE_H
	#include "Marquee.h"
#endif	//	MARQUEE_H

#ifndef TRACKTIP_H
	#include "TrackTip.h"
#endif	//	TRACKTIP_H

#ifndef MARQUEEWINDOW_H
	#include "MarqueeWindow.h"
#endif	//	MARQUEEWINDOW_H

class CMainWindow  
{
public:
	CMainWindow();
	~CMainWindow();

	LRESULT WndProc( UINT message, WPARAM wParam, LPARAM lParam );

	void Attach( HWND hwnd );
	inline int MessageBox( UINT uIDS, UINT uFlags );

	void AdjustWindowPositionForScreenLayout();

private:
	void OnHotkey( int nID );
	void SetTransparent( bool bTransparent );
	void ZoomToSelection();
	void DrawHighlightRect( HDC hdc, RECT &rc );
	void UpdateStatusbarPanels();
	void UpdateToolbar();
	void UpdateStatusbar();
	void CalcTrackingRect();
	bool OnContextMenu( const POINT &pt );
	bool GetHelpIDFromPoint( const POINT pt, DWORD &dwHelpID ) const;

	void SaveBitmapToFile( bool bSelection );
	HBITMAP GetAsBitmap( bool bZoomed, bool bSelection );
	void AddTooltip();
	void UpdateOnTopStatus();
	void UpdateFollowMouse();
	void UpdateGridStatus();
	void Draw( HDC hdc ) const;
	void UpdateSelection();
	void MoveZoomBy( int cx, int cy );
	inline void GetClientRect( RECT &rc ) const;

	void SetZoom( int nNewZoom );

	void SetUpdate( int nUpdate, bool bForce = false );

	void RecalcLayout();
	void UpdateNow() const;

	HWND m_hwnd;

	//	Zoom ratio based on window size
	int m_cyZoomRatio, m_cxZoomRatio;
	int m_cyHalfZoomRatio, m_cxHalfZoomRatio;
	WinHelper::CRect m_rcTracking;

	int m_nScreenMaxX, m_nScreenMaxY;

	//
	//	Tracking stuff
	bool m_bTracking;
	POINT m_ptTracking;
	CTrackTip	m_tracktip;
	COLORREF m_crColorAtMousePoint;
	void DrawTrackingRect();
	void StopTracking();


	bool m_bDisplayingSelection;
	bool m_bRightDrag;
	POINT m_ptRightDragStart, m_ptRightDragEnd;
	CMarqueeWindow m_Marquee;
	void GetSelectionRect( WinHelper::CRect &rc ) const;
	void StopRightTracking();

	inline BOOL IsChild( HWND hwnd ) const { return ::IsChild( m_hwnd, hwnd ); }

	CToolTip m_Tooltip;

	HPALETTE m_hPalette;
	HRGN m_hrgnGrip;
	HRGN m_hrgnMain;

	HCURSOR m_hMouseCursor;

	//
	//	Toolbar stuff
	bool m_bRebarCreated;
	int m_nToolbarHeight;
	HWND m_hwndToolbar;
	HWND m_hwndRebar;
	HWND m_hwndZoomSlider;
	HWND m_hwndUpdateSlider;

	int m_nStatusBarHeight;
	HWND m_hwndStatusBar;

	bool m_bNoDraw;


	bool OnCommand( UINT uID );
	void OnKeyDown( int nVirtKey );
	bool OnCreate();
};


#endif //MAINWINDOW_H