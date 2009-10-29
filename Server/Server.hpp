/*
    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1994-1999 All Rights Reserved
    ______________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com

*/

#ifndef BASE_MULTITHREAD_SERVER_HPP
#define BASE_MULTITHREAD_SERVER_HPP

#include <platform/include.hpp>
#include <Thread/Thread.hpp>
#include <Server/Connection.hpp>
#include <Thread/ThreadPool.hpp>
#include <Date/Interval.hpp>
    
class CServer : public CThread {
  property(unsigned short, Port);
  property(unsigned int, Addr);
  readonly_property(bool, Binding);
  readonly_property(bool, Bound);
  readonly_property(struct sockaddr_in, ServerAddrIn);
  readonly_property(int, ListenSocket);
  readonly_property(CThreadPool, ThreadPool);  
  property(int, AcceptInterval);
  property(CInterval, RestartInterval);
#ifdef _UNIX
  property(pid_t,  ForkedParentPid);
#endif
  property(bool, Nagle);
public:
  CServer(unsigned short = 0, const CString& Address = CString::EmptyCString);  
  virtual ~CServer(void);
  virtual void ExecuteClient(const CConnection&);
  virtual void Bind(void);
  virtual void UnBind(void);
  virtual void Restart(void);
  virtual void Shutdown(void);
  static bool GetBindAddressPort(const CString& Pair, CString * pAddress, CString * pPort);
#ifdef _UNIX
  static bool DaemonForkedProcess(void);
#endif
private:
  void ExecuteRemoteClient(void);
  virtual void Execute(void * Arguments = NULL);
  bool BindSocket(void);
  bool CreateSocket(void);
  bool ListenSocket(void);
};

#endif
