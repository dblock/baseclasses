/*
    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1994-1999 All Rights Reserved
    ______________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com

*/

#ifndef BASE_HTTP_REQUEST_HPP
#define BASE_HTTP_REQUEST_HPP

#include <platform/include.hpp>
#include <Io/HttpIo.hpp>
#include <Socket/Socket.hpp>
#include <String/GStrings.hpp>
#include <Security/State.hpp>

class CHttpRequest : public CObject {
	readonly_property(CVectorTable<CStringTable>, HttpFields); // requested HTTP fields
	readonly_property(CVectorTable<CStringTable>, PostFields); // posted HTTP fields
	property(CUrl, Proxy);
	private_property(CSocket, ClientSocket);
	property(bool, FollowRedirections);
	property(int, RequestSizeLimit);
	property(CHttpIoType, RequestMethod);
	property(CUrl, Url);
	property(CVectorTable<CStringTable>, RFields);
    property(CVector<CStringPair>, RPairFields);
	property(CVector<CString>, RRedirections);
    property(int, RStatusValue);
	property(CString, RStatusString);
	property(CString, RStatusVersion);
	property(float, RStatusVersionValue);
	property(CString, RData);
    property(CAuthenticationState, ServerAuthState);    
    property(bool, Dump);
    // $(TODO): property(CAuthenticationState, ProxyAuthState);
	/* response */
private:
	void MakeRequestString(const float, CString *);
	void ClearResults(bool);
	int ExecuteGet(bool Recurse = true);
    bool ParseStatusString(void);
public:
	CHttpRequest(void);
    virtual ~CHttpRequest(void);
	/* socket ops */
	inline void SetVerbose(const bool Value) { m_ClientSocket.SetVerbose(Value); }
	inline void SetTimeout(const int Value) { m_ClientSocket.SetTimeout(Value); }
	inline int GetTimeout(void) const { return m_ClientSocket.GetTimeout(); }
	inline CString GetError(void) const { return m_ClientSocket.GetError(); }
    inline bool GetUnrecoverableError(void) const { return m_ClientSocket.GetUnrecoverableError(); }
	/* retrieval */
	bool Execute(const CUrl&);
	bool CreateSocket(void);
	bool GetHTTP(float);
	/* HTTP request fields */
	void SetRField(const CString& Name, const CString& Value);
	void SetRField(const CString& Group, const CString& Name, const CString& Value);
    inline CString GetRField(const CString& Name) const { return m_RFields.FindElement(Name).GetValue(Name); }
	void SetHttpField(const CString& Name, const CString& Value);
	void SetHttpField(const CString& Group, const CString& Name, const CString& Value);
    inline CString GetHttpField(const CString& Name) const { return m_HttpFields.FindElement(Name).GetValue(g_strValue); }
	inline void RemoveHttpField(const CString& Name) { m_HttpFields.Remove(Name); }
	inline void RemoveAllHttpFields(void) { m_HttpFields.RemoveAll(); }
	void SetPostField(const CString& Name, const CString& Value);
	void SetPostField(const CString& Group, const CString& Name, const CString& Value);
	inline void RemoveAllHttpPosts(void) { m_PostFields.RemoveAll(); }
};

#endif



