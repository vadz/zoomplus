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

File:	GetComCtlVersion.cpp
Owner:	russf@gipsysoft.com
Purpose:	return the major and minor version number of the Common control DLL
					This code was taken from the help topic URL:
	mk:@MSITStore:G:\MSDNV6~1\CD1\MSDN\Shellcc.chm::/inet401/help/itt/CommCtls/Common/Common.htm
----------------------------------------------------------------------*/
#include "stdafx.h"
#include <shlwapi.h>

extern HRESULT FASTCALL GetComCtlVersion( LPDWORD pdwMajor, LPDWORD pdwMinor );

HRESULT FASTCALL GetComCtlVersion(LPDWORD pdwMajor, LPDWORD pdwMinor)
{
	if( IsBadWritePtr( pdwMajor, sizeof(DWORD) ) || IsBadWritePtr( pdwMinor, sizeof(DWORD) ) )
		return E_INVALIDARG;

	//load the DLL
	HINSTANCE hComCtl = LoadLibrary( TEXT("comctl32.dll") );

	if(hComCtl)
	{
		HRESULT           hr = S_OK;
		DLLGETVERSIONPROC pDllGetVersion;
   
		/*
		You must get this function explicitly because earlier versions of the DLL 
		don't implement this function. That makes the lack of implementation of the 
		function a version marker in itself.
		*/
		pDllGetVersion = (DLLGETVERSIONPROC)GetProcAddress(hComCtl, "DllGetVersion" );	//	Note that GetProcAddress intentionally NOT _T()
   
		if(pDllGetVersion)
		{
			DLLVERSIONINFO    dvi;
      
			ZeroMemory(&dvi, sizeof(dvi));
			dvi.cbSize = sizeof(dvi);
   
			hr = (*pDllGetVersion)(&dvi);
      
			if( SUCCEEDED( hr ) )
			{
				*pdwMajor = dvi.dwMajorVersion;
				*pdwMinor = dvi.dwMinorVersion;
			}
			else
			{
				hr = E_FAIL;
			}   
		}
		else
		{
			/*
			If GetProcAddress failed, then the DLL is a version previous to the one 
			shipped with IE 3.x.
			*/
			*pdwMajor = 4;
			*pdwMinor = 0;
		}
   
		(void)FreeLibrary(hComCtl);

		return hr;
	}

	return E_FAIL;
}
