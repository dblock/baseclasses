/*
    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1994-1999 All Rights Reserved
    ______________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com

*/

#ifndef BASE_INCLUDE_HPP
#define BASE_INCLUDE_HPP

#include "platform.hpp"
#include "debug.hpp"

#ifdef _UNIX
#include <sys/param.h>
#include <dirent.h>
#if HAVE_UNISTD_H
 #include <sys/types.h>
 #include <unistd.h>
#endif

#if TIME_WITH_SYS_TIME
 #include <sys/time.h>
 #include <time.h>
#else
 #if HAVE_SYS_TIME_H
  #include <sys/time.h>
 #else
  #include <time.h>
 #endif
#endif

#ifdef HAVE_PTH
 #include <pth.h>
#else
 #include <pthread.h>
#endif
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/timeb.h>
#include <sys/utsname.h>
#include <sys/times.h>
#include <limits.h>
#include <setjmp.h>

#ifdef HAVE_STRINGS_H
 #include <strings.h>
#endif

#include <sys/mman.h>
#endif // _UNIX

#ifdef _WIN32
#include <io.h>
#include <conio.h>
#include <direct.h>
#include <windows.h>
#include <winbase.h>
#include <winsock.h>
#include <time.h>
#include <sys/timeb.h>
#include <process.h>
// Windows platform SDK specific
#define SECURITY_WIN32
#include <sspi.h>
#include <NtSecApi.h>
#include <NtSecPkg.h>
#include <Rpc.h>
#endif // _WIN32

// C specific

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <signal.h>
#include <stdarg.h>

// C++ specific
#include <new.h>
#include <iostream.h>
#include <ctype.h>
#include <fstream.h>
#include <assert.h>

// BASE specific
#ifndef bool
#ifdef BOOL
 #define bool BOOL
#else
 #ifdef byte
  #define bool byte
 #else
  #ifdef BYTE
   #define bool BYTE
  #else
   #define bool short int
  #endif
 #endif
#endif
#endif

#include "defines.hpp"

// TLI - Transport Layer Interface
#ifdef  __TLI_SUPPORTED
 #define DEV_TCP "/dev/tcp"
 #include <tiuser.h>
#endif

// needed for domain resolution and/or pthread h_errno
#ifdef HAVE_RESOLV_H
 #ifndef HAVE_MAXDNAME // default domain, needed for resolv.h on Solaris
  #define MAXDNAME MAXPATHLEN
 #endif
 #include <resolv.h>
#endif

#include "trace.hpp"
#include "classtypes.hpp"

#endif
