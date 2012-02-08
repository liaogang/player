# Microsoft Developer Studio Project File - Name="playerT" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=playerT - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "playerT.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "playerT.mak" CFG="playerT - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "playerT - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "playerT - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "playerT - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "STRICT" /D "_ATL_MIN_CRT" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "playerT - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "STRICT" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "playerT - Win32 Release"
# Name "playerT - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\player\AboutDlg.h
# End Source File
# Begin Source File

SOURCE=..\..\player\CMyView.h
# End Source File
# Begin Source File

SOURCE=..\..\player\CPropertyDlgs.h
# End Source File
# Begin Source File

SOURCE=..\..\player\DialogConfig.cpp
# End Source File
# Begin Source File

SOURCE=..\..\player\DialogConfig.h
# End Source File
# Begin Source File

SOURCE=..\..\player\DsoundControl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\player\DsoundControl.h
# End Source File
# Begin Source File

SOURCE=..\..\player\FFT.h
# End Source File
# Begin Source File

SOURCE=..\..\player\LrcMng.cpp
# End Source File
# Begin Source File

SOURCE=..\..\player\LrcMng.h
# End Source File
# Begin Source File

SOURCE=..\..\player\mainfrm.CPP
# End Source File
# Begin Source File

SOURCE=..\..\player\MainFrm.h
# End Source File
# Begin Source File

SOURCE=..\..\player\Mp3File.h
# End Source File
# Begin Source File

SOURCE=..\..\player\mpg123.h
# End Source File
# Begin Source File

SOURCE=..\..\player\music.cpp
# End Source File
# Begin Source File

SOURCE=..\..\player\music.h
# End Source File
# Begin Source File

SOURCE=..\..\player\MusicFile.h
# End Source File
# Begin Source File

SOURCE=..\..\player\MusicLibrary.cpp
# End Source File
# Begin Source File

SOURCE=..\..\player\player.cpp
# End Source File
# Begin Source File

SOURCE=..\..\player\player.h
# End Source File
# Begin Source File

SOURCE=..\..\player\player.rc
# End Source File
# Begin Source File

SOURCE=..\..\player\PlayList.cpp
# End Source File
# Begin Source File

SOURCE=..\..\player\PlayList.h
# End Source File
# Begin Source File

SOURCE=..\..\player\ProgertyDlg.h
# End Source File
# Begin Source File

SOURCE=..\..\player\resource.h
# End Source File
# Begin Source File

SOURCE=..\..\player\stdafx.cpp
# End Source File
# Begin Source File

SOURCE=..\..\player\stdafx.h
# End Source File
# Begin Source File

SOURCE=..\..\player\WaveFile.cpp
# End Source File
# Begin Source File

SOURCE=..\..\player\WaveFile.h
# End Source File
# Begin Source File

SOURCE=..\..\player\WaveFileEx.h
# End Source File
# End Group
# End Target
# End Project
