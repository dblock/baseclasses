/*
    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1994-1999 All Rights Reserved
    ______________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com

*/

#ifndef BASE_PING_THREAD_CLIENT_HPP
#define BASE_PING_THREAD_CLIENT_HPP
    
#include <platform/include.hpp>
#include <Thread/Thread.hpp>
#include <File/RemoteFile.hpp>
#include <Internet/Url.hpp>

class CPingClient : public CThread {
  property(CUrl, PingUrl);
  property(int, PingInterval);
  property(CAtomic, PingFailed);
  property(bool, bPing);
  property(bool, Terminate);
public:
  CPingClient(void);
  CPingClient(const CString& PingUrl);
  virtual ~CPingClient(void);
  virtual void Execute(void * Arguments);
  virtual void PingFailed(int Count) const;
  virtual void PingSucceeded(void) const;
};
    
#endif
