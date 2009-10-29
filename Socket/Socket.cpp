/*
    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1994-1999 All Rights Reserved
    ______________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com        
    
    Revision history:
    
    24.09.1999: (UNIX) using reentrant gethostbyname_r and gethostbyaddr_r
                (verified that NT allocates different static structures per thread)
    26.05.2000: protected non-mt functions and critical socket operations
    
    todo: AF-independent socket programming for IPv6:
    http://www.kame.net/newsletter/19980604/
    
*/

#include <baseclasses.hpp>

#include <Socket/Socket.hpp>
#include <File/Progress.hpp>
#include <String/GStrings.hpp>
#include <Mutex/Mutex.hpp>
#include <Date/Interval.hpp>

CDomainNameService * CSocket :: m_pDNService = NULL;

CSocket::CSocket(void) : 
m_Progress(20, false) 
{
    
    Trace(tagSocket, levVerbose, ("CSocket :: CSocket - constructor."));    

    m_PeerPort = 0;
    m_PeerSocket = -1;
    m_Timeout = 30;
    m_Verbose = false;
    m_UnrecoverableError = false;
#ifdef __TLI_SUPPORTED
    m_TliCallPtr = NULL;
#endif
    m_RStatus = 0;

    assert(m_pDNService);
}

CSocket::CSocket(const int Port, const CString& Name) : 
m_Progress(20, false) 
{
    Trace(tagSocket, levVerbose, ("CSocket :: CSocket - %s:%d constructor.", Name.GetBuffer(), Port));

    m_PeerPort = 0;
    m_PeerSocket = -1;
    m_Timeout = 30;
    m_Verbose = false;
    m_UnrecoverableError = false;
#ifdef __TLI_SUPPORTED
    m_TliCallPtr = NULL;
#endif
    m_RStatus = 0;
    SetSocket(Port, Name);
}

void CSocket::SetSocket(const int Port, const CString& Name) {
    
    Trace(tagSocket, levVerbose, ("CSocket :: SetSocket %s:%d.", Name.GetBuffer(), Port));    

    Close();
    
    m_UnrecoverableError = false;
    m_RStatus = 0;
    m_PeerPort = Port;
    
    if (Name.Same(g_strHostLocalName)) {
        m_PeerHostName = g_strHostLocalAddress;
        Trace(tagSocket, levVerbose, ("CSocket :: SetSocket - this is a local host."));    
    } else {
        m_PeerHostName = Name;
    }
    
    m_PeerSocket = -1;	
    m_Error.Empty();
}

bool CSocket::Bind(void) {

    Trace(tagSocket, levVerbose, ("CSocket :: Bind %s:%d.", m_PeerHostName.GetBuffer(), m_PeerPort));    

    if (m_PeerSocket < 0) 
          return false;	

    struct sockaddr_in Address;

    Address.sin_family = AF_INET;
    Address.sin_port = 0;
    Address.sin_addr.s_addr = INADDR_ANY;
    
    bool bResult = ! bind(m_PeerSocket, (struct sockaddr *) & Address, sizeof(Address));

    if (! bResult) {
        Trace(tagSocket, levError, ("CSocket :: Bind %s:%d failed.", m_PeerHostName.GetBuffer(), m_PeerPort));
        m_RStatus = 924; // socket bind error
    }

    return bResult;    
}

bool CSocket::Open(void) {
    
    Trace(tagSocket, levVerbose, ("CSocket :: Open %s:%d.", m_PeerHostName.GetBuffer(), m_PeerPort));    

    Close();
    m_Error.Empty();
    
    bool bResult;
    bResult = CreateSocket();
    if (!bResult) {
        Trace(tagSocket, levError, ("CSocket :: Open %s:%d - CreateSocket failed.", m_PeerHostName.GetBuffer(), m_PeerPort));    
        return false;
    }

    bResult = CreateHost();
    
    if (!bResult)
        return false;
    
    return true;
}

bool CSocket::CreateHost(void) {
	
    Trace(tagSocket, levVerbose, ("CSocket :: CreateHost %s:%d.", m_PeerHostName.GetBuffer(), m_PeerPort));    
	
    if (m_PeerSocket < 0) 
          return false;	
        
    m_PeerAddrIn.sin_family = AF_INET;
	m_PeerAddrIn.sin_port = htons(m_PeerPort);

    if (! m_pDNService->CreateHost(m_PeerHostName, m_PeerAddrIn, m_RStatus)) {
        Trace(tagSocket, levError, ("CSocket :: CreateHost %s:%d - DNS service CreateHost failed.", m_PeerHostName.GetBuffer(), m_PeerPort));   
        m_Error = m_PeerHostName;
        m_Error += " - DNS resolution failed.";
        m_UnrecoverableError = true;
        return false;
    }
    
    return true;
}

bool CSocket::CreateSocket(void) {
    Trace(tagSocket, levVerbose, ("CSocket :: CreateSocket %s:%d.", m_PeerHostName.GetBuffer(), m_PeerPort));    
#ifdef __TLI_SUPPORTED
	if ((m_PeerSocket = t_open(DEV_TCP, O_RDWR|O_NONBLOCK, NULL)) < 0) {		
		m_Error = m_PeerHostName;
		switch(t_errno) {
		case TBADFLAG: m_Error+=" - (TLI) An invalid flag is specified."; break;
		case TBADNAME: m_Error+=" - (TLI) Invalid transport provider name."; break;
		case TPROTO: m_Error+=" - (TLI) A communication  problem  has been detected between XTI and the transport provider"; break;
		case TSYSERR: m_Error+=" - (TLI) An invalid flag is specified."; break;		
		default: m_Error+=" - (TLI) Unable to create TLI socket, unexpected error."; break;
		};
		Trace(tagSocket, levError, ("CSocket :: CreateSocket %s:%d - %s.", m_PeerHostName.GetBuffer(), m_PeerPort, m_Error.GetBuffer()));    
		return false;
	}
#else // __TLI_SUPPORTED	
    m_PeerSocket = socket(AF_INET, SOCK_STREAM, 0);	
	if (m_PeerSocket == -1) {		
		m_Error = m_PeerHostName;
#ifdef _UNIX
		switch(errno){
		case EACCES: m_UnrecoverableError = true; m_Error+=" - The process does not have appropriate privileges."; break;
		case EAFNOSUPPORT: m_UnrecoverableError = true; m_Error+=" - The implementation does not support the specified address family."; break;
		case EPROTOTYPE: m_UnrecoverableError = true; m_Error+=" - The socket type is not supported by the protocol."; break;
		case ENOBUFS: m_Error+=" - Insufficient resources were available in the system to perform the operation."; break;
#ifdef ENOSR
		case ENOSR: m_Error+=" - There were insufficient STREAMS resources available for the operation to complete."; break;
#endif
		case EPROTONOSUPPORT: m_UnrecoverableError = true; m_Error+=" - The protocol is not supported by the address family, or the protocol is not supported by the implementation."; break;
		case EMFILE: m_Error+=" - No more file descriptors are available for this process."; break;
		case ENFILE: m_Error+=" - No more file descriptors are available for the system."; break;
		case ENOMEM: m_Error+=" - Insufficient memory was available to fulfill the request."; break;
		default: m_Error+=" - Unable to create socket, unexpected error."; break;
		}
#endif // _UNIX
#ifdef _WIN32
		switch(WSAGetLastError()) {
		case WSANOTINITIALISED: m_UnrecoverableError = true; m_Error+=" - A successful WSAStartup must occur before using this function."; break;
		case WSAENETDOWN: m_Error+=" - The network subsystem or the associated service provider has failed."; break;
		case WSAEAFNOSUPPORT: m_UnrecoverableError = true; m_Error+=" - The specified address family is not supported."; break;
		case WSAEINPROGRESS: m_Error+=" - A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function."; break;
		case WSAEMFILE: m_Error+=" - No more socket descriptors are available."; break;
		case WSAENOBUFS: m_Error+=" - No buffer space is available. The socket cannot be created."; break;
		case WSAEPROTONOSUPPORT: m_UnrecoverableError = true; m_Error+=" - The specified protocol is not supported."; break;
		case WSAEPROTOTYPE: m_UnrecoverableError = true; m_Error+=" - The specified protocol is the wrong type for this socket."; break;
		case WSAESOCKTNOSUPPORT: m_UnrecoverableError = true; m_Error+=" - The specified socket type is not supported in this address family."; break;
		default: m_Error+=" - Unable to create socket, unexpected error."; break;
		}
#endif
        Trace(tagSocket, levError, ("CSocket :: CreateSocket %s:%d - %s.", m_PeerHostName.GetBuffer(), m_PeerPort, m_Error.GetBuffer()));    
        m_RStatus = 921; // Socket error
		return false;
	}
#endif // __TLI_SUPPORTED
	Trace(tagSocket, levInfo, ("CSocket :: CreateSocket %s:%d - success.", m_PeerHostName.GetBuffer(), m_PeerPort));    
	return true;
}

/* set socket in non-blocking mode */
void CSocket::SetBlockMode(bool Blocking) {
    Trace(tagSocket, levInfo, ("CSocket :: SetBlockMode %s:%d - %s.", m_PeerHostName.GetBuffer(), m_PeerPort, Blocking?"blocking":"non-blocking"));    
#ifdef _UNIX
	int SocketState = fcntl(m_PeerSocket, F_GETFL);
	if (Blocking) SocketState &= (~O_NONBLOCK);
	else SocketState |= O_NONBLOCK;
	fcntl(m_PeerSocket, F_SETFL, SocketState);
#endif
#ifdef _WIN32
	unsigned long BlockingParameter = !Blocking;
    if (ioctlsocket(m_PeerSocket, FIONBIO, &BlockingParameter) == SOCKET_ERROR) {
        Trace(tagSocket, levError, ("CSocket :: SetBlockMode ioctlsocket %s:%d - failed.", m_PeerHostName.GetBuffer(), m_PeerPort));    
        // DWORD dwError = WSAGetLastError();
    }
#endif
}

bool CSocket::Connect(void) {
	Trace(tagSocket, levInfo, ("CSocket :: Connect %s:%d.", m_PeerHostName.GetBuffer(), m_PeerPort));    
    if (m_PeerSocket < 0) 
        return false;		
#ifdef __TLI_SUPPORTED // already opened non-blocking
	if (m_TliCallPtr) t_free((char *) m_TliCallPtr, T_CALL);	
	m_TliCallPtr = (struct t_call *)t_alloc(m_PeerSocket, T_CALL, T_ADDR);
	if (!m_TliCallPtr || !(m_TliCallPtr->addr.buf)) {
        m_RStatus = 925; // TLI Error
		m_Error = m_PeerHostName + " - TLI Allocator has failed.";
		return false;		
	}
	m_TliCallPtr->opt.len = m_TliCallPtr->udata.len = 0;
	m_TliCallPtr->addr.len = sizeof(m_PeerAddrIn);
	m_TliCallPtr->addr.buf = (char *) &m_PeerAddrIn;	
	if (t_bind(m_PeerSocket, NULL, NULL) < 0) {
        m_RStatus = 925; // TLI Error
		m_Error = m_PeerHostName + " - (TLI) Unable to bind.";		
        Trace(tagSocket, levError, ("CSocket :: Connect TLI %s:%d - %s.", m_PeerHostName.GetBuffer(), m_PeerPort, m_Error.GetBuffer()));    
		return false;
	}
	fd_set fdset; FD_ZERO(&fdset); FD_SET(m_PeerSocket,&fdset);
	struct timeval tv; tv.tv_sec=m_Timeout; tv.tv_usec=0;
	if (
	    (!t_connect(m_PeerSocket, m_TliCallPtr, NULL) < 0) ||
	    (t_errno == TNODATA && 
	     base_select(m_PeerSocket + 1, &fdset, NULL, NULL, &tv) && 
	     !t_rcvconnect(m_PeerSocket, m_TliCallPtr))
	    ) {
		// connection succeeded
	} else {        
        m_RStatus = 922; // Connection timed out
		m_Error = m_PeerHostName;		
		switch(t_errno) {
		case TACCES: m_Error+=" - (TLI) The user does not have permission to use the specified address or options."; return false;			
		case TADDRBUSY: m_Error+=" - (TLI) This transport provider does not support multiple connections with the same local and remote addresses."; break; // connection already exists
		case TBADADDR: m_Error+=" - (TLI) The specified protocol address was in  an incorrect format or contained illegal information."; return false;
		case TBADDATA: m_Error+=" - (TLI) The amount of user data specified was not within the bounds allowed by the transport provider."; return false;
		case TBADF: m_Error+=" - (TLI) The specified file descriptor does not refer to a transport endpoint."; return false;
		case TBADOPT: m_Error+=" - (TLI) The specified protocol options were in an incorrect format or contained  illegal information."; return false;
		case TBUFOVFLW: m_Error+=" - (TLI) The number of bytes allocated for an incoming  argument is greater than 0 but not sufficient to store the value of that argument."; return false;
		case TLOOK: m_Error+=" - (TLI) An asynchronous event has occurred on this transport endpoint and requires immediate attention."; return false;
		case TNODATA: m_Error+=" - (TLI) O_NONBLOCK was set, so the function successfully initiated the connection establishment procedure, but did not wait for a response from the remote user."; break;
		case TNOTSUPPORT: m_Error+=" - (TLI) This function is not supported by the underlying transport provider."; return false;
		case TOUTSTATE: m_Error+=" - (TLI) The communications endpoint referenced by fd or resfd is not in one of the states in which a call to this function is valid."; return false;
		case TPROTO: m_Error+=" - (TLI) This error indicates that a communication problem has been detected between XTI and the transport provider for which there is no other suitable XTI t_errno value."; return false;
		case TSYSERR: m_Error+=" - (TLI) A system error has occurred during execution  of this function."; return false;
		default: m_Error+=" - (TLI) t_connect() has failed, unexpected error " + CString::IntToStr(t_errno) + "."; break;
		};

        Trace(tagSocket, levError, ("CSocket :: Connect %s:%d - %s.", m_PeerHostName.GetBuffer(), m_PeerPort, m_Error.GetBuffer()));    
	}	
#else
	SetBlockMode(false);	
    int SocketConnect = base_connect(m_PeerSocket,(struct sockaddr *) &m_PeerAddrIn, sizeof(m_PeerAddrIn));
	if (SocketConnect == -1) {
		m_Error = m_PeerHostName;
#ifdef _UNIX
		switch(errno) {
		case EADDRNOTAVAIL: m_UnrecoverableError = true; m_Error+=" - The specified address is not available from the local machine."; return false;
		case EAFNOSUPPORT: m_UnrecoverableError = true; m_Error+=" - The specified address is not a valid address for the address family of the specified socket."; return false;
		case EALREADY: m_Error+=" - A connection request is already in progress for the specified socket."; break;
		case EBADF: m_Error+=" - The socket argument is not a valid file descriptor."; return false;
		case ECONNREFUSED: m_Error+=" - The target address was not listening for connections or refused the connection request."; return false;
		case EINPROGRESS: m_Error+=" - O_NONBLOCK is set for the file descriptor for the socket and the connection cannot be immediately established; the connection will be established asynchronously."; break;
		case EINTR: m_Error+=" - The attempt to establish a connection was interrupted by delivery of a signal that was caught; the connection will be established asynchronously."; break;
		case EISCONN: m_Error+=" - The specified socket is connection-mode and is already connected."; break;
		case ENETUNREACH: m_Error+=" - No route to the network is present."; return false;
		case ENOTSOCK: m_UnrecoverableError = true; m_Error+=" - The socket argument does not refer to a socket."; return false;
		case EPROTOTYPE: m_Error+=" - The specified address has a different type than the socket bound to the specified peer address."; return false;
		case ETIMEDOUT: m_Error+=" - The attempt to connect timed out before a connection was made."; return false;
		case EADDRINUSE: m_Error+=" - Attempt to establish a connection that uses addresses that are already in use."; return false;
		case ECONNRESET: m_Error+=" - Remote host reset the connection request."; return false;
		case EHOSTUNREACH: m_Error+=" - The destination host cannot be reached (probably because the host is down or a remoterouter cannot reach it)."; return false;
		case EINVAL: m_UnrecoverableError = true; m_Error+=" - The address_len argument is not a valid length for the address family; or invalid address family in sockaddr structure."; return false;
		case ENAMETOOLONG: m_UnrecoverableError = true; m_Error+=" - Pathname resolution of a symbolic link produced an intermediate result whose length exceeds PATH_MAX."; return false;
		case ENETDOWN: m_Error+=" - The local interface used to reach the destination is down."; return false;
		case ENOBUFS: m_Error+=" - No buffer space is available."; return false;
#ifdef ENOSR
		case ENOSR: m_Error+=" - There were insufficient STREAMS resources available to complete the operation."; return false;
#endif
		case EOPNOTSUPP: m_UnrecoverableError = true; m_Error+=" - The socket is listening and can not be connected."; return false;
		default: m_Error+=" - connect() has failed, unexpected error " + CString::IntToStr(errno) + "."; break;
		}
#endif // _UNIX
#ifdef _WIN32
		switch(WSAGetLastError()) {
		case WSANOTINITIALISED: m_UnrecoverableError = true; m_Error+=" - A successful WSAStartup must occur before using this function."; return false;
		case WSAENETDOWN: m_Error+=" - The network subsystem has failed."; return false;
		case WSAEADDRINUSE: m_Error+=" - The socket's local address is already in use and the socket was not marked to allow address reuse with SO_REUSEADDR. This error usually occurs when executing bind, but could be delayed until this function if the bind was to a partially wild-card address (involving ADDR_ANY) and if a specific address needs to be committed at the time of this function."; return false;
		case WSAEINTR: m_Error+=" - The (blocking) Windows Socket 1.1 call was canceled through WSACancelBlockingCall."; return false;
		case WSAEINPROGRESS: m_Error+=" - A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function."; return false;
		case WSAEALREADY: /*m_Error+=" - A non-blocking connect call is in progress on the specified socket.";*/ break;
		case WSAEADDRNOTAVAIL: m_UnrecoverableError = true; m_Error+=" - The remote address is not a valid address (such as ADDR_ANY)."; return false;
		case WSAEAFNOSUPPORT: m_UnrecoverableError = true; m_Error+=" - Addresses in the specified family cannot be used with this socket."; return false;
		case WSAECONNREFUSED: m_Error+=" - The attempt to connect was forcefully rejected."; return false;
		case WSAEFAULT: m_UnrecoverableError = true; m_Error+=" - The name or the namelen parameter is not a valid part of the user address space, the namelen parameter is too small, or the name parameter contains incorrect address format for the associated address family."; return false;
		case WSAEINVAL: m_UnrecoverableError = true; m_Error+=" - The parameter is a listening socket, or the destination address specified is not consistent with that of the constrained group the socket belongs to."; return false;
		case WSAEISCONN: m_Error+=" - The socket is already connected (connection-oriented sockets only)."; return false;
		case WSAENETUNREACH: m_Error+=" - The network cannot be reached from this host at this time."; return false;
		case WSAENOBUFS: m_Error+=" - No buffer space is available. The socket cannot be connected."; return false;
		case WSAENOTSOCK: m_UnrecoverableError = true; m_Error+=" - The descriptor is not a socket."; return false;
		case WSAETIMEDOUT: m_Error+=" - Attempt to connect timed out without establishing a connection."; return false;
		case WSAEWOULDBLOCK: /*m_Error+=" - The socket is marked as non-blocking and the connection cannot be completed immediately.";*/ break;
		case WSAEACCES: m_Error+=" - Attempt to connect datagram socket to broadcast address failed because setsockopt option SO_BROADCAST is not enabled."; return false;
		default: m_Error+=" - connect() has failed, unexpected error " + CString::IntToStr(WSAGetLastError()) + "."; return false;
		}
#endif // WIN32
        Trace(tagSocket, levError, ("CSocket :: Connect %s:%d - %s.", m_PeerHostName.GetBuffer(), m_PeerPort, m_Error.GetBuffer()));
	}	
	/* poll the non-blocking socket to see if the connection has succeeded */
	fd_set fdset; FD_ZERO(&fdset); FD_SET((unsigned int) m_PeerSocket,&fdset);
	struct timeval tv; 
    tv.tv_sec = m_Timeout; 
    tv.tv_usec = 0;
	if (base_select(m_PeerSocket + 1, NULL, &fdset, NULL, &tv) <= 0) {        
        m_RStatus = 922; // Connection timed out
		if (m_Verbose) { cout << "[timed out]"; cout.flush(); }		
		m_Error = m_PeerHostName + " - Connection timed out (waited " + CString::IntToStr(m_Timeout) + " seconds).";
		Trace(tagSocket, levError, ("CSocket :: Connect %s:%d - %s.", m_PeerHostName.GetBuffer(), m_PeerPort, m_Error.GetBuffer()));    
		return false;
	}
#endif // __TLI_SUPPORTED

	/* leave out-of-band data inline (from telnet truss on Solaris) */
	int Value = 1; 
	setsockopt(m_PeerSocket, SOL_SOCKET, SO_OOBINLINE, (char *) &Value, sizeof(int));
	// setsockopt(m_PeerSocket, SOL_SOCKET, SO_KEEPALIVE, (char *) &Value, sizeof(int));
    
	Trace(tagSocket, levInfo, ("CSocket :: Connect %s:%d - succeeded (%d).", m_PeerHostName.GetBuffer(), m_PeerPort, m_PeerSocket));    
	return true;
}

void CSocket::Close(void) {
    Trace(tagSocket, levInfo, ("CSocket :: Close %s:%d.", m_PeerHostName.GetBuffer(), m_PeerPort));    

    if (m_PeerSocket >= 0) {
        LingeringClose();        
    }
}

CSocket::CSocket(const CSocket&) : m_Progress(20, false) {
	// socket copy is not allowed	
	assert(0);
}

CSocket& CSocket::operator=(const CSocket&){
	// socket copy is not allowed
	assert(0);
	return * this;
}

CSocket::~CSocket(void) {	
	Close();
}

void CSocket::MakePeerInformation(bool bAliases) {
	
    Trace(tagSocket, levInfo, ("CSocket :: MakePeerInformation %s:%d.", m_PeerHostName.GetBuffer(), m_PeerPort));    

    if (m_PeerSocket < 0) 
        return;

    m_RemoteHost = inet_ntoa(m_PeerAddrIn.sin_addr);
    m_RemotePort = ntohs(m_PeerAddrIn.sin_port);
    m_RemoteAddr = inet_addr((const char *) m_RemoteHost.GetBuffer());
    if (m_RemoteHost == g_strHostLocalAddress) {
        m_PeerAddresses.Add(g_strHostLocalName);
        return;
    }
        
    if (! bAliases)
        return;

    m_pDNService->CreatePeer(
        m_RemoteAddr,
        m_PeerAddresses,
        m_PeerAliases);	
}

bool CSocket::LingerSocket(void) {
    if (m_PeerSocket < 0) 
        return false;
    
    Trace(tagSocket, levInfo, ("CSocket :: Linger %s:%d.", m_PeerHostName.GetBuffer(), m_PeerPort));    
    
    /* linger on close if unsent data is present */
    struct linger l_Linger;
    l_Linger.l_onoff  = 1;
    l_Linger.l_linger = m_Timeout;
    
    if (setsockopt(
            m_PeerSocket, 
            SOL_SOCKET, 
            SO_LINGER, 
            (char *) & l_Linger, 
            sizeof(l_Linger)) == 0) {
        return true;
    }
    
    Trace(tagSocket, levError, ("CSocket :: Linger %s:%d failed.", m_PeerHostName.GetBuffer(), m_PeerPort));    

    return false;
}

bool CSocket::DisableNagle(void) {
  
    if (m_PeerSocket < 0) 
        return false;
    
    Trace(tagSocket, levInfo, ("CSocket :: DisableNagle %s:%d.", m_PeerHostName.GetBuffer(), m_PeerPort));    
  
    // (from Apache Server)
    // The Nagle algorithm says that we should delay sending partial
    // packets in hopes of getting more data.  We don't want to do
    // this; we are not telnet.  There are bad interactions between
    // persistent connections and Nagle's algorithm that have very severe
    // performance penalties.  (Failing to disable Nagle is not much of a
    // problem with simple HTTP.)
    // In spite of these problems, failure here is not a shooting offense.
    
    int nDisable = 1;
    
    if (setsockopt(
            m_PeerSocket, 
            IPPROTO_TCP, 
            TCP_NODELAY, 
            (char *) & nDisable, 
            sizeof(int)) == 0) {
        return true;
    }
    
    Trace(tagSocket, levError, ("CSocket :: DisableNagle %s:%d failed.", m_PeerHostName.GetBuffer(), m_PeerPort));    

    return false;
}
    
int CSocket::Write(const CString& String) const {
    
    Trace(tagSocket, levVerbose, ("CSocket :: Write %s:%d {%d bytes}.", m_PeerHostName.GetBuffer(), m_PeerPort, String.GetLength()));    

    if (m_PeerSocket < 0) 
      return 0;
    return base_send(m_PeerSocket, (char *) String.GetBuffer(), String.GetLength(), 0);
}

int CSocket::WriteLine(const CString& String) const {
	if (m_PeerSocket < 0) 
        return 0;
    
	if (!String.EndsWith(g_strCrLf))
        return Write(String + g_strCrLf);    
	else return Write(String);
}

#define DEFAULT_SEG 1024

int CSocket::ReadLine(CString * pResult) const {
    pResult->Empty();        
    
    if (m_PeerSocket < 0)
        return 0;
    
    fd_set fdset;
    struct timeval tv;
    tv.tv_sec=m_Timeout; tv.tv_usec=0;
    
    int BytesRead;
#ifdef __TLI_SUPPORTED
    int Flags;
#endif
    // time_t tmTime; time(&tmTime);
    while(1) { 	
      int sPos;		
      if ((sPos = m_PendingBuffer.Pos((char) 10)) != -1) {
	m_PendingBuffer.Mid(0, sPos, pResult);
	m_PendingBuffer.Delete(0, sPos+1);
	pResult->Trim32();
	Trace(tagSocket, levInfo, ("CSocket :: ReadLine %s:%d {%d bytes} (from pending chunk)\n\t[%s].", m_PeerHostName.GetBuffer(), m_PeerPort, pResult->GetLength(), pResult->GetBuffer()));    
	return pResult->GetLength();
      } else {			
	FD_ZERO(&fdset);
	FD_SET((unsigned int) m_PeerSocket,&fdset);			
	if (base_select(m_PeerSocket + 1, &fdset, NULL, NULL, &tv) > 0) {
	  if (FD_ISSET(m_PeerSocket,&fdset)) {
	    m_PendingBuffer.SetSize(m_PendingBuffer.GetLength() + DEFAULT_SEG);
#ifdef __TLI_SUPPORTED
	    BytesRead = base_recv(m_PeerSocket,((char *) m_PendingBuffer.GetBuffer()) + m_PendingBuffer.GetLength(), DEFAULT_SEG, &Flags);
#else
	    BytesRead = base_recv(m_PeerSocket,((char *) m_PendingBuffer.GetBuffer()) + m_PendingBuffer.GetLength(), DEFAULT_SEG, 0);
#endif
	    Trace(tagSocket, levInfo, ("CSocket :: ReadLine chunk of %d bytes.", BytesRead));
	    
	    if (BytesRead > 0) {
	      m_PendingBuffer.SetLength(m_PendingBuffer.GetLength() + BytesRead, 0, false);
	    } else if (BytesRead == 0) {
	      // time_t tmNow; time(&tmNow);
	      // if (difftime(tmNow, tmTime) > m_Timeout) 
	      break;
	    } else {                        
	      break;
	    }
	  } else break;
	} else break;
      }
    }
    
    Trace(tagSocket, levInfo, ("CSocket :: ReadLine pending chunk of %d bytes.", m_PendingBuffer.GetLength()));
    * pResult = m_PendingBuffer;
    Trace(tagSocket, levInfo, ("CSocket :: ReadLine %s:%d {%d bytes}\n\t[%s].", m_PeerHostName.GetBuffer(), m_PeerPort, pResult->GetLength(), pResult->GetBuffer()));    
    m_PendingBuffer.Empty();
    return pResult->GetLength();	
}

int CSocket::Read(CString * pResult, const int MaxLength) const {		
        pResult->Empty();        
	if (m_PeerSocket < 0) 
          return 0;
	if (MaxLength > 0)
          pResult->SetSize(BASE_MIN((int)m_PendingBuffer.GetLength(), MaxLength));
	* pResult = m_PendingBuffer;
	m_PendingBuffer.Empty();	
	m_Progress.Init(m_Verbose);
        // time_t tmTime; time(&tmTime);
	if ((MaxLength < 0)||((int) pResult->GetLength() < MaxLength)) {
		
		char Segment[DEFAULT_SEG];	
		int BytesRead = 0;
		
		fd_set fdset;
#ifdef __TLI_SUPPORTED
		int Flags
#endif
		struct timeval tv;
		FD_ZERO(&fdset);
		FD_SET((unsigned int) m_PeerSocket,&fdset);
		tv.tv_sec=m_Timeout; tv.tv_usec=0;	
		while (base_select(m_PeerSocket + 1, &fdset, NULL, NULL, &tv) > 0) {		
			if (FD_ISSET(m_PeerSocket,&fdset)) {
#ifdef __TLI_SUPPORTED
                BytesRead = base_recv(m_PeerSocket,Segment,DEFAULT_SEG, &Flags);
#else
                BytesRead = base_recv(m_PeerSocket,Segment,DEFAULT_SEG, 0);
#endif
                if (BytesRead > 0) {				
                    pResult->Append(Segment, BytesRead);
                    m_Progress.Show(pResult->GetLength(), MaxLength, m_Verbose);
                    if ((MaxLength > 0)&&((int) pResult->GetLength() >= MaxLength)) break;
                } else if (BytesRead == 0) {
                    // time_t tmNow; time(&tmNow);
                    // if (difftime(tmNow, tmTime) > m_Timeout) 
                    break;
                } else{                          
                    break;
                }
			} else break;
			FD_ZERO(&fdset);
			FD_SET((unsigned int) m_PeerSocket, &fdset);	  
		}
	} // enough bytes in m_PendingBuffer
	m_Progress.Finish(m_Verbose);
	Trace(tagSocket, levInfo, ("CSocket :: Read %s:%d {%d bytes}.", m_PeerHostName.GetBuffer(), m_PeerPort, pResult->GetLength()));    
    return pResult->GetLength();
}

bool CSocket::IsPeerLocalHost(void) const { 
  return (m_PeerAddrIn.sin_addr.s_addr == inet_addr(g_strHostLocalAddress));
}

void CSocket::LingeringClose(void) {

    Trace(tagSocket, levInfo, ("CSocket :: LingeringClose %s:%d - entering.", m_PeerHostName.GetBuffer(), m_PeerPort));    

    //
    // this is roughly Apache's implementation
    //
    
#define SOL_DUMMY_BUFFER_SIZE 512
#define SOL_LINGER_TIMEOUT 2
    
    char pszDummyBuffer[SOL_DUMMY_BUFFER_SIZE];
    struct timeval tv;
    fd_set lfds;
    int select_rv;    

    CInterval SlowClientInterval;

    //
    // all data has been sent
    //
    
    //
    // close our half of the connection - send the client a FIN
    //
    
    if (shutdown(m_PeerSocket, 0x01) == 0) {

        Trace(tagSocket, levInfo, ("CSocket :: LingeringClose %s:%d - setting up wait.", m_PeerHostName.GetBuffer(), m_PeerPort));    

        //
        // set up to wait for readable data on socket
        //
        
        FD_ZERO(&lfds);
        
        //
        // wait for readable data or error condition on socket;
        // slurp up any data that arrives - exit when we go for an
        // interval of tv length without getting any more data, get an error
        // from select(), get an error or EOF on a read, or the timer expires.
        //
        
        do {
            
            //
            // we use a SOL_LINGER_TIMEOUT second timeout because current (Feb 97) browsers
            // fail to close a connection after the server closes it.  Thus,
            // to avoid keeping the child busy, we are only lingering long enough
            // for a client that is actively sending data on a connection.
            // This should be sufficient unless the connection is massively
            // losing packets, in which case we might have missed the RST anyway.
            // These parameters are reset on each pass, since they might be
            // changed by select.
            //
            
            FD_SET((unsigned int) m_PeerSocket, &lfds);
            tv.tv_sec = SOL_LINGER_TIMEOUT;
            tv.tv_usec = 0;
            
            select_rv = base_select(m_PeerSocket + 1, &lfds, NULL, NULL, &tv);

            if (SlowClientInterval.Get(itSeconds) >= SOL_LINGER_TIMEOUT)
                break;

            Trace(tagSocket, levInfo, ("CSocket :: LingeringClose %s:%d - lingering period.", m_PeerHostName.GetBuffer(), m_PeerPort));    
            
        } while ((select_rv > 0) && 
            (base_read(m_PeerSocket, pszDummyBuffer, sizeof(pszDummyBuffer)) > 0));
        
        Trace(tagSocket, levInfo, ("CSocket :: LingeringClose %s:%d - lingering closure.", m_PeerHostName.GetBuffer(), m_PeerPort));    

        //
        // should now have seen final ack
        // safe to finally kill socket
        //
    }
    
    Trace(tagSocket, levInfo, ("CSocket :: LingeringClose %s:%d - closing socket.", m_PeerHostName.GetBuffer(), m_PeerPort));    

#ifdef _UNIX
#ifdef __TLI_SUPPORTED
    if (m_TliCallPtr) {
        t_free((char *) m_TliCallPtr, T_CALL);			
        m_TliCallPtr = NULL;
    }
    t_close(m_PeerSocket);
#else
    base_close(m_PeerSocket);
#endif // __TLI_SUPPORTED
#endif
#ifdef _WIN32
    closesocket(m_PeerSocket);
#endif

    Trace(tagSocket, levInfo, ("CSocket :: LingeringClose %s:%d - done.", m_PeerHostName.GetBuffer(), m_PeerPort));    

    m_PendingBuffer.Empty();	
    m_PeerSocket = -1;
}

//
// A TCP window the amount of outstanding (unacknowledged by the recipient) data a sender can send on a particular
// connection before it gets an acknowledgment back from the receiver that it has gotten some of it.
// For example if a pair of hosts are talking over a TCP connection that has a TCP window size of 64 KB (kilobytes), 
// the sender can only send 64 KB of data and then it must stop and wait for an acknowledgment from the receiver that 
// some or all of the data has been received. If the receiver acknowledges that all the data has been received then
// the sender is free to send another 64 KB. If the sender gets back an acknowledgment from the receiver that it received
// the first 32 KB (which could happen if the second 32 KB was still in transit or it could happen if the second 32 KB got lost),
// then the sender could only send another 32 KB since it can't have more than 64 KB of unacknowledged data outstanding 
// (the second 32 KB of data plus the third).
//
// The primary reason for the window is congestion control. The whole network connection, which consists of the hosts
// at both ends, the routers in between and the actual connections themselves (be they fiber, copper, satellite or whatever) 
// will have a bottleneck somewhere that can only handle data so fast. Unless the bottleneck is the sending speed of the
// transmitting host, then if the transmitting occurs too fast the bottleneck will be surpassed resulting in lost data. 
// The TCP window throttles the transmission speed down to a level where congestion and data loss do not occur.
// 
// For directions on how to set it up as a default for a host and a discussion of other factors important to high-speed
// networking, see Jamshid Mahdavi's Enabling High Performance Data Transfers on Hosts 
//
// For most operating systems (known exceptions are listed below) the window size is done by setting the socket send and
// receive buffer sizes. 
// The only real trick here is that to be sure of the two ends correctly negotiating the correct window size you must
// set the buffer sizes before making the connection. 
// 
// The reason for this is that there are only 16 bits reserved for the window size in the TCP header, which only allows
// for window sizes up to 64 kilobytes. To work around this limitation a special option, called the TCP window scale option, 
// was introduced. This option is negotiated at the opening of the connection, so if the a window size of greater than 64 KB
// is to be established it must be done at connection set-up time. 
//

bool CSocket::SetSendTcpWindowSize(int nSize) {

    if (m_PeerSocket < 0) 
        return false;

    if (setsockopt(
            m_PeerSocket, 
            SOL_SOCKET, 
            SO_SNDBUF, 
            (char *) & nSize, 
            sizeof(int)) == 0) {
    
        Trace(tagSocket, levError, ("CSocket :: SetSendTcpWindowSize %s:%d (%d bytes) succeeded.", m_PeerHostName.GetBuffer(), m_PeerPort, nSize));    
        
        return true;
    }
    
    Trace(tagSocket, levError, ("CSocket :: SetSendTcpWindowSize %s:%d (%d bytes) failed.", m_PeerHostName.GetBuffer(), m_PeerPort, nSize));    
    
    return false;    
}

bool CSocket::SetRecvTcpWindowSize(int nSize) {

    if (m_PeerSocket < 0) 
        return false;

    if (setsockopt(
            m_PeerSocket, 
            SOL_SOCKET, 
            SO_RCVBUF, 
            (char *) & nSize, 
            sizeof(int)) == 0) {
    
        Trace(tagSocket, levError, ("CSocket :: SetRecvTcpWindowSize %s:%d (%d bytes) succeeded.", m_PeerHostName.GetBuffer(), m_PeerPort, nSize));    
        
        return true;
    }
    
    Trace(tagSocket, levError, ("CSocket :: SetRecvTcpWindowSize %s:%d (%d bytes) failed.", m_PeerHostName.GetBuffer(), m_PeerPort, nSize));    
    
    return false;    
}
