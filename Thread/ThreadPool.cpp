/*

    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1994-2000 All Rights Reserved
    ______________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com

*/

#include <baseclasses.hpp>
#include <String/String.hpp>
#include <Date/Interval.hpp>
#include "ThreadPool.hpp"
 
CThreadPoolThread::CThreadPoolThread(void) : 
    CThread(),
    m_ThreadPool(NULL) 
{
    
}

CThreadPoolThread::~CThreadPoolThread(void) {
    
}

bool CThreadPoolThread::AcquireJob(CThreadPoolJob * CurrentJob) {
    
    CurrentJob->SetThreadFunction(NULL);
    CurrentJob->SetThreadArguments(NULL);
    
    if (m_ThreadPool->GetJobsList().GetSize()) {
        m_ThreadPool->GetJobsMutex().Lock();
        if (m_ThreadPool->GetJobsList().GetSize()) {		
            // _L_DEBUG(4, cout << (long) this << ":: Requesting a job from a list of " << m_ThreadPool->GetJobsList().GetSize() << " elements." << endl);
            * CurrentJob = m_ThreadPool->GetJobsList()[0];
            m_ThreadPool->GetJobsList().RemoveAt(0);
            // _L_DEBUG(4, cout << (long) this << ":: Owning job " << (int) CurrentJob->GetThreadFunction() << endl);
            // _L_DEBUG(4, cout << (long) this << ":: New job list count of " << m_ThreadPool->GetJobsList().GetSize() << " elements." << endl);    
        }
        m_ThreadPool->GetJobsMutex().UnLock();
        m_ActivityInterval.Reset();
        return true;
    }
    return false;
}

void CThreadPoolThread::RunJob(CThreadPoolJob * CurrentJob) {
    while (CurrentJob->GetThreadFunction()) {
        
        if (m_ThreadPool->GetTerminate())
            break;
        
        // _L_DEBUG(4, cout << (long) this << " :: Starting job " << (long) CurrentJob->GetThreadFunction() << endl);        
        void (* FunctionPointer)(void *) = (void (*)(void *)) CurrentJob->GetThreadFunction();
        (* FunctionPointer)(CurrentJob->GetThreadArguments());        
        // _L_DEBUG(4, cout << (long) this << " :: Done job " << (long) CurrentJob->GetThreadFunction() << endl);
        
        AcquireJob(CurrentJob);
    }
}

void CThreadPoolThread::Execute(void * Arguments) {
    
    // _L_DEBUG(4, cout << (long) this << " :: Started thread." << endl);

    // increment waiting and current threads counter
    
    CThreadPoolJob CurrentJob;
    
    m_ThreadPool->GetWaitingThreads().Inc();
    bool bIncState = true; // marked as waiting on the ThreadPool
    
    while(1) {

        if (AcquireJob(&CurrentJob)) {
            
            if (bIncState) {
                // if marked as waiting on the thread pool and a job was acquired, mark as running
                m_ThreadPool->GetWaitingThreads().Dec();
                bIncState = false;
            }

            // run the job as long as possible
            RunJob(&CurrentJob);
        }
        
        if (!bIncState) {
            // if not marked as waiting, then mark as waiting            
            // _L_DEBUG(4, cout << (long) this << " :: Idle, waiting for job." << endl);
            m_ThreadPool->GetWaitingThreads().Inc();
            bIncState = true;
        }
        
        if (m_ThreadPool->GetTerminate())
            break;

        base_sleep_ms(100);
        if (m_ActivityInterval.Get(itSeconds) >= m_ThreadPool->GetMaxThreadIdle()) {
            // _L_DEBUG(4, cout << (long) this << " :: Idle period expired, dying." << endl);
            break;
        }
            
    }
        
    if (bIncState) {
        // if marked as waiting, mark as ending
        m_ThreadPool->GetWaitingThreads().Dec();
        // _L_DEBUG(4, cout << (long) this << " :: Removing from waiting list." << endl);            
    }
    
    m_ThreadPool->GetJobsMutex().Lock();
    m_ThreadPool->GetThreads().Remove(this);
    m_ThreadPool->GetJobsMutex().UnLock();
    
    // _L_DEBUG(4, cout << (long) this << " :: Preparing to die." << endl);
    
    CThread::Execute(Arguments);
}

CThreadPool::CThreadPool(int MaxThreads, int MaxQueueSize) :
    m_RampupThreads(THREADPOOL_RAMPUP_DEFAULT),
    m_MaxThreads(MaxThreads),
    m_MaxQueueSize(MaxQueueSize),
    m_MaxThreadIdle(THREADPOOL_IDLETHREAD_LIFE),
    m_Terminate(false)
{
    // _L_DEBUG(4, cout << "Initializing thread pool (mt=" << m_MaxThreads << " / mq=" << m_MaxQueueSize << " jobs=" << m_JobsList.GetSize() << ")." << endl);
}

CThreadPool::~CThreadPool(void) {
    m_Terminate = true;
    
    bool bRemainingThreads = false;
    
    do {    
        m_JobsMutex.Lock();
        bRemainingThreads = (m_Threads.GetSize() > 0);
        m_JobsMutex.UnLock();
        
        if (bRemainingThreads)
            base_sleep_ms(100);
        
    } while (bRemainingThreads);      
}

bool CThreadPool::AddJob(const CThreadPoolJob& ThreadPoolJob) {
    // _L_DEBUG(4, cout << "Adding job to a queue of " << m_JobsList.GetSize() << " elements. (mqs=" << m_MaxQueueSize << ", term=" << m_Terminate << ")" << endl);
    if (m_Terminate)
        return false; 
    bool Result = true;
    m_JobsMutex.Lock();
    if ((m_MaxQueueSize != -1) && ((int) m_JobsList.GetSize() >= m_MaxQueueSize))
        Result = false;
    else m_JobsList += ThreadPoolJob;
    m_JobsMutex.UnLock();
    AdjustThreads();
    return Result;
}

void CThreadPool::AdjustThreads(void) {
    long PendingJobs = m_JobsList.GetSize();
    long CurrentThreads = m_Threads.GetSize();
    long WaitingThreads = m_WaitingThreads.Get();
    long CreateThreads = 0;
    // how many new threads to create
    CreateThreads = PendingJobs - WaitingThreads;
    
    if (CreateThreads <= 0)
        return;
    
    // always create two more threads in anticipation of load
    CreateThreads += m_RampupThreads;
    
    if (CreateThreads + CurrentThreads > m_MaxThreads)
        CreateThreads = m_MaxThreads - CurrentThreads;
    
    if (CreateThreads <= 0)
        return;
    
    // _L_DEBUG(4, cout << "Creating " << CreateThreads << " threads (max is " << m_MaxThreads << ", current is " << CurrentThreads << ")." << endl);
    
    if (CreateThreads) {    
        
        m_JobsMutex.Lock();
        
        for (register int i=0;i<CreateThreads;i++) {
            CThreadPoolThread * ThreadPoolThread = new CThreadPoolThread;
            m_Threads.Add(ThreadPoolThread);
            // cout << "Creating " << (long) ThreadPoolThread << endl;
            // cout << "New thread, thread size is now " << m_Threads.GetSize() << endl;    
            ThreadPoolThread->SetThreadPool(this);
            ThreadPoolThread->Launch();
        }
        
        m_JobsMutex.UnLock();
    }
}

void CThreadPool::PassiveWait(bool bClearJobs, int ReturnTimeout) {
    
    if (bClearJobs) {
        // cout << "CThreadPool :: cleaning up jobs ..."; cout.flush();
        m_JobsMutex.Lock();    
        while (m_JobsList.GetSize()) {
            CThreadPoolJob CurrentJob = GetJobsList()[0];
            void (*CleanupFunctionPointer)(void *) = (void (*)(void *)) CurrentJob.GetCleanupFunction();
            if (CleanupFunctionPointer) {
                (* CleanupFunctionPointer)(CurrentJob.GetThreadArguments());
            }
            GetJobsList().RemoveAt(0);
        }
        m_JobsMutex.UnLock();  
        // cout << " done." << endl;
    }
    
    CInterval TimeoutInterval;
    int JobsSize = m_JobsList.GetSize();
    int RemainingThreads = m_Threads.GetSize() - m_WaitingThreads.Get();
    while (JobsSize || RemainingThreads) {
        
        // cout << "jobs left: " << JobsSize << endl;
        // cout << "threads left: " << RemainingThreads << endl;
        // cout << "total threads: " << m_Threads.GetSize() << endl;
        // cout << "waiting threads: " << m_WaitingThreads.Get() << endl;
        
        // _L_DEBUG(2, cout << "jobs left: " << JobsSize << endl);
        // _L_DEBUG(2, cout << "threads left: " << RemainingThreads << endl);
        base_sleep(1);
        JobsSize = m_JobsList.GetSize();
        RemainingThreads = m_Threads.GetSize() - m_WaitingThreads.Get();    
        if (ReturnTimeout && (TimeoutInterval.Get() >= ReturnTimeout))
            break;
    }
}

void CThreadPool::TerminateThreads(void) {
    
    bool bRemainingThreads = false;
    
    do {    
        
        m_JobsMutex.Lock();
        bRemainingThreads = (m_Threads.GetSize() > 0);
        if (bRemainingThreads) {
            ((CThread *) m_Threads[m_Threads.GetSize() - 1])->Terminate();
            m_Threads.RemoveAt(m_Threads.GetSize() - 1);
        }
        m_JobsMutex.UnLock();
        
    } while (bRemainingThreads);          
}
