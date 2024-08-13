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

File:	CreatePhysicalPalette.cpp
Owner:	russf@gipsysoft.com
Purpose:	Create a palette that is mapped to the current system palette
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "utils.h"


static const int knPaletteEntries = 256;


HPALETTE FASTCALL CreatePhysicalPalette()
//
//	Create a physical palette so that the grabbed images will render correctly
{
	HPALETTE hpal = NULL;
	PLOGPALETTE ppal = (PLOGPALETTE)GlobalAlloc( GPTR, sizeof(LOGPALETTE) + sizeof(PALETTEENTRY) * knPaletteEntries );
	if( ppal )
	{
		ppal->palVersion = 0x300;
		ppal->palNumEntries = knPaletteEntries;

		//lint -e662 -e661	moaning about possible out of bounds due to it's value tracking, bogus
    for( int i = 0; i < knPaletteEntries; i++ )
		{
			ppal->palPalEntry[i].peFlags = (BYTE)PC_EXPLICIT;
			ppal->palPalEntry[i].peRed   = (BYTE)i;
			ppal->palPalEntry[i].peGreen = (BYTE)0;
			ppal->palPalEntry[i].peBlue  = (BYTE)0;
    }
		//lint +e662 +e621	

		hpal = CreatePalette( ppal );
		GlobalFree( ppal );
	}

	return hpal;
}
