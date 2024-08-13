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

File:	ToolTip.cpp
Owner:	russf@gipsysoft.com
Purpose:	wrapper for the tooltip control
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "ToolTip.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CToolTip::CToolTip()
	: m_hwnd( NULL )
{

}

CToolTip::~CToolTip()
{
	m_hwnd = NULL;
}


bool CToolTip::Create( HINSTANCE hInst, HWND hwndParent )
{
	m_hwnd = CreateWindow(TOOLTIPS_CLASS, (LPTSTR) NULL, TTS_ALWAYSTIP, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, hwndParent, (HMENU) NULL, hInst, NULL);
	if( m_hwnd )
	{
		return true;
	}
	return false;
}


bool CToolTip::AddTool( HWND hwnd, LPCTSTR pcszText )
{
	TOOLINFO ti = {0};
	ti.cbSize = sizeof(TOOLINFO);
	ti.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
	ti.hwnd = m_hwnd;
	ti.uId = (UINT) hwnd;
	if( GetClientRect( hwnd, &ti.rect ) )
	{
		ti.lpszText = const_cast<LPTSTR>( pcszText );
		if( ::SendMessage( m_hwnd, TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO) &ti)) 
		{
			return true;
		}
	}
	return false;
}


void CToolTip::Activate( BOOL bActive )
{
	(void)::SendMessage( m_hwnd, TTM_ACTIVATE, static_cast<WPARAM>(bActive), 0);
}


void CToolTip::UpdateTipText( HWND hwnd, LPCTSTR pcszText )
{
	TOOLINFO ti = {0};
	ti.cbSize = sizeof(TOOLINFO);
	ti.hwnd = m_hwnd;
	ti.uId = (UINT) hwnd;
	ti.lpszText = const_cast<LPTSTR>( pcszText );
	(void)::SendMessage( m_hwnd, TTM_UPDATETIPTEXT, 0, (LPARAM) (LPTOOLINFO) &ti);
}


bool CToolTip::AddTool( HWND hwnd, RECT &rc, LPCTSTR pcszText, UINT uID )
{
	TOOLINFO ti = {0};
	ti.cbSize = sizeof(TOOLINFO);
	ti.uFlags = TTF_SUBCLASS;
	ti.hwnd = hwnd;
	ti.uId = uID;
	ti.rect = rc;
	ti.lpszText = const_cast<LPTSTR>( pcszText );
	if( ::SendMessage( m_hwnd, TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO) &ti) )
	{
		return true;
	}
	return false;
}


void CToolTip::UpdateToolRect( HWND hwnd, UINT uID, const RECT &rc )
{
	TOOLINFO ti = {0};
	ti.cbSize = sizeof(TOOLINFO);
	ti.uFlags = 0;
	ti.hwnd = hwnd;
	ti.uId = uID;
	ti.rect = rc;
	ti.lpszText = NULL;
	(void)::SendMessage( m_hwnd, TTM_NEWTOOLRECT, 0, (LPARAM) (LPTOOLINFO) &ti );
}