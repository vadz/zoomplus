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

File:	Marquee.h
Owner:	russf@gipsysoft.com
Purpose:	Scrolling marquee, marching ants.
----------------------------------------------------------------------*/
#ifndef MARQUEE_H
#define MARQUEE_H

class CMarquee  
{
public:
	CMarquee();
	virtual ~CMarquee();

	void Draw( HDC hDC );

	void SetRect(RECT rect);

private:
	void ShiftBrushOrigin();

	// handles of bitmaps used for effect
	HBITMAP m_hHorz;
	HBITMAP m_hVert;

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

#endif //MARQUEE_H