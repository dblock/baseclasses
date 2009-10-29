/*
    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1994-1999 All Rights Reserved
    ______________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com

    Revision history:

    16.09.1999: accept() - argument 3 changes signess (gcc 2.95.1)
    02.03.2000: SIGPIPE blocked through sigmask (Xo3 bugfix)
    07.05.2000: thread pool / AcceptError crashed thread
*/

#include <baseclasses.hpp>
#include <Object/Tracer.hpp>
#include "Server.hpp"
 
void CServer::ExecuteClient(const CConnection&) {
    
}

CServer::~CServer(void) {
    UnBind();
}

static void ServerCleanupThreadFunction(void * ConnectionPointer) {
    if (ConnectionPointer)
        delete (CConnection *) ConnectionPointer;
}   

static void ServerExecuteThreadFunction(void * ConnectionPointer) {
    Trace(tagServer, levInfo, ("CServer::ServerExecuteThreadFunction - new connection %#x.", (long) ConnectionPointer));
    ((CConnection *) ConnectionPointer)->Execute(NULL);
    delete (CConnection *) ConnectionPointer;
    Trace(tagServer, levInfo, ("CServer::ServerExecuteThreadFunction - done connection %#x.", (long) ConnectionPointer));
}

void CServer::ExecuteRemoteClient(void) {
    struct sockaddr_in l_PeerAddrIn;
    memset((char *)&l_PeerAddrIn, 0, sizeof(struct sockaddr_in));       
    size_t l_AddrLen = sizeof(struct sockaddr_in);       		
    Trace(tagServer, levInfo, ("CServer::ExecuteRemoteClient - accepting connections."));
    int l_ClientSocket = base_accept(m_ListenSocket, (sockaddr *)&l_PeerAddrIn, __ACCEPT_ADDR_TYPECAST &l_AddrLen);
    if (l_ClientSocket == -1) {
#ifdef _WIN32
        int nError = WSAGetLastError();
#endif
#ifdef _UNIX
        int nError = errno;
#endif        
        Trace(tagServer, levError, ("CServer::ExecuteRemoteClient - accept failed, error: %#x.", nError));
        switch(nError) {
#ifdef _UNIX
        case EMFILE:
        case ENFILE:
        case ENOMEM:
        case ENOBUFS:
       #ifdef ENOSR
        case ENOSR:
       #endif
#endif
#ifdef _WIN32
        case WSAENETDOWN:
        case WSAEMFILE:
        case WSAENOBUFS:      
#endif      
            Restart();
            return;
        default:
            break;
        }
        return;
    }
    
    CConnection * Incoming = ::new CConnection();
    Incoming->SetServer(this);
    Incoming->SetTimeout(10);
    Incoming->SetPeerSocket(l_ClientSocket);
    Incoming->SetPeerAddrIn(l_PeerAddrIn);
    Incoming->GetSocket().SetBlockMode(true);
        
    CThreadPoolJob ThreadPoolJob(
        (void *) ServerExecuteThreadFunction, 
        (void *) Incoming, 
        (void *) ServerCleanupThreadFunction);
    
    if (! m_ThreadPool.AddJob(ThreadPoolJob)) {
        Trace(tagServer, levError, ("CServer::ExecuteRemoteClient - unable to add job to queue."));
        // no more space in queue
        Incoming->WriteLine("Server is busy.");
        Incoming->Close();
        delete Incoming;
    }
    
    if (m_AcceptInterval)
        base_sleep_ms(m_AcceptInterval);    
}

#define CSDoSINTERVAL 7500
 
void CServer::Restart(void) {
    Trace(tagServer, levInfo, ("CServer::Restart - unbinding."));

    UnBind();
    if (m_RestartInterval.Get() < CSDoSINTERVAL / 5) {
        Trace(tagServer, levInfo, ("CServer::Restart - DoS L3."));            
        _exit(0);
    } else if (m_RestartInterval.Get() < CSDoSINTERVAL) {
        Trace(tagServer, levInfo, ("CServer::Restart - DoS L2."));    
        m_ThreadPool.PassiveWait(true, CSDoSINTERVAL * 3);
    } else {
        Trace(tagServer, levInfo, ("CServer::Restart - DoS L1."));            
        m_ThreadPool.PassiveWait(false, CSDoSINTERVAL);
    }
    
    Trace(tagServer, levInfo, ("CServer::Restart - DoS Binding."));
    Bind();
}

void CServer::Execute(void * Arguments) {
    Bind();
    while (m_Bound) {
        ExecuteRemoteClient();        
    }
    CThread::Execute(Arguments);
}

CServer::CServer(unsigned short Port, const CString& Address) : 
    m_Port(ntohs(Port)),
    m_Bound(false),
    m_AcceptInterval(0),
#ifdef _UNIX
    m_ForkedParentPid(0),
#endif
    m_Nagle(true) // leave nagle to whatever system defaults
{
    // bind to a different address (multiple network cards)
    if (Address.GetLength()) {
        CSocket Socket(Port, Address);
        Socket.Open();
        m_Addr = Socket.GetPeerAddrIn().sin_addr.s_addr;
    } else {
        m_Addr = INADDR_ANY;
    }
    m_Binding = true;
}

bool CServer::CreateSocket(void) {
    Trace(tagServer, levInfo, ("CServer::CreateSocket ..."));
	memset ((char *)&m_ServerAddrIn, 0, sizeof(struct sockaddr_in));
	m_ListenSocket = socket (AF_INET, SOCK_STREAM, 0);
	if (m_ListenSocket == -1) {
        Trace(tagServer, levError, ("CServer::CreateSocket - failed."));
		// raise some error
		return false;
	} else {
    
        int window_size = 128 * 1024;	/* 128 kilobytes */
        setsockopt(m_ListenSocket, SOL_SOCKET, SO_SNDBUF, (char *) &window_size, sizeof(window_size));
        setsockopt(m_ListenSocket, SOL_SOCKET, SO_RCVBUF, (char *) &window_size, sizeof(window_size));
	    
		m_ServerAddrIn.sin_family = AF_INET;
		m_ServerAddrIn.sin_addr.s_addr = m_Addr;
		m_ServerAddrIn.sin_port = m_Port;
		int OptVal = 1;
		setsockopt(m_ListenSocket, SOL_SOCKET, SO_REUSEADDR, (char *) & OptVal, sizeof(int));
		return true;
	}
}

bool CServer::BindSocket(void) {
    Trace(tagServer, levInfo, ("CServer::BindSocket ..."));
	if (bind(m_ListenSocket, (sockaddr *) &m_ServerAddrIn, sizeof(struct sockaddr_in)) != 0) {
        Trace(tagServer, levError, ("CServer::BindSocket - failed."));
		// raise some error
		return false;
	} else return true;
}

bool CServer::ListenSocket(void) {
    Trace(tagServer, levInfo, ("CServer::ListenSocket ..."));
	if (listen(m_ListenSocket, 5) == -1) {
        Trace(tagServer, levError, ("CServer::ListenSocket - failed."));            
		return false;
	} else return true;
}

void CServer::Bind(void) {
    Trace(tagServer, levInfo, ("CServer::Bind ..."));
    if (!m_Bound) {
        m_RestartInterval.Reset();
        if (CreateSocket()) {
            if (BindSocket()) {
                if (ListenSocket()) {
                    m_Bound = true;
                }
            }
        }
    }
    m_Binding = false;
}

void CServer::UnBind(void) {
    Trace(tagServer, levInfo, ("CServer::UnBind ..."));
    if (m_ListenSocket >= 0) {
        shutdown(m_ListenSocket, 1);
#ifdef _UNIX
        base_close(m_ListenSocket);
#endif
#ifdef _WIN32
        closesocket(m_ListenSocket);
#endif
        m_Bound = false;
    }
}

bool CServer::GetBindAddressPort(const CString& Pair, CString * pAddress, CString * pPort) {
    int baPos = Pair.Pos(':');
    if (baPos != -1) {
        Pair.Mid(0, baPos, pAddress);
        Pair.Mid(baPos + 1, Pair.GetLength(), pPort);
    } else {
        * pPort = Pair;
    }
    return pPort->IsInt(); 
}

void CServer::Shutdown(void) {    
    Trace(tagServer, levInfo, ("CServer::Shutdown ..."));
#ifdef _UNIX
    if (m_ForkedParentPid) {
        kill(m_ForkedParentPid, SIGTERM);
    }
    kill(0, SIGTERM);
#endif
#ifdef _WIN32
    CHandler :: Terminate(0);
#endif
}

#ifdef _UNIX
bool CServer::DaemonForkedProcess(void) {
    Trace(tagServer, levInfo, ("CServer::DaemonForkedProcess ..."));
    // this comes from the Apache web server (dblock)
#ifdef HAVE_SETSID
    if (setsid() == -1) {
        perror("setsid");
        fprintf(stderr, "[setsid failed]\n");
        return false;
    }
#elif defined(HAVE_SETPGRP_2) && defined(HAVE_GETPGRP_1)
    if ((setpgrp(0, getpid()) == -1) || (getpgrp(0) == -1)) {
        perror("setpgrp");
        fprintf(stderr, "[setpgrp or getpgrp failed]\n");
        return false;
    }
#elif defined(HAVE_SETPGRP) && defined(HAVE_GETPGRP)
    if ((setpgrp() == -1) || (getpgrp() == -1)) {
        perror("setpgrp");
        fprintf(stderr, "[setpgrp or getpgrp failed]\n");
        return false;
    }
#endif    
    
    // close out the standard file descriptors
    if (freopen("/dev/null", "r", stdin) == NULL) {
        fprintf(stderr, "[unable to replace stdin with /dev/null: %s]\n", strerror(errno));
        
        // continue anyhow -- note we can't close out descriptor 0 because we
        // have nothing to replace it with, and if we didn't have a descriptor
        // 0 the next file would be created with that value        
    }
    
    if (freopen("/dev/null", "w", stdout) == NULL) {
        fprintf(stderr, "[unable to replace stdout with /dev/null: %s]\n", strerror(errno));
    }
    
    // $(TODO) stderr should be the error log
    if (freopen("/dev/null", "w", stderr) == NULL) {
        fprintf(stderr, "[unable to replace stderr with /dev/null: %s]\n", strerror(errno));
    }
    
    return true;
}
#endif
