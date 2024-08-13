# Microsoft Developer Studio Project File - Name="ZoomPlus" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=ZoomPlus - Win32 UNICODE Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ZoomPlus.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ZoomPlus.mak" CFG="ZoomPlus - Win32 UNICODE Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ZoomPlus - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "ZoomPlus - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "ZoomPlus - Win32 UNICODE Debug" (based on "Win32 (x86) Application")
!MESSAGE "ZoomPlus - Win32 UNICODE Release" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Utility/ZoomPlus", KJFAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ZoomPlus - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Gr /MT /W4 /WX /O1 /Op /Oy /I "..\..\components" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "STRICT" /D "OWND_DYNAMIC" /D WINVER=0x0500 /FAs /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386 /out:"../../bin/ZoomPlus.exe" /libpath:"../../bin"

!ELSEIF  "$(CFG)" == "ZoomPlus - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W4 /WX /Gm /ZI /Od /I "..\..\components" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "STRICT" /D "OWND_DYNAMIC" /D WINVER=0x0500 /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"../../bin/ZoomPlusD.exe" /pdbtype:sept /libpath:"../../bin"
# SUBTRACT LINK32 /nodefaultlib

!ELSEIF  "$(CFG)" == "ZoomPlus - Win32 UNICODE Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "ZoomPlus___Win32_UNICODE_Debug"
# PROP BASE Intermediate_Dir "ZoomPlus___Win32_UNICODE_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "UNICODE_Debug"
# PROP Intermediate_Dir "UNICODE_Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W4 /WX /Gm /ZI /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "STRICT" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W4 /WX /Gm /ZI /Od /I "..\..\components" /D "_DEBUG" /D "UNICODE" /D "_UNICODE" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "STRICT" /D "OWND_DYNAMIC" /D WINVER=0x0500 /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ../../bin/debughlp.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"../../bin/ZoomPlusDU.exe" /pdbtype:sept /libpath:"../../bin"

!ELSEIF  "$(CFG)" == "ZoomPlus - Win32 UNICODE Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ZoomPlus___Win32_UNICODE_Release"
# PROP BASE Intermediate_Dir "ZoomPlus___Win32_UNICODE_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "UNICODE_Release"
# PROP Intermediate_Dir "UNICODE_Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W4 /WX /O1 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "STRICT" /FR /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Gr /MTd /W4 /WX /O1 /I "..\..\components" /D "OWND_DYNAMIC" /D WINVER=0x0400 /D "NDEBUG" /D "UNICODE" /D "_UNICODE" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "STRICT" /D WINVER=0x0500 /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386 /out:"../../bin/ZoomPlusU.exe" /libpath:"../../bin"

!ENDIF 

# Begin Target

# Name "ZoomPlus - Win32 Release"
# Name "ZoomPlus - Win32 Debug"
# Name "ZoomPlus - Win32 UNICODE Debug"
# Name "ZoomPlus - Win32 UNICODE Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "ReUse"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CopyBitmapToClipboard.cpp
# End Source File
# Begin Source File

SOURCE=.\GetComCtlVersion.cpp
# End Source File
# Begin Source File

SOURCE=.\GetFileTime.cpp
# End Source File
# Begin Source File

SOURCE=.\GetModulePath.cpp
# End Source File
# Begin Source File

SOURCE=.\GetSpecialFolderPath.cpp
# End Source File
# Begin Source File

SOURCE=.\HandleMouseWheelRegisteredMessage.cpp
# End Source File
# Begin Source File

SOURCE=.\NormalizeRect.cpp
# End Source File
# Begin Source File

SOURCE=.\PtInWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\RemoveFilename.cpp
# End Source File
# Begin Source File

SOURCE=.\resstring.cpp
# End Source File
# Begin Source File

SOURCE=.\resstring.h
# End Source File
# Begin Source File

SOURCE=.\ScreenToClientRect.cpp
# End Source File
# Begin Source File

SOURCE=.\SetWindowTransparent.cpp
# End Source File
# Begin Source File

SOURCE=.\strrepc.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\About.cpp
# End Source File
# Begin Source File

SOURCE=.\ConvertBitmapToDIB.cpp
# End Source File
# Begin Source File

SOURCE=.\CreatePhysicalPalette.cpp
# End Source File
# Begin Source File

SOURCE=.\GetScreenLayout.cpp
# End Source File
# Begin Source File

SOURCE=.\Hotkeys.cpp
# End Source File
# Begin Source File

SOURCE=.\MainWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\Marquee.cpp
# End Source File
# Begin Source File

SOURCE=.\Marquee.h
# End Source File
# Begin Source File

SOURCE=.\MarqueeWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\RegistrySettings.cpp
# End Source File
# Begin Source File

SOURCE=.\ShowLastError.cpp
# End Source File
# Begin Source File

SOURCE=.\ShowOptionsDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\ToolTip.cpp
# End Source File
# Begin Source File

SOURCE=.\TrackTip.cpp
# End Source File
# Begin Source File

SOURCE=.\Window.cpp
# End Source File
# Begin Source File

SOURCE=.\WinHelp.cpp
# End Source File
# Begin Source File

SOURCE=.\WriteDIB.cpp
# End Source File
# Begin Source File

SOURCE=.\ZoomPlus.cpp
# End Source File
# Begin Source File

SOURCE=.\ZoomPlus.rc
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\MainWindow.h
# End Source File
# Begin Source File

SOURCE=.\MarqueeWindow.h
# End Source File
# Begin Source File

SOURCE=.\RegistrySettings.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\ToolTip.h
# End Source File
# Begin Source File

SOURCE=.\TrackTip.h
# End Source File
# Begin Source File

SOURCE=.\Utils.h
# End Source File
# Begin Source File

SOURCE=.\Window.h
# End Source File
# Begin Source File

SOURCE=.\ZoomPlus.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\arrow.cur
# End Source File
# Begin Source File

SOURCE=.\res\Arrow.cur
# End Source File
# Begin Source File

SOURCE=.\Res\MainToolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\MarqueeHorizontal.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\MarqueeVertical.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\zoomcurs.cur
# End Source File
# Begin Source File

SOURCE=.\Res\ZoomPlus.ico
# End Source File
# End Group
# Begin Group "Help Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Hlp\MakeHelp.bat
# End Source File
# Begin Source File

SOURCE=.\resource.hm
# End Source File
# Begin Source File

SOURCE=.\ZoomPlus.hm
# End Source File
# Begin Source File

SOURCE=.\Hlp\ZoomPlus.hpj

!IF  "$(CFG)" == "ZoomPlus - Win32 Release"

# PROP Intermediate_Dir "hlp"
# PROP Ignore_Default_Tool 1
USERDEP__ZOOMP="hlp\zoomplus.rtf"	
# Begin Custom Build
InputPath=.\Hlp\ZoomPlus.hpj
InputName=ZoomPlus

"ZoomPlus.hlp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	hlp\MakeHelp $(InputName)

# End Custom Build

!ELSEIF  "$(CFG)" == "ZoomPlus - Win32 Debug"

# PROP Intermediate_Dir "hlp"
# PROP Ignore_Default_Tool 1
USERDEP__ZOOMP="hlp\zoomplus.rtf"	
# Begin Custom Build
InputPath=.\Hlp\ZoomPlus.hpj
InputName=ZoomPlus

"ZoomPlus.hlp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	hlp\MakeHelp $(InputName)

# End Custom Build

!ELSEIF  "$(CFG)" == "ZoomPlus - Win32 UNICODE Debug"

# PROP Intermediate_Dir "hlp"
# PROP Ignore_Default_Tool 1
USERDEP__ZOOMP="hlp\zoomplus.rtf"	
# Begin Custom Build
InputPath=.\Hlp\ZoomPlus.hpj
InputName=ZoomPlus

"ZoomPlus.hlp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	hlp\MakeHelp $(InputName)

# End Custom Build

!ELSEIF  "$(CFG)" == "ZoomPlus - Win32 UNICODE Release"

# PROP Intermediate_Dir "hlp"
# PROP Ignore_Default_Tool 1
USERDEP__ZOOMP="hlp\zoomplus.rtf"	
# Begin Custom Build
InputPath=.\Hlp\ZoomPlus.hpj
InputName=ZoomPlus

"ZoomPlus.hlp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	hlp\MakeHelp $(InputName)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Hlp\ZoomPlus.rtf
# End Source File
# End Group
# Begin Source File

SOURCE=".\Features Not Done.txt"
# End Source File
# Begin Source File

SOURCE=.\History.txt
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
