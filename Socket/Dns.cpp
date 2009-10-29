#include <baseclasses.hpp>
#include <Socket/Dns.hpp>
#include <Object/Tracer.hpp>
#include <Date/Interval.hpp>

#define DNS_INTERVAL_RESTART 60 * 10 // re-resolve every 10 minutes
#define DNS_INTERVAL_HUNG 60 * 60 // consider a thread hung after 1 hour

static void DnsPeerExecuteFunction(void * DnsPeerPointer) {    
    ((CDnsPeer *) DnsPeerPointer)->Resolve();
    ((CDnsPeer *) DnsPeerPointer)->DecRef();
}

CDnsPeer :: CDnsPeer(void) {
    m_Started = false;
    m_Resolved = false;
    m_Done = false;
    m_DomainNameService = NULL;
    m_RefCount = 1;
    memset(& m_Peer, 0, sizeof(sockaddr_in));
}

CDnsPeer :: ~CDnsPeer(void) {
    assert(m_RefCount.Get() == 0);
    Trace(tagDns, levInfo, ("CDnsPeer :: ~CDnsPeer - disposing record for %s.", m_PeerHostname.GetBuffer()));    
}
    
void CDnsPeer :: Resolve(void) {
    assert(m_DomainNameService);

    m_Resolved = m_DomainNameService->GetHostname(
        m_PeerHostname,
        m_Peer);
    
    m_Done = true;	
}

bool CDomainNameService :: WalkHostent(
    struct hostent * hp,
    CVector<CString>& PeerAddresses, 
    CVector<CString>& PeerAliases) {
    
    Trace(tagDns, levInfo, ("CDnsPeer :: WalkHostEnt - entering."));    
    
    if (! hp)
        return false;
    
    for (char ** l_P = hp->h_addr_list; * l_P != 0; l_P++) {
        struct in_addr l_IN;
        memcpy(&l_IN.s_addr, *l_P, sizeof (l_IN.s_addr));
        PeerAddresses.Add(inet_ntoa(l_IN));
    }
    
    PeerAliases.Add(hp->h_name);
    for (char ** l_Q = hp->h_aliases; * l_Q != 0; l_Q++) {
        PeerAliases.Add((char *) * l_Q);
    }
    
    Trace(tagDns, levInfo, ("CDnsPeer :: WalkHostEnt - leaving with %d addresses and %d aliases.", PeerAddresses.GetSize(), PeerAliases.GetSize()));    

    return true;
}        

#ifdef HAVE_GETHOSTBYNAME_R6
bool CDomainNameService::CreatePeer(
    unsigned long RemoteAddress, 
    CVector<CString>& PeerAddresses, 
    CVector<CString>& PeerAliases
    ) {

    Trace(tagDns, levInfo, ("CDns :: CreatePeer(R6)"));
    
    bool bResult = false;
    struct hostent hostbuf;
    struct hostent * hp = NULL;
	int herr = 0;
    int res = 0;
    size_t hstbuflen = 1024;
    char * tmphstbuf = new char[hstbuflen];
    memset(tmphstbuf, 0, hstbuflen);

	while (( res = 
             gethostbyaddr_r (
                 (const char *) RemoteAddress,
                 sizeof(RemoteAddress),
                 AF_INET,
                 & hostbuf,
                 tmphstbuf,
                 hstbuflen,                 
                 & hp,
                 & herr))
           && (herr == ERANGE))
	{

        Trace(tagDns, levInfo, ("CDns :: CreatePeer(R6) - ERANGE, current buffer size is %d bytes.", hstbuflen));
    
		hstbuflen *= 2;
        delete[] tmphstbuf;
		tmphstbuf = (char *) new char[hstbuflen];
        memset(tmphstbuf, 0, hstbuflen);
	}

    Trace(tagDns, levInfo, ("CDns :: CreatePeer(R6) - gethostbyaddr_r done, returned %d.", res));
    
	if (! res) {        
        bResult = WalkHostent(hp, PeerAddresses, PeerAliases);
    }
    
    delete[] tmphstbuf;

    Trace(tagDns, levInfo, ("CDns :: CreatePeer(R6) - returning %s", bResult?"true":"false"));
    
    return bResult;
}

bool CDomainNameService::GetHostname(
    const CString& PeerHostname,
    sockaddr_in& Peer,
    CVector<CString> * PeerAddresses,
    CVector<CString> * PeerAliases
    ) {

    Trace(tagDns, levInfo, ("CDns :: GetHostname(R6) - %s", PeerHostname.GetBuffer()));
    
    bool bResult = false;
    struct hostent hostbuf;
    struct hostent * hp = NULL;
	int herr = 0;
    int res = 0;
    size_t hstbuflen = 1024;
    char * tmphstbuf = new char[hstbuflen];
    memset(tmphstbuf, 0, hstbuflen);
        
	while (( res = 
             gethostbyname_r(
                 (const char *) PeerHostname.GetBuffer(),
                 & hostbuf, 
                 tmphstbuf,
                 hstbuflen,
                 & hp,
                 & herr))
           && (herr == ERANGE))
	{		

        Trace(tagDns, levInfo, ("CDns :: GetHostname(R6) - E_RANGE - %s", PeerHostname.GetBuffer()));
    
		hstbuflen *= 2;
        delete[] tmphstbuf;
		tmphstbuf = (char *) new char[hstbuflen];
        memset(tmphstbuf, 0, hstbuflen);
	}

	if (! res) {
        Peer.sin_addr.s_addr = * ((unsigned long *) hp->h_addr);
        bResult = true;
    
        if (PeerAddresses && PeerAliases) {
            bResult |= WalkHostent(hp, * PeerAddresses, * PeerAliases);
        }
    }
    
    delete[] tmphstbuf;
    
    Trace(tagDns, levInfo, ("CDns :: GetHostname(R6) - %s %s", PeerHostname.GetBuffer(), bResult?"(resolved)":"(unresolved)"));
    
    return bResult;    
}
#endif

#ifdef HAVE_GETHOSTBYNAME_R5
bool CDomainNameService::CreatePeer(
    unsigned long RemoteAddress, 
    CVector<CString>& PeerAddresses, 
    CVector<CString>& PeerAliases
    ) {

    Trace(tagDns, levInfo, ("CDns :: CreatePeer(R5)"));
    
    bool bResult = false;
    struct hostent hostbuf;
    struct hostent * hp = NULL;
	int herr = 0;
    size_t hstbuflen = 1024;
    char * tmphstbuf = new char[hstbuflen];
    memset(tmphstbuf, 0, hstbuflen);
        
	while ((NULL == ( hp = 
             gethostbyaddr_r(
                 (const char *) RemoteAddress,
                 sizeof(RemoteAddress),
                 AF_INET,
                 & hostbuf, 
                 tmphstbuf,
                 hstbuflen,
                 & herr)))
           && (herr == ERANGE))
	{
		hstbuflen *= 2;
        delete[] tmphstbuf;
		tmphstbuf = (char *) new char[hstbuflen];
        memset(tmphstbuf, 0, hstbuflen);
	}
    
    bResult = WalkHostent(hp, PeerAddresses, PeerAliases);
    
    delete[] tmphstbuf;
    
    Trace(tagDns, levInfo, ("CDns :: CreatePeer(R5) - returning %s", bResult?"true":"false"));
    
    return bResult;    
}

bool CDomainNameService::GetHostname(
    const CString& PeerHostname,
    sockaddr_in& Peer,
    CVector<CString> * PeerAddresses,
    CVector<CString> * PeerAliases
    ) {

    Trace(tagDns, levInfo, ("CDns :: GetHostname(R5) - %s", PeerHostname.GetBuffer()));
    
    bool bResult = false;
    struct hostent hostbuf;    
    struct hostent * hp = NULL;
	int herr = 0;
    size_t hstbuflen = 1024;
    char * tmphstbuf = new char[hstbuflen];
    memset(tmphstbuf, 0, hstbuflen);
        
	while ((NULL == ( hp = 
             gethostbyname_r(
                 (const char *) PeerHostname.GetBuffer(),
                 & hostbuf, 
                 tmphstbuf,
                 hstbuflen,
                 & herr)))
           && (herr == ERANGE))
	{
        
        Trace(tagDns, levInfo, ("CDns :: GetHostname(R5) - E_RANGE - %s", PeerHostname.GetBuffer()));
    
		hstbuflen *= 2;
        delete[] tmphstbuf;
		tmphstbuf = (char *) new char[hstbuflen];
        memset(tmphstbuf, 0, hstbuflen);
	}
    
	if (hp) {
        Peer.sin_addr.s_addr = * ((unsigned long *) hp->h_addr);
        bResult = true;

        if (PeerAddresses && PeerAliases) {
            bResult |= WalkHostent(hp, * PeerAddresses, * PeerAliases);
        }
    }
    
    delete[] tmphstbuf;
    
    Trace(tagDns, levInfo, ("CDns :: GetHostname(R5) - %s - returning %s", PeerHostname.GetBuffer(), bResult?"true":"false"));
    
    return bResult;    
}
    
#endif

#ifdef HAVE_GETHOSTBYNAME_R3
bool CDomainNameService::CreatePeer(
    unsigned long RemoteAddress, 
    CVector<CString>& PeerAddresses, 
    CVector<CString>& PeerAliases
    ) {
    
    Trace(tagDns, levInfo, ("CDns :: CreatePeer(R3)"));
    
    bool bResult = false;
    struct hostent_data * hp;
    struct hostent_data buffer;
    memset(& hp, 0, sizeof(hostent_data));
            
	if (0 == gethostbyaddr_r(
            (const char *) PeerHostname.GetBuffer(),
            sizeof(RemoteAddress),
            AF_INET,
            & hp, 
            & buffer))
	{
        bResult = WalkHostent(hp, PeerAddresses, PeerAliases);
	}    
    
    Trace(tagDns, levInfo, ("CDns :: CreatePeer(R3) - returning %s", bResult?"true":"false"));
    
    return bResult;
    
}

bool CDomainNameService::GetHostname(
    const CString& PeerHostname,
    sockaddr_in& Peer
    CVector<CString> * PeerAddresses,
    CVector<CString> * PeerAliases
    ) {
    
    Trace(tagDns, levInfo, ("CDns :: GetHostname(R3) - %s", PeerHostname.GetBuffer()));
    
    bool bResult = false;
    struct hostent_data * hp;
    struct hostent_data buffer;
    memset(& hp, 0, sizeof(hostent_data));
    
	if (0 == gethostbyname_r(
            (const char *) PeerHostname.GetBuffer(),
            & hp, 
            & buffer))
	{
		Peer.sin_addr.s_addr = * ((unsigned long *) hp->h_addr);
        bResult = true;

        if (PeerAddresses && PeerAliases) {
            bResult |= WalkHostent(hp, * PeerAddresses, * PeerAliases);
        }
    
	}
    
    Trace(tagDns, levInfo, ("CDns :: GetHostname(R3) - %s - returning %s", PeerHostname.GetBuffer(), bResult?"true":"false"));
    
    return bResult;
}
    
#endif
    
#ifdef HAVE_GETHOSTBYNAME
bool CDomainNameService::CreatePeer(
    unsigned long RemoteAddress, 
    CVector<CString>& PeerAddresses, 
    CVector<CString>& PeerAliases
    ) {

    Trace(tagDns, levInfo, ("CDns :: CreatePeer(R)"));
    
    bool bResult = false;
    static CMutex s_GetHostnameMutex;
    
    s_GetHostnameMutex.Lock();
    
    struct hostent * hp = gethostbyaddr((const char *) RemoteAddress, sizeof(RemoteAddress), AF_INET);

    bResult = WalkHostent(hp, PeerAddresses, PeerAliases);

    s_GetHostnameMutex.UnLock();
    
    Trace(tagDns, levInfo, ("CDns :: CreatePeer(R) - returning %s", bResult?"true":"false"));
    
    return bResult;
}

bool CDomainNameService::GetHostname(
    const CString& PeerHostname,
    sockaddr_in& Peer,
    CVector<CString> * PeerAddresses,
    CVector<CString> * PeerAliases
    ) {

    Trace(tagDns, levInfo, ("CDns :: GetHostname(R) - %s", PeerHostname.GetBuffer()));
    
    bool bResult = false;
    static CMutex s_GetHostnameMutex;

    s_GetHostnameMutex.Lock();
    
    struct hostent * hp = gethostbyname((const char *) PeerHostname.GetBuffer());
    
    if (hp)
	{
		Peer.sin_addr.s_addr = * ((unsigned long *) hp->h_addr);
        bResult = true;

        if (PeerAddresses && PeerAliases) {
            bResult |= WalkHostent(hp, * PeerAddresses, * PeerAliases);
        }
        
	} else {
        bResult = false;
    } 
    
    s_GetHostnameMutex.UnLock();
    
    Trace(tagDns, levInfo, ("CDns :: GetHostname(R) - %s - returning %s", PeerHostname.GetBuffer(), bResult?"true":"false"));
    
    return bResult;
}
    
#endif

#ifdef _WIN32
bool CDomainNameService::CreatePeer(
    unsigned long RemoteAddress, 
    CVector<CString>& PeerAddresses, 
    CVector<CString>& PeerAliases
    ) {

    Trace(tagDns, levInfo, ("CDns :: CreatePeer(WIN32)"));
    
    bool bResult = false;
    struct hostent * hp = gethostbyaddr((const char *) RemoteAddress, sizeof(RemoteAddress), AF_INET);       
    
    bResult = WalkHostent(hp, PeerAddresses, PeerAliases);
    
    Trace(tagDns, levInfo, ("CDns :: CreatePeer(WIN32) - returning %s", bResult?"true":"false"));
    
    return bResult;    
}

bool CDomainNameService::GetHostname(
    const CString& PeerHostname,
    sockaddr_in& Peer,
    CVector<CString> * PeerAddresses,
    CVector<CString> * PeerAliases
    ) {

    Trace(tagDns, levInfo, ("CDns :: GetHostname(WIN32) - %s", PeerHostname.GetBuffer()));
    
    bool bResult = false;
    struct hostent * hp = gethostbyname((const char *) PeerHostname.GetBuffer());
    
    if (hp)
	{
		Peer.sin_addr.s_addr = * ((unsigned long *) hp->h_addr);
        bResult = true;
	}
    
    Trace(tagDns, levInfo, ("CDns :: GetHostname(WIN32) - %s - returning %s", PeerHostname.GetBuffer(), bResult?"true":"false"));
    
    if (PeerAddresses && PeerAliases) {
        bResult |= WalkHostent(hp, * PeerAddresses, * PeerAliases);
    }
    
    return bResult;
}
#endif

CDomainNameService::CDomainNameService(void) :
    m_DnsTimeout(30)
{
#ifdef _UNIX
    // select might return pending write 
    // data after a failed async connect which 
    // causes a SIGPIPE   
    signal(SIGPIPE, SIG_IGN);
    signal(SIGALRM, SIG_IGN);
#endif 
#ifdef HAVE_HOSTENT
    sethostent(1);
#endif
}

CDomainNameService::~CDomainNameService(void) {
   
    m_DnsCacheMutex.Lock();

    Trace(tagDns, levInfo, ("CDns :: ~CDns - %d cached elements.", m_DnsCache.GetSize()));    
    
    for (int i = 0; i < (int) m_DnsCache.GetSize(); i++) {
        CDnsPeer * pPeer = m_DnsCache.GetElementAt(i);

        pPeer->m_PeerMutex.Lock();
        if (pPeer->m_RefCount.Get() > 1) {
            Trace(tagDns, levInfo, ("CDns :: ~CDns - %s is pending, thread will be cancelled.", pPeer->m_PeerHostname.GetBuffer()));    
        }
        pPeer->m_PeerMutex.UnLock();
        
        pPeer->DecRef();        
    }
    m_DnsCacheMutex.UnLock();

    // TerminateThreads();    
    
#ifdef HAVE_HOSTENT
    endhostent();
#endif    
}

bool CDomainNameService::CreateHost(
    const CString& PeerHostname, 
    sockaddr_in& Peer,
    int& RStatus) {
    
    Trace(tagDns, levInfo, ("CDns :: CreateHost - %s", PeerHostname.GetBuffer()));
    
#ifdef _WIN32
    Peer.sin_addr.s_addr = inet_addr((const char *) PeerHostname.GetBuffer());

    if (Peer.sin_addr.s_addr != INADDR_NONE) {
        Trace(tagDns, levInfo, ("CDns :: CreateHost - resolved %s with inet_addr", PeerHostname.GetBuffer()));        
        return true;
    }
#endif

#ifdef _UNIX
 #ifdef HAVE_INET_ATON
    // check whether this is a numeric address
    if (inet_aton((const char *) PeerHostname.GetBuffer(), & Peer.sin_addr) != 0) {
        Trace(tagDns, levInfo, ("CDns :: CreateHost - resolved %s with inet_aton", PeerHostname.GetBuffer()));        
        return true;
    }
 #else
    Peer.sin_addr.s_addr = inet_addr((const char *) PeerHostname.GetBuffer());

    if (Peer.sin_addr.s_addr != (unsigned long) -1) {
        Trace(tagDns, levInfo, ("CDns :: CreateHost - resolved %s with inet_addr", PeerHostname.GetBuffer()));        
        return true;
    }    
 #endif
#endif

    //
    // get a new or an existing entry from the dns cache table
    //
    
    CDnsPeer * pDnsPeer = NULL;
    CInterval Interval;
    bool bResult = false;
    
    m_DnsCacheMutex.Lock();

    if (m_DnsCache.FindAndCopy(PeerHostname, pDnsPeer)) {
        Trace(tagDns, levInfo, ("CDns :: CreateHost - %s - found a record in cache.", PeerHostname.GetBuffer()));                    
        pDnsPeer->AddRef();
    }
    
    if (! pDnsPeer) {

        // new objects start with a ref count of one
        
        pDnsPeer = ::new CDnsPeer();
        
        pDnsPeer->SetPeerHostname(PeerHostname);
        pDnsPeer->SetDomainNameService(this);
        pDnsPeer->SetPeer(Peer);
    
        pDnsPeer->AddRef();
        m_DnsCache.Set(PeerHostname, pDnsPeer);
    }
    
    m_DnsCacheMutex.UnLock();
    
    // look at the job
    
    pDnsPeer->GetPeerMutex().Lock();
    
    // if the record is older than an hour and
    // has been completed restart it
    if (pDnsPeer->GetDone() && pDnsPeer->GetStarted()) {    
        if (pDnsPeer->GetTimestamp().Get(itSeconds) > DNS_INTERVAL_RESTART) {
            Trace(tagDns, levInfo, ("CDns :: CreateHost - %s - record %ld seconds old (too old).", PeerHostname.GetBuffer(), pDnsPeer->GetTimestamp().Get(itSeconds)));
            pDnsPeer->SetDone(false);
            pDnsPeer->SetStarted(false);
			pDnsPeer->GetTimestamp().Reset();
        }
    }
    
    // the job is not started, queue it
    
    if (! pDnsPeer->GetDone() && ! pDnsPeer->GetStarted()) {        
        
        Trace(tagDns, levInfo, ("CDns :: CreateHost - adding job to the dns pool for %s.", PeerHostname.GetBuffer()));                
        
        // the job has not been started
        
        pDnsPeer->AddRef(); // as if the job was properly queued    
        pDnsPeer->SetStarted(true);
        
        CThreadPoolJob DnsJob((void *) DnsPeerExecuteFunction, (void *) pDnsPeer);    
        
        if (! AddJob(DnsJob)) {
            Trace(tagDns, levInfo, ("CDns :: CreateHost - cannot add job to pool %s.", PeerHostname.GetBuffer()));                
            pDnsPeer->DecRef(); // as if the job was properly queued        
        }
    }
    
    pDnsPeer->GetPeerMutex().UnLock();
    
    // wait for a started job to complete or to timeout
    
    if (pDnsPeer->GetStarted()) {
    
        while (! pDnsPeer->GetDone() && (Interval.Get(itSeconds) < m_DnsTimeout))
            base_sleep_ms(100);
        
    }
    
    if (! pDnsPeer->GetStarted()) {
        // never started, queue probably too full
        RStatus = 926;
    } else if (! pDnsPeer->GetDone()) {
        // timed out
        Trace(tagDns, levInfo, ("CDns :: CreateHost - %s - timed out at %ld second(s).", PeerHostname.GetBuffer(), Interval.Get(itSeconds)));
		// check whether this background thread is hung
		if (pDnsPeer->GetTimestamp().Get(itSeconds) > DNS_INTERVAL_HUNG) {
			// reset it for the next round and fail for this resolve
            pDnsPeer->SetDone(false);
            pDnsPeer->SetStarted(false);
			pDnsPeer->GetTimestamp().Reset();			
		}
        RStatus = 927;
        bResult = false;
    } else if (pDnsPeer->GetResolved()) {
        // successfuly resolved
        Trace(tagDns, levInfo, ("CDns :: CreateHost - %s - resolved.", PeerHostname.GetBuffer()));
        bResult = true;
        Peer = pDnsPeer->GetPeer();
    } else {
        // simply couldn't be resolved
        RStatus = 923;
    }
    
    pDnsPeer->DecRef(); 
    Trace(tagDns, levInfo, ("CDns :: CreateHost - returning %s as %s.", PeerHostname.GetBuffer(), bResult?"resolved":"unresolved"));
    return bResult;
}

bool CDomainNameService::GetHostname(CString * pLocalHostname) {
    
    const size_t nBufSize = 256;
    
    if (! pLocalHostname)
        return false;
    
    pLocalHostname->SetSize(nBufSize);
    
    if (gethostname((CSTRING_CHARTYPE *) pLocalHostname->GetBuffer(), nBufSize) != -1) {
        pLocalHostname->SetLength();
    } else {
        (* pLocalHostname) = "localhost";
    }

    Trace(tagDns, levInfo, ("CDns :: GetHostname - returning %s.", pLocalHostname->GetBuffer()));
    
    return true;
}
