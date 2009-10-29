/*

    © Vestris Inc., Geneva Switzerland
    http://www.vestris.com, 1998, All Rights Reserved
    __________________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com

*/

/*
  [RFC1738]
  "Uniform Resource Locators", T. Berners-Lee, L. Masinter, and M. McCahill, December 1994.
  Available at http://ds.internic.net/rfc/rfc1738.txt.
  http://www.faqs.org/rfcs/rfc1738.html
*/

#ifndef BASE_URL_HPP
#define BASE_URL_HPP

#include <platform/platform.hpp>
#include <Object/Object.hpp>
#include <String/String.hpp>
#include <Vector/Vector.hpp>

class CUrl : public CObject {
	property(CString, Brute);
	property(CString, Scheme);
	property(CString, Specific);
	property(CVector<CString>, HostVector);
	property(CVector<CString>, FtpPath);
	property(char, FtpPathType);
	property(CString, HttpPath);
	property(CString, HttpSearchPath);
	property(CString, HttpArg);
	property(CString, HttpSig);
	property(CString, HttpDirectory);
	property(CString, HttpFilename);
	property(bool, Valid);
	readonly_property(CString, Error);
private:
	CString m_User; //property(CString, User);
	CString m_Password; //property(CString, Password);
	CString m_Port; // property(CString, Port);
	CString m_Host; //property(CString, Host);
	CString m_UrlPath; //property(CString, UrlPath);
public:
	static CString Escape(const CString&, bool Everything = false);
	static CString UnEscape(const CString&);
	static char X2C(char * what);
private:
	CString ReadScheme(int& curPos, const CString& iUrl);
	void ParseUrl(const CString&);
	bool ParseUrlSpecific(int&, const CString&);
	bool ParseHost(const CString&);
	bool ParseFtpPath(const CString&);
	bool ParseHttpPath(const CString&);
public:
	CUrl(void);
	CUrl(const CUrl&);
	virtual ~CUrl(void);
	CUrl(const CString&);
	void SetUrl(const CString&);
	CUrl& operator=(const CUrl&);
	bool operator==(const CUrl&) const;

	inline CString GetUser(void) const { return UnEscape(m_User); }
	inline CString GetPassword(void) const { return UnEscape(m_Password); }
	inline CString GetPort(void) const { return UnEscape(m_Port); }
	inline int GetPortValue(void) const { return CString::StrToInt(UnEscape(m_Port)); }
	inline CString GetHost(void) const { return UnEscape(m_Host); }
    CString GetDomain(void) const;
	CString GetUrlPath(void) const;
	CUrl Resolve(const CString&) const;
	CUrl Resolve(const CUrl& iUrl) const;
	CString GetHttpAll(void) const;
    CString GetHttpServer(void) const;
	inline void SetPassword(const CString& Value) { m_Password = Escape(Value); }
	inline void SetPort(const CString& Value) { m_Port = Escape(Value); }
	inline void SetPort(int Value) { m_Port = CString::StrToInt(Value); }
	inline void SetHost(const CString& Value) { m_Host = Escape(Value); }
	int Includes(const CUrl& iUrl) const;
};

#endif
