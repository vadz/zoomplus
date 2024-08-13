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

File:	RegistrySettings.h
Owner:	russf@gipsysoft.com
Purpose:	registry settings for the application
----------------------------------------------------------------------*/
#ifndef REGISTRYSETTINGS_H
#define REGISTRYSETTINGS_H

class CRegistrySettings
{
public:

	//	Determine if the window position has been loaded from the registy
	bool GotWindowPos() const;

	void Save();
	CRegistrySettings();
	virtual ~CRegistrySettings();

	BOOL m_bAlwaysOntop;
	enum Update { knInvalidUpdate = 0, knNoUpdate, knUpdateSlow, knUpdateMedium, knUpdateFast } m_Update;
	int m_nZoomFactor;

	RECT m_rcWindow;
	POINT m_ptZoom;

	TCHAR m_szInitialDirectory[ MAX_PATH ];
	BOOL m_bEmailed;
	BOOL m_bSaveZoomedImage;

	int m_nUpdate1;
	int m_nUpdate2;
	int m_nUpdate3;

	bool m_bMouseTracking;
	bool m_bDisplayToolbar;
	bool m_bVisited;

	FILETIME m_ftHelp;
	bool m_bDisplayStatusbar;

	bool m_bDisplayMouseWhenFollowing;
	bool m_bDisplayRGBColourPanel;
	bool m_bSingleInstance;

	bool m_bShowGrid;
	COLORREF m_crGrid;
	int m_nGridSize;
	bool m_bRelativeCoords;

	bool m_bTransparent;
	int m_nTransparency;

	bool m_bUseOSCursor;

	//
	//	Scroll rates for the keyboard
	int m_nSlowScrollRate;
	int m_nFastScrollRate;

	//
	//	Global keyboard shortcuts
	int m_nHotKeyColourVK;
	int m_nHotKeyBitmapVK;
	int m_nHotKeyColourModifiers;
	int m_nHotKeyBitmapModifiers;

private:

	HKEY m_hkeyCurrent;
};

extern CRegistrySettings g_reg;

#endif //REGISTRYSETTINGS_H