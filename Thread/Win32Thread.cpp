/*
    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1994-1999 All Rights Reserved
    ______________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com

*/

#include <baseclasses.hpp>

#include "Win32Thread.hpp"

#ifdef _WIN32

CAtomic CThread::m_UserTime;
CAtomic CThread::m_KernelTime;

void CThread::Run(void) {
	if (m_ThreadState != ctsRunning) {
		if (m_ThreadState == ctsSuspended)
			m_ThreadHandle = CreateThread(NULL, 0, CThreadExecute, this, CREATE_SUSPENDED, &m_ThreadID);
		else m_ThreadHandle = CreateThread(NULL, 0, CThreadExecute, this, 0, &m_ThreadID);
		m_ThreadState = ctsRunning;
		if (m_ThreadStartMutex) m_ThreadStartMutex->UnLock();
	} else cerr << "CThread::CThread(WIN32) - call of Run() in an invalid state!" << endl;
}

void CThread::Terminate(void) {
	if (m_ThreadState != ctsStopped) {
		m_ThreadState = ctsTerminated;
		TerminateThread(m_ThreadHandle, ctsTerminated);
	} else cerr << "CThread::CThread(WIN32) - call of Terminate() in an invalid state!" << endl;
}

void CThread::Suspend(void) {
	if (m_ThreadState == ctsRunning) {
		SuspendThread(m_ThreadHandle);
		m_ThreadState = ctsSuspended;
	} else cerr << "CThread::CThread(WIN32) - call of Suspend() in an invalid state!" << endl;
}

void CThread::Resume(void) {
	if (m_ThreadState == ctsSuspended) {
		ResumeThread(m_ThreadHandle);
		m_ThreadState = ctsRunning;
	} else if (m_ThreadState == ctsStopped) {
		CreateThread(NULL, 0, CThreadExecute, this, 0, &m_ThreadID);
		m_ThreadState = ctsRunning;
	} else cerr << "CThread::CThread(WIN32) - call of Resume() in an invalid state!" << endl;
}

void CThread::Execute(void *) {
	m_ThreadState = ctsTerminated;
}

bool CThread::SetPriority(int PriorityLevel) {
	if (m_ThreadState != ctsTerminated) {
		return (bool) ::SetThreadPriority(m_ThreadHandle, PriorityLevel);
	} else return false;
}

bool CThread::SetPriorityBoost(bool Boost) {
	if (m_ThreadState != ctsTerminated)
		return (bool) ::SetThreadPriorityBoost(m_ThreadHandle, Boost);
	else return false;
}

bool CThread::SetPriorityClass(unsigned long PriorityClass) {
	if (m_ThreadState != ctsTerminated)
		return (bool) ::SetPriorityClass(m_ThreadHandle, PriorityClass);
	else return false;
}

long CThread::GetPriorityClass(void) const {
	if (m_ThreadState != ctsTerminated)
		return ::GetPriorityClass(m_ThreadHandle);
	else return -1;
}

long CThread::GetPriority(void) const {
	if (m_ThreadState != ctsTerminated)
		return ::GetThreadPriority(m_ThreadHandle);
	else return -1;
}

void CThread::PassiveWait(unsigned long Milliseconds) const {
	WaitForSingleObject(m_ThreadHandle, Milliseconds);
}

bool CThread::SetThreadOption(const CString& Option) {
	if (Option.Same("THREAD_NOBOOST")) SetPriorityBoost(FALSE);
	else if (Option.Same("THREAD_BOOST")) SetPriorityBoost(TRUE);
	else if (Option.Same("THREAD_ABOVE_NORMAL")) SetPriority(THREAD_PRIORITY_ABOVE_NORMAL);
	else if (Option.Same("THREAD_BELOW_NORMAL")) SetPriority(THREAD_PRIORITY_BELOW_NORMAL);
	else if (Option.Same("THREAD_HIGHEST")) SetPriority(THREAD_PRIORITY_HIGHEST);
	else if (Option.Same("THREAD_IDLE")) SetPriority(THREAD_PRIORITY_IDLE);
	else if (Option.Same("THREAD_LOWEST")) SetPriority(THREAD_PRIORITY_LOWEST);
	else if (Option.Same("THREAD_NORMAL")) SetPriority(THREAD_PRIORITY_NORMAL);
	else if (Option.Same("THREAD_TIME_CRITICAL")) SetPriority(THREAD_PRIORITY_TIME_CRITICAL);
	else if (Option.Same("THREAD_HIGH_CLASS")) SetPriorityClass(HIGH_PRIORITY_CLASS);
	else if (Option.Same("THREAD_IDLE_CLASS")) SetPriorityClass(IDLE_PRIORITY_CLASS);
	else if (Option.Same("THREAD_NORMAL_CLASS")) SetPriorityClass(NORMAL_PRIORITY_CLASS);
	else if (Option.Same("THREAD_REALTIME_CLASS")) SetPriorityClass(REALTIME_PRIORITY_CLASS);
	else return false;
	return true;
}

int CThread::StringToPriority(const CString& Priority) {
	if (Priority.Same("THREAD_PRIORITY_ABOVE_NORMAL")) return THREAD_PRIORITY_ABOVE_NORMAL;
	else if (Priority.Same("THREAD_PRIORITY_BELOW_NORMAL")) return THREAD_PRIORITY_BELOW_NORMAL;
	else if (Priority.Same("THREAD_PRIORITY_HIGHEST")) return THREAD_PRIORITY_HIGHEST;
	else if (Priority.Same("THREAD_PRIORITY_IDLE")) return THREAD_PRIORITY_IDLE;
	else if (Priority.Same("THREAD_PRIORITY_LOWEST")) return THREAD_PRIORITY_LOWEST;
	else if (Priority.Same("THREAD_PRIORITY_NORMAL")) return THREAD_PRIORITY_NORMAL;
	else if (Priority.Same("THREAD_PRIORITY_TIME_CRITICAL")) return THREAD_PRIORITY_TIME_CRITICAL;
	else return THREAD_PRIORITY_ABOVE_NORMAL;
}

CString CThread::PriorityToString(int Priority) {
	switch(Priority){
	case THREAD_PRIORITY_ABOVE_NORMAL: return "THREAD_PRIORITY_ABOVE_NORMAL";
	case THREAD_PRIORITY_BELOW_NORMAL: return "THREAD_PRIORITY_BELOW_NORMAL";
	case THREAD_PRIORITY_HIGHEST: return "THREAD_PRIORITY_HIGHEST";
	case THREAD_PRIORITY_IDLE: return "THREAD_PRIORITY_IDLE";
	case THREAD_PRIORITY_LOWEST: return "THREAD_PRIORITY_LOWEST";
	case THREAD_PRIORITY_NORMAL: return "THREAD_PRIORITY_NORMAL";
	case THREAD_PRIORITY_TIME_CRITICAL: return "THREAD_PRIORITY_TIME_CRITICAL";
	}
	return "THREAD_PRIORITY_NORMAL";
}

bool CThread::GetTimes(long& Kernel, long& User) const {
	FILETIME CreationTime, ExitTime, KernelTime, UserTime;
	if (::GetThreadTimes(m_ThreadHandle, &CreationTime, &ExitTime, &KernelTime, &UserTime)) {
		Kernel = (long) ((KernelTime.dwHighDateTime*(double)4295000) + (KernelTime.dwLowDateTime/(double)10000));
		User = (long) ((UserTime.dwHighDateTime*(double)4295000) + (UserTime.dwLowDateTime/(double)10000));
		return true;
	} else return false;
}

#endif
