/*
    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1994-1999 All Rights Reserved
    ______________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com

*/

#include <baseclasses.hpp>

#include "Handler.hpp"
#include <Object/Tracer.hpp>
#include <File/LocalFile.hpp>
#include <File/LocalPath.hpp>
#include <Virtual/FileMapping.hpp>
#include <Socket/Socket.hpp>
#include <Socket/Dns.hpp>

CHandler * g_pHandler = NULL;

/*
  Initialize all static stuff
  # memory exhaustion handler
  # WSA startup functions (Winsock)
*/

CHandler::CHandler(void) :
    m_Initialized(false),
    m_SignalSigterm(false) 
{
    Initialize();
}

CHandler::~CHandler(void) {
    if (m_Initialized) {
      base_set_new_handler(0);
      RemoveTmpFiles();
#ifdef _WIN32
      WSACleanup();
#endif    
    }
}

#ifdef _WIN32
int CHandler::OutOfMemory(size_t) {
	cerr << "Runtime Error::Not enough memory." << endl;	
    Terminate(-1);
    return 0;
}
#endif

#ifdef _UNIX
void CHandler::OutOfMemory(void) {
	cerr << "Runtime Error::Not enough memory." << endl;
    Terminate(-1);
}
#endif

void CHandler::RemoveTmpFiles(void) {
#ifdef _UNIX
    CString TmpPrefix = CLocalFile::GetTmpPrefix();
    CLocalPath LocalPath("/tmp/");
    CVector<CString> TmpContents = LocalPath.GetPathContents(false);
    for (register int i=0;i< (int) TmpContents.GetSize();i++) {
        if (TmpContents[i].StartsWith(TmpPrefix)) {
            base_unlink(TmpContents[i].GetBuffer());
        }
    }
#endif
}

void CHandler::SignalSigterm(int) {
    if (!g_pHandler->GetSignalSigterm()) {
        g_pHandler->SetSignalSigterm(true);
    	Trace(tagSystem, levVerbose, ("CHandler :: SignalSigterm - caught Ctrl-C, pid=%d.", base_getpid()));
        base_sleep(2);
        Terminate(-2);
    }
}

#ifdef _WIN32
void CHandler::ShowLastError(void) {
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR) &lpMsgBuf,
		0,
		NULL);
	cout << (LPCTSTR)lpMsgBuf << endl;
	LocalFree(lpMsgBuf);
}
#endif

void CHandler::Initialize(void) {
  if (!m_Initialized) {
    g_pHandler = this;
	m_Initialized = true;
    
#ifdef _UNIX
    struct sigaction action;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
	action.sa_handler = SignalSigterm;
	sigaction(SIGTERM, &action, NULL);
    sigaction(SIGINT, &action, NULL);
        
    sigset_t set;
    sigemptyset(&set);
	sigaddset(&set, SIGALRM);
    sigaddset(&set, SIGHUP);
    sigaddset(&set, SIGCHLD);
    sigaddset(&set, SIGABRT);
    sigaddset(&set, SIGSEGV);
    sigprocmask(SIG_BLOCK, &set, NULL);    
#endif
#ifdef _WIN32
    signal(SIGTERM, &SignalSigterm);
    signal(SIGINT, &SignalSigterm);
    signal(SIGABRT, SIG_IGN);
    signal(SIGSEGV, SIG_IGN);
#endif
    
	Trace(tagSystem, levVerbose, ("CHandler :: Initialize - setting out-of-memory handler."));
	base_set_new_handler(OutOfMemory);  
#ifdef BASE_TRACE_ENABLED
    memset(s_TraceLevels, 0, MAX_TRACE * sizeof(bool));
    memset(s_TraceTags, 0, MAX_TRACE * sizeof(bool));
#endif
#ifdef _WIN32
 	WSADATA W;
 	WSAStartup(0x101,&W);
#endif
  }
}

void GlobalInitialize(bool bEnableSwap, char * pFilename) {
    CFileMapping::InitializeSwap();

    if (bEnableSwap) {        
        ((CFileMapping *) CVirtualMemory :: m_pSwap)->SetEnabled(true, pFilename);
    }

	g_pHandler = ::new CHandler;

    CSocket :: m_pDNService = new CDomainNameService;
}

void GlobalTerminate(void) {
    delete CSocket :: m_pDNService;
	delete g_pHandler;    
    CFileMapping::UnInitializeSwap();
}

void CHandler :: Terminate(int nResult) {
    ((CFileMapping *) CVirtualMemory :: m_pSwap)->SetState(vsTerminating);    
    CFileMapping::UnInitializeSwap();    
    base_exit(nResult);
}
