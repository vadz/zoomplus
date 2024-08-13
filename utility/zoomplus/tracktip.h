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

File:	TrackTip.h
Owner:	russf@gipsysoft.com
Purpose:	Tracking tooltip, until I can ensure the OS version works I
					think I'll do my own special one...
----------------------------------------------------------------------*/
#ifndef TRACKTIP_H
#define TRACKTIP_H

#ifndef WINDOW_H
	#include "Window.h"
#endif	//	WINDOW_H

class CTrackTip : public CWindow
{
public:
	CTrackTip();
	virtual ~CTrackTip();
	void Set( HWND hwndParent, POINT ptBottomLeft, LPCTSTR pcszText );
	inline bool IsSet() const { return m_hwnd ? true : false; }

	virtual LRESULT WndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
private:
	void Show( HWND hwndParent );

	TCHAR m_szTextBuffer[ 256 ];
};

#endif //TRACKTIP_H