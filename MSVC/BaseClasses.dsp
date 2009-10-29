# Microsoft Developer Studio Project File - Name="BaseClasses" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=BaseClasses - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "BaseClasses.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "BaseClasses.mak" CFG="BaseClasses - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "BaseClasses - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "BaseClasses - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "BaseClasses - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Yu"baseclasses.hpp" /FD /c
# ADD CPP /nologo /MT /W3 /O2 /I "../../Rockall" /I "../" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_CONSOLE" /D "_LIB" /Yu"baseclasses.hpp" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "BaseClasses - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MT /W3 /Gm /ZI /I "../../Rockall" /I "../" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_CONSOLE" /Yu"baseclasses.hpp" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "BaseClasses - Win32 Release"
# Name "BaseClasses - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\Mutex\Atomic.cpp
# End Source File
# Begin Source File

SOURCE=..\Security\Authentication.cpp
# End Source File
# Begin Source File

SOURCE=..\platform\baseclasses.cpp
# ADD CPP /Yc"baseclasses.hpp"
# End Source File
# Begin Source File

SOURCE=..\Security\BasicAuth.cpp
# End Source File
# Begin Source File

SOURCE=..\BitSet\BitSet.cpp
# End Source File
# Begin Source File

SOURCE=..\Server\Connection.cpp
# End Source File
# Begin Source File

SOURCE=..\Internet\Cookie.cpp
# End Source File
# Begin Source File

SOURCE=..\Internet\CookieStorage.cpp
# End Source File
# Begin Source File

SOURCE=..\Date\Date.cpp
# End Source File
# Begin Source File

SOURCE=..\Socket\Dns.cpp
# End Source File
# Begin Source File

SOURCE=..\Internet\EmailAddress.cpp
# End Source File
# Begin Source File

SOURCE=..\Email\EmailEx.cpp
# End Source File
# Begin Source File

SOURCE=..\Encryption\Encryption.cpp
# End Source File
# Begin Source File

SOURCE=..\String\EStrings.cpp
# End Source File
# Begin Source File

SOURCE=..\Virtual\FileMapping.cpp
# End Source File
# Begin Source File

SOURCE=..\String\GStrings.cpp
# End Source File
# Begin Source File

SOURCE=..\Object\Handler.cpp
# End Source File
# Begin Source File

SOURCE=..\HashTable\HashTable.cpp
# End Source File
# Begin Source File

SOURCE=..\Internet\HtmlParser.cpp
# End Source File
# Begin Source File

SOURCE=..\Internet\HtmlTag.cpp
# End Source File
# Begin Source File

SOURCE=..\Io\HttpIo.cpp
# End Source File
# Begin Source File

SOURCE=..\Internet\HttpRequest.cpp
# End Source File
# Begin Source File

SOURCE=..\Date\Interval.cpp
# End Source File
# Begin Source File

SOURCE=..\Vector\IntVecIter.cpp
# End Source File
# Begin Source File

SOURCE=..\Vector\IntVector.cpp
# End Source File
# Begin Source File

SOURCE=..\Io\Io.cpp
# End Source File
# Begin Source File

SOURCE=..\List\Iterator.cpp
# End Source File
# Begin Source File

SOURCE=..\List\List.cpp
# End Source File
# Begin Source File

SOURCE=..\File\LocalFile.cpp
# End Source File
# Begin Source File

SOURCE=..\File\LocalPath.cpp
# End Source File
# Begin Source File

SOURCE=..\FileSystem\LocalSystem.cpp
# End Source File
# Begin Source File

SOURCE=..\Encryption\Md5.cpp
# End Source File
# Begin Source File

SOURCE=..\Tree\Md5Tree.cpp
# End Source File
# Begin Source File

SOURCE=..\Email\mimebody.cpp
# End Source File
# Begin Source File

SOURCE=..\Email\mimeheader.cpp
# End Source File
# Begin Source File

SOURCE=..\Email\mimemessage.cpp
# End Source File
# Begin Source File

SOURCE=..\Internet\MimeParser.cpp
# End Source File
# Begin Source File

SOURCE=..\File\MMapFile.cpp
# End Source File
# Begin Source File

SOURCE=..\Mutex\Mutex.cpp
# End Source File
# Begin Source File

SOURCE=..\Security\NtlmAuth.cpp
# End Source File
# Begin Source File

SOURCE=..\Server\NTService.cpp
# End Source File
# Begin Source File

SOURCE=..\Object\Object.cpp
# End Source File
# Begin Source File

SOURCE=..\Server\PingClient.cpp
# End Source File
# Begin Source File

SOURCE=..\Email\PmdfEmail.cpp
# End Source File
# Begin Source File

SOURCE=..\Thread\PosixThread.cpp
# End Source File
# Begin Source File

SOURCE=..\File\Progress.cpp
# End Source File
# Begin Source File

SOURCE=..\RegExp\RegExp.cpp
# End Source File
# Begin Source File

SOURCE=..\File\RemoteFile.cpp
# End Source File
# Begin Source File

SOURCE=..\Mutex\RWMutex.cpp
# End Source File
# Begin Source File

SOURCE=..\Mutex\RWNamedTable.cpp
# End Source File
# Begin Source File

SOURCE=..\Server\Server.cpp
# End Source File
# Begin Source File

SOURCE=..\Socket\Socket.cpp
# End Source File
# Begin Source File

SOURCE=..\Security\SspiAuth.cpp
# End Source File
# Begin Source File

SOURCE=..\Security\State.cpp
# End Source File
# Begin Source File

SOURCE=..\String\String.cpp
# End Source File
# Begin Source File

SOURCE=..\String\StringPair.cpp
# End Source File
# Begin Source File

SOURCE=..\String\StringTable.cpp
# End Source File
# Begin Source File

SOURCE=..\Vector\SVector.cpp
# End Source File
# Begin Source File

SOURCE=..\Thread\Thread.cpp
# End Source File
# Begin Source File

SOURCE=..\Thread\ThreadPool.cpp
# End Source File
# Begin Source File

SOURCE=..\Thread\ThreadPoolJob.cpp
# End Source File
# Begin Source File

SOURCE=..\Object\Tracer.cpp
# End Source File
# Begin Source File

SOURCE=..\Tree\Tree.cpp
# End Source File
# Begin Source File

SOURCE=..\String\TSStringTable.cpp
# End Source File
# Begin Source File

SOURCE=..\Internet\Url.cpp
# End Source File
# Begin Source File

SOURCE=..\Tree\UrlTree.cpp
# End Source File
# Begin Source File

SOURCE=..\Vector\Vector.cpp
# End Source File
# Begin Source File

SOURCE=..\HashTable\VectorTable.cpp
# End Source File
# Begin Source File

SOURCE=..\Virtual\VirtualMemory.cpp
# End Source File
# Begin Source File

SOURCE=..\Server\WebServer.cpp
# End Source File
# Begin Source File

SOURCE=..\Security\Win32SecInterface.cpp
# End Source File
# Begin Source File

SOURCE=..\Thread\Win32Thread.cpp
# End Source File
# Begin Source File

SOURCE=..\Tree\XmlTree.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\Mutex\Atomic.hpp
# End Source File
# Begin Source File

SOURCE=..\Security\Authentication.hpp
# End Source File
# Begin Source File

SOURCE=..\platform\baseclasses.hpp
# End Source File
# Begin Source File

SOURCE=..\Security\BasicAuth.hpp
# End Source File
# Begin Source File

SOURCE=..\BitSet\BitSet.hpp
# End Source File
# Begin Source File

SOURCE=..\platform\classtypes.hpp
# End Source File
# Begin Source File

SOURCE=..\Server\Connection.hpp
# End Source File
# Begin Source File

SOURCE=..\Internet\Cookie.hpp
# End Source File
# Begin Source File

SOURCE=..\Internet\CookieStorage.hpp
# End Source File
# Begin Source File

SOURCE=..\Date\Date.hpp
# End Source File
# Begin Source File

SOURCE=..\platform\debug.hpp
# End Source File
# Begin Source File

SOURCE=..\platform\defines.hpp
# End Source File
# Begin Source File

SOURCE=..\Socket\Dns.hpp
# End Source File
# Begin Source File

SOURCE=..\Internet\EmailAddress.hpp
# End Source File
# Begin Source File

SOURCE=..\Email\EmailEx.hpp
# End Source File
# Begin Source File

SOURCE=..\Encryption\Encryption.hpp
# End Source File
# Begin Source File

SOURCE=..\String\EStrings.hpp
# End Source File
# Begin Source File

SOURCE=..\Virtual\FileMapping.hpp
# End Source File
# Begin Source File

SOURCE=..\String\GStrings.hpp
# End Source File
# Begin Source File

SOURCE=..\Object\Handler.hpp
# End Source File
# Begin Source File

SOURCE=..\HashTable\HashTable.hpp
# End Source File
# Begin Source File

SOURCE=..\Internet\HtmlParser.hpp
# End Source File
# Begin Source File

SOURCE=..\Internet\HtmlTag.hpp
# End Source File
# Begin Source File

SOURCE=..\Io\HttpIo.hpp
# End Source File
# Begin Source File

SOURCE=..\Internet\HttpRequest.hpp
# End Source File
# Begin Source File

SOURCE=..\platform\include.hpp
# End Source File
# Begin Source File

SOURCE=..\Date\Interval.hpp
# End Source File
# Begin Source File

SOURCE=..\Vector\IntVecIter.hpp
# End Source File
# Begin Source File

SOURCE=..\Vector\IntVector.hpp
# End Source File
# Begin Source File

SOURCE=..\Io\Io.hpp
# End Source File
# Begin Source File

SOURCE=..\List\Iterator.hpp
# End Source File
# Begin Source File

SOURCE=..\List\List.hpp
# End Source File
# Begin Source File

SOURCE=..\File\LocalFile.hpp
# End Source File
# Begin Source File

SOURCE=..\File\LocalPath.hpp
# End Source File
# Begin Source File

SOURCE=..\FileSystem\LocalSystem.hpp
# End Source File
# Begin Source File

SOURCE=..\Encryption\Md5.hpp
# End Source File
# Begin Source File

SOURCE=..\Tree\Md5Tree.hpp
# End Source File
# Begin Source File

SOURCE=..\platform\messages.hpp
# End Source File
# Begin Source File

SOURCE=..\Email\mimebody.hpp
# End Source File
# Begin Source File

SOURCE=..\Email\mimeheader.hpp
# End Source File
# Begin Source File

SOURCE=..\Email\mimemessage.hpp
# End Source File
# Begin Source File

SOURCE=..\Internet\MimeParser.hpp
# End Source File
# Begin Source File

SOURCE=..\File\MMapFile.hpp
# End Source File
# Begin Source File

SOURCE=..\Mutex\Mutex.hpp
# End Source File
# Begin Source File

SOURCE=..\Security\NtlmAuth.hpp
# End Source File
# Begin Source File

SOURCE=..\Server\NTService.hpp
# End Source File
# Begin Source File

SOURCE=..\Object\Object.hpp
# End Source File
# Begin Source File

SOURCE=..\Server\PingClient.hpp
# End Source File
# Begin Source File

SOURCE=..\platform\platform.hpp
# End Source File
# Begin Source File

SOURCE=..\Email\PmdfEmail.hpp
# End Source File
# Begin Source File

SOURCE=..\Thread\PosixThread.hpp
# End Source File
# Begin Source File

SOURCE=..\File\Progress.hpp
# End Source File
# Begin Source File

SOURCE=..\RegExp\RegExp.hpp
# End Source File
# Begin Source File

SOURCE=..\File\RemoteFile.hpp
# End Source File
# Begin Source File

SOURCE=..\Mutex\RWMutex.hpp
# End Source File
# Begin Source File

SOURCE=..\Mutex\RWNamedTable.hpp
# End Source File
# Begin Source File

SOURCE=..\Server\Server.hpp
# End Source File
# Begin Source File

SOURCE=..\Socket\Socket.hpp
# End Source File
# Begin Source File

SOURCE=..\Socket\SockTypes.hpp
# End Source File
# Begin Source File

SOURCE=..\Security\SspiAuth.hpp
# End Source File
# Begin Source File

SOURCE=..\Security\State.hpp
# End Source File
# Begin Source File

SOURCE=..\String\String.hpp
# End Source File
# Begin Source File

SOURCE=..\String\StringPair.hpp
# End Source File
# Begin Source File

SOURCE=..\String\StringTable.hpp
# End Source File
# Begin Source File

SOURCE=..\Vector\SVector.hpp
# End Source File
# Begin Source File

SOURCE=..\Thread\Thread.hpp
# End Source File
# Begin Source File

SOURCE=..\Thread\ThreadPool.hpp
# End Source File
# Begin Source File

SOURCE=..\Thread\ThreadPoolJob.hpp
# End Source File
# Begin Source File

SOURCE=..\platform\tlist.hpp
# End Source File
# Begin Source File

SOURCE=..\platform\tmore.hpp
# End Source File
# Begin Source File

SOURCE=..\platform\trace.hpp
# End Source File
# Begin Source File

SOURCE=..\Object\Tracer.hpp
# End Source File
# Begin Source File

SOURCE=..\Tree\Tree.hpp
# End Source File
# Begin Source File

SOURCE=..\String\TSStringTable.hpp
# End Source File
# Begin Source File

SOURCE=..\platform\tvector.hpp
# End Source File
# Begin Source File

SOURCE=..\Internet\Url.hpp
# End Source File
# Begin Source File

SOURCE=..\Tree\UrlTree.hpp
# End Source File
# Begin Source File

SOURCE=..\Vector\Vector.hpp
# End Source File
# Begin Source File

SOURCE=..\HashTable\VectorTable.hpp
# End Source File
# Begin Source File

SOURCE=..\Virtual\VirtualMemory.hpp
# End Source File
# Begin Source File

SOURCE=..\Server\WebServer.hpp
# End Source File
# Begin Source File

SOURCE=..\Security\Win32SecInterface.hpp
# End Source File
# Begin Source File

SOURCE=..\Thread\Win32Thread.hpp
# End Source File
# Begin Source File

SOURCE=..\Tree\XmlTree.hpp
# End Source File
# End Group
# End Target
# End Project
