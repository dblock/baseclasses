/*
    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1994-1999 All Rights Reserved
    ______________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com

*/

#include <baseclasses.hpp>

#include "RemoteFile.hpp"

CRemoteFile::CRemoteFile(void) {
    
}

CRemoteFile::CRemoteFile(const CUrl& Url) {    
	SetUrl(Url);
}

CRemoteFile::CRemoteFile(const CString& Url) {    
	m_HttpRequest.RemoveAllHttpFields();
	m_HttpRequest.RemoveAllHttpPosts();
	m_Url.SetUrl(Url);
}

bool CRemoteFile::Get(const CString& Proxy) {
	CUrl ProxyUrl(Proxy);
	return Get(ProxyUrl);
}

bool CRemoteFile::Get(const CUrl& Proxy) {
	m_HttpRequest.SetProxy(Proxy);
	return Get();
}

bool CRemoteFile::Get(void) {
	return m_HttpRequest.Execute(m_Url);
}

void CRemoteFile::SetUrl(const CUrl& Url) {
	m_HttpRequest.RemoveAllHttpFields();
	m_HttpRequest.RemoveAllHttpPosts();
	m_Url = Url;
}


CRemoteFile::~CRemoteFile(void) {
    
}

