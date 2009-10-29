/*
    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1994-1999 All Rights Reserved
    ______________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com

*/

#ifndef BASE_REMOTE_FILE_HPP
#define BASE_REMOTE_FILE_HPP

#include <platform/include.hpp>
#include <String/String.hpp>
#include <String/StringPair.hpp>
#include <Internet/Url.hpp>
#include <Internet/HttpRequest.hpp>

class CRemoteFile : public CObject {
    readonly_property(CHttpRequest, HttpRequest);
	readonly_property(CUrl, Url);
public:
	CRemoteFile(void);
	CRemoteFile(const CUrl& Url);
	CRemoteFile(const CString& Url);
	virtual ~CRemoteFile(void);
	/* proxy/no proxy request */
	bool Get(const CString& Proxy); // proxy (CString)
	bool Get(const CUrl& Proxy); // proxy (CUrl)
	bool Get(void); // no proxy
	/* URL */
	void SetUrl(const CUrl& Url);
	/* Request Object */
    inline void SetProxy(const CString& ProxyName) { CUrl Url(ProxyName); m_HttpRequest.SetProxy(Url); }
    inline void SetProxy(const CUrl& ProxyUrl) { m_HttpRequest.SetProxy(ProxyUrl); }
    inline void SetAuth(const CVector<CStringPair>& AuthVector) { m_HttpRequest.GetServerAuthState().SetAuth(AuthVector); }
    inline void AddAuth(const CStringPair& AuthPair) { m_HttpRequest.GetServerAuthState().AddAuth(AuthPair); }
    inline void AddAuth(const CString& Username, const CString& Password) { m_HttpRequest.GetServerAuthState().AddAuth(Username, Password); }
	inline void SetTimeout(const int Value) { m_HttpRequest.SetTimeout(Value); }
	inline void SetMaxSize(const int Value) { m_HttpRequest.SetRequestSizeLimit(Value); }
    inline void SetParameter(const CString& Name, const CString& Value) { m_HttpRequest.SetHttpField(Name, Value); }
    inline CString GetParameter(const CString& Name) const { return m_HttpRequest.GetHttpField(Name); }
    inline void RemoveParameter(const CString& Name) { m_HttpRequest.RemoveHttpField(Name); }
	inline void SetRequestMethod(const CHttpIoType IoType) { m_HttpRequest.SetRequestMethod(IoType); }
	inline void SetVerbose(const bool Value) { m_HttpRequest.SetVerbose(Value); }
	inline void SetFollowRedirections(const bool Value) { m_HttpRequest.SetFollowRedirections(Value); }
	/* request results */
	inline int GetTimeout(void) const { return m_HttpRequest.GetTimeout(); }
	inline const int& GetRStatusValue(void) const { return m_HttpRequest.GetRStatusValue(); }
	inline const CVector<CString>& GetRRedirections(void) const { return m_HttpRequest.GetRRedirections(); }
	inline const CString& GetRData(void) const { return m_HttpRequest.GetRData(); }
    inline CString GetValue(const CString& Name) const { return m_HttpRequest.GetRField(Name); }
    inline void SetValue(const CString& Name, const CString& Value) { m_HttpRequest.SetRField(Name, Value); }
    inline CString GetError(void) const { return m_HttpRequest.GetError(); }
    inline bool GetUnrecoverableError(void) const { return m_HttpRequest.GetUnrecoverableError(); }
};


#endif
