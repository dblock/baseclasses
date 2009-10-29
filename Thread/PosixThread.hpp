/*

  © Vestris Inc., Geneva, Switzerland
  http://www.vestris.com, 1994-1999 All Rights Reserved
  ______________________________________________

  written by Daniel Doubrovkine - dblock@vestris.com

*/

#ifndef BASE_POSIX_THREAD_HPP
#define BASE_POSIX_THREAD_HPP

#include <platform/include.hpp>
#include <Object/Object.hpp>
#include "Thread.hpp"

#ifdef _UNIX

#ifdef HAVE_PTH
class CThreadInitializer : public CObject {
public:
  CThreadInitializer(void) {
    pth_init();
  }
  ~CThreadInitializer(void) {
    pth_kill();
  }
};
#endif

class CThread : public CObject {
   #ifdef HAVE_PTH
    static CThreadInitializer m_ThreadInitializer;
   #endif
	copy_property(CMutex *, ThreadStartMutex);
   #ifdef HAVE_PTH
    readonly_property(pth_t, ThreadID);
   #else
	readonly_property(pthread_t, ThreadID);
   #endif
	property(CThreadState, ThreadState);
	readonly_copy_property(void *, Arguments);
	readonly_property(CThreadState, InitialState);
public:
	CThread(void * = NULL, CThreadState = ctsRunning);
	virtual ~CThread(void);
	virtual void Launch(void);
	virtual void Terminate(void);
	virtual void Suspend(void);
	virtual void Resume(void);
	virtual void Execute(void *);
	void PassiveWait(unsigned long = 0) const;
private:
	void Run(void);
	static void * CThreadExecute(void * ThreadPointer);
};

#endif
#endif
