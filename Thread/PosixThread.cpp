/*
    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1994-1999 All Rights Reserved
    ______________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com

*/

#include <baseclasses.hpp>

#include "PosixThread.hpp"

#ifdef _UNIX

#ifdef HAVE_PTH
 CThreadInitializer CThread::m_ThreadInitializer;
#endif
 
void CThread::Run(void) {
	if (m_ThreadState != ctsRunning) {
		if (m_ThreadState == ctsSuspended) {
			cerr << "CThread::CThread(POSIX) - does not support thread sleeping!" << endl;
		}
		m_ThreadState = ctsRunning;
#ifdef HAVE_PTH
        pth_attr_t l_ThreadAttributes = pth_attr_new();
        pth_attr_set(l_ThreadAttributes, PTH_ATTR_NAME, "thread");
        pth_attr_set(l_ThreadAttributes, PTH_ATTR_STACK_SIZE, 64*1024);
        pth_attr_set(l_ThreadAttributes, PTH_ATTR_JOINABLE, FALSE);
        m_ThreadID = pth_spawn(l_ThreadAttributes, &CThreadExecute, (void *) this);     
#else
 #ifndef _THREAD_DCE
		pthread_attr_t l_ThreadAttributes;
		pthread_attr_init(&l_ThreadAttributes);
		// pthread_attr_setscope(&l_ThreadAttributes, PTHREAD_SCOPE_SYSTEM);
		pthread_attr_setdetachstate(&l_ThreadAttributes, PTHREAD_CREATE_DETACHED);
		pthread_create(&m_ThreadID, &l_ThreadAttributes, &CThreadExecute, this);
		pthread_attr_destroy(&l_ThreadAttributes);
 #else
		pthread_create(&m_ThreadID, NULL, &CThreadExecute, this);
		::pthread_detach(&m_ThreadID);
 #endif
#endif
		m_ThreadState = ctsRunning;
		if (m_ThreadStartMutex) m_ThreadStartMutex->UnLock();
	} else cerr << "CThread::CThread(POSIX) - call of Run() in an invalid state!" << endl;
}


void CThread::Terminate(void) {
	if (m_ThreadState != ctsStopped) {
		m_ThreadState = ctsTerminated;
       #ifdef HAVE_PTH
        pth_cancel(m_ThreadID);
       #endif
       #ifdef HAVE_POSIX_THREAD_CANCEL
		pthread_cancel(m_ThreadID);
       #endif
	} else {
        cerr << "CThread::CThread(POSIX) - call of Terminate() in an invalid state!" << endl;
    }
}

void CThread::Suspend(void) {
	/*
	  this can be done from the calling thread only,
	  I don't know how to force this from outside the calling thread
	*/
	/*
	  sigset_t ThreadMask;
	  sigemptyset(&ThreadMask);
	  sigaddset(&ThreadMask, _SIG_THREAD_SUSPEND);
	  pthread_sigmask(SIG_BLOCK, &ThreadBask, NULL);
	*/
}

void CThread::Resume(void) {
	/*
	  this can be done from the calling thread only,
	  I don't know how to force this from outside the calling thread
	*/
	/*
	  sigset_t ThreadMask;
	  sigemptyset(&ThreadMask);
	  sigaddset(&ThreadMask, _SIG_THREAD_SUSPEND);
	  pthread_sigmask(SIG_UNBLOCK, &ThreadBask, NULL);
	*/
}

void CThread::Execute(void *) {
	m_ThreadState = ctsTerminated;
}

void CThread::PassiveWait(long unsigned int Milliseconds) const {
	if (Milliseconds && (m_ThreadState == ctsRunning)) {
		base_sleep_ms(Milliseconds);
	} else {
		//pthread_join(m_ThreadID, NULL); // cannot pthread_join a detached thread
		if (!Milliseconds) Milliseconds = 1000;
		if (Milliseconds > 1000) while (m_ThreadState == ctsRunning) base_sleep(Milliseconds / 1000);
		else while (m_ThreadState == ctsRunning) base_sleep_ms(Milliseconds);
	}
}

#endif
