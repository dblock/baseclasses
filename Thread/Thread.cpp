/*
    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1994-1999 All Rights Reserved
    ______________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com

*/

#include <baseclasses.hpp>

#include "Thread.hpp"

#ifdef _WIN32
unsigned long WINAPI
#endif
#ifdef _UNIX
void *
#endif
CThread::CThreadExecute(void * ThreadPointer) {
    ((CThread *) ThreadPointer)->Execute(((CThread *) ThreadPointer)->GetArguments());
#ifdef _WIN32
	long UserTime, KernelTime; if (((CThread *) ThreadPointer)->GetTimes(KernelTime, UserTime)) { m_UserTime.Inc(UserTime); m_KernelTime.Inc(KernelTime);	}
#endif
	delete (CThread *) ThreadPointer;
#ifdef _WIN32
	ExitThread(ctsTerminated);
#endif
#ifdef _UNIX
 #ifdef HAVE_PTH
        pth_exit(NULL);
 #else
	pthread_exit(NULL);
 #endif
#endif
	return
#ifdef _UNIX
	    (void *)
#endif
	    0;
}

CThread::CThread(void * Arguments, CThreadState InitialState) {
	m_ThreadStartMutex = NULL;
	m_ThreadState = ctsStopped;
	m_Arguments = Arguments;
	m_InitialState = InitialState;
}

void CThread::Launch(void) {
	switch(m_InitialState) {
	case ctsRunning:
		Run();
		break;
	case ctsSuspended:
		m_ThreadState = ctsSuspended;
		Run();
		break;
	default:
		cerr << "CThread::CThread(WIN32) - creation of thread in an invalid state!" << endl;
		break;
	}
}

CThread::~CThread(void) {
#ifdef _WIN32
	CloseHandle(m_ThreadHandle);
#endif
	if (m_ThreadState != ctsTerminated) {
		cerr << "CThread::~CThread(BOTH) - destructor violates thread termination!" << endl;
		while (m_ThreadState != ctsTerminated)
            base_sleep(0);
	}
}
