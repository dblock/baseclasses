/*
  
  © Vestris Inc., Geneva, Switzerland
  http://www.vestris.com, 1994-1999 All Rights Reserved
  ______________________________________________
  
  written by Daniel Doubrovkine - dblock@vestris.com
  
  Revision history:

  17.09.1999: OSF1 does not support usleep (or has it well hidden, didn't look for it, dB.)
  17.09.1999: HPUX, AIX definitions for h_errno

*/


#ifndef BASE_DEFINES_HPP
#define BASE_DEFINES_HPP

#ifndef _REENTRANT
 #define _REENTRANT
#endif

#ifdef _UNIX
/* reentrant definitions */
#ifdef _REENTRANT
 #define base_localtime(TIME, TMTIME) localtime_r(&TIME, &TMTIME)
 #define base_gmtime(TIME, TMTIME) gmtime_r(&TIME, &TMTIME)
#endif

/* gethostname and getpagesize for IRIX */
#if defined(_OS_IRIX) || defined(_OS_IRIX64)
 extern "C" { extern int getpagesize(void); }
 extern "C" { extern int gethostname(char *, size_t); }
#endif

/* errno - h_errno */
#ifndef HAVE_H_ERRNO
 extern int h_errno;
#endif
#define base_errno errno
#define setpgrp ::setsid
#define base_exit ::exit
#define base_chdir(x) ::chdir((const char *) x)
#define base_getpid ::getpid
#define base_getcwd ::getcwd
#define base_mkdir(x) ::mkdir((const char *) x, S_IRWXU)
#define base_seek ::lseek
#define base_open ::open
#define base_close ::close
#define base_unlink ::unlink
#define base_set_new_handler ::set_new_handler
#define base_stat ::stat
#define struct_stat struct stat
#define base_fileno fileno
#define base_fdopen ::fdopen
#define base_fclose ::fclose
#define base_fread ::fread
#define base_fwrite ::fwrite
#define base_fgetc ::fgetc
#define base_popen ::popen
#define base_pclose ::pclose
/* char manip */
#define base_strlen(x) ::strlen((const char *) (x))
#define base_strcspn ::strcspn
#define base_strspn ::strspn
#define base_strstr ::strstr
#define base_strncpy ::strncpy
#define base_strchr ::strchr
#define base_strncmp ::strncmp
#define base_snprintf snprintf
#define base_vsnprintf vsnprintf
/* ftime */
#define base_ftime ::ftime
#define base_timeb timeb
/* environment */
extern char ** environ;
#define _environ environ
/* pthread / pth */
#ifdef HAVE_PTH
 #define base_connect pth_connect
 #define base_accept pth_accept
 #define base_select pth_select
 #define base_poll pth_poll
 #define base_read pth_read
 #define base_write pth_write
 #define base_send(X,Y,Z,A) pth_write(X,Y,Z)
 #define base_recv(X,Y,Z,A) pth_read(X,Y,Z) 
 #define base_sleep_ms(_X) pth_usleep(_X * 1000)
 #define base_sleep pth_sleep
#else
 #define base_connect connect
 #define base_accept accept
 #define base_select select
 #define base_poll poll
 #define base_read read
 #define base_write write
 #define base_send(X,Y,Z,A) ::write(X,Y,Z)
 #define base_recv(X,Y,Z,A) ::read(X,Y,Z)
 #ifdef HAVE_USLEEP
  #define base_sleep_ms(_X) ::usleep(_X * 1000)
 #else
  #define base_sleep_ms(_X) ::sleep(_X / 1000)
 #endif
 #define base_sleep ::sleep
#endif 
#define __cdecl
#endif

#ifdef _WIN32
/* reentrant */
#define base_localtime(TIME, TMTIME) memcpy(&TMTIME, localtime(&TIME), sizeof(TMTIME))
#define base_gmtime(TIME, TMTIME) memcpy(&TMTIME, gmtime(&TIME), sizeof(TMTIME))
/* other */
#define base_exit ::_exit
#define base_send(X,Y,Z,A) ::send(X,Y,Z,A)
#define base_recv(X,Y,Z,A) ::recv(X,Y,Z,A)
#define base_sleep_ms(x) ::Sleep(x)
#define base_sleep(x) ::Sleep(x * 1000)
#define base_getpid ::_getpid
#define base_chdir(x) ::_chdir((const char *) x)
#define base_getcwd ::_getcwd
#define base_mkdir(x) ::_mkdir((const char *) x)
#define base_set_new_handler ::_set_new_handler
#define base_errno errno
/* char manip */
#define base_strlen(x) ::strlen((const char *) (x))
#define base_strcmpi ::_strcmpi
#define base_strncmpi ::_strnicmp
#define base_strcspn ::strcspn
#define base_strspn ::strspn
#define base_strstr ::strstr
#define base_strncpy ::strncpy
#define base_strchr ::strchr
#define base_strncmp ::strncmp
#define base_snprintf _snprintf
#define base_vsnprintf _vsnprintf
/* file-related */
#define off_t long
#define ssize_t size_t
#define mode_t unsigned int
#define base_seek ::_lseek
#define base_read ::_read
#define base_write ::_write
#define base_open ::_open
#define base_close ::_close
#define base_unlink ::_unlink
#define base_ftime ::_ftime
#define base_timeb _timeb
#define base_stat ::_stat
#define struct_stat struct _stat
#define base_fileno _fileno
#define base_fdopen _fdopen
#define base_fclose fclose
#define base_fread fread
#define base_fwrite fwrite
#define base_fgetc fgetc
#define base_popen ::_popen
#define base_pclose ::_pclose
/* handle related */
#define O_APPEND _O_APPEND
#define O_BINARY _O_BINARY
#define O_CREAT _O_CREAT
#define O_SHORT_LIVED _O_SHORT_LIVED
#define O_TEMPORARY _O_TEMPORARY
#define O_EXCL _O_EXCL
#define O_RANDOM _O_RANDOM
#define O_RDONLY _O_RDONLY
#define O_RDWR _O_RDWR
#define O_SEQUENTIAL _O_SEQUENTIAL
#define O_TEXT _O_TEXT
#define O_TRUNC _O_TRUNC
#define O_WRONLY _O_WRONLY
/* socketcomm */
#define base_connect ::connect
#define base_accept ::accept
#define base_select ::select
#define base_poll ::poll
#endif

#ifndef MAXPATHLEN
#define MAXPATHLEN _MAX_PATH
#endif

#ifndef MAXHOSTENT
#define MAXHOSTENT 16384
#endif

#ifndef _WIN32
 #ifndef BYTE
  #define BYTE char
 #endif
#endif

#define KBYTE 1024
#define	MBYTE 1048576L
#define	GBYTE 1073741824L

#define BASE_MAX(X,Y) ((X)>(Y)?(X):(Y))
#define BASE_MIN(X,Y) ((X)<(Y)?(X):(Y))
#define BASE_DIM(X) (int) (sizeof(X) / sizeof(X[0]))

/* URLTree of CSwapString/CString switch */
// #define __URL_TREE_SWAP

/* TLI - Transport Layer Interface instead of sockets */
// #define __TLI_SUPPORTED

#ifndef false
 #ifdef FALSE
  #define false FALSE
 #else
  #define false 0
 #endif
#endif

#ifndef true
 #ifdef TRUE
  #define true TRUE
 #else
  #define true 1
 #endif
#endif

#endif
