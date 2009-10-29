/*
    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1994-2000 All Rights Reserved
    ______________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com

*/

#ifndef BASE_THREADPOOLJOB_HPP
#define BASE_THREADPOOLJOB_HPP

#include <platform/include.hpp>
#include <Object/Object.hpp>
#include <Thread/Thread.hpp>

class CThreadPoolJob : public CObject {
  copy_property(void *, ThreadFunction);
  copy_property(void *, CleanupFunction);
  copy_property(void *, ThreadArguments);
public:
  CThreadPoolJob(void);
  virtual ~CThreadPoolJob(void);
  CThreadPoolJob(void * ThreadFunction, void * ThreadArguments = NULL, void * CleanupFunction = NULL);
  CThreadPoolJob& operator=(const CThreadPoolJob& ThreadPoolJob);
  CThreadPoolJob(const CThreadPoolJob& ThreadPoolJob);
  ostream& operator<<(ostream& Stream) const;
};

#endif
