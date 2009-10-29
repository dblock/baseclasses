/*
    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1994-1999 All Rights Reserved
    ______________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com
  
*/

#ifndef BASE_MUTEX_HPP
#define BASE_MUTEX_HPP

#include <platform/include.hpp>
#include <Object/Object.hpp>

class CMutex {
#ifdef _DEBUG
    bool m_bAcquired;
#endif
  #ifdef _UNIX
   #ifdef HAVE_PTH
    property(pth_mutex_t, Mutex);
   #else
    property(pthread_mutex_t, Mutex);
    #ifdef HAVE_POSIX_THREAD_MUTEX_ATTR
     private_property(pthread_mutexattr_t, MutexAttr);
    #endif
   #endif
  #endif
  #ifdef _WIN32
    property(HANDLE, Mutex);
  #endif
public:
    CMutex(void);
    CMutex(const CMutex&);
    CMutex& operator=(const CMutex&);
    
    virtual ~CMutex(void);
    void Lock(void);
    void UnLock(void);
};

#endif
