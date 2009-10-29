/*
    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1994-1999 All Rights Reserved
    ______________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com

*/


#ifndef BASE_THREAD_HPP
#define BASE_THREAD_HPP

#include <platform/include.hpp>
#include <Object/Object.hpp>
#include <Mutex/Mutex.hpp>

enum CThreadState { ctsStopped, ctsSuspended, ctsRunning, ctsTerminated };

#ifdef _WIN32
unsigned long WINAPI
#endif
#ifdef _UNIX
void *
#endif
CThreadExecute(void * ThreadPointer);

#ifdef _UNIX
#include <Thread/PosixThread.hpp>
#endif

#ifdef _WIN32
#include <Thread/Win32Thread.hpp>
#endif

#endif
