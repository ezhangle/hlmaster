# Microsoft Developer Studio Project File - Name="pfc++" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=pfc++ - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "pfc++.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "pfc++.mak" CFG="pfc++ - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "pfc++ - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "pfc++ - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "pfc++ - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I ".\include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x807 /d "NDEBUG"
# ADD RSC /l 0x807 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "pfc++ - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I ".\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x807 /d "_DEBUG"
# ADD RSC /l 0x807 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "pfc++ - Win32 Release"
# Name "pfc++ - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\src\CInetAddress.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CStringBuffer.cpp
# End Source File
# Begin Source File

SOURCE=.\src\PArgumentList.cpp
# End Source File
# Begin Source File

SOURCE=.\src\PLogfile.cpp
# End Source File
# Begin Source File

SOURCE=.\src\PSocket.cpp
# End Source File
# Begin Source File

SOURCE=.\src\PString.cpp
# End Source File
# Begin Source File

SOURCE=.\src\PStringList.cpp
# End Source File
# Begin Source File

SOURCE=.\src\PUDPSocket.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "pfc++"

# PROP Default_Filter ""
# Begin Source File

SOURCE=".\include\pfc++\file.h"
# End Source File
# Begin Source File

SOURCE=".\include\pfc++\inet.h"
# End Source File
# Begin Source File

SOURCE=".\include\pfc++\socket.h"
# End Source File
# Begin Source File

SOURCE=".\include\pfc++\string.h"
# End Source File
# Begin Source File

SOURCE=".\include\pfc++\system.h"
# End Source File
# Begin Source File

SOURCE=".\include\pfc++\types.h"
# End Source File
# End Group
# Begin Source File

SOURCE=.\include\CInetAddress.h
# End Source File
# Begin Source File

SOURCE=.\include\CStringBuffer.h
# End Source File
# Begin Source File

SOURCE=.\include\PArgumentList.h
# End Source File
# Begin Source File

SOURCE=.\include\PException.h
# End Source File
# Begin Source File

SOURCE=.\include\PFile.h
# End Source File
# Begin Source File

SOURCE=.\include\PLogfile.h
# End Source File
# Begin Source File

SOURCE=.\include\PSocket.h
# End Source File
# Begin Source File

SOURCE=.\include\PString.h
# End Source File
# Begin Source File

SOURCE=.\include\PStringList.h
# End Source File
# Begin Source File

SOURCE=.\include\PUDPSocket.h
# End Source File
# End Group
# End Target
# End Project
