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

File:	Marquee.cpp
Owner:	russf@gipsysoft.com
Purpose:	Scrolling marquee, marching ants.
					Code taken form the September 1998 article of WDJ
					Thanks Tom Brown.
----------------------------------------------------------------------*/
#include "Stdafx.h"
#include "Marquee.h"
#include "resource.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMarquee::CMarquee()
	: m_iLoopCnt( 0 )
{
}

CMarquee::~CMarquee()
{
	if( m_hHorz )
		VERIFY( DeleteObject(m_hHorz) );
	if( m_hVert )
		VERIFY( DeleteObject(m_hVert) );

}

                
void CMarquee::SetRect(RECT rect)
{
	m_iUpperLeftX = rect.left;
	m_iUpperLeftY = rect.top;
	m_iLowerRightX = rect.right;
	m_iLowerRightY = rect.bottom;

}

///////////////////////////////////////////////////////////////////
// this changes the origin of the brush based on the rectangle 
// coordinates
void CMarquee::ShiftBrushOrigin()
{
    // shift origin based on coordinates (see text)
    m_iLeftShift = m_iUpperLeftX % 2;
    m_iRightShift = m_iLowerRightX % 2;

    m_iTopShift = m_iUpperLeftY % 2;
    m_iBottomShift = m_iLowerRightY % 2;

#ifndef _WIN32           
    // compensate for un-explained 16 bit behaviour (see text)
    m_iTopShift += 2;
    m_iBottomShift += 2;
#endif
}


///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
// this does the actual drawing of the selection rectangle
void CMarquee::Draw( HDC hDC )
{
	if( !m_hVert )
	{
		m_hHorz = ::LoadBitmap( g_hInst, MAKEINTRESOURCE( IDB_HORIZONTAL_BITMAP ) );
		m_hVert = ::LoadBitmap( g_hInst, MAKEINTRESOURCE( IDB_VERTICAL_BITMAP ) );
	}

  HBRUSH hHorz = NULL;
  HBRUSH hVert = NULL;

  int iLineWidth = 0;
  int iLineHeight = 0;
  int iHorzIndent = 0;
  int iVertIndent = 0;

  HBRUSH hOldBrush = NULL;

  // create the brushes
  hHorz = CreatePatternBrush( m_hHorz );
  hVert = CreatePatternBrush( m_hVert );
        
    // do the drawing (see text)

  iLineWidth = 1; // vertical lines
  iLineHeight = 1; // horizontal lines

  // iLineWidth/2 -> start half a line width from edge
    
  // Then, to end half a line width from edge, 
  // means we need to cut the length of the line by a full 
  // line width
  // (the amount of the indent on both ends) 
  // thus -> indent * 2

	iHorzIndent = iLineWidth/2;
	iVertIndent = iLineHeight/2;

	// Top
	UnrealizeObject(hHorz);
	(void)SetBrushOrgEx(hDC,m_iLoopCnt,m_iTopShift,NULL);
	hOldBrush = (HBRUSH)SelectObject(hDC, hHorz);
        
	PatBlt(hDC, m_iUpperLeftX+iHorzIndent, m_iUpperLeftY,   
		m_iLowerRightX-m_iUpperLeftX-(iHorzIndent*2), 
		iLineHeight, PATCOPY);
        
  // Bottom
	UnrealizeObject(hHorz);
	(void)SetBrushOrgEx(hDC,8-m_iLoopCnt,m_iBottomShift,NULL);
	SelectObject(hDC, hHorz);
        
	PatBlt(hDC, m_iUpperLeftX+iHorzIndent, 
		m_iLowerRightY-iLineHeight,  
		m_iLowerRightX-m_iUpperLeftX-(iHorzIndent*2), 
		iLineHeight, PATCOPY);
        
	// Left
	UnrealizeObject(hVert);
	(void)SetBrushOrgEx(hDC,m_iLeftShift,8-m_iLoopCnt,NULL);
	SelectObject(hDC, hVert);
        
	PatBlt(hDC, m_iUpperLeftX,  m_iUpperLeftY+iVertIndent, 
		iLineWidth,  
		(m_iLowerRightY-m_iUpperLeftY)-(iVertIndent*2), 
		PATCOPY);
        
  // Right
  UnrealizeObject(hVert);
  (void)SetBrushOrgEx(hDC,m_iRightShift,m_iLoopCnt,NULL);
  SelectObject(hDC, hVert);
        
	PatBlt(hDC, m_iLowerRightX-iLineWidth,  
		m_iUpperLeftY+iVertIndent, iLineWidth,  
		(m_iLowerRightY-m_iUpperLeftY)-(iVertIndent*2), 
		PATCOPY);

	// clean up 
	SelectObject(hDC, hOldBrush);

	DeleteObject(hVert);
	DeleteObject(hHorz);

	// inc the loop count, x2 to have a good speed march
	m_iLoopCnt+=2;
    
	if (m_iLoopCnt > 7)
		m_iLoopCnt = 0;
}

