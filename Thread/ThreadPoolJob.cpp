/*
    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1994-2000 All Rights Reserved
    ______________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com

*/

#include <baseclasses.hpp>
#include "ThreadPoolJob.hpp"

CThreadPoolJob::CThreadPoolJob(void) {
  m_ThreadFunction = m_ThreadArguments = m_CleanupFunction = NULL;
}

CThreadPoolJob::~CThreadPoolJob(void) {

}

CThreadPoolJob::CThreadPoolJob(void * ThreadFunction, void * ThreadArguments, void * CleanupFunction) {
  m_ThreadFunction = ThreadFunction;
  m_ThreadArguments = ThreadArguments;
  m_CleanupFunction = CleanupFunction;
}

CThreadPoolJob& CThreadPoolJob::operator=(const CThreadPoolJob& ThreadPoolJob) {
  m_ThreadFunction = ThreadPoolJob.m_ThreadFunction;
  m_ThreadArguments = ThreadPoolJob.m_ThreadArguments;
  m_CleanupFunction = ThreadPoolJob.m_CleanupFunction;
  return * this;
}

CThreadPoolJob::CThreadPoolJob(const CThreadPoolJob& ThreadPoolJob) {
  operator=(ThreadPoolJob);
}  

ostream& CThreadPoolJob::operator<<(ostream& Stream) const {
  return Stream;
}
