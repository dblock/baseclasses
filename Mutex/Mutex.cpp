/*
    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1994-1999 All Rights Reserved
    ______________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com

*/

#include <baseclasses.hpp>

#include "Mutex.hpp"
#include <Object/Tracer.hpp>

CMutex::CMutex(void) {
    Trace(tagLocks, levVerbose, ("CMutex - ctor (%#x)", (long) this));
#ifdef _DEBUG    
    m_bAcquired = false;
#endif
#ifdef _UNIX
 #ifdef HAVE_PTH
        pth_mutex_init(&m_Mutex);
 #else
      #ifdef HAVE_POSIX_THREAD_MUTEX_ATTR
        pthread_mutexattr_init(&m_MutexAttr);
       #ifdef HAVE_POSIX_THREAD_MUTEX_ATTR_SETTYPE
        pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
       #endif
       #ifdef HAVE_POSIX_THREAD_MUTEX_ATTR_SETKIND_NP
        pthread_mutexattr_setkind_np(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
       #endif	
        pthread_mutex_init(&m_Mutex, &m_MutexAttr);
      #else
        pthread_mutex_init(&m_Mutex, NULL);
      #endif
 #endif
#endif
#ifdef _WIN32
	m_Mutex = CreateMutex(NULL, FALSE, NULL);
    if (m_Mutex == NULL) {
		cerr << "Cannot create new mutex!" << endl;
		// assert(false);
	}
#endif
}

CMutex::~CMutex(void) {
    Trace(tagLocks, levVerbose, ("CMutex - dtor (%#x)", (long) this));
#ifdef _DEBUG
    assert(! m_bAcquired);    
#endif
#ifdef _UNIX
 #ifdef HAVE_PTH
        pth_mutex_release(&m_Mutex);
 #else
        pthread_mutex_unlock(&m_Mutex);    
	pthread_mutex_destroy(&m_Mutex);
       #ifdef HAVE_POSIX_THREAD_MUTEX_ATTR
        pthread_mutexattr_destroy(&m_MutexAttr);
       #endif
 #endif
#endif
#ifdef _WIN32
	if (!CloseHandle(m_Mutex)) {
		cerr << "Error while closing mutex object!" << endl;
		// assert(false);
	}
#endif
}

CMutex::CMutex(const CMutex&) {
	// mutex copy is not allowed
	assert(0);
}

CMutex& CMutex::operator=(const CMutex&){
	// mutex copy is not allowed
	assert(0);
	return * this;
}

void CMutex::Lock(void) {
    Trace(tagLocks, levVerbose, ("CMutex - lock (%#x)", (long) this));
#ifdef _UNIX
       #ifdef HAVE_PTH
               pth_mutex_acquire(&m_Mutex, FALSE, NULL);
       #else
       switch (pthread_mutex_lock(&m_Mutex)) {
	case 0 : break;
	case EINVAL :
		cerr << "Mutex lock error : mutex not initialised correctly !" << endl;
        // assert(false);
		break;
	case EDEADLK:
                cerr << "libpthread :: Mutex lock error, deadlock." << endl;
                // assert(false);
		break;
	default:
		cerr << "Error while waiting for mutex lock !" << endl;
		// assert(false);
	}
       #endif
#endif
#ifdef _WIN32
	if (WaitForSingleObject(m_Mutex, INFINITE) == WAIT_FAILED) {
		cerr << "Error while waiting for mutex lock !" << endl;
#ifdef _DEBUG
        CHandler::ShowLastError();
		assert(false);
#endif
	}
#endif
#ifdef _DEBUG
    assert(! m_bAcquired);
    m_bAcquired = true;
#endif
    Trace(tagLocks, levVerbose, ("CMutex - acquired (%#x)", (long) this));
}

void CMutex::UnLock(void) {
    Trace(tagLocks, levVerbose, ("CMutex - unlock (%#x)", (long) this));
#ifdef _DEBUG
    assert(m_bAcquired);
    m_bAcquired = false;
#endif
#ifdef _UNIX
 #ifdef HAVE_PTH
        pth_mutex_release(&m_Mutex);
 #else
	if (pthread_mutex_unlock(&m_Mutex)) {
		cerr << "Error while unlocking mutex !" << endl;
		// assert(false);
	}
 #endif
#endif
#ifdef _WIN32
	if (!ReleaseMutex(m_Mutex)) {
		cerr << "Error while releasing mutex lock !" << endl;
#ifdef _DEBUG
        CHandler::ShowLastError();
		assert(false);
#endif
	}
#endif	
}
