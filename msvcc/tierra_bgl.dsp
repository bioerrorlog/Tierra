# Microsoft Developer Studio Project File - Name="tierra_bgl" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=tierra_bgl - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "tierra_bgl.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "tierra_bgl.mak" CFG="tierra_bgl - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "tierra_bgl - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "tierra_bgl - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "tierra_bgl - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\tierra"
# PROP Intermediate_Dir "tierra_bgl___Win32_Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\Bglserv" /I "..\Bglcom" /I "..\tierra" /I "..\Bglclnt" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "TIERRA" /D FRONTEND=0 /D "BGL" /D "BGL_SERV" /D "DSKGENBNKRD" /D PLOIDY=1 /D "HAVE_TDT_FST_LST_OFSTTYP" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 ws2_32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "tierra_bgl - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\tierra"
# PROP Intermediate_Dir "tierra_bgl___Win32_Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\Bglserv" /I "..\Bglcom" /I "..\tierra" /I "..\Bglclnt" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "TIERRA" /D FRONTEND=0 /D "BGL" /D "BGL_SERV" /D "DSKGENBNKRD" /D PLOIDY=1 /D "HAVE_TDT_FST_LST_OFSTTYP" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ws2_32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "tierra_bgl - Win32 Release"
# Name "tierra_bgl - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\Bglcom\bgl_dat_xdr.c
# End Source File
# Begin Source File

SOURCE=..\Bglcom\Bglcode.c
# End Source File
# Begin Source File

SOURCE=..\Bglcom\Bglnet.c
# End Source File
# Begin Source File

SOURCE=..\Bglcom\Bglprint.c
# End Source File
# Begin Source File

SOURCE=..\Bglcom\Bgltcp.c
# End Source File
# Begin Source File

SOURCE=..\Tierra\Bookeep.c
# End Source File
# Begin Source File

SOURCE=..\Tierra\Decode.c
# End Source File
# Begin Source File

SOURCE=..\Tierra\Diskbank.c
# End Source File
# Begin Source File

SOURCE=..\Tierra\Frontend.c
# End Source File
# Begin Source File

SOURCE=..\tierra\genebank_xdr.c
# End Source File
# Begin Source File

SOURCE=..\Tierra\Instruct.c
# End Source File
# Begin Source File

SOURCE=..\Tierra\Memalloc.c
# End Source File
# Begin Source File

SOURCE=..\Tierra\Memtree.c
# End Source File
# Begin Source File

SOURCE=..\Tierra\Micromon.c
# End Source File
# Begin Source File

SOURCE=..\Tierra\Operator.c
# End Source File
# Begin Source File

SOURCE=..\Tierra\Port_xdr.c
# End Source File
# Begin Source File

SOURCE=..\Tierra\Rambank.c
# End Source File
# Begin Source File

SOURCE=..\Tierra\Slicers.c
# End Source File
# Begin Source File

SOURCE=..\Bglserv\Tbgl_com.c
# End Source File
# Begin Source File

SOURCE=..\Bglserv\Tbgl_dat.c
# End Source File
# Begin Source File

SOURCE=..\Bglserv\tbgl_proc.c
# End Source File
# Begin Source File

SOURCE=..\Tierra\Tie_inc.c
# End Source File
# Begin Source File

SOURCE=..\tierra\tieaudsrv.c
# End Source File
# Begin Source File

SOURCE=..\Tierra\Tierra.c
# End Source File
# Begin Source File

SOURCE=..\tierra\tiexdrcom_xdr.c
# End Source File
# Begin Source File

SOURCE=..\Tierra\Trand.c
# End Source File
# Begin Source File

SOURCE=..\Tierra\Tsetup.c
# End Source File
# Begin Source File

SOURCE=..\Tierra\Ttools.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
