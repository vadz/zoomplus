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

File:	MarqueeWindow.h
Owner:	russf@gipsysoft.com
Purpose:	Provide a marquee window for the selection and for the drag
					rectangle.
----------------------------------------------------------------------*/
#ifndef MARQUEEWINDOW_H
#define MARQUEEWINDOW_H

#ifndef WINDOW_H
	#include "Window.h"
#endif	//	WINDOW_H

class CMarqueeWindow: public CWindow
{
public:
	CMarqueeWindow();
	virtual ~CMarqueeWindow();

	void Display( HWND hwndParent );
	void Move( POINT pt, int x, int y );
	virtual void Destroy();
	virtual LRESULT WndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );

private:
	void SetRect( RECT rect );
	void ShiftBrushOrigin();

	HBRUSH m_hbrHorz;
	HBRUSH m_hbrVert;

	// amount to shift the origin of the brush based on coordinates of 
	// rectangle
	int m_iLeftShift;
	int m_iRightShift;
	int m_iTopShift;
	int m_iBottomShift;

	int m_iUpperLeftX;
	int m_iUpperLeftY;
	int m_iLowerRightX;
	int m_iLowerRightY;

	int m_iLoopCnt;

};

#endif //MARQUEEWINDOW_H