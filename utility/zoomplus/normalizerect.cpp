/*----------------------------------------------------------------------
Copyright (c) 2000 Gipsysoft. All Rights Reserved.
File:	NormalizeRect.cpp
Owner:	russf@gipsysoft.com
Purpose:	Normalize the coordinates in a RECT structure
----------------------------------------------------------------------*/
#include "stdafx.h"

void FASTCALL NormalizeRect( RECT &rc )
{
	int nTemp;
	if( rc.left > rc.right )
	{
		nTemp = rc.left;
		rc.left = rc.right;
		rc.right = nTemp;
	}

	if( rc.top > rc.bottom )
	{
		nTemp = rc.top;
		rc.top = rc.bottom;
		rc.bottom = nTemp;
	}
}
