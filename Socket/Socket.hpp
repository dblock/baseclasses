/*
    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1994-1999 All Rights Reserved
    ______________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com

    01.09.1999: full implementation of TLI instead of sockets __TLI_SUPPORTED 
                must be defined to prefer TLI to sockets
    01.09.1999: retrieving of for example 
                http://www.arch.ethz.ch/infostud/index.phtml?lang=de&sub=obl1&text=ber2 
                cuts the connection at the same byte count    
    06.09.1999: fixed read of MaxLength bytes in Read (avoid select when no more data)    
    17.09.1999: tests show that the problem with the sockets reaching a dead 
                TIME_WAIT or CLOSE_WAIT state has been fixed
    13.11.2001: setting TCP send and receive buffer sizes

*/

#ifndef BASE_SOCKET_HPP
#define BASE_SOCKET_HPP

#include <platform/include.hpp>
#include <String/String.hpp>
#include <Mutex/Mutex.hpp>
#include <File/Progress.hpp>
#include <Socket/SockTypes.hpp>
#include <Socket/Dns.hpp>

class CSocket : public CObject {
	property(bool, Verbose);
	property(CString, PeerHostName);
	property(CString, Error);
	property(bool, UnrecoverableError);
	property(CVector<CString>, PeerAddresses);
	property(CVector<CString>, PeerAliases);
	property(CString, RemoteHost);
	property(int, PeerPort);
	property(sockaddr_in, PeerAddrIn);
	property(int, PeerSocket);
	property(int, Timeout);
	property(unsigned long, RemoteAddr);
	property(int, RemotePort);
    property(int, RStatus);
public:
    static CDomainNameService * m_pDNService;
private:
#ifdef __TLI_SUPPORTED
	struct t_call * m_TliCallPtr;
#endif
	mutable CString m_PendingBuffer;
	mutable CProgress m_Progress;
	bool CreateHost(void);
	bool CreateSocket(void);
public:
	void SetBlockMode(bool Blocking = false);
	void MakePeerInformation(bool bAliases = true);
	bool LingerSocket(void);
    bool DisableNagle(void);
    // on AIX or Unicos, the default value must be 1 and the option to setsockopt should be TCP_RFC1323
    bool SetSendTcpWindowSize(int nSize = 128 * 1024);
    bool SetRecvTcpWindowSize(int nSize = 128 * 1024);
    bool IsPeerLocalHost(void) const;
	/* read-write */
	int Write(const CString&) const;
	int WriteLine(const CString&) const;
	int ReadLine(CString *) const;
	int Read(CString *, const int MaxLength = -1) const;
	/* connect */
	void SetSocket(const int, const CString&);
	CSocket(void);
	CSocket(const CSocket&);
	CSocket(const int, const CString&);
	CSocket& operator=(const CSocket&);	
	virtual ~CSocket(void);
    bool Bind(void);
	bool Open(void);
	bool Connect(void);
	void Close(void);
    void LingeringClose(void);
};

#endif
