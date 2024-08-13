/*----------------------------------------------------------------------
Copyright (c) 1998,1999 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	SetWindowTransparent.cpp
Owner:	russf@gipsysoft.com
Purpose:	Set the transparency of a window and optionally switch it on/off.
					Dynamically loads the SetLayeredWindowAttributes function as it doesn't
					appear on all Windows versions.
----------------------------------------------------------------------*/
#include "stdafx.h"


//
//	These two are hardcoded here so you can compile this code even if you don't have the latest
//	PLatform SDK

#ifndef LWA_ALPHA
	#define LWA_ALPHA 0x02L
#endif	//	LWA_ALPHA

#ifndef WS_EX_LAYERED
	#define WS_EX_LAYERED 0x00080000L
#endif	//	WS_EX_LAYERED

extern void FASTCALL SetWindowTransparent( HWND hwnd, bool bTransparent, int nTransparency );

void FASTCALL SetWindowTransparent( HWND hwnd, bool bTransparent, int nTransparency )
{
	LONG lExStyle = GetWindowLong( hwnd, GWL_EXSTYLE );

	//
	//	Code taken straight out of the platform SDK and modified to dynamically load the
	//	required function and to *optionally* set the transparency of the window.
	if( bTransparent )
	{
		HMODULE h = LoadLibrary( _T("User32") );
		if( h )
		{
			typedef BOOL ( WINAPI *pSetLayeredWindowAttributes)( HWND hwnd, COLORREF crKey, BYTE bAlpha, DWORD dwFlags);
			pSetLayeredWindowAttributes p = (pSetLayeredWindowAttributes)GetProcAddress( h, "SetLayeredWindowAttributes" );
			if( p )
			{
				(void)SetWindowLong( hwnd, GWL_EXSTYLE, lExStyle | WS_EX_LAYERED );

				(void)p( hwnd, 0, (BYTE)nTransparency, LWA_ALPHA );
			}

			(void)FreeLibrary( h );
		}
	}
	else
	{
		// Remove WS_EX_LAYERED from this window styles
		(void)SetWindowLong( hwnd, GWL_EXSTYLE, lExStyle & ~WS_EX_LAYERED );

		// Ask the window and its children to repaint
		(void)RedrawWindow( hwnd, NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_FRAME | RDW_ALLCHILDREN );
	}
}
