/*----------------------------------------------------------------------
Copyright (c) 2000 Gipsysoft. All Rights Reserved.
File:	GetSpecialFolderPath.cpp
Owner:	russf@gipsysoft.com
Purpose:	Get the path to a special folder such as the desktop or the start
					menu.
----------------------------------------------------------------------*/
#include "stdafx.h"
#include <SHLOBJ.H>
#ifndef _INC_TCHAR
	#include <tchar.h>
#endif	//	_INC_TCHAR


extern bool FASTCALL GetSpecialFolderPath( LPTSTR pszPathBuffer, int nFolder );

bool FASTCALL GetSpecialFolderPath( LPTSTR pszPathBuffer, int nFolder )
//
//	Code taken from Jeffery Richter's May 1997 MSJ article and modified 
//	to add some error checking.
//
{
	*pszPathBuffer = _T('\000');

	// Allocate a pointer to an Item ID list
	LPITEMIDLIST pidl;

	bool bRetVal = false;
	// Get a pointer to an item ID list that
	// represents the path of a special folder
	if( SUCCEEDED( SHGetSpecialFolderLocation( NULL, nFolder, &pidl ) ) )
	{
		// Convert the item ID list's binary
		// representation into a file system path
		VERIFY( SHGetPathFromIDList( pidl, pszPathBuffer ) );

		// Allocate a pointer to an IMalloc interface
		LPMALLOC pMalloc;

		// Get the address of our task allocator's IMalloc interface
		if( SUCCEEDED( SHGetMalloc(&pMalloc) ) )
		{
			// Free the item ID list allocated by SHGetSpecialFolderLocation
			pMalloc->Free(pidl);   // Free our task allocator
			(void)pMalloc->Release();
			bRetVal = true;
		}
	}
	return bRetVal;
}