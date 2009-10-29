/*
    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1994-2000 All Rights Reserved
    ______________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com

*/

#ifndef BASE_THREADPOOL_HPP
#define BASE_THREADPOOL_HPP

#include <platform/include.hpp>
#include <Object/Object.hpp>
#include <List/List.hpp>
#include <Mutex/Atomic.hpp>
#include <Thread/Thread.hpp>
#include <Thread/ThreadPoolJob.hpp>
#include <String/String.hpp>
#include <Vector/Vector.hpp>
#include <Date/Interval.hpp>

#define THREADPOOL_MAXTHREADS_DEFAULT       25 // maximum threads to hold in the pool
#define THREADPOOL_QUEUESIZE_DEFAULT        24 // maximum amount of jobs to queue
#define THREADPOOL_IDLETHREAD_LIFE          15 // number of seconds a thread lives when it's doing nothing
#define THREADPOOL_RAMPUP_DEFAULT           0  // default thread rampup

class CThreadPool;

class CThreadPoolThread : public CThread {
    copy_property(CThreadPool *, ThreadPool);
    property(CInterval, ActivityInterval);
public:
    CThreadPoolThread(void);
    virtual ~CThreadPoolThread(void);
    virtual void Execute(void * Arguments);
    bool AcquireJob(CThreadPoolJob * CurrentJob);
    void RunJob(CThreadPoolJob * CurrentJob);
};

class CThreadPool : public CObject {
    property(CAtomic, WaitingThreads);
    // property(CAtomic, CurrentThreads);
    property(CVector<CObject *>, Threads);
    property(int, RampupThreads);
    property(int, MaxThreads);
    property(int, MaxQueueSize);
    property(int, MaxThreadIdle);
    property(bool, Terminate);
    property(CList<CThreadPoolJob>, JobsList);
    property(CMutex, JobsMutex);
private:
    // disable copying
    CThreadPool(const CThreadPool&);
    CThreadPool& operator=(const CThreadPool&);
    void AdjustThreads(void);
protected:
    void TerminateThreads(void);
public:
    CThreadPool(
        int MaxThreads = THREADPOOL_MAXTHREADS_DEFAULT,
        int MaxQueueSize = THREADPOOL_QUEUESIZE_DEFAULT);
    virtual ~CThreadPool(void);
    // jobs
    bool AddJob(const CThreadPoolJob& ThreadPoolJob);
    void SetTerminate(void);
    friend inline ostream& operator<<(ostream&, const CThreadPoolJob&);
    void PassiveWait(bool bClearJobs = false, int ReturnTimeout = 0);
};

inline ostream& operator<<(ostream& Stream, const CThreadPoolJob& Job) { return Job.operator<<(Stream); }

#endif
