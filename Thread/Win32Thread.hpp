/*
  © Vestris Inc., Geneva, Switzerland
  http://www.vestris.com, 1994-1999 All Rights Reserved
  ______________________________________________

  written by Daniel Doubrovkine - dblock@vestris.com

*/

#ifndef BASE_WIN32_THREAD_HPP
#define BASE_WIN32_THREAD_HPP

#ifdef _WIN32

#include <platform/include.hpp>
#include <Object/Object.hpp>
#include <Thread/Thread.hpp>
#include <String/String.hpp>
#include <Mutex/Atomic.hpp>

class CThread : public CObject {
	copy_property(CMutex *, ThreadStartMutex);
	readonly_property(unsigned long, ThreadID);
	readonly_property(HANDLE, ThreadHandle);
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
	inline void Sleep(const int Interval) const { base_sleep_ms(Interval); }
	virtual void Execute(void *);
	/* priority modifiers */
	bool SetPriority(int PriorityLevel = THREAD_PRIORITY_NORMAL);
	bool SetPriorityClass(unsigned long PriorityClass = NORMAL_PRIORITY_CLASS);
	bool SetPriorityBoost(bool Boost);
	long GetPriorityClass(void) const;
	long GetPriority(void) const;
	void PassiveWait(unsigned long Milliseconds = INFINITE) const;
	bool SetThreadOption(const CString& Option);
	static CString PriorityToString(int Priority);
	static int StringToPriority(const CString& Priority);
	static CAtomic m_UserTime;
	static CAtomic m_KernelTime;
	bool GetTimes(long& Kernel, long& User) const;
private:
	void Run(void);
	static unsigned long WINAPI CThreadExecute(void * ThreadPointer);
};

#endif
#endif
