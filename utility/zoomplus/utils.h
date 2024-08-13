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

File:	Utils.h
Owner:	russf@gipsysoft.com
Purpose:	Gather up all of the utility functions for this project
----------------------------------------------------------------------*/
#ifndef UTILS_H
#define UTILS_H
#pragma warning(disable: 4127)  // constant expression for TRACE/ASSERT

#define BOUND(x,min,max) ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))

void FASTCALL NormalizeRect( RECT &rc );
void FASTCALL ShowLastError( HWND hwndParent, LPCTSTR pcszCaption );
bool FASTCALL ScreenToClientRect( HWND hwnd, LPRECT lprc );
HPALETTE FASTCALL CreatePhysicalPalette();
void FASTCALL WinHelp( HWND hWndMain, UINT uCommand, DWORD dwData );

HANDLE DDBToDIB( HBITMAP hBitmap, DWORD dwCompression, HPALETTE hPal );
bool WriteDIB( LPTSTR szFile, HANDLE hDIB );

LRESULT  HandleMouseWheelRegisteredMessage( HWND hwnd, WPARAM wParam, LPARAM lParam );
void FASTCALL RemoveFilename( const TCHAR * pcszFilePath, TCHAR * pszBuffer );
void FASTCALL strrepc( TCHAR * pcszString, TCHAR cSearch, TCHAR cReplace );
bool FASTCALL PtInWindow( HWND hwnd, const POINT &pt );
DWORD FASTCALL GetModulePath( HINSTANCE hInst, LPTSTR pszBuffer, DWORD dwSize );
bool FASTCALL GetFileTime( LPCTSTR pcszFileName, FILETIME &ft );

HRESULT FASTCALL GetComCtlVersion(LPDWORD pdwMajor, LPDWORD pdwMinor);
bool FASTCALL GetSpecialFolderPath( LPTSTR pszPathBuffer, int nFolder );

void FASTCALL SetWindowTransparent( HWND hwnd, bool bTransparent, int nTransparency );

#endif //UTILS_H