/*
    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1994-1999 All Rights Reserved
    ______________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com

*/

#ifndef BASE_DNS_HPP
#define BASE_DNS_HPP

#include <platform/include.hpp>
#include <String/String.hpp>
#include <Socket/SockTypes.hpp>
#include <Mutex/Mutex.hpp>
#include <Mutex/RWMutex.hpp>
#include <HashTable/HashTable.hpp>
#include <Thread/ThreadPool.hpp>

class CDomainNameService;
    
class CDnsPeer  {    
    friend class CDomainNameService;
    property(CInterval, Timestamp);
    property(bool, Resolved);
    property(bool, Started);
    property(bool, Done);
    copy_property(CDomainNameService *, DomainNameService);
    property(CAtomic, RefCount);
    property(CMutex, PeerMutex);
    property(CString, PeerHostname);
    property(sockaddr_in, Peer);    
public:
    inline void DecRef(void);
    inline void AddRef(void);
    void Resolve(void);
    CDnsPeer(void);
    ~CDnsPeer(void);
};

inline void CDnsPeer :: DecRef(void) { 
    if (! m_RefCount--)
        delete this; 
}

inline void CDnsPeer :: AddRef(void) { 
    m_RefCount++; 
}

class CDomainNameService : public CThreadPool {
    
    property(CHashTable<CDnsPeer *>, DnsCache);
    property(CMutex, DnsCacheMutex);
    property(int, DnsTimeout);
    
public:
    
    bool WalkHostent(
        struct hostent * hp,
        CVector<CString>& PeerAddresses, 
        CVector<CString>& PeerAliases);
    
    bool GetHostname(
        const CString& PeerHostname,
        sockaddr_in& Peer,
        CVector<CString> * PeerAddresses = NULL,
        CVector<CString> * PeerAliases = NULL
        );
    
    bool CreatePeer(
        unsigned long RemoteAddress, 
        CVector<CString>& PeerAddresses, 
        CVector<CString>& PeerAliases
        );
    
    bool CreateHost(
        const CString& PeerHostname, 
        sockaddr_in& Peer,
        int& RStatus);
    
    bool GetHostname(
        CString * pHostName);
    
    CDomainNameService(void);
    ~CDomainNameService(void);
};

#endif
