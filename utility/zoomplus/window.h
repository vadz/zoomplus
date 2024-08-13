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

File:	Window.h
Owner:	russf@gipsysoft.com
Purpose:	base class for all of my windows
----------------------------------------------------------------------*/
#ifndef WINDOW_H
#define WINDOW_H

class CWindow
{
public:
	CWindow();
	virtual ~CWindow();

	//	Destroy the contained window.
	virtual void Destroy();

	//	Create the window
	bool Create( ATOM atomClass, DWORD dwStyle, HWND hwndParent );
	bool CreateEx( ATOM atomClass, DWORD dwStyleEx, DWORD dwStyle, HWND hwndParent );

	//	standard register for these window types
	static ATOM RegisterClassEx( WNDCLASSEX *pwndclassex );

	virtual LRESULT WndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam ) = 0;
protected:
	HWND m_hwnd;
};

#endif //WINDOW_H