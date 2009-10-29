/*
    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1994-1999 All Rights Reserved
    ______________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com

    (http://www.netscape.com/newsref/std/cookie_spec.html)
    
*/

#include <baseclasses.hpp>

#include "Cookie.hpp"
#include <Date/Date.hpp>
#include <String/GStrings.hpp>

#define C_COOKIE_INIT 				\
  m_Changed = false;				\
  m_Secure = false;				\
  m_Path = g_strSlash;                          \
  memset(&m_SExpires, 0, sizeof(m_SExpires));   \
  m_TExpires = 0;

CCookie::CCookie(void) {
  C_COOKIE_INIT;
}

CCookie::CCookie(const CString& Name, const CString& Value) {
  m_Name = Name;
  m_Value = Value;
  C_COOKIE_INIT;
}

CCookie::~CCookie(void) {
  
}

CCookie::CCookie(const CCookie& Cookie) {
  operator=(Cookie);
}

CCookie& CCookie::operator=(const CCookie& Cookie) {
  m_Name = Cookie.m_Name;
  m_Value = Cookie.m_Value;
  m_Changed = Cookie.m_Changed;
  m_Secure = Cookie.m_Secure;
  m_Domain = Cookie.m_Domain;
  m_Path = Cookie.m_Path;
  m_TExpires = Cookie.m_TExpires;
  m_SExpires = Cookie.m_SExpires;
  return * this;
}

bool CCookie::IsExpired(time_t * pTime) const {
  if (!m_TExpires) return false;
  if (pTime) {
    return (* pTime) >= m_TExpires;
  } else {
    time_t Now; time(&Now);
    return (Now >= m_TExpires);
  }
}

bool CCookie::IsParent(const CString& Domain, const CString& Path) const {
  // cout << "Dom=" << Domain << " Path=" << Path << " (" << "Dom=" << m_Domain << " Path=" << m_Path << ")" << endl;
  if (!m_Domain.GetLength() && (m_Path.Same("/"))) 
    return true;
  if (!Domain.EndsWith(m_Domain)) 
    return false;
  if (!Path.StartsWith(m_Path)) 
    return false;
  return true;
}

bool CCookie::IsSendable(const CUrl& Url) const {
  return ((!IsExpired()) && IsParent(Url.GetDomain(), Url.GetHttpDirectory()));
}

CString CCookie::GetSetCookieString(void) const {
  CString Result;
  Result.SetSize(64 + m_Name.GetLength() + m_Value.GetLength());
  Result = m_Name;
  Result += '=';
  Result += m_Value;
  if (m_TExpires) {
    CDate Date((tm *) &m_SExpires);
    Result += g_strHttpCookieExpires;
    Result += Date.Map(g_strHttpCookieDateFormat);
    Result += g_strHttpCookieGmt;
  }
  Result += g_strHttpCookiePath;
  Result += m_Path;
  if (m_Domain.GetLength()) {
    Result += g_strHttpCookieDomain;
    Result += m_Domain;
  }
  if (m_Secure)
    Result += g_strHttpCookieSecure;
  return Result;
}

ostream& CCookie::operator<<(ostream& Stream) const {
  Stream << GetSetCookieString();
  return Stream;
}

bool CCookie::SetExpires(const CString& DateString) {
  // date format: Fri, 16-Jul-1999 15:35:06 GMT
  if (CDate::EncodeDate(DateString, g_strHttpCookieScanDateFormat, m_SExpires)) {
    m_TExpires = mktime(&m_SExpires);
    return true;
  } else return false;  
}
