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

File:	ToolTip.h
Owner:	russf@gipsysoft.com
Purpose:	Wrapper for a tooltip
----------------------------------------------------------------------*/
#ifndef TOOLTIP_H
#define TOOLTIP_H


class CToolTip
{
public:
	inline HWND GetSafeHwnd() { return m_hwnd; }

	bool AddTool( HWND hwnd, RECT &rc, LPCTSTR pcszText, UINT uID );
	bool AddTool( HWND hwnd, LPCTSTR pcszText );
	void UpdateTipText( HWND hwnd, LPCTSTR pcszText );

	void UpdateToolRect( HWND hwnd, UINT uID, const RECT &rc );

	bool Create( HINSTANCE hInst, HWND hwndParent );
	void Activate( BOOL bActive = TRUE );

	CToolTip();
	virtual ~CToolTip();

private:
	HWND m_hwnd;
};

#endif //TOOLTIP_H